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
constexpr int32_t kBarRowPitch    =  12;    // tighter than Wave-3A's 14, per reference
constexpr int32_t kPortraitSize   =  44;    // tighter portrait, ~3-bar stack height
constexpr int32_t kPortraitInset  =   2;

// Bar visuals — tuned against docs/ui/plyr_stats_panel.png.
constexpr int32_t kBarFillH       =   8;    // thicker bars to match reference presence
constexpr int32_t kBarFillInsetY  =   2;    // vertical center within row pitch

// Per-bar length stagger from Wave-3A's slot 23 right-side offsets
// (0x88=136, 0x91=145, 0x79=121). These are inset-from-right values:
// HEALTH (0x88) ends 136 from right edge — LONGEST bar.
// MANA   (0x91) ends 145 from right — slightly shorter.
// STAMINA(0x79) ends 121 from right — actually longest by this math but
//                                     reference shows stamina SHORTEST.
// The retail offsets may include icon-region width; for the mockup the
// PROPORTIONAL pattern is what matters. Tuned by eye against reference:
// All bars share the same TRACK width — only the FILL differs. The
// per-bar visual stagger in the reference is from the per-stat current
// level (stamina at 191/max looks much shorter than health at 1833/max).
constexpr int32_t kBarBodyW       = 120;

// Per-bar dark base + bright highlight for the gradient/sheen look.
// Colors tuned against docs/ui/plyr_stats_panel.png — reference shows:
// - health: deep crimson body, bright orange-red top highlight
// - mana:   deep blue-purple body, brighter violet-blue highlight
// - stamina: deep amber body, bright yellow-orange top highlight
struct SBarColor { uint8_t r, g, b; };
struct SBarStyle { SBarColor base; SBarColor highlight; SBarColor outline; };
constexpr SBarStyle kBarStyles[3] = {
    { { 0x70, 0x10, 0x10 }, { 0xFF, 0x60, 0x30 }, { 0x30, 0x04, 0x04 } },  // health
    { { 0x30, 0x18, 0x70 }, { 0x90, 0x70, 0xFF }, { 0x10, 0x04, 0x28 } },  // mana
    { { 0x80, 0x40, 0x04 }, { 0xFF, 0xD0, 0x20 }, { 0x30, 0x18, 0x02 } },  // stamina
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
        // Levels approximate sample_screen_1.jpg stats: health ~85%,
        // mana 100%, stamina LOW ~15%. Slight sine wobble for animation.
        DrawBlock(/*outerX=*/kBlockMargin, blockW, barW, /*isRight=*/false,
                  /*lv=*/{
                      0.85f + 0.05f * float(std::sin(t * 0.7)),
                      0.95f + 0.05f * float(std::sin(t * 0.9 + 1.0)),
                      0.15f + 0.05f * float(std::sin(t * 1.1 + 2.0)),
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

    // Draws an octagonal portrait frame at (x, y, size, size). Faked via
    // stacked horizontal strips of decreasing width near top/bottom edges
    // to approximate a chamfered octagon. Gold frame border + dark fill;
    // the actual portrait sprite would be blitted inside this frame.
    void DrawOctagonalPortrait(int32_t x, int32_t y, int32_t size, uint8_t a)
    {
        const int32_t chamfer = size / 4;  // pixels chamfered at each corner
        const uint8_t fr = 60, fg = 50, fb = 30;             // fill (dark brown)
        const uint8_t br = 200, bg = 160, bb = 70;           // border (gold)

        // Body: stacked horizontal strips. For rows in the chamfered
        // top/bottom region, narrow the strip.
        for (int32_t row = 0; row < size; ++row)
        {
            int32_t inset = 0;
            if (row < chamfer)              inset = chamfer - row;
            else if (row >= size - chamfer) inset = chamfer - (size - 1 - row);
            const int32_t rx = x + inset;
            const int32_t rw = size - 2 * inset;
            // Fill row
            Renderer->DrawSolidRect(rx, y + row, rw, 1, fr, fg, fb, a);
            // Border pixels: left + right edges (1px)
            Renderer->DrawSolidRect(rx, y + row, 1, 1, br, bg, bb, a);
            Renderer->DrawSolidRect(rx + rw - 1, y + row, 1, 1, br, bg, bb, a);
        }
        // Top + bottom border strips (1px) — bridge the chamfered edges
        const int32_t topY = y;
        const int32_t botY = y + size - 1;
        const int32_t topW = size - 2 * chamfer;
        Renderer->DrawSolidRect(x + chamfer, topY, topW, 1, br, bg, bb, a);
        Renderer->DrawSolidRect(x + chamfer, botY, topW, 1, br, bg, bb, a);
        // Diagonal chamfer borders (1px steps)
        for (int32_t s = 0; s < chamfer; ++s)
        {
            // top-left → top-center
            Renderer->DrawSolidRect(x + s, y + chamfer - s, 1, 1, br, bg, bb, a);
            // top-right
            Renderer->DrawSolidRect(x + size - 1 - s, y + chamfer - s, 1, 1, br, bg, bb, a);
            // bottom-left
            Renderer->DrawSolidRect(x + s, y + size - 1 - (chamfer - s), 1, 1, br, bg, bb, a);
            // bottom-right
            Renderer->DrawSolidRect(x + size - 1 - s, y + size - 1 - (chamfer - s), 1, 1, br, bg, bb, a);
        }
    }

    // 9x9 blue diamond icon centered on (cx, cy).
    void DrawDiamondIcon(int32_t cx, int32_t cy,
                         uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        for (int32_t row = -4; row <= 4; ++row)
        {
            const int32_t w = 9 - 2 * (row < 0 ? -row : row);
            Renderer->DrawSolidRect(cx - w / 2, cy + row, w, 1, r, g, b, a);
        }
    }

    // 7x7 upward-pointing yellow chevron centered on (cx, cy).
    void DrawChevronIcon(int32_t cx, int32_t cy,
                         uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        // Filled triangle pointing up: top point at row -3, base at row +3.
        for (int32_t row = -3; row <= 3; ++row)
        {
            const int32_t w = (row + 4);  // 1..7
            Renderer->DrawSolidRect(cx - w / 2, cy + row, w, 1, r, g, b, a);
        }
    }

    void DrawBlock(int32_t outerX, int32_t blockW, int32_t barW,
                   bool isRight, SBarLevels lv, float fadeAlpha = 1.0f)
    {
        const uint8_t aFrame = uint8_t(255 * fadeAlpha);
        const int32_t blockY = kBlockMargin;

        // Portrait at OUTER corner — faked as an octagonal frame to
        // approximate the retail ornate gem-shaped portrait frame.
        // (Real portrait sprite comes from character data, not
        // playscrn.dat; that wiring is deferred to a later iteration.)
        const int32_t portraitX = isRight
            ? outerX + blockW - kPortraitSize
            : outerX;
        DrawOctagonalPortrait(portraitX, blockY, kPortraitSize, aFrame);

        // 3 stacked bars with per-bar length stagger (matches reference).
        // Each bar = outline + dark base + bright highlight strip
        // (faked gradient/sheen) + tapered scimitar tip + icon at outer end.
        // Tight packing: bars start at portrait edge - 4 so icons can
        // overlap the portrait slightly (matches reference).
        const int32_t barX = isRight
            ? outerX + 4                          // RIGHT block: bars left of portrait
            : outerX + kPortraitSize - 4;         // LEFT  block: bars start at portrait edge -4

        (void)g_textHealthBar;  // raw green bitmap doesn't match per-bar colors yet

        const float lvls[3] = { lv.h, lv.m, lv.s };
        for (int i = 0; i < 3; ++i)
        {
            const int32_t rowY = blockY + 4 + i * kBarRowPitch;
            const int32_t totalW = kBarBodyW;
            const int32_t bodyW  = int32_t(totalW * 0.90f);    // 90% solid (subtle taper)
            const int32_t tipW   = totalW - bodyW;
            const int32_t fillH  = kBarFillH;
            const float   lvl    = std::clamp(lvls[i], 0.0f, 1.0f);
            const int32_t filledTotal = int32_t(float(totalW) * lvl);
            const int32_t actualBodyW = (bodyW < filledTotal) ? bodyW : filledTotal;
            const int32_t actualTipW  = (filledTotal > actualBodyW)
                                        ? (filledTotal - actualBodyW) : 0;

            const int32_t fillSlotY = rowY + kBarFillInsetY;
            const auto& st = kBarStyles[i];

            // 1px darker outline behind the bar (offset by 1 in all dirs).
            // Drawn first so the fill paints over the inner area.
            const int32_t outlineX = isRight ? barX - 1 : barX - 1;
            Renderer->DrawSolidRect(outlineX, fillSlotY - 1,
                                    totalW + 2, fillH + 2,
                                    st.outline.r, st.outline.g, st.outline.b,
                                    aFrame);

            // Solid base body — drawn as gradient slices (brighter near
            // the icon end, darker toward the tip) for the reference's
            // metallic-sheen look. Each slice = vertical 1px column.
            if (actualBodyW > 0)
            {
                const int32_t bx = isRight ? barX + totalW - actualBodyW : barX;
                for (int32_t s = 0; s < actualBodyW; ++s)
                {
                    // t01 = 0 at icon end, 1 at tip end.
                    const float t01 = isRight
                        ? float(s) / float(actualBodyW)
                        : float(actualBodyW - 1 - s) / float(actualBodyW);
                    // 1 - t01 lerp factor toward darkness
                    const float dark = 0.50f * t01;  // 50% darken at tip end
                    const uint8_t br = uint8_t(st.base.r * (1.0f - dark));
                    const uint8_t bgc = uint8_t(st.base.g * (1.0f - dark));
                    const uint8_t bb = uint8_t(st.base.b * (1.0f - dark));
                    Renderer->DrawSolidRect(bx + s, fillSlotY, 1, fillH,
                                            br, bgc, bb, aFrame);
                }
                // Bright highlight strip (top 1/3 of bar height) for sheen
                // — also gradient-darkened from icon end to tip end.
                const int32_t hiH = fillH / 3 + 1;
                for (int32_t s = 0; s < actualBodyW; ++s)
                {
                    const float t01 = isRight
                        ? float(s) / float(actualBodyW)
                        : float(actualBodyW - 1 - s) / float(actualBodyW);
                    const float dark = 0.40f * t01;
                    const uint8_t hr = uint8_t(st.highlight.r * (1.0f - dark));
                    const uint8_t hg = uint8_t(st.highlight.g * (1.0f - dark));
                    const uint8_t hb = uint8_t(st.highlight.b * (1.0f - dark));
                    Renderer->DrawSolidRect(bx + s, fillSlotY + 1, 1, hiH,
                                            hr, hg, hb, aFrame);
                }
            }
            // Tapered scimitar tip
            if (actualTipW > 0 && tipW > 0)
            {
                for (int32_t s = 0; s < actualTipW; ++s)
                {
                    const float t01 = float(s) / float(tipW);
                    const int32_t sh = std::max<int32_t>(1,
                        int32_t(float(fillH) * (1.0f - t01)));
                    const int32_t sy = fillSlotY + (fillH - sh) / 2;
                    const int32_t sx = isRight
                        ? barX + tipW - 1 - s
                        : barX + bodyW + s;
                    // Base color, with highlight on top half if slice tall enough
                    Renderer->DrawSolidRect(sx, sy, 1, sh,
                                            st.base.r, st.base.g, st.base.b, aFrame);
                    if (sh >= 3)
                        Renderer->DrawSolidRect(sx, sy, 1, 1,
                                                st.highlight.r, st.highlight.g, st.highlight.b,
                                                aFrame);
                }
            }

            // === Icon at outer end of bar (overlapping the bar start) ===
            // Per reference: icons sit AT the start of the bar, slightly
            // overlapping into the portrait region.
            const int32_t iconCenterX = isRight ? barX + totalW : barX;
            const int32_t iconCenterY = rowY + kBarRowPitch / 2;
            if (i == 0 && g_lifeIcon)
            {
                // Real heart asset for health bar.
                const int32_t ix = iconCenterX - g_lifeIcon->width / 2;
                const int32_t iy = iconCenterY - g_lifeIcon->height / 2;
                Renderer->DrawBitmap(g_lifeIcon, ix, iy);
            }
            else if (i == 1)
            {
                // Mana: small blue diamond — 4 triangular sides via stacked rects
                DrawDiamondIcon(iconCenterX, iconCenterY,
                                st.highlight.r, st.highlight.g, st.highlight.b, aFrame);
            }
            else if (i == 2)
            {
                // Stamina: small upward yellow chevron
                DrawChevronIcon(iconCenterX, iconCenterY,
                                st.highlight.r, st.highlight.g, st.highlight.b, aFrame);
            }
            else
            {
                // Default tiny square
                const int32_t ix = iconCenterX - 3;
                const int32_t iy = iconCenterY - 3;
                const auto& hi = st.highlight;
                Renderer->DrawSolidRect(ix, iy, 6, 6, hi.r, hi.g, hi.b, aFrame);
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
