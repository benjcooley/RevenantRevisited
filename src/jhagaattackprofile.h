#pragma once
#include <cstdint>
// Exact shipped JhagaAttack aeaeeb32. Fingerprints admit the original records only.
namespace retail_jhagaattack_profile {
struct Object {const char*name;int material,vertices,faces;int keys[3];uint64_t key_hash[3],vertex_hash,face_hash;int face_counts[2];};
inline constexpr Object objects[]={
{"#hfire",0,4,2,{9,6,6},{0x197d332059d35187ull,0x60497081785fc31bull,0x60497081785fc31bull},0xaaa203f746306bb2ull,0x8de70ea438f81391ull,{0,2}},
{"hfire",1,0,0,{6,6,6},{0x5b2b785049d078a7ull,0x5b2b785049d078a7ull,0x5b2b785049d078a7ull},0x14650fb0739d0383ull,0x14650fb0739d0383ull,{0,0}},
{"sparks",1,0,0,{6,6,6},{0x2081f0be8548f3e6ull,0x2081f0be8548f3e6ull,0x2081f0be8548f3e6ull},0x14650fb0739d0383ull,0x14650fb0739d0383ull,{0,0}},
{"#sparks",0,4,2,{9,9,6},{0x4a2c1e343d1d252aull,0x4a2c1e343d1d252aull,0x2e1658c0ef656f66ull},0x4445f6ad3fe0b157ull,0x8de70ea438f81391ull,{0,2}},
{"#$head",0,4,2,{28,23,20},{0xfaf6d0ba6b7c2f53ull,0x967f6e62c567a6c5ull,0xd1a1a8e6eba1c25dull},0xa4d58feac5abfe06ull,0x8de70ea438f81391ull,{0,2}},
};
struct Tag {int state,frame;const char*name;const char*parameters;};
inline constexpr Tag tags[]={
{0,7,"blendcont","litadd"},
{1,24,"partsys","obj=(hfire),particle=#hfire,pps=[0:0,15:75],lifespan=5:20,initialvelocity=4:-4,scale=[0:0.5,100:3],rlocalrotation=[0:(0,0,0),100:(0,15,0)],color=[0:(255,255,255),15:(178,147,228),45:(110,51,202),65:(75,35,138),100:(10,1,1)],spread=60,azimuth=60,alpha=[0:1,100:0],friction=[0:0.5],emittersize=1,relvel=0.1"},
{2,38,"partsys","obj=(hfire),particle=#hfire,pps=[0:100,10:0],lifespan=5:20,initialvelocity=-5:-8,scale=[0:0.5,100:3],rlocalrotation=[0:(0,0,0),100:(0,35,0)],color=[0:(255,255,255),15:(178,147,228),45:(110,51,202),65:(75,35,138),100:(10,1,1)],spread=20,azimuth=20,friction=0.25,relvel=0.1"},
{2,42,"partsys","obj=(sparks),particle=#sparks,pps=[0:400,2:0],lifespan=30:45,initialvelocity=16:20,scale=[0:0.25,50:0.75,100:0.1],color=[0:(255,255,255),15:(178,147,228),45:(110,51,202),65:(75,35,138),100:(10,1,1)],spread=90,azimuth=90,alpha=[0:1,60:0.8,100:0],gravity=1.75,emittertype=circle,emittersize=1,bounce=-50:-50"},
};
inline constexpr uint64_t materials[]={0xaa234e3981f0b7b3ull,0x8237bf10191e1de0ull};
inline constexpr int frames[]={20,15,55};
inline constexpr int flags[]={8192,8192,8192};
inline constexpr int widths[]={64};
inline constexpr int heights[]={64};
}
