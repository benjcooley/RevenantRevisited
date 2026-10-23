// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *      uihudmockuptest.h - --test=ui-hud-mockup: full HUD wireframe     *
// *************************************************************************
//
// Composite HUD-mockup test (burndown B.13). Constructs every visible
// retail HUD pane at its retail rect, registers one visualizer with the
// HUD pipeline that paints each pane's outline with a color-coded fill,
// and animates the status-bar gauges so the wireframe isn't fully static.
//
// Each pane class sets its rect in the default constructor (defines from
// revdefs.h). No Initialize() is called -- that would require GameData
// (playscrn.dat) and the pane's per-class data file. The mockup verifies
// the HUD layout topology, not per-pane content; content rendering is
// per-pane B-phase work.
//
// *************************************************************************

#pragma once

bool InitializeUIHudMockupMode();
void RenderUIHudMockupMode();
void CloseUIHudMockupMode();
