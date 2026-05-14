// *************************************************************************
// *                         Cinematix Revenant                            *
// *                                                                        *
// *   time.h — delta-time and frame counters for the port.                *
// *                                                                        *
// *   Retail Revenant ran on a fixed 24fps timer; every Pulse/Animate     *
// *   tick assumed one "frame" had passed. This port drives at whatever   *
// *   rate sokol_app delivers, so the engine reads scaled delta time from *
// *   TTime and legacy frame-counter code reads LegacyFrameCount().       *
// *                                                                        *
// *   TimeScale follows the Unity-style model: gameplay, legacy ticks,    *
// *   and render interpolation advance on scaled simulation time, while   *
// *   frame profiling can still read unscaled real delta time.             *
// *************************************************************************

#pragma once

#include <cstdint>

class TTime
{
  public:
    TTime() = delete;

    // Scaled simulation seconds since the first BeginFrame call.
    [[nodiscard]] static double   Time()              { return m_time; }

    // Scaled seconds between the start of the previous frame and this one.
    // Primary driver for gameplay/simulation time-based code.
    [[nodiscard]] static double   DeltaTime()         { return m_deltaTime; }

    // Unscaled render-frame delta for profiling and UI. This is deliberately
    // not affected by debug slow motion.
    [[nodiscard]] static double   RealDeltaTime()     { return m_realDeltaTime; }

    // Real frames rendered since app init. Increments by 1 each BeginFrame.
    [[nodiscard]] static int64_t  FrameCount()        { return m_frameCount; }

    // 24fps-equivalent frames elapsed in scaled simulation time. Speeds above
    // 1.0 may make the screen process multiple legacy ticks in one render.
    [[nodiscard]] static int64_t  LegacyFrameCount()  { return m_legacyFrameCount; }
    [[nodiscard]] static double   LegacyFrameFraction() { return m_legacyFrameFraction; }
        // Fractional progress from the current legacy frame toward the next.
        // Render-only interpolation can use this without advancing game logic.

    // Retail target framerate.
    static constexpr int    LegacyFramerate    = 24;
    static constexpr double LegacyFrameSeconds = 1.0 / LegacyFramerate;

    // Debug simulation scale. 1.0 is real-time; 0.5/0.25/0.125 are useful for
    // inspecting animation interpolation without changing authored framerates.
    [[nodiscard]] static double TimeScale() { return m_timeScale; }
    static void SetTimeScale(double scale);

    // Call once at the top of each render frame.
    static void BeginFrame(double dt_seconds);

  private:
    static double  m_time;
    static double  m_deltaTime;
    static double  m_realDeltaTime;
    static double  m_legacyFrameFraction;
    static double  m_timeScale;
    static int64_t m_frameCount;
    static int64_t m_legacyFrameCount;
};
