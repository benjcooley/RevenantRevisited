// *************************************************************************
// *                         Cinematix Revenant                            *
// *                                                                        *
// *   time.cpp — TTime implementation.                                     *
// *************************************************************************

#include "time.h"

#include <cmath>

double  TTime::m_time             = 0.0;
double  TTime::m_deltaTime        = 0.0;
int64_t TTime::m_frameCount       = 0;
int64_t TTime::m_legacyFrameCount = 0;

void TTime::BeginFrame(double dt_seconds)
{
    if (dt_seconds < 0.0)
        dt_seconds = 0.0;
    // Clamp to a sane ceiling so a sleep/wake, debugger pause, or other
    // long stall looks like one dropped frame rather than a multi-minute
    // jump that dt-scaled consumers try to integrate through.
    if (dt_seconds > 0.1)
        dt_seconds = 0.1;

    m_deltaTime = dt_seconds;
    m_time     += dt_seconds;
    m_frameCount++;

    // Advance toward floor(time * 24), but cap at real frame count so each
    // legacy frame is backed by at least one real Pulse. When the renderer
    // runs slower than 24 Hz, the legacy counter slows with it — the game
    // stretches rather than skipping ticks.
    int64_t target = static_cast<int64_t>(m_time * LegacyFramerate);
    if (target > m_frameCount)
        target = m_frameCount;
    if (target > m_legacyFrameCount)
        m_legacyFrameCount = target;
}
