// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uibottombartest.cpp - --test=ui-bottombar                            *
// *************************************************************************
//
// Clean-room reconstruction of TBottomBarPane (the full-width 60px dark chrome
// strip pinned to the bottom of the play screen). Built ONLY from
// docs/ui/forensics/BottomBarPane_SPEC.md (with FORENSICS_PROTOCOL,
// UI_METHOD_MAP §12, RECONSTRUCTION_PROTOCOL).
//
// What this pane draws (spec §1 / §4 / §5):
//   - UtilityBar (bottombar.dat idx 0, 640x60 RGB555 opaque) X-stretched to
//     fill the full live pane width (= display width) at bar-pane-local (0,0).
//   - BarEndCap (bottombar.dat idx 2, 10x60 RGB555 opaque) stamped at the
//     right edge at bar-pane-local (w-10, 0).
//
// Both assets are 15-bit, kc=0 with NO alpha and NO chroma key — they are plain
// opaque plates (spec §7: "no drop shadow, no chroma key, no text").
//
// The pane is BOTTOM-anchored: pane_y = display_h - 60 (spec §3 corner
// verification). Width is greedy: live width tracks the display width via the
// retail SetRect path (spec §3 / §9).
//
// Architecture (spec §3 direct-renderer contract): compose the chrome into
// ONE offscreen TSurface render target via *ToTarget primitives, then
// DrawSurface it once in the HUD pass at (0, display_h-60). This mirrors the
// canonical pipeline used by uiplyrstatusbartest / uisidetabstest.
//
// Primitives (UI_METHOD_MAP §12 — canonical shared toolbox only):
//   Renderer->DrawBitmapSubrectStretchedToTarget — for the UtilityBar X-stretch
//       (the to-target twin of retail meth_0x4bd5e0, added once in renderer
//       per spec §13 / §14 "method-map gap"; reused going forward).
//   Renderer->DrawBitmapToTarget — for the BarEndCap opaque stamp (the to-
//       target twin of retail meth_0x4bd680 with drawmode 0x80000000).
//
// Sibling controls (spec §1 / §5): the quick-spell ring, the BarInv potion
// slots and the action arrows are OUT OF SCOPE for this pane — they belong to
// TQuickSpellPane / TBarInvPane and will render on top of the chrome plate
// once those panes are reconstructed. This test mode draws only the chrome.
//
// *************************************************************************

#include "uibottombartest.h"

#include "bitmap.h"
#include "bitmapatlas.h"
#include "display.h"
#include "hudstate.h"
#include "logging.h"
#include "multi.h"
#include "renderer.h"
#include "surface.h"

#include <cstdint>
#include <cstring>

namespace {

// =====================================================================
// Geometry constants — all cited to BottomBarPane_SPEC.md.
// =====================================================================

// Pane height — literal `0x3c = 60` from spec §3 / §4 (`cls_0x5a5808.cpp:36`).
// The Draw stretch passes dh = 0x3c regardless of the live mbr_0x10, so the
// chrome height is fixed at 60 (spec §4 footnote).
constexpr int32_t kBarH = 0x3c;          // 60 — chrome height (§3, §4)

// Sidebar width contribution — retail TPlayScreen::Pulse sets DAT_0066615c =
// 0xbc = 188 when the sidebar is OPEN, 0 when CLOSED (Pulse:84, Pulse:72).
// The bottom-bar live width formula is: display_w - DAT_0066615c
// (Pulse:269: `_DAT_0065be64 = (display_w - DAT_0065be5c) - DAT_0066615c`
//  where DAT_0065be5c is 0 for this pane — cite Pulse_47b4d0.cpp:269).
// We read SHudState::sidebarState each frame (versions-over-flags pattern).
constexpr int32_t kSidebarW = 0xbc;      // 188 — sidebar chrome width (Pulse:84)

// Source bitmap dimensions (spec §2 asset roster — measured via dump_dat.py).
constexpr int32_t kUtilityBarSrcW = 640; // measured (§2)
constexpr int32_t kUtilityBarSrcH = 60;  // measured (§2)
constexpr int32_t kBarEndCapW     = 10;  // measured (§2); literal `0xa = 10` at §6 call table
constexpr int32_t kBarEndCapH     = 60;  // measured (§2)

// Asset names (spec §2 — archive bottombar.dat, entries 0 + 2).
constexpr const char* kArchive     = "bottombar.dat";
constexpr const char* kUtilityName = "UtilityBar";   // §2 idx 0 (`cls_0x5a5808.cpp:181`)
constexpr const char* kEndCapName  = "BarEndCap";    // §2 idx 2 (`cls_0x5a5808.cpp:182`)

// =====================================================================
// Loaded assets + cached RT.
// =====================================================================
TMulti*   g_bottombarDat = nullptr;
PTBitmap  g_utilityBar   = nullptr;
PTBitmap  g_barEndCap    = nullptr;

TSurface* g_pane         = nullptr;
int32_t   g_paneW        = 0;   // live pane width (this+0xc, §3) = display width

PTBitmap LookupByName(TMulti* m, const char* name)
{
    if (!m || !name) return nullptr;
    for (int32_t i = 0; i < m->numoffsets; ++i)
    {
        const char* nm = (const char*)m->names[i].ptr();
        if (nm && !std::strcmp(nm, name))
            return m->Bitmap(i);
    }
    return nullptr;
}

// =====================================================================
// HUD drawable — composes the chrome RT then DrawSurface's it once
// at the bottom-anchored screen origin (spec §3 / §5).
// =====================================================================
class TBottomBarHud : public THudDrawable
{
public:
    void Draw() override
    {
        if (!GetHudState().bottomBarOpen) return;
        if (!g_pane) return;
        // Spec §3: pane_y = display_h - 60 (bottom-anchored, mandatory formula).
        const int32_t dh = Display.Height();
        const int32_t y  = (dh > 0 ? dh : kBarH) - kBarH;
        Renderer->DrawSurface(g_pane, 0, y);
    }

    void Refresh()
    {
        if (!GetHudState().bottomBarOpen) return;
        if (!g_utilityBar || !g_barEndCap) return;
        EnsurePane();
        if (!g_pane) return;

        const int32_t tw = g_pane->Width();
        const int32_t th = g_pane->Height();

        // Opaque clear — the bar is OPAQUE chrome (spec §7: "no chroma key,
        // no alpha"). Clearing transparent would leak the playfield through
        // any unstretched gap; the chrome itself covers every pixel, but a
        // black clear is the closest match to "solid plate" intent.
        g_pane->StartPass(0.0f, 0.0f, 0.0f, 1.0f);

        // Spec §6 step 1 (chrome `0x52c880`, `cls_0x5a5808.cpp:181`):
        //   meth_0x4bd5e0(surf, 0, 0, UtilityBar, 0, 0, mbr_0xc(=w), 0x3c(=60))
        // = sub-rect → stretch blit of the full UtilityBar bitmap (sx=0, sy=0,
        //   sw=640, sh=60) into the dst rect (0, 0, w, 60) at the target. The
        //   chrome is X-stretched to fill the live pane width; Y is fixed at
        //   60 (NOT mbr_0x10 — spec §4 footnote).
        // Port primitive: DrawBitmapSubrectStretchedToTarget — the to-target
        // twin of retail meth_0x4bd5e0 (spec §13 / §14 "method-map gap"; added
        // once in renderer.{h,cpp} for reuse).
        Renderer->DrawBitmapSubrectStretchedToTarget(
            g_utilityBar,
            /*dst*/ 0, 0, tw, kBarH,
            /*src*/ 0, 0, kUtilityBarSrcW, kUtilityBarSrcH,
            tw, th);

        // Spec §6 step 2 (chrome `0x52c880`, `cls_0x5a5808.cpp:182`):
        //   meth_0x4bd680(surf, mbr_0xc - 0xa, 0, BarEndCap, 0x80000000)
        // = opaque sprite stamp of the full 10x60 BarEndCap at (w-10, 0).
        //   `0x80000000` = DM_USEDEFAULT → opaque copy (UI_METHOD_MAP §1).
        // Port primitive: DrawBitmapToTarget (opaque copy at dst, full size).
        Renderer->DrawBitmapToTarget(
            g_barEndCap,
            /*dst*/ tw - kBarEndCapW, 0,
            tw, th);

        g_pane->EndPass();
    }

private:
    void EnsurePane()
    {
        // Spec §3: live pane width = display width (greedy, X-stretched),
        // minus sidebar width when the sidebar is OPEN (item #3 in the
        // HUD verification list). Retail formula: display_w - DAT_0066615c
        // where DAT_0066615c is 0xbc=188 (OPEN) or 0 (CLOSED).
        // Cite: TPlayScreen::Pulse_47b4d0.cpp:269, Pulse:84, Pulse:72.
        const int32_t dw         = Display.Width();
        const SHudState& hs      = GetHudState();
        const int32_t sidebarAdj = (hs.sidebarState == HUD_SIDEBAR_OPEN) ? kSidebarW : 0;
        const int32_t want       = (dw > 0 ? dw : kUtilityBarSrcW) - sidebarAdj;
        if (g_pane && g_paneW == want) return;
        delete g_pane;
        g_paneW = want;
        // Compose the WHOLE bar (UtilityBar + BarEndCap) into one RT of
        // (live_w x 60) — spec §3 direct-renderer contract.
        g_pane = new TSurface(g_paneW > 0 ? g_paneW : kUtilityBarSrcW, kBarH, SG_PIXELFORMAT_RGBA8);
    }
};

TBottomBarHud g_hud;

}  // namespace

// =====================================================================
// Entry points (testmodes.cpp links these — keep the signatures).
// =====================================================================
bool InitializeUIBottomBarMode()
{
    log_info("[ui-bottombar] === TBottomBarPane (clean-room from spec) ===");

    // Spec §2: real retail bottombar.dat — UtilityBar (idx 0) + BarEndCap (idx
    // 2). LoadMulti goes through the standard archive path used by all panes.
    g_bottombarDat = TMulti::LoadMulti((char*)kArchive);
    RegisterUIBitmapAtlasArchive(g_bottombarDat);

    if (g_bottombarDat)
    {
        g_utilityBar = LookupByName(g_bottombarDat, kUtilityName);
        g_barEndCap  = LookupByName(g_bottombarDat, kEndCapName);
    }

    log_info("[ui-bottombar] assets: UtilityBar=%s BarEndCap=%s",
             g_utilityBar ? "OK" : "MISS",
             g_barEndCap  ? "OK" : "MISS");

    if (g_utilityBar)
        log_info("[ui-bottombar] UtilityBar %dx%d (expect 640x60)",
                 g_utilityBar->width, g_utilityBar->height);
    if (g_barEndCap)
        log_info("[ui-bottombar] BarEndCap %dx%d (expect 10x60)",
                 g_barEndCap->width, g_barEndCap->height);

    delete g_pane;
    g_pane  = nullptr;
    g_paneW = 0;

    Renderer->AddHud(&g_hud, 0.0f);
    return true;
}

void RenderUIBottomBarMode()
{
    RenderUIBottomBarModeEmbedded();

    // Backdrop so the bar's bottom-anchored placement reads clearly in
    // isolation (no playfield behind it in test mode). A muted slate-blue
    // approximates the open-world tint enough that the dark bar plate
    // contrasts against it visually.
    Display.BackBuffer()->StartPass(0.18f, 0.20f, 0.26f, 1.0f);
    Display.BackBuffer()->EndPass();
}

void RenderUIBottomBarModeEmbedded()
{
    g_hud.Refresh();
}

void CloseUIBottomBarMode()
{
    Renderer->RemoveHud(&g_hud);
    delete g_pane;
    g_pane         = nullptr;
    g_paneW        = 0;
    g_utilityBar   = nullptr;
    g_barEndCap    = nullptr;
    g_bottombarDat = nullptr;
}
