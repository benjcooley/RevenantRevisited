// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uiplyrstatusbartest.cpp - --test=ui-plyrstatusbar mockup             *
// *************************************************************************
//
// See uiplyrstatusbartest.h. Layout numbers come from Wave-3A's extraction
// of FUN_0054af20 (cls_0x5a54e4 vtable slot 23, the BIG two-pass paint):
//
//   LEFT (player) block — fixed coords, painted unconditionally when
//     DAT_00667fcc (current player) != 0:
//       - 3 value-text rects at (0, 0/14/28, 50, 14)  // health/mana/stamina
//       - portrait region near (64, 0)
//       - 3 bar fills at x=68 (0x44), stacked vertically
//
//   RIGHT (target) block — mirrored coords, painted only when
//     DAT_00667fcc[0x38][0x11] (player→target→character) != 0:
//       - 3 bar fills at x = pane_width + {-0x88, -0x91, -0x79}
//                       = pane_width - {136, 145, 121}
//       - portrait + name area further left of that
//
// The retail width-relative right-side coords mean the pane is wide enough
// to span across both anchored content regions. For the mockup we use a
// full-screen-width pane anchored to the top edge.
//
// The two-pass-draw architecture is the iconic Revenant face-off framing:
// player upper-left, current target upper-right, like a Street Fighter
// life bar layout. Target appears/disappears as the player switches enemies.
// Mockup animates this with a 3-second on/off cycle to verify the mirror
// math.
//
// *************************************************************************

#include "uiplyrstatusbartest.h"

#include "display.h"
#include "logging.h"
#include "renderer.h"
#include "time.h"

#include <cmath>
#include <cstdint>

namespace {

// Retail-derived layout constants (from Wave-3A extraction).
constexpr int32_t kPanelHeight     = 64;
constexpr int32_t kBarRowH         = 14;     // 0x0e
constexpr int32_t kValueTextW      = 50;     // 0x32 — numeric value text width
constexpr int32_t kBarFillH        = 10;     // visible bar fill height (< row height)
constexpr int32_t kBarFillW        = 80;     // approximation — bar fill width
constexpr int32_t kLeftBarX        = 0x44;   // 68 — bar fill x (LEFT block)
constexpr int32_t kPortraitSize    = 56;
constexpr int32_t kPortraitLeftX   = 0x40;   // 64 — portrait region (LEFT)
constexpr int32_t kPortraitLeftY   = 4;

// RIGHT (target) block — mirrored bar fill offsets from pane right edge.
// Wave-3A: x = pane_width - {0x88, 0x91, 0x79}. Three different offsets
// for the three bars (presumably for visual stagger / icon placement).
constexpr int32_t kRightBarOff[3]  = { 0x88, 0x91, 0x79 };  // 136, 145, 121

// Per-bar colors. Health = red heart, mana = purple gem, stamina = yellow/orange
// per CLASSIC_HUD_REFERENCE §1.
struct SBarColor { uint8_t r, g, b; };
constexpr SBarColor kBarColors[3] = {
    { 0xC8, 0x20, 0x20 },   // health — red
    { 0x80, 0x40, 0xC0 },   // mana — purple
    { 0xE0, 0xB0, 0x10 },   // stamina — gold/orange
};

// Mockup pane visualizer. Paints the entire char-panel layout in solid
// rects at the retail-derived coords. Synthetic stats (sine-cycled) so
// the bars animate. Target block toggles on/off on a 3-second cycle to
// verify the two-pass-draw architecture renders correctly.
class TPlyrStatusBarMockHud : public THudDrawable
{
public:
    void Draw() override
    {
        const int32_t panel_w = Display.Width();
        const int32_t panel_h = kPanelHeight;
        const int32_t panel_x = 0;
        const int32_t panel_y = 0;

        const double t = TTime::Time();

        // Background frame (semi-transparent gold border, dark interior).
        // Placeholder for the retail mosaic-surface backdrop.
        Renderer->DrawSolidRect(panel_x, panel_y, panel_w, panel_h,
                                20, 16, 8, 200);
        // 1px top/bottom border to make the panel rect visible
        Renderer->DrawSolidRect(panel_x, panel_y + panel_h - 1,
                                panel_w, 1, 180, 140, 60, 255);

        // === LEFT (player) block ===
        DrawSide(/*originX=*/0, /*rightEdgeX=*/0, /*isRight=*/false,
                 /*levels=*/{
                     0.6f + 0.3f * float(std::sin(t * 0.7)),       // health
                     0.5f + 0.3f * float(std::sin(t * 0.9 + 1.0)), // mana
                     0.7f + 0.2f * float(std::sin(t * 1.1 + 2.0)), // stamina
                 });

        // === RIGHT (target) block — gated on the synthetic "have target"
        // cycle. 3-second period, target visible for first half.
        const double tgt_phase = std::fmod(t, 6.0);
        const bool have_target = tgt_phase < 3.0;
        if (have_target)
        {
            // Optional fade for the smooth-swap counter (Wave-3A: animation
            // counters at +0xd4/+0xdc ramp toward 6, suggesting fade alpha).
            float fade = 1.0f;
            if (tgt_phase < 0.5)       fade = float(tgt_phase / 0.5);
            else if (tgt_phase > 2.5)  fade = float((3.0 - tgt_phase) / 0.5);

            DrawSide(/*originX=*/panel_w, /*rightEdgeX=*/panel_w, /*isRight=*/true,
                     /*levels=*/{
                         0.4f + 0.3f * float(std::sin(t * 1.3 + 0.5)),
                         0.3f + 0.3f * float(std::sin(t * 1.5 + 1.5)),
                         0.5f + 0.2f * float(std::sin(t * 1.7 + 2.5)),
                     }, fade);
        }
    }

private:
    struct SBarLevels { float h, m, s; };

    void DrawSide(int32_t originX, int32_t rightEdgeX, bool isRight,
                  SBarLevels lv, float fadeAlpha = 1.0f)
    {
        const uint8_t aFrame = uint8_t(255 * fadeAlpha);

        // Portrait box.
        const int32_t px = isRight
            ? rightEdgeX - kPortraitLeftX - kPortraitSize
            : originX + kPortraitLeftX;
        Renderer->DrawSolidRect(px, kPortraitLeftY,
                                kPortraitSize, kPortraitSize,
                                90, 70, 30, aFrame);
        // 1px portrait border
        Renderer->DrawSolidRect(px, kPortraitLeftY,
                                kPortraitSize, 1, 180, 140, 60, aFrame);
        Renderer->DrawSolidRect(px, kPortraitLeftY + kPortraitSize - 1,
                                kPortraitSize, 1, 180, 140, 60, aFrame);

        // Three bars (health, mana, stamina) — track + fill.
        const float lvls[3] = { lv.h, lv.m, lv.s };
        for (int i = 0; i < 3; ++i)
        {
            const int32_t y = i * kBarRowH + 2;

            // Bar X depends on side.
            int32_t bx;
            if (isRight)
            {
                // Wave-3A: each bar has its own offset from right edge
                bx = rightEdgeX - kRightBarOff[i];
            }
            else
            {
                bx = originX + kLeftBarX;
            }

            // Track (dark)
            Renderer->DrawSolidRect(bx, y, kBarFillW, kBarFillH,
                                    30, 24, 16, aFrame);
            // Fill (per-bar color, scaled by level)
            const int32_t fw = int32_t(float(kBarFillW) * std::clamp(lvls[i], 0.0f, 1.0f));
            const auto& c = kBarColors[i];
            if (fw > 0)
                Renderer->DrawSolidRect(bx, y, fw, kBarFillH,
                                        c.r, c.g, c.b, aFrame);
            // 1px top/bottom border on track
            Renderer->DrawSolidRect(bx, y, kBarFillW, 1, 120, 100, 50, aFrame);
            Renderer->DrawSolidRect(bx, y + kBarFillH - 1,
                                    kBarFillW, 1, 120, 100, 50, aFrame);

            // Numeric value text placeholder — small rect next to the bar
            // on the inside (away from the portrait).
            const int32_t vx = isRight ? bx - kValueTextW - 2 : bx + kBarFillW + 2;
            Renderer->DrawSolidRect(vx, y, kValueTextW, kBarFillH,
                                    10, 10, 18, aFrame);
        }

        // Player/target name area placeholder — slim rect above bars.
        const int32_t nameX = isRight
            ? rightEdgeX - kPortraitLeftX - kPortraitSize - kValueTextW - 4
            : originX + kPortraitLeftX + kPortraitSize + 4;
        Renderer->DrawSolidRect(nameX, 0, kValueTextW + 60, kBarRowH,
                                40, 40, 80, aFrame);
    }
};

TPlyrStatusBarMockHud g_hud;

}  // namespace

bool InitializeUIPlyrStatusBarMode()
{
    log_info("[ui-plyrstatusbar] === TPlyrStatusBar layout mockup ===");
    log_info("[ui-plyrstatusbar] Pane spans full screen width, anchored top.");
    log_info("[ui-plyrstatusbar] LEFT block (player) painted unconditionally;");
    log_info("[ui-plyrstatusbar] RIGHT block (target) toggles 3s on / 3s off");
    log_info("[ui-plyrstatusbar]   with 0.5s fade in/out at the boundaries");
    log_info("[ui-plyrstatusbar]   (placeholder for the +0xd4/+0xdc animation");
    log_info("[ui-plyrstatusbar]   counters in retail per Wave-3A).");
    log_info("[ui-plyrstatusbar] Layout coords sourced from FUN_0054af20");
    log_info("[ui-plyrstatusbar]   (slot 23 two-pass paint, per B_r8 brief).");

    Renderer->AddHud(&g_hud, 0.0f);
    return true;
}

void RenderUIPlyrStatusBarMode()
{
    // Soft dark background so the panel reads against it.
    const double t = TTime::Time();
    const float r = 0.08f + 0.02f * float(std::sin(t * 0.5));
    const float g = 0.10f + 0.02f * float(std::sin(t * 0.7 + 1.0));
    const float b = 0.14f + 0.02f * float(std::sin(t * 0.9 + 2.0));
    Display.BackBuffer()->StartPass(r, g, b, 1.0f);
    Display.BackBuffer()->EndPass();
}

void CloseUIPlyrStatusBarMode()
{
    Renderer->RemoveHud(&g_hud);
}
