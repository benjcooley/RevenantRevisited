// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   mainwnd.h - Main Window Object                      *
// *************************************************************************

#pragma once

#include "revenant.h"

_CLASSDEF(TMainWindow)

class TMainWindow
{
  public:
  // Main Window hwnd handle
    HWND          hwnd;

    TMainWindow() { hwnd = nullptr; }
    virtual ~TMainWindow();

    bool Initialize(HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpCmdLine, int32_t nCmdShow);
      // Create window
    void Close();
      // Destroy window
    HWND Hwnd() {return hwnd;}
};

static LRESULT CALLBACK MainWndProc(HWND hWnd, uint32_t wMessage, WPARAM wParam, LPARAM lParam);

