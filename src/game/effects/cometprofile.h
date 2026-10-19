#pragma once
#include <cstdint>
// Exact shipped MeteorStorm f32bcfac / Comet, not the snapshot storm.
namespace retail_comet {
struct Object {const char* name;int material,vertices,faces,keys;uint64_t key_hash,vertex_hash,face_hash;int face_counts[3];};
inline constexpr Object objects[]={
{"#comet",0,4,2,6,0xcc9481aaaa85740full,0xaaa203f746306bb2ull,0x8de70ea438f81391ull,{0,2,0}},
{"comet",1,0,0,32,0xe5421775c4f1e0ceull,0x14650fb0739d0383ull,0x14650fb0739d0383ull,{0,0,0}},
{"#sparks",0,4,2,6,0xdcd9006c6e5c808dull,0x9483d82b22f285a9ull,0x8de70ea438f81391ull,{0,2,0}},
{"fgeys",0,0,0,6,0x5b2b785049d078a7ull,0x14650fb0739d0383ull,0x14650fb0739d0383ull,{0,0,0}},
{"#$head",0,4,2,46,0x7a9023935b0c96deull,0xe9ff8a4e0b273fb6ull,0x8de70ea438f81391ull,{0,2,0}},
{"#trail",0,4,2,6,0xe12d8d960c2d7f91ull,0x13ccb6dce946ce72ull,0x8de70ea438f81391ull,{0,2,0}},
{"head",0,0,0,46,0x26347bfd3cd35c76ull,0x14650fb0739d0383ull,0x14650fb0739d0383ull,{0,0,0}},
{"shock01",2,4,2,33,0x0dc18baca7d81e67ull,0xd5b42e7945925b57ull,0x381ca01c1b911d31ull,{0,0,2}},
};
struct Tag {int state,frame;const char* name;const char* parameters;};
inline constexpr Tag tags[]={
{0,3,"blendcont","litaddz"},
{0,10,"partsys","obj=(comet),particle=#comet,pps=[0:50,15:150,20:0],lifespan=5:20,initialvelocity=4:-4,scale=[0:0.5,100:3],rlocalrotation=[0:(0,0,0),100:(0,15,0)],color=[0:(255,255,255),15:(255,197,1),45:(255,126,1),65:(255,72,1),100:(10,1,1)],spread=60,azimuth=60,alpha=[0:1,100:0],friction=[0:0.5],emittersize=1,bounce=-80:-80,relvel=0.1"},
{0,18,"partsys","obj=(head),particle=#trail,pps=[0:100,10:100,15:0],lifespan=5:5,initialvelocity=1:1,scale=[0:2,100:0],color=[0:(255,255,255),100:(0,0,0)],spread=1"},
{0,27,"partsys","obj=(fgeys),particle=#sparks,pps=[10:0,11:150,12:0],lifespan=10:18,initialvelocity=-15:-25,scale=[0:0.25,50:1,100:0.1],color=[0:(255,255,255),100:(0,0,0)],spread=180,azimuth=180,gravity=1,friction=0.1,bounce=-50:-50"},
};
inline constexpr uint64_t materials[]={0xd8e513ea1c37f4adull,0x8237bf10191e1de0ull,0xaa234e3981f0b7b3ull};
inline constexpr int material_textures[]={0,-1,1};
}
