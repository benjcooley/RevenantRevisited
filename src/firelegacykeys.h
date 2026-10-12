#pragma once
#include "vortexlegacykeys.h"
namespace fire_legacy {
// Exact shipped old-format body only. Includes all33 texture frames; this
// admits its one authored key, never an inferred texture-animation clock.
inline bool Matches(const char* path,const SOld3DImageryBody* body,size_t bytes,
                    int states,int frames,uint32_t flags) {
 if(!path || !body || bytes!=277496 || states!=1 || frames!=1 || flags!=0x2001)return false;
 std::string name(path);for(char&c:name)c=c=='\\'?'/' :char(std::tolower(static_cast<unsigned char>(c)));
 constexpr auto suffix="misc/fire.i3d";const size_t n=std::strlen(suffix);
 if(name.size()<n || name.compare(name.size()-n,n,suffix) || (name.size()!=n && name[name.size()-n-1]!='/'))return false;
 return vortex_legacy::Hash(body,277496)==0xfe918bfb715ad74full;
}
}
