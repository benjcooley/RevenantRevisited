// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uisidebartest.h - --test=ui-sidebar                                  *
// *************************************************************************
//
// Integration harness for TSideTabsPane (6-button tab strip). The isolated
// --test=ui-sidebar mode can also draw representative selected-pane chrome
// as a visual aid; composed HUD/game modes disable that so the real content
// panes draw themselves, matching the retail ownership split.
//
// State lives in `SHudState` (see `hudstate.h`) so it is serializable per
// user note 2026-05-30 ("HUD state needs to be serializable. That should
// have been something we picked up in the forensics.").
//
// The command-line harness can synthesize state cycling; live PlayScreen
// leaves that off so mouse clicks, saved state, and remapped commands own
// pane visibility.
//
// *************************************************************************

#pragma once

#include <cstdint>

bool InitializeUISidebarMode();
void RenderUISidebarMode();
void RenderUISidebarModeEmbedded();
void CloseUISidebarMode();

// The sidebar's automatic panel-cycling driver is only for command-line
// harness modes such as --test=ui-sidebar / --test=ui-hud. Live PlayScreen
// keeps this disabled so real input / saved state owns the HUD.
void SetUISidebarSyntheticStateEnabled(bool enabled);
void SetUISidebarContentChromeEnabled(bool enabled);
void ToggleUISidebarPanel();
void ToggleUIBottomPanel();

// Mouse-click dispatch — hit-tests the 6 tab buttons and updates
// SHudState.topSlot / .bottomSlot accordingly. Reuses the same coords the
// renderer uses, so any input-script that hovers (X,Y) over a button and
// emits left_down/left_up will switch that slot.
bool HandleMouseClickUISidebarModeConsumed(int32_t button, int32_t x, int32_t y);
bool HandleMouseMoveUISidebarModeConsumed(int32_t button, int32_t x, int32_t y);
void HandleMouseClickUISidebarMode(int32_t button, int32_t x, int32_t y);

// Keyboard dispatch — same end effect as the mouse click handler, but
// driven by retail-identified DispatchCommand IDs (cases 7/8/9/0xb/0xc)
// plus V (sidebar toggle) and B (bottom-bar toggle). Default bindings:
// V/B toggles; 1=Book/Spell, 2=Stats, 3=Equip/Inv, 4=Inv, 5=Map, 6=Spell.
// `key` is the VK_* / ASCII-letter code as delivered by testmodes.cpp's
// HandleKeyPress; `down` true = press, false = release (release ignored).
void HandleKeyPressUISidebarMode(int32_t key, bool down);
