// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           editorfonts.h - Editor font atlas (Inter + JetBrains)       *
// *************************************************************************
//
// Builds the ImGui font atlas before the first editor frame:
//   - Inter (proportional UI default; loaded as `font_default`)
//   - Material Symbols Outlined merged into the default at the same size,
//     covering U+E000..U+F8FF (private use area icons via IconFontCppHeaders)
//   - JetBrains Mono Regular (separate, accessed via GetMonoFont() for
//     Log / Console panels)
//
// Caller order:
//   simgui_desc_t d{ .no_default_font = true, ... };
//   simgui_setup(&d);
//   EditorFonts::Build();   // populates atlas + uploads renderer texture
//
// *************************************************************************

#pragma once

#include <imgui.h>

namespace EditorFonts {

// Build the font atlas + renderer texture. Safe to call once after simgui_setup.
// Falls back to ImGui default font on any failure (so the app still runs
// if the TTFs aren't on disk).
void Build();
void Shutdown();

// Pointer to the JetBrains Mono font for log / console panels. Returns
// the default font when the mono font failed to load.
ImFont* GetMonoFont();

// Material Symbols rasterised at a larger size for toolbar / icon-only
// buttons. Push around an icon group with ImGui::PushFont / PopFont so
// glyphs render crisply instead of being upscaled by SetWindowFontScale.
ImFont* GetIconFont();

} // namespace EditorFonts
