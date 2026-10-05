// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uiquickspelltest.h - --test=ui-quickspell                            *
// *************************************************************************
//
// Isolated harness for TQuickSpellPane (cls_0x5a5a30) — the 4-slot
// quick-spell ring strip at the left of the bottom utility bar. Built from
// QuickSpellPane_SPEC.md using the real retail Ring{U,D,G} sprites from
// SpellIcons.dat plus the 40x40 spell circle icons of the main player's
// quick spells.
//
// *************************************************************************

#pragma once

#include <cstdint>

bool InitializeUIQuickSpellMode();
void RenderUIQuickSpellMode();
void RenderUIQuickSpellModeEmbedded();
void SetUIQuickSpellSyntheticStateEnabled(bool enabled);
void CloseUIQuickSpellMode();
void HandleMouseClickUIQuickSpellMode(int32_t button, int32_t x, int32_t y);
void HandleMouseMoveUIQuickSpellMode(int32_t button, int32_t x, int32_t y);

