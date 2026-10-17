#pragma once
#include <cstdint>
namespace pulp_profile {
struct Object {const char* name;int vertices,faces,slot,material;uint64_t vertex_hash,face_hash;};
inline constexpr Object objects[]={
{"chunk.lw",29,38,1,0,0x6da4df94b9bda4dfull,0xe3fa1524c1dd7e73ull},
{"box01",4,2,2,1,0x9555131c2243c2ebull,0xf44208a16a617ed8ull},
{"sphere01",21,10,3,2,0x32f3df49e44656c6ull,0xe60a35c9f05452cfull},
{"pyramid01",24,8,4,3,0xa445ffff263a9ea4ull,0xedf9443634501febull},
{"sphere02",34,24,1,0,0x8d1255ef54fd40dfull,0xeec63431e28a23f7ull},
{"pyramid02",24,8,1,0,0xf52511a2d398fee7ull,0xedf9443634501febull},
{"sphere03",34,24,1,0,0x1080a44c99d8e300ull,0xeec63431e28a23f7ull},
{"sphere04",21,10,3,2,0x5f931170c8880a33ull,0xe60a35c9f05452cfull},
{"geosphere0",13,8,4,3,0xf556125d6ac61450ull,0xf95289eca0e8d823ull},
{"prism01",30,12,3,2,0x2b779a494d7cb258ull,0x3218b12ffcbb98c8ull},
{"sphere05",21,10,4,3,0xe6d6712d96991590ull,0xe60a35c9f05452cfull},
};
}
