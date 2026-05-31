// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uiloadscreentest.h - --test=ui-loadscreen mockup                     *
// *************************************************************************
//
// Clean-room reconstruction of the boot/level-load progress screen
// (TLoadScreen, retail class cls_0x4485a0) built ONLY from
// docs/ui/forensics/LoadingScreen_SPEC.md. A full-screen 640x480 backdrop
// ("background" from loadbar.dat, with the baked "LOADING GAME" banner) plus a
// red progress bar ("bar", 584x48, alpha) that fills left->right as the
// per-mille accumulator ramps 0..1000.
//
// *************************************************************************

#pragma once

bool InitializeUILoadScreenMode();
void RenderUILoadScreenMode();
void CloseUILoadScreenMode();
