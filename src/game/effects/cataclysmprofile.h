#pragma once
#include <cstdint>
// Exact Cataclysm ad92bc19 geometry. Matrices/colors come from its custom leaf.
namespace retail_cataclysm {
struct Object {const char* name;int material,vertices,faces;uint64_t vertex_hash,face_hash;int face_counts[5];};
inline constexpr Object objects[]={
{"sphere01",0,106,132,0x699516e6398391bcull,0x58ecceed648c07a0ull,{0,132,0,0,0}},
{"torus01",1,136,256,0x4d7aafeb118ce1aaull,0xa6d913b7921543a3ull,{0,0,256,0,0}},
{"circle01",2,12,10,0x779aceb562a1c456ull,0x86ea3017cbaca355ull,{10,0,0,0,0}},
{"box01",3,4,2,0x078c5fb81757140bull,0xf44208a16a617ed8ull,{0,0,0,2,0}},
{"box02",4,4,2,0x808572cf788c0968ull,0xf44208a16a617ed8ull,{0,0,0,0,2}},
};
}
