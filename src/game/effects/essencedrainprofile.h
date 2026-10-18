#pragma once
#include "game/object/object.h"
#include "game/imagery/3dimage.h"
#include <cctype>
#include <cstring>
#include <string>
#include <vector>
namespace essencedrain_profile {
struct Object{const char*name;int material,vertices,faces,keys;uint64_t key_hash,vertex_hash,face_hash;int counts[5];};
inline uint64_t Hash(const void*p,size_t n){uint64_t h=1469598103934665603ull;for(auto*b=static_cast<const uint8_t*>(p);n--;){h^=*b++;h*=1099511628211ull;}return h;}
inline constexpr Object objects[]={
{"cylinder02",0,35,48,49,0xf6d91e841bc61019ull,0x8bdee70ed55c8b06ull,0xb72b7e156e96cd67ull,{0,48,0,0,0}},
{"skirt01",1,26,24,0,0x14650fb0739d0383ull,0x3459ca04040baf98ull,0x73e07e57b8e31bfdull,{0,0,24,0,0}},
{"skirt02",1,26,24,0,0x14650fb0739d0383ull,0xcf8a320248e8a8a6ull,0x73e07e57b8e31bfdull,{0,0,24,0,0}},
{"skirt03",1,26,24,0,0x14650fb0739d0383ull,0x5b083a30a3b3d95eull,0x73e07e57b8e31bfdull,{0,0,24,0,0}},
{"flare01",2,4,2,0,0x14650fb0739d0383ull,0x59b49392893ab379ull,0x8de70ea438f81391ull,{0,0,0,2,0}},
{"flare02",2,4,2,0,0x14650fb0739d0383ull,0x9144bf30039af6f3ull,0x8de70ea438f81391ull,{0,0,0,2,0}},
{"flare03",2,4,2,0,0x14650fb0739d0383ull,0xd304866b2c9a1cddull,0x8de70ea438f81391ull,{0,0,0,2,0}},
{"flare04",2,4,2,0,0x14650fb0739d0383ull,0x73e2ad78c0314e7cull,0x8de70ea438f81391ull,{0,0,0,2,0}},
{"flare05",2,4,2,0,0x14650fb0739d0383ull,0x7feebcad9836dc46ull,0x8de70ea438f81391ull,{0,0,0,2,0}},
{"flare06",2,4,2,0,0x14650fb0739d0383ull,0x719d49b59cd0fc0cull,0x8de70ea438f81391ull,{0,0,0,2,0}},
{"flare07",2,4,2,0,0x14650fb0739d0383ull,0x58624deb005d82b9ull,0x8de70ea438f81391ull,{0,0,0,2,0}},
{"flare08",2,4,2,0,0x14650fb0739d0383ull,0x229ab45574cf3223ull,0x8de70ea438f81391ull,{0,0,0,2,0}},
{"flare09",2,4,2,0,0x14650fb0739d0383ull,0xd9b4a51ccfee12acull,0x8de70ea438f81391ull,{0,0,0,2,0}},
{"flare10",2,4,2,0,0x14650fb0739d0383ull,0x29c1fc0c8fad9f8full,0x8de70ea438f81391ull,{0,0,0,2,0}},
{"flare11",2,4,2,0,0x14650fb0739d0383ull,0x80e6a7596a1513ull,0x8de70ea438f81391ull,{0,0,0,2,0}},
{"flare12",2,4,2,0,0x14650fb0739d0383ull,0x9d590d5d52d20f26ull,0x8de70ea438f81391ull,{0,0,0,2,0}},
{"flare13",2,4,2,0,0x14650fb0739d0383ull,0xd9b4a51ccfee12acull,0x8de70ea438f81391ull,{0,0,0,2,0}},
{"flare14",2,4,2,0,0x14650fb0739d0383ull,0x33580cab2ce806cbull,0x8de70ea438f81391ull,{0,0,0,2,0}},
{"flare15",2,4,2,0,0x14650fb0739d0383ull,0x870de8b3c5592a40ull,0x8de70ea438f81391ull,{0,0,0,2,0}},
{"flare16",2,4,2,0,0x14650fb0739d0383ull,0x75b3dcf899cbaab0ull,0x8de70ea438f81391ull,{0,0,0,2,0}},
{"flare17",2,4,2,0,0x14650fb0739d0383ull,0x1d644eea321770ebull,0x8de70ea438f81391ull,{0,0,0,2,0}},
{"flare18",2,4,2,0,0x14650fb0739d0383ull,0xbcf3cfa32ccd8dfaull,0x8de70ea438f81391ull,{0,0,0,2,0}},
{"flare19",2,4,2,0,0x14650fb0739d0383ull,0x2b5f60773aa49da8ull,0x8de70ea438f81391ull,{0,0,0,2,0}},
{"flare20",2,4,2,0,0x14650fb0739d0383ull,0xc29bc8c5d8f9979ull,0x8de70ea438f81391ull,{0,0,0,2,0}},
{"shock01",3,4,2,0,0x14650fb0739d0383ull,0x21acf8318a82edfbull,0x381ca01c1b911d31ull,{0,0,0,0,2}},
{"shock02",3,4,2,0,0x14650fb0739d0383ull,0xbae6ca5ddfcfc7d6ull,0x381ca01c1b911d31ull,{0,0,0,0,2}},
{"shock03",3,4,2,0,0x14650fb0739d0383ull,0x8b15f73b6536b22ull,0x381ca01c1b911d31ull,{0,0,0,0,2}},
{"cylinder07",0,35,48,49,0x58a2f278717c2a60ull,0x6a88d25f62da46b1ull,0xb72b7e156e96cd67ull,{0,48,0,0,0}},
{"cylinder01",0,74,100,49,0x7790a18733259720ull,0xed5534005a8f187cull,0x9b9ac377c1e07ad9ull,{0,100,0,0,0}}
};
inline bool Matches(T3DImagery*im){
 if(!im||!im->GetResFilename()||im->NumStates()!=1||im->GetAniLength(0)!=200||im->GetAniFlags(0)!=0x2000||im->NumObjects()!=29||im->NumMaterials()!=4||im->NumTextures()!=4||im->NumTags()!=3)return false;
 const auto*body=static_cast<const S3DImageryBody*>(im->GetBody());if(!body||body->flags!=0xdc||body->version!=3)return false;
 std::string path(im->GetResFilename());for(char&c:path)c=c=='\\'?'/' :char(std::tolower(static_cast<unsigned char>(c)));constexpr auto suffix="magic/worm.i3d";const size_t n=std::strlen(suffix);if(path.size()<n||path.compare(path.size()-n,n,suffix)||(path.size()!=n&&path[path.size()-n-1]!='/'))return false;
 for(int i=0;i<29;++i){S3DObj o={};im->GetObject(i,&o);const auto&e=objects[i];if(std::strcmp(o.name,e.name)||o.material!=e.material||o.parent[0]!=-1||o.numanikeys[0]!=e.keys||(e.keys&&!o.anikeys[0])||Hash(o.anikeys[0],size_t(e.keys)*4)!=e.key_hash||im->NumObjVerts(i)!=e.vertices||im->NumObjFaces(i)!=e.faces)return false;std::vector<S3DVertex>v(size_t(e.vertices));im->GetObjVerts(i,v.data());if(Hash(v.data(),v.size()*sizeof(S3DVertex))!=e.vertex_hash)return false;std::vector<S3DFace>f(size_t(e.faces));int starts[MAXTEXTURES+1]={},counts[MAXTEXTURES+1]={};im->GetObjFaces(i,f.data(),starts,counts);if(Hash(f.data(),f.size()*sizeof(S3DFace))!=e.face_hash)return false;for(int t=0;t<5;++t)if(o.numtexfaces[t]!=e.counts[t])return false;}
 constexpr uint64_t materials[]={0xaa234e3981f0b7b3ull,0xaa234e3981f0b7b3ull,0xaa234e3981f0b7b3ull,0xaa234e3981f0b7b3ull};for(int i=0;i<4;++i){S3DMat m={};im->GetMaterial(i,&m);if(m.texture!=i||Hash(&m.matdesc.diffuse,17*sizeof(float))!=materials[i])return false;}
 for(int i=0;i<4;++i){S3DTex t={};im->GetTexture(i,&t);const auto&f=t.desc.pixelFormat;const uint32_t size=(i==0||i==3)?128:64;if(t.numframes!=1||t.desc.width!=size||t.desc.height!=size||f.dwRGBBitCount!=16||f.dwRBitMask!=0xf800||f.dwGBitMask!=0x7e0||f.dwBBitMask!=0x1f||f.dwRGBAlphaBitMask)return false;}
 const char*names[]={"scrolltex","play","blendcont"},*values[]={"obj=(cylinder01,cylinder02,cylinder07),du=-0.01,dv=0","worm","litadd"};const int frames[]={0,1,40};for(int i=0;i<3;++i){auto*t=im->GetTag(i);if(!t||t->state||t->frame!=frames[i]||!t->name||!t->str||std::strcmp(t->name,names[i])||std::strcmp(t->str,values[i]))return false;}return true;
}
}
