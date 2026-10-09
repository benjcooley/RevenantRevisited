// *************************************************************************
// *                      Revenant Revisited 2026                          *
// * retailab_fixture.cpp - fixture worlds for the combat A/B targets      *
// *************************************************************************
//
// See retailab_fixture.h.

#include "retailab_fixture.h"

#include "dls.h"                  // MakeColorTables (the trig tables too)
#include "gameoptions.h"
#include "playscreen.h"
#include "revenant.h"
#include "revutils.h"

void GetINISettings();            // revmain.cpp: the [Paths] section

namespace RetailAB::Fixture
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
namespace
{
std::vector<std::string> g_seams;
}

void Seam(const std::string& record) { g_seams.push_back(record); }

void ClearSeams() { g_seams.clear(); }

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

namespace
{

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

}  // namespace

void WriteSeams(JsonOut& j)
{
    j.Key("seams").Begin('[');
    for (const std::string& s : g_seams)
        j.Raw(s);
    j.End(']');
}

namespace
{
// The RNG tape (retail fixture: guest.py set_rng / _random / _rand).
std::vector<int32_t> g_tape;
size_t g_tapeAt = 0;
uint32_t g_tapeSeed = 1;
std::vector<std::string> g_draws;

int32_t TapeSource()
{
    int32_t value;
    if (g_tapeAt < g_tape.size())
        value = g_tape[g_tapeAt++] & 0x7fff;
    else
    {
        g_tapeSeed = g_tapeSeed * 214013u + 2531011u;   // retail's generator
        value = (int32_t)((g_tapeSeed >> 16) & 0x7fff);
    }
    JsonOut j;
    j.Begin('{').Field("rand", value).End('}');
    g_draws.push_back(j.str());
    return value;
}

// random(lo, hi) draws through TapeSource and then reports here: its raw
// record becomes the range record.
void TapeRange(int32_t lo, int32_t hi, int32_t result)
{
    JsonOut j;
    j.Begin('{').Field("lo", lo).Field("hi", hi).Field("result", result).End('}');
    if (!g_draws.empty())
        g_draws.back() = j.str();
}
}  // namespace

void WriteDraws(JsonOut& j)
{
    j.Key("draws").Begin('[');
    for (const std::string& d : g_draws)
        j.Raw(d);
    j.End(']');
}

SCaseScope::SCaseScope(const JsonValue& cs, const TFixtureWorld& world)
{
    g_tape.clear();
    for (const JsonValue& v : cs["tape"].Items())
        g_tape.push_back((int32_t)v.Int());
    g_tapeAt = 0;
    g_tapeSeed = (uint32_t)cs["seed"].Int(1);
    g_draws.clear();
    SetRandomSource(TapeSource);
    SetRandomRangeObserver(TapeRange);

    const JsonValue& g = cs["globals"];
    CombatFace = g["combatface"].Bool(true);
    PlayScreen.SetFixtureState((int32_t)g["frame"].Int(0), g["control"].Bool(true), g["ps_5d8"].Bool(false));
    g_world = &world;
    g_blocked = cs["blocked"].Bool(false);
    g_sees = cs["sees"].Bool(true);
    TCharacter::findCharactersSeam = EmptyWorld;
    TCharacter::blockedSeam = CaseBlocked;
    TCharacter::canSeeSeam = CaseSees;
    ClearSeams();
}

SCaseScope::~SCaseScope()
{
    SetRandomSource(nullptr);
    SetRandomRangeObserver(nullptr);
    TCharacter::findCharactersSeam = nullptr;
    TCharacter::blockedSeam = nullptr;
    TCharacter::canSeeSeam = nullptr;
    g_world = nullptr;
}

}  // namespace RetailAB::Fixture
