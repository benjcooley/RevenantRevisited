// *************************************************************************
// *                      Revenant Revisited 2026                          *
// * retailab_fixture.cpp - fixture worlds for the combat A/B targets      *
// *************************************************************************
//
// See retailab_fixture.h.

#include "retailab_fixture.h"

#include "dls.h"                  // MakeColorTables (the trig tables too)
#include "gameoptions.h"
#include "mappane.h"                // walkGridSeam
#include "player.h"
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
    // As step 8 picks it: the IMAGERY\ folder unless normals are on. With the
    // wrong one, the class.def types whose headers aren't in the quick-load
    // cache (18, the Chaos talisman among them) don't register and vanish.
    if (NoNormals)
        TObjectImagery::SetImageryPath(NONORMALPATH);
    else
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
    {0x100, "waitpivot"}, {0x200, "noroot"},     {0x400, "loop?"}, {0x800, "walkto"},
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

// ---- Character data from the case --------------------------------------------

namespace
{
void ReadText(const JsonValue& v, char* out, size_t size)
{
    if (!v.IsNull())
        strncpyz(out, v.Str().c_str(), (int32_t)size);
}

template <typename T>
void ReadInt(const JsonValue& v, T& out)
{
    if (!v.IsNull())
        out = (T)v.Int();
}

template <typename T>
void ReadInts(const JsonValue& v, T* out, size_t count)
{
    for (size_t i = 0; i < count && i < v.Items().size(); ++i)
        out[i] = (T)v[i].Int();
}

void ReadImpact(const JsonValue& v, SCharAttackImpact& ai)
{
    memset(&ai, 0, sizeof(ai));
    ReadText(v["name"], ai.impactname, sizeof(ai.impactname));
    ReadInt(v["index"], ai.index);
    ReadInt(v["flags"], ai.flags);
    ReadText(v["loopname"], ai.loopname, sizeof(ai.loopname));
    ReadInt(v["looptime"], ai.looptime);
    ReadInt(v["damagemin"], ai.damagemin);
    ReadInt(v["damagemax"], ai.damagemax);
    ReadInt(v["snapdist"], ai.snapdist);
    ReadInt(v["snaptime"], ai.snaptime);
}

void ReadAttack(const JsonValue& v, SCharAttackData& ad)
{
    memset(&ad, 0, sizeof(ad));
    ReadText(v["name"], ad.attackname, sizeof(ad.attackname));
    ReadInt(v["flags"], ad.flags);
    ReadInt(v["button"], ad.button);
    ReadInt(v["attackpcnt"], ad.attackpcnt);
    ReadInt(v["mindist"], ad.mindist);
    ReadInt(v["maxdist"], ad.maxdist);
    if (ad.flags & CA_MAGICATTACK)
    {
        ReadText(v["spellname"], ad.spellname, sizeof(ad.spellname));
        const JsonValue& src = v["spellsource"];
        ad.spellsource = S3DPoint((int32_t)src[0].Int(), (int32_t)src[1].Int(), (int32_t)src[2].Int());
        ReadInt(v["condition"], ad.condition);
        ReadInt(v["conditionvalue"], ad.conditionvalue);
        return;
    }
    ReadText(v["responsename"], ad.responsename, sizeof(ad.responsename));
    ReadText(v["blockname"], ad.blockname, sizeof(ad.blockname));
    ReadText(v["missname"], ad.missname, sizeof(ad.missname));
    ReadText(v["chainname"], ad.chainname, sizeof(ad.chainname));
    ReadInt(v["blocktime"], ad.blocktime);
    ReadInt(v["impacttime"], ad.impacttime);
    ReadInt(v["chainexptime"], ad.chainexptime);
    ReadInt(v["nextwait"], ad.nextwait);
    ReadInt(v["hitminrange"], ad.hitminrange);
    ReadInt(v["hitmaxrange"], ad.hitmaxrange);
    ReadInt(v["hitangle"], ad.hitangle);
    ReadInt(v["damagemod"], ad.damagemod);
    ReadInt(v["fatigue"], ad.fatigue);
    ReadInt(v["attackskill"], ad.attackskill);
    ReadInt(v["weaponmask"], ad.weaponmask);
    ReadInt(v["weaponskill"], ad.weaponskill);
    ReadInt(v["swipeframeon"], ad.swipeframeon);
    ReadInt(v["swipeframeoff"], ad.swipeframeoff);
    ReadInt(v["maxfatigue"], ad.maxfatigue);
    for (const JsonValue& imp : v["impacts"].Items())
        if (ad.numimpacts < MAXATTACKIMPACTS)
            ReadImpact(imp, ad.impacts[ad.numimpacts++]);
}
}  // namespace

void ReadCharData(const JsonValue& c, SCharData& cd)
{
    ReadText(c["groups"], cd.groups, sizeof(cd.groups));
    ReadText(c["enemies"], cd.enemies, sizeof(cd.enemies));
    ReadInt(c["flags"], cd.flags);
    ReadText(c["blocksounds"], cd.blocksounds, sizeof(cd.blocksounds));
    ReadText(c["misssounds"], cd.misssounds, sizeof(cd.misssounds));
    int32_t three[3] = {cd.playerblockmin, cd.playerblockstep, cd.playerblockinc};
    ReadInts(c["playerblock"], three, 3);
    cd.playerblockmin = three[0], cd.playerblockstep = three[1], cd.playerblockinc = three[2];
    int32_t two[2] = {cd.combatrangemin, cd.combatrangemax};
    ReadInts(c["combatrange"], two, 2);
    cd.combatrangemin = two[0], cd.combatrangemax = two[1];
    ReadInt(c["maxattackrange"], cd.maxattackrange);
    ReadInt(c["bleeder"], cd.bleeder);
    ReadInt(c["mana"], cd.mana);                       // a character's MaxMana (retail +0x1e0)
    ReadText(c["bodytype"], cd.bodytype, sizeof(cd.bodytype));
    int32_t block[3] = {cd.blockfreq, cd.blockmin, cd.blockmax};
    ReadInts(c["block"], block, 3);
    cd.blockfreq = block[0], cd.blockmin = block[1], cd.blockmax = block[2];
    int32_t sight[4] = {cd.sightmin, cd.sightmax, cd.sightrange, cd.sightangle};
    ReadInts(c["sight"], sight, 4);
    cd.sightmin = sight[0], cd.sightmax = sight[1], cd.sightrange = sight[2], cd.sightangle = sight[3];
    int32_t hearing[3] = {cd.hearingmin, cd.hearingmax, cd.hearingrange};
    ReadInts(c["hearing"], hearing, 3);
    cd.hearingmin = hearing[0], cd.hearingmax = hearing[1], cd.hearingrange = hearing[2];
    ReadInt(c["weapontype"], cd.weapontype);
    ReadInt(c["weapondamage"], cd.weapondamage);
    ReadInt(c["defensemod"], cd.defensemod);
    ReadInt(c["attackmod"], cd.attackmod);
    int32_t freq[2] = {cd.minattackfreq, cd.maxattackfreq};
    ReadInts(c["attackfreq"], freq, 2);
    cd.minattackfreq = freq[0], cd.maxattackfreq = freq[1];
    int32_t magic[2] = {cd.minmagicfreq, cd.maxmagicfreq};
    ReadInts(c["magicfreq"], magic, 2);
    cd.minmagicfreq = magic[0], cd.maxmagicfreq = magic[1];
    ReadInt(c["mana"], cd.mana);
    ReadInt(c["fatigue"], cd.fatigue);
    ReadInt(c["health"], cd.health);
    ReadInt(c["retreatat"], cd.retreatat);
    ReadInt(c["retreatatmana"], cd.retreatatmana);
    ReadInt(c["retreatfor"], cd.retreatfor);
    ReadInts(c["runfatigue"], cd.runfatigue, 2);
    if (!c["noparalyze"].IsNull())
        cd.noparalyze = c["noparalyze"].Bool();
    ReadInt(c["poisonchance"], cd.poisonchance);
    int32_t arrow[3] = {cd.arrowpos.x, cd.arrowpos.y, cd.arrowpos.z};
    ReadInts(c["arrowpos"], arrow, 3);
    cd.arrowpos = S3DPoint(arrow[0], arrow[1], arrow[2]);
    ReadInt(c["arrowspeed"], cd.arrowspeed);
    ReadInt(c["bowwait"], cd.bowwait);
    ReadInt(c["bowaimspeed"], cd.bowaimspeed);
    if (c.Has("impacts"))
    {
        std::fill(std::begin(cd.impacts), std::end(cd.impacts), SCharAttackImpact{});   // the unused slots zero, as retail's
        cd.numimpacts = 0;
        for (const JsonValue& imp : c["impacts"].Items())
            if (cd.numimpacts < MAXCHARIMPACTS)
                ReadImpact(imp, cd.impacts[cd.numimpacts++]);
    }
    if (c.Has("attacks"))
    {
        cd.attacks.Clear();
        for (const JsonValue& v : c["attacks"].Items())
        {
            SCharAttackData ad;
            ReadAttack(v, ad);
            cd.attacks.Add(ad);
        }
    }
}

// Retail's stat ids (charstats.h) under the names the retail fixture
// (slots/combat/guest.py) gives them.
const char* ObjStatName(int32_t statid)
{
    static const char* const flags[] = {"aggressive", "poisoned", "sleeping"};
    static const char* const values[] = {"health", "fatigue", "mana"};
    static const char* const attributes[] = {"strn", "cons", "agil", "rflx", "mind", "luck"};
    static const char* const skills[] = {"attack", "defense", "invoke", "hands", "knife", "sword",
                                         "bludgeons", "axes", "bows", "stealth", "lockpick"};
    if (statid >= CHRFLAG_FIRST && statid < CHRFLAG_FIRST + NUM_CHRFLAGS)
        return flags[statid - CHRFLAG_FIRST];
    if (statid >= CHRSTAT_FIRST && statid < CHRSTAT_FIRST + NUM_CHRSTATS)
        return values[statid - CHRSTAT_FIRST];
    if (statid == CHRVAL_DAMAGEMOD)
        return "damagemod";
    if (statid == PLRVAL_FIRST + PLRVAL_LEVEL)
        return "level";
    if (statid == PLRVAL_FIRST + PLRVAL_ATTACKLEVEL)
        return "attacklevel";
    if (statid == PLRVAL_FIRST + PLRVAL_ACBONUS)
        return "acbonus";
    if (statid == PLRVAL_FIRST + PLRVAL_EDGEBONUS)
        return "edgebonus";
    if (statid == PLRVAL_FIRST + PLRVAL_MANACOSTPCT)
        return "manacostpct";
    if (statid == PLRVAL_FIRST + PLRVAL_SPELLDAMAGEINC)
        return "spelldamageinc";
    if (statid == SKE_FIRST + SK_INVOKE)
        return "invokeexp";
    if (statid >= PLRSTAT_FIRST && statid < PLRSTAT_FIRST + NUM_PLRSTATS)
        return attributes[statid - PLRSTAT_FIRST];
    if (statid >= SK_FIRST && statid < SK_FIRST + NUM_SKILLS)
        return skills[statid - SK_FIRST];
    return nullptr;
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
            for (const JsonValue& m : s["motion"].Items())
                st.motion.push_back({(int32_t)m[0].Int(), (int32_t)m[1].Int(), (int32_t)m[2].Int(),
                                     (int32_t)m[3].Int(), (int32_t)m[4].Int(), (int32_t)m[5].Int()});
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

// The case's `walls` (pairs of names) block the line of sight from the
// first's eyes to the second's (CanSeeCharacter's, LIGHTINGCHARHEIGHT above
// each); every other line is clear (both sides).
std::vector<std::pair<std::string, std::string>> g_walls;

std::string EyesOf(const S3DPoint& p)
{
    for (TCharacter* c : g_world->Order())
        if (c->Pos().x == p.x && c->Pos().y == p.y && c->Pos().z + LIGHTINGCHARHEIGHT == p.z)
            return g_world->NameOf(c);
    return "?";
}

bool CaseLineOfSight(const S3DPoint& from, const S3DPoint& to)
{
    const std::pair<std::string, std::string> line{EyesOf(from), EyesOf(to)};
    const bool clear = std::find(g_walls.begin(), g_walls.end(), line) == g_walls.end();
    JsonOut j;
    j.Begin('{').FieldString("seam", "LineOfSight").FieldString("from", line.first);
    j.FieldString("to", line.second).Field("result", clear ? 1 : 0).End('}');
    Seam(j.str());
    return clear;
}

bool CaseBlocked(TCharacter* self, const S3DPoint& pos, const S3DPoint& newpos, uint32_t bits, TCharacter** bychar)
{
    if (bychar)
        *bychar = nullptr;
    JsonOut j;
    j.Begin('{').FieldString("seam", "FindClearPath").FieldString("who", g_world->NameOf(self));
    j.Key("to").Begin('[').Value(newpos.x).Value(newpos.y).Value(newpos.z).End(']');
    j.Field("result", g_blocked ? 1 : 0).End('}');
    Seam(j.str());
    return g_blocked;
}

// The case's ground (both sides answer the walk cells from it; guest.py
// ground_height) and the characters the map gives near a point.
struct SGroundBox
{
    int32_t gx0 = 0, gy0 = 0, gx1 = 0, gy1 = 0, height = 0;
};
struct SGround
{
    int32_t z = 0;
    std::vector<SGroundBox> cells;
    std::vector<std::pair<int32_t, int32_t>> nosector;
};
SGround g_ground;
std::vector<TCharacter*> g_nearby;

int32_t CaseWalkCell(int32_t gx, int32_t gy)
{
    const std::pair<int32_t, int32_t> sector(gx >> 6, gy >> 6);
    if (std::find(g_ground.nosector.begin(), g_ground.nosector.end(), sector) != g_ground.nosector.end())
        return 0;
    int32_t h = g_ground.z;
    for (const SGroundBox& b : g_ground.cells)
        if (b.gx0 <= gx && gx <= b.gx1 && b.gy0 <= gy && gy <= b.gy1)
            h = b.height;
    return h;
}

std::vector<TCharacter*> CaseNearby(const S3DPoint& pos, int32_t range)
{
    JsonOut j;
    j.Begin('{').FieldString("seam", "NearbyCharacters");
    j.Key("pos").Begin('[').Value(pos.x).Value(pos.y).Value(pos.z).End(']');
    j.Field("range", range).End('}');
    Seam(j.str());
    return g_nearby;
}

}  // namespace

void WriteSeams(JsonOut& j)
{
    WriteSeamsSince(j, 0);
}

size_t SeamCount()
{
    return g_seams.size();
}

void WriteSeamsSince(JsonOut& j, size_t first)
{
    j.Key("seams").Begin('[');
    for (size_t i = first; i < g_seams.size(); ++i)
        j.Raw(g_seams[i]);
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

void ClearDraws() { g_draws.clear(); }

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
    // MapPane's ambient light as the case gives it (SetAmbientLight adds the
    // gamma offset).
    savedAmbient = MapPane.GetAmbientLight();
    MapPane.SetAmbientLight((int32_t)g["ambient"].Int(128) - GammaAmbientOffset(GammaLevel), true);
    PlayScreen.SetFixtureState((int32_t)g["frame"].Int(0), g["control"].Bool(true), g["ps_5d8"].Bool(false));
    g_world = &world;
    // The player: the last one the case builds, as the retail fixture's
    // G_PLAYER (0x00667fcc) is.
    savedPlayer = Player;
    Player = nullptr;
    for (TCharacter* c : world.Order())
        if (c->ObjClass() == OBJCLASS_PLAYER)
            Player = static_cast<TPlayer*>(c);
    g_blocked = cs["blocked"].Bool(false);
    g_sees = cs["sees"].Bool(true);
    // With `perception`, the characters find and see each other as the
    // game's own code (kata M9b), only the line of sight the case's;
    // otherwise FindCharacters finds no one and CanSeeCharacter answers
    // `sees`.
    if (cs["perception"].Bool())
    {
        g_walls.clear();
        for (const JsonValue& w : cs["walls"].Items())
            g_walls.emplace_back(w[0].Str(), w[1].Str());
        TMapPane::lineOfSightSeam = CaseLineOfSight;
    }
    else
    {
        TCharacter::findCharactersSeam = EmptyWorld;
        TCharacter::canSeeSeam = CaseSees;
    }
    // The characters near a point: the case's `nearby`, else all of them,
    // in case order (as the retail fixture's map iterator gives them).
    g_nearby.clear();
    if (cs.Has("nearby"))
        for (const JsonValue& n : cs["nearby"].Items())
            g_nearby.push_back(world.Get(n.Str()));
    else
        g_nearby = world.Order();
    TCharacter::nearbyCharactersSeam = CaseNearby;
    TActionBlock::destroyedSeam = [](const TActionBlock* ab) { g_world->ForgetBlock(ab); };
    TMapPane::playMouseClickSeam = [](int32_t button, int32_t x, int32_t y) {
        JsonOut j;
        j.Begin('{').FieldString("seam", "PlayMouseClick").Field("button", button).Field("x", x);
        j.Field("y", y).End('}');
        Seam(j.str());
    };
    TPlayer::playerStateSeam = [](TPlayer* player, int32_t newstate) {
        JsonOut j;
        j.Begin('{').FieldString("seam", "SetPlayerState").FieldString("who", g_world->NameOf(player));
        j.Field("value", newstate).End('}');
        Seam(j.str());
    };
    if (cs.Has("ground"))
    {
        const JsonValue& gr = cs["ground"];
        g_ground = SGround{};
        g_ground.z = (int32_t)gr["z"].Int();
        for (const JsonValue& b : gr["cells"].Items())
            g_ground.cells.push_back(SGroundBox{(int32_t)b[0].Int(), (int32_t)b[1].Int(), (int32_t)b[2].Int(),
                                                (int32_t)b[3].Int(), (int32_t)b[4].Int()});
        for (const JsonValue& sc : gr["nosector"].Items())
            g_ground.nosector.emplace_back((int32_t)sc[0].Int(), (int32_t)sc[1].Int());
        TMapPane::walkGridSeam = CaseWalkCell;
    }
    else
        TCharacter::blockedSeam = CaseBlocked;
    ClearSeams();
}

SCaseScope::~SCaseScope()
{
    MapPane.SetAmbientLight(savedAmbient - GammaAmbientOffset(GammaLevel), true);
    SetRandomSource(nullptr);
    SetRandomRangeObserver(nullptr);
    TCharacter::findCharactersSeam = nullptr;
    TCharacter::blockedSeam = nullptr;
    TCharacter::canSeeSeam = nullptr;
    TPlayer::playerStateSeam = nullptr;
    TActionBlock::destroyedSeam = nullptr;
    TMapPane::playMouseClickSeam = nullptr;
    TCharacter::nearbyCharactersSeam = nullptr;
    TMapPane::walkGridSeam = nullptr;
    TMapPane::lineOfSightSeam = nullptr;
    Player = savedPlayer;
    g_world = nullptr;
}

}  // namespace RetailAB::Fixture
