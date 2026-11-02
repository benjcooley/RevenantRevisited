#pragma once
#include <cstdint>
// Exact shipped headfireball d000f005. Fingerprints admit the original records only.
namespace retail_headfireball_profile {
struct Object {const char*name;int material,vertices,faces;int keys[3];uint64_t key_hash[3],vertex_hash,face_hash;int face_counts[2];};
inline constexpr Object objects[]={
{"#hfire",0,4,2,{9,6,6},{0x197d332059d35187ull,0x60497081785fc31bull,0x60497081785fc31bull},0xaaa203f746306bb2ull,0x8de70ea438f81391ull,{0,2}},
{"hfire",1,0,0,{6,6,6},{0x26c66075deb51017ull,0x26c66075deb51017ull,0x26c66075deb51017ull},0x14650fb0739d0383ull,0x14650fb0739d0383ull,{0,0}},
{"sparks",1,0,0,{6,6,6},{0x0ae1da86ad5d4c1dull,0x0ae1da86ad5d4c1dull,0x0ae1da86ad5d4c1dull},0x14650fb0739d0383ull,0x14650fb0739d0383ull,{0,0}},
{"#sparks",0,4,2,{9,9,6},{0x4a2c1e343d1d252aull,0x4a2c1e343d1d252aull,0x2e1658c0ef656f66ull},0x4445f6ad3fe0b157ull,0x8de70ea438f81391ull,{0,2}},
{"#head",0,4,2,{28,23,20},{0x9307da5a29c8bce7ull,0x58b91f18ace94be9ull,0x32025bbeef9c4dd9ull},0x86252065c4f9a8ccull,0x8de70ea438f81391ull,{0,2}},
};
struct Tag {int state,frame;const char*name;const char*parameters;};
inline constexpr Tag tags[]={
{0,1,"blendcont","litadd"},
{1,24,"partsys","obj=(hfire),particle=#hfire,pps=[0:0,15:75],lifespan=5:20,initialvelocity=4:-4,scale=[0:0.5,100:3],rlocalrotation=[0:(0,0,0),100:(0,15,0)],color=[0:(255,255,255),15:(255,197,1),45:(255,126,1),65:(255,72,1),100:(10,1,1)],spread=60,azimuth=60,alpha=[0:1,100:0],friction=[0:0.5],emittersize=1,relvel=0.1"},
{2,38,"partsys","obj=(hfire),particle=#hfire,pps=[0:100,10:0],lifespan=5:20,initialvelocity=-5:-8,scale=[0:0.5,100:3],rlocalrotation=[0:(0,0,0),100:(0,35,0)],color=[0:(255,255,255),15:(255,197,1),45:(255,126,1),65:(255,72,1),100:(10,1,1)],spread=20,azimuth=20,friction=0.25,relvel=0.1"},
{2,42,"partsys","obj=(sparks),particle=#sparks,pps=[0:500,2:0],lifespan=10:20,initialvelocity=16:20,scale=[0:0.25,50:0.75,100:0.1],color=[0:(255,255,255),65:(255,200,1),100:(0,0,0)],spread=90,azimuth=90,alpha=[0:1,60:0.8,100:0],gravity=1.75,emittertype=circle,emittersize=1,bounce=-50:-50"},
};
inline constexpr uint64_t materials[]={0xd8e513ea1c37f4adull,0x8237bf10191e1de0ull};
inline constexpr int frames[]={20,15,55};
inline constexpr int flags[]={8192,8192,8192};
inline constexpr int widths[]={64};
inline constexpr int heights[]={64};
}
