#pragma once
#include <algorithm>
#include <array>

// 3dscene.cpp::SetAmbientLight/SetAmbientColor/BeginScene, followed by
// blue/swscene.cpp::SetAmbientLightColor. These are source constants,
// not a brightness fit to screenshots. Point-light selection is separate.
struct SRetailMeshLighting {
    std::array<float, 3> ambient{};
    std::array<float, 3> directional{};
};

inline SRetailMeshLighting RetailSoftwareMeshLighting(int ambient, int ambient3d,
        std::array<int, 3> color, bool use_directional, int directional_percent)
{
    const int value = (std::min)(ambient * 8 * ambient3d / 100, 255);
    const int maximum = (std::max)({color[0], color[1], color[2], 1});
    SRetailMeshLighting out;
    for (int i = 0; i < 3; ++i) {
        const int normalized = (std::min)(color[i] * 256 / maximum, 255);
        const int packed = normalized * value / 256;
        const int base = use_directional ? packed * (100 - directional_percent) / 100 : packed;
        out.ambient[i] = float(base) / 255.0f;
        out.directional[i] = use_directional
            ? (std::min)(float(packed) / 256.0f * (float(directional_percent) / 100.0f * 1.5f), 1.0f)
            : 0.0f;
    }
    return out;
}
