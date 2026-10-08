#ifdef NDEBUG
#undef NDEBUG
#endif
#include "../src/retailmeshlighting.h"
#include <cassert>
#include <cmath>
#include <iostream>

static bool near(float a, float b) { return std::fabs(a - b) < 1e-6f; }

int main()
{
    // Retained software fixture: saturated white, source 85% directional split.
    for (int adjustment : {100, 130}) {
        auto lit = RetailSoftwareMeshLighting(32, adjustment, {255,255,255}, true, 85);
        for (int c = 0; c < 3; ++c) {
            assert(near(lit.ambient[c], 38.0f / 255.0f));
            assert(near(lit.directional[c], 1.0f));
        }
    }
    // Dim colored area: normalization preserves hue, then integer /256 packing.
    auto dim = RetailSoftwareMeshLighting(4, 100, {64,128,32}, false, 85);
    assert(near(dim.ambient[0], 16.0f/255.0f));
    assert(near(dim.ambient[1], 31.0f/255.0f));
    assert(near(dim.ambient[2], 8.0f/255.0f));
    for (float v : dim.directional) assert(v == 0);
    auto split = RetailSoftwareMeshLighting(4, 100, {255,255,255}, true, 85);
    assert(near(split.ambient[0], 4.0f/255.0f));
    assert(near(split.directional[0], 0.15439453125f));
    auto dark = RetailSoftwareMeshLighting(0, 130, {255,255,255}, true, 85);
    auto black = RetailSoftwareMeshLighting(32, 130, {0,0,0}, true, 85);
    for (int c = 0; c < 3; ++c) {
        assert(dark.ambient[c] == 0 && dark.directional[c] == 0);
        assert(black.ambient[c] == 0 && black.directional[c] == 0);
    }
    std::cout << "Retail software mesh lighting source-value checks passed\n";
}
