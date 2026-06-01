// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uiquickspelltest.h - --test=ui-quickspell                            *
// *************************************************************************
//
// Isolated harness for TQuickSpellPane (cls_0x5a5a30) — the 4-slot
// quick-spell ring strip at the left of the bottom utility bar. Built from
// QuickSpellPane_SPEC.md using the real retail Ring{U,D,G} sprites from
// SpellIcons.dat plus the 40x40 spell circle icons keyed by spell name.
//
// *************************************************************************

#pragma once

#include <cstdint>

struct TBitmap;

bool InitializeUIQuickSpellMode();
void RenderUIQuickSpellMode();
void CloseUIQuickSpellMode();
void HandleMouseClickUIQuickSpellMode(int32_t button, int32_t x, int32_t y);

// Bind a spell into a quickspell slot from an external drag source.
// slot in [0, 3]; spellName is the SpellIcons.dat key; icon is the
// resolved 40×40 bitmap from SpellIcons.dat (may be nullptr = clear).
//
// Production integration note (Agent A):
//   Add char quickspellBindings[4][64] to SHudState so this survives
//   save/load. QuickSpell_BindSlot writes that array in production; in the
//   test harness it uses a local g_quickspellBindings[].
void QuickSpell_BindSlot(int32_t slot, const char* spellName, TBitmap* icon);
