// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *           uicliptest.h - --test=ui-clip: TPane clip-rect API          *
// *************************************************************************
//
// API verification for TPane::SetClipRect / ClearClipRect / HasClipRect /
// GetClipRect. Renderer-side scissoring is a TODO -- this test only
// exercises the TPane storage / setter / clearer contract. Visual clip
// verification arrives when the first B-phase pane wires the renderer
// scissor through DrawHud.
//
// *************************************************************************

#pragma once

bool InitializeUIClipMode();
void RenderUIClipMode();
void CloseUIClipMode();
