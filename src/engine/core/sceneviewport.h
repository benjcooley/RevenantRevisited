#pragma once
#include <charconv>
#include <cstring>
#include <cstdint>
namespace scene_viewport {
inline bool Parse(const char* text,int32_t out[2]) {
    if(!text||!*text)return false;
    const char* end=text+std::strlen(text);int32_t width=0,height=0;
    auto w=std::from_chars(text,end,width);
    if(w.ec!=std::errc()||w.ptr==end||*w.ptr!=',')return false;
    auto h=std::from_chars(w.ptr+1,end,height);
    if(h.ec!=std::errc()||h.ptr!=end||width<1||height<1||width>1920||height>1080)return false;
    out[0]=width;out[1]=height;return true;
}
inline bool FitsDisplay(const int32_t viewport[2],int32_t width,int32_t height) {
    return viewport[0]>0&&viewport[1]>0&&viewport[0]<=1920&&viewport[1]<=1080&&
           viewport[0]<=width&&viewport[1]<=height;
}
}
