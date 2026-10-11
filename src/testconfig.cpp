// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 testconfig.cpp - Test startup config                  *
// *************************************************************************

#include "testconfig.h"

char  StartupTestMode[32]     = "";
char  StartupSectorId[32]     = "";
char  StartupLevelId[32]      = "";
int32_t StartupSceneCamera[4] = {0,0,0,0};
bool StartupSceneCameraSet = false;
char StartupSceneModule[64] = "";
char StartupSceneCommandFile[MAXPATHLEN] = "";
int32_t StartupSceneAmbient[4] = {0,0,0,0};
bool StartupSceneAmbientSet = false;
char  StartupAssetPath[128]   = "";
float StartupAssetScale       = 0.0f;   // 0 = auto-fit based on bbox
char  StartupDumpTilesPath[MAXPATHLEN] = "";
char  StartupDumpIconsPath[MAXPATHLEN] = "";
char  StartupDumpGltfPath[MAXPATHLEN]    = "";
bool  StartupPlayerAI                    = false;
char  StartupDumpGltfOutPath[MAXPATHLEN] = "";
char  StartupDumpI3DPath[MAXPATHLEN]    = "";
char  StartupDumpI3DOutPath[MAXPATHLEN] = "";
char  StartupVfxId[64]        = "";
bool  StartupVfxHideUi        = false;
bool  StartupVfxWireframe     = false;
int32_t StartupVfxLightingMode = -1;
bool StartupVfxNativeDomain = false;
float StartupVfxReviewSpeed = 48.0f;
float StartupVfxReviewRatio = 2.5f;
float StartupVfxReviewSpacing = 240.0f;
float StartupVfxReviewGap = 720.0f;
float StartupVfxReviewOffset = 0.0f;
char StartupVfxReviewEffects[2048] = "";
char StartupVfxReviewSpacingOverrides[2048] = "";
float StartupVfxReviewPathTilt = .25f;
float StartupVfxReviewPathLength = 200.0f;
bool StartupVfxReviewFirst = false;
bool StartupVfxReviewSourceLighting = false;
int32_t StartupPartSysQuality = 0;
int32_t StartupPartSysIncomingBlend = 0;
char  StartupCinematicPath[MAXPATHLEN] = "";
char  StartupExec[4096]             = "";
bool  StartupQuickstart                 = false;
char  StartupQuickstartSave[MAXPATHLEN] = "";
bool  StartupNoIntro                    = false;
char  StartupMenuButton[32]             = "";
char  StartupVfxBackground[16] = "";
char  StartupVfxBackdrop[MAXPATHLEN] = "";
float StartupVfxCamera[2] = {-1.0f, -1.0f};
int32_t StartupVfxOrigin[3] = {0, 0, 0};
bool StartupVfxOriginSet = false;
std::string StartupInputScript;
std::string StartupAbCase;
std::string StartupAbOut;
