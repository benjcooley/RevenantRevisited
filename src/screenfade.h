// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   screenfade.h - full-screen fade state shared by scripts             *
// *************************************************************************
//
// The script commands "fadescreenout" / "fadescreenin" (retail 0x00427e80 /
// 0x00427f60) drive a full-screen fade, and "wait screenfade" (retail wait
// type 6 in TScript) blocks a script until that fade has finished. This
// header is the seam between the two: the fade owner implements it, the
// script engine only asks whether a fade is still running.
#pragma once

// True while a screen fade (in either direction) is still in progress.
bool ScreenFadeInProgress();
