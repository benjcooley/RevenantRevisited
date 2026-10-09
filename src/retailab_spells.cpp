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

#include "revenant.h"
#include "revutils.h"
#include "spell.h"
#include "textencoding.h"

#include <cstring>
#include <string>

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

}  // namespace

// Registered with the A/B driver by name (retailab.h).
static const bool registered = RegisterTarget("spell-data", SpellData);

}  // namespace RetailAB
