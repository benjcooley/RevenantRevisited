// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *   retailab_melee.cpp - the port's side of the melee A/B dumps         *
// *************************************************************************
//
// The combat dojo's melee katas (docs/gameplay/COMBAT_DOJO.md C2-C7): attack
// choice, hit resolution, Damage, the resolvers, the AI, the character's
// tick, experience. Mirrors
// tools/retail_runtime/slots/combat/melee_attack.py: the same case (one JSON
// object, field 0 of the case line), the same seams answered from it, the
// same JSON out (schema `combat.melee.v1`).

#include "retailab_melee.h"

#include "3dimage.h"              // ResolveTagSound
#include "effect.h"               // TBloodEffect
#include "gameflow.h"             // TGameFlow::playerDiedSeam
#include "gameoptions.h"          // NoCombatResults
#include "logging.h"
#include "mappane.h"                // TMapPane::lineOfSightSeam
#include "playscreen.h"
#include "revenant.h"             // CheatNahkranoth, CheatAlreadyDead, AutoBeginCombat
#include "sound.h"

#include <optional>

namespace RetailAB
{
namespace
{

using namespace Fixture;

// ---- The melee seams ---------------------------------------------------------

// What the case answers for the seams the melee code reaches beyond the
// shared ones (melee_attack.py's): who FindCharacters finds, whether the
// way is blocked and by whom, what Block and a spell cast return.
struct SMeleeAnswers
{
    const TFixtureWorld* world = nullptr;
    TCharacter* found = nullptr;
    bool blocked = false;
    TCharacter* blocker = nullptr;
    bool blockResult = true;
    std::vector<std::string> spells;
    bool castResult = true;
    const JsonValue* cs = nullptr;
    bool active = false;                    // a case is running (the text bar capture)
};
SMeleeAnswers g_answers;

std::string AttackName(const TFixtureWorld& world, const JsonValue& cs, const SCharAttackData* ad);
std::string ImpactName(const TFixtureWorld& world, const JsonValue& cs, const SCharAttackImpact* imp);

void WriteNameOrNull(JsonOut& j, const char* key, const TObjectInstance* o)
{
    j.Key(key);
    if (o)
        j.String(g_answers.world->NameOf(o));
    else
        j.Null();
}

int32_t CaseFound(TCharacter* self, TCharacter* chars[], int32_t maxchars, int32_t range, int32_t angle,
                  int32_t anglerange, int32_t flags)
{
    const int32_t n = g_answers.found && maxchars > 0 ? 1 : 0;
    if (maxchars > 0)
        chars[0] = g_answers.found;
    JsonOut j;
    j.Begin('{').FieldString("seam", "FindCharacters").FieldString("who", g_answers.world->NameOf(self));
    j.Field("max", maxchars).Field("range", range).Field("angle", angle).Field("anglerange", anglerange);
    j.Field("flags", flags).Field("result", n).End('}');
    Seam(j.str());
    return n;
}

bool CaseBlocked(TCharacter* self, const S3DPoint& pos, const S3DPoint& newpos, uint32_t bits, TCharacter** bychar)
{
    TCharacter* blocker = g_answers.blocked ? g_answers.blocker : nullptr;
    if (bychar)
        *bychar = blocker;
    JsonOut j;
    j.Begin('{').FieldString("seam", "FindClearPath").FieldString("who", g_answers.world->NameOf(self));
    j.Key("to").Begin('[').Value(newpos.x).Value(newpos.y).Value(newpos.z).End(']');
    WriteNameOrNull(j, "blocker", blocker);
    j.Field("result", g_answers.blocked ? 1 : 0).End('}');
    Seam(j.str());
    return g_answers.blocked;
}

bool CaseBlock(TCharacter* self, int32_t frames)
{
    JsonOut j;
    j.Begin('{').FieldString("seam", "Block").FieldString("who", g_answers.world->NameOf(self));
    j.Field("frames", frames).Field("result", g_answers.blockResult ? 1 : 0).End('}');
    Seam(j.str());
    return g_answers.blockResult;
}

bool CaseCast(TCharacter* self, const char* spell, TObjectInstance** targets, int32_t numtargs,
              const S3DPoint* source)
{
    const bool known = std::any_of(g_answers.spells.begin(), g_answers.spells.end(),
                                   [&](const std::string& s) { return !strcasecmp(s.c_str(), spell); });
    JsonOut j;
    j.Begin('{').FieldString("seam", "CastByName").FieldString("spell", spell);
    if (known)
    {
        j.Key("targets").Begin('[');
        for (int32_t i = 0; i < numtargs; ++i)
            j.String(g_answers.world->NameOf(targets[i]));
        j.End(']');
        j.Key("source");
        if (source)
            j.Begin('[').Value(source->x).Value(source->y).Value(source->z).End(']');
        else
            j.Null();
    }
    j.Field("result", known && g_answers.castResult ? 1 : 0).End('}');
    Seam(j.str());
    return known && g_answers.castResult;
}

// IsEnemy: the case's `friends` are [who, other] pairs that aren't enemies.
bool CaseIsEnemy(TCharacter* self, TCharacter* other)
{
    const std::string who = g_answers.world->NameOf(self), them = g_answers.world->NameOf(other);
    bool enemy = true;
    for (const JsonValue& pair : (*g_answers.cs)["friends"].Items())
        if (pair[0].Str() == who && pair[1].Str() == them)
            enemy = false;
    JsonOut j;
    j.Begin('{').FieldString("seam", "IsEnemy").FieldString("who", who).FieldString("other", them);
    j.Field("result", enemy ? 1 : 0).End('}');
    Seam(j.str());
    return enemy;
}

bool CaseBeginFighting(TCharacter* self, TCharacter* target, ACTION action)
{
    JsonOut j;
    j.Begin('{').FieldString("seam", "BeginFighting").FieldString("who", g_answers.world->NameOf(self));
    WriteNameOrNull(j, "target", target);
    j.Field("action", (int32_t)action).Field("result", 1).End('}');
    Seam(j.str());
    return true;
}

// Damage, recorded with the block it's given by meaning; the seam owns the
// block, as Damage does: kept if it is now one of the victim's three, else
// freed.
void CaseDamage(TCharacter* self, int32_t damage, int32_t damagetype, int32_t modifier, TActionBlock* ab,
                TCharacter* attacker)
{
    const TFixtureWorld& world = *g_answers.world;
    JsonOut j;
    j.Begin('{').FieldString("seam", "Damage").FieldString("who", world.NameOf(self));
    j.Field("damage", damage).Field("type", damagetype).Field("mod", modifier);
    j.Key("block");
    if (ab)
    {
        j.Begin('{').Field("action", (int32_t)ab->action).FieldString("name", ab->name).Field("wait", ab->wait);
        j.Field("damage", ab->damage);
        WriteNameOrNull(j, "obj", ab->obj);
        j.Key("attack");
        if (ab->attack)
            j.String(AttackName(world, *g_answers.cs, ab->attack));
        else
            j.Null();
        j.Key("impact");
        if (ab->impact)
            j.String(ImpactName(world, *g_answers.cs, ab->impact));
        else
            j.Null();
        WriteFlags(j, ab->flags);
        j.End('}');
    }
    else
        j.Null();
    WriteNameOrNull(j, "attacker", attacker);
    j.End('}');
    Seam(j.str());
    IFixtureChar* fx = world.Fixture(self);
    if (ab && ab != fx->Root() && ab != fx->Doing() && ab != fx->Desired())
        delete ab;
}

void CaseCombatFlash(TCharacter* self)
{
    JsonOut j;
    j.Begin('{').FieldString("seam", "EffectCombatFlash").FieldString("who", g_answers.world->NameOf(self)).End('}');
    Seam(j.str());
}

// CanSeeCharacter: the case's `sees`, yes or no, or the names seen.
bool CaseSeesMelee(TCharacter* self, TCharacter* chr, int32_t angle)
{
    const JsonValue& sees = (*g_answers.cs)["sees"];
    const std::string them = g_answers.world->NameOf(chr);
    bool seen = sees.Bool(true);
    if (sees.GetKind() == JsonValue::Kind::Array)
        seen = std::any_of(sees.Items().begin(), sees.Items().end(),
                           [&](const JsonValue& n) { return n.Str() == them; });
    JsonOut j;
    j.Begin('{').FieldString("seam", "CanSeeCharacter").FieldString("who", g_answers.world->NameOf(self));
    j.FieldString("target", them).Field("result", seen ? 1 : 0).End('}');
    Seam(j.str());
    return seen;
}

// The helpers near a point: the case's `waypoints` ({name, pos, type,
// level}, in the order the map gives them), each a helper object of that
// type (with a fixture imagery of no states, kept as the world's
// characters' are: nothing here draws) at map index kHelperIndex + k. Made
// before the case's seams are cleared.
struct SCaseHelper
{
    std::string name;
    std::unique_ptr<TObjectInstance> object;
    int32_t level = 0;
};
std::vector<SCaseHelper> g_helpers;
constexpr int32_t kHelperIndex = 0x7000;        // map indices the helpers take (retail HELPER_INDEX)

void MakeHelpers(const JsonValue& cs)
{
    g_helpers.clear();
    const TObjectClass* helpers = TObjectClass::GetClass(OBJCLASS_HELPER);
    for (const JsonValue& w : cs["waypoints"].Items())
    {
        SObjectDef def{};
        def.objclass = OBJCLASS_HELPER;
        const std::string type = w.Has("type") ? w["type"].Str() : "Waypoint";
        def.objtype = (short)helpers->FindObjType(type.c_str());
        if (def.objtype < 0)
            throw std::runtime_error("no helper type " + type);
        def.pos = S3DPoint((int32_t)w["pos"][0].Int(), (int32_t)w["pos"][1].Int(), (int32_t)w["pos"][2].Int());
        def.level = (uint16_t)w["level"].Int(0);
        auto* imagery = new TFixtureImagery(TFixtureImagery::Register({}), w["name"].Str(), {}, false);
        auto object = std::make_unique<TObjectInstance>(&def, imagery);
        object->SetMapIndex(kHelperIndex + (int32_t)g_helpers.size());
        g_helpers.push_back({w["name"].Str(), std::move(object), (int32_t)def.level});
    }
}

TObjectInstance* HelperOrCharacter(const TFixtureWorld& world, const std::string& name)
{
    for (const SCaseHelper& h : g_helpers)
        if (h.name == name)
            return h.object.get();
    return world.Get(name);
}

std::string HelperName(const TObjectInstance* o)
{
    for (const SCaseHelper& h : g_helpers)
        if (h.object.get() == o)
            return h.name;
    return g_answers.world->NameOf(o);
}

int32_t CaseWaypoints(TCharacter* self, const S3DPoint& centre, TObjectInstance** found, int32_t max)
{
    const int32_t n = (int32_t)g_helpers.size();
    for (int32_t i = 0; i < n && i < max; ++i)
        found[i] = g_helpers[i].object.get();
    JsonOut j;
    j.Begin('{').FieldString("seam", "FindObjectsInRange").FieldString("who", g_answers.world->NameOf(self));
    j.Key("centre").Begin('[').Value(centre.x).Value(centre.y).Value(centre.z).End(']');
    j.Field("max", max).Field("result", n).End('}');
    Seam(j.str());
    return n;
}

// The line of sight from a helper (WaypointReachable 0x00528850 looks from
// it, at the character's height, to the character): clear unless the case's
// `walls` name the pair.
bool CaseHelperSight(const S3DPoint& from, const S3DPoint& to)
{
    std::string a = "?", b = "?";
    for (const SCaseHelper& h : g_helpers)
        if (h.object->Pos().x == from.x && h.object->Pos().y == from.y)
            a = h.name;
    for (const JsonValue& spec : (*g_answers.cs)["chars"].Items())
    {
        const S3DPoint p = g_answers.world->Get(spec["name"].Str())->Pos();
        if (p.x == to.x && p.y == to.y && p.z == to.z)
            b = spec["name"].Str();
    }
    bool clear = true;
    for (const JsonValue& w : (*g_answers.cs)["walls"].Items())
        if (w[0].Str() == a && w[1].Str() == b)
            clear = false;
    JsonOut j;
    j.Begin('{').FieldString("seam", "LineOfSight").FieldString("from", a).FieldString("to", b);
    j.Field("result", clear ? 1 : 0).End('}');
    Seam(j.str());
    return clear;
}

// SetWalkMode / SetRunMode / EndFighting (melee_attack.py WANDER_SEAMS):
// recorded, done.
bool CaseModeCall(const char* seam, TCharacter* self)
{
    JsonOut j;
    j.Begin('{').FieldString("seam", seam).FieldString("who", g_answers.world->NameOf(self)).Field("result", 1);
    j.End('}');
    Seam(j.str());
    return true;
}

void CaseEffectBurst(TCharacter* self, const char* name, int32_t height)
{
    JsonOut j;
    j.Begin('{').FieldString("seam", "EffectBurst").FieldString("who", g_answers.world->NameOf(self));
    j.FieldString("name", name).Field("height", height).End('}');
    Seam(j.str());
}

// No sound by any name: nothing plays (retail's lookup 0x0049c430).
int32_t CaseSound(const char* name, int32_t nr)
{
    JsonOut j;
    j.Begin('{').FieldString("seam", "Sound").FieldString("name", name ? name : "").End('}');
    Seam(j.str());
    return -1;
}

void ExpRecord(TPlayer* self, const char* kind, int32_t skill, TCharacter* victim)
{
    JsonOut j;
    j.Begin('{').FieldString("seam", "Exp").FieldString("who", g_answers.world->NameOf(self));
    j.FieldString("kind", kind).Field("skill", skill);
    WriteNameOrNull(j, "victim", victim);
    j.End('}');
    Seam(j.str());
}

void CaseKillExp(TPlayer* self, TCharacter* victim) { ExpRecord(self, "kill", -1, victim); }
void CaseSkillExp(TPlayer* self, int32_t skill, TCharacter* victim) { ExpRecord(self, "skill", skill, victim); }
void CaseStealthExp(TPlayer* self, TCharacter* victim) { ExpRecord(self, "stealth", -1, victim); }

// What the text bar is sent (TTextBar logs each message as "[textbar] ..."):
// retail's 0x0054d170.
void CaptureTextBar(log_Event* ev)
{
    if (!g_answers.active || ev->level != LOG_DEBUG || !ev->fmt || strncmp(ev->fmt, "[textbar] ", 10) != 0)
        return;
    char buf[1024];
    vsnprintf(buf, sizeof(buf), ev->fmt, ev->ap);
    JsonOut j;
    j.Begin('{').FieldString("seam", "TextBar").FieldString("text", buf + 10).End('}');
    Seam(j.str());
}

// The case's rules (the combat-data dump's names), the cheat, the seams
// above; the seams come off when the scope ends.
class SMeleeScope
{
  public:
    SMeleeScope(const JsonValue& cs, const TFixtureWorld& world)
    {
        const JsonValue& r = cs["rules"];
        Rules.tohitcenter = (int32_t)r["tohitcenter"].Int();
        Rules.tohitrangechar = (int32_t)r["tohitrangechar"].Int();
        Rules.tohitrangeplyr = (int32_t)r["tohitrangeplyr"].Int();
        Rules.tohitblock = (int32_t)r["tohitblock"].Int();
        Rules.tohitface = (int32_t)r["tohitface"].Int();
        for (int32_t i = 0; i < TRules::kToHitDamageRows; ++i)
        {
            Rules.tohitdamage[i].minvalue = (int32_t)r["tohitdamage"][i][0].Int();
            Rules.tohitdamage[i].damagepercent = (int32_t)r["tohitdamage"][i][1].Int();
        }
        const JsonValue& tables = r["statlevels"];
        for (int32_t t = 0; t < PlayerStats::TStatLevels::kTables; ++t)
            for (int32_t level = 0; level < PlayerStats::TStatLevels::kLevels; ++level)
                Rules.SetStatLevel(t, level, (int32_t)tables[t][level].Int(PlayerStats::TStatLevels::kUnset));
        CheatNahkranoth = cs["globals"]["nahkranoth"].Bool(false);
        CheatAlreadyDead = cs["globals"]["alreadydead"].Bool(false);

        g_answers = SMeleeAnswers{};
        g_answers.world = &world;
        if (cs.Has("found") && !cs["found"].IsNull())
            g_answers.found = world.Get(cs["found"].Str());
        g_answers.blocked = cs["blocked"].Bool(false);
        if (cs.Has("blocker") && !cs["blocker"].IsNull())
            g_answers.blocker = world.Get(cs["blocker"].Str());
        g_answers.blockResult = cs["block_result"].Bool(true);
        for (const JsonValue& s : cs["spells"].Items())
            g_answers.spells.push_back(s.Str());
        g_answers.castResult = cs["cast_result"].Bool(true);
        g_answers.cs = &cs;
        g_answers.active = true;
        NoCombatResults = cs["globals"]["nocombatresults"].Bool(false);
        // With `perception` the characters find, see and judge each other as
        // the game's own code (kata M9b's; the line of sight is SCaseScope's);
        // `seam_path` false finds their way as the game's own (over the
        // case's ground).
        if (!cs["perception"].Bool())
        {
            TCharacter::findCharactersSeam = CaseFound;
            TCharacter::canSeeSeam = CaseSeesMelee;
            TCharacter::isEnemySeam = CaseIsEnemy;
        }
        if (cs["seam_path"].Bool(true))
            TCharacter::blockedSeam = CaseBlocked;
        TCharacter::blockSeam = cs["seam_block"].Bool(true) ? CaseBlock : nullptr;
        TCharacter::castSeam = CaseCast;
        TCharacter::beginFightingSeam = cs["seam_begin"].Bool(true) ? CaseBeginFighting : nullptr;
        TCharacter::damageSeam = cs["seam_damage"].Bool(true) ? CaseDamage : nullptr;
        TCharacter::effectBurstSeam = CaseEffectBurst;
        TCharacter::effectCombatFlashSeam = CaseCombatFlash;
        // With `real_map` the waypoint search and its line of sight run as
        // the port's own (FindObjectsInRange over the helpers, LineOfSight
        // over the case's ground); otherwise the case answers them.
        if (cs["real_map"].Bool(false))
        {
            std::vector<SMapObject> walked;
            for (const SCaseHelper& h : g_helpers)
                walked.push_back(SMapObject{h.object.get(), h.level});
            SetMapObjects(std::move(walked));
        }
        else
        {
            TCharacter::waypointsSeam = CaseWaypoints;
            if (!g_helpers.empty())
                TMapPane::lineOfSightSeam = CaseHelperSight;
        }
        // Wander's callees (kata M9's `seam_wander`): recorded.
        if (cs["seam_wander"].Bool(false))
        {
            TCharacter::setWalkModeSeam = [](TCharacter* self) { return CaseModeCall("SetWalkMode", self); };
            TCharacter::setRunModeSeam = [](TCharacter* self) { return CaseModeCall("SetRunMode", self); };
            TCharacter::endFightingSeam = [](TCharacter* self) { return CaseModeCall("EndFighting", self); };
        }
        Editor = cs["globals"]["editor"].Bool(false);
        NoAI = cs["globals"]["ai_off"].Bool(false);       // retail 0x00668110, the dummies cheat
        TSoundPlayer::findSeam = CaseSound;
        if (cs["seam_exp"].Bool(true))
        {
            TPlayer::killExpSeam = CaseKillExp;
            TPlayer::awardSkillExpSeam = CaseSkillExp;
            TPlayer::stealthExpSeam = CaseStealthExp;
        }
        static const bool captured = log_add_callback(CaptureTextBar, nullptr, LOG_DEBUG) == 0;
        (void)captured;
    }
    ~SMeleeScope()
    {
        TCharacter::blockSeam = nullptr;
        TCharacter::castSeam = nullptr;
        TCharacter::isEnemySeam = nullptr;
        TCharacter::beginFightingSeam = nullptr;
        TCharacter::damageSeam = nullptr;
        TCharacter::effectBurstSeam = nullptr;
        TCharacter::effectCombatFlashSeam = nullptr;
        TCharacter::waypointsSeam = nullptr;
        TCharacter::setWalkModeSeam = nullptr;
        TCharacter::setRunModeSeam = nullptr;
        TCharacter::endFightingSeam = nullptr;
        if (!g_helpers.empty())
            TMapPane::lineOfSightSeam = nullptr;
        g_helpers.clear();
        Editor = false;
        NoAI = false;
        TSoundPlayer::findSeam = nullptr;
        TPlayer::killExpSeam = nullptr;
        TPlayer::awardSkillExpSeam = nullptr;
        TPlayer::stealthExpSeam = nullptr;
        CheatNahkranoth = false;
        CheatAlreadyDead = false;
        NoCombatResults = false;
        g_answers = SMeleeAnswers{};
    }
    SMeleeScope(const SMeleeScope&) = delete;
    SMeleeScope& operator=(const SMeleeScope&) = delete;
};

// ---- The tick's world (kata C6) and experience's (C7) ---------------------------

// An object NewObject makes (blood, a level-up's PowerUp): the def recorded
// as the spells katas record it, a stand-in at the next map index whose
// SetParams (retail slot 0x200) is recorded. TBloodEffect, as the tick's
// blood takes what it finds there to be.
class TFixtureMade : public TBloodEffect
{
  public:
    TFixtureMade(TObjectImagery* im, std::string label) : TBloodEffect(im), who(std::move(label)) {}
    void SetParams(int32_t he, int32_t ha, int32_t va, int32_t hs, int32_t vs, int32_t nu) override
    {
        JsonOut j;
        j.Begin('{').FieldString("seam", "blood.SetParams").FieldString("who", who);
        j.Key("args").Begin('[').Value(he).Value(ha).Value(va).Value(hs).Value(vs).Value(nu).End(']');
        j.Field("result", 0).End('}');
        Seam(j.str());
    }

  private:
    std::string who;
};
// Made before the case's seams start (their imagery is asked about the
// animator as they are built); NewObject hands them out in turn.
std::vector<std::unique_ptr<TFixtureMade>> g_made;
size_t g_madeUsed = 0;
constexpr int32_t kMadeIndex = 0x7100;          // map indices the made objects take (retail BLOOD_INDEX)
constexpr size_t kMadeObjects = 16;

void MakeMadeObjects()
{
    g_made.clear();
    g_madeUsed = 0;
    for (size_t i = 0; i < kMadeObjects; ++i)
    {
        const std::string label = "made " + std::to_string(i + 1);
        auto* imagery = new TFixtureImagery(TFixtureImagery::Register({}), label, {}, false);
        g_made.push_back(std::make_unique<TFixtureMade>(imagery, label));
    }
}

// The type by name, in lower case: the retail fixture has the name the
// code looks up, the port the class's own spelling of it.
std::string LowerName(const char* name)
{
    std::string s = name;
    for (char& c : s)
        c = (char)tolower((unsigned char)c);
    return s;
}

// The def NewObject is given, as the retail fixture records it.
void RecordNewObject(const SObjectDef* def, int32_t imagery)
{
    TObjectClass* cl = TObjectClass::GetClass(def->objclass);
    SObjectInfo* info = cl ? cl->GetObjType(def->objtype) : nullptr;
    JsonOut j;
    j.Begin('{').FieldString("seam", "NewObject").Field("class", def->objclass).Key("type");
    if (info)
        j.String(LowerName(info->name));
    else
        j.Value(def->objtype);
    j.Field("flags", (int64_t)def->flags).Field("state", def->state).Field("level", def->level);
    j.Key("pos").Begin('[').Value(def->pos.x).Value(def->pos.y).Value(def->pos.z).End(']');
    j.Key("vel").Begin('[').Value(def->vel.x).Value(def->vel.y).Value(def->vel.z).End(']');
    j.Field("facing", def->facing).Field("imagery", imagery).End('}');
    Seam(j.str());
}

int32_t CaseNewObject(SObjectDef* def, int32_t imagery)
{
    RecordNewObject(def, imagery);
    if (g_madeUsed == g_made.size())
        throw std::runtime_error("more objects made than the fixture holds");
    const int32_t index = kMadeIndex + (int32_t)g_madeUsed;
    g_made[g_madeUsed++]->SetMapIndex(index);
    return index;
}

void CaseRecord(const char* seam, const TObjectInstance* who)
{
    JsonOut j;
    j.Begin('{').FieldString("seam", seam);
    if (who)
        j.FieldString("who", g_answers.world->NameOf(who));
    j.End('}');
    Seam(j.str());
}

// The recovery rules by the combat-data dump's names (melee_attack.py
// RULES_RECOVERY).
struct SRecoveryRule
{
    const char* name;
    int32_t TRules::*field;
};
constexpr std::array<SRecoveryRule, 8> kRecovery{{
    {"healthrecovval", &TRules::healthrecovval}, {"fatiguerecovval", &TRules::fatiguerecovval},
    {"manarecovval", &TRules::manarecovval}, {"healthrecovrate", &TRules::healthrecovrate},
    {"fatiguerecovrate", &TRules::fatiguerecovrate}, {"manarecovrate", &TRules::manarecovrate},
    {"poisondamageval", &TRules::poisondamageval}, {"poisondamagerate", &TRules::poisondamagerate}}};

// The tick's globals for a case (melee_attack.py _pulse_world): the play
// screen as the current screen with the case's screen frames and game
// time, the AutoBeginCombat option, the recovery rules; the seams of the
// tick and of experience. All put back when the scope ends.
class SPulseScope
{
  public:
    SPulseScope(const JsonValue& cs, bool ticking)
        : savedScreen(CurrentScreen), savedGameTime(PlayScreen.GameTime()),
          savedScreenFrames(PlayScreen.FrameCount()), savedAutoBegin(AutoBeginCombat)
    {
        for (size_t i = 0; i < kRecovery.size(); ++i)
            savedRules[i] = Rules.*kRecovery[i].field;
        const JsonValue& g = cs["globals"];
        if (ticking)
        {
            CurrentScreen = &PlayScreen;
            PlayScreen.SetFixtureGameTime((int32_t)g["gametime"].Int(0));
            PlayScreen.SetFixtureScreenFrames((int32_t)g["screenframe"].Int(0));
            AutoBeginCombat = g["autobegincombat"].Bool(true);
            for (const SRecoveryRule& r : kRecovery)
                Rules.*r.field = (int32_t)cs["rules"][r.name].Int(Rules.*r.field);
            MakeMadeObjects();
            ClearSeams();
            TMapPane::newObjectSeam = CaseNewObject;
            TGameFlow::playerDiedSeam = [] { CaseRecord("DeathScreen", nullptr); };
            TPlayer::refreshStatsSeam = [](TPlayer* p) { CaseRecord("RefreshStats", p); };
        }
        const bool pulse = cs["call"].Str() == "pulse";
        quietReads = cs["quiet_reads"].Bool(ticking);
        seamUpdateAction = pulse && cs["seam_update"].Bool(true);
        seamAI = pulse && cs["seam_ai"].Bool(true);
    }
    ~SPulseScope()
    {
        CurrentScreen = savedScreen;
        PlayScreen.SetFixtureGameTime(savedGameTime);
        PlayScreen.SetFixtureScreenFrames(savedScreenFrames);
        AutoBeginCombat = savedAutoBegin;
        for (size_t i = 0; i < kRecovery.size(); ++i)
            Rules.*kRecovery[i].field = savedRules[i];
        TMapPane::newObjectSeam = nullptr;
        TGameFlow::playerDiedSeam = nullptr;
        TPlayer::refreshStatsSeam = nullptr;
        quietReads = seamUpdateAction = seamAI = false;
        g_made.clear();
    }
    SPulseScope(const SPulseScope&) = delete;
    SPulseScope& operator=(const SPulseScope&) = delete;

  private:
    TScreen* savedScreen;
    int32_t savedGameTime;
    int32_t savedScreenFrames;
    bool savedAutoBegin;
    std::array<int32_t, 8> savedRules{};
};

TPlayer* AsPlayer(TCharacter* c)
{
    auto* p = dynamic_cast<TPlayer*>(c);
    if (!p)
        throw std::runtime_error("the experience calls need a player");
    return p;
}

// ---- The bosses (kata C3c, docs/gameplay/forensics/BOSS_AI.md) -------------------

// A map object of the case's that isn't a character (`props`: the ring wall,
// the table, waypoints): an object of its class.def type with the case's
// name, position and id; its SetState recorded as the retail fixture's
// shared seam records it.
class TFixtureProp : public TObjectInstance
{
  public:
    TFixtureProp(SObjectDef* def, std::string label, std::vector<std::string> statenames)
        : TObjectInstance(def, new TFixtureImagery(TFixtureImagery::Register({}), label, {}, false)),
          who(std::move(label)), states(std::move(statenames))
    {
        this->name = strdup(who.c_str());
    }

    bool SetState(int32_t index) override
    {
        JsonOut j;
        j.Begin('{').FieldString("seam", "SetState").FieldString("who", who).Field("state", index).Key("name");
        if (index >= 0 && (size_t)index < states.size())
            j.String(states[index]);
        else
            j.Null();
        j.End('}');
        Seam(j.str());
        this->state = (uint16_t)index;
        return true;
    }

    [[nodiscard]] const std::string& Who() const { return who; }

  private:
    std::string who;
    std::vector<std::string> states;
};

// A boss's state by the retail fixture's names (melee_attack.py BOSS_FIELDS):
// its whole-number fields and points by member; the arrays by hand.
template <typename S>
struct SBossField
{
    const char* name;
    int32_t S::*value = nullptr;
    S3DPoint S::*point = nullptr;
};

constexpr std::array<SBossField<SBaezState>, 16> kBaezFields{{
    {"stunned", &SBaezState::stunned}, {"stunticks", &SBaezState::stunticks},
    {"stunlevel", &SBaezState::stunlevel}, {"stage", &SBaezState::stage},
    {"baezspot", nullptr, &SBaezState::baezspot}, {"playerspot", nullptr, &SBaezState::playerspot},
    {"table", &SBaezState::table}, {"frame", &SBaezState::frame}, {"throwlength", &SBaezState::throwlength},
    {"watchersfound", &SBaezState::watchersfound}, {"playerhealth", &SBaezState::playerhealth},
    {"health", &SBaezState::health}, {"falling", &SBaezState::falling},
    {"firstspot", nullptr, &SBaezState::firstspot}, {"playerarrived", &SBaezState::playerarrived},
    {"waitticks", &SBaezState::waitticks}}};
constexpr std::array<SBossField<SSolifugeState>, 10> kSolifugeFields{{
    {"skytimer", &SSolifugeState::skytimer}, {"skyphase", &SSolifugeState::skyphase},
    {"skyticks", &SSolifugeState::skyticks}, {"band", &SSolifugeState::band},
    {"stunticks", &SSolifugeState::stunticks}, {"stunned", &SSolifugeState::stunned},
    {"tinted", &SSolifugeState::tinted}, {"landing", &SSolifugeState::landing},
    {"pissedoff", &SSolifugeState::pissedoff}, {"slam", &SSolifugeState::slam}}};
constexpr std::array<SBossField<SJhagaState>, 6> kJhagaFields{{
    {"mode", &SJhagaState::mode}, {"resttimer", &SJhagaState::resttimer}, {"draw", &SJhagaState::draw},
    {"hop", &SJhagaState::hop}, {"dest", nullptr, &SJhagaState::dest}, {"frame", &SJhagaState::frame}}};
constexpr std::array<SBossField<SYhagoroState>, 3> kYhagoroFields{{
    {"mode", &SYhagoroState::mode}, {"towalk", &SYhagoroState::towalk}, {"tocombat", &SYhagoroState::tocombat}}};

S3DPoint PointOf(const JsonValue& v)
{
    return S3DPoint((int32_t)v[0].Int(), (int32_t)v[1].Int(), (int32_t)v[2].Int());
}

void WritePoint(JsonOut& j, const S3DPoint& p)
{
    j.Begin('[').Value(p.x).Value(p.y).Value(p.z).End(']');
}

template <typename S, size_t N>
void ReadBossFields(const std::array<SBossField<S>, N>& fields, const JsonValue& b, S& s)
{
    for (const SBossField<S>& f : fields)
    {
        if (!b.Has(f.name))
            continue;
        if (f.value)
            s.*f.value = (int32_t)b[f.name].Int();
        else
            s.*f.point = PointOf(b[f.name]);
    }
}

template <typename S, size_t N>
void WriteBossFields(const std::array<SBossField<S>, N>& fields, JsonOut& j, const S& s)
{
    for (const SBossField<S>& f : fields)
    {
        if (f.value)
            j.Field(f.name, s.*f.value);
        else
        {
            j.Key(f.name);
            WritePoint(j, s.*f.point);
        }
    }
}

// The case's `boss` as the character's state (its kind is `monsterkind`).
void ReadBossState(const TFixtureWorld& world, IFixtureChar* fx, const JsonValue& b)
{
    TBossState& state = fx->Boss();
    switch (fx->MonsterKind())
    {
        case 1:
        {
            auto& s = state.emplace<SBaezState>();
            ReadBossFields(kBaezFields, b, s);
            const auto& names = b["watchers"].Items();
            for (size_t i = 0; i < names.size() && i < s.watchers.size(); ++i)
                if (!names[i].IsNull())
                    s.watchers[i] = world.Get(names[i].Str());
            break;
        }
        case 2:
        {
            auto& s = state.emplace<SSolifugeState>();
            ReadBossFields(kSolifugeFields, b, s);
            const auto& brood = b["brood"].Items();
            for (size_t i = 0; i < brood.size() && i < s.brood.size(); ++i)
                s.brood[i] = (int32_t)brood[i].Int();
            break;
        }
        case 3:
        {
            auto& s = state.emplace<SJhagaState>();
            ReadBossFields(kJhagaFields, b, s);
            const auto& points = b["points"].Items();
            for (size_t i = 0; i < points.size() && i < s.points.size(); ++i)
                s.points[i] = PointOf(points[i]);
            break;
        }
        case 4:
            ReadBossFields(kYhagoroFields, b, state.emplace<SYhagoroState>());
            break;
        default:
            throw std::runtime_error("a `boss` needs a boss's monsterkind");
    }
}

// A character's boss as compared (melee_attack.py _boss_dump): the kind,
// where a MoveTo heads, and a boss's state by name.
void WriteBoss(const TFixtureWorld& world, JsonOut& j, IFixtureChar* fx)
{
    j.Begin('{').Field("kind", fx->MonsterKind());
    fx->WriteMoveTo(j);
    const TBossState& state = fx->Boss();
    const int32_t kind = fx->MonsterKind();
    if (const auto* s = std::get_if<SBaezState>(&state); s && kind == 1)
    {
        j.Key("state").Begin('{');
        WriteBossFields(kBaezFields, j, *s);
        j.Key("watchers").Begin('[');
        for (const TSafeRef<TComplexObject>& ref : s->watchers)
        {
            if (const TComplexObject* watcher = ref.Get())
                j.String(world.NameOf(watcher));
            else
                j.Null();
        }
        j.End(']').End('}');
    }
    else if (const auto* s = std::get_if<SSolifugeState>(&state); s && kind == 2)
    {
        j.Key("state").Begin('{');
        WriteBossFields(kSolifugeFields, j, *s);
        j.Key("brood").Begin('[');
        for (const int32_t index : s->brood)
            j.Value(index);
        j.End(']').End('}');
    }
    else if (const auto* s = std::get_if<SJhagaState>(&state); s && kind == 3)
    {
        j.Key("state").Begin('{');
        WriteBossFields(kJhagaFields, j, *s);
        j.Key("points").Begin('[');
        for (const S3DPoint& p : s->points)
            WritePoint(j, p);
        j.End(']').End('}');
    }
    else if (const auto* s = std::get_if<SYhagoroState>(&state); s && kind == 4)
    {
        j.Key("state").Begin('{');
        WriteBossFields(kYhagoroFields, j, *s);
        j.End('}');
    }
    j.End('}');
}

// What the boss case answers beyond the melee seams.
struct SBossAnswers
{
    const TFixtureWorld* world = nullptr;
    const JsonValue* cs = nullptr;
    std::vector<std::unique_ptr<TFixtureProp>> props;
    size_t madeHandedOut = 0;
};
SBossAnswers g_boss;
constexpr int32_t kBossMadeIndex = 0x7200;     // an id no object has (melee_attack.py MADE_INDEX)

// The registry GetInstance answers from: a fixture character keeps its case
// id without registering it.
void RegisterId(TObjectInstance* o, int32_t id)
{
    o->SetMapIndex(-1);
    o->SetMapIndex(id);
}

TObjectInstance* BossObject(const std::string& name)
{
    for (const auto& prop : g_boss.props)
        if (prop->Who() == name)
            return prop.get();
    return g_boss.world->Get(name);
}

void RecordWho(const char* seam, const TObjectInstance* who)
{
    JsonOut j;
    j.Begin('{').FieldString("seam", seam).FieldString("who", g_boss.world->NameOf(who)).End('}');
    Seam(j.str());
}

// The walk of the pane's sectors: the case's `paneobjects`, in order.
std::vector<TObjectInstance*> CasePaneObjects(TCharacter* self)
{
    RecordWho("PaneObjects", self);
    std::vector<TObjectInstance*> objects;
    for (const JsonValue& n : (*g_boss.cs)["paneobjects"].Items())
        objects.push_back(BossObject(n.Str()));
    return objects;
}

// NewObject: the def recorded; the case's next `made` character, now in
// the map; none left, an id no object has.
int32_t CaseBossNewObject(SObjectDef* def, int32_t imagery)
{
    RecordNewObject(def, imagery);
    const auto& made = (*g_boss.cs)["made"].Items();
    const size_t n = g_boss.madeHandedOut++;
    if (n >= made.size())
        return kBossMadeIndex + (int32_t)n;
    TCharacter* chr = g_boss.world->Get(made[n].Str());
    const int32_t id = chr->GetMapIndex();
    RegisterId(chr, id);
    return id;
}

// The boss case's world: the props, the ids GetInstance answers (the case's
// characters but the `made` ones not yet handed out, and the props), the
// boss states; the seams above and EndFighting, the tint, the control, the
// animator. All taken back when the scope ends.
class SBossScope
{
  public:
    SBossScope(const JsonValue& cs, const TFixtureWorld& world)
    {
        g_boss = SBossAnswers{};
        g_boss.world = &world;
        g_boss.cs = &cs;
        for (const JsonValue& p : cs["props"].Items())
        {
            SObjectDef def{};
            def.objclass = (short)p["class"].Int(OBJCLASS_TILE);
            const TObjectClass* cl = TObjectClass::GetClass(def.objclass);
            if (!cl)
                throw std::runtime_error("no class " + std::to_string(def.objclass) + " loaded");
            def.objtype = (short)cl->FindObjType(p["type"].Str().c_str());
            if (def.objtype < 0)
                throw std::runtime_error("class.def has no type '" + p["type"].Str() + "'");
            def.pos = PointOf(p["pos"]);
            def.flags = (uint32_t)p["flags"].Int(0);
            std::vector<std::string> states;
            for (const JsonValue& st : p["states"].Items())
                states.push_back(st.Str());
            g_boss.props.push_back(std::make_unique<TFixtureProp>(&def, p["name"].Str(), std::move(states)));
            if (const int32_t id = (int32_t)p["id"].Int(0))
                RegisterId(g_boss.props.back().get(), id);
        }
        std::vector<std::string> made;
        for (const JsonValue& n : cs["made"].Items())
            made.push_back(n.Str());
        for (const JsonValue& spec : cs["chars"].Items())
        {
            TCharacter* chr = world.Get(spec["name"].Str());
            const int32_t id = (int32_t)spec["id"].Int(0);
            if (id && std::find(made.begin(), made.end(), spec["name"].Str()) == made.end())
                RegisterId(chr, id);
            if (spec.Has("boss"))
                ReadBossState(world, world.Fixture(chr), spec["boss"]);
        }
        TCharacter::paneObjectsSeam = CasePaneObjects;
        TCharacter::endFightingSeam = [](TCharacter* self) {
            RecordWho("EndFighting", self);
            return true;
        };
        TCharacter::tintSeam = [](TCharacter* self, bool tint) { RecordWho(tint ? "Tint" : "Untint", self); };
        TPlayScreen::controlSeam = [](bool on) {
            JsonOut j;
            j.Begin('{').FieldString("seam", "SetControl").Field("on", on ? 1 : 0).End('}');
            Seam(j.str());
        };
        TMapPane::newObjectSeam = CaseBossNewObject;
        seamCreateAnimator = true;
        MapPane.CenterOnObj(nullptr);       // no camera object, as the retail fixture's fresh MapPane
        ClearSeams();       // making the props asks their imagery
    }
    ~SBossScope()
    {
        MapPane.CenterOnObj(nullptr);       // the case's objects outlive the case (the world keeps them)
        TCharacter::paneObjectsSeam = nullptr;
        TCharacter::endFightingSeam = nullptr;
        TCharacter::tintSeam = nullptr;
        TPlayScreen::controlSeam = nullptr;
        TMapPane::newObjectSeam = nullptr;
        seamCreateAnimator = false;
        g_boss = SBossAnswers{};
    }
    SBossScope(const SBossScope&) = delete;
    SBossScope& operator=(const SBossScope&) = delete;

    // The dumps: each character's boss, the props, the camera.
    static void Write(JsonOut& j, const TFixtureWorld& world, const JsonValue& cs)
    {
        j.Key("boss").Begin('{');
        for (const JsonValue& spec : cs["chars"].Items())
        {
            j.Key(spec["name"].Str().c_str());
            WriteBoss(world, j, world.Fixture(world.Get(spec["name"].Str())));
        }
        j.End('}');
        j.Key("props").Begin('{');
        for (const auto& prop : g_boss.props)
        {
            j.Key(prop->Who().c_str()).Begin('{').Field("nowalk", (prop->GetFlags() & OF_NOWALK) ? 1 : 0);
            j.Field("state", (int32_t)(int16_t)prop->TObjectInstance::GetState()).End('}');
        }
        j.End('}');
        j.Key("camera");
        if (const TObjectInstance* camera = MapPane.GetCenterOnObj())
        {
            const auto* prop = dynamic_cast<const TFixtureProp*>(camera);
            j.Begin('{').FieldString("obj", prop ? prop->Who() : world.NameOf(camera));
            j.Field("flags", (int64_t)MapPane.CenterOnFlags()).End('}');
        }
        else
            j.Null();
    }
};

// ---- Dumps -------------------------------------------------------------------

// An attack record as "<owner>#<index>" in the case's characters' tables.
std::string AttackName(const TFixtureWorld& world, const JsonValue& cs, const SCharAttackData* ad)
{
    for (const JsonValue& spec : cs["chars"].Items())
    {
        SCharData* cd = world.Get(spec["name"].Str())->GetCharData();
        for (int32_t i = 0; i < cd->attacks.NumItems(); ++i)
            if (&cd->attacks[i] == ad)
                return spec["name"].Str() + "#" + std::to_string(i);
    }
    char buf[32];
    snprintf(buf, sizeof(buf), "%p", (const void*)ad);
    return buf;
}

// An attack's impact as "<owner>#<attack>/<impact>", a CHARIMPACT as
// "<owner>/<impact>"; the empty one Damage gives for the slot after a full
// list "past-end" (as the retail fixture names that slot).
std::string ImpactName(const TFixtureWorld& world, const JsonValue& cs, const SCharAttackImpact* imp)
{
    for (const JsonValue& spec : cs["chars"].Items())
    {
        SCharData* cd = world.Get(spec["name"].Str())->GetCharData();
        for (int32_t i = 0; i < cd->attacks.NumItems(); ++i)
        {
            const SCharAttackImpact* first = cd->attacks[i].impacts;
            if (imp >= first && imp < first + MAXATTACKIMPACTS)
                return spec["name"].Str() + "#" + std::to_string(i) + "/" + std::to_string(imp - first);
        }
        if (imp >= cd->impacts && imp < cd->impacts + MAXCHARIMPACTS)
            return spec["name"].Str() + "/" + std::to_string(imp - cd->impacts);
    }
    if (!imp->impactname[0] && !imp->flags && !imp->loopname[0] && !imp->snapdist)
        return "past-end";
    char buf[32];
    snprintf(buf, sizeof(buf), "%p", (const void*)imp);
    return buf;
}

void WriteBlockExtra(JsonOut& j, const TFixtureWorld& world, const JsonValue& cs, const TActionBlock& ab)
{
    j.Key("attack");
    if (ab.attack)
        j.String(AttackName(world, cs, ab.attack));
    else
        j.Null();
    j.Key("impact");
    if (ab.impact)
        j.String(ImpactName(world, cs, ab.impact));
    else
        j.Null();
    j.Field("damage", ab.damage).Field("tohit", ab.tohit).Field("roll", ab.roll);
}

struct SOuts
{
    int32_t attack = -1, impact = -1, damage = -1, tohit = -1, roll = -1;

    explicit SOuts(const JsonValue& v)
        : attack((int32_t)v["attack"].Int(-1)), impact((int32_t)v["impact"].Int(-1)),
          damage((int32_t)v["damage"].Int(-1)), tohit((int32_t)v["tohit"].Int(-1)), roll((int32_t)v["roll"].Int(-1))
    {
    }

    void Write(JsonOut& j, bool withAttack) const
    {
        j.Begin('{');
        if (withAttack)
            j.Field("attack", attack);
        j.Field("impact", impact).Field("damage", damage).Field("tohit", tohit).Field("roll", roll);
        j.End('}');
    }
};

TCharacter* Named(const TFixtureWorld& world, const JsonValue& v)
{
    return v.IsNull() ? nullptr : world.Get(v.Str());
}

// What a block of the case carries beyond the shared spec (melee_attack.py
// _block_fields): its attack (an index into the table of `chr`, or of the
// character `attack_of` names), impact (of that attack, or of attack
// `impact_attack` of `impact_of`), damage, to-hit and roll.
void ApplyBlockFields(const TFixtureWorld& world, TCharacter* chr, const JsonValue& b, TActionBlock* ab)
{
    auto record = [&](const JsonValue& owner, int64_t index) {
        TCharacter* c = owner.IsNull() ? chr : world.Get(owner.Str());
        return &c->GetCharData()->attacks[(int32_t)index];
    };
    if (b.Has("attack"))
        ab->attack = record(b["attack_of"], b["attack"].Int());
    if (b.Has("impact"))
    {
        const JsonValue& owner = b.Has("impact_of") ? b["impact_of"] : b["attack_of"];
        const int64_t index = b.Has("impact_attack") ? b["impact_attack"].Int() : b["attack"].Int();
        ab->impact = &record(owner, index)->impacts[(int32_t)b["impact"].Int()];
    }
    if (b.Has("damage"))
        ab->damage = (int32_t)b["damage"].Int();
    if (b.Has("tohit"))
        ab->tohit = (int32_t)b["tohit"].Int();
    if (b.Has("roll"))
        ab->roll = (int32_t)b["roll"].Int();
}

void SetBlockFields(const TFixtureWorld& world, const JsonValue& cs)
{
    for (const JsonValue& spec : cs["chars"].Items())
    {
        TCharacter* chr = world.Get(spec["name"].Str());
        IFixtureChar* fx = world.Fixture(chr);
        TActionBlock* blocks[] = {fx->Root(), fx->Doing(), fx->Desired()};
        const char* roles[] = {"root", "doing", "desired"};
        for (int32_t r = 0; r < 3; ++r)
            if (spec[roles[r]].GetKind() == JsonValue::Kind::Object)
                ApplyBlockFields(world, chr, spec[roles[r]], blocks[r]);
    }
}

// ---- The melee calls -----------------------------------------------------------

// Case (field 0, JSON): see slots/combat/melee_attack.py. `call`: "iva",
// "find-button", "find-pcnt", "find-interactive", "do-attack",
// "button-attack", "button-action", "random-attack", "specific-attack",
// "resolve-attack", "resolve-hit", "on-attacked", "damage", "resolve-impact",
// "resolve-block", "resolve-dead", "block", "stop-block", "dodge", "ai",
// "per-monster", "pulse", "kill-exp", "exp-kill", "exp-skill", "exp-stealth",
// "add-skill-exp".
std::string MeleeCall(const Case& c, std::string& error)
{
    try
    {
        if (!LoadGameData(error))
            return {};
        const JsonValue cs = JsonValue::Parse(c.Field(0));
        Melee::TMeleeWorld melee(cs);
        TFixtureWorld& world = melee.World();
        const std::string call = cs["call"].Str();
        std::optional<SBossScope> bossScope;
        if (call == "per-monster" || cs["bossworld"].Bool(false))
            bossScope.emplace(cs, world);
        TCharacter* me = world.Get(cs["self"].Str());
        const JsonValue& a = cs["args"];

        JsonOut j;
        j.Begin('{').FieldString("schema", "combat.melee.v1").FieldString("side", "port");
        if (call == "iva")
        {
            j.Key("calls").Begin('[');
            for (const JsonValue& v : cs["calls"].Items())
            {
                SOuts o(v);
                const bool ok = me->IsValidAttack((int32_t)v["attack"].Int(), o.impact, o.damage, o.tohit, o.roll,
                    (int32_t)v["tdist"].Int(10000), (int32_t)v["button"].Int(-1), (int32_t)v["pcnt"].Int(-1),
                    (int32_t)v["dmgpcnt"].Int(0), (int32_t)v["mask"].Int(0), (int32_t)v["flags"].Int(0),
                    Named(world, v["targ"]));
                j.Begin('{').Field("returned", ok ? 1 : 0).Field("impact", o.impact).Field("damage", o.damage);
                j.Field("tohit", o.tohit).Field("roll", o.roll).End('}');
            }
            j.End(']');
        }
        else if (call == "find-button" || call == "find-pcnt" || call == "find-interactive")
        {
            SOuts o(a);
            const int32_t dmgpcnt = (int32_t)a["dmgpcnt"].Int(0);
            bool ok;
            if (call == "find-button")
                ok = me->FindButtonAttack((int32_t)a["button"].Int(), dmgpcnt, o.attack, o.impact, o.damage, o.tohit,
                                          o.roll, a["isaction"].Bool(false), Named(world, a["targ"]));
            else if (call == "find-pcnt")
                ok = me->FindPcntAttack((int32_t)a["pcnt"].Int(), dmgpcnt, o.attack, o.impact, o.damage, o.tohit, o.roll);
            else
                ok = me->FindInteractiveAttack((int32_t)a["pcnt"].Int(), dmgpcnt, o.attack, o.impact, o.damage,
                                               o.tohit, o.roll);
            j.Field("returned", ok ? 1 : 0);
            j.Key("outs");
            o.Write(j, true);
        }
        else if (call == "do-attack")
            j.Field("returned", me->DoAttack((int32_t)a["attack"].Int(), (int32_t)a["impact"].Int(-1),
                (int32_t)a["damage"].Int(0), (int32_t)a["tohit"].Int(0), (int32_t)a["roll"].Int(0),
                Named(world, a["targ"])) ? 1 : 0);
        else if (call == "resolve-attack" || call == "resolve-impact" || call == "resolve-block" ||
                 call == "resolve-dead")
            j.Field("returned", world.Fixture(me)->RunResolver(call.substr(8), (int32_t)a["bits"].Int(0)));
        else if (call == "block")
            j.Field("returned", me->Block((int32_t)a["frames"].Int(-1)) ? 1 : 0);
        else if (call == "stop-block")
            j.Field("returned", me->StopBlock() ? 1 : 0);
        else if (call == "dodge")
            j.Field("returned", me->Dodge((int32_t)a["dir"].Int(-1)) ? 1 : 0);
        else if (call == "ai")
        {
            me->AI();
            j.Field("returned", 0);
        }
        else if (call == "per-monster")
        {
          // Tick k on game frame `frame` + k, then a Tick record.
            const JsonValue& g = cs["globals"];
            const int32_t frame = (int32_t)g["frame"].Int(0);
            IFixtureChar* fx = world.Fixture(me);
            for (int32_t k = 0; k < (int32_t)a["ticks"].Int(1); ++k)
            {
                PlayScreen.SetFixtureState(frame + k, g["control"].Bool(true), g["ps_5d8"].Bool(false));
                fx->RunPerMonster();
                JsonOut t;
                t.Begin('{').FieldString("seam", "Tick").Field("n", k).End('}');
                Seam(t.str());
            }
            j.Field("returned", 0);
        }
        else if (call == "resolve-hit")
        {
            SCharAttackData* ad = &me->GetCharData()->attacks[(int32_t)a["attack"].Int()];
            const int32_t k = (int32_t)a["impact"].Int(-1);
            j.Field("returned", me->ResolveHit(Named(world, a["targ"]), ad, k >= 0 ? &ad->impacts[k] : nullptr,
                (int32_t)a["damage"].Int(), (int32_t)a["tohit"].Int(), (int32_t)a["roll"].Int()) ? 1 : 0);
        }
        else if (call == "on-attacked")
        {
            me->SignalAttack(Named(world, a["attacker"]), Named(world, a["victim"]), (int32_t)a["flag"].Int(0));
            j.Field("returned", 0);
        }
        else if (call == "button-attack")
            j.Field("returned", me->ButtonAttack((int32_t)a["button"].Int()) ? 1 : 0);
        else if (call == "button-action")
            j.Field("returned", me->ButtonAction((int32_t)a["button"].Int()) ? 1 : 0);
        else if (call == "random-attack")
            j.Field("returned", me->RandomAttack((int32_t)a["pcnt"].Int()) ? 1 : 0);
        else if (call == "specific-attack")
            j.Field("returned", me->SpecificAttack((int32_t)a["attack"].Int()) ? 1 : 0);
        else if (call == "pulse")
        {
          // Tick k: game frame `frame` + k, game time `gametime` + k * 100 / 24,
          // screen frame `screenframe` + k; then the vitals.
            const JsonValue& g = cs["globals"];
            const int32_t frame = (int32_t)g["frame"].Int(0), gametime = (int32_t)g["gametime"].Int(0);
            const int32_t screenframe = (int32_t)g["screenframe"].Int(0);
            IFixtureChar* fx = world.Fixture(me);
            for (int32_t k = 0; k < (int32_t)a["ticks"].Int(1); ++k)
            {
                PlayScreen.SetFixtureState(frame + k, g["control"].Bool(true), g["ps_5d8"].Bool(false));
                PlayScreen.SetFixtureGameTime(gametime + k * 100 / 24);
                PlayScreen.SetFixtureScreenFrames(screenframe + k);
                me->Pulse();
                JsonOut t;
                t.Begin('{').FieldString("seam", "Tick").Field("n", k);
                for (const char* vital : {"health", "fatigue", "mana", "poisoned"})
                {
                    t.Key(vital);
                    if (const int32_t* v = fx->PeekStat(vital))
                        t.Value(*v);
                    else
                        t.Null();
                }
                t.End('}');
                Seam(t.str());
            }
            j.Field("returned", 0);
        }
        else if (call == "kill-exp")
            j.Field("returned", AsPlayer(me)->KillExp((int32_t)a["value"].Int()));
        else if (call == "exp-kill" || call == "exp-stealth" || call == "exp-skill" || call == "add-skill-exp")
        {
            TPlayer* player = AsPlayer(me);
            if (call == "exp-kill")
                player->AwardKillExp(Named(world, a["victim"]));
            else if (call == "exp-stealth")
                player->AwardStealthExp(Named(world, a["victim"]));
            else if (call == "exp-skill")
                player->AwardSkillExp((int32_t)a["skill"].Int(), Named(world, a["victim"]));
            else
                player->AddSkillExp((int32_t)a["skill"].Int(), (int32_t)a["exp"].Int());
            j.Field("returned", 0);
        }
        else if (call == "tag-sound")
        {
          // The sample each name plays, empty for none (retail 0x0040af50).
            j.Key("returned").Begin('[');
            for (const JsonValue& n : a["names"].Items())
            {
                std::string sample;
                j.String(ResolveTagSound(*me, n.Str().c_str(), sample) ? sample : std::string());
            }
            j.End(']');
        }
        else if (call == "damage")
        {
          // The given block's attack and impact are the attacker's by default.
            TActionBlock* ab = nullptr;
            if (a["block"].GetKind() == JsonValue::Kind::Object)
            {
                ab = world.NewBlock(a["block"]);
                ApplyBlockFields(world, Named(world, a["attacker"]), a["block"], ab);
            }
            me->Damage((int32_t)a["damage"].Int(), (int32_t)a["type"].Int(DT_NONE), (int32_t)a["mod"].Int(0), ab,
                       Named(world, a["attacker"]));
            j.Field("returned", 0);
        }
        else
            throw std::runtime_error("unknown call '" + call + "'");

        j.Key("chars").Begin('{');
        for (const JsonValue& spec : cs["chars"].Items())
            world.WriteCharacter(j, spec["name"].Str().c_str(), world.Get(spec["name"].Str()));
        j.End('}');
        melee.WriteEnd(j);
        if (bossScope)
            SBossScope::Write(j, world, cs);
        WriteSeams(j);
        WriteDraws(j);
        j.End('}');
        return j.str();
    }
    catch (const std::exception& e)
    {
        error = e.what();
        return {};
    }
}

}  // namespace

// ---- The world, for the targets that use it -------------------------------------

namespace Melee
{

// The scopes in the order they go on (and come off in reverse): the case's
// own seams and globals (SCaseScope), the melee seams and rules, the tick's.
struct TMeleeWorld::SScopes
{
    SScopes(const JsonValue& cs, const TFixtureWorld& world, bool ticking)
        : scope(cs, world), melee(cs, world), pulse(cs, ticking)
    {
    }

    SCaseScope scope;
    SMeleeScope melee;
    SPulseScope pulse;
};

TMeleeWorld::TMeleeWorld(const JsonValue& c) : cs(&c), world(std::make_unique<TFixtureWorld>(c))
{
    const std::string call = c["call"].Str();
    ticking = call == "pulse" || call == "scenario" || call == "kill-exp" || call == "exp-kill" ||
              call == "exp-skill" || call == "exp-stealth" || call == "add-skill-exp";
    SetBlockFields(*world, c);
    MakeHelpers(c);
    world->writeAttackState = true;
    world->blockExtra = [this](JsonOut& j, const TActionBlock& ab) { WriteBlockExtra(j, *world, *cs, ab); };
    scopes = std::make_unique<SScopes>(c, *world, ticking);
    for (const JsonValue& spec : c["chars"].Items())
        if (spec.Has("aistate"))
        {
            const JsonValue& a = spec["aistate"];
            auto named = [&](const JsonValue& n) { return n.IsNull() ? nullptr : HelperOrCharacter(*world, n.Str()); };
            world->Fixture(world->Get(spec["name"].Str()))->ReadAIState(a, named(a["lookat"]), named(a["lastwaypoint"]));
        }
}

// The scopes come off before the world goes (they hold seams into it).
TMeleeWorld::~TMeleeWorld()
{
    scopes.reset();
}

void TMeleeWorld::WriteEnd(JsonOut& j) const
{
    j.Key("ai").Begin('{');
    for (const JsonValue& spec : (*cs)["chars"].Items())
    {
        const IFixtureChar* fx = world->Fixture(world->Get(spec["name"].Str()));
        j.Key(spec["name"].Str().c_str()).Begin('{');
        for (const auto& [key, o] : {std::pair{"lookat", fx->AILookAt()}, std::pair{"lastwaypoint", fx->LastWaypoint()}})
        {
            j.Key(key);
            if (o)
                j.String(HelperName(o));
            else
                j.Null();
        }
        fx->WriteAIState(j);
        j.End('}');
    }
    j.End('}');
    if (!ticking)
        return;
    j.Key("pulse").Begin('{');
    for (const JsonValue& spec : (*cs)["chars"].Items())
        if (spec.Has("pulsestate"))
        {
            j.Key(spec["name"].Str().c_str());
            world->Fixture(world->Get(spec["name"].Str()))->WritePulseState(j);
        }
    j.End('}');
}

}  // namespace Melee

// Registered with the A/B driver by name (retailab.h).
static const bool registered = RegisterTarget("melee-attack-choice", MeleeCall) &&
                               RegisterTarget("melee-hit", MeleeCall) &&
                               RegisterTarget("melee-damage", MeleeCall) &&
                               RegisterTarget("melee-resolvers", MeleeCall) &&
                               RegisterTarget("melee-ai", MeleeCall) &&
                               RegisterTarget("melee-boss", MeleeCall) &&
                               RegisterTarget("melee-pulse", MeleeCall) &&
                               RegisterTarget("melee-wander", MeleeCall) &&
                               RegisterTarget("melee-exp", MeleeCall) &&
                               RegisterTarget("melee-tagsound", MeleeCall);

}  // namespace RetailAB
