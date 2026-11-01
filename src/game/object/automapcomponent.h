// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   automapcomponent.h - a player's explored automap                    *
// *************************************************************************
//
// The explored area of every level the player has visited, as retail's
// visgrid packs it (automap.h TAutoMapGrid). Every player has one from
// construction, reached through Automap(); the automap pane binds to its
// record. The record is the player's (SPlayerState::automap).
//
// *************************************************************************

#pragma once

#include "engine/scene/component.h"

#include <string>
#include <vector>

struct SAutoMapLevel
{
    int32_t              level = 0;
    std::vector<int16_t> mask;
};

struct SAutoMapRecord
{
    std::string                module;
    std::vector<SAutoMapLevel> levels;
};

class TAutomapComponent final : public TComponent
{
  public:
    [[nodiscard]] const char* ComponentName() const override { return "automap"; }

    SAutoMapRecord record;   // +0x314: the explored levels
};
