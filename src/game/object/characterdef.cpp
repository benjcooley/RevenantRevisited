// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   characterdef.cpp - the complex object's and the character's defs    *
// *************************************************************************

#include "game/object/characterdef.h"

#include "game/object/character.h"
#include "game/object/vitalscomponent.h"
#include "game/object/complexobj.h"
#include "game/object/player.h"
#include "game/imagery/imagery.h"

#include <algorithm>
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

    TVitalsComponent& vitals = o.Vitals();
    vitals.lasthealthrecov   = cs->lasthealthrecov;
    vitals.lastfatiguerecov  = cs->lastfatiguerecov;
    vitals.lastmanarecov     = cs->lastmanarecov;
    vitals.lastpoisondamage  = cs->lastpoisondamage;
    vitals.teleport_position = cs->teleport_position;
    vitals.teleport_level    = cs->teleport_level;

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
    const TVitalsComponent& vitals = o.Vitals();
    cs->lasthealthrecov   = vitals.lasthealthrecov;
    cs->lastfatiguerecov  = vitals.lastfatiguerecov;
    cs->lastmanarecov     = vitals.lastmanarecov;
    cs->lastpoisondamage  = vitals.lastpoisondamage;
    cs->teleport_position = vitals.teleport_position;
    cs->teleport_level    = vitals.teleport_level;
}

// ---- TPlayer ----------------------------------------------------------------

SAutoMapRecordState AutoMapToState(const SAutoMapRecord& record)
{
    SAutoMapRecordState state;
    state.module = record.module;
    for (const SAutoMapLevel& level : record.levels)
    {
        SAutoMapLevelState& ls = state.levels.emplace_back();
        ls.level = level.level;
        ls.mask.reserve(level.mask.size() * 2);
        for (const int16_t w : level.mask)
        {
            ls.mask.push_back(uint8_t(uint16_t(w) & 0xFF));
            ls.mask.push_back(uint8_t(uint16_t(w) >> 8));
        }
    }
    return state;
}

SAutoMapRecord AutoMapFromState(const SAutoMapRecordState& state)
{
    SAutoMapRecord record;
    record.module = state.module;
    for (const SAutoMapLevelState& ls : state.levels)
    {
        SAutoMapLevel& level = record.levels.emplace_back();
        level.level = ls.level;
        level.mask.resize(ls.mask.size() / 2);
        for (size_t i = 0; i < level.mask.size(); i++)
            level.mask[i] = int16_t(uint16_t(ls.mask[i * 2]) | (uint16_t(ls.mask[i * 2 + 1]) << 8));
    }
    return record;
}

TObjectInstance* TPlayerDef::Create(SObjectState& state, TObjectImagery* img) const
{
    return new TPlayer(&state, img);
}

TObjectInstance* TPlayerDef::Create(TObjectImagery* img) const
{
    return new TPlayer(img);
}

// REVSYNC: the record half of TPlayer::Load @ 0x0051b960 (SAVE_GAME.md
// §11.4), with what retail derived on load: the play clock in frames, a
// fresh known-spells version, and RefreshStats (0x0051c660), the modified
// stats from the loaded ones.
void TPlayerDef::Apply(const SObjectState& state, TObjectInstance& inst) const
{
    TCharacterDef::Apply(state, inst);
    const auto* ps = dynamic_cast<const SPlayerState*>(&state);
    if (!ps)
        return;
    auto& o = static_cast<TPlayer&>(inst);

    for (int32_t i = 0; i < QSPELL_NUM; i++)
    {
        const std::string& spell = size_t(i) < ps->quickspells.size() ? ps->quickspells[size_t(i)] : std::string();
        const size_t n = std::min<size_t>(spell.size(), MAXTALISMANLEN - 1);
        memcpy(o.Spellbook().quickspells[i], spell.data(), n);
        o.Spellbook().quickspells[i][n] = 0;
    }

    o.Spellbook().known.clear();
    for (const std::string& code : ps->knownspells)
    {
        TSpellCode& c = o.Spellbook().known.emplace_back();
        c.fill(0);
        memcpy(c.data(), code.data(), std::min<size_t>(code.size(), kSpellCodeBytes));
    }
    ++o.Spellbook().version;

    o.hudwords.sidebarOpen = ps->sidebarOpen;
    o.hudwords.upperMode   = ps->upperMode;
    o.hudwords.lowerMode   = ps->lowerMode;
    o.hudwords.drawerOpen  = ps->drawerOpen;

    o.levelupstats = {};
    for (size_t i = 0; i < o.levelupstats.size() && i < ps->levelupstats.size(); i++)
        o.levelupstats[i] = ps->levelupstats[i];

    o.team = {};
    o.team.id        = ps->teamid;
    o.team.name      = ps->teamname;
    o.team.name2     = ps->teamname2;
    o.team.value     = ps->teamvalue;
    o.team.teamindex = ps->teamindex;
    o.modulename     = ps->modulename;

    o.playerstate = ps->playerstate;
    o.statetime   = ps->statetime;
    o.stateframes = int32_t(int64_t(ps->statetime) * 24 / 100);

    o.profile = {};
    for (size_t i = 0; i < o.profile.size() && i < ps->profile.size(); i++)
        o.profile[i] = ps->profile[i];
    o.frags = {};
    for (size_t i = 0; i < o.frags.size() && i < ps->frags.size(); i++)
        o.frags[i] = ps->frags[i];

    o.Automap().record = AutoMapFromState(ps->automap);
    o.RefreshStats();
}

void TPlayerDef::Capture(const TObjectInstance& inst, SObjectState& state) const
{
    TCharacterDef::Capture(inst, state);
    auto* ps = dynamic_cast<SPlayerState*>(&state);
    if (!ps)
        return;
    const auto& o = static_cast<const TPlayer&>(inst);

    ps->quickspells.clear();
    for (const char* spell : o.Spellbook().quickspells)
        ps->quickspells.emplace_back(spell);
    ps->knownspells.clear();
    for (const TSpellCode& code : o.Spellbook().known)
        ps->knownspells.emplace_back(code.data(), strnlen(code.data(), kSpellCodeBytes));

    ps->sidebarOpen = o.hudwords.sidebarOpen;
    ps->upperMode   = o.hudwords.upperMode;
    ps->lowerMode   = o.hudwords.lowerMode;
    ps->drawerOpen  = o.hudwords.drawerOpen;
    ps->levelupstats.assign(o.levelupstats.begin(), o.levelupstats.end());
    ps->teamid      = o.team.id;
    ps->teamname    = o.team.name;
    ps->teamname2   = o.team.name2;
    ps->teamvalue   = o.team.value;
    ps->teamindex   = o.team.teamindex;
    ps->modulename  = o.modulename;
    ps->playerstate = o.playerstate;
    ps->statetime   = o.statetime;
    ps->profile.assign(o.profile.begin(), o.profile.end());
    ps->frags.assign(o.frags.begin(), o.frags.end());
    ps->automap     = AutoMapToState(o.Automap().record);
}

// ---- Registration -----------------------------------------------------------

static TObjectDef::SRegistrar g_character_def("Character",
    []() -> std::unique_ptr<TObjectDef> { return std::make_unique<TCharacterDef>(); });
static TObjectDef::SRegistrar g_player_def("Player",
    []() -> std::unique_ptr<TObjectDef> { return std::make_unique<TPlayerDef>(); });
