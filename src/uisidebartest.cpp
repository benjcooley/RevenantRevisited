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
//   - Side-tabs strip: 52x232 in the play field's lower-right corner, flush
//     against the side panel and the bottom drawer: (400, 188) with both
//     open (TabStripOrigin).
//
// *************************************************************************

#include "uisidebartest.h"

#include "bitmap.h"
#include "bitmapatlas.h"
#include "cursor.h"
#include "display.h"
#include "hudstate.h"
#include "inventory.h"
#include "logging.h"
#include "multi.h"
#include "player.h"
#include "playscreen.h"
#include "renderer.h"
#include "revdefs.h"
#include "revenant.h"
#include "statpane.h"
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
constexpr int32_t kBottomBarH = 60;   // matches BottomBarPane_SPEC §3

// Approximations matching uisidetabstest.cpp (UNCONFIRMED-A: BSS constants
// not yet extracted; refined in pixel-perfect alignment pass).
constexpr int32_t kPaneRightInset = 0;

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
    if (!StartupInputScript.empty() || TestModes::InputScriptActive()) return;

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

// REVSYNC: Pulse 0x0047bc1a..0x0047bc3d -- the strip sits in the play
// field's lower-right corner, flush: x = display_w - strip w - side panel
// w, y = play field bottom - strip h. The play field is the display less
// whatever chrome is open, so the strip moves with the corner. Used by both
// DrawTabStrip and UISideTabButtonAt, so the two stay in lockstep.
void TabStripOrigin(int32_t& x, int32_t& y)
{
    const SHudState& s = GetHudState();
    const int32_t dw = Display.Width();
    const int32_t dh = Display.Height();
    const int32_t playfieldRight  = (dw > 0 ? dw : kStripW)
                                  - (s.sidebarState == HUD_SIDEBAR_OPEN ? kPaneW : 0);
    // The bottom drawer (the bottom bar, or the shop) sits under the
    // playfield; PlayScreen owns it (retail lays the side tabs out from its
    // drawer code, 0x0047b4d0).
    const int32_t playfieldBottom = (dh > 0 ? dh : kStripH + kBottomBarH)
                                  - PlayScreen.DrawerHeight();
    x = playfieldRight  - kStripW;
    y = playfieldBottom - kStripH;
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

// REVSYNC: 0x004364d0 (TButtonPane, here on the side tabs 0x0065be50): the
// point must be in the pane, then in a button's rect (TButton 0x0042d0d0).
// The tabs' button flags (0x00180010, 0x0053cc30) carry no pixel test
// (0x20000), so a round button's corners count and the gaps between buttons
// don't; a selected (pressed) tab holds its point like the others.
int32_t UISideTabButtonAt(int32_t x, int32_t y)
{
    int32_t strip_x = 0, strip_y = 0;
    TabStripOrigin(strip_x, strip_y);
    const int32_t px = x - strip_x;
    const int32_t py = y - strip_y;
    if (px < 0 || py < 0 || px >= kStripW || py >= kStripH)
        return -1;
    for (int32_t i = 0; i < kBtnCount; ++i)
        if (px >= kBtnX && px < kBtnX + kBtnW && py >= kBtnY[i] && py < kBtnY[i] + kBtnH)
            return i;
    return -1;
}

// The inventory pane's screen origin: the side panel's lower slot.
static void InvPaneOrigin(int32_t& x, int32_t& y)
{
    const int32_t dw = Display.Width();
    x = (dw > 0 ? dw : kPaneW + kPaneRightInset) - kPaneW - kPaneRightInset;
    y = kTopH;
}

// Whether the side panel shows the inventory pane and (x, y) is on it.
static bool InInvPane(int32_t x, int32_t y)
{
    const SHudState& s = GetHudState();
    if (s.sidebarState != HUD_SIDEBAR_OPEN || s.bottomSlot != HUD_BOT_INV)
        return false;
    int32_t pane_x = 0, pane_y = 0;
    InvPaneOrigin(pane_x, pane_y);
    return x >= pane_x && x < pane_x + kPaneW && y >= pane_y && y < pane_y + kBotH;
}

// REVSYNC: 0x005385f2..0x00538610 -- the inventory's container icon, the
// backpack or the bag shown at (10, 10): 10..30 across, 10..29 down.
static bool OnInvContainerIcon(int32_t x, int32_t y)
{
    if (!InInvPane(x, y))
        return false;
    int32_t pane_x = 0, pane_y = 0;
    InvPaneOrigin(pane_x, pane_y);
    const int32_t lx = x - pane_x;
    const int32_t ly = y - pane_y;
    return lx >= 10 && lx <= 30 && ly >= 10 && ly < 30;
}

// Hit-test the inventory 4x3 grid. Returns the slot number of the shown
// container (page + col*3 + row) when (x,y) is inside a cell, -1 otherwise.
// Per InventoryPane_SPEC: origin pane-local (8, 42), pitch 45x44,
// interior 40x40, column-major.
static bool HitInvSlotRect(int32_t x, int32_t y, int32_t page,
                           int32_t& slot, int32_t& sx, int32_t& sy)
{
    int32_t inv_x = 0, inv_y = 0;
    InvPaneOrigin(inv_x, inv_y);
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

// The Equip paperdoll's 11 cells, pane-local top-left corners in EQ_* enum
// order: the {x, y} table at DAT_005e3f60 that retail's MouseClick tests
// each 40x40 cell against (0x005363fc..0x00536437; EquipPane_SPEC §4,
// mirrored in uiequiptest.cpp).
struct SEquipAnchor { int32_t x; int32_t y; };
static constexpr SEquipAnchor kEquipAnchor[NUM_EQ_SLOTS] = {
    { 0x4a, 0x25 }, { 0x07, 0x3a }, { 0x8d, 0x3a }, { 0x8d, 0x6a },
    { 0x07, 0x6a }, { 0x07, 0x9a }, { 0x8d, 0x9a }, { 0x18, 0x0b },
    { 0x7c, 0x0b }, { 0x07, 0xca }, { 0x8d, 0xca },
};
static constexpr int32_t kEquipCell = 40;

// The equip pane's screen origin: the side panel's upper slot.
static void EquipPaneOrigin(int32_t& x, int32_t& y)
{
    const int32_t dw = Display.Width();
    x = (dw > 0 ? dw : kPaneW + kPaneRightInset) - kPaneW - kPaneRightInset;
    y = 0;
}

// Hit-test the Equip paperdoll 11-slot layout. Returns 0..10 in EQ_* enum
// order, -1 otherwise.
static bool HitEquipSlotRect(int32_t x, int32_t y,
                             int32_t& slot, int32_t& sx, int32_t& sy)
{
    int32_t pane_x = 0, pane_y = 0;
    EquipPaneOrigin(pane_x, pane_y);
    for (int32_t i = 0; i < NUM_EQ_SLOTS; ++i)
    {
        const int32_t bx = pane_x + kEquipAnchor[i].x;
        const int32_t by = pane_y + kEquipAnchor[i].y;
        if (x >= bx && x < bx + kEquipCell && y >= by && y < by + kEquipCell)
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

bool UIEquipSlotCenter(int32_t slot, int32_t& x, int32_t& y)
{
    const SHudState& s = GetHudState();
    if (s.sidebarState != HUD_SIDEBAR_OPEN || s.topSlot != HUD_TOP_EQUIP || slot < 0 || slot >= NUM_EQ_SLOTS)
        return false;
    EquipPaneOrigin(x, y);
    x += kEquipAnchor[slot].x + kEquipCell / 2;
    y += kEquipAnchor[slot].y + kEquipCell / 2;
    return true;
}

// The equip pane's whole rect (top slot, 188x306), the area retail's
// button-up bounds test covers (0x005363e0: 0 <= x < w, 0 <= y < h).
static bool InEquipPane(int32_t x, int32_t y)
{
    int32_t pane_x = 0, pane_y = 0;
    EquipPaneOrigin(pane_x, pane_y);
    return x >= pane_x && x < pane_x + kPaneW && y >= pane_y && y < pane_y + kTopH;
}

// REVSYNC: the right button up with Ctrl held, on the inventory
// (0x00538267..0x00538333) and the belt (0x0052cff9..0x0052d0b5), before
// anything else they do: the item under the pointer shows on the stats pane
// (ShowStatPane 0x0047eff0, StatPane.SetObject 0x005496a0); with none, the
// player does again when the stats pane is up already. Never an equip or a
// compaction. Off the belt's boxes, nothing.
static bool ShowItemInfoAt(int32_t x, int32_t y)
{
    const SHudState& s = GetHudState();
    TObjectInstance* item = nullptr;
    if (InInvPane(x, y))
    {
        TObjectInstance* const shown = Inventory.GetContainer();
        if (!shown)
            return false;
        const int32_t slot = HitInvSlot(x, y, Inventory.FirstSlot());
        item = slot >= 0 ? shown->GetInventorySlot(slot) : nullptr;
    }
    else if (const int32_t bs = s.bottomBarOpen && Player ? HitBarInvSlot(x, y) : -1; bs >= 0)
        item = Player->GetInventorySlot(kInvSlotBeltFirst + bs);
    else
        return false;

    if (item)
    {
        log_info("[ui-sidebar] item info: %s", item->GetName());
        PlayScreen.ShowStatPane();
        StatPane.SetObject(item);
    }
    else if (s.topSlot == HUD_TOP_STATS)
        StatPane.SetObject(Player);
    return true;
}

bool HandleMouseClickUISidebarModeConsumed(int32_t button, int32_t x, int32_t y)
{
    SHudState& s = GetHudState();

    // ---- Left button up: where an item grabbed in a pane lands --------
    // Retail sends a release to every pane, and each item pane acts on the
    // part that is over its own cells (UIDragState::Release). The equip pane
    // takes a drop anywhere on it (0x005363e0): the item goes to its own
    // slot, not the cell under the cursor.
    if (button == MB_LEFTUP)
    {
        const EDragSource held = UIDragState::Get().source;
        const bool itemGrab = held == EDragSource::Inventory || held == EDragSource::BarInv ||
                              held == EDragSource::Equip;
        if (held != EDragSource::None && !itemGrab)
            return false;               // a spell: the book and the rings take it
        if (OnInvContainerIcon(x, y))
        {
            UIDragState::ReleaseOnContainerIcon();
            return true;
        }
        if (!itemGrab)
            return false;

        EDragSource dest = EDragSource::None;
        int32_t cell = -1;
        if (s.sidebarState == HUD_SIDEBAR_OPEN && s.topSlot == HUD_TOP_EQUIP && InEquipPane(x, y))
        {
            dest = EDragSource::Equip;
            cell = HitEquipSlot(x, y);
        }
        else if (s.bottomBarOpen && (cell = HitBarInvSlot(x, y)) >= 0)
            dest = EDragSource::BarInv;
        else if (InInvPane(x, y) && (cell = HitInvSlot(x, y, Inventory.FirstSlot())) >= 0)
            dest = EDragSource::Inventory;
        UIDragState::Release(dest, cell);
        return true;
    }

    // ---- Right button up on an item cell, nothing held -----------------
    // The inventory (0x00538210) and the belt (0x0052cfb0) with button 5:
    // UIDragState::RightClick. On the inventory pane off its cells the shown
    // container closes up, as on an empty cell; the other panes see that
    // release too. Deviation: retail's belt read equipment slot 10 (0x10a)
    // off its boxes, so it closed up the inventory, or re-equipped what is
    // worn there with its sound; the port leaves a release off the boxes.
    if (button == MB_RIGHTUP)
    {
        if (CtrlDown)
            return ShowItemInfoAt(x, y);
        if (UIDragState::IsActive())
            return false;
        if (InInvPane(x, y))
        {
            TObjectInstance* const shown = Inventory.GetContainer();
            const int32_t slot = HitInvSlot(x, y, Inventory.FirstSlot());
            UIDragState::RightClick(shown, shown && slot >= 0 ? shown->GetInventorySlot(slot) : nullptr);
            return slot >= 0;
        }
        if (s.bottomBarOpen && Player)
        {
            if (const int32_t bs = HitBarInvSlot(x, y); bs >= 0)
            {
                UIDragState::RightClick(Player, Player->GetInventorySlot(kInvSlotBeltFirst + bs));
                return true;
            }
        }
        return false;
    }

    // ---- Right press on the equip pane: a worn item's sheet ------------
    // REVSYNC: TEquipPane::MouseClick 0x005363f7..0x00536475 -- Ctrl or not,
    // the item worn in the slot under the pointer shows on the stats pane.
    if (button == MB_RIGHTDOWN)
    {
        if (s.sidebarState != HUD_SIDEBAR_OPEN || s.topSlot != HUD_TOP_EQUIP || !Player)
            return false;
        const int32_t es = HitEquipSlot(x, y);
        if (es < 0)
            return false;
        if (TObjectInstance* const worn = Player->GetEquip(es))
        {
            log_info("[ui-sidebar] item info: %s", worn->GetName());
            PlayScreen.ShowStatPane();
            StatPane.SetObject(worn);
        }
        return true;
    }

    if (button != MB_LEFTDOWN) return false;

    // ---- Sidebar tab strip (TAKES PRECEDENCE over chrome-pane slots) -
    // The tab strip is its own controller at the playfield corner. It should
    // see clicks before content panes so tab changes never turn into item
    // grabs when the strip visually occupies the pointer point.
    if (const int32_t i = UISideTabButtonAt(x, y); i >= 0)
    {
        const int32_t mode = g_buttons[i].mode;
        const char*   region;
        if (g_buttons[i].region == 0) { s.topSlot    = mode; region = "top"; }
        else                          { s.bottomSlot = mode; region = "bottom"; }
        s.sidebarState = HUD_SIDEBAR_OPEN;

        // REVSYNC: TSideTabsPane::OnControl 0x0053d790, a tab turned on
        // (0xbb9): Book and Spell show the book over the spell maker, Stats
        // and Equip their pane over the inventory, Inv and Map only theirs.
        bool paired = true;
        if (mode == HUD_TOP_BOOK && g_buttons[i].region == 0)
            s.bottomSlot = HUD_BOT_SPELL;
        else if (g_buttons[i].region == 0)
            s.bottomSlot = HUD_BOT_INV;
        else if (mode == HUD_BOT_SPELL)
            s.topSlot = HUD_TOP_BOOK;
        else
            paired = false;

        log_info("[ui-sidebar] click btn %d (%s/mode %d)%s -> top=%d bottom=%d",
                 i, region, mode, paired ? " [paired]" : "",
                 s.topSlot, s.bottomSlot);
        return true;
    }

    // ---- Item slots: click-down on an item starts a grab. The inventory's
    // items are the shown container's; the belt's (slot 0x10b + n) and the
    // doll's, the main player's.
    // ---- Inventory grid ------------------------------------------------
    if (s.sidebarState == HUD_SIDEBAR_OPEN && s.bottomSlot == HUD_BOT_INV)
    {
        int32_t slot = -1, sx = 0, sy = 0;
        if (HitInvSlotRect(x, y, Inventory.FirstSlot(), slot, sx, sy))
        {
            TObjectInstance* const shown = Inventory.GetContainer();
            TObjectInstance* item = shown ? shown->GetInventorySlot(slot) : nullptr;
            UIDragState::BeginDrag(EDragSource::Inventory, slot,
                                   item, x, y,
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
            TObjectInstance* item =
                Player ? Player->GetInventorySlot(kInvSlotBeltFirst + bs) : nullptr;
            UIDragState::BeginDrag(EDragSource::BarInv, bs,
                                   item, x, y,
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
            TObjectInstance* item = Player ? Player->GetEquip(es) : nullptr;
            UIDragState::BeginDrag(EDragSource::Equip, es,
                                   item, x, y,
                                   nullptr, x - sx, y - sy);
            return true;
        }
    }

    // (The inventory's page arrows are its own buttons: TInventory.)

    // (Tab strip already handled at the top of the LEFTDOWN section,
    // before chrome-pane slot hit-tests, so it takes precedence on
    // overlapping coords.)
    return false;
}

void HandleMouseClickUISidebarMode(int32_t button, int32_t x, int32_t y)
{
    (void)HandleMouseClickUISidebarModeConsumed(button, x, y);
}

// REVSYNC: the item panes' Animate -- the inventory 0x00537f90, the belt
// 0x0052cd80, the doll 0x00536970: with no button held, the item under the
// pointer picks the cursor's corner picture (CursorOverObject 0x0043a820),
// the mouth over food, the eye over a scroll, the hand over a bag.
void AnimateUISidebarItemCursor(int32_t x, int32_t y)
{
    if (mousebutton || !Player)
        return;
    const SHudState& s = GetHudState();
    TObjectInstance* item = nullptr;
    if (InInvPane(x, y))
    {
        TObjectInstance* const shown = Inventory.GetContainer();
        const int32_t slot = HitInvSlot(x, y, Inventory.FirstSlot());
        if (shown && slot >= 0)
            item = shown->GetInventorySlot(slot);
    }
    else if (const int32_t bs = s.bottomBarOpen ? HitBarInvSlot(x, y) : -1; bs >= 0)
        item = Player->GetInventorySlot(kInvSlotBeltFirst + bs);
    else if (s.sidebarState == HUD_SIDEBAR_OPEN && s.topSlot == HUD_TOP_EQUIP)
    {
        if (const int32_t es = HitEquipSlot(x, y); es >= 0)
            item = Player->GetEquip(es);
    }
    CursorOverObject(item);
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
