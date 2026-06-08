// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uistatstest.h - --test=ui-stats                                       *
// *************************************************************************
//
// Isolated harness for TStatPane (the right-sidebar Stats/character sheet
// shown when the Stats tab is selected). The retail pane is DEF-driven by
// statpane.def — chrome bitmap "Stats" (188x306) blitted opaque, then a
// walk of the CLASS "Player" FORMAT "Page1" block emits cream / aqua /
// teal / red / blue / green / yellow GDI text with a 3-pass black drop
// shadow (right + down, the §6b shape).
//
// This test mode draws the static chrome plus a representative sample of
// the POS/TAB/NEXTLINE-driven lines so the layout (positions, colors,
// shadow, right-align of values) is visually verifiable; the full DEF
// interpreter is a separate engine task (statpane.def §6a).
//
// Built from docs/ui/forensics/CharacterStatsPane_SPEC.md.
//
// *************************************************************************

#pragma once

bool InitializeUIStatsMode();
void RenderUIStatsMode();
void RenderUIStatsModeEmbedded();
void SetUIStatsModeVisible(bool visible);
void CloseUIStatsMode();
