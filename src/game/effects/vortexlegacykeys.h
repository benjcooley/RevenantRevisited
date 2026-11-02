#pragma once
#include "engine/assets/3dimagebody.h"
#include <cctype>
#include <cstdint>
#include <cstring>
#include <string>
namespace vortex_legacy {
inline uint64_t Hash(const void* p,size_t n){uint64_t h=1469598103934665603ull;const auto*b=static_cast<const uint8_t*>(p);while(n--){h^=*b++;h*=1099511628211ull;}return h;}
inline bool Matches(const char* path,const SOld3DImageryBody* mesh,size_t bytes,int states,int frames,uint32_t aflags)
{
 if(!path || !mesh || bytes<7132 || states!=1 || frames!=1 || aflags!=0x2001)return false;
 std::string name(path);for(char&c:name)c=c=='\\'?'/' : char(std::tolower(static_cast<unsigned char>(c)));constexpr auto suffix="magic/vortex.i3d";const size_t n=std::strlen(suffix);if(name.size()<n || name.compare(name.size()-n,n,suffix) || (name.size()!=n && name[name.size()-n-1]!='/'))return false;
 if(mesh->flags || mesh->numverts!=205 || mesh->numfaces!=304 || mesh->nummaterials!=2 || mesh->numtextures!=1 || mesh->numobjects!=3 || std::strcmp(mesh->objname[0],"cylinder01") || std::strcmp(mesh->objname[1],"line03") || std::strcmp(mesh->objname[2],"box01"))return false;
 auto range=[&](const void* p,size_t count){const auto base=reinterpret_cast<uintptr_t>(mesh),at=reinterpret_cast<uintptr_t>(p);return at>=base && at-base<=bytes && count<=bytes-size_t(at-base);};
 const auto* keys=mesh->anikeys[0].ptr();const auto* faces=mesh->faces.ptr();const auto* table=static_cast<const OFFSET*>(mesh->verts[0].ptr());if(!range(keys,24) || !range(faces,1824) || !range(table,sizeof(OFFSET)))return false;const auto* vertices=table[0].ptr();if(!range(vertices,6560))return false;
 if(Hash(keys,24)!=0x92c83bc4db6804ddull || Hash(vertices,6560)!=0x93227d582dd7e4f1ull || Hash(faces,1824)!=0xd1e2b7c9fec05f1bull || Hash(mesh->material,160)!=0x88a6ef32ade0b984ull)return false;
 constexpr int starts[]={0,165,201},counts[]={165,36,4},materials[]={0,0,1};
 constexpr int face_starts[][2]={{0,270},{270,302},{302,302}},face_counts[][2]={{270,0},{32,0},{0,2}};
 for(int i=0;i<3;++i){if(mesh->objmaterial[i]!=materials[i] || mesh->objvertpos[i]!=starts[i] || mesh->objvertnum[i]!=counts[i])return false;for(int t=0;t<2;++t)if(mesh->objfacepos[i][t]!=face_starts[i][t] || mesh->objfacenum[i][t]!=face_counts[i][t])return false;}
 const auto&d=mesh->texturedesc[0];const auto&f=d.pixelFormat;if(mesh->textureframes[0]!=1 || d.width!=32 || d.height!=32 || f.dwRGBBitCount!=16 || f.dwRBitMask!=0xf00 || f.dwGBitMask!=0xf0 || f.dwBBitMask!=0xf || f.dwRGBAlphaBitMask!=0xf000)return false;
 const auto*texture_frames=static_cast<const OFFSET*>(mesh->texturebits[0].ptr());if(!range(texture_frames,sizeof(OFFSET)))return false;const auto*bits=texture_frames[0].ptr();if(!range(bits,2048) || Hash(bits,2048)!=0x3afbac83d54ff12bull)return false;
 return true;
}
}
