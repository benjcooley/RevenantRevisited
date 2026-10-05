// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   gameflow.h - TGameFlow, the application's screen sequencing         *
// *************************************************************************
//
// Which screen is up and when a game starts or ends. Retail had no single
// owner for this: WinMain, the title buttons, the death pane, TPlayer and the
// endgame command each set a screen's `nextscreen` and closed it. TGameFlow
// gives every transition one named intent; screens and commands call these
// instead of switching screens themselves. See docs/gameflow/ARCHITECTURE.md
// §2 and docs/gameflow/forensics/GAME_FLOW.md for the retail sequence.
#pragma once

#include <cstdint>
#include <string>

class TScreen;

// How the application starts (WinMain @ 0x004865a0 + command-line flags).
struct SBootOptions
{
    bool        quickstart = false;   // retail QUICKSTART: no intro, no title
    std::string quickstartSave;       // QUICKSTART="<save>": load it instead of a new game
    bool        noIntro = false;      // skip the intro movie
    int32_t     menuButton = -1;      // press this title button automatically (tests)
};

class TGameFlow
{
  public:
    // The first screen to show for `options`: intro movie -> title, or
    // straight into a game for QUICKSTART.
    TScreen* Boot(const SBootOptions& options);

    void StartNewGame();                    // title "New Game"
    void LoadGame(const char* saveName);    // QUICKSTART="<save>", load screens
    void ReturnToTitle();                   // Quit Module, death Exit, endgame
    void QuitApplication();                 // title Exit, in-game Exit Program

  private:
    // Ends the current screen with `next` as its successor (null = quit). Before
    // the first screen runs, `next` simply becomes the boot screen.
    void SwitchTo(TScreen* next);
};

extern TGameFlow GameFlow;
