#pragma once

#include <cstdint>

// Literal TeleportDoorInside animator (retail 4ff9d0/4ffa60/4ffae0).
// Each owner keeps its own state. The first four samples use row zero;
// subsequent rows cycle 1, .75, .5, .25 rather than a modulo-V wrap.
namespace retail_warp {

struct State {
    float u = 0.0f;
    float v = 0.0f;
    double accumulator = 0.0;

    void Reset() { u = v = 0.0f; accumulator = 0.0; }
    void Step() {
        u += 0.25f;
        if (u >= 1.0f) {
            u = 0.0f;
            v -= 0.25f;
            if (v <= 0.0f) v = 1.0f;
        }
    }
    void Advance(double seconds) {
        if (!(seconds > 0.0)) return;
        constexpr double tick = 1.0 / 24.0;
        accumulator += seconds;
        while (accumulator + 1e-9 >= tick) {
            accumulator -= tick;
            Step();
        }
    }
};

struct Profile { uint32_t id; const char* asset; };
inline constexpr Profile profiles[] = {
    {0xad92bc28u, "Magic\\WarpB.I3D"},
    {0xad92bc29u, "Magic\\WarpO.I3D"},
    {0xad92bc30u, "Magic\\WarpP.I3D"},
    {0xad92bc31u, "Magic\\WarpR.I3D"},
    {0xad92bc32u, "Magic\\WarpW.I3D"},
    {0xad92bc33u, "Magic\\WarpY.I3D"},
    {0xad92bc34u, "Magic\\WarpG.I3D"},
};

inline const char* AssetPath(uint32_t id) {
    for (const auto& profile : profiles)
        if (profile.id == id) return profile.asset;
    return nullptr;
}

template<class Draw>
inline void ConfigureDraw(Draw& draw, const State& state) {
    draw.uv_offset[0] = state.u;
    draw.uv_offset[1] = state.v;
    draw.additive_blend = false;
    draw.premultiply_alpha = false;
    draw.shade = decltype(draw.shade)::Texture;
    draw.retail_lighting = 2;
    for (int i = 0; i < 4; ++i) {
        draw.diffuse[i] = draw.ambient[i] = 1.0f;
        draw.specular[i] = draw.emissive[i] = 0.0f;
    }
    draw.power = 0.0f;
}

} // namespace retail_warp
