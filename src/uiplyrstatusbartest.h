// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uiplyrstatusbartest.h - --test=ui-plyrstatusbar mockup               *
// *************************************************************************
//
// Layout mockup of the TPlyrStatusBar character panel (the iconic Revenant
// upper-left + upper-right face-off HUD element). Uses solid-rect placeholders
// at retail-derived positions extracted by Wave-3A from cls_0x5a54e4 slot 23
// (FUN_0054af20). Validates the two-pass-draw architecture in the live
// renderer before bitmap/font assets are wired through.
//
// Architecture under test (per Wave-3A):
//   - Single pane instance spanning full screen width
//   - LEFT block: player content at x=0..96 (portrait + 3 horizontal bars)
//   - RIGHT block: mirrored target content at x = panel_w - 96..panel_w
//   - Target block hidden when no target (here: animated 3s on/off cycle)
//
// *************************************************************************

#pragma once

bool InitializeUIPlyrStatusBarMode();
void RenderUIPlyrStatusBarMode();
void RenderUIPlyrStatusBarModeEmbedded();
void CloseUIPlyrStatusBarMode();
