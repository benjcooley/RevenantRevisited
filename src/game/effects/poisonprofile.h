#pragma once
#include <cstdint>
// Exact shipped poison e0a3bc42 / PoisonCloud, with four root emitters.
namespace retail_poison {
struct Object {const char* name;int material,vertices,faces,keys;uint64_t key_hash,vertex_hash,face_hash;int face_counts[3];};
inline constexpr Object objects[]={
{"#bubble",0,4,2,6,0x74e23d481e3d70faull,0xd31f2ae73bbf0fc3ull,0x8de70ea438f81391ull,{0,2,0}},
{"#pcloud",1,4,2,9,0xdd1683e62e64f810ull,0x14683fd6ec3bb063ull,0x8de70ea438f81391ull,{0,0,2}},
{"ground01",2,0,0,9,0x5045dcfdd17df2f5ull,0x14650fb0739d0383ull,0x14650fb0739d0383ull,{0,0,0}},
{"ground02",2,0,0,9,0xbf36ca8ba6de8a48ull,0x14650fb0739d0383ull,0x14650fb0739d0383ull,{0,0,0}},
{"ground03",2,0,0,9,0x5eb94c7d56fa06ddull,0x14650fb0739d0383ull,0x14650fb0739d0383ull,{0,0,0}},
{"ground04",2,0,0,9,0x9e9b6908b8e898bbull,0x14650fb0739d0383ull,0x14650fb0739d0383ull,{0,0,0}},
};
struct Tag {int state,frame;const char* name;const char* parameters;};
inline constexpr Tag tags[]={
{0,6,"partsys","obj=(ground01,ground02,ground03,ground04),particle=#pcloud,pps=20,initialvelocity=2:3,lifespan=10:20,localrotation=[0:(0,0,0),100:(0,90,0)],friction=[0:1.1],gravity=[0:-1.5,100:0],color=[0:(25,150,1),100:(0,0,0)],spread=270,scale=[0:0.5,10:0.5,100:3],posjitter=0:0,scljitter=0.2:0.4"},
{0,15,"partsys","obj=(ground01,ground02,ground03,ground04),particle=#bubble,pps=30,initialvelocity=2:3,relvel=0.1,lifespan=10:25,localrotation=[0:(0,0,0),100:(0,540,0)],friction=[0:0.1],gravity=[0:-0.2],color=[0:(0,0,0),30:(25,150,1),100:(0,0,0)],spread=60,azimuth=60,scale=[0:0.75,50:2,80:0.5,100:0],scljitter=0.5:1"},
};
inline constexpr uint64_t materials[]={0x84ef911a6ccadbbbull,0xaa234e3981f0b7b3ull,0x8237bf10191e1de0ull};
inline constexpr int material_textures[]={0,1,-1};
}
