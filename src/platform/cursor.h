// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *      cursor.h - Platform-native cursor pixel handoff (facade)         *
// *************************************************************************
//
// Push our game cursor pixels into the OS pointer so the OS handles all
// the visibility plumbing (mouse-leaves-window, window-deactivates, ...)
// instead of us toggling sapp_show_mouse + drawing the cursor ourselves.
//
// Per-OS implementations live under sibling folders:
//   src/platform/macosx/cursor.mm      (NSCursor)
//   src/platform/windows/cursor.cpp    (SetCursor + HCURSOR)        -- TODO
//   src/platform/linux/cursor.cpp      (XcursorImage + XDefineCursor) -- TODO
// src/platform/cursor_stub.cpp provides a no-op for builds that don't
// link any of the above.

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

// Re-apply the most-recently-Set OS cursor. macOS does NOT keep an
// [NSCursor set] call sticky across tracking-area / view-boundary /
// title-bar crossings -- AppKit auto-pops back to the default whenever
// the cursor enters a region that didn't install a cursor rect, which
// covers a lot of accidental terrain. Calling this every tick while
// the game owns the mouse is the simplest robust fix: it's a single
// [cachedCursor set] (no alloc), defeats the auto-revert, and is
// cheap enough at 60 Hz. No-op when SetOSCursor hasn't been called
// yet or on platforms without the macOS backend.
void ReassertOSCursor();

} // namespace rev_platform
