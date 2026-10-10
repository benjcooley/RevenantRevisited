// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *   retailab_melee.cpp - the port's side of the melee A/B dumps         *
// *************************************************************************
//
// The combat dojo's melee katas (docs/gameplay/COMBAT_DOJO.md C2-C5): attack
// choice, hit resolution, Damage, the resolvers. Mirrors
// tools/retail_runtime/slots/combat/melee_attack.py: the same case (one JSON
// object, field 0 of the case line), the same seams answered from it, the
// same JSON out (schema `combat.melee.v1`).

#include "retailab_fixture.h"

#include "gameoptions.h"          // NoCombatResults
#include "logging.h"
#include "revenant.h"             // CheatNahkranoth
#include "sound.h"

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
        TCharacter::findCharactersSeam = CaseFound;
        TCharacter::blockedSeam = CaseBlocked;
        TCharacter::blockSeam = CaseBlock;
        TCharacter::castSeam = CaseCast;
        TCharacter::isEnemySeam = CaseIsEnemy;
        TCharacter::beginFightingSeam = CaseBeginFighting;
        TCharacter::damageSeam = cs["seam_damage"].Bool(true) ? CaseDamage : nullptr;
        TCharacter::effectBurstSeam = CaseEffectBurst;
        TSoundPlayer::findSeam = CaseSound;
        TPlayer::killExpSeam = CaseKillExp;
        TPlayer::awardSkillExpSeam = CaseSkillExp;
        TPlayer::stealthExpSeam = CaseStealthExp;
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
        TSoundPlayer::findSeam = nullptr;
        TPlayer::killExpSeam = nullptr;
        TPlayer::awardSkillExpSeam = nullptr;
        TPlayer::stealthExpSeam = nullptr;
        CheatNahkranoth = false;
        NoCombatResults = false;
        g_answers = SMeleeAnswers{};
    }
    SMeleeScope(const SMeleeScope&) = delete;
    SMeleeScope& operator=(const SMeleeScope&) = delete;
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

// An attack's impact as "<owner>#<attack>/<impact>".
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
    }
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
// _block_fields): its attack (an index into the owner's table, `attack_of`
// naming another owner), impact, damage, to-hit and roll.
void SetBlockFields(const TFixtureWorld& world, const JsonValue& cs)
{
    for (const JsonValue& spec : cs["chars"].Items())
    {
        TCharacter* chr = world.Get(spec["name"].Str());
        IFixtureChar* fx = world.Fixture(chr);
        TActionBlock* blocks[] = {fx->Root(), fx->Doing(), fx->Desired()};
        const char* roles[] = {"root", "doing", "desired"};
        for (int32_t r = 0; r < 3; ++r)
        {
            const JsonValue& b = spec[roles[r]];
            if (b.GetKind() != JsonValue::Kind::Object)
                continue;
            TActionBlock* ab = blocks[r];
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
    }
}

// ---- The melee calls -----------------------------------------------------------

// Case (field 0, JSON): see slots/combat/melee_attack.py. `call`: "iva",
// "find-button", "find-pcnt", "find-interactive", "do-attack",
// "button-attack", "button-action", "random-attack", "specific-attack".
std::string MeleeCall(const Case& c, std::string& error)
{
    try
    {
        if (!LoadGameData(error))
            return {};
        const JsonValue cs = JsonValue::Parse(c.Field(0));
        TFixtureWorld world(cs);
        SetBlockFields(world, cs);
        world.writeAttackState = true;
        world.blockExtra = [&](JsonOut& j, const TActionBlock& ab) { WriteBlockExtra(j, world, cs, ab); };
        SCaseScope scope(cs, world);
        SMeleeScope melee(cs, world);
        TCharacter* me = world.Get(cs["self"].Str());
        const std::string call = cs["call"].Str();
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
        else if (call == "resolve-attack")
            j.Field("returned", world.Fixture(me)->RunResolver("attack", (int32_t)a["bits"].Int(0)));
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
        else
            throw std::runtime_error("unknown call '" + call + "'");

        j.Key("chars").Begin('{');
        for (const JsonValue& spec : cs["chars"].Items())
            world.WriteCharacter(j, spec["name"].Str().c_str(), world.Get(spec["name"].Str()));
        j.End('}');
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

// Registered with the A/B driver by name (retailab.h).
static const bool registered = RegisterTarget("melee-attack-choice", MeleeCall) &&
                               RegisterTarget("melee-hit", MeleeCall);

}  // namespace RetailAB
