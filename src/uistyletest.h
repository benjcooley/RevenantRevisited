// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *       uistyletest.h - --test=ui-style: UIStyle push/pop stack         *
// *************************************************************************
//
// Verifies the UIStyle stack: base default at bottom; PushStyle copies
// and exposes the new active state; PopStyle returns to the previous
// state; balanced push/pop ends back at the base.
//
// *************************************************************************

#pragma once

bool InitializeUIStyleMode();
void RenderUIStyleMode();
void CloseUIStyleMode();
