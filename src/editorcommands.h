// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           editorcommands.h - Editor command registry                  *
// *************************************************************************
//
// One global registry of editor commands. Each command is just a bag of
// std::function<>'s plus metadata: human label, optional menu path,
// optional ImGui hotkey chord. Register a command from anywhere with:
//
//   EditorCommands::Register({
//       .id          = "View.Frame",
//       .label       = "Frame Selection",
//       .menu_path   = "View",
//       .chord       = ImGuiKey_F,
//       .shortcut    = "F",
//       .is_enabled  = []{ return !g_selection.empty(); },
//       .execute     = []{ /* ... */ },
//   });
//
// The editor's chrome calls DrawMenuItems(menu_path) inside each Begin
// Menu so menu placement is automatic, and DispatchHotkeys() once per
// frame so chords fire even when no menu is open.
//
// *************************************************************************

#pragma once

#include <functional>
#include <string>
#include <vector>

#include <imgui.h>

struct SEditorCommand
{
    std::string id;             // unique key, e.g. "View.Frame"
    std::string label;          // menu label
    std::string menu_path;      // top-level menu, e.g. "View" -- empty hides
    ImGuiKeyChord chord = 0;    // 0 = no hotkey
    std::string shortcut;       // display-only hint, e.g. "F" or "Ctrl+Z"
    std::function<bool()>  is_enabled;   // null = always enabled
    std::function<bool()>  is_checked;   // null = not a toggle
    std::function<void()>  execute;      // required
};

namespace EditorCommands {

void Register(SEditorCommand cmd);

const std::vector<SEditorCommand>& All();
const SEditorCommand* Find(const char* id);

// Pump global hotkeys. Skipped when ImGui has keyboard focus on a text
// input. Call once per editor frame, before menus.
void DispatchHotkeys();

// Render every command whose menu_path matches; caller owns the
// surrounding BeginMenu / EndMenu.
void DrawMenuItems(const char* menu_path);

} // namespace EditorCommands
