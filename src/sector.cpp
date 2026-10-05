// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     sector.cpp - TSector object                       *
// *************************************************************************

#include "sector.h"

#include "bitmap.h"
#include "logging.h"
#include "parse.h"
#include "revutils.h"
#include "sectorstore.h"
#include "stream.h"
#include "textbar.h"

#include <algorithm>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include <algorithm>

char sectorfilename[80] = "%d_%d_%d.DAT";
uint32_t SectrorMapFCC = (('M' << 0) | ('A' << 8) | ('P' << 16) | (' ' << 24));
#define STARTSIZE 32768
#define GROWSIZE 16284

// ******************************
// * Constructor and Destructor *
// ******************************

TSector::TSector(int32_t newlevel, int32_t newsectorx, int32_t newsectory)
{
    level = newlevel;
    sectorx = newsectorx;
    sectory = newsectory;
    usecount = 0;
    objects.Clear();
    for (int32_t c = 1; c < NUMOBJSETS; c++)
        objsets[c-1].Clear();

    walkmap = new uint16_t[WALKMAPSIZE];

    memset(walkmap, 0, sizeof(uint16_t) * WALKMAPSIZE);

    snprintf(filename, sizeof(filename), sectorfilename, level, sectorx, sectory);
}

TSector::~TSector()
{
    delete[] walkmap;

    for (int32_t c = 1; c < NUMOBJSETS; c++)
        objsets[c-1].Clear();

    for (TObjectIterator i(&objects); i; i++)
    {
        TObjectInstance* inst = i.Item();

        if (!inst)
            continue;

        inst->ForceSector(nullptr);

        objects.Remove(i);

        if (inst->Flags() & OF_NONMAP) // Don't delete NONMAP objects
            continue;
        else
            delete inst;
    }
}

// ************************
// * Load and Save Sector *
// ************************

// Loads the sector.. keeps file open so sector is locked
TSector* TSector::LoadSector(int32_t newlevel, int32_t newsectorx, int32_t newsectory, bool preload)
{
    TSector* sector;

    if (preload)
    {
        sector = FindPreloadSector(newlevel, newsectorx, newsectory);
        if (sector)
        {
            sector->usecount++;
            return sector;
        }
    }

    sector = new TSector(newlevel, newsectorx, newsectory);

    // REVSYNC-DIVERGENCE: retail LoadSector @ 0x004982b0 ignored Load's
    // result and kept the sector, empty or half read. The port loads only
    // sectors whose files exist (TGameMap::Load), so a failure means the file
    // is unreadable or malformed, and keeping the sector would write the
    // partial copy over it when the map unloads. The sector is dropped; the
    // map leaves it out and logs it.
    if (!sector->Load())
    {
        delete sector;
        return nullptr;
    }

    return sector;
}

// Save and delete the sector (doesn't really delete it if sector is preload)
void TSector::CloseSector(TSector* sector)
{
    if (!sector->preloaded)
    {
        sector->Save();
        delete sector;
    }
    else
        sector->usecount--;
}

// Delete the sector without saving it: its working-set copy is about to be
// replaced (a new or loaded game).
void TSector::DiscardSector(TSector* sector)
{
    if (!sector->preloaded)
        delete sector;
    else
        sector->usecount--;
}

// Straight (no preloaded sectors) load/save functions

#define MAKEINDEX(level, sx, sy, item)  ((level<<24) | (sx<<18) | (sy<<12) | (item & 0xFFF))

// REVSYNC: TSector::Load @ 0x004984d0 + LoadFromStream @ 0x00498780.
// Returns false when the file is missing, can't be read, or is malformed as
// far as the sector can tell (header, object count, stream running out);
// retail rejected only a missing "MAP " header. Objects read before a
// failure stay in the sector: LoadSector discards such a sector.
bool TSector::Load(bool lock)
{
    int32_t version = 0;

    // The working set's copy if this game has written one, else the base map.
    FILE* fp = SectorStore::OpenForRead(filename);
    if (!fp)
        return false;

    fseek(fp, 0, SEEK_SET);

    // An empty file reads as a sector with no objects (the 1998 format had
    // no header). The buffer outlives `is`, which doesn't own it (retail
    // freed it after the read; the 1998 code leaked it).
    const int32_t filesize = flen(fp);
    std::vector<uint8_t> buf((size_t)std::max<int32_t>(filesize, sizeof(int32_t)), 0);
    const bool readok = filesize <= 0 || fread(buf.data(), (size_t)filesize, 1, fp) == 1;
    fclose(fp);

    auto malformed = [this](const char* why) {
        log_error("[sector] %s: %s", filename, why);
        return false;
    };
    if (!readok)
        return malformed("read failed");

    TInputStream is(buf.data(), (int32_t)buf.size());

    int32_t numobjects;
    is >> numobjects;

    // See if this is a sector map with header information
    if ((uint32_t)numobjects == SectrorMapFCC)
    {
        // Get which sector map version this is
        if (is.Remaining() < 4)
            return malformed("truncated header");
        is >> version;

        // v14+ adds a 4-byte hash (statehash) between version and
        // numobjects. See TSector in sector.h and
        // recon/docs/SECTOR_FILE_FORMAT.md. Gate matches retail
        // (FUN_00498780 @ 0x498780): `if (version > 13)`.
        if (is.Remaining() < (version > 13 ? 8 : 4))
            return malformed("truncated header");
        if (version > 13)
            is >> statehash;

        is >> numobjects;
    }

    if (numobjects < 0 || numobjects > MAXSECTOROBJECTS)
        return malformed("bad object count");

    for (int32_t c = 0; c < numobjects; c++)
    {
        // Every object record starts with a 16-bit field (objversion, or the
        // objclass in pre-v8 maps). A record that runs past the end of the
        // file marks the stream overrun (below).
        if (is.Remaining() < 2)
            return malformed("object data ends early");

        TObjectInstance* inst = TObjectInstance::LoadObject(is, version, OSTREAM_MAP);
        // Note: inst can be nullptr here if a placeholder (-1) was saved for the obj class id

        if (inst)
        {
            inst->ForceSector(this);
            inst->ForceLevel(level); // Directly sets the inst's level variable
            KeepInside(inst);
            // World positions stay in tile-Z space (matches the tile
            // draw path). The mesh-only 1.5 scaling lives entirely in
            // the per-instance mesh model matrix at draw time, so a
            // 3D object at pos.z = floor_height is visually at floor
            // height with its 1.5x-stretched mesh extending above.
        }

        objects.Set(inst, c);

        if (inst)
        {
            for (int32_t d = 1; d < NUMOBJSETS; d++)
            {
                if (InObjSet(inst, d))
                    objsets[d-1].Set(c, objsets[d-1].NumItems());
            }
        }

        if (version < 3 && inst) // This is nolonger used (indexes are now unique id's)
            inst->SetMapIndex(MAKEINDEX(level, sectorx, sectory, c));
    }

    if (is.Overrun())
        return malformed("object data overruns the file");

    // Load is the initial "contents are populated" event.
    contentver = 1;
    return true;
}

// (Historical band-aid: a `g_revenant_shutting_down` flag used to short-
// circuit Save() at process exit, because the global ~TMapManager would
// otherwise walk into freed TObjectInstances. That flag is gone now --
// ShutdownGlobals (revmain.cpp) explicitly calls MapManager.Shutdown() while
// the rest of the engine is still alive, so by the time the global dtor
// fires the cache is already empty and Save isn't reached.)

// REVSYNC: the position fix-up in TSector::Load @ 0x00498780: an object
// whose position lies outside the sector is moved into it by whole sectors
// on x and y (z unchanged).
void TSector::KeepInside(TObjectInstance* inst) const
{
    auto inside = [](int32_t v, int32_t lo, int32_t size) {
        const int32_t offset = (v - lo) % size;
        return lo + (offset < 0 ? offset + size : offset);
    };
    S3DPoint p = inst->Pos();
    const int32_t x = inside(p.x, sectorx * SECTORWIDTH, SECTORWIDTH);
    const int32_t y = inside(p.y, sectory * SECTORHEIGHT, SECTORHEIGHT);
    if (x != p.x || y != p.y)
    {
        p.x = x;
        p.y = y;
        inst->ForcePos(p);
    }
}

namespace {

// Adler-32 as retail computes it (0x0056ff60 / 0x0056ff80): the bytes are
// signed chars, and the sums are reduced mod 65521 only after each
// 5552-byte chunk, with 32-bit wraparound in between.
class TRetailAdler32
{
  public:
    void Update(const void* data, size_t count)
    {
        const int8_t* bytes = static_cast<const int8_t*>(data);
        uint32_t a = state & 0xffff;
        uint32_t b = state >> 16;
        while (count > 0)
        {
            const size_t chunk = std::min<size_t>(count, kChunk);
            for (size_t i = 0; i < chunk; i++)
            {
                a += (uint32_t)(int32_t)bytes[i];
                b += a;
            }
            bytes += chunk;
            count -= chunk;
            a %= kModulus;
            b %= kModulus;
        }
        state = (b << 16) | a;
    }

    [[nodiscard]] uint32_t Value() const { return state; }

  private:
    static constexpr size_t   kChunk   = 5552;
    static constexpr uint32_t kModulus = 65521;
    uint32_t state = 1;
};

}  // namespace

// REVSYNC: the sector state hash @ 0x00499e90 (SAVE_GAME.md §11.6): the
// sector's coordinates and object count, then the stream of each character
// and player in it, saved as for a map without inventories.
uint32_t TSector::StateHash()
{
    TRetailAdler32 hash;
    const int32_t header[4] = { level, sectorx, sectory, objects.NumItems() };
    for (int32_t value : header)
        hash.Update(&value, sizeof(value));

    TOutputStream os(0x8000, 0x3f9c);
    for (TPointerIterator<TObjectInstance> i(&objects); i; i++)
    {
        TObjectInstance* inst = i.Item();
        if (!inst || (inst->ObjClass() != OBJCLASS_CHARACTER && inst->ObjClass() != OBJCLASS_PLAYER))
            continue;
        os.Reset();
        TObjectInstance::SaveObject(inst, os, OSTREAM_MAP | OSTREAM_NOINVENTORY);
        hash.Update(os.Buffer(), os.DataSize());
    }

    constexpr uint32_t kZeroHash = 0xf0f0f0f0;
    return hash.Value() != 0 ? hash.Value() : kZeroHash;
}

// REVSYNC: TSector::Save @ 0x00498c90 + file write @ 0x00498a40. Retail
// writes empty sectors too (a retail curmap holds 2_8_10.DAT with 0 objects);
// the 1998 source deleted the file instead, which let a sector the player
// emptied fall back to its base-map contents. The state hash is computed
// after the objects are written, as retail (a lit object's Save sets flags).
void TSector::Save()
{
    TOutputStream os(STARTSIZE, GROWSIZE);

    os << SectrorMapFCC;
    os << (int32_t)MAP_VERSION;
    const int32_t hashpos = os.GetPos();
    os << (uint32_t)0;
    os << objects.NumItems();

    for (TPointerIterator<TObjectInstance> i(&objects); i; i++)
        TObjectInstance::SaveObject(i.Item(), os, OSTREAM_MAP);

    statehash = (int32_t)StateHash();
    const int32_t end = os.GetPos();
    os.SetPos(hashpos);
    os << (uint32_t)statehash;
    os.SetPos(end);

    FILE *fp = SectorStore::OpenForWrite(filename);
    if (!fp)
    {
        log_error("[sector] can't write %s to the working set", filename);
        return;
    }

    fwrite(os.Buffer(), os.DataSize(), 1, fp);
    fclose(fp);
}

// ****************************
// * Rectangles for Intersect *
// ****************************

// Added to the basic sector screen rect to include tiles which extend outside the sector rect
#define SECTORLREDGE    512             // Left/right edge
#define SECTORUEDGE     1024            // Upper edge
#define SECTORLEDGE     1024            // Lower edge

void TSector::GetMaxScreenRect(RSRect r)
{
    SRect maprect;
    GetMaxMapRect(maprect);

    S3DPoint p1, p2;

    p1.x = maprect.left;
    p1.y = maprect.top;
    p1.z = 0;

    p2.x = maprect.right;
    p2.y = maprect.bottom;
    p2.z = 0;

    WorldToScreen(p1, r.left, r.top);
    WorldToScreen(p2, r.right, r.bottom);
    r.left -= SECTORWIDTH;
    r.right += SECTORWIDTH;

  // Widen sector screen rect to include EVERYTHING that might overlap the sector boundries
    r.left -= SECTORLREDGE;
    r.right += SECTORLREDGE;
    r.top -= SECTORUEDGE;
    r.bottom += SECTORLEDGE;
}

void TSector::GetMaxMapRect(RSRect r)
{
    if ((uint32_t)sectorx >= MAXSECTORX || (uint32_t)sectory >= MAXSECTORY)
        r.left = r.top = r.right = r.bottom = 0;
    else
    {
        r.left = sectorx << SECTORWSHIFT;
        r.top = sectory << SECTORHSHIFT;
        r.right = r.left + SECTORWIDTH - 1;
        r.bottom = r.top + SECTORHEIGHT - 1;
    }
}

// ********************
// * Sector Utilities *
// ********************

// Adds an object to the sector
int32_t TSector::AddObject(TObjectInstance* oi, int32_t item)
{
    if (!oi)
        return -1;

    oi->ForceSector(this);
    oi->ForceLevel(level);

    if (item < 0)
        item = objects.Add(oi);
    else
        item = objects.Set(oi, item);

    for (int32_t d = 1; d < NUMOBJSETS; d++)
    {
        if (InObjSet(oi, d))
            objsets[d-1].Set(item, objsets[d-1].NumItems());
    }

    ++contentver;
    return item;
}

// Sets an object into the sector at the given item index
int32_t TSector::SetObject(TObjectInstance* oi, int32_t item)
{
    item = objects.Set(oi, item);

    if (oi)
    {
        oi->ForceSector(this);
        oi->ForceLevel(level);

        for (int32_t d = 1; d < NUMOBJSETS; d++)
        {
            if (InObjSet(oi, d))
                objsets[d-1].Set(item, objsets[d-1].NumItems());
        }
    }

    ++contentver;
    return item;
}

// Removes an object from the sector
TObjectInstance* TSector::RemoveObject(int32_t item)
{
    // TVirtualArray::Remove doesn't compact; slots vacated earlier in
    // the same frame return null. Guard before ForceSector — the dtor
    // already does the same null skip when iterating live slots.
    // Without this, TMapPane::TransferObject ate a SIGSEGV when AI
    // movement triggered a sector transfer for an object whose old
    // index pointed at a now-empty slot.
    TObjectInstance* oi = objects[item];
    objects.Remove(item);

    if (oi)
        oi->ForceSector(nullptr);

  // Remove object from sets
    for (int32_t d = 1; d < NUMOBJSETS; d++)
    {
        TObjSetArray &set = objsets[d-1];

        for (int32_t c = 0; c < set.NumItems(); c++)
        {
            if (set[c] == item)
            {
                set.Collapse(c);
                break;
            }
        }
    }

    ++contentver;
    return oi;
}

// REVSYNC: TSector::RemoveObject @ 0x00499250 (retail removes by object).
int32_t TSector::RemoveObject(TObjectInstance* oi)
{
    if (!oi)
        return -1;
    const int32_t item = GetObjIndex(oi);
    if (item >= 0)
        RemoveObject(item);
    return item;
}

int32_t TSector::GetObjIndex(const TObjectInstance* oi) const
{
    for (int32_t i = 0; i < objects.NumItems(); i++)
    {
        if (objects[i] == oi)
            return i;
    }
    return -1;
}

void TSector::ObjectFlagsChanged(TObjectInstance* oi, uint32_t oldflags, uint32_t newflags)
{
    if (oi->GetSector() != this)
        return;

    if (OBJSETOBJFLAGS & (oldflags ^ newflags))
    {
        int32_t index = GetObjIndex(oi);
        if (index < 0)
            return;

        RemoveObject(index);
        AddObject(oi);
    }
}

// *****************
// * Walkmap Stuff *
// *****************

void TSector::WalkmapHandler(int32_t mode, uint8_t *walk, int32_t zpos, int32_t x, int32_t y, int32_t width, int32_t length, int32_t stride, bool override)
{
    if (!walkmap || !walk)
        return;

    // first clip vars to be relative to the sector
    x -= (sectorx << SECTORWSHIFT) >> WALKMAPSHIFT;
    y -= (sectory << SECTORHSHIFT) >> WALKMAPSHIFT;

    int32_t cx = 0, cy = 0, w = width, l = length;

    // now clip
    if (x < 0)
    {
        w += x;
        cx = -x;
        x = 0;
    }

    if (y < 0)
    {
        l += y;
        cy = -y;
        y = 0;
    }

    if ((x + w) > (SECTORWIDTH >> WALKMAPSHIFT))
        w = (SECTORWIDTH >> WALKMAPSHIFT) - x;

    if ((y + l) > (SECTORHEIGHT >> WALKMAPSHIFT))
        l = (SECTORHEIGHT >> WALKMAPSHIFT) - y;

    // cliped out completely?
    if (!w && !l)
        return;

    // Just Do It(tm)
    uint16_t *start = walkmap + (y * (SECTORWIDTH >> WALKMAPSHIFT)) + x;
    walk += (cy * stride) + cx;

    int32_t srcstride = stride - w;
    int32_t dststride = (SECTORWIDTH >> WALKMAPSHIFT) - w;

    for (int32_t outerloop = 0; outerloop < l; outerloop++)
    {
        for (int32_t innerloop = 0; innerloop < w; innerloop++)
        {
            if (mode == WALK_TRANSFER)
            {
                if (*walk)
                {
                    int32_t walkval = *walk + zpos;

                    walkval = min(0xffff, max(1, walkval));

                    if (*start < walkval || override)
                        *start = walkval;
                }
            }
            else if (mode == WALK_CAPTURE)
            {
                if (*start == 0)
                    *walk = 0;
                else
                    *walk = min(255, max(1, *start - zpos));
            }
            else if (mode == WALK_CLEAR)
            {
                *start = 0;
            }

            start++;
            walk++;
        }

        walk += srcstride;
        start += dststride;
    }
}

void TSector::ClearWalkmap()
{
    memset(walkmap, 0, WALKMAPSIZE * 2);
}

// ----------------------- Preload Sector System -------------------

TSectorArray TSector::preloads;
int32_t TSector::preloadlevel = -1;
int32_t TSector::numpreloadrects = 0;
SRect TSector::preloadrects[MAXPRELOADRECTS];

// Preload sectors in this area (takes an area rectangle array from the area system)
bool TSector::LoadPreloadSectors(int32_t level, int32_t numrects, SRect *rects)
{
    ClearPreloadSectors(level, numrects, rects);

    preloadlevel = level;

    SRect srect;

    if (numrects > MAXPRELOADRECTS)
        numrects = MAXPRELOADRECTS;

    numpreloadrects = numrects;

    int32_t maxcount = 0;
    int32_t count = 0;

    for (int32_t c = 0; c < numrects; c++)
    {
      // Copy to rect list
        memcpy(&(preloadrects[c]), &(rects[c]), sizeof(SRect));

      // convert map coords to sector coords
        srect.left = rects[c].left >> SECTORWSHIFT;
        srect.top = rects[c].top >> SECTORHSHIFT;
        srect.right = (rects[c].right + (SECTORWIDTH - 1)) >> SECTORWSHIFT;
        srect.bottom = (rects[c].bottom + (SECTORHEIGHT - 1)) >> SECTORHSHIFT;

      // sanity check
        srect.left = max(0, min(srect.left, MAXSECTORX - 1));
        srect.top = max(0, min(srect.top, MAXSECTORY - 1));
        srect.right = max(0, min(srect.right, MAXSECTORX - 1));
        srect.bottom = max(0, min(srect.bottom, MAXSECTORY - 1));

        maxcount = srect.w() * srect.h() * 2;
        
      // Preload sectors here       
        for (int32_t y = srect.top; y <= srect.bottom; y++)
        {
          for (int32_t x = srect.left; x <= srect.right; x++)
          {

          // Is this sector left over from last preloading
            bool alreadyloaded = false;
            for (int32_t s = 0; s < preloads.NumItems(); s++)
            {
              TSector* loaded = preloads[s];
              if (loaded->level == level && loaded->sectorx == x && loaded->sectory == y)
              {
                alreadyloaded = true;
                break;
              }
            }

          // If not already in sector list, load it
            if (!alreadyloaded)
            {
                // Retail loads through LoadSector here too (0x004998b0).
                if (TSector* sector = LoadSector(level, x, y, false))
                {
                    sector->preloaded = true;
                    preloads.Add(sector);
                }
            }

          // Show levels in textbar
            if (TextBar.IsOpen() && !TextBar.IsHidden() && CurrentScreen->FrameCount() > 0)
            {
                int32_t level = 300 * count / (maxcount * 2);
                TextBar.SetLevels(level, level);
                TextBar.DrawImmediate();
                TextBar.PutToScreen();
            }
            count++;
          }
        }

      // Cache object graphics here 
      // (telescope in from outer sector borders to inner sectors so that the 
      // center sectors are the ones most likely to have cached graphics!)
        SRect b = srect;

        while (b.left < b.right && b.top < b.bottom)
        {
            for (int32_t c = 0; c < preloads.NumItems(); c++)
            {
                TSector* s = preloads[c];
                if (s->sectorx == b.left || s->sectorx == b.right || // On borders of rect
                    s->sectory == b.top || s->sectory == b.bottom)
                {
                    TObjectArray &objects = s->objects;
                    for (int32_t c = 0; c < objects.NumItems(); c++)
                    {
                        if (objects[c])
                            objects[c]->GetImagery()->CacheImagery();
                    }

                  // Show levels in textbar
                    if (TextBar.IsOpen() && !TextBar.IsHidden() && CurrentScreen->FrameCount() > 0)
                    {
                        int32_t level = 300 * count / (maxcount * 2);
                        TextBar.SetLevels(level, level);
                        TextBar.DrawImmediate();
                        TextBar.PutToScreen();
                    }
                    count++;
                }
            }

            b.left++;
            b.right--;
            b.top++;
            b.bottom--;

        }
    }

    return true;
}

// Clear preload sectors
void TSector::ClearPreloadSectors(int32_t level, int32_t numrects, SRect *rects)
{
    for (int32_t c = preloads.NumItems() - 1; c >= 0; c--)
    {
        TSector* sector = preloads[c];

      // Can't delete sectors which are currently in use!!
        if (sector->usecount > 0)
            continue;

      // Does this sector match our current level?
        bool in = true;
        if (level != sector->level)
            in = false; // No.. kill it

      // Is this sector in any of our current rectangles?
        if (in && numrects > 0 && rects)
        {
            SRect r;
            sector->GetMaxMapRect(r);
            int32_t d;
            for (d = 0; d < numrects; d++)
            {
                if (rects[d].Intersects(r))
                    break;
            }
            if (d >= numrects)
                in = false;     // Uh uh.. so kill it!
        }

      // This sector isn't in anything, so get rid of it!
        if (!in)
        {
            sector->Save();
            preloads.Collapse(c, true);
        }
    }

  // Clear the rectangle list
    numpreloadrects = 0;
    preloadlevel = -1;
}

// Returns a preload sector
TSector* TSector::FindPreloadSector(int32_t level, int32_t sectorx, int32_t sectory)
{
    for (int32_t c = 0; c < preloads.NumItems(); c++)
    {
        if (preloads[c]->level == level &&
            preloads[c]->sectorx == sectorx &&
            preloads[c]->sectory == sectory)
                return preloads[c];
    }

    return nullptr;
}

// Are we in the preload area?
bool TSector::InPreloadArea(const S3DPoint& p, int32_t level)
{
    if (level != preloadlevel)
        return false;

    SPoint a;
    a.x = p.x;
    a.y = p.y;

    for (int32_t c = 0; c < numpreloadrects; c++)
    {
        if (preloadrects[c].In(a))
            return true;
    }

    return false;
}

#ifdef _MAPPANE_H
#error If you need to access TMapPane from this file, you're doing something wrong... BEN
#endif

