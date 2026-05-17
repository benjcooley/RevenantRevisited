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

// Bars atlas — proven values from FUN_0054a5d0 helper:
//   bar width  = param_12 = 0x77 = 119
//   bar height = param_11 = 0x11 = 17
//   cap width  = param_13 = 6
// Atlas rows pitched at 17 (height 17, 0-spacing) starting at y=0:
//   y=0  bright health (red)
//   y=17 bright mana (purple)
//   y=34 bright stamina (yellow)
//   y=51 dim health  (track / empty)
//   y=68 dim mana    (track)
//   y=85 dim stamina (track)
constexpr int32_t kBarAtlasW       = 128;
constexpr int32_t kBarMaxW         = 0x77;    // 119
constexpr int32_t kBarRowH         = 0x11;    // 17 per recon arg param_11
constexpr int32_t kBarCapW         = 6;
constexpr int32_t kBarHealthAtlasY = 0;
constexpr int32_t kBarManaAtlasY   = 17;
constexpr int32_t kBarStaminaAtlasY = 34;
constexpr int32_t kBarDimYOffset   = 51;      // dim row = bright + 51 (3 rows down)

// Retail bar coordinates from
// recon/discovered/cls_0x5a54e4_TPlyrStatusBar_slot23_TwoPassDraw_54af20.cpp.
//
// Per user 2026-05-17: the (0x44, 0xf) coords passed to the char's
// draw method are the SHADOW position, not the bar itself. The bar is
// drawn at (-4, -4) from the shadow per FUN_00438d80(buf, 4, 4)
// shadow-offset setup that immediately precedes each bar draw block.
// Bar coords (after subtracting the (4, 4) shadow offset):
//   Health  : (x=0x40=64, y=0x0b=11)   (shadow at (0x44, 0xf) = (68, 15))
//   Mana    : (x=0x40=64, y=0x1b=27)   (shadow at (0x44, 0x1f))
//   Stamina : (x=0x40=64, y=0x28=40)   (shadow at (0x44, 0x2c))
//
// RIGHT (target) shadow X offsets (line 657/672/686):
//   pane_width - 0xc1 / 0x91 / 0x79  — these are SHADOW positions too.
//   Actual bar X = shadow_x - 4.
constexpr int32_t kBarShadowOffX   = 4;
constexpr int32_t kBarShadowOffY   = 4;
constexpr int32_t kBarFillX        = 0x44 - kBarShadowOffX;  // 64 (bar X)
constexpr int32_t kBarRowY[3]      = { 0x0f - kBarShadowOffY,
                                       0x1f - kBarShadowOffY,
                                       0x2c - kBarShadowOffY };  // 11, 27, 40
constexpr int32_t kTargetBarOff[3] = { 0xc1, 0x91, 0x79 };       // shadow offsets

// Surface-local coordinates extracted from FUN_0054a0a0 (PLAYER side
// draw helper) — surface is 128x64 (the +0x6c mosaic surface):
//   BackPanel    at (0, 0)
//   Ring         CENTERED at (0x1a, 0x1f) = (26, 31)  → top-left (4, 9)
//   HealthIcon   at (0x2b, 3)   = (43, 3)
//   ManaIcon     at (0x2b, 0x11) = (43, 17)
//   FatigueIcon  at (0x2b, 0x20) = (43, 32)
//
// And from FUN_0054a310 (TARGET side):
//   Ring         CENTERED at (0x62, 0x1f) = (98, 31) → top-left (76, 9)
//   HealthIcon   at (0x3a, 3)   = (58, 3)
//   ManaIcon     at (0x3a, 0x11) = (58, 17)
//   FatigueIcon  at (0x3a, 0x20) = (58, 32)
//
// LockeFace (30x30) is centered inside Ring (44x44). Real retail
// portrait is in chars\locke.i3d — needs I3D decoder, deferred.
constexpr int32_t kRingX_player  = 4;     // top-left of Ring on player side
constexpr int32_t kRingX_target  = 76;
constexpr int32_t kRingY         = 9;
constexpr int32_t kPortraitOff   = (44 - 30) / 2;   // center 30x30 in 44x44

constexpr int32_t kIconX_player  = 43;
constexpr int32_t kIconX_target  = 58;
constexpr int32_t kIconRowY[3]   = { 3, 17, 32 };   // health/mana/fatigue

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
        // Composition order per FUN_0054a0a0 (player-side draw helper):
        // BackPanel (backdrop) -> Ring -> Icons -> Bars -> LockeFace.
        // All coords from extracted recon helpers (pane-local on the
        // 128x64 player mosaic surface).
        Renderer->DrawBitmap(g_backPanel, kPanelX, kPanelY);
        if (g_ring)
            Renderer->DrawBitmap(g_ring, kPanelX + kRingX_player, kPanelY + kRingY);
        if (g_lockeFace)
            Renderer->DrawBitmap(g_lockeFace,
                kPanelX + kRingX_player + kPortraitOff,
                kPanelY + kRingY + kPortraitOff);
        DrawIconAt(g_healthIcon,  kPanelX + kIconX_player, kPanelY + kIconRowY[0]);
        DrawIconAt(g_manaIcon,    kPanelX + kIconX_player, kPanelY + kIconRowY[1]);
        DrawIconAt(g_fatigueIcon, kPanelX + kIconX_player, kPanelY + kIconRowY[2]);
        DrawBar(false, kBarHealthAtlasY,  0, playerH);
        DrawBar(false, kBarManaAtlasY,    1, playerM);
        DrawBar(false, kBarStaminaAtlasY, 2, playerS);

        // === RIGHT (target) ===
        // Synthetic 3s on / 3s off target visibility cycle.
        const double tgt_phase = std::fmod(t, 6.0);
        if (tgt_phase < 3.0)
        {
            // Right-edge X for the backdrop chrome: mirror the LEFT pane
            // position. The retail BackPanel is the same bitmap drawn at
            // (pane_width - panel_w) for the right side.
            // RIGHT-side composition per FUN_0054a310 (target-side draw
            // helper). Coords are mirrored within the target's own
            // 128x64 surface (Ring at x=76, icons at x=58).
            const int32_t rPanelX = paneW - kPanelX - g_backPanel->width;
            Renderer->DrawBitmap(g_backPanel, rPanelX, kPanelY);
            if (g_ring)
                Renderer->DrawBitmap(g_ring, rPanelX + kRingX_target, kPanelY + kRingY);
            if (g_lockeFace)
                Renderer->DrawBitmap(g_lockeFace,
                    rPanelX + kRingX_target + kPortraitOff,
                    kPanelY + kRingY + kPortraitOff);
            DrawIconAt(g_healthIcon,  rPanelX + kIconX_target, kPanelY + kIconRowY[0]);
            DrawIconAt(g_manaIcon,    rPanelX + kIconX_target, kPanelY + kIconRowY[1]);
            DrawIconAt(g_fatigueIcon, rPanelX + kIconX_target, kPanelY + kIconRowY[2]);
            DrawBar(true, kBarHealthAtlasY,  0, targetH);
            DrawBar(true, kBarManaAtlasY,    1, targetM);
            DrawBar(true, kBarStaminaAtlasY, 2, targetS);
        }
    }

private:
    // Per FUN_0054a5d0: bar = filled (BRIGHT row) + empty (DIM row)
    // composed at the bar position. A dropshadow is drawn FIRST at
    // (+kBarShadowOffX, +kBarShadowOffY) — recon slot 23 calls
    // FUN_00438d80(buf, 4, 4) immediately before each bar block to set
    // up the shadow blit. The shadow uses the same bar geometry in a
    // dark/alpha-tinted color. For now we approximate the shadow as a
    // semi-transparent black rect at the bar's geometry +4,+4.
    void DrawBar(bool isRight, int32_t atlasY, int32_t row, float level)
    {
        if (!g_bars) return;
        const int32_t paneW = Display.Width();
        const int32_t dstX = isRight
            ? paneW - kTargetBarOff[row] - kBarShadowOffX
            : kPanelX + kBarFillX;
        const int32_t dstY = kPanelY + kBarRowY[row];
        const float clamped = level < 0.0f ? 0.0f : (level > 1.0f ? 1.0f : level);
        const int32_t fillW  = int32_t(float(kBarMaxW) * clamped);
        const int32_t emptyW = kBarMaxW - fillW;
        const int32_t dimAtlasY = atlasY + kBarDimYOffset;

        // === Bar (filled+empty composite) with dropshadow ===
        // Each section uses DrawBitmapSubrectShadowed which renders the
        // bitmap as a darkened silhouette at (+kBarShadowOffX, +kBarShadowOffY)
        // first, then the bar atlas on top. The shadow preserves bar
        // geometry (tapered caps, etc.) from the atlas alpha. This is
        // the modern equivalent of retail's blit-effect-pipeline shadow
        // pass per memory project-retail-blit-effect-pipeline.
        if (isRight)
        {
            if (emptyW > 0)
                Renderer->DrawBitmapSubrectShadowed(g_bars, dstX, dstY,
                                                    0, dimAtlasY, emptyW, kBarRowH,
                                                    kBarShadowOffX, kBarShadowOffY);
            if (fillW > 0)
                Renderer->DrawBitmapSubrectShadowed(g_bars, dstX + emptyW, dstY,
                                                    emptyW, atlasY, fillW, kBarRowH,
                                                    kBarShadowOffX, kBarShadowOffY);
        }
        else
        {
            if (fillW > 0)
                Renderer->DrawBitmapSubrectShadowed(g_bars, dstX, dstY,
                                                    0, atlasY, fillW, kBarRowH,
                                                    kBarShadowOffX, kBarShadowOffY);
            if (emptyW > 0)
                Renderer->DrawBitmapSubrectShadowed(g_bars, dstX + fillW, dstY,
                                                    fillW, dimAtlasY, emptyW, kBarRowH,
                                                    kBarShadowOffX, kBarShadowOffY);
        }
    }

    // Direct icon blit at proven recon coords (callers compute the
    // panel-local x,y). No more guessing relative to bar position.
    void DrawIconAt(PTBitmap icon, int32_t x, int32_t y)
    {
        if (!icon) return;
        Renderer->DrawBitmap(icon, x, y);
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
