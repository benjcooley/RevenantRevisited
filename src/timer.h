// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                       timer.h - Timer objects                         *
// *************************************************************************

#pragma once

#include <windows.h>
#include <mmsystem.h>

#include "revenant.h"

// ***************************************
// * TTimer Class Varible/Function List *
// ***************************************

_CLASSDEF(TTimer)

class TTimer
{
  static uint32_t    clockticks;   // Timer clock tick count
  MMRESULT        timerID;      // Stores timerID 
  
  // Public TBitmap Functions
  public:
    TTimer();
    ~TTimer();

    bool Initialize();
      // Initializes Timer
    void Close();
      // Releases Timer

    int32_t  ClockTicks() {return clockticks;}
      // Return current clocktick count
    static void PASCAL TimerCallback(uint32_t wTimerID, uint32_t msg, uint32_t dwUser, uint32_t dw1, uint32_t dw2);
      // Timer CallBack Routine
    void WaitForTick();
      // Checks if tick has occured, if not, waits for one
    void ResetTick();
      // Resets the tick occured flag so we can wait for another tick.  WaitForTick will
      // immediately return until this is called
};

