// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *      uitextbartest.h - --test=ui-textbar: TTextBar data API           *
// *************************************************************************
//
// First-pane bring-up test mode (burndown B.1). Exercises TTextBar's
// public data API (Print, Clear, SetHealthDisplay, ClearHealthDisplay)
// with synthesized inputs and logs state at each step. The pane's
// DrawBackground still uses the old Display CPU-surface API and depends
// on GameData (playscrn.dat) which isn't loaded in test mode -- the
// render port is incremental (see TODO in DrawBackground); this commit
// validates the data-model contract so we can wire it confidently when
// the new HUD render path is ready.
//
// *************************************************************************

#pragma once

bool InitializeUITextBarMode();
void RenderUITextBarMode();
void RenderUITextBarModeEmbedded();
void CloseUITextBarMode();
