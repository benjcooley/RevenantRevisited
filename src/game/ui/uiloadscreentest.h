// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uiloadscreentest.h - --test=ui-loadscreen                            *
// *************************************************************************
//
// Shows the production loading bar (TLoadBarPane, src/loadscreen.h; retail
// cls_0x4485a0) with a timer ramping it, outside a game.
//
// *************************************************************************

#pragma once

bool InitializeUILoadScreenMode();
void RenderUILoadScreenMode();
void CloseUILoadScreenMode();
