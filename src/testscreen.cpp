// *************************************************************************
// *                         Cinematix Revenant                            *
// *                                                                       *
// *   testscreen.cpp — bring-up harness for the retail sync port          *
// *************************************************************************

#include "testscreen.h"

#include "logging.h"
#include "revenant.h"

#include <cstring>

TTestScreen TestScreen;
char StartupTestMode[32] = "";

bool TTestScreen::Initialize()
{
    const char *mode = StartupTestMode[0] ? StartupTestMode : "blank";
    log_info("[test] initialize mode='%s'", mode);

    if (strcmp(mode, "blank") == 0)
    {
        // Minimum viable test: prove the harness routes correctly and the
        // screen base can be constructed + initialized without touching any
        // unsynced drawing code. Nothing else to do here.
        return true;
    }

    log_error("[test] unknown mode '%s' — falling back to blank", mode);
    return true;
}

void TTestScreen::Close()
{
    log_info("[test] close (ran %d frames)", frames);
}

void TTestScreen::Pulse()
{
    if (quit)
        SetNextScreen(nullptr);
}

void TTestScreen::DrawBackground()
{
}

void TTestScreen::Animate(bool)
{
    ++frames;
}

void TTestScreen::KeyPress(int32_t key, bool down)
{
    if (down && key == VK_ESCAPE)
        quit = true;
}
