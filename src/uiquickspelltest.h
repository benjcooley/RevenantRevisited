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

bool InitializeUIQuickSpellMode();
void RenderUIQuickSpellMode();
void CloseUIQuickSpellMode();
