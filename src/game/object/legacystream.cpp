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
#include "game/world/lightcomponent.h"

#include "game/object/ammo.h"
#include "game/world/automap.h"
#include "game/object/character.h"
#include "game/object/characterdef.h"
#include "game/object/inventorycomponent.h"
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
    SObjectState def;
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

// REVSYNC: TObjectInstance::Load @ 0x0046f0b0, the bytes half: into the
// record. `o` supplies what the read depends on (the constructor's fixed
// flags, the class); nothing is written to it here.
TLegacyDeserialize::SLoadDerived TLegacyDeserialize::ReadObjectInstanceState(SObjectState& st, TObjectInstance& o,
                                                                              TInputStream& is, int32_t version)
{
    SLoadDerived derived;
    st.objclass = o.objclass;
    st.objtype  = o.objtype;

    // Names are stored with bit 7 set on every byte; length 0 means "the
    // type's name", which the constructor already set.
    uint8_t len;
    is >> len;
    st.name.resize(len);
    for (int32_t i = 0; i < len; i++)
    {
        uint8_t encoded;
        is >> encoded;
        st.name[i] = (char)(encoded & kNameByteMask);
    }

    uint32_t newflags;
    is >> newflags >> st.pos.x >> st.pos.y >> st.pos.z;

  // The constructor's fixed flags stay; everything else comes from the file
    st.flags = (o.flags & OF_FIXEDFLAGS) | (newflags & ~OF_FIXEDFLAGS);

  // Don't load velocity vectors if not mobile
    if (version < 6 || !(st.flags & OF_IMMOBILE))
        is >> st.vel.x >> st.vel.y >> st.vel.z;

    if (version < 9)
    {
        uint8_t statebyte;
        is >> statebyte;
        st.state = statebyte;
    }
    else
        is >> st.state;

  // Non map objects (i.e. players) store their level: they come from the
  // save, not a sector, so the level says where to put them.
    st.level = 0;
    if (version >= 6 && (st.flags & OF_NONMAP))
    {
        if (version < 9)
        {
            uint8_t levelbyte;
            is >> levelbyte;
            st.level = levelbyte;
        }
        else
            is >> st.level;
    }

    if (version < 5)
        is >> derived.health; // Now an object stat

    if (version < 3)
    {
        uint8_t dummy8;
        short dummy16, inventnum;
        int32_t shadow;
        is >> st.rotatez >> dummy16 >> inventnum >> dummy16 >> shadow >> dummy8;
        st.inventnum = -1;      // ignore inventories in old versions
        st.shadow = shadow;
        derived.clearmapindex = true;   // the sector sets old-style indexes after the load
    }
    else
    {
        short inventnum, invindex;
        is >> inventnum >> invindex >> st.shadow >> st.rotatex >> st.rotatey >> st.rotatez >> st.mapindex;
        st.inventnum = inventnum;
        st.invindex  = invindex;
    }

    if (version < 5)  // Empty stat array with health stuck in it
    {
        st.frame = 0;
        st.framerate = 1;
        st.group = 0;
    }
    else
    {
        if (version < 6 || (st.flags & OF_ANIMATE))
        {
            short frame, framerate;
            is >> frame >> framerate;
            st.frame = frame;
            st.framerate = framerate;
        }

        is >> st.group;

        // Each saved stat carries its unique id (bit 7 in every byte); the
        // record keeps the pairs and Apply matches them to the class's stats.
        uint8_t numstats;
        is >> numstats;
        for (int32_t n = 0; n < numstats; n++)
        {
            SObjectStatState saved;
            is >> saved.value >> saved.id;
            saved.id &= kStatIdMask;
            st.stats.push_back(saved);
        }
    }

    if (st.flags & OF_LIGHT)
    {
        uint8_t flags, red, green, blue, intensity;
        short multiplier;
        is >> flags >> st.light.offset.x >> st.light.offset.y >> st.light.offset.z >>
            red >> green >> blue >> intensity >> multiplier;
        st.light.flags = flags;
        st.light.red = red; st.light.green = green; st.light.blue = blue;
        st.light.intensity = intensity;
        st.light.multiplier = multiplier;
        st.flags |= OF_LIGHT | OF_ANIMATE;
    }
    return derived;
}

// REVSYNC: the tail of TObjectInstance::Load @ 0x0046f0b0: what retail
// settled once the object was loaded, after the record is on it.
void TLegacyDeserialize::FinishObjectInstance(TObjectInstance& o, int32_t version, const SLoadDerived& derived)
{
  // What retail derived at load: the move angle starts as the facing; an
  // old map's health lands in the stat it became; old indexes are cleared
    o.moveangle = IntFacing(o);
    if (version < 5)
        o.SetHealth(derived.health);
    if (derived.clearmapindex)
        o.Node()->SetMapIndex(-1);

  // Set Object info pointer
    o.cl = TObjectClass::GetClass(o.objclass);
    if (!o.cl)
        FatalError("Bad object class!"); // This should never happen!!
    o.inf = o.cl->GetObjType(o.objtype);

  // Any script the object had goes; the right one is attached below
    o.ClearScript();

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

// REVSYNC: TObjectInstance::Load @ 0x0046f0b0: a base object, whole.
void TLegacyDeserialize::ReadObjectInstance(TObjectInstance& o, TInputStream& is, int32_t version, int32_t objversion)
{
    (void)objversion;
    SObjectState st;
    const SLoadDerived derived = ReadObjectInstanceState(st, o, is, version);
    o.Archetype()->Apply(st, o);
    FinishObjectInstance(o, version, derived);
}

// REVSYNC: TComplexObject::Load @ 0x004db930 (SAVE_GAME.md §11.3), the
// bytes half: the base record, then the root action block. The root's
// fallback and the state that goes with it are the def's (TComplexObjectDef).
void TLegacyDeserialize::ReadComplexObjectState(SComplexObjectState& st, TComplexObject& o, TInputStream& is,
                                                int32_t version, int32_t objversion, SLoadDerived& derived)
{
    uint8_t basever = 0;
    if (objversion >= 1)
        is >> basever;
    derived = ReadObjectInstanceState(st, o, is, version);
  // Load root state (a map older than version 7 has none: the type's root)
    if (version >= 7)
    {
        uint8_t action;
        is >> action;
        st.rootaction = action;
        st.rootname = is.ReadString();
    }
}

void TLegacyDeserialize::ReadComplexObject(TComplexObject& o, TInputStream& is, int32_t version, int32_t objversion)
{
    SComplexObjectState st;
    SLoadDerived derived;
    ReadComplexObjectState(st, o, is, version, objversion, derived);
    o.Archetype()->Apply(st, o);
    FinishObjectInstance(o, version, derived);
}

// REVSYNC: TCharacter::Load @ 0x004d4eb0 (SAVE_GAME.md §11.3), the bytes
// half: the complex object's record, then the character's fields. What
// retail settled after (the vitals clamped to the type's, a dead character
// removed, the fade reset) is the def's (TCharacterDef::Apply).
void TLegacyDeserialize::ReadCharacterState(SCharacterState& st, TCharacter& o, TInputStream& is,
                                            int32_t version, int32_t objversion, SLoadDerived& derived)
{
    uint8_t basever = 0;
    if (objversion >= 3)
        is >> basever;
    ReadComplexObjectState(st, o, is, version, basever, derived);

  // Get saved last pulse values (so we can figure what has happened to char)
    if (objversion >= 1)
    {
      // Last time any health/fatigue/mana was recovered
        is >> st.lasthealthrecov;
        is >> st.lastfatiguerecov;
        is >> st.lastmanarecov;
        if (objversion >= 4)
            is >> st.lastpoisondamage;
        else
            st.lastpoisondamage = -1;
        if (objversion >= 2)
        {
            is >> st.teleport_position.x;
            is >> st.teleport_position.y;
            is >> st.teleport_position.z;
            is >> st.teleport_level;
        }
        else
        {
            st.teleport_position = {-1, -1, -1};
            st.teleport_level = -1;
        }
    }
}

void TLegacyDeserialize::ReadCharacter(TCharacter& o, TInputStream& is, int32_t version, int32_t objversion)
{
    SCharacterState st;
    SLoadDerived derived;
    ReadCharacterState(st, o, is, version, objversion, derived);
    o.Archetype()->Apply(st, o);
    FinishObjectInstance(o, version, derived);
}

// REVSYNC: TPlayer::Load @ 0x0051b960 (SAVE_GAME.md §11.4): the player's
// record on top of the character's. What retail derived on load (the clock
// in frames, RefreshStats) is the def's (TPlayerDef::Apply).
void TLegacyDeserialize::ReadPlayer(TPlayer& o, TInputStream& is, int32_t version, int32_t objversion)
{
    SPlayerState st;
    SLoadDerived derived;
    uint8_t basever = (uint8_t)objversion;
    if (objversion >= 4)
        is >> basever;
    ReadCharacterState(st, o, is, version, basever, derived);

    if (objversion >= 4)
    {
        for (int32_t i = 0; i < QSPELL_NUM; i++)
        {
            // A stored name longer than the buffer was cut to fit
            std::string spell = is.ReadString();
            spell.resize(std::min<size_t>(spell.size(), MAXTALISMANLEN - 1));
            st.quickspells.push_back(spell);
        }
    }
    if (objversion >= 5)
    {
        int32_t count = 0;
        is >> count;
        for (int32_t i = 0; i < count && !is.Overrun(); i++)
        {
            char code[kSpellCodeBytes] = {};
            is.ReadBytes(code, kSpellCodeBytes);
            st.knownspells.emplace_back(code, strnlen(code, kSpellCodeBytes));
        }
        if (objversion >= 6 && objversion <= 8)
            is.MovePos(count * 4);  // a per-spell int those versions had
    }
    // Retail kept these in a member nothing reads; the port restores the HUD
    // from them (TSaveGame).
    if (objversion >= 7)
        is >> st.sidebarOpen >> st.upperMode >> st.lowerMode >> st.drawerOpen;
    if (objversion >= 8)
    {
        st.levelupstats.resize(3);
        is >> st.levelupstats[0] >> st.levelupstats[1] >> st.levelupstats[2];
    }
    if (objversion >= 13)
    {
        st.teamname  = is.ReadString();
        st.teamname2 = is.ReadString();
        is >> st.teamvalue >> st.teamid >> st.teamindex;
        st.modulename = is.ReadString();
    }
    else if (objversion >= 11)
    {
        st.teamname  = is.ReadString();
        st.teamname2 = is.ReadString();
        is >> st.teamvalue;
        st.modulename = is.ReadString();
    }
    else
    {
        if (objversion == 10)
        {
            char fixedname[50];
            is.ReadBytes(fixedname, (int32_t)sizeof(fixedname));
            st.teamname.assign(fixedname, strnlen(fixedname, sizeof(fixedname)));
        }
        st.teamvalue = -1;
    }
    if (objversion >= 13)
    {
        is >> st.playerstate >> st.statetime;
        st.profile.resize(4);
        for (std::string& text : st.profile)
            text = is.ReadString();
        st.frags.resize(4);
        is >> st.frags[0] >> st.frags[1];
        if (objversion >= 15)
            is >> st.frags[2] >> st.frags[3];
    }
    else
    {
        st.playerstate = 0;
        st.statetime = PlayScreen.GameTime();
    }
    if (objversion >= 14)
    {
        SAutoMapRecord record;
        ReadAutoMapRecord(record, is);
        st.automap = AutoMapToState(record);
    }
    o.Archetype()->Apply(st, o);
    FinishObjectInstance(o, version, derived);
    o.LogStats("loaded");
}

// REVSYNC: TContainer::Load. An old map (versions 2-4) carried the lock as
// fields that became stats; a container in the world re-enters its saved
// state (open or shut).
void TLegacyDeserialize::FinishContainer(TContainer& o, TInputStream& is, int32_t version)
{
    if (version >= 2 && version < 5) // We're using object stats now
    {
        int32_t contflags, pickdifficulty;
        is >> contflags >> pickdifficulty;
        o.SetStat("Locked", contflags != 0);
        o.SetStat("PickDifficulty", pickdifficulty);
    }
    if (!(o.flags & OF_INVENTORY))
    {
        o.SetCommandDone(true);
        o.SetState(o.state);
    }
}

void TLegacyDeserialize::ReadContainer(TContainer& o, TInputStream& is, int32_t version, int32_t objversion)
{
    ReadObjectInstance(o, is, version, objversion);
    FinishContainer(o, is, version);
}

void TLegacyDeserialize::ReadExit(TExit& o, TInputStream& is, int32_t version, int32_t objversion)
{
    (void)objversion;
    SExitState st;
    const SLoadDerived derived = ReadObjectInstanceState(st, o, is, version);
    // The old-map lock fields sit between the base and the exit flags
    int32_t oldlock[2] = {0, 0};
    const bool oldmap = version >= 2 && version < 5;
    if (oldmap)
        is >> oldlock[0] >> oldlock[1];
    is >> st.exitflags;
    o.Archetype()->Apply(st, o);
    FinishObjectInstance(o, version, derived);
    if (oldmap)
    {
        o.SetStat("Locked", oldlock[0] != 0);
        o.SetStat("PickDifficulty", oldlock[1]);
    }
    if (!(o.flags & OF_INVENTORY))
    {
        o.SetCommandDone(true);
        o.SetState(o.state);
    }
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
    (void)objversion;
    SScrollState st;
    const SLoadDerived derived = ReadObjectInstanceState(st, o, is, version);
    short len;
    is >> len;
    if (len > 0)
    {
        st.text.resize(size_t(len));
        for (int32_t i = 0; i < len; i++)
            is >> st.text[size_t(i)];
    }
    o.Archetype()->Apply(st, o);
    FinishObjectInstance(o, version, derived);
}

void TLegacyDeserialize::ReadWeapon(TWeapon& o, TInputStream& is, int32_t version, int32_t objversion)
{
    (void)objversion;
    SWeaponState st;
    const SLoadDerived derived = ReadObjectInstanceState(st, o, is, version);
    is >> st.poison;
    o.Archetype()->Apply(st, o);
    FinishObjectInstance(o, version, derived);
}

void TLegacyDeserialize::ReadDripEffect(TDripEffect& o, TInputStream& is, int32_t version, int32_t objversion)
{
    (void)objversion;
    SDripState st;
    const SLoadDerived derived = ReadObjectInstanceState(st, o, is, version);
    is >> st.ripplesize >> st.height >> st.period;
    o.Archetype()->Apply(st, o);
    FinishObjectInstance(o, version, derived);
}

// REVSYNC: TMonsterGen::Load @ 0x00516e00 (vtable 0x160) -- base fields,
// then uint8 slot count; per slot the type name (NUL-terminated), int32
// treasure, int32 frequency, and from stream version 15 int32 clock start;
// int32 cap. Before version 15 a loaded clock starts at the load.
void TLegacyDeserialize::ReadMonsterGen(TMonsterGen& o, TInputStream& is, int32_t version, int32_t objversion)
{
    (void)objversion;
    SMonsterGenState st;
    const SLoadDerived derived = ReadObjectInstanceState(st, o, is, version);
    uint8_t count = 0;
    is >> count;
    for (int32_t i = 0; i < count; i++)
    {
        SMonsterGenSlotState slot;
        slot.type = ReadCString(is);
        is >> slot.treasure >> slot.frequency;
        if (version >= 15)
            is >> slot.start;
        st.slots.push_back(slot);
    }
    is >> st.maxmonsters;
    o.Archetype()->Apply(st, o);
    FinishObjectInstance(o, version, derived);
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
        inst->invindex = (short)o.EnsureInventory().Add(inst);
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

// Retail saved a lit object animating and pulsing (and left it so).
void TLegacySerialize::SettleFlagsForSave(TObjectInstance& o)
{
    if (o.flags & OF_LIGHT)
        o.flags |= OF_PULSE | OF_ANIMATE;
}

// REVSYNC: TObjectInstance::Save @ 0x0046f8a0: the record to the bytes, in
// retail's order.
void TLegacySerialize::WriteObjectInstanceState(const SObjectState& st, TOutputStream& os)
{
    // Only a renamed instance stores its name, each byte with bit 7 set.
    const uint8_t len = (uint8_t)std::min<size_t>(st.name.size(), 255);
    os << len;
    for (int32_t i = 0; i < len; i++)
        os << (uint8_t)(st.name[i] | kNameByteFlag);

    os << st.flags << st.pos.x << st.pos.y << st.pos.z;

    if (!(st.flags & OF_IMMOBILE))
        os << st.vel.x << st.vel.y << st.vel.z;

    os << st.state;

    if (st.flags & OF_NONMAP)
        os << st.level;

    os << (short)st.inventnum << (short)st.invindex << st.shadow <<
        st.rotatex << st.rotatey << st.rotatez << st.mapindex;

    if (st.flags & OF_ANIMATE)
        os << (short)st.frame << (short)st.framerate;

    os << st.group;

    os << (uint8_t)st.stats.size();
    for (const SObjectStatState& stat : st.stats)
        os << stat.value << (uint32_t)(stat.id | kStatIdFlag);

    if (st.flags & OF_LIGHT)
        os << (uint8_t)st.light.flags << st.light.offset.x << st.light.offset.y << st.light.offset.z <<
            (uint8_t)st.light.red << (uint8_t)st.light.green << (uint8_t)st.light.blue <<
            (uint8_t)st.light.intensity << (short)st.light.multiplier;
}

void TLegacySerialize::WriteObjectInstance(TObjectInstance& o, TOutputStream& os)
{
    SettleFlagsForSave(o);
    SObjectState st;
    o.Archetype()->Capture(o, st);
    WriteObjectInstanceState(st, os);
}

// REVSYNC: TComplexObject::Save @ 0x004dbb80: the base's bytes under its
// version, then the root action block.
void TLegacySerialize::WriteComplexObjectState(const SComplexObjectState& st, TOutputStream& os)
{
    os << (uint8_t)kObjectInstanceVersion;
    WriteObjectInstanceState(st, os);
    os << (uint8_t)st.rootaction;
    os << st.rootname.c_str();
}

void TLegacySerialize::WriteComplexObject(TComplexObject& o, TOutputStream& os)
{
    SettleFlagsForSave(o);
    SComplexObjectState st;
    o.Archetype()->Capture(o, st);
    WriteComplexObjectState(st, os);
}

// REVSYNC: TCharacter::Save @ 0x004d50d0.
void TLegacySerialize::WriteCharacterState(const SCharacterState& st, TOutputStream& os)
{
    os << (uint8_t)kComplexObjectVersion;
    WriteComplexObjectState(st, os);
  // Last time any health/fatigue/mana was recovered
    os << st.lasthealthrecov;
    os << st.lastfatiguerecov;
    os << st.lastmanarecov;
  // Last time poison damage was charged
    os << st.lastpoisondamage;
  // Our teleport positions
    os << st.teleport_position.x;
    os << st.teleport_position.y;
    os << st.teleport_position.z;
    os << st.teleport_level;
}

void TLegacySerialize::WriteCharacter(TCharacter& o, TOutputStream& os)
{
    SettleFlagsForSave(o);
    SCharacterState st;
    o.Archetype()->Capture(o, st);
    WriteCharacterState(st, os);
}

// REVSYNC: TPlayer::Save @ 0x0051bdc0 (objversion 15). Retail wrote its
// live HUD globals where the HUD words go; TSaveGame sets them from the
// HUD before saving.
void TLegacySerialize::WritePlayer(TPlayer& o, TOutputStream& os)
{
    SettleFlagsForSave(o);
    // REVSYNC: 0x00529830 -- the level the automap shows goes into the
    // record first.
    if (AutoMap.IsBoundTo(o.automap))
        AutoMap.FlushGrid();
    SPlayerState st;
    o.Archetype()->Capture(o, st);
    os << (uint8_t)kCharacterVersion;
    WriteCharacterState(st, os);
    for (int32_t i = 0; i < QSPELL_NUM; i++)
        os << (size_t(i) < st.quickspells.size() ? st.quickspells[size_t(i)].c_str() : "");
    os << (int32_t)st.knownspells.size();
    for (const std::string& code : st.knownspells)
    {
        char bytes[kSpellCodeBytes] = {};
        memcpy(bytes, code.data(), std::min<size_t>(code.size(), kSpellCodeBytes));
        os.WriteBytes(bytes, kSpellCodeBytes);
    }
    os << st.sidebarOpen << st.upperMode << st.lowerMode << st.drawerOpen;
    for (int32_t i = 0; i < 3; i++)
        os << (size_t(i) < st.levelupstats.size() ? st.levelupstats[size_t(i)] : 0);
    os << st.teamname.c_str() << st.teamname2.c_str();
    os << st.teamvalue << st.teamid << st.teamindex;
    os << st.modulename.c_str();
    os << st.playerstate << st.statetime;
    for (int32_t i = 0; i < 4; i++)
        os << (size_t(i) < st.profile.size() ? st.profile[size_t(i)].c_str() : "");
    for (int32_t i = 0; i < 4; i++)
        os << (size_t(i) < st.frags.size() ? st.frags[size_t(i)] : 0);
    WriteAutoMapRecord(AutoMapFromState(st.automap), os);
}

void TLegacySerialize::WriteContainer(TContainer& o, TOutputStream& os)
{
    WriteObjectInstance(o, os);
}

void TLegacySerialize::WriteVialRack(TVialRack& o, TOutputStream& os)
{
    // A vial rack is saved empty (retail deleted its vials here, with the
    // array's delete; they are node-owned objects, so DeleteObject)
    for (TInventoryIterator i(&o); i; i++)
    {
        i.Item()->RemoveFromInventory();
        DeleteObject(i.Item());
    }
    WriteContainer(o, os);
}

void TLegacySerialize::WriteExit(TExit& o, TOutputStream& os)
{
    SettleFlagsForSave(o);
    SExitState st;
    o.Archetype()->Capture(o, st);
    WriteObjectInstanceState(st, os);
    os << uint32_t(st.exitflags);
}

void TLegacySerialize::WriteScroll(TScroll& o, TOutputStream& os)
{
    SettleFlagsForSave(o);
    SScrollState st;
    o.Archetype()->Capture(o, st);
    WriteObjectInstanceState(st, os);
    const short len = (short)st.text.size();
    os << len;
    for (int32_t i = 0; i < len; i++)
        os << st.text[size_t(i)];
}

void TLegacySerialize::WriteWeapon(TWeapon& o, TOutputStream& os)
{
    SettleFlagsForSave(o);
    SWeaponState st;
    o.Archetype()->Capture(o, st);
    WriteObjectInstanceState(st, os);
    os << st.poison;
}

void TLegacySerialize::WriteDripEffect(TDripEffect& o, TOutputStream& os)
{
    SettleFlagsForSave(o);
    SDripState st;
    o.Archetype()->Capture(o, st);
    WriteObjectInstanceState(st, os);
    os << st.ripplesize << st.height << st.period;
}

// REVSYNC: TMonsterGen::Save @ 0x00516eb0 (vtable 0x164) -- the same, the
// used slots only.
void TLegacySerialize::WriteMonsterGen(TMonsterGen& o, TOutputStream& os)
{
    SettleFlagsForSave(o);
    SMonsterGenState st;
    o.Archetype()->Capture(o, st);
    WriteObjectInstanceState(st, os);
    os << uint8_t(st.slots.size());
    for (const SMonsterGenSlotState& slot : st.slots)
    {
        os.WriteBytes(slot.type.c_str(), int32_t(slot.type.size()) + 1);
        os << slot.treasure << slot.frequency << slot.start;
    }
    os << st.maxmonsters;
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
