// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  MainWnd.cpp - Main Window Module                     *
// *                                                                       *
// *  Ported (2026): sokol_app replaces the Win32 message pump. Input      *
// *  event translation lives in revmain.cpp's sokol event_cb. This file   *
// *  now just owns the TMainWindow lifecycle flag. The pre-port Win32     *
// *  implementation (RegisterClass, CreateWindowEx, WNDPROC) is in        *
// *  attic/src/mainwnd_win32.cpp for reference.                           *
// *************************************************************************

#include "mainwnd.h"

bool TMainWindow::Initialize()
{
    live = true;
    return true;
}

void TMainWindow::Close()
{
    live = false;
}
