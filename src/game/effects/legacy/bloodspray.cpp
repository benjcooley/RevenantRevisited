// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   bloodspray.cpp - retail blood droplet simulation (no engine types)  *
// *************************************************************************
//
// Transcribed from TBloodAnimator::Initialize @ 0x004f18d0 and Pulse
// @ 0x004f1c60; notes in docs/gameplay/forensics/COMBAT_EFFECTS.md §2.

#include "game/effects/legacy/bloodspray.h"

#include <algorithm>
#include <cmath>

namespace retail_blood
{
namespace
{
// Literal .rdata values: the 1990s degree factor is not modern pi/180.
constexpr double kDegToRad    = 0.017453292522222223; // 0x005a8a98
constexpr double kOneHundredth = 0.01;                // 0x005a35a0
constexpr double kOneTwoHundredth = 0.005;            // 0x005acd88
constexpr double kOneSeventieth = 0.014285714285714285; // 0x005acd80
constexpr double kOneEightieth = 0.0125;              // 0x005acd78
constexpr float  kAirResist   = 0.949999988f;         // 0x005aad14
constexpr float  kGravity     = 0.370000005f;         // 0x005ac614
constexpr float  kFlyGrowth   = 0.00999999978f;       // 0x005a3524
constexpr float  kGround      = 20.0f;                // 0x005a8d0c
constexpr float  kSplatSize   = 1.79999995f;          // 0x005acd90
constexpr float  kSplatStep   = 0.100000001f;         // 0x005a3538 (Pulse), 0x005a4848 (shrink)
constexpr int    kSplatTicks  = 30;                   // 0x004f1d0e cmp al, 0x1e
constexpr float  kStartScale  = 0.5f;                 // 0x3f000000

float ByteAngle(int32_t angle)
{
    // fild; fmul 360.0f; fmul 1/256; fmul deg-to-rad; fstp float.
    return float(double(angle) * 360.0 * 0.00390625 * kDegToRad);
}
} // namespace

void TSpray::Initialize(const SParams& params, int32_t violence, const Random& random)
{
    drops = {};
    // 0x004f1a20..0x004f1a56: min(violence * 3, 14), less a random part. The
    // max() macro evaluates its argument twice, so the count is a second
    // draw and the first only decides whether there is any blood at all.
    const int32_t most = (std::min)(violence * 3, 14);
    int32_t count = 0;
    if (most - random(0, most) > 0)
        count = most - random(0, most);

    // Every droplet i has a trailing partner at i + count (0x004f1a72).
    for (int32_t i = 0; i < count; ++i)
    {
        SDrop& drop = drops[size_t(i)];
        SDrop& trail = drops[size_t(count + i)];

        const float heading = ByteAngle((random(-params.hspread, params.hspread) + params.hangle) & 0xff);
        const float elevation = ByteAngle((random(-params.vspread, params.vspread) + params.vangle) & 0xff);
        drop.used = true;
        drop.size = uint8_t(random(0, 2));
        drop.pos[2] = float(params.height);
        const double cos_heading = std::cos(double(heading));
        int32_t r = random(100, 300);
        drop.vel[0] = float(double(r + r) * cos_heading * kOneHundredth);
        const double sin_heading = std::sin(double(heading));
        r = random(100, 300);
        drop.vel[1] = float(double(r + r) * sin_heading * kOneHundredth);
        const double sin_elevation = std::sin(double(elevation));
        r = random(100, 300);
        // Half the snapshot's vertical speed (0.005, not 0.01).
        drop.vel[2] = float(double(r + r) * sin_elevation * kOneTwoHundredth);
        drop.scale = kStartScale;
        drop.stage = StageFly;
        drop.delay = 0;

        trail.used = true;
        // random(0, trail.size) on the still-cleared record: always small,
        // and equal endpoints draw nothing.
        trail.size = uint8_t(random(0, trail.size));
        trail.pos[2] = float(params.height);
        // The trailing velocity takes the heading's cosine/sine a second
        // time (0x004f1bd9, 0x004f1bfa, 0x004f1c1b).
        r = random(60, 80);
        trail.vel[0] = float(double(r) * drop.vel[0] * cos_heading * kOneSeventieth);
        r = random(60, 80);
        trail.vel[1] = float(double(r) * drop.vel[1] * sin_heading * kOneSeventieth);
        r = random(60, 80);
        trail.vel[2] = float(double(r) * drop.vel[2] * sin_elevation * kOneEightieth);
        trail.delay = uint8_t(random(-1, 1) + drop.delay + 3);
        trail.stage = StageFly;
        trail.scale = kStartScale;
    }
}

bool TSpray::Step(int32_t violence, const Random& random)
{
    bool done = true;
    for (SDrop& drop : drops)
    {
        if (!drop.used)
            continue;
        if (drop.delay)
        {
            --drop.delay;
            continue;
        }
        done = false;
        switch (drop.stage)
        {
        case StageFly:
            for (int axis = 0; axis < 3; ++axis)
                drop.pos[axis] += drop.vel[axis];
            drop.vel[0] *= kAirResist;
            drop.vel[1] *= kAirResist;
            drop.vel[2] -= kGravity;
            drop.scale += kFlyGrowth;
            if (drop.pos[2] <= kGround)
            {
                drop.pos[2] = kGround;
                drop.stage = StageSplat;
                drop.count = 0;
                if (violence < 3)
                    drop.used = false;
                if (drop.size == 0)
                    drop.used = false;
            }
            break;
        case StageSplat:
            if (drop.count == 0)
                drop.scale = float(double(kSplatSize) - double(random(0, 7)) * double(kSplatStep));
            if (++drop.count > kSplatTicks)
            {
                drop.stage = StageShrink;
                drop.count = 0;
            }
            if (drop.size == 2)
                drop.size = 3;   // a big droplet lies as the splat sprite
            break;
        case StageShrink:
            drop.scale -= kSplatStep;
            if (drop.scale <= 0.0f)
            {
                drop.scale = 0.0f;
                drop.used = false;
            }
            break;
        default:
            break;
        }
    }
    return done;
}

uint32_t PackedVertexColor(float brightness)
{
    const uint32_t e = uint32_t(int32_t(double(brightness) * 255.0));  // _ftol truncates
    uint32_t c = e | 0xffffff00u;
    c = (c << 8) | e;
    c = (c << 8) | e;
    return c;
}
} // namespace retail_blood
