// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           cursor_os.h - Platform-native cursor pixel handoff          *
// *************************************************************************
//
// Push our game cursor pixels into the OS pointer so the OS handles all
// the visibility plumbing (mouse-leaves-window, window-deactivates, ...)
// instead of us toggling sapp_show_mouse + drawing the cursor ourselves.
//
// macOS implementation lives in src/platform/osx/cursor_macos.mm.
// Non-Apple builds get no-op stubs (we don't run there yet).

#pragma once

#include <cstdint>

class TBitmap;

namespace rev_platform
{

// Convert the given game bitmap to RGBA8, hand it to the OS as the
// current pointer cursor. hot_x/hot_y are the "registration point" --
// the pixel-precise spot the OS should treat as the cursor position
// (typically the wedge tip / hand fingertip / etc.). Safe to call
// every cursor-bitmap change; cheap enough that per-bitmap is fine.
// Returns false if the platform layer isn't available or the bitmap
// failed to decode.
bool SetOSCursor(const TBitmap *bm, int32_t hot_x, int32_t hot_y);

// Restore the platform default arrow cursor. Called when the game
// hands control back to the OS (mode change, ImGui captures mouse).
void ResetOSCursor();

} // namespace rev_platform
