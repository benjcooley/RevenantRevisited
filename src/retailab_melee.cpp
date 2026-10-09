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

#include "revenant.h"             // CheatNahkranoth

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
};
SMeleeAnswers g_answers;

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
        TCharacter::findCharactersSeam = CaseFound;
        TCharacter::blockedSeam = CaseBlocked;
        TCharacter::blockSeam = CaseBlock;
        TCharacter::castSeam = CaseCast;
    }
    ~SMeleeScope()
    {
        TCharacter::blockSeam = nullptr;
        TCharacter::castSeam = nullptr;
        CheatNahkranoth = false;
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
static const bool registered = RegisterTarget("melee-attack-choice", MeleeCall);

}  // namespace RetailAB
