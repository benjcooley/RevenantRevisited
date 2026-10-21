// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *           fxinterp.h - sub-tick motion for the effect ports           *
// *************************************************************************
//
// Retail Revenant stepped every effect on the 24 Hz legacy tick, so a
// picture of drifting dust, sparks or mist moved in jerks while the game
// draws at the display rate. The port draws the same authored states and
// eases between the last two of them, so the motion is smooth at any
// frame rate. The picture trails the simulation by at most one tick.
//
// The helpers here are the shared part of that trick: the fraction of the
// current tick that has already passed, a plain interpolation of a point
// and of an angle, and the guard that tells a tick's step from a jump.
//
// The fraction comes from the effect that owns the picture. An effect
// with an accumulator of its own must use its own leftover time, not the
// global legacy phase: its ticks start when the effect was spawned, so
// the two phases drift apart and the global fraction would make the
// picture stutter.
//
// Header only and free of engine types: the source-slice tests compile a
// piece of the renderer against this file.
// *************************************************************************

#pragma once

// The largest fraction a tick reports: the picture is a hair short of the
// next tick instead of exactly on it.
constexpr float kFxTickFractionMax = 0.999f;

// The part of the current tick that has already passed, in [0,1).
// accumulated_seconds is the effect's own leftover time, tick_seconds the
// length of one of its ticks.
inline float FxTickFraction(double accumulated_seconds, double tick_seconds)
{
    if (!(tick_seconds > 0.0)) return 0.0f;
    const double fraction = accumulated_seconds / tick_seconds;
    if (!(fraction > 0.0)) return 0.0f;          // also catches NaN
    if (fraction >= 1.0) return kFxTickFractionMax;
    return float(fraction);
}

// The floor of a number, without pulling in a math library.
inline float FxFloor(float value)
{
    if (!(value > -1.0e9f && value < 1.0e9f)) return 0.0f;
    const float truncated = float(int(value));
    return (value < truncated) ? truncated - 1.0f : truncated;
}

// An angle in degrees, brought into [-180,180): the half turn a rotation
// can be read from, so a lerp between two angles can take the short way
// round the circle.
inline float FxWrapDeg180(float degrees)
{
    const float turns = FxFloor((degrees + 180.0f) / 360.0f);
    return degrees - turns * 360.0f;
}

// A point halfway between two points.
inline void FxLerp3(const float from[3], const float to[3], float f, float out[3])
{
    out[0] = from[0] + (to[0] - from[0]) * f;
    out[1] = from[1] + (to[1] - from[1]) * f;
    out[2] = from[2] + (to[2] - from[2]) * f;
}

// A number halfway between two numbers.
inline float FxLerp(float from, float to, float f)
{
    return from + (to - from) * f;
}

// An angle halfway between two angles, the short way round the circle:
// a spin of 350 to 10 degrees passes through 0, not through 180.
inline float FxLerpAngleDeg(float from, float to, float f)
{
    const float delta = FxWrapDeg180(to - from);
    return FxWrapDeg180(from + delta * f);
}

// True when the two states are the ends of one tick of the same motion.
// The particle was alive before the tick and is alive now, it did not
// change phase, and the step is small enough to be a tick and not a jump
// (a teleport, a particle reused in a slot that was empty a tick ago).
inline bool FxShouldInterpolate(bool was_alive, bool is_alive, bool same_phase,
                                 float step_magnitude, float step_limit)
{
    if (!was_alive || !is_alive) return false;
    if (!same_phase) return false;
    if (!(step_magnitude <= step_limit)) return false;   // also catches NaN
    return true;
}
