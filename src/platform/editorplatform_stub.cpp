// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   editorplatform_stub.cpp - Non-macOS EditorPlatform fallback         *
// *************************************************************************
//
// sokol_app.h exposes no window-resize helper, and only the macOS backend
// (editorplatform.mm) currently pokes the native NSWindow. On Windows /
// Linux the editor just lays its panels out inside the existing window:
// GetWindowSize reports the sokol drawable size and the setters are no-ops.
// Mirrors the `#else` fallbacks in platform/macosx/editorplatform.mm.
//
// Guarded so the macOS build (which compiles the .mm) doesn't get a second
// definition -- the *.cpp glob picks this file up on every platform.
// *************************************************************************

#if !defined(__APPLE__)

#include "../editorplatform.h"

#include <sokol_app.h>

namespace EditorPlatform {

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

void SetWindowFrame(int32_t /*origin_x*/, int32_t /*origin_y*/,
                    int32_t /*w*/, int32_t /*h*/)
{
}

void SetWindowSizeCenteredOnCurrent(int32_t /*w*/, int32_t /*h*/)
{
}

} // namespace EditorPlatform

#endif // !__APPLE__
