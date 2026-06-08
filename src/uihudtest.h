// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uihudtest.h - --test=ui-hud                                          *
// *************************************************************************
//
// Full HUD assembly: composes all per-panel test modes into one screen so
// the assembled in-game HUD layout can be verified end-to-end.
//
//   Top edge:    TPlyrStatusBar (player + target chips)
//   Right edge:  TSideTabsPane + selected top-slot pane + selected bottom-slot pane
//   Bottom edge: TBottomBarPane chrome + TBarInvPane shelf + TQuickSpellPane
//   Overlay:     TTextBar (message log)
//
// Each content pane composes into its own render texture and registers a
// THudDrawable at its screen-anchored position. The orchestrator refreshes
// only the panes selected by SHudState.
//
// *************************************************************************

#pragma once

bool InitializeUIHudMode();
void RenderUIHudMode();
void RenderUIHudModeEmbedded();
void CloseUIHudMode();

// Command-line harness helper. Live PlayScreen leaves this off because
// GameMode already registers the cursor / drag overlay.
void SetUIHudCursorOverlayEnabled(bool enabled);
