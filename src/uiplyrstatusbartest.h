// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uiplyrstatusbartest.h - --test=ui-plyrstatusbar: TPlyrStatusBar      *
// *************************************************************************
//
// Hosts the production status bar (TPlyrStatusBar, statusbar.h) on the test
// screen over a flat backdrop. The pane reads the demo player that the test
// modes install (uidemoplayer.h); the demo engages and releases its opponent
// on a cycle, so the target chip fades in and out and its bars drain.
//
// With no play screen HUD up, the map view is the whole display. The
// player's chip sits at the top left and the opponent's at the top right.
//
// --test=ab-plyrstatusbar is one case of the status bar's retail A/B
// (tools/retail_ab/hud_ab.py statusbar). It takes the case from
// --ab-case="player=H,M,F;target=H,M,F|none;ticks=N": the demo player's and
// opponent's health, mana and fatigue as fractions of their maxima, whether
// they fight, and how many ticks the pane runs. On the first frame at a tick
// boundary (run it with --snapstep=0.03125: every fourth frame is one) the
// host stops the clock and runs the pane, so the frame shows the chips
// exactly as that tick leaves them, over black. --snap=<png> writes the
// frame (give it --snapwarmup=12). --ab-out=<json> writes what the chips
// show (names, levels, stats and the portraits' TBitmap bytes), which the
// A/B gives to retail's pane.
//
// *************************************************************************

#pragma once

bool InitializeUIPlyrStatusBarMode();
void RenderUIPlyrStatusBarMode();
void CloseUIPlyrStatusBarMode();

bool InitializeABPlyrStatusBarMode();
void RenderABPlyrStatusBarMode();
void CloseABPlyrStatusBarMode();
