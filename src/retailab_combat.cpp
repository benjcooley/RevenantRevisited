// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *   retailab_combat.cpp - the port's side of the combat A/B dumps       *
// *************************************************************************
//
// The combat dojo's targets (docs/gameplay/COMBAT_DOJO.md). Each mirrors a
// retail fixture in the emulator's combat slot
// (tools/retail_runtime/slots/combat/ in the main checkout): the same case
// (one JSON object, field 0 of the case line), the same seams answered from
// it, the same JSON out, so the compare is a plain diff of the two dumps.
//
// Seams (COMBAT_DOJO.md §6.3): a fixture character answers FindState /
// FindTransitionState from the case's state table, its stats from the
// case, and SetState by recording it; its imagery is a header built from
// the same table (frame counts, animation flags) whose NumStates and
// GetAniFlags are recorded; FindCharacters goes to the case's world through
// TCharacter::findCharactersSeam. Every seam call is part of the result.

#include "retailab.h"
#include "retailab_json.h"

#include "character.h"
#include "dls.h"                  // MakeColorTables (the trig tables too)
#include "imagery.h"
#include "imageres.h"
#include "gameoptions.h"
#include "player.h"
#include "playscreen.h"
#include "revenant.h"
#include "revutils.h"
#include "rules.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <map>
#include <memory>
#include <string>
#include <strings.h>
#include <vector>

void GetINISettings();                 // revmain.cpp: the [Paths] section

namespace RetailAB
{
namespace
{

// ---- Game data ----------------------------------------------------------------

// What the combat fixtures build on: class.def (object types and their
// stat definitions; a TPlayer sizes its stats from its class). The data
// steps of InitGlobals (revmain.cpp) in its order -- program paths, the
// [Paths] INI section, the two base archives, the imagery path (step 8),
// the trig tables (step 12, MakeColorTables: every angle and distance reads
// them), the classes (step 16) -- without the display, sound or map. Once
// per process.
bool LoadGameData(std::string& error)
{
    static bool loaded = false;
    if (loaded)
        return true;
    rev_resolve_program_paths(RunPath, SavePath, MAXPATHLEN);
    INISetPath(RunPath);
    GetINISettings();
    if (!MountArchive("resources.rvr") || !MountArchive("imagery.rvi"))
    {
        error = "can't mount resources.rvr / imagery.rvi";
        return false;
    }
    TObjectImagery::SetImageryPath(NORMALPATH);
    MakeColorTables();
    if (!TObjectClass::LoadClasses())
    {
        error = "can't load class.def";
        return false;
    }
    loaded = true;
    return true;
}

// ---- The seam log ---------------------------------------------------------

// The seam calls of the case being run, in order, each one JSON object (as
// the retail fixture records them).
std::vector<std::string> g_seams;

void Seam(const std::string& record) { g_seams.push_back(record); }

// ---- Action block flags by meaning -----------------------------------------

// The port's TActionBlock bits under the names the retail fixture gives its
// own (guest.py AB_FLAGS). Retail inserted a bit below `priority`, so the
// numbers differ; the names are what's compared. A `?` marks a name not yet
// confirmed on the retail side.
const std::pair<uint32_t, const char*> kPortFlags[] = {
    {0x001, "firsttime"}, {0x002, "transition"}, {0x004, "forced"}, {0x008, "priority"},
    {0x010, "interrupt"}, {0x020, "nowaitdone"}, {0x040, "dontforce"},   {0x080, "stop"},
    {0x100, "waitpivot"}, {0x200, "noroot"},     {0x400, "loop?"}, {0x800, "pickup"},
};

uint32_t FlagBits(const JsonValue& names)
{
    uint32_t bits = 0;
    for (const JsonValue& n : names.Items())
    {
        bool found = false;
        for (const auto& [bit, name] : kPortFlags)
            if (n.Str() == name)
            {
                bits |= bit;
                found = true;
            }
        if (!found)
            throw std::runtime_error("the port has no action block flag '" + n.Str() + "'");
    }
    return bits;
}

void WriteFlags(JsonOut& j, uint32_t bits)
{
    std::vector<std::string> names;
    for (const auto& [bit, name] : kPortFlags)
        if (bits & bit)
            names.emplace_back(name);
    std::sort(names.begin(), names.end());
    j.Key("flags").Begin('[');
    for (const std::string& n : names)
        j.String(n);
    j.End(']');
}

// ---- Fixture imagery ------------------------------------------------------

// A state table entry as the case gives it: a name, or
// {"name", "frames" (10), "aniflags" (0)}.
struct SFixtureState
{
    std::string name;
    int32_t frames = 10;
    int32_t aniflags = 0;
};

std::vector<SFixtureState> ReadStates(const JsonValue& list)
{
    std::vector<SFixtureState> states;
    for (const JsonValue& s : list.Items())
    {
        SFixtureState st;
        if (s.GetKind() == JsonValue::Kind::String)
            st.name = s.Str();
        else
        {
            st.name = s["name"].Str();
            st.frames = (int32_t)s["frames"].Int(10);
            st.aniflags = (int32_t)s["aniflags"].Int(0);
        }
        states.push_back(st);
    }
    return states;
}

// Imagery whose header is the case's state table, registered under a name
// of its own (the registry keeps every entry, so each one is new).
class TFixtureImagery : public TObjectImagery
{
  public:
    TFixtureImagery(int32_t id, std::string owner) : TObjectImagery(id), who(std::move(owner)) {}

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
        const int32_t flags = TObjectImagery::GetAniFlags(state);
        JsonOut j;
        j.Begin('{').FieldString("seam", "imagery.GetAniFlags").FieldString("who", who);
        j.Key("args").Begin('[').Value(state).End(']').Field("result", flags).End('}');
        Seam(j.str());
        return flags;
    }

  private:
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
        this->chardata = cd.get();
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

    int32_t Health() override { return ObjStat("health"); }
    int32_t Fatigue() override { return ObjStat("fatigue"); }
    int32_t Mana() override { return ObjStat("mana"); }
    int32_t Radius() override { return ClassStat("radius"); }

  private:
    int32_t ObjStat(const char* stat) { return StatSeam("GetObjStat", stats, stat); }
    int32_t ClassStat(const char* stat) { return StatSeam("GetStat", classstats, stat); }

    int32_t StatSeam(const char* seam, const std::map<std::string, int32_t>& from, const char* stat)
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

    std::string who;
    std::vector<SFixtureState> states;
    std::map<std::string, int32_t> stats, classstats;
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
            auto* imagery = new TFixtureImagery(TFixtureImagery::Register(table), name);
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
        j.End('}');
    }

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

// The world FindCharacters sees: M3's is empty. The query is recorded as
// the retail fixture records it.
const TFixtureWorld* g_world = nullptr;

int32_t EmptyWorld(TCharacter* self, TCharacter* chars[], int32_t maxchars, int32_t range, int32_t angle,
                   int32_t anglerange, int32_t flags)
{
    JsonOut j;
    j.Begin('{').FieldString("seam", "FindCharacters").FieldString("who", g_world->NameOf(self));
    j.Field("max", maxchars).Field("range", range).Field("angle", angle).Field("anglerange", anglerange);
    j.Field("flags", flags).Field("result", 0).End('}');
    Seam(j.str());
    if (maxchars > 0)
        chars[0] = nullptr;
    return 0;
}

// The case's `blocked` answers FindClearPath (retail) / Blocked (port).
bool g_blocked = false;

// The case's `sees` answers CanSeeCharacter (both sides).
bool g_sees = true;

bool CaseSees(TCharacter* self, TCharacter* chr, int32_t angle)
{
    JsonOut j;
    j.Begin('{').FieldString("seam", "CanSeeCharacter").FieldString("who", g_world->NameOf(self));
    j.FieldString("target", g_world->NameOf(chr)).Field("result", g_sees ? 1 : 0).End('}');
    Seam(j.str());
    return g_sees;
}

bool CaseBlocked(TCharacter* self, const S3DPoint& pos, const S3DPoint& newpos, uint32_t bits)
{
    JsonOut j;
    j.Begin('{').FieldString("seam", "FindClearPath").FieldString("who", g_world->NameOf(self));
    j.Key("to").Begin('[').Value(newpos.x).Value(newpos.y).Value(newpos.z).End(']');
    j.Field("result", g_blocked ? 1 : 0).End('}');
    Seam(j.str());
    return g_blocked;
}

void WriteSeams(JsonOut& j)
{
    j.Key("seams").Begin('[');
    for (const std::string& s : g_seams)
        j.Raw(s);
    j.End(']');
}

// ---- M3 / M5: one character method per case --------------------------------

// Case (field 0, JSON): {"globals", "chars": [...], "self", "call", ...};
// see slots/combat/combat_call.py. `call`: "go" (Go(angle), kata M3),
// "resolve-combat" / "resolve-combat-move" (the resolvers on the doing
// block with `bits`, kata M5). Not modelled on the port side, because the
// port has no such thing yet: CombatFace (the case's value is ignored),
// FindClearPath (the port's Go doesn't probe ahead), CanSeeCharacter in the
// resolvers, the frame cadence of retargeting.
std::string CombatCall(const Case& c, std::string& error)
{
    try
    {
        if (!LoadGameData(error))
            return {};
        const JsonValue cs = JsonValue::Parse(c.Field(0));
        const JsonValue& g = cs["globals"];
        CombatFace = g["combatface"].Bool(true);
        PlayScreen.SetFixtureState((int32_t)g["frame"].Int(0), g["control"].Bool(true), g["ps_5d8"].Bool(false));
        g_seams.clear();
        TFixtureWorld world(cs);
        g_world = &world;
        TCharacter::findCharactersSeam = EmptyWorld;
        TCharacter::blockedSeam = CaseBlocked;
        TCharacter::canSeeSeam = CaseSees;
        g_blocked = cs["blocked"].Bool(false);
        g_sees = cs["sees"].Bool(true);
        TCharacter* me = world.Get(cs["self"].Str());
        IFixtureChar* fx = world.Fixture(me);
        g_seams.clear();
        const std::string call = cs.Has("call") ? cs["call"].Str() : "go";
        int32_t returned = 0;
        std::vector<int32_t> outputs;
        if (call == "calculate-damage")
            for (const JsonValue& in : cs["inputs"].Items())
                outputs.push_back(me->CalculateDamage((int32_t)in[0].Int(), (int32_t)in[1].Int(), (int32_t)in[2].Int()));
        else if (call == "go")
            returned = me->Go((int32_t)cs["angle"].Int()) ? 1 : 0;
        else if (call == "resolve-combat")
            returned = fx->ResolveCombat((int32_t)cs["bits"].Int());
        else if (call == "resolve-combat-move")
            returned = fx->ResolveCombatMove((int32_t)cs["bits"].Int());
        else
            throw std::runtime_error("unknown call '" + call + "'");
        TCharacter::findCharactersSeam = nullptr;
        TCharacter::blockedSeam = nullptr;
        TCharacter::canSeeSeam = nullptr;

        JsonOut j;
        j.Begin('{').FieldString("schema", "combat.call.v1").FieldString("side", "port");
        if (call == "calculate-damage")
        {
            j.Key("returned").Begin('[');
            for (int32_t v : outputs)
                j.Value(v);
            j.End(']');
        }
        else
            j.Field("returned", returned);
        world.WriteCharacter(j, "self", me);
        WriteSeams(j);
        j.End('}');
        g_world = nullptr;
        return j.str();
    }
    catch (const std::exception& e)
    {
        TCharacter::findCharactersSeam = nullptr;
        TCharacter::blockedSeam = nullptr;
        TCharacter::canSeeSeam = nullptr;
        g_world = nullptr;
        error = e.what();
        return {};
    }
}

// ---- M2: the angle and distance kernels ----------------------------------

// Case (field 0, JSON): {"kernel", "inputs": [[...], ...]}; see
// slots/combat/kernels.py. The tables these read are built by
// MakeColorTables (InitGlobals step 12), once.
std::string CombatKernels(const Case& c, std::string& error)
{
    static bool tables = false;
    if (!tables)
    {
        MakeColorTables();
        tables = true;
    }
    try
    {
        const JsonValue cs = JsonValue::Parse(c.Field(0));
        const std::string kernel = cs["kernel"].Str();
        auto point = [](const JsonValue& v) {
            return S3DPoint((int32_t)v[0].Int(), (int32_t)v[1].Int(), (int32_t)v[2].Int(0));
        };
        TObjectInstance a(nullptr), b(nullptr);
        JsonOut j;
        j.Begin('{').FieldString("schema", "combat.kernels.v1").FieldString("side", "port");
        j.FieldString("kernel", kernel).Key("outputs").Begin('[');
        for (const JsonValue& in : cs["inputs"].Items())
        {
            if (kernel == "angle-diff")
                j.Value(AngleDiff((int32_t)in[0].Int(), (int32_t)in[1].Int()));
            else if (kernel == "facing")
                j.Value(ConvertToFacing(point(in[0]), point(in[1])));
            else if (kernel == "distance")
                j.Value(Distance(point(in[0]), point(in[1])));
            else if (kernel == "vector")
            {
                S3DPoint v{0, 0, 0};
                ConvertToVector((int32_t)in[0].Int(), (int32_t)in[1].Int(), v, (int32_t)in[2].Int());
                j.Begin('[').Value(v.x).Value(v.y).Value(v.z).End(']');
            }
            else if (kernel == "obj-distance" || kernel == "obj-angle")
            {
                a.ForcePos(point(in[0]));
                b.ForcePos(point(in[1]));
                j.Value(kernel == "obj-distance" ? a.Distance(&b) : a.AngleTo(&b));
            }
            else
                throw std::runtime_error("unknown kernel '" + kernel + "'");
        }
        j.End(']').End('}');
        return j.str();
    }
    catch (const std::exception& e)
    {
        error = e.what();
        return {};
    }
}

}  // namespace

Target CombatTarget(const std::string& name)
{
    if (name == "combat-go" || name == "combat-resolve" || name == "combat-damage")
        return CombatCall;
    if (name == "combat-kernels")
        return CombatKernels;
    return nullptr;
}

}  // namespace RetailAB
