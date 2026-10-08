// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   cinematicscreen.h - full-motion-video (.SMK) playback screen         *
// *************************************************************************
//
// A screen that plays one movie (TMoviePane) and ends with it: the intro
// before the title (retail WinMain, Mix_FMV1.smk), and `--test=ui-cinematic`.
// Movies during play are modal panes on the PlayScreen instead.
#pragma once

#include "moviepane.h"
#include "revenant.h"   // MAXPATHLEN
#include "screen.h"

class TCinematicScreen : public TScreen
{
  public:
    // The .SMK to play, set before entering the screen. `--test=ui-cinematic`
    // reads StartupCinematicPath instead; with neither, the retail intro.
    void SetVideo(const char* path);

    bool Initialize() override;
    void Close() override;

  private:
    TMoviePane movie;
    char       path_[MAXPATHLEN] = {};
};

extern TCinematicScreen CinematicScreen;
