// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *       uistyletest.cpp - --test=ui-style: UIStyle push/pop stack       *
// *************************************************************************
//
// See uistyletest.h.
//
// *************************************************************************

#include "uistyletest.h"

#include "display.h"
#include "logging.h"
#include "time.h"
#include "uistyle.h"

#include <cmath>

namespace {

void LogStyle(const char* label)
{
    const UIStyle& s = UI::Style();
    log_info("[ui-style] %s (depth=%zu)", label, UI::StyleDepth());
    log_info("[ui-style]   color_text          = 0x%08X", s.color_text);
    log_info("[ui-style]   color_text_dim      = 0x%08X", s.color_text_dim);
    log_info("[ui-style]   color_text_accent   = 0x%08X", s.color_text_accent);
    log_info("[ui-style]   color_panel_bg      = 0x%08X", s.color_panel_bg);
    log_info("[ui-style]   button_padding      = L%d T%d R%d B%d",
             s.button_padding.left, s.button_padding.top,
             s.button_padding.right, s.button_padding.bottom);
    log_info("[ui-style]   panel_padding       = L%d T%d R%d B%d",
             s.panel_padding.left, s.panel_padding.top,
             s.panel_padding.right, s.panel_padding.bottom);
    log_info("[ui-style]   item_spacing        = %d", s.item_spacing);
}

}  // namespace

bool InitializeUIStyleMode()
{
    log_info("[ui-style] === base style (default) ===");
    LogStyle("base");

    log_info("[ui-style] === push override: red text, bigger padding ===");
    UIStyle alt = UI::Style();
    alt.color_text     = 0xFFFF4444u;
    alt.color_text_dim = 0xFFCC2222u;
    alt.panel_padding  = SSpacing(16, 12, 16, 12);
    alt.item_spacing   = 8;
    UI::PushStyle(alt);
    LogStyle("after push #1");

    log_info("[ui-style] === push another: green text, tiny padding ===");
    UIStyle alt2 = UI::Style();
    alt2.color_text    = 0xFF44FF44u;
    alt2.panel_padding = SSpacing(2, 2, 2, 2);
    UI::PushStyle(alt2);
    LogStyle("after push #2");

    log_info("[ui-style] === pop ===");
    UI::PopStyle();
    LogStyle("after pop #1 (back to push #1 state)");

    log_info("[ui-style] === pop ===");
    UI::PopStyle();
    LogStyle("after pop #2 (back to base)");

    log_info("[ui-style] === pop again (should be no-op) ===");
    UI::PopStyle();
    LogStyle("after extra pop (still base)");

    return true;
}

void RenderUIStyleMode()
{
    const double t = TTime::Time();
    const float r = 0.18f + 0.04f * float(std::sin(t * 0.6));
    const float g = 0.14f + 0.04f * float(std::sin(t * 0.8 + 1.0));
    const float b = 0.10f + 0.04f * float(std::sin(t * 1.0 + 2.0));
    Display.BackBuffer()->StartPass(r, g, b, 1.0f);
    Display.BackBuffer()->EndPass();
}

void CloseUIStyleMode()
{
}
