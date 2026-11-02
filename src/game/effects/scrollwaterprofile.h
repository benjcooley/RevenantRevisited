#pragma once
#include "game/object/object.h"
#include "game/imagery/3dimage.h"
#include <cctype>
#include <cstring>
#include <string>
#include <vector>
namespace scroll_water_profile {
struct Profile{uint32_t id;const char*path,*object;int vertices,faces;const char*parameters;int frames,keys,tags;uint64_t vertex_hash,face_hash,key_hash,material_hash;};
inline constexpr Profile profiles[]={
{0xadbcef15u,"misc/flowwater.i3d","waterstr",4,2,"obj=water,du=0,dv=0.01",100,6,1,0x5d49693d21184f2bull,0x28290cd4dd04f640ull,0x5b2b785049d078a7ull,0x2d6e7c4f0d54f390ull},
{0xadbcef16u,"misc/bendwater1.i3d","waterbend",28,36,"obj=water,du=0,dv=0.01 ",100,6,1,0x689f0b092f979f2dull,0x9cb1ef5ae61314b3ull,0x5b2b785049d078a7ull,0x2d6e7c4f0d54f390ull},
{0xadbcef17u,"misc/bendwater2.i3d","waterbend",28,36,"obj=water,du=0,dv=-0.01 ",100,6,1,0x689f0b092f979f2dull,0x9cb1ef5ae61314b3ull,0x5b2b785049d078a7ull,0x2d6e7c4f0d54f390ull},
{0xadbcef19u,"misc/sewerw.i3d","box02",4,2,"",1,9,0,0x5ed98b844e7bd053ull,0x28290cd4dd04f640ull,0x2d2f1ce48e120923ull,0xaa234e3981f0b7b3ull},
{0x9d92bc1du,"misc/wave.i3d","wave",38,47,"obj=wave,du=0,dv=-0.005",1,6,1,0x4a0b958712e25d38ull,0xd0ab47dc836c4c89ull,0x5b2b785049d078a7ull,0xfa1022423e58b839ull},
{0x9d92bc1eu,"misc/waves.i3d","wave02",38,47,"obj=wave,du=0,dv=-0.005",1,6,1,0x6347ba9c8ac6d141ull,0xd0ab47dc836c4c89ull,0x5b2b785049d078a7ull,0xfa1022423e58b839ull},
{0x9d92bc1fu,"misc/wavem.i3d","wave01",38,47,"obj=wave01,du=0,dv=-0.005",1,6,1,0xa95812e11d94ac6bull,0xd0ab47dc836c4c89ull,0x5b2b785049d078a7ull,0xfa1022423e58b839ull},
{0xd0c0f03au,"misc/riverfall.i3d","water",14,12,"obj=water,du=0,dv=0.01 ",100,9,1,0xafe840ae77f0a17eull,0xf59ec0416de17b8aull,0x0857b73768e383a1ull,0x2d6e7c4f0d54f390ull}};
inline const Profile* Find(uint32_t id){for(const auto& p:profiles)if(p.id==id)return &p;return nullptr;}
inline uint64_t Hash(const void* ptr,size_t size){uint64_t h=1469598103934665603ull;const auto* data=static_cast<const uint8_t*>(ptr);for(size_t i=0;i<size;++i){h^=data[i];h*=1099511628211ull;}return h;}
inline bool Matches(const TObjectInstance* owner,T3DImagery* imagery)
{
 if(!owner || !imagery || owner->ObjClass()!=OBJCLASS_EFFECT || owner->GetState()!=0)return false;
 const auto* p=Find(owner->ObjId());if(!p || !imagery->GetResFilename())return false;
 if(imagery->NumStates()!=1 || imagery->GetAniFlags(0)!=0x2001 || imagery->GetAniLength(0)!=p->frames ||
    imagery->NumObjects()!=1 || imagery->NumMaterials()!=1 || imagery->NumTextures()!=1 || imagery->NumTags()!=p->tags ||
    imagery->NumObjVerts(0)!=p->vertices || imagery->NumObjFaces(0)!=p->faces || imagery->IsHidden(0,0))return false;
 std::string path(imagery->GetResFilename());for(char& c:path)c=c=='\\' ? '/' : char(std::tolower(static_cast<unsigned char>(c)));
 const size_t length=std::strlen(p->path);
 if(path.size()<length || path.compare(path.size()-length,length,p->path) || (path.size()!=length && path[path.size()-length-1]!='/'))return false;
 S3DObj object={};imagery->GetObject(0,&object);
 if(std::strcmp(object.name,p->object) || object.material!=0 || object.parent[0]!=-1 || object.numanikeys[0]!=p->keys ||
    !object.anikeys[0] || Hash(object.anikeys[0],p->keys*sizeof(SAniKey32))!=p->key_hash)return false;
 if(p->tags){const auto* tag=imagery->GetTag(0);
 if(!tag || tag->state!=0 || tag->frame!=0 || !tag->name || !tag->str || std::strcmp(tag->name,"scrolltex") || std::strcmp(tag->str,p->parameters))return false;}
 S3DTex texture={};imagery->GetTexture(0,&texture);const auto& format=texture.desc.pixelFormat;
 if(texture.numframes!=1 || texture.desc.width!=128 || texture.desc.height!=128 || format.dwRGBBitCount!=16 ||
    format.dwRBitMask!=0xf00 || format.dwGBitMask!=0xf0 || format.dwBBitMask!=0xf || format.dwRGBAlphaBitMask!=0xf000)return false;
 S3DMat material={};imagery->GetMaterial(0,&material);
 if(material.texture!=0 || Hash(&material.matdesc.diffuse,17*sizeof(float))!=p->material_hash)return false;
 std::vector<S3DVertex> vertices(size_t(p->vertices));std::vector<S3DFace> faces(size_t(p->faces));
 int starts[MAXTEXTURES+1]={},counts[MAXTEXTURES+1]={};
 imagery->GetObjVerts(0,vertices.data(),0,0);imagery->GetObjFaces(0,faces.data(),starts,counts);
 if(counts[0]!=0 || counts[1]!=p->faces || Hash(vertices.data(),vertices.size()*sizeof(S3DVertex))!=p->vertex_hash ||
    Hash(faces.data(),faces.size()*sizeof(S3DFace))!=p->face_hash)return false;
 return imagery->HasScrollTex()==bool(p->tags);
}
}
