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

#include "bitmap.h"
#include "display.h"
#include "logging.h"
#include "multi.h"
#include "renderer.h"
#include "revenant.h"
#include "time.h"

#include <cmath>
#include <cstdint>
#include <cstring>

namespace {

// Retail layout — verified by inspecting actual `texthealthbar` bitmap
// dimensions at runtime (200x11). The retail panel composes:
//   - 3 stacked `texthealthbar` blits, one per bar (health/mana/stamina)
//   - portrait bitmap (sourced from character data, not playscrn.dat)
//   - name + level text overlay
//
// Wave-3A slot 23 body extraction:
//   - 3 value-text rects at (0, 0/14/28, 50, 14) — value text PER bar
//   - bars stacked vertically at 14px row pitch (0x0e)
//   - bar fills at x=0x44=68 from pane origin (LEFT block)
//   - portrait at (0x40=64, 0) from pane origin
//
// No single "whole panel" backdrop bitmap exists in playscrn.dat — the
// texthealthbar is THE bar-element bitmap, used 3x stacked.
constexpr int32_t kBlockMargin    =   4;    // inset from screen edge
constexpr int32_t kBarRowPitch    =  14;    // Wave-3A row height per bar
constexpr int32_t kPortraitSize   =  64;    // 0x40 - portrait surface allocation per recon
constexpr int32_t kPortraitInset  =   4;

// Bar-fill rect inside the texthealthbar (200x11) bitmap. The bitmap
// has a baked frame + icon area; the fill region is the inset slot
// where the colored bar appears. These offsets are visual estimates
// until the bitmap is inspected pixel-by-pixel.
constexpr int32_t kBarFillInsetX  = 36;     // inset from bar bitmap left
constexpr int32_t kBarFillInsetY  =  2;     // inset from bar bitmap top
constexpr int32_t kBarFillW       = 160;    // bar bitmap is 200 wide, leave 36+4 margin
constexpr int32_t kBarFillH       =  7;     // bar bitmap is 11 tall, leave 2+2 margin

// Per-bar colors. Health = red, mana = purple, stamina = gold per
// CLASSIC_HUD_REFERENCE §1.
struct SBarColor { uint8_t r, g, b; };
constexpr SBarColor kBarColors[3] = {
    { 0xC8, 0x20, 0x20 },   // health — red
    { 0x80, 0x40, 0xC0 },   // mana — purple
    { 0xE0, 0xB0, 0x10 },   // stamina — gold/orange
};

// Cached assets from playscrn.dat.
PTBitmap g_textHealthBar = nullptr;   // 200x11 single-bar backdrop
PTBitmap g_lifeIcon      = nullptr;   // 24x21 heart icon for health bar

// Mockup pane visualizer. The panel for each side is:
//   - 3 stacked `texthealthbar` blits (200x11 each, 14px row pitch)
//   - portrait box at the outer corner (64x64 placeholder for now —
//     real portrait sourced from character data, not playscrn.dat)
//   - colored bar fill overlaid in the bar fill slot of each texthealthbar
//
// Mirror handling: the texthealthbar bitmap is authored for the LEFT
// (player) side. For the RIGHT (target) side we draw it at the mirrored
// X coordinate; the visual asymmetry (icon on left edge of bitmap) will
// show as "icons closer to inside of right panel" — that's exactly how
// retail does it per Wave-3A's slot 23 body (no flipped variant in the
// asset list; mirror is purely coordinate math).
class TPlyrStatusBarMockHud : public THudDrawable
{
public:
    void Draw() override
    {
        const int32_t screen_w = Display.Width();
        const double t = TTime::Time();

        // Block width = bar bitmap + portrait + small gap.
        const int32_t barW = g_textHealthBar ? g_textHealthBar->width : 200;
        const int32_t blockW = kPortraitSize + 4 + barW;

        // === LEFT (player) block — anchored TopLeft ===
        DrawBlock(/*outerX=*/kBlockMargin, blockW, barW, /*isRight=*/false,
                  /*lv=*/{
                      0.6f + 0.3f * float(std::sin(t * 0.7)),
                      0.5f + 0.3f * float(std::sin(t * 0.9 + 1.0)),
                      0.7f + 0.2f * float(std::sin(t * 1.1 + 2.0)),
                  });

        // === RIGHT (target) block — anchored TopRight, 3s-on / 3s-off ===
        const double tgt_phase = std::fmod(t, 6.0);
        const bool have_target = tgt_phase < 3.0;
        if (have_target)
        {
            float fade = 1.0f;
            if (tgt_phase < 0.5)       fade = float(tgt_phase / 0.5);
            else if (tgt_phase > 2.5)  fade = float((3.0 - tgt_phase) / 0.5);

            const int32_t outerX = screen_w - kBlockMargin - blockW;
            DrawBlock(outerX, blockW, barW, /*isRight=*/true,
                      /*lv=*/{
                          0.4f + 0.3f * float(std::sin(t * 1.3 + 0.5)),
                          0.3f + 0.3f * float(std::sin(t * 1.5 + 1.5)),
                          0.5f + 0.2f * float(std::sin(t * 1.7 + 2.5)),
                      }, fade);
        }
    }

private:
    struct SBarLevels { float h, m, s; };

    void DrawBlock(int32_t outerX, int32_t blockW, int32_t barW,
                   bool isRight, SBarLevels lv, float fadeAlpha = 1.0f)
    {
        const uint8_t aFrame = uint8_t(255 * fadeAlpha);
        const int32_t blockY = kBlockMargin;

        // Portrait box at the OUTER corner — placeholder solid rect for
        // now (real portrait comes from character data, not playscrn.dat).
        const int32_t portraitX = isRight
            ? outerX + blockW - kPortraitSize
            : outerX;
        Renderer->DrawSolidRect(portraitX, blockY,
                                kPortraitSize, kPortraitSize,
                                60, 50, 30, aFrame);
        Renderer->DrawSolidRect(portraitX, blockY, kPortraitSize, 1,
                                180, 140, 60, aFrame);
        Renderer->DrawSolidRect(portraitX, blockY + kPortraitSize - 1,
                                kPortraitSize, 1, 180, 140, 60, aFrame);
        Renderer->DrawSolidRect(portraitX, blockY, 1, kPortraitSize,
                                180, 140, 60, aFrame);
        Renderer->DrawSolidRect(portraitX + kPortraitSize - 1, blockY,
                                1, kPortraitSize, 180, 140, 60, aFrame);

        // 3 stacked bars. Bars sit to the INNER side of the portrait.
        // Each bar = colored gradient fill (placeholder for retail's
        // per-bar mosaic-surface composition) + icon at outer end.
        const int32_t barX = isRight
            ? outerX                              // RIGHT block: bars left of portrait
            : outerX + kPortraitSize + 4;         // LEFT  block: bars right of portrait

        const float lvls[3] = { lv.h, lv.m, lv.s };
        for (int i = 0; i < 3; ++i)
        {
            const int32_t rowY = blockY + 2 + i * kBarRowPitch;

            // Skip the green texthealthbar bitmap blit — it doesn't
            // match the retail per-bar colors and currently dominates
            // visually. Once the mosaic-surface compositing pipeline
            // lands, the bitmap's track + icon-slot + value-text-region
            // will be color-tinted per bar and blitted directly.
            (void)g_textHealthBar;

            // === Bar fill ===
            // Approximated scimitar shape: rectangular body with a
            // tapered triangular tip. Drawn as a stack of decreasing-
            // height rect slices along the fade region.
            const float lvl = std::clamp(lvls[i], 0.0f, 1.0f);
            const int32_t totalW = std::max<int32_t>(kBarFillW, 80);
            const int32_t bodyW  = int32_t(totalW * 0.7f);     // 70% solid
            const int32_t tipW   = totalW - bodyW;             // 30% tapered tip
            const int32_t fillH  = kBarFillH;
            const int32_t filledTotal = int32_t(float(totalW) * lvl);
            const int32_t actualBodyW = (bodyW < filledTotal) ? bodyW : filledTotal;
            const int32_t actualTipW  = (filledTotal > actualBodyW)
                                        ? (filledTotal - actualBodyW) : 0;

            const int32_t fillSlotY = rowY + kBarFillInsetY;
            const auto& c = kBarColors[i];

            // Solid body
            if (actualBodyW > 0)
            {
                const int32_t bx = isRight
                    ? barX + totalW - actualBodyW
                    : barX;
                Renderer->DrawSolidRect(bx, fillSlotY, actualBodyW, fillH,
                                        c.r, c.g, c.b, aFrame);
            }
            // Tapered tip (slices of decreasing height)
            if (actualTipW > 0 && tipW > 0)
            {
                for (int32_t s = 0; s < actualTipW; ++s)
                {
                    // Slice height shrinks linearly from fillH at start of tip
                    // to 1 at end of tip.
                    const float t01 = float(s) / float(tipW);
                    const int32_t sh = std::max<int32_t>(1,
                        int32_t(float(fillH) * (1.0f - t01)));
                    const int32_t sy = fillSlotY + (fillH - sh) / 2;
                    const int32_t sx = isRight
                        ? barX + tipW - 1 - s
                        : barX + bodyW + s;
                    Renderer->DrawSolidRect(sx, sy, 1, sh,
                                            c.r, c.g, c.b, aFrame);
                }
            }

            // === Icon at outer end of bar ===
            // Use `life` (heart, 24x21) for the health bar (i=0).
            // No matching mana/stamina icons in playscrn.dat — leave
            // those as small colored squares for now.
            if (i == 0 && g_lifeIcon)
            {
                const int32_t ix = isRight
                    ? barX + totalW + 2
                    : barX - g_lifeIcon->width - 2;
                const int32_t iy = rowY + (kBarRowPitch - g_lifeIcon->height) / 2;
                Renderer->DrawBitmap(g_lifeIcon, ix, iy);
            }
            else
            {
                // Placeholder icon: 8x8 colored square in the bar color.
                const int32_t ix = isRight ? barX + totalW + 2 : barX - 10;
                const int32_t iy = rowY + 2;
                Renderer->DrawSolidRect(ix, iy, 8, 8, c.r, c.g, c.b, aFrame);
            }
        }
    }
};

TPlyrStatusBarMockHud g_hud;

}  // namespace

bool InitializeUIPlyrStatusBarMode()
{
    log_info("[ui-plyrstatusbar] === TPlyrStatusBar layout mockup ===");

    // Load playscrn.dat into GameData if not already done. Mirrors
    // playscreen.cpp:205+ — same global, same path.
    if (!GameData)
    {
        log_info("[ui-plyrstatusbar] loading playscrn.dat...");
        GameData = TMulti::LoadMulti((char*)"playscrn.dat");
        if (!GameData)
            log_warn("[ui-plyrstatusbar] failed to load playscrn.dat -- "
                     "no assets available, bars will paint as solid rects");
        else
            log_info("[ui-plyrstatusbar] playscrn.dat loaded (%d assets)",
                     GameData->numoffsets);
    }

    // Look up real assets used by retail TPlyrStatusBar.
    // texthealthbar (#98): one bar's track+text bitmap (plain green
    //   rectangle 200x11 — per-bar color comes from mosaic-surface
    //   compositing in retail; not yet wired here).
    // life (#54): 24x21 heart icon for the health bar's left-end icon
    if (GameData)
    {
        g_textHealthBar = GameData->Bitmap((char*)"texthealthbar");
        g_lifeIcon      = GameData->Bitmap((char*)"life");
        if (g_textHealthBar)
            log_info("[ui-plyrstatusbar] texthealthbar loaded (%dx%d)",
                     g_textHealthBar->width, g_textHealthBar->height);
        if (g_lifeIcon)
            log_info("[ui-plyrstatusbar] life (heart icon) loaded (%dx%d)",
                     g_lifeIcon->width, g_lifeIcon->height);
    }

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
