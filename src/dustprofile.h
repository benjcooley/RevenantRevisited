#pragma once
#include <cstdint>
// Exact shipped Dust 0c052637 / Dustcloud, with four root emitters.
namespace retail_dust {
struct Object {const char* name;int material,vertices,faces,keys;uint64_t key_hash,vertex_hash,face_hash;int face_counts[2];};
inline constexpr Object objects[]={
{"ground01",0,0,0,9,0x901941f2217a661eull,0x14650fb0739d0383ull,0x14650fb0739d0383ull,{0,0}},
{"#cloud",1,4,2,9,0xdd1683e62e64f810ull,0x557d240d9639e8dfull,0x8de70ea438f81391ull,{0,2}},
{"ground02",0,0,0,9,0xc49f5d7acafe4643ull,0x14650fb0739d0383ull,0x14650fb0739d0383ull,{0,0}},
{"ground03",0,0,0,9,0x02d1e07d43002fcdull,0x14650fb0739d0383ull,0x14650fb0739d0383ull,{0,0}},
{"ground04",0,0,0,9,0x8c6cb2fdb284f3efull,0x14650fb0739d0383ull,0x14650fb0739d0383ull,{0,0}},
};
struct Tag {int state,frame;const char* name;const char* parameters;};
inline constexpr Tag tags[]={
{0,1,"partsys","obj=(ground01,ground02,ground03,ground04),particle=#cloud,pps=[0:100,10:0,100:0],initialvelocity=3:4,lifespan=18:24,localrotation=[0:(0,0,0),100:(0,90,0)],friction=[0:0.8],gravity=[0:2,100:-4],color=[0:(40,30,25),100:(0,0,0)],spread=270,scale=[0:0.5,10:0.5,100:2.5],posjitter=0:0,scljitter=0.2:0.4,bounce=-100:-100"},
};
inline constexpr uint64_t materials[]={0x8237bf10191e1de0ull,0xaa234e3981f0b7b3ull};
inline constexpr int material_textures[]={-1,0};
}
