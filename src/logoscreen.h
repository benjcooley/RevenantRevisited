// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   logoscreen.h - TLogoScreen, the title / main-menu screen            *
// *************************************************************************
//
// Retail TLogoScreen (cls_0x5a5d18; Initialize 0x0053a2c0, Animate
// 0x0053a6d0): the "MainMenu" backdrop from menus.dat, five sprite buttons
// (New Game / Load Game / Multiplayer / Options / Exit) and the version text.
// Visual reference: docs/ui/forensics/MainMenu_SPEC.md and the retail
// screenshot docs/ui/main_menu_ui.jpg.
#pragma once

#include "cursor.h"     // TCursorHud
#include "defpane.h"
#include "screen.h"

class TLogoScreen : public TScreen
{
  public:
    enum EButton : int32_t
    {
        BTN_NEWGAME = 0,
        BTN_LOADGAME,
        BTN_MULTI,
        BTN_OPTIONS,
        BTN_EXIT,
        BTN_COUNT
    };

    bool Initialize() override;
    void Close() override;
    void Pulse() override;

    // Runs a button's retail callback, exactly as clicking it would.
    void Activate(int32_t button);

    // Press `button` automatically once the menu has been up for a moment
    // (--menu=<name>, for tests). -1 = off.
    void SetAutoActivate(int32_t button) { autoActivate = button; }

    // "newgame" / "loadgame" / "multi" / "options" / "exit" (or the retail
    // sprite base names, "MenuNewGame" ...) -> button; -1 if unknown.
    [[nodiscard]] static int32_t ButtonFromName(const char* name);

  private:
    TDefPane   menu;            // retail TButtonPane (+0x88) with the five buttons
    TCursorHud cursorHud;
    int32_t    autoActivate = -1;
};

extern TLogoScreen LogoScreen;
