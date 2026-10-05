// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   gameflow.cpp - TGameFlow, the application's screen sequencing       *
// *************************************************************************

#include "gameflow.h"

#include "cinematicscreen.h"
#include "death.h"
#include "logging.h"
#include "logoscreen.h"
#include "playscreen.h"
#include "revenant.h"

#include <cstdio>

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
        // given slot, straight into the PlayScreen.
        SSessionStart start;
        if (!options.quickstartSave.empty())
        {
            start.kind = SSessionStart::EKind::LoadSlot;
            start.slot = options.quickstartSave;
        }
        start.devLevel   = options.devLevel;
        start.devSectorX = options.devSectorX;
        start.devSectorY = options.devSectorY;

        log_info("[gameflow] boot: quickstart");
        if (StartSession(start))
            return &PlayScreen;
        log_error("[gameflow] boot: the game didn't start; showing the title screen");
        return &LogoScreen;
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

// REVSYNC: TLogoScreen New Game @ 0x0053a1f0 (start mode 0).
void TGameFlow::StartNewGame()
{
    PlayGame(SSessionStart{});
}

// Start mode 1: a slot that can't be loaded becomes a new game.
void TGameFlow::LoadGame(const char* slot)
{
    SSessionStart start;
    start.kind = SSessionStart::EKind::LoadSlot;
    start.slot = slot ? slot : "";
    PlayGame(start);
}

// REVSYNC: TPlayer::Animate @ 0x00518aa0 sets PlayScreen's next screen to
// the death screen and closes it once the death countdown runs out.
void TGameFlow::PlayerDied()
{
    log_info("[gameflow] player died");
    SwitchTo(&DeathScreen);
}

// REVSYNC: death pane Restart @ 0x00533950 reopens the PlayScreen with the
// start mode it last had. Its slot name and index were consumed when that
// game started, so retail starts a new game. See ARCHITECTURE.md §8 Q1.
void TGameFlow::RestartAfterDeath()
{
    StartNewGame();
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

// Retail tore the world down in TPlayScreen::Close (0x0047b290); here the
// game ends when the flow moves on from the PlayScreen.
void TGameFlow::ScreenEnded(TScreen* next)
{
    if (next != &PlayScreen)
        session.End();
}

// The load steps run in one go until the loading screen spreads them across
// frames (ARCHITECTURE.md §3.5, 2d).
bool TGameFlow::StartSession(const SSessionStart& start)
{
    session.Start(start);
    while (session.Step())
        ;
    if (session.Ready())
        return true;
    session.End();
    return false;
}

void TGameFlow::PlayGame(const SSessionStart& start)
{
    if (StartSession(start))
        SwitchTo(&PlayScreen);
    else
        log_error("[gameflow] the game didn't start");
}

void TGameFlow::SwitchTo(TScreen* next)
{
    if (!CurrentScreen)
    {
        log_warn("[gameflow] transition requested with no current screen");
        return;
    }
    CurrentScreen->SetNextScreen(next);
    CurrentScreen->RequestClose();
}
