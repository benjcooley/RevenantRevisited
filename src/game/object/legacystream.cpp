// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   legacystream.cpp - the retail object stream, byte for byte          *
// *************************************************************************
//
// Every REVSYNC note names the retail function the method mirrors; the
// field layouts are in docs/SAVE_GAME.md §11. The instance's own state is
// read and written through its members (the reader and writer are friends),
// its transform through the integer views in intworld.h.
//
// *************************************************************************

#include "game/object/legacystream.h"

#include "game/object/ammo.h"
#include "game/world/automap.h"
#include "game/object/character.h"
#include "game/object/complexobj.h"
#include "game/object/container.h"
#include "game/effects/effect.h"
#include "game/object/exit.h"
#include "game/object/food.h"
#include "engine/scene/intworld.h"
#include "game/object/inventory.h"
#include "engine/core/logging.h"
#include "game/object/money.h"
#include "game/object/monstergen.h"
#include "game/object/object.h"
#include "game/object/player.h"
#include "game/ui/playscreen.h"
#include "game/app/revenant.h"
#include "game/script/script.h"
#include "game/object/scroll.h"
#include "engine/core/stream.h"
#include "game/object/weapon.h"

#include <algorithm>
#include <cstring>
#include <set>
#include <string>

// Diagnostic counters for why ReadObject returned nullptr. Wired up by the
// sector test harness to tell "legitimate empty placeholder" apart from
// "loader couldn't recover this object".
int32_t g_loadObjNullObjVerNeg   = 0;
int32_t g_loadObjNullClassNeg    = 0;
int32_t g_loadObjNullBadClass    = 0;
int32_t g_loadObjNullBadType     = 0;
int32_t g_loadObjNullNewObjFail  = 0;
int32_t g_loadObjNullCorruptDrop = 0;
int32_t g_loadObjNullNonMapDrop  = 0;
int32_t g_loadObjOk              = 0;

namespace {

// Object names in sector and save files carry bit 7 on every byte.
constexpr uint8_t kNameByteFlag = 0x80;
constexpr uint8_t kNameByteMask = 0x7f;

// The stats an object untouched since placement keeps through a load
// (health, fatigue and mana for a character); the rest come from its type.
constexpr int32_t kFirstKeptStat = 3;
constexpr int32_t kLastKeptStat  = 5;

// Object stats are saved with their unique ids (<=4 char codes like "AMT",
// "TYPE", "AC"), each id byte with bit 7 set.
constexpr uint32_t kStatIdFlag = 0x80808080;
constexpr uint32_t kStatIdMask = 0x7f7f7f7f;

// A NUL-terminated string from the stream (the stream reads zeros past its
// end, so this always ends).
std::string ReadCString(TInputStream& is)
{
    std::string text;
    char c = 0;
    is >> c;
    while (c != '\0')
    {
        text += c;
        is >> c;
    }
    return text;
}

// Stream string into a fixed char buffer, cut to fit.
void ReadFixedString(TInputStream& is, char* dst, size_t capacity)
{
    const std::string s = is.ReadString();
    const size_t n = std::min<size_t>(s.size(), capacity - 1);
    std::memcpy(dst, s.data(), n);
    dst[n] = 0;
}

}  // namespace

// *********************
// * TLegacyDeserialize *
// *********************

// REVSYNC: TObjectInstance::LoadObject (universal stream decoder) @ 0x00471ce0.
// docs/gameflow/forensics/SAVE_GAME.md §11.1. `streamflags` carries what
// retail kept in DAT_0065a254: OSTREAM_MAP discards NONMAP objects (players
// never come from a sector file), OSTREAM_NOINVENTORY skips inventories.
//
// The type comes from the unique id; when the id belongs to another class
// the object is built as that class and read with the base object layout
// ("force simple"), as retail. Retail looked the id up in one global table;
// the port asks the stored class first and then every class, which finds the
// same type because ids are unique.
TObjectInstance* TLegacyDeserialize::ReadObject(TInputStream& is, int32_t version, uint32_t streamflags)
{
    uint32_t uniqueid = 0;
    short objversion = 0;
    short objclass;
    short objtype = -1;
    short blocksize = -1;
    short invblocksize = -1;  // v14+: the inventory's share of blocksize
    bool forcesimple = false;
    bool corrupted = false;

    // ****** Load object block header ******

    if (version >= 8)
        is >> objversion;
    if (objversion < 0) // Placeholder for an empty slot (map version 8 and up)
    {
        ++g_loadObjNullObjVerNeg;
        return nullptr;
    }

    is >> objclass;
    if (objclass < 0)   // Placeholder for an empty slot
    {
        ++g_loadObjNullClassNeg;
        return nullptr;
    }

    if (version < 1)
        is >> objtype;              // Version 0: no unique ids
    else
    {
        is >> uniqueid;
        if (version >= 4)
            is >> blocksize;        // Version 4: block size, so unknown objects can be skipped
        if (version >= 14)
            is >> invblocksize;
    }

    const int32_t headerend = is.GetPos();
    auto skipBlock = [&]() { is.SetPos(headerend + blocksize); };

    // ****** Is this object any good? ******

    TObjectClass* cl = TObjectClass::GetClass(objclass);
    if (!cl || !cl->ClassName())
    {
        if (Debug)
            FatalError("Object in map file has invalid class - possible file corruption");
        else if (blocksize >= 0)
        {
            skipBlock();
            ++g_loadObjNullBadClass;
            return nullptr;
        }
        else                        // Try to fix it by assuming its a tile
        {
            objclass = OBJCLASS_TILE;
            cl = TObjectClass::GetClass(objclass);
            corrupted = true;
        }
    }

    // Retail never loads a map scroll from a file.
    if (objclass == OBJCLASS_MAPSCROLL && blocksize >= 0)
    {
        skipBlock();
        return nullptr;
    }

    if (objtype < 0)
    {
        objtype = cl->FindObjType(uniqueid);
        if (objtype < 0)
        {
            for (int32_t newobjclass = 0; newobjclass < MAXOBJECTCLASSES; newobjclass++)
            {
                TObjectClass* newcl = TObjectClass::GetClass(newobjclass);
                const int32_t newobjtype = newcl ? newcl->FindObjType(uniqueid) : -1;
                if (newobjtype >= 0)
                {
                    objclass = (short)newobjclass;
                    objtype = (short)newobjtype;
                    cl = newcl;
                    forcesimple = true;
                    break;
                }
            }
        }

        if (objtype < 0)  // Still can't find type
        {
            if (Debug)
            {
                char buf[80];
                snprintf(buf, sizeof(buf), "Object unique id 0x%x not found in class.def", uniqueid);
                FatalError(buf);
            }
            else if (blocksize >= 0)
            {
                skipBlock();
                ++g_loadObjNullBadType;
                return nullptr;
            }
            else
            {
                objtype = 0;
                corrupted = true;
            }
        }
    }

    // ****** Create the object ******

    // Retail builds it LOADING: the constructor leaves the script to the read.
    SObjectDef def;
    def.objclass = objclass;
    def.objtype  = objtype;
    def.flags    = OF_LOADING;

    TObjectInstance* inst = cl->NewObject(&def);
    if (!inst)
    {
        // A type whose class.def builder the port lacks. Retail would have
        // crashed here ("Trouble creating loaded object", then an unchecked
        // call); skip the block instead.
        if (blocksize >= 0)
            skipBlock();
        ++g_loadObjNullNewObjFail;
        return nullptr;
    }

    // ****** Load the object ******

    // v14+: blocksize covers body and inventory; invblocksize is the
    // inventory's tail of it. Older versions don't say where the body ends.
    const int32_t bodystart = is.GetPos();
    const int32_t bodysize  = (version >= 14 && blocksize >= 0)
                                  ? blocksize - std::max<int32_t>(invblocksize, 0) : -1;

    if (forcesimple)
        ReadObjectInstance(*inst, is, version, objversion);
    else
        Read(*inst, is, version, objversion);

    if (bodysize >= 0)
    {
        const int32_t consumed = is.GetPos() - bodystart;
        if (consumed < bodysize)
            KeepUnreadClassData(*inst, is, bodystart + consumed, bodystart + bodysize);
        else if (consumed > bodysize)
            log_warn("[object] block size error for object %s: body is %d bytes, the read took %d",
                     inst->GetName() ? inst->GetName() : "?", bodysize, consumed);
        is.SetPos(bodystart + bodysize);
    }

    // Before v14 the inventory can't be skipped (its size isn't recorded).
    const bool loadInventory =
        version < 14 || (!(streamflags & OSTREAM_NOINVENTORY) && invblocksize >= 1);
    if (loadInventory)
    {
        if (auto* player = dynamic_cast<TPlayer*>(inst))
            ReadPlayerInventory(*player, is, version, streamflags);
        else
            ReadInventory(*inst, is, version, streamflags);
    }

    if (blocksize >= 0)
        is.SetPos(bodystart + blocksize);

    const bool nonmap_drop = (streamflags & OSTREAM_MAP) && (inst->Flags() & OF_NONMAP);
    if (corrupted || nonmap_drop || inst->ObjClass() == OBJCLASS_MAPSCROLL)
    {
        DeleteObject(inst);
        if (nonmap_drop)
            ++g_loadObjNullNonMapDrop;
        else
            ++g_loadObjNullCorruptDrop;
        return nullptr;
    }

    ++g_loadObjOk;
    return inst;
}

// Most derived first: a class without a layout of its own reads as its
// nearest base, as the retail virtuals resolved.
void TLegacyDeserialize::Read(TObjectInstance& o, TInputStream& is, int32_t version, int32_t objversion)
{
    if (auto* p = dynamic_cast<TPlayer*>(&o))             ReadPlayer(*p, is, version, objversion);
    else if (auto* c = dynamic_cast<TCharacter*>(&o))     ReadCharacter(*c, is, version, objversion);
    else if (auto* x = dynamic_cast<TComplexObject*>(&o)) ReadComplexObject(*x, is, version, objversion);
    else if (auto* e = dynamic_cast<TExit*>(&o))          ReadExit(*e, is, version, objversion);
    else if (auto* n = dynamic_cast<TContainer*>(&o))     ReadContainer(*n, is, version, objversion);
    else if (auto* a = dynamic_cast<TAmmo*>(&o))          ReadAmmo(*a, is, version, objversion);
    else if (auto* m = dynamic_cast<TMoney*>(&o))         ReadMoney(*m, is, version, objversion);
    else if (auto* f = dynamic_cast<TFood*>(&o))          ReadFood(*f, is, version, objversion);
    else if (auto* s = dynamic_cast<TScroll*>(&o))        ReadScroll(*s, is, version, objversion);
    else if (auto* w = dynamic_cast<TWeapon*>(&o))        ReadWeapon(*w, is, version, objversion);
    else if (auto* d = dynamic_cast<TDripEffect*>(&o))    ReadDripEffect(*d, is, version, objversion);
    else if (auto* g = dynamic_cast<TMonsterGen*>(&o))    ReadMonsterGen(*g, is, version, objversion);
    else                                                  ReadObjectInstance(o, is, version, objversion);
}

// REVSYNC: TObjectInstance::Load @ 0x00472430 (vtable slot 0x160). Fields:
// SAVE_GAME.md §11.2 (the older-version branches follow the 1998 source and
// retail).
void TLegacyDeserialize::ReadObjectInstance(TObjectInstance& o, TInputStream& is, int32_t version, int32_t objversion)
{
    (void)objversion;

    // Names are stored with bit 7 set on every byte; length 0 means "the
    // type's name", which the constructor already set.
    uint8_t len;
    is >> len;
    if (len > 0)
    {
        o.name = (char *)malloc(len + 1);
        for (int32_t i = 0; i < len; i++)
        {
            uint8_t encoded;
            is >> encoded;
            o.name[i] = (char)(encoded & kNameByteMask);
        }
        o.name[len] = 0;
    }

    uint32_t newflags;
    {
        S3DPoint loaded;
        is >> newflags >> loaded.x >> loaded.y >> loaded.z;
        SetIntPos(o, loaded);
    }

  // The constructor's fixed flags stay; everything else comes from the file
    o.flags = (o.flags & OF_FIXEDFLAGS) | (newflags & ~OF_FIXEDFLAGS);

  // Don't load velocity vectors if not mobile
    if (version < 6 || !(o.flags & OF_IMMOBILE))
        is >> o.vel.x >> o.vel.y >> o.vel.z;

    if (version < 9)
    {
        uint8_t statebyte;
        is >> statebyte;
        o.state = statebyte;
    }
    else
        is >> o.state;

  // Non map objects (i.e. players) store their level: they come from the
  // save, not a sector, so the level says where to put them.
    if (version >= 6 && (o.flags & OF_NONMAP))
    {
        if (version < 9)
        {
            uint8_t levelbyte;
            is >> levelbyte;
            o.ForceLevel(levelbyte);
        }
        else
        {
            uint16_t level;
            is >> level;
            o.ForceLevel(level);
        }
    }
    else
        o.ForceLevel(0);

    uint8_t health = 0;
    if (version < 5)
        is >> health; // Now an object stat

    uint8_t rotatex = 0, rotatey = 0, rotatez = 0;
    if (version < 3)
    {
        uint8_t dummy8;
        short dummy16;
        is >> rotatez >> dummy16 >> o.inventnum >> dummy16 >> o.shadow >> dummy8;

        o.inventnum = -1;   // ignore inventories in old versions
        o.Node()->SetMapIndex(-1);    // the sector sets old-style indexes after the load
    }
    else
    {
        int32_t loaded_mapindex = -1;
        is >> o.inventnum >> o.invindex >> o.shadow >> rotatex >> rotatey >> rotatez >> loaded_mapindex;
        // Through SetMapIndex so the mapindex registry sees every streamed-in
        // instance; TSafeRef<T>::Get() relies on it.
        o.SetMapIndex(loaded_mapindex);
    }
    SetIntRotateX(o, rotatex);
    SetIntRotateY(o, rotatey);
    SetIntFacing(o, rotatez);
    o.moveangle = rotatez;

    if (version < 5)  // Empty stat array with health stuck in it
    {
        o.frame = 0;
        o.framerate = 1;
        o.group = 0;

        if (o.cl->NumObjStats() > 0)
        {
            o.stats.SetNumItems(o.cl->NumObjStats());
            for (int32_t c = 0; c < o.cl->NumObjStats(); c++)
                o.ResetObjStat(c);
        }

        o.SetHealth(health);
    }
    else
    {
        if (version < 6 || (o.flags & OF_ANIMATE))
            is >> o.frame >> o.framerate;

        is >> o.group;

        if (o.cl->NumObjStats() > 0)
            o.stats.SetNumItems(o.cl->NumObjStats());

        // Each saved stat is matched to the class's stats by unique id (the
        // saved id carries bit 7 in every byte); the slow path searches every
        // class stat, so stats survive a reordered class.def.
        uint8_t numstats;
        is >> numstats;
        int32_t statid = 0;
        for (int32_t st = 0; st < numstats && statid < o.stats.NumItems(); st++)
        {
            int32_t stat;
            uint32_t uniqueid;
            is >> stat >> uniqueid;
            uniqueid &= kStatIdMask;

            if (uniqueid == o.cl->ObjStatUniqueId(statid))
            {
                o.stats[statid] = stat;
                statid++;
            }
            else
            {
                for (int32_t c = 0; c < o.cl->NumObjStats(); c++)
                {
                    if (uniqueid == o.cl->ObjStatUniqueId(c))
                        o.stats[c] = stat;
                }
            }
        }

        // An object untouched since it was placed takes its type's current
        // stats; only its health, fatigue and mana (stats 3-5) are its own.
        if (o.flags & OF_VIRGIN)
        {
            for (int32_t c = 0; c < o.cl->NumObjStats(); c++)
            {
                if (c < kFirstKeptStat || c > kLastKeptStat)
                    o.ResetObjStat(c);
            }
        }
    }

    if (o.flags & OF_LIGHT)
    {
        is >> o.lightdef.flags >> o.lightdef.pos.x >> o.lightdef.pos.y >> o.lightdef.pos.z >>
            o.lightdef.color.red >> o.lightdef.color.green >> o.lightdef.color.blue >>
            o.lightdef.intensity >> o.lightdef.multiplier;
        o.flags |= OF_LIGHT | OF_ANIMATE;
    }

  // Set Object info pointer
    o.cl = TObjectClass::GetClass(o.objclass);
    if (!o.cl)
        FatalError("Bad object class!"); // This should never happen!!
    o.inf = o.cl->GetObjType(o.objtype);

  // Any script the object had goes; the right one is attached below
    delete o.script;
    o.script = nullptr;

  // Reset inventory
    if (&o == Inventory.GetContainer())
        Inventory.Update();

  // Flags retail settles once the object is loaded
    if (version < 12)
        o.flags &= ~(OF_DRAWFLIP | OF_VIRGIN | OF_LOADING | OF_INVULNERABLE | OF_BACKGROUND |
                     OF_INVENTORY | OF_CALLEDPREDEL);
    o.flags &= ~OF_LOADING;
    if (o.objclass == OBJCLASS_TILE && (o.flags & OF_LIGHT))
        o.flags &= ~OF_PULSE;
    if (o.flags & OF_INVENTORY)
        o.flags |= OF_ANIMATE | OF_PULSE;

  // The script, now that the name and flags are final
    if (o.CanHaveScript())
        o.InitScript(ScriptManager.ObjectScript(&o));
}

// REVSYNC: TComplexObject::Load @ 0x004db930 (SAVE_GAME.md §11.3).
void TLegacyDeserialize::ReadComplexObject(TComplexObject& o, TInputStream& is, int32_t version, int32_t objversion)
{
    uint8_t basever = 0;
    if (objversion >= 1)
        is >> basever;
    ReadObjectInstance(o, is, version, basever);

  // Load root state
    TActionBlock* ab;
    if (version < 7)
    {
        ab = new TActionBlock(o.DefaultRootState());
    }
    else
    {
        uint8_t action;
        is >> action;
        const std::string rootname = is.ReadString();
        ab = new TActionBlock(rootname.substr(0, RESNAMELEN - 1).c_str(), (ACTION)action);
    }

  // REVSYNC-DIVERGENCE: a root that names no state of the model can never be
  // entered. Every ForceCommand back to it is impossible, so a speaker who
  // finishes a line stays in "say" and the script's speech wait never ends.
  // The shipped level-46 map saves Shegra and the slaves with a "combat" root
  // their walk-and-say models lack (docs/gameflow/forensics/DIALOG.md §2.5).
  // Such a root becomes the default one, else "walk", as a plain animation
  // block like the one Clear builds.
    if (o.imagery && !o.imagery->HasState(ab->name))
    {
        const char* fallback = o.imagery->HasState(o.DefaultRootState()) ? o.DefaultRootState() : "walk";
        if (o.imagery->HasState(fallback))
        {
            delete ab;
            ab = new TActionBlock(fallback);
        }
    }

    o.SetRoot(ab);
    o.SetDoing(ab);
    o.SetDesired(ab);

  // Keep the saved state when it is the root's own (an animation state is
  // named "<group>:<name>"); otherwise start the root.
    if (const char* statename = o.imagery ? o.imagery->GetAniName(o.state) : nullptr)
    {
        if (const char* colon = strchr(statename, ':'))
            statename = colon + 1;
        if (stricmp(ab->name, statename) == 0)
            return;
    }
    o.state = o.FindState(ab->name);
}

// REVSYNC: TCharacter::Load @ 0x004d4eb0 (SAVE_GAME.md §11.3).
void TLegacyDeserialize::ReadCharacter(TCharacter& o, TInputStream& is, int32_t version, int32_t objversion)
{
    uint8_t basever = 0;
    if (objversion >= 3)
        is >> basever;
    ReadComplexObject(o, is, version, basever);

  // Get saved last pulse values (so we can figure what has happened to char)
    if (objversion < 1)
        return;

  // Last time any health/fatigue/mana was recovered
    is >> o.lasthealthrecov;
    is >> o.lastfatiguerecov;
    is >> o.lastmanarecov;

    if (objversion >= 4)
    {
        is >> o.lastpoisondamage;
    }
    else
    {
        o.lastpoisondamage = -1;
    }

    if (objversion >= 2)
    {
        is >> o.teleport_position.x;
        is >> o.teleport_position.y;
        is >> o.teleport_position.z;
        is >> o.teleport_level;
    }
    else
    {
        o.teleport_position.x = o.teleport_position.y = o.teleport_position.z = -1;
        o.teleport_level = -1;
    }

    if (o.ObjClass() == OBJCLASS_CHARACTER && o.chardata)
    {
        if (o.Health() > o.chardata->health)
            o.SetHealth(o.chardata->health);
        if (o.MaxHealth() > o.chardata->health)
            o.SetMaxHealth(o.chardata->health);
        if (o.Fatigue() > o.chardata->fatigue)
            o.SetFatigue(o.chardata->fatigue);
        if (o.MaxFatigue() > o.chardata->fatigue)
            o.SetMaxFatigue(o.chardata->fatigue);
        if (o.Mana() > o.chardata->mana)
            o.SetMana(o.chardata->mana);
        if (o.MaxMana() > o.chardata->mana)
            o.SetMaxMana(o.chardata->mana);
    }

  // A character saved dead is removed on its first frame.
    if (o.Health() < 1)
        o.ResetFlags(o.flags | OF_KILL);

  // Fully visible, not fading.
    o.fade = 100;
    o.fade_step = 0;
    o.fade_limit = 100;
    o.fade_direction = 0;
}

// REVSYNC: TPlayer::Load @ 0x0051b960 (SAVE_GAME.md §11.4). Ends with
// RefreshStats (0x0051c660): the modified stats from the loaded ones.
void TLegacyDeserialize::ReadPlayer(TPlayer& o, TInputStream& is, int32_t version, int32_t objversion)
{
    uint8_t basever = (uint8_t)objversion;
    if (objversion >= 4)
        is >> basever;
    ReadCharacter(o, is, version, basever);

    if (objversion >= 4)
    {
        for (char* quickspell : o.quickspells)
            ReadFixedString(is, quickspell, MAXTALISMANLEN);
    }

    if (objversion >= 5)
    {
        int32_t count = 0;
        is >> count;
        o.knownspells.clear();
        for (int32_t i = 0; i < count && !is.Overrun(); i++)
            is.ReadBytes(o.knownspells.emplace_back().data(), kSpellCodeBytes);
        ++o.knownspellsversion;
        if (objversion >= 6 && objversion <= 8)
            is.MovePos(count * 4);  // a per-spell int those versions had
    }

    // Retail kept these in a member nothing reads; the port restores the HUD
    // from them (TSaveGame).
    if (objversion >= 7)
        is >> o.hudwords.sidebarOpen >> o.hudwords.upperMode >> o.hudwords.lowerMode >> o.hudwords.drawerOpen;

    o.levelupstats = {};
    if (objversion >= 8)
        is >> o.levelupstats[0] >> o.levelupstats[1] >> o.levelupstats[2];

    o.team = {};
    o.modulename.clear();
    if (objversion >= 13)
    {
        o.team.name  = is.ReadString();
        o.team.name2 = is.ReadString();
        is >> o.team.value >> o.team.id >> o.team.teamindex;
        o.modulename = is.ReadString();
    }
    else if (objversion >= 11)
    {
        o.team.name  = is.ReadString();
        o.team.name2 = is.ReadString();
        is >> o.team.value;
        o.modulename = is.ReadString();
    }
    else
    {
        if (objversion == 10)
        {
            char fixedname[50];
            is.ReadBytes(fixedname, (int32_t)sizeof(fixedname));
            o.team.name.assign(fixedname, strnlen(fixedname, sizeof(fixedname)));
        }
        o.team.value = -1;
    }

    o.profile = {};
    o.frags = {};
    if (objversion >= 13)
    {
        is >> o.playerstate >> o.statetime;
        o.stateframes = int32_t(int64_t(o.statetime) * 24 / 100);
        for (std::string& text : o.profile)
            text = is.ReadString();
        is >> o.frags[0] >> o.frags[1];
        if (objversion >= 15)
            is >> o.frags[2] >> o.frags[3];
    }
    else
    {
        o.playerstate = 0;
        o.statetime = PlayScreen.GameTime();
        o.stateframes = PlayScreen.GameFrame();
    }

    o.automap = {};
    if (objversion >= 14)
        ReadAutoMapRecord(o.automap, is);

    o.RefreshStats();
    o.LogStats("loaded");
}

// REVSYNC: TContainer::Load @ 0x004dd3e0 (SAVE_GAME.md §11.3).
void TLegacyDeserialize::ReadContainer(TContainer& o, TInputStream& is, int32_t version, int32_t objversion)
{
    ReadObjectInstance(o, is, version, objversion);

    if (version >= 2 && version < 5) // We're using object stats now
    {
        int32_t contflags, pickdifficulty;
        is >> contflags >> pickdifficulty;
        o.SetStat("Locked", contflags != 0);
        o.SetStat("PickDifficulty", pickdifficulty);
    }

  // A container in the world re-enters its saved state (open or shut).
    if (!(o.flags & OF_INVENTORY))
    {
        o.SetCommandDone(true);
        o.SetState(o.state);
    }
}

void TLegacyDeserialize::ReadExit(TExit& o, TInputStream& is, int32_t version, int32_t objversion)
{
    ReadContainer(o, is, version, objversion);
    is >> o.exitflags;
}

void TLegacyDeserialize::ReadAmmo(TAmmo& o, TInputStream& is, int32_t version, int32_t objversion)
{
    ReadObjectInstance(o, is, version, objversion);

    if (version < 5)
    {
        int32_t amount;
        is >> amount;
        amount++;
        o.SetStat("Amount", amount);
    }
}

// REVSYNC: TMoney::Load @ 0x00515f40. Retail cleared OF_VIRGIN before
// loading; the base read replaces that flag from the file, so the clear has
// no effect and isn't ported.
void TLegacyDeserialize::ReadMoney(TMoney& o, TInputStream& is, int32_t version, int32_t objversion)
{
    ReadObjectInstance(o, is, version, objversion);

    if (version < 5)
    {
        int32_t amount;
        is >> amount;
        amount++;
        o.SetStat("Amount", amount);
    }
}

// REVSYNC: TFood::Load @ 0x0050eae0 (also POTION's; SAVE_GAME.md §11.3).
void TLegacyDeserialize::ReadFood(TFood& o, TInputStream& is, int32_t version, int32_t objversion)
{
    ReadObjectInstance(o, is, version, objversion);
    if (o.Amount() == 0)
        o.SetAmount(1);
}

void TLegacyDeserialize::ReadScroll(TScroll& o, TInputStream& is, int32_t version, int32_t objversion)
{
    ReadObjectInstance(o, is, version, objversion);

    short len;
    is >> len;

    if (len < 1)
        o.text = nullptr;
    else
    {
        o.text = new char[len+1];

        if (o.text)
        {
            int32_t i;
            for (i = 0; i < len; i++)
                is >> o.text[i];

            o.text[i] = 0;
        }
    }
}

void TLegacyDeserialize::ReadWeapon(TWeapon& o, TInputStream& is, int32_t version, int32_t objversion)
{
    ReadObjectInstance(o, is, version, objversion);

    is >> o.poison;
}

void TLegacyDeserialize::ReadDripEffect(TDripEffect& o, TInputStream& is, int32_t version, int32_t objversion)
{
    ReadObjectInstance(o, is, version, objversion);
    is >> o.ripplesize >> o.height >> o.period;
}

// REVSYNC: TMonsterGen::Load @ 0x00516e00 (vtable 0x160) -- base fields,
// then uint8 slot count; per slot the type name (NUL-terminated), int32
// treasure, int32 frequency, and from stream version 15 int32 clock start;
// int32 cap.
void TLegacyDeserialize::ReadMonsterGen(TMonsterGen& o, TInputStream& is, int32_t version, int32_t objversion)
{
    ReadObjectInstance(o, is, version, objversion);

    uint8_t count = 0;
    is >> count;
    for (int32_t i = 0; i < count; i++)
    {
        const std::string type = ReadCString(is);
        int32_t treasure = 0, frequency = 0;
        is >> treasure >> frequency;
        o.AddType(type.c_str(), frequency, treasure);

        // Before version 15 a loaded clock starts at the load. Retail put the
        // start in the i-th slot, which AddType has just filled.
        if (version >= 15)
        {
            int32_t start = 0;
            is >> start;
            if (i < TMonsterGen::kMaxTypes && o.slots[i])
                o.slots[i]->start = start;
        }
    }
    is >> o.maxmonsters;
}

// REVSYNC: TObjectInstance::LoadInventory @ 0x00472310 (vtable slot 0x168).
// Each item takes the first free slot (retail's array Add) and its invindex
// is set to that slot, so a loaded inventory is packed whatever invindex the
// file recorded.
void TLegacyDeserialize::ReadInventory(TObjectInstance& o, TInputStream& is, int32_t version, uint32_t streamflags)
{
    if (version < 3)
        return;

    int32_t num = 0;
    is >> num;
    if (num < 1 || num > 2048)
    {
        if (num != 0)
            log_warn("[object] invalid inventory size %d for %s", num, o.GetName() ? o.GetName() : "?");
        return;
    }

    for (int32_t i = 0; i < num; i++)
    {
        TObjectInstance* inst = ReadObject(is, version, streamflags);
        if (!inst)
            continue;
        inst->invindex = (short)o.inventory.Add(inst);
        inst->SetOwner(&o);
    }
}

// REVSYNC: TPlayer::LoadInventory @ 0x0051bd20. Equipping changes the
// maxima, which would clamp the saved health, fatigue and mana, so those
// are kept (as saved, not as modified) and set back afterwards.
void TLegacyDeserialize::ReadPlayerInventory(TPlayer& o, TInputStream& is, int32_t version, uint32_t streamflags)
{
    ReadInventory(o, is, version, streamflags);

    const int32_t health  = o.BaseObjStat(CHRSTAT_FIRST + CHRSTAT_HEALTH);
    const int32_t fatigue = o.BaseObjStat(CHRSTAT_FIRST + CHRSTAT_FATIGUE);
    const int32_t mana    = o.BaseObjStat(CHRSTAT_FIRST + CHRSTAT_MANA);
    o.RefreshStats();
    o.RefreshEquip();
    o.RefreshStats();
    o.SetHealth(health);
    o.SetMana(mana);
    o.SetFatigue(fatigue);

    o.LogStats("equipped");
}

// REVSYNC: automap record Load @ 0x00529770 (SAVE_GAME.md §11.5).
void TLegacyDeserialize::ReadAutoMapRecord(SAutoMapRecord& record, TInputStream& is)
{
    record.module = is.ReadString();
    int32_t count = 0;
    is >> count;
    record.levels.clear();
    for (int32_t i = 0; i < count && !is.Overrun(); i++)
    {
        SAutoMapLevel& entry = record.levels.emplace_back();
        int32_t words = 0;
        is >> entry.level >> words;
        if (words < 0 || words * 2 > is.Remaining())
        {
            log_warn("[player] automap record: level %d claims %d words; dropped", entry.level, words);
            record.levels.pop_back();
            break;
        }
        entry.mask.resize(words);
        for (int16_t& w : entry.mask)
            is >> w;
    }
}

// Class data a port class doesn't read yet: kept as bytes and written back
// after the class's own fields, so the object survives a round trip intact.
// REVSYNC-DIVERGENCE: retail always had the class (SAVE_GAME.md §11.3 lists
// the layouts these bytes follow).
void TLegacyDeserialize::KeepUnreadClassData(TObjectInstance& o, TInputStream& is, int32_t from, int32_t to)
{
    const int32_t count = to - from;
    if (count <= 0)
        return;
    o.unreadClassData = std::make_unique<std::vector<uint8_t>>(count);
    is.SetPos(from);
    is.ReadBytes(o.unreadClassData->data(), count);

    static std::set<std::string> logged;
    const char* type = o.inf && o.inf->name ? o.inf->name : "?";
    if (logged.insert(type).second)
        log_info("[object] '%s' (class %d): keeping %d bytes of class data the port doesn't read",
                 type, o.objclass, count);
}

// *******************
// * TLegacySerialize *
// *******************

int32_t TLegacySerialize::ObjVersion(const TObjectInstance& o)
{
    if (dynamic_cast<const TPlayer*>(&o))        return kPlayerVersion;
    if (dynamic_cast<const TCharacter*>(&o))     return kCharacterVersion;
    if (dynamic_cast<const TComplexObject*>(&o)) return kComplexObjectVersion;
    return kObjectInstanceVersion;
}

// REVSYNC: TObjectInstance::SaveObject (universal stream encoder) @ 0x00472110.
// Layout: SAVE_GAME.md §11.1. `streamflags` carries what retail kept in
// DAT_0065a250: OSTREAM_MAP writes a NONMAP object as the empty placeholder,
// OSTREAM_NOINVENTORY leaves inventories out.
void TLegacySerialize::WriteObject(TObjectInstance* o, TOutputStream& os, uint32_t streamflags)
{
    if (!o || ((streamflags & OSTREAM_MAP) && (o->Flags() & OF_NONMAP)))
    {
        os << (short)-1;
        return;
    }

    os << (short)ObjVersion(*o);
    os << (short)o->ObjClass();
    os << (uint32_t)o->ObjId();
    const int32_t sizes = os.GetPos();
    os << (short)0;                           // blocksize, patched below
    os << (short)0;                           // invblocksize, patched below

    const int32_t bodystart = os.GetPos();
    Write(*o, os);
    WriteUnreadClassData(*o, os);
    const int32_t bodyend = os.GetPos();

    // Retail calls SaveInventory only when there is something to save, so
    // an empty inventory writes no count and invblocksize stays 0.
    if (!(streamflags & OSTREAM_NOINVENTORY) && o->RealNumInventoryItems() > 0)
        WriteInventory(*o, os, streamflags);

    const int32_t end = os.GetPos();
    os.SetPos(sizes);
    os << (short)(end - bodystart);
    os << (short)(end - bodyend);
    os.SetPos(end);
}

void TLegacySerialize::Write(TObjectInstance& o, TOutputStream& os)
{
    if (auto* p = dynamic_cast<TPlayer*>(&o))             WritePlayer(*p, os);
    else if (auto* c = dynamic_cast<TCharacter*>(&o))     WriteCharacter(*c, os);
    else if (auto* x = dynamic_cast<TComplexObject*>(&o)) WriteComplexObject(*x, os);
    else if (auto* e = dynamic_cast<TExit*>(&o))          WriteExit(*e, os);
    else if (auto* v = dynamic_cast<TVialRack*>(&o))      WriteVialRack(*v, os);
    else if (auto* n = dynamic_cast<TContainer*>(&o))     WriteContainer(*n, os);
    else if (auto* s = dynamic_cast<TScroll*>(&o))        WriteScroll(*s, os);
    else if (auto* w = dynamic_cast<TWeapon*>(&o))        WriteWeapon(*w, os);
    else if (auto* d = dynamic_cast<TDripEffect*>(&o))    WriteDripEffect(*d, os);
    else if (auto* g = dynamic_cast<TMonsterGen*>(&o))    WriteMonsterGen(*g, os);
    else                                                  WriteObjectInstance(o, os);
}

// REVSYNC: TObjectInstance::Save @ 0x00472980 (vtable slot 0x164).
// Byte-symmetric with ReadObjectInstance above (SAVE_GAME.md §11.2).
void TLegacySerialize::WriteObjectInstance(TObjectInstance& o, TOutputStream& os)
{
    if (o.flags & OF_LIGHT)
        o.flags |= OF_PULSE | OF_ANIMATE;

    // Only a renamed instance stores its name, each byte with bit 7 set.
    const bool renamed = o.name && o.inf && o.inf->name && strcmp(o.name, o.inf->name) != 0;
    const uint8_t len = renamed ? (uint8_t)std::min<size_t>(strlen(o.name), 255) : 0;
    os << len;
    for (int32_t i = 0; i < len; i++)
        os << (uint8_t)(o.name[i] | kNameByteFlag);

    const S3DPoint pos = IntPos(o);
    os << o.flags << pos.x << pos.y << pos.z;

    if (!(o.flags & OF_IMMOBILE))
        os << o.vel.x << o.vel.y << o.vel.z;

    os << o.state;

    if (o.flags & OF_NONMAP)
        os << (uint16_t)o.GetLevel();

    os << o.inventnum << o.invindex << o.shadow <<
        (uint8_t)IntRotateX(o) << (uint8_t)IntRotateY(o) << (uint8_t)IntFacing(o) << o.FileMapIndex();

    if (o.flags & OF_ANIMATE)
        os << o.frame << o.framerate;

    os << o.group;

    os << (uint8_t)(o.cl->NumObjStats());
    for (int32_t c = 0; c < o.cl->NumObjStats(); c++)
        os << o.stats[c] << (uint32_t)(o.cl->ObjStatUniqueId(c) | kStatIdFlag);

    if (o.flags & OF_LIGHT)
        os << o.lightdef.flags << o.lightdef.pos.x << o.lightdef.pos.y << o.lightdef.pos.z <<
            o.lightdef.color.red << o.lightdef.color.green << o.lightdef.color.blue <<
            o.lightdef.intensity << o.lightdef.multiplier;
}

// REVSYNC: TComplexObject::Save @ 0x004dbb80.
void TLegacySerialize::WriteComplexObject(TComplexObject& o, TOutputStream& os)
{
    os << (uint8_t)kObjectInstanceVersion;
    WriteObjectInstance(o, os);

  // Save root state
    os << (uint8_t)o.root->action;
    os << o.root->name;
}

// REVSYNC: TCharacter::Save @ 0x004d50d0.
void TLegacySerialize::WriteCharacter(TCharacter& o, TOutputStream& os)
{
    os << (uint8_t)kComplexObjectVersion;
    WriteComplexObject(o, os);

  // Last time any health/fatigue/mana was recovered
    os << o.lasthealthrecov;
    os << o.lastfatiguerecov;
    os << o.lastmanarecov;

  // Last time poison damage was charged
    os << o.lastpoisondamage;

  // Our teleport positions
    os << o.teleport_position.x;
    os << o.teleport_position.y;
    os << o.teleport_position.z;
    os << o.teleport_level;
}

// REVSYNC: TPlayer::Save @ 0x0051bdc0 (objversion 15). Retail wrote its
// live HUD globals where the HUD words go; TSaveGame sets them from the
// HUD before saving.
void TLegacySerialize::WritePlayer(TPlayer& o, TOutputStream& os)
{
    os << (uint8_t)kCharacterVersion;
    WriteCharacter(o, os);

    for (const char* quickspell : o.quickspells)
        os << quickspell;

    os << (int32_t)o.knownspells.size();
    for (const TSpellCode& code : o.knownspells)
        os.WriteBytes(code.data(), kSpellCodeBytes);

    os << o.hudwords.sidebarOpen << o.hudwords.upperMode << o.hudwords.lowerMode << o.hudwords.drawerOpen;
    os << o.levelupstats[0] << o.levelupstats[1] << o.levelupstats[2];

    os << o.team.name.c_str() << o.team.name2.c_str();
    os << o.team.value << o.team.id << o.team.teamindex;
    os << o.modulename.c_str();

    os << o.playerstate << o.statetime;
    for (const std::string& text : o.profile)
        os << text.c_str();
    for (int32_t count : o.frags)
        os << count;

    // REVSYNC: 0x00529830 -- the level the automap shows goes into the
    // record first.
    if (AutoMap.IsBoundTo(o.automap))
        AutoMap.FlushGrid();
    WriteAutoMapRecord(o.automap, os);
}

void TLegacySerialize::WriteContainer(TContainer& o, TOutputStream& os)
{
    WriteObjectInstance(o, os);
}

void TLegacySerialize::WriteVialRack(TVialRack& o, TOutputStream& os)
{
    o.inventory.DeleteAll();
    WriteContainer(o, os);
}

void TLegacySerialize::WriteExit(TExit& o, TOutputStream& os)
{
    WriteContainer(o, os);
    os << o.exitflags;
}

void TLegacySerialize::WriteScroll(TScroll& o, TOutputStream& os)
{
    WriteObjectInstance(o, os);

    short len = o.text ? strlen(o.text) : 0;
    os << len;

    if (len)
    {
        for (int32_t i = 0; i < len; i++)
            os << o.text[i];
    }
}

void TLegacySerialize::WriteWeapon(TWeapon& o, TOutputStream& os)
{
    WriteObjectInstance(o, os);

    os << o.poison;
}

void TLegacySerialize::WriteDripEffect(TDripEffect& o, TOutputStream& os)
{
    WriteObjectInstance(o, os);
    os << o.ripplesize << o.height << o.period;
}

// REVSYNC: TMonsterGen::Save @ 0x00516eb0 (vtable 0x164) -- the same, the
// used slots only.
void TLegacySerialize::WriteMonsterGen(TMonsterGen& o, TOutputStream& os)
{
    WriteObjectInstance(o, os);

    os << uint8_t(o.NumTypes());
    for (const auto& slot : o.slots)
    {
        if (!slot)
            continue;
        os.WriteBytes(slot->type.c_str(), int32_t(slot->type.size()) + 1);
        os << slot->treasure << slot->frequency << slot->start;
    }
    os << o.maxmonsters;
}

// REVSYNC: TObjectInstance::SaveInventory @ 0x00472380 (vtable slot 0x16c):
// int32 count, then each item. REVSYNC-DIVERGENCE: the count is the number
// of items. Retail wrote the array's high-water mark, which overcounts after
// an item is removed from the middle, so its next load read the following
// data as inventory (SAVE_GAME.md §11.1).
void TLegacySerialize::WriteInventory(TObjectInstance& o, TOutputStream& os, uint32_t streamflags)
{
    os << (int32_t)o.RealNumInventoryItems();
    for (TInventoryIterator i(&o); i; i++)
        WriteObject(i.Item(), os, streamflags);
}

void TLegacySerialize::WriteUnreadClassData(const TObjectInstance& o, TOutputStream& os)
{
    if (o.unreadClassData && !o.unreadClassData->empty())
        os.WriteBytes(o.unreadClassData->data(), (int32_t)o.unreadClassData->size());
}

// REVSYNC: automap record Save @ 0x00529830. The player's save first has
// the automap fold the level it shows into the record (0x0052c5c0).
void TLegacySerialize::WriteAutoMapRecord(const SAutoMapRecord& record, TOutputStream& os)
{
    os << record.module.c_str();
    os << (int32_t)record.levels.size();
    for (const SAutoMapLevel& entry : record.levels)
    {
        os << entry.level << (int32_t)entry.mask.size();
        for (int16_t w : entry.mask)
            os << w;
    }
}
