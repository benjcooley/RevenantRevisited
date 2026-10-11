#pragma once
#include <cstdint>
#include <algorithm>

namespace fireball_ring {
// Original5113b5 supplies these packed stops to TShockAnimator. Render50b8e0
// preserves RGB and fades only alpha, then truncates the packed byte.
inline uint32_t PackedColor(int ring,float scale,bool y_variant=false)
{
    constexpr uint32_t stops[4]={0x009e0f0cu,0xffc63d0fu,0xbff79b0fu,0x00fcd384u};
    const float fade=(120.0f-scale)/(120.0f-25.0f);
    // Original Y513942 supplies its own green stops.
    constexpr uint32_t y_stops[4]={0x000f9e0cu,0xff3dc60fu,0xbf9bf70fu,0x00d3fc84u};
    const uint32_t stop=y_variant?y_stops[ring]:stops[ring];
    const long double alpha=static_cast<long double>(stop>>24)*
        static_cast<long double>(0.0039215688593685627f)*fade*255.0L;
    return (uint32_t(std::clamp(int(alpha),0,255))<<24)|(stop&0xffffffu);
}
}
