// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uiplyrstatusbartest.h - --test=ui-plyrstatusbar mockup               *
// *************************************************************************
//
// TPlyrStatusBar, the upper-left + upper-right face-off HUD element, as
// reconstructed from cls_0x5a54e4 slot 23 (FUN_0054af20):
//   - Single pane instance spanning the playfield width
//   - LEFT block: the main player (portrait + 3 horizontal bars)
//   - RIGHT block: mirrored, the character the player is fighting; fades in
//     and out as the target comes and goes
//
// *************************************************************************

#pragma once

bool InitializeUIPlyrStatusBarMode();
void RenderUIPlyrStatusBarMode();
void RenderUIPlyrStatusBarModeEmbedded();
void CloseUIPlyrStatusBarMode();
