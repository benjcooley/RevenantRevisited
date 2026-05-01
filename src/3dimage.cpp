// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   3dImage.cpp - 3D image module                       *
// *                                                                       *
// *  Ported (Phase 2): D3D3 texture surfaces + material handles + execute *
// *  buffers replaced by sokol-facing stubs. Animation-key decoders,      *
// *  matrix builders, tag/sound wiring, and mesh loading are unchanged    *
// *  portable logic. AddTexture / LoadTexture / RemoveTexture and         *
// *  AddMaterial are now #if 0 stubs — they'll grow real sg_image uploads *
// *  in Phase 3.                                                          *
// *************************************************************************

#include "3dimage.h"

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "3dscene.h"
#include "animation.h"
#include "bitmap.h"
#include "logging.h"
#include "mappane.h"
#include "math3d.h"
#include "parse.h"
#include "sound.h"

T3DAnimatorBuilder T3DAnimatorBuilderInstance;  // Register default builder

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
                obj.anikeys[d] = new SAniKey[GetAniLength(d)];
                obj.numanikeys[d] = GetAniLength(d);
                std::memcpy(obj.anikeys[d],
                    ((SAniKey*)mesh->anikeys[d].ptr()) + GetAniLength(d) * c,
                    sizeof(SAniKey) * GetAniLength(d));
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
                if (!icons)
                    icons = new S3DImageryIcons[NumStates()];
                std::memset(icons, 0, sizeof(S3DImageryIcons) * NumStates());

                uint8_t* icon = new uint8_t[im->invsize];
                if ((void*)im->invitem)
                {
                    std::memcpy(icon, im->invitem, im->invsize);
                    icons[c].invitem = (TBitmap*)icon;
                    Convert15to16((TBitmap*)icon);
                }
                else if ((void*)im->invanim)
                {
                    std::memcpy(icon, im->invanim, im->invsize);
                    icons[c].invanim = (TAnimation*)icon;
                    for (int32_t d = 0; d < ((TAnimation*)icon)->numframes; d++)
                    {
                        Convert15to16((TBitmap*)((TAnimation*)icon)->frames[d].bitmap);
                    }
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
                if (!icons)
                    icons = new S3DImageryIcons[NumStates()];
                std::memset(icons, 0, sizeof(S3DImageryIcons) * NumStates());

                uint8_t* icon = new uint8_t[im->invsize];
                if ((void*)im->invitem)
                {
                    std::memcpy(icon, im->invitem, im->invsize);
                    icons[c].invitem = (TBitmap*)icon;
                    Convert15to16((TBitmap*)icon);
                }
                else if ((void*)im->invanim)
                {
                    std::memcpy(icon, im->invanim, im->invsize);
                    icons[c].invanim = (TAnimation*)icon;
                    for (int32_t d = 0; d < ((TAnimation*)icon)->numframes; d++)
                    {
                        Convert15to16((TBitmap*)((TAnimation*)icon)->frames[d].bitmap);
                    }
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
    if (pos.X != 0.0f || pos.Y != 0.0f || pos.Z != 0.0f)
        MtxTranslate(&m, &pos);
    if (scl.X != 1.0f || scl.Y != 1.0f || scl.Z != 1.0f)
        MtxScale(&m, &scl);
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

                if ((num <= 0) || (curframe + frames >= frame))
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

#define INTERFRAMES 5

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

    int32_t iframes = INTERFRAMES;

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
            MakeMatrix(m, animobj->pos, animobj->rot, animobj->scl);
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
        sg_image       texsurface = {0};
        if (!hastextures || !UseTextures) { /* leave invalid */ }
        else if (animobj->flags & OBJ3D_TEX)
        {
            htexture   = animobj->htextures[t];
            texsurface = animobj->surfaces[t];
        }
        else if (t > 0)
        {
            htexture   = textures[t - 1].htexture;
            texsurface = textures[t - 1].surface;
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
            Scene3D.SetTexture(htexture, texsurface);

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

void T3DImagery::PlaySound(TObjectInstance* inst, int32_t state, int32_t frame)
{
    char* soundlist = FindTag((char*)"play", state, frame);
    if (soundlist)
    {
        if (!inst)
            PLAY(listrnd(soundlist));
        else
            inst->PlayWave(listrnd(soundlist));
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
    if (!LoadTexture(&tex, srcsd, pixels, frames, palette, false))
        return -1;
    return textures.Add(tex);
}

bool T3DImagery::LoadTexture(S3DTex* tex, SSurfaceDesc* srcsd,
    OFFSET* pixels, int32_t frames, void* palette, bool copyframes)
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
        tex->framesurfs = new sg_image[frames];
        tex->framehtexs = new TTextureHandle[frames];
        for (int32_t f = 0; f < frames; f++)
        {
            tex->framesurfs[f] = sg_image{0};
            tex->framehtexs[f] = kInvalidTexture;
        }
    }
    else
    {
        tex->framesurfs = nullptr;
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

        sg_image_desc id = {};
        id.width = (int)srcsd->width;
        id.height = (int)srcsd->height;
        id.pixel_format = SG_PIXELFORMAT_RGBA8;
        id.min_filter = SG_FILTER_LINEAR;
        id.mag_filter = SG_FILTER_LINEAR;
        id.data.subimage[0][0] = { rgba.data(), rgba.size() };
        id.label = "i3d.texture";
        sg_image img = sg_make_image(&id);

        if (frames > 1)
        {
            tex->framesurfs[f] = img;
            tex->framehtexs[f] = 0;
        }
        else
        {
            tex->surface = img;
            tex->htexture = 0;
        }
    }

    if (frames > 1)
    {
        tex->surface = tex->framesurfs[0];
        tex->htexture = tex->framehtexs[0];
    }

    return true;
}

void T3DImagery::RemoveTexture(int32_t texnum)
{
    S3DTex* t = &textures[texnum];
    const uint32_t active_id = t->surface.id;

    if (t->surface.id) sg_destroy_image(t->surface);
    t->surface  = sg_image{0};
    t->htexture = kInvalidTexture;

    if (t->framesurfs)
    {
        for (int32_t i = 0; i < t->numframes; ++i)
            if (t->framesurfs[i].id && t->framesurfs[i].id != active_id)
                sg_destroy_image(t->framesurfs[i]);
        delete[] t->framesurfs;
        delete[] t->framehtexs;
        t->framesurfs = nullptr;
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
        textures[texnum].surface  = textures[texnum].framesurfs[framenum];
        textures[texnum].htexture = textures[texnum].framehtexs[framenum];
        textures[texnum].framenum = framenum;
        return true;
    }

#if 0 // TODO(port): copy frame pixels into the active sg_image — Phase 3
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

#if 0 // TODO(port): Display->AddUpdateRect lost its method in the TDisplay
      // refactor; add the rect through whatever the new dirty-rect API is — Phase 3.
    if (!NoUpdateRects)
        Display->AddUpdateRect(extents->x1, extents->y1,
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

TBitmap* T3DImagery::GetInvImage(int32_t state, int32_t /*num*/)
{
    if (!meshinitialized)
        if (!InitializeMesh((S3DImageryBody*)GetBody()))
            FatalError("Unable to initialize 3D imagery");

    if (!icons)
        return nullptr;
    return icons[state].invitem;
}

TAnimation* T3DImagery::GetInvAnimation(int32_t state)
{
    if (!meshinitialized)
        InitializeMesh((S3DImageryBody*)GetBody());

    if (!icons)
        return nullptr;
    return icons[state].invanim;
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

    return (TObjectAnimator*)builder->Build(oi);
}

bool T3DImagery::NeedsAnimator(TObjectInstance*)
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

    animid = Scene3D.AddAnimator(this);
}

void T3DAnimator::Close()
{
    Scene3D.RemoveAnimator(animid);

    for (int32_t c = 0; c < animobjs.NumItems(); c++)
        RemoveObject(c);

    animobjs.DeleteAll();

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
    ((T3DImagery*)image)->PlaySound(inst, state, frame);
}

void T3DAnimator::Animate(bool draw)
{
    TObjectAnimator::Animate(draw);

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

    for (int32_t c = 0; c < animobjs.NumItems(); c++)
    {
        int32_t parent = ((T3DImagery*)image)->GetObjectParent(animobjs[c]->objnum, 0);
        if ((uint32_t)parent < (uint32_t)animobjs.NumItems() &&
            animobjs[parent]->objnum == parent)
            animobjs[c]->parent = animobjs[parent];
        else
            animobjs[c]->parent = nullptr;
    }
}

S3DAnimObj* T3DAnimator::NewObject(int32_t objnum, int32_t newflags)
{
    S3DAnimObj* obj = new S3DAnimObj;
    std::memset(obj, 0, sizeof(S3DAnimObj));
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
    {
        obj->htextures[c] = Get3DImagery()->GetTextureHandle(c);
        S3DTex tex;
        Get3DImagery()->GetTexture(c, &tex);
        obj->surfaces[c] = tex.surface;
    }

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
    if (objnum < 0)
        return false;

    S3DAnimObj* animobj = GetObject(objnum);
    Get3DImagery()->CalcObjectMatrixCopy(animobj, inst->GetState(), inst->GetFrame(), m, 0);
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
