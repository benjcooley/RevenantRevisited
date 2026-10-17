// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   editorplatform_stub.cpp - editor window shim without an OS hook     *
// *************************************************************************
//
// Only macOS resizes the window for the editor so far
// (platform/macosx/editorplatform.mm). Elsewhere the window keeps its size
// and the editor lays its panels out inside it.

#if !defined(__APPLE__)

#include "../editorplatform.h"

#include <sokol_app.h>

namespace EditorPlatform
{

void GetWindowSize(int32_t& w, int32_t& h)
{
    w = sapp_width();
    h = sapp_height();
}

void GetWindowOrigin(int32_t& x, int32_t& y)
{
    x = 0;
    y = 0;
}

void SetWindowFrame(int32_t /*origin_x*/, int32_t /*origin_y*/, int32_t /*w*/, int32_t /*h*/) {}

void SetWindowSizeCenteredOnCurrent(int32_t /*w*/, int32_t /*h*/) {}

} // namespace EditorPlatform

#endif
