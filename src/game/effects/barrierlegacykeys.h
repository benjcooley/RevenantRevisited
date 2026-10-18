#pragma once
#include "game/effects/vortexlegacykeys.h"

namespace barrier_legacy {
// Admit only the two immutable retail bodies. The complete body fingerprint
// includes all five legacy keys, parents, geometry, materials and textures.
// This does not authorize other old-format resources.
inline bool Matches(const char* path, const SOld3DImageryBody* mesh,
                    size_t bytes, int states, int frames, uint32_t flags) {
    if(!path || !mesh || bytes<25244 || states!=1 || frames!=1 || flags!=0x2001)
        return false;
    std::string name(path);
    for(char& c:name) c=c=='\\'?'/' :char(std::tolower(static_cast<unsigned char>(c)));
    constexpr const char* suffixes[]={"misc/barrierew.i3d","misc/barrierns.i3d"};
    constexpr uint64_t hashes[]={0x6cafc8534f398164ull,0x8d348d83e4d51367ull};
    for(int i=0;i<2;++i) {
        const size_t n=std::strlen(suffixes[i]);
        if(name.size()<n || name.compare(name.size()-n,n,suffixes[i]) ||
           (name.size()!=n && name[name.size()-n-1]!='/')) continue;
        return vortex_legacy::Hash(mesh,25244)==hashes[i];
    }
    return false;
}
}
