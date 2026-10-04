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
char  StartupDumpIconsPath[MAXPATHLEN] = "";
char  StartupDumpGltfPath[MAXPATHLEN]    = "";
char  StartupDumpGltfOutPath[MAXPATHLEN] = "";
char  StartupDumpI3DPath[MAXPATHLEN]    = "";
char  StartupDumpI3DOutPath[MAXPATHLEN] = "";
char  StartupVfxId[64]        = "";
bool  StartupVfxHideUi        = false;
bool  StartupVfxWireframe     = false;
char  StartupCinematicPath[MAXPATHLEN] = "";
char  StartupVfxBackground[16] = "";
char  StartupInputScript[1024] = "";
