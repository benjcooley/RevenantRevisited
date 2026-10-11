#pragma once
#include <cstdint>
// Exact shipped MPAppear ad99fdf2 / Magic/appear.i3d.
namespace retail_mpappear {
struct Object {const char* name;int material,vertices,faces;int keys[1];uint64_t key_hash[1],vertex_hash,face_hash;int face_counts[5];};
inline constexpr Object objects[]={
{"main pulse",0,22,20,{225},{0x5ca5af897f337e4bull},0xe681fc56e0cf2277ull,0xdb9ab368aa7ee7a2ull,{0,20,0,0,0}},
{"#$star",1,4,2,{59},{0x06fc643bf823f89bull},0xabc48f2fed1eefdaull,0x8de70ea438f81391ull,{0,0,2,0,0}},
{"second pulse",2,22,20,{242},{0xd33104f7c2ac7b62ull},0x106551bfdab480d8ull,0xdb9ab368aa7ee7a2ull,{0,0,0,20,0}},
{"skirt01",3,22,20,{220},{0x3f85f68a04176316ull},0x9b537559372567dcull,0xdb9ab368aa7ee7a2ull,{0,0,0,0,20}},
{"skirt02",3,22,20,{216},{0x86f4521e17e63288ull},0xc9bdfbbc8d3e0b0dull,0xdb9ab368aa7ee7a2ull,{0,0,0,0,20}},
{"skirt03",3,22,20,{214},{0xe2899f80250c5114ull},0x05f9cbd22e726ffbull,0xdb9ab368aa7ee7a2ull,{0,0,0,0,20}},
{"rectangle01",1,4,2,{177},{0x1dded56c45d96427ull},0x537fda5cd1d89193ull,0x8de70ea438f81391ull,{0,0,2,0,0}},
};
inline constexpr uint64_t materials[]={0xaa234e3981f0b7b3ull,0xaa234e3981f0b7b3ull,0xaa234e3981f0b7b3ull,0xaa234e3981f0b7b3ull};
inline constexpr int material_textures[]={0,1,2,3};
}
