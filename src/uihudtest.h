// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uihudtest.h - --test=ui-hud                                          *
// *************************************************************************
//
// Full HUD assembly: composes all per-panel test modes into one screen so
// the assembled in-game HUD layout can be verified end-to-end.
//
//   Top edge:    TPlyrStatusBar (player + target chips)
//   Right edge:  Sidebar = TSideTabsPane + top-slot pane + bottom-slot pane
//   Bottom edge: TBottomBarPane chrome + TBarInvPane shelf + TQuickSpellPane
//   Overlay:     TTextBar (message log)
//
// Each existing per-panel test mode already registers a THudDrawable with
// Renderer->AddHud at its correct screen-anchored position. We Initialize
// them in sequence so all their HUDs paint together; this orchestrator's
// own Render does just the backdrop clear.
//
// *************************************************************************

#pragma once

bool InitializeUIHudMode();
void RenderUIHudMode();
void CloseUIHudMode();
