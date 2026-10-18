#pragma once
#include <cstdint>
// Exact shipped LightStrip 10da54d0; hashes pin decoded records, not replacement art.
namespace retail_lightstrip {
struct Object {const char* name;int material,vertices,faces;int keys[3];uint64_t key_hash[3],vertex_hash,face_hash;int face_counts[3];};
inline constexpr Object objects[]={
{"#flare",0,4,2,{18,39,20},{0xee1fde8852dbc720ull,0x06eeda9aa329bf89ull,0xed5049da5beb5517ull},0x402c5a2349f5fad3ull,0x8de70ea438f81391ull,{0,2,0}},
{"rectangle08",1,4,2,{27,69,19},{0x827f614a974a3052ull,0x272c82b18cc2153cull,0x7f0de0ee8c5783c8ull},0x23b40e2f66594089ull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle07",1,4,2,{27,69,19},{0x2a39c6adf08a4a71ull,0x1cb7676d960e25e9ull,0x532c67e0ca8d6329ull},0x1c7a9cdee0adc700ull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle02",1,4,2,{27,69,19},{0xac76846d933dabdbull,0xed20f3d28abf9326ull,0xeaf5aa7556791be3ull},0xfda5ed9ebbf3e554ull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle09",1,4,2,{27,69,19},{0x2b0d5135d9702ec7ull,0xbd662a6eb429d493ull,0x5a358f0225e7c16dull},0x580f5b7fc4e783a9ull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle03",1,4,2,{27,69,19},{0xf81339d0b1b457ddull,0xee93588139938b6cull,0xb06f242eb43723adull},0x2262df5954526171ull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle05",1,4,2,{27,69,19},{0xb7057d3b7308dd5dull,0xbcd2c0d20038e057ull,0xe025c52f9874ea1dull},0x96552f97663c8a4cull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle11",1,4,2,{27,69,19},{0x318cb65f7a120100ull,0xdc4c6c50dc7a07cdull,0xbae35a712da8eeb1ull},0xf28ea67efe97f6a2ull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle10",1,4,2,{27,71,19},{0x9fd26b876b18294cull,0x5d845cf03f38a17full,0xe3c571cc4da62ad3ull},0xe8c5096e90915b18ull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle06",1,4,2,{27,69,19},{0x0d80808179e336daull,0xf02f4e9fa9d7b899ull,0xda64e62fec407bb1ull},0xfe718758a0680508ull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle04",1,4,2,{27,69,19},{0xad05f90bc76ea64bull,0x2b3fb96e811170d1ull,0xe66ebd45059238e6ull},0x5546cd003887d4baull,0x8de70ea438f81391ull,{0,0,2}},
{"#sparks",0,4,2,{6,6,6},{0x9f6982dc6658a2e3ull,0x9f6982dc6658a2e3ull,0x9f6982dc6658a2e3ull},0xcee4d361b685ef75ull,0x8de70ea438f81391ull,{0,2,0}},
{"csparks",1,0,0,{6,6,6},{0x85d6c9dd895f5620ull,0x85d6c9dd895f5620ull,0x85d6c9dd895f5620ull},0x14650fb0739d0383ull,0x14650fb0739d0383ull,{0,0,0}},
};
struct Tag {int state,frame;const char* name;const char* parameters;};
inline constexpr Tag tags[]={
{0,1,"partsys","obj=(csparks),particle=#sparks,pps=25,lifespan=15:30,initialvelocity=1:2,scale=[0:0.1,50:1.5,100:0.1],rlocalrotation=[0:(0,0,0),100:(0,40,0)],color=[0:(255,255,255),10:(150,33,240),100:(0,0,0)],spread=360,azimuth=360,friction=[0:0.025],gravity=0.15,bounce=-25:-25,scljitter=-1:1"},
{0,2,"blendcont","litaddz"},
{1,11,"partsys","obj=(csparks),particle=#sparks,pps=25,lifespan=15:30,initialvelocity=1:2,scale=[0:0.1,25:1.5,100:0.1],rlocalrotation=[0:(0,0,0),100:(0,40,0)],color=[0:(255,255,255),10:(150,33,240),100:(0,0,0)],spread=360,azimuth=360,friction=[0:0.025],gravity=0.15,bounce=-25:-25,scljitter=-1:1"},
{2,42,"partsys","obj=(csparks),particle=#sparks,pps=[0:275,2:0],lifespan=25:30,initialvelocity=15:30,scale=[0:1.5,100:0.1],rlocalrotation=[0:(0,0,0),100:(0,40,0)],color=[0:(160,50,255),100:(0,0,0)],spread=360,azimuth=360,friction=[0:0.075],gravity=0.15,scljitter=-1:1"},
};
inline constexpr uint64_t materials[]={0xaa234e3981f0b7b3ull,0xaa234e3981f0b7b3ull};
inline constexpr int frames[]={10,31,29};
inline constexpr int flags[]={8192,8192,8192};
inline constexpr int widths[]={64,128};
inline constexpr int heights[]={64,128};
}
