// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   3dImage.cpp - 3D image module                       *
// *                                                                       *
// *  Ported (Phase 2): D3D3 texture surfaces + material handles + execute *
// *  buffers replaced by sokol-facing stubs. Animation-key decoders,      *
// *  matrix builders, tag/sound wiring, and mesh loading are unchanged    *
// *  portable logic. AddTexture / LoadTexture / RemoveTexture and         *
// *  AddMaterial is now handle-based; concrete GPU objects stay in renderer *
// *  in Phase 3.                                                          *
// *************************************************************************

#include "3dimage.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <string>
#include <vector>

#include "3dscene.h"
#include "authoredpartsys.h"
#include "testconfig.h"
#include "goldauthoredmatrix.h"
#include "speedauthoredmatrix.h"
#include "staticpartsysprofiles.h"
#include "animation.h"
#include "bitmap.h"
#include "logging.h"
#include "mappane.h"
#include "math3d.h"
#include "i3danimpose.h"
#include "parse.h"
#include "renderer.h"
#include "revutils.h"
#include "sound.h"
#include "time.h"

T3DAnimatorBuilder T3DAnimatorBuilderInstance;  // Register default builder

static bool IsGoldImageryFilename(const char* filename)
{
    if (!filename) return false;
    std::string path(filename);
    for (char& c : path)
        c = c == '\\' ? '/' : char(std::tolower(static_cast<unsigned char>(c)));
    constexpr const char* suffix = "misc/goldp.i3d";
    constexpr size_t length = 14;
    return path.size() >= length && path.compare(path.size() - length, length, suffix) == 0 &&
           (path.size() == length || path[path.size() - length - 1] == '/');
}

bool T3DImagery::ValidateGoldPartSysProfile()
{
    // Fail closed on modified/later assets. This is the literal shipped Goldp
    // vertical slice, not general blendcont or alpha-particle enablement.
    if (!IsGoldImageryFilename(GetResFilename()) || version != 3 ||
        (flags & I3D_ISMORPH) || NumStates() != 1 || GetAniLength(0) != 45 ||
        GetAniFlags(0) != 0x2000 || NumObjects() != 3 || NumTextures() != 2 || NumTags() != 2)
        return false;
    if (stricmp(GetObjectName(0), "gold") || stricmp(GetObjectName(1), "#$iflare") ||
        stricmp(GetObjectName(2), "#goldp") || NumObjVerts(0) != 0 ||
        NumObjVerts(1) != 4 || NumObjFaces(1) != 2 || NumObjVerts(2) != 4 ||
        NumObjFaces(2) != 2 || IsHidden(1, 0) || IsHidden(2, 0))
        return false;
    const S3DTag& blend = *GetTag(0);
    const S3DTag& particles = *GetTag(1);
    constexpr const char* parameters = "obj=(gold),particle=#goldp,pps=[0:400,2:0],initialvelocity=45:50,blendmode=alpha,scale=1.25,lifespan=35:35,color=[0:(255,255,255),100:(255,255,255)],localrotation=[0:(0,0,0),100:(0,0,0)],gravity=1.5,friction=0.05,spread=8,azimuth=8,bounce=-25:-25";
    if (blend.state != 0 || blend.frame != 5 || !blend.name || !blend.str ||
        stricmp(blend.name, "blendcont") || stricmp(blend.str, "litadd") ||
        particles.state != 0 || particles.frame != 10 || !particles.name || !particles.str ||
        stricmp(particles.name, "partsys") || std::strcmp(particles.str, parameters))
        return false;
    for (int32_t object = 1; object <= 2; ++object)
    {
        int32_t starts[MAXTEXTURES + 1] = {}, counts[MAXTEXTURES + 1] = {};
        GetObjFaces(object, nullptr, starts, counts);
        if (counts[object] != 2 || counts[0] || counts[3 - object]) return false;
    }
    S3DTex flare = {}, coins = {};
    GetTexture(0, &flare); GetTexture(1, &coins);
    const auto& rgb = flare.desc.pixelFormat;
    const auto& argb = coins.desc.pixelFormat;
    return flare.desc.width == 64 && flare.desc.height == 64 && flare.numframes == 1 &&
           rgb.dwRGBBitCount == 16 && rgb.dwRBitMask == 0xf800 && rgb.dwGBitMask == 0x07e0 &&
           rgb.dwBBitMask == 0x001f && rgb.dwRGBAlphaBitMask == 0 &&
           coins.desc.width == 16 && coins.desc.height == 16 && coins.numframes == 1 &&
           argb.dwRGBBitCount == 16 && argb.dwRBitMask == 0x0f00 && argb.dwGBitMask == 0x00f0 &&
           argb.dwBBitMask == 0x000f && argb.dwRGBAlphaBitMask == 0xf000;
}

static bool IsCombatFlashImageryFilename(const char* filename)
{
    if (!filename) return false;
    std::string path(filename);
    for (char& c : path) c = c == '\\' ? '/' : char(std::tolower(static_cast<unsigned char>(c)));
    constexpr const char* suffix = "misc/impact.i3d";
    const size_t length = std::strlen(suffix);
    return path.size() >= length && path.compare(path.size() - length, length, suffix) == 0 &&
           (path.size() == length || path[path.size() - length - 1] == '/');
}

bool T3DImagery::ValidateCombatFlashStart1PartSysProfile()
{
    // Exact Impact inventory is checked; only start1/state0 is enabled below.
    // Later tags/trails are not inferred from a successful first-state profile.
    if (!IsCombatFlashImageryFilename(GetResFilename()) || version != 3 ||
        (flags & I3D_ISMORPH) || NumStates() != 18 || NumObjects() != 21 ||
        NumMaterials() != 11 || NumTextures() != 10 || NumTags() != 18 ||
        GetAniLength(0) != 19 || GetAniFlags(0) != 0x2000)
        return false;
    constexpr const char* names[] = {"#particle01","impact01","#impact01","#particle02","impact02","#impact02",
        "#particle03","impact03","#impact03","#impact06","#impact04","#impact05","impact04","impact05",
        "impact06","impact07","impact08","impact09","#impact08","#impact07","#impact09"};
    for (int object = 0; object < 21; ++object)
        if (stricmp(GetObjectName(object), names[object])) return false;
    if (NumObjVerts(0) != 4 || NumObjFaces(0) != 2 || NumObjVerts(1) != 0 ||
        NumObjFaces(1) != 0 || NumObjVerts(2) != 4 || NumObjFaces(2) != 2 ||
        IsHidden(0,0) || IsHidden(2,0)) return false;
    const S3DTag& blend = *GetTag(0); const S3DTag& particles = *GetTag(1);
    constexpr const char* parameters = "obj=(impact01),particle=#particle01,pps=[0:250,2:0],initialvelocity=15:30,scale=[0:2.5,100:0],lifespan=30:45,localrotation=[0:(0,0,0),100:(0,360,0)],friction=0.3,gravity=0.5,color=[0:(255,225,100),25:(255,175,50),99:(0,0,0)],spread=120";
    if (blend.state != 0 || blend.frame != 0 || !blend.name || !blend.str ||
        stricmp(blend.name,"blendcont") || stricmp(blend.str,"litadd") ||
        particles.state != 0 || particles.frame != 1 || !particles.name || !particles.str ||
        stricmp(particles.name,"partsys") || std::strcmp(particles.str,parameters)) return false;
    for (int object : {0,2}) {
        S3DObj obj = {}; S3DMat material = {}; GetObject(object,&obj);
        if (obj.material != 0) return false;
        GetMaterial(obj.material,&material); if (material.texture != 0) return false;
        S3DFace faces[2] = {}; int starts[MAXTEXTURES+1] = {},counts[MAXTEXTURES+1] = {};
        GetObjFaces(object,faces,starts,counts);
        if (counts[1] != 2 || faces[0].v1 != 2 || faces[0].v2 != 3 || faces[0].v3 != 0 ||
            faces[1].v1 != 1 || faces[1].v2 != 2 || faces[1].v3 != 0) return false;
        for (int slot=0;slot<=NumTextures();++slot) if (slot!=1 && counts[slot]) return false;
    }
    S3DTex texture = {}; GetTexture(0,&texture); const auto& rgb=texture.desc.pixelFormat;
    return texture.desc.width==64 && texture.desc.height==64 && texture.numframes==1 &&
           rgb.dwRGBBitCount==16 && rgb.dwRBitMask==0xf800 && rgb.dwGBitMask==0x07e0 &&
           rgb.dwBBitMask==0x001f && rgb.dwRGBAlphaBitMask==0;
}

bool T3DImagery::ValidateRetailPunchProfile()
{
    if (!GetResFilename() || version != 3 || flags != 0xdc || NumStates() != 1 ||
        GetAniLength(0) != 581 || GetAniFlags(0) != 0x2001 || NumObjects() != 8 ||
        numverts != 262 || numfaces != 220 || NumMaterials() != 2 || NumTextures() != 2 || NumTags()) return false;
    std::string path(GetResFilename());
    for (char& c : path) c = c == '\\' ? '/' : char(std::tolower(static_cast<unsigned char>(c)));
    constexpr const char* suffix = "misc/punchandjudy.i3d";
    const size_t length = std::strlen(suffix);
    if (path.size() < length || path.compare(path.size() - length, length, suffix) ||
        (path.size() != length && path[path.size() - length - 1] != '/')) return false;
    constexpr const char* names[8] = {"phead01","pbody01","prightarm01","pleftarm01","jrightarm","jbody","jhead","jrightarm01"};
    constexpr int verts[8] = {62,33,20,16,16,42,57,16};
    constexpr int faces[8] = {71,15,19,15,15,26,44,15};
    constexpr int keys[8] = {752,726,859,887,701,425,658,669};
    constexpr uint64_t key_hashes[8] = {0x13c550526d7fe5ffull,0x9f98a86110344625ull,0x36f37be8325d0e1bull,0xfc0490d9498406e5ull,0xb82986f9ea1bb25full,0x14866a9bc8d221b0ull,0x645b815854f05009ull,0x5ccb9eb3826162a4ull};
    for (int i = 0; i < 8; ++i) {
        const S3DObj& object = objects[i];
        const int slot = i < 4 ? 1 : 2;
        if (std::strcmp(object.name, names[i]) || object.parent[0] != -1 ||
            object.numverts != verts[i] || object.numfaces != faces[i] || object.material != slot - 1 ||
            object.numanikeys[0] != keys[i] || !object.anikeys[0] ||
            object.numtexfaces[0] || object.numtexfaces[slot] != faces[i] || object.numtexfaces[3-slot]) return false;
        uint64_t hash = 1469598103934665603ull;
        const auto* bytes = static_cast<const uint8_t*>(object.anikeys[0]);
        for (size_t j = 0; j < size_t(keys[i]) * sizeof(SAniKey32); ++j) { hash ^= bytes[j]; hash *= 1099511628211ull; }
        if (hash != key_hashes[i]) return false;
    }
    for (int i = 0; i < 2; ++i) {
        const S3DTex& texture = textures[i]; const auto& pf = texture.desc.pixelFormat;
        if (texture.desc.width != 64 || texture.desc.height != 64 || texture.numframes != 1 ||
            pf.dwRGBBitCount != 16 || pf.dwRBitMask != 0xf800 || pf.dwGBitMask != 0x07e0 ||
            pf.dwBBitMask != 0x001f || pf.dwRGBAlphaBitMask) return false;
    }
    return true;
}


bool T3DImagery::ValidateRetailMPAppearStartProfile()
{
    // Exact shipped default start only; no generic blendcont enablement.
    if (!GetResFilename() || version != 3 || flags != 0xdc || NumStates() != 1 ||
        GetAniLength(0) != 90 || GetAniFlags(0) != 0x2000 || NumObjects() != 7 ||
        numverts != 118 || numfaces != 104 || NumMaterials() != 4 || NumTextures() != 4 || NumTags() != 2)
        return false;
    std::string path(GetResFilename());
    for (char& c : path) c = c == '\\' ? '/' : char(std::tolower(static_cast<unsigned char>(c)));
    constexpr const char* suffix = "magic/appear.i3d";
    const size_t length = std::strlen(suffix);
    if (path.size() < length || path.compare(path.size()-length,length,suffix) ||
        (path.size()!=length && path[path.size()-length-1]!='/')) return false;
    const S3DTag& play = *GetTag(0); const S3DTag& blend = *GetTag(1);
    if (play.state!=0 || play.frame!=1 || !play.name || !play.str ||
        std::strcmp(play.name,"play") || std::strcmp(play.str,"appear") ||
        blend.state!=0 || blend.frame!=20 || !blend.name || !blend.str ||
        std::strcmp(blend.name,"blendcont") || std::strcmp(blend.str,"litaddz")) return false;
    constexpr const char* names[7] = {"main pulse","#$star","second pulse","skirt01","skirt02","skirt03","rectangle01"};
    constexpr int vertex_counts[7] = {22,4,22,22,22,22,4}, face_counts[7] = {20,2,20,20,20,20,2};
    constexpr int key_counts[7] = {225,59,242,220,216,214,177}, material_indices[7] = {0,1,2,3,3,3,1}, slots[7] = {1,2,3,4,4,4,2};
    constexpr uint64_t key_hashes[7] = {0x5ca5af897f337e4bull,0x6fc643bf823f89bull,0xd33104f7c2ac7b62ull,0x3f85f68a04176316ull,0x86f4521e17e63288ull,0xe2899f80250c5114ull,0x1dded56c45d96427ull};
    for (int i=0;i<7;++i) {
        const S3DObj& object=objects[i];
        if (std::strcmp(object.name,names[i]) || object.parent[0]!=-1 ||
            object.numverts!=vertex_counts[i] || object.numfaces!=face_counts[i] ||
            object.material!=material_indices[i] || object.numanikeys[0]!=key_counts[i] || !object.anikeys[0]) return false;
        for (int slot=0;slot<=4;++slot)
            if (object.numtexfaces[slot]!=(slot==slots[i] ? face_counts[i] : 0)) return false;
        uint64_t hash=1469598103934665603ull;
        const auto* bytes=static_cast<const uint8_t*>(object.anikeys[0]);
        for (size_t j=0;j<size_t(key_counts[i])*sizeof(SAniKey32);++j) { hash^=bytes[j]; hash*=1099511628211ull; }
        if (hash!=key_hashes[i]) return false;
    }
    for (int i=0;i<4;++i) {
        const S3DTex& texture=textures[i]; const auto& pf=texture.desc.pixelFormat;
        const int size=i<2 ? 128 : 64;
        if (texture.desc.width!=size || texture.desc.height!=size || texture.numframes!=1 ||
            pf.dwRGBBitCount!=16 || pf.dwRBitMask!=0xf800 || pf.dwGBitMask!=0x07e0 ||
            pf.dwBBitMask!=0x001f || pf.dwRGBAlphaBitMask || materials[i].texture!=i) return false;
    }
    return true;
}

bool T3DImagery::ValidateRetailShadowfistProfile()
{
    // Exact shipped looping defaultstart only; no generic blendcont enablement.
    if (!GetResFilename() || version != 3 || flags != 0xdc || NumStates() != 1 ||
        GetAniLength(0) != 60 || GetAniFlags(0) != 0x2001 || NumObjects() != 8 ||
        numverts != 176 || numfaces != 160 || NumMaterials() != 1 || NumTextures() != 1 || NumTags() != 1)
        return false;
    std::string path(GetResFilename());
    for (char& c : path) c = c == '\\' ? '/' : char(std::tolower(static_cast<unsigned char>(c)));
    constexpr const char* suffix = "magic/sfist.i3d";
    const size_t length = std::strlen(suffix);
    if (path.size() < length || path.compare(path.size()-length,length,suffix) ||
        (path.size()!=length && path[path.size()-length-1]!='/')) return false;
    const S3DTag& blend = *GetTag(0);
    if (blend.state!=0 || blend.frame!=10 || !blend.name || !blend.str ||
        std::strcmp(blend.name,"blendcont") || std::strcmp(blend.str,"litadd")) return false;
    constexpr const char* names[8] = {"#flare02","#flare03","#flare04","#flare05","#flare06","#flare07","#flare08","#flare01"};
    constexpr int vertex_counts[8] = {22,22,22,22,22,22,22,22}, face_counts[8] = {20,20,20,20,20,20,20,20};
    constexpr int key_counts[8] = {68,68,68,68,68,68,68,68}, material_indices[8] = {0,0,0,0,0,0,0,0}, slots[8] = {1,1,1,1,1,1,1,1};
    constexpr uint64_t key_hashes[8] = {0xed9eafa7674b9738ull,0x1d963457c15cbee0ull,0xb9473b666e5bc9eull,0x569f9bfde63f195aull,0xd2e12bc81c6778eeull,0x5fcbf69117f6b28aull,0xe6a0a45bf5ff6b04ull,0x4b5d88c8d81647afull};
    for (int i=0;i<8;++i) {
        const S3DObj& object=objects[i];
        if (std::strcmp(object.name,names[i]) || object.parent[0]!=-1 ||
            object.numverts!=vertex_counts[i] || object.numfaces!=face_counts[i] ||
            object.material!=material_indices[i] || object.numanikeys[0]!=key_counts[i] || !object.anikeys[0]) return false;
        for (int slot=0;slot<=1;++slot)
            if (object.numtexfaces[slot]!=(slot==slots[i] ? face_counts[i] : 0)) return false;
        uint64_t hash=1469598103934665603ull;
        const auto* bytes=static_cast<const uint8_t*>(object.anikeys[0]);
        for (size_t j=0;j<size_t(key_counts[i])*sizeof(SAniKey32);++j) { hash^=bytes[j]; hash*=1099511628211ull; }
        if (hash!=key_hashes[i]) return false;
    }
    for (int i=0;i<1;++i) {
        const S3DTex& texture=textures[i]; const auto& pf=texture.desc.pixelFormat;
        const int size=32;
        if (texture.desc.width!=size || texture.desc.height!=size || texture.numframes!=1 ||
            pf.dwRGBBitCount!=16 || pf.dwRBitMask!=0xf800 || pf.dwGBitMask!=0x07e0 ||
            pf.dwBBitMask!=0x001f || pf.dwRGBAlphaBitMask || materials[i].texture!=i) return false;
    }
    return true;
}

bool T3DImagery::ValidateRetailWarriorbornProfile()
{
    // Exact shipped looping defaultstart only; no generic blendcont enablement.
    if (!GetResFilename() || version != 3 || flags != 0xdc || NumStates() != 1 ||
        GetAniLength(0) != 60 || GetAniFlags(0) != 0x2001 || NumObjects() != 8 ||
        numverts != 176 || numfaces != 160 || NumMaterials() != 1 || NumTextures() != 1 || NumTags() != 1)
        return false;
    std::string path(GetResFilename());
    for (char& c : path) c = c == '\\' ? '/' : char(std::tolower(static_cast<unsigned char>(c)));
    constexpr const char* suffix = "magic/wborn.i3d";
    const size_t length = std::strlen(suffix);
    if (path.size() < length || path.compare(path.size()-length,length,suffix) ||
        (path.size()!=length && path[path.size()-length-1]!='/')) return false;
    const S3DTag& blend = *GetTag(0);
    if (blend.state!=0 || blend.frame!=10 || !blend.name || !blend.str ||
        std::strcmp(blend.name,"blendcont") || std::strcmp(blend.str,"litadd")) return false;
    constexpr const char* names[8] = {"#flare02","#flare03","#flare04","#flare05","#flare06","#flare07","#flare08","#flare01"};
    constexpr int vertex_counts[8] = {22,22,22,22,22,22,22,22}, face_counts[8] = {20,20,20,20,20,20,20,20};
    constexpr int key_counts[8] = {68,68,68,68,68,68,68,68}, material_indices[8] = {0,0,0,0,0,0,0,0}, slots[8] = {1,1,1,1,1,1,1,1};
    constexpr uint64_t key_hashes[8] = {0x8e28c0cf42be59a8ull,0x396198019732f598ull,0xa63c11233cfeedaull,0xb5edf1a83a8c5396ull,0x3ca32a18ce8ddef2ull,0x4d9caec728983d32ull,0xae2172b537b38880ull,0x97ceba4228b3a926ull};
    for (int i=0;i<8;++i) {
        const S3DObj& object=objects[i];
        if (std::strcmp(object.name,names[i]) || object.parent[0]!=-1 ||
            object.numverts!=vertex_counts[i] || object.numfaces!=face_counts[i] ||
            object.material!=material_indices[i] || object.numanikeys[0]!=key_counts[i] || !object.anikeys[0]) return false;
        for (int slot=0;slot<=1;++slot)
            if (object.numtexfaces[slot]!=(slot==slots[i] ? face_counts[i] : 0)) return false;
        uint64_t hash=1469598103934665603ull;
        const auto* bytes=static_cast<const uint8_t*>(object.anikeys[0]);
        for (size_t j=0;j<size_t(key_counts[i])*sizeof(SAniKey32);++j) { hash^=bytes[j]; hash*=1099511628211ull; }
        if (hash!=key_hashes[i]) return false;
    }
    for (int i=0;i<1;++i) {
        const S3DTex& texture=textures[i]; const auto& pf=texture.desc.pixelFormat;
        const int size=64;
        if (texture.desc.width!=size || texture.desc.height!=size || texture.numframes!=1 ||
            pf.dwRGBBitCount!=16 || pf.dwRBitMask!=0xf800 || pf.dwGBitMask!=0x07e0 ||
            pf.dwBBitMask!=0x001f || pf.dwRGBAlphaBitMask || materials[i].texture!=i) return false;
    }
    return true;
}

bool T3DImagery::ValidateRetailTeleportationProfile()
{
    // Exact shipped looping defaultstart only; no generic blendcont enablement.
    if (!GetResFilename() || version != 3 || flags != 0xdc || NumStates() != 1 ||
        GetAniLength(0) != 100 || GetAniFlags(0) != 0x2001 || NumObjects() != 6 ||
        numverts != 263 || numfaces != 254 || NumMaterials() != 8 || NumTextures() != 8 || NumTags() != 1)
        return false;
    std::string path(GetResFilename());
    for (char& c : path) c = c == '\\' ? '/' : char(std::tolower(static_cast<unsigned char>(c)));
    constexpr const char* suffix = "magic/teleportation.i3d";
    const size_t length = std::strlen(suffix);
    if (path.size() < length || path.compare(path.size()-length,length,suffix) ||
        (path.size()!=length && path[path.size()-length-1]!='/')) return false;
    const S3DTag& blend = *GetTag(0);
    if (blend.state!=0 || blend.frame!=35 || !blend.name || !blend.str ||
        std::strcmp(blend.name,"blendcont") || std::strcmp(blend.str,"litadd")) return false;
    constexpr const char* names[6] = {"upflame","in flame","r1","r2","r3","glow06"};
    constexpr int vertex_counts[6] = {38,38,61,61,61,4}, face_counts[6] = {36,36,60,60,60,2};
    constexpr int key_counts[6] = {157,171,108,105,108,70}, material_indices[6] = {0,0,1,3,5,7};
    constexpr int face_groups[6][9] = {{0,36,0,0,0,0,0,0,0},{0,36,0,0,0,0,0,0,0},{0,0,40,20,0,0,0,0,0},{0,0,0,0,40,20,0,0,0},{0,0,0,0,0,0,40,20,0},{0,0,0,0,0,0,0,0,2}};
    constexpr uint64_t key_hashes[6] = {0x5ff0f075ec64e9e0ull,0xb29a17bcab1d4043ull,0x70b5c76723506739ull,0x31f982395099527full,0x82ab3efea99888e2ull,0x4898a220736f1f75ull};
    for (int i=0;i<6;++i) {
        const S3DObj& object=objects[i];
        if (std::strcmp(object.name,names[i]) || object.parent[0]!=-1 ||
            object.numverts!=vertex_counts[i] || object.numfaces!=face_counts[i] ||
            object.material!=material_indices[i] || object.numanikeys[0]!=key_counts[i] || !object.anikeys[0]) return false;
        for (int slot=0;slot<=8;++slot)
            if (object.numtexfaces[slot]!=face_groups[i][slot]) return false;
        uint64_t hash=1469598103934665603ull;
        const auto* bytes=static_cast<const uint8_t*>(object.anikeys[0]);
        for (size_t j=0;j<size_t(key_counts[i])*sizeof(SAniKey32);++j) { hash^=bytes[j]; hash*=1099511628211ull; }
        if (hash!=key_hashes[i]) return false;
    }
    for (int i=0;i<8;++i) {
        const S3DTex& texture=textures[i]; const auto& pf=texture.desc.pixelFormat;
        const int size=i==0 ? 128 : 64;
        if (texture.desc.width!=size || texture.desc.height!=size || texture.numframes!=1 ||
            pf.dwRGBBitCount!=16 || pf.dwRBitMask!=0xf800 || pf.dwGBitMask!=0x07e0 ||
            pf.dwBBitMask!=0x001f || pf.dwRGBAlphaBitMask || materials[i].texture!=i) return false;
    }
    return true;
}


bool T3DImagery::ValidateRetailMightPartSysProfile()
{
    if (!GetResFilename() || version!=3 || flags!=0xdc || NumStates()!=1 ||
        GetAniLength(0)!=30 || GetAniFlags(0)!=0x2001 || NumObjects()!=2 || NumMaterials()!=2 ||
        NumTextures()!=1 || NumTags()!=1 || numverts!=4 || numfaces!=2) return false;
    std::string path(GetResFilename());
    for(char& c:path)c=c=='\\' ? '/' : char(std::tolower(static_cast<unsigned char>(c)));
    constexpr const char* suffix="magic/might.i3d";const size_t length=std::strlen(suffix);
    if(path.size()<length || path.compare(path.size()-length,length,suffix) ||
       (path.size()!=length && path[path.size()-length-1]!='/'))return false;
    constexpr const char* names[2]={"#rect","cfire"};
    constexpr uint64_t hashes[2]={0x72efcd2cf641afull,0x43f82d294c438c35ull};
    for(int i=0;i<2;++i){const auto& object=objects[i];
        if(std::strcmp(object.name,names[i]) || object.parent[0]!=-1 || object.material!=i ||
           object.numverts!=(i?0:4) || object.numfaces!=(i?0:2) ||
           object.numanikeys[0]!=6 || !object.anikeys[0] || object.numtexfaces[0] ||
           object.numtexfaces[1]!=(i?0:2))return false;
        uint64_t h=1469598103934665603ull;const auto* bytes=static_cast<const uint8_t*>(object.anikeys[0]);
        for(size_t j=0;j<6*sizeof(SAniKey32);++j){h^=bytes[j];h*=1099511628211ull;}
        if(h!=hashes[i])return false;
    }
    const auto& t=*GetTag(0);
    constexpr const char* parameters="obj=(cfire),particle=#rect,pps=30,lifespan=10:25,initialvelocity=2.5:5.5,scale=[0:0.75,100:0],rlocalrotation=[0:(0,0,0),100:(0,15,0)],color=[0:(255,255,255),15:(255,197,1),45:(255,126,1),65:(255,72,1),100:(10,1,1)],spread=90,alpha=[0:1,100:0],friction=[0:0.1],emittertype=circle,emittersize=1,relvel=0.25";
    if(t.state!=0 || t.frame!=5 || !t.name || !t.str || std::strcmp(t.name,"partsys") ||
       std::strcmp(t.str,parameters))return false;
    const auto& tex=textures[0];const auto& pf=tex.desc.pixelFormat;
    return tex.desc.width==64 && tex.desc.height==64 && tex.numframes==1 &&
        pf.dwRGBBitCount==16 && pf.dwRBitMask==0xf800 && pf.dwGBitMask==0x07e0 &&
        pf.dwBBitMask==0x001f && pf.dwRGBAlphaBitMask==0 && materials[0].texture==0;
}

bool T3DImagery::ValidateRetailImmortalmightPartSysProfile()
{
    if (!GetResFilename() || version!=3 || flags!=0xdc || NumStates()!=1 ||
        GetAniLength(0)!=30 || GetAniFlags(0)!=0x2001 || NumObjects()!=3 || NumMaterials()!=2 ||
        NumTextures()!=1 || NumTags()!=1 || numverts!=8 || numfaces!=4) return false;
    std::string path(GetResFilename());
    for(char& c:path)c=c=='\\' ? '/' : char(std::tolower(static_cast<unsigned char>(c)));
    constexpr const char* suffix="magic/imight.i3d";const size_t length=std::strlen(suffix);
    if(path.size()<length || path.compare(path.size()-length,length,suffix) ||
       (path.size()!=length && path[path.size()-length-1]!='/'))return false;
    constexpr const char* names[3]={"#rect","cfire","#flare"};
    constexpr uint64_t hashes[3]={0xbb6a60243fe988deull,0x72571c90fccc0036ull,0x95fb61ec8f6b7831ull};
    for(int i=0;i<3;++i){const auto& object=objects[i];
        if(std::strcmp(object.name,names[i]) || object.parent[0]!=-1 || object.material!=(i==1?1:0) ||
           object.numverts!=(i==1?0:4) || object.numfaces!=(i==1?0:2) ||
           object.numanikeys[0]!=(i==2?38:6) || !object.anikeys[0] || object.numtexfaces[0] ||
           object.numtexfaces[1]!=(i==1?0:2))return false;
        uint64_t h=1469598103934665603ull;const auto* bytes=static_cast<const uint8_t*>(object.anikeys[0]);
        for(size_t j=0;j<size_t(i==2?38:6)*sizeof(SAniKey32);++j){h^=bytes[j];h*=1099511628211ull;}
        if(h!=hashes[i])return false;
    }
    const auto& t=*GetTag(0);
    constexpr const char* parameters="obj=(cfire),particle=#rect,pps=30,lifespan=10:25,initialvelocity=3:6,scale=[0:1,100:0],rlocalrotation=[0:(0,0,0),100:(0,15,0)],color=[0:(255,255,255),15:(255,185,1),100:(10,1,1)],spread=90,alpha=[0:1,100:0],friction=[0:0.1],emittertype=circle,emittersize=1,relvel=0.25";
    if(t.state!=0 || t.frame!=5 || !t.name || !t.str || std::strcmp(t.name,"partsys") ||
       std::strcmp(t.str,parameters))return false;
    // Guard authored material identity. RGB565 alpha-mask0 skips retail blend4 selection409e57..5c.
    const auto& m=materials[0].matdesc;
    if(m.diffuse.r!=1 || m.diffuse.g!=1 || m.diffuse.b!=1 || m.diffuse.a!=1 ||
       m.ambient.r!=1 || m.ambient.g!=1 || m.ambient.b!=1 || m.ambient.a!=1 ||
       m.specular.r!=0.9f || m.specular.g!=0.9f || m.specular.b!=0.9f || m.specular.a!=1 ||
       m.emissive.r!=1 || m.emissive.g!=1 || m.emissive.b!=1 || m.emissive.a!=1 || m.power!=0) return false;
    const auto& tex=textures[0];const auto& pf=tex.desc.pixelFormat;
    return tex.desc.width==64 && tex.desc.height==64 && tex.numframes==1 &&
        pf.dwRGBBitCount==16 && pf.dwRBitMask==0xf800 && pf.dwGBitMask==0x07e0 &&
        pf.dwBBitMask==0x001f && pf.dwRGBAlphaBitMask==0 && materials[0].texture==0;
}

bool T3DImagery::HasRetailWarpProfile(uint32_t type_id)
{
    const char* expected = retail_warp::AssetPath(type_id);
    if (!expected) return false;
    // NumObjects loads the immutable imagery before inspecting its metadata.
    if (NumObjects()!=1 || version!=3 || flags!=0xdc || NumStates()!=1 ||
        GetAniLength(0)!=1 || GetAniFlags(0)!=0x2001 || NumTags()!=0 ||
        NumMaterials()!=1 || NumTextures()!=1 || numverts!=4 || numfaces!=3 ||
        !GetResFilename()) return false;
    auto normalized = [](const char* value) {
        std::string result(value);
        for (char& c:result) c=c=='\\' ? '/' : char(std::tolower(static_cast<unsigned char>(c)));
        return result;
    };
    const std::string path=normalized(GetResFilename()), suffix=normalized(expected);
    if (path.size()<suffix.size() || path.compare(path.size()-suffix.size(),suffix.size(),suffix) ||
        (path.size()!=suffix.size() && path[path.size()-suffix.size()-1]!='/')) return false;
    const auto& object=objects[0];
    if (std::strcmp(object.name,"gate") || object.parent[0]!=-1 || object.material!=0 ||
        object.numverts!=4 || object.numfaces!=3 || object.numanikeys[0]!=9 ||
        !object.anikeys[0] || object.numtexfaces[0]!=0 || object.numtexfaces[1]!=3) return false;
    auto fingerprint = [](const void* data,size_t length) {
        uint64_t hash=1469598103934665603ull;
        const auto* bytes=static_cast<const uint8_t*>(data);
        for(size_t i=0;i<length;++i) { hash^=bytes[i]; hash*=1099511628211ull; }
        return hash;
    };
    if (fingerprint(object.anikeys[0],9*sizeof(SAniKey32))!=0x99e1daf6cdfd3ca4ull) return false;
    S3DVertex vertices[4]{}; GetObjVerts(0,vertices);
    if (fingerprint(vertices,sizeof(vertices))!=0x9caa112ebf96f34cull) return false;
    S3DFace faces[3]{}; GetObjFaces(0,faces);
    constexpr uint16_t expected_faces[9]={2,0,1,3,0,2,1,0,3};
    if (sizeof(faces)!=sizeof(expected_faces) || std::memcmp(faces,expected_faces,sizeof(faces))) return false;
    const auto& diffuse=materials[0].matdesc.diffuse;
    if (diffuse.r!=1 || diffuse.g!=1 || diffuse.b!=1 || diffuse.a!=1) return false;
    const auto& material=materials[0].matdesc;
    if (material.ambient.r!=1 || material.ambient.g!=1 || material.ambient.b!=1 || material.ambient.a!=1 ||
        material.emissive.r!=1 || material.emissive.g!=1 || material.emissive.b!=1 || material.emissive.a!=1 ||
        material.specular.r!=.9f || material.specular.g!=.9f || material.specular.b!=.9f || material.specular.a!=1 ||
        material.power!=0) return false;
    const auto& texture=textures[0]; const auto& format=texture.desc.pixelFormat;
    return texture.desc.width==256 && texture.desc.height==256 && texture.numframes==1 &&
        format.dwRGBBitCount==16 && format.dwRBitMask==0x0f00 && format.dwGBitMask==0x00f0 &&
        format.dwBBitMask==0x000f && format.dwRGBAlphaBitMask==0xf000 && materials[0].texture==0;
}

bool T3DImagery::ValidateRetailFmasteryPartSysProfile()
{
    if (!GetResFilename() || version!=3 || flags!=0xdc || NumStates()!=1 ||
        GetAniLength(0)!=30 || GetAniFlags(0)!=0x2001 || NumObjects()!=3 || NumMaterials()!=2 ||
        NumTextures()!=1 || NumTags()!=1 || numverts!=8 || numfaces!=4) return false;
    std::string path(GetResFilename());
    for(char& c:path)c=c=='\\' ? '/' : char(std::tolower(static_cast<unsigned char>(c)));
    constexpr const char* suffix="magic/fmaster.i3d";const size_t length=std::strlen(suffix);
    if(path.size()<length || path.compare(path.size()-length,length,suffix) ||
       (path.size()!=length && path[path.size()-length-1]!='/'))return false;
    constexpr const char* names[3]={"#rect","fist","#flare"};
    constexpr uint64_t hashes[3]={0xc1066a8ed52e6f34ull,0xc1eeb3c3fa2acd63ull,0x128790d8d4a0698ull};
    for(int i=0;i<3;++i){const auto& object=objects[i];
        if(std::strcmp(object.name,names[i]) || object.parent[0]!=-1 || object.material!=(i==1?1:0) ||
           object.numverts!=(i==1?0:4) || object.numfaces!=(i==1?0:2) ||
           object.numanikeys[0]!=(i==2?38:6) || !object.anikeys[0] || object.numtexfaces[0] ||
           object.numtexfaces[1]!=(i==1?0:2))return false;
        uint64_t h=1469598103934665603ull;const auto* bytes=static_cast<const uint8_t*>(object.anikeys[0]);
        for(size_t j=0;j<size_t(i==2?38:6)*sizeof(SAniKey32);++j){h^=bytes[j];h*=1099511628211ull;}
        if(h!=hashes[i])return false;
    }
    const auto& t=*GetTag(0);
    constexpr const char* parameters="obj=(fist),particle=#rect,pps=30,lifespan=5:15,initialvelocity=1:3,scale=[0:0.75,100:0],color=[0:(255,10,1),10:(255,50,1),100:(10,1,0)],spread=360,azimuth=360,friction=[0:0.1],relvel=0.1";
    if(t.state!=0 || t.frame!=5 || !t.name || !t.str || std::strcmp(t.name,"partsys") ||
       std::strcmp(t.str,parameters))return false;
    // Guard authored material identity. RGB565 alpha-mask0 skips retail blend4 selection409e57..5c.
    const auto& m=materials[0].matdesc;
    if(m.diffuse.r!=1 || m.diffuse.g!=1 || m.diffuse.b!=1 || m.diffuse.a!=1 ||
       m.ambient.r!=1 || m.ambient.g!=1 || m.ambient.b!=1 || m.ambient.a!=1 ||
       m.specular.r!=0.9f || m.specular.g!=0.9f || m.specular.b!=0.9f || m.specular.a!=1 ||
       m.emissive.r!=1 || m.emissive.g!=1 || m.emissive.b!=1 || m.emissive.a!=1 || m.power!=0) return false;
    const auto& tex=textures[0];const auto& pf=tex.desc.pixelFormat;
    return tex.desc.width==64 && tex.desc.height==64 && tex.numframes==1 &&
        pf.dwRGBBitCount==16 && pf.dwRBitMask==0xf800 && pf.dwGBitMask==0x07e0 &&
        pf.dwBBitMask==0x001f && pf.dwRGBAlphaBitMask==0 && materials[0].texture==0;
}

bool T3DImagery::ValidateRetailSpeedPartSysProfile()
{
    if(!GetResFilename() || version!=3 || flags!=0xdc || NumStates()!=1 ||
       GetAniLength(0)!=30 || GetAniFlags(0)!=0x2001 || NumObjects()!=3 ||
       NumMaterials()!=3 || NumTextures()!=2 || NumTags()!=2 || numverts!=8 || numfaces!=4) return false;
    std::string path(GetResFilename());
    for(char& c:path)c=c=='\\' ? '/' : char(std::tolower(static_cast<unsigned char>(c)));
    constexpr const char* suffix="magic/speed.i3d";const size_t length=std::strlen(suffix);
    if(path.size()<length || path.compare(path.size()-length,length,suffix) ||
       (path.size()!=length && path[path.size()-length-1]!='/'))return false;
    const auto fingerprint=[](const void* data,size_t length){
        uint64_t h=1469598103934665603ull;const auto* bytes=static_cast<const uint8_t*>(data);
        for(size_t i=0;i<length;++i){h^=bytes[i];h*=1099511628211ull;}return h;
    };
    constexpr const char* names[3]={"speed","#speedflare","#speedo"};
    constexpr int keycounts[3]={9,38,6};
    constexpr uint64_t keys[3]={0x5eee8f50e1e1f5e6ull,0x0445297c176fd498ull,0xf3ca8b067ae3b3ecull};
    for(int i=0;i<3;++i){const auto& o=objects[i];
        if(std::strcmp(o.name,names[i]) || o.parent[0]!=-1 || o.material!=i ||
           o.numverts!=(i?4:0) || o.numfaces!=(i?2:0) || !o.anikeys[0] ||
           o.numanikeys[0]!=keycounts[i] || fingerprint(o.anikeys[0],keycounts[i]*sizeof(SAniKey32))!=keys[i])return false;
        for(int slot=0;slot<3;++slot)if(o.numtexfaces[slot]!=(slot==i && i?2:0))return false;
    }
    const auto& blend=*GetTag(0);const auto& particles=*GetTag(1);
    constexpr const char* parameters="obj=(#speedflare),particle=#speedo,pps=30,lifespan=20:25,scale=[0:1.5,100:0],localrotation=[0:(0,0,0),100:(0,-360,0)],color=[0:(200,255,55),25:(100,255,25),100:(1,10,1)],emittersize=2,relvel=0.1";
    if(blend.state!=0 || blend.frame!=1 || !blend.name || !blend.str || std::strcmp(blend.name,"blendcont") || std::strcmp(blend.str,"litaddz") ||
       particles.state!=0 || particles.frame!=5 || !particles.name || !particles.str || std::strcmp(particles.name,"partsys") || std::strcmp(particles.str,parameters))return false;
    for(int i=0;i<2;++i){const auto& t=textures[i];const auto& p=t.desc.pixelFormat;
        if(t.desc.width!=(i?32:64) || t.desc.height!=(i?32:64) || t.numframes!=1 ||
           p.dwRGBBitCount!=16 || p.dwRBitMask!=0xf800 || p.dwGBitMask!=0x07e0 || p.dwBBitMask!=0x001f || p.dwRGBAlphaBitMask)return false;
    }
    constexpr uint64_t material_hashes[3]={0x8237bf10191e1de0ull,0xe82f1a8b1ab2978dull,0x2d6e7c4f0d54f390ull};
    for(int i=0;i<3;++i)if(fingerprint(&materials[i].matdesc.diffuse,17*sizeof(float))!=material_hashes[i])return false;
    constexpr uint64_t vertex_hashes[2]={0xdf878a3c89245939ull,0x7795981c8f84ffe0ull};
    constexpr uint16_t face_indices[6]={2,3,0,1,2,0};
    for(int i=1;i<3;++i){S3DVertex vertices[4]{};S3DFace faces[2]{};
        GetObjVerts(i,vertices,0,0);GetObjFaces(i,faces);
        if(fingerprint(vertices,sizeof(vertices))!=vertex_hashes[i-1] || std::memcmp(faces,face_indices,sizeof(faces)))return false;
    }
    return materials[0].texture==-1 && materials[1].texture==0 && materials[2].texture==1;
}

bool T3DImagery::ValidateRetailQuicksilverPartSysProfile()
{
    if(!GetResFilename() || version!=3 || flags!=0xdc || NumStates()!=1 ||
       GetAniLength(0)!=30 || GetAniFlags(0)!=0x2001 || NumObjects()!=3 ||
       NumMaterials()!=3 || NumTextures()!=2 || NumTags()!=2 || numverts!=8 || numfaces!=4) return false;
    std::string path(GetResFilename());
    for(char& c:path)c=c=='\\' ? '/' : char(std::tolower(static_cast<unsigned char>(c)));
    constexpr const char* suffix="magic/quicksilver.i3d";const size_t length=std::strlen(suffix);
    if(path.size()<length || path.compare(path.size()-length,length,suffix) ||
       (path.size()!=length && path[path.size()-length-1]!='/'))return false;
    const auto fingerprint=[](const void* data,size_t length){
        uint64_t h=1469598103934665603ull;const auto* bytes=static_cast<const uint8_t*>(data);
        for(size_t i=0;i<length;++i){h^=bytes[i];h*=1099511628211ull;}return h;
    };
    constexpr const char* names[3]={"speed","#speedflare","#speedo"};
    constexpr int keycounts[3]={9,38,6};
    constexpr uint64_t keys[3]={0x5eee8f50e1e1f5e6ull,0x9d24a4c7a3977790ull,0xf3ca8b067ae3b3ecull};
    for(int i=0;i<3;++i){const auto& o=objects[i];
        if(std::strcmp(o.name,names[i]) || o.parent[0]!=-1 || o.material!=i ||
           o.numverts!=(i?4:0) || o.numfaces!=(i?2:0) || !o.anikeys[0] ||
           o.numanikeys[0]!=keycounts[i] || fingerprint(o.anikeys[0],keycounts[i]*sizeof(SAniKey32))!=keys[i])return false;
        for(int slot=0;slot<3;++slot)if(o.numtexfaces[slot]!=(slot==i && i?2:0))return false;
    }
    const auto& blend=*GetTag(0);const auto& particles=*GetTag(1);
    constexpr const char* parameters="obj=(#speedflare),particle=#speedo,pps=30,lifespan=20:25,scale=[0:1.5,100:0],localrotation=[0:(0,0,0),100:(0,-360,0)],color=[0:(5,80,230),25:(10,75,190),100:(1,25,70)],emittersize=2,relvel=0.1";
    if(blend.state!=0 || blend.frame!=1 || !blend.name || !blend.str || std::strcmp(blend.name,"blendcont") || std::strcmp(blend.str,"litaddz") ||
       particles.state!=0 || particles.frame!=5 || !particles.name || !particles.str || std::strcmp(particles.name,"partsys") || std::strcmp(particles.str,parameters))return false;
    for(int i=0;i<2;++i){const auto& t=textures[i];const auto& p=t.desc.pixelFormat;
        if(t.desc.width!=(i?32:64) || t.desc.height!=(i?32:64) || t.numframes!=1 ||
           p.dwRGBBitCount!=16 || p.dwRBitMask!=0xf800 || p.dwGBitMask!=0x07e0 || p.dwBBitMask!=0x001f || p.dwRGBAlphaBitMask)return false;
    }
    constexpr uint64_t material_hashes[3]={0x8237bf10191e1de0ull,0xe82f1a8b1ab2978dull,0x2d6e7c4f0d54f390ull};
    for(int i=0;i<3;++i)if(fingerprint(&materials[i].matdesc.diffuse,17*sizeof(float))!=material_hashes[i])return false;
    constexpr uint64_t vertex_hashes[2]={0xbfaaf45209499646ull,0x7795981c8f84ffe0ull};
    constexpr uint16_t face_indices[6]={2,3,0,1,2,0};
    for(int i=1;i<3;++i){S3DVertex vertices[4]{};S3DFace faces[2]{};
        GetObjVerts(i,vertices,0,0);GetObjFaces(i,faces);
        if(fingerprint(vertices,sizeof(vertices))!=vertex_hashes[i-1] || std::memcmp(faces,face_indices,sizeof(faces)))return false;
    }
    return materials[0].texture==-1 && materials[1].texture==0 && materials[2].texture==1;
}

uint32_t T3DImagery::ValidateRetailStaticParticleProfile()
{
    if(!GetResFilename() || version!=3 || flags!=0xdc || NumStates()!=1 ||
       GetAniFlags(0)!=0x2001 || NumTextures()!=1 || NumTags()!=1 ||
       numverts!=4 || numfaces!=2)return 0;
    std::string path(GetResFilename());
    for(char& c:path)c=c=='\\' ? '/' : char(std::tolower(static_cast<unsigned char>(c)));
    const auto fingerprint=[](const void* data,size_t length){
        uint64_t h=1469598103934665603ull;const auto* b=static_cast<const uint8_t*>(data);
        for(size_t i=0;i<length;++i){h^=b[i];h*=1099511628211ull;}return h;
    };
    for(const auto& profile:retail_static_particles::profiles){
        const size_t length=std::strlen(profile.suffix);
        if(path.size()<length || path.compare(path.size()-length,length,profile.suffix) ||
           (path.size()!=length && path[path.size()-length-1]!='/'))continue;
        if(GetAniLength(0)!=profile.frames || NumObjects()!=profile.objects ||
           NumMaterials()!=profile.materials)return 0;
        for(int i=0;i<profile.objects;++i){const auto& o=objects[i];const auto& p=profile.object[i];
            if(std::strcmp(o.name,p.name) || o.parent[0]!=-1 || o.material!=p.material ||
               o.numverts!=(i==0?4:0) || o.numfaces!=(i==0?2:0) || !o.anikeys[0] ||
               o.numanikeys[0]!=p.key_count ||
               fingerprint(o.anikeys[0],p.key_count*sizeof(SAniKey32))!=p.keys ||
               o.numtexfaces[0]!=0 || o.numtexfaces[1]!=(i==0?2:0))return 0;
        }
        const auto& tag=*GetTag(0);
        if(tag.state!=0 || tag.frame!=profile.tag_frame || !tag.name || !tag.str ||
           std::strcmp(tag.name,"partsys") || std::strcmp(tag.str,profile.parameters))return 0;
        const auto& t=textures[0];const auto& pf=t.desc.pixelFormat;
        if(t.desc.width!=profile.texture_size || t.desc.height!=profile.texture_size || t.numframes!=1 ||
           pf.dwRGBBitCount!=16 || pf.dwRBitMask!=0xf800 || pf.dwGBitMask!=0x07e0 ||
           pf.dwBBitMask!=0x001f || pf.dwRGBAlphaBitMask)return 0;
        for(int i=0;i<profile.materials;++i)
            if(materials[i].texture!=(i==0?0:-1) ||
               fingerprint(&materials[i].matdesc.diffuse,17*sizeof(float))!=profile.material[i])return 0;
        S3DVertex vertices[4]{};S3DFace faces[2]{};
        GetObjVerts(0,vertices,0,0);GetObjFaces(0,faces);
        constexpr uint16_t indices[6]={2,3,0,1,2,0};
        if(fingerprint(vertices,sizeof(vertices))!=profile.vertices || std::memcmp(faces,indices,sizeof(faces)))return 0;
        return profile.id;
    }
    return 0;
}

void T3DImagery::InitializePartSysTracks()
{
    partsys_tracks.clear();
    gold_partsys_profile = ValidateGoldPartSysProfile();
    might_partsys_profile = ValidateRetailMightPartSysProfile();
    immortalmight_partsys_profile = ValidateRetailImmortalmightPartSysProfile();
    fmastery_partsys_profile = ValidateRetailFmasteryPartSysProfile();
    speed_partsys_profile = ValidateRetailSpeedPartSysProfile();
    quicksilver_partsys_profile = ValidateRetailQuicksilverPartSysProfile();
    static_particles_profile = ValidateRetailStaticParticleProfile();
    combatflash_start1_partsys_profile = ValidateCombatFlashStart1PartSysProfile();
    for (int32_t i = 0; i < NumTags(); ++i)
    {
        const S3DTag& tag = *GetTag(i);
        if (!tag.name || stricmp(tag.name, "partsys")) continue;
        SPartSysTrack track;
        track.state = tag.state;
        track.tagframe = tag.frame;
        const auto unsupported = [&](const char* reason) {
            track.diagnostic = reason;
        };
        if (!authored_partsys::ParseDefinition(tag.str ? tag.str : "", track.definition,
                                             track.diagnostic))
        {
            // Atomic parser failure retains no guessed emitter or prototype.
        }
        else if (combatflash_start1_partsys_profile && tag.state != 0)
            unsupported("CombatFlash start1 profile supports state0 only");
        else if ((flags & I3D_ISMORPH) || tag.state < 0 || tag.state >= NumStates())
            unsupported("morph or all-state partsys rendering is not recovered");
        else
        {
            track.prototype = GetObjectNum(const_cast<char*>(track.definition.particle.c_str()));
            for (const std::string& name : track.definition.objects)
            {
                const int32_t emitter = GetObjectNum(const_cast<char*>(name.c_str()));
                if (emitter < 0) { unsupported("authored emitter object not found"); break; }
                track.emitters.push_back(emitter);
            }
            if (track.definition.objects.empty())
                unsupported("implicit all-object emitters are not recovered");
            if (track.prototype < 0 || NumObjVerts(track.prototype) != 4 ||
                NumObjFaces(track.prototype) != 2)
                unsupported("only the authored four-vertex/two-face water prototype is recovered");
            if (track.diagnostic.empty())
            {
                S3DFace faces[2] = {};
                int32_t starts[MAXTEXTURES + 1] = {}, counts[MAXTEXTURES + 1] = {};
                GetObjFaces(track.prototype, faces, starts, counts);
                if (faces[0].v1 != 2 || faces[0].v2 != 3 || faces[0].v3 != 0 ||
                    faces[1].v1 != 1 || faces[1].v2 != 2 || faces[1].v3 != 0)
                    unsupported("prototype topology differs from the audited water triangles");
                for (int32_t slot = 0; slot <= NumTextures(); ++slot)
                    if (counts[slot])
                    {
                        if (slot == 0 || counts[slot] != 2 || track.texture_slot >= 0)
                            unsupported("prototype requires one textured triangle pair");
                        track.texture_slot = slot;
                    }
                if (track.texture_slot < 1)
                    unsupported("prototype texture is absent");
                if (track.definition.blendmode != 16 &&
                    !(gold_partsys_profile && track.definition.blendmode == 2))
                    unsupported("only the recovered water litadd blend mode is implemented");
                if (track.diagnostic.empty())
                {
                    GetObjVerts(track.prototype, track.vertices.data(), tag.state, 0);
                    track.supported = true;
                }
            }
        }
        // Other controllers cannot be silently skipped on a claimed supported
        // owner. These four water assets have exactly their single partsys tag.
        for (int32_t j = 0; j < NumTags() && track.supported; ++j)
        {
            const char* name = GetTag(j)->name;
            if ((gold_partsys_profile || combatflash_start1_partsys_profile || speed_partsys_profile || quicksilver_partsys_profile) && name && !stricmp(name, "blendcont")) continue;
            if (name && stricmp(name, "partsys") && stricmp(name, "scrolltex") &&
                stricmp(name, "play") && stricmp(name, "beg") && stricmp(name, "end"))
            {
                track.supported = false;
                track.diagnostic = std::string("unrecovered companion controller: ") + name;
            }
        }
        if (!track.supported)
            log_warn("[partsys] unsupported imagery=%s state=%d tagframe=%d reason=%s",
                     GetResFilename(), track.state, track.tagframe, track.diagnostic.c_str());
        partsys_tracks.push_back(std::move(track));
    }
}

// Narrow source-backed controller grammar; unsupported syntax is explicit.
static bool ParseScrollTexParams(const char* params, std::string& object,
                                 float& du, float& dv)
{
    if (!params) return false;
    bool have_obj = false, have_du = false, have_dv = false;
    std::string input(params);
    size_t start = 0;
    const auto trim = [](std::string value) {
        const size_t first = value.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return std::string();
        return value.substr(first, value.find_last_not_of(" \t\r\n") - first + 1);
    };
    while (start < input.size())
    {
        const size_t comma = input.find(',', start);
        const std::string item = trim(input.substr(start, comma - start));
        const size_t equal = item.find('=');
        if (equal == std::string::npos) return false;
        const std::string key = trim(item.substr(0, equal));
        const std::string value = trim(item.substr(equal + 1));
        if (value.empty()) return false;
        if (!stricmp(key.c_str(), "obj"))
        {
            if (have_obj || value.find_first_of("()\"'") != std::string::npos) return false;
            for (unsigned char c : value)
                if (!std::isalnum(c) && c != '_' && c != '#') return false;
            object = value; have_obj = true;
        }
        else if (!stricmp(key.c_str(), "du") || !stricmp(key.c_str(), "dv"))
        {
            char* end = nullptr;
            const double parsed = std::strtod(value.c_str(), &end);
            const float rounded = float(parsed);
            if (end == value.c_str() || *end || !std::isfinite(rounded)) return false;
            if (!stricmp(key.c_str(), "du"))
            { if (have_du) return false; du = rounded; have_du = true; }
            else
            { if (have_dv) return false; dv = rounded; have_dv = true; }
        }
        else return false;
        if (comma == std::string::npos) break;
        start = comma + 1;
        if (start == input.size()) return false;
    }
    return have_obj && have_du && have_dv;
}

void T3DImagery::InitializeScrollTexTracks(S3DImageryBody* mesh)
{
    scrolltex_tracks.clear();
    if (!mesh || mesh->version < 3) return;
    for (int32_t i = 0; i < mesh->numtags; ++i)
    {
        S3DImageryTag& tag = mesh->tags[i];
        const char* name = static_cast<const char*>(tag.name.ptr());
        if (!name || stricmp(name, "scrolltex")) continue;
        std::string object;
        SScrollTexTrack track;
        track.state = tag.state; track.tagframe = tag.frame;
        const char* params = static_cast<const char*>(tag.str.ptr());
        if (!ParseScrollTexParams(params, object, track.du, track.dv))
        {
            log_warn("[scrolltex] unsupported tag in %s: '%s' (requires single obj, du, dv)",
                     GetResFilename(), params ? params : "<null>");
            continue;
        }
        const int32_t matched = GetObjectNum(const_cast<char*>(object.c_str()));
        // Retail 0x401839..0x401866 selects every animator object if the
        // successfully parsed selector found none. Several shipped water tags
        // retain an old object name (water vs waterstr/waterbend, wave vs wave02).
        const int32_t first = matched >= 0 ? matched : 0;
        const int32_t last = matched >= 0 ? matched + 1 : NumObjects();
        for (int32_t target = first; target < last; ++target)
        {
            track.object = target;
            // Multiple controllers on one object can capture a prior controller's
            // modified UVs; keep those unsupported rather than approximate them.
            bool overlaps = false;
            for (const auto& prior : scrolltex_tracks)
                if (prior.object == track.object &&
                    (prior.state == track.state || prior.state == -1 || track.state == -1))
                    overlaps = true;
            if (overlaps)
            {
                log_warn("[scrolltex] overlapping controllers unsupported in %s object=%s",
                         GetResFilename(), object.c_str());
                continue;
            }
            scrolltex_tracks.push_back(track);
            log_info("[scrolltex] authored %s object=%s index=%d state=%d tagframe=%d du=%g dv=%g",
                     GetResFilename(), object.c_str(), target, track.state, track.tagframe, track.du, track.dv);
        }
    }
}

bool T3DImagery::ScrollTexOffset(int32_t object, int32_t state, int32_t frame,
                                int64_t legacy_frame, float out_uv[2]) const
{
    out_uv[0] = out_uv[1] = 0.0f;
    for (const auto& track : scrolltex_tracks)
    {
        if (track.object != object || (track.state != -1 && track.state != state) ||
            frame < track.tagframe) continue;
        // legacy/3dcont.cpp:148-150: global synchronized float frame count,
        // never render FPS or time elapsed since this instance appeared.
        const float f = float(legacy_frame);
        out_uv[0] = track.du * f;
        out_uv[1] = track.dv * f;
        return true;
    }
    return false;
}

static inline uint8_t ExpandBitsTo8(uint32_t value, int32_t bits)
{
    if (bits <= 0) return 0;
    const uint32_t maxv = (1u << bits) - 1u;
    return (uint8_t)((value * 255u + (maxv >> 1)) / maxv);
}

static inline int32_t CountBits32(uint32_t d)
{
    int32_t n = 0;
    for (int32_t c = 0; c < 32; c++)
        if (d & (1u << c))
            n++;
    return n;
}

static inline int32_t MaskShift32(uint32_t mask)
{
    if (!mask) return 0;
    int32_t s = 0;
    while (!(mask & 1u) && s < 32) { mask >>= 1; s++; }
    return s;
}

static bool DecodeTextureFrameRGBA(const SSurfaceDesc* srcsd, const void* srcpixels, const void* srcpal,
    std::vector<uint8_t>& rgba)
{
    if (!srcsd || !srcpixels || srcsd->width == 0 || srcsd->height == 0)
        return false;

    const int32_t w = (int32_t)srcsd->width;
    const int32_t h = (int32_t)srcsd->height;
    const int32_t src_bpp = (int32_t)(srcsd->pixelFormat.dwRGBBitCount / 8u);
    const int32_t pitch = srcsd->pitch > 0 ? srcsd->pitch : (w * (src_bpp > 0 ? src_bpp : 1));
    rgba.resize((size_t)w * (size_t)h * 4u);

    const uint32_t pf_flags = srcsd->pixelFormat.dwFlags;
    const uint32_t rmask = srcsd->pixelFormat.dwRBitMask;
    const uint32_t gmask = srcsd->pixelFormat.dwGBitMask;
    const uint32_t bmask = srcsd->pixelFormat.dwBBitMask;
    const uint32_t amask = srcsd->pixelFormat.dwRGBAlphaBitMask;

    if (srcsd->pixelFormat.dwRGBBitCount == 16 && rmask && gmask && bmask)
    {
        const int32_t rs = MaskShift32(rmask), gs = MaskShift32(gmask), bs = MaskShift32(bmask), as = MaskShift32(amask);
        const int32_t rb = CountBits32(rmask), gb = CountBits32(gmask), bb = CountBits32(bmask), ab = CountBits32(amask);
        for (int32_t y = 0; y < h; ++y)
        {
            const uint16_t* row = (const uint16_t*)((const uint8_t*)srcpixels + y * pitch);
            uint8_t* dst = rgba.data() + (size_t)y * (size_t)w * 4u;
            for (int32_t x = 0; x < w; ++x, dst += 4)
            {
                const uint32_t px = row[x];
                dst[0] = ExpandBitsTo8((px & rmask) >> rs, rb);
                dst[1] = ExpandBitsTo8((px & gmask) >> gs, gb);
                dst[2] = ExpandBitsTo8((px & bmask) >> bs, bb);
                dst[3] = amask ? ExpandBitsTo8((px & amask) >> as, ab) : 255;
            }
        }
        return true;
    }

    if ((srcsd->pixelFormat.dwRGBBitCount == 24 || srcsd->pixelFormat.dwRGBBitCount == 32) &&
        rmask && gmask && bmask)
    {
        const int32_t rs = MaskShift32(rmask), gs = MaskShift32(gmask), bs = MaskShift32(bmask), as = MaskShift32(amask);
        const int32_t rb = CountBits32(rmask), gb = CountBits32(gmask), bb = CountBits32(bmask), ab = CountBits32(amask);
        const int32_t bytespp = (int32_t)(srcsd->pixelFormat.dwRGBBitCount / 8u);
        for (int32_t y = 0; y < h; ++y)
        {
            const uint8_t* row = (const uint8_t*)srcpixels + y * pitch;
            uint8_t* dst = rgba.data() + (size_t)y * (size_t)w * 4u;
            for (int32_t x = 0; x < w; ++x, dst += 4)
            {
                uint32_t px = 0;
                std::memcpy(&px, row + x * bytespp, bytespp);
                dst[0] = ExpandBitsTo8((px & rmask) >> rs, rb);
                dst[1] = ExpandBitsTo8((px & gmask) >> gs, gb);
                dst[2] = ExpandBitsTo8((px & bmask) >> bs, bb);
                dst[3] = amask ? ExpandBitsTo8((px & amask) >> as, ab) : 255;
            }
        }
        return true;
    }

    // Common paletted path: palette entries are 16-bit 555/565-style colors.
    if ((srcsd->pixelFormat.dwRGBBitCount == 8 || (pf_flags & 0x20)) && srcpal)
    {
        const uint16_t* pal = (const uint16_t*)srcpal;
        for (int32_t y = 0; y < h; ++y)
        {
            const uint8_t* row = (const uint8_t*)srcpixels + y * pitch;
            uint8_t* dst = rgba.data() + (size_t)y * (size_t)w * 4u;
            for (int32_t x = 0; x < w; ++x, dst += 4)
            {
                const uint16_t px = pal[row[x]];
                dst[0] = ExpandBitsTo8((px >> 10) & 0x1F, 5);
                dst[1] = ExpandBitsTo8((px >> 5) & 0x1F, 5);
                dst[2] = ExpandBitsTo8(px & 0x1F, 5);
                dst[3] = 255;
            }
        }
        return true;
    }

    return false;
}

static uint64_t I3DTextureAssetKey(AssetUid asset_id, int32_t texture_index, int32_t frame_index,
                                  bool repeat = false)
{
    uint64_t hash = 1469598103934665603ull;
    auto mix = [&hash](uint64_t value) {
        hash ^= value;
        hash *= 1099511628211ull;
    };
    mix(asset_id);
    mix(uint64_t(uint32_t(texture_index)));
    mix(uint64_t(uint32_t(frame_index)));
    if (repeat) mix(0x525054u); // distinct sampler identity; preserve existing clamp keys
    return hash ? hash : 1ull;
}

// Use the shared row-vector matrix helpers from math3d.cpp directly so the
// imagery hierarchy math matches the rest of the source path.

// **********************
// * 3DImagery Funtions *
// **********************

REGISTER_IMAGERYBUILDER(T3DImagery);

T3DImagery::T3DImagery(int32_t imageryid) : TObjectImagery(imageryid)
{
    meshinitialized = false;
}

T3DImagery::~T3DImagery()
{
    ClearMesh();
}

// ****************** MESH LOADING STUFF **********************

// Static — off by default. The i3ddump test mode (testmodes.cpp) sets
// this to true before invoking FindImagery so the texture loader retains
// the decoded RGBA bytes per frame for PNG export.
bool T3DImagery::g_retain_decoded_rgba = false;

bool T3DImagery::OldInitializeMesh(SOld3DImageryBody* mesh)
{
    int32_t c;

    if (meshinitialized)
        return true;
    meshinitialized = true;

    flags = mesh->flags;
    version = 0;

    // ---- morph vertex lists ----
    numverts = mesh->numverts;

    int32_t vertstates = !(flags & I3D_ISMORPH) ? 1 : NumStates();

    verts = new S3DVertex**[vertstates];

    for (c = 0; c < vertstates; c++)
    {
        int32_t vertframes = !(flags & I3D_ISMORPH) ? 1 : GetAniLength(c);

        verts[c] = new S3DVertex*[vertframes];
        for (int32_t fr = 0; fr < vertframes; fr++)
        {
            verts[c][fr] = new S3DVertex[numverts];
            OFFSET* meshverts = (OFFSET*)((void*)mesh->verts[c]);
            std::memcpy(verts[c][fr], (void*)meshverts[fr], sizeof(S3DVertex) * numverts);
        }
    }

    // ---- faces ----
    numfaces = mesh->numfaces;
    faces = (S3DFace*)new uint16_t[mesh->numfaces * 3];
    std::memcpy(faces, (void*)mesh->faces, sizeof(uint16_t) * mesh->numfaces * 3);

    // ---- objects ----
    if (mesh->numobjects > 5000 || mesh->numobjects <= 0)
        return false;

    for (c = 0; c < mesh->numobjects; c++)
    {
        S3DObj obj;
        int32_t d;

        std::memcpy(obj.name, mesh->objname[c], RESNAMELEN);
        obj.material = mesh->objmaterial[c];
        obj.numverts = mesh->objvertnum[c];
        obj.startvert = mesh->objvertpos[c];
        obj.numfaces = 0;
        obj.startface = mesh->objfacepos[c][0];
        obj.numtexfaces = new int32_t[mesh->numtextures + 1];
        obj.texfaces = new int32_t[mesh->numtextures + 1];
        for (d = 0; d < mesh->numtextures + 1; d++)
        {
            obj.numfaces += mesh->objfacenum[c][d];
            obj.numtexfaces[d] = mesh->objfacenum[c][d];
            obj.texfaces[d] = mesh->objfacepos[c][d] - obj.startface;
        }

        obj.parent = new int32_t[NumStates()];
        if (mesh->flags & I3D_HASHIERARCHY)
        {
            unsigned char* objparent = (unsigned char*)mesh->objparent[c].ptr();
            for (d = 0; d < NumStates(); d++)
                obj.parent[d] = objparent[d];
        }
        else
        {
            for (d = 0; d < NumStates(); d++)
                obj.parent[d] = -1;
        }

        obj.anikeys = new void*[NumStates()];
        obj.numanikeys = new int32_t[NumStates()];
        for (d = 0; d < NumStates(); d++)
        {
            if (mesh->anikeys[d].ptr() == nullptr)
            {
                obj.numanikeys[d] = 0;
                obj.anikeys[d] = nullptr;
            }
            else
            {
                // Old (pre-release) I3D anikeys layout is broken in our
                // build: SOld3DImageryBody.anikeys[d] is one OFFSET per
                // state, but its actual buffer is shorter than
                // `sizeof(SAniKey) * GetAniLength(d)` — likely a struct-
                // size mismatch (retail SAniKey32 = 4 bytes vs our
                // SAniKey = 8 bytes) or a per-state-vs-flattened layout
                // disagreement. ASan caught it as a 1200-byte heap-
                // buffer-overflow READ.
                //
                // Until the loader is reverse-engineered against the on-
                // disk Old format, allocate empty keys here. Affected
                // meshes (anything that goes through OldInitializeMesh)
                // will animate with zero deltas — visible as "frozen"
                // bones — but the engine no longer over-reads heap.
                // TODO retail: figure out the real source size of
                //   SOld3DImageryBody.anikeys[d] (probably SAniKey32 +
                //   per-state count) and copy correctly.
                obj.anikeys[d] = new SAniKey[GetAniLength(d)];
                obj.numanikeys[d] = GetAniLength(d);
                std::memset(obj.anikeys[d], 0, sizeof(SAniKey) * GetAniLength(d));
            }
        }

        AddObject(&obj);
    }

    // ---- motion data ----
    motion = new SMotionData*[NumStates()];
    std::memset(motion, 0, sizeof(SMotionData*) * NumStates());
    if (!(flags & I3D_ISMORPH))
    {
        for (c = 0; c < NumStates(); c++)
        {
            if (mesh->motion[c] == nullptr)
            {
                motion[c] = nullptr;
                GetHeader()->states[c].aniflags |= AF_NOMOTION;
            }
            else
            {
                motion[c] = new SMotionData[GetAniLength(c)];

                SOldOldMotionData* omd = (SOldOldMotionData*)(mesh->motion[c].ptr());
                SMotionData* nmd = motion[c];
                for (int32_t d = 0; d < GetAniLength(c); d++, omd++, nmd++)
                {
                    nmd->dist = (omd->dist) >> 8;
                    nmd->vert = 0;
                    nmd->ang = omd->ang;
                    nmd->rotx = nmd->roty = 0;
                    nmd->rotz = omd->ang;
                }
            }
        }
    }

    // ---- textures ----
    hastextures = mesh->numtextures != 0;
    textures.Clear();
    if (UseTextures)
    {
        for (c = 0; c < mesh->numtextures; c++)
        {
            AddTexture(
                &mesh->texturedesc[c],
                (OFFSET*)((void*)mesh->texturebits[c]),
                mesh->textureframes[c],
                (void*)mesh->texturepals[c]);
        }
    }

    // ---- materials ----
    materials.Clear();
    for (c = 0; c < mesh->nummaterials; c++)
    {
        AddMaterial(&mesh->material[c], (int32_t)mesh->material[c].hTexture);
    }

    // ---- icons ----
    icons = nullptr;
    if (flags & I3D_HASICONS)
    {
        for (c = 0; c < NumStates(); c++)
        {
            S3DStateImagery* im = (S3DStateImagery*)((void*)mesh->imagery[c]);
            if (!im)
                continue;

            if (im->invsize > 0)
            {
                // Allocate AND zero the icon array exactly once, on the first
                // state that has an icon. (Zeroing inside the loop wiped every
                // previously-loaded state's icon, leaving only the last one — so
                // GetInvImage(state) returned null for any other state.)
                if (!icons)
                {
                    icons = new S3DImageryIcons[NumStates()];
                    std::memset(icons, 0, sizeof(S3DImageryIcons) * NumStates());
                }

                uint8_t* icon = new uint8_t[im->invsize];
                if ((void*)im->invitem)
                {
                    std::memcpy(icon, im->invitem, im->invsize);
                    icons[c].invitem = (TBitmap*)icon;
                }
                else if ((void*)im->invanim)
                {
                    std::memcpy(icon, im->invanim, im->invsize);
                    icons[c].invanim = (TAnimation*)icon;
                }
            }
        }
    }

    tags.Clear();
    FreeBody();
    return true;
}

bool T3DImagery::InitializeMesh(S3DImageryBody* mesh)
{
    int32_t c;

    if (meshinitialized)
        return true;

    if (!(mesh->flags & I3D_3DIMAGEBODY2))
        return OldInitializeMesh((SOld3DImageryBody*)mesh);

    meshinitialized = true;

    flags = mesh->flags;
    version = mesh->version;
    if (version > VERSION3DIMAGEBODY)
        log_warn("[i3d] newer imagery version in file (%u) for %s",
                 version, GetResFilename());

    // ---- morph vertex lists ----
    numverts = mesh->numverts;

    int32_t vertstates = !(flags & I3D_ISMORPH) ? 1 : NumStates();

    verts = new S3DVertex**[vertstates];

    for (c = 0; c < vertstates; c++)
    {
        int32_t vertframes = !(flags & I3D_ISMORPH) ? 1 : GetAniLength(c);

        verts[c] = new S3DVertex*[vertframes];
        for (int32_t fr = 0; fr < vertframes; fr++)
        {
            verts[c][fr] = new S3DVertex[numverts];
            S3DVertex* meshverts = (S3DVertex*)mesh->verts[c][fr];
            std::memcpy(verts[c][fr], meshverts, sizeof(S3DVertex) * numverts);
        }
    }

    // ---- faces ----
    numfaces = mesh->numfaces;
    faces = (S3DFace*)new uint16_t[mesh->numfaces * 3];
    std::memcpy(faces, (void*)mesh->faces, sizeof(uint16_t) * mesh->numfaces * 3);

    // ---- objects ----
    if (mesh->numobjects > 5000 || mesh->numobjects <= 0)
        return false;

    for (c = 0; c < mesh->numobjects; c++)
    {
        S3DObj obj;
        int32_t d;

        std::memcpy(obj.name, mesh->objects[c].name, RESNAMELEN);
        obj.material = mesh->objects[c].material;
        obj.numverts = mesh->objects[c].vertnum;
        obj.startvert = mesh->objects[c].vertpos;
        obj.numfaces = 0;
        obj.startface = mesh->objects[c].textures[0].facepos;
        obj.numtexfaces = new int32_t[mesh->numtextures + 1];
        obj.texfaces = new int32_t[mesh->numtextures + 1];
        for (d = 0; d < mesh->numtextures + 1; d++)
        {
            obj.numfaces += mesh->objects[c].textures[d].facenum;
            obj.numtexfaces[d] = mesh->objects[c].textures[d].facenum;
            obj.texfaces[d] = mesh->objects[c].textures[d].facepos - obj.startface;
        }

        obj.parent = new int32_t[NumStates()];
        if (mesh->flags & I3D_HASHIERARCHY)
        {
            for (d = 0; d < NumStates(); d++)
            {
                if (mesh->flags & I3D_ANIKEY32)
                    obj.parent[d] = mesh->objects[c].states[d].parent;
                else
                {
                    S3DOldImageryObjectState* st =
                        (S3DOldImageryObjectState*)mesh->objects[c].states.ptr();
                    obj.parent[d] = st[d].parent;
                }
            }
        }
        else
        {
            for (d = 0; d < NumStates(); d++)
                obj.parent[d] = -1;
        }

        obj.anikeys = new void*[NumStates()];
        obj.numanikeys = new int32_t[NumStates()];
        for (d = 0; d < NumStates(); d++)
        {
            if (mesh->flags & I3D_ANIKEY32)
            {
                S3DImageryObjectState* st = &(mesh->objects[c].states[d]);
                if (st->anikeys.ptr() == nullptr)
                {
                    obj.anikeys[d] = nullptr;
                    obj.numanikeys[d] = 0;
                }
                else
                {
                    obj.anikeys[d] = new SAniKey32[st->numanikeys];
                    obj.numanikeys[d] = st->numanikeys;
                    std::memcpy(obj.anikeys[d], st->anikeys.ptr(),
                        sizeof(SAniKey32) * st->numanikeys);
                }
            }
            else
            {
                S3DOldImageryObjectState* st =
                    &(((S3DOldImageryObjectState*)(mesh->objects[c].states.ptr()))[d]);
                if (st->anikeys.ptr() == nullptr)
                {
                    obj.anikeys[d] = nullptr;
                    obj.numanikeys[d] = 0;
                }
                else
                {
                    obj.anikeys[d] = new SAniKey[GetAniLength(d)];
                    obj.numanikeys[d] = GetAniLength(d);
                    std::memcpy(obj.anikeys[d], st->anikeys.ptr(),
                        sizeof(SAniKey) * GetAniLength(d));
                }
            }
        }

        AddObject(&obj);
    }

    // ---- motion data ----
    motion = new SMotionData*[NumStates()];
    std::memset(motion, 0, sizeof(SMotionData*) * NumStates());
    if (!(flags & I3D_ISMORPH))
    {
        for (c = 0; c < NumStates(); c++)
        {
            S3DImageryState* ist;
            if (mesh->version < 2)
                ist = (S3DImageryState*)&(((S3DOldImageryState2*)(mesh->statedata.ptr()))[c]);
            else if (mesh->version == 2)
                ist = (S3DImageryState*)&(((S3DOldImageryState3*)(mesh->statedata.ptr()))[c]);
            else
                ist = &(mesh->statedata[c]);

            if (!ist->motion.ptr())
            {
                motion[c] = nullptr;
                GetHeader()->states[c].aniflags |= AF_NOMOTION;
            }
            else
            {
                motion[c] = new SMotionData[GetAniLength(c)];

                if (mesh->flags & I3D_ROOTMOTION)
                    std::memcpy(motion[c], ist->motion.ptr(),
                        sizeof(SMotionData) * GetAniLength(c));
                else if (mesh->flags & I3D_FACINGMOTION)
                {
                    SOldMotionData* omd = (SOldMotionData*)(ist->motion.ptr());
                    SMotionData* nmd = motion[c];
                    for (int32_t d = 0; d < GetAniLength(c); d++, omd++, nmd++)
                    {
                        nmd->dist = omd->dist;
                        nmd->vert = 0;
                        nmd->ang = omd->ang;
                        nmd->rotx = nmd->roty = 0;
                        nmd->rotz = omd->face;
                    }
                }
                else
                {
                    SOldOldMotionData* omd = (SOldOldMotionData*)(ist->motion.ptr());
                    SMotionData* nmd = motion[c];
                    for (int32_t d = 0; d < GetAniLength(c); d++, omd++, nmd++)
                    {
                        nmd->dist = (omd->dist) >> 8;
                        nmd->vert = 0;
                        nmd->ang = omd->ang;
                        nmd->rotx = nmd->roty = 0;
                        nmd->rotz = omd->ang;
                    }
                }
            }
        }
    }

    // Parse immutable controller metadata before choosing texture samplers.
    InitializeScrollTexTracks(mesh);

    // ---- textures ----
    hastextures = mesh->numtextures != 0;
    textures.Clear();
    if (UseTextures)
    {
        for (c = 0; c < mesh->numtextures; c++)
        {
            AddTexture(
                &mesh->textures[c].desc,
                (OFFSET*)((void*)mesh->textures[c].bits),
                mesh->textures[c].frames,
                (void*)mesh->textures[c].pals);
        }
    }

    // ---- materials ----
    materials.Clear();
    for (c = 0; c < mesh->nummaterials; c++)
    {
        AddMaterial(&mesh->materials[c], (int32_t)mesh->materials[c].hTexture);
    }

    // ---- icons ----
    icons = nullptr;
    if (flags & I3D_HASICONS)
    {
        for (c = 0; c < NumStates(); c++)
        {
            S3DImageryState* im;
            if (mesh->version < 2)
                im = (S3DImageryState*)&(((S3DOldImageryState2*)(mesh->statedata.ptr()))[c]);
            else if (mesh->version == 2)
                im = (S3DImageryState*)&(((S3DOldImageryState3*)(mesh->statedata.ptr()))[c]);
            else
                im = &(mesh->statedata[c]);

            if (im->invsize > 0)
            {
                // Allocate AND zero the icon array exactly once, on the first
                // state that has an icon. (Zeroing inside the loop wiped every
                // previously-loaded state's icon, leaving only the last one — so
                // GetInvImage(state) returned null for any other state.)
                if (!icons)
                {
                    icons = new S3DImageryIcons[NumStates()];
                    std::memset(icons, 0, sizeof(S3DImageryIcons) * NumStates());
                }

                uint8_t* icon = new uint8_t[im->invsize];
                if ((void*)im->invitem)
                {
                    std::memcpy(icon, im->invitem, im->invsize);
                    icons[c].invitem = (TBitmap*)icon;
                }
                else if ((void*)im->invanim)
                {
                    std::memcpy(icon, im->invanim, im->invsize);
                    icons[c].invanim = (TAnimation*)icon;
                }
            }
        }
    }

    // ---- tags ----
    tags.Clear();

    if (mesh->version < 2 && (mesh->flags & I3D_HASPLAYSOUND))
    {
        S3DImageryPlaySound* ps = (S3DImageryPlaySound*)(mesh->tags.ptr());
        for (int32_t pspos = 0; pspos < mesh->numtags; pspos++, ps++)
            tags.AddPtr(new S3DTag(ps->state, ps->frame, (char*)"play", ps->sounds));
    }
    else if (mesh->version == 2)
    {
        S3DImageryPlaySound* ps = (S3DImageryPlaySound*)mesh->tags.ptr();
        int32_t pspos = 0;

        for (c = 0; c < NumStates(); c++)
        {
            S3DOldImageryState3* ist = &(((S3DOldImageryState3*)(mesh->statedata.ptr()))[c]);

            if (ist->begstate[0])
                tags.AddPtr(new S3DTag(c, 0, (char*)"beg", ist->begstate));
            if (ist->endstate[0])
                tags.AddPtr(new S3DTag(c, 0, (char*)"end", ist->endstate));

            if (mesh->flags & I3D_HASPLAYSOUND)
            {
                while (pspos < mesh->numtags && ps->state == c)
                {
                    tags.AddPtr(new S3DTag(ps->state, ps->frame, (char*)"play", ps->sounds));
                    ps++;
                    pspos++;
                }
            }
        }
    }
    else if (mesh->version >= 3)
    {
        S3DImageryTag* tg = mesh->tags;
        for (c = 0; c < mesh->numtags; c++, tg++)
        {
            tags.AddPtr(new S3DTag(tg->state, tg->frame,
                (char*)tg->name.ptr(), (char*)tg->str.ptr()));
        }
    }

    InitializePartSysTracks();
    retail_punch_keys = ValidateRetailPunchProfile();
    retail_mpappear_start_profile = ValidateRetailMPAppearStartProfile();
    retail_shadowfist_profile = ValidateRetailShadowfistProfile();
    retail_warriorborn_profile = ValidateRetailWarriorbornProfile();
    retail_teleportation_profile = ValidateRetailTeleportationProfile();
    ResolveStateBlends();

    // ---- mount sounds referenced by play tags ----
    for (c = 0; c < tags.NumItems(); c++)
    {
        if (!stricmp(tags[c].name, "play"))
        {
            int32_t num = listnum(tags[c].name);
            for (int32_t n = 0; n < num; n++)
                SoundPlayer.Mount(listget(tags[c].str, n));
        }
    }

    FreeBody();
    return true;
}

void T3DImagery::ClearMesh()
{
    int32_t i;

    if (!meshinitialized)
        return;

    ClearObjects();
    ClearTextures();
    ClearMaterials();
    stateblend.clear();

    for (i = 0; i < NumStates(); i++)
    {
        if (motion[i])
            delete motion[i];
    }
    delete motion;
    motion = nullptr;

    int32_t vstates = !(flags & I3D_ISMORPH) ? 1 : NumStates();
    for (i = 0; i < vstates; i++)
    {
        int32_t vframes = !(flags & I3D_ISMORPH) ? 1 : GetAniLength(i);
        for (int32_t j = 0; j < vframes; j++)
            delete verts[i][j];
        delete verts[i];
    }
    delete verts;
    verts = nullptr;
    numverts = 0;

    delete faces;
    faces = nullptr;

    if (icons)
    {
        for (int32_t c = 0; c < NumStates(); c++)
        {
            if (icons[c].invitem)
            {
                delete icons[c].invitem;
                icons[c].invitem = nullptr;
            }
            if (icons[c].invanim)
            {
                delete icons[c].invanim;
                icons[c].invanim = nullptr;
            }
        }
        delete icons;
    }
    icons = nullptr;

    for (i = 0; i < tags.NumItems(); i++)
    {
        if (!stricmp(tags[i].name, "play"))
        {
            int32_t num = listnum(tags[i].str);
            for (int32_t n = 0; n < num; n++)
                SoundPlayer.Unmount(listget(tags[i].str, n));
        }
    }

    tags.Clear();
    scrolltex_tracks.clear();
    partsys_tracks.clear();
    gold_partsys_profile = false;
    might_partsys_profile = false;
    immortalmight_partsys_profile = false;
    fmastery_partsys_profile = false;
    speed_partsys_profile = false;
    quicksilver_partsys_profile = false;
    static_particles_profile = 0;
    retail_punch_keys = false;
    retail_mpappear_start_profile = false;
    retail_shadowfist_profile = false;
    retail_warriorborn_profile = false;
    retail_teleportation_profile = false;
    combatflash_start1_partsys_profile = false;

    meshinitialized = false;
}

bool T3DImagery::Restore()
{
    if (SurfacesLost())
    {
        ClearMesh();
        if (!InitializeMesh((S3DImageryBody*)GetBody()))
            return false;
    }
    return true;
}

// *****************************
// * Verts and Faces Functions *
// *****************************

int32_t T3DImagery::NumVerts()
{
    if (!meshinitialized)
        InitializeMesh((S3DImageryBody*)GetBody());
    return numverts;
}

void T3DImagery::GetVerts(void* vertbuf, int32_t state, int32_t frame,
    ERender3DVertex verttype, int32_t beg, int32_t len)
{
    if (!meshinitialized)
        InitializeMesh((S3DImageryBody*)GetBody());

    if (beg < 0) beg = 0;
    if (len < 0) len = numverts;

    S3DVertex* v = (flags & I3D_ISMORPH) ? verts[state][frame] : verts[0][0];

    if (verttype == ERender3DVertex::Vertex)
    {
        std::memcpy(vertbuf, v + beg, sizeof(S3DVertex) * len);
    }
    else if (verttype == ERender3DVertex::LitVertex)
    {
        S3DVertex*   s = v + beg;
        S3DLVertex*  d = (S3DLVertex*)vertbuf;
        for (int32_t c = 0; c < len; c++, s++, d++)
        {
            d->pos = s->pos;
            d->diffuse  = 0xFF808080;
            d->specular = 0x00000000;
            d->tu = s->tu;
            d->tv = s->tv;
        }
    }
    else if (verttype == ERender3DVertex::TLVertex)
    {
        S3DVertex*   s = v + beg;
        S3DTLVertex* d = (S3DTLVertex*)vertbuf;
        for (int32_t c = 0; c < len; c++, s++, d++)
        {
            d->sx = 100.0f + s->pos.X;
            d->sy = 100.0f + s->pos.Y;
            d->sz = 500.0f + s->pos.Z;
            d->rhw = 1.0f;
            d->diffuse  = 0xFFFFFFFF;
            d->specular = 0x00000000;
            d->tu = s->tu;
            d->tv = s->tv;
        }
    }
}

int32_t T3DImagery::NumObjVerts(int32_t objnum)
{
    if (!meshinitialized)
        InitializeMesh((S3DImageryBody*)GetBody());

    if (objnum < 0 || objnum > objects.NumItems() || !objects.Used(objnum))
        return 0;
    return objects[objnum].numverts;
}

void T3DImagery::GetObjVerts(int32_t objnum, void* vertbuf, int32_t state, int32_t frame,
    ERender3DVertex verttype)
{
    if (!meshinitialized)
        InitializeMesh((S3DImageryBody*)GetBody());

    GetVerts(vertbuf, state, frame, verttype,
        objects[objnum].startvert, objects[objnum].numverts);
}

int32_t T3DImagery::NumFaces()
{
    if (!meshinitialized)
        InitializeMesh((S3DImageryBody*)GetBody());
    return numfaces;
}

void T3DImagery::GetFaces(S3DFace* facesbuf)
{
    if (!meshinitialized)
        InitializeMesh((S3DImageryBody*)GetBody());
    std::memcpy(facesbuf, faces, sizeof(S3DFace) * numfaces);
}

int32_t T3DImagery::NumObjFaces(int32_t objnum)
{
    if (!meshinitialized)
        InitializeMesh((S3DImageryBody*)GetBody());
    if (objnum < 0 || objnum > objects.NumItems() || !objects.Used(objnum))
        return 0;
    return objects[objnum].numfaces;
}

void T3DImagery::GetObjFaces(int32_t objnum, S3DFace* facesbuf,
    int32_t* texfaces, int32_t* numtexfaces)
{
    if (!meshinitialized)
        InitializeMesh((S3DImageryBody*)GetBody());
    if (objnum < 0 || objnum > objects.NumItems() || !objects.Used(objnum))
        return;

    const int32_t ntex = textures.NumItems() < MAXTEXTURES ? textures.NumItems() : MAXTEXTURES;
    const int32_t slots = ntex + 1; // slot 0 = untextured faces
    int32_t packed_face = 0;

    for (int32_t t = 0; t < slots; ++t)
    {
        const int32_t count = objects[objnum].numtexfaces[t];
        if (texfaces) texfaces[t] = packed_face;
        if (numtexfaces) numtexfaces[t] = count;

        if (facesbuf && count > 0)
        {
            std::memcpy(facesbuf + packed_face,
                faces + objects[objnum].startface + objects[objnum].texfaces[t],
                sizeof(S3DFace) * count);
        }

        packed_face += count;
    }
}

// ********************
// * Object functions *
// ********************

int32_t T3DImagery::NumObjects()
{
    if (!meshinitialized)
        InitializeMesh((S3DImageryBody*)GetBody());
    return objects.NumItems();
}

int32_t T3DImagery::AddObject(S3DObj* obj)
{
    if (!meshinitialized)
        InitializeMesh((S3DImageryBody*)GetBody());
    return objects.Add(*obj);
}

void T3DImagery::RemoveObject(int32_t objnum)
{
    if (!meshinitialized)
        InitializeMesh((S3DImageryBody*)GetBody());
    if (objnum < 0 || objnum > objects.NumItems() || !objects.Used(objnum))
        return;

    S3DObj* obj = &(objects[objnum]);

    delete obj->numtexfaces;
    delete obj->texfaces;
    delete obj->parent;

    for (int32_t c = 0; c < NumStates(); c++)
    {
        if (obj->anikeys[c])
            delete obj->anikeys[c];
    }
    delete obj->anikeys;
    delete obj->numanikeys;

    objects.Remove(objnum);
}

void T3DImagery::ClearObjects()
{
    for (int32_t c = 0; c < objects.NumItems(); c++)
        RemoveObject(c);
    objects.Clear();
}

int32_t T3DImagery::GetObjectNum(char* objname)
{
    if (!meshinitialized)
        InitializeMesh((S3DImageryBody*)GetBody());

    for (int32_t c = 0; c < objects.NumItems(); c++)
    {
        if (objects.Used(c) && !stricmp(objects[c].name, objname))
            return c;
    }
    return -1;
}

char* T3DImagery::GetObjectName(int32_t objnum)
{
    if (!meshinitialized)
        InitializeMesh((S3DImageryBody*)GetBody());

    if (objnum < 0 || objnum > objects.NumItems() || !objects.Used(objnum))
        return nullptr;
    return objects[objnum].name;
}

void T3DImagery::GetObject(int32_t objnum, S3DObj* objbuf)
{
    if (!meshinitialized)
        InitializeMesh((S3DImageryBody*)GetBody());

    if (objnum < 0 || objnum > objects.NumItems() || !objects.Used(objnum))
        return;
    std::memcpy(objbuf, &objects[objnum], sizeof(S3DObj));
}

static void MakeMatrix(hmm_mat4& m, hmm_vec3& pos, hmm_vec3& rot, hmm_vec3& scl)
{
    if (rot.X != 0.0f) MtxRotateX(&m, rot.X);
    if (rot.Y != 0.0f) MtxRotateY(&m, rot.Y);
    if (rot.Z != 0.0f) MtxRotateZ(&m, rot.Z);
    if (scl.X != 1.0f || scl.Y != 1.0f || scl.Z != 1.0f)
        MtxScale(&m, &scl);
    // Retail CalcObjectMatrix 40a55f..40a576 scales the rotation basis,
    // then stores key position unchanged. Translating first scales that
    // position too (visible in Ribbon's non-unit-scale floor glow).
    if (pos.X != 0.0f || pos.Y != 0.0f || pos.Z != 0.0f)
        MtxTranslate(&m, &pos);
}

bool T3DImagery::IsHidden(int32_t objnum, int32_t state)
{
    if (!meshinitialized)
        InitializeMesh((S3DImageryBody*)GetBody());

    if (objnum < 0 || objnum > objects.NumItems() || !objects.Used(objnum))
        return true;
    if (objects[objnum].anikeys[state] == nullptr)
        return true;
    return false;
}

// ---- animation-key traversal ----
// Portable, pure-math. Unchanged from the Phase-1 code save for pointer
// typedef cleanup.

#define POSCHANGED 1
#define ROTCHANGED 2
#define SCLCHANGED 4

static inline int32_t SkipAniKey32(int32_t key, SAniKey32* keys, int32_t numkeys,
    int32_t& frames, int32_t& flags,
    int32_t& posidx, int32_t& rotidx, int32_t& sclidx)
{
    int32_t num = 0;
    SAniKey32* ak32 = &(keys[key]);
    frames = 1;
    flags = 0;

    while (key < numkeys)
    {
        int32_t flags32 = ak32->flags;
        switch (flags32)
        {
          case ANIFLAG32_CODE: {
            int32_t code = ak32->code;
            switch (code)
            {
              case ANICODE32_NEXTFRAME:
                if (!(flags & (POSCHANGED | ROTCHANGED | SCLCHANGED)))
                    frames = 0;
                key++; ak32++; num++;
                return num;
              case ANICODE32_SKIP:
                if (!(flags & (POSCHANGED | ROTCHANGED | SCLCHANGED)))
                    frames = 0;
                frames += ak32->value - 1;
                key++; ak32++; num++;
                return num;
              case ANICODE32_POSX:
              case ANICODE32_POSY:
              case ANICODE32_POSZ:
                if (flags & (POSCHANGED | ROTCHANGED | SCLCHANGED)) return num;
                if (code == ANICODE32_POSX) posidx = key;
                else if (code == ANICODE32_POSZ) flags |= POSCHANGED;
                break;
              case ANICODE32_ROTX:
              case ANICODE32_ROTY:
              case ANICODE32_ROTZ:
                if (flags & (ROTCHANGED | SCLCHANGED)) return num;
                if (code == ANICODE32_ROTX) rotidx = key;
                else if (code == ANICODE32_ROTZ) flags |= ROTCHANGED;
                break;
              case ANICODE32_SCLX:
              case ANICODE32_SCLY:
              case ANICODE32_SCLZ:
                if (flags & SCLCHANGED) return num;
                if (code == ANICODE32_SCLX) sclidx = key;
                else if (code == ANICODE32_SCLZ) flags |= SCLCHANGED;
                break;
            }
            break;
          }
          case ANIFLAG32_POS:
            if (flags & (POSCHANGED | ROTCHANGED | SCLCHANGED)) return num;
            posidx = key; flags |= POSCHANGED;
            break;
          case ANIFLAG32_ROT:
            if (flags & (ROTCHANGED | SCLCHANGED)) return num;
            rotidx = key; flags |= ROTCHANGED;
            break;
          case ANIFLAG32_SCL:
            if (flags & SCLCHANGED) return num;
            sclidx = key; flags |= SCLCHANGED;
            break;
        }
        key++; ak32++; num++;
    }
    return num;
}

static inline void GetAniKey32(int32_t /*key*/, SAniKey32* keys, int32_t numkeys,
    int32_t posidx, int32_t rotidx, int32_t sclidx,
    hmm_vec3& pos, hmm_vec3& rot, hmm_vec3& scl)
{
    if ((uint32_t)posidx < (uint32_t)numkeys)
    {
        SAniKey32* ak32 = &(keys[posidx]);
        if (ak32->flags == ANIFLAG32_CODE && ak32->code == ANICODE32_POSX)
        {
            pos.X = (float)ak32[0].value / (float)ANIKEY32_CODESCALE;
            pos.Y = (float)ak32[1].value / (float)ANIKEY32_CODESCALE;
            pos.Z = (float)ak32[2].value / (float)ANIKEY32_CODESCALE;
        }
        else
        {
            pos.X = (float)ak32->x / (float)ANIKEY32_POSSCALE;
            pos.Y = (float)ak32->y / (float)ANIKEY32_POSSCALE;
            pos.Z = (float)ak32->z / (float)ANIKEY32_POSSCALE;
        }
    }
    else pos.X = pos.Y = pos.Z = 0.0f;

    if ((uint32_t)rotidx < (uint32_t)numkeys)
    {
        SAniKey32* ak32 = &(keys[rotidx]);
        if (ak32->flags == ANIFLAG32_CODE && ak32->code == ANICODE32_ROTX)
        {
            rot.X = (float)ak32[0].value / (float)ANIKEY32_CODESCALE;
            rot.Y = (float)ak32[1].value / (float)ANIKEY32_CODESCALE;
            rot.Z = (float)ak32[2].value / (float)ANIKEY32_CODESCALE;
        }
        else
        {
            rot.X = (float)ak32->x / (float)ANIKEY32_ROTSCALE;
            rot.Y = (float)ak32->y / (float)ANIKEY32_ROTSCALE;
            rot.Z = (float)ak32->z / (float)ANIKEY32_ROTSCALE;
        }
    }
    else rot.X = rot.Y = rot.Z = 0.0f;

    if ((uint32_t)sclidx < (uint32_t)numkeys)
    {
        SAniKey32* ak32 = &(keys[sclidx]);
        if (ak32->flags == ANIFLAG32_CODE && ak32->code == ANICODE32_SCLX)
        {
            scl.X = (float)ak32[0].value / (float)ANIKEY32_CODESCALE;
            scl.Y = (float)ak32[1].value / (float)ANIKEY32_CODESCALE;
            scl.Z = (float)ak32[2].value / (float)ANIKEY32_CODESCALE;
        }
        else
        {
            scl.X = (float)ak32->x / (float)ANIKEY32_SCLSCALE;
            scl.Y = (float)ak32->y / (float)ANIKEY32_SCLSCALE;
            scl.Z = (float)ak32->z / (float)ANIKEY32_SCLSCALE;
        }
    }
    else scl.X = scl.Y = scl.Z = 1.0f;
}

bool T3DImagery::GetUninterpolatedAniKey(int32_t objnum, int32_t state, int32_t frame,
    hmm_vec3& pos, hmm_vec3& rot, hmm_vec3& scl)
{
    S3DObj* obj = &objects[objnum];

    if (!meshinitialized)
        InitializeMesh((S3DImageryBody*)GetBody());

    if ((uint32_t)state > (uint32_t)NumStates() ||
        (uint32_t)frame >= (uint32_t)GetAniLength(state))
        return false;

    if (obj->anikeys[state])
    {
        if (!(flags & I3D_ANIKEY32))
        {
            SAniKey& ak = ((SAniKey*)(obj->anikeys[state]))[frame];
            rot.X = (float)ak_rx(ak) / (float)ANIKEY_ANGSCALE;
            rot.Y = (float)ak_ry(ak) / (float)ANIKEY_ANGSCALE;
            rot.Z = (float)ak_rz(ak) / (float)ANIKEY_ANGSCALE;
            pos.X = (float)ak_x(ak)  / (float)ANIKEY_POSSCALE;
            pos.Y = (float)ak_y(ak)  / (float)ANIKEY_POSSCALE;
            pos.Z = (float)ak_z(ak)  / (float)ANIKEY_POSSCALE;
            scl.X = scl.Y = scl.Z = 1.0f;
        }
        else
        {
            int32_t curframe = 0;
            int32_t curkey = 0;
            int32_t numkeys = obj->numanikeys[state];
            SAniKey32* keys = &(((SAniKey32*)obj->anikeys[state])[curkey]);
            int32_t posidx = -1, rotidx = -1, sclidx = -1;

            for (;;)
            {
                int32_t num, frames, aflags;
                if (curkey < numkeys)
                    num = SkipAniKey32(curkey, keys, numkeys, frames, aflags,
                        posidx, rotidx, sclidx);
                else
                    num = 0;

                // `frames` is a duration, not an inclusive end index. A key
                // run starting at curframe covers [curframe, curframe+frames).
                // Using >= here makes every one-frame key last two integer
                // frames: frame 1 still samples key 0, the displayed final
                // frame samples the second-to-last key, and loop playback
                // visibly goes second-last -> first -> held first.
                // Actual retail4096bc JGE is inclusive. Opt in only the
                // exact audited Punch, Appear, Sfist and Wborn keys; others keep their policy.
                const bool reached = (retail_punch_keys || retail_mpappear_start_profile || retail_shadowfist_profile || retail_warriorborn_profile || retail_teleportation_profile || might_partsys_profile || immortalmight_partsys_profile || fmastery_partsys_profile || speed_partsys_profile || quicksilver_partsys_profile) ? frame <= curframe + frames : frame < curframe + frames;
                if ((num <= 0) || reached)
                {
                    GetAniKey32(curkey, keys, numkeys,
                        posidx, rotidx, sclidx, pos, rot, scl);
                    break;
                }
                curkey += num;
                curframe += frames;
            }
        }
    }
    else
    {
        rot.X = rot.Y = rot.Z = pos.X = pos.Y = pos.Z = 0.0f;
    }

    return true;
}

constexpr int32_t kLegacyTransitionBlendFrames = 5;

static void InterpolatePoints(hmm_vec3& v1, hmm_vec3& v2, float& i)
{
    if (v1.X != v2.X || v1.Y != v2.Y || v1.Z != v2.Z)
    {
        v1.X = v1.X * i + v2.X * (1.0f - i);
        v1.Y = v1.Y * i + v2.Y * (1.0f - i);
        v1.Z = v1.Z * i + v2.Z * (1.0f - i);
    }
}

static void NormalizeRot(hmm_vec3& v1, hmm_vec3& v2)
{
    const float PI  = (float)M_PI;
    const float TPI = (float)(M_PI * 2.0);
    if (v2.X - v1.X >  PI) v2.X -= TPI; else if (v2.X - v1.X < -PI) v2.X += TPI;
    if (v2.Y - v1.Y >  PI) v2.Y -= TPI; else if (v2.Y - v1.Y < -PI) v2.Y += TPI;
    if (v2.Z - v1.Z >  PI) v2.Z -= TPI; else if (v2.Z - v1.Z < -PI) v2.Z += TPI;

    if (fabsf(v2.X - v1.X) > PI * 0.5f ||
        fabsf(v2.Y - v1.Y) > PI * 0.5f ||
        fabsf(v2.Z - v1.Z) > PI * 0.5f)
        v2 = v1;
}

bool T3DImagery::GetAniKey(int32_t objnum, int32_t state, int32_t frame,
    hmm_vec3& pos, hmm_vec3& rot, hmm_vec3& scl)
{
    if (!meshinitialized)
        InitializeMesh((S3DImageryBody*)GetBody());

    if (!GetUninterpolatedAniKey(objnum, state, frame, pos, rot, scl))
        return false;

    if (!Interpolate) return true;
    if (GetHeader()->states[state].aniflags & AF_NOINTERPOLATION) return true;
    if ((uint32_t)prevstate > (uint32_t)GetHeader()->numstates) return true;
    if (GetHeader()->states[prevstate].aniflags & AF_NOINTERPOLATION) return true;

    char* begstate = FindTag((char*)"beg", state);
    char* endstate = FindTag((char*)"end", prevstate);

    if (begstate && endstate &&
        prevframe >= (GetHeader()->states[prevstate].frames - 1) &&
        !stricmp(endstate, begstate))
        return true;

    int32_t iframes = kLegacyTransitionBlendFrames;

    float i = 1.0f;
    hmm_vec3 ipos, irot, iscl;
    if (prevstate >= 0 && frame < iframes)
    {
        i = (float)(frame + 1) / (float)iframes;
        GetUninterpolatedAniKey(objnum, prevstate, prevframe, ipos, irot, iscl);
        NormalizeRot(rot, irot);
        InterpolatePoints(pos, ipos, i);
        InterpolatePoints(rot, irot, i);
        InterpolatePoints(scl, iscl, i);
    }

    return true;
}

bool T3DImagery::CalcObjectMatrix(S3DAnimObj* animobj, int32_t state, int32_t frame,
    hmm_mat4* pos, bool calcparents)
{
    hmm_mat4& m = animobj->matrix;
    MtxClear(&m);

    bool hastrans = (animobj->flags & (OBJ3D_ROTMASK | OBJ3D_POSMASK | OBJ3D_SCLMASK)) != 0;

    if (!(flags & I3D_ISMORPH) && (!hastrans || (animobj->flags & OBJ3D_ADDTOANI)))
    {
        int32_t animtrack = (animobj->flags & OBJ3D_ANIMTRACK)
                            ? animobj->animtrack : animobj->objnum;
        if (GetAniKey(animtrack, state, frame, animobj->pos, animobj->rot, animobj->scl))
        {
            MakeMatrix(m, animobj->pos, animobj->rot, animobj->scl);
            // Mirror the per-bone local TRS into the bone's TTransform.
            // 3DSMax exports standard SRT, so feeding the same values
            // into TTransform's SetLocal*/Rot/Scl produces an
            // equivalent local matrix to MakeMatrix's output when the
            // common scl == 1 case holds. Combined with the parent
            // linkage wired at SetupObjects (in-tree bones to parent
            // bone, root bones to inst->Transform()), this means
            // bone.transform.Matrix() resolves to a world-space
            // matrix that future consumers can use directly. Today
            // nobody reads from it -- the legacy `bone->matrix` +
            // BuildRootMatrixSource path is still the renderer's
            // input -- but the data is now in lockstep, ready to
            // switch consumers to in C4.
            animobj->transform.SetLocalPos(animobj->pos);
            animobj->transform.SetLocalRotEuler(animobj->rot);
            animobj->transform.SetLocalScl(animobj->scl);
        }
    }

    if (hastrans)
    {
        // First transform
        if      ((animobj->flags & OBJ3D_POSMASK) == OBJ3D_POS1) MtxTranslate(&m, &animobj->pos);
        else if ((animobj->flags & OBJ3D_ROTMASK) == OBJ3D_ROT1) {
            MtxRotateX(&m, animobj->rot.X);
            MtxRotateY(&m, animobj->rot.Y);
            MtxRotateZ(&m, animobj->rot.Z);
        }
        else if ((animobj->flags & OBJ3D_SCLMASK) == OBJ3D_SCL1) MtxScale(&m, &animobj->scl);

        // Second transform
        if      ((animobj->flags & OBJ3D_POSMASK) == OBJ3D_POS2) MtxTranslate(&m, &animobj->pos);
        else if ((animobj->flags & OBJ3D_ROTMASK) == OBJ3D_ROT2) {
            MtxRotateX(&m, animobj->rot.X);
            MtxRotateY(&m, animobj->rot.Y);
            MtxRotateZ(&m, animobj->rot.Z);
        }
        else if ((animobj->flags & OBJ3D_SCLMASK) == OBJ3D_SCL2) MtxScale(&m, &animobj->scl);

        // Third transform
        if      ((animobj->flags & OBJ3D_POSMASK) == OBJ3D_POS3) MtxTranslate(&m, &animobj->pos);
        else if ((animobj->flags & OBJ3D_ROTMASK) == OBJ3D_ROT3) {
            MtxRotateX(&m, animobj->rot.X);
            MtxRotateY(&m, animobj->rot.Y);
            MtxRotateZ(&m, animobj->rot.Z);
        }
        else if ((animobj->flags & OBJ3D_SCLMASK) == OBJ3D_SCL3) MtxScale(&m, &animobj->scl);
    }

    if (animobj->parent)
    {
        if (calcparents)
            CalcObjectMatrix(animobj->parent, state, frame, pos, calcparents);
        MtxMultiply(&m, &m, &animobj->parent->matrix);
    }

    return true;
}

bool T3DImagery::CalcObjectMatrixCopy(S3DAnimObj* animobj, int32_t state, int32_t frame,
    hmm_mat4* pos, bool calcparents)
{
    hmm_mat4 m;
    std::memcpy(&m, &animobj->matrix, sizeof(hmm_mat4));

    if (animobj->parent && calcparents)
    {
        CalcObjectMatrixCopy(animobj->parent, state, frame, pos, calcparents);
        MtxMultiply(&m, &m, &animobj->parent->matrix);
    }

    std::memcpy(pos, &m, sizeof(hmm_mat4));
    return true;
}

// Renders an object individually. Most of the body is now T3DScene calls
// (which are Phase-2 stubs) — what remains is the matrix concat, material /
// texture selection, and per-texture face dispatch.
bool T3DImagery::RenderObject(S3DAnimObj* animobj, int32_t state, int32_t frame,
    hmm_mat4* pos, int32_t tex, bool uselastmatrix)
{
    int32_t t;
    int32_t objnum = animobj->objnum;

    if (objnum < 0 || objnum > objects.NumItems() || !objects.Used(objnum))
        return false;
    if (state >= NumStates())
        return false;

    if (hastextures)
    {
        if ((!UseTextures && textures.NumItems() > 0) ||
            (UseTextures && textures.NumItems() <= 0))
            ClearMesh();
    }

    if (!meshinitialized)
        if (!InitializeMesh((S3DImageryBody*)GetBody()))
            return false;

    S3DObj* obj = &objects[objnum];

    // ---- position ----
    if (!(animobj->flags & OBJ3D_MATRIX) && !uselastmatrix)
        CalcObjectMatrix(animobj, state, frame, pos, false);

    if (animobj->flags & OBJ3D_HIDE)
        return true;

    if (!(flags & I3D_ISMORPH) ||
        (animobj->flags & (OBJ3D_POSMASK | OBJ3D_ROTMASK | OBJ3D_SCLMASK | OBJ3D_MATRIX)))
    {
        if (animobj->flags & OBJ3D_ABSPOS)
        {
            Scene3D.SetTransform(ERender3DTransform::World, &animobj->matrix);
        }
        else
        {
            hmm_mat4 world;
            if (Double3D || Triple3D)
            {
                hmm_mat4 temp;
                std::memcpy(&temp, &animobj->matrix, sizeof(hmm_mat4));
                hmm_vec3 s = {Triple3D ? 3.0f : 2.0f, Triple3D ? 3.0f : 2.0f, Triple3D ? 3.0f : 2.0f};
                MtxScale(&temp, &s);
                MtxMultiply(&world, &temp, pos);
            }
            else
            {
                MtxMultiply(&world, &animobj->matrix, pos);
            }
            Scene3D.SetTransform(ERender3DTransform::World, &world);
        }
    }
    else
    {
        Scene3D.SetTransform(ERender3DTransform::World, pos);
    }

    // ---- material ----
    TMaterialHandle hmaterial = (animobj->flags & OBJ3D_MAT)
        ? animobj->hmaterial
        : materials[obj->material].hmaterial;
    uint32_t holdmat = 0;
    Scene3D.GetLightState(ERender3DLightState::Material, &holdmat);
    if (holdmat != hmaterial)
        Scene3D.SetLightState(ERender3DLightState::Material, hmaterial);

    // ---- per-texture face dispatch ----
    for (t = 0; t < textures.NumItems() + 1; t++)
    {
        if (tex >= 0 && tex != t)
            continue;

        int32_t numvrt;
        ERender3DVertex vrttype;
        void* vrt;
        int32_t numfac;
        S3DFace* fac;

        if (animobj->flags & OBJ3D_VERTS)
        {
            numvrt = animobj->numverts;
            vrttype = animobj->verttype;
            vrt = animobj->verts;
        }
        else
        {
            numvrt = obj->numverts;
            vrttype = ERender3DVertex::Vertex;
            if (flags & I3D_ISMORPH)
                vrt = &verts[state][frame] + obj->startvert;
            else
                vrt = verts[0][0] + obj->startvert;
        }

        if (animobj->flags & OBJ3D_FACES)
        {
            numfac = animobj->numtexfaces[t];
            fac = animobj->faces + animobj->texfaces[t];
        }
        else
        {
            numfac = obj->numtexfaces[t];
            fac = &faces[obj->startface + obj->texfaces[t]];
        }

        if (numvrt <= 0 || numfac <= 0)
            continue;

        if (hastextures && UseTextures &&
            t > 0 && !(animobj->flags & OBJ3D_TEX))
        {
            int32_t texframe = (animobj->flags & OBJ3D_TEXFRAME)
                ? animobj->textureframe[t]
                : frame;
            SetTextureFrame(t - 1, texframe);
        }

        TTextureHandle htexture = kInvalidTexture;
        if (!hastextures || !UseTextures) { /* leave invalid */ }
        else if (animobj->flags & OBJ3D_TEX)
        {
            htexture   = animobj->htextures[t];
        }
        else if (t > 0)
        {
            htexture   = textures[t - 1].htexture;
        }

        uint32_t holdtex = 0;
        Scene3D.GetRenderState(ERender3DState::TexAddress, &holdtex);
        if (materials[obj->material].matdesc.hTexture != htexture)
        {
            materials[obj->material].matdesc.hTexture = htexture;
#if 0 // TODO(port): push updated material uniforms — Phase 3
            // Original: materials[].material->SetMaterial(&matdesc);
#endif
        }
        if (holdtex != htexture)
            Scene3D.SetTexture(htexture);

        Scene3D.DrawIndexedPrimitive(
            ERender3DPrim::TriangleList,
            vrttype,
            vrt,
            numvrt,
            (uint16_t*)fac,
            numfac * 3,
            0);
    }

    return true;
}

void T3DImagery::BeginRender(bool /*clearbuf*/)
{
    // The legacy execute-buffer cache is retired. Just forward to the scene.
    Scene3D.BeginScene();
}

void T3DImagery::EndRender()
{
    Scene3D.EndScene();
}

namespace {

// REVSYNC: 0x0040af50 -- the sample an object's tag sound plays. Retail
// upper-cases the name and then:
//   * a name with "STEP" in it, on a character or the player, gets the
//     terrain under it appended ("wood", "stone", "carpet", "grass", else
//     "dirt": walkmap bits 10-15 at its position, 0x00452ea0) and plays at
//     volume 0x5c (0x40 while sneaking);
//   * a name starting "IMP" and not ending in a digit gets the struck
//     character's weapon kind appended ("sword", "bigsword" one time in
//     five for two-handers, "staff", "bow", "hand");
//   * a name starting "BLOCK" plays nothing;
//   * any other name plays as it is.
// REVSYNC-DIVERGENCE: only the BLOCK rule is ported. The port's walkmap
// keeps heights alone (no terrain bits), so STEP names play as they are:
// the plain step samples are the dirt recordings (step1r.wav ==
// step1rdirt.wav), what retail plays on terrain 0. The IMP weapon suffix
// waits on the combat port's weapon kinds.
bool ResolveTagSound(const char* name)
{
    return strnicmp(name, "BLOCK", 5) != 0;
}

} // namespace

// REVSYNC: the "blendcont" tag. T3DAnimator::RefreshControllers (0x0040df90)
// builds a controller for every tag of the state the animator enters (and
// every state -1 tag on its first refresh) whose name, compared without
// case, is a registered controller; "play", "beg" and "end" never are.
// blendcont (builder 0x00405750) parses its string as "item[=value],..."
// (0x0040d750): a blend name sets the mode (0x00405770, below), "obj" names
// objects (base 0x0040d4a0), anything else fails the tag. Its Initialize
// (0x00405940) then flags every object of the animator to draw with the
// mode -- but only when the tag named no objects: a tag with an object
// list draws nothing differently. The controller has no Pulse or Render;
// the mode is all it does.
// The shipped imagery has 56 blendcont tags in 42 files (litadd 36,
// litaddz 12, litalpha 8), none with an object list or a "filename" item
// (retail's items read from a file, not ported).
// REVSYNC-DIVERGENCE: retail's mode stays on the objects after the animator
// leaves the tagged state; the port answers per state (a state tag over a
// state -1 one). Single-state effects (gvortex, appear) can't tell the
// difference.
void T3DImagery::ResolveStateBlends()
{
    static constexpr struct { const char* name; uint32_t mode; } kModes[] = {
        { "none",      0 },
        { "normal",    BLEND3D_NORMAL },
        { "alpha",     BLEND3D_ALPHA },
        { "litalpha",  BLEND3D_LITALPHA },
        { "add",       BLEND3D_ADD },
        { "litadd",    BLEND3D_LITADD },
        { "nocheckz",  BLEND3D_NORMAL   | BLEND3D_NOZCHECK },
        { "litalphaz", BLEND3D_LITALPHA | BLEND3D_NOZCHECK },
        { "litaddz",   BLEND3D_LITADD   | BLEND3D_NOZCHECK },
        { "alphaadd",  BLEND3D_ALPHAADD },
    };

    const int32_t numstates = NumStates();
    stateblend.assign(size_t(numstates > 0 ? numstates : 0), 0);
    std::vector<bool> fromstatetag(stateblend.size(), false);

    for (int32_t c = 0; c < tags.NumItems(); c++)
    {
        const S3DTag& tag = tags[c];
        if (stricmp(tag.name, "blendcont") != 0)
            continue;

        // Items are split at top-level commas: "obj=(a,b)" is one item.
        uint32_t mode = 0;
        bool hasmode = false, hasobjects = false, failed = false;
        const char* p = tag.str;
        while (*p && !failed)
        {
            while (*p == ' ' || *p == '\t')
                ++p;
            const char* start = p;
            int32_t depth = 0;
            while (*p && (depth > 0 || *p != ','))
            {
                if (*p == '(') ++depth;
                else if (*p == ')') --depth;
                ++p;
            }
            std::string item(start, size_t(p - start));
            if (*p == ',')
                ++p;
            const size_t eq = item.find('=');
            std::string key = item.substr(0, eq);
            while (!key.empty() && (key.back() == ' ' || key.back() == '\t'))
                key.pop_back();
            if (key.empty())
                continue;

            if (stricmp(key.c_str(), "obj") == 0)
            {
                hasobjects = true;
                continue;
            }
            bool known = false;
            for (const auto& m : kModes)
            {
                if (stricmp(key.c_str(), m.name) == 0)
                {
                    mode = m.mode;
                    hasmode = known = true;
                    break;
                }
            }
            failed = !known;
        }
        if (failed)
        {
            log_warn("[i3d] %s: blendcont '%s' (state %d) has an item retail rejects; tag dropped",
                     GetResFilename(), tag.str, tag.state);
            continue;
        }
        if (!hasmode || hasobjects)
            continue;

        if (tag.state < 0)
        {
            for (size_t s = 0; s < stateblend.size(); s++)
                if (!fromstatetag[s])
                    stateblend[s] = mode;
        }
        else if (size_t(tag.state) < stateblend.size())
        {
            stateblend[size_t(tag.state)] = mode;
            fromstatetag[size_t(tag.state)] = true;
        }
    }
}

// REVSYNC: 0x0040b2e0 -- each tick (T3DAnimator::Pulse) every "play" tag of
// the state whose frame is the animator's frame + 1 plays one name picked
// at random from its list. The animator still holds the frame it drew last;
// the object has already stepped to the next one (TMapPane::Pulse runs
// NextFrameObjects before PulseObjects), so a tag fires on the tick its
// frame comes up. Without an object the sample plays flat, at full volume.
void T3DImagery::PlaySound(TObjectInstance* inst, int32_t state, int32_t frame)
{
    for (int32_t c = 0; c < tags.NumItems(); c++)
    {
        const S3DTag& tag = tags[c];
        if (tag.state != state || tag.frame != frame + 1 || strcmp(tag.name, "play") != 0)
            continue;

        char* sound = listrnd(tag.str);
        if (!ResolveTagSound(sound))
            continue;
        if (!inst)
            PLAY(sound);
        else
            inst->PlayWave(sound);
    }
}

// **********************
// * Material Functions *
// **********************

int32_t T3DImagery::AddMaterial(S3DMaterial* newmat, int32_t tex)
{
    S3DMat mat;
    std::memset(&mat, 0, sizeof(S3DMat));
    std::memcpy(&mat.matdesc, newmat, sizeof(S3DMaterial));
    mat.hmaterial = kInvalidMaterial;   // Phase 3 will issue real handles

#if 0 // TODO(port): issue a sokol-backed material handle here — Phase 3
    // Original: Direct3D->CreateMaterial, SetMaterial, GetHandle.
#endif

    if (UseTextures && tex >= 0 && tex < textures.NumItems() &&
        textures.Used(tex) && textures[tex].htexture != kInvalidTexture)
    {
        mat.matdesc.hTexture = textures[tex].htexture;
        mat.texture = tex;
    }
    else
    {
        mat.matdesc.hTexture = 0;
        mat.texture = -1;
    }

    return materials.Add(mat);
}

void T3DImagery::GetMaterial(int32_t matnum, S3DMat* mat)
{
    if (matnum < 0 || matnum >= materials.NumItems() || !materials.Used(matnum))
        return;
    std::memcpy(mat, &materials[matnum], sizeof(S3DMat));
}

TMaterialHandle T3DImagery::GetMaterialHandle(int32_t matnum)
{
    if (matnum < 0 || matnum >= materials.NumItems() || !materials.Used(matnum))
        return kInvalidMaterial;
    return materials[matnum].hmaterial;
}

void T3DImagery::SetMaterial(int32_t matnum, S3DMat* mat)
{
    if (matnum < 0 || matnum >= materials.NumItems() || !materials.Used(matnum))
        return;
    std::memcpy(&materials[matnum].matdesc, &mat->matdesc, sizeof(S3DMaterial));
    if (mat->texture < 0 || mat->texture >= textures.NumItems() || !textures.Used(mat->texture))
    {
        materials[matnum].matdesc.hTexture = 0;
        materials[matnum].texture = -1;
    }
    else
    {
        materials[matnum].matdesc.hTexture = textures[mat->texture].htexture;
        materials[matnum].texture = mat->texture;
    }
#if 0 // TODO(port): push updated material uniforms — Phase 3
#endif
}

void T3DImagery::RemoveMaterial(int32_t matnum)
{
    if (!materials.Used(matnum))
        return;
    materials[matnum].hmaterial = kInvalidMaterial;
    materials.Remove(matnum);
}

void T3DImagery::ClearMaterials()
{
    for (int32_t c = 0; c < materials.NumItems(); c++)
        RemoveMaterial(c);
    materials.Clear();
}

// *********************
// * Texture Functions *
// *********************

int32_t T3DImagery::AddTexture(SSurfaceDesc* srcsd,
    OFFSET* pixels, int32_t frames, void* palette)
{
    S3DTex tex;
    std::memset(&tex, 0, sizeof(S3DTex));
    const int32_t texture_index = textures.NumItems();
    if (!LoadTexture(&tex, srcsd, pixels, frames, palette, false, texture_index))
        return -1;
    return textures.Add(tex);
}

bool T3DImagery::LoadTexture(S3DTex* tex, SSurfaceDesc* srcsd,
    OFFSET* pixels, int32_t frames, void* palette, bool copyframes,
    int32_t texture_index)
{
    // Retail D3D3 required square, power-of-two textures in 8..512. Modern
    // GPUs (sokol backends) don't care; log a debug note instead of fatal-ing.
    if (srcsd->width != srcsd->height)
        log_info("[i3d] %s has non-square texture (%dx%d) -- accepting",
                 GetResFilename(), srcsd->width, srcsd->height);
    if (srcsd->width != 128 && srcsd->width != 64 && srcsd->width != 256 &&
        srcsd->width != 32  && srcsd->width != 512 && srcsd->width != 16 &&
        srcsd->width != 8)
        log_info("[i3d] %s texture is not a D3D3-era power of two (%d) -- accepting",
                 GetResFilename(), srcsd->width);

    SSurfaceDesc dstsd;
    Scene3D.GetClosestTextureFormat(srcsd, &dstsd);
    std::memcpy(&tex->desc, &dstsd, sizeof(SSurfaceDesc));

    if (frames <= 1) copyframes = false;
    tex->copyframes = copyframes;
    tex->numframes  = frames;
    tex->framenum   = 0;

    if (frames > 1)
    {
        tex->framehtexs = new TTextureHandle[frames];
        for (int32_t f = 0; f < frames; f++)
            tex->framehtexs[f] = kInvalidTexture;
    }
    else
    {
        tex->framehtexs = nullptr;
    }

    std::vector<uint8_t> rgba;
    for (int32_t f = 0; f < frames; ++f)
    {
        void* frame_pixels = nullptr;
        if (pixels)
            frame_pixels = pixels[f].ptr();
        if (!frame_pixels)
            continue;

        if (!DecodeTextureFrameRGBA(srcsd, frame_pixels, palette, rgba))
            continue;

        // Chroma-key + premultiply pass for I3D-era sprite atlases
        // whose transparent background is pure black (no alpha
        // channel in the source). Without this, bilinear sampling at
        // the splat-to-bg boundary produces dark-red transition
        // pixels (visible fringe).
        //
        // Heuristic: if >20% of the texture is pure black, treat it
        // as chroma-keyed. Set those pixels to fully transparent
        // (a=0, rgb=0 = premultiplied form) and leave opaque pixels
        // alone (a=255, rgb unchanged = already premultiplied since
        // a=1.0). Bucket consumers should pair this with the
        // PremulAlpha blend mode for correct edge blending.
        //
        // TODO: drive this from an explicit per-asset flag once the
        // tile-set loader gets one, instead of the heuristic. For
        // Phase 2 it covers Blood/Mist/Smoke/spell-disc style
        // atlases which all use the chroma-key convention.
        if (!rgba.empty())
        {
            const size_t px_count = rgba.size() / 4;
            size_t black_count = 0;
            for (size_t i = 0; i < px_count; ++i)
            {
                const uint8_t r = rgba[i * 4 + 0];
                const uint8_t g = rgba[i * 4 + 1];
                const uint8_t b = rgba[i * 4 + 2];
                if (r == 0 && g == 0 && b == 0)
                    ++black_count;
            }
            if (px_count > 0 && black_count * 5 > px_count) // >20%
            {
                for (size_t i = 0; i < px_count; ++i)
                {
                    uint8_t* px = &rgba[i * 4];
                    if (px[0] == 0 && px[1] == 0 && px[2] == 0)
                        px[3] = 0;   // fully transparent (rgb already 0 = premul'd)
                }
            }
        }

        // i3ddump tool retains the decoded RGBA bytes per frame so we can
        // write them out as PNG. Stored on T3DImagery (NOT S3DTex) because
        // S3DTex lives inside a TVirtualArray which copies via memcpy and
        // would corrupt any std::vector member.
        if (T3DImagery::g_retain_decoded_rgba && !rgba.empty())
        {
            if (int32_t(dump_textures.size()) <= texture_index)
                dump_textures.resize(texture_index + 1);
            if (int32_t(dump_textures[texture_index].size()) < frames)
                dump_textures[texture_index].resize(frames);
            dump_textures[texture_index][f] = rgba;
        }

        TTextureHandle htexture = kInvalidTexture;
        if (Renderer)
        {
            const uint64_t key = I3DTextureAssetKey(AssetId(), texture_index, f, HasScrollTex());
            htexture = Renderer->RegisterTextureAsset(key,
                                                       rgba.data(),
                                                       rgba.size(),
                                                       int32_t(srcsd->width),
                                                       int32_t(srcsd->height),
                                                       ERendererTextureFormat::RGBA8,
                                                       uint64_t(rgba.size()),
                                                       ERendererTextureFilter::Linear,
                                                       HasScrollTex());
            if (htexture != kInvalidTexture)
            {
                Renderer->AddTextureAssetRef(htexture);
                AddReferencedResource(
                    EAssetReferencedResourceKind::RendererTexture,
                    key,
                    htexture,
                    "i3d.texture",
                    [htexture]() {
                        if (Renderer)
                            Renderer->ReleaseTextureAssetRef(htexture);
                    });
            }
        }

        if (frames > 1)
            tex->framehtexs[f] = htexture;
        else
            tex->htexture = htexture;
    }

    if (frames > 1)
        tex->htexture = tex->framehtexs[0];

    return true;
}

void T3DImagery::RemoveTexture(int32_t texnum)
{
    S3DTex* t = &textures[texnum];
    t->htexture = kInvalidTexture;

    if (t->framehtexs)
    {
        delete[] t->framehtexs;
        t->framehtexs = nullptr;
    }
    t->numframes = 0;
    t->framenum = 0;

    textures.Remove(texnum);
}

int32_t T3DImagery::NumTextures()
{
    return textures.NumItems();
}

void T3DImagery::GetTexture(int32_t texnum, S3DTex* tex)
{
    if (texnum < 0 || texnum >= textures.NumItems() || !textures.Used(texnum))
        return;
    std::memcpy(tex, &textures[texnum], sizeof(S3DTex));
}

TTextureHandle T3DImagery::GetTextureHandle(int32_t texnum)
{
    if (texnum < 0 || texnum >= textures.NumItems() || !textures.Used(texnum))
        return kInvalidTexture;
    return textures[texnum].htexture;
}

void T3DImagery::ClearTextures()
{
    // T3DImagery is the source asset. It owns renderer texture refs; the
    // renderer owns the actual GPU objects.
    ReleaseReferencedResources(EAssetReferencedResourceKind::RendererTexture);
    for (int32_t c = 0; c < textures.NumItems(); c++)
        RemoveTexture(c);
    textures.Clear();
}

int32_t T3DImagery::GetTextureFrame(int32_t texnum)
{
    if (texnum < 0 || texnum >= textures.NumItems() || !textures.Used(texnum))
        return 0;
    return textures[texnum].framenum;
}

bool T3DImagery::SetTextureFrame(int32_t texnum, int32_t framenum)
{
    if (texnum < 0 || texnum >= textures.NumItems() || !textures.Used(texnum))
        return false;
    if (textures[texnum].numframes <= 1)
        return true;
    if (framenum >= textures[texnum].numframes)
        framenum = framenum % textures[texnum].numframes;
    if (textures[texnum].framenum == framenum)
        return true;

    if (!textures[texnum].copyframes)
    {
        textures[texnum].htexture = textures[texnum].framehtexs[framenum];
        textures[texnum].framenum = framenum;
        return true;
    }

#if 0 // TODO(port): copy frame pixels into a renderer-owned texture -- Phase 3
#endif
    textures[texnum].framenum = framenum;
    return true;
}

bool T3DImagery::SurfacesLost()
{
    // sokol doesn't lose surfaces the way DirectDraw did.
    return false;
}

void T3DImagery::RestoreSurfaces()
{
    if (!SurfacesLost())
        return;
    Restore();
}

// *****************************
// * General Purpose Functions *
// *****************************

char* T3DImagery::FindTag(const char* name, int32_t state, int32_t frame,
    int32_t* foundstate, int32_t* foundframe)
{
    for (int32_t c = 0; c < tags.NumItems(); c++)
    {
        S3DTag& tag = tags[c];

        if ((state >= 0 && tag.state > state) ||
           (frame >= 0 && tag.state == state && tag.frame > frame))
            break;

        if ((state >= 0 && tag.state != state) ||
            (frame >= 0 && tag.frame != frame) ||
            stricmp(tag.name, name) != 0)
            continue;

        if (foundstate) *foundstate = tag.state;
        if (foundframe) *foundframe = tag.frame;
        return tag.str;
    }
    return nullptr;
}

void T3DImagery::ResetExtents()
{
    // The old code cleared the D3D clip-status extents registers here; the
    // Phase-3 path will track extents on the CPU in T3DAnimator.
}

void T3DImagery::GetExtents(SRenderRect* extents)
{
    if (!extents) return;
#if 0 // TODO(port): real extents come from the per-draw vertex-post-xform
      // bounding box tracker — Phase 3.
#endif
    extents->x1 = extents->y1 = 0;
    extents->x2 = extents->y2 = 0;
}

void T3DImagery::AddUpdateRect(SRenderRect* extents, int32_t /*uflags*/)
{
    if (!extents) return;
    if (extents->x2 < 0 || extents->x1 >= WIDTH ||
        extents->y2 < 0 || extents->y1 >= HEIGHT ||
        extents->x2 - extents->x1 <= 0 ||
        extents->y2 - extents->y1 <= 0)
        return;

#if 0 // TODO(port): Display.AddUpdateRect lost its method in the TDisplay
      // refactor; add the rect through whatever the new dirty-rect API is — Phase 3.
    if (!NoUpdateRects)
        Display.AddUpdateRect(extents->x1, extents->y1,
            extents->x2 - extents->x1 + 1, extents->y2 - extents->y1 + 1, uflags);
#endif
}

void T3DImagery::UpdateBoundingRect(TObjectInstance* oi, int32_t state, SRenderRect* extents)
{
    if (!extents) return;
    if (extents->x2 <= 0 || extents->x1 <= 0 || extents->x1 >= WIDTH ||
        extents->y2 <= 0 || extents->y1 <= 0 || extents->y1 >= HEIGHT ||
        extents->x2 - extents->x1 <= 0 ||
        extents->y2 - extents->y1 <= 0)
        return;

    S3DPoint p;
    oi->GetScreenPos(p);
    SRect nr, r;
    p.x = p.x - MapPane.GetScrollX() + MapPane.GetPosX();
    p.y = p.y - MapPane.GetScrollY() + MapPane.GetPosY();
    r.left = -GetRegX(state);
    r.top  = -GetRegY(state);
    r.right  = r.left + GetWidth(state)  - 1;
    r.bottom = r.top  + GetHeight(state) - 1;
    nr.left   = extents->x1 - p.x;
    nr.top    = extents->y1 - p.y;
    nr.right  = extents->x2 - p.x;
    nr.bottom = extents->y2 - p.y;
    if (nr.left   < r.left)   r.left   = nr.left;
    if (nr.top    < r.top)    r.top    = nr.top;
    if (nr.right  > r.right)  r.right  = nr.right;
    if (nr.bottom > r.bottom) r.bottom = nr.bottom;
    SetReg(state, -r.left, -r.top, 0);
    SetWidthHeight(state, r.right - r.left + 1, r.bottom - r.top + 1);
}

void T3DImagery::RefreshZBuffer(TObjectInstance* oi)
{
    SRect r;
    GetScreenRect(oi, r);
    Scene3D.RestoreZBuffer(r);
}

bool T3DImagery::GetMotion(int32_t state, int32_t frame,
    int32_t& dist, int32_t& vert, int32_t& ang,
    int32_t& rotx, int32_t& roty, int32_t& rotz)
{
    if ((uint32_t)state >= (uint32_t)GetHeader()->numstates ||
        (uint32_t)frame >= (uint32_t)GetHeader()->states[state].frames ||
        !motion[state])
    {
        dist = vert = ang = rotx = roty = rotz = 0;
        return false;
    }

    SMotionData* md = &(motion[state][frame]);

    dist = md_dist(*md);
    vert = md_vert(*md);
    ang  = md->ang;
    rotx = md->rotx;
    roty = md->roty;
    rotz = md->rotz;
    return true;
}

void T3DImagery::SetObjectMotion(TObjectInstance* inst)
{
    int32_t state = inst->GetState();
    int32_t frame = inst->GetFrame();
    int32_t oldstate = inst->GetPrevState();

    if (MeshInitialized() &&
        !(GetAniFlags(state) & AF_NOMOTION) &&
        !inst->CommandDone())
    {
        if ((state != oldstate) && (GetAniFlags(state) & AF_ROOT))
            inst->ClearAccum();

        int32_t dist, vert, ang, rotx, roty, rotz;
        if (GetMotion(state, frame, dist, vert, ang, rotx, roty, rotz))
        {
            ang  = (ang  + inst->GetFace())     & 255;
            rotx = (rotx + inst->GetRotateX()) & 255;
            roty = (roty + inst->GetRotateY()) & 255;
            rotz = (rotz + inst->GetRotateZ()) & 255;
            inst->SetMoveDist(dist);
            inst->SetMoveVert(vert);
            inst->SetMoveAngle(ang);
            inst->SetRotateX(rotx);
            inst->SetRotateY(roty);
            inst->SetRotateZ(rotz);
        }
    }
    else
        inst->Halt();
}

// ****************** NORMAL IMAGERY STUFF **********************

// REVSYNC: 0x0040ce60. An object has one icon whatever its state: once the
// body has loaded, retail keeps a copy of state 0's -- its invitem, else the
// first frame of its invanim (TObjectImagery::LoadBody 0x00447ac0, imagery
// entry +0x74) -- and answers with that. Without one, the state's own
// invitem, else state 0's (null).
TBitmap* T3DImagery::GetInvImage(int32_t state, int32_t /*num*/)
{
    if (!meshinitialized)
        if (!InitializeMesh((S3DImageryBody*)GetBody()))
            FatalError("Unable to initialize 3D imagery");

    if (!icons)
        return nullptr;
    if (TBitmap* icon = icons[0].invitem)
        return icon;
    if (TBitmap* frame = icons[0].invanim ? icons[0].invanim->GetFrame(0) : nullptr)
        return frame;
    if (state < 0 || state >= NumStates())
        return nullptr;
    return icons[state].invitem;
}

// REVSYNC: 0x0040cef0 -- the state's own invanim; none for a state out of range.
TAnimation* T3DImagery::GetInvAnimation(int32_t state)
{
    if (!meshinitialized)
        InitializeMesh((S3DImageryBody*)GetBody());

    if (!icons || state < 0 || state >= NumStates())
        return nullptr;
    return icons[state].invanim;
}

void T3DImagery::AttachAnimatorComponents(TObjectInstance* oi)
{
    if (!meshinitialized)
        if (!InitializeMesh((S3DImageryBody*)GetBody()))
            FatalError("Unable to initialize 3D imagery for %s", oi->GetName());

    const char* name = oi->GetTypeName();
    T3DAnimatorBuilder* builder = T3DAnimatorBuilder::GetBuilder(name);
    const char* initial_name = name;

    if (builder == &T3DAnimatorBuilderInstance)
    {
        name = oi->GetClassName();
        builder = T3DAnimatorBuilder::GetBuilder(name);
    }

    const bool interesting = (builder != &T3DAnimatorBuilderInstance) ||
                             (oi && oi->ObjClass() == OBJCLASS_EFFECT);
    static int logged_count = 0;
    if (interesting && logged_count < 64)
    {
        ++logged_count;
        log_info("[anim-components] inst=%p class='%s' type='%s' builder-name='%s' fallback-from='%s' builder=%p default=%p",
                 (void*)oi,
                 oi ? oi->GetClassName() : "<null>",
                 oi ? oi->GetTypeName() : "<null>",
                 name ? name : "<null>",
                 initial_name ? initial_name : "<null>",
                 (void*)builder,
                 (void*)&T3DAnimatorBuilderInstance);
    }
    builder->AttachComponents(oi);
}

TObjectAnimator* T3DImagery::NewObjectAnimator(TObjectInstance* oi)
{
    if (!meshinitialized)
        if (!InitializeMesh((S3DImageryBody*)GetBody()))
            FatalError("Unable to initialize 3D imagery for %s", oi->GetName());

    const char* name = oi->GetTypeName();
    T3DAnimatorBuilder* builder = T3DAnimatorBuilder::GetBuilder(name);

    if (builder == &T3DAnimatorBuilderInstance)
    {
        name = oi->GetClassName();
        builder = T3DAnimatorBuilder::GetBuilder(name);
    }

    builder->AttachComponents(oi);
    return (TObjectAnimator*)builder->Build(oi);
}

bool T3DImagery::NeedsAnimator(const TObjectInstance* /*oi*/) const
{
    return true;
}

// *******************************
// * T3DAnimatorBuilder Funtions *
// *******************************

int32_t T3DAnimatorBuilder::numanimtypes = 0;
T3DAnimatorBuilder* T3DAnimatorBuilder::builders[MAX3DANIMATORTYPES];

T3DAnimatorBuilder::T3DAnimatorBuilder(const char* name)
{
    if (numanimtypes < MAX3DANIMATORTYPES)
        builders[numanimtypes++] = this;

    animatorname = _strdup(name);
}

T3DAnimatorBuilder::T3DAnimatorBuilder()
{
    if (numanimtypes < MAX3DANIMATORTYPES)
        builders[numanimtypes++] = this;

    animatorname = (char*)"default";
}

T3DAnimator* T3DAnimatorBuilder::Build(TObjectInstance* oi)
{
    return new T3DAnimator(oi);
}

T3DAnimatorBuilder* T3DAnimatorBuilder::GetBuilder(const char* name)
{
    for (int32_t i = 0; i < numanimtypes; i++)
        if (stricmp(name, builders[i]->animatorname) == 0)
            return builders[i];

    return &T3DAnimatorBuilderInstance;
}

// ************************
// * T3DAnimator Funtions *
// ************************

struct T3DAnimator::SPartSysControllers
{
    struct Controller
    {
        T3DImagery::SPartSysTrack track;
        authored_partsys::State simulation;
        authored_partsys::TickInputs inputs;
        int64_t last_render_frame = -1;
    };
    std::vector<Controller> controllers;
    int32_t previous_state = -1;
    bool initialized = false;
    bool unsupported = false;
    uint64_t pulses = 0;
    uint64_t quads = 0, renders = 0;

    static void FillInputs(Controller& controller, T3DAnimator& animator,
                           TObjectInstance& owner)
    {
        auto& inputs = controller.inputs;
        const S3DPoint p = owner.Pos();
        inputs.owner_position = {float(p.x), float(p.y), float(p.z)};
        inputs.owner_face = static_cast<unsigned char>(owner.GetFace());
        // Retail 403771 reads animator+14; Animate copies owner frame later.
        // Gold's transient PPS curve must see initial cached0, not owner1.
        inputs.animation_frame = (owner.ObjId() == 0xd0c0f035u ||
            (owner.ObjId()==0x5be39ae0u && owner.GetState()==0 && animator.Get3DImagery()->HasRetailMightPartSysProfile()) ||
            (owner.ObjId()==0x82aeb30fu && owner.GetState()==0 && animator.Get3DImagery()->HasRetailImmortalmightPartSysProfile()) ||
            (owner.ObjId()==0xb0e024dfu && owner.GetState()==0 && animator.Get3DImagery()->HasRetailFmasteryPartSysProfile()) ||
            (owner.GetState()==0 && animator.Get3DImagery()->HasRetailSpeedFamilyPartSysProfile(owner.ObjId())) ||
            (owner.GetState()==0 && animator.Get3DImagery()->HasRetailStaticParticleProfile(owner.ObjId())) ||
            (owner.ObjId() == 0xad92bd29u && owner.GetState() == 0)) ? animator.GetFrame() : owner.GetFrame();
        // Retail Initialize obtains object zero's origin before resolving the
        // authored emitter list. Keep that distinct from its first emitter.
        hmm_mat4 object_zero;
        inputs.has_object_zero_origin = animator.GetObjectMatrix(0, &object_zero);
        if (inputs.has_object_zero_origin)
            inputs.object_zero_origin = {object_zero.Elements[3][0],
                                         object_zero.Elements[3][1], object_zero.Elements[3][2]};
        for (size_t i = 0; i < controller.track.emitters.size(); ++i)
        {
            const int32_t object = controller.track.emitters[i];
            S3DAnimObj* bone = animator.GetObject(object);
            hmm_mat4 matrix;
            // This is the source owner-local matrix, NOT the render bone's
            // stretched world matrix. The simulation adds live owner XYZ itself.
            if (!bone || !animator.GetObjectMatrix(object, &matrix)) continue;
            hmm_vec3 speed_position{},speed_scale{};
            const bool speed_pose=owner.GetState()==0 &&
                animator.Get3DImagery()->HasRetailSpeedFamilyPartSysProfile(owner.ObjId()) && object==1;
            if(speed_pose && !animator.SpeedEmitterLocalMatrix(matrix,&speed_position,&speed_scale))continue;
            auto& pose = inputs.emitters[i];
            std::memcpy(pose.matrix.data(), matrix.Elements, sizeof(matrix));
            pose.origin = {matrix.Elements[3][0], matrix.Elements[3][1], matrix.Elements[3][2]};
            pose.position = {bone->pos.X, bone->pos.Y, bone->pos.Z};
            pose.scale = {bone->scl.X, bone->scl.Y, bone->scl.Z};
            if(speed_pose){pose.position={speed_position.X,speed_position.Y,speed_position.Z};
                pose.scale={speed_scale.X,speed_scale.Y,speed_scale.Z};}
        }
    }
};

T3DAnimator::T3DAnimator(TObjectInstance* oi) : TObjectAnimator(oi) {}

void T3DAnimator::RefreshPartSysControllers()
{
    T3DImagery* imagery = Get3DImagery();
    if (!inst || !imagery || imagery->partsys_tracks.empty()) return;
    if (!partsys_controllers)
        partsys_controllers = std::make_unique<SPartSysControllers>();
    auto& state = *partsys_controllers;
    const int32_t current_state = inst->GetState();
    if (state.initialized && state.previous_state == current_state) return;

    // Runtime support is deliberately exact-type scoped. Successful syntax
    // parsing does not establish the semantics of later geyser/blend tags.
    const uint32_t id = inst->ObjId();
    const bool gold = id == 0xd0c0f035u && imagery->gold_partsys_profile && current_state == 0;
    const bool combat_start1 = id == 0xad92bd29u && imagery->combatflash_start1_partsys_profile && current_state == 0;
    const bool might = id==0x5be39ae0u && imagery->might_partsys_profile && current_state==0;
    const bool immortalmight = id==0x82aeb30fu && imagery->immortalmight_partsys_profile && current_state==0;
    const bool fmastery=id==0xb0e024dfu && imagery->fmastery_partsys_profile && current_state==0;
    const bool speed=imagery->HasRetailSpeedFamilyPartSysProfile(id) && current_state==0;
    const bool static_particles=imagery->HasRetailStaticParticleProfile(id) && current_state==0;
    if (!gold && !combat_start1 && !might && !immortalmight && !fmastery && !speed && !static_particles && (id < 0xd0c0f036u || id > 0xd0c0f039u))
    {
        if (!state.initialized)
            log_warn("[partsys] unsupported runtime type=%s id=%08x map_index=%d: only exact audited particle profiles are enabled",
                     inst->GetTypeName(), id, inst->GetMapIndex());
        state.unsupported = true;
        state.controllers.clear();
        state.previous_state = current_state;
        state.initialized = true;
        return;
    }

    // Legacy RefreshControllers keeps all-state controllers and removes only
    // controllers belonging to another state. Loop wrap never reinitializes.
    auto& controllers = state.controllers;
    controllers.erase(std::remove_if(controllers.begin(), controllers.end(),
        [current_state](const SPartSysControllers::Controller& controller) {
            return controller.track.state != -1 && controller.track.state != current_state;
        }), controllers.end());
    state.unsupported = false;
    if(static_particles)
        if(S3DAnimObj* prototype=GetObject(0))prototype->flags|=OBJ3D_GOLD_CAMERA_FACING;
    if (gold || combat_start1 || speed)
    {
        // Retail blendcont Initialize 405940: immediate state-entry writes to
        // every animator object, preserving the prior no-depth-test bit. Its
        // Pulse/Render are empty and Close does not restore these values.
        for (int32_t object = 0; object < NumObjects(); ++object)
            if (S3DAnimObj* bone = GetObject(object))
            {
                const char* name = imagery->GetObjectName(object);
                const unsigned char first = static_cast<unsigned char>(name[0]);
                if (!((first >= '0' && first <= '9') || (first >= 'a' && first <= 'z') ||
                      (first >= 'A' && first <= 'Z')))
                {
                    // Actual NewObject 409f61..409fda. Restore only this exact
                    // profile; generic/water objects retain their existing path.
                    if (std::strchr(name, '#')) bone->flags |= OBJ3D_GOLD_CAMERA_FACING;
                    if (std::strchr(name, '*')) bone->flags |= OBJ3D_HIDE;
                    if (std::strchr(name, '$')) bone->blend |= 0x40u;
                }
                bone->flags |= OBJ3D_BLEND;
                bone->blend = (bone->blend & 0x40u) | (speed?80u:16u);
            }
        log_info("[blendcont] init type=%s id=%08x map_index=%d state=0 tagframe=%d mode=%d objects=%d profile=%s",
                 inst->GetTypeName(), id, inst->GetMapIndex(), gold ? 5 : speed ? 1 : 0,
                 speed ? 80 : 16,NumObjects(),gold ? "gold" : speed ? (id==0xad92bd35u ? "quicksilver" : "speed") : "combatflash-start1");
    }
    for (const auto& track : imagery->partsys_tracks)
    {
        if (track.state != current_state && !(track.state == -1 && !state.initialized)) continue;
        if (!track.supported)
        {
            state.unsupported = true;
            log_warn("[partsys] unsupported owner type=%s id=%08x map_index=%d state=%d reason=%s",
                     inst->GetTypeName(), id, inst->GetMapIndex(), current_state, track.diagnostic.c_str());
            continue;
        }
        SPartSysControllers::Controller controller;
        controller.track = track;
        controller.inputs.emitters.resize(track.emitters.size());
        controller.inputs.ground_height = [](int x, int y, int z) {
            S3DPoint position = {x, y, z};
            return MapPane.GetWalkHeight(position);
        };
        SPartSysControllers::FillInputs(controller, *this, *inst);
        std::string diagnostic;
        // Default quality0 retains full emission. Explicit capture overrides
        // reproduce the measured retail setting through its existing policy.
        if (!controller.simulation.Initialize(track.definition, controller.inputs,
                                              StartupPartSysQuality, diagnostic))
        {
            state.unsupported = true;
            log_warn("[partsys] unsupported owner type=%s id=%08x map_index=%d state=%d reason=%s",
                     inst->GetTypeName(), id, inst->GetMapIndex(), current_state, diagnostic.c_str());
            continue;
        }
        EnableObject(track.prototype, false);
        log_info("[partsys] init type=%s id=%08x map_index=%d state=%d tagframe=%d emitters=%zu capacity=%zu quality=%d prototype=%s",
                 inst->GetTypeName(), id, inst->GetMapIndex(), track.state, track.tagframe,
                 track.emitters.size(), controller.simulation.Capacity(), StartupPartSysQuality,
                 imagery->GetObjectName(track.prototype));
        controllers.push_back(std::move(controller));
    }
    // An unsupported companion cannot leave a partially functioning substitute.
    if (state.unsupported) controllers.clear();
    state.previous_state = current_state;
    state.initialized = true;
}

void T3DAnimator::AdvancePartSysControllers()
{
    RefreshPartSysControllers();
    if (!partsys_controllers || partsys_controllers->unsupported) return;
    auto& state = *partsys_controllers;
    if (state.controllers.empty()) return;
    ++state.pulses;
    const authored_partsys::RandomRange rng = [](int minimum, int maximum) {
        return random(minimum, maximum);
    };
    for (auto& controller : state.controllers)
    {
        SPartSysControllers::FillInputs(controller, *this, *inst);
        // Authoritative map Pulse already runs once for each 24Hz tick, even
        // when several ticks catch up within one render frame. A global frame
        // deduplication here would incorrectly discard those pulses.
        // The retail controller ignores tagframe and evaluates raw owner frame.
        controller.simulation.Advance(controller.inputs, rng);
    }
}

bool T3DAnimator::PartSysOwnsObject(int32_t object) const
{
    if (!partsys_controllers) return false;
    if (partsys_controllers->unsupported) return true;
    for (const auto& controller : partsys_controllers->controllers)
        if (controller.track.prototype == object) return true;
    return false;
}


bool T3DAnimator::ShadowfistMeshBlend(int32_t object, uint32_t& blend) const
{
    if (!inst || inst->ObjId()!=0x550decafu || inst->GetState()!=0 ||
        !Get3DImagery()->HasRetailShadowfistProfile() || object<0 || object>=8 ||
        !animobjs.Used(object) || !(animobjs[object]->flags&OBJ3D_BLEND)) return false;
    blend=animobjs[object]->blend;
    return blend==16u;
}

bool T3DAnimator::ShadowfistMeshWorldMatrix(int32_t object, hmm_mat4& world)
{
    uint32_t blend=0;
    if (!ShadowfistMeshBlend(object,blend) || !IsObjectEnabled(object)) return false;
    auto* imagery=Get3DImagery(); hmm_vec3 position{},rotation{},scale{};
    // Actual default Render consumes integer authored frame. Do not smooth the
    // source pose with the modern renderer's fractional next-frame blend.
    imagery->SetPrevState(inst->GetPrevState(),inst->GetPrevFrame());
    if (!imagery->GetAniKey(object,0,inst->GetFrame(),position,rotation,scale) ||
        scale.X<=0 || scale.Y<=0 || scale.Z<=0) return false;
    // Retail40a4fc..40a55f: rotation then nonuniform scale; translation is
    // restored directly40a570..576, and '#' then preserves it40a71f..40a852.
    MtxClear(&world);
    MtxRotateX(&world,rotation.X); MtxRotateY(&world,rotation.Y); MtxRotateZ(&world,rotation.Z);
    MtxScale(&world,&scale);
    if (animobjs[object]->flags&OBJ3D_GOLD_CAMERA_FACING) ApplyRetailGoldCameraOrientation(world);
    MtxTranslate(&world,&position); // source preserves XYZ after fixed '#' camera matrices
    MtxMultiply(&world,&world,&inst->Transform().Matrix());
    return true;
}

bool T3DAnimator::TeleportationMeshBlend(int32_t object, uint32_t& blend) const
{
    if (!inst || inst->ObjId()!=0xad92bd40u || inst->GetState()!=0 ||
        !Get3DImagery()->HasRetailTeleportationProfile() || object<0 || object>=6 ||
        !animobjs.Used(object) || !(animobjs[object]->flags&OBJ3D_BLEND)) return false;
    blend=animobjs[object]->blend;
    return blend==16u;
}

bool T3DAnimator::TeleportationMeshWorldMatrix(int32_t object, hmm_mat4& world)
{
    uint32_t blend=0;
    if (!TeleportationMeshBlend(object,blend) || !IsObjectEnabled(object)) return false;
    auto* imagery=Get3DImagery(); hmm_vec3 position{},rotation{},scale{};
    // Actual default Render consumes integer authored frame. Do not smooth the
    // source pose with the modern renderer's fractional next-frame blend.
    imagery->SetPrevState(inst->GetPrevState(),inst->GetPrevFrame());
    if (!imagery->GetAniKey(object,0,inst->GetFrame(),position,rotation,scale) ||
        scale.X<=0 || scale.Y<=0 || scale.Z<=0) return false;
    // Retail40a4fc..40a55f: rotation then nonuniform scale; translation is
    // restored directly40a570..576, and '#' then preserves it40a71f..40a852.
    MtxClear(&world);
    MtxRotateX(&world,rotation.X); MtxRotateY(&world,rotation.Y); MtxRotateZ(&world,rotation.Z);
    MtxScale(&world,&scale);
    // All six exact source names lack #/$; no fixed camera matrices or depth bypass.
    MtxTranslate(&world,&position); // source preserves XYZ after fixed '#' camera matrices
    MtxMultiply(&world,&world,&inst->Transform().Matrix());
    return true;
}

bool T3DAnimator::WarriorbornMeshBlend(int32_t object, uint32_t& blend) const
{
    if (!inst || inst->ObjId()!=0x9de2a0feu || inst->GetState()!=0 ||
        !Get3DImagery()->HasRetailWarriorbornProfile() || object<0 || object>=8 ||
        !animobjs.Used(object) || !(animobjs[object]->flags&OBJ3D_BLEND)) return false;
    blend=animobjs[object]->blend;
    return blend==16u;
}

bool T3DAnimator::WarriorbornMeshWorldMatrix(int32_t object, hmm_mat4& world)
{
    uint32_t blend=0;
    if (!WarriorbornMeshBlend(object,blend) || !IsObjectEnabled(object)) return false;
    auto* imagery=Get3DImagery(); hmm_vec3 position{},rotation{},scale{};
    // Actual default Render consumes integer authored frame. Do not smooth the
    // source pose with the modern renderer's fractional next-frame blend.
    imagery->SetPrevState(inst->GetPrevState(),inst->GetPrevFrame());
    if (!imagery->GetAniKey(object,0,inst->GetFrame(),position,rotation,scale) ||
        scale.X<=0 || scale.Y<=0 || scale.Z<=0) return false;
    // Retail40a4fc..40a55f: rotation then nonuniform scale; translation is
    // restored directly40a570..576, and '#' then preserves it40a71f..40a852.
    MtxClear(&world);
    MtxRotateX(&world,rotation.X); MtxRotateY(&world,rotation.Y); MtxRotateZ(&world,rotation.Z);
    MtxScale(&world,&scale);
    if (animobjs[object]->flags&OBJ3D_GOLD_CAMERA_FACING) ApplyRetailGoldCameraOrientation(world);
    MtxTranslate(&world,&position); // source preserves XYZ after fixed '#' camera matrices
    MtxMultiply(&world,&world,&inst->Transform().Matrix());
    return true;
}

bool T3DAnimator::MPAppearStartMeshBlend(int32_t object, uint32_t& blend) const
{
    if (!inst || inst->ObjId()!=0xad99fdf2u || inst->GetState()!=0 ||
        !Get3DImagery()->HasRetailMPAppearStartProfile() || object<0 || object>=7 ||
        !animobjs.Used(object) || !(animobjs[object]->flags&OBJ3D_BLEND)) return false;
    blend=animobjs[object]->blend;
    return blend==80u;
}

bool T3DAnimator::MPAppearStartMeshWorldMatrix(int32_t object, hmm_mat4& world)
{
    uint32_t blend=0;
    if (!MPAppearStartMeshBlend(object,blend) || !IsObjectEnabled(object)) return false;
    auto* imagery=Get3DImagery(); hmm_vec3 position{},rotation{},scale{};
    // Actual default Render consumes integer authored frame. Do not smooth the
    // source pose with the modern renderer's fractional next-frame blend.
    imagery->SetPrevState(inst->GetPrevState(),inst->GetPrevFrame());
    if (!imagery->GetAniKey(object,0,inst->GetFrame(),position,rotation,scale) ||
        scale.X<=0 || scale.Y<=0 || scale.Z<=0) return false;
    // Retail40a4fc..40a55f: rotation then nonuniform scale; translation is
    // restored directly40a570..576, and '#' then preserves it40a71f..40a852.
    MtxClear(&world);
    MtxRotateX(&world,rotation.X); MtxRotateY(&world,rotation.Y); MtxRotateZ(&world,rotation.Z);
    MtxScale(&world,&scale);
    if (animobjs[object]->flags&OBJ3D_GOLD_CAMERA_FACING) ApplyRetailGoldCameraOrientation(world);
    MtxTranslate(&world,&position); // source preserves XYZ after fixed '#' camera matrices
    MtxMultiply(&world,&world,&inst->Transform().Matrix());
    return true;
}

bool T3DAnimator::GoldBaseMeshBlend(int32_t object, uint32_t& blend) const
{
    if (!inst || inst->ObjId() != 0xd0c0f035u || !Get3DImagery()->HasGoldPartSysProfile() ||
        !partsys_controllers || partsys_controllers->unsupported || object != 1 ||
        !animobjs.Used(object) || !(animobjs[object]->flags & OBJ3D_BLEND)) return false;
    blend = animobjs[object]->blend;
    return true;
}

bool T3DAnimator::GoldBaseMeshWorldMatrix(hmm_mat4& world)
{
    uint32_t blend = 0;
    if (!GoldBaseMeshBlend(1, blend) || blend != 80u) return false;
    auto* imagery = Get3DImagery();
    hmm_vec3 position{}, rotation{}, scale{};
    imagery->SetPrevState(inst->GetPrevState(), inst->GetPrevFrame());
    if (!imagery->GetAniKey(1, inst->GetState(), inst->GetFrame(), position, rotation, scale)) return false;
    if (scale.X <= 0 || scale.Y <= 0 || scale.Z <= 0) return false;
    MtxClear(&world);
    MtxScale(&world, &scale);
    MtxRotateX(&world, rotation.X); MtxRotateY(&world, rotation.Y); MtxRotateZ(&world, rotation.Z);
    ApplyRetailGoldCameraOrientation(world);
    // Retail writes preserved local XYZ into matrix._41/_42/_43 after the
    // camera matrices; translation must not be rotated or scaled by them.
    MtxTranslate(&world, &position);
    MtxMultiply(&world, &world, &inst->Transform().Matrix());
    return true;
}

bool T3DAnimator::CombatFlashStart1BaseMeshBlend(int32_t object, uint32_t& blend) const
{
    if (!inst || inst->ObjId()!=0xad92bd29u || inst->GetState()!=0 ||
        !Get3DImagery()->HasCombatFlashStart1PartSysProfile() || !partsys_controllers ||
        partsys_controllers->unsupported || object!=2 || !animobjs.Used(object) ||
        !(animobjs[object]->flags & OBJ3D_BLEND)) return false;
    blend=animobjs[object]->blend;
    return blend==16u; // '#impact01' has no '$': depth test remains enabled.
}

bool T3DAnimator::CombatFlashStart1BaseMeshWorldMatrix(hmm_mat4& world)
{
    uint32_t blend=0;
    if (!CombatFlashStart1BaseMeshBlend(2,blend)) return false;
    auto* imagery=Get3DImagery(); hmm_vec3 position{},rotation{},scale{};
    imagery->SetPrevState(inst->GetPrevState(),inst->GetPrevFrame());
    if (!imagery->GetAniKey(2,0,inst->GetFrame(),position,rotation,scale)) return false;
    if (scale.X<=0 || scale.Y<=0 || scale.Z<=0) return false;
    MtxClear(&world); MtxScale(&world,&scale);
    MtxRotateX(&world,rotation.X); MtxRotateY(&world,rotation.Y); MtxRotateZ(&world,rotation.Z);
    ApplyRetailGoldCameraOrientation(world); // Recovered generic '#' branch, no new fitted matrix.
    MtxTranslate(&world,&position); // Original preserves translation after fixed camera orientation.
    MtxMultiply(&world,&world,&inst->Transform().Matrix());
    return true;
}

bool T3DAnimator::ImmortalmightBaseMeshBlend(int32_t object,uint32_t& blend) const
{
    if(!inst || inst->ObjId()!=0x82aeb30fu || inst->GetState()!=0 ||
       !Get3DImagery()->HasRetailImmortalmightPartSysProfile() || !partsys_controllers ||
       partsys_controllers->unsupported || object!=2 || !animobjs.Used(2)) return false;
    // Actual RGB565 NewObject adds no OBJ3D_BLEND. Source controller Render
    // leaves its last particle's mode16 in the scene for the later base mesh.
    // Before a particle is submitted, BeginRender preserves caller state.
    // An explicit measured capture input covers that case; 0 retains visible
    // ordinary opaque compatibility, without claiming unmeasured retail state.
    blend=partsys_controllers->quads ? 16u : uint32_t(StartupPartSysIncomingBlend);
    return blend==0u || blend==16u || blend==80u;
}

bool T3DAnimator::ImmortalmightBaseMeshWorldMatrix( hmm_mat4& world)
{
    uint32_t blend=0;
    if (!ImmortalmightBaseMeshBlend(2,blend) || !IsObjectEnabled(2)) return false;
    auto* imagery=Get3DImagery(); hmm_vec3 position{},rotation{},scale{};
    // Actual default Render consumes integer authored frame. Do not smooth the
    // source pose with the modern renderer's fractional next-frame blend.
    imagery->SetPrevState(inst->GetPrevState(),inst->GetPrevFrame());
    if (!imagery->GetAniKey(2,0,inst->GetFrame(),position,rotation,scale) ||
        scale.X<=0 || scale.Y<=0 || scale.Z<=0) return false;
    // Retail40a4fc..40a55f: rotation then nonuniform scale; translation is
    // restored directly40a570..576, and '#' then preserves it40a71f..40a852.
    MtxClear(&world);
    MtxRotateX(&world,rotation.X); MtxRotateY(&world,rotation.Y); MtxRotateZ(&world,rotation.Z);
    MtxScale(&world,&scale);
    if (animobjs[2]->flags&OBJ3D_GOLD_CAMERA_FACING) ApplyRetailGoldCameraOrientation(world);
    MtxTranslate(&world,&position); // source preserves XYZ after fixed '#' camera matrices
    MtxMultiply(&world,&world,&inst->Transform().Matrix());
    return true;
}

bool T3DAnimator::FmasteryBaseMeshBlend(int32_t object,uint32_t& blend) const
{
    if(!inst || inst->ObjId()!=0xb0e024dfu || inst->GetState()!=0 ||
       !Get3DImagery()->HasRetailFmasteryPartSysProfile() || !partsys_controllers ||
       partsys_controllers->unsupported || object!=2 || !animobjs.Used(2)) return false;
    // Actual RGB565 NewObject adds no OBJ3D_BLEND. Source controller Render
    // leaves its last particle's mode16 in the scene for the later base mesh.
    // Before a particle is submitted, BeginRender preserves caller state.
    // An explicit measured capture input covers that case; 0 retains visible
    // ordinary opaque compatibility, without claiming unmeasured retail state.
    blend=partsys_controllers->quads ? 16u : uint32_t(StartupPartSysIncomingBlend);
    return blend==0u || blend==16u || blend==80u;
}

bool T3DAnimator::FmasteryBaseMeshWorldMatrix( hmm_mat4& world)
{
    uint32_t blend=0;
    if (!FmasteryBaseMeshBlend(2,blend) || !IsObjectEnabled(2)) return false;
    auto* imagery=Get3DImagery(); hmm_vec3 position{},rotation{},scale{};
    // Actual default Render consumes integer authored frame. Do not smooth the
    // source pose with the modern renderer's fractional next-frame blend.
    imagery->SetPrevState(inst->GetPrevState(),inst->GetPrevFrame());
    if (!imagery->GetAniKey(2,0,inst->GetFrame(),position,rotation,scale) ||
        scale.X<=0 || scale.Y<=0 || scale.Z<=0) return false;
    // Retail #branch40a71f..40a852 rebuilds scale then rotations before
    // fixed camera. Exact Fmaster flare is anisotropic: order cannot commute.
    MtxClear(&world);
    MtxScale(&world,&scale);
    MtxRotateX(&world,rotation.X); MtxRotateY(&world,rotation.Y); MtxRotateZ(&world,rotation.Z);
    if (animobjs[2]->flags&OBJ3D_GOLD_CAMERA_FACING) ApplyRetailGoldCameraOrientation(world);
    MtxTranslate(&world,&position); // source preserves XYZ after fixed '#' camera matrices
    MtxMultiply(&world,&world,&inst->Transform().Matrix());
    return true;
}

bool T3DAnimator::SpeedEmitterLocalMatrix(hmm_mat4& matrix,hmm_vec3* output_position,hmm_vec3* output_scale)
{
    if(!inst || inst->GetState()!=0 ||
       !Get3DImagery()->HasRetailSpeedFamilyPartSysProfile(inst->ObjId()) || !animobjs.Used(1))return false;
    hmm_vec3 position{},rotation{},scale{};
    auto* imagery=Get3DImagery();imagery->SetPrevState(-1,0);
    // Original controller Pulse reads the animator's cached integer frame.
    if(!imagery->GetAniKey(1,0,GetFrame(),position,rotation,scale))return false;
    BuildRetailSpeedCameraMatrix(matrix,position,rotation,scale);
    if(output_position)*output_position=position;
    if(output_scale)*output_scale=scale;
    return true;
}

bool T3DAnimator::SpeedBaseMeshBlend(int32_t object,uint32_t& blend) const
{
    if(!inst || inst->GetState()!=0 ||
       !Get3DImagery()->HasRetailSpeedFamilyPartSysProfile(inst->ObjId()) || !partsys_controllers ||
       partsys_controllers->unsupported || object!=1 || !animobjs.Used(1) ||
       !(animobjs[1]->flags&OBJ3D_BLEND))return false;
    blend=animobjs[1]->blend;return blend==80u;
}

bool T3DAnimator::SpeedBaseMeshWorldMatrix(hmm_mat4& world)
{
    uint32_t blend=0;
    if(!SpeedBaseMeshBlend(1,blend) || !IsObjectEnabled(1))return false;
    hmm_vec3 position{},rotation{},scale{};auto* imagery=Get3DImagery();
    imagery->SetPrevState(inst->GetPrevState(),inst->GetPrevFrame());
    if(!imagery->GetAniKey(1,0,inst->GetFrame(),position,rotation,scale))return false;
    BuildRetailSpeedCameraMatrix(world,position,rotation,scale);
    MtxMultiply(&world,&world,&inst->Transform().Matrix());return true;
}

uint64_t T3DAnimator::PartSysPulseCount() const
{ return partsys_controllers ? partsys_controllers->pulses : 0; }

size_t T3DAnimator::PartSysControllerCount() const
{ return partsys_controllers ? partsys_controllers->controllers.size() : 0; }

bool T3DAnimator::PartSysUnsupported() const
{ return partsys_controllers && partsys_controllers->unsupported; }

size_t T3DAnimator::PartSysLiveParticles() const
{
    size_t alive = 0;
    if (partsys_controllers)
        for (const auto& controller : partsys_controllers->controllers)
            for (const auto& particle : controller.simulation.Particles())
                alive += particle.alive ? 1 : 0;
    return alive;
}

T3DAnimator::SAuthoredPartSysStats T3DAnimator::PartSysStats() const
{
    SAuthoredPartSysStats stats;
    if (!partsys_controllers) return stats;
    const auto& state = *partsys_controllers;
    stats.controllers = state.controllers.size();
    stats.ticks = state.pulses;
    stats.quads = state.quads;
    stats.renders = state.renders;
    stats.unsupported = state.unsupported;
    for (const auto& controller : state.controllers)
    {
        stats.capacity += controller.simulation.Capacity();
        stats.emitters += controller.track.emitters.size();
        for (const auto& particle : controller.simulation.Particles())
            stats.alive += particle.alive ? 1 : 0;
    }
    if (!state.controllers.empty())
        stats.next_emitter = state.controllers.front().simulation.NextEmitter();
    return stats;
}

int32_t T3DAnimator::SubmitPartSys(TRenderer& renderer, int32_t object,
                                  int32_t texture_slot, const hmm_vec3& render_scale)
{
    if (!partsys_controllers || partsys_controllers->unsupported) return 0;
    int32_t submitted = 0;
    const authored_partsys::RandomRange rng = [](int minimum, int maximum) {
        return random(minimum, maximum);
    };
    for (auto& controller : partsys_controllers->controllers)
    {
        const auto& track = controller.track;
        if (track.prototype != object || track.texture_slot != texture_slot ||
            track.state != inst->GetState() ||
            controller.last_render_frame == TTime::FrameCount()) continue;
        const TTextureHandle texture = Get3DImagery()->GetTextureHandle(texture_slot - 1);
        if (texture == kInvalidTexture) continue;
        controller.last_render_frame = TTime::FrameCount();
        ++partsys_controllers->renders;
        const auto& particles = controller.simulation.Particles();
        for (size_t slot = 0; slot < particles.size(); ++slot)
        {
            if (!particles[slot].alive) continue;
            // The retail helper samples three rotation ranges during Render.
            // Keep those calls here; Submit never advances simulation state.
            const auto sample = controller.simulation.SampleRender(slot, rng);
            // Native Speed RenderObject40a9e7..9f3 skips geometry when its
            // CalcObjectMatrix scale gate40a591..5c1 returns0. Sampling still
            // happens first, preserving its three render-time RNG calls.
            if(inst->GetState()==0 &&
               ((Get3DImagery()->HasRetailSpeedFamilyPartSysProfile(inst->ObjId()) && track.prototype==2) ||
                (Get3DImagery()->HasRetailStaticParticleProfile(inst->ObjId()) && track.prototype==0)) &&
               sample.scale<=9.999999747378752e-6f)continue;
            hmm_mat4 matrix;
            MtxClear(&matrix);
            const hmm_vec3 scale = {sample.scale, sample.scale, sample.scale};
            MtxScale(&matrix, &scale);
            MtxRotateX(&matrix, sample.rotation[0]);
            MtxRotateY(&matrix, sample.rotation[1]);
            MtxRotateZ(&matrix, sample.rotation[2]);
            const bool might = inst->ObjId()==0x5be39ae0u && inst->GetState()==0 &&
                Get3DImagery()->HasRetailMightPartSysProfile() && track.prototype==0;
            const bool immortalmight=inst->ObjId()==0x82aeb30fu && inst->GetState()==0 &&
                Get3DImagery()->HasRetailImmortalmightPartSysProfile() && track.prototype==0;
            const bool fmastery=inst->ObjId()==0xb0e024dfu && inst->GetState()==0 &&
                Get3DImagery()->HasRetailFmasteryPartSysProfile() && track.prototype==0;
            const bool speed=inst->GetState()==0 &&
                Get3DImagery()->HasRetailSpeedFamilyPartSysProfile(inst->ObjId()) && track.prototype==2;
            const bool static_particles=inst->GetState()==0 &&
                Get3DImagery()->HasRetailStaticParticleProfile(inst->ObjId()) && track.prototype==0;
            if (might || immortalmight || fmastery || speed || static_particles || inst->ObjId() == 0xd0c0f035u ||
                (inst->ObjId() == 0xad92bd29u && inst->GetState() == 0 &&
                 Get3DImagery()->HasCombatFlashStart1PartSysProfile() && track.prototype == 0))
                ApplyRetailGoldCameraOrientation(matrix); // actual '#' flag retained on both audited prototypes
            const hmm_vec3 position = {sample.position[0], sample.position[1], sample.position[2]};
            MtxTranslate(&matrix, &position);

            SQuadDrawItem item = {};
            item.key.texture = texture;
            // Retail particle Render overwrites the prototype's companion
            // blend with its own per-particle mode. Gold coins are alpha2;
            // the four water profiles remain litadd16 exactly as before.
            item.key.blend = uint8_t(sample.blendmode == 2 ? EFxBlend::Alpha : EFxBlend::AdditiveStraight);
            // Preserve source packed prelit vertex color. Combat particles are
            // RGB565 with litadd16, not normal-lit material meshes or alpha coins.
            item.retail_texture = (might || immortalmight || fmastery || speed || static_particles || inst->ObjId() == 0xd0c0f035u ||
                (inst->ObjId() == 0xad92bd29u && inst->GetState() == 0)) ? 1 : 0;
            item.key.depth_mode = uint8_t(EFxDepthMode::TestNoWrite);
            item.light_mode = EFxLightMode::Unlit;
            for (int c = 0; c < 3; ++c)
                item.color_rgba[c] = (might || immortalmight || fmastery || speed || static_particles) ? float((int(sample.color[c]) & 255)>>3)/31.0f : sample.color[c]/255.0f;
            // Actual SW LV prelit channels are5bit; canonicalDEST_ONE ignores
            // alpha in its8wordXYZ/UV/RGB packet. Keep literal alpha metadata,
            // no premultiplication/alternateblend. PixelLSB-table raster gate open.
            item.color_rgba[3] = sample.alpha;
            // Original faces (2,3,0)/(1,2,0) are preserved by this reorder
            // into FXquad's (2,0,3)/(1,3,0); first face differs only cyclically.
            constexpr int corners[4] = {0, 1, 3, 2};
            for (int c = 0; c < 4; ++c)
            {
                const S3DVertex& vertex = track.vertices[size_t(corners[c])];
                hmm_vec3 world;
                MtxTransform(&matrix, &vertex.pos, &world);
                item.world_pos[c][0] = world.X * render_scale.X;
                item.world_pos[c][1] = world.Y * render_scale.Y;
                // SampleRender's centre is retail MODELZ (FIX applied), while
                // Particle.position remains raw map/common-world simulation XYZ.
                // Match ordinary mesh raw translation + local MODELZ stretch;
                // ABS particles bypass the owner's transform, so bridge here once.
                item.world_pos[c][2] = (particles[slot].position[2] +
                    (world.Z - sample.position[2]) * WORLD3D_Z_SCALE) * render_scale.Z;
                item.uv[c][0] = vertex.tu;
                item.uv[c][1] = vertex.tv;
            }
            renderer.SubmitFxQuad(item);
            ++submitted;
            ++partsys_controllers->quads;
        }
    }
    return submitted;
}

T3DAnimator::~T3DAnimator()
{
    Close();
}

void T3DAnimator::Initialize()
{
    TObjectAnimator::Initialize();

    pos.X = pos.Y = pos.Z = 100000.0f;
    rot.X = rot.Y = rot.Z = 0.0f;
    changed = true;

    flags = ANI3D_ADDUPDATERECT;
    inst->SetFlags(OF_MOVING);

    SetupObjects();
    retail_warp_state.Reset();
    retail_warp_enabled = inst->GetState()==0 && Get3DImagery()->HasRetailWarpProfile(inst->ObjId());
    if (retail_warp_enabled)
        log_info("[warp-atlas] init id=%08x map_index=%d mode=2 state=0",inst->ObjId(),inst->GetMapIndex());
    if(inst->ObjId()==0x82aeb30fu && inst->GetState()==0 && Get3DImagery()->HasRetailImmortalmightPartSysProfile()) {
        if(auto* prototype=GetObject(0))prototype->flags|=OBJ3D_GOLD_CAMERA_FACING;
        // Retail409e57..5c skips material-alpha detection when texture alpha
        // mask0. Keep only '#' camera; no forced blend metadata on the base.
        if(auto* flare=GetObject(2))flare->flags|=OBJ3D_GOLD_CAMERA_FACING;
        log_info("[immortalmight-base] incoming=%d explicit=%d cold-policy=%s",StartupPartSysIncomingBlend,
                 StartupPartSysIncomingBlend!=0,StartupPartSysIncomingBlend==16?"measured16":StartupPartSysIncomingBlend==80?"explicit80-diagnostic":"ordinary-opaque-compatibility-unmeasured");
    }
    if(inst->ObjId()==0xb0e024dfu && inst->GetState()==0 && Get3DImagery()->HasRetailFmasteryPartSysProfile()) {
        if(auto* prototype=GetObject(0))prototype->flags|=OBJ3D_GOLD_CAMERA_FACING;
        // Retail409e57..5c skips material-alpha detection when texture alpha
        // mask0. Keep only '#' camera; no forced blend metadata on the base.
        if(auto* flare=GetObject(2))flare->flags|=OBJ3D_GOLD_CAMERA_FACING;
        log_info("[fmastery-base] incoming=%d explicit=%d cold-policy=%s",StartupPartSysIncomingBlend,
                 StartupPartSysIncomingBlend!=0,StartupPartSysIncomingBlend==16?"measured16":StartupPartSysIncomingBlend==80?"explicit80-diagnostic":"ordinary-opaque-compatibility-unmeasured");
    }
    if(inst->ObjId()==0x5be39ae0u && inst->GetState()==0 && Get3DImagery()->HasRetailMightPartSysProfile())
        if(auto* prototype=GetObject(0))prototype->flags|=OBJ3D_GOLD_CAMERA_FACING;

    if (inst->ObjId()==0xad99fdf2u && inst->GetState()==0 && Get3DImagery()->HasRetailMPAppearStartProfile())
    {
        // Retail blendcont Initialize405940 writes immediately on state entry;
        // tag frame20 is metadata, not delayed activation. Pulse/Render are RET.
        for (int object=0;object<NumObjects();++object) if (auto* bone=GetObject(object)) {
            const char* name=Get3DImagery()->GetObjectName(object);
            if (name[0]=='#') bone->flags|=OBJ3D_GOLD_CAMERA_FACING;
            if (name[0]=='#' && std::strchr(name,'$')) bone->blend|=0x40u;
            bone->flags|=OBJ3D_BLEND;
            bone->blend=(bone->blend&0x40u)|80u;
        }
        log_info("[blendcont] init type=MPAppear id=ad99fdf2 map_index=%d state=0 tagframe=20 mode=80 objects=7 profile=appear-start",inst->GetMapIndex());
    }
    if (inst->ObjId()==0x550decafu && inst->GetState()==0 && Get3DImagery()->HasRetailShadowfistProfile())
    {
        // Retail409f7b '#' only sets camera flag; '$'409fa9 is the sole
        // source bit40 writer. These eight exact names have no '$'.
        // Blendcont405940 initializes immediately, not at metadata frame10.
        for (int object=0;object<NumObjects();++object) if (auto* bone=GetObject(object)) {
            bone->flags|=OBJ3D_GOLD_CAMERA_FACING|OBJ3D_BLEND;
            bone->blend=(bone->blend&0x40u)|16u;
        }
        log_info("[blendcont] init type=Shadowfist id=550decaf map_index=%d state=0 tagframe=10 mode=16 objects=8 profile=sfist-loop",inst->GetMapIndex());
    }
    if (inst->ObjId()==0x9de2a0feu && inst->GetState()==0 && Get3DImagery()->HasRetailWarriorbornProfile())
    {
        // Retail409f7b '#' only sets camera flag; '$'409fa9 is the sole
        // source bit40 writer. These eight exact names have no '$'.
        // Blendcont405940 initializes immediately, not at metadata frame10.
        for (int object=0;object<NumObjects();++object) if (auto* bone=GetObject(object)) {
            bone->flags|=OBJ3D_GOLD_CAMERA_FACING|OBJ3D_BLEND;
            bone->blend=(bone->blend&0x40u)|16u;
        }
        log_info("[blendcont] init type=Warriorborn id=9de2a0fe map_index=%d state=0 tagframe=10 mode=16 objects=8 profile=wborn-loop",inst->GetMapIndex());
    }
    if (inst->ObjId()==0xad92bd40u && inst->GetState()==0 && Get3DImagery()->HasRetailTeleportationProfile())
    {
        // Exact default Teleportation imagery, not the custom Teleporter animator.
        // Retail blendcont405940 initializes immediately; metadata frame35 is not a delay.
        // No source name contains #/$, so litadd16 remains depth-tested.
        for (int object=0;object<NumObjects();++object) if (auto* bone=GetObject(object)) {
            bone->flags|=OBJ3D_BLEND;
            bone->blend=(bone->blend&0x40u)|16u;
        }
        log_info("[blendcont] init type=teleportation id=ad92bd40 map_index=%d state=0 tagframe=35 mode=16 objects=6 profile=teleportation-loop",inst->GetMapIndex());
    }
    ConfigureI3DLegacyPoseLayout(Get3DImagery(), pose_layout);
    pose_current.Configure(pose_layout);
    pose_next.Configure(pose_layout);
    pose_blended.Configure(pose_layout);

    RefreshPartSysControllers();

    animid = Scene3D.AddAnimator(this);
}

void T3DAnimator::Close()
{
    retail_warp_enabled = false;
    retail_warp_state.Reset();
    partsys_controllers.reset();
    Scene3D.RemoveAnimator(animid);

    for (int32_t c = 0; c < animobjs.NumItems(); c++)
        RemoveObject(c);

    animobjs.DeleteAll();
    pose_current = {};
    pose_next = {};
    pose_blended = {};
    pose_layout = {};
    pose_update_render_frame = -1;
    pose_update_state = -1;
    pose_update_frame = -1;
    pose_update_next_state = -1;
    pose_update_next_frame = -1;
    pose_update_prev_state = -1;
    pose_update_prev_frame = -1;
    pose_update_frame_frac = -1.0f;

    TObjectAnimator::Close();
}

void T3DAnimator::RecordNewExtents(TObjectInstance* oi, int32_t state, bool frontonly)
{
    if (state < 0)
    {
        for (int32_t i = 0; i < oi->NumStates(); i++)
        {
            SImageryStateHeader* st = image->GetState(i);
            st->regx = st->regy = 0;
            st->width = st->height = 0;
        }
        flags |= ANI3D_UPDATEALLSTATES;
        state = 0;
    }
    else
    {
        SImageryStateHeader* st = image->GetState(state);
        st->regx = st->regy = 0;
        st->width = st->height = 0;
    }

    flags |= ANI3D_UPDATEBOUNDRECT;

    if (frontonly)
    {
        oi->Face(0);
        flags |= ANI3D_UPDATEFRONTONLY;
    }
    else
        oi->Face(32);
    oi->SetState(state);

    NoFrameSkip = true;
    DisableTimer = true;
    UpdatingBoundingRect = true;
}

void T3DAnimator::AnimateResetBoundRect()
{
    int32_t face = inst->GetFace();
    face += (flags & ANI3D_UPDATEFRONTONLY) ? 256 : 64;
    inst->Face(face & 255);
    if (face < 1 || face >= 256)
    {
        bool done = false;
        if (flags & ANI3D_UPDATEALLSTATES)
        {
            inst->Face((flags & ANI3D_UPDATEFRONTONLY) ? 0 : 32);
            if (!inst->SetState(state + 1))
                done = true;
        }
        else
            done = true;

        if (done)
        {
            flags &= ~(ANI3D_UPDATEBOUNDRECT | ANI3D_UPDATEALLSTATES | ANI3D_UPDATEFRONTONLY);
            image->SetHeaderDirty(true);
            UpdatingBoundingRect = false;
            NoFrameSkip = false;
            DisableTimer = false;
        }

        image->SetHeaderDirty(true);
    }

    inst->SetCommandDone(false);
    inst->ResetState();
}

void T3DAnimator::Pulse()
{
    // Map Pulse is authoritative: one call per 24Hz tick, including catch-up.
    // Each owner begins at (0,0), independently of the global animation clock.
    if (retail_warp_enabled && inst->GetState()==0) retail_warp_state.Step();
    AdvancePartSysControllers();
    ((T3DImagery*)image)->PlaySound(inst, state, frame);
}

bool T3DAnimator::WarpAtlasOffset(int32_t object, float output[2]) const
{
    if (!retail_warp_enabled || object!=0 || inst->GetState()!=0) return false;
    output[0]=retail_warp_state.u;
    output[1]=retail_warp_state.v;
    return true;
}

// ---------------------------------------------------------------------------
// SKELETON-TO-TTRANSFORM MIGRATION -- REMAINING WORK (C5)
// ---------------------------------------------------------------------------
//
// The renderer's main mesh path (maprenderer.cpp Submit, mesh branch)
// reads bone.transform.Matrix() for any object that has a live
// T3DAnimator with a populated bone. A fallback compose path
// (BuildAnimPoseObjectMatrix + BuildRootMatrixSource +
// per-instance Z-stretch post-multiply) is still present for two
// cases that haven't been migrated:
//
//   1. Editor preview (force_mesh_preview_pose). This path uses
//      meshextract.cpp::BuildStaticObjectMatrix, which spins up a
//      temporary std::vector<S3DAnimObj> per call, sets parent
//      pointers, and calls T3DImagery::CalcObjectMatrix. To migrate:
//      wire the temp bones' TTransform parents the same way
//      T3DAnimator::SetupObjects does, RefreshHierarchy on the root
//      bone (no inst.transform_ involved -- preview is bone-local),
//      and apply the (1, 1, WORLD3D_Z_SCALE) Z stretch as a local
//      scale on the root temp bone so bone.transform.Matrix() comes
//      out of the call already-stretched. Then the renderer's
//      preview branch can read bone.transform.Matrix() the same way
//      the live-animator branch does.
//
//   2. Non-character mesh instances without a live animator. For
//      characters TCharacter::IsAnimatorPermanent keeps the animator
//      attached for the lifetime of the instance; non-character 3D
//      meshes (props, helpers, etc.) get an animator lazily via
//      OnScreen() and lose it on OffScreen / SetState. The renderer
//      fallback exists for the "no animator at draw time" window.
//      To migrate: extend the permanent-animator rule to every
//      TObjectInstance whose imagery is T3DImagery (or just declare
//      "everything 3D has a permanent T3DAnimator"). Then the
//      renderer can assume bone.transform is always available and
//      drop the fallback, BuildRootMatrixSource, and the
//      per-instance Z-stretch post-multiply.
//
// Once both are in place: delete S3DAnimObj.matrix, S3DAnimObj.parent,
// the manual parent-chain MtxMultiply at the tail of CalcObjectMatrix,
// BuildRootMatrixSource itself, and the legacy compose blocks in
// maprenderer.cpp Submit and meshextract.cpp BuildStatic/Animated*.
// Estimated 1-2 hours of careful work + smoke testing. Deferred
// (as of c3a8f21) so the AI / combat bring-up doesn't block on it.
// ---------------------------------------------------------------------------
struct SLegacyRenderKey
{
    int32_t state = -1;
    int32_t frame = 0;
};

static int32_t FindTaggedContinuationState(T3DImagery* img, int32_t state)
{
    if (!img || state < 0 || state >= img->NumStates())
        return -1;

    char* endtag = img->FindTag((char*)"end", state);
    if (!endtag || !*endtag)
        return -1;

    int32_t same_state = -1;
    for (int32_t candidate = 0; candidate < img->NumStates(); ++candidate)
    {
        if (img->GetAniLength(candidate) <= 0)
            continue;

        char* begtag = img->FindTag((char*)"beg", candidate);
        if (!begtag || stricmp(endtag, begtag) != 0)
            continue;

        if (candidate != state)
            return candidate;
        same_state = candidate;
    }
    return same_state;
}

static SLegacyRenderKey NextRenderKey(TObjectInstance* inst, T3DImagery* img,
                                      int32_t state, int32_t frame)
{
    if (!inst || !img || state < 0)
        return { state, frame };

    const int32_t statesize = inst->IsInInventory()
        ? img->GetInvAniLength(state)
        : img->GetAniLength(state);
    const int32_t stateflags = inst->IsInInventory()
        ? img->GetInvAniFlags(state)
        : img->GetAniFlags(state);
    const int32_t rate = inst->GetFrameRate();

    if (statesize <= 1 || rate == 0)
        return { state, frame };

    const int32_t next = frame + rate;
    if (rate < 0)
    {
        if (next < 0)
        {
            if (stateflags & AF_LOOPING)
                return { state, statesize - 1 };
            if (stateflags & AF_PINGPONG)
                return { state, statesize > 1 ? 1 : 0 };
            return { state, 0 };
        }
        return { state, next };
    }

    if (next >= statesize)
    {
        if (stateflags & AF_LOOPING)
            return { state, 0 };
        if (stateflags & AF_PINGPONG)
            return { state, statesize - 1 };

        // Non-looping retail movement steps often chain into the next step by
        // state tags. The final frame still owns a full 1/24s interval; when
        // an authored continuation exists, render that interval toward frame 0
        // of the continuation instead of holding the last key.
        if (!inst->IsInInventory())
        {
            const int32_t continuation = FindTaggedContinuationState(img, state);
            if (continuation >= 0)
                return { continuation, 0 };
        }

        return { state, statesize - 1 };
    }
    return { state, next };
}

void T3DAnimator::UpdateLegacyTransitionWindow(T3DImagery* img,
                                               int32_t state,
                                               int32_t frame,
                                               int32_t prevstate,
                                               int32_t prevframe)
{
    const bool transition_changed =
        state != pose_transition_state ||
        prevstate != pose_transition_prev_state ||
        prevframe != pose_transition_prev_frame;

    if (transition_changed)
    {
        pose_transition_state = state;
        pose_transition_prev_state = prevstate;
        pose_transition_prev_frame = prevframe;
        pose_transition_last_frame = frame;
        pose_transition_highest_frame = frame;
        pose_transition_active =
            img &&
            state >= 0 && state < img->NumStates() &&
            prevstate >= 0 && prevstate < img->NumStates();
        return;
    }

    if (frame > pose_transition_highest_frame)
        pose_transition_highest_frame = frame;

    const bool wrapped_loop =
        img &&
        state >= 0 && state < img->NumStates() &&
        (img->GetAniFlags(state) & AF_LOOPING) &&
        pose_transition_last_frame >= 0 &&
        frame < pose_transition_last_frame;

    if (wrapped_loop ||
        pose_transition_highest_frame >= kLegacyTransitionBlendFrames)
        pose_transition_active = false;

    pose_transition_last_frame = frame;
}

void T3DAnimator::UpdateBoneTransforms()
{
    if (!inst) return;
    T3DImagery* img = Get3DImagery();
    if (!img) return;

    // Skeletons are stable for a given I3D imagery. If code ever swaps the
    // imagery under a live animator, resize here once for that structural
    // change; steady-state frames must only reuse the existing buffers.
    const int32_t expected_bones = img->NumObjects();
    if (pose_layout.bone_count != expected_bones ||
        pose_current.Count() != pose_layout.channel_count)
    {
        ConfigureI3DLegacyPoseLayout(img, pose_layout);
        pose_current.Configure(pose_layout);
        pose_next.Configure(pose_layout);
        pose_blended.Configure(pose_layout);
        pose_update_render_frame = -1;
    }
    if (!pose_layout.IsValid())
        return;

    const int32_t s = inst->GetState();
    const int32_t f = inst->GetFrame();
    const SLegacyRenderKey nextkey = NextRenderKey(inst, img, s, f);
    const float framefrac = (float)TTime::LegacyFrameFraction();
    const int64_t render_frame = TTime::FrameCount();
    const int32_t prevstate = inst->GetPrevState();
    const int32_t prevframe = inst->GetPrevFrame();
    UpdateLegacyTransitionWindow(img, s, f, prevstate, prevframe);
    const bool use_transition_blend =
        pose_transition_active && f < kLegacyTransitionBlendFrames;
    const int32_t sample_prevstate = use_transition_blend ? prevstate : -1;
    const int32_t sample_prevframe = use_transition_blend ? prevframe : 0;

    if (pose_update_render_frame == render_frame &&
        pose_update_state == s &&
        pose_update_frame == f &&
        pose_update_next_state == nextkey.state &&
        pose_update_next_frame == nextkey.frame &&
        pose_update_prev_state == sample_prevstate &&
        pose_update_prev_frame == sample_prevframe &&
        pose_update_frame_frac == framefrac)
        return;

    const float* pose = nullptr;
    if (!SampleI3DLegacyFrameBlendToPose(img, pose_layout, s, f,
            nextkey.state, nextkey.frame,
            framefrac, sample_prevstate, sample_prevframe,
            pose_current.Data(), pose_next.Data(), pose_blended.Data(),
            pose_current.Count(), &pose) || !pose)
        return;

    for (int32_t c = 0; c < animobjs.NumItems(); c++)
    {
        S3DAnimObj* obj = animobjs[c];
        if (!obj) continue;

        const bool hastrans =
            (obj->flags & (OBJ3D_ROTMASK | OBJ3D_POSMASK | OBJ3D_SCLMASK)) != 0;
        if (hastrans || (obj->flags & OBJ3D_ADDTOANI))
        {
            // Quarantined legacy matrix path for the small set of objects that
            // layer authored masks/transforms over animation. The ordinary bone
            // path below is fixed-buffer pose data only.
            img->SetPrevState(sample_prevstate, sample_prevframe);
            img->CalcObjectMatrix(obj, s, f, /*pos*/nullptr, /*calcparents*/false);
            continue;
        }

        const int32_t target = (obj->flags & OBJ3D_ANIMTRACK)
            ? obj->animtrack
            : obj->objnum;
        const int32_t base = pose_layout.BoneOffset(target);
        if (base < 0 || base + ANIM_BONE_STRIDE > pose_layout.channel_count)
            continue;

        obj->pos = {
            pose[base + ANIM_BONE_POS_X],
            pose[base + ANIM_BONE_POS_Y],
            pose[base + ANIM_BONE_POS_Z],
        };
        obj->scl = {
            pose[base + ANIM_BONE_SCALE_X],
            pose[base + ANIM_BONE_SCALE_Y],
            pose[base + ANIM_BONE_SCALE_Z],
        };
        const SAnimQuat q = {
            pose[base + ANIM_BONE_ROT_X],
            pose[base + ANIM_BONE_ROT_Y],
            pose[base + ANIM_BONE_ROT_Z],
            pose[base + ANIM_BONE_ROT_W],
        };
        obj->transform.SetLocalPos(obj->pos);
        obj->transform.SetLocalRot(q);
        obj->transform.SetLocalScl(obj->scl);

        // Keep the old per-bone matrix mirror alive for attachment/render
        // paths that have not moved to TTransform yet. This is composition of
        // already-written local transforms, not another legacy key sample.
        obj->matrix = obj->transform.LocalMatrix();
        if (obj->parent)
            MtxMultiply(&obj->matrix, &obj->matrix, &obj->parent->matrix);
    }

    // Single top-down sweep on the instance's transform resolves all
    // bone globals in dependency order. Cheap because by here every
    // dirty bone's local matrix is already cached.
    inst->Transform().RefreshHierarchy();
    pose_update_render_frame = render_frame;
    pose_update_state = s;
    pose_update_frame = f;
    pose_update_next_state = nextkey.state;
    pose_update_next_frame = nextkey.frame;
    pose_update_prev_state = sample_prevstate;
    pose_update_prev_frame = sample_prevframe;
    pose_update_frame_frac = framefrac;
}

void T3DAnimator::Animate(bool draw)
{
    TObjectAnimator::Animate(draw);

    // Per-frame: refresh the bone hierarchy so consumers (renderer,
    // attachment lookup, etc.) can read up-to-date world matrices off
    // bone.transform.Matrix(). Runs before the legacy pos/rot mirror
    // below so anything reading inst->Pos() in this frame gets the
    // pose-aware bone state.
    UpdateBoneTransforms();

    S3DPoint p;
    inst->GetPos(p);

    changed = false;
    if (pos.X != (float)p.x || pos.Y != (float)p.y || pos.Z != FIX_Z_VALUE(p.z))
    {
        pos.X = (float)p.x;
        pos.Y = (float)p.y;
        pos.Z = FIX_Z_VALUE(p.z);
        changed = true;
    }

    float newrotz = inst->GetFace() / 256.0f * (float)(M_PI * 2.0);
    if (fabsf(rot.Z - newrotz) > 0.0001f)
    {
        rot.Z = newrotz;
        changed = true;
    }

    if ((flags & ANI3D_UPDATEBOUNDRECT) && inst->CommandDone())
        AnimateResetBoundRect();
}

bool T3DAnimator::SurfacesLost()
{
    return ((T3DImagery*)image)->SurfacesLost();
}

void T3DAnimator::RestoreSurfaces()
{
    ((T3DImagery*)image)->RestoreSurfaces();
}

void T3DAnimator::MakeMatrix(hmm_mat4* m)
{
    hmm_mat4 m2;
    MtxClear(m);
    MtxRotateZ(m, rot.Z);
    MtxClear(&m2); MtxRotateX(&m2, rot.X); MtxMultiply(m, m, &m2);
    MtxClear(&m2); MtxRotateY(&m2, rot.Y); MtxMultiply(m, m, &m2);
    MtxClear(&m2); MtxTranslate(&m2, &pos); MtxMultiply(m, m, &m2);
}

void T3DAnimator::PreRender()
{
    S3DPoint curpos;
    inst->GetPos(curpos);
    Scene3D.LightAffectObject(curpos.x, curpos.y, curpos.z + LIGHTINGCHARHEIGHT);

    if (changed)
        MakeMatrix(&matrix);

    ((T3DImagery*)image)->SetPrevState(inst->GetPrevState(), inst->GetPrevFrame());
    ((T3DImagery*)image)->BeginRender(false);

    ResetExtents();
}

bool T3DAnimator::Render()
{
    S3DAnimObj* objstack[MAXOBJECTS];
    int32_t stackpos;
    int32_t c;

    for (c = 0; c < animobjs.NumItems(); c++)
    {
        if (!(animobjs[c]->flags & OBJ3D_PARENT))
        {
            int32_t parent = ((T3DImagery*)image)->GetObjectParent(animobjs[c]->objnum, state);
            if ((uint32_t)parent < (uint32_t)animobjs.NumItems() &&
                animobjs[parent]->objnum == parent)
                animobjs[c]->parent = animobjs[parent];
            else
                animobjs[c]->parent = nullptr;
        }
    }

    for (int32_t t = 0; t < Get3DImagery()->NumTextures() + 1; t++)
    {
        for (c = 0; c < animobjs.NumItems(); c++)
            animobjs[c]->flags &= ~OBJ3D_RENDERED;

        for (c = 0; c < animobjs.NumItems(); c++)
        {
            if (!animobjs[c] || (animobjs[c]->flags & OBJ3D_RENDERED))
                continue;

            objstack[0] = animobjs[c];
            stackpos = 1;

            S3DAnimObj* parent = animobjs[c]->parent;
            while (parent && !(parent->flags & OBJ3D_RENDERED) && stackpos < MAXOBJECTS)
            {
                objstack[stackpos++] = parent;
                parent = parent->parent;
            }

            while (stackpos)
            {
                stackpos--;
                S3DAnimObj* obj = objstack[stackpos];

                if (Get3DImagery()->IsHidden(obj->objnum, state))
                    obj->flags |= OBJ3D_HIDE;

                Get3DImagery()->RenderObject(obj, state, frame, &matrix, t, t > 0);

                obj->flags |= OBJ3D_RENDERED;
            }
        }
    }

    return true;
}

void T3DAnimator::PostRender()
{
    ((T3DImagery*)image)->EndRender();

    if (!WasUpdated())
        UpdateExtents();

    Scene3D.ResetAllLights();
}

// *****************************
// * Animator Object Functions *
// *****************************

void T3DAnimator::SetupObjects()
{
    for (int32_t c = 0; c < Get3DImagery()->NumObjects(); c++)
    {
        S3DAnimObj* o = NewObject(c);
        AddObject(o);
    }

    // Wire the bone hierarchy. Both linkages are established:
    //   * Legacy `parent` raw pointer (still consumed by
    //     CalcObjectMatrix's manual parent-chain compose) -- stays
    //     nullptr at the imagery's roots so the legacy chain
    //     terminates correctly.
    //   * TTransform parent linkage. In-tree parents go to the
    //     parent bone's transform; root bones (no in-tree parent)
    //     are parented under the OWNING object's transform_ so
    //     bone.transform.Matrix() yields a world-space matrix
    //     directly. Lifetime is safe: bones live in the animator
    //     component, which is destroyed before the instance's own
    //     transform_ member, so each bone's dtor runs while
    //     inst->Transform() is still alive and detaches itself
    //     from the children list cleanly.
    for (int32_t c = 0; c < animobjs.NumItems(); c++)
    {
        const int32_t parent = ((T3DImagery*)image)
                               ->GetObjectParent(animobjs[c]->objnum, 0);
        if ((uint32_t)parent < (uint32_t)animobjs.NumItems() &&
            animobjs[parent]->objnum == parent)
        {
            animobjs[c]->parent = animobjs[parent];
            animobjs[c]->transform.SetParent(&animobjs[parent]->transform);
        }
        else
        {
            animobjs[c]->parent = nullptr;
            animobjs[c]->transform.SetParent(inst ? &inst->Transform() : nullptr);
        }
    }
}

S3DAnimObj* T3DAnimator::NewObject(int32_t objnum, int32_t newflags)
{
    // Default member initializers in S3DAnimObj zero everything that
    // needs zeroing (and identity-initialize scl + transform). The
    // legacy memset was wrong for embedded class members like
    // TTransform anyway -- it would clobber the registry-registered
    // state and the children vector.
    S3DAnimObj* obj = new S3DAnimObj;
    obj->objnum = objnum;
    obj->parent = nullptr;

    S3DObj o;
    Get3DImagery()->GetObject(objnum, &o);

    int32_t n = Get3DImagery()->NumObjects() - 1;
    obj->animtrack = objnum < n ? objnum : n;
    obj->primtype  = ERender3DPrim::TriangleList;
    obj->verttype  = ERender3DVertex::Vertex;
    obj->hmaterial = Get3DImagery()->GetMaterialHandle(o.material);
    for (int32_t c = 0; c < MAXTEXTURES; c++)
        obj->htextures[c] = Get3DImagery()->GetTextureHandle(c);

    if (newflags & OBJ3D_COPYVERTS)
        GetVerts(obj, obj->verttype);
    if (newflags & OBJ3D_COPYFACES)
        GetFaces(obj);

    return obj;
}

void T3DAnimator::GetVerts(S3DAnimObj* obj, ERender3DVertex verttype)
{
    if (obj->verts)
        FreeVerts(obj);

    obj->flags |= (OBJ3D_VERTS | OBJ3D_COPYVERTS | OBJ3D_OWNSVERTS);
    obj->verttype = verttype;

    obj->numverts = Get3DImagery()->NumObjVerts(obj->objnum);
    if (verttype == ERender3DVertex::Vertex)
        obj->verts = new S3DVertex[obj->numverts];
    else if (verttype == ERender3DVertex::LitVertex)
        obj->verts = new S3DLVertex[obj->numverts];
    else if (verttype == ERender3DVertex::TLVertex)
        obj->verts = new S3DTLVertex[obj->numverts];
    Get3DImagery()->GetObjVerts(obj->objnum, obj->verts, 0, 0, verttype);
}

void T3DAnimator::FreeVerts(S3DAnimObj* obj)
{
    if (obj->verts && (obj->flags & OBJ3D_OWNSVERTS))
        // vert buffer layout depends on verttype; just free the raw bytes.
        delete[] (uint8_t*)obj->verts;

    obj->flags &= ~(OBJ3D_VERTS | OBJ3D_COPYVERTS | OBJ3D_OWNSVERTS);
    obj->verts = nullptr;
    obj->numverts = 0;
}

void T3DAnimator::GetFaces(S3DAnimObj* obj)
{
    if (obj->faces)
        FreeFaces(obj);

    obj->flags |= (OBJ3D_FACES | OBJ3D_COPYFACES | OBJ3D_OWNSFACES);

    obj->numfaces = Get3DImagery()->NumObjFaces(obj->objnum);
    obj->faces = new S3DFace[obj->numfaces];
    Get3DImagery()->GetObjFaces(obj->objnum, obj->faces);
}

void T3DAnimator::FreeFaces(S3DAnimObj* obj)
{
    if (obj->faces && (obj->flags & OBJ3D_OWNSFACES))
        delete obj->faces;

    obj->flags &= ~(OBJ3D_FACES | OBJ3D_COPYFACES | OBJ3D_OWNSFACES);
    obj->faces = nullptr;
    obj->numfaces = 0;
}

int32_t T3DAnimator::AddObject(S3DAnimObj* obj)
{
    return animobjs.Add(obj);
}

void T3DAnimator::RemoveObject(int32_t objnum)
{
    if (objnum < 0 || objnum >= MAX3DANIMOBJECTS || animobjs[objnum] == nullptr)
        return;

    if ((animobjs[objnum]->flags & OBJ3D_OWNSVERTS) && animobjs[objnum]->verts)
        delete[] (uint8_t*)animobjs[objnum]->verts;

    if ((animobjs[objnum]->flags & OBJ3D_OWNSFACES) && animobjs[objnum]->faces)
        delete animobjs[objnum]->faces;

    animobjs.Delete(objnum);
}

int32_t T3DAnimator::GetObjectNum(const char* name)
{
    for (int32_t c = 0; c < animobjs.NumItems(); c++)
    {
        if (animobjs[c] &&
            !stricmp(Get3DImagery()->GetObjectName(animobjs[c]->objnum), name))
            return c;
    }
    return -1;
}

S3DAnimObj* T3DAnimator::GetObject(int32_t objnum)
{
    if (objnum < 0 || objnum >= MAX3DANIMOBJECTS || animobjs[objnum] == nullptr)
        return nullptr;
    return animobjs[objnum];
}

bool T3DAnimator::IsObjectEnabled(int32_t objnum)
{
    if (objnum < 0 || objnum >= MAX3DANIMOBJECTS || animobjs[objnum] == nullptr)
        return false;
    return !(animobjs[objnum]->flags & OBJ3D_HIDE);
}

void T3DAnimator::EnableObject(int32_t objnum, bool enable)
{
    if (objnum < 0 || objnum >= MAX3DANIMOBJECTS || animobjs[objnum] == nullptr)
        return;

    if (enable) animobjs[objnum]->flags &= ~OBJ3D_HIDE;
    else        animobjs[objnum]->flags |=  OBJ3D_HIDE;
}

bool T3DAnimator::GetObjectMatrix(int32_t objnum, hmm_mat4* m)
{
    if (objnum < 0 || !m)
        return false;

    S3DAnimObj* animobj = GetObject(objnum);
    if (!animobj)
        return false;

    // Attachment/effect callers historically receive a character-local bone
    // matrix and apply the character root themselves. Keep that contract, but
    // source the matrix from the current blended pose instead of resampling an
    // integer legacy frame through CalcObjectMatrix.
    UpdateBoneTransforms();
    *m = animobj->matrix;
    return true;
}

bool T3DAnimator::GetObjectPos(int32_t objnum, hmm_vec3& v, hmm_vec3* s)
{
    hmm_mat4 m;
    if (objnum < 0) return false;
    if (!GetObjectMatrix(objnum, &m)) return false;

    hmm_vec3 p;
    if (s) { p.X = s->X; p.Y = s->Y; p.Z = s->Z; }
    else   { p.X = p.Y = p.Z = 0.0f; }

    // Transform p by m → v. hmm_mat4 * vec3 via a 4-vec.
    hmm_vec4 p4 = {p.X, p.Y, p.Z, 1.0f};
    hmm_vec4 r4 = HMM_MultiplyMat4ByVec4(m, p4);
    v.X = r4.X; v.Y = r4.Y; v.Z = r4.Z;
    return true;
}

bool T3DAnimator::GetObjectPos(const char* objname, hmm_vec3& v, hmm_vec3* s)
{
    int32_t objnum = GetObjectNum(objname);
    if (objnum < 0) return false;
    return GetObjectPos(objnum, v, s);
}

bool T3DAnimator::GetObjectMapPos(int32_t objnum, S3DPoint& pos)
{
    hmm_vec3 v;
    if (!GetObjectPos(objnum, v)) return false;
    pos.x = (int32_t)v.X;
    pos.y = (int32_t)v.Y;
    pos.z = (int32_t)REV_FIX_Z_VALUE(v.Z);
    return true;
}

bool T3DAnimator::GetObjectMapPos(const char* objname, S3DPoint& pos)
{
    int32_t objnum = GetObjectNum(objname);
    if (objnum < 0) return false;
    return GetObjectMapPos(objnum, pos);
}
