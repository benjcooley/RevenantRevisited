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

// Cached single-bar backdrop bitmap (200x11) from playscrn.dat. The retail
// asset is `texthealthbar` (asset #98) — ONE bar's track+text composite.
// The full TPlyrStatusBar panel stacks 3 of these vertically (one each
// for health / mana / stamina).
PTBitmap g_textHealthBar = nullptr;

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

        // 3 stacked `texthealthbar` blits, one per bar.
        // Bars sit to the INNER side of the portrait.
        const int32_t barX = isRight
            ? outerX                              // RIGHT block: bars left of portrait
            : outerX + kPortraitSize + 4;         // LEFT  block: bars right of portrait

        const float lvls[3] = { lv.h, lv.m, lv.s };
        for (int i = 0; i < 3; ++i)
        {
            const int32_t rowY = blockY + 2 + i * kBarRowPitch;

            // Backdrop bitmap (the bar's track + icon + text frame).
            if (g_textHealthBar)
            {
                Renderer->DrawBitmap(g_textHealthBar, barX, rowY);
            }

            // Bar fill — colored rect overlaid on the bar bitmap to
            // indicate current level. The retail mechanism is different:
            // it pre-composites each bar into a TMosaicSurface in the
            // intended color and blits the surface (cached, regenerated
            // only when level changes). Wiring that requires the
            // surface-to-HUD pipeline and per-bar color compositing,
            // both pending. For now the colored fill rect lives on top
            // of the bar bitmap as a level indicator.
            const float lvl = std::clamp(lvls[i], 0.0f, 1.0f);
            const int32_t fw = int32_t(float(kBarFillW) * lvl);
            const int32_t fillSlotX = barX + kBarFillInsetX;
            const int32_t fillSlotY = rowY + kBarFillInsetY;
            const auto& c = kBarColors[i];
            if (fw > 0)
            {
                const int32_t fillX = isRight
                    ? fillSlotX + (kBarFillW - fw)
                    : fillSlotX;
                Renderer->DrawSolidRect(fillX, fillSlotY, fw, kBarFillH,
                                        c.r, c.g, c.b, aFrame);
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
    // Asset name discovered by dumping playscrn.dat's name table — the
    // panel backdrop bitmap is `texthealthbar` (asset #98 in the .dat).
    if (GameData)
    {
        g_textHealthBar = GameData->Bitmap((char*)"texthealthbar");
        if (g_textHealthBar)
            log_info("[ui-plyrstatusbar] texthealthbar bitmap loaded (%dx%d)",
                     g_textHealthBar->width, g_textHealthBar->height);
        else
            log_warn("[ui-plyrstatusbar] texthealthbar not found in GameData "
                     "-- falling back to solid-rect placeholder");
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
