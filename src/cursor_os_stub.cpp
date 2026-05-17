// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *      cursor_os_stub.cpp - No-op cursor backend for non-Apple builds   *
// *************************************************************************
//
// Non-Apple platforms keep the old "sapp_show_mouse + in-game cursor
// draw" path; the OS-cursor handoff is a macOS-only optimization for
// now. This stub lets cursor_os.h compile without #ifdef pollution at
// call sites.

#if !defined(__APPLE__)

#include "cursor_os.h"

namespace rev_platform
{
bool SetOSCursor(const TBitmap * /*bm*/, int32_t /*hot_x*/, int32_t /*hot_y*/) { return false; }
void ResetOSCursor() {}
} // namespace rev_platform

#endif
