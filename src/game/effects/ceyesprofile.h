#pragma once
#include <cstdint>
// Exact shipped CEyes 0c052639 / Magic/ceyes.i3d.
namespace retail_ceyes {
struct Object {const char* name;int material,vertices,faces;int keys[9];uint64_t key_hash[9],vertex_hash,face_hash;int face_counts[2];};
inline constexpr Object objects[]={
{"#eyes01",0,4,2,{32,9,9,9,9,9,9,9,9},{0x7fa4bd09a85b8d16ull,0x8e184884f623971dull,0x8e184884f623971dull,0x8e184884f623971dull,0x8e184884f623971dull,0x8e184884f623971dull,0x8e184884f623971dull,0x8e184884f623971dull,0x8e184884f623971dull},0xbe2d90f64c7e94c8ull,0x8de70ea438f81391ull,{0,2}},
{"#eyes02",0,4,2,{9,9,9,33,9,9,9,9,9},{0x8e184884f623971dull,0x8e184884f623971dull,0x8e184884f623971dull,0xec789bd422cda1edull,0x8e184884f623971dull,0x8e184884f623971dull,0x8e184884f623971dull,0x8e184884f623971dull,0x8e184884f623971dull},0xe01528adf1f5a822ull,0x8de70ea438f81391ull,{0,2}},
{"#eyes03",0,4,2,{9,9,9,9,9,22,9,9,9},{0x8e184884f623971dull,0x8e184884f623971dull,0x8e184884f623971dull,0x8e184884f623971dull,0x8e184884f623971dull,0x22243b8c16883209ull,0x8e184884f623971dull,0x8e184884f623971dull,0x8e184884f623971dull},0xcb518d963aa7c238ull,0x8de70ea438f81391ull,{0,2}},
{"#eyes04",0,4,2,{9,9,9,9,9,9,9,44,9},{0x8e184884f623971dull,0x8e184884f623971dull,0x8e184884f623971dull,0x8e184884f623971dull,0x8e184884f623971dull,0x8e184884f623971dull,0x8e184884f623971dull,0x95c49e8466ee2f1dull,0xf2e4f65ec1fbebe5ull},0x7ccc08e943b28e07ull,0x8de70ea438f81391ull,{0,2}},
};
inline constexpr uint64_t materials[]={0xaa234e3981f0b7b3ull};
inline constexpr int material_textures[]={0};
}
