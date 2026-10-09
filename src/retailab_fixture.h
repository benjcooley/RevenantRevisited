// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *  retailab_fixture.h - fixture worlds for the combat A/B targets       *
// *************************************************************************
//
// The port's side of the combat dojo's fixtures (docs/gameplay/COMBAT_DOJO.md
// §1, §6.3), shared by the retailab_*.cpp targets: characters built from a
// case (a JSON world), their imagery as a header from the case's state
// table, and the seams -- each answered from the case and recorded, as the
// retail fixture in tools/retail_runtime/slots/combat/ records its own.

#pragma once

#include "retailab.h"
#include "retailab_json.h"

#include "3dimage.h"
#include "character.h"
#include "imagery.h"
#include "imageres.h"
#include "player.h"
#include "rules.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <initializer_list>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <strings.h>
#include <type_traits>
#include <vector>

namespace RetailAB::Fixture
{

// The game data the fixtures build on (class.def, the trig tables), once per
// process; false with `error` set if it can't be loaded.
bool LoadGameData(std::string& error);

// The case's seam calls, in order, each one JSON object.
void Seam(const std::string& record);
void ClearSeams();
void WriteSeams(JsonOut& j);
size_t SeamCount();
void WriteSeamsSince(JsonOut& j, size_t first);   // "seams": those from `first` on

// The case's random draws ("draws": {lo, hi, result} for random(), {rand}
// for a direct draw), from its `tape` and then retail's generator from its
// `seed`, as the retail fixture answers them (SCaseScope installs it).
void WriteDraws(JsonOut& j);
// Forget the draws so far (the tape goes on where it was): a case that sets
// something up with draws of its own before the call it compares.
void ClearDraws();

// Action block flags by meaning (the port's bits under retail's names).
uint32_t FlagBits(const JsonValue& names);
void WriteFlags(JsonOut& j, uint32_t bits);

// A case's character data, by the field names of the combat-data dump
// (retailab_data.cpp): only the fields given are set, over SCharData's
// defaults. `attacks` and `impacts` replace the tables.
void ReadCharData(const JsonValue& c, SCharData& cd);

// The name a case gives an object stat (retail's stat ids: charstats.h),
// as the retail fixture names them; null for one it doesn't name.
const char* ObjStatName(int32_t statid);

// ---- Fixture imagery ------------------------------------------------------

// A state table entry as the case gives it: a name, or
// {"name", "frames" (10), "aniflags" (0), "motion"}; motion, one
// [dist, vert, ang, rotx, roty, rotz] a frame (SMotionData's fields).
struct SFixtureState
{
    std::string name;
    int32_t frames = 10;
    int32_t aniflags = 0;
    std::vector<std::array<int32_t, 6>> motion;
};

// Kata M8: the animation layer runs as the game's own (SetState, the
// imagery's SetObjectMotion) instead of being answered (set per case).
inline bool g_realAnimation = false;

std::vector<SFixtureState> ReadStates(const JsonValue& list);

// 3D imagery whose header is the case's state table, registered under a
// name of its own (the registry keeps every entry, so each one is new), its
// mesh marked built and its motion the case's: T3DImagery's SetObjectMotion
// runs over it. The state queries are recorded.
class TFixtureImagery : public T3DImagery
{
  public:
    TFixtureImagery(int32_t id, std::string owner, const std::vector<SFixtureState>& states, bool needsanim)
        : T3DImagery(id), who(std::move(owner)), needsanimator(needsanim)
    {
        meshinitialized = true;
        motion = new SMotionData*[states.empty() ? 1 : states.size()]();
        for (size_t i = 0; i < states.size(); ++i)
        {
            if (states[i].motion.empty())
                continue;
            motion[i] = new SMotionData[states[i].motion.size()]();
            for (size_t f = 0; f < states[i].motion.size(); ++f)
            {
                const auto& m = states[i].motion[f];
                SMotionData& md = motion[i][f];
                md.dist = (uint32_t)m[0] & 0xffff;
                md.vert = (int16_t)m[1];
                md.ang = (uint32_t)m[2] & 0xff;
                md.rotx = (uint32_t)m[3] & 0xff;
                md.roty = (uint32_t)m[4] & 0xff;
                md.rotz = (uint32_t)m[5] & 0xff;
            }
        }
    }

    static int32_t Register(const std::vector<SFixtureState>& states)
    {
        static int32_t serial = 0;
        const size_t n = states.empty() ? 1 : states.size();
        const size_t size = sizeof(SImageryHeader) + (n - 1) * sizeof(SImageryStateHeader);
        auto* header = (SImageryHeader*)calloc(1, size);
        header->numstates = (int32_t)states.size();
        for (size_t i = 0; i < states.size(); ++i)
        {
            strncpy(header->states[i].animname, states[i].name.c_str(), MAXANIMNAME - 1);
            header->states[i].frames = (short)states[i].frames;
            header->states[i].aniflags = (short)states[i].aniflags;
        }
        char filename[64];
        snprintf(filename, sizeof(filename), "retailab-combat-%d", ++serial);
        const int32_t id = RegisterImagery(filename, header, (uint32_t)size);
        // Resident: there is no body file to load, and nothing here draws.
        SImageryEntry* entry = GetImageryEntry(id);
        entry->body = (SImageryBody*)calloc(1, 16);
        entry->status = QE_LOADED;
        return id;
    }

    int32_t NumStates() const override
    {
        const int32_t n = TObjectImagery::NumStates();
        JsonOut j;
        j.Begin('{').FieldString("seam", "imagery.NumStates").FieldString("who", who);
        j.Key("args").Begin('[').End(']').Field("result", n).End('}');
        Seam(j.str());
        return n;
    }

    int32_t GetAniFlags(int32_t state) const override
    {
        return Query("GetAniFlags", state, TObjectImagery::GetAniFlags(state));
    }
    int32_t GetAniLength(int32_t state) const override
    {
        return Query("GetAniLength", state, TObjectImagery::GetAniLength(state));
    }
    int32_t GetInvAniFlags(int32_t state) const override
    {
        return Query("GetInvAniFlags", state, TObjectImagery::GetAniFlags(state));
    }
    int32_t GetInvAniLength(int32_t state) const override
    {
        return Query("GetInvAniLength", state, TObjectImagery::GetAniLength(state));
    }
    bool NeedsAnimator(const TObjectInstance* oi) const override
    {
        JsonOut j;
        j.Begin('{').FieldString("seam", "imagery.NeedsAnimator").FieldString("who", who);
        j.Key("args").Begin('[').Value(0).End(']').Field("result", needsanimator ? 1 : 0).End('}');
        Seam(j.str());
        return needsanimator;
    }

  private:
    int32_t Query(const char* name, int32_t state, int32_t result) const
    {
        JsonOut j;
        j.Begin('{').FieldString("seam", std::string("imagery.") + name).FieldString("who", who);
        j.Key("args").Begin('[').Value(state).End(']').Field("result", result).End('}');
        Seam(j.str());
        return result;
    }

    std::string who;
    bool needsanimator = true;
};

// An animator that records what the object layer tells it (retail's
// stand-in answers the same slots: Close, ResetState, SetComplete,
// SetNewState, and the delete).
class TFixtureAnimator : public TObjectAnimator
{
  public:
    TFixtureAnimator(TObjectInstance* oi, std::string owner) : TObjectAnimator(oi), who(std::move(owner)) {}
    ~TFixtureAnimator() override
    {
        // ~TObjectAnimator closes it: Close, then the delete, as retail's
        // FreeAnimator calls them.
        Record("Close", {});
        Record("delete", {1});
    }
    void ResetState() override { Record("ResetState", {}); }
    void SetComplete(bool comp) override
    {
        Record("SetComplete", {comp ? 1 : 0});
        TObjectAnimator::SetComplete(comp);
    }
    void SetNewState(bool newst) override
    {
        Record("SetNewState", {newst ? 1 : 0});
        TObjectAnimator::SetNewState(newst);
    }

  private:
    void Record(const char* name, std::initializer_list<int32_t> args) const
    {
        JsonOut j;
        j.Begin('{').FieldString("seam", std::string("animator.") + name).FieldString("who", who);
        j.Key("args").Begin('[');
        for (int32_t a : args)
            j.Value(a);
        j.End(']').Field("result", 0).End('}');
        Seam(j.str());
    }

    std::string who;
};

// ---- Fixture characters ---------------------------------------------------

// What the world needs from a fixture character, whatever its base class.
class IFixtureChar
{
  public:
    virtual ~IFixtureChar() = default;
    virtual void SetBlocks(TActionBlock* r, TActionBlock* d, TActionBlock* w) = 0;
    virtual TActionBlock* Root() = 0;
    virtual TActionBlock* Doing() = 0;
    virtual TActionBlock* Desired() = 0;
    // The resolvers ResolveAction would call on the doing block.
    virtual int32_t ResolveCombat(int32_t bits) = 0;
    virtual int32_t ResolveCombatMove(int32_t bits) = 0;
    // The attack bookkeeping (timers, request bits, the player's button
    // repeats, the chain, the last attack by index), by retail's names.
    virtual void WriteAttackState(JsonOut& j) = 0;
    virtual void RunUpdateAction(int32_t bits) = 0;
    virtual void RunComplexPulse() = 0;         // TComplexObject::Pulse (UpdateAction with the move bits)
    // What a move changes beyond the character dump ("motion").
    virtual void WriteMotion(JsonOut& j) = 0;
};

// A TCharacter or TPlayer built from the case, without a class record or
// loaded imagery: it answers the seams itself.
template <typename Base>
class TFixtureChar : public Base, public IFixtureChar
{
  public:
    TFixtureChar(TObjectImagery* im, const JsonValue& spec, std::vector<SFixtureState> table)
        : Base(Def(spec).get(), im), states(std::move(table))
    {
        who = spec["name"].Str();
        this->name = strdup(who.c_str());
        this->mapindex = (int32_t)spec["id"].Int(0);
        const JsonValue& p = spec["pos"];
        this->ForcePos(S3DPoint((int32_t)p[0].Int(), (int32_t)p[1].Int(), (int32_t)p[2].Int()));
        this->SetRotateZ((int32_t)spec["facing"].Int());
        this->SetMoveAngle((int32_t)spec["moveangle"].Int(spec["facing"].Int()));
        this->state = (uint16_t)spec["state"].Int(0);
        this->charflags = (uint32_t)spec["charflags"].Int();
        // The object flags combat code tests (immobile, paralysed, iced), as the case gives them.
        constexpr uint32_t caseflags = OF_IMMOBILE | OF_PARALIZE | OF_ICED;
        this->flags = (this->flags & ~caseflags) | ((uint32_t)spec["objflags"].Int(0) & caseflags);
        this->target_out_of_sight = spec["out_of_sight"].Bool();
        this->monsterkind = (int32_t)spec["monsterkind"].Int(0);
        for (const auto& [k, v] : spec["stats"].Members())
            stats[k] = (int32_t)v.Int();
        for (const auto& [k, v] : spec["classstats"].Members())
            classstats[k] = (int32_t)v.Int();
        cd = std::make_unique<SCharData>();
        const JsonValue& c = spec["chardata"];
        cd->combatrangemax = (int32_t)c["combatrangemax"].Int();
        cd->armorvalue = (int32_t)c["armor"].Int();
        for (size_t i = 0; i < c["damagemods"].Items().size() && i < NUMDAMAGETYPES; ++i)
            cd->damagemods[i] = (int32_t)c["damagemods"][i].Int();
        cd->walkspeed = (int32_t)c["walkspeed"].Int(-1);
        cd->runspeed = (int32_t)c["runspeed"].Int(-1);
        cd->sneakspeed = (int32_t)c["sneakspeed"].Int(-1);
        cd->combatwalkspeed = (int32_t)c["combatwalkspeed"].Int(-1);
        ReadCharData(c, *cd);
        this->chardata = cd.get();

        // Motion (kata M7): object flags, inventory slot, vel and accum,
        // the animation's move, a MoveTo, the shove side, the sight fields.
        this->flags |= (uint32_t)spec["objflags"].Int();
        this->SetInventNum((short)spec["inventnum"].Int(-1));
        this->vel = Point(spec["vel"]);
        this->accum = Point(spec["accum"]);
        this->SetMoveDist((int32_t)spec["movedist"].Int());
        this->SetMoveVert((int32_t)spec["movevert"].Int());
        if (spec.Has("moveto"))
        {
            this->movepos = Point(spec["moveto"]);
            this->movetopos = true;
        }
        this->forcenomove = spec["forcenomove"].Bool();
        this->shovedir = (int32_t)spec["shovedir"].Int(-1);
        this->target_out_of_sight_prev = spec["out_of_sight_prev"].Bool();
        this->sight_lost_ticks = (int32_t)spec["sight_lost_ticks"].Int();
        if constexpr (std::is_same_v<Base, TPlayer>)
            this->TPlayer::SetPlayerState((int32_t)spec["playerstate"].Int(0));   // as the retail fixture's zeroed +0x36c

        // Action state (kata M1u, M8): commanddone, an animator, the frame
        // and its rate, the previous state, the stealth values.
        this->commanddone = spec["commanddone"].Bool();
        this->FreeAnimator();               // the one the constructor made
        if (spec["animator"].Bool())
            this->AddComponent(std::make_unique<TFixtureAnimator>(this, who));
        this->frame = (short)spec["frame"].Int();
        this->framerate = (short)spec["framerate"].Int(1);
        this->prevstate = (short)spec["prevstate"].Int(spec["state"].Int(0));
        this->glimpse = (int32_t)spec["glimpse"].Int();
        this->noise = (int32_t)spec["noise"].Int();
        ReadAttackState(spec["attackstate"]);
        if constexpr (std::is_base_of_v<TPlayer, Base>)
        {
            const JsonValue& r = spec["resists"];
            for (int32_t i = 0; i < NUMDAMAGETYPES; ++i)
                Base::SetObjStat(CHRRESIST_FIRST + i, (int32_t)r[i].Int(0));
            if (spec.Has("weapon"))
            {
                weapon = true;
                weapontype = (int32_t)spec["weapon"]["type"].Int();
                weapondamage = (int32_t)spec["weapon"]["damage"].Int();
            }
            if (spec.Has("maxmana"))
            {
                hasmaxmana = true;
                maxmana = (int32_t)spec["maxmana"].Int();
            }
        }
    }
    TFixtureChar(const TFixtureChar&) = delete;
    TFixtureChar& operator=(const TFixtureChar&) = delete;

    const std::string& Who() const { return who; }

    // The object definition the game builds a character from: its class
    // (player or character) and its class.def type -- the case's "type",
    // else its name -- so the class record and stat layout are the real ones.
    static std::unique_ptr<SObjectDef> Def(const JsonValue& spec)
    {
        auto def = std::make_unique<SObjectDef>();
        memset(def.get(), 0, sizeof(SObjectDef));
        def->objclass = (short)spec["class"].Int(OBJCLASS_CHARACTER);
        const std::string type = spec.Has("type") ? spec["type"].Str() : spec["name"].Str();
        TObjectClass* cl = TObjectClass::GetClass(def->objclass);
        if (!cl)
            throw std::runtime_error("no class " + std::to_string(def->objclass) + " loaded");
        def->objtype = (short)cl->FindObjType(type.c_str());
        if (def->objtype < 0)
            throw std::runtime_error("class.def has no type '" + type + "'");
        return def;
    }

    // Root, doing and desired from the case (ClearChar's blocks dropped).
    void SetBlocks(TActionBlock* r, TActionBlock* d, TActionBlock* w) override
    {
        this->root = r;
        this->doing = d;
        this->desired = w;
    }
    TActionBlock* Root() override { return this->root; }
    TActionBlock* Doing() override { return this->doing; }
    TActionBlock* Desired() override { return this->desired; }
    int32_t ResolveCombat(int32_t bits) override { return Base::ResolveCombat(this->doing, bits); }
    int32_t ResolveCombatMove(int32_t bits) override { return Base::ResolveCombatMove(this->doing, bits); }
    void RunUpdateAction(int32_t bits) override { this->UpdateAction(bits); }
    void RunComplexPulse() override { this->TComplexObject::Pulse(); }

    void WriteMotion(JsonOut& j) override
    {
        j.Key("motion").Begin('{');
        j.Key("vel").Begin('[').Value(this->vel.x).Value(this->vel.y).Value(this->vel.z).End(']');
        j.Key("accum").Begin('[').Value(this->accum.x).Value(this->accum.y).Value(this->accum.z).End(']');
        j.Field("movetopos", this->movetopos ? 1 : 0).Field("forcenomove", this->forcenomove ? 1 : 0);
        j.Field("shovedir", this->shovedir);
        j.Field("out_of_sight", this->target_out_of_sight ? 1 : 0);
        j.Field("out_of_sight_prev", this->target_out_of_sight_prev ? 1 : 0);
        j.Field("sight_lost_ticks", this->sight_lost_ticks);
        j.Field("movedist", this->GetMoveDist()).Field("commanddone", this->commanddone ? 1 : 0);
        j.Field("glimpse", this->glimpse).Field("noise", this->noise);
        j.Field("framerate", (int32_t)this->framerate).Field("prevstate", (int32_t)this->prevstate);
        j.Field("prevframe", (int32_t)this->prevframe);
        j.Field("animate", (this->flags & OF_ANIMATE) ? 1 : 0).Field("animator", this->HasAnimator() ? 1 : 0);
        j.End('}');
    }

    // Retail's SetPos (slot 8) moves sectors, walkmaps and the redraw; here
    // it is recorded and the position stored.
    int32_t SetPos(const S3DPoint& p, int32_t newlevel = -1, bool override = false) override
    {
        JsonOut j;
        j.Begin('{').FieldString("seam", "SetPos").FieldString("who", who);
        j.Key("pos").Begin('[').Value(p.x).Value(p.y).Value(p.z).End(']').End('}');
        Seam(j.str());
        this->ForcePos(p);
        return 1;
    }

    int32_t FindState(const char* n, int32_t pcnt = -1) const override
    {
        int32_t index = -1;
        for (size_t i = 0; i < states.size(); ++i)
            if (!strcasecmp(states[i].name.c_str(), n))
            {
                index = (int32_t)i;
                break;
            }
        JsonOut j;
        j.Begin('{').FieldString("seam", "FindState").FieldString("who", who).FieldString("name", n);
        j.Field("result", index).End('}');
        Seam(j.str());
        return index;
    }

    int32_t FindTransitionState(const char* from, const char* to, int32_t pcnt = -1) const override
    {
        const std::string key = std::string(from) + " to " + to;
        int32_t index = -1;
        for (size_t i = 0; i < states.size(); ++i)
            if (!strcasecmp(states[i].name.c_str(), key.c_str()))
            {
                index = (int32_t)i;
                break;
            }
        JsonOut j;
        j.Begin('{').FieldString("seam", "FindTransitionState").FieldString("who", who);
        j.FieldString("frm", from).FieldString("to", to).Field("result", index).End('}');
        Seam(j.str());
        return index;
    }

    bool SetState(int32_t index) override
    {
        if (g_realAnimation)
            return Base::SetState(index);
        JsonOut j;
        j.Begin('{').FieldString("seam", "SetState").FieldString("who", who).Field("state", index);
        j.Key("name");
        if (index >= 0 && (size_t)index < states.size())
            j.String(states[index].name);
        else
            j.Null();
        j.End('}');
        Seam(j.str());
        this->state = (uint16_t)index;
        return true;
    }

    int32_t Sleeping() override { return ObjStat("sleeping"); }
    void SetSleeping(int32_t v) override
    {
        stats["sleeping"] = v;
        JsonOut j;
        j.Begin('{').FieldString("seam", "SetObjStat").FieldString("who", who).FieldString("stat", "sleeping");
        j.Field("value", v).End('}');
        Seam(j.str());
    }

    int32_t Health() override { return ObjStat("health"); }
    int32_t Fatigue() override { return ObjStat("fatigue"); }
    int32_t Mana() override { return ObjStat("mana"); }
    int32_t Radius() override { return ClassStat("radius"); }

    // Any other object stat the case names (retail's GetObjStat seam, slot
    // 0xdc); the rest as the port has them, unrecorded.
    int32_t GetObjStat(int32_t statid) const override
    {
        const char* stat = ObjStatName(statid);
        if (!stat || !stats.count(stat))
            return Base::GetObjStat(statid);
        return StatSeam("GetObjStat", stats, stat);
    }

    // Setting one (retail's SetObjStat seam, slot 0xe8): recorded, and the
    // case's value follows (a stat the case didn't give is added).
    void SetObjStat(int32_t statid, int32_t value) override
    {
        const char* stat = ObjStatName(statid);
        if (!stat)
        {
            Base::SetObjStat(statid, value);
            return;
        }
        JsonOut j;
        j.Begin('{').FieldString("seam", "SetObjStat").FieldString("who", who).FieldString("stat", stat);
        j.Field("value", value).End('}');
        Seam(j.str());
        stats[stat] = value;
    }

    // A type stat by name (retail's GetStat seam at slot 0xd4), when the
    // case gives it.
    using Base::GetStat;
    int32_t GetStat(const char* statname) const override
    {
        std::string stat = statname;
        for (char& ch : stat)
            ch = (char)tolower((unsigned char)ch);
        if (!classstats.count(stat))
            return Base::GetStat(statname);
        return StatSeam("GetStat", classstats, stat.c_str());
    }

    // The player's weapon, when the case gives one (retail's seams at
    // TPlayer::WeaponType 0x00520810 / WeaponDamage 0x00520830).
    int32_t WeaponType() override
    {
        if (!weapon)
            return Base::WeaponType();
        return ValueSeam("WeaponType", weapontype);
    }
    int32_t WeaponDamage() override
    {
        if (!weapon)
            return Base::WeaponDamage();
        return ValueSeam("WeaponDamage", weapondamage);
    }

    // A player's MaxMana, when the case gives `maxmana` (retail's seam at
    // TPlayer::MaxMana 0x00520770); a character's is its chardata's.
    int32_t MaxMana() override
    {
        if (!hasmaxmana)
            return Base::MaxMana();
        return ValueSeam("MaxMana", maxmana);
    }

    void WriteAttackState(JsonOut& j) override
    {
        j.Key("attackstate").Begin('{');
        j.Field("nextattack", this->nextattack).Field("magictimer", this->magictimer);
        j.Field("requestbits", this->requestbits).Field("attackcount", this->attackcount);
        j.Field("lastbutton", this->lastbutton).Field("buttonrepeat", this->buttonrepeat);
        j.Field("chainhits", this->chainhits).Field("lastattackticks", this->lastattackticks);
        j.Field("lasthit", this->lasthit);
        j.Key("lastattack");
        const int32_t last = AttackIndex(this->lastattack);
        if (last >= 0)
            j.Value(last);
        else
            j.Null();
        j.End('}');
    }

    // The index of an attack record in this character's table, -1 if none.
    int32_t AttackIndex(const SCharAttackData* ad) const
    {
        for (int32_t i = 0; ad && i < cd->attacks.NumItems(); ++i)
            if (&cd->attacks[i] == ad)
                return i;
        return -1;
    }

  private:
    static S3DPoint Point(const JsonValue& v)
    {
        return S3DPoint((int32_t)v[0].Int(), (int32_t)v[1].Int(), (int32_t)v[2].Int());
    }

    int32_t ObjStat(const char* stat) { return StatSeam("GetObjStat", stats, stat); }
    int32_t ClassStat(const char* stat) { return StatSeam("GetStat", classstats, stat); }

    int32_t StatSeam(const char* seam, const std::map<std::string, int32_t>& from, const char* stat) const
    {
        auto it = from.find(stat);
        if (it == from.end())
            throw std::runtime_error(who + " has no " + stat + " in the case");
        JsonOut j;
        j.Begin('{').FieldString("seam", seam).FieldString("who", who).FieldString("stat", stat);
        j.Field("result", it->second).End('}');
        Seam(j.str());
        return it->second;
    }

    int32_t ValueSeam(const char* seam, int32_t value) const
    {
        JsonOut j;
        j.Begin('{').FieldString("seam", seam).FieldString("who", who).Field("result", value).End('}');
        Seam(j.str());
        return value;
    }

    // The case's attack bookkeeping, where given (retail +0x120 ... +0x290).
    void ReadAttackState(const JsonValue& a)
    {
        this->nextattack = (int32_t)a["nextattack"].Int(this->nextattack);
        this->magictimer = (int32_t)a["magictimer"].Int(this->magictimer);
        this->requestbits = (uint32_t)a["requestbits"].Int(this->requestbits);
        this->attackcount = (int32_t)a["attackcount"].Int(this->attackcount);
        this->lastbutton = (int32_t)a["lastbutton"].Int(this->lastbutton);
        this->buttonrepeat = (int32_t)a["buttonrepeat"].Int(this->buttonrepeat);
        this->chainhits = (int32_t)a["chainhits"].Int(this->chainhits);
        this->lastattackticks = (int32_t)a["lastattackticks"].Int(this->lastattackticks);
        this->lasthit = (int32_t)a["lasthit"].Int(this->lasthit);
        if (a.Has("lastattack") && !a["lastattack"].IsNull())
            this->lastattack = &cd->attacks[(int32_t)a["lastattack"].Int()];
    }

    bool weapon = false;
    int32_t weapontype = 0, weapondamage = 0;
    bool hasmaxmana = false;
    int32_t maxmana = 0;

    std::string who;
    std::vector<SFixtureState> states;
    mutable std::map<std::string, int32_t> stats;
    std::map<std::string, int32_t> classstats;
    std::unique_ptr<SCharData> cd;
};

// ---- The fixture world ------------------------------------------------------

// The case's characters, their blocks, and how to name a block in a dump.
class TFixtureWorld
{
  public:
    explicit TFixtureWorld(const JsonValue& cs)
    {
        for (const JsonValue& spec : cs["chars"].Items())
        {
            std::vector<SFixtureState> table = ReadStates(spec["states"]);
            const std::string name = spec["name"].Str();
            auto* imagery = new TFixtureImagery(TFixtureImagery::Register(table), name, table,
                                                spec["needsanimator"].Bool(true));
            TCharacter* chr;
            IFixtureChar* fx;
            if (spec["class"].Int(OBJCLASS_CHARACTER) == OBJCLASS_PLAYER)
            {
                auto* p = new TFixtureChar<TPlayer>(imagery, spec, std::move(table));
                chr = p;
                fx = p;
            }
            else
            {
                auto* m = new TFixtureChar<TCharacter>(imagery, spec, std::move(table));
                chr = m;
                fx = m;
            }
            fixtures[chr] = fx;
            byname[name] = chr;
            names[chr] = name;
            order.push_back(chr);
        }
        // Blocks after every character exists: a block's obj names one.
        const auto& specs = cs["chars"].Items();
        for (size_t i = 0; i < specs.size(); ++i)
            SetBlocks(order[i], specs[i]);
    }

    IFixtureChar* Fixture(const TCharacter* c) const { return fixtures.at(c); }
    const std::vector<TCharacter*>& Order() const { return order; }

    TCharacter* Get(const std::string& name) const
    {
        auto it = byname.find(name);
        if (it == byname.end())
            throw std::runtime_error("no character '" + name + "' in the case");
        return it->second;
    }

    std::string NameOf(const TObjectInstance* o) const
    {
        auto it = names.find(o);
        if (it != names.end())
            return it->second;
        char buf[32];
        snprintf(buf, sizeof(buf), "%p", (const void*)o);
        return buf;
    }

    void WriteBlock(JsonOut& j, const char* key, TActionBlock* ab, std::vector<TActionBlock*>& made) const
    {
        j.Key(key);
        if (!ab)
        {
            j.Null();
            return;
        }
        std::string id;
        auto it = roles.find(ab);
        if (it != roles.end())
            id = it->second;
        else
        {
            auto at = std::find(made.begin(), made.end(), ab);
            if (at == made.end())
            {
                made.push_back(ab);
                at = made.end() - 1;
            }
            id = "new " + std::to_string(at - made.begin() + 1);
        }
        j.Begin('{');
        j.FieldString("id", id).Field("action", (int32_t)ab->action).FieldString("name", ab->name);
        j.Field("frame", ab->frame).Field("wait", ab->wait).Field("angle", ab->angle);
        j.Field("moveangle", ab->moveangle).Field("turnrate", ab->turnrate);
        j.Key("target").Begin('[').Value(ab->target.x).Value(ab->target.y).Value(ab->target.z).End(']');
        j.Key("obj");
        if (ab->obj)
            j.String(NameOf(ab->obj));
        else
            j.Null();
        WriteFlags(j, ab->flags);
        if (blockExtra)
            blockExtra(j, *ab);
        j.End('}');
    }

    // Optional extras a target adds to its dumps: fields of each block, and
    // each character's attack bookkeeping (IFixtureChar::WriteAttackState).
    std::function<void(JsonOut&, const TActionBlock&)> blockExtra;
    bool writeAttackState = false;

    void WriteCharacter(JsonOut& j, const char* key, TCharacter* c) const
    {
        std::vector<TActionBlock*> made;
        j.Key(key).Begin('{');
        j.Field("state", (int16_t)c->TObjectInstance::GetState()).Field("frame", (int32_t)c->GetFrame());
        j.Field("facing", c->GetFace()).Field("moveangle", c->GetMoveAngle());
        const S3DPoint p = c->Pos();
        j.Key("pos").Begin('[').Value(p.x).Value(p.y).Value(p.z).End(']');
        IFixtureChar* fx = fixtures.at(c);
        WriteBlock(j, "root", fx->Root(), made);
        WriteBlock(j, "doing", fx->Doing(), made);
        WriteBlock(j, "desired", fx->Desired(), made);
        if (writeAttackState)
            fx->WriteAttackState(j);
        j.End('}');
    }

  private:
    TActionBlock* NewBlock(const JsonValue& b)
    {
        auto* ab = new TActionBlock(b["name"].Str().c_str(), (ACTION)b["action"].Int());
        if (b.Has("angle"))
            ab->angle = (int32_t)b["angle"].Int();
        if (b.Has("moveangle"))
            ab->moveangle = (int32_t)b["moveangle"].Int();
        if (b.Has("turnrate"))
            ab->turnrate = (int32_t)b["turnrate"].Int();
        if (b.Has("frame"))
            ab->frame = (int32_t)b["frame"].Int();
        if (b.Has("wait"))
            ab->wait = (int32_t)b["wait"].Int();
        if (b.Has("obj") && !b["obj"].IsNull())
            ab->obj = Get(b["obj"].Str());
        if (b.Has("flags"))
            ab->flags = FlagBits(b["flags"]);
        return ab;
    }

    void SetBlocks(TCharacter* chr, const JsonValue& spec)
    {
        std::map<std::string, TActionBlock*> made;
        for (const char* role : {"root", "doing", "desired"})
        {
            const JsonValue& b = spec[role];
            TActionBlock* ab;
            if (b.IsNull() && strcmp(role, "root") != 0)
                ab = made.at("root");
            else if (b.GetKind() == JsonValue::Kind::String)
                ab = made.at(b.Str());
            else if (b.IsNull())
                throw std::runtime_error(spec["name"].Str() + ": a root block is required");
            else
                ab = NewBlock(b);
            made[role] = ab;
            roles.emplace(ab, role);           // a shared block keeps its first role
        }
        fixtures.at(chr)->SetBlocks(made["root"], made["doing"], made["desired"]);
    }

    std::map<const TCharacter*, IFixtureChar*> fixtures;
    std::map<std::string, TCharacter*> byname;
    std::map<const TObjectInstance*, std::string> names;
    std::map<const TActionBlock*, std::string> roles;
    std::vector<TCharacter*> order;
};

// The case's globals and seams for one call: CombatFace, the PlayScreen frame
// and control flags, an empty FindCharacters world, the case's `blocked`
// (FindClearPath / Blocked) and `sees` (CanSeeCharacter), the RNG tape. A
// case with a `ground` runs Blocked itself over that walkmap (walk cells:
// `z`, or the last of its `cells` boxes [gx0, gy0, gx1, gy1, height]; no
// sector in `nosector`) with CharBlocking walking the case's `nearby`
// characters (else all of them, in case order). Removed when the scope ends.
class SCaseScope
{
  public:
    SCaseScope(const JsonValue& cs, const TFixtureWorld& world);
    ~SCaseScope();
    SCaseScope(const SCaseScope&) = delete;
    SCaseScope& operator=(const SCaseScope&) = delete;

  private:
    int32_t savedAmbient = 0;
};

}  // namespace RetailAB::Fixture
