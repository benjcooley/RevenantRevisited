// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uiplyrstatusbartest.cpp - --test=ui-plyrstatusbar                    *
// *************************************************************************
//
// Pixel-faithful TPlyrStatusBar harness using the real retail assets from
// statusbarnotex.dat + portraits.dat. Composition follows the retail
// layout established by Wave-3A's slot 23 extraction and verified by
// dumping each asset to PNG via tools/ui/dump_dat.py.
//
// Asset roster:
//   BackPanel   128x64   — panel chrome backdrop
//   Bars        128x128  — sprite atlas:
//                          y=  0..11  health full (red)
//                          y= 16..27  mana full (purple)
//                          y= 32..43  stamina/fatigue full (orange)
//                          y= 48..59  health dim variant
//                          y= 64..75  mana dim variant
//                          y= 80..91  stamina dim variant
//                          y= 96+     icons + tiny font
//   HealthIcon  24x24    — red heart
//   ManaIcon    24x24    — blue gem
//   FatigueIcon 24x24    — yellow chevron
//   Ring        44x44    — gold portrait frame
//   LockeFace   30x30    — Locke portrait sprite (from portraits.dat)
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

TMulti*  g_statusbarDat = nullptr;
TMulti*  g_portraitsDat = nullptr;

PTBitmap g_backPanel    = nullptr;
PTBitmap g_bars         = nullptr;
PTBitmap g_healthIcon   = nullptr;
PTBitmap g_manaIcon     = nullptr;
PTBitmap g_fatigueIcon  = nullptr;
PTBitmap g_ring         = nullptr;
PTBitmap g_lockeFace    = nullptr;

// Panel top-left position (arbitrary for test mode; retail places at a
// specific X,Y on the playfield).
constexpr int32_t kPanelX = 4;
constexpr int32_t kPanelY = 4;

// Bars atlas row layout — measured from /tmp/dump_statusbar3/01_Bars.png.
constexpr int32_t kBarAtlasW       = 128;
constexpr int32_t kBarRowH         = 12;
constexpr int32_t kBarHealthAtlasY = 0;
constexpr int32_t kBarManaAtlasY   = 16;
constexpr int32_t kBarStaminaAtlasY = 32;

// Retail bar coordinates from
// recon/discovered/cls_0x5a54e4_TPlyrStatusBar_slot23_TwoPassDraw_54af20.cpp.
//
// LEFT (player) bars — slot 23 lines 484/499/511, hi-res branch:
//   Health  : (x=0x44=68, y=0x0f=15)
//   Mana    : (x=0x44=68, y=0x1f=31)
//   Stamina : (x=0x44=68, y=0x2c=44)
//
// RIGHT (target) bars — slot 23 lines 657/672/686, hi-res branch:
//   x is computed as `*(int *)(param_1 + 0xc) - <offset>` where +0xc
//   is the pane width. Per-bar offsets:
//     Health  : pane_width - 0xc1 = pane_width - 193
//     Mana    : pane_width - 0x91 = pane_width - 145
//     Stamina : pane_width - 0x79 = pane_width - 121
//   Y values are the same as LEFT (15/31/44).
//
// Row pitch is non-uniform (16 then 13). Coords are PANE-relative
// (the pane spans the full screen width in retail; here the test mode
// uses screen width for pane width).
constexpr int32_t kBarFillX        = 0x44;                  // 68
constexpr int32_t kBarRowY[3]      = { 0x0f, 0x1f, 0x2c };  // 15, 31, 44
constexpr int32_t kTargetBarOff[3] = { 0xc1, 0x91, 0x79 };  // 193, 145, 121

// Portrait Ring + LockeFace positions — currently visual estimates;
// will be replaced once the recon Init body or helper extractions
// surface the explicit Ring blit X,Y.
constexpr int32_t kRingX        = 2;
constexpr int32_t kRingY        = 6;
constexpr int32_t kPortraitX    = 9;
constexpr int32_t kPortraitY    = 13;

class TPlyrStatusBarRealHud : public THudDrawable
{
public:
    void Draw() override
    {
        if (!g_backPanel) return;
        const double t = TTime::Time();
        const int32_t paneW = Display.Width();

        // Synthetic levels matching reference.
        const float playerH = 0.85f + 0.05f * float(std::sin(t * 0.7));
        const float playerM = 0.95f + 0.05f * float(std::sin(t * 0.9 + 1.0));
        const float playerS = 0.15f + 0.05f * float(std::sin(t * 1.1 + 2.0));
        const float targetH = 0.40f + 0.05f * float(std::sin(t * 1.3 + 0.5));
        const float targetM = 0.60f + 0.05f * float(std::sin(t * 1.5 + 1.5));
        const float targetS = 0.30f + 0.05f * float(std::sin(t * 1.7 + 2.5));

        // === LEFT (player) ===
        // BackPanel chrome at panel origin (0,0 in pane coords).
        Renderer->DrawBitmap(g_backPanel, kPanelX, kPanelY);
        DrawBar(false, kBarHealthAtlasY,  0, playerH);
        DrawBar(false, kBarManaAtlasY,    1, playerM);
        DrawBar(false, kBarStaminaAtlasY, 2, playerS);
        DrawIcon(false, g_healthIcon,  0);
        DrawIcon(false, g_manaIcon,    1);
        DrawIcon(false, g_fatigueIcon, 2);
        if (g_ring)
            Renderer->DrawBitmap(g_ring, kPanelX + kRingX, kPanelY + kRingY);
        if (g_lockeFace)
            Renderer->DrawBitmap(g_lockeFace, kPanelX + kPortraitX,
                                 kPanelY + kPortraitY);

        // === RIGHT (target) ===
        // Synthetic 3s on / 3s off target visibility cycle.
        const double tgt_phase = std::fmod(t, 6.0);
        if (tgt_phase < 3.0)
        {
            // Right-edge X for the backdrop chrome: mirror the LEFT pane
            // position. The retail BackPanel is the same bitmap drawn at
            // (pane_width - panel_w) for the right side.
            const int32_t rightPanelX = paneW - kPanelX - g_backPanel->width;
            Renderer->DrawBitmap(g_backPanel, rightPanelX, kPanelY);
            DrawBar(true, kBarHealthAtlasY,  0, targetH);
            DrawBar(true, kBarManaAtlasY,    1, targetM);
            DrawBar(true, kBarStaminaAtlasY, 2, targetS);
            DrawIcon(true, g_healthIcon,  0);
            DrawIcon(true, g_manaIcon,    1);
            DrawIcon(true, g_fatigueIcon, 2);
            if (g_ring)
                Renderer->DrawBitmap(g_ring,
                    rightPanelX + (g_backPanel->width - g_ring->width - kRingX),
                    kPanelY + kRingY);
            if (g_lockeFace)
                Renderer->DrawBitmap(g_lockeFace,
                    rightPanelX + (g_backPanel->width - g_lockeFace->width - kPortraitX),
                    kPanelY + kPortraitY);
        }
    }

private:
    void DrawBar(bool isRight, int32_t atlasY, int32_t row, float level)
    {
        if (!g_bars) return;
        const int32_t paneW = Display.Width();
        // LEFT: bar X = kBarFillX (68). RIGHT: bar X = paneW - kTargetBarOff[row].
        const int32_t dstX = isRight
            ? paneW - kTargetBarOff[row]
            : kPanelX + kBarFillX;
        const int32_t dstY = kPanelY + kBarRowY[row];
        const float clamped = level < 0.0f ? 0.0f : (level > 1.0f ? 1.0f : level);
        const int32_t filled = int32_t(float(kBarAtlasW) * clamped);
        if (filled > 0)
        {
            if (isRight)
            {
                // Mirror fill: target bars deplete TOWARD the screen
                // edge (right). Blit from the right side of the atlas
                // row so the visible portion ends at the bar's right
                // edge, not the left.
                Renderer->DrawBitmapSubrect(g_bars,
                    dstX + (kBarAtlasW - filled), dstY,
                    kBarAtlasW - filled, atlasY, filled, kBarRowH);
            }
            else
            {
                Renderer->DrawBitmapSubrect(g_bars, dstX, dstY,
                                            0, atlasY, filled, kBarRowH);
            }
        }
    }

    void DrawIcon(bool isRight, PTBitmap icon, int32_t row)
    {
        if (!icon) return;
        const int32_t paneW = Display.Width();
        // Icon sits at the bar's INNER end (toward portrait), centered.
        // LEFT bars: icon at bar's left (icon center = bar X)
        // RIGHT bars: icon at bar's right (icon center = bar X + bar_width)
        const int32_t barX = isRight
            ? paneW - kTargetBarOff[row]
            : kPanelX + kBarFillX;
        const int32_t centerX = isRight ? barX + kBarAtlasW : barX;
        const int32_t ix = centerX - icon->width / 2;
        const int32_t iy = kPanelY + kBarRowY[row]
                           + kBarRowH / 2 - icon->height / 2;
        Renderer->DrawBitmap(icon, ix, iy);
    }
};

TPlyrStatusBarRealHud g_hud;

PTBitmap LookupByName(TMulti* m, const char* name)
{
    if (!m || !name) return nullptr;
    for (int32_t i = 0; i < m->numoffsets; ++i)
    {
        const char* nm = (const char*)m->names[i].ptr();
        if (nm && !strcmp(nm, name))
            return m->Bitmap(i);
    }
    return nullptr;
}

}  // namespace

bool InitializeUIPlyrStatusBarMode()
{
    log_info("[ui-plyrstatusbar] === TPlyrStatusBar (real retail assets) ===");

    g_statusbarDat = TMulti::LoadMulti((char*)"statusbarnotex.dat");
    g_portraitsDat = TMulti::LoadMulti((char*)"portraits.dat");

    g_backPanel   = LookupByName(g_statusbarDat, "BackPanel");
    g_bars        = LookupByName(g_statusbarDat, "Bars");
    g_healthIcon  = LookupByName(g_statusbarDat, "HealthIcon");
    g_manaIcon    = LookupByName(g_statusbarDat, "ManaIcon");
    g_fatigueIcon = LookupByName(g_statusbarDat, "FatigueIcon");
    g_ring        = LookupByName(g_statusbarDat, "Ring");
    g_lockeFace   = LookupByName(g_portraitsDat, "LockeFace");

    log_info("[ui-plyrstatusbar] assets: backPanel=%s bars=%s ring=%s lockeFace=%s",
             g_backPanel ? "OK" : "MISS",
             g_bars ? "OK" : "MISS",
             g_ring ? "OK" : "MISS",
             g_lockeFace ? "OK" : "MISS");

    Renderer->AddHud(&g_hud, 0.0f);
    return true;
}

void RenderUIPlyrStatusBarMode()
{
    Display.BackBuffer()->StartPass(0.08f, 0.10f, 0.14f, 1.0f);
    Display.BackBuffer()->EndPass();
}

void CloseUIPlyrStatusBarMode()
{
    Renderer->RemoveHud(&g_hud);
    g_backPanel = g_bars = g_healthIcon = g_manaIcon = nullptr;
    g_fatigueIcon = g_ring = g_lockeFace = nullptr;
    g_statusbarDat = g_portraitsDat = nullptr;
}
