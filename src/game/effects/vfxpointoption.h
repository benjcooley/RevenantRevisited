#pragma once
#include <charconv>
#include <cstdint>
#include <string_view>

// Exact signed world coordinates; reject trailing fields and overflow rather
// than recording an endpoint that differs from the requested fixture.
inline bool ParseVfxPointOption(std::string_view text, int32_t (&point)[3])
{
    int32_t parsed[3]{};
    for (int i=0; i<3; ++i) {
        const auto comma=text.find(',');
        if ((i<2 && comma==std::string_view::npos) ||
            (i==2 && comma!=std::string_view::npos)) return false;
        const auto field=text.substr(0,comma);
        if(field.empty()) return false;
        const auto result=std::from_chars(field.data(),field.data()+field.size(),parsed[i]);
        if (result.ec!=std::errc{} || result.ptr!=field.data()+field.size()) return false;
        if (i<2) text.remove_prefix(comma+1);
    }
    for(int i=0;i<3;++i) point[i]=parsed[i];
    return true;
}
