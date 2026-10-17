#pragma once
#include "vortexlegacykeys.h"
namespace smoke_legacy {
inline bool Matches(const char*path,const SOld3DImageryBody*mesh,size_t bytes,int states,int f0,int f1,int f2,uint32_t flags){
 if(!path||!mesh||bytes<7132||states!=3||f0!=1||f1!=1||f2!=8||flags!=0x2000)return false;
 std::string name(path);for(char&c:name)c=c=='\\'?'/':char(std::tolower(static_cast<unsigned char>(c)));constexpr auto suffix="misc/smoke.i3d";const size_t n=std::strlen(suffix);if(name.size()<n||name.compare(name.size()-n,n,suffix)||(name.size()!=n&&name[name.size()-n-1]!='/'))return false;
 if(mesh->flags||mesh->numverts!=4||mesh->numfaces!=2||mesh->nummaterials!=1||mesh->numtextures!=1||mesh->numobjects!=1||std::strcmp(mesh->objname[0],"photon")||mesh->objmaterial[0]||mesh->objvertpos[0]||mesh->objvertnum[0]!=4)return false;
 for(int i=0;i<2;++i)if(mesh->objfacepos[0][i]||mesh->objfacenum[0][i]!=(i?2:0))return false;
 auto range=[&](const void*p,size_t count){const auto base=reinterpret_cast<uintptr_t>(mesh),at=reinterpret_cast<uintptr_t>(p);return at>=base&&at-base<=bytes&&count<=bytes-size_t(at-base);};
 for(int state=0;state<3;++state){const int count=state==2?8:1;auto*keys=static_cast<const uint8_t*>(mesh->anikeys[state].ptr());if(!range(keys,size_t(count)*8))return false;for(int i=0;i<count;++i)if(vortex_legacy::Hash(keys+i*8,8)!=0x7b26658f6534fe83ull)return false;}
 const auto*faces=mesh->faces.ptr();const auto*table=static_cast<const OFFSET*>(mesh->verts[0].ptr());if(!range(faces,12)||!range(table,sizeof(OFFSET)))return false;const auto*vertices=table[0].ptr();if(!range(vertices,128))return false;
 if(vortex_legacy::Hash(vertices,128)!=0x173405a01633a803ull||vortex_legacy::Hash(faces,12)!=0x28290cd4dd04f640ull||vortex_legacy::Hash(mesh->material,80)!=0x8800222cadcd6e3dull)return false;
 const auto&d=mesh->texturedesc[0];const auto&f=d.pixelFormat;if(mesh->textureframes[0]!=1||d.width!=64||d.height!=64||f.dwRGBBitCount!=16||f.dwRBitMask!=0xf00||f.dwGBitMask!=0xf0||f.dwBBitMask!=0xf||f.dwRGBAlphaBitMask!=0xf000)return false;
 auto*frames=static_cast<const OFFSET*>(mesh->texturebits[0].ptr());if(!range(frames,sizeof(OFFSET)))return false;auto*bits=frames[0].ptr();return range(bits,8192)&&vortex_legacy::Hash(bits,8192)==0x278a36b56d4c83c7ull;
}
}
