#pragma once
#include <cstdint>

// Explicit legacy Sand restoration only. Not an alias of the Sparks resource.
namespace sandswirl_runtime {
inline bool LegacySandResourceAllowed(uint32_t id,int objects,int states,int frames,
                                     uint32_t vertices_hash,uint32_t indices_hash) {
    return id==0xfecafaceu && objects==1 && states==1 && frames==1 &&
           vertices_hash==0x9fb5ee98u && indices_hash==0x4d2f57c6u;
}
}
