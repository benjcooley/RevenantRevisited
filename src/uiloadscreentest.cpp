// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uiloadscreentest.cpp - --test=ui-loadscreen                          *
// *************************************************************************
//
// Clean-room reconstruction of TLoadScreen (the boot / level-load progress
// screen, retail class cls_0x4485a0). Built ONLY from
// docs/ui/forensics/LoadingScreen_SPEC.md. Every coordinate / asset / formula
// cites its spec section inline.
//
// Anatomy (spec §2 / §4):
//   - Background  "background"  640x480  regx,regy=(0,0)   drawmode 0 (opaque)
//   - Bar         "bar"         584x48   regx,regy=(-24,-414)  BM_ALPHA
//       dst-anchor = (abs(regx), abs(regy)) = (24, 414)   absolute screen.
//       drawn as a LEFT slice: subrect (0,0, dwidth, 48),
//       dwidth = (progress * Bar.width) / 1000, progress in [0,1000]  (§6 alg).
//
// Architecture (spec §3 / §13 direct-swapchain note): the screen composes the
// two blits into ONE 640x480 offscreen TSurface render target via the
// `…ToTarget` primitive family, then DrawSurface's it once in the HUD pass --
// the same compose-to-target contract every other ui-* test mode follows
// (uiplyrstatusbartest.cpp). The backdrop is opaque so it fully covers the
// frame; the bar's BM_ALPHA gives the soft stone-corner endcaps automatically.
//
// This is NOT the in-game loadgame.def saved-game picker (cls_0x5b9584, a DEF
// screen owned by the other UI track) -- it is the tiny non-DEF, non-pane boot
// progress screen (spec §0).
//
// *************************************************************************

#include "uiloadscreentest.h"

#include "bitmap.h"
#include "bitmapdata.h"
#include "display.h"
#include "logging.h"
#include "multi.h"
#include "renderer.h"
#include "surface.h"
#include "time.h"

#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace {

// --- native asset frame (spec §2 / §3 — baked Classic 640x480) -----------
constexpr int32_t kScreenW = 640;
constexpr int32_t kScreenH = 480;

// --- progress accumulator (spec §1 / §6: integer per-mille [0,1000]) ------
constexpr int32_t kProgressFull = 1000;

// Synthetic ramp cadence for the test rig: fill 0->100% over kRampSecs, hold
// full briefly, then reset and loop so a screencap can catch the bar mid-fill
// (spec §9 "the bar grows then snaps to full"). Retail drives the accumulator
// from TPlayScreen::Initialize subsystem-init steps; here we ramp on a timer.
constexpr double kRampSecs = 3.0;
constexpr double kHoldSecs = 0.6;
constexpr double kCycleSecs = kRampSecs + kHoldSecs;

// =====================================================================
// Loaded assets (spec §2 — one archive, loadbar.dat).
// =====================================================================
TMulti*  g_loadbarDat = nullptr;
PTBitmap g_background  = nullptr;   // "background" 640x480, opaque
PTBitmap g_bar         = nullptr;   // "bar" 584x48, BM_ALPHA

TSurface* g_pane = nullptr;         // 640x480 compose RT

// =====================================================================
// HUD drawable — composes the screen RT then DrawSurface's it once
// (spec §3 / §13 direct-swapchain note), centered/letterboxed in the display.
// =====================================================================
class TLoadScreenHud : public THudDrawable
{
public:
    void Draw() override
    {
        if (!g_pane) return;
        // Classic asset paints unscaled at native pixels; at a larger display
        // it is centered/letterboxed by the upper display pipeline (spec §13).
        const int32_t x = (Display.Width()  - g_pane->Width())  / 2;
        const int32_t y = (Display.Height() - g_pane->Height()) / 2;
        Renderer->DrawSurface(g_pane, x > 0 ? x : 0, y > 0 ? y : 0);
    }

    void Refresh()
    {
        if (!g_background || !g_bar) return;
        EnsurePane();
        if (!g_pane) return;

        const int32_t progress = SyntheticProgress();
        const int32_t tw = g_pane->Width();
        const int32_t th = g_pane->Height();

        g_pane->StartPass(0.0f, 0.0f, 0.0f, 1.0f);

        // --- Background: opaque full-frame at (0,0) (spec §4 row 1 / §6.A) ---
        Renderer->DrawBitmapToTarget(g_background, 0, 0, tw, th);

        // --- Bar: left slice at the bitmap's absolute REG anchor (spec §4 row 2 /
        // §6.B). dst = (abs(regx), abs(regy)); subrect (0,0,dwidth,height);
        // dwidth = progress * Bar.width / 1000. BM_ALPHA -> soft endcaps. ---
        const int32_t dstX   = std::abs(g_bar->regx);                 // = 24
        const int32_t dstY   = std::abs(g_bar->regy);                 // = 414
        const int32_t dwidth = (progress * g_bar->width) / kProgressFull;
        if (dwidth > 0)
            Renderer->DrawBitmapSubrectToTarget(
                g_bar, dstX, dstY, /*sx*/0, /*sy*/0,
                /*sw*/dwidth, /*sh*/g_bar->height, tw, th);

        g_pane->EndPass();
    }

private:
    void EnsurePane()
    {
        const int32_t w = g_background ? g_background->width  : kScreenW;
        const int32_t h = g_background ? g_background->height : kScreenH;
        if (g_pane && g_pane->Width() == w && g_pane->Height() == h) return;
        delete g_pane;
        g_pane = new TSurface(w, h, SG_PIXELFORMAT_RGBA8);
    }

    // Timer-driven per-mille progress: ramp 0..1000 over kRampSecs, hold full
    // for kHoldSecs, then loop. (Test-rig driver for the §9 fill animation.)
    static int32_t SyntheticProgress()
    {
        const double t     = TTime::Time();
        const double phase = t - kCycleSecs * std::floor(t / kCycleSecs);
        if (phase >= kRampSecs) return kProgressFull;       // hold at 100%
        const double frac = phase / kRampSecs;              // 0..1 linear ramp
        int32_t p = (int32_t)(frac * kProgressFull);
        if (p < 0) p = 0;
        if (p > kProgressFull) p = kProgressFull;
        return p;
    }
};

TLoadScreenHud g_hud;

}  // namespace

// =====================================================================
// Entry points (testmodes.cpp links these — keep the signatures).
// =====================================================================
bool InitializeUILoadScreenMode()
{
    log_info("[ui-loadscreen] === TLoadScreen (clean-room from spec) ===");

    // Single archive (spec §2). loadbar.dat ships in resources.rvr.
    g_loadbarDat = TMulti::LoadMulti((char*)"loadbar.dat");
    if (g_loadbarDat)
    {
        g_background = g_loadbarDat->Bitmap((char*)"background");   // §6.C
        g_bar        = g_loadbarDat->Bitmap((char*)"bar");
    }

    log_info("[ui-loadscreen] assets: background=%s bar=%s",
             g_background ? "OK" : "MISS", g_bar ? "OK" : "MISS");
    if (g_background)
        log_info("[ui-loadscreen] background %dx%d reg(%d,%d)",
                 g_background->width, g_background->height,
                 g_background->regx, g_background->regy);
    if (g_bar)
        log_info("[ui-loadscreen] bar %dx%d reg(%d,%d) -> dst(%d,%d)",
                 g_bar->width, g_bar->height, g_bar->regx, g_bar->regy,
                 std::abs(g_bar->regx), std::abs(g_bar->regy));

    delete g_pane;
    g_pane = nullptr;

    Renderer->AddHud(&g_hud, 0.0f);
    return true;
}

void RenderUILoadScreenMode()
{
    g_hud.Refresh();

    // Clear the swapchain; the backdrop is opaque and fully covers the frame,
    // so the clear only shows in the letterbox bars at non-native resolutions.
    Display.BackBuffer()->StartPass(0.0f, 0.0f, 0.0f, 1.0f);
    Display.BackBuffer()->EndPass();
}

void CloseUILoadScreenMode()
{
    Renderer->RemoveHud(&g_hud);
    delete g_pane;
    g_pane       = nullptr;
    g_background = nullptr;
    g_bar        = nullptr;
    g_loadbarDat = nullptr;
}
