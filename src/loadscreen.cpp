// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   loadscreen.cpp - TLoadScreen, the loading bar while a game starts   *
// *************************************************************************

#include "loadscreen.h"

#include "bitmap.h"
#include "display.h"
#include "gameflow.h"
#include "logging.h"
#include "multi.h"
#include "renderer.h"
#include "surface.h"

#include <algorithm>
#include <cstdlib>

TLoadScreen LoadScreen;

// ****************
// * TLoadBarPane *
// ****************

bool TLoadBarPane::Initialize()
{
    TPane::Initialize();

    progress = 0;
    composed = -1;
    dat = TMulti::LoadMulti(const_cast<char*>("loadbar.dat"));
    if (dat)
    {
        background = dat->Bitmap(const_cast<char*>("background"));
        bar        = dat->Bitmap(const_cast<char*>("bar"));
    }
    if (!background || !bar)
        log_warn("[loadscreen] loadbar.dat: background=%s bar=%s",
                 background ? "ok" : "missing", bar ? "ok" : "missing");
    return true;
}

void TLoadBarPane::Close()
{
    surface.reset();
    background = bar = nullptr;
    delete dat;
    dat = nullptr;
    TPane::Close();
}

void TLoadBarPane::Step(int32_t delta)
{
    progress = std::clamp(progress + delta, 0, kFull);
}

void TLoadBarPane::Set(float fraction)
{
    progress = std::clamp(static_cast<int32_t>(fraction * kFull), 0, kFull);
}

// Spec §4-§6: the background opaque at (0, 0); the bar at its bitmap's
// registration point taken as a screen position (24, 414), as a left slice
// progress * width / 1000 wide (BM_ALPHA gives the stone end caps).
void TLoadBarPane::Compose()
{
    if (!background || !bar || composed == progress)
        return;

    if (!surface || surface->Width() != background->width || surface->Height() != background->height)
        surface = std::make_unique<TSurface>(background->width, background->height, SG_PIXELFORMAT_RGBA8);

    const int32_t w = surface->Width();
    const int32_t h = surface->Height();
    surface->StartPass(0.0f, 0.0f, 0.0f, 1.0f);
    Renderer->DrawBitmapToTarget(background, 0, 0, w, h);
    const int32_t fill = progress * bar->width / kFull;
    if (fill > 0)
        Renderer->DrawBitmapSubrectToTarget(bar, std::abs(bar->regx), std::abs(bar->regy),
                                            0, 0, fill, bar->height, w, h);
    surface->EndPass();
    composed = progress;
    log_debug("[loadscreen] bar %d/1000", progress);
}

// The Classic 640x480 picture, centred at larger displays (spec §13).
void TLoadBarPane::Draw()
{
    if (!surface)
        return;
    const int32_t x = (std::max)(0, (Display.Width() - surface->Width()) / 2);
    const int32_t y = (std::max)(0, (Display.Height() - surface->Height()) / 2);
    Renderer->DrawSurface(surface.get(), x, y);
}

// ***************
// * TLoadScreen *
// ***************

bool TLoadScreen::Initialize()
{
    bar.Initialize();
    AddPane(&bar);
    log_info("[loadscreen] up");
    return true;
}

void TLoadScreen::Close()
{
    log_info("[loadscreen] done at %d/1000", bar.Progress());
    RemovePane(&bar);
    bar.Close();
}

// The first frame shows the empty bar before any loading stalls the loop.
void TLoadScreen::Pulse()
{
    if (!FirstFrame() && !IsDone())
        GameFlow.ContinueLoading();
    TScreen::Pulse();
}

void TLoadScreen::SetProgress(int32_t permille)
{
    bar.Set(static_cast<float>(permille) / TLoadBarPane::kFull);
}
