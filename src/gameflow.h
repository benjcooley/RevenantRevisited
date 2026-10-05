// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   gameflow.h - TGameFlow, the application's screen sequencing         *
// *************************************************************************
//
// Which screen is up and when a game starts or ends. Retail had no single
// owner for this: WinMain, the title buttons, the death pane, TPlayer and the
// endgame command each set a screen's `nextscreen` and closed it. TGameFlow
// gives every transition one named intent; screens and commands call these
// instead of switching screens themselves. It also owns the game session:
// a game lasts while the PlayScreen is up. See docs/gameflow/ARCHITECTURE.md
// §2-3 and docs/gameflow/forensics/GAME_FLOW.md for the retail sequence.
#pragma once

#include "gamesession.h"

#include <cstdint>
#include <string>

class TScreen;

// How the application starts (WinMain @ 0x004865a0 + command-line flags).
struct SBootOptions
{
    bool        quickstart = false;   // retail QUICKSTART: no intro, no title
    std::string quickstartSave;       // QUICKSTART="<slot>": load it instead of a new game
    bool        noIntro = false;      // skip the intro movie
    int32_t     menuButton = -1;      // press this title button automatically (tests)

    // Developer override (--level / --sector) for quickstarted games; see
    // SSessionStart.
    int32_t     devLevel   = -1;
    int32_t     devSectorX = -1;
    int32_t     devSectorY = -1;
};

class TGameFlow
{
  public:
    // The first screen to show for `options`: intro movie -> title, or
    // straight into a game for QUICKSTART.
    TScreen* Boot(const SBootOptions& options);

    void StartNewGame();                    // title "New Game"
    void LoadGame(const char* slot);        // title / death "Load"
    void PlayerDied();                      // TPlayer death countdown ran out
    void RestartAfterDeath();               // death screen "Restart"
    void ReturnToTitle();                   // Quit Module, death Exit, endgame
    void QuitApplication();                 // title Exit, in-game Exit Program

    // Called once the current screen has closed, with the screen that
    // follows it. Leaving the PlayScreen ends the game.
    void ScreenEnded(TScreen* next);

    [[nodiscard]] TGameSession& Session() { return session; }

  private:
    // Loads a game into the session. False (and no game) if it failed.
    bool StartSession(const SSessionStart& start);
    // Starts a game and shows it, or returns to the title if it can't start.
    void PlayGame(const SSessionStart& start);
    // Ends the current screen with `next` as its successor (null = quit).
    void SwitchTo(TScreen* next);

    TGameSession session;
};

extern TGameFlow GameFlow;
