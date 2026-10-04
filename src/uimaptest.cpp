// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uimaptest.cpp - --test=ui-map                                        *
// *************************************************************************
//
// Clean-room reconstruction of MapSidebarPane (cls_0x5a5658, candidate
// TAutoMap; the lower-sidebar minimap). Built ONLY from
// docs/ui/forensics/MapPane_SPEC.md (with FORENSICS_PROTOCOL,
// UI_METHOD_MAP §12, RECONSTRUCTION_PROTOCOL).
//
// What this pane draws (spec §1 / §4 / §5):
//   - Amap chrome frame (automap.dat:Amap, 188x174, flags=0x10000) blitted at
//     the screen-anchored map-viewport origin VX=452, VY=306 (spec §3 globals
//     DAT_0065b4f4..500, decoded from the CRT inline-init thunk @ 0x4808a0).
//   - PlusSel zoom-in button (32x32, flags=0x10400) at map-viewport-local
//     +(0x14+8, 0x12+3) — i.e. screen (480, 327) at retail anchor (spec §4).
//   - MinusSel zoom-out button (32x32, flags=0x10400) at map-viewport-local
//     +(0x14-0x70, 0x12+3) — i.e. screen (360, 327) at retail anchor (spec §4).
//   - A handful of synthetic Marker blips (automap.dat:Marker, 16x16) at
//     fixed body-window positions inside the (20,18)-inset 144-wide body
//     window (spec §3 body inset + clip; §6c blip placement). Drawn through
//     the real retail Marker bitmap so the chrome-vs-blip pixel relationship
//     can be checked against the spec layout diagram (spec §3 ASCII art).
//
// OUT OF SCOPE (per task brief / spec §6e / §14 #1): the lit/scrolled 3D
// heightfield mesh body. The mesh path (meth_0x52af00 builds the vertex grid;
// meth_0x52b520 submits it through the cls_0x411eb0 display surface) belongs
// to the renderer 3D pipeline and is deferred. The body window is left
// transparent inside the chrome so the chrome + zoom-button + blip layout
// can be verified against retail without competing visual content.
//
// Architecture (spec §3 direct-renderer contract): compose chrome + buttons
// + synthetic blips into ONE offscreen TSurface render target via *ToTarget
// primitives, then DrawSurface it once at the map-viewport screen anchor.
// This mirrors uibottombartest / uiplyrstatusbartest / uisidetabstest — the
// canonical compose-to-target contract.
//
// Primitives (UI_METHOD_MAP §12 — canonical shared toolbox only):
//   Renderer->DrawBitmapToTarget        — opaque chrome stamp + button stamps
//                                          + Marker blips (asset-driven alpha)
// No hand-rolled shadow passes / glyph walks (the panel has no text, spec §8);
// no procedural stand-ins for assets (feedback_no_standins /
// feedback_ui_no_mock_use_retail).
//
// *************************************************************************

#include "uimaptest.h"

#include "bitmap.h"
#include "bitmapatlas.h"
#include "display.h"
#include "logging.h"
#include "multi.h"
#include "renderer.h"
#include "surface.h"
#include "time.h"

#include <cmath>
#include <cstdint>
#include <cstring>

namespace {

// =====================================================================
// Geometry constants — all cited to MapPane_SPEC.md.
// =====================================================================

// --- map-viewport rect on screen (spec §3 globals + objdump @ 0x4808a0) -
// DAT_0065b4f4=0x1c4=452 (VX); DAT_0065b4f8=0x132=306 (VY);
// DAT_0065b4fc=0xbc=188 (W);   DAT_0065b500=0xae=174 (H).
// The Amap chrome (188x174) defines the viewport size; the retail draws read
// these globals directly each frame (spec §3 frame table cite).
constexpr int32_t kViewportX = 452;   // §3 — VX (DAT_0065b4f4)
constexpr int32_t kViewportY = 306;   // §3 — VY (DAT_0065b4f8)
constexpr int32_t kViewportW = 188;   // §3 — DAT_0065b4fc (= Amap.w)
constexpr int32_t kViewportH = 174;   // §3 — DAT_0065b500 (= Amap.h)

// --- map-body inset inside the Amap chrome (spec §3 frame table) ------
// Body TL = map-viewport TL + (0x14, 0x12) = +(20, 18); body window clip
// width = 0x90 = 144 (set by meth_0x412150(...,0x90) at :702). Body window
// height is implied by the +0xa2=+162 blip y-clip bound (spec §6c).
constexpr int32_t kBodyInsetX = 0x14;   // 20 — §3 (cls_0x5a5658.cpp:686)
constexpr int32_t kBodyInsetY = 0x12;   // 18 — §3 (cls_0x5a5658.cpp:689)
constexpr int32_t kBodyClipW  = 0x90;   // 144 — §3/§6b (meth_0x412150 :702)
// Blip y-clip bound from §6c: blip_y in [body_y, DAT_0065b4f8+0xa2). The
// body height in display coords is +0xa2 − +0x12 = 0x90 = 144 (relative to
// the chrome top); we use that as the body window height for blip clipping.
constexpr int32_t kBodyClipH  = 0xa2 - 0x12;   // 144 — §6c blip y-clip span

// --- zoom-button offsets in map-viewport-local (spec §4 table) --------
// PlusSel  at +(kBodyInsetX + 8,    kBodyInsetY + 3) → screen (480, 327)
// MinusSel at +(kBodyInsetX - 0x70, kBodyInsetY + 3) → screen (360, 327)
// (cls_0x5a5658.cpp:1350-1357 — cVar4+8/cVar4-0x70 from the truncated
// (char)DAT_0065b4f4; here we apply the offsets directly to the chrome TL
// in map-viewport-local since we compose into an RT aligned to the chrome.)
constexpr int32_t kPlusBtnDX  = kBodyInsetX + 8;       // 28 — §4
constexpr int32_t kPlusBtnDY  = kBodyInsetY + 3;       // 21 — §4
constexpr int32_t kMinusBtnDX = kBodyInsetX - 0x70;    // -92 — §4 (left of chrome)
constexpr int32_t kMinusBtnDY = kBodyInsetY + 3;       // 21 — §4

// --- asset dims (spec §2 measured roster) -----------------------------
constexpr int32_t kAmapW    = 188;
constexpr int32_t kAmapH    = 174;
constexpr int32_t kMarkerW  = 16;       // §2 — Marker bitmap is 16x16 on disk
constexpr int32_t kMarkerH  = 16;
constexpr int32_t kBtnW     = 32;       // §2 — PlusSel/MinusSel 32x32
constexpr int32_t kBtnH     = 32;

// --- Marker is a 4x4 atlas of color-coded blip variants -----------------
// The on-disk Marker bitmap is 16x16 but it's laid out as a 4x4 GRID of
// 4x4-pixel color cells (visible in tools/ui/dump_dat.py output / the
// /tmp/Marker_argb4444 dump). Each row is a hue family (top=green/ally,
// then orange, then red/enemy, then blue) with 4 shades across. Retail
// rotates this into mbr_0x70/0x74 at init (§3 surfaces table) — selecting
// the right CELL per character (ally vs enemy, §6c UNCONFIRMED #4).
//
// To render ONE proper blip the test mode slices a single 4x4 cell out of
// the 16x16 atlas via DrawBitmapSubrectToTarget. We pick a different cell
// per synthetic blip below so the atlas's role as a color-variant table
// is exercised, not the whole-sheet-as-one-image stamp the spec's
// not-quite-right "16x16 blip sprite" reading would suggest.
constexpr int32_t kBlipFrameW = 4;
constexpr int32_t kBlipFrameH = 4;
constexpr int32_t kBlipFramesPerRow = kMarkerW / kBlipFrameW;   // 4

// =====================================================================
// Asset roster — spec §2 (all from automap.dat).
// =====================================================================
constexpr const char* kArchive      = "automap.dat";
constexpr const char* kMarkerName   = "Marker";    // §2 idx 0
constexpr const char* kAmapName     = "Amap";      // §2 idx 1
constexpr const char* kPlusSelName  = "PlusSel";   // §2 idx 2
constexpr const char* kMinusSelName = "MinusSel";  // §2 idx 3

TMulti*  g_automapDat = nullptr;
PTBitmap g_marker     = nullptr;
PTBitmap g_amap       = nullptr;
PTBitmap g_plusSel    = nullptr;
PTBitmap g_minusSel   = nullptr;

TSurface* g_pane = nullptr;     // composed RT (188 wide; tall enough for
                                // the chrome PLUS the MinusSel that sits
                                // 0x70 px left of the body inset — see
                                // EnsurePane / DrawSurface anchor below)
bool g_hudVisible = true;

// =====================================================================
// Synthetic blip set — spec §6c style, fixed body-window-local positions.
// These exist only to prove the body inset + clip math: each blip TL is in
// body-window-local coords (the +(20,18) inset), so a correct render lands
// the Marker fully inside the chrome viewport without overlapping the
// chrome border. One blip near the center represents the player (mbr_0x70,
// §6c "the player gets the same Marker"); the others ring out at fixed
// offsets, matching how nearby creatures would project (spec §6c formula).
//
// Animating these would not test additional geometry, so they're static.
// =====================================================================
// Per-blip atlas cell selection: (row, col) into the 4x4 Marker atlas.
// Row 0 = top hue family in source (greens), row 2 = reds (enemy-color),
// row 3 = blues. Picking a different cell per blip both demonstrates the
// atlas's role and yields a visually-distinguishable set of blips.
struct SBlip { int32_t bx; int32_t by; int32_t frameRow; int32_t frameCol; };
constexpr SBlip kBlips[] = {
    //  (body-local x, body-local y, frame_row, frame_col) — body-local TL
    //  clipped against [0, kBodyClipW) × [0, kBodyClipH); kBlipFrameW/H px
    //  sliced from Marker atlas at (frameCol*4, frameRow*4).
    {  70,  70, 3, 2 },   // player-equivalent — center, blue cell
    {  20,  20, 2, 0 },   // upper-left "enemy" — red cell
    { 120,  30, 2, 3 },   // upper-right "enemy" — red shade variant
    {  16, 120, 0, 1 },   // lower-left "ally"  — green cell
    { 120, 120, 1, 2 },   // lower-right "ally" — orange shade
};
constexpr int32_t kBlipCount = sizeof(kBlips) / sizeof(kBlips[0]);

// =====================================================================
// Asset lookup helper — same shape as the other ui*test panes.
// =====================================================================
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
// HUD drawable — composes the chrome + buttons + blips RT then
// DrawSurface's it once at the map-viewport screen anchor (spec §3).
// =====================================================================
//
// Compose layout inside the RT (RT-local coordinates):
//   The chrome (Amap) is 188x174 and the MinusSel button is at chrome-
//   local x = kMinusBtnDX = -92 (i.e. 92 px LEFT of the chrome's left
//   edge). To keep all the drawn pixels inside ONE RT we widen the RT to
//   cover the MinusSel range: RT spans chrome-local x in [-112, 188) and
//   y in [0, 174). RT width = 112 + 188 = 300; RT height = 174. We then
//   DrawSurface the RT at screen (kViewportX - 112, kViewportY) so the
//   chrome itself lands at the spec-mandated screen (452, 306).
//
// This is purely a compositing detail — the spec coords (chrome screen TL =
// (452,306); MinusSel screen TL = (360,327); PlusSel screen TL = (480,327))
// are honored exactly. See spec §4 table.
//
constexpr int32_t kRtLeftPad = 112;                         // covers MinusSel
constexpr int32_t kRtW       = kRtLeftPad + kViewportW;     // 300
constexpr int32_t kRtH       = kViewportH;                  // 174

class TMapHud : public THudDrawable
{
public:
    void Draw() override
    {
        if (!g_hudVisible || !g_pane) return;
        // Spec §3: chrome at screen (kViewportX, kViewportY). RT is left-
        // padded by kRtLeftPad to fit MinusSel (which sits left of chrome).
        Renderer->DrawSurface(g_pane, kViewportX - kRtLeftPad, kViewportY);
    }

    void Refresh()
    {
        if (!g_hudVisible) return;
        if (!g_amap) return;
        EnsurePane();
        if (!g_pane) return;

        const int32_t tw = g_pane->Width();
        const int32_t th = g_pane->Height();

        // Transparent clear — the chrome is opaque art (Amap covers its own
        // 188x174 rect) and the buttons live outside the chrome's left edge,
        // so anything outside the painted regions stays see-through to the
        // playfield (spec §3 — chrome composites on top, not as a plate).
        g_pane->StartPass(0.0f, 0.0f, 0.0f, 0.0f);

        // Chrome-TL in RT-local coords: shifted right by kRtLeftPad so the
        // MinusSel (chrome-local x = -0x70) can also fit in the RT.
        const int32_t chromeRtX = kRtLeftPad;
        const int32_t chromeRtY = 0;

        // Spec §6 step 3 / §5 step 3 (chrome overlay, meth_0x52bdb0 :1345-
        // 1348): the Amap full 188x174 stamped at the chrome TL. Drawn FIRST
        // here so the synthetic blips below composite ON TOP of the chrome's
        // body window (the body window is a fully-transparent region inside
        // the chrome art; blips landing there appear over the playfield in
        // production, and over the transparent clear in test mode).
        Renderer->DrawBitmapToTarget(g_amap, chromeRtX, chromeRtY, tw, th);

        // --- Synthetic Marker blips (spec §6c style; chrome-bug-free
        // §11 — no procedural stand-in, real Marker bitmap only) ---
        // Each blip is a single kBlipFrameW × kBlipFrameH cell sliced from
        // the 16x16 Marker atlas (NOT the whole 16x16 sheet — the source is
        // a 4x4 color grid, not a single blip sprite; see kBlipFrame*
        // constants for rationale). Body window is kBodyClipW × kBodyClipH
        // at chrome-local (kBodyInsetX, kBodyInsetY); blip TL is clipped
        // against the window since the to-target primitive doesn't take
        // a clip rect.
        if (g_marker)
        {
            for (int32_t i = 0; i < kBlipCount; ++i)
            {
                const int32_t bx = kBlips[i].bx;
                const int32_t by = kBlips[i].by;
                // Spec §6c clip: blip is INSIDE the body window iff its TL
                // is within [0, kBodyClipW) × [0, kBodyClipH). The retail
                // does the same point-in-rect check (`:644-650`).
                if (bx < 0 || bx >= kBodyClipW) continue;
                if (by < 0 || by >= kBodyClipH) continue;
                const int32_t dstX = chromeRtX + kBodyInsetX + bx;
                const int32_t dstY = chromeRtY + kBodyInsetY + by;
                // Pick the atlas cell. Marker is ARGB4444 (flags=0x10000),
                // so it carries its own per-pixel alpha — no chroma-key
                // applies (spec §7 chroma-key note is for the 15-bit
                // sprites; this is the 0x10000 hi-res variant).
                const int32_t srcX = kBlips[i].frameCol * kBlipFrameW;
                const int32_t srcY = kBlips[i].frameRow * kBlipFrameH;
                Renderer->DrawBitmapSubrectToTarget(
                    g_marker, dstX, dstY,
                    srcX, srcY, kBlipFrameW, kBlipFrameH,
                    tw, th);
            }
        }

        // Spec §6 step 3 / §4 zoom buttons (meth_0x52bdb0 :1350-1357). Both
        // are drawn unconditionally in the test mode (retail gates them on
        // mbr_0x9c/0xa0 — see spec §14 UNCONFIRMED #3 — but the test pane
        // is exercising the layout, not the in-game gating).
        if (g_plusSel)
        {
            // PlusSel chrome-local TL = (kBodyInsetX+8, kBodyInsetY+3).
            const int32_t dstX = chromeRtX + kPlusBtnDX;
            const int32_t dstY = chromeRtY + kPlusBtnDY;
            Renderer->DrawBitmapToTarget(g_plusSel, dstX, dstY, tw, th);
        }
        if (g_minusSel)
        {
            // MinusSel chrome-local TL = (kBodyInsetX-0x70, kBodyInsetY+3)
            //                          = (-92, 21) — sits LEFT of the chrome,
            // which is why the RT was left-padded by kRtLeftPad.
            const int32_t dstX = chromeRtX + kMinusBtnDX;
            const int32_t dstY = chromeRtY + kMinusBtnDY;
            Renderer->DrawBitmapToTarget(g_minusSel, dstX, dstY, tw, th);
        }

        g_pane->EndPass();
    }

private:
    void EnsurePane()
    {
        if (g_pane) return;
        // Spec §3: the panel is screen-anchored at fixed 4:3 coordinates
        // (DAT_0065b4f4..0065b500 are bare constants, not display-relative).
        // The RT is fixed-size to fit the chrome plus the MinusSel that
        // straddles its left edge (see header comment for compose layout).
        g_pane = new TSurface(kRtW, kRtH, SG_PIXELFORMAT_RGBA8);
    }
};

TMapHud g_hud;

}  // namespace

// =====================================================================
// Entry points (testmodes.cpp links these — keep the signatures).
// =====================================================================
bool InitializeUIMapMode()
{
    log_info("[ui-map] === MapSidebarPane (clean-room from spec) ===");

    // Spec §2: real retail automap.dat — Marker, Amap, PlusSel, MinusSel
    // in archive order (cls_0x5a5658.cpp:153-165 iterator). LoadMulti is
    // the standard archive path used by every other pane.
    g_automapDat = TMulti::LoadMulti((char*)kArchive);
    RegisterUIBitmapAtlasArchive(g_automapDat);

    if (g_automapDat)
    {
        g_marker   = LookupByName(g_automapDat, kMarkerName);
        g_amap     = LookupByName(g_automapDat, kAmapName);
        g_plusSel  = LookupByName(g_automapDat, kPlusSelName);
        g_minusSel = LookupByName(g_automapDat, kMinusSelName);
    }

    log_info("[ui-map] assets: Marker=%s Amap=%s PlusSel=%s MinusSel=%s",
             g_marker   ? "OK" : "MISS",
             g_amap     ? "OK" : "MISS",
             g_plusSel  ? "OK" : "MISS",
             g_minusSel ? "OK" : "MISS");

    if (g_amap)
        log_info("[ui-map] Amap %dx%d (expect %dx%d)",
                 g_amap->width, g_amap->height, kAmapW, kAmapH);
    if (g_marker)
        log_info("[ui-map] Marker %dx%d (expect %dx%d)",
                 g_marker->width, g_marker->height, kMarkerW, kMarkerH);
    if (g_plusSel)
        log_info("[ui-map] PlusSel %dx%d (expect %dx%d)",
                 g_plusSel->width, g_plusSel->height, kBtnW, kBtnH);
    if (g_minusSel)
        log_info("[ui-map] MinusSel %dx%d (expect %dx%d)",
                 g_minusSel->width, g_minusSel->height, kBtnW, kBtnH);

    delete g_pane;
    g_pane = nullptr;
    g_hudVisible = true;

    Renderer->AddHud(&g_hud, 0.0f);
    return true;
}

void RenderUIMapMode()
{
    RenderUIMapModeEmbedded();

    // Backdrop so the chrome's transparent body window + the Marker blips
    // read against a neutral surface (no playfield behind the HUD in test
    // mode). A muted earthy backdrop approximates the typical open-world
    // tile color so the body-window cutout reads as see-through (spec §3).
    Display.BackBuffer()->StartPass(0.18f, 0.20f, 0.26f, 1.0f);
    Display.BackBuffer()->EndPass();
}

void RenderUIMapModeEmbedded()
{
    if (!g_hudVisible) return;
    g_hud.Refresh();
}

void SetUIMapModeVisible(bool visible)
{
    g_hudVisible = visible;
}

void CloseUIMapMode()
{
    Renderer->RemoveHud(&g_hud);
    delete g_pane;
    g_pane       = nullptr;
    g_marker     = nullptr;
    g_amap       = nullptr;
    g_plusSel    = nullptr;
    g_minusSel   = nullptr;
    g_automapDat = nullptr;
    g_hudVisible = true;
}
