// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  testconfig.h - Test startup config                   *
// *************************************************************************

#pragma once

#include "revenant.h"

#include <string>

extern char StartupTestMode[32];
// --sector=L_X_Y — which sector --test=sector should render. Empty = default.
extern char StartupSectorId[32];
// --level=L — which level --test=sector should render, centered on world 0,0.
extern char StartupLevelId[32];
// --scene-camera=L,X,Y,Z --scene-module=name: reproducible real-map VFX
// captures through --test=sector, in the retail editor's 640x340 viewport.
extern int32_t StartupSceneCamera[4];
extern bool StartupSceneCameraSet;
extern char StartupSceneModule[64];
// Optional sector-only timeline of real engine commands, in legacy 24Hz ticks.
extern char StartupSceneCommandFile[MAXPATHLEN];
// Optional authored scene lighting override: intensity,R,G,B.
extern int32_t StartupSceneAmbient[4];
extern bool StartupSceneAmbientSet;
// --asset=path — which imagery --test=i3d3d should load (default Misc\\Blood.I3D).
extern char StartupAssetPath[128];
// --scale=f — uniform mesh scale multiplier for --test=i3d3d.
extern float StartupAssetScale;
// --dumptiles=<path> — export tile albedo PNGs into the given folder, then exit.
extern char StartupDumpTilesPath[MAXPATHLEN];
// --dumpicons=<path> — export every baked inventory icon / portrait
// (GetInvImage, all classes/types/states) as PNGs into the folder, then exit.
extern char StartupDumpIconsPath[MAXPATHLEN];
// --dumpgltf=<asset|@list> [--dumpgltfout=<dir|file.glb>] — export I3D
// asset(s) as Blender-loadable glTF 2.0 binaries (.glb) with per-state
// animations, then exit.
extern char StartupDumpGltfPath[MAXPATHLEN];
extern char StartupDumpGltfOutPath[MAXPATHLEN];
// --dumpi3d=<asset> [--dumpi3dout=<dir>] — extract textures (PNG) +
// sub-objects (Wavefront OBJ) + manifest.txt from one I3D file.
// Default output: ./i3d_dump/<asset-basename>/. Triggers --test=i3ddump.
extern char StartupDumpI3DPath[MAXPATHLEN];
extern char StartupDumpI3DOutPath[MAXPATHLEN];
// --vfx=<id> — pre-select an effect by id in --test=vfx (e.g.
// --vfx=TStripEffect). Empty = first alphabetically-sorted entry.
// Useful for clean screencaps + scripted iteration.
extern char StartupVfxId[64];
// --vfx-no-ui — suppress the ImGui VFX Browser panel so screencaps
// show only the effect render.
extern bool StartupVfxHideUi;
// --vfx-wireframe — render every FX submit as a thin quad outline
// instead of filled textured. Useful for verifying rotation,
// projection, and screen-space distortion independently of the
// authored texture content.
extern bool StartupVfxWireframe;
// --vfx-lighting-mode=auto|classic|modern — VFX-only render diagnostic.
// Auto (-1) preserves explicit source ambient -> classic, otherwise modern.
extern int32_t StartupVfxLightingMode;
// --partsys-quality=0|1|2 — match retail controller quality during captures.
// 0 preserves full emission; retail 1 uses one quarter, 2 uses one half.
extern int32_t StartupPartSysQuality;
// --partsys-incoming-blend=auto|16|80: exact Imight/Fmastery capture input before first
// particle submission. Auto(0) keeps visible ordinary compatibility; not a
// guessed retail scene state. Does not change other profiles/general rendering.
extern int32_t StartupPartSysIncomingBlend;
// --cinematic=<path> — which .SMK file --test=ui-cinematic should play.
// Empty = default intro FMV (data/Disk2/MIX_FMV1.SMK).
extern char StartupCinematicPath[MAXPATHLEN];
// --exec="cmd; cmd; sleep N; ..." — console commands run in the live
// PlayScreen once a player exists (see consoleexec.h).
extern char StartupExec[4096];
// Boot options (TGameFlow::Boot): --quickstart[=<save>] is retail QUICKSTART
// (no intro / title; new game or load <save>); --nointro skips the intro movie;
// --menu=<button> presses a title button automatically
// (newgame|loadgame|multi|options|exit).
extern bool StartupQuickstart;
extern char StartupQuickstartSave[MAXPATHLEN];
extern bool StartupNoIntro;
extern char StartupMenuButton[32];


// --vfx-bg=<black|ltgray|forest|dungeon> — pre-select the diagnostic
// backdrop for --test=vfx (default ltgray). Empty / unknown = default.
extern char StartupVfxBackground[16];
// Retail comparison controls: optional baked backdrop, authored camera origin
// in screen pixels, and an explicit world-space effect origin (no auto-fit).
extern char StartupVfxBackdrop[MAXPATHLEN];
extern float StartupVfxCamera[2];
extern int32_t StartupVfxOrigin[3];
extern bool StartupVfxOriginSet;

// --input-script="..." (alias: --mouse-script) — a scripted synthetic input
// sequence (mouse + keyboard) replayed into the active --test mode's input
// dispatch, for driving + verifying UI behavior headlessly (no real
// mouse/keyboard needed). Semicolon-separated commands:
//   moveto X Y [MS]         — glide the synthetic cursor to (X,Y) [Classic
//     | move X Y [MS]          640x480 content pixels] over MS milliseconds
//                             (default 500; MS=0 jumps instantly). The glide
//                             emits ~60 Hz intermediate moves, so hover passes
//                             over any widgets along the path.
//   pause MS | wait MS      — advance the timeline by MS milliseconds
//   left_down | right_down  — emit a mouse button-down at the current cursor
//     | middle_down            pos. A press is HELD across later moves, so
//                             left_down; moveto …; left_up expresses a drag.
//   left_up | right_up |    — emit the matching button-up at the current pos.
//     middle_up
//   key_down KEY            — emit a keyboard key-down for KEY: a single
//   key_up KEY                 char (a, 5, …), a named key (esc/escape, enter/
//                             return, tab, space, up/down/left/right, f1..f12,
//                             del, home, end, pgup, pgdn, backspace), or a
//                             numeric VK code.
//   type TEXT               — type TEXT (the rest of the command, spaces
//                             kept): one character event per character, 30 ms
//                             apart, as the platform sends for printable keys
//                             (what an EDIT field takes).
//   loop                    — restart the script from the top when it ends
//   take_snapshot [LABEL]   — capture one manual-filmstrip frame (only when
//   | snapshot [LABEL]        --filmstrip=N,0 manual mode is active). Put a
//                             short `pause` before it so the just-driven state
//                             has reached the backbuffer. Optional LABEL
//                             (free-form text, joined to end of line) is
//                             baked into the per-frame PNG filename (kebab-
//                             cased) AND drawn as a black-bar overlay on the
//                             scaled cell in the composite filmstrip.
//   log TEXT                — emit a log line (handy to mark capture points)
// Example (per-item move→hover→press→release→snap):
//   --filmstrip=6,0 --input-script="moveto 435 161; pause 300; take_snapshot;
//     left_down; pause 150; take_snapshot; left_up; pause 300; ..."
extern std::string StartupInputScript;      // any length
