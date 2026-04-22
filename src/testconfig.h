// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  testconfig.h - Test startup config                   *
// *************************************************************************

#pragma once

extern char StartupTestMode[32];
// --sector=L_X_Y — which sector --test=sector should render. Empty = default.
extern char StartupSectorId[32];
// --level=L — which level --test=sector should render, centered on world 0,0.
extern char StartupLevelId[32];
