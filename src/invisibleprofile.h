#pragma once
#include "object.h"
#include "3dimage.h"
#include <cctype>
#include <cstring>
#include <string>
#include <vector>
namespace invisible_profile {
struct Object{const char*name;int material,vertices,faces,keys;uint64_t key_hash,vertex_hash,face_hash;int counts[3];};
inline uint64_t Hash(const void*p,size_t n){uint64_t h=1469598103934665603ull;for(auto*b=static_cast<const uint8_t*>(p);n--;){h^=*b++;h*=1099511628211ull;}return h;}
inline constexpr Object objects[]={
{"box02",0,12,12,6,0x296d593c958fc3ecull,0x031af4960ec07803ull,0x7b43abebc3c7a23bull,{0,12,0}},
{"box01",1,4,2,9,0x57ae1b85859ac6d2ull,0xe1ba5e6f44b16707ull,0xf44208a16a617ed8ull,{0,0,2}}
};
inline bool Matches(T3DImagery*im){
 if(!im||!im->GetResFilename()||im->NumStates()!=1||im->GetAniLength(0)!=100||im->GetAniFlags(0)!=0x2001||im->NumObjects()!=2||im->NumMaterials()!=2||im->NumTextures()!=2||im->NumTags()!=0)return false;
 const auto*body=static_cast<const S3DImageryBody*>(im->GetBody());if(!body||body->flags!=0xfc||body->version!=2)return false;
 std::string path(im->GetResFilename());for(char&c:path)c=c=='\\'?'/' :char(std::tolower(static_cast<unsigned char>(c)));constexpr auto suffix="magic/invisible.i3d";const size_t n=std::strlen(suffix);if(path.size()<n||path.compare(path.size()-n,n,suffix)||(path.size()!=n&&path[path.size()-n-1]!='/'))return false;
 for(int i=0;i<2;++i){S3DObj o={};im->GetObject(i,&o);const auto&e=objects[i];if(std::strcmp(o.name,e.name)||o.material!=e.material||o.parent[0]!=-1||o.numanikeys[0]!=e.keys||(e.keys&&!o.anikeys[0])||Hash(o.anikeys[0],size_t(e.keys)*4)!=e.key_hash||im->NumObjVerts(i)!=e.vertices||im->NumObjFaces(i)!=e.faces)return false;std::vector<S3DVertex>v(size_t(e.vertices));im->GetObjVerts(i,v.data());if(Hash(v.data(),v.size()*sizeof(S3DVertex))!=e.vertex_hash)return false;std::vector<S3DFace>f(size_t(e.faces));int starts[MAXTEXTURES+1]={},counts[MAXTEXTURES+1]={};im->GetObjFaces(i,f.data(),starts,counts);if(Hash(f.data(),f.size()*sizeof(S3DFace))!=e.face_hash)return false;for(int t=0;t<3;++t)if(o.numtexfaces[t]!=e.counts[t])return false;}
 constexpr uint64_t materials[]={0xd8e513ea1c37f4adull,0xd8e513ea1c37f4adull};for(int i=0;i<2;++i){S3DMat m={};im->GetMaterial(i,&m);if(m.texture!=i||Hash(&m.matdesc.diffuse,17*sizeof(float))!=materials[i])return false;}
 {S3DTex t={};im->GetTexture(0,&t);const auto&f=t.desc.pixelFormat;if(t.numframes!=1||t.desc.width!=64||t.desc.height!=64||f.dwRGBBitCount!=16||f.dwRBitMask!=3840u||f.dwGBitMask!=240u||f.dwBBitMask!=15u||f.dwRGBAlphaBitMask!=61440u)return false;}
 {S3DTex t={};im->GetTexture(1,&t);const auto&f=t.desc.pixelFormat;if(t.numframes!=1||t.desc.width!=64||t.desc.height!=64||f.dwRGBBitCount!=16||f.dwRBitMask!=63488u||f.dwGBitMask!=2016u||f.dwBBitMask!=31u||f.dwRGBAlphaBitMask!=0u)return false;}
 return true;
}
}
