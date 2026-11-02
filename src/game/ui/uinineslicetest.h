// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *     uinineslicetest.h - --test=ui-nineslice: 9-slice rect math       *
// *************************************************************************
//
// Verification mode for TRenderer::DrawNineSlice. The render-side blits
// happen during a normal frame -- visual verification arrives when the
// first B-phase pane wires a real widget atlas. For now this mode logs
// the computed source / dest rect breakdown for several target sizes from
// one synthetic source (32x32, insets {8,8,8,8}) so the slicing math is
// externally auditable.
//
// *************************************************************************

#pragma once

bool InitializeUINineSliceMode();
void RenderUINineSliceMode();
void CloseUINineSliceMode();
