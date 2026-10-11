#pragma once
#include <cstdint>
// Exact original Funnel ad92bc17 resource structure and decoded-byte fingerprints.
namespace retail_funnel {
struct Object {const char* name;int material,vertices,faces;int keys[3];uint64_t key_hash[3],vertex_hash,face_hash;int face_counts[6];};
inline constexpr Object objects[]={
{"big gust",0,39,48,{27,38,27},{0x410fb986988905abull,0xae0eba699435902aull,0xb6cc3d3a8b428bf5ull},0x75957965cc92b5b5ull,0x98303f82896c375cull,{0,48,0,0,0,0}},
{"small gust",0,39,48,{27,38,27},{0x83e09f696ec54eb8ull,0x560ee5e607e4cda8ull,0x5910e647168281acull},0x84d39acf20fd2285ull,0x98303f82896c375cull,{0,48,0,0,0,0}},
{"main wind",1,165,276,{27,38,27},{0x8eedb34ff02fe7a2ull,0xe2eba61e31c7978full,0x702bb83e97648d4eull},0x78a9913061fc6604ull,0x385e056f5dcd4ce7ull,{0,0,276,0,0,0}},
{"center wind",2,25,36,{27,38,27},{0x3fb9870e60c3bfeaull,0xe49bd39cd8f3a2e8ull,0x6d9fdbbe7f158da6ull},0xa4fcbd74c5effe82ull,0xbed8a446b13adfabull,{0,0,0,36,0,0}},
{"#rect",3,4,2,{6,6,6},{0xb3f9dce3ebf2823bull,0xb3f9dce3ebf2823bull,0xb3f9dce3ebf2823bull},0x3381e4413e54ecb8ull,0x8de70ea438f81391ull,{0,0,0,0,2,0}},
{"cfire",0,0,0,{6,6,6},{0x3d2d5f0f381d1757ull,0x3d2d5f0f381d1757ull,0x3d2d5f0f381d1757ull},0x14650fb0739d0383ull,0x14650fb0739d0383ull,{0,0,0,0,0,0}},
{"#sparks",4,4,2,{6,6,6},{0xdd657805b1a4b156ull,0xdd657805b1a4b156ull,0xdd657805b1a4b156ull},0x3381e4413e54ecb8ull,0x8de70ea438f81391ull,{0,0,0,0,0,2}},
{"csparks",0,0,0,{6,6,6},{0x1a79063c2eb64f17ull,0x1a79063c2eb64f17ull,0x1a79063c2eb64f17ull},0x14650fb0739d0383ull,0x14650fb0739d0383ull,{0,0,0,0,0,0}},
};
struct Tag {int state,frame;const char* name;const char* parameters;};
inline constexpr Tag tags[]={
{0,1,"blendcont","litadd"},
{0,2,"partsys","obj=(cfire),particle=#rect,pps=30,lifespan=10:20,initialvelocity=-3:-5,scale=[0:2.5,25:1.5,100:0],rlocalrotation=[0:(0,0,0),100:(0,25,0)],color=[0:(255,255,255),15:(77,140,228),100:(1,1,1)],spread=45,azimuth=45,friction=[0:0.1],gravity=-0.75,rglobalrotation=[0:(0,-30,0),100:(0,30,0)]"},
{0,3,"partsys","obj=(csparks),particle=#sparks,pps=15,lifespan=20:30,initialvelocity=-8:-12,scale=[0:0.5,50:0.15,100:0.1],rlocalrotation=[0:(0,0,0),100:(0,36,0)],color=[0:(255,255,255),10:(77,141,230),100:(0,0,0)],spread=180,azimuth=180,friction=0.25,bounce=-50:-50,gravity=3"},
{1,11,"blendcont","litadd"},
{1,12,"partsys","obj=(cfire),particle=#rect,pps=30,lifespan=10:20,initialvelocity=-3:-5,scale=[0:2.5,25:1.5,100:0],rlocalrotation=[0:(0,0,0),100:(0,25,0)],color=[0:(255,255,255),15:(77,140,228),100:(1,1,1)],spread=45,azimuth=45,friction=[0:0.1],gravity=-0.75,rglobalrotation=[0:(0,-30,0),100:(0,30,0)]"},
{1,13,"partsys","obj=(csparks),particle=#sparks,pps=15,lifespan=20:30,initialvelocity=-8:-12,scale=[0:0.5,50:0.15,100:0.1],rlocalrotation=[0:(0,0,0),100:(0,36,0)],color=[0:(255,255,255),10:(77,141,230),100:(0,0,0)],spread=180,azimuth=180,friction=0.25,bounce=-50:-50,gravity=3"},
{2,41,"blendcont","litadd"},
};
inline constexpr uint64_t materials[]={0xaa234e3981f0b7b3ull,0xaa234e3981f0b7b3ull,0xbbb691aa526f15deull,0xd8e513ea1c37f4adull,0xd8e513ea1c37f4adull};
inline constexpr int frames[]={10,30,10};
inline constexpr int flags[]={8192,8192,8192};
inline constexpr int widths[]={128,128,64,64,16};
inline constexpr int heights[]={128,128,64,64,16};
}
