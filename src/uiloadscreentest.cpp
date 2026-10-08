// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uiloadscreentest.cpp - --test=ui-loadscreen                          *
// *************************************************************************
//
// A host for the production loading bar (TLoadBarPane, src/loadscreen.cpp;
// spec docs/ui/forensics/LoadingScreen_SPEC.md). The game drives the bar from
// its load steps; here a timer ramps it 0 -> 100% over three seconds, holds,
// and repeats, so a capture can catch it mid-fill (spec §9).
//
// *************************************************************************
#include "uiloadscreentest.h"

#include "display.h"
#include "loadscreen.h"
#include "logging.h"
#include "renderer.h"
#include "time.h"

#include <cmath>

namespace {

constexpr double kRampSecs  = 3.0;
constexpr double kHoldSecs  = 0.6;
constexpr double kCycleSecs = kRampSecs + kHoldSecs;

TLoadBarPane g_bar;

class TLoadBarHud : public THudDrawable
{
  public:
    void Draw() override { g_bar.Draw(); }
};

TLoadBarHud g_hud;

// The test's stand-in for the load steps.
float RampFraction()
{
    const double t     = TTime::Time();
    const double phase = t - kCycleSecs * std::floor(t / kCycleSecs);
    return phase >= kRampSecs ? 1.0f : static_cast<float>(phase / kRampSecs);
}

}  // namespace

bool InitializeUILoadScreenMode()
{
    log_info("[ui-loadscreen] host for TLoadBarPane");
    g_bar.Initialize();
    Renderer->AddHud(&g_hud, 0.0f);
    return true;
}

void RenderUILoadScreenMode()
{
    g_bar.Set(RampFraction());
    g_bar.Compose();

    // The backdrop is opaque; the clear only shows in letterbox bars.
    Display.BackBuffer()->StartPass(0.0f, 0.0f, 0.0f, 1.0f);
    Display.BackBuffer()->EndPass();
}

void CloseUILoadScreenMode()
{
    Renderer->RemoveHud(&g_hud);
    g_bar.Close();
}
