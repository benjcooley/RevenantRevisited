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
#include "bitmapatlas.h"
#include "display.h"
#include "hudstate.h"
#include "logging.h"
#include "multi.h"
#include "renderer.h"
#include "revdefs.h"
#include "revenant.h"
#include "surface.h"
#include "testconfig.h"
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

// Tab-strip placement is anchored to the LOWER-RIGHT corner of the live
// PLAYFIELD VIEW, not the display:
//   playfield_right  = display_w - (sidebar_open    ? kPaneW : 0)
//   playfield_bottom = display_h - (bottombar_open  ? kBarH  : 0)
//   strip rect       = playfield - (kStripW + insets, kStripH + insets)
// So when the sidebar opens/closes (or bottombar toggles), the six-button
// strip moves with the corner, sitting a constant distance inside it.
// (UNCONFIRMED-A: spec leaves the inset literal as a BSS const; small
// gap by visual measurement until refined.)
constexpr int32_t kTabsRightInset  = 4;   // px gap from playfield right edge
constexpr int32_t kTabsBottomInset = 2;   // px gap from playfield bottom edge
constexpr int32_t kBottomBarH      = 60;  // matches BottomBarPane_SPEC §3
// kPaneW (188) is the sidebar chrome width — defined above as kPaneW.

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

bool g_syntheticStateEnabled = false;
bool g_contentChromeEnabled = true;

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

// Synthetic driver — cycles modal states + open/close so command-line UI
// harness captures can exercise the multiplex without hand input. Disabled
// in live PlayScreen; also suppressed for any run launched with an
// --input-script so scripted clicks fully own state for the whole process.
int32_t g_lastLoggedTop  = -1;
int32_t g_lastLoggedBot  = -1;
int32_t g_lastLoggedOpen = -1;
void DriveSyntheticState()
{
    if (!g_syntheticStateEnabled) return;

    // When an input-script was provided on the command line, the script owns
    // state for the entire run. It may drain before sapp_request_quit()
    // finishes, so gate on the startup arg in addition to live queue state.
    if (StartupInputScript[0] || TestModes::InputScriptActive()) return;

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

// Tab-strip screen origin — anchored to the playfield's lower-right corner.
// Playfield = display minus whatever chrome is currently open. See the
// constants block above for the formula. Used by both DrawTabStrip and the
// hit-test in HandleMouseClickUISidebarMode so the two stay in lockstep.
void TabStripOrigin(int32_t& x, int32_t& y)
{
    const SHudState& s = GetHudState();
    const int32_t dw = Display.Width();
    const int32_t dh = Display.Height();
    const int32_t playfieldRight  = (dw > 0 ? dw : kStripW)
                                  - (s.sidebarState == HUD_SIDEBAR_OPEN ? kPaneW : 0);
    const int32_t playfieldBottom = (dh > 0 ? dh : kStripH + kBottomBarH)
                                  - (s.bottomBarOpen ? kBottomBarH : 0);
    x = playfieldRight  - kStripW - kTabsRightInset;
    y = playfieldBottom - kStripH - kTabsBottomInset;
}

class TSidebarHud : public THudDrawable
{
public:
    void Draw() override
    {
        const SHudState& s = GetHudState();
        // Tab strip (#15): always visible — it is the controller that
        // opens/closes the sidebar (TSideTabsPane_SPEC §1). Per retail,
        // "the pane draws whenever the playscreen HUD is up"; there is no
        // per-pane visibility predicate inside the class itself.
        DrawTabStrip(s);
        if (!g_contentChromeEnabled) return;
        // Chrome panes (#15): only drawn when sidebar is OPEN.
        if (s.sidebarState != HUD_SIDEBAR_OPEN) return;
        DrawTopSlot(s);
        DrawBottomSlot(s);
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
        int32_t x = 0, y = 0;
        TabStripOrigin(x, y);
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

void SetUISidebarSyntheticStateEnabled(bool enabled)
{
    g_syntheticStateEnabled = enabled;
    for (SButton& b : g_buttons)
    {
        b.cur = 0;
        b.target = 0;
        b.hovered = false;
    }
    g_lastTickMs = 0.0;
    g_lastLoggedTop = g_lastLoggedBot = g_lastLoggedOpen = -1;
}

bool InitializeUISidebarMode()
{
    log_info("[ui-sidebar] === sidebar integration (tabs + top + bottom) ===");

    // Reset HUD state to a known retail start. In command-line harness modes
    // the optional synthetic driver will animate from here; live PlayScreen
    // leaves that driver disabled so input/save-state owns changes.
    SHudState& s = GetHudState();
    s.topSlot       = HUD_TOP_EQUIP;
    s.bottomSlot    = HUD_BOT_INV;
    s.sidebarState  = HUD_SIDEBAR_OPEN;
    s.bottomBarOpen = 1;
    s.textBarVisible = 1;
    s.statsBarVisible = 1;

    // Tab-strip art (sidebartabsnotex.dat — Classic-faithful per spec §2/§13)
    g_sidetabsDat  = TMulti::LoadMulti((char*)"sidebartabsnotex.dat");
    RegisterUIBitmapAtlasArchive(g_sidetabsDat);
    g_sidetabsUp   = LookupByName(g_sidetabsDat, "Up");
    g_sidetabsDown = LookupByName(g_sidetabsDat, "Down");
    g_sidetabsSel  = LookupByName(g_sidetabsDat, "Select");

    // Top-slot chrome — each pane archive's chrome-name from its SPEC.
    g_equipDat        = TMulti::LoadMulti((char*)"equippane.dat");
    RegisterUIBitmapAtlasArchive(g_equipDat);
    g_equipChrome     = LookupByName(g_equipDat, "EquipTop");
    if (!g_equipChrome) g_equipChrome = LookupByName(g_equipDat, "Equip");

    g_spellscrollDat  = TMulti::LoadMulti((char*)"spellscroll.dat");
    RegisterUIBitmapAtlasArchive(g_spellscrollDat);
    g_spellbookChrome = LookupByName(g_spellscrollDat, "Scroll");
    if (!g_spellbookChrome && g_spellscrollDat && g_spellscrollDat->numoffsets > 0)
        g_spellbookChrome = g_spellscrollDat->Bitmap(0);

    g_statpaneDat     = TMulti::LoadMulti((char*)"statspane.dat");
    RegisterUIBitmapAtlasArchive(g_statpaneDat);
    g_statsChrome     = LookupByName(g_statpaneDat, "Stats");
    if (!g_statsChrome && g_statpaneDat && g_statpaneDat->numoffsets > 0)
        g_statsChrome = g_statpaneDat->Bitmap(0);

    // Bottom-slot chrome
    g_inventoryDat    = TMulti::LoadMulti((char*)"inventory.dat");
    RegisterUIBitmapAtlasArchive(g_inventoryDat);
    g_invChrome       = LookupByName(g_inventoryDat, "Inventory");

    g_automapDat      = TMulti::LoadMulti((char*)"automap.dat");
    RegisterUIBitmapAtlasArchive(g_automapDat);
    g_mapChrome       = LookupByName(g_automapDat, "Amap");

    g_spellpaneDat    = TMulti::LoadMulti((char*)"spellpane.dat");
    RegisterUIBitmapAtlasArchive(g_spellpaneDat);
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
    g_contentChromeEnabled = true;

    Renderer->AddHud(&g_hud, 10.0f);
    return true;
}

void RenderUISidebarMode()
{
    RenderUISidebarModeEmbedded();
    Display.BackBuffer()->StartPass(0.30f, 0.32f, 0.36f, 1.0f);
    Display.BackBuffer()->EndPass();
}

void RenderUISidebarModeEmbedded()
{
    g_hud.Refresh();
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
    g_syntheticStateEnabled = false;
    g_contentChromeEnabled = true;
}

void SetUISidebarContentChromeEnabled(bool enabled)
{
    g_contentChromeEnabled = enabled;
}

void ToggleUISidebarPanel()
{
    SHudState& s = GetHudState();
    s.sidebarState = (s.sidebarState == HUD_SIDEBAR_OPEN)
                     ? HUD_SIDEBAR_CLOSED
                     : HUD_SIDEBAR_OPEN;
    log_info("[ui-sidebar] sidebar %s",
             s.sidebarState == HUD_SIDEBAR_OPEN ? "OPEN" : "CLOSED");
}

void ToggleUIBottomPanel()
{
    SHudState& s = GetHudState();
    s.bottomBarOpen = s.bottomBarOpen ? 0 : 1;
    log_info("[ui-sidebar] bottombar %s",
             s.bottomBarOpen ? "OPEN" : "CLOSED");
}

// Hit-test the inventory 4x3 grid. Returns column-major harness slot index
// (page + col*3 + row) when (x,y) is inside a cell, -1 otherwise.
// Per InventoryPane_SPEC: origin pane-local (8, 42), pitch 45x44,
// interior 40x40, column-major.
static bool HitInvSlotRect(int32_t x, int32_t y, int32_t page,
                           int32_t& slot, int32_t& sx, int32_t& sy)
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
    if (local_x < 0 || local_y < 0) return false;
    const int32_t col = local_x / kPitchX;
    const int32_t row = local_y / kPitchY;
    if (col >= kCols || row >= kRows) return false;
    const int32_t cell_lx = local_x - col * kPitchX;
    const int32_t cell_ly = local_y - row * kPitchY;
    if (cell_lx >= kCellSize || cell_ly >= kCellSize) return false;
    slot = page + col * kRows + row;
    sx = inv_x + kOriginX + col * kPitchX;
    sy = inv_y + kOriginY + row * kPitchY;
    return true;
}

static int32_t HitInvSlot(int32_t x, int32_t y, int32_t page)
{
    int32_t slot = -1, sx = 0, sy = 0;
    return HitInvSlotRect(x, y, page, slot, sx, sy) ? slot : -1;
}

// Hit-test the BarInv (bottom-bar) 9-slot quick shelf. Per BarInvPane_SPEC:
// 9 slots at pitch 45 from origin (220, 10) within the BottomBar's
// 640x60 strip (bottom-anchored). Returns slot 0..8 or -1.
static bool HitBarInvSlotRect(int32_t x, int32_t y,
                              int32_t& slot, int32_t& sx, int32_t& sy)
{
    const int32_t dh       = Display.Height();
    constexpr int32_t kBarH       = 60;
    constexpr int32_t kSlotOriginX = 220;
    constexpr int32_t kSlotOriginY = 10;
    constexpr int32_t kSlotPitchX  = 45;
    constexpr int32_t kSlotSize    = 40;
    constexpr int32_t kSlotCount   = 9;
    const int32_t bar_x = 0;
    const int32_t bar_y = dh - kBarH;
    const int32_t dw = Display.Width();
    const SHudState& hs = GetHudState();
    const int32_t bar_w = (dw > 0 ? dw : 640)
                        - (hs.sidebarState == HUD_SIDEBAR_OPEN ? kPaneW : 0);
    if (x >= bar_x + bar_w) return false;
    const int32_t local_x = x - bar_x - kSlotOriginX;
    const int32_t local_y = y - bar_y - kSlotOriginY;
    if (local_x < 0 || local_y < 0) return false;
    if (local_y >= kSlotSize) return false;
    const int32_t col = local_x / kSlotPitchX;
    if (col >= kSlotCount) return false;
    const int32_t cell_lx = local_x - col * kSlotPitchX;
    if (cell_lx >= kSlotSize) return false;
    slot = col;
    sx = bar_x + kSlotOriginX + col * kSlotPitchX;
    sy = bar_y + kSlotOriginY;
    if (sx + kSlotSize > bar_x + bar_w) return false;
    return true;
}

static int32_t HitBarInvSlot(int32_t x, int32_t y)
{
    int32_t slot = -1, sx = 0, sy = 0;
    return HitBarInvSlotRect(x, y, slot, sx, sy) ? slot : -1;
}

// Hit-test the Equip paperdoll 11-slot layout. Per EquipPane_SPEC
// DAT_005e3f60 table (mirrored in uiequiptest.cpp). Returns 0..10 in
// EQ_* enum order, -1 otherwise.
static bool HitEquipSlotRect(int32_t x, int32_t y,
                             int32_t& slot, int32_t& sx, int32_t& sy)
{
    struct SA { int32_t x; int32_t y; };
    static constexpr SA kAnchor[11] = {
        { 0x4a, 0x25 }, { 0x07, 0x3a }, { 0x8d, 0x3a }, { 0x8d, 0x6a },
        { 0x07, 0x6a }, { 0x07, 0x9a }, { 0x8d, 0x9a }, { 0x18, 0x0b },
        { 0x7c, 0x0b }, { 0x07, 0xca }, { 0x8d, 0xca },
    };
    constexpr int32_t kSlot = 40;
    const int32_t dw     = Display.Width();
    const int32_t pane_x = (dw > 0 ? dw : kPaneW + kPaneRightInset) - kPaneW - kPaneRightInset;
    const int32_t pane_y = 0;
    for (int32_t i = 0; i < 11; ++i)
    {
        const int32_t bx = pane_x + kAnchor[i].x;
        const int32_t by = pane_y + kAnchor[i].y;
        if (x >= bx && x < bx + kSlot && y >= by && y < by + kSlot)
        {
            slot = i;
            sx = bx;
            sy = by;
            return true;
        }
    }
    return false;
}

static int32_t HitEquipSlot(int32_t x, int32_t y)
{
    int32_t slot = -1, sx = 0, sy = 0;
    return HitEquipSlotRect(x, y, slot, sx, sy) ? slot : -1;
}

bool HandleMouseClickUISidebarModeConsumed(int32_t button, int32_t x, int32_t y)
{
    SHudState& s = GetHudState();

    // ---- MouseUp: commit / cancel an in-flight drag -------------------
    if (button == MB_LEFTUP)
    {
        if (!UIDragState::IsActive()) return false;
        const SUIDragState& drag = UIDragState::Get();
        if (drag.source != EDragSource::Inventory &&
            drag.source != EDragSource::BarInv &&
            drag.source != EDragSource::Equip)
        {
            return false;
        }
        if (UIDragState::IsPending())
        {
            UIDragState::CompleteClick();
            return true;
        }

        // Drop targets in priority order: Equip (most specific) → BarInv →
        // Inventory grid. First one to hit wins. Each could refuse on a
        // real-item type-filter; the test harness accepts all.
        if (s.sidebarState == HUD_SIDEBAR_OPEN && s.topSlot == HUD_TOP_EQUIP)
        {
            const int32_t es = HitEquipSlot(x, y);
            if (es >= 0)
            {
                UIDragState::CompleteDrag(EDragSource::Equip, es, true);
                return true;
            }
        }
        if (s.bottomBarOpen)
        {
            const int32_t bs = HitBarInvSlot(x, y);
            if (bs >= 0)
            {
                UIDragState::CompleteDrag(EDragSource::BarInv, bs, true);
                return true;
            }
        }
        if (s.sidebarState == HUD_SIDEBAR_OPEN && s.bottomSlot == HUD_BOT_INV)
        {
            const int32_t dest_slot = HitInvSlot(x, y, s.inventoryPage);
            if (dest_slot >= 0)
            {
                UIDragState::CompleteDrag(EDragSource::Inventory, dest_slot, true);
                return true;
            }
        }
        // Released somewhere uninteresting → cancel (the source's slot
        // keeps its item — no move committed).
        UIDragState::Cancel();
        return true;
    }

    // ---- Right-click: Use / Open-bag (retail eventType == 5) ---------
    if (button == MB_RIGHTDOWN)
    {
        if (s.sidebarState == HUD_SIDEBAR_OPEN && s.bottomSlot == HUD_BOT_INV)
        {
            const int32_t slot = HitInvSlot(x, y, s.inventoryPage);
            if (slot >= 0)
            {
                // For the test harness, any right-clicked slot is treated
                // as a bag-open toggle. Real impl would inspect the slot's
                // item type: bag → swap inventoryContainer; consumable →
                // Use(); equipment → SetInventorySlot to auto-equip.
                const int32_t was = s.inventoryContainer;
                s.inventoryContainer = (was == slot + 1) ? 0 : (slot + 1);
                log_info("[ui-sidebar] right-click inv slot=%d -> container=%d (was %d, %s)",
                         slot, s.inventoryContainer, was,
                         s.inventoryContainer == 0 ? "back to root" : "opened bag");
                return true;
            }
        }
        return false;
    }

    if (button != MB_LEFTDOWN) return false;

    // ---- Sidebar tab strip (TAKES PRECEDENCE over chrome-pane slots) -
    // The tab strip is its own controller at the playfield corner. It should
    // see clicks before content panes so tab changes never turn into item
    // grabs when the strip visually occupies the pointer point.
    {
        int32_t strip_x = 0, strip_y = 0;
        TabStripOrigin(strip_x, strip_y);
        for (int32_t i = 0; i < kBtnCount; ++i)
        {
            const int32_t bx = strip_x + kBtnX;
            const int32_t by = strip_y + kBtnY[i];
            if (x < bx || x >= bx + kBtnW) continue;
            if (y < by || y >= by + kBtnH) continue;

            const int32_t mode = g_buttons[i].mode;
            const char*   region;
            if (g_buttons[i].region == 0) { s.topSlot    = mode; region = "top"; }
            else                          { s.bottomSlot = mode; region = "bottom"; }
            s.sidebarState = HUD_SIDEBAR_OPEN;

            // Retail dispatch pairs only the upper-region convenience presets:
            // case 7/0xa = Book+Spell, case 9 = Equip+Inv. Lower-region
            // commands 0xb/0xc update only DAT_0065d1bc, so clicking Inv/Map
            // must not yank the upper pane away from Stats/Book/Equip.
            bool paired = false;
            if (s.topSlot == HUD_TOP_BOOK && g_buttons[i].region == 0)
            { s.bottomSlot = HUD_BOT_SPELL; paired = true; }
            else if (s.topSlot == HUD_TOP_EQUIP && g_buttons[i].region == 0)
            { s.bottomSlot = HUD_BOT_INV;   paired = true; }

            log_info("[ui-sidebar] click btn %d (%s/mode %d)%s -> top=%d bottom=%d",
                     i, region, mode, paired ? " [paired]" : "",
                     s.topSlot, s.bottomSlot);
            return true;
        }
    }

    // ---- Inventory grid: click-down on a cell starts a drag ----------
    if (s.sidebarState == HUD_SIDEBAR_OPEN && s.bottomSlot == HUD_BOT_INV)
    {
        int32_t slot = -1, sx = 0, sy = 0;
        if (HitInvSlotRect(x, y, s.inventoryPage, slot, sx, sy))
        {
            TObjectInstance* fake_item =
                reinterpret_cast<TObjectInstance*>(uintptr_t(slot + 1));
            UIDragState::BeginDrag(EDragSource::Inventory, slot,
                                   fake_item, x, y,
                                   nullptr, x - sx, y - sy);
            return true;
        }
    }

    // ---- BarInv shelf: click-down on a slot starts a drag ------------
    if (s.bottomBarOpen)
    {
        int32_t bs = -1, sx = 0, sy = 0;
        if (HitBarInvSlotRect(x, y, bs, sx, sy))
        {
            TObjectInstance* fake_item =
                reinterpret_cast<TObjectInstance*>(uintptr_t(0x100 + bs));
            UIDragState::BeginDrag(EDragSource::BarInv, bs,
                                   fake_item, x, y,
                                   nullptr, x - sx, y - sy);
            return true;
        }
    }

    // ---- Equip paperdoll: click-down on a slot starts a drag ---------
    if (s.sidebarState == HUD_SIDEBAR_OPEN && s.topSlot == HUD_TOP_EQUIP)
    {
        int32_t es = -1, sx = 0, sy = 0;
        if (HitEquipSlotRect(x, y, es, sx, sy))
        {
            TObjectInstance* fake_item =
                reinterpret_cast<TObjectInstance*>(uintptr_t(0x200 + es));
            UIDragState::BeginDrag(EDragSource::Equip, es,
                                   fake_item, x, y,
                                   nullptr, x - sx, y - sy);
            return true;
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
                return true;
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
                return true;
            }
        }
    }

    // (Tab strip already handled at the top of the LEFTDOWN section,
    // before chrome-pane slot hit-tests, so it takes precedence on
    // overlapping coords.)
    return false;
}

void HandleMouseClickUISidebarMode(int32_t button, int32_t x, int32_t y)
{
    (void)HandleMouseClickUISidebarModeConsumed(button, x, y);
}

bool HandleMouseMoveUISidebarModeConsumed(int32_t button, int32_t x, int32_t y)
{
    if (!(button & MB_LEFTDOWN)) return false;
    if (!UIDragState::IsActive()) return false;

    const SUIDragState& drag = UIDragState::Get();
    if (drag.source != EDragSource::Inventory &&
        drag.source != EDragSource::BarInv &&
        drag.source != EDragSource::Equip)
    {
        return false;
    }

    UIDragState::UpdateDrag(x, y);
    return true;
}

// =====================================================================
// Keyboard control for the HUD/sidebar — same dispatch as the mouse
// click handler, but driven by retail-identified command IDs (cases 7,
// 8, 9, 0xb, 0xc in TPlayScreen::DispatchCommand_47b4d0). Default key
// bindings follow the HUD.md mention "Number keys (1-4) for panel
// switching" + "V key: toggles the entire Side Panel"; B toggles the
// bottom bar (no retail key documented; chosen for memorability — the
// hide/show is dispatch cmd 4 in TPlayScreen).
//
//   V          → toggle SHudState.sidebarState (CLOSED ↔ OPEN)
//   B          → toggle SHudState.bottomBarOpen (0 ↔ 1)
//   1          → top=Book + bottom=Spell  (cmd 7)
//   2          → top=Stats                (cmd 8)
//   3          → top=Equip + bottom=Inv   (cmd 9)
//   4          → bottom=Inv               (cmd 0xb)
//   5          → bottom=Map               (cmd 0xc)
//   6          → bottom=Spell             (lower-only; no clean cmd ID
//                                          in DispatchCommand, treated
//                                          symmetric to 5/0xc)
//
// For modes that compose the full HUD (--test=ui-hud) AND for the
// sidebar-only mode (--test=ui-sidebar). The handler is a no-op on
// key-up; we react on key-down only (single trigger per press).
// =====================================================================
void HandleKeyPressUISidebarMode(int32_t key, bool down)
{
    if (!down) return;
    SHudState& s = GetHudState();
    const char* logTag = nullptr;
    switch (key)
    {
        case 'V': case 'v':
            ToggleUISidebarPanel();
            logTag = (s.sidebarState == HUD_SIDEBAR_OPEN) ? "V → sidebar OPEN"
                                                          : "V → sidebar CLOSED";
            break;
        case 'B': case 'b':
            ToggleUIBottomPanel();
            logTag = s.bottomBarOpen ? "B → bottombar OPEN"
                                     : "B → bottombar CLOSED";
            break;
        case '1':                                    // cmd 7: Book + Spell
            s.topSlot       = HUD_TOP_BOOK;
            s.bottomSlot    = HUD_BOT_SPELL;
            s.sidebarState  = HUD_SIDEBAR_OPEN;
            logTag = "1 → Book/Spell";
            break;
        case '2':                                    // cmd 8: Stats
            s.topSlot       = HUD_TOP_STATS;
            s.sidebarState  = HUD_SIDEBAR_OPEN;
            logTag = "2 → Stats";
            break;
        case '3':                                    // cmd 9: Equip + Inv
            s.topSlot       = HUD_TOP_EQUIP;
            s.bottomSlot    = HUD_BOT_INV;
            s.sidebarState  = HUD_SIDEBAR_OPEN;
            logTag = "3 → Equip/Inv";
            break;
        case '4':                                    // cmd 0xb: Inv only
            s.bottomSlot    = HUD_BOT_INV;
            s.sidebarState  = HUD_SIDEBAR_OPEN;
            logTag = "4 → Inv";
            break;
        case '5':                                    // cmd 0xc: Map
            s.bottomSlot    = HUD_BOT_MAP;
            s.sidebarState  = HUD_SIDEBAR_OPEN;
            logTag = "5 → Map";
            break;
        case '6':                                    // lower-only Spell
            s.bottomSlot    = HUD_BOT_SPELL;
            s.sidebarState  = HUD_SIDEBAR_OPEN;
            logTag = "6 → Spell";
            break;
        default:
            return;
    }
    log_info("[ui-sidebar] key: %s (state: top=%d bottom=%d sidebar=%d bottombar=%d)",
             logTag, s.topSlot, s.bottomSlot, s.sidebarState, s.bottomBarOpen);
}
