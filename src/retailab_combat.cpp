// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *   retailab_combat.cpp - the port's side of the combat A/B dumps       *
// *************************************************************************
//
// The combat dojo's movement and damage targets (docs/gameplay/COMBAT_DOJO.md).
// Each mirrors a retail fixture in the emulator's combat slot
// (tools/retail_runtime/slots/combat/): the same case (one JSON object, field
// 0 of the case line), the same seams answered from it (retailab_fixture.h),
// the same JSON out, so the compare is a plain diff of the two dumps.

#include "retailab_fixture.h"

#include "dls.h"                  // MakeColorTables (the trig tables too)
#include "ctrlmap.h"              // the held controls (kata M6)
#include "mappane.h"
#include "player.h"
#include "playscreen.h"           // the game frame (kata M8), UpdateMove (M6)

namespace RetailAB
{
namespace
{

using namespace Fixture;

// ---- M3 / M5: one character method per case --------------------------------

// Kata M8: `ticks` game ticks in retail's order (TimerTick 0x490bd0): the
// case's inputs[t] ({"go": angle} -> Go), then for every character
// TComplexObject::Pulse, Move (its bits kept for the next Pulse),
// SetObjectMotion, the game frame, NextFrame; SetState and the imagery's
// SetObjectMotion as the game's own (g_realAnimation). One record a tick.
std::string Sequence(const JsonValue& cs, TFixtureWorld& world, TCharacter* me)
{
    g_realAnimation = true;
    struct SReset
    {
        ~SReset() { g_realAnimation = false; }
    } reset;
    const JsonValue& g = cs["globals"];
    int32_t frame = (int32_t)g["frame"].Int(0);
    const bool control = g["control"].Bool(true), demo = g["ps_5d8"].Bool(false);
    for (TCharacter* c : world.Order())
        c->SetMoveBits((uint32_t)cs["movebits"].Int());
    const auto& inputs = cs["inputs"].Items();

    JsonOut j;
    j.Begin('{').FieldString("schema", "combat.call.v1").FieldString("side", "port").Field("returned", 0);
    j.Key("ticks").Begin('[');
    for (int32_t t = 0; t < (int32_t)cs["ticks"].Int(); ++t)
    {
        const size_t first = SeamCount();
        if ((size_t)t < inputs.size() && inputs[t].Has("go"))
            me->Go((int32_t)inputs[t]["go"].Int());
        for (TCharacter* c : world.Order())
            world.Fixture(c)->RunComplexPulse();
        uint32_t mine = 0;
        for (TCharacter* c : world.Order())
        {
            const uint32_t bits = c->Move();
            c->SetMoveBits(bits);
            if (c == me)
                mine = bits;
        }
        for (TCharacter* c : world.Order())
            c->SetObjectMotion();
        PlayScreen.SetFixtureState(++frame, control, demo);
        for (TCharacter* c : world.Order())
            c->NextFrame();
        j.Begin('{').Field("tick", t).Field("bits", (int32_t)mine);
        world.WriteCharacter(j, "self", me);
        world.Fixture(me)->WriteMotion(j);
        WriteSeamsSince(j, first);
        j.End('}');
    }
    j.End(']');
    WriteDraws(j);
    j.End('}');
    return j.str();
}

// Case (field 0, JSON): {"globals", "chars": [...], "self", "call", ...};
// see slots/combat/combat_call.py. `call`: "go" (Go(angle), kata M3),
// "resolve-combat" / "resolve-combat-move" (the resolvers on the doing
// block with `bits`, kata M5), "move" (Move, kata M7: the case's ground and
// nearby characters, retailab_fixture.h; adds "motion"). Not modelled on the port side, because the
// port has no such thing yet: CombatFace (the case's value is ignored),
// FindClearPath (the port's Go doesn't probe ahead), CanSeeCharacter in the
// resolvers, the frame cadence of retargeting.
std::string CombatCall(const Case& c, std::string& error)
{
    try
    {
        if (!LoadGameData(error))
            return {};
        const JsonValue cs = JsonValue::Parse(c.Field(0));
        TFixtureWorld world(cs);
        SCaseScope scope(cs, world);
        TCharacter* me = world.Get(cs["self"].Str());
        IFixtureChar* fx = world.Fixture(me);
        const std::string call = cs.Has("call") ? cs["call"].Str() : "go";
        int32_t returned = 0;
        std::vector<int32_t> outputs;
        if (call == "calculate-damage")
            for (const JsonValue& in : cs["inputs"].Items())
                outputs.push_back(me->CalculateDamage((int32_t)in[0].Int(), (int32_t)in[1].Int(), (int32_t)in[2].Int()));
        else if (call == "go")
            returned = me->Go((int32_t)cs["angle"].Int()) ? 1 : 0;
        else if (call == "resolve-combat")
            returned = fx->ResolveCombat((int32_t)cs["bits"].Int());
        else if (call == "resolve-combat-move")
            returned = fx->ResolveCombatMove((int32_t)cs["bits"].Int());
        else if (call == "move")
            returned = (int32_t)me->Move();
        else if (call == "sequence")
            return Sequence(cs, world, me);
        else if (call == "update-action")
            fx->RunUpdateAction((int32_t)cs["bits"].Int());
        else if (call == "sidestep")
        {
            const std::string dir = cs["dir"].Str();
            me->SideStep(dir.empty() ? 0 : dir[0]);    // the result isn't compared: retail's is a leftover
        }
        else if (call == "leap")
            returned = me->Leap((int32_t)cs["angle"].Int()) ? 1 : 0;
        else if (call == "start-retreat")
            me->StartRetreat();
        else if (call == "update-move")
        {
            const JsonValue& ctl = cs["controls"];
            ControlMap.SetCommandFlags((uint32_t)ctl["state"].Int(), (uint32_t)ctl["changed"].Int());
            PlayScreen.UpdateMove();
        }
        else
            throw std::runtime_error("unknown call '" + call + "'");

        JsonOut j;
        j.Begin('{').FieldString("schema", "combat.call.v1").FieldString("side", "port");
        if (call == "calculate-damage")
        {
            j.Key("returned").Begin('[');
            for (int32_t v : outputs)
                j.Value(v);
            j.End(']');
        }
        else
            j.Field("returned", returned);
        world.WriteCharacter(j, "self", me);
        WriteSeams(j);
        WriteDraws(j);
        if (call == "move" || call == "update-action" || call == "start-retreat")
            fx->WriteMotion(j);
        if (call == "update-move")
        {
            uint32_t state, changed;
            ControlMap.GetCommandFlags(state, changed);
            j.Key("controls").Begin('{').Field("state", (int32_t)state).Field("changed", (int32_t)changed).End('}');
        }
        j.End('}');
        return j.str();
    }
    catch (const std::exception& e)
    {
        error = e.what();
        return {};
    }
}

// ---- M2: the angle and distance kernels ----------------------------------

// Case (field 0, JSON): {"kernel", "inputs": [[...], ...]}; see
// slots/combat/kernels.py. The tables these read are built by
// MakeColorTables (InitGlobals step 12), once.
std::string CombatKernels(const Case& c, std::string& error)
{
    if (!LoadGameData(error))
        return {};
    try
    {
        const JsonValue cs = JsonValue::Parse(c.Field(0));
        const std::string kernel = cs["kernel"].Str();
        auto point = [](const JsonValue& v) {
            return S3DPoint((int32_t)v[0].Int(), (int32_t)v[1].Int(), (int32_t)v[2].Int(0));
        };
        TObjectInstance a(nullptr), b(nullptr);
        JsonOut j;
        j.Begin('{').FieldString("schema", "combat.kernels.v1").FieldString("side", "port");
        j.FieldString("kernel", kernel).Key("outputs").Begin('[');
        for (const JsonValue& in : cs["inputs"].Items())
        {
            if (kernel == "angle-diff")
                j.Value(AngleDiff((int32_t)in[0].Int(), (int32_t)in[1].Int()));
            else if (kernel == "facing")
                j.Value(ConvertToFacing(point(in[0]), point(in[1])));
            else if (kernel == "distance")
                j.Value(Distance(point(in[0]), point(in[1])));
            else if (kernel == "vector")
            {
                S3DPoint v{0, 0, 0};
                ConvertToVector((int32_t)in[0].Int(), (int32_t)in[1].Int(), v, (int32_t)in[2].Int());
                j.Begin('[').Value(v.x).Value(v.y).Value(v.z).End(']');
            }
            else if (kernel == "obj-distance" || kernel == "obj-angle")
            {
                a.ForcePos(point(in[0]));
                b.ForcePos(point(in[1]));
                j.Value(kernel == "obj-distance" ? a.Distance(&b) : a.AngleTo(&b));
            }
            else
                throw std::runtime_error("unknown kernel '" + kernel + "'");
        }
        j.End(']').End('}');
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
static const bool registered = RegisterTarget("combat-go", CombatCall) &&
                               RegisterTarget("combat-resolve", CombatCall) &&
                               RegisterTarget("combat-damage", CombatCall) &&
                               RegisterTarget("combat-move", CombatCall) &&
                               RegisterTarget("combat-update", CombatCall) &&
                               RegisterTarget("combat-sequence", CombatCall) &&
                               RegisterTarget("combat-input", CombatCall) &&
                               RegisterTarget("combat-steps", CombatCall) &&
                               RegisterTarget("combat-kernels", CombatKernels);

}  // namespace RetailAB
