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
// --cinematic=<path> — which .SMK file --test=ui-cinematic should play.
// Empty = default intro FMV (data/Disk2/MIX_FMV1.SMK).
extern char StartupCinematicPath[MAXPATHLEN];


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
extern char StartupInputScript[1024];
