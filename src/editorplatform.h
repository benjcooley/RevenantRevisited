// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           editorplatform.h - Editor platform-specific window shim     *
// *************************************************************************
//
// Thin abstraction over OS window manipulation. The platform impl lives
// in editorplatform.mm (macOS / Cocoa). Other platforms can grow their
// own .cpp / .mm later; the no-op fallback inside .mm just leaves the
// window size unchanged.
//
// *************************************************************************

#pragma once

#include <cstdint>

namespace EditorPlatform {

// Read current window content size (pixels). Falls back to sapp_width /
// sapp_height when no platform shim is available.
void GetWindowSize(int32_t& w, int32_t& h);

// Read current window origin in screen coords (where the OS reports it).
void GetWindowOrigin(int32_t& x, int32_t& y);

// Resize the window's *content* rect to (w, h). Origin is the bottom-left
// of the content rect (Cocoa convention on macOS).
void SetWindowFrame(int32_t origin_x, int32_t origin_y, int32_t w, int32_t h);

// Resize the window's content rect to (w, h) keeping the centre of the
// window pinned to where it was. Useful for editor enter/exit so the
// game view stays under the same screen pixels.
void SetWindowSizeCenteredOnCurrent(int32_t w, int32_t h);

} // namespace EditorPlatform
