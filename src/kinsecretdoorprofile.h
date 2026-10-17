#pragma once
#include "object.h"
#include "3dimage.h"
#include <cctype>
#include <cstring>
#include <string>

// Shared runtime/preview guard; no mutable imagery/actor state and no new effect class.
inline bool IsRetailKinSecretDoorStill(const TObjectInstance* owner, T3DImagery* imagery)
{
    if (!owner || !imagery || owner->ObjClass()!=OBJCLASS_EFFECT || owner->ObjId()!=0xad92bd23u ||
        owner->GetState()!=0 || imagery->NumStates()!=3 || imagery->GetAniLength(0)!=2 ||
        imagery->GetAniFlags(0)!=0x2001 || imagery->NumObjects()!=1 || imagery->NumTags()!=0 ||
        imagery->NumMaterials()!=2 || imagery->NumTextures()!=2 ||
        imagery->NumObjVerts(0)!=231 || imagery->NumObjFaces(0)!=260 ||
        stricmp(imagery->GetObjectName(0),"wall") || imagery->IsHidden(0,0)) return false;
    const char* filename=imagery->GetResFilename(); if (!filename) return false;
    std::string path(filename);
    for (char& c:path) c=c=='\\' ? '/' : char(std::tolower(static_cast<unsigned char>(c)));
    constexpr const char* suffix="misc/secretdoor.i3d"; const size_t length=std::strlen(suffix);
    if (path.size()<length || path.compare(path.size()-length,length,suffix) ||
        (path.size()!=length && path[path.size()-length-1]!='/')) return false;
    int starts[MAXTEXTURES+1]={},counts[MAXTEXTURES+1]={};
    imagery->GetObjFaces(0,nullptr,starts,counts);
    if (counts[0] || counts[1]!=158 || counts[2]!=102) return false;
    for (int i=0;i<2;++i) {
        S3DTex texture={};imagery->GetTexture(i,&texture);const auto& p=texture.desc.pixelFormat;
        const uint32_t size=i==0?256u:128u;
        if (texture.htexture==kInvalidTexture || texture.desc.width!=size || texture.desc.height!=size ||
            texture.numframes!=1 || p.dwRGBBitCount!=16 || p.dwRBitMask!=0xf800 || p.dwGBitMask!=0x07e0 ||
            p.dwBBitMask!=0x001f || p.dwRGBAlphaBitMask) return false;
    }
    return true;
}
