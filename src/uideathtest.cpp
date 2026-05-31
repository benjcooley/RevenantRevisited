// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uideathtest.cpp - --test=ui-death                                    *
// *************************************************************************
//
// Clean-room reconstruction of TDeathPane (the player-death "Game Over" modal,
// retail class cls_0x5b93c4, with the TDeathScreen wrapper cls_0x5b9374). Built
// ONLY from docs/ui/forensics/DeathPane_SPEC.md. Every coordinate / asset cites
// its spec section inline.
//
// Anatomy (spec §2 / §4):
//   - "background" 640x480 opaque backdrop ("GAME OVER" graveyard scene) at
//     (0,0), DM_BACKGROUND.                                          (§4 row 1)
//   - three bitmap word-buttons stamped at their bitmap REG (DM_USEREG):
//       Restart {U,D,S} 170x62 @ (22,404)      reg (-22,-404)         (§2/§4)
//       Load    {U,D,S} 172x62 @ (232,404)     reg (-232,-404)
//       Exit    {U,D,S} 172x62 @ (438,404)     reg (-438,-404)
//     state per button: U (default) / D (mouse-down). S (selected) is loaded
//     for asset parity but never shown -- radiogroup = -1, no focus nav (§4).
//
// Architecture (spec §3 / §5 direct-renderer contract): retail draws straight
// to the display, but the port composes background + 3 button sprites into ONE
// 640x480 offscreen TSurface render target via the `…ToTarget` family, then
// DrawSurface's it once in the HUD pass -- the same contract every other ui-*
// test mode follows (uiplyrstatusbartest.cpp). No glyphs, no shadows: all text
// is baked into the bitmaps (§7 / §8). The button bitmaps carry real per-pixel
// alpha (flags 0x402); the renderer's to-target path honors it (no magenta key).
//
// Out of scope for the visual reconstruction (spec §6 / §9): the random
// death-gasp SFX, the SaveGame.ReadGame() button handlers, and the
// PlayScreen-exclusive activation wiring -- those belong to the screen/gameflow
// layer, not this standalone visual rig.
//
// *************************************************************************

#include "uideathtest.h"

#include "bitmap.h"
#include "bitmapdata.h"
#include "cursor.h"
#include "display.h"
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

// =====================================================================
// The three death buttons, in retail Initialize order (spec §4 / §6.2).
// =====================================================================
enum { BTN_RESTART = 0, BTN_LOAD, BTN_EXIT, BTN_COUNT };

const char* const kButtonBaseNames[BTN_COUNT] = { "Restart", "Load", "Exit" };

struct SButton
{
    PTBitmap up   = nullptr;     // <base>U — default
    PTBitmap down = nullptr;     // <base>D — mouse-down
    PTBitmap sel  = nullptr;     // <base>S — vestigial (never shown, §4)
    int32_t  x = 0, y = 0;       // screen TL = (-regx, -regy) of the U sprite (§2/§4)
    int32_t  w = 0, h = 0;       // U sprite size (§2/§4)
};

// =====================================================================
// Loaded assets (spec §2 — one archive, death.dat).
// =====================================================================
TMulti*  g_deathDat = nullptr;
PTBitmap g_background = nullptr;     // "background" 640x480 opaque
SButton  g_buttons[BTN_COUNT];

TSurface* g_pane = nullptr;          // 640x480 compose RT

// Interaction state (display-space hit testing -> RT-local). Hover does NOT
// change state in retail (no glow/selected for this non-radio set); only
// mouse-down swaps to the D sprite (§9).
int32_t g_downBtn = -1;

void DisplayToPane(int32_t dx, int32_t dy, int32_t& px, int32_t& py)
{
    const int32_t ox = (Display.Width()  - kScreenW) / 2;
    const int32_t oy = (Display.Height() - kScreenH) / 2;
    px = dx - (ox > 0 ? ox : 0);
    py = dy - (oy > 0 ? oy : 0);
}

// Rect hit test. (Retail uses bitmap-alpha pixelcheck for non-square hit areas,
// spec §10; the bounding rect is a faithful-enough approximation for the rig --
// the alpha-shaped refinement is a hit-test detail, not a visual one.)
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
class TDeathHud : public THudDrawable
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
        if (!g_background) return;
        EnsurePane();
        if (!g_pane) return;

        const int32_t tw = g_pane->Width();
        const int32_t th = g_pane->Height();

        g_pane->StartPass(0.0f, 0.0f, 0.0f, 1.0f);

        // --- (1) background, opaque full-frame at (0,0) (spec §4 / §5 pass 1) ---
        Renderer->DrawBitmapToTarget(g_background, 0, 0, tw, th);

        // --- (2) three buttons at their bitmap-REG screen pos (spec §4 / §5
        // pass 3). U (default) unless this button is mouse-down -> D (§9). ---
        for (int32_t i = 0; i < BTN_COUNT; ++i)
        {
            const SButton& b = g_buttons[i];
            PTBitmap sprite = (i == g_downBtn && b.down) ? b.down : b.up;
            if (sprite)
                Renderer->DrawBitmapToTarget(sprite, b.x, b.y, tw, th);
        }

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
};

TDeathHud g_hud;

// The retail death screen re-registers the game cursor (TDeathScreen.Initialize
// FUN_0046d710("cursor")/FUN_0043a020, §2/§6.1). We install it in-frame
// (hardware cursor disabled) so it draws over the modal and the --mouse-script
// simulator can drive it. z=10 > pane z=0.
TCursorHud g_cursorHud;

}  // namespace

// =====================================================================
// Entry points (testmodes.cpp links these — keep the signatures).
// =====================================================================
bool InitializeUIDeathMode()
{
    log_info("[ui-death] === TDeathPane (clean-room from spec) ===");

    g_deathDat = TMulti::LoadMulti((char*)"death.dat");
    if (g_deathDat)
        g_background = g_deathDat->Bitmap((char*)"background");

    char nm[64];
    for (int32_t i = 0; i < BTN_COUNT; ++i)
    {
        SButton& b = g_buttons[i];
        if (!g_deathDat) continue;
        std::snprintf(nm, sizeof(nm), "%sU", kButtonBaseNames[i]); b.up   = g_deathDat->Bitmap(nm);
        std::snprintf(nm, sizeof(nm), "%sD", kButtonBaseNames[i]); b.down = g_deathDat->Bitmap(nm);
        std::snprintf(nm, sizeof(nm), "%sS", kButtonBaseNames[i]); b.sel  = g_deathDat->Bitmap(nm);
        if (b.up)
        {
            b.x = std::abs(b.up->regx);   // dst TL = (-regx,-regy) (§2/§4)
            b.y = std::abs(b.up->regy);
            b.w = b.up->width;
            b.h = b.up->height;
        }
        log_info("[ui-death] %-8s U=%s D=%s S=%s @ (%d,%d) %dx%d",
                 kButtonBaseNames[i], b.up ? "OK" : "MISS", b.down ? "OK" : "MISS",
                 b.sel ? "OK" : "MISS", b.x, b.y, b.w, b.h);
    }

    log_info("[ui-death] backdrop background=%s", g_background ? "OK" : "MISS");

    delete g_pane;
    g_pane    = nullptr;
    g_downBtn = -1;

    // Install the in-frame game cursor (hardware cursor disabled). GameData
    // (playscrn.dat) holds "cursor"; load it if a standalone test left it null.
    if (!GameData) GameData = TMulti::LoadMulti((char*)"playscrn.dat");
    SetHardwareCursorEnabled(false);
    if (GameData)
        if (PTBitmap c = GameData->Bitmap((char*)"cursor")) SetMouseBitmap(c);
    cursorx = Display.Width()  / 2;
    cursory = Display.Height() / 2;

    Renderer->AddHud(&g_hud, 0.0f);
    Renderer->AddHud(&g_cursorHud, 10.0f);   // cursor on top of the modal
    return true;
}

void RenderUIDeathMode()
{
    g_hud.Refresh();

    Display.BackBuffer()->StartPass(0.0f, 0.0f, 0.0f, 1.0f);
    Display.BackBuffer()->EndPass();
}

void HandleMouseMoveUIDeathMode(int32_t /*x*/, int32_t /*y*/)
{
    // No hover state for the death buttons (no glow/selected sprite shown, §9).
}

void HandleMouseClickUIDeathMode(int32_t button, int32_t x, int32_t y)
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
            log_info("[ui-death] activate button %d (%s)", hit, kButtonBaseNames[hit]);
        g_downBtn = -1;
    }
}

void CloseUIDeathMode()
{
    Renderer->RemoveHud(&g_cursorHud);
    Renderer->RemoveHud(&g_hud);
    SetHardwareCursorEnabled(true);   // restore the OS pointer for the next mode
    delete g_pane;
    g_pane       = nullptr;
    g_background = nullptr;
    for (auto& b : g_buttons) b = SButton{};
    g_deathDat   = nullptr;
}
