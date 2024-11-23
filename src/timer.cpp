// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   timer.cpp - Timer oject module                      *  
// *************************************************************************

#include <windows.h>
#include <mmsystem.h>

#include "revenant.h"
#include "screen.h"
#include "timer.h"

uint32_t TTimer::clockticks;
uint32_t period = (uint32_t)(1000 / FRAMERATE);
HANDLE tickevent;

void WINAPI TTimer::TimerCallback(uint32_t wTimerID, uint32_t msg, uint32_t dwUser, uint32_t dw1, uint32_t dw2)
{
    clockticks++;
    TickOccured = true;
    PulseEvent(tickevent);
}

TTimer::TTimer()
{
    clockticks = 0;
    timerID = nullptr;
}

TTimer::~TTimer()
{
    if (timerID) 
    {
        timeEndPeriod(period);
        uint32_t Result = timeKillEvent(timerID);
    }
}

bool TTimer::Initialize()
{
    TIMECAPS tc;
    uint32_t period = 1000 / FRAMERATE;

    tickevent = CreateEvent(nullptr, false, false, nullptr);

    if (timerID) return true;

    if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR)
        return false;

    period = min(max(tc.wPeriodMin, period), tc.wPeriodMax);

    timeBeginPeriod(period);

    timerID = timeSetEvent(period, RESOLUTION, TimerCallback, nullptr, TIME_PERIODIC);

    if (!timerID) return false;

    return true;
}

void TTimer::Close()
{
    if (timerID == nullptr) 
        return;
    
    timeEndPeriod(period);
    timeKillEvent(timerID);
    timerID = nullptr;
    CloseHandle(tickevent);
}
  
void TTimer::WaitForTick()
{
    if (TickOccured)
        return;

    WaitForSingleObject(tickevent, INFINITE);
}

void TTimer::ResetTick()
{
    TickOccured = false;
}
