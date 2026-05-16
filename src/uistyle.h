// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *                 uistyle.h - UI style asset + push/pop stack            *
// *************************************************************************
//
// One struct of UI style primitives (colors, paddings, spacings, default
// 9-slice atom references, font role choices). Modeled on ImGui's style
// struct and Unity's style-asset pattern. NOT a themable-component cascade
// -- components read from the single active UIStyle directly. See memory
// project-ui-layout-system for the "what NOT to build" list.
//
// Stack discipline mirrors ImGui::PushStyleVar:
//
//   UIStyle alt = UI::Style();
//   alt.color_text = 0xFFFF0000u;
//   UI::PushStyle(alt);
//   // ... draw things that read UI::Style() ...
//   UI::PopStyle();
//
// One global stack; one active style at a time. Default-constructed
// UIStyle sits at the base of the stack -- never popped.
//
// *************************************************************************

#pragma once

#include "screen.h"   // SSpacing

#include <cstdint>

class TBitmap;
class TFont;

struct UIStyle
{
    // Text colors (RGBA8 packed; A.2e ships unused -- consumers come in B.x).
    uint32_t color_text          = 0xFFFFFFFFu;   // primary
    uint32_t color_text_dim      = 0xFF999999u;   // secondary / muted
    uint32_t color_text_accent   = 0xFFFFD850u;   // highlights, emphasis
    uint32_t color_text_disabled = 0xFF555555u;

    // Panel/background colors (premultiplied or not is a B.x concern).
    uint32_t color_panel_bg      = 0x00000000u;   // transparent by default

    // Default paddings / spacings used by container panes that don't
    // override them explicitly. Matches the SSpacing convention.
    SSpacing button_padding{6, 4, 6, 4};
    SSpacing panel_padding{8, 8, 8, 8};
    int32_t  item_spacing  = 4;

    // 9-slice atom references for default backgrounds. Null = no bg --
    // the pane just doesn't draw a backdrop. Populated by consumers (B.x)
    // when they load the retail widget atlas.
    TBitmap* button_bg = nullptr;
    TBitmap* panel_bg  = nullptr;
    TBitmap* dialog_bg = nullptr;

    // Font role choices. Null = "use whatever the pane decides" -- a
    // pane that wants the active style font for its body text reads
    // font_body; if null, it falls back to its own logic. Populated by
    // consumers in B.x once TFontTable wiring tells us which font to
    // call which role.
    TFont* font_label = nullptr;
    TFont* font_title = nullptr;
    TFont* font_body  = nullptr;
};

namespace UI {

// Returns the active style (top of the stack). Always safe -- the base
// of the stack is a default-constructed UIStyle that's never popped.
UIStyle& Style();

// Push a new style onto the stack. PushStyle copies; modifying the
// argument after pushing does not affect the active style. Pair with
// PopStyle in the same logical scope.
void PushStyle(const UIStyle& s);

// Pop the top style. No-op when only the base style remains.
void PopStyle();

// Stack depth. 1 = only the base default style is active. Useful for
// assertions in callers that want to confirm balanced push/pop.
size_t StyleDepth();

}  // namespace UI
