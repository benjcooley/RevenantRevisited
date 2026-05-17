// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uiplyrstatusbartest.cpp - --test=ui-plyrstatusbar                    *
// *************************************************************************
//
// Test-mode harness that brings up the TPlyrStatusBar panel in isolation
// (outside the game loop). Goal: pixel-accurate match to the retail panel
// (see docs/ui/plyr_stats_panel.png) using the real retail bitmap assets
// loaded from plyrstatusbar.dat and rendered through our modern Renderer
// primitives.
//
// Retail assets used (all from plyrstatusbar.dat):
//   healthind  416x44 — full panel chrome (portrait frame + bar slots +
//                       icon slots + name area composite bitmap)
//   healthbar  140x12 — red health bar fill
//   manabar    140x12 — purple mana bar fill
//   healthfade 140x12 — fade overlay (damage flash, deferred)
//   manafade   140x12 — fade overlay (deferred)
//
// Bar fill animation: clip the bar bitmap's width to (level * 140) so the
// fill shrinks toward zero as the stat depletes. Retail composes bar +
// fade into a TMosaicSurface; we draw bar bitmap then a black rect over
// the depleted portion (same visual, simpler plumbing).
//
// Stamina + portrait sprite + text are NOT yet wired (no staminabar asset
// in plyrstatusbar.dat — needs further investigation; portrait sprite
// comes from character data not playscrn.dat; text needs HUD-layer text
// pipeline). Each will land as we discover the right asset/pipeline.
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

namespace {

// Loaded TMulti for plyrstatusbar.dat. Held for the lifetime of the test
// mode. Owns the bitmap pointers below.
TMulti* g_psbData = nullptr;

PTBitmap g_healthInd  = nullptr;   // 416x44 panel backdrop
PTBitmap g_healthBar  = nullptr;   // 140x12 red fill
PTBitmap g_manaBar    = nullptr;   // 140x12 purple fill
PTBitmap g_healthFade = nullptr;   // 140x12 fade overlay
PTBitmap g_manaFade   = nullptr;   // 140x12 fade overlay

// Panel render position — anchored to top-left of screen for the test.
// Retail's TPlayScreen places the panel at a specific X,Y on its 640x480
// playfield; we use (0, 0) here to make the A/B clearer.
constexpr int32_t kPanelX = 4;
constexpr int32_t kPanelY = 4;

// Bar fill rect within the healthind backdrop — measured visually from
// plyr_stats_panel.png. These are offsets from (kPanelX, kPanelY) where
// each bar's left edge sits inside the backdrop's bar slot.
// TODO: confirm exact coords once the healthind bitmap renders + we can
// see where the bar slots actually are within it.
constexpr int32_t kBarSlotX = 65;     // approx — bars start ~65px from panel left
constexpr int32_t kBarRowY0 = 4;      // first bar Y
constexpr int32_t kBarRowPitch = 14;
constexpr int32_t kBarWidth = 140;    // matches bar bitmap width
constexpr int32_t kBarHeight = 12;    // matches bar bitmap height

class TPlyrStatusBarRealHud : public THudDrawable
{
public:
    void Draw() override
    {
        if (!g_healthInd) return;
        const double t = TTime::Time();

        // === Backdrop ===
        Renderer->DrawBitmap(g_healthInd, kPanelX, kPanelY);

        // === Bar fills ===
        // Synthetic levels matching the reference (1833 health ~85%,
        // 2174 mana ~95%, 191 stamina ~15%).
        const float healthLvl = 0.85f + 0.05f * float(std::sin(t * 0.7));
        const float manaLvl   = 0.95f + 0.05f * float(std::sin(t * 0.9 + 1.0));

        DrawBar(g_healthBar, 0, healthLvl);
        DrawBar(g_manaBar,   1, manaLvl);
        // Stamina (row 2) deferred — no `staminabar` asset in
        // plyrstatusbar.dat. Possibly tinted from one of these bitmaps
        // in retail, or in a different asset file.
    }

private:
    // Blit the bar bitmap into the panel's row, clipped horizontally to
    // (level * bar width) so the fill shrinks as the stat depletes.
    void DrawBar(PTBitmap bar, int32_t row, float level)
    {
        if (!bar) return;
        const int32_t bx = kPanelX + kBarSlotX;
        const int32_t by = kPanelY + kBarRowY0 + row * kBarRowPitch;
        // Always blit the full bar bitmap — the empty portion is masked
        // by a black rect drawn over it. Cheaper than a clipped blit
        // and keeps the bar's right-edge taper intact.
        Renderer->DrawBitmap(bar, bx, by);
        const float clamped = level < 0.0f ? 0.0f : (level > 1.0f ? 1.0f : level);
        const int32_t filled = int32_t(float(bar->width) * clamped);
        const int32_t emptyW = bar->width - filled;
        if (emptyW > 0)
        {
            // Cover the empty portion with the backdrop color (~ near
            // black to match the dark inside of the bar slot).
            // TODO: replace with actual healthind sub-rect blit so the
            // bar slot's empty background appears correctly.
            Renderer->DrawSolidRect(bx + filled, by, emptyW, bar->height,
                                    20, 18, 14, 255);
        }
    }
};

TPlyrStatusBarRealHud g_hud;

}  // namespace

bool InitializeUIPlyrStatusBarMode()
{
    log_info("[ui-plyrstatusbar] === TPlyrStatusBar (real assets, plyrstatusbar.dat) ===");

    g_psbData = TMulti::LoadMulti((char*)"plyrstatusbar.dat");
    if (!g_psbData)
    {
        log_warn("[ui-plyrstatusbar] plyrstatusbar.dat not loaded");
        return true;  // test mode still runs, just renders nothing
    }
    log_info("[ui-plyrstatusbar] plyrstatusbar.dat: %d assets", g_psbData->numoffsets);
    for (int32_t i = 0; i < g_psbData->numoffsets; ++i)
    {
        const char* nm = (const char*)g_psbData->names[i].ptr();
        PTBitmap bm = g_psbData->Bitmap(i);
        log_info("[ui-plyrstatusbar]   [%d] %s : %dx%d flags=0x%x",
                 i, nm ? nm : "?", bm ? bm->width : 0,
                 bm ? bm->height : 0, bm ? bm->flags : 0);

        // Cache by name (indexed lookup; safer than name lookup which
        // FatalErrors on miss).
        if (!nm) continue;
        if (!strcmp(nm, "healthind"))  g_healthInd  = bm;
        if (!strcmp(nm, "healthbar"))  g_healthBar  = bm;
        if (!strcmp(nm, "manabar"))    g_manaBar    = bm;
        if (!strcmp(nm, "healthfade")) g_healthFade = bm;
        if (!strcmp(nm, "manafade"))   g_manaFade   = bm;
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
    // TMulti pointers are owned by the resource cache; not freed here.
    g_healthInd = g_healthBar = g_manaBar = g_healthFade = g_manaFade = nullptr;
    g_psbData = nullptr;
}
