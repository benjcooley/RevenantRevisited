// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *  retailab_scenario.cpp - the port's side of the scenario kata         *
// *************************************************************************
//
// The combat dojo's scenario kata (docs/gameplay/forensics/SCENARIO_KATA.md):
// the game's own tick over a small world, tick after tick -- the player's
// keys, UpdateMove, the map pane's PulseObjects and MoveObjects, the frame
// count and game time, NextFrameObjects -- against retail's in the emulator
// (tools/retail_runtime/slots/combat/scenario.py). The world is the melee
// fixture's (retailab_melee.h) with the animation layer and the imagery's
// state queries as the game's own, the senses and the fight as the game's
// own, a flat floor, and the player's weapon in hand. Same case, same JSON
// out (schema `combat.scenario.v1`), one record a tick.

#include "game/ab/retailab_melee.h"

#include "game/ui/ctrlmap.h"              // the held controls UpdateMove reads
#include "game/world/mappane.h"              // the tick's loops
#include "game/ui/playscreen.h"           // UpdateMove, the frame count
#include "game/object/weapon.h"

#include <set>

namespace RetailAB
{
namespace
{

using namespace Fixture;

// ---- The tick's world ---------------------------------------------------------

// What the map's tick loops walk (TMapPane::tickObjectsSeam): the case's
// characters in case order -- each is in the pulse, moving and animate sets,
// as the retail fixture's map iterator gives them -- less those PulseObjects
// took out of the map.
struct STickWorld
{
    const TFixtureWorld* world = nullptr;
    std::vector<TObjectInstance*> objects;
    std::set<const TObjectInstance*> removed;
};
STickWorld g_tick;

std::vector<TObjectInstance*> TickObjects(int32_t objset)
{
    return g_tick.objects;
}

void RemoveObject(TObjectInstance* inst)
{
    JsonOut j;
    j.Begin('{').FieldString("seam", "RemoveObject").FieldString("who", g_tick.world->NameOf(inst)).End('}');
    Seam(j.str());
    g_tick.removed.insert(inst);
    g_tick.objects.erase(std::remove(g_tick.objects.begin(), g_tick.objects.end(), inst), g_tick.objects.end());
}

// The scenario's modes for as long as it runs: the animation layer and the
// imagery's state queries as the game's own, the tick loops over the case's
// world. All put back when it ends.
class SScenarioScope
{
  public:
    explicit SScenarioScope(const TFixtureWorld& world)
    {
        g_realAnimation = true;
        g_realImagery = true;
        g_tick = STickWorld{&world, {world.Order().begin(), world.Order().end()}, {}};
        TMapPane::tickObjectsSeam = TickObjects;
        TMapPane::removeObjectSeam = RemoveObject;
    }
    ~SScenarioScope()
    {
        g_realAnimation = false;
        g_realImagery = false;
        g_tick = STickWorld{};
        TMapPane::tickObjectsSeam = nullptr;
        TMapPane::removeObjectSeam = nullptr;
    }
    SScenarioScope(const SScenarioScope&) = delete;
    SScenarioScope& operator=(const SScenarioScope&) = delete;
};

// A player's weapon in hand (`wielding`: {"name", "type", "damage",
// "damagetype"}): the class.def type of that name in the prime hand, where
// the combat root's name and the damage type look (GetCombatRoot,
// GetDamageType; retail 0x0051b340 / 0x0051b2f0 read +0x2b0). Put there as a
// loaded game has it -- in the hand's inventory slot, then RefreshEquip --
// so the case's blocks stand: Equip itself would swap a fighting player's
// combat root for the weapon's. The player's inventory owns it.
void Wield(const TFixtureWorld& world, const JsonValue& cs)
{
    for (const JsonValue& spec : cs["chars"].Items())
    {
        if (!spec.Has("wielding"))
            continue;
        auto* player = dynamic_cast<TPlayer*>(world.Get(spec["name"].Str()));
        if (!player)
            throw std::runtime_error(spec["name"].Str() + ": only a player wields");
        const std::string name = spec["wielding"]["name"].Str();
        SObjectDef def{};
        def.objclass = OBJCLASS_WEAPON;
        def.objtype = (short)TObjectClass::GetClass(OBJCLASS_WEAPON)->FindObjType(name.c_str());
        if (def.objtype < 0)
            throw std::runtime_error("class.def has no weapon '" + name + "'");
        auto* weapon = new TWeapon(&def, new TFixtureImagery(TFixtureImagery::Register({}), name, {}, false));
        if (!player->AddToInventory(weapon, kInvSlotEquipFirst + EQ_PRIMEHAND))
        {
            delete weapon;
            throw std::runtime_error("can't carry " + name);
        }
        player->RefreshEquip();
        if (player->PrimeHand() != weapon)
            throw std::runtime_error("can't wield " + name);
        world.Fixture(player)->ResetStats(spec);
    }
}

// ---- The tick -------------------------------------------------------------------

// The player's side of a tick (the case's `inputs`, merged by tick as the
// retail fixture merges them): the held controls from then on, a command
// key, an attack key -- taken in that order.
struct SInput
{
    bool controls = false;
    uint32_t state = 0;                   // the control map's held mask
    uint32_t changed = 0;                 // ... and its changed mask
    GAMECOMMAND command = GAMECMD_NONE;   // a command key
    int32_t attack = 0;                   // an attack key's button, 0 none
};

// The command keys a case presses, by the name the retail fixture gives
// them (scenario.py COMMANDS: retail's Command numbers).
GAMECOMMAND CommandNamed(const std::string& name)
{
    if (name == "combat")
        return GAMECMD_COMBAT;
    throw std::runtime_error("no command key '" + name + "'");
}

std::map<int64_t, SInput> ReadInputs(const JsonValue& events)
{
    std::map<int64_t, SInput> inputs;
    for (const JsonValue& event : events.Items())
    {
        SInput& in = inputs[event["tick"].Int()];
        if (event.Has("controls"))
        {
            in.controls = true;
            in.state = (uint32_t)event["controls"][0].Int();
            in.changed = (uint32_t)event["controls"][1].Int();
        }
        if (event.Has("command"))
            in.command = CommandNamed(event["command"].Str());
        if (event.Has("attack"))
            in.attack = (int32_t)event["attack"].Int();
    }
    return inputs;
}

// One tick in retail's order (TScreen::TimerTick 0x00490bd0, as the retail
// fixture runs it): the keys that came before it (the message pump's,
// 0x00491278), UpdateMove while the player has control, the map pane's
// PulseObjects and MoveObjects, the frame count and game time,
// NextFrameObjects, the screen's frame.
void Tick(const SInput& in)
{
    if (in.controls)
        ControlMap.SetCommandFlags(in.state, in.changed);
    if (in.command != GAMECMD_NONE)
        PlayScreen.Command(in.command);
    // TPlayScreen::Command (0x0047cf40): an attack key is the player's
    // ButtonAttack (cases 0x21..0x2f).
    if (in.attack && Player)
        Player->ButtonAttack(in.attack);
    if (PlayScreen.IsControlOn())
        PlayScreen.UpdateMove();
    MapPane.PulseObjects();
    MapPane.MoveObjects();
    PlayScreen.AdvanceGameFrame();
    MapPane.NextFrameObjects();
    PlayScreen.SetFixtureScreenFrames(PlayScreen.FrameCount() + 1);
}

// Case (field 0, JSON): the melee fixture's world, "call": "scenario",
// "ticks", "inputs" ([{tick, controls: [state, changed]} | {tick, attack}]);
// see slots/combat/scenario.py.
std::string ScenarioCall(const Case& c, std::string& error)
{
    try
    {
        if (!LoadGameData(error))
            return {};
        const JsonValue cs = JsonValue::Parse(c.Field(0));
        if (cs["call"].Str() != "scenario")
            throw std::runtime_error("the scenario target runs `scenario` cases");
        Melee::TMeleeWorld melee(cs);
        TFixtureWorld& world = melee.World();
        world.charExtra = [&world](JsonOut& j, TCharacter* chr) {
            j.Key("vitals").Begin('{');
            for (const char* vital : {"health", "fatigue", "mana"})
            {
                j.Key(vital);
                if (const int32_t* v = world.Fixture(chr)->PeekStat(vital))
                    j.Value(*v);
                else
                    j.Null();
            }
            j.End('}');
        };
        SScenarioScope scenario(world);
        {
            // The port's Equip asks the imagery for the combat root (a
            // frequency variant's draw); retail's stand-in is simply put in
            // the hand.
            SUntapedDraws equipping;
            Wield(world, cs);
        }
        ControlMap.SetCommandFlags(0, 0);
        ClearSeams();
        ClearDraws();
        const std::map<int64_t, SInput> inputs = ReadInputs(cs["inputs"]);

        JsonOut j;
        j.Begin('{').FieldString("schema", "combat.scenario.v1").FieldString("side", "port");
        j.Key("ticks").Begin('[');
        for (int32_t t = 0; t < (int32_t)cs["ticks"].Int(); ++t)
        {
            const size_t firstSeam = SeamCount(), firstDraw = DrawCount();
            const auto in = inputs.find(t);
            Tick(in == inputs.end() ? SInput{} : in->second);
            j.Begin('{').Field("tick", t).Field("frame", PlayScreen.GameFrame());
            j.Key("chars").Begin('{');
            for (TCharacter* chr : world.Order())
                if (!g_tick.removed.count(chr))
                    world.WriteCharacter(j, world.NameOf(chr).c_str(), chr);
            j.End('}');
            WriteSeamsSince(j, firstSeam);
            WriteDrawsSince(j, firstDraw);
            j.End('}');
        }
        j.End(']');
        melee.WriteEnd(j);
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
static const bool registered = RegisterTarget("combat-scenario", ScenarioCall);

}  // namespace RetailAB
