// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *     uianchortest.h - --test=ui-anchors: TPane anchor positioning      *
// *************************************************************************
//
// Builds a None-layout root with 5 anchored children (4 corners + center)
// at fixed sizes, runs RunLayoutPass at retail 640x480, then resizes the
// root to Revisited 1920x1080 and re-runs, logging the resulting rects
// each time. Verifies anchors track the parent's content rect when the
// canvas changes -- the core mechanism behind Revisited 1920x1080 HUD
// layout (project-resolution-modes).
//
// *************************************************************************

#pragma once

bool InitializeUIAnchorMode();
void RenderUIAnchorMode();
void CloseUIAnchorMode();
