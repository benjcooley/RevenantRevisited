#pragma once
#include "3dimagebody.h"
#include <cctype>
#include <cstdint>
#include <cstring>
#include <string>
namespace irisflare_profile {
inline uint64_t Hash(const void* p,size_t n){uint64_t h=1469598103934665603ull;const auto*b=static_cast<const uint8_t*>(p);while(n--){h^=*b++;h*=1099511628211ull;}return h;}
inline bool Matches(const char* path,const SOld3DImageryBody* mesh,size_t bytes,int states,int frames,uint32_t aflags)
{
 if(!path || !mesh || bytes<sizeof(SOld3DImageryBody) || states!=1 || frames!=1 || aflags!=0x2001)return false;
 std::string name(path);for(char&c:name)c=c=='\\'?'/' : char(std::tolower(static_cast<unsigned char>(c)));constexpr auto suffix="misc/irisflare.i3d";const size_t n=std::strlen(suffix);if(name.size()<n || name.compare(name.size()-n,n,suffix) || (name.size()!=n && name[name.size()-n-1]!='/'))return false;
 if(mesh->flags!=4 || mesh->numverts!=186 || mesh->numfaces!=204 || mesh->nummaterials!=2 || mesh->numtextures!=2 || mesh->numobjects!=2 || std::strcmp(mesh->objname[0],"base") || std::strcmp(mesh->objname[1],"glow"))return false;
 auto range=[&](const void* p,size_t count){const auto base=reinterpret_cast<uintptr_t>(mesh),at=reinterpret_cast<uintptr_t>(p);return at>=base && at-base<=bytes && count<=bytes-size_t(at-base);};
 const auto* keys=mesh->anikeys[0].ptr();const auto* faces=mesh->faces.ptr();const auto* table=static_cast<const OFFSET*>(mesh->verts[0].ptr());if(!range(keys,16) || !range(faces,1224) || !range(table,sizeof(OFFSET)))return false;const auto* vertices=table[0].ptr();if(!range(vertices,5952))return false;
 if(Hash(keys,16)!=0x2057f3c1c67e25f3ull || Hash(vertices,5952)!=0x448b9b31ef97f1d3ull || Hash(faces,1224)!=0xc9dfd10ca5851391ull || Hash(mesh->material,160)!=0xea68b0b7472deb02ull)return false;
 constexpr int starts[]={0,12},counts[]={12,174};
 constexpr int face_starts[][3]={{0,0,12},{12,12,12}},face_counts[][3]={{0,12,0},{0,0,192}};
 constexpr uint64_t texture_hashes[]={0x9e313c1db7add49bull,0xdbe5be8e1865782cull};
 for(int i=0;i<2;++i){if(mesh->objmaterial[i]!=i || mesh->objvertpos[i]!=starts[i] || mesh->objvertnum[i]!=counts[i])return false;for(int t=0;t<3;++t)if(mesh->objfacepos[i][t]!=face_starts[i][t] || mesh->objfacenum[i][t]!=face_counts[i][t])return false;
  const auto*parents=static_cast<const int8_t*>(mesh->objparent[i].ptr());if(!range(parents,1)||*parents!=-1)return false;
  const auto&d=mesh->texturedesc[i];const auto&f=d.pixelFormat;if(mesh->textureframes[i]!=1 || d.width!=64 || d.height!=64 || f.dwRGBBitCount!=16 || f.dwRBitMask!=0xf00 || f.dwGBitMask!=0xf0 || f.dwBBitMask!=0xf || f.dwRGBAlphaBitMask!=0xf000)return false;
  const auto*frames=static_cast<const OFFSET*>(mesh->texturebits[i].ptr());if(!range(frames,sizeof(OFFSET)))return false;const auto*bits=frames[0].ptr();if(!range(bits,8192)||Hash(bits,8192)!=texture_hashes[i])return false;
 }
 return true;
}
}
