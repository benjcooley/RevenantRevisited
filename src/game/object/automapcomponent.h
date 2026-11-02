// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   automapcomponent.h - a player's explored automap                    *
// *************************************************************************
//
// The explored area of every level the player has visited, as retail's
// visgrid packs it (automap.h TAutoMapGrid: run-length words). Every
// player has one from construction, reached through Automap(); the
// automap pane binds to its record. The record is reflected and is the
// player's state too (SPlayerState::automap), one type for both: the
// masks are the packed words as bytes, little-endian, a byte blob to the
// codecs and the inspector.
//
// *************************************************************************

#pragma once

#include "engine/scene/component.h"
#include "engine/refl/refl.h"

#include <cstdint>
#include <string>
#include <vector>

// One level of the explored automap: the grid as retail packs it
struct SAutoMapLevel
{
    REFL_IDS(1, 9)
    PROP(1, "Map level")                                  int32_t              level = 0;
    PROP(2, "The visited grid, packed words little-endian") std::vector<uint8_t> mask;
};

// The explored automap (TPlayer::Load @ 0x0051b960, the record)
struct SAutoMapRecord
{
    REFL_IDS(1, 9)
    PROP(1, "The module it belongs to")                   std::string                module;
    PROP(2, "The explored levels")                        std::vector<SAutoMapLevel> levels;
};

class TAutomapComponent final : public TComponent
{
  public:
    DEF_COMPONENT("automap")
    [[nodiscard]] const char* ComponentName() const override { return "automap"; }

    PROP(1, "The explored levels")                        SAutoMapRecord record;   // +0x314
};
