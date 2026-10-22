// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *        spritedebugtab.h - the debug panel's Sprites tab               *
// *************************************************************************
//
// The tab owns the art knobs: which art the game shows (retail, or the
// modern art from the local trees) and the buttons that re-read the art so
// an edit shows up without leaving the game. It is drawn as a top-level
// tab after the contributors (debugui.cpp) and has two sections: the
// location's tile art (tiletreload.h) and the 3D models' textures
// (spritetexoverride.h).

#pragma once

// Draws the tab's contents (no BeginTab wrappers of its own).
void DrawSpritesTab();
