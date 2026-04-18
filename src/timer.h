// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                       timer.h - Timer objects                         *
// *************************************************************************

#pragma once

#include "revenant.h"

// Frame-pacing under sokol_app is driven by the host run loop, so TTimer is a
// thin counter/no-op in the port. TODO(port): Subsystem 4 — threading; a worker
// pool could resurrect callback-based ticks if needed later.

class TTimer
{
    static uint32_t clockticks;

  public:
    TTimer();
    ~TTimer();

    bool Initialize();
    void Close();

    int32_t ClockTicks() { return clockticks; }
    void WaitForTick();
    void ResetTick();
};
