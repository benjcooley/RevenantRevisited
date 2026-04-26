// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *                editor.h - ImGui-based in-game editor                  *
// *************************************************************************
//
// Replaces the retail TPane-based editor (now in attic/src/editor.{h,cpp})
// with an ImGui-driven UI. F12 toggles the editor on/off.
//
// Layout: a docked-panel system separated by drag-able splitters.
//
//   +------------------------------------------------------+  <- menu bar
//   | Scene |              Game View              | Props  |
//   |       |  (center hole; renderer composites  |        |
//   |       |   the game image at engine          |        |
//   |       |   resolution, centered)             |        |
//   |-------+-------------------------------------+--------+
//   |       |       Console / Palette             |        |
//   +------------------------------------------------------+
//
// All splitter positions are stored in EditorPrefs (editor.def). Adding
// new panels / state is "trivial" via EditorPrefs::Get/Set -- delete
// the file and the editor reopens with default geometry.
//
// *************************************************************************

#pragma once

#include "revenant.h"

#include <cstdint>

// Read editor.def into EditorPrefs (best-effort). Called once during
// boot from TPlayScreen::Initialize.
void EditorLoadState();

// Save the current preference store (panel sizes, etc.) back to disk.
void EditorSaveState();

// F12 transitions. EditorEnter activates the editor layout; EditorExit
// reverts to the fullscreen game view + saves prefs.
void EditorEnter();
void EditorExit();

// Per-frame ImGui chrome. Called after the world render so panels draw
// over the game view. No-op when Editor is off.
void EditorDrawChrome();

// Game-view sub-rect in current swapchain pixel space. Default
// (Editor=off) returns the full window. With Editor=on returns the
// center hole between the scene / properties / console panels.
struct SEditorRect { int32_t x, y, w, h; };
SEditorRect EditorGameViewRect();
