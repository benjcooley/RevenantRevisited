// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *   retailab_spells.cpp - the port's side of the spell A/B dumps        *
// *************************************************************************
//
// The combat dojo's spell and missile targets (docs/gameplay/COMBAT_DOJO.md
// §2 S1-S4, D1s; docs/gameplay/forensics/SPELLS_MISSILES.md). Each mirrors a
// retail fixture in the emulator's combat slot (tools/retail_runtime/slots/
// combat/spell_*.py): the same case (field 0 of the case line, JSON), the
// same seams answered from it, the same JSON out.

#include "retailab.h"
#include "retailab_fixture.h"
#include "retailab_json.h"

#include "logging.h"
#include "player.h"
#include "revenant.h"
#include "revutils.h"
#include "sector.h"
#include "spell.h"
#include "textencoding.h"

#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>

namespace RetailAB
{
namespace
{

using namespace Fixture;

// ---- The spell list by field name (slots/combat/spell_guest.py) -------------

void Text(JsonOut& j, const char* key, const char* text)
{
    j.FieldString(key, ToUtf8(text));
}

void Ints(JsonOut& j, const char* key, const int32_t* values, size_t count)
{
    j.Key(key).Begin('[');
    for (size_t i = 0; i < count; i++)
        j.Value(values[i]);
    j.End(']');
}

void WriteControlData(JsonOut& j, const SSpellControlData& cd)
{
    j.Begin('{');
    Text(j, "name", cd.name);
    j.Field("flags", cd.flags).Field("radius", cd.radius).Field("hits", cd.hits).Field("duration", cd.duration);
    j.Field("wait", cd.wait).Field("duration2", cd.duration2).Field("pattern", cd.pattern).Field("shake", cd.shake);
    j.Field("unknown40", cd.unknown40).Field("repeatdamage", cd.repeatdamage).Field("hittarget", cd.hittarget);
    j.Field("oncaster", cd.oncaster).Field("attachset", cd.attachset).Field("follow", cd.follow);
    j.Field("posset", cd.posset).Field("repeatset", cd.repeatset).Field("multipletargets", cd.multipletargets);
    Ints(j, "rangedamage", cd.rangedamage, 2);
    Text(j, "attach", cd.attach);
    j.Key("attachmultiple");
    if (cd.attachmultiple)
    {
        j.Begin('[');
        for (int32_t i = 0; i < cd.hits; i++)
            j.String(ToUtf8(cd.attachmultiple + i * RESNAMELEN));
        j.End(']');
    }
    else
        j.Null();
    Text(j, "sound", cd.sound);
    j.Key("imagery");
    if (cd.imageryname[0])
        j.String(ToUtf8(cd.imageryname));
    else
        j.Null();
    Ints(j, "pos", cd.pos, 3);
    j.End('}');
}

void WriteVariant(JsonOut& j, const SSpellVariant& v)
{
    j.Begin('{');
    Text(j, "name", v.name);
    Text(j, "talismans", v.talismans);
    Text(j, "effect", v.effect);
    j.Field("flags", v.flags).Field("type", v.type).Field("mana", v.mana).Field("wait", v.nextspellwait);
    j.Field("min", v.mindamage).Field("max", v.maxdamage).Field("skill", v.skilllevel);
    j.Field("height", v.height).Field("facing", v.facing).Field("ani_delay", v.ani_delay);
    j.Key("statline");
    if (v.statline)
        j.String(ToUtf8(v.statline));
    else
        j.Null();
    j.Key("controldata");
    if (v.controldata)
        WriteControlData(j, *v.controldata);
    else
        j.Null();
    j.End('}');
}

void WriteSpell(JsonOut& j, SSpellData& sd)
{
    j.Begin('{');
    Text(j, "name", sd.name);
    Text(j, "objname", sd.objname);
    Text(j, "iconname", sd.iconname);
    j.Key("description");
    if (sd.desc)
        j.String(ToUtf8(sd.desc));
    else
        j.Null();
    j.Field("flags", sd.flags).Field("damagetype", sd.damagetype);
    Text(j, "invoke", sd.invoke);
    j.Field("delay", sd.effectstart).Field("poisonchance", sd.poisonchance);
    j.Key("light").Begin('{');
    j.Key("color").Begin('[').Value(sd.light.color[0]).Value(sd.light.color[1]).Value(sd.light.color[2]).End(']');
    j.Field("mult", sd.light.mult).Field("intensity", sd.light.intensity);
    Ints(j, "pos", sd.light.pos, 3);
    j.Field("fadein", sd.light.fadein).Field("fadeout", sd.light.fadeout);
    j.End('}');
    j.Key("variants").Begin('[');
    for (int32_t i = 0; i < sd.variants.NumItems(); i++)
        WriteVariant(j, sd.variants[i]);
    j.End(']');
    j.End('}');
}

// ---- D1s: spell.def as the port parses it ----------------------------------

// Case (field 0, JSON): {"dir": <a folder with Resources/spell.def>}. The
// spell list's search path points there for the case; TSpellList::Load
// reads it the way it reads the install's.
std::string SpellData(const Case& c, std::string& error)
{
    try
    {
        if (!LoadGameData(error))
            return {};
        const JsonValue cs = JsonValue::Parse(c.Field(0));
        char saved[MAXPATHLEN];
        strncpyz(saved, ClassDefPath, MAXPATHLEN);
        strncpyz(ClassDefPath, (cs["dir"].Str() + "/Resources/").c_str(), MAXPATHLEN);
        SpellList.Close();
        const bool loaded = SpellList.Initialize();
        strncpyz(ClassDefPath, saved, MAXPATHLEN);

        JsonOut j;
        j.Begin('{').FieldString("schema", "combat.spelldata.v1").FieldString("side", "port");
        j.Key("fatal").Null().Field("loaded", loaded ? 1 : 0);
        j.Key("spells").Begin('[');
        for (int32_t i = 0; i < SpellList.NumSpells(); i++)
            WriteSpell(j, *SpellList.GetSpellData(i));
        j.End(']');
        j.End('}');
        return j.str();
    }
    catch (const std::exception& e)
    {
        error = e.what();
        return {};
    }
}

// ---- The fixture world's spell pieces (slots/combat/spell_guest.py SpellWorld) ----

// The text bar, as a seam: TTextBar logs each message as "[textbar] <text>";
// while a case runs, each becomes a {"seam": "TextBar"} record in order.
bool g_recordTextBar = false;

void CaptureTextBar(log_Event* ev)
{
    if (!g_recordTextBar || ev->level != LOG_DEBUG || !ev->fmt || strncmp(ev->fmt, "[textbar] ", 10) != 0)
        return;
    char buf[2048];
    vsnprintf(buf, sizeof(buf), ev->fmt, ev->ap);
    JsonOut j;
    j.Begin('{').FieldString("seam", "TextBar").FieldString("text", buf + 10).End('}');
    Seam(j.str());
}

// The cast entry points as a seam (retail's TCharacter::CastByTalismans /
// CastByName boundaries): recorded, answered from the case's `casts` in order
// (1 when they run out).
std::vector<int32_t> g_casts;
const TFixtureWorld* g_spellWorld = nullptr;

bool RecordCast(const char* kind, TCharacter* self, const char* text, TObjectInstance** targets, int32_t numtargs,
                const S3DPoint* sourcepos)
{
    int32_t result = 1;
    if (!g_casts.empty())
    {
        result = g_casts.front();
        g_casts.erase(g_casts.begin());
    }
    JsonOut j;
    j.Begin('{').FieldString("seam", kind).FieldString("who", g_spellWorld->NameOf(self));
    j.FieldString("text", ToUtf8(text)).Key("targets");
    if (targets)
    {
        j.Begin('[');
        for (int32_t i = 0; i < (numtargs > 1 ? numtargs : 1); i++)
            j.String(g_spellWorld->NameOf(targets[i]));
        j.End(']');
    }
    else
        j.Null();
    j.Field("numtargs", numtargs).Key("source");
    if (sourcepos)
        j.Begin('[').Value(sourcepos->x).Value(sourcepos->y).Value(sourcepos->z).End(']');
    else
        j.Null();
    j.Field("result", result).End('}');
    Seam(j.str());
    return result != 0;
}

bool CaseCastByName(TCharacter* self, const char* name, TObjectInstance** targets, int32_t numtargs,
                    const S3DPoint* sourcepos)
{
    return RecordCast("CastByName", self, name, targets, numtargs, sourcepos);
}

bool CaseCastByTalismans(TCharacter* self, const char* talismans, TObjectInstance** targets, int32_t numtargs,
                         const S3DPoint* sourcepos)
{
    return RecordCast("CastByTalismans", self, talismans, targets, numtargs, sourcepos);
}

// The player's experience, stat effects and state (retail's seams at
// AddSkillExp 0x0051ac90, AddStatEffect 0x0051c2c0, SetPlayerState
// 0x0051d680): recorded.
void CaseSkillExp(TPlayer* self, int32_t skill, int32_t exp)
{
    JsonOut j;
    j.Begin('{').FieldString("seam", "AddSkillExp").FieldString("who", g_spellWorld->NameOf(self));
    j.Field("skill", skill).Field("exp", exp).End('}');
    Seam(j.str());
}

void CaseStatEffect(TPlayer* self, const char* line)
{
    JsonOut j;
    j.Begin('{').FieldString("seam", "AddStatEffect").FieldString("who", g_spellWorld->NameOf(self)).Key("line");
    if (line)
        j.String(ToUtf8(line));
    else
        j.Null();
    j.End('}');
    Seam(j.str());
}

void CasePlayerState(TPlayer* self, int32_t value)
{
    JsonOut j;
    j.Begin('{').FieldString("seam", "SetPlayerState").FieldString("who", g_spellWorld->NameOf(self));
    j.Field("value", value).End('}');
    Seam(j.str());
}

// The objects around a caster a buff cast walks (retail's map iterator over
// OBJSET_ANIMATE, 5): the case's `nearby`, recorded.
std::vector<std::string> g_nearby;

std::vector<TObjectInstance*> CaseNearby(TObjectInstance* center)
{
    std::vector<TObjectInstance*> around;
    JsonOut j;
    j.Begin('{').FieldString("seam", "MapIterator").FieldString("who", g_spellWorld->NameOf(center));
    j.Field("objset", OBJSET_ANIMATE).Key("result").Begin('[');
    for (const std::string& n : g_nearby)
    {
        around.push_back(g_spellWorld->Get(n));
        j.String(n);
    }
    j.End(']').End('}');
    Seam(j.str());
    return around;
}

// What a spell case adds to the world and its scope: the player as the main
// player (retail's 0x00667fcc; `mainplayer: false` for none), the editor flag
// and the magic cheat (`globals`), the seams above -- the casts' only when
// `seamCasts` (kata S1; S2 runs them). Put back when the case ends.
class SSpellScope
{
  public:
    SSpellScope(const JsonValue& cs, const TFixtureWorld& world, bool seamCasts)
    {
        static const bool installed = (log_add_callback(CaptureTextBar, nullptr, LOG_DEBUG), true);
        (void)installed;
        savedPlayer = Player;
        Player = nullptr;
        if (cs["mainplayer"].Bool(true))
            for (const JsonValue& spec : cs["chars"].Items())
                if (spec["class"].Int(OBJCLASS_CHARACTER) == OBJCLASS_PLAYER)
                    Player = static_cast<TPlayer*>(world.Get(spec["name"].Str()));
        g_casts.clear();
        for (const JsonValue& v : cs["casts"].Items())
            g_casts.push_back((int32_t)v.Int());
        g_spellWorld = &world;
        if (seamCasts)
        {
            TCharacter::castSeam = CaseCastByName;
            TCharacter::castByTalismansSeam = CaseCastByTalismans;
        }
        g_nearby.clear();
        for (const JsonValue& v : cs["nearby"].Items())
            g_nearby.push_back(v.Str());
        TPlayer::skillExpSeam = CaseSkillExp;
        TPlayer::statEffectSeam = CaseStatEffect;
        TPlayer::playerStateSeam = CasePlayerState;
        TSpellManager::nearbySeam = CaseNearby;
        savedEditor = Editor;
        Editor = cs["globals"]["editor"].Bool(false);
        MagicCheat = cs["globals"]["cheat"].Bool(false);
        g_recordTextBar = true;
    }
    ~SSpellScope()
    {
        g_recordTextBar = false;
        MagicCheat = false;
        Editor = savedEditor;
        TSpellManager::nearbySeam = nullptr;
        TPlayer::playerStateSeam = nullptr;
        TPlayer::statEffectSeam = nullptr;
        TPlayer::skillExpSeam = nullptr;
        TCharacter::castSeam = nullptr;
        TCharacter::castByTalismansSeam = nullptr;
        g_spellWorld = nullptr;
        Player = savedPlayer;
    }
    SSpellScope(const SSpellScope&) = delete;
    SSpellScope& operator=(const SSpellScope&) = delete;

  private:
    TPlayer* savedPlayer = nullptr;
    bool savedEditor = false;
};

// An inventory from the case: [{"name", "class" (0), "type" (the name),
// "items": [...]}, ...] or names, placed in order (slot = index).
void BuildInventory(TObjectInstance* owner, const JsonValue& items)
{
    int32_t slot = 0;
    for (const JsonValue& spec : items.Items())
    {
        const bool plain = spec.GetKind() == JsonValue::Kind::String;
        const std::string name = plain ? spec.Str() : spec["name"].Str();
        const std::string type = (!plain && spec.Has("type")) ? spec["type"].Str() : name;
        SObjectDef def{};
        def.objclass = (short)(plain ? 0 : spec["class"].Int(0));
        TObjectClass* cl = TObjectClass::GetClass(def.objclass);
        if (!cl)
            throw std::runtime_error("no class " + std::to_string(def.objclass) + " loaded");
        def.objtype = (short)cl->FindObjType(type.c_str());
        if (def.objtype < 0)
            throw std::runtime_error("class.def has no type '" + type + "' in class " + std::to_string(def.objclass));
        // Imagery with no states, as the fixture's characters have: nothing draws.
        auto* item = new TObjectInstance(&def, new TFixtureImagery(TFixtureImagery::Register({}), name));
        item->SetName(const_cast<char*>(name.c_str()));
        owner->PlaceInInventory(item, slot++);
        if (!plain && spec.Has("items"))
            BuildInventory(item, spec["items"]);
    }
}

// The case's per-character spell pieces: inventory, quick spells, and the
// doing block's attack / impact records (`doing_attack` / `doing_impact`:
// {"flags": n}, retail's CA_ / CAI_ bits).
void BuildSpellPieces(const JsonValue& cs, const TFixtureWorld& world)
{
    for (const JsonValue& spec : cs["chars"].Items())
    {
        TCharacter* chr = world.Get(spec["name"].Str());
        if (spec.Has("inventory"))
            BuildInventory(chr, spec["inventory"]);
        if (spec.Has("quickspells"))
        {
            auto* player = static_cast<TPlayer*>(chr);
            const auto& q = spec["quickspells"].Items();
            for (size_t i = 0; i < q.size(); i++)
                player->SetQuickSpell((int32_t)i, const_cast<char*>(q[i].Str().c_str()));
        }
        IFixtureChar* fx = world.Fixture(chr);
        if (spec.Has("doing_attack") && !spec["doing_attack"].IsNull())
        {
            auto* attack = new SCharAttackData();
            attack->flags = (int32_t)spec["doing_attack"]["flags"].Int();
            fx->Doing()->attack = attack;
        }
        if (spec.Has("doing_impact") && !spec["doing_impact"].IsNull())
        {
            auto* impact = new SCharAttackImpact();
            impact->flags = (int32_t)spec["doing_impact"]["flags"].Int();
            fx->Doing()->impact = impact;
        }
    }
}

// The spell list loaded once from the install (the packs first).
bool LoadSpells(std::string& error)
{
    if (!LoadGameData(error))
        return false;
    if (!SpellList.Initialize())
    {
        error = "can't load spell.def";
        return false;
    }
    return true;
}

// A variant named as the retail dump names it: [spell, variant].
void WriteVariantRef(JsonOut& j, const char* key, const SSpellVariant* v)
{
    j.Key(key);
    for (int32_t i = 0; v && i < SpellList.NumSpells(); i++)
    {
        SSpellData* sd = SpellList.GetSpellData(i);
        for (int32_t k = 0; k < sd->variants.NumItems(); k++)
            if (&sd->variants[k] == v)
            {
                j.Begin('[').String(ToUtf8(sd->name)).String(ToUtf8(v->name)).End(']');
                return;
            }
    }
    j.Null();
}

// ---- S1: talismans to spell (slots/combat/spell_talismans.py) -----------------

// Case (field 0, JSON): {"call": "lookup", "queries"} | {"call":
// "has-talismans" | "quick-spell", "chars", "self", ...}; see
// spell_talismans.py.
std::string SpellTalismans(const Case& c, std::string& error)
{
    try
    {
        if (!LoadSpells(error))
            return {};
        const JsonValue cs = JsonValue::Parse(c.Field(0));
        const std::string call = cs["call"].Str();
        JsonOut j;
        j.Begin('{').FieldString("schema", "combat.spell.v1").FieldString("side", "port");
        if (call == "lookup")
        {
            j.Key("returned").Begin('[');
            for (const JsonValue& q : cs["queries"].Items())
            {
                const char* text = q["q"].Str().c_str();
                const bool bytalismans = q["by"].Str() == "talismans";
                SSpellData* sd = bytalismans ? SpellList.GetSpellDataByTalismans(text)
                                             : SpellList.GetSpellDataByName(text);
                SSpellVariant* v = bytalismans ? SpellList.GetVariantDataByTalismans(text)
                                               : SpellList.GetVariantDataByName(text);
                j.Begin('{').Key("spell");
                if (sd)
                    j.String(ToUtf8(sd->name));
                else
                    j.Null();
                WriteVariantRef(j, "variant", v);
                j.End('}');
            }
            j.End(']').End('}');
            return j.str();
        }

        TFixtureWorld world(cs);
        BuildSpellPieces(cs, world);
        SCaseScope scope(cs, world);
        SSpellScope spells(cs, world, true);
        TCharacter* me = world.Get(cs["self"].Str());
        if (call == "has-talismans")
        {
            auto* player = static_cast<TPlayer*>(me);
            j.Key("returned").Begin('[');
            for (const JsonValue& q : cs["queries"].Items())
                j.Value(player->HasTalismans(const_cast<char*>(q.Str().c_str())) ? 1 : 0);
            j.End(']');
        }
        else if (call == "quick-spell")
            j.Field("returned", static_cast<TPlayer*>(me)->InvokeQuickSpell((int32_t)cs["button"].Int()) ? 1 : 0);
        else
            throw std::runtime_error("unknown call '" + call + "'");
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

// ---- S2: the cast (slots/combat/spell_cast.py) -------------------------------

// A spell as the retail fixture dumps it (TSpell's layout by name).
void WriteSpellState(JsonOut& j, const TFixtureWorld& world, TSpell* sp)
{
    auto name = [&](TObjectInstance* o) {
        if (o)
            j.String(world.NameOf(o));
        else
            j.Null();
    };
    j.Begin('{').FieldString("class", sp->ClassName()).Key("invoker");
    name(sp->GetInvoker());
    j.Key("targets").Begin('[');
    for (int32_t i = 0; i < sp->GetTargetNum(); i++)
        name(sp->GetTarget(i));
    j.End(']');
    j.Field("timer", sp->TimerValue()).Field("frame", sp->FrameValue()).Key("master");
    if (sp->Master())
        j.String("master");
    else
        j.Null();
    j.Key("spell");
    if (sp->SpellData())
        j.String(ToUtf8(sp->SpellData()->name));
    else
        j.Null();
    WriteVariantRef(j, "variant", sp->VariantData());
    j.Field("wait", sp->WaitValue());
    const S3DPoint& src = sp->Source();
    j.Key("source").Begin('[').Value(src.x).Value(src.y).Value(src.z).End(']');
    j.Field("defense", sp->GetDefense()).Field("offense", sp->GetOffense());
    if (auto* strike = dynamic_cast<TStrikeSpell*>(sp))
    {
        j.Key("striketarget");
        name(strike->StrikeTarget());
    }
    j.End('}');
}

// What a cast changes on a character besides its blocks: the buff flag
// among the charflags, the invoke delay, the cooldown and the spells.
void WriteCaster(JsonOut& j, const TFixtureWorld& world, TCharacter* c)
{
    TSpellManager* m = c->GetSpellManager();
    j.Begin('{').Field("charflags", c->CharFlags()).Field("invokedelay", c->InvokeDelay());
    j.Field("wait", m->Wait()).Key("spells").Begin('[');
    for (int32_t i = 0; i < m->NumSpells(); i++)
        if (TSpell* sp = m->GetSpell(i))
            WriteSpellState(j, world, sp);
    j.End(']').End('}');
}

// Case (field 0, JSON): {"call", "self", "text", "targets", "numtargs",
// "sourcepos", "invoker", "chars", "globals", "nearby", ...}; see
// spell_cast.py.
std::string SpellCast(const Case& c, std::string& error)
{
    try
    {
        if (!LoadSpells(error))
            return {};
        const JsonValue cs = JsonValue::Parse(c.Field(0));
        TFixtureWorld world(cs);
        BuildSpellPieces(cs, world);
        for (const JsonValue& spec : cs["chars"].Items())
            world.Get(spec["name"].Str())->GetSpellManager()->SetWait((int32_t)spec["spellwait"].Int(0));
        SCaseScope scope(cs, world);
        SSpellScope spells(cs, world, false);
        TCharacter* me = world.Get(cs["self"].Str());

        // The targets: an array of the names (one null slot for an empty
        // list, as the retail fixture allocates), or none.
        std::vector<TObjectInstance*> targets;
        const bool hasTargets = cs.Has("targets") && !cs["targets"].IsNull();
        if (hasTargets)
            for (const JsonValue& t : cs["targets"].Items())
                targets.push_back(t.IsNull() ? nullptr : world.Get(t.Str()));
        if (hasTargets && targets.empty())
            targets.push_back(nullptr);
        const int32_t numtargs =
            (int32_t)cs["numtargs"].Int(hasTargets ? (int64_t)cs["targets"].Items().size() : 0);
        TObjectInstance** array = hasTargets ? targets.data() : nullptr;
        S3DPoint source;
        S3DPoint* sourcepos = nullptr;
        if (cs.Has("sourcepos") && !cs["sourcepos"].IsNull())
        {
            const JsonValue& p = cs["sourcepos"];
            source = S3DPoint((int32_t)p[0].Int(), (int32_t)p[1].Int(), (int32_t)p[2].Int());
            sourcepos = &source;
        }
        std::string text = cs["text"].Str();
        const std::string call = cs["call"].Str();
        int32_t returned = 0;
        if (call == "cast-talismans")
            returned = me->CastByTalismans(text.data(), array, numtargs, sourcepos) ? 1 : 0;
        else if (call == "cast-name")
            returned = me->CastByName(text.data(), array, numtargs, sourcepos) ? 1 : 0;
        else if (call == "cast")
            returned = me->Cast(text.data(), sourcepos) ? 1 : 0;
        else if (call == "manager-cast-name" || call == "manager-cast-talismans")
        {
            TObjectInstance* invoker = cs["invoker"].IsNull() ? nullptr : world.Get(cs["invoker"].Str());
            TSpellManager* m = me->GetSpellManager();
            const bool ok = call == "manager-cast-name"
                                ? m->CastByName(text.data(), invoker, array, numtargs, sourcepos)
                                : m->CastByTalismans(text.data(), invoker, array, numtargs, sourcepos);
            returned = ok ? 1 : 0;
        }
        else
            throw std::runtime_error("unknown call '" + call + "'");

        JsonOut j;
        j.Begin('{').FieldString("schema", "combat.spell.v1").FieldString("side", "port");
        j.Field("returned", returned);
        j.Key("chars").Begin('{');
        for (const JsonValue& spec : cs["chars"].Items())
            world.WriteCharacter(j, spec["name"].Str().c_str(), world.Get(spec["name"].Str()));
        j.End('}');
        j.Key("casters").Begin('{');
        for (const JsonValue& spec : cs["chars"].Items())
        {
            j.Key(spec["name"].Str().c_str());
            WriteCaster(j, world, world.Get(spec["name"].Str()));
        }
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

// ---- S3: a spell's construction and damage (slots/combat/spell_damage.py) -----

// TCharacter::Damage as a seam (retail 0x004c4950), recorded as the melee
// kata records it; a spell or an arrow passes no action block.
void CaseDamage(TCharacter* self, int32_t damage, int32_t damagetype, int32_t modifier, TActionBlock* action,
                TCharacter* attacker)
{
    if (action)
        throw std::runtime_error("Damage with an action block: not modelled here");
    JsonOut j;
    j.Begin('{').FieldString("seam", "Damage").FieldString("who", g_spellWorld->NameOf(self));
    j.Field("damage", damage).Field("type", damagetype).Field("mod", modifier).Key("attacker");
    if (attacker)
        j.String(g_spellWorld->NameOf(attacker));
    else
        j.Null();
    j.Key("block").Null().End('}');
    Seam(j.str());
}

void CaseKillExp(TPlayer* self, TCharacter* victim)
{
    JsonOut j;
    j.Begin('{').FieldString("seam", "AwardKillExp").FieldString("who", g_spellWorld->NameOf(self)).Key("victim");
    if (victim)
        j.String(g_spellWorld->NameOf(victim));
    else
        j.Null();
    j.End('}');
    Seam(j.str());
}

// The case's variant, [spell, variant] by name: the first of that pair.
std::pair<SSpellData*, SSpellVariant*> FindVariant(const JsonValue& names)
{
    for (int32_t i = 0; i < SpellList.NumSpells(); i++)
    {
        SSpellData* sd = SpellList.GetSpellData(i);
        if (names[0].Str() != sd->name)
            continue;
        for (int32_t k = 0; k < sd->variants.NumItems(); k++)
            if (names[1].Str() == sd->variants[k].name)
                return {sd, &sd->variants[k]};
    }
    throw std::runtime_error("no variant " + names[0].Str() + " / " + names[1].Str());
}

// Case (field 0, JSON): {"call": "spell-new" | "spell-damage", "class",
// "variant", "invoker", "targets", "numtargs", "sourcepos", "target",
// "chars", ...}; see spell_damage.py.
std::string SpellDamage(const Case& c, std::string& error)
{
    try
    {
        if (!LoadSpells(error))
            return {};
        const JsonValue cs = JsonValue::Parse(c.Field(0));
        TFixtureWorld world(cs);
        BuildSpellPieces(cs, world);
        for (const JsonValue& spec : cs["chars"].Items())
            if (spec.Has("magicresist"))     // per mille
                world.Get(spec["name"].Str())->SetMagicResistance((float)(spec["magicresist"].Int() / 1000.0));
        SCaseScope scope(cs, world);
        SSpellScope spells(cs, world, false);
        TCharacter::damageSeam = CaseDamage;
        TPlayer::killExpSeam = CaseKillExp;
        struct SDamageSeamsOff
        {
            ~SDamageSeamsOff()
            {
                TCharacter::damageSeam = nullptr;
                TPlayer::killExpSeam = nullptr;
            }
        } off;

        std::vector<TObjectInstance*> targets;
        const bool hasTargets = cs.Has("targets") && !cs["targets"].IsNull();
        if (hasTargets)
            for (const JsonValue& t : cs["targets"].Items())
                targets.push_back(t.IsNull() ? nullptr : world.Get(t.Str()));
        if (hasTargets && targets.empty())
            targets.push_back(nullptr);
        const int32_t numtargs =
            (int32_t)cs["numtargs"].Int(hasTargets ? (int64_t)cs["targets"].Items().size() : 0);
        S3DPoint source;
        S3DPoint* sourcepos = nullptr;
        if (cs.Has("sourcepos") && !cs["sourcepos"].IsNull())
        {
            const JsonValue& p = cs["sourcepos"];
            source = S3DPoint((int32_t)p[0].Int(), (int32_t)p[1].Int(), (int32_t)p[2].Int());
            sourcepos = &source;
        }
        auto [sd, variant] = FindVariant(cs["variant"]);
        TObjectInstance* invoker = cs["invoker"].IsNull() ? nullptr : world.Get(cs["invoker"].Str());
        const SSpellClass* cls = FindSpellClass(cs.Has("class") ? cs["class"].Str().c_str() : "Spell");
        if (!cls)
            throw std::runtime_error("no spell class " + cs["class"].Str());
        std::unique_ptr<TSpell> spell(cls->create(invoker, hasTargets ? targets.data() : nullptr, numtargs, sourcepos,
                                                  sd, variant, nullptr));

        const std::string call = cs["call"].Str();
        if (call == "spell-damage")
        {
            ClearSeams();
            ClearDraws();
            spell->Damage(cs["target"].IsNull() ? nullptr : world.Get(cs["target"].Str()));
        }
        else if (call != "spell-new")
            throw std::runtime_error("unknown call '" + call + "'");

        JsonOut j;
        j.Begin('{').FieldString("schema", "combat.spell.v1").FieldString("side", "port");
        j.Key("spell");
        WriteSpellState(j, world, spell.get());
        j.Key("casters").Begin('{');
        for (const JsonValue& spec : cs["chars"].Items())
        {
            j.Key(spec["name"].Str().c_str());
            WriteCaster(j, world, world.Get(spec["name"].Str()));
        }
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
static const bool registered = RegisterTarget("spell-data", SpellData) &&
                               RegisterTarget("spell-lookup", SpellTalismans) &&
                               RegisterTarget("spell-talismans", SpellTalismans) &&
                               RegisterTarget("spell-quick", SpellTalismans) &&
                               RegisterTarget("spell-cast", SpellCast) &&
                               RegisterTarget("spell-new", SpellDamage) &&
                               RegisterTarget("spell-damage", SpellDamage);

}  // namespace RetailAB
