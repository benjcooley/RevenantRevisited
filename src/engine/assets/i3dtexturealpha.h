#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>

// Alpha-bearing images already encode coverage. The legacy black-background
// heuristic is solely for RGB images, including shipped RGB565 sprite atlases.
inline void ApplyI3DBlackKey(std::vector<uint8_t>& rgba, bool authored_alpha)
{
    if (authored_alpha) return;
    const std::size_t count = rgba.size() / 4;
    std::size_t black = 0;
    for (std::size_t i=0; i<count; ++i)
        if (!rgba[i*4] && !rgba[i*4+1] && !rgba[i*4+2]) ++black;
    if (!count || black * 5 <= count) return;
    for (std::size_t i=0; i<count; ++i)
        if (!rgba[i*4] && !rgba[i*4+1] && !rgba[i*4+2]) rgba[i*4+3]=0;
}
