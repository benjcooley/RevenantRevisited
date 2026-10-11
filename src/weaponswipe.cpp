// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *       weaponswipe.cpp - retail weapon swipe (no engine types)         *
// *************************************************************************
//
// Transcribed from retail TWeaponSwipe (vtable 0x005a7e98); notes in
// docs/gameplay/forensics/COMBAT_EFFECTS.md §6. The x87 chains are kept in
// double with the original operand order and stored to float where the
// original stores, so the emulator A/B compares bit for bit.

#include "weaponswipe.h"

#include <cctype>

namespace
{
constexpr float kStartAlpha = 0.699999988f;   // 0x3f333333 (0x004d93db)
constexpr float kFadeAlpha = 0.699999988f;    // 0x005a7ec0, spread over the strip
constexpr float kByteScale = 255.0f;          // 0x005a352c

using SVec3 = TWeaponSwipe::SVec3;

// 0x0043ad80: v * M, x87 sums in this order.
SVec3 Transform(const TWeaponSwipe::SMatrix& m, const SVec3& v)
{
    const double x = v[0], y = v[1], z = v[2];
    return {float(((z * m[8] + y * m[4]) + x * m[0]) + m[12]),
            float(((z * m[9] + y * m[5]) + x * m[1]) + m[13]),
            float(((z * m[10] + y * m[6]) + x * m[2]) + m[14])};
}

// 0x0043b280: Catmull-Rom through p1 (t = 0) and p2 (t = 1). The first three
// weights are stored as floats; x sums from p0, y and z from p3.
SVec3 Spline(float t, const SVec3& p0, const SVec3& p1, const SVec3& p2, const SVec3& p3)
{
    const double t1 = t;
    const double t2 = t1 * t1;
    const double t3 = t1 * t2;
    const double w0 = float(((-t1 + -t3) + (t2 + t2)) * 0.5);
    const double w1 = float(((t3 * 3.0 + t2 * -5.0) + 2.0) * 0.5);
    const double w2 = float(((t3 * -3.0 + t2 * 4.0) + t1) * 0.5);
    const double w3 = (-t2 + t3) * 0.5;
    return {float(((w0 * p0[0] + w1 * p1[0]) + w2 * p2[0]) + w3 * p3[0]),
            float(((w3 * p3[1] + w2 * p2[1]) + w1 * p1[1]) + w0 * p0[1]),
            float(((w3 * p3[2] + w2 * p2[2]) + w1 * p1[2]) + w0 * p0[2])};
}

// _ftol @ 0x0058a840: truncation.
uint32_t Byte(double value)
{
    return uint32_t(int32_t(value));
}

// _stricmp @ 0x0059a530.
bool SameName(const std::string& a, const char* b)
{
    size_t i = 0;
    for (; i < a.size() && b[i]; ++i)
        if (std::tolower(uint8_t(a[i])) != std::tolower(uint8_t(b[i])))
            return false;
    return i == a.size() && !b[i];
}
} // namespace

void TWeaponSwipe::Init(const SParams& params, const SFrame& now)
{
    if (initialized)
        return;
    initialized = true;
    color = params.color;
    extent_min = params.extent_min;
    extent_max = params.extent_max;
    weapon = params.weapon;
    full = params.full;
    // The two sample arrays are allocated cleared (0x00482fb0 zero-fills).
    ring = {};
    // 0x004d92b0: maxsegs + 1 samples of the weapon as it is; they sample the
    // previous axis (zero the first time), which ChooseAxis then sets.
    for (int32_t i = 0; i < kMaxSegs + 1; ++i)
        AddPoint(now);
    ChooseAxis();
}

void TWeaponSwipe::Close()
{
    initialized = false;
    weapon.clear();
}

void TWeaponSwipe::Update(const SFrame& now)
{
    if (!initialized)
        return;
    if (!now.attacking)
    {
        // 0x004d967e: maxsegs / 4 shrinks a frame closes an idle swipe.
        for (int32_t i = 0; i < kMaxSegs / 4; ++i)
            Shrink();
        return;
    }
    if (now.frame_on != 0 || now.frame_off != 0)
    {
        // The window opens with a fresh history (0x004d964c).
        if (now.frame == now.frame_on)
            Reset(now);
        if (now.frame >= now.frame_on && now.frame <= now.frame_off)
        {
            AddPoint(now);
            return;
        }
    }
    // Outside the window: the history moves on without a new sample.
    Shift();
    Shrink();
}

void TWeaponSwipe::AddPoint(const SFrame& now)
{
    if (!initialized)
        return;
    // A player's weapon change closes the swipe (0x004d970a..0x004d973c).
    if (now.player && now.in_hand)
    {
        if (weapon.empty() || !SameName(weapon, now.in_hand))
        {
            Close();
            return;
        }
    }
    Shift();
    Shrink();
    // Retail transforms through a null matrix when the character has no
    // weapon object (it would fault); every SWIPECOLOR character has one.
    if (now.weapon)
    {
        ring.hilt[0] = Transform(*now.weapon, hilt_local);
        ring.tip[0] = Transform(*now.weapon, tip_local);
    }
}

void TWeaponSwipe::Reset(const SFrame& now)
{
    if (!initialized || !now.weapon)
        return;
    const SVec3 hilt = Transform(*now.weapon, hilt_local);
    const SVec3 tip = Transform(*now.weapon, tip_local);
    ring.hilt.fill(hilt);
    ring.tip.fill(tip);
}

void TWeaponSwipe::Shift()
{
    for (int32_t i = kPoints - 1; i > 0; --i)
    {
        ring.hilt[size_t(i)] = ring.hilt[size_t(i - 1)];
        ring.tip[size_t(i)] = ring.tip[size_t(i - 1)];
    }
}

void TWeaponSwipe::Shrink()
{
    // Each pair closes on its midpoint by 1 - 1/points. The midpoint's x
    // stays on the x87 stack; y and z are stored as floats first.
    const double keep = 1.0 - 1.0 / double(kPoints);
    for (int32_t i = 0; i < kPoints; ++i)
    {
        SVec3& hilt = ring.hilt[size_t(i)];
        SVec3& tip = ring.tip[size_t(i)];
        const double mid_x = (double(tip[0]) + hilt[0]) * 0.5;
        const double mid_y = float((double(tip[1]) + hilt[1]) * 0.5);
        const double mid_z = float((double(hilt[2]) + tip[2]) * 0.5);
        hilt[0] = float((hilt[0] - mid_x) * keep + mid_x);
        hilt[1] = float((hilt[1] - mid_y) * keep + mid_y);
        hilt[2] = float((hilt[2] - mid_z) * keep + mid_z);
        tip[0] = float((tip[0] - mid_x) * keep + mid_x);
        tip[1] = float((tip[1] - mid_y) * keep + mid_y);
        tip[2] = float((tip[2] - mid_z) * keep + mid_z);
    }
}

void TWeaponSwipe::ChooseAxis()
{
    // The bounds' longest axis; ties go to the later axis. The hilt end is
    // the weapon's pivot unless the swipe is full. On y the tip is the low
    // end, not the high one (0x004d9909 reads +0x388, min.y, again): a
    // weapon modelled along y sweeps its short side.
    if (!initialized)
        return;
    const double dx = double(extent_max[0]) - extent_min[0];
    const double dy = float(double(extent_max[1]) - extent_min[1]);
    const double dz = float(double(extent_max[2]) - extent_min[2]);
    int32_t axis = 2;
    if (dx > dy && dx > dz)
        axis = 0;
    else if (dy > dx && dy > dz)
        axis = 1;
    hilt_local = {};
    tip_local = {};
    hilt_local[size_t(axis)] = full ? extent_min[size_t(axis)] : 0.0f;
    tip_local[size_t(axis)] = axis == 1 ? extent_min[1] : extent_max[size_t(axis)];
}

TWeaponSwipe::SVec3 TWeaponSwipe::ColorOf(uint8_t red, uint8_t green, uint8_t blue)
{
    constexpr float kByteToUnit = 0.00392156886f;   // 0x005a3830
    return {float(double(red) * kByteToUnit), float(double(green) * kByteToUnit),
            float(double(blue) * kByteToUnit)};
}

void TWeaponSwipe::BuildStrip(const SRing& ring, const SVec3& color, SStrip& out)
{
    // The segments run from sample 1 to sample kPoints - 2; sample 0, the
    // newest, only shapes the first one.
    const float step = float(1.0 / double(kSmooth));
    const double fade = double(kFadeAlpha) / double((kPoints - 2) * kSmooth);
    float alpha = kStartAlpha;
    size_t vertex = 0;
    for (int32_t seg = 0; seg + 1 < kPoints - 2; ++seg)
    {
        float t = 0.0f;
        for (int32_t s = 0; s < kSmooth; ++s)
        {
            const uint32_t argb = Byte(double(alpha) * kByteScale) << 24 |
                                  Byte(double(color[0]) * kByteScale) << 16 |
                                  Byte(double(color[1]) * kByteScale) << 8 |
                                  Byte(double(color[2]) * kByteScale);
            for (const auto* side : {&ring.hilt, &ring.tip})
            {
                const auto& p = *side;
                out[vertex].pos = Spline(t, p[size_t(seg)], p[size_t(seg + 1)], p[size_t(seg + 2)],
                                         p[size_t(seg + 3)]);
                out[vertex].argb = argb;
                ++vertex;
            }
            t = float(double(step) + t);
            alpha = alpha < fade ? 0.0f : float(alpha - fade);
        }
    }
}

TWeaponSwipe::SRing TWeaponSwipe::Blend(const SRing& from, const SRing& to, float t)
{
    SRing out;
    const auto mix = [t](const SVec3& a, const SVec3& b) {
        return SVec3{a[0] + (b[0] - a[0]) * t, a[1] + (b[1] - a[1]) * t, a[2] + (b[2] - a[2]) * t};
    };
    for (size_t i = 0; i < size_t(kPoints); ++i)
    {
        out.hilt[i] = mix(from.hilt[i], to.hilt[i]);
        out.tip[i] = mix(from.tip[i], to.tip[i]);
    }
    return out;
}

bool TWeaponSwipe::HasWidth(const SRing& ring)
{
    for (size_t i = 0; i < size_t(kPoints); ++i)
        if (ring.hilt[i] != ring.tip[i])
            return true;
    return false;
}
