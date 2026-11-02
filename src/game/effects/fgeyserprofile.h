#pragma once
#include <cstdint>
// Exact shipped fgeyser ad92bd31 / Cavfgeyser; steam is a separate contract.
namespace retail_fgeyser {
struct Object {const char* name;int material,vertices,faces,keys;uint64_t key_hash,vertex_hash,face_hash;int face_counts[2];};
inline constexpr Object objects[]={
{"#fire",0,4,2,6,0x5b2b785049d078a7ull,0x02d715f2e5d713a4ull,0x8de70ea438f81391ull,{0,2}},
{"fgey",0,0,0,6,0x6eec9e9ca8cb36a6ull,0x14650fb0739d0383ull,0x14650fb0739d0383ull,{0,0}},
{"#sparks",0,4,2,6,0x5ab44d7034a8f927ull,0x9483d82b22f285a9ull,0x8de70ea438f81391ull,{0,2}},
{"fgeys",0,0,0,6,0x5b2b785049d078a7ull,0x14650fb0739d0383ull,0x14650fb0739d0383ull,{0,0}},
{"#glow",0,4,2,44,0xa82783e2e574c3a0ull,0x30cd96449d2dee94ull,0x8de70ea438f81391ull,{0,2}},
};
struct Tag {int state,frame;const char* name;const char* parameters;};
inline constexpr Tag tags[]={
{0,0,"blendcont","litadd"},
{0,1,"partsys","obj=(fgey),particle=#fire,pps=[0:0,5:75,10:100,20:45,30:0],lifespan=10:35,initialvelocity=10:15,scale=[0:0.25,100:2],rlocalrotation=[0:(0,0,0),100:(0,45,0)],color=[0:(255,255,255),15:(255,175,45),45:(255,125,1),70:(100,25,1),100:(0,0,0)],spread=10,alpha=[0:1,100:0],friction=[0:0.1],gravity=[0:0.3],emittertype=circle,emittersize=1"},
{0,50,"partsys","obj=(fgeys),particle=#sparks,pps=[0:0,5:50,7:0],lifespan=30:50,initialvelocity=-6:-10,scale=[0:0.25,50:0.75,100:0.1],color=[0:(255,255,255),65:(255,200,1),100:(0,0,0)],spread=45,azimuth=45,alpha=[0:1,60:0.8,100:0],gravity=1.75,friction=0,emittertype=circle,emittersize=1,bounce=-50:-50"},
};
inline constexpr uint64_t materials[]={0xd8e513ea1c37f4adull};
inline constexpr int material_textures[]={0};
}
