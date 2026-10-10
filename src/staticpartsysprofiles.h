#pragma once
#include <cstdint>
// Literal shipped identities for three constant-emitter, particle-only profiles.
namespace retail_static_particles {
struct Object {const char* name;int material,key_count;uint64_t keys;};
struct Profile {uint32_t id;const char* suffix;int frames,objects,materials,texture_size,tag_frame;
 const Object* object;const uint64_t* material;uint64_t vertices;const char* parameters;};
inline constexpr Object regeneration_objects[]={
{"#particle",0,6,0x3825b2a582bb7cd1ull},
{"regen01",1,9,0x0d90e605c542d570ull},
{"regen04",1,9,0x97bbd93188d8dc0dull},
{"regen03",1,9,0x98f5384076d691d5ull},
{"regen06",1,9,0xa014d0201faca85eull},
{"regen07",1,9,0x7d2d783938dc62f2ull},
{"regen08",1,9,0xc3605699c292be81ull},
{"regen11",1,9,0xd7ba62fa5a3f0e86ull},
{"regen12",1,9,0x367cda506c2562edull},
{"regen15",1,9,0x5eb38bc0433b8513ull},
{"regen17",1,9,0xc5a3d7ffcce7779eull},
{"regen20",1,9,0xac08e091cff2a419ull},
{"regen22",1,9,0xd4de38a8d13c7fbbull},
{"regen23",1,9,0x05da22c4bb2fdee9ull},
{"regen27",1,9,0xe230b79600c81b91ull},
{"regen28",1,9,0x2f14cba6fce63b5cull},
{"regen30",1,9,0x2ef3347d8599d023ull},
{"regen32",1,9,0x72738e64e9944db2ull},
{"regen02",1,9,0x68276b8e0d880cbcull},
{"regen05",1,9,0x602df7d22255edbeull},
{"regen09",1,9,0xf7439dd583d85d85ull},
{"regen10",1,9,0x6c1a3ac2e0789434ull},
{"regen13",1,9,0x664e5978004888efull},
{"regen14",1,9,0x7ea4a6063f7799d2ull},
{"regen16",1,9,0x6e8db3e92966fc11ull},
{"regen18",1,9,0xa6d96da48b3430eeull},
{"regen19",1,9,0x00fcf36ced46660cull},
{"regen21",1,9,0xb2bfccc26f4bd6f0ull},
{"regen24",1,9,0xaf83e84985fa3507ull},
{"regen25",1,9,0xd39899aa34f2d6dbull},
{"regen26",1,9,0x857bc9a2efb4e85dull},
{"regen29",1,9,0x3a2cf9469b235f49ull},
{"regen31",1,9,0x5f8dbf66ddda734full},
{"rarmpad",2,6,0xd2c4f58f0af4da49ull},
};
inline constexpr uint64_t regeneration_materials[]={0xd8e513ea1c37f4adull,0x8237bf10191e1de0ull,0x51495411ce7c242full};
inline constexpr Object swiftstrike_objects[]={
{"#particle",0,6,0x3825b2a582bb7cd1ull},
{"regen01",1,9,0x2b5cf95101aace72ull},
{"regen04",1,9,0x640a0fc535b76d69ull},
{"regen03",1,9,0x7902d9962a7ab24bull},
{"regen06",1,9,0x9df2acfd102374abull},
{"regen07",1,9,0xb5cfed1bef9e78e4ull},
{"regen08",1,9,0x1d5d749ccb8763f8ull},
{"regen11",1,9,0x333acdda08cdda9dull},
{"regen12",1,9,0x118f7ef161b93ceaull},
{"regen15",1,9,0xe68709617f108d83ull},
{"regen17",1,9,0x1e00878830aeddf6ull},
{"regen20",1,9,0x06d9962ceb1ed74full},
{"regen22",1,9,0x106ce1c2f6772b6aull},
{"regen23",1,9,0xc656ec54fe8c16ffull},
{"regen27",1,9,0x99803abc8acf1477ull},
{"regen28",1,9,0xd11d57de7ca5fb29ull},
{"regen30",1,9,0x7d78410dce9c0257ull},
{"regen32",1,9,0x75e8b44309c8b2c1ull},
{"regen02",1,9,0x91007199c420a96dull},
{"regen05",1,9,0x7b74373cce9e5bc7ull},
{"regen09",1,9,0xc236f308959d5684ull},
{"regen10",1,9,0x2f940f314dcecea1ull},
{"regen13",1,9,0x2bf58f3496a1b079ull},
{"regen14",1,9,0x7e194f178d419abbull},
{"regen16",1,9,0x8341686fbb0053d9ull},
{"regen18",1,9,0xee9c4e4fd0c8b283ull},
{"regen19",1,9,0x5b1ce7141ba3b58eull},
{"regen21",1,9,0x5228ab3086cf33fcull},
{"regen24",1,9,0x61938a005e241e8full},
{"regen25",1,9,0x6aea117cfaee368dull},
{"regen26",1,9,0x3413704e1055579dull},
{"regen29",1,9,0x6b0ceb74d5e16fd8ull},
{"regen31",1,9,0x70b8845db4b6a864ull},
};
inline constexpr uint64_t swiftstrike_materials[]={0xd8e513ea1c37f4adull,0x8237bf10191e1de0ull};
inline constexpr Object fspray_objects[]={
{"#particle",0,6,0x3825b2a582bb7cd1ull},
{"spray",1,9,0x63861e19c8e2a9b1ull},
};
inline constexpr uint64_t fspray_materials[]={0xd8e513ea1c37f4adull,0x8237bf10191e1de0ull};
inline constexpr Profile profiles[]={{0x10ac03deu,"magic/regen.i3d",60,34,3,32,7,regeneration_objects,regeneration_materials,0x3c1a6c9954839ed7ull,"obj=(regen01,regen02,regen03,regen04,regen05,regen06,regen07,regen08,regen09,regen10,regen11,regen12,regen13,regen14,regen15,regen16,regen17,regen18,regen19,regen20,regen21,regen22,regen23,regen24,regen25,regen26,regen27,regen28,regen29,regen30,regen31,regen32),particle=#particle,pps=50,lifespan=18:18,initialvelocity=2:5,scale=[0:0.5,65:1,100:2],localrotation=[0:(0,0,0),100:(0,360,0)],color=[0:(50,50,50),100:(150,125,120)],friction=0.0"},
{0xe0a3bc43u,"magic/swiftstrike.i3d",60,33,2,32,7,swiftstrike_objects,swiftstrike_materials,0x3c1a6c9954839ed7ull,"obj=(regen01,regen02,regen03,regen04,regen05,regen06,regen07,regen08,regen09,regen10,regen11,regen12,regen13,regen14,regen15,regen16,regen17,regen18,regen19,regen20,regen21,regen22,regen23,regen24,regen25,regen26,regen27,regen28,regen29,regen30,regen31,regen32),particle=#particle,pps=50,lifespan=18:18,initialvelocity=2:5,scale=[0:0.5,65:1,100:2],localrotation=[0:(0,0,0),100:(0,360,0)],color=[0:(50,50,50),100:(150,125,120)]"},
{0x0c052638u,"misc/fspray.i3d",15,2,2,16,5,fspray_objects,fspray_materials,0x91b7cef666f68607ull,"obj=(spray),particle=#particle,pps=55,initialvelocity=16:19,lifespan=15:35,localrotation=[0:(0,0,0),100:(0,0,0)],friction=0,gravity=0.5,color=[0:(106,126,155),80:(32,85,55),100:(0,0,0)],relvel=0,spread=12,azimuth=12,scale=[0:0.25,20:1,100:0]"}};
inline bool IsType(uint32_t id){for(const auto& p:profiles)if(p.id==id)return true;return false;}
}
