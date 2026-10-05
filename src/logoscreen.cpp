// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   logoscreen.cpp - TLogoScreen, the title / main-menu screen          *
// *************************************************************************

#include "logoscreen.h"

#include "gameflow.h"
#include "logging.h"
#include "multi.h"
#include "player.h"
#include "renderer.h"
#include "revenant.h"

#include <cstdio>
#include <cstring>

TLogoScreen LogoScreen;

namespace {

// Retail button order and sprite base names (Initialize @ 0x0053a2c0); each
// button loads <base>U / <base>D / <base>S from menus.dat.
const char* const kButtonSprites[TLogoScreen::BTN_COUNT] = {
    "MenuNewGame", "MenuLoadGame", "MenuMulti", "MenuOptions", "MenuExit",
};
const char* const kButtonNames[TLogoScreen::BTN_COUNT] = {
    "newgame", "loadgame", "multi", "options", "exit",
};

// Version text (Animate @ 0x0053a6d0): "Revenant   v%d.%02d" with the
// retail version bytes 1 / 22 (0x005d79dc/dd), cell (15,450) 150x40, flags
// 0x41 = TEXT_LEFT | TEXT_VCENTER, color (175,72,255) — the violet visible
// in the retail screenshot.
constexpr int32_t kVersionMajor = 1;
constexpr int32_t kVersionMinor = 22;
constexpr uint32_t kTextLeft    = 0x0001;
constexpr uint32_t kTextVCenter = 0x0040;

// --menu auto-press delay: long enough for the menu to be on screen (and in a
// filmstrip) before the button fires. Pulses run at 24 Hz.
constexpr int32_t kAutoActivatePulses = 24;

constexpr float kCursorHudZ = 1000.0f;

}  // namespace

int32_t TLogoScreen::ButtonFromName(const char* name)
{
    if (!name || !*name)
        return -1;
    for (int32_t i = 0; i < BTN_COUNT; ++i)
        if (!strcasecmp(name, kButtonNames[i]) || !strcasecmp(name, kButtonSprites[i]))
            return i;
    return -1;
}

// REVSYNC: TLogoScreen::Initialize @ 0x0053a2c0. Retail paints the MainMenu
// backdrop as the screen background and the buttons from a separate
// TButtonPane; here both are the one menu pane (backdrop = pane background),
// which draws the same pixels in the same order.
bool TLogoScreen::Initialize()
{
    log_info("[logoscreen] initialize");

    // Retail clears the player list first (0x0051eda0): the title screen is
    // also where an ended game lands.
    PlayerManager.Clear();

    int32_t ox = 0, oy = 0;
    ClassicCanvasOrigin(ox, oy);
    if (!menu.OpenChrome(ox, oy, WIDTH, HEIGHT, "menus.dat", "MainMenu"))
        return false;
    for (int32_t i = 0; i < BTN_COUNT; ++i)
        menu.AddSpriteButton(kButtonNames[i], kButtonSprites[i]);

    char version[32];
    std::snprintf(version, sizeof(version), "Revenant   v%d.%02d", kVersionMajor, kVersionMinor);
    menu.AddText("version", 15, 450, 150, 40, version, kTextLeft | kTextVCenter,
                 SDefColor{175, 72, 255, true}, "Med");

    menu.SetOnActivate([this](TDefPane&, const SDefWidget& widget, int32_t) {
        Activate(ButtonFromName(widget.name.c_str()));
    });
    AddPane(&menu);

    // Retail installs the GameData "cursor" (0x0043a020).
    if (PTBitmap cursor = GameData ? GameData->Bitmap(const_cast<char*>("cursor")) : nullptr)
        SetMouseBitmap(cursor);
    if (Renderer)
        Renderer->AddHud(&cursorHud, kCursorHudZ);
    return true;
}

void TLogoScreen::Close()
{
    if (Renderer)
        Renderer->RemoveHud(&cursorHud);
    RemovePane(&menu);
    menu.Close();
    autoActivate = -1;
}

void TLogoScreen::Pulse()
{
    TScreen::Pulse();
    if (autoActivate >= 0 && FrameCount() >= kAutoActivatePulses)
    {
        const int32_t button = autoActivate;
        autoActivate = -1;
        log_info("[logoscreen] --menu: pressing %s", kButtonNames[button]);
        Activate(button);
    }
}

// REVSYNC: button callbacks @ 0x0053a1f0 (New Game), 0x0053a220 (Load Game),
// 0x0053a240 (Multiplayer), 0x0053a260 (Options), 0x0053a2a0 (Exit).
void TLogoScreen::Activate(int32_t button)
{
    if (button < 0 || button >= BTN_COUNT)
        return;
    log_info("[logoscreen] %s", kButtonNames[button]);
    switch (button)
    {
    case BTN_NEWGAME:
        GameFlow.StartNewGame();
        break;
    case BTN_EXIT:
        GameFlow.QuitApplication();
        break;
    case BTN_LOADGAME:
        // Retail switches to the load-game screen (0x0066fa78, loadgame.def);
        // that screen is not ported yet (BURNDOWN T6).
        log_warn("[logoscreen] Load Game screen not ported yet");
        break;
    case BTN_OPTIONS:
        // Retail switches to the options screen (0x0066fe88, options.def);
        // TOptionsPane exists, its hosting screen is not ported yet (T2).
        log_warn("[logoscreen] Options screen not ported yet");
        break;
    case BTN_MULTI:
        log_warn("[logoscreen] multiplayer is not supported");
        break;
    }
}
