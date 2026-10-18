#pragma once
#include <cstdint>
// Exact Rocks.I3D geometry and authored constant poses, type ad92bd22.
namespace retail_rockstorm {
struct Object{const char* name;int material,vertices,faces,keys;uint64_t vertex_hash,face_hash,key_hash;int face_counts[4];};
inline constexpr Object objects[]={
{"sphere01",0,54,56,6,0x640f2c9335c1399bull,0xbf1ec8ccb19cea04ull,0x3f2d5e018aab58a2ull,{0,56,0,0}},
{"sphere02",0,24,8,6,0xad7d6f4fc8d5466eull,0x4c605aae7d5ec91bull,0x84bbc56989ffe9d5ull,{0,8,0,0}},
{"sphere03",0,144,48,9,0xea8cec70b2ab909full,0x1da4db1ac5a6e4cbull,0x04b8ccf7fd31bff7ull,{0,48,0,0}},
{"rectangle0",1,18,12,9,0x53f66ebd8f67a362ull,0x1b316895fb30a364ull,0xbffe299053c64ec2ull,{0,0,12,0}},
{"rectangle1",2,18,12,9,0x156cfe2901739986ull,0x14486645227fe924ull,0x8ec9d58f823b94ddull,{0,0,0,12}},
};
}
