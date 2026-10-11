#pragma once
#include <cstdint>
// Literal shipped identities; only exact named records enter this adapter.
namespace retail_static_particles {
struct Object {const char* name;int material,key_count;uint64_t keys;int vertices,faces;};
struct Profile {uint32_t id;const char* suffix;int frames,objects,materials,texture_size,tag_frame;
 const Object* object;const uint64_t* material;uint64_t vertices;const char* parameters;
 int ani_flags,total_vertices,total_faces,prototype,partsys_tag,blend_tag,blend_frame;};
inline constexpr Object regeneration_objects[]={
{"#particle",0,6,0x3825b2a582bb7cd1ull,4,2},
{"regen01",1,9,0x0d90e605c542d570ull,0,0},
{"regen04",1,9,0x97bbd93188d8dc0dull,0,0},
{"regen03",1,9,0x98f5384076d691d5ull,0,0},
{"regen06",1,9,0xa014d0201faca85eull,0,0},
{"regen07",1,9,0x7d2d783938dc62f2ull,0,0},
{"regen08",1,9,0xc3605699c292be81ull,0,0},
{"regen11",1,9,0xd7ba62fa5a3f0e86ull,0,0},
{"regen12",1,9,0x367cda506c2562edull,0,0},
{"regen15",1,9,0x5eb38bc0433b8513ull,0,0},
{"regen17",1,9,0xc5a3d7ffcce7779eull,0,0},
{"regen20",1,9,0xac08e091cff2a419ull,0,0},
{"regen22",1,9,0xd4de38a8d13c7fbbull,0,0},
{"regen23",1,9,0x05da22c4bb2fdee9ull,0,0},
{"regen27",1,9,0xe230b79600c81b91ull,0,0},
{"regen28",1,9,0x2f14cba6fce63b5cull,0,0},
{"regen30",1,9,0x2ef3347d8599d023ull,0,0},
{"regen32",1,9,0x72738e64e9944db2ull,0,0},
{"regen02",1,9,0x68276b8e0d880cbcull,0,0},
{"regen05",1,9,0x602df7d22255edbeull,0,0},
{"regen09",1,9,0xf7439dd583d85d85ull,0,0},
{"regen10",1,9,0x6c1a3ac2e0789434ull,0,0},
{"regen13",1,9,0x664e5978004888efull,0,0},
{"regen14",1,9,0x7ea4a6063f7799d2ull,0,0},
{"regen16",1,9,0x6e8db3e92966fc11ull,0,0},
{"regen18",1,9,0xa6d96da48b3430eeull,0,0},
{"regen19",1,9,0x00fcf36ced46660cull,0,0},
{"regen21",1,9,0xb2bfccc26f4bd6f0ull,0,0},
{"regen24",1,9,0xaf83e84985fa3507ull,0,0},
{"regen25",1,9,0xd39899aa34f2d6dbull,0,0},
{"regen26",1,9,0x857bc9a2efb4e85dull,0,0},
{"regen29",1,9,0x3a2cf9469b235f49ull,0,0},
{"regen31",1,9,0x5f8dbf66ddda734full,0,0},
{"rarmpad",2,6,0xd2c4f58f0af4da49ull,0,0},
};
inline constexpr uint64_t regeneration_materials[]={0xd8e513ea1c37f4adull,0x8237bf10191e1de0ull,0x51495411ce7c242full};
inline constexpr Object swiftstrike_objects[]={
{"#particle",0,6,0x3825b2a582bb7cd1ull,4,2},
{"regen01",1,9,0x2b5cf95101aace72ull,0,0},
{"regen04",1,9,0x640a0fc535b76d69ull,0,0},
{"regen03",1,9,0x7902d9962a7ab24bull,0,0},
{"regen06",1,9,0x9df2acfd102374abull,0,0},
{"regen07",1,9,0xb5cfed1bef9e78e4ull,0,0},
{"regen08",1,9,0x1d5d749ccb8763f8ull,0,0},
{"regen11",1,9,0x333acdda08cdda9dull,0,0},
{"regen12",1,9,0x118f7ef161b93ceaull,0,0},
{"regen15",1,9,0xe68709617f108d83ull,0,0},
{"regen17",1,9,0x1e00878830aeddf6ull,0,0},
{"regen20",1,9,0x06d9962ceb1ed74full,0,0},
{"regen22",1,9,0x106ce1c2f6772b6aull,0,0},
{"regen23",1,9,0xc656ec54fe8c16ffull,0,0},
{"regen27",1,9,0x99803abc8acf1477ull,0,0},
{"regen28",1,9,0xd11d57de7ca5fb29ull,0,0},
{"regen30",1,9,0x7d78410dce9c0257ull,0,0},
{"regen32",1,9,0x75e8b44309c8b2c1ull,0,0},
{"regen02",1,9,0x91007199c420a96dull,0,0},
{"regen05",1,9,0x7b74373cce9e5bc7ull,0,0},
{"regen09",1,9,0xc236f308959d5684ull,0,0},
{"regen10",1,9,0x2f940f314dcecea1ull,0,0},
{"regen13",1,9,0x2bf58f3496a1b079ull,0,0},
{"regen14",1,9,0x7e194f178d419abbull,0,0},
{"regen16",1,9,0x8341686fbb0053d9ull,0,0},
{"regen18",1,9,0xee9c4e4fd0c8b283ull,0,0},
{"regen19",1,9,0x5b1ce7141ba3b58eull,0,0},
{"regen21",1,9,0x5228ab3086cf33fcull,0,0},
{"regen24",1,9,0x61938a005e241e8full,0,0},
{"regen25",1,9,0x6aea117cfaee368dull,0,0},
{"regen26",1,9,0x3413704e1055579dull,0,0},
{"regen29",1,9,0x6b0ceb74d5e16fd8ull,0,0},
{"regen31",1,9,0x70b8845db4b6a864ull,0,0},
};
inline constexpr uint64_t swiftstrike_materials[]={0xd8e513ea1c37f4adull,0x8237bf10191e1de0ull};
inline constexpr Object fspray_objects[]={
{"#particle",0,6,0x3825b2a582bb7cd1ull,4,2},
{"spray",1,9,0x63861e19c8e2a9b1ull,0,0},
};
inline constexpr uint64_t fspray_materials[]={0xd8e513ea1c37f4adull,0x8237bf10191e1de0ull};
inline constexpr Object yenergy_objects[]={
{"#particle",0,9,0x18077238f721f4d8ull,4,2},
{"emitter",1,6,0xdb7529335d5fefd7ull,0,0},
};
inline constexpr uint64_t yenergy_materials[]={0x2d6e7c4f0d54f390ull,0x8237bf10191e1de0ull};
inline constexpr Object yenergylose_objects[]={
{"#particle",0,9,0x18077238f721f4d8ull,4,2},
{"emitter",1,6,0xdb7529335d5fefd7ull,0,0},
};
inline constexpr uint64_t yenergylose_materials[]={0x2d6e7c4f0d54f390ull,0x8237bf10191e1de0ull};
inline constexpr Object yabsorb_objects[]={
{"#particle",0,9,0xeb0073d4b2e3605cull,4,2},
{"emitter01",1,9,0xeef3d2cf9dcd67e0ull,0,0},
{"emitter02",1,9,0xcb233be0c091428dull,0,0},
{"emitter03",1,9,0x116be318f4be21ddull,0,0},
{"emitter04",1,9,0xaac121c885f9136aull,0,0},
{"emitter05",1,9,0x268b3c4b7d162285ull,0,0},
{"emitter06",1,9,0x5f720e03979f0d9bull,0,0},
{"emitter07",1,9,0x8c39b9642270e4a5ull,0,0},
{"emitter08",1,9,0x590c1b65de48ea96ull,0,0},
{"emitter09",1,9,0x4dba2de3a9c330a2ull,0,0},
{"emitter10",1,9,0xfe778a1f1ce0598eull,0,0},
{"emitter11",1,9,0x79ccb1cd8f5b6f35ull,0,0},
{"emitter12",1,9,0x326a4b21b13e5fe4ull,0,0},
{"emitter13",1,9,0x8491f28a38c92473ull,0,0},
{"emitter14",1,9,0x49effcb4fb9fe07bull,0,0},
{"emitter15",1,9,0xfc9aa5a557d7fa53ull,0,0},
{"emitter16",1,9,0x48d157c70851648eull,0,0},
{"emitter17",1,9,0x6e72b78a4b6801c8ull,0,0},
{"emitter18",1,9,0x7a8eaaab603367bcull,0,0},
{"emitter19",1,9,0x7c6bc9a03a8c65d2ull,0,0},
{"emitter20",1,6,0x748a9b3d6a91bd68ull,0,0},
{"emitter21",1,9,0xfbbd87471ff3f31full,0,0},
{"emitter22",1,9,0x57f8ea40c8bc1ef3ull,0,0},
{"emitter23",1,9,0x38353a51ff78e57bull,0,0},
{"emitter24",1,9,0x1c0cd90d0a832c93ull,0,0},
};
inline constexpr uint64_t yabsorb_materials[]={0xd255f18bf062cb9cull,0x8237bf10191e1de0ull};
inline constexpr Object nullifier_objects[]={
{"emitter01",0,33,0x9cf1f9dc4e51e560ull,0,0},
{"#particle",1,6,0xd1a1e085bc2acfefull,4,2},
{"emitter02",0,33,0x152f6f70b112a16bull,0,0},
{"emitter03",0,33,0x43b178d1444bdc4bull,0,0},
{"emitter04",0,33,0x2a599fa417f7fabfull,0,0},
{"emitter05",0,33,0xe199acfa24ee91eeull,0,0},
{"emitter06",0,33,0x09087d6c5ac33821ull,0,0},
{"emitter07",0,33,0x6e62eda366195e24ull,0,0},
{"emitter08",0,33,0x66b78223c3ad220bull,0,0},
{"*line01",0,9,0x7b78da8de30d3d8bull,46,0},
{"*line02",0,9,0x337f932273955d61ull,46,0},
{"*line03",0,9,0x8a1f81e396ddee8dull,46,0},
{"*line04",0,9,0x10f8a32c99deaafaull,46,0},
{"*line05",0,9,0x95cd91a9d80c22ffull,46,0},
{"*line06",0,9,0x07ada6bd001f11f1ull,46,0},
{"*line07",0,9,0x6a46e045da68964eull,46,0},
{"*line08",0,9,0xcf88625cae9d195dull,46,0},
};
inline constexpr uint64_t nullifier_materials[]={0x8237bf10191e1de0ull,0x49ab2bff2e73c095ull};
inline constexpr Profile profiles[]={{0x10ac03deu,"magic/regen.i3d",60,34,3,32,7,regeneration_objects,regeneration_materials,0x3c1a6c9954839ed7ull,"obj=(regen01,regen02,regen03,regen04,regen05,regen06,regen07,regen08,regen09,regen10,regen11,regen12,regen13,regen14,regen15,regen16,regen17,regen18,regen19,regen20,regen21,regen22,regen23,regen24,regen25,regen26,regen27,regen28,regen29,regen30,regen31,regen32),particle=#particle,pps=50,lifespan=18:18,initialvelocity=2:5,scale=[0:0.5,65:1,100:2],localrotation=[0:(0,0,0),100:(0,360,0)],color=[0:(50,50,50),100:(150,125,120)],friction=0.0",8193,4,2,0,0,-1,-1},
{0xe0a3bc43u,"magic/swiftstrike.i3d",60,33,2,32,7,swiftstrike_objects,swiftstrike_materials,0x3c1a6c9954839ed7ull,"obj=(regen01,regen02,regen03,regen04,regen05,regen06,regen07,regen08,regen09,regen10,regen11,regen12,regen13,regen14,regen15,regen16,regen17,regen18,regen19,regen20,regen21,regen22,regen23,regen24,regen25,regen26,regen27,regen28,regen29,regen30,regen31,regen32),particle=#particle,pps=50,lifespan=18:18,initialvelocity=2:5,scale=[0:0.5,65:1,100:2],localrotation=[0:(0,0,0),100:(0,360,0)],color=[0:(50,50,50),100:(150,125,120)]",8193,4,2,0,0,-1,-1},
{0x0c052638u,"misc/fspray.i3d",15,2,2,16,5,fspray_objects,fspray_materials,0x91b7cef666f68607ull,"obj=(spray),particle=#particle,pps=55,initialvelocity=16:19,lifespan=15:35,localrotation=[0:(0,0,0),100:(0,0,0)],friction=0,gravity=0.5,color=[0:(106,126,155),80:(32,85,55),100:(0,0,0)],relvel=0,spread=12,azimuth=12,scale=[0:0.25,20:1,100:0]",8193,4,2,0,0,-1,-1},
{0xaeaeeb30u,"magic/yenergy.i3d",30,2,2,64,2,yenergy_objects,yenergy_materials,0x470f987cb4d08e27ull,"obj=\"emitter\",particle=\"#particle\",emittertype=\"sphere\",emittersize=0.25,pps=45,initialvelocity=1:3,lifespan=50:60,color=[0:(255,255,255),35:(255,255,255),50:(0,0,0)],azimuth=180,spread=180,rlocalrotation=[0:(0,0,0),30:(0,5,0)],gravity=-0.3,scale=[0:0.5,10:3,40:2.5,60:0.5]",8193,4,2,0,0,1,2},
{0xaeaeeb33u,"magic/yenergylose.i3d",80,2,2,64,2,yenergylose_objects,yenergylose_materials,0x470f987cb4d08e27ull,"obj=\"emitter\",particle=\"#particle\",emittertype=\"sphere\",emittersize=0.25,pps=[0:45,7:45,10:0],initialvelocity=1:3,lifespan=50:60,color=[0:(255,255,255),35:(255,255,255),50:(0,0,0)],azimuth=180,spread=180,rlocalrotation=[0:(0,0,0),30:(0,5,0)],gravity=-0.3,scale=[0:0.5,10:3,40:2.5,60:0.5]",8192,4,2,0,1,0,2},
{0xaeaeeb29u,"magic/yabsorb.i3d",120,25,2,64,2,yabsorb_objects,yabsorb_materials,0x754ca8b49706dca3ull,"obj=(\"emitter01\",\"emitter02\",\"emitter03\",\"emitter04\",\"emitter05\",\"emitter06\",\"emitter07\",\"emitter08\",\"emitter09\",\"emitter10\",\"emitter11\",\"emitter12\",\"emitter13\",\"emitter14\",\"emitter15\",\"emitter16\",\"emitter17\",\"emitter18\",\"emitter19\",\"emitter20\",\"emitter21\",\"emitter22\",\"emitter23\",\"emitter24\"),particle=\"#particle\",emittertype=\"sphere\",emittersize=5,scale=[0:0,50:2,60:0],pps=[0:15,30:25,65:0],initialvelocity=2:4,lifespan=40:40,color=[0:(255,255,255)]",8192,4,2,0,1,0,2},
{0xad92bd38u,"magic/nullifier.i3d",25,17,2,64,2,nullifier_objects,nullifier_materials,0xdff5eac4de952848ull,"obj=(emitter01,emitter02,emitter03,emitter04,emitter05,emitter06,emitter07,emitter08),particle=#particle,pps=[0:75,17:150,25:300],initialvelocity=2:5,lifespan=5:10,friction=[0:0.3],gravity=[0:-2],color=[0:(125,185,20),100:(0,0,0)],scale=[0:0.5,100:0]",8193,372,2,1,0,-1,-1}};
inline bool IsType(uint32_t id){for(const auto& p:profiles)if(p.id==id)return true;return false;}
}
