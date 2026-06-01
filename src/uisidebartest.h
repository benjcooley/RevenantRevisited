// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uisidebartest.h - --test=ui-sidebar                                  *
// *************************************************************************
//
// Integration harness: TSideTabsPane (6-button tab strip) + currently-
// selected top-slot pane (Equip / Spellbook / Stats) + currently-selected
// bottom-slot pane (Inv / Map / SpellCreate), all composed into the
// right-anchored sidebar block as the in-game HUD does it.
//
// State lives in `SHudState` (see `hudstate.h`) so it is serializable per
// user note 2026-05-30 ("HUD state needs to be serializable. That should
// have been something we picked up in the forensics.").
//
// v1: synthetic state cycling exercises the multiplex (no real mouse/key
// dispatch yet — those land in the full --test=ui-hud step).
//
// *************************************************************************

#pragma once

#include <cstdint>

bool InitializeUISidebarMode();
void RenderUISidebarMode();
void CloseUISidebarMode();

// Mouse-click dispatch — hit-tests the 6 tab buttons and updates
// SHudState.topSlot / .bottomSlot accordingly. Reuses the same coords the
// renderer uses, so any input-script that hovers (X,Y) over a button and
// emits left_down/left_up will switch that slot.
void HandleMouseClickUISidebarMode(int32_t button, int32_t x, int32_t y);

// Keyboard dispatch — same end effect as the mouse click handler, but
// driven by retail-identified DispatchCommand IDs (cases 7/8/9/0xb/0xc)
// plus V (sidebar toggle) and B (bottom-bar toggle). Default bindings:
// V/B toggles; 1=Book/Spell, 2=Stats, 3=Equip/Inv, 4=Inv, 5=Map, 6=Spell.
// `key` is the VK_* / ASCII-letter code as delivered by testmodes.cpp's
// HandleKeyPress; `down` true = press, false = release (release ignored).
void HandleKeyPressUISidebarMode(int32_t key, bool down);
