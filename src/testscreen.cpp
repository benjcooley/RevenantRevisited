// *************************************************************************
// *                         Cinematix Revenant                            *
// *                                                                        *
// *   testscreen.cpp — bring-up harness for the retail sync port           *
// *************************************************************************

#include "testscreen.h"

#include "logging.h"
#include "testconfig.h"
#include "testmodes.h"
#include "time.h"
#include "vfxreview.h"

#include <cstring>

TTestScreen TestScreen;

bool TTestScreen::Initialize()
{
    const char *mode = StartupTestMode[0] ? StartupTestMode : "blank";
    log_info("[test] initialize mode='%s'", mode);
    lastLoggedLegacyFrame = -1;
    return TestModes::Initialize(mode);
}

void TTestScreen::Close()
{
    const char* mode = StartupTestMode[0] ? StartupTestMode : "blank";
    TestModes::Close(mode);
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

void TTestScreen::DrawBackground()
{
    if (strcmp(StartupTestMode,"vfx-review")==0) VfxReview::DrawOverlay();
}

void TTestScreen::Animate(bool)
{
    const char* mode = StartupTestMode[0] ? StartupTestMode : "blank";
    TestModes::Render(mode);
}

// Input reaches the screen's panes first (test modes that host production
// panes), then the test mode's own handler.

void TTestScreen::KeyPress(int32_t key, bool down)
{
    if (down && key == VK_ESCAPE)
        SetDone();
    TScreen::KeyPress(key, down);
    const char* mode = StartupTestMode[0] ? StartupTestMode : "blank";
    TestModes::HandleKeyPress(mode, key, down);
}

void TTestScreen::MouseClick(int32_t button, int32_t x, int32_t y)
{
    TScreen::MouseClick(button, x, y);
    const char* mode = StartupTestMode[0] ? StartupTestMode : "blank";
    TestModes::HandleMouseClick(mode, button, x, y);
}

void TTestScreen::MouseMove(int32_t button, int32_t x, int32_t y)
{
    TScreen::MouseMove(button, x, y);
    const char* mode = StartupTestMode[0] ? StartupTestMode : "blank";
    TestModes::HandleMouseMove(mode, button, x, y);
}
