// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uimainmenutest.cpp - --test=ui-mainmenu                              *
// *************************************************************************
//
// Clean-room reconstruction of TLogoScreen (the title / main-menu screen,
// retail class cls_0x5a5d18). Built ONLY from docs/ui/forensics/MainMenu_SPEC.md
// (with NOMENCLATURE / UI_METHOD_MAP). Every coordinate / asset / color cites
// its spec section inline.
//
// Anatomy (spec §1 / §2 / §4):
//   - "MainMenu" 640x480 backdrop, opaque, at (0,0)             (§4 row 1)
//   - five word buttons, each placed at its U-sprite (-regx,-regy)  (§4)
//       New Game / Load Game / Multiplayer / Options / Exit
//       state per button: U (default) / S (hover) / D (mouse-down)  (§6.6)
//   - "Revenant   v1.22" version text, bottom-left cell (15,450,150,40),
//       LEFT + BOTTOM aligned, intended blue-violet, NO shadow      (§8)
//
// Faithfulness fixes the spec calls out (§11):
//   - Version color: retail mis-routes the color word (param_9 instead of
//     param_7) and renders BLACK; we render the INTENDED (R=175,G=72,B=255).
//   - Version shadow: SpellTitle font index has no 0x400 bit -> NO 3-pass
//     shadow (§7 / §14 item 3). We draw a single (no-shadow) text pass.
//
// Architecture (spec §3 / §5 direct-renderer contract): compose backdrop +
// buttons + version text into ONE 640x480 offscreen TSurface render target via
// the `…ToTarget` family, then DrawSurface it once in the HUD pass -- the same
// contract every other ui-* test mode follows (uiplyrstatusbartest.cpp).
//
// *************************************************************************

#include "uimainmenutest.h"

#include "bitmap.h"
#include "bitmapdata.h"
#include "cursor.h"
#include "display.h"
#include "font.h"
#include "fonttable.h"
#include "logging.h"
#include "multi.h"
#include "renderer.h"
#include "surface.h"

#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace {

// --- native asset frame (spec §3 — baked Classic 640x480) ----------------
constexpr int32_t kScreenW = 640;
constexpr int32_t kScreenH = 480;

// --- version text (spec §8) ----------------------------------------------
// Cell (15,450,150,40), LEFT + BOTTOM, NO shadow. Format "Revenant   v%d.%02d"
// (three spaces). Shipping retail version bytes = 1, 22 -> "Revenant   v1.22".
constexpr int32_t     kVerCellX = 15;     // §8 (0x0f)
constexpr int32_t     kVerCellW = 150;    // §8 (0x96)
constexpr int32_t     kVerMajor = 1;      // §6.3 byte [0x5d79dc]
constexpr int32_t     kVerMinor = 22;     // §6.3 byte [0x5d79dd]
constexpr const char* kVersionFmt = "Revenant   v%d.%02d";

// Intended version-text color (spec §8 / §11 item 5): R=175, G=72, B=255.
constexpr float kVerR = 175.0f / 255.0f;
constexpr float kVerG =  72.0f / 255.0f;
constexpr float kVerB = 255.0f / 255.0f;

// TTF fallback if the retail SpellTitle face isn't in the font table.
constexpr const char* kFallbackFontPath = "thirdparty/fonts/Arimo-Regular.ttf";
constexpr int32_t     kFallbackFontPx   = 14;

// =====================================================================
// The five menu buttons, in retail Initialize order (spec §4 / §12).
// =====================================================================
enum { BTN_NEWGAME = 0, BTN_LOADGAME, BTN_MULTI, BTN_OPTIONS, BTN_EXIT, BTN_COUNT };

const char* const kButtonBaseNames[BTN_COUNT] = {
    "MenuNewGame", "MenuLoadGame", "MenuMulti", "MenuOptions", "MenuExit",
};

struct SButton
{
    PTBitmap up   = nullptr;     // <base>U — default
    PTBitmap down = nullptr;     // <base>D — mouse-down
    PTBitmap sel  = nullptr;     // <base>S — hover (purple+gold)
    int32_t  x = 0, y = 0;       // screen TL = (-regx, -regy) of the U sprite (§4)
    int32_t  w = 0, h = 0;       // U sprite size (§4)
};

// =====================================================================
// Loaded assets (spec §2 — one archive, menus.dat).
// =====================================================================
TMulti*  g_menusDat   = nullptr;
PTBitmap g_mainMenuBg = nullptr;     // "MainMenu" 640x480
SButton  g_buttons[BTN_COUNT];

const SFontAtlas* g_verFont        = nullptr;
bool              g_verFontColored = false;   // true if a baked-color bitmap font

TSurface* g_pane = nullptr;          // 640x480 compose RT

// Interaction state (display-space hit testing -> RT-local).
int32_t g_hoverBtn = -1;     // index of hovered button, or -1
int32_t g_downBtn  = -1;     // index of pressed button, or -1

// Map a display-space point to the RT-local frame (undo the centering offset).
void DisplayToPane(int32_t dx, int32_t dy, int32_t& px, int32_t& py)
{
    const int32_t ox = (Display.Width()  - kScreenW) / 2;
    const int32_t oy = (Display.Height() - kScreenH) / 2;
    px = dx - (ox > 0 ? ox : 0);
    py = dy - (oy > 0 ? oy : 0);
}

int32_t HitTest(int32_t px, int32_t py)
{
    for (int32_t i = 0; i < BTN_COUNT; ++i)
    {
        const SButton& b = g_buttons[i];
        if (!b.up) continue;
        if (px >= b.x && px < b.x + b.w && py >= b.y && py < b.y + b.h)
            return i;
    }
    return -1;
}

// =====================================================================
// HUD drawable — composes the whole screen RT then DrawSurface's it once
// (spec §3 / §5 direct-renderer contract), centered/letterboxed.
// =====================================================================
class TMainMenuHud : public THudDrawable
{
public:
    void Draw() override
    {
        if (!g_pane) return;
        const int32_t x = (Display.Width()  - g_pane->Width())  / 2;
        const int32_t y = (Display.Height() - g_pane->Height()) / 2;
        Renderer->DrawSurface(g_pane, x > 0 ? x : 0, y > 0 ? y : 0);
    }

    void Refresh()
    {
        if (!g_mainMenuBg) return;
        EnsurePane();
        if (!g_pane) return;

        const int32_t tw = g_pane->Width();
        const int32_t th = g_pane->Height();

        g_pane->StartPass(0.0f, 0.0f, 0.0f, 1.0f);

        // --- (1) backdrop, opaque full-frame at (0,0) (spec §4 / §6.2) ---
        Renderer->DrawBitmapToTarget(g_mainMenuBg, 0, 0, tw, th);

        // --- (2) five buttons, each its current-state sprite at (-regx,-regy)
        // (spec §4 / §6.6). down (mouse-down) > sel (hover) > up (default). ---
        for (int32_t i = 0; i < BTN_COUNT; ++i)
        {
            const SButton& b = g_buttons[i];
            PTBitmap sprite = b.up;
            if (i == g_downBtn && b.down)      sprite = b.down;   // §6.6 D
            else if (i == g_hoverBtn && b.sel) sprite = b.sel;    // §6.6 S
            if (sprite)
                Renderer->DrawBitmapToTarget(sprite, b.x, b.y, tw, th);
        }

        // --- (3) version text, bottom-left, no shadow (spec §8) ---
        DrawVersionText(tw, th);

        g_pane->EndPass();
    }

private:
    void EnsurePane()
    {
        const int32_t w = g_mainMenuBg ? g_mainMenuBg->width  : kScreenW;
        const int32_t h = g_mainMenuBg ? g_mainMenuBg->height : kScreenH;
        if (g_pane && g_pane->Width() == w && g_pane->Height() == h) return;
        delete g_pane;
        g_pane = new TSurface(w, h, SG_PIXELFORMAT_RGBA8);
    }

    static void DrawVersionText(int32_t tw, int32_t th)
    {
        if (!g_verFont) return;

        char buf[64];
        std::snprintf(buf, sizeof(buf), kVersionFmt, kVerMajor, kVerMinor);

        // §8: LEFT + BOTTOM. The shared helper top-aligns the baseline at
        // cellY+ascent, so to bottom-align we compute the baseline directly.
        // Cell bottom = 450+40 = 490 (below the 480 frame -- spec §8 notes the
        // cell extends past the bottom); clamp the baseline a couple px above
        // the frame edge so the (color-bug-fixed) text is fully visible.
        const float ascent  = TextAscent(g_verFont);
        const float lh      = TextLineHeight(g_verFont);
        const float descent = lh - ascent;
        float baselineY = float(450 + 40) - descent;            // §8 cell bottom
        const float maxBaseline = float(th) - descent - 2.0f;
        if (baselineY > maxBaseline) baselineY = maxBaseline;   // keep on-frame
        const int32_t cellY = (int32_t)(baselineY - ascent + 0.5f);

        // Baked-color bitmap fonts already carry their hue -> draw white-tint;
        // a neutral (white-coverage) TTF is tinted to the intended blue-violet.
        const float r = g_verFontColored ? 1.0f : kVerR;
        const float g = g_verFontColored ? 1.0f : kVerG;
        const float bcol = g_verFontColored ? 1.0f : kVerB;

        DrawTextToTarget(g_verFont, buf, kVerCellX, cellY, kVerCellW,
                         (int32_t)(lh + 0.5f), ETextAlign::Left,
                         r, g, bcol, tw, th);
    }
};

TMainMenuHud g_hud;

// The retail title screen owns a cursor (FUN_0043a020("cursor"), §0/§2). We
// install the game cursor in-frame (hardware cursor disabled) so it draws over
// the menu and the scripted mouse simulator can drive it. z=10 > pane z=0.
TCursorHud g_cursorHud;

// Resolve the retail SpellTitle face (spec §8) from the font table; fall back
// through a couple of likely bitmap faces, then a neutral TTF.
void LoadVersionFont()
{
    g_verFont        = nullptr;
    g_verFontColored = false;

    // Retail renders the version string with the "SpellTitle" font via GDI
    // DrawTextA (§6.5) — a small TrueType winface, NOT the ornate all-caps
    // bitmap logo font that FontTable->Bitmap("SpellTitle") returns (that one
    // has no lowercase/digit glyphs, so "Revenant   v1.22" comes out blank).
    // Use the canonical small-UI TTF (Arimo, Arial-metric — the port's "Small"
    // face, see [[project-ui-text-rendering]]) so the string actually renders.
    g_verFont = BuildTTFAtlas(kFallbackFontPath, kFallbackFontPx);
    g_verFontColored = false;   // neutral white-coverage atlas -> tint to color
    log_info("[ui-mainmenu] version font: %s @%dpx = %s",
             kFallbackFontPath, kFallbackFontPx, g_verFont ? "OK" : "MISS");
}

}  // namespace

// =====================================================================
// Entry points (testmodes.cpp links these — keep the signatures).
// =====================================================================
bool InitializeUIMainMenuMode()
{
    log_info("[ui-mainmenu] === TLogoScreen (clean-room from spec) ===");

    g_menusDat = TMulti::LoadMulti((char*)"menus.dat");
    if (g_menusDat)
        g_mainMenuBg = g_menusDat->Bitmap((char*)"MainMenu");

    // Each button: <base>U/<base>D/<base>S; position from the U-sprite REG.
    char nm[64];
    for (int32_t i = 0; i < BTN_COUNT; ++i)
    {
        SButton& b = g_buttons[i];
        if (!g_menusDat) continue;
        std::snprintf(nm, sizeof(nm), "%sU", kButtonBaseNames[i]); b.up   = g_menusDat->Bitmap(nm);
        std::snprintf(nm, sizeof(nm), "%sD", kButtonBaseNames[i]); b.down = g_menusDat->Bitmap(nm);
        std::snprintf(nm, sizeof(nm), "%sS", kButtonBaseNames[i]); b.sel  = g_menusDat->Bitmap(nm);
        if (b.up)
        {
            b.x = std::abs(b.up->regx);   // dst TL = (-regx,-regy) (§4)
            b.y = std::abs(b.up->regy);
            b.w = b.up->width;
            b.h = b.up->height;
        }
        log_info("[ui-mainmenu] %-13s U=%s D=%s S=%s @ (%d,%d) %dx%d",
                 kButtonBaseNames[i], b.up ? "OK" : "MISS", b.down ? "OK" : "MISS",
                 b.sel ? "OK" : "MISS", b.x, b.y, b.w, b.h);
    }

    log_info("[ui-mainmenu] backdrop MainMenu=%s", g_mainMenuBg ? "OK" : "MISS");

    LoadVersionFont();

    delete g_pane;
    g_pane     = nullptr;
    g_hoverBtn = -1;
    g_downBtn  = -1;

    // Install the in-frame game cursor (hardware cursor disabled). GameData
    // (playscrn.dat) holds "cursor"; load it if a standalone test left it null.
    // cursorx/cursory are driven by real mouse moves and by --mouse-script.
    if (!GameData) GameData = TMulti::LoadMulti((char*)"playscrn.dat");
    SetHardwareCursorEnabled(false);
    PTBitmap cur = GameData ? GameData->Bitmap((char*)"cursor") : nullptr;
    if (cur) SetMouseBitmap(cur);
    log_info("[ui-mainmenu] cursor: GameData=%s bitmap=%s hwcursor=%d",
             GameData ? "OK" : "NULL", cur ? "OK" : "MISS", HardwareCursorEnabled() ? 1 : 0);
    cursorx = Display.Width()  / 2;
    cursory = Display.Height() / 2;

    Renderer->AddHud(&g_hud, 0.0f);
    Renderer->AddHud(&g_cursorHud, 10.0f);   // cursor on top of the menu
    return true;
}

void RenderUIMainMenuMode()
{
    g_hud.Refresh();

    // Clear; the opaque backdrop fully covers the native frame.
    Display.BackBuffer()->StartPass(0.0f, 0.0f, 0.0f, 1.0f);
    Display.BackBuffer()->EndPass();
}

void HandleMouseMoveUIMainMenuMode(int32_t x, int32_t y)
{
    int32_t px, py;
    DisplayToPane(x, y, px, py);
    const int32_t hit = HitTest(px, py);
    if (hit != g_hoverBtn)
        log_info("[ui-mainmenu] move disp(%d,%d) pane(%d,%d) disp=%dx%d hover=%d(%s)",
                 x, y, px, py, Display.Width(), Display.Height(),
                 hit, hit >= 0 ? kButtonBaseNames[hit] : "none");
    g_hoverBtn = hit;
}

void HandleMouseClickUIMainMenuMode(int32_t button, int32_t x, int32_t y)
{
    int32_t px, py;
    DisplayToPane(x, y, px, py);
    const int32_t hit = HitTest(px, py);
    if (button == MB_LEFTDOWN)
    {
        g_downBtn = hit;
    }
    else if (button == MB_LEFTUP)
    {
        if (hit >= 0 && hit == g_downBtn)
            log_info("[ui-mainmenu] activate button %d (%s)", hit, kButtonBaseNames[hit]);
        g_downBtn = -1;
    }
}

void CloseUIMainMenuMode()
{
    Renderer->RemoveHud(&g_cursorHud);
    Renderer->RemoveHud(&g_hud);
    SetHardwareCursorEnabled(true);   // restore the OS pointer for the next mode
    delete g_pane;
    g_pane       = nullptr;
    g_mainMenuBg = nullptr;
    for (auto& b : g_buttons) b = SButton{};
    g_menusDat   = nullptr;
    g_verFont    = nullptr;
}
