#pragma once
#include <cstdint>
// Exact installed Misc/cfire geometry and keys; missing filename presets are not synthesized.
namespace retail_cfire {
struct Object {const char* name;int material,vertices,faces,keys;uint64_t key_hash,vertex_hash,face_hash;int bins[3];};
inline constexpr Object objects[]={
{"#rect",0,4,2,6,0x5b2b785049d078a7ull,0x3381e4413e54ecb8ull,0x8de70ea438f81391ull,{0,2,0}},
{"cfire",1,0,0,6,0xe3ada3c09050332bull,0x14650fb0739d0383ull,0x14650fb0739d0383ull,{0,0,0}},
{"#sparks",2,4,2,6,0x9f6982dc6658a2e3ull,0x3381e4413e54ecb8ull,0x8de70ea438f81391ull,{0,0,2}},
{"csparks",1,0,0,6,0x6acf34c78fac0348ull,0x14650fb0739d0383ull,0x14650fb0739d0383ull,{0,0,0}},
};
inline constexpr uint64_t materials[]={0xd8e513ea1c37f4adull,0x8237bf10191e1de0ull,0xd8e513ea1c37f4adull};
inline constexpr int material_textures[]={0,-1,1};
}
