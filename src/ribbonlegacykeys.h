#pragma once
#include "3dimagebody.h"
#include <cctype>
#include <cstdint>
#include <cstring>
#include <string>
namespace ribbon_legacy {
inline uint64_t Hash(const void* p,size_t n){uint64_t h=1469598103934665603ull;const auto*b=static_cast<const uint8_t*>(p);while(n--){h^=*b++;h*=1099511628211ull;}return h;}
inline bool Matches(const char* path,const SOld3DImageryBody* mesh,size_t bytes,int states,int frames,uint32_t aflags)
{
 if(!path || !mesh || bytes<7132 || states!=1 || frames!=1 || aflags!=0x2001)return false;
 std::string name(path);for(char&c:name)c=c=='\\'?'/' : char(std::tolower(static_cast<unsigned char>(c)));constexpr auto suffix="magic/ribbon.i3d";const size_t n=std::strlen(suffix);if(name.size()<n || name.compare(name.size()-n,n,suffix) || (name.size()!=n && name[name.size()-n-1]!='/'))return false;
 if(mesh->flags || mesh->numverts!=186 || mesh->numfaces!=204 || mesh->nummaterials!=2 || mesh->numtextures!=2 || mesh->numobjects!=2 || std::strcmp(mesh->objname[0],"box01") || std::strcmp(mesh->objname[1],"tube01"))return false;
 auto range=[&](const void* p,size_t count){const auto base=reinterpret_cast<uintptr_t>(mesh),at=reinterpret_cast<uintptr_t>(p);return at>=base && at-base<=bytes && count<=bytes-size_t(at-base);};
 const auto* keys=mesh->anikeys[0].ptr();const auto* faces=mesh->faces.ptr();const auto* table=static_cast<const OFFSET*>(mesh->verts[0].ptr());if(!range(keys,16) || !range(faces,1224) || !range(table,sizeof(OFFSET)))return false;const auto* vertices=table[0].ptr();if(!range(vertices,5952))return false;
 if(Hash(keys,16)!=0x2057f3c1c67e25f3ull || Hash(vertices,5952)!=0xa2da2ff42d6e458aull || Hash(faces,1224)!=0xc9dfd10ca5851391ull || Hash(mesh->material,160)!=0xea68b0b7472deb02ull)return false;
 for(int i=0;i<2;++i){const auto& d=mesh->texturedesc[i];const auto& f=d.pixelFormat;if(mesh->textureframes[i]!=(i?1:5) || d.width!=uint32_t(i?128:64) || d.height!=d.width || f.dwRGBBitCount!=16 || f.dwRBitMask!=0xf00 || f.dwGBitMask!=0xf0 || f.dwBBitMask!=0xf || f.dwRGBAlphaBitMask!=0xf000 || mesh->objmaterial[i]!=i || mesh->objvertpos[i]!=(i?12:0) || mesh->objvertnum[i]!=(i?174:12))return false;}
 return true;
}
}
