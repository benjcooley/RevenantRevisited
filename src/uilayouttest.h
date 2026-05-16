// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *      uilayouttest.h - --test=ui-layout: TPane 2-pass layout proof     *
// *************************************************************************
//
// Builds a synthetic TPane tree with mixed fixed / greedy / margin / padding
// children, runs the new TPane two-pass measure->layout, and logs the
// resulting rects so the layout math is externally verifiable. Visual
// rendering is intentionally minimal -- a cycling backdrop -- because A.2b
// only adds the layout API; A.2d (9-slice) is what gives us a real way to
// draw boxed UI atoms. Once A.2d lands, this test mode grows a real visual
// render of the laid-out tree.
//
// *************************************************************************

#pragma once

bool InitializeUILayoutMode();
void RenderUILayoutMode();
void CloseUILayoutMode();
