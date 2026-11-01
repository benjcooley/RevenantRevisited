// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   spellbookcomponent.h - the spells a player knows and keeps ready   *
// *************************************************************************
//
// The talisman codes learned and the quick spells (the composer's and the
// four buttons'). Every player has one from construction, reached through
// Spellbook(). Both are the player's record (SPlayerState: known spells
// and quick spells as lists of strings); the version counts changes to
// the known spells for the spellbook pane. The quick spells keep retail's
// fixed buffers, which the HUD's composer writes into.
//
// *************************************************************************

#pragma once

#include "engine/scene/component.h"
#include "game/spells/spell.h"       // MAXTALISMANLEN

#include <array>
#include <vector>

// Quick spell buttons
constexpr int32_t QSPELL_CONSTRUCT = 0;
constexpr int32_t QSPELL_1         = 1;
constexpr int32_t QSPELL_2         = 2;
constexpr int32_t QSPELL_3         = 3;
constexpr int32_t QSPELL_4         = 4;
constexpr int32_t QSPELL_NUM       = 5;

// A known spell: its talisman code, as retail stored it.
constexpr int32_t kSpellCodeBytes = 6;
using TSpellCode = std::array<char, kSpellCodeBytes + 1>;

class TSpellbookComponent final : public TComponent
{
  public:
    [[nodiscard]] const char* ComponentName() const override { return "spellbook"; }

    char quickspells[QSPELL_NUM][MAXTALISMANLEN] = {};   // the composer's, then the four buttons'
    std::vector<TSpellCode> known;                        // +0x2ec: talisman codes learned
    int32_t version = 0;                                  // counts changes to `known`
};
