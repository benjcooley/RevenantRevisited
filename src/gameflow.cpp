// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   gameflow.cpp - TGameFlow, the application's screen sequencing       *
// *************************************************************************

#include "gameflow.h"

#include "cinematicscreen.h"
#include "logging.h"
#include "logoscreen.h"
#include "playscreen.h"
#include "revenant.h"

TGameFlow GameFlow;

namespace {

// Retail WinMain plays <MoviePath>\Mix_FMV1.smk before the title screen; the
// name is stored as raw dwords at 0x005d8ddc.
constexpr const char* kIntroMovie = "Mix_FMV1.smk";

}  // namespace

// REVSYNC: WinMain @ 0x004865a0 (boot-screen selection after engine init).
TScreen* TGameFlow::Boot(const SBootOptions& options)
{
    if (options.quickstart)
    {
        // QUICKSTART (GetParameters @ 0x00483bc0): start mode 0, or 1 with the
        // given save name, straight into the PlayScreen.
        if (!options.quickstartSave.empty())
        {
            log_info("[gameflow] boot: quickstart, load '%s'", options.quickstartSave.c_str());
            PlayScreen.SetStartMode(TPlayScreen::STARTMODE_LOADGAME, -1, -1,
                                    options.quickstartSave.c_str());
        }
        else
        {
            log_info("[gameflow] boot: quickstart, new game");
            PlayScreen.SetStartMode(TPlayScreen::STARTMODE_NEWGAME);
        }
        return &PlayScreen;
    }

    LogoScreen.SetAutoActivate(options.menuButton);
    if (options.noIntro || options.menuButton >= 0)
    {
        log_info("[gameflow] boot: title screen");
        return &LogoScreen;
    }

    log_info("[gameflow] boot: intro movie, then title screen");
    char intro[MAXPATHLEN];
    std::snprintf(intro, sizeof(intro), "%s%s", MoviePath, kIntroMovie);
    CinematicScreen.SetVideo(intro);
    CinematicScreen.SetNextScreen(&LogoScreen);
    return &CinematicScreen;
}

// REVSYNC: TLogoScreen New Game @ 0x0053a1f0.
void TGameFlow::StartNewGame()
{
    PlayScreen.SetStartMode(TPlayScreen::STARTMODE_NEWGAME);
    SwitchTo(&PlayScreen);
}

void TGameFlow::LoadGame(const char* saveName)
{
    PlayScreen.SetStartMode(TPlayScreen::STARTMODE_LOADGAME, -1, -1, saveName);
    SwitchTo(&PlayScreen);
}

// REVSYNC: endgame @ 0x00427060, in-game Quit Module (0x0047e500 case 4),
// death pane Exit @ 0x00533990 — each sets next = title and closes.
void TGameFlow::ReturnToTitle()
{
    SwitchTo(&LogoScreen);
}

// REVSYNC: TLogoScreen Exit @ 0x0053a2a0 (next = none).
void TGameFlow::QuitApplication()
{
    SwitchTo(nullptr);
}

void TGameFlow::SwitchTo(TScreen* next)
{
    if (!CurrentScreen)
    {
        log_warn("[gameflow] transition requested with no current screen");
        return;
    }
    CurrentScreen->SetNextScreen(next);
    CurrentScreen->SetDone();
}
