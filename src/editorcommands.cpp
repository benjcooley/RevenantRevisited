// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           editorcommands.cpp - Editor command registry                *
// *************************************************************************

#include "editorcommands.h"

#include <imgui.h>

namespace {

std::vector<SEditorCommand>& Storage()
{
    static std::vector<SEditorCommand> s;
    return s;
}

} // namespace

void EditorCommands::Register(SEditorCommand cmd)
{
    // Replace if a command with the same id already exists -- handy when
    // a hot-reloaded module re-registers its commands.
    auto& s = Storage();
    for (auto& existing : s) {
        if (existing.id == cmd.id) { existing = std::move(cmd); return; }
    }
    s.push_back(std::move(cmd));
}

const std::vector<SEditorCommand>& EditorCommands::All() { return Storage(); }

const SEditorCommand* EditorCommands::Find(const char* id)
{
    for (const auto& c : Storage())
        if (c.id == id) return &c;
    return nullptr;
}

void EditorCommands::DispatchHotkeys()
{
    const ImGuiIO& io = ImGui::GetIO();

    for (const auto& c : Storage()) {
        if (!c.execute || c.chord == 0) continue;
        // Only suppress mod-less hotkeys (F, Q, W, ...) while a text
        // field has focus -- otherwise typing in the property panel
        // would switch tools. Modifier-bearing chords (Cmd+Z, etc.)
        // are unambiguous and should still fire; standard Mac apps
        // route Cmd+Z to the focused undo manager too.
        const bool has_mods = (c.chord & ImGuiMod_Mask_) != 0;
        if (io.WantTextInput && !has_mods) continue;
        if (c.is_enabled && !c.is_enabled()) continue;
        if (ImGui::IsKeyChordPressed(c.chord))
            c.execute();
    }
}

void EditorCommands::DrawMenuItems(const char* menu_path)
{
    for (const auto& c : Storage()) {
        if (c.menu_path != menu_path) continue;
        const bool enabled = !c.is_enabled || c.is_enabled();
        const bool checked = c.is_checked && c.is_checked();
        const char* sc     = c.shortcut.empty() ? nullptr : c.shortcut.c_str();
        if (ImGui::MenuItem(c.label.c_str(), sc, checked, enabled) && c.execute)
            c.execute();
    }
}
