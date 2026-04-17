// *************************************************************************
// *                         Cinematix Revenant                            *
// *                                                                        *
// *   testscreen.cpp — bring-up harness for the retail sync port           *
// *************************************************************************

#include "testscreen.h"

#include "logging.h"
#include "revenant.h"
#include "time.h"

#include <cstring>

TTestScreen TestScreen;
char        StartupTestMode[32] = "";

bool TTestScreen::Initialize()
{
    const char *mode = StartupTestMode[0] ? StartupTestMode : "blank";
    log_info("[test] initialize mode='%s'", mode);
    lastLoggedLegacyFrame = -1;
    quit = false;

    if (strcmp(mode, "blank") == 0 || strcmp(mode, "ticker") == 0)
        return true;

    log_error("[test] unknown mode '%s' — falling back to blank", mode);
    return true;
}

void TTestScreen::Close()
{
    log_info("[test] close (real frames=%lld, legacy frames=%lld, time=%.3fs)",
        static_cast<long long>(TTime::FrameCount()),
        static_cast<long long>(TTime::LegacyFrameCount()),
        TTime::Time());
}

void TTestScreen::Pulse()
{
    // ticker mode: sample TTime once per legacy second so we can eyeball the
    // counters without flooding the log.
    if (strcmp(StartupTestMode, "ticker") == 0)
    {
        int64_t now = TTime::LegacyFrameCount();
        if (now / TTime::LegacyFramerate != lastLoggedLegacyFrame / TTime::LegacyFramerate)
        {
            log_info("[ticker] t=%.3fs dt=%.4fs real=%lld legacy=%lld",
                TTime::Time(), TTime::DeltaTime(),
                static_cast<long long>(TTime::FrameCount()),
                static_cast<long long>(now));
            lastLoggedLegacyFrame = now;
        }
    }
}

void TTestScreen::DrawBackground() {}

void TTestScreen::Animate(bool) {}

void TTestScreen::KeyPress(int32_t key, bool down)
{
    if (down && key == VK_ESCAPE)
        quit = true;
}
