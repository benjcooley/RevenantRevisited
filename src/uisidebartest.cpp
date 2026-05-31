// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uisidebartest.cpp - --test=ui-sidebar                                *
// *************************************************************************
//
// Sidebar integration: TSideTabsPane (six-button tab strip) + currently-
// selected top-slot pane chrome (Equip/Spellbook/Stats) + currently-
// selected bottom-slot pane chrome (Inv/Map/SpellCreate).
//
// State lives in `SHudState` (hudstate.h) so it round-trips to disk.
//
// v1 scope: chrome composition + synthetic state cycling demonstrates the
// multiplex. Real mouse-click tab switching + keyboard open/close land in
// the full --test=ui-hud follow-up.
//
// Position (Classic 640x480 reference):
//   - Pane content right-anchored at x = display_w − 188; width 188.
//   - Top slot:    y=0,   h=306 (Equip 188x306 / Scroll 188x306 / Stats 188x306)
//   - Bottom slot: y=306, h=174 (Inv 188x174 / Amap 188x174 / spellconstr 188x174)
//   - Side-tabs strip: 52x232 overlaying the right edge of the pane block.
//     Strip x = display_w − 52 − inset; strip y = ~24 (UNCONFIRMED-A in spec —
//     BSS constants DAT_0066614c/DAT_0065be5c/DAT_00667c60/DAT_0065be60 are
//     read at TSideTabsPane Initialize 0x53cc30 but their VALUES are not yet
//     extracted from the shipped Revenant.exe BSS init; the existing
//     uisidetabstest.cpp approximation is reused here and refined in the
//     pixel-perfect alignment pass).
//
// *************************************************************************

#include "uisidebartest.h"

#include "bitmap.h"
#include "display.h"
#include "hudstate.h"
#include "logging.h"
#include "multi.h"
#include "renderer.h"
#include "revdefs.h"
#include "revenant.h"
#include "surface.h"
#include "testmodes.h"
#include "time.h"
#include "uidragstate.h"

#include <cstdint>
#include <cstring>

namespace {

// ---- side-tabs strip (per uisidetabstest / TSideTabsPane_SPEC) ----------
constexpr int32_t kStripW   = 52;
constexpr int32_t kStripH   = 232;
constexpr int32_t kBtnX     = 3;
constexpr int32_t kBtnW     = 0x24;   // 36
constexpr int32_t kBtnH     = 0x23;   // 35
constexpr int32_t kBtnCount = 6;
constexpr int32_t kBtnY[kBtnCount] = { 2, 0x27, 0x4c, 0x71, 0x96, 0xbb };

// ---- pane block (Classic-reference) -------------------------------------
constexpr int32_t kPaneW    = 188;
constexpr int32_t kTopH     = 306;
constexpr int32_t kBotH     = 174;

// Approximations matching uisidetabstest.cpp (UNCONFIRMED-A: BSS constants
// not yet extracted; refined in pixel-perfect alignment pass).
constexpr int32_t kPaneRightInset = 0;
constexpr int32_t kTabsRightInset = 8;
constexpr int32_t kTabsTopInset   = 24;

// ---- hover-fade ramp (spec §6/§9) ---------------------------------------
constexpr int32_t kRampMax    = 8;
constexpr double  kSimTickMs  = 1000.0 / 24.0;

// ---- assets --------------------------------------------------------------
// Sidebar tab strip
TMulti*  g_sidetabsDat   = nullptr;
PTBitmap g_sidetabsUp    = nullptr;
PTBitmap g_sidetabsDown  = nullptr;
PTBitmap g_sidetabsSel   = nullptr;

// Top-slot chrome (one of three visible at a time, gated on HudState.topSlot)
TMulti*  g_equipDat      = nullptr;   PTBitmap g_equipChrome     = nullptr;
TMulti*  g_spellscrollDat= nullptr;   PTBitmap g_spellbookChrome = nullptr;
TMulti*  g_statpaneDat   = nullptr;   PTBitmap g_statsChrome     = nullptr;

// Bottom-slot chrome
TMulti*  g_inventoryDat  = nullptr;   PTBitmap g_invChrome       = nullptr;
TMulti*  g_automapDat    = nullptr;   PTBitmap g_mapChrome       = nullptr;
TMulti*  g_spellpaneDat  = nullptr;   PTBitmap g_spellcreateChrome = nullptr;

// Composite tab-strip surface (kept across frames; rebuilt every Refresh)
TSurface* g_stripSurface = nullptr;

// Hover-fade per button (synthetic for v1)
struct SButton {
    int32_t  y       = 0;
    int32_t  region  = 0;  // 0 = upper, 1 = lower
    int32_t  mode    = 0;
    int32_t  cur     = 0;
    int32_t  target  = 0;
    bool     hovered = false;
};
// Each button stores the MODE VALUE it sets in SHudState when clicked
// (i.e. one of HUD_TOP_* or HUD_BOT_* depending on region). This is NOT
// the button's index — the button order in the strip is Book/Stats/Equip
// (upper) then Spell/Inv/Map (lower), but the modal-int values are:
//   topSlot:    HUD_TOP_EQUIP=0  HUD_TOP_STATS=1  HUD_TOP_BOOK=2
//   bottomSlot: HUD_BOT_INV=0    HUD_BOT_MAP=1    HUD_BOT_SPELL=2
SButton g_buttons[kBtnCount] = {
    { kBtnY[0], 0, HUD_TOP_BOOK,  0, 0, false },  // upper btn 0 - Book
    { kBtnY[1], 0, HUD_TOP_STATS, 0, 0, false },  // upper btn 1 - Stats
    { kBtnY[2], 0, HUD_TOP_EQUIP, 0, 0, false },  // upper btn 2 - Equip
    { kBtnY[3], 1, HUD_BOT_SPELL, 0, 0, false },  // lower btn 0 - Spell composer
    { kBtnY[4], 1, HUD_BOT_INV,   0, 0, false },  // lower btn 1 - Inv
    { kBtnY[5], 1, HUD_BOT_MAP,   0, 0, false },  // lower btn 2 - Map
};
double g_lastTickMs = 0.0;

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

// Map SHudState.topSlot (EQUIP=0, STATS=1, BOOK=2) and SHudState.bottomSlot
// (INV=0, MAP=1, SPELL=2) to which button in g_buttons[] is the held-pressed
// "selected" art (modal->button index per spec §10).
//
// Upper buttons in add-order:    [0]=Book(mode 2), [1]=Stats(mode 1), [2]=Equip(mode 0)
// Lower buttons in add-order:    [3]=Spell(mode 2),[4]=Inv(mode 0),   [5]=Map(mode 1)
bool IsButtonSelected(int32_t idx, const SHudState& s)
{
    const SButton& b = g_buttons[idx];
    if (b.region == 0)
        return b.mode == s.topSlot;
    return b.mode == s.bottomSlot;
}

int32_t RampAlpha(int32_t cur)
{
    if (cur <= 0) return 0;
    if (cur >= kRampMax) return 255;
    return (cur * 255 + 4) / 8;
}

PTBitmap CurrentTopChrome(const SHudState& s)
{
    switch (s.topSlot)
    {
        case HUD_TOP_EQUIP: return g_equipChrome;
        case HUD_TOP_STATS: return g_statsChrome;
        case HUD_TOP_BOOK:  return g_spellbookChrome;
    }
    return nullptr;
}

PTBitmap CurrentBottomChrome(const SHudState& s)
{
    switch (s.bottomSlot)
    {
        case HUD_BOT_INV:   return g_invChrome;
        case HUD_BOT_MAP:   return g_mapChrome;
        case HUD_BOT_SPELL: return g_spellcreateChrome;
    }
    return nullptr;
}

const char* TopSlotName(int32_t s)
{
    switch (s) {
        case HUD_TOP_EQUIP: return "Equip";
        case HUD_TOP_STATS: return "Stats";
        case HUD_TOP_BOOK:  return "Book";
    }
    return "?";
}
const char* BottomSlotName(int32_t s)
{
    switch (s) {
        case HUD_BOT_INV:   return "Inv";
        case HUD_BOT_MAP:   return "Map";
        case HUD_BOT_SPELL: return "Spell";
    }
    return "?";
}

// Synthetic driver — cycles modal states + open/close so a single capture
// exercises the multiplex without real input wiring. SUPPRESSED when an
// --input-script is active so scripted clicks fully drive the state.
int32_t g_lastLoggedTop  = -1;
int32_t g_lastLoggedBot  = -1;
int32_t g_lastLoggedOpen = -1;
void DriveSyntheticState()
{
    // When an input-script is running, the script owns state — don't cycle.
    if (TestModes::InputScriptActive()) return;

    SHudState& s = GetHudState();

    const double t = TTime::Time();

    // Top slot cycles every 2.5 s; bottom every 3.5 s (out of phase).
    s.topSlot    = int32_t(t / 2.5) % 3;
    s.bottomSlot = (int32_t(t / 3.5) + 1) % 3;

    // Sidebar open for 8 s, closed for 3 s, repeat (11 s cycle).
    const int32_t cycle = int32_t(t) % 11;
    s.sidebarState = (cycle < 8) ? HUD_SIDEBAR_OPEN : HUD_SIDEBAR_CLOSED;

    // Log state transitions for the harness log so we can verify cycling
    // ran during the run window even if visual capture races the window.
    if (s.topSlot != g_lastLoggedTop ||
        s.bottomSlot != g_lastLoggedBot ||
        s.sidebarState != g_lastLoggedOpen)
    {
        log_info("[ui-sidebar] state: top=%s bottom=%s sidebar=%s",
                 TopSlotName(s.topSlot),
                 BottomSlotName(s.bottomSlot),
                 s.sidebarState == HUD_SIDEBAR_OPEN ? "OPEN" : "CLOSED");
        g_lastLoggedTop  = s.topSlot;
        g_lastLoggedBot  = s.bottomSlot;
        g_lastLoggedOpen = s.sidebarState;
    }

    // Hover focus walks down the 6 buttons (visual interest only).
    const int32_t hoverIdx = int32_t(t / 1.5) % kBtnCount;
    for (int32_t i = 0; i < kBtnCount; ++i)
        g_buttons[i].hovered = (i == hoverIdx);
}

void AdvanceRamps()
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

class TSidebarHud : public THudDrawable
{
public:
    void Draw() override
    {
        const SHudState& s = GetHudState();
        if (s.sidebarState != HUD_SIDEBAR_OPEN) return;
        DrawTopSlot(s);
        DrawBottomSlot(s);
        DrawTabStrip(s);
    }

    void Refresh()
    {
        DriveSyntheticState();
        AdvanceRamps();
        RebuildStripSurface();
    }

private:
    void DrawTopSlot(const SHudState& s)
    {
        PTBitmap chrome = CurrentTopChrome(s);
        if (!chrome) return;
        const int32_t dw = Display.Width();
        const int32_t x  = (dw > 0 ? dw : kPaneW + kPaneRightInset) - kPaneW - kPaneRightInset;
        const int32_t y  = 0;
        Renderer->DrawBitmap(chrome, x, y);
    }

    void DrawBottomSlot(const SHudState& s)
    {
        PTBitmap chrome = CurrentBottomChrome(s);
        if (!chrome) return;
        const int32_t dw = Display.Width();
        const int32_t x  = (dw > 0 ? dw : kPaneW + kPaneRightInset) - kPaneW - kPaneRightInset;
        const int32_t y  = kTopH;
        Renderer->DrawBitmap(chrome, x, y);
    }

    void DrawTabStrip(const SHudState& /*s*/)
    {
        if (!g_stripSurface) return;
        const int32_t dw = Display.Width();
        const int32_t x  = (dw > 0 ? dw : kStripW + kTabsRightInset) - kStripW - kTabsRightInset;
        const int32_t y  = kTabsTopInset;
        Renderer->DrawSurface(g_stripSurface, x, y);
    }

    void RebuildStripSurface()
    {
        if (!g_sidetabsUp) return;
        if (!g_stripSurface)
            g_stripSurface = new TSurface(kStripW, kStripH, SG_PIXELFORMAT_RGBA8);

        const int32_t tw = g_stripSurface->Width();
        const int32_t th = g_stripSurface->Height();
        g_stripSurface->StartPass(0.0f, 0.0f, 0.0f, 0.0f);

        // (a) base strip — Up art opaque
        Renderer->DrawBitmapSubrectToTarget(g_sidetabsUp, 0, 0, 0, 0,
                                            kStripW, kStripH, tw, th);

        // (b) selection — Down sub-rect at full alpha for each held-pressed button
        const SHudState& s = GetHudState();
        for (int32_t i = 0; i < kBtnCount; ++i)
        {
            if (!IsButtonSelected(i, s)) continue;
            Renderer->DrawBitmapSubrectToTarget(g_sidetabsDown,
                kBtnX, g_buttons[i].y, kBtnX, g_buttons[i].y,
                kBtnW, kBtnH, tw, th);
        }

        // (c) hover fade — Select tinted by ramp alpha
        for (int32_t i = 0; i < kBtnCount; ++i)
        {
            const int32_t a = RampAlpha(g_buttons[i].cur);
            if (a <= 0) continue;
            Renderer->DrawBitmapSubrectTintedToTarget(g_sidetabsSel,
                kBtnX, g_buttons[i].y, kBtnX, g_buttons[i].y,
                kBtnW, kBtnH, tw, th,
                1.0f, 1.0f, 1.0f, float(a) / 255.0f);
        }

        g_stripSurface->EndPass();
    }
};

TSidebarHud g_hud;

}  // namespace

bool InitializeUISidebarMode()
{
    log_info("[ui-sidebar] === sidebar integration (tabs + top + bottom) ===");

    // Reset HUD state to a known start so the cycling demo starts clean.
    SHudState& s = GetHudState();
    s.topSlot       = HUD_TOP_EQUIP;
    s.bottomSlot    = HUD_BOT_INV;
    s.sidebarState  = HUD_SIDEBAR_OPEN;
    s.bottomBarOpen = 1;
    s.textBarVisible = 1;
    s.statsBarVisible = 1;

    // Tab-strip art (sidebartabsnotex.dat — Classic-faithful per spec §2/§13)
    g_sidetabsDat  = TMulti::LoadMulti((char*)"sidebartabsnotex.dat");
    g_sidetabsUp   = LookupByName(g_sidetabsDat, "Up");
    g_sidetabsDown = LookupByName(g_sidetabsDat, "Down");
    g_sidetabsSel  = LookupByName(g_sidetabsDat, "Select");

    // Top-slot chrome — each pane archive's chrome-name from its SPEC.
    g_equipDat        = TMulti::LoadMulti((char*)"equippane.dat");
    g_equipChrome     = LookupByName(g_equipDat, "EquipTop");
    if (!g_equipChrome) g_equipChrome = LookupByName(g_equipDat, "Equip");

    g_spellscrollDat  = TMulti::LoadMulti((char*)"spellscroll.dat");
    g_spellbookChrome = LookupByName(g_spellscrollDat, "Scroll");
    if (!g_spellbookChrome && g_spellscrollDat && g_spellscrollDat->numoffsets > 0)
        g_spellbookChrome = g_spellscrollDat->Bitmap(0);

    g_statpaneDat     = TMulti::LoadMulti((char*)"statspane.dat");
    g_statsChrome     = LookupByName(g_statpaneDat, "Stats");
    if (!g_statsChrome && g_statpaneDat && g_statpaneDat->numoffsets > 0)
        g_statsChrome = g_statpaneDat->Bitmap(0);

    // Bottom-slot chrome
    g_inventoryDat    = TMulti::LoadMulti((char*)"inventory.dat");
    g_invChrome       = LookupByName(g_inventoryDat, "Inventory");

    g_automapDat      = TMulti::LoadMulti((char*)"automap.dat");
    g_mapChrome       = LookupByName(g_automapDat, "Amap");

    g_spellpaneDat    = TMulti::LoadMulti((char*)"spellpane.dat");
    g_spellcreateChrome = LookupByName(g_spellpaneDat, "spellconstr");

    log_info("[ui-sidebar] tabs strips Up=%s Down=%s Sel=%s",
             g_sidetabsUp ? "OK" : "MISS",
             g_sidetabsDown ? "OK" : "MISS",
             g_sidetabsSel ? "OK" : "MISS");
    log_info("[ui-sidebar] top chrome Equip=%s Book=%s Stats=%s",
             g_equipChrome ? "OK" : "MISS",
             g_spellbookChrome ? "OK" : "MISS",
             g_statsChrome ? "OK" : "MISS");
    log_info("[ui-sidebar] bot chrome Inv=%s Map=%s Spell=%s",
             g_invChrome ? "OK" : "MISS",
             g_mapChrome ? "OK" : "MISS",
             g_spellcreateChrome ? "OK" : "MISS");

    delete g_stripSurface;
    g_stripSurface = nullptr;
    g_lastTickMs   = 0.0;
    g_lastLoggedTop = g_lastLoggedBot = g_lastLoggedOpen = -1;

    Renderer->AddHud(&g_hud, 0.0f);
    return true;
}

void RenderUISidebarMode()
{
    g_hud.Refresh();
    Display.BackBuffer()->StartPass(0.30f, 0.32f, 0.36f, 1.0f);
    Display.BackBuffer()->EndPass();
}

void CloseUISidebarMode()
{
    Renderer->RemoveHud(&g_hud);
    delete g_stripSurface;
    g_stripSurface = nullptr;

    g_sidetabsDat = g_equipDat = g_spellscrollDat = g_statpaneDat = nullptr;
    g_inventoryDat = g_automapDat = g_spellpaneDat = nullptr;
    g_sidetabsUp = g_sidetabsDown = g_sidetabsSel = nullptr;
    g_equipChrome = g_spellbookChrome = g_statsChrome = nullptr;
    g_invChrome = g_mapChrome = g_spellcreateChrome = nullptr;
    g_lastTickMs = 0.0;
}

// Hit-test the inventory 4x3 grid. Returns column-major slot index
// 0..11 (col*3 + row + page*12) when (x,y) is inside a cell, -1 otherwise.
// Per InventoryPane_SPEC: origin pane-local (8, 42), pitch 45x44,
// interior 40x40, column-major.
static int32_t HitInvSlot(int32_t x, int32_t y, int32_t page)
{
    const int32_t dw     = Display.Width();
    const int32_t inv_x  = (dw > 0 ? dw : kPaneW + kPaneRightInset) - kPaneW - kPaneRightInset;
    const int32_t inv_y  = kTopH;
    constexpr int32_t kOriginX  = 8;
    constexpr int32_t kOriginY  = 42;
    constexpr int32_t kPitchX   = 45;
    constexpr int32_t kPitchY   = 44;
    constexpr int32_t kCellSize = 40;
    constexpr int32_t kCols     = 4;
    constexpr int32_t kRows     = 3;
    const int32_t local_x = x - inv_x - kOriginX;
    const int32_t local_y = y - inv_y - kOriginY;
    if (local_x < 0 || local_y < 0) return -1;
    const int32_t col = local_x / kPitchX;
    const int32_t row = local_y / kPitchY;
    if (col >= kCols || row >= kRows) return -1;
    // Reject the inter-cell gap (cell content is 40px inside the 45/44 pitch).
    const int32_t cell_lx = local_x - col * kPitchX;
    const int32_t cell_ly = local_y - row * kPitchY;
    if (cell_lx >= kCellSize || cell_ly >= kCellSize) return -1;
    return page * (kCols * kRows) + col * kRows + row;
}

void HandleMouseClickUISidebarMode(int32_t button, int32_t x, int32_t y)
{
    SHudState& s = GetHudState();

    // ---- MouseUp: commit / cancel an in-flight drag -------------------
    if (button == MB_LEFTUP)
    {
        if (!UIDragState::IsActive()) return;
        // Find the drop target. For this test mode we only support Inv
        // grid as a drop target; future passes wire BarInv + Equip too.
        if (s.sidebarState == HUD_SIDEBAR_OPEN && s.bottomSlot == HUD_BOT_INV)
        {
            const int32_t dest_slot = HitInvSlot(x, y, s.inventoryPage);
            if (dest_slot >= 0)
            {
                // Always accept in this test (allowed_type == 0). Real
                // Equip slots would gate on CanEquip here.
                UIDragState::CompleteDrag(EDragSource::Inventory, dest_slot, true);
                return;
            }
        }
        // Released somewhere uninteresting → cancel.
        UIDragState::Cancel();
        return;
    }

    if (button != MB_LEFTDOWN) return;

    // ---- Inventory grid: click-down on a cell starts a drag ----------
    // Click-on-bag would open the bag (per user: "Clicking on a bag in
    // the inventory replaces the pack icon with a miniature image of the
    // bag, and the inventory shows the contents of the bag."). The bag
    // detection requires real item refs which the test harness doesn't
    // have yet — for now any cell click starts a drag from that slot.
    if (s.sidebarState == HUD_SIDEBAR_OPEN && s.bottomSlot == HUD_BOT_INV)
    {
        const int32_t slot = HitInvSlot(x, y, s.inventoryPage);
        if (slot >= 0)
        {
            // Test harness has no real items — use the slot index as a
            // synthetic "item id" stand-in (cast to nullptr-equivalent
            // pointer with a non-null offset so logs distinguish slots).
            TObjectInstance* fake_item =
                reinterpret_cast<TObjectInstance*>(uintptr_t(slot + 1));
            UIDragState::BeginDrag(EDragSource::Inventory, slot,
                                   fake_item, x, y);
            return;
        }
    }

    // --- Inventory page arrows (only when bottom slot = Inv) ---------
    // Per docs/ui/forensics/InventoryPane_SPEC.md:392/393:
    //   scrollleft  button at pane-local (140, 11) 24x24
    //   scrollright button at pane-local (161, 12) 24x24
    // Pane is bottom-right at (display_w - 188, kTopH=306) → screen coords.
    {
        SHudState& s = GetHudState();
        if (s.sidebarState == HUD_SIDEBAR_OPEN && s.bottomSlot == HUD_BOT_INV)
        {
            const int32_t dw       = Display.Width();
            const int32_t inv_x    = (dw > 0 ? dw : kPaneW + kPaneRightInset) - kPaneW - kPaneRightInset;
            const int32_t inv_y    = kTopH;
            const int32_t leftBx   = inv_x + 140;
            const int32_t leftBy   = inv_y + 11;
            const int32_t rightBx  = inv_x + 161;
            const int32_t rightBy  = inv_y + 12;
            constexpr int32_t kArrW = 24;
            constexpr int32_t kArrH = 24;
            if (x >= leftBx && x < leftBx + kArrW && y >= leftBy && y < leftBy + kArrH)
            {
                if (s.inventoryPage > 0)
                {
                    s.inventoryPage--;
                    log_info("[ui-sidebar] inv L-arrow click -> page=%d", s.inventoryPage);
                }
                else
                {
                    log_info("[ui-sidebar] inv L-arrow click (at page 0, no-op)");
                }
                return;
            }
            if (x >= rightBx && x < rightBx + kArrW && y >= rightBy && y < rightBy + kArrH)
            {
                // Retail's hard upper bound is 0xf3 per spec UNCONFIRMED-D;
                // clamp here so the test harness can't run away.
                if (s.inventoryPage < 0xf3)
                {
                    s.inventoryPage++;
                    log_info("[ui-sidebar] inv R-arrow click -> page=%d", s.inventoryPage);
                }
                else
                {
                    log_info("[ui-sidebar] inv R-arrow click (at max page, no-op)");
                }
                return;
            }
        }
    }

    // Mirror the same right-anchored placement used in DrawTabStrip.
    const int32_t dw     = Display.Width();
    const int32_t strip_x = (dw > 0 ? dw : kStripW + kTabsRightInset) - kStripW - kTabsRightInset;
    const int32_t strip_y = kTabsTopInset;

    // Hit-test the 6 buttons. Each is at (kBtnX, kBtnY[i]) within the strip,
    // size kBtnW x kBtnH. Convert to screen coords and check (x, y).
    for (int32_t i = 0; i < kBtnCount; ++i)
    {
        const int32_t bx = strip_x + kBtnX;
        const int32_t by = strip_y + kBtnY[i];
        if (x < bx || x >= bx + kBtnW) continue;
        if (y < by || y >= by + kBtnH) continue;

        SHudState& s = GetHudState();
        // Per spec §10 modal mapping (matches g_buttons add-order):
        //   Upper buttons (region 0): i=0 Book(mode 2), i=1 Stats(mode 1), i=2 Equip(mode 0)
        //   Lower buttons (region 1): i=3 Spell(mode 2), i=4 Inv(mode 0),  i=5 Map(mode 1)
        const int32_t mode = g_buttons[i].mode;
        const char*   region;
        if (g_buttons[i].region == 0)
        {
            s.topSlot = mode;
            region = "top";
        }
        else
        {
            s.bottomSlot = mode;
            region = "bottom";
        }
        s.sidebarState = HUD_SIDEBAR_OPEN;

        // Auto-pairing (per FUN_0047cf40_TPlayScreen_DispatchCommand cases 7/9):
        //   case 7  ("Spell view"): upper=2 Book + lower=2 Spell  — paired
        //   case 9  ("Equip view"): upper=0 Equip + lower=0 Inv   — paired
        //   case 8  (Stats only):   upper=1 Stats, lower untouched
        //   case 11 (Inv only):     lower=0, upper untouched
        //   case 12 (Map only):     lower=1, upper untouched
        // So Stats / Map clicks are "solo"; Book / Equip / Spell / Inv pair.
        // User confirmed 2026-05-30: "spell list and spell construction
        // panels are tied together".
        bool paired = false;
        if (s.topSlot == HUD_TOP_BOOK && g_buttons[i].region == 0)
        {
            s.bottomSlot = HUD_BOT_SPELL;
            paired = true;
        }
        else if (s.topSlot == HUD_TOP_EQUIP && g_buttons[i].region == 0)
        {
            s.bottomSlot = HUD_BOT_INV;
            paired = true;
        }
        else if (s.bottomSlot == HUD_BOT_SPELL && g_buttons[i].region == 1)
        {
            s.topSlot = HUD_TOP_BOOK;
            paired = true;
        }
        else if (s.bottomSlot == HUD_BOT_INV && g_buttons[i].region == 1)
        {
            s.topSlot = HUD_TOP_EQUIP;
            paired = true;
        }

        log_info("[ui-sidebar] click btn %d (%s/mode %d)%s -> top=%d bottom=%d",
                 i, region, mode,
                 paired ? " [paired]" : "",
                 s.topSlot, s.bottomSlot);
        return;
    }
}
