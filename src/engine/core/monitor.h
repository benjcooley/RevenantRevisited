// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                    monitor.h  - Monitor globals                       *
// *************************************************************************

#pragma once

#include "multimon.h"

// Multi-Monitor Variables
extern int32_t MonitorNum;          // Monitor game will run on (default is 1, primary)
extern int32_t MonitorX, MonitorY;  // Relative position of monitor in desktop coordinates
extern int32_t MonitorW, MonitorH;  // Width and height of monitor (before changing video modes)
extern HMONITOR Monitor;        // Windows monitor handle
extern MONITORINFOEX MonitorInfo; // Windows monitor info structure
                                   // i.e. use if string is "permidia" and driver desc is "Glint Permidia 2 3D"
