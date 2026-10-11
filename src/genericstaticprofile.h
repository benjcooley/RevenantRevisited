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
struct Profile{uint32_t id;const char*path;int states,frames,objects,materials,textures;const Object*object;const Texture*texture;const uint64_t*material_hash;};
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
inline constexpr Profile profiles[]={{0xadbcef13u,"misc/charutility.i3d",1,100,5,5,5,charutility_objects,charutility_textures,charutility_materials},
{0xad92bd23u,"misc/secretdoor.i3d",3,2,1,2,2,kinsecretdoor_objects,kinsecretdoor_textures,kinsecretdoor_materials}};
inline const Profile*Find(uint32_t id){for(const auto&p:profiles)if(p.id==id)return &p;return nullptr;}
inline uint64_t Hash(const void*ptr,size_t size){uint64_t h=1469598103934665603ull;const auto*b=static_cast<const uint8_t*>(ptr);for(size_t i=0;i<size;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline bool Matches(const TObjectInstance*owner,T3DImagery*im){
 if(!owner||!im||owner->ObjClass()!=OBJCLASS_EFFECT||owner->GetState()!=0)return false;const auto*p=Find(owner->ObjId());if(!p||!im->GetResFilename())return false;
 if(im->NumStates()!=p->states||im->GetAniLength(0)!=p->frames||im->GetAniFlags(0)!=0x2001||im->NumObjects()!=p->objects||im->NumMaterials()!=p->materials||im->NumTextures()!=p->textures||im->NumTags()!=0)return false;
 std::string path(im->GetResFilename());for(char&c:path)c=c=='\\'?'/' : char(std::tolower(static_cast<unsigned char>(c)));const size_t n=std::strlen(p->path);if(path.size()<n||path.compare(path.size()-n,n,p->path)||(path.size()!=n&&path[path.size()-n-1]!='/'))return false;
 for(int i=0;i<p->objects;++i){const auto&o=p->object[i];S3DObj actual={};im->GetObject(i,&actual);if(std::strcmp(actual.name,o.name)||actual.material!=o.material||actual.parent[0]!=-1||actual.numanikeys[0]!=o.keys||!actual.anikeys[0]||Hash(actual.anikeys[0],o.keys*sizeof(SAniKey32))!=o.key_hash||im->NumObjVerts(i)!=o.vertices||im->NumObjFaces(i)!=o.faces||im->IsHidden(i,0))return false;
  int starts[MAXTEXTURES+1]={},counts[MAXTEXTURES+1]={};std::vector<S3DVertex>vertices(size_t(o.vertices));std::vector<S3DFace>faces(size_t(o.faces));im->GetObjVerts(i,vertices.data(),0,0);im->GetObjFaces(i,faces.data(),starts,counts);for(int t=0;t<=p->textures;++t)if(counts[t]!=o.counts[t])return false;if(Hash(vertices.data(),vertices.size()*sizeof(S3DVertex))!=o.vertex_hash||Hash(faces.data(),faces.size()*sizeof(S3DFace))!=o.face_hash)return false;
 }
 for(int i=0;i<p->textures;++i){S3DTex t={};im->GetTexture(i,&t);const auto&pf=t.desc.pixelFormat;const auto&e=p->texture[i];if(t.numframes!=1||t.desc.width!=e.width||t.desc.height!=e.height||pf.dwRGBBitCount!=e.masks[0]||pf.dwRBitMask!=e.masks[1]||pf.dwGBitMask!=e.masks[2]||pf.dwBBitMask!=e.masks[3]||pf.dwRGBAlphaBitMask!=e.masks[4])return false;}
 for(int i=0;i<p->materials;++i){S3DMat m={};im->GetMaterial(i,&m);if(m.texture!=i||Hash(&m.matdesc.diffuse,17*sizeof(float))!=p->material_hash[i])return false;}
 return true;
}
}
