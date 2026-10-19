#pragma once
#include <cstddef>

// Uniform FireBall specialization of retail shared subparticles. Original
// 0x50adb0 updates the pool, and 0x50af40 creates one first-free particle.
// Other effects with nonuniform scale or velocity direction need their own
// parameter adapter; this does not replace the general particle system.
namespace fireball_spark
{
template<class Spark, std::size_t N, class Random>
void Advance(Spark (&pool)[N], int desired, int chance,
             float owner_x, float owner_y, float owner_z,
             int spread_xy, int spread_z, float initial_scale,
             float scale_decay, float gravity, int min_life, int max_life,
             Random random)
{
    int live = 0;
    for (const auto& p : pool) if (p.used) ++live;
    if (desired > int(N)) desired = int(N);
    const int attempts = desired - live;
    for (int attempt = 0; attempt < attempts; ++attempt)
    {
        if (random(1, 100) > chance) continue;
        for (auto& p : pool)
        {
            if (p.used) continue;
            p.used = true;
            // Preserve even equal-endpoint RandomRange calls. Retail returns
            // without drawing CRT rand() for these six zero spreads.
            p.pos.X = owner_x + float(random(0, 0));
            p.pos.Y = owner_y + float(random(0, 0));
            p.pos.Z = owner_z + float(random(0, 0));
            p.scale = initial_scale + float(random(0, 0));
            (void)random(0, 0); // Uniform Y scale.
            (void)random(0, 0); // Uniform Z scale.
            p.vel.X = float(random(-spread_xy, spread_xy));
            p.vel.Y = float(random(-spread_xy, spread_xy));
            p.vel.Z = float(random(-spread_z, spread_z));
            p.life = random(min_life, max_life);
            p.flicker_status = random(0, 1) != 0;
            break;
        }
    }
    for (auto& p : pool)
    {
        if (!p.used) continue;
        p.flicker_status = random(0, 1) != 0;
        if (--p.life < 0) p.used = false;
        // Original still updates all fields on the tick that expires life.
        p.scale *= scale_decay;
        p.pos.X += p.vel.X;
        p.pos.Y += p.vel.Y;
        p.pos.Z += p.vel.Z;
        p.vel.Z -= gravity;
    }
}
}
