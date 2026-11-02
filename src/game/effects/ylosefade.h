#pragma once
#include <cstdint>

namespace retail_ylose {
// Original408a8e initializes copied FVF1e2 vertices to this literal color.
inline constexpr uint32_t initial_color=0xfffffff8u;
// Actual parsed malformed list: boltring08 (object12) is not selected.
inline bool FadedObject(int object){return object>=2 && object<=16 && object!=12;}
// Original controller Render4022b0 selects the first key strictly after the
// current owner frame. At/after the final key it leaves the last rendered
// vertex colors untouched. This is called on submission, never on Pulse.
inline void RenderColor(int frame,uint32_t& packed)
{
    if(frame<0 || frame>=13)return;
    const int value=frame<5?100+16*frame:int(180.0-22.5*double(frame-5));
    packed=uint32_t(value)*0x010101u;
}
}
