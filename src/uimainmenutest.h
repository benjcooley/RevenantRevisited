// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uimainmenutest.h - --test=ui-mainmenu mockup                         *
// *************************************************************************
//
// Clean-room reconstruction of the title / main-menu screen (TLogoScreen,
// retail class cls_0x5a5d18) built ONLY from docs/ui/forensics/MainMenu_SPEC.md.
// A full-screen 640x480 backdrop ("MainMenu" from menus.dat) + five word
// buttons (New Game / Load Game / Multiplayer / Options / Exit), each a baked
// sprite positioned by its U-sprite registration point, + the bottom-left
// "Revenant  v1.22" version text.
//
// Non-DEF screen (dispatches via TButtonPane + direct globals, not the .DEF UI
// system) -- belongs to the OOG/non-DEF reconstruction track.
//
// *************************************************************************

#pragma once

#include <cstdint>

bool InitializeUIMainMenuMode();
void RenderUIMainMenuMode();
void CloseUIMainMenuMode();
void HandleMouseMoveUIMainMenuMode(int32_t x, int32_t y);
void HandleMouseClickUIMainMenuMode(int32_t button, int32_t x, int32_t y);
