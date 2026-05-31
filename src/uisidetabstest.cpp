// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uisidetabstest.cpp - --test=ui-sidetabs                              *
// *************************************************************************
//
// Pixel-faithful TSideTabsPane harness using the real retail strips from
// sidebartabsnotex.dat. Built directly from TSideTabsPane_SPEC.md.
//
// The pane is the vertical strip of six round icon buttons that runs down
// the inner edge of the in-game sidebar. It is NOT a flat 1-of-6 picker:
// it is TWO independent 3-way region selectors (spec §1):
//   - Upper region (modal DAT_0065d1b8 in {0,1,2}): Book / Stats / Equip
//   - Lower region (modal DAT_0065d1bc in {0,1,2}): Spell / Inv / Map
//
// Asset roster (spec §2): three 52x232 vertical sprite strips, same dims:
//   Up      idx 0 — idle / unpressed art (all 6 icons)
//   Down    idx 1 — pressed art
//   Select  idx 2 — highlighted / current-selection art
// All RGB555 (flags=0x2), kc=0x0 → they key pure-black 0x0000 to alpha and
// carry real alpha otherwise; the normal TMulti decode handles this — we do
// NOT additionally force the magenta key (spec §11).
//
// Per-button source rect == dest rect within the strip (spec §4):
//   x=3, w=0x24=36, h=0x23=35; y in {2, 0x27, 0x4c, 0x71, 0x96, 0xbb}
//   (=2, 39, 76, 113, 150, 187), band stride 0x25 = 37.
//
// Draw model (spec §5/§6, Classic DrawBackground path):
//   a. blit the base Up strip opaque, full 52x232, at the pane origin
//      (paints all six buttons resting / idle).
//   b. for the CURRENT SELECTION of each region: overlay its Down (pressed)
//      sub-rect at full alpha — the permanently-pressed radio state (spec §9/§10).
//   c. for each button with hover ramp cur>0: overlay its Select sub-rect
//      tinted with alpha = round(cur*255/8) — the hover fade (spec §6).
//
// Hover fade ramp (spec §9): per-button int cur in 0..8, steps +/-1 per sim
// tick (24Hz) toward target (8 hovered, 0 not). Gated to kSimTickMs.
//
// *************************************************************************

#include "uisidetabstest.h"

#include "bitmap.h"
#include "display.h"
#include "logging.h"
#include "multi.h"
#include "renderer.h"
#include "revenant.h"
#include "surface.h"
#include "time.h"

#include <cstdint>
#include <cstring>

namespace {

// ---- strip geometry (spec §2/§4) ----------------------------------------
constexpr int32_t kStripW    = 52;
constexpr int32_t kStripH    = 232;
constexpr int32_t kBtnX      = 3;         // pane-local x, same for all (spec §4)
constexpr int32_t kBtnW      = 0x24;      // 36
constexpr int32_t kBtnH      = 0x23;      // 35
constexpr int32_t kBtnCount  = 6;
constexpr int32_t kBtnY[kBtnCount] = { 2, 0x27, 0x4c, 0x71, 0x96, 0xbb };  // 2,39,76,113,150,187

// ---- hover-fade ramp (spec §9) ------------------------------------------
constexpr int32_t kRampMax    = 8;
constexpr double  kSimTickMs  = 1000.0 / 24.0;   // 24Hz sim gate (protocol rule 6)

// ---- pane placement on screen -------------------------------------------
// The retail pane is right-anchored:
//   pane_x = display_w - DAT_0066614c - DAT_0065be5c
//   pane_y = DAT_00667c60 - DAT_0065be60
// Those are BSS layout constants (spec §3 UNCONFIRMED-A) — not body literals,
// so they cannot be reproduced exactly here. For this isolated harness we
// right-anchor the strip near the screen's right edge with a small inset,
// which matches the in-game placement (inner edge of the sidebar). The
// strip's own size (52x232) and per-button layout ARE exact from the spec.
constexpr int32_t kPaneRightInset = 8;    // harness-only inset; see UNCONFIRMED-A
constexpr int32_t kPaneTopInset   = 24;   // harness-only inset; see UNCONFIRMED-A

// Region modal state (spec §10): which mode index is the current selection.
//   upper region buttons: index 0=Book, 1=Stats, 2=Equip
//   lower region buttons: index 0=Spell, 1=Inv, 2=Map
// (Button add-order; the modal value picks which is held pressed.)
struct SButton {
    int32_t  y      = 0;     // pane-local y (= source y in strip)
    int32_t  region = 0;     // 0 = upper, 1 = lower
    int32_t  mode   = 0;     // mode index within its region (0..2)
    int32_t  cur    = 0;     // hover-fade ramp counter (0..8)
    int32_t  target = 0;     // ramp target (0 or 8)
    bool     hovered = false;
};

// Add-order matches spec §4 (Book,Stats,Equip,Spell,Inv,Map). region/mode
// per spec §10 modal-state mapping. Upper modes: Book selects upper=2 visual,
// but for the harness we model each button's "is selected" as
// (region modal == its add-index within the region) which is sufficient to
// exercise the pressed/idle/hover art per button.
SButton g_buttons[kBtnCount] = {
    { kBtnY[0], 0, 0, 0, 0, false },  // Book   (upper, slot 0)
    { kBtnY[1], 0, 1, 0, 0, false },  // Stats  (upper, slot 1)
    { kBtnY[2], 0, 2, 0, 0, false },  // Equip  (upper, slot 2)
    { kBtnY[3], 1, 0, 0, 0, false },  // Spell  (lower, slot 0)
    { kBtnY[4], 1, 1, 0, 0, false },  // Inv    (lower, slot 1)
    { kBtnY[5], 1, 2, 0, 0, false },  // Map    (lower, slot 2)
};

// Current selection per region (spec §10 modal globals, synthetic here).
int32_t g_upperSel = 0;   // 0..2 → which upper button is held pressed
int32_t g_lowerSel = 0;   // 0..2 → which lower button is held pressed

TMulti*   g_sidetabsDat = nullptr;
PTBitmap  g_up      = nullptr;   // strip 0 — idle
PTBitmap  g_down    = nullptr;   // strip 1 — pressed
PTBitmap  g_select  = nullptr;   // strip 2 — highlight
TSurface* g_pane    = nullptr;

double    g_lastTickMs = 0.0;

// round(cur * 255 / 8) — the integer hover-fade alpha (spec §6 table).
int32_t RampAlpha(int32_t cur)
{
    if (cur <= 0) return 0;
    if (cur >= kRampMax) return 255;
    return (cur * 255 + 4) / 8;   // round-to-nearest, cur in [1,7]
}

bool IsSelected(const SButton& b)
{
    return b.region == 0 ? (b.mode == g_upperSel) : (b.mode == g_lowerSel);
}

class TSideTabsHud : public THudDrawable
{
public:
    void Draw() override
    {
        if (!g_pane) return;
        Renderer->DrawSurface(g_pane, m_paneX, m_paneY);
    }

    void Refresh()
    {
        if (!g_up) return;
        EnsurePane();
        if (!g_pane) return;

        AdvanceRamps();
        UpdateSyntheticState();

        const int32_t tw = g_pane->Width();
        const int32_t th = g_pane->Height();

        // Transparent clear so the strip's keyed (black 0x0000) pixels and
        // inter-button gaps show the background, not a fill (spec §3).
        g_pane->StartPass(0.0f, 0.0f, 0.0f, 0.0f);

        // (a) base strip — full Up art, opaque, at pane origin (spec §5.2a).
        Renderer->DrawBitmapSubrectToTarget(g_up, 0, 0, 0, 0, kStripW, kStripH, tw, th);

        // (b) per-region current selection held pressed → Down art at full
        //     alpha (spec §9/§10 radio pressed state).
        for (const SButton& b : g_buttons)
        {
            if (!IsSelected(b)) continue;
            Renderer->DrawBitmapSubrectToTarget(
                g_down, kBtnX, b.y, kBtnX, b.y, kBtnW, kBtnH, tw, th);
        }

        // (c) hover-fade overlay — Select art tinted by ramp alpha (spec §6).
        for (const SButton& b : g_buttons)
        {
            const int32_t a = RampAlpha(b.cur);
            if (a <= 0) continue;
            Renderer->DrawBitmapSubrectTintedToTarget(
                g_select, kBtnX, b.y, kBtnX, b.y, kBtnW, kBtnH, tw, th,
                1.0f, 1.0f, 1.0f, float(a) / 255.0f);
        }

        g_pane->EndPass();
    }

private:
    void EnsurePane()
    {
        if (g_pane) { PlacePane(); return; }
        g_pane = new TSurface(kStripW, kStripH, SG_PIXELFORMAT_RGBA8);
        PlacePane();
    }

    void PlacePane()
    {
        const int32_t dw = Display.Width();
        m_paneX = (dw > 0 ? dw : kStripW + 2 * kPaneRightInset)
                  - kStripW - kPaneRightInset;
        if (m_paneX < 0) m_paneX = 0;
        m_paneY = kPaneTopInset;
    }

    // Hover-fade ramp advance, gated to 24Hz (spec §9, protocol rule 6).
    static void AdvanceRamps()
    {
        const double nowMs = TTime::Time() * 1000.0;
        if (g_lastTickMs == 0.0) g_lastTickMs = nowMs;
        int32_t guard = 0;
        while (nowMs - g_lastTickMs >= kSimTickMs && guard < 64)
        {
            g_lastTickMs += kSimTickMs;
            ++guard;
            for (SButton& b : g_buttons)
            {
                b.target = b.hovered ? kRampMax : 0;
                if (b.cur < b.target)      ++b.cur;
                else if (b.target < b.cur) --b.cur;
            }
        }
    }

    // Synthetic driver: cycle both region selections and the hover focus so a
    // single capture exercises idle / pressed / hovering and the two-region
    // independence (protocol test-harness requirement; spec §10 model).
    static void UpdateSyntheticState()
    {
        const double t = TTime::Time();

        // Region selections advance on independent periods so they are
        // visibly out of phase (two independent selectors).
        g_upperSel = int32_t(t / 2.0) % 3;          // changes every 2 s
        g_lowerSel = (int32_t(t / 3.0) + 1) % 3;    // changes every 3 s, offset

        // Hover focus walks down the six buttons, ~1.5 s each, so the fade
        // ramps in and out repeatedly across the full button set.
        const int32_t hoverIdx = int32_t(t / 1.5) % kBtnCount;
        for (int32_t i = 0; i < kBtnCount; ++i)
            g_buttons[i].hovered = (i == hoverIdx);
    }

    int32_t m_paneX = 0;
    int32_t m_paneY = 0;
};

TSideTabsHud g_hud;

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

bool InitializeUISideTabsMode()
{
    log_info("[ui-sidetabs] === TSideTabsPane (real retail strips) ===");

    // Classic-faithful: use the RGB555 `notex` strips (spec §2/§13). The
    // hi-res `sidebartabs.dat` 0x10000 variant is UNCONFIRMED-E and not needed.
    g_sidetabsDat = TMulti::LoadMulti((char*)"sidebartabsnotex.dat");

    g_up     = LookupByName(g_sidetabsDat, "Up");
    g_down   = LookupByName(g_sidetabsDat, "Down");
    g_select = LookupByName(g_sidetabsDat, "Select");

    log_info("[ui-sidetabs] strips: Up=%s Down=%s Select=%s",
             g_up ? "OK" : "MISS",
             g_down ? "OK" : "MISS",
             g_select ? "OK" : "MISS");

    if (g_up)
        log_info("[ui-sidetabs] Up strip %dx%d (expect 52x232)",
                 g_up->width, g_up->height);

    delete g_pane;
    g_pane = nullptr;
    g_lastTickMs = 0.0;

    Renderer->AddHud(&g_hud, 0.0f);
    return true;
}

void RenderUISideTabsMode()
{
    g_hud.Refresh();

    // Mid-gray backdrop so the strip's transparent gaps and the round-button
    // edges are visible (no playfield behind the panel in isolation).
    Display.BackBuffer()->StartPass(0.45f, 0.45f, 0.48f, 1.0f);
    Display.BackBuffer()->EndPass();
}

void CloseUISideTabsMode()
{
    Renderer->RemoveHud(&g_hud);
    delete g_pane;
    g_pane = nullptr;
    g_up = g_down = g_select = nullptr;
    g_sidetabsDat = nullptr;
    g_lastTickMs = 0.0;
}
