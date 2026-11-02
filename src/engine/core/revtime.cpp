// *************************************************************************
// *                         Cinematix Revenant                            *
// *                                                                        *
// *   time.cpp — TTime implementation.                                     *
// *************************************************************************

#include "engine/core/revtime.h"

#include <chrono>

#include <cmath>

double  TTime::m_time                = 0.0;
double  TTime::m_deltaTime           = 0.0;
double  TTime::m_realDeltaTime       = 0.0;
double  TTime::m_legacyFrameFraction = 0.0;
double  TTime::m_timeScale           = 1.0;
int64_t TTime::m_frameCount          = 0;
int64_t TTime::m_legacyFrameCount    = 0;

void TTime::SetTimeScale(double scale)
{
    if (scale < 0.0)
        scale = 0.0;
    if (scale > 4.0)
        scale = 4.0;
    m_timeScale = scale;
}

void TTime::BeginFixedFrame()
{
    m_realDeltaTime = LegacyFrameSeconds;
    m_deltaTime = LegacyFrameSeconds;
    m_frameCount++;
    m_legacyFrameCount++;
    m_time = static_cast<double>(m_legacyFrameCount) * LegacyFrameSeconds;
    m_legacyFrameFraction = 0.0;
}

void TTime::BeginFrame(double dt_seconds)
{
    if (dt_seconds < 0.0)
        dt_seconds = 0.0;
    // Clamp to a sane ceiling so a sleep/wake, debugger pause, or other
    // long stall looks like one dropped frame rather than a multi-minute
    // jump that dt-scaled consumers try to integrate through.
    if (dt_seconds > 0.1)
        dt_seconds = 0.1;

    m_realDeltaTime = dt_seconds;
    m_deltaTime = dt_seconds * m_timeScale;
    m_time     += m_deltaTime;
    m_frameCount++;

    // Advance toward floor(scaled_time * 24). At slow scales this yields many
    // render frames between legacy ticks; at fast scales TScreen may process
    // multiple fixed legacy ticks before drawing.
    const double legacy_time = m_time * LegacyFramerate;
    int64_t target = static_cast<int64_t>(std::floor(legacy_time));
    double fraction = legacy_time - static_cast<double>(target);
    m_legacyFrameFraction = fraction;
    if (target > m_legacyFrameCount)
        m_legacyFrameCount = target;
}

// Returns system ticks in milliseconds since system was turned on
uint32_t tickcount()
{
    using namespace std::chrono;
    static const auto epoch = steady_clock::now();
    return static_cast<uint32_t>(
        duration_cast<milliseconds>(steady_clock::now() - epoch).count());
}
