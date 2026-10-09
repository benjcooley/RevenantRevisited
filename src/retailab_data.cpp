// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *    retailab_data.cpp - the port's side of the combat data A/B dump    *
// *************************************************************************
//
// Kata D1 of the combat dojo (docs/gameplay/COMBAT_DOJO.md): the combat data
// as the port parses it. Mirrors tools/retail_runtime/slots/combat/
// data_parse.py, which runs retail's TRules::Initialize and BindTypes over
// the same files: the same case, the same records by field name
// (docs/gameplay/forensics/COMBAT_DATA.md §3-§8), schema `combat.data.v1`.
//
// A case (field 0, JSON): {"dir": <a folder laid out as an install, with
// Resources/ and Imagery/>, "gamespeed": 3}. The rules loader's search paths
// point there for the case; TRules::Initialize then reads its files the way
// it reads the install's.

#include "retailab.h"
#include "retailab_fixture.h"
#include "retailab_json.h"

#include "revenant.h"
#include "revutils.h"
#include "rules.h"
#include "textencoding.h"

#include <cstring>
#include <string>

namespace RetailAB
{
namespace
{

// The rules loader's inputs for one case -- ClassDefPath and ImageryPath at
// the case's Resources/ and Imagery/, the game speed -- put back after.
class TCaseInstall
{
  public:
    TCaseInstall(const std::string& dir, int32_t gamespeed)
    {
        strncpyz(savedClassDefPath, ClassDefPath, MAXPATHLEN);
        strncpyz(savedImageryPath, ImageryPath, MAXPATHLEN);
        savedGameSpeed = GameSpeed;
        strncpyz(ClassDefPath, (dir + "/Resources/").c_str(), MAXPATHLEN);
        strncpyz(ImageryPath, (dir + "/Imagery/").c_str(), MAXPATHLEN);
        GameSpeed = gamespeed;
    }
    ~TCaseInstall()
    {
        strncpyz(ClassDefPath, savedClassDefPath, MAXPATHLEN);
        strncpyz(ImageryPath, savedImageryPath, MAXPATHLEN);
        GameSpeed = savedGameSpeed;
    }
    TCaseInstall(const TCaseInstall&) = delete;
    TCaseInstall& operator=(const TCaseInstall&) = delete;

  private:
    char savedClassDefPath[MAXPATHLEN] = {};
    char savedImageryPath[MAXPATHLEN] = {};
    int32_t savedGameSpeed = 0;
};

// ---- The dump, by retail's field names --------------------------------------

void Text(JsonOut& j, const char* key, const char* text)
{
    j.FieldString(key, ToUtf8(text));
}

template <typename T>
void Ints(JsonOut& j, const char* key, const T* values, size_t count)
{
    j.Key(key).Begin('[');
    for (size_t i = 0; i < count; i++)
        j.Value(values[i]);
    j.End(']');
}

void WriteImpact(JsonOut& j, const SCharAttackImpact& ai)
{
    j.Begin('{');
    Text(j, "name", ai.impactname);
    j.Field("index", ai.index).Field("flags", ai.flags);
    Text(j, "loopname", ai.loopname);
    j.Field("looptime", ai.looptime).Field("damagemin", ai.damagemin).Field("damagemax", ai.damagemax);
    j.Field("snapdist", ai.snapdist).Field("snaptime", ai.snaptime);
    j.End('}');
}

void WriteImpacts(JsonOut& j, const SCharAttackImpact* impacts, int32_t count)
{
    j.Key("impacts").Begin('[');
    for (int32_t i = 0; i < count; i++)
        WriteImpact(j, impacts[i]);
    j.End(']');
}

void WriteAttack(JsonOut& j, const SCharAttackData& ad)
{
    j.Begin('{');
    Text(j, "name", ad.attackname);
    j.Field("flags", ad.flags).Field("button", ad.button).Field("attackpcnt", ad.attackpcnt);
    j.Field("mindist", ad.mindist).Field("maxdist", ad.maxdist);
    if (ad.flags & CA_MAGICATTACK)
    {
        Text(j, "spellname", ad.spellname);
        j.Key("spellsource").Begin('[').Value(ad.spellsource.x).Value(ad.spellsource.y).Value(ad.spellsource.z).End(']');
        j.Field("condition", ad.condition).Field("conditionvalue", ad.conditionvalue);
    }
    else
    {
        Text(j, "responsename", ad.responsename);
        Text(j, "blockname", ad.blockname);
        Text(j, "missname", ad.missname);
        Text(j, "chainname", ad.chainname);
        j.Field("blocktime", ad.blocktime).Field("impacttime", ad.impacttime);
        j.Field("chainexptime", ad.chainexptime).Field("nextwait", ad.nextwait);
        j.Field("hitminrange", ad.hitminrange).Field("hitmaxrange", ad.hitmaxrange).Field("hitangle", ad.hitangle);
        j.Field("damagemod", ad.damagemod).Field("fatigue", ad.fatigue).Field("attackskill", ad.attackskill);
        j.Field("weaponmask", ad.weaponmask).Field("weaponskill", ad.weaponskill);
        j.Field("swipeframeon", ad.swipeframeon).Field("swipeframeoff", ad.swipeframeoff);
        j.Field("maxfatigue", ad.maxfatigue);
        WriteImpacts(j, ad.impacts, ad.numimpacts);
    }
    j.End('}');
}

void WriteCharacter(JsonOut& j, SCharData& cd)
{
    j.Begin('{');
    Text(j, "name", cd.name);
    Text(j, "groups", cd.groups);
    Text(j, "enemies", cd.enemies);
    j.Field("objtype", cd.objtype).Field("objclass", cd.objclass).Field("flags", cd.flags);
    Ints(j, "damagemods", cd.damagemods, NUMDAMAGETYPES);
    Text(j, "blocksounds", cd.blocksounds);
    Text(j, "misssounds", cd.misssounds);
    const int32_t playerblock[] = {cd.playerblockmin, cd.playerblockstep, cd.playerblockinc};
    Ints(j, "playerblock", playerblock, 3);
    const int32_t combatrange[] = {cd.combatrangemin, cd.combatrangemax};
    Ints(j, "combatrange", combatrange, 2);
    j.Field("maxattackrange", cd.maxattackrange).Field("bleeder", cd.bleeder).Field("swipefull", cd.swipefull);
    Text(j, "bodytype", cd.bodytype);
    const int32_t block[] = {cd.blockfreq, cd.blockmin, cd.blockmax};
    Ints(j, "block", block, 3);
    const int32_t sight[] = {cd.sightmin, cd.sightmax, cd.sightrange, cd.sightangle};
    Ints(j, "sight", sight, 4);
    const int32_t hearing[] = {cd.hearingmin, cd.hearingmax, cd.hearingrange};
    Ints(j, "hearing", hearing, 3);
    j.Field("weapontype", cd.weapontype).Field("weapondamage", cd.weapondamage).Field("armor", cd.armorvalue);
    j.Field("defensemod", cd.defensemod).Field("attackmod", cd.attackmod);
    const int32_t attackfreq[] = {cd.minattackfreq, cd.maxattackfreq};
    Ints(j, "attackfreq", attackfreq, 2);
    const int32_t magicfreq[] = {cd.minmagicfreq, cd.maxmagicfreq};
    Ints(j, "magicfreq", magicfreq, 2);
    j.Field("mana", cd.mana).Field("fatigue", cd.fatigue).Field("health", cd.health);
    j.Field("walkspeed", cd.walkspeed).Field("runspeed", cd.runspeed).Field("sneakspeed", cd.sneakspeed);
    j.Field("combatwalkspeed", cd.combatwalkspeed);
    j.Key("arrowpos").Begin('[').Value(cd.arrowpos.x).Value(cd.arrowpos.y).Value(cd.arrowpos.z).End(']');
    j.Field("arrowspeed", cd.arrowspeed).Field("bowwait", cd.bowwait).Field("bowaimspeed", cd.bowaimspeed);
    j.Field("retreatat", cd.retreatat).Field("retreatatmana", cd.retreatatmana).Field("retreatfor", cd.retreatfor);
    Ints(j, "runfatigue", cd.runfatigue, 2);
    j.Field("noparalyze", cd.noparalyze).Field("poisonchance", cd.poisonchance).Field("labelheight", cd.labelheight);
    if (cd.classdata)
        Text(j, "class", cd.classdata->name);
    else
        j.Key("class").Null();
    j.Key("swipecolor").Begin('[').Value(cd.swipecolor.red).Value(cd.swipecolor.green).Value(cd.swipecolor.blue).End(']');
    j.Key("attacheffects").Begin('[');
    for (const SCharAttachEffect& e : cd.attacheffects)
    {
        j.Begin('{').Field("used", e.used);
        Text(j, "name", e.name);
        Ints(j, "values", e.values, 3);
        Text(j, "arg5", e.arg5);
        Text(j, "arg6", e.arg6);
        j.End('}');
    }
    j.End(']');
    WriteImpacts(j, cd.impacts, cd.numimpacts);
    j.Key("attacks").Begin('[');
    for (int32_t i = 0; i < cd.attacks.NumItems(); i++)
        WriteAttack(j, cd.attacks[i]);
    j.End(']');
    j.End('}');
}

void WriteItems(JsonOut& j, const char* key, const std::vector<SItemData>& items)
{
    j.Key(key).Begin('[');
    for (const SItemData& item : items)
    {
        j.Begin('{');
        Text(j, "name", item.name.c_str());
        Text(j, "description", item.description.c_str());
        Ints(j, "basicmods", item.basicmods.data(), item.basicmods.size());
        Text(j, "statline", item.statline.c_str());
        j.End('}');
    }
    j.End(']');
}

void WriteRules(JsonOut& j, bool initialized)
{
    const TRules& r = Rules;
    j.Key("rules").Begin('{');
    j.Field("initialized", initialized);
    j.Field("daylength", r.daylength).Field("twilight", r.twilight).Field("twilightsteps", r.twilightsteps);
    j.Field("healthperlevel", r.healthperlevel).Field("fatigueperlevel", r.fatigueperlevel);
    j.Field("manaperlevel", r.manaperlevel);
    j.Field("healthrecovval", r.healthrecovval).Field("fatiguerecovval", r.fatiguerecovval);
    j.Field("manarecovval", r.manarecovval);
    j.Field("healthrecovrate", r.healthrecovrate).Field("fatiguerecovrate", r.fatiguerecovrate);
    j.Field("manarecovrate", r.manarecovrate);
    j.Field("poisondamageval", r.poisondamageval).Field("poisondamagerate", r.poisondamagerate);
    j.Field("tohitcenter", r.tohitcenter).Field("tohitrangechar", r.tohitrangechar);
    j.Field("tohitrangeplyr", r.tohitrangeplyr).Field("tohitblock", r.tohitblock).Field("tohitface", r.tohitface);
    j.Field("maxstealth", r.maxstealth).Field("sneakstealth", r.sneakstealth).Field("minstealth", r.minstealth);
    j.Key("tohitdamage").Begin('[');
    for (const TRules::SToHitDamage& row : r.tohitdamage)
        j.Begin('[').Value(row.minvalue).Value(row.damagepercent).End(']');
    j.End(']');
    Ints(j, "ammodata", r.ammodata.data(), r.ammodata.size());
    j.Key("exp").Begin('[');
    for (int32_t level = 1; level <= TRules::kMaxPlayerLevel; level++)
        j.Value(r.ExpForLevel(level));
    j.End(']');
    j.Key("skillexp").Begin('[');
    for (int32_t level = 1; level <= TRules::kMaxSkillLevel; level++)
        j.Value(r.SkillExpForLevel(level));
    j.End(']');
    j.Key("statlevels").Begin('[');
    for (int32_t table = 0; table < PlayerStats::TStatLevels::kTables; table++)
    {
        j.Begin('[');
        for (int32_t value = 0; value < PlayerStats::TStatLevels::kLevels; value++)
            j.Value(r.StatLevel(table, value));
        j.End(']');
    }
    j.End(']');
    if (const SCharData* def = r.DefaultCharData())
        Text(j, "default", def->name);
    else
        j.Key("default").Null();
    j.End('}');
}

// ---- D1: the combat data -----------------------------------------------------

std::string CombatData(const Case& c, std::string& error)
{
    try
    {
        if (!Fixture::LoadGameData(error))
            return {};
        const JsonValue cs = JsonValue::Parse(c.Field(0));
        const TCaseInstall install(cs["dir"].Str(), (int32_t)cs["gamespeed"].Int(3));

        Rules.Close();
        const bool initialized = Rules.Initialize();

        JsonOut j;
        j.Begin('{').FieldString("schema", "combat.data.v1").FieldString("side", "port");
        j.Key("fatal").Null().Field("initialize", initialized);
        WriteRules(j, initialized);
        j.Key("classes").Begin('[');
        for (int32_t i = 0; i < Rules.GetNumClasses(); i++)
        {
            const SClassData& cl = *Rules.GetClass(i);
            j.Begin('{');
            Text(j, "name", cl.name);
            Ints(j, "statreqs", cl.statreqs, NUM_PLRSTATS);
            Ints(j, "skillmods", cl.skillmods, NUM_SKILLS);
            j.Field("healthmod", cl.healthmod).Field("fatiguemod", cl.fatiguemod).Field("manamod", cl.manamod);
            j.Field("weapons", cl.weapons);
            j.End('}');
        }
        j.End(']');
        j.Key("chars").Begin('[');
        for (int32_t i = 0; i < Rules.GetNumCharData(); i++)
            WriteCharacter(j, *Rules.GetCharData(i));
        j.End(']');
        WriteItems(j, "weapons", Rules.Weapons());
        WriteItems(j, "armor", Rules.Armors());
        j.End('}');

        Rules.Close();
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
static const bool registered = RegisterTarget("combat-data", CombatData);

}  // namespace RetailAB
