// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *             subtick.h - sub-tick motion for moving objects            *
// *************************************************************************
//
// The world moves at the legacy tick rate (24 Hz) but the picture is drawn
// at the display rate, so an object that moved during the tick is drawn
// where the tick started, advanced by the fraction of the tick that has
// passed: it glides instead of stepping once per tick.
//
// The step has to be the step of ONE tick. TObjectInstance::Move() records
// the tick's starting position in prevpos; a class that overrides Move()
// without recording it (TCharacter::Move, and so TPlayer::Move) leaves
// prevpos at the spawn point, and the guard below then reads the distance
// from the spawn as the step and refuses to interpolate -- the character
// then steps once per tick while the world glides.
//
// Header only and free of engine types, so the source-slice tests can
// compile a piece of the renderer against it.
// *************************************************************************

#pragma once

// The largest step a tick may make before it counts as a jump (a teleport)
// instead of a step of a tick.
constexpr int32_t kSubTickMaxStep = 8;

// The offset the drawn position gets from the tick's step: the tick's
// starting position advanced by the fraction of the tick that has passed
// (frac is the fraction of the legacy tick, 0..1). Returns false for a
// jump -- then the object is drawn where it is.
inline bool SubTickOffset(int32_t step_x, int32_t step_y, int32_t step_z,
                         float frac, float& out_x, float& out_y, float& out_z)
{
    if (step_x > kSubTickMaxStep || step_x < -kSubTickMaxStep ||
        step_y > kSubTickMaxStep || step_y < -kSubTickMaxStep ||
        step_z > kSubTickMaxStep || step_z < -kSubTickMaxStep)
        return false;
    const float scale = frac - 1.0f;
    out_x = float(step_x) * scale;
    out_y = float(step_y) * scale;
    out_z = float(step_z) * scale;
    return true;
}
