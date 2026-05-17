// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *      cursor_stub.cpp - No-op cursor backend for non-macOS builds      *
// *************************************************************************
//
// Non-Apple builds keep the old "sapp_show_mouse + in-game cursor draw"
// path; the OS-cursor handoff is a macOS-only optimization until
// platform/windows/ and platform/linux/ are filled in. This stub lets
// platform/cursor.h compile without #ifdef pollution at call sites.

#if !defined(__APPLE__)

#include "cursor.h"

namespace rev_platform
{
bool SetOSCursor(const TBitmap * /*bm*/, int32_t /*hot_x*/, int32_t /*hot_y*/) { return false; }
void ResetOSCursor() {}
void ReassertOSCursor() {}
} // namespace rev_platform

#endif
