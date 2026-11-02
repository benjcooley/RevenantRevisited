// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   stateffectscomponent.h - a player's modified stats and stat effects *
// *************************************************************************
//
// The player's stats as the game uses them are a modified copy of its
// own: the equipment's and the spells' stat lines applied on top
// (docs/gameplay/forensics/PLAYER_STATS.md). Every player has one from
// construction, reached through Effects(). The effects in force are the
// port's (retail rebuilt them; the record does not carry them); the
// modified copy is derived by TPlayer::RefreshStats and not saved.
//
// *************************************************************************

#pragma once

#include "engine/scene/component.h"

#include <string>
#include <vector>

struct SStatEffect
{
    PROP(1, "The STATLINE in force")                std::string statline;   // ends "INITIALIZE" until first applied
    PROP(2, "Play-clock time it ends; -1 never")    int32_t     expires = -1;
};

class TStatEffectsComponent final : public TComponent
{
  public:
    DEF_COMPONENT("stateffects")
    [[nodiscard]] const char* ComponentName() const override { return "stateffects"; }

    PROP(1, "Stat effects in force")                              std::vector<SStatEffect> effects;    // +0x358
    PROP(2, "Play-clock time to refresh again; -1 none")          int32_t                  expires = -1; // +0x354
    PROP_NOSAVE(3, "The modified stats, by stat id (RefreshStats)") std::vector<int32_t>   modstats;    // +0x34c
};
