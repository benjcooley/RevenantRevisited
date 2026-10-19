// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uistatusbartest.h - --test=ui-statusbars: THealthBar/TStaminaBar API *
// *************************************************************************
//
// Bring-up test for the two status-bar panes (burndown B.2). Verifies the
// shared TStatusBar data API (SetLevel / ChangeLevel / GetLevel) plus
// each subclass's GetHue() across the level range. Logs whether the
// data-side tubedata (health.dat / stamina.dat) loads successfully via
// LoadMulti through the overlay-aware resource path. Visual DrawBackground
// is a follow-up alongside the TTextBar visual port.
//
// *************************************************************************

#pragma once

bool InitializeUIStatusBarMode();
void RenderUIStatusBarMode();
void CloseUIStatusBarMode();
