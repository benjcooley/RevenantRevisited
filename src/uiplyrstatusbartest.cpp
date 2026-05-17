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

// Retail-derived layout constants (from Wave-3A extraction + sample_screen_1
// proportions). The retail panel is small: ~140px wide, ~52px tall. Portrait
// in the corner (top-left for player, top-right for target), 3 bars stacked
// to its inner side, name+level above the bars.
//
// Wave-3A coords from FUN_0054af20 slot 23 body:
//   - 3 value-text rects at (0, 0/14/28, 50, 14) — these are number labels
//     beside each bar (one per bar, 14px row height)
//   - bar fills at x=0x44=68 (LEFT) — i.e. bars start 68px in from left edge
//   - bar fills at x=pane_width - {0x88=136, 0x91=145, 0x79=121} (RIGHT)
//   - portrait at (0x40=64, 0)
//
// The retail pane stretches across the full screen width so the mirror
// math lands the right block at the right edge. For our mockup we use
// TWO separate small blocks anchored to the corners — same visual result,
// cleaner architecture.
constexpr int32_t kBlockW         = 200;    // panel block width (per side)
constexpr int32_t kBlockH         =  56;    // panel block height
constexpr int32_t kBlockMargin    =   4;    // inset from screen edge

constexpr int32_t kBarRowH        =  14;    // 0x0e
constexpr int32_t kValueTextW     =  32;    // numeric value text width
constexpr int32_t kBarFillH       =  10;
constexpr int32_t kBarFillW       =  88;    // bar fill width (tuned for kBlockW)
constexpr int32_t kPortraitSize   =  44;
constexpr int32_t kPortraitInset  =   4;    // portrait offset from outer corner

// Per-bar Wave-3A stagger: each bar's x offset relative to its track origin
// differs by a few px (0x88-0x79=15 across the 3). Skip the stagger in the
// mockup — it's only visible when bars have icons, not yet wired.

// Per-bar colors. Health = red, mana = purple, stamina = gold per
// CLASSIC_HUD_REFERENCE §1 (matches red-heart / blue-mana orb / yellow
// in the actual game per sample_screen_1.jpg).
struct SBarColor { uint8_t r, g, b; };
constexpr SBarColor kBarColors[3] = {
    { 0xC8, 0x20, 0x20 },   // health — red
    { 0x80, 0x40, 0xC0 },   // mana — purple
    { 0xE0, 0xB0, 0x10 },   // stamina — gold/orange
};

// Mockup pane visualizer. Paints the char-panel layout as TWO separate
// corner-anchored blocks: LEFT block at TopLeft (always visible), RIGHT
// block at TopRight (gated on synthetic "have target" cycle). Each block
// has the same internal structure: portrait at outer corner + 3 bars
// stacked vertically on the inner side + numeric value rects + name area.
//
// The retail pane is one full-screen-width pane with both blocks painted
// from a single draw method (Wave-3A's slot 23 body). Architecturally
// equivalent — the corner-anchored split is cleaner for the modern
// retained-mode TPane layout and produces the same visual.
class TPlyrStatusBarMockHud : public THudDrawable
{
public:
    void Draw() override
    {
        const int32_t screen_w = Display.Width();
        const double t = TTime::Time();

        // === LEFT (player) block — anchored TopLeft ===
        DrawBlock(/*outerX=*/kBlockMargin, /*isRight=*/false,
                  /*lv=*/{
                      0.6f + 0.3f * float(std::sin(t * 0.7)),
                      0.5f + 0.3f * float(std::sin(t * 0.9 + 1.0)),
                      0.7f + 0.2f * float(std::sin(t * 1.1 + 2.0)),
                  });

        // === RIGHT (target) block — anchored TopRight, gated on
        // synthetic 3s-on / 3s-off cycle with 0.5s fade at boundaries.
        const double tgt_phase = std::fmod(t, 6.0);
        const bool have_target = tgt_phase < 3.0;
        if (have_target)
        {
            float fade = 1.0f;
            if (tgt_phase < 0.5)       fade = float(tgt_phase / 0.5);
            else if (tgt_phase > 2.5)  fade = float((3.0 - tgt_phase) / 0.5);

            const int32_t outerX = screen_w - kBlockMargin - kBlockW;
            DrawBlock(outerX, /*isRight=*/true,
                      /*lv=*/{
                          0.4f + 0.3f * float(std::sin(t * 1.3 + 0.5)),
                          0.3f + 0.3f * float(std::sin(t * 1.5 + 1.5)),
                          0.5f + 0.2f * float(std::sin(t * 1.7 + 2.5)),
                      }, fade);
        }
    }

private:
    struct SBarLevels { float h, m, s; };

    // outerX = block's left edge (in screen coords)
    // isRight = whether this is the target block (mirror portrait + bar
    //           layout so portrait sits at the outer corner)
    void DrawBlock(int32_t outerX, bool isRight,
                   SBarLevels lv, float fadeAlpha = 1.0f)
    {
        const uint8_t aFrame = uint8_t(255 * fadeAlpha);
        const int32_t blockY = kBlockMargin;

        // Block background (dark interior + thin gold border placeholder
        // for the retail mosaic-surface backdrop frame).
        Renderer->DrawSolidRect(outerX, blockY, kBlockW, kBlockH,
                                20, 16, 8, uint8_t(220 * fadeAlpha));
        Renderer->DrawSolidRect(outerX, blockY, kBlockW, 1,
                                180, 140, 60, aFrame);
        Renderer->DrawSolidRect(outerX, blockY + kBlockH - 1, kBlockW, 1,
                                180, 140, 60, aFrame);
        Renderer->DrawSolidRect(outerX, blockY, 1, kBlockH,
                                180, 140, 60, aFrame);
        Renderer->DrawSolidRect(outerX + kBlockW - 1, blockY, 1, kBlockH,
                                180, 140, 60, aFrame);

        // Portrait box at the OUTER corner of the block.
        const int32_t px = isRight
            ? outerX + kBlockW - kPortraitInset - kPortraitSize
            : outerX + kPortraitInset;
        const int32_t py = blockY + kPortraitInset;
        Renderer->DrawSolidRect(px, py, kPortraitSize, kPortraitSize,
                                90, 70, 30, aFrame);
        // Portrait border
        Renderer->DrawSolidRect(px, py, kPortraitSize, 1, 180, 140, 60, aFrame);
        Renderer->DrawSolidRect(px, py + kPortraitSize - 1,
                                kPortraitSize, 1, 180, 140, 60, aFrame);
        Renderer->DrawSolidRect(px, py, 1, kPortraitSize, 180, 140, 60, aFrame);
        Renderer->DrawSolidRect(px + kPortraitSize - 1, py,
                                1, kPortraitSize, 180, 140, 60, aFrame);

        // Bars + values fill the inner half of the block (opposite the portrait).
        const int32_t innerX = isRight
            ? outerX + kPortraitInset
            : outerX + kPortraitInset + kPortraitSize + 4;
        const int32_t innerW = kBlockW - kPortraitSize - kPortraitInset * 2 - 4;

        const float lvls[3] = { lv.h, lv.m, lv.s };
        for (int i = 0; i < 3; ++i)
        {
            const int32_t rowY = blockY + 4 + i * kBarRowH;

            // value box + bar: value on the OUTER side of the row, bar on
            // the inner side (so bar+value pair reads consistently across
            // sides — values closer to the screen edge, bars closer to center).
            const int32_t valueX = isRight
                ? innerX + innerW - kValueTextW
                : innerX;
            const int32_t barXrow = isRight
                ? innerX
                : innerX + kValueTextW + 2;
            const int32_t fillWidth = innerW - kValueTextW - 2;

            // Value text placeholder
            Renderer->DrawSolidRect(valueX, rowY, kValueTextW, kBarFillH,
                                    10, 10, 18, aFrame);

            // Bar track
            Renderer->DrawSolidRect(barXrow, rowY, fillWidth, kBarFillH,
                                    30, 24, 16, aFrame);
            // Bar fill
            const int32_t fw = int32_t(float(fillWidth) * std::clamp(lvls[i], 0.0f, 1.0f));
            const auto& c = kBarColors[i];
            if (fw > 0)
            {
                // Fill side mirrors: LEFT side fills LTR, RIGHT side fills RTL.
                const int32_t fillX = isRight ? barXrow + fillWidth - fw : barXrow;
                Renderer->DrawSolidRect(fillX, rowY, fw, kBarFillH,
                                        c.r, c.g, c.b, aFrame);
            }
            // Track 1px borders
            Renderer->DrawSolidRect(barXrow, rowY, fillWidth, 1, 120, 100, 50, aFrame);
            Renderer->DrawSolidRect(barXrow, rowY + kBarFillH - 1,
                                    fillWidth, 1, 120, 100, 50, aFrame);
        }
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
