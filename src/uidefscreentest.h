// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uidefscreentest.h - --test=ui-ingamemenu / ui-options / ui-savegame   *
// *                      / ui-loadgame  (DEF widget engine harness)        *
// *************************************************************************
//
// Drives the data-driven DEF widget engine (TDefPane) for one DEF screen in
// isolation, composing it to a render target and blitting over a dim backdrop.
// One entry point serves every DEF screen; the mode string selects the panel.

#pragma once

#include <cstdint>

// Returns false if the mode is not a known DEF-screen mode (so the dispatcher
// can fall through). `mode` is the --test= string (e.g. "ui-ingamemenu").
bool IsUIDefScreenMode(const char* mode);

bool InitializeUIDefScreenMode(const char* mode);
void RenderUIDefScreenMode();
void CloseUIDefScreenMode();

// --test=ab-options: one case of the Options pane's retail A/B
// (tools/retail_ab/hud_ab.py options), from --ab-case, fields split by ';':
//   settings=Name:V,...   the setting globals the pane copies on opening, by
//                         its control names (Realtime, Auto, Dialog, Face,
//                         Enhanced, Limit, NoCombatRes, Violence, Music,
//                         Sound, Gamma)
//   from_game=0|1         the in-game menu's modal (DEF flags 0x11) or the
//                         title's Options screen
//   input=down:X,Y|up:X,Y|move:X,Y|...   pointer input, pane-local, in order
// On the first frame at a tick boundary (run it with --snapstep=0.03125) the
// host stops the clock and composes the pane once over black, recording its
// text. --snap=<png> writes the frame (give it --snapwarmup=12);
// --ab-out=<json> writes the settings the pane holds and its text draws.
bool InitializeABOptionsMode();
void RenderABOptionsMode();
void CloseABOptionsMode();
