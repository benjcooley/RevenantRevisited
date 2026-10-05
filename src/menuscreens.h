// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   menuscreens.h - the title route's Load Game and Options screens     *
// *************************************************************************
//
// Retail's Load Game screen (0x0066fa78, vtable 0x5b9538) and Options screen
// (0x0066fe88, vtable 0x5b96f4): small screens hosting the load dialog and
// the options pane as ordinary panes, reached from the title (and the death
// screen's Load). In the game the same panes run as modals over the play
// screen (TInGameMenu). docs/gameflow/forensics/INGAME_MENU.md §8,
// docs/gameflow/ARCHITECTURE.md §4.5-4.6.
#pragma once

#include "cursor.h"     // TCursorHud
#include "optionspane.h"
#include "savegamepane.h"
#include "screen.h"

class TLoadGameScreen : public TScreen
{
  public:
    // REVSYNC: Initialize @ 0x005392e0 -- the cursor, the fader, the load
    // dialog (loadFromGame = 0) as a pane.
    bool Initialize() override;
    // REVSYNC: Close @ 0x00539360
    void Close() override;

  private:
    TLoadGamePane pane;         // retail's shared load pane 0x0066f8d0
    TCursorHud    cursorHud;
    TScreenFade   screenfade;   // retail +0x70
};

class TOptionsScreen : public TScreen
{
  public:
    // REVSYNC: Initialize @ 0x0053a800 -- the options pane (fromGame = 0)
    // and the cursor. No fader.
    bool Initialize() override;
    // REVSYNC: Close @ 0x0053a860
    void Close() override;

  private:
    TOptionsPane pane;          // retail's shared options pane 0x0066fcc0
    TCursorHud   cursorHud;
};

extern TLoadGameScreen LoadGameScreen;
extern TOptionsScreen  OptionsScreen;
