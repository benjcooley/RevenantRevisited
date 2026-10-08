#pragma once
#include <cstdint>
#include <cstddef>

// Shared head/history stage of retail Animate 0x510e40..0x511179.
// Caller updates sparks first and supplies the same random stream. This
// stage deliberately does not implement launch, burst, ring or damage.
namespace fireball_core
{
template<class Card, std::size_t N, class Random>
void Advance(Card& head, Card (&trail)[N], float owner_x, float owner_y,
             float owner_z, int frame_count, int glow_frame,
             float trail_decay, Random random)
{
    static_assert(N == 10, "Retail FireBall history has ten slots");
    for (std::size_t i = N - 1; i > 0; --i)
    {
        trail[i] = trail[i - 1];
        trail[i].scale *= trail_decay;
    }
    trail[0] = head;
    trail[0].scale *= trail_decay;
    trail[0].pos.X = head.pos.X + owner_x;
    trail[0].pos.Y = head.pos.Y + owner_y;
    trail[0].pos.Z = head.pos.Z + owner_z;

    head.glow = 1.0f + 0.05f * float(random(0, 15));
    head.frame += 1.0f;
    if (head.frame > float(frame_count)) head.frame = 0.0f;
    if (int32_t(head.frame) == glow_frame)
    {
        head.frame += 1.0f;
        if (head.frame > float(frame_count)) head.frame = 0.0f;
    }
    // The retail field is an integer and uses signed remainder (idiv).
    head.rotation = float((int32_t(head.rotation) + 2) % 360);
}
}
