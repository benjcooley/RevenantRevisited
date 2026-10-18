// *************************************************************************
// *                         Revenant Revisited                            *
// *       equipanimator.cpp - The animator for equipment on the map       *
// *************************************************************************

#include "equipanimator.h"

#include "object.h"
#include "time.h"

#include <cstring>

REGISTER_3DANIMATOR("Equip", T3DEquipAnimator)

namespace {

constexpr float kHalfPi    = 1.5707964f;    // 0x3fc90fdb
constexpr float kArmorTurn = -0.7853982f;   // 0xbf490fdb, -π/4
constexpr float kArmorSize = 1.5f;          // 0x3fc00000
constexpr double kTwoPi    = 6.283185307179586;

// The part a frame shows between two toss ticks, the angle the short way round.
double Between(double older, double newer, double frac)
{
    return older + (newer - older) * frac;
}

double AngleBetween(double older, double newer, double frac)
{
    double turn = newer - older;
    if (turn > kTwoPi / 2.0)
        turn -= kTwoPi;
    else if (turn < -kTwoPi / 2.0)
        turn += kTwoPi;
    return older + turn * frac;
}

}  // namespace

void T3DEquipAnimator::Animate(bool draw)
{
    T3DAnimator::Animate(draw);
    PoseParts();
}

// REVSYNC: the Equip render 0x0040f5b0. Every part's transform is replaced
// (flags SCL1 | ROT2 | POS3): at rest (0x0040fdc8) rotated (π/2, π/2, π/2),
// scale 1, at the object's origin; armor (0, 0, -π/4) at 1.5. While a toss
// is live and the object isn't armor (0x0040f63f), at the toss's pose: its
// angles, at its position less the object's (the object stands at the
// target). Retail draws the toss at each tick's pose; the port draws a frame
// between the last two ticks (DEATH_LOOT.md §8). Not ported yet: the four
// trailing ghosts and the landing flash (§6).
void T3DEquipAnimator::PoseParts()
{
    const bool armor = stricmp(inst->GetClassName(), "ARMOR") == 0;
    const SToss* const tossed = armor ? nullptr : GetToss();

    hmm_vec3 offset{ 0.0f, 0.0f, 0.0f };
    hmm_vec3 rot{ kHalfPi, kHalfPi, kHalfPi };
    hmm_vec3 scl{ 1.0f, 1.0f, 1.0f };
    if (tossed)
    {
        // Settled (counting down), the pose stands still at the newest tick.
        const double frac = tossed->countdown < 0 ? TTime::LegacyFrameFraction() : 1.0;
        const SToss::SVec& p0 = tossed->pos[1];
        const SToss::SVec& p1 = tossed->pos[0];
        const SToss::SVec& a0 = tossed->angle[1];
        const SToss::SVec& a1 = tossed->angle[0];
        const S3DPoint at = IntPos(inst);
        offset = { float(Between(p0.x, p1.x, frac) - at.x),
                   float(Between(p0.y, p1.y, frac) - at.y),
                   float(Between(p0.z, p1.z, frac) - at.z) };
        rot = { float(AngleBetween(a0.x, a1.x, frac)),
                float(AngleBetween(a0.y, a1.y, frac)),
                float(AngleBetween(a0.z, a1.z, frac)) };
    }
    else if (armor)
    {
        rot = { 0.0f, 0.0f, kArmorTurn };
        scl = { kArmorSize, kArmorSize, kArmorSize };
    }

    for (int32_t c = 0; c < animobjs.NumItems(); c++)
    {
        S3DAnimObj* part = animobjs[c];
        if (!part)
            continue;
        part->flags = (part->flags & ~(OBJ3D_POSMASK | OBJ3D_ROTMASK | OBJ3D_SCLMASK))
                    | OBJ3D_SCL1 | OBJ3D_ROT2 | OBJ3D_POS3;
        part->pos = offset;
        part->rot = rot;
        part->scl = scl;
        // Scale, then rotation about x, y, z, then position: the transform's
        // own order (TTransform), so the renderer's bone matrix is this pose.
        part->transform.SetLocalPos(offset);
        part->transform.SetLocalRotEuler(rot);
        part->transform.SetLocalScl(scl);
    }
}
