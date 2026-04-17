// *************************************************************************
// *                         Cinematix Revenant                            *
// *                                                                        *
// *   time.h — delta-time and frame counters for the port.                *
// *                                                                        *
// *   Retail Revenant ran on a fixed 24fps timer; every Pulse/Animate     *
// *   tick assumed one "frame" had passed. This port drives at whatever   *
// *   rate sokol_app delivers, so the engine reads real delta time from  *
// *   TTime and legacy frame-counter code reads LegacyFrameCount().       *
// *                                                                        *
// *   Invariant: LegacyFrameCount() <= FrameCount(). Every 24fps-         *
// *   equivalent tick is guaranteed at least one real Pulse — if the      *
// *   render rate dips below 24Hz the game slows rather than skipping.    *
// *************************************************************************

#pragma once

#include <cstdint>

class TTime
{
  public:
    TTime() = delete;

    // Real wall-clock seconds since the first BeginFrame call.
    [[nodiscard]] static double   Time()              { return m_time; }

    // Seconds between the start of the previous frame and this one.
    // Primary driver for anything time-based.
    [[nodiscard]] static double   DeltaTime()         { return m_deltaTime; }

    // Real frames rendered since app init. Increments by 1 each BeginFrame.
    [[nodiscard]] static int64_t  FrameCount()        { return m_frameCount; }

    // 24fps-equivalent frames elapsed. Advances at LegacyFramerate per second
    // of real time, clamped so it never runs ahead of FrameCount — each
    // legacy frame is guaranteed at least one real Pulse.
    [[nodiscard]] static int64_t  LegacyFrameCount()  { return m_legacyFrameCount; }

    // Retail target framerate.
    static constexpr int    LegacyFramerate    = 24;
    static constexpr double LegacyFrameSeconds = 1.0 / LegacyFramerate;

    // Call once at the top of each render frame.
    static void BeginFrame(double dt_seconds);

  private:
    static double  m_time;
    static double  m_deltaTime;
    static int64_t m_frameCount;
    static int64_t m_legacyFrameCount;
};
