// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   characterdef.cpp - the complex object's and the character's defs    *
// *************************************************************************

#include "game/object/characterdef.h"

#include "game/object/character.h"
#include "game/object/complexobj.h"
#include "game/object/player.h"
#include "game/imagery/imagery.h"

#include <cstring>

// ---- TComplexObject ---------------------------------------------------------

// REVSYNC: the root half of TComplexObject::Load @ 0x004db930. A record
// with no root (a spawn record, or a map older than version 7) leaves the
// constructor's root standing.
void TComplexObjectDef::Apply(const SObjectState& state, TObjectInstance& inst) const
{
    TObjectDef::Apply(state, inst);
    const auto* cs = dynamic_cast<const SComplexObjectState*>(&state);
    if (!cs)
        return;
    auto& o = static_cast<TComplexObject&>(inst);

    TActionBlock* ab = cs->rootname.empty()
        ? new TActionBlock(o.DefaultRootState())
        : new TActionBlock(cs->rootname.substr(0, RESNAMELEN - 1).c_str(), (ACTION)cs->rootaction);

  // REVSYNC-DIVERGENCE: a root that names no state of the model can never be
  // entered. Every ForceCommand back to it is impossible, so a speaker who
  // finishes a line stays in "say" and the script's speech wait never ends.
  // The shipped level-46 map saves Shegra and the slaves with a "combat" root
  // their walk-and-say models lack (docs/gameflow/forensics/DIALOG.md §2.5).
  // Such a root becomes the default one, else "walk", as a plain animation
  // block like the one Clear builds.
    if (o.imagery && !o.imagery->HasState(ab->name))
    {
        const char* fallback = o.imagery->HasState(o.DefaultRootState()) ? o.DefaultRootState() : "walk";
        if (o.imagery->HasState(fallback))
        {
            delete ab;
            ab = new TActionBlock(fallback);
        }
    }
    o.SetRoot(ab);
    o.SetDoing(ab);
    o.SetDesired(ab);

  // Keep the saved state when it is the root's own (an animation state is
  // named "<group>:<name>"); otherwise start the root.
    if (const char* statename = o.imagery ? o.imagery->GetAniName(o.state) : nullptr)
    {
        if (const char* colon = strchr(statename, ':'))
            statename = colon + 1;
        if (stricmp(ab->name, statename) == 0)
            return;
    }
    o.state = o.FindState(ab->name);
}

void TComplexObjectDef::Capture(const TObjectInstance& inst, SObjectState& state) const
{
    TObjectDef::Capture(inst, state);
    auto* cs = dynamic_cast<SComplexObjectState*>(&state);
    if (!cs)
        return;
    const auto& o = static_cast<const TComplexObject&>(inst);
    cs->rootaction = o.root ? int32_t(o.root->action) : 0;
    cs->rootname   = o.root ? o.root->name : "";
}

// ---- TCharacter -------------------------------------------------------------

TObjectInstance* TCharacterDef::Create(SObjectState& state, TObjectImagery* img) const
{
    return new TCharacter(&state, img);
}

TObjectInstance* TCharacterDef::Create(TObjectImagery* img) const
{
    return new TCharacter(img);
}

// REVSYNC: the record half of TCharacter::Load @ 0x004d4eb0, with what
// retail settled on load: the vitals clamped to the type's, a character
// saved dead removed on its first frame, fully visible and not fading.
void TCharacterDef::Apply(const SObjectState& state, TObjectInstance& inst) const
{
    TComplexObjectDef::Apply(state, inst);
    const auto* cs = dynamic_cast<const SCharacterState*>(&state);
    if (!cs)
        return;
    auto& o = static_cast<TCharacter&>(inst);

    o.lasthealthrecov   = cs->lasthealthrecov;
    o.lastfatiguerecov  = cs->lastfatiguerecov;
    o.lastmanarecov     = cs->lastmanarecov;
    o.lastpoisondamage  = cs->lastpoisondamage;
    o.teleport_position = cs->teleport_position;
    o.teleport_level    = cs->teleport_level;

    if (o.ObjClass() == OBJCLASS_CHARACTER && o.chardata)
    {
        if (o.Health() > o.chardata->health)
            o.SetHealth(o.chardata->health);
        if (o.MaxHealth() > o.chardata->health)
            o.SetMaxHealth(o.chardata->health);
        if (o.Fatigue() > o.chardata->fatigue)
            o.SetFatigue(o.chardata->fatigue);
        if (o.MaxFatigue() > o.chardata->fatigue)
            o.SetMaxFatigue(o.chardata->fatigue);
        if (o.Mana() > o.chardata->mana)
            o.SetMana(o.chardata->mana);
        if (o.MaxMana() > o.chardata->mana)
            o.SetMaxMana(o.chardata->mana);
    }
    if (o.Health() < 1)
        o.ResetFlags(o.flags | OF_KILL);
    o.fade = 100;
    o.fade_step = 0;
    o.fade_limit = 100;
    o.fade_direction = 0;
}

void TCharacterDef::Capture(const TObjectInstance& inst, SObjectState& state) const
{
    TComplexObjectDef::Capture(inst, state);
    auto* cs = dynamic_cast<SCharacterState*>(&state);
    if (!cs)
        return;
    const auto& o = static_cast<const TCharacter&>(inst);
    cs->lasthealthrecov   = o.lasthealthrecov;
    cs->lastfatiguerecov  = o.lastfatiguerecov;
    cs->lastmanarecov     = o.lastmanarecov;
    cs->lastpoisondamage  = o.lastpoisondamage;
    cs->teleport_position = o.teleport_position;
    cs->teleport_level    = o.teleport_level;
}

// ---- TPlayer ----------------------------------------------------------------

TObjectInstance* TPlayerDef::Create(SObjectState& state, TObjectImagery* img) const
{
    return new TPlayer(&state, img);
}

TObjectInstance* TPlayerDef::Create(TObjectImagery* img) const
{
    return new TPlayer(img);
}

// ---- Registration -----------------------------------------------------------

static TObjectDef::SRegistrar g_character_def("Character",
    []() -> std::unique_ptr<TObjectDef> { return std::make_unique<TCharacterDef>(); });
static TObjectDef::SRegistrar g_player_def("Player",
    []() -> std::unique_ptr<TObjectDef> { return std::make_unique<TPlayerDef>(); });
