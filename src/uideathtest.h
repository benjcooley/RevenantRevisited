// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uideathtest.h - --test=ui-death mockup                               *
// *************************************************************************
//
// Clean-room reconstruction of the player-death modal (TDeathPane, retail
// class cls_0x5b93c4 + TDeathScreen wrapper cls_0x5b9374) built ONLY from
// docs/ui/forensics/DeathPane_SPEC.md. A full-screen 640x480 "Game Over"
// backdrop ("background" from death.dat) with three bitmap word-buttons --
// Restart / Load / Exit -- stamped on top at their bitmap registration points.
//
// Non-DEF screen (TButtonPane harness + bitmap-REG placement, not the .DEF UI
// system) -- belongs to the OOG/non-DEF reconstruction track.
//
// *************************************************************************

#pragma once

#include <cstdint>

bool InitializeUIDeathMode();
void RenderUIDeathMode();
void CloseUIDeathMode();
void HandleMouseMoveUIDeathMode(int32_t x, int32_t y);
void HandleMouseClickUIDeathMode(int32_t button, int32_t x, int32_t y);
