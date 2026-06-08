// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uibottombartest.h - --test=ui-bottombar                              *
// *************************************************************************
//
// Isolated harness for TBottomBarPane: the full-width 60px dark chrome strip
// pinned to the bottom of the play screen. Renders the two chrome bitmaps —
// UtilityBar (640x60, X-stretched to display width) + BarEndCap (10x60 at the
// right edge) — using the real retail bottombar.dat assets. Built from
// docs/ui/forensics/BottomBarPane_SPEC.md (clean-room, no engine code authored
// outside the spec contract).
//
// *************************************************************************

#pragma once

bool InitializeUIBottomBarMode();
void RenderUIBottomBarMode();
void RenderUIBottomBarModeEmbedded();
void CloseUIBottomBarMode();
