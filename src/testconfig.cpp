// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 testconfig.cpp - Test startup config                  *
// *************************************************************************

#include "testconfig.h"

char  StartupTestMode[32]     = "";
char  StartupSectorId[32]     = "";
char  StartupLevelId[32]      = "";
char  StartupAssetPath[128]   = "";
float StartupAssetScale       = 0.0f;   // 0 = auto-fit based on bbox
char  StartupDumpTilesPath[MAXPATHLEN] = "";
char  StartupVfxId[64]        = "";
bool  StartupVfxHideUi        = false;
