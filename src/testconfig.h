// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  testconfig.h - Test startup config                   *
// *************************************************************************

#pragma once

#include "revenant.h"

extern char StartupTestMode[32];
// --sector=L_X_Y — which sector --test=sector should render. Empty = default.
extern char StartupSectorId[32];
// --level=L — which level --test=sector should render, centered on world 0,0.
extern char StartupLevelId[32];
// --asset=path — which imagery --test=i3d3d should load (default Misc\\Blood.I3D).
extern char StartupAssetPath[128];
// --scale=f — uniform mesh scale multiplier for --test=i3d3d.
extern float StartupAssetScale;
// --dumptiles=<path> — export tile albedo PNGs into the given folder, then exit.
extern char StartupDumpTilesPath[MAXPATHLEN];
// --vfx=<id> — pre-select an effect by id in --test=vfx (e.g.
// --vfx=TStripEffect). Empty = first alphabetically-sorted entry.
// Useful for clean screencaps + scripted iteration.
extern char StartupVfxId[64];
// --vfx-no-ui — suppress the ImGui VFX Browser panel so screencaps
// show only the effect render.
extern bool StartupVfxHideUi;
