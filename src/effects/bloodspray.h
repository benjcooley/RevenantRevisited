// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *     bloodspray.h - retail blood droplet simulation (no engine types)  *
// *************************************************************************
//
// The droplet half of retail's TBloodAnimator (vtable 0x005acd18): the
// spray Initialize (0x004f18d0) builds and the per-tick step Pulse
// (0x004f1c60) runs. Kept free of engine types so the retail A/B probe
// (tools/retail_runtime/blood_spray_probe.py) can compile it next to the
// original code in the emulator. The animator that owns it, draws it and
// registers the "Blood" effect is effects/retailblood.{h,cpp}.
//
// Shipped retail differs from the 1998 snapshot's TBloodSystem
// (effectcomp.cpp): it ignores the requested count and sizes the spray from
// the violence level, pairs every droplet with a slower trailing one, flies
// at half the vertical speed, randomises the splat size and turns a big
// droplet into the splat sprite when it lands.
//
// *************************************************************************

#pragma once

#include <array>
#include <cstdint>
#include <functional>

namespace retail_blood
{
constexpr int32_t kMaxDrops = 30;   // Initialize clears 0x10e dwords = 30 x 0x24

enum EStage : uint8_t { StageFly = 0, StageSplat = 1, StageShrink = 2 };

// One droplet: retail's 0x24-byte record, the array at animator +0xfc.
struct SDrop
{
    float   vel[3] = {};       // +0x00
    float   pos[3] = {};       // +0x0c, from the effect; z starts at the spray height
    float   scale  = 0.0f;     // +0x18
    bool    used   = false;    // +0x1c
    uint8_t size   = 0;        // +0x20: 0 small, 1 medium, 2 big, 3 splat (a big one that landed)
    uint8_t stage  = StageFly; // +0x21
    uint8_t count  = 0;        // +0x22: ticks spent splatted
    uint8_t delay  = 0;        // +0x23: ticks a trailing droplet waits before it moves
};

// TBloodEffect's spray parameters (+0x184..+0x194; the sixth, the count at
// +0x198, is stored but retail's animator never reads it).
struct SParams
{
    int32_t height  = 0;   // spawn height above the bleeder; the ground is 20
    int32_t hangle  = 0;   // spray heading, 0..255 = one turn
    int32_t vangle  = 0;   // spray elevation, same units
    int32_t hspread = 0;   // +- jitter on the heading
    int32_t vspread = 0;   // +- jitter on the elevation
};

// Retail random(lo, hi) @ 0x00483300: the game's one random stream.
using Random = std::function<int32_t(int32_t, int32_t)>;

class TSpray
{
  public:
    // TBloodAnimator::Initialize @ 0x004f18d0 (after T3DAnimator::Initialize).
    void Initialize(const SParams& params, int32_t violence, const Random& random);
    // TBloodAnimator::Pulse @ 0x004f1c60 (after T3DAnimator::Pulse): one
    // tick. Returns true when no droplet moved: the effect kills itself.
    bool Step(int32_t violence, const Random& random);

    [[nodiscard]] const std::array<SDrop, kMaxDrops>& Drops() const { return drops; }

  private:
    std::array<SDrop, kMaxDrops> drops{};
};

// Render @ 0x004f1fb6..0x004f1fda packs the droplet's light into a D3D ARGB
// vertex colour with an ill-formed byte merge: for the bytes e0..e2 of
// ftol(light * 255) the colour is A=0xff, R=e0|e1|e2, G=e0|e1, B=e0.
[[nodiscard]] uint32_t PackedVertexColor(float light);
} // namespace retail_blood
