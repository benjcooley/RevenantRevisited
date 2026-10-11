#pragma once
#include "object.h"
#include "3dimage.h"
#include <cctype>
#include <cstring>
#include <string>
#include <vector>
namespace generic_static_profile {
struct Object{const char*name;int material,vertices,faces,keys;uint64_t key_hash,vertex_hash,face_hash;int counts[6];};
struct Texture{uint32_t width,height,masks[5];};
struct Profile{uint32_t id;const char*path;int states,frames,objects,materials,textures;const Object*object;const Texture*texture;const uint64_t*material_hash;int key_bytes=4;};
inline constexpr Object charutility_objects[]={
{"shadow",0,4,2,9,0xb7659ee754def8e5ull,0xe0d2d56e83783a7dull,0xf44208a16a617ed8ull,{0,2,0,0,0,0}},
{"comflash",1,4,2,9,0xe38b064e4fce0d67ull,0x1ee85f906011721dull,0xf44208a16a617ed8ull,{0,0,2,0,0,0}},
{"vision",2,12,10,9,0xfcb6b3decd423564ull,0x846784cf51764ff4ull,0xf626eaa575966988ull,{0,0,0,10,0,0}},
{"flamef",3,4,2,9,0xe15d9b524e5bea9eull,0x17c6ce41d892b6f7ull,0x8de70ea438f81391ull,{0,0,0,0,2,0}},
{"comring",4,26,24,9,0xd7bbd152014abf57ull,0x1ee09d09bcbf20e2ull,0x1d0f36a13ceb15ebull,{0,0,0,0,0,24}}
};
inline constexpr Texture charutility_textures[]={{64,64,{0x10,0xf00,0xf0,0xf,0xf000}},{64,64,{0x10,0xf800,0x7e0,0x1f,0x0}},{128,128,{0x10,0xf00,0xf0,0xf,0xf000}},{64,64,{0x10,0xf800,0x7e0,0x1f,0x0}},{64,64,{0x10,0xf800,0x7e0,0x1f,0x0}}};
inline constexpr uint64_t charutility_materials[]={0x8534d7c3db82ed8aull,0xaa234e3981f0b7b3ull,0xccd0f646e4775d9dull,0xaa234e3981f0b7b3ull,0xaa234e3981f0b7b3ull};
inline constexpr Object kinsecretdoor_objects[]={
{"wall",0,231,260,9,0x677a74ff34068f29ull,0xfae95daf5dd777ebull,0x093be6b844fb80beull,{0,158,102}}
};
inline constexpr Texture kinsecretdoor_textures[]={{256,256,{0x10,0xf800,0x7e0,0x1f,0x0}},{128,128,{0x10,0xf800,0x7e0,0x1f,0x0}}};
inline constexpr uint64_t kinsecretdoor_materials[]={0x3bde87235cffd7bdull,0x156fcec510a5b023ull};
inline constexpr Object snow_objects[]={
{"cube",0,24,12,100,0xdd8c7137a396f42bull,0x64c5562b8ade0448ull,0x4412aadc4bb9c18bull,{0,12,0}},
{"face01",0,6,2,100,0xddc7a7494f1ac303ull,0x3b4fb42c1b235703ull,0x3dc5eb8effa2cb62ull,{0,2,0}},
{"face02",0,4,2,100,0xddc7a7494f1ac303ull,0x5ebfceff16bc8935ull,0xf44208a16a617ed8ull,{0,2,0}},
{"face03",0,6,2,100,0xddc7a7494f1ac303ull,0xe42e1978a144ca65ull,0x3dc5eb8effa2cb62ull,{0,2,0}},
{"face04",0,4,2,100,0xddc7a7494f1ac303ull,0xe38e2cb920f07766ull,0x882662629d42e980ull,{0,2,0}},
{"face05",0,4,2,100,0xddc7a7494f1ac303ull,0x8e4e37020ecf8043ull,0x882662629d42e980ull,{0,2,0}},
{"face06",0,6,2,100,0xddc7a7494f1ac303ull,0x62bd67a24234940dull,0x612788f22824b4faull,{0,2,0}},
{"face07",0,6,2,100,0xddc7a7494f1ac303ull,0x5f3d0388c4c3bc7cull,0xbec5d08e4ae93dd2ull,{0,2,0}},
{"face08",0,4,2,100,0xddc7a7494f1ac303ull,0xc3537553bc887759ull,0xc257a831adf65568ull,{0,2,0}},
{"face09",0,6,2,100,0xddc7a7494f1ac303ull,0xdd89bef3d43f0691ull,0x3dc5eb8effa2cb62ull,{0,2,0}},
{"face10",0,6,2,100,0xddc7a7494f1ac303ull,0xb78814fda1878c48ull,0x3dc5eb8effa2cb62ull,{0,2,0}},
{"face11",0,6,2,100,0xddc7a7494f1ac303ull,0x500e12f4cb163c65ull,0x612788f22824b4faull,{0,2,0}},
{"face12",0,6,2,100,0xddc7a7494f1ac303ull,0x101db3b5a0ea2d66ull,0x3dc5eb8effa2cb62ull,{0,2,0}},
{"face13",0,6,2,100,0xddc7a7494f1ac303ull,0x7b9941647551722bull,0xe2276df1736b276aull,{0,2,0}},
{"face14",0,6,2,100,0xddc7a7494f1ac303ull,0xf724d2d110bd17f0ull,0xe2276df1736b276aull,{0,2,0}},
{"face15",0,6,2,100,0xddc7a7494f1ac303ull,0x144debdd282341dcull,0xe2276df1736b276aull,{0,2,0}},
{"face16",0,6,2,100,0xddc7a7494f1ac303ull,0x979c2ff1877c0925ull,0x3dc5eb8effa2cb62ull,{0,2,0}},
{"face17",0,6,2,100,0xddc7a7494f1ac303ull,0xf7aee35b304e2b60ull,0x3dc5eb8effa2cb62ull,{0,2,0}},
{"face18",0,6,2,100,0xddc7a7494f1ac303ull,0x46baf0c2543ddd90ull,0x3dc5eb8effa2cb62ull,{0,2,0}},
{"face19",0,6,2,100,0xddc7a7494f1ac303ull,0xb05959ba18f15a3eull,0xbec5d08e4ae93dd2ull,{0,2,0}},
{"face20",0,6,2,100,0xddc7a7494f1ac303ull,0x59627af93aa5e4ecull,0xe2276df1736b276aull,{0,2,0}},
{"face21",0,6,2,100,0xddc7a7494f1ac303ull,0xb03d6cc13ca4b60aull,0xe2276df1736b276aull,{0,2,0}},
{"face22",0,6,2,100,0xddc7a7494f1ac303ull,0x1a5b8caab7752963ull,0x3dc5eb8effa2cb62ull,{0,2,0}},
{"face23",0,6,2,100,0xddc7a7494f1ac303ull,0xbcef2a465a407fdbull,0xe2276df1736b276aull,{0,2,0}},
{"face24",0,6,2,100,0xddc7a7494f1ac303ull,0x39e7fe483c69b8a0ull,0x3dc5eb8effa2cb62ull,{0,2,0}},
{"face25",0,6,2,100,0xddc7a7494f1ac303ull,0x4fcc69b7c39d9c2cull,0x3dc5eb8effa2cb62ull,{0,2,0}},
{"face26",0,6,2,100,0xddc7a7494f1ac303ull,0xa95f62af31c8e9e9ull,0x3dc5eb8effa2cb62ull,{0,2,0}},
{"face27",0,6,2,100,0xddc7a7494f1ac303ull,0x5ca0ddb5aef5f786ull,0xe2276df1736b276aull,{0,2,0}},
{"face28",0,6,2,100,0xddc7a7494f1ac303ull,0x9714149eceb18952ull,0x3dc5eb8effa2cb62ull,{0,2,0}},
{"face29",0,6,2,100,0xddc7a7494f1ac303ull,0x603342da384b3a89ull,0x612788f22824b4faull,{0,2,0}},
{"face30",0,6,2,100,0xddc7a7494f1ac303ull,0xded0ab91f44ce0daull,0xe2276df1736b276aull,{0,2,0}},
{"face31",0,6,2,100,0xddc7a7494f1ac303ull,0x184e1163c39b48aaull,0xe2276df1736b276aull,{0,2,0}},
{"face32",0,6,2,100,0xddc7a7494f1ac303ull,0xb09165700d3189ebull,0x3dc5eb8effa2cb62ull,{0,2,0}},
{"face33",0,6,2,100,0xddc7a7494f1ac303ull,0xc2c85f53ffb5ebe3ull,0x3dc5eb8effa2cb62ull,{0,2,0}},
{"face34",0,6,2,100,0xddc7a7494f1ac303ull,0x3ee4570fddb1baeeull,0xe2276df1736b276aull,{0,2,0}},
{"face35",0,6,2,100,0xddc7a7494f1ac303ull,0xf86f30de2633d4a2ull,0x3dc5eb8effa2cb62ull,{0,2,0}},
{"face36",0,4,2,100,0xddc7a7494f1ac303ull,0x98cb43b4059635f0ull,0xa91d8d6a3ebc3558ull,{0,2,0}},
{"face37",0,6,2,100,0xddc7a7494f1ac303ull,0xb6cb46064c80293bull,0x3dc5eb8effa2cb62ull,{0,2,0}},
{"face38",0,6,2,100,0xddc7a7494f1ac303ull,0x672388db1c33fe8eull,0xe2276df1736b276aull,{0,2,0}},
{"face39",0,6,2,100,0xddc7a7494f1ac303ull,0x1bb9a7e91fad459cull,0x3dc5eb8effa2cb62ull,{0,2,0}},
{"face40",0,6,2,100,0xddc7a7494f1ac303ull,0x36ff1ce383dbc37dull,0xe2276df1736b276aull,{0,2,0}},
{"face41",0,4,2,100,0xddc7a7494f1ac303ull,0xcf1ddee5aa3e4002ull,0x28290cd4dd04f640ull,{0,2,0}},
{"face42",0,4,2,100,0xddc7a7494f1ac303ull,0x12584bc543ee13fbull,0xc257a831adf65568ull,{0,2,0}},
{"face43",0,4,2,100,0xddc7a7494f1ac303ull,0xade765e062ab5237ull,0x85bf5607163d2ee9ull,{0,2,0}},
{"icicle02",1,4,2,100,0xddc7a7494f1ac303ull,0x28938656f176efddull,0x28290cd4dd04f640ull,{0,0,2}},
{"icicle01",1,4,2,100,0xddc7a7494f1ac303ull,0x3584c309710b8582ull,0x28290cd4dd04f640ull,{0,0,2}},
{"icicle03",1,4,2,100,0xddc7a7494f1ac303ull,0x427dfb5ff8b2451dull,0x28290cd4dd04f640ull,{0,0,2}},
{"icicle04",1,4,2,100,0xddc7a7494f1ac303ull,0xddfbe0b79fa704deull,0x28290cd4dd04f640ull,{0,0,2}},
{"icicle05",1,4,2,100,0xddc7a7494f1ac303ull,0x1938e606ac951984ull,0x28290cd4dd04f640ull,{0,0,2}},
{"icicle06",1,4,2,100,0xddc7a7494f1ac303ull,0xf1d9f7a95f1979d1ull,0x28290cd4dd04f640ull,{0,0,2}}
};
inline constexpr Texture snow_textures[]={{128,128,{0x10,0xf00,0xf0,0xf,0xf000}},{64,64,{0x10,0xf00,0xf0,0xf,0xf000}}};
inline constexpr uint64_t snow_materials[]={0xabd68d398362b740ull,0xaa234e3981f0b7b3ull};
inline constexpr Profile profiles[]={{0xadbcef13u,"misc/charutility.i3d",1,100,5,5,5,charutility_objects,charutility_textures,charutility_materials},
{0xad92bd23u,"misc/secretdoor.i3d",3,2,1,2,2,kinsecretdoor_objects,kinsecretdoor_textures,kinsecretdoor_materials},
{0xfacedbedu,"magic/iced.i3d",1,100,50,2,2,snow_objects,snow_textures,snow_materials,8}};
inline const Profile*Find(uint32_t id){for(const auto&p:profiles)if(p.id==id)return &p;return nullptr;}
inline uint64_t Hash(const void*ptr,size_t size){uint64_t h=1469598103934665603ull;const auto*b=static_cast<const uint8_t*>(ptr);for(size_t i=0;i<size;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline bool Matches(const TObjectInstance*owner,T3DImagery*im){
 if(!owner||!im||owner->ObjClass()!=OBJCLASS_EFFECT||owner->GetState()!=0)return false;const auto*p=Find(owner->ObjId());if(!p||!im->GetResFilename())return false;
 if(im->NumStates()!=p->states||im->GetAniLength(0)!=p->frames||im->GetAniFlags(0)!=0x2001||im->NumObjects()!=p->objects||im->NumMaterials()!=p->materials||im->NumTextures()!=p->textures||im->NumTags()!=0)return false;
 std::string path(im->GetResFilename());for(char&c:path)c=c=='\\'?'/' : char(std::tolower(static_cast<unsigned char>(c)));const size_t n=std::strlen(p->path);if(path.size()<n||path.compare(path.size()-n,n,p->path)||(path.size()!=n&&path[path.size()-n-1]!='/'))return false;
 for(int i=0;i<p->objects;++i){const auto&o=p->object[i];S3DObj actual={};im->GetObject(i,&actual);if(std::strcmp(actual.name,o.name)||actual.material!=o.material||actual.parent[0]!=-1||actual.numanikeys[0]!=o.keys||!actual.anikeys[0]||Hash(actual.anikeys[0],o.keys*size_t(p->key_bytes))!=o.key_hash||im->NumObjVerts(i)!=o.vertices||im->NumObjFaces(i)!=o.faces||im->IsHidden(i,0))return false;
  int starts[MAXTEXTURES+1]={},counts[MAXTEXTURES+1]={};std::vector<S3DVertex>vertices(size_t(o.vertices));std::vector<S3DFace>faces(size_t(o.faces));im->GetObjVerts(i,vertices.data(),0,0);im->GetObjFaces(i,faces.data(),starts,counts);for(int t=0;t<=p->textures;++t)if(counts[t]!=o.counts[t])return false;if(Hash(vertices.data(),vertices.size()*sizeof(S3DVertex))!=o.vertex_hash||Hash(faces.data(),faces.size()*sizeof(S3DFace))!=o.face_hash)return false;
 }
 for(int i=0;i<p->textures;++i){S3DTex t={};im->GetTexture(i,&t);const auto&pf=t.desc.pixelFormat;const auto&e=p->texture[i];if(t.numframes!=1||t.desc.width!=e.width||t.desc.height!=e.height||pf.dwRGBBitCount!=e.masks[0]||pf.dwRBitMask!=e.masks[1]||pf.dwGBitMask!=e.masks[2]||pf.dwBBitMask!=e.masks[3]||pf.dwRGBAlphaBitMask!=e.masks[4])return false;}
 for(int i=0;i<p->materials;++i){S3DMat m={};im->GetMaterial(i,&m);if(m.texture!=i||Hash(&m.matdesc.diffuse,17*sizeof(float))!=p->material_hash[i])return false;}
 return true;
}
}
