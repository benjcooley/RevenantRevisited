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
#include "stream.h"
#include "textbar.h"

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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

    sprintf(filename, sectorfilename, level, sectorx, sectory);
}

TSector::~TSector()
{
    if (walkmap)
        delete walkmap;

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
    sector->Load(); // Assume this works

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

// Straight (no preloaded sectors) load/save functions

#define MAKEINDEX(level, sx, sy, item)  ((level<<24) | (sx<<18) | (sy<<12) | (item & 0xFFF))

bool TSector::Load(bool lock)
{
    int32_t version = 0;
    FILE *fp;

    char mappath[MAXPATHLEN];

    // rev_fopen handles SavePath/RunPath/data-root/VFS fallbacks so both the
    // loose .DAT under data/Curmap and rvr-packed sectors resolve cleanly.
    strcpy(mappath, CurMapPath);
    strcat(mappath, CURMAPDIR "\\");
    strcat(mappath, filename);
    fp = rev_fopen(mappath, "rb");

    if (!fp)
    {
        strcpy(mappath, BaseMapPath);
        strcat(mappath, BASEMAPDIR "\\");
        strcat(mappath, filename);
        fp = rev_fopen(mappath, "rb");

        if (!fp)
            return false;
    }

    fseek(fp, 0, SEEK_SET);

    int32_t bufsize = flen(fp);
    uint8_t *buf;

    if (bufsize > 0)
    {
        buf = (uint8_t *)malloc(bufsize);
        fread(buf, bufsize, 1, fp);
    }
    else
    {
        buf = (uint8_t *)malloc(sizeof(int32_t));
        bufsize = 4;
        *((int32_t *)buf) = 0;
    }

    //if (!lock)
    {
        fclose(fp);
        fp = nullptr;
    }

    TInputStream is(buf, bufsize);

    int32_t numobjects;
    is >> numobjects;

    // See if this is a sector map with header information
    if ((uint32_t)numobjects == SectrorMapFCC)
    {
        // Get which sector map version this is
        is >> version;

        // v14+ adds a 4-byte hash (statehash) between version and
        // numobjects. See TSector in sector.h and
        // recon/docs/SECTOR_FILE_FORMAT.md. Gate matches retail
        // (FUN_00498780 @ 0x498780): `if (version > 13)`.
        if (version > 13)
            is >> statehash;

        is >> numobjects;
    }

    for (int32_t c = 0; c < numobjects; c++)
    {
        TObjectInstance* inst = TObjectInstance::LoadObject(is, version, true);
        // Note: inst can be nullptr here if a placeholder (-1) was saved for the obj class id

        if (inst)
        {
            inst->ForceSector(this);
            inst->ForceLevel(level); // Directly sets the inst's level variable
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

    // Load is the initial "contents are populated" event.
    contentver = 1;
    return true;
}

void TSector::Save()
{
    int32_t version = MAP_VERSION; // Current sector map version #

    if (objects.NumItems() == 0)
    {
        // don't save empty sectors, and clear out old save file
        std::remove(filename);
        return;
    }

    bool closeit = false;
    TOutputStream os(STARTSIZE, GROWSIZE);
    TPointerIterator<TObjectInstance> i(&objects);

    // Write out the "header" with "MAP " followed by the version #
    os << SectrorMapFCC;
    os << version;

    // v14+ writes statehash between version and numobjects. Retail
    // regenerates this hash at save time (FUN_00499e90). We round-trip
    // the last-loaded value for now.
    if (version > 13)
        os << statehash;

    os << objects.NumItems();

    for ( ; i; i++)
    {
        TObjectInstance::SaveObject(i.Item(), os, true);
    }

    char mappath[MAXPATHLEN];

    strcpy(mappath, CurMapPath);
    strcpy(mappath, CURMAPDIR "\\");
    strcat(mappath, filename);

    FILE *fp = fopen(mappath, "wb");    // Path open (uses program path)
    if (!fp)
        return;

    fseek(fp, 0, 0);
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
    TObjectInstance* oi = objects[item];
    objects.Remove(item);
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
                TSector* sector = new TSector(level, x, y);
                if (sector)
                {
                    sector->Load();
                    sector->preloaded = true;
                    preloads.Add(sector);
                }
            }

          // Show levels in textbar
            if (TextBar.IsOpen() && !TextBar.IsHidden() && CurrentScreen->FrameCount() > 0)
            {
                int32_t level = 300 * count / (maxcount * 2);
                TextBar.SetLevels(level, level);
                TextBar.Draw();
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
                        TextBar.Draw();
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

