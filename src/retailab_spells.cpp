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

#include "ammo.h"
#include "dialog.h"
#include "effect.h"
#include "logging.h"
#include "mappane.h"
#include "player.h"
#include "rules.h"
#include "revenant.h"
#include "revutils.h"
#include "sector.h"
#include "sound.h"
#include "spell.h"
#include "textencoding.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <map>
#include <memory>
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
// OBJSET_ANIMATE, 5): the case's `around`, recorded.
std::vector<std::string> g_around;

std::vector<TObjectInstance*> CaseAround(TObjectInstance* center)
{
    std::vector<TObjectInstance*> around;
    JsonOut j;
    j.Begin('{').FieldString("seam", "MapIterator").FieldString("who", g_spellWorld->NameOf(center));
    j.Field("objset", OBJSET_ANIMATE).Key("result").Begin('[');
    for (const std::string& n : g_around)
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
        g_around.clear();
        for (const JsonValue& v : cs["around"].Items())
            g_around.push_back(v.Str());
        TPlayer::skillExpSeam = CaseSkillExp;
        TPlayer::statEffectSeam = CaseStatEffect;
        TPlayer::playerStateSeam = CasePlayerState;
        TSpellManager::nearbySeam = CaseAround;
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
        auto* item = new TObjectInstance(&def, new TFixtureImagery(TFixtureImagery::Register({}), name, {}, false));
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
// "sourcepos", "invoker", "chars", "globals", "around", ...}; see
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

// IsEnemy as the melee kata answers it (retail 0x004c89c0): an enemy unless
// the case's `friends` lists [who, other].
std::vector<std::pair<std::string, std::string>> g_friends;

bool CaseIsEnemy(TCharacter* self, TCharacter* other)
{
    const std::string who = g_spellWorld->NameOf(self), them = g_spellWorld->NameOf(other);
    bool enemy = true;
    for (const auto& [a, b] : g_friends)
        if (a == who && b == them)
            enemy = false;
    JsonOut j;
    j.Begin('{').FieldString("seam", "IsEnemy").FieldString("who", who).FieldString("other", them);
    j.Field("result", enemy ? 1 : 0).End('}');
    Seam(j.str());
    return enemy;
}

// KnockBack (retail 0x004d3750): recorded.
void CaseKnockBack(TCharacter* self, const S3DPoint& from, int32_t variant)
{
    JsonOut j;
    j.Begin('{').FieldString("seam", "KnockBack").FieldString("who", g_spellWorld->NameOf(self));
    j.Key("from").Begin('[').Value(from.x).Value(from.y).Value(from.z).End(']');
    j.Field("variant", variant).End('}');
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
        TCharacter::isEnemySeam = CaseIsEnemy;
        TCharacter::knockBackSeam = CaseKnockBack;
        g_friends.clear();
        for (const JsonValue& f : cs["friends"].Items())
            g_friends.emplace_back(f[0].Str(), f[1].Str());
        struct SDamageSeamsOff
        {
            ~SDamageSeamsOff()
            {
                TCharacter::damageSeam = nullptr;
                TPlayer::killExpSeam = nullptr;
                TCharacter::isEnemySeam = nullptr;
                TCharacter::knockBackSeam = nullptr;
            }
        } off;
        const std::string call = cs["call"].Str();

        if (call == "area-damage")
        {
            const JsonValue& p = cs["pos"];
            const S3DPoint pos((int32_t)p[0].Int(), (int32_t)p[1].Int(), (int32_t)p[2].Int());
            TObjectInstance* attacker = cs["attacker"].IsNull() ? nullptr : world.Get(cs["attacker"].Str());
            AreaDamage(attacker, pos, (int32_t)cs["radius"].Int(), (int32_t)cs["min"].Int(), (int32_t)cs["max"].Int(),
                       (int32_t)cs["type"].Int(), (int32_t)cs["minradius"].Int(0));
            JsonOut j;
            j.Begin('{').FieldString("schema", "combat.spell.v1").FieldString("side", "port");
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


// ---- S4b: an arrow's hit (slots/combat/spell_arrow.py) -------------------------

// A seam's record of a stat answered from the case.
void StatRecord(const std::string& who, const char* stat, int32_t value)
{
    JsonOut j;
    j.Begin('{').FieldString("seam", "GetStat").FieldString("who", who).FieldString("stat", stat);
    j.Field("result", value).End('}');
    Seam(j.str());
}

// The case's arrow, a real TAmmo of its type with its stats as seams
// (retail's at TAmmo MagicType 0x004c0d40 / DamageMod 0x004c0d70).
class TCaseArrow : public TAmmo
{
  public:
    TCaseArrow(SObjectDef* def, TObjectImagery* im, const JsonValue& spec)
        : TAmmo(def, im), who(spec.Has("name") ? spec["name"].Str() : "arrow"),
          magictype((int32_t)spec["magictype"].Int()), damagemod((int32_t)spec["damagemod"].Int())
    {
        killwait = (int32_t)spec["killwait"].Int(-1);
    }

    int32_t MagicType() override
    {
        StatRecord(who, "magictype", magictype);
        return magictype;
    }
    int32_t DamageMod() override
    {
        StatRecord(who, "damagemod", damagemod);
        return damagemod;
    }

    const std::string who;

  private:
    int32_t magictype = 0;
    int32_t damagemod = 0;
};

// A player's bow: a real ranged weapon whose stats the case names answer
// GetStat by name (retail's seam at its vtable slot 0xd4).
class TCaseItem : public TObjectInstance
{
  public:
    TCaseItem(SObjectDef* def, TObjectImagery* im, const JsonValue& spec) : TObjectInstance(def, im)
    {
        who = spec["name"].Str();
        for (const auto& [k, v] : spec.Members())
            if (k != "name" && k != "type")
                answers[k] = (int32_t)v.Int();
    }

    using TObjectInstance::GetStat;
    int32_t GetStat(const char* statname) const override
    {
        std::string stat = statname;
        for (char& ch : stat)
            ch = (char)tolower((unsigned char)ch);
        auto it = answers.find(stat);
        if (it == answers.end())
            return TObjectInstance::GetStat(statname);
        StatRecord(who, stat.c_str(), it->second);
        return it->second;
    }

    std::string who;

  private:
    std::map<std::string, int32_t> answers;
};

SObjectDef ItemDef(int32_t objclass, const std::string& type)
{
    SObjectDef def{};
    def.objclass = (short)objclass;
    TObjectClass* cl = TObjectClass::GetClass(objclass);
    if (!cl)
        throw std::runtime_error("no class " + std::to_string(objclass) + " loaded");
    def.objtype = (short)cl->FindObjType(type.c_str());
    if (def.objtype < 0)
        throw std::runtime_error("class.def has no type '" + type + "' in class " + std::to_string(objclass));
    return def;
}

// The seams an arrow's hit meets beyond spell-damage's.
const JsonValue* g_arrowCase = nullptr;
std::string g_arrowName;

uint32_t CaseFlight(TAmmo*)
{
    JsonOut j;
    j.Begin('{').FieldString("seam", "Move").FieldString("who", g_arrowName).End('}');
    Seam(j.str());
    return (uint32_t)(*g_arrowCase)["bits"].Int();
}

int32_t CaseFindObjects(const S3DPoint& pos, int32_t* array, int32_t width, int32_t height, int32_t objclass,
                        int32_t maxnum, int32_t objset)
{
    JsonOut j;
    j.Begin('{').FieldString("seam", "FindObjectsInRange");
    j.Key("pos").Begin('[').Value(pos.x).Value(pos.y).Value(pos.z).End(']');
    j.Field("width", width).Field("height", height).Field("objclass", objclass).Field("maxnum", maxnum);
    j.Field("objset", objset).Key("result").Begin('[');
    int32_t n = 0;
    for (const JsonValue& name : (*g_arrowCase)["iced"].Items())
    {
        if (n >= maxnum)
            break;
        array[n++] = g_spellWorld->Get(name.Str())->GetMapIndex();
        j.String(name.Str());
    }
    j.End(']').End('}');
    Seam(j.str());
    return n;
}

int32_t CaseNewObject(SObjectDef* def)
{
    TObjectClass* cl = TObjectClass::GetClass(def->objclass);
    SObjectInfo* info = cl ? cl->GetObjType(def->objtype) : nullptr;
    JsonOut j;
    j.Begin('{').FieldString("seam", "NewObject").Field("class", def->objclass).Key("type");
    if (info)
        j.String(info->name);
    else
        j.Value(def->objtype);
    j.Field("flags", (int64_t)def->flags).Field("state", def->state).Field("level", def->level);
    j.Key("pos").Begin('[').Value(def->pos.x).Value(def->pos.y).Value(def->pos.z).End(']');
    j.Key("vel").Begin('[').Value(def->vel.x).Value(def->vel.y).Value(def->vel.z).End(']');
    j.Field("facing", def->facing).End('}');
    Seam(j.str());
    return (int32_t)(*g_arrowCase)["newobject"].Int(-1);
}

int32_t CaseFindSound(const char* name, int32_t)
{
    JsonOut j;
    j.Begin('{').FieldString("seam", "FindSound").FieldString("name", ToUtf8(name)).End('}');
    Seam(j.str());
    return -1;
}

void CaseAwardSkillExp(TPlayer* self, int32_t skill, TCharacter* victim)
{
    JsonOut j;
    j.Begin('{').FieldString("seam", "AwardSkillExp").FieldString("who", g_spellWorld->NameOf(self));
    j.Field("skill", skill).Key("victim");
    if (victim)
        j.String(g_spellWorld->NameOf(victim));
    else
        j.Null();
    j.End('}');
    Seam(j.str());
}

bool CaseManagerCast(TSpellManager* self, const char* name, TObjectInstance* invoker, TObjectInstance** targets,
                     int32_t numtargs, const S3DPoint* sourcepos, const TSpell* master)
{
    TCharacter* owner = nullptr;
    for (TCharacter* c : g_spellWorld->Order())
        if (c->GetSpellManager() == self)
            owner = c;
    int32_t result = 1;
    if (!g_casts.empty())
    {
        result = g_casts.front();
        g_casts.erase(g_casts.begin());
    }
    auto name_of = [&](TObjectInstance* o) {
        if (o)
            return g_spellWorld->NameOf(o);
        return std::string();
    };
    JsonOut j;
    j.Begin('{').FieldString("seam", "CastByName").FieldString("who", name_of(owner));
    j.FieldString("name", ToUtf8(name)).Key("invoker");
    if (invoker)
        j.String(name_of(invoker));
    else
        j.Null();
    j.Key("targets");
    if (targets)
    {
        j.Begin('[');
        for (int32_t i = 0; i < (numtargs > 1 ? numtargs : 1); i++)
            j.String(name_of(targets[i]));
        j.End(']');
    }
    else
        j.Null();
    j.Field("numtargs", numtargs).Key("source");
    if (sourcepos)
        j.Begin('[').Value(sourcepos->x).Value(sourcepos->y).Value(sourcepos->z).End(']');
    else
        j.Null();
    j.Key("master");
    if (master)
        j.String("master");
    else
        j.Null();
    j.Field("result", result).End('}');
    Seam(j.str());
    return result != 0;
}

void CaseOnAttacked(TCharacter* self, TObjectInstance* actor, TObjectInstance* target, int32_t flag)
{
    JsonOut j;
    j.Begin('{').FieldString("seam", "OnAttacked").FieldString("who", g_spellWorld->NameOf(self)).Key("attacker");
    if (actor)
        j.String(g_spellWorld->NameOf(actor));
    else
        j.Null();
    j.Key("victim");
    if (target)
        j.String(g_spellWorld->NameOf(target));
    else
        j.Null();
    j.Field("flag", flag).End('}');
    Seam(j.str());
}

// The case's characters in the map's id registry while it runs (the
// arrow's shooter and the ice burst find them by id).
struct SRegistered
{
    explicit SRegistered(const TFixtureWorld& world)
    {
        for (TCharacter* c : world.Order())
            if (c->GetMapIndex() > 0)
            {
                MapPane.RegisterInstance(c, c->GetMapIndex());
                ids.push_back(c->GetMapIndex());
            }
    }
    ~SRegistered()
    {
        for (int32_t id : ids)
            MapPane.UnregisterInstance(id);
    }
    SRegistered(const SRegistered&) = delete;
    SRegistered& operator=(const SRegistered&) = delete;
    std::vector<int32_t> ids;
};

// Case (field 0, JSON): {"call": "arrow-move" | "arrow-pulse", "arrow", "bits",
// "chars" (a player's "bow"), "iced", "newobject", "casts", "friends",
// "globals", ...}; see spell_arrow.py.
std::string MissileArrow(const Case& c, std::string& error)
{
    try
    {
        if (!LoadSpells(error))
            return {};
        if (!DialogList.Initialize())     // the game's messages (english.def)
        {
            error = "can't load the message table";
            return {};
        }
        const JsonValue cs = JsonValue::Parse(c.Field(0));
        TFixtureWorld world(cs);
        BuildSpellPieces(cs, world);
        SRegistered registered(world);

        // The bows, equipped before the case starts (each player's inventory
        // owns its bow).
        for (const JsonValue& spec : cs["chars"].Items())
            if (spec.Has("bow"))
            {
                const JsonValue& b = spec["bow"];
                SObjectDef def = ItemDef(OBJCLASS_RANGEDWEAPON, b.Has("type") ? b["type"].Str() : b["name"].Str());
                auto* bow = new TCaseItem(&def, new TFixtureImagery(TFixtureImagery::Register({}), b["name"].Str(), {},
                                                                    false), b);
                bow->SetName(const_cast<char*>(b["name"].Str().c_str()));
                auto* player = static_cast<TPlayer*>(world.Get(spec["name"].Str()));
                if (!player->Equip(bow, EQ_RANGEDWEAPON))
                {
                    delete bow;
                    throw std::runtime_error("can't equip " + b["name"].Str());
                }
                world.Fixture(player)->ResetStats(spec);
            }

        // The arrow.
        const JsonValue& a = cs["arrow"];
        SObjectDef def = ItemDef(OBJCLASS_AMMO, a.Has("type") ? a["type"].Str() : "Arrow");
        def.flags = (uint32_t)a["flags"].Int(OF_WEIGHTLESS);
        def.level = (uint16_t)a["level"].Int(0);
        const JsonValue& p = a["pos"];
        def.pos = S3DPoint((int32_t)p[0].Int(), (int32_t)p[1].Int(), (int32_t)p[2].Int());
        const JsonValue& v = a["vel"];
        def.vel = S3DPoint((int32_t)v[0].Int(), (int32_t)v[1].Int(), (int32_t)v[2].Int());
        auto* arrow = new TCaseArrow(&def, new TFixtureImagery(TFixtureImagery::Register({}), "arrow", {}, false), a);
        std::unique_ptr<TCaseArrow> owned(arrow);
        arrow->SetMapIndex(-1);
        arrow->ForcePos(def.pos);
        if (a.Has("shooter") && !a["shooter"].IsNull())
            arrow->SetShooter(world.Get(a["shooter"].Str()));
        if (a.Has("owner") && !a["owner"].IsNull())
        {
            world.Get(a["owner"].Str())->PlaceInInventory(arrow, 0);
            owned.release();      // its owner deletes it
        }

        SCaseScope scope(cs, world);
        SSpellScope spells(cs, world, false);
        // AMMODATA as the case gives it (the rules' defaults otherwise).
        struct SAmmoData
        {
            explicit SAmmoData(const JsonValue& given) : saved(Rules.ammodata)
            {
                Rules.ammodata = {20, 6, 4, 1, 25};
                for (size_t i = 0; i < given.Items().size() && i < Rules.ammodata.size(); i++)
                    Rules.ammodata[i] = (int32_t)given[i].Int();
            }
            ~SAmmoData() { Rules.ammodata = saved; }
            SAmmoData(const SAmmoData&) = delete;
            SAmmoData& operator=(const SAmmoData&) = delete;
            std::array<int32_t, 5> saved;
        } ammodata(cs["globals"]["ammodata"]);
        g_arrowCase = &cs;
        g_arrowName = arrow->who;
        TCharacter::damageSeam = CaseDamage;
        TCharacter::isEnemySeam = CaseIsEnemy;
        TCharacter::knockBackSeam = CaseKnockBack;
        TCharacter::signalAttackSeam = CaseOnAttacked;
        TPlayer::killExpSeam = CaseKillExp;
        TPlayer::awardSkillExpSeam = CaseAwardSkillExp;
        TSpellManager::castByNameSeam = CaseManagerCast;
        TAmmo::flightSeam = CaseFlight;
        TMapPane::findObjectsSeam = CaseFindObjects;
        TMapPane::newObjectSeam = CaseNewObject;
        TSoundPlayer::findSeam = CaseFindSound;
        g_friends.clear();
        for (const JsonValue& f : cs["friends"].Items())
            g_friends.emplace_back(f[0].Str(), f[1].Str());
        struct SArrowSeamsOff
        {
            ~SArrowSeamsOff()
            {
                TCharacter::damageSeam = nullptr;
                TCharacter::isEnemySeam = nullptr;
                TCharacter::knockBackSeam = nullptr;
                TCharacter::signalAttackSeam = nullptr;
                TPlayer::killExpSeam = nullptr;
                TPlayer::awardSkillExpSeam = nullptr;
                TSpellManager::castByNameSeam = nullptr;
                TAmmo::flightSeam = nullptr;
                TMapPane::findObjectsSeam = nullptr;
                TMapPane::newObjectSeam = nullptr;
                TSoundPlayer::findSeam = nullptr;
                g_arrowCase = nullptr;
            }
        } off;
        ClearSeams();
        ClearDraws();

        const std::string call = cs["call"].Str();
        JsonOut j;
        j.Begin('{').FieldString("schema", "combat.spell.v1").FieldString("side", "port");
        int64_t bits = 0;
        if (call == "arrow-move")
            bits = arrow->Move();
        else if (call == "arrow-pulse")
            arrow->Pulse();
        else
            throw std::runtime_error("unknown call '" + call + "'");
        j.Key("arrow").Begin('{').Field("killed", (arrow->GetFlags() & OF_KILL) ? 1 : 0);
        j.Field("killwait", arrow->KillWait());
        if (call == "arrow-move")
            j.Field("bits", bits);
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


// ---- S4c: the bow (slots/combat/spell_bow.py) ---------------------------------

bool CaseEquip(TPlayer* self, TObjectInstance* item, int32_t slot)
{
    JsonOut j;
    j.Begin('{').FieldString("seam", "Equip").FieldString("who", g_spellWorld->NameOf(self)).Key("item");
    if (item)
        j.String(item->GetName());
    else
        j.Null();
    j.Field("slot", slot).End('}');
    Seam(j.str());
    return true;
}

// A character's ammunition from the case: [{"name" (its AMMO type),
// "amount", "equipped"}, ...], real TAmmo bundles in inventory order (the
// player's equipped one in the ammo slot).
void BuildAmmo(TCharacter* chr, const JsonValue& items)
{
    int32_t slot = 0;
    for (const JsonValue& spec : items.Items())
    {
        SObjectDef def = ItemDef(OBJCLASS_AMMO, spec["name"].Str());
        auto* ammo = new TAmmo(&def, new TFixtureImagery(TFixtureImagery::Register({}), spec["name"].Str(), {}, false));
        ammo->SetName(const_cast<char*>(spec["name"].Str().c_str()));
        ammo->SetObjStat(ammo->FindObjStat("Amount"), (int32_t)spec["amount"].Int(1));
        chr->PlaceInInventory(ammo, slot++);
        if (spec["equipped"].Bool() && !static_cast<TPlayer*>(chr)->Equip(ammo, EQ_AMMO))
            throw std::runtime_error("can't equip " + spec["name"].Str());
    }
}

// Case (field 0, JSON): {"call": "draw-bow" | "aim-bow" | "aim-left" |
// "aim-right" | "shoot-bow" | "is-bow-drawn" | "resolve-bow-aim" |
// "resolve-bow-shoot", "self", "angle", "bits", "chars" (each's "ammo",
// "lastbowshot", "bowshots"), "arrow" (what NewObject's id gives),
// "newobject", ...}; see spell_bow.py.
std::string MissileBow(const Case& c, std::string& error)
{
    try
    {
        if (!LoadSpells(error))
            return {};
        const JsonValue cs = JsonValue::Parse(c.Field(0));
        TFixtureWorld world(cs);
        BuildSpellPieces(cs, world);
        SRegistered registered(world);
        for (const JsonValue& spec : cs["chars"].Items())
            if (spec.Has("ammo"))
            {
                TCharacter* chr = world.Get(spec["name"].Str());
                BuildAmmo(chr, spec["ammo"]);
                world.Fixture(chr)->ResetStats(spec);
            }

        // The arrow NewObject's id names, when the case gives one.
        std::unique_ptr<TCaseArrow> arrow;
        if (cs.Has("arrow"))
        {
            const JsonValue& a = cs["arrow"];
            SObjectDef def = ItemDef(OBJCLASS_AMMO, a.Has("type") ? a["type"].Str() : "Arrow");
            def.flags = OF_WEIGHTLESS;
            arrow = std::make_unique<TCaseArrow>(&def, new TFixtureImagery(TFixtureImagery::Register({}), "arrow", {},
                                                                           false), a);
            arrow->SetMapIndex(-1);
            MapPane.RegisterInstance(arrow.get(), (int32_t)a["id"].Int(900));
        }
        struct SArrowOff
        {
            ~SArrowOff()
            {
                if (id >= 0)
                    MapPane.UnregisterInstance(id);
            }
            int32_t id = -1;
        } arrowoff;
        if (arrow)
            arrowoff.id = (int32_t)cs["arrow"]["id"].Int(900);

        SCaseScope scope(cs, world);
        SSpellScope spells(cs, world, false);
        g_arrowCase = &cs;
        TPlayer::equipSeam = CaseEquip;
        TMapPane::newObjectSeam = CaseNewObject;
        TSoundPlayer::findSeam = CaseFindSound;
        inventorySeams = true;
        struct SBowSeamsOff
        {
            ~SBowSeamsOff()
            {
                TPlayer::equipSeam = nullptr;
                TMapPane::newObjectSeam = nullptr;
                TSoundPlayer::findSeam = nullptr;
                inventorySeams = false;
                g_arrowCase = nullptr;
            }
        } off;
        ClearSeams();
        ClearDraws();

        TCharacter* me = world.Get(cs["self"].Str());
        const std::string call = cs["call"].Str();
        const int32_t angle = (int32_t)cs["angle"].Int();
        const int32_t bits = (int32_t)cs["bits"].Int();
        int32_t returned = 0;
        if (call == "draw-bow")
            returned = me->DrawBow() ? 1 : 0;
        else if (call == "aim-bow")
            returned = me->AimBow(angle) ? 1 : 0;
        else if (call == "aim-left")
            returned = me->AimBowLeft() ? 1 : 0;
        else if (call == "aim-right")
            returned = me->AimBowRight() ? 1 : 0;
        else if (call == "shoot-bow")
            returned = me->ShootBow(angle) ? 1 : 0;
        else if (call == "is-bow-drawn")
            returned = me->IsBowDrawn() ? 1 : 0;
        else if (call == "resolve-bow-aim")
            returned = world.Fixture(me)->RunResolver("bow-aim", bits);
        else if (call == "resolve-bow-shoot")
            returned = world.Fixture(me)->RunResolver("bow-shoot", bits);
        else
            throw std::runtime_error("unknown call '" + call + "'");

        JsonOut j;
        j.Begin('{').FieldString("schema", "combat.spell.v1").FieldString("side", "port");
        j.Field("returned", returned);
        world.WriteCharacter(j, "self", me);
        world.Fixture(me)->WriteMotion(j);
        j.Key("bow").Begin('{').Field("lastbowshot", me->LastBowShot()).Field("bowshots", me->BowShots()).End('}');
        if (arrow)
        {
            j.Key("arrow").Begin('{').Key("shooter");
            TObjectInstance* by = arrow->Shooter();
            if (by)
                j.String(world.NameOf(by));
            else
                j.Null();
            j.End('}');
        }
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


// ---- S4d: a spell's fireball, tick by tick (slots/combat/spell_missile.py) ----

// The fireball's imagery: its three states, asked without a record (the
// retail fixture's FireBall keeps its state itself).
class TQuietImagery : public TFixtureImagery
{
  public:
    using TFixtureImagery::TFixtureImagery;
    int32_t NumStates() const override { return TObjectImagery::NumStates(); }
    int32_t GetAniFlags(int32_t state) const override { return TObjectImagery::GetAniFlags(state); }
    int32_t GetAniLength(int32_t state) const override { return TObjectImagery::GetAniLength(state); }
    bool NeedsAnimator(const TObjectInstance*) const override { return false; }
};

void WriteMissileTick(JsonOut& j, int32_t tick, TFireBallEffect& fb)
{
    const missile_state::State& m = fb.ProjectileState();
    static const std::pair<uint32_t, const char*> names[] = {
        {OF_IMMOBILE, "immobile"}, {OF_KILL, "kill"}, {OF_MOVING, "moving"}, {OF_PULSE, "pulse"},
        {OF_WEIGHTLESS, "weightless"}};
    std::vector<std::string> set;
    for (const auto& [bit, name] : names)
        if (fb.GetFlags() & bit)
            set.emplace_back(name);
    std::sort(set.begin(), set.end());
    const S3DPoint p = fb.Pos();
    j.Begin('{').Field("tick", tick).Field("state", m.state);
    j.Key("pos").Begin('[').Value(p.x).Value(p.y).Value(p.z).End(']');
    j.Key("vel").Begin('[').Value(m.velocity_fixed.x).Value(m.velocity_fixed.y).Value(m.velocity_fixed.z).End(']');
    j.Key("accum").Begin('[').Value(m.accumulator.x).Value(m.accumulator.y).Value(m.accumulator.z).End(']');
    j.Field("life", m.range).Field("armed", fb.DamageArmed() ? 1 : 0).Key("flags").Begin('[');
    for (const std::string& s : set)
        j.String(s);
    j.End(']');
    WriteSeams(j);
    WriteDraws(j);
    j.End('}');
}

// Case (field 0, JSON): {"ticks", "fireball": {pos, level, animator},
// "variant", "invoker", "targets", "spell" (true), "chars", "ground",
// "nearby", "friends", ...}; see spell_missile.py.
std::string MissileFireball(const Case& c, std::string& error)
{
    try
    {
        if (!LoadSpells(error))
            return {};
        const JsonValue cs = JsonValue::Parse(c.Field(0));
        TFixtureWorld world(cs);
        BuildSpellPieces(cs, world);
        SRegistered registered(world);      // the spell's references find them
        SCaseScope scope(cs, world);
        SSpellScope spells(cs, world, false);
        TCharacter::damageSeam = CaseDamage;
        TPlayer::killExpSeam = CaseKillExp;
        TCharacter::isEnemySeam = CaseIsEnemy;
        TCharacter::knockBackSeam = CaseKnockBack;
        g_friends.clear();
        for (const JsonValue& f : cs["friends"].Items())
            g_friends.emplace_back(f[0].Str(), f[1].Str());
        struct SFireballSeamsOff
        {
            ~SFireballSeamsOff()
            {
                TCharacter::damageSeam = nullptr;
                TPlayer::killExpSeam = nullptr;
                TCharacter::isEnemySeam = nullptr;
                TCharacter::knockBackSeam = nullptr;
            }
        } off;

        // The spell, as spell-new builds it.
        std::vector<TObjectInstance*> targets;
        const bool hasTargets = cs.Has("targets") && !cs["targets"].IsNull();
        if (hasTargets)
            for (const JsonValue& t : cs["targets"].Items())
                targets.push_back(t.IsNull() ? nullptr : world.Get(t.Str()));
        if (hasTargets && targets.empty())
            targets.push_back(nullptr);
        const int32_t numtargs = hasTargets ? (int32_t)cs["targets"].Items().size() : 0;
        auto [sd, variant] = FindVariant(cs["variant"]);
        TObjectInstance* invoker = cs["invoker"].IsNull() ? nullptr : world.Get(cs["invoker"].Str());
        const SSpellClass* cls = FindSpellClass("Spell");
        std::unique_ptr<TSpell> spell(cls->create(invoker, hasTargets ? targets.data() : nullptr, numtargs, nullptr,
                                                  sd, variant, nullptr));

        // The fireball, Initialize run by its constructor.
        const JsonValue& f = cs["fireball"];
        SObjectDef def = ItemDef(OBJCLASS_EFFECT, "FireBall");
        def.flags = OF_IMMOBILE | OF_PULSE;
        def.level = (uint16_t)f["level"].Int(0);
        if (f.Has("pos"))
        {
            const JsonValue& p = f["pos"];
            def.pos = S3DPoint((int32_t)p[0].Int(), (int32_t)p[1].Int(), (int32_t)p[2].Int());
        }
        else
            def.pos = S3DPoint(1000, 1000, 60);
        const std::vector<SFixtureState> states = ReadStates(JsonValue::Parse(R"(["launch", "fly", "explode"])"));
        TFireBallEffect fb(&def, new TQuietImagery(TFixtureImagery::Register(states), "fireball", states, false));
        fb.SetMapIndex(-1);
        fb.ForcePos(def.pos);
        if (cs["spell"].Bool(true))
            fb.SetSpell(spell.get());
        ClearSeams();
        ClearDraws();

        JsonOut j;
        j.Begin('{').FieldString("schema", "combat.spell.v1").FieldString("side", "port").Key("ticks").Begin('[');
        for (int32_t t = 0; t < (int32_t)cs["ticks"].Int(1); t++)
        {
            ClearSeams();
            ClearDraws();
            fb.PulseMissile();
            WriteMissileTick(j, t, fb);
        }
        j.End(']');
        j.Key("casters").Begin('{');
        for (const JsonValue& spec : cs["chars"].Items())
        {
            j.Key(spec["name"].Str().c_str());
            WriteCaster(j, world, world.Get(spec["name"].Str()));
        }
        j.End('}');
        j.End('}');
        fb.SetSpell(nullptr);
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
                               RegisterTarget("spell-damage", SpellDamage) &&
                               RegisterTarget("missile-area", SpellDamage) &&
                               RegisterTarget("missile-arrow", MissileArrow) &&
                               RegisterTarget("missile-bow", MissileBow) &&
                               RegisterTarget("missile-fireball", MissileFireball);

}  // namespace RetailAB
