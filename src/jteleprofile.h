#pragma once
#include "object.h"
#include "3dimage.h"
#include <cctype>
#include <cstring>
#include <string>
#include <vector>
namespace jtele_profile {
struct Object{const char*name;int material,vertices,faces,keys;uint64_t key_hash,vertex_hash,face_hash;int starts[3],counts[3];};
struct Profile{uint32_t id;const char*path;const Object*objects;};
inline uint64_t Hash(const void*p,size_t n){uint64_t h=1469598103934665603ull;for(auto*b=static_cast<const uint8_t*>(p);n--;){h^=*b++;h*=1099511628211ull;}return h;}
inline constexpr Object jtele_objects[]={
{"#trail",0,4,2,6,0x90fd8a23ba5db2bbull,0x59b0d2af51091d3cull,0x8de70ea438f81391ull,{0,0,2},{0,2,0}},
{"*helix01",1,161,0,55,0x044bd442a50b2f02ull,0xe90460256bc90b1eull,0x14650fb0739d0383ull,{2,2,2},{0,0,0}},
{"#ogre01",0,4,2,147,0xa543886560979d91ull,0xa9a547715fd2842full,0x8de70ea438f81391ull,{2,2,4},{0,2,0}},
{"#ogre02",0,4,2,141,0xaf7eda9cb514a07aull,0xa9a547715fd2842full,0x8de70ea438f81391ull,{4,4,6},{0,2,0}},
{"#ogre03",0,4,2,130,0xf8a10e8cebd6f7b4ull,0xa9a547715fd2842full,0x8de70ea438f81391ull,{6,6,8},{0,2,0}},
{"teleport",2,22,20,105,0x4490fabf2f13ce19ull,0x1627aecf9d9264c7ull,0xdb9ab368aa7ee7a2ull,{8,8,8},{0,0,20}},
{"core",2,22,20,103,0x6280ce1ebe9fbc18ull,0x0fa8c6dac914016full,0xdb9ab368aa7ee7a2ull,{28,28,28},{0,0,20}},
{"rectangle01",0,4,2,57,0x484a4d1c5b79b2c7ull,0xf26a29d8dbbb0ff6ull,0x8de70ea438f81391ull,{48,48,50},{0,2,0}},
};
inline constexpr Object jteled_objects[]={
{"#trail",0,4,2,6,0xfce92c07833adefbull,0x59b0d2af51091d3cull,0x8de70ea438f81391ull,{0,0,2},{0,2,0}},
{"*helix01",1,161,0,55,0x8542c393314f486dull,0x12609a7fe8163e32ull,0x14650fb0739d0383ull,{2,2,2},{0,0,0}},
{"#ogre01",0,4,2,127,0x03302c0163c0087bull,0x9aab318513260a03ull,0x8de70ea438f81391ull,{2,2,4},{0,2,0}},
{"#ogre02",0,4,2,141,0xf54a6fa60d0cedc4ull,0x9aab318513260a03ull,0x8de70ea438f81391ull,{4,4,6},{0,2,0}},
{"#ogre03",0,4,2,150,0x3d741e5887ad06deull,0x9aab318513260a03ull,0x8de70ea438f81391ull,{6,6,8},{0,2,0}},
{"teleport",2,22,20,105,0xa1a32d01122d537eull,0x2c4a7d508964e87eull,0xdb9ab368aa7ee7a2ull,{8,8,8},{0,0,20}},
{"core",2,22,20,103,0x3a636979d27d797bull,0x0fa8c6dac914016full,0xdb9ab368aa7ee7a2ull,{28,28,28},{0,0,20}},
{"rectangle01",0,4,2,57,0xfeb94eebbb6eefe8ull,0xf26a29d8dbbb0ff6ull,0x8de70ea438f81391ull,{48,48,50},{0,2,0}},
};
inline constexpr Profile profiles[]={{0x0c05263bu,"magic/jtele.i3d",jtele_objects},{0x0c05263cu,"magic/jteled.i3d",jteled_objects}};
inline const Profile*Find(uint32_t id){for(const auto&p:profiles)if(p.id==id)return &p;return nullptr;}
inline constexpr auto particle_tag="obj=(#ogre01,#ogre02,#ogre03),particle=#trail,pps=125,lifespan=5:8,initialvelocity=0:2,scale=[0:1.25,100:0],localrotation=[0:(0,0,0),100:(0,15,0)],color=[0:(155,200,255),2:(75,100,255),100:(1,1,1)],spread=360,azimuth=360,alpha=[0:1,100:0],gravity=1,friction=0.25,bounce=-50:-50,scljitter=-0.5:0.5,relvel=0.01";
inline bool Matches(uint32_t id,T3DImagery*im){
 const auto*p=Find(id);if(!p||!im||!im->GetResFilename()||im->NumStates()!=1||im->GetAniLength(0)!=50||im->GetAniFlags(0)!=0x2000||im->NumObjects()!=8||im->NumMaterials()!=3||im->NumTextures()!=2||im->NumTags()!=3)return false;
 std::string path(im->GetResFilename());for(char&c:path)c=c=='\\'?'/' :char(std::tolower(static_cast<unsigned char>(c)));const size_t n=std::strlen(p->path);if(path.size()<n||path.compare(path.size()-n,n,p->path)||(path.size()!=n&&path[path.size()-n-1]!='/'))return false;
 for(int i=0;i<8;++i){S3DObj o={};im->GetObject(i,&o);const auto&e=p->objects[i];if(std::strcmp(o.name,e.name)||o.material!=e.material||o.parent[0]!=-1||!o.anikeys[0]||o.numanikeys[0]!=e.keys||Hash(o.anikeys[0],e.keys*4)!=e.key_hash||im->NumObjVerts(i)!=e.vertices||im->NumObjFaces(i)!=e.faces)return false;std::vector<S3DVertex>v(size_t(e.vertices));im->GetObjVerts(i,v.data());if(Hash(v.data(),v.size()*sizeof(S3DVertex))!=e.vertex_hash)return false;std::vector<S3DFace>f(size_t(e.faces));int starts[MAXTEXTURES+1]={},counts[MAXTEXTURES+1]={};if(e.faces)im->GetObjFaces(i,f.data(),starts,counts);if(Hash(f.data(),f.size()*sizeof(S3DFace))!=e.face_hash)return false;for(int t=0;t<3;++t)if(o.numtexfaces[t]!=e.counts[t])return false;}
 constexpr uint64_t materials[]={0xaa234e3981f0b7b3ull,0x8237bf10191e1de0ull,0xaa234e3981f0b7b3ull};constexpr int textures[]={0,-1,1};for(int i=0;i<3;++i){S3DMat m={};im->GetMaterial(i,&m);if(m.texture!=textures[i]||Hash(&m.matdesc.diffuse,17*sizeof(float))!=materials[i])return false;}
 for(int i=0;i<2;++i){S3DTex t={};im->GetTexture(i,&t);const auto&f=t.desc.pixelFormat;if(t.numframes!=1||t.desc.width!=64||t.desc.height!=64||f.dwRGBBitCount!=16||f.dwRBitMask!=0xf800||f.dwGBitMask!=0x7e0||f.dwBBitMask!=0x1f||f.dwRGBAlphaBitMask)return false;}
 const char*names[]={"scrolltex","play","partsys"},*values[]={"obj=teleport,du=0,dv=0.1","jtele",particle_tag};const int frames[]={0,1,15};for(int i=0;i<3;++i){auto*t=im->GetTag(i);if(!t||t->state||t->frame!=frames[i]||!t->name||!t->str||std::strcmp(t->name,names[i])||std::strcmp(t->str,values[i]))return false;}
 return true;
}
}
