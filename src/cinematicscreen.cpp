// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   cinematicscreen.cpp - full-motion-video (.SMK) playback screen       *
// *************************************************************************
//
// See cinematicscreen.h.

#include "cinematicscreen.h"

#include "revutils.h"
#include "testconfig.h"

#include <cstdio>

TCinematicScreen CinematicScreen;

void TCinematicScreen::SetVideo(const char* path)
{
    if (path)
        strncpyz(path_, path, sizeof(path_));
}

bool TCinematicScreen::Initialize()
{
    char path[MAXPATHLEN];
    if (path_[0])
        strncpyz(path, path_, sizeof(path));
    else if (StartupCinematicPath[0])
        strncpyz(path, StartupCinematicPath, sizeof(path));
    else
        std::snprintf(path, sizeof(path), "%sMix_FMV1.smk", MoviePath);

    // The screen ends with the movie (or at once if it can't play, so a
    // missing file doesn't wedge the boot).
    movie.SetOnFinished([this] { SetDone(); });
    movie.Initialize();
    AddPane(&movie);
    movie.Open(path);
    return true;
}

void TCinematicScreen::Close()
{
    RemovePane(&movie);
    movie.Close();
}
