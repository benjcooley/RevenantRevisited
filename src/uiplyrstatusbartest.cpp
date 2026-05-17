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

// Retail bar coordinates extracted from
// recon/discovered/cls_0x5a54e4_TPlyrStatusBar_slot23_TwoPassDraw_54af20.cpp
// lines 484, 499, 511 (LEFT side bar draws in hi-res branch):
//   Health  : x=0x44 (68), y=0x0f (15)
//   Mana    : x=0x44 (68), y=0x1f (31)
//   Stamina : x=0x44 (68), y=0x2c (44)
// Row pitch is non-uniform (16 then 13). These are pane-relative.
constexpr int32_t kBarFillX     = 0x44;  // 68 — LEFT side bar X (retail slot 23)
constexpr int32_t kBarRowY[3]   = { 0x0f, 0x1f, 0x2c };  // 15, 31, 44

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

        // Synthetic levels matching reference: health ~85%, mana ~95%,
        // stamina ~15%. Slight sine wobble for animation life.
        const float healthLvl  = 0.85f + 0.05f * float(std::sin(t * 0.7));
        const float manaLvl    = 0.95f + 0.05f * float(std::sin(t * 0.9 + 1.0));
        const float staminaLvl = 0.15f + 0.05f * float(std::sin(t * 1.1 + 2.0));

        // Backdrop
        Renderer->DrawBitmap(g_backPanel, kPanelX, kPanelY);

        // Bars: per-row blit from the Bars atlas, clipped horizontally
        // to (level * bar_width). The retail panel composes bars from
        // this atlas; we do the same via subrect blit.
        DrawBar(kBarHealthAtlasY, 0, healthLvl);
        DrawBar(kBarManaAtlasY,   1, manaLvl);
        DrawBar(kBarStaminaAtlasY, 2, staminaLvl);

        // Bar icons (overlap the panel's icon slot at the bar's start)
        DrawIcon(g_healthIcon,  0);
        DrawIcon(g_manaIcon,    1);
        DrawIcon(g_fatigueIcon, 2);

        // Portrait: Ring frame then LockeFace inside it
        if (g_ring)
            Renderer->DrawBitmap(g_ring, kPanelX + kRingX, kPanelY + kRingY);
        if (g_lockeFace)
            Renderer->DrawBitmap(g_lockeFace, kPanelX + kPortraitX,
                                 kPanelY + kPortraitY);
    }

private:
    void DrawBar(int32_t atlasY, int32_t row, float level)
    {
        if (!g_bars) return;
        const int32_t dstX = kPanelX + kBarFillX;
        const int32_t dstY = kPanelY + kBarRowY[row];
        const float clamped = level < 0.0f ? 0.0f : (level > 1.0f ? 1.0f : level);
        const int32_t filled = int32_t(float(kBarAtlasW) * clamped);
        if (filled > 0)
            Renderer->DrawBitmapSubrect(g_bars, dstX, dstY,
                                        0, atlasY, filled, kBarRowH);
    }

    void DrawIcon(PTBitmap icon, int32_t row)
    {
        if (!icon) return;
        // Icon centered on the start of the bar row.
        const int32_t ix = kPanelX + kBarFillX - icon->width / 2;
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
