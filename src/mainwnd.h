// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   mainwnd.h - Main Window Object                      *
// *                                                                       *
// *  Ported (2026): sokol_app replaces the Win32 HWND + WNDPROC message   *
// *  pump. The class is retained for the rest of the engine's sake --     *
// *  callers still say `MainWindow.Close()` etc. -- but the body no       *
// *  longer tracks an HWND. Sokol owns the window; we just reflect        *
// *  lifecycle state the engine cares about. The pre-port Win32 version   *
// *  is in attic/src/mainwnd_win32.h/.cpp.                                *
// *************************************************************************

#pragma once

#include <cstdint>

class TMainWindow
{
  public:
    TMainWindow() = default;
    ~TMainWindow() = default;

  // Initialize is called from sokol_app's init callback once the window
  // and rendering context are live. Returns true on success.
    bool Initialize();

  // Close is called from cleanup (or FatalError) to tear down window-
  // owned resources. With sokol_app owning the window there is nothing
  // to destroy here, but the engine calls it in several places so the
  // method must remain.
    void Close();

  // True once Initialize() has been called and sokol_app has given us
  // a live window/context, false after Close() or before init.
    bool IsLive() const { return live; }

  private:
    bool live = false;
};
