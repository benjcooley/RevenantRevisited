// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   3dImage.cpp - 3D image module                       *
// *************************************************************************

#include "3dimage.h"

#include "3dscene.h"
#include "animation.h"
#include "bitmap.h"
#include "complexobj.h"
#include "display.h"
#include "mappane.h"
#include "multi.h"
#include "parse.h"
#include "playscreen.h"
#include "sound.h"

T3DAnimatorBuilder T3DAnimatorBuilderInstance;  // Register default builder

// **********************
// * 3DImagery Funtions *
// **********************

REGISTER_IMAGERYBUILDER(T3DImagery);

T3DImagery::T3DImagery(int32_t imageryid) : TObjectImagery(imageryid)
{
    meshinitialized = false;
    executebuf = nullptr;
    cacheexbuf = false;
}

T3DImagery::~T3DImagery()
{
    ClearMesh();
}

// ****************** DIRECT3D IMAGERY STUFF **********************

void replace(char *buf, char *str, char *rep)
{
    char save[RESNAMELEN];
    char *ptr = strstr(buf, str);
    if (!ptr)
        return;

    int32_t pos = (int32_t)((uint32_t)ptr - (uint32_t)buf);
    int32_t slen = strlen(str);
    int32_t rlen = strlen(rep);
    strcpy(save, buf);
    strcpy(buf + pos, rep);
    strcpy(buf + pos + rlen, save + pos + slen);
}

bool T3DImagery::OldInitializeMesh(PSOld3DImageryBody mesh)
{
    int32_t c;

    if (meshinitialized)
        return true;
    meshinitialized = true;

    if (executebuf)
    {
        delete executebuf;
        executebuf = nullptr;
    }

    /*
     * General Mesh Data
     */

    flags = mesh->flags;
    version = 0;

//  /*
//   * Fix names hack-o-matic
//   */
//
//  for (c = 0; c < NumStates(); c++)
//  {
//      replace(GetHeader()->states[c].animname, "stand", "walk");
//      replace(GetHeader()->states[c].animname, "swing", "attack1");
//      replace(GetHeader()->states[c].animname, "thrust", "attack2");
//  }

    /*
     * Setup morph vertice lists
     */

    numverts = mesh->numverts;

    int32_t vertstates;
    if (!(flags & I3D_ISMORPH))
        vertstates = 1;
    else
        vertstates = NumStates();

    verts = new LPD3DVERTEX*[vertstates];

    for (c = 0; c < vertstates; c++)
    {
        int32_t vertframes;
        if (!(flags & I3D_ISMORPH))
            vertframes = 1;
        else
            vertframes = GetAniLength(c);

        verts[c] = new LPD3DVERTEX[vertframes];
        for (int32_t fr = 0; fr < vertframes; fr++)
        {
            verts[c][fr] = new hmm_vec3[numverts];
            OFFSET *meshverts = (OFFSET *)((void *)mesh->verts[c]);
            memcpy(verts[c][fr], (void *)meshverts[fr], sizeof(hmm_vec3) * numverts);
        }
    }

    /*
     * Setup faces lists
     */

    numfaces = mesh->numfaces;
    faces = (S3DFace *)new uint16_t[mesh->numfaces * 3];
    memcpy(faces, (void *)mesh->faces, sizeof(uint16_t) * mesh->numfaces * 3);

    /*
     * Setup objects
     */

    // sanity checking
    if (mesh->numobjects > 5000 || mesh->numobjects <= 0)
        return false;

    for (c = 0; c < mesh->numobjects; c++)
    {
        S3DObj obj;
        int32_t d;

        memcpy(obj.name, mesh->objname[c], RESNAMELEN);
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
            unsigned char *objparent = (unsigned char *)mesh->objparent[c].ptr();
            for (int32_t d = 0; d < NumStates(); d++)
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
                obj.anikeys[d] = nullptr;              // This is a hidden object!!
            }
            else
            {
                obj.anikeys[d] = new SAniKey[GetAniLength(d)];
                obj.numanikeys[d] = GetAniLength(d);
                memcpy(obj.anikeys[d], 
                    ((PSAniKey)mesh->anikeys[d].ptr()) + GetAniLength(d) * c,
                    sizeof(SAniKey) * GetAniLength(d));
            }
        }

        AddObject(&obj);
    }

    /*
     * Setup animation keys and motion data
     */

    motion = new SMotionData*[NumStates()];
    memset(motion, 0, sizeof(SMotionData*) * NumStates());
    if (!(flags & I3D_ISMORPH))
    {
      for (c = 0; c < NumStates(); c++)
      {
        if (mesh->motion[c] == nullptr)
        {
            motion[c] = nullptr;
            GetHeader()->states[c].aniflags |= AF_NOMOTION; // Make sure this flag is set
        }
        else
        {
            motion[c] = new SMotionData[GetAniLength(c)];

          // Convert old motion data format to new format
            PSOldOldMotionData omd = (PSOldOldMotionData)(mesh->motion[c].ptr());
            PSMotionData nmd = motion[c];
            for (int32_t d = 0; d < GetAniLength(c); d++, omd++, nmd++)
            {
                nmd->dist = (omd->dist) >> 8; // Old dist was 24 bit, convert to 16
                nmd->vert = 0;
                nmd->ang = omd->ang;
                nmd->rotx = nmd->roty = 0;
                nmd->rotz = omd->ang;
            }
        }
      }
    }

    /*
     * Setup textures
     */

    hastextures = mesh->numtextures != 0;
    textures.Clear();
    if (UseTextures)
    {
        for (c = 0; c < mesh->numtextures; c++)
        {
            AddTexture(
                &mesh->texturedesc[c],
                (OFFSET *)((void *)mesh->texturebits[c]),
                mesh->textureframes[c],
                (LPPALETTEENTRY)(void *)mesh->texturepals[c]);
        }
    }

    /*
     * Setup a materials
     */

    materials.Clear();
    for (c = 0; c < mesh->nummaterials; c++)
    {
        AddMaterial(&mesh->material[c], (int32_t)mesh->material[c].hTexture);
    }

    /*
     * Setup icons
     */

    icons = nullptr;
    if (flags & I3D_HASICONS)
    {
        for (c = 0; c < NumStates(); c++)
        {
            PS3DStateImagery im = (PS3DStateImagery)((void *)mesh->imagery[c]);
            if (!im)
                continue;

            if (im->invsize > 0)
            {
                if (!icons)
                    icons = new S3DImageryIcons[NumStates()];
                memset(icons, 0, sizeof(S3DImageryIcons) * NumStates());

                uint8_t *icon = new uint8_t[im->invsize];
                if ((void *)im->invitem)
                {
                    memcpy(icon, im->invitem, im->invsize);
                    icons[c].invitem = (PTBitmap)icon;
                    Convert15to16((PTBitmap)icon);
                }
                else if ((void *)im->invanim)
                {
                    memcpy(icon, im->invanim, im->invsize);
                    icons[c].invanim = (PTAnimation)icon;
                    for (int32_t d = 0; d < ((PTAnimation)icon)->numframes; d++)
                    {
                        Convert15to16((PTBitmap)((PTAnimation)icon)->frames[d].bitmap);
                    }
                }
            }
        }
    }

  // Clear Tags
    tags.Clear();

    FreeBody(); // Causes the body resource object to be deleted

    return true;
}

bool T3DImagery::InitializeMesh(PS3DImageryBody mesh)
{
    int32_t c;

    if (meshinitialized)
        return true;

    if (!(mesh->flags & I3D_3DIMAGEBODY2))
        return OldInitializeMesh((PSOld3DImageryBody)mesh);

    meshinitialized = true;

    if (executebuf)
    {
        delete executebuf;
        executebuf = nullptr;
    }

    /*
     * General Mesh Data
     */

    flags = mesh->flags;
    version = mesh->version;

    /*
     * Fix names hack-o-matic
     */

//  for (c = 0; c < NumStates(); c++)
//  {
//      replace(GetHeader()->states[c].animname, "stand", "walk");
//      replace(GetHeader()->states[c].animname, "swing", "attack1");
//      replace(GetHeader()->states[c].animname, "thrust", "attack2");
//  }

    /*
     * Setup morph vertice lists
     */

    numverts = mesh->numverts;

    int32_t vertstates;
    if (!(flags & I3D_ISMORPH))
        vertstates = 1;
    else
        vertstates = NumStates();

    verts = new LPD3DVERTEX*[vertstates];

    for (c = 0; c < vertstates; c++)
    {
        int32_t vertframes;
        if (!(flags & I3D_ISMORPH))
            vertframes = 1;
        else
            vertframes = GetAniLength(c);

        verts[c] = new LPD3DVERTEX[vertframes];
        for (int32_t fr = 0; fr < vertframes; fr++)
        {
            verts[c][fr] = new hmm_vec3[numverts];
            LPD3DVERTEX meshverts = mesh->verts[c][fr];
            memcpy(verts[c][fr], meshverts, sizeof(hmm_vec3) * numverts);
        }
    }

    /*
     * Setup faces lists
     */

    numfaces = mesh->numfaces;
    faces = (S3DFace *)new uint16_t[mesh->numfaces * 3];
    memcpy(faces, (void *)mesh->faces, sizeof(uint16_t) * mesh->numfaces * 3);

    /*
     * Setup objects
     */

    // sanity checking
    if (mesh->numobjects > 5000 || mesh->numobjects <= 0)
        return false;

  // Go through all objects
    for (c = 0; c < mesh->numobjects; c++)
    {
        S3DObj obj;
        int32_t d;

      // Setup vertex and face list stuff
        memcpy(obj.name, mesh->objects[c].name, RESNAMELEN);
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

      // Get parent indexes for each object for each state
        obj.parent = new int32_t[NumStates()];
        if (mesh->flags & I3D_HASHIERARCHY)
        {
            for (int32_t d = 0; d < NumStates(); d++)
            {
                if (mesh->flags & I3D_ANIKEY32) // New objstate structure format for anikey32
                    obj.parent[d] = mesh->objects[c].states[d].parent; // use new structure
                else
                {
                    PS3DOldImageryObjectState st = 
                        (PS3DOldImageryObjectState)mesh->objects[c].states.ptr();
                    obj.parent[d] = st[d].parent;
                }
            }
        }
        else    // No parent structure saved.. just do parentless
        {
            for (d = 0; d < NumStates(); d++)
                obj.parent[d] = -1;
        }

        obj.anikeys = new void*[NumStates()];
        obj.numanikeys = new int32_t[NumStates()];
        for (d = 0; d < NumStates(); d++)
        {

            if (mesh->flags & I3D_ANIKEY32)  // New anikey32 format
            {
                PS3DImageryObjectState st = &(mesh->objects[c].states[d]);

                if (st->anikeys.ptr() == nullptr)
                {
                    obj.anikeys[d] = nullptr;              // This is a hidden object!!
                    obj.numanikeys[d] = 0;
                }
                else                                    // Allocate NEW anikey format
                {
                    obj.anikeys[d] = new SAniKey32[st->numanikeys];
                    obj.numanikeys[d] = st->numanikeys;
                    memcpy(obj.anikeys[d], st->anikeys.ptr(),
                        sizeof(SAniKey32) * st->numanikeys);
                    PSAniKey32 keys = (PSAniKey32)st->anikeys.ptr();
                    int32_t q = 3;
                }
            }
            else                            // Old anikey format
            {
                PS3DOldImageryObjectState st = 
                    &(((PS3DOldImageryObjectState)(mesh->objects[c].states.ptr()))[d]);

                if (st->anikeys.ptr() == nullptr)
                {
                    obj.anikeys[d] = nullptr;              // This is a hidden object!!
                    obj.numanikeys[d] = 0;
                }
                else                                    // Allocate OLD anikey format
                {
                    obj.anikeys[d] = new SAniKey[GetAniLength(d)];
                    obj.numanikeys[d] = GetAniLength(d);
                    memcpy(obj.anikeys[d], st->anikeys.ptr(),
                        sizeof(SAniKey) * GetAniLength(d));
                }
            }
        }

        AddObject(&obj);
    }

    /*
     * Setup animation keys and motion data
     */

    motion = new SMotionData*[NumStates()];
    memset(motion, 0, sizeof(SMotionData*) * NumStates());
    if (!(flags & I3D_ISMORPH))
    {
        for (c = 0; c < NumStates(); c++)
        {
            PS3DImageryState ist;
            if (mesh->version < 2)
                ist = (PS3DImageryState)&(((PS3DOldImageryState2)(mesh->statedata.ptr()))[c]);
            else if (mesh->version == 2)
                ist = (PS3DImageryState)&(((PS3DOldImageryState3)(mesh->statedata.ptr()))[c]);
            else
                ist = &(mesh->statedata[c]);

            if (ist->motion == nullptr)
            {
                motion[c] = nullptr;
                GetHeader()->states[c].aniflags |= AF_NOMOTION; // Make sure this flag is set
            }
            else
            {
                motion[c] = new SMotionData[GetAniLength(c)];

              // Current motion data includes vert, and rotx,roty,rotz
                if (mesh->flags & I3D_ROOTMOTION)
                    memcpy(motion[c], ist->motion.ptr(), 
                        sizeof(SMotionData) * GetAniLength(c));
                else if (mesh->flags & I3D_FACINGMOTION) // Old facing data (no vert,rotx,roty)
                {
                  // Convert old motion data format to new format
                    PSOldMotionData omd = (PSOldMotionData)(ist->motion.ptr());
                    PSMotionData nmd = motion[c];
                    for (int32_t d = 0; d < GetAniLength(c); d++, omd++, nmd++)
                    {
                        nmd->dist = omd->dist;
                        nmd->vert = 0;
                        nmd->ang = omd->ang;
                        nmd->rotx = nmd->roty = 0;
                        nmd->rotz = omd->face;
                    }
                }
                else // Very old facing data (no facing motion data)
                {
                  // Convert old motion data format to new format
                    PSOldOldMotionData omd = (PSOldOldMotionData)(ist->motion.ptr());
                    PSMotionData nmd = motion[c];
                    for (int32_t d = 0; d < GetAniLength(c); d++, omd++, nmd++)
                    {
                        nmd->dist = (omd->dist) >> 8; // Uses 24 bits, convert to 16
                        nmd->vert = 0;
                        nmd->ang = omd->ang;
                        nmd->rotx = nmd->roty = 0;
                        nmd->rotz = omd->ang;
                    }
                }
            }
        }
    }

    /*
     * Setup textures
     */

    hastextures = mesh->numtextures != 0;
    textures.Clear();
    if (UseTextures)
    {
        for (c = 0; c < mesh->numtextures; c++)
        {
            AddTexture(
                &mesh->textures[c].desc,
                (OFFSET *)((void *)mesh->textures[c].bits),
                mesh->textures[c].frames,
                (LPPALETTEENTRY)(void *)mesh->textures[c].pals);
        }
    }

    /*
     * Setup a materials
     */

    materials.Clear();
    for (c = 0; c < mesh->nummaterials; c++)
    {
        AddMaterial(&mesh->materials[c], (int32_t)mesh->materials[c].hTexture);
    }

    /*
     * Setup icons
     */

    icons = nullptr;
    if (flags & I3D_HASICONS)
    {
        for (c = 0; c < NumStates(); c++)
        {
            PS3DImageryState im;
            if (mesh->version < 2)
                im = (PS3DImageryState)&(((PS3DOldImageryState2)(mesh->statedata.ptr()))[c]);
            else if (mesh->version == 2)
                im = (PS3DImageryState)&(((PS3DOldImageryState3)(mesh->statedata.ptr()))[c]);
            else
                im = &(mesh->statedata[c]);

            if (im->invsize > 0)
            {
                if (!icons)
                    icons = new S3DImageryIcons[NumStates()];
                memset(icons, 0, sizeof(S3DImageryIcons) * NumStates());

                uint8_t *icon = new uint8_t[im->invsize];
                if ((void *)im->invitem)
                {
                    memcpy(icon, im->invitem, im->invsize);
                    icons[c].invitem = (PTBitmap)icon;
                    Convert15to16((PTBitmap)icon);
                }
                else if ((void *)im->invanim)
                {
                    memcpy(icon, im->invanim, im->invsize);
                    icons[c].invanim = (PTAnimation)icon;
                    for (int32_t d = 0; d < ((PTAnimation)icon)->numframes; d++)
                    {
                        Convert15to16((PTBitmap)((PTAnimation)icon)->frames[d].bitmap);
                    }
                }
            }
        }
    }

    /*
     * Load animation tags
     */

  // Clear tags before loading old tag data
    tags.Clear();

  // Mesh only has playsound list
    if (mesh->version < 2 && (mesh->flags & I3D_HASPLAYSOUND))
    {
     // Playsounds used same struct position as tags do now
        PS3DImageryPlaySound ps = (PS3DImageryPlaySound)(mesh->tags.ptr());
        for (int32_t pspos = 0; pspos < mesh->numtags; pspos++, ps++)
        {
            tags.AddPtr(new S3DTag(ps->state, ps->frame, "play", ps->sounds));
        }
    }

  // Mesh has playsound and begstate/endstate
    else if (mesh->version == 2)
    {
        PS3DImageryPlaySound ps = (PS3DImageryPlaySound)mesh->tags.ptr();
        int32_t pspos = 0;

        for (c = 0; c < NumStates(); c++)
        {
            PS3DOldImageryState3 ist = &(((PS3DOldImageryState3)(mesh->statedata.ptr()))[c]);
        
          // Add any begstate, endstate tags
            if (ist->begstate[0])
                tags.AddPtr(new S3DTag(c, 0, "beg", ist->begstate));

            if (ist->endstate[0])
                tags.AddPtr(new S3DTag(c, 0, "end", ist->endstate));

          // Add whatever play tags are in old play list
            if (mesh->flags & I3D_HASPLAYSOUND)
            {
                while (pspos < mesh->numtags && ps->state == c)
                {
                    tags.AddPtr(new S3DTag(ps->state, ps->frame, "play", ps->sounds));
                    ps++;
                    pspos++;
                }
            }
        }
    }

  // Load tag list
    else if (mesh->version >= 3)
    {
        PS3DImageryTag tg = mesh->tags;
        for (c = 0; c < mesh->numtags; c++, tg++)
        {
            tags.AddPtr(new S3DTag(tg->state, tg->frame, 
                (char*)tg->name.ptr(), (char*)tg->str.ptr()));
        }
    }

    /*
     * Mount sounds
     */
    
  // Load playsounds for old mesh
    for (c = 0; c < tags.NumItems(); c++)
    {
        if (!stricmp(tags[c].name, "play"))
        {
            int32_t num = listnum(tags[c].name);  // Get number of sounds in list
            for (int32_t n = 0; n < num; n++)
                SoundPlayer.Mount(listget(tags[c].str, n));
        }
    }

    FreeBody(); // Causes the body resource object to be deleted

    return true;
}

void T3DImagery::ClearMesh()
{
    int32_t i;

    if (!meshinitialized)
        return;

  // Clear the execute buffer
    if (executebuf)
    {
        delete executebuf;
        executebuf = nullptr;
        cacheexbuf = false;
    }

  // Clear all objects
    ClearObjects();

  // Clear all textures
    ClearTextures();

  // Clear all materials
    ClearMaterials();

  // Delete animation keys (if not morph)
    for (i = 0; i < NumStates(); i++)
    {
        if (motion[i])
            delete motion[i];
    }
    delete motion;
    motion = nullptr;

  // Delete morph verts (if is morph)
    int32_t vstates;
    if (!(flags & I3D_ISMORPH))
        vstates = 1;
    else
        vstates = NumStates();
    for (i = 0; i < vstates; i++)
    {
        int32_t vframes;
        if (!(flags & I3D_ISMORPH))
            vframes = 1;
        else
            vframes = GetAniLength(i);
        for (int32_t j = 0; j < vframes; j++)
        {
            delete verts[i][j];
        }
        delete verts[i];
    }
    delete verts;
    verts = nullptr;
    numverts = 0;

  // Delete faces
    delete faces;
    faces = nullptr;

  // Delete icons
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

  // Unmount sounds
    for (i = 0; i < tags.NumItems(); i++)
    {
        if (!stricmp(tags[i].name, "play"))
        {
            int32_t num = listnum(tags[i].str);  // Get number of sounds in list
            for (int32_t n = 0; n < num; n++)
                SoundPlayer.Unmount(listget(tags[i].str, n));
        }
    }

  // Kill tags
    tags.Clear();

    meshinitialized = false;
}

// Restores lost surfaces, textures, etc.
bool T3DImagery::Restore()
{
    if (SurfacesLost())
    {
        ClearMesh();
        if (!InitializeMesh((PS3DImageryBody)GetBody()))
            return false;
    }

    return true;
}

// *****************************
// * Verts and Faces Functions *
// *****************************

// Returns number of vertex in the vertex array for a single frame
int32_t T3DImagery::NumVerts()
{
    if (!meshinitialized)
        InitializeMesh((PS3DImageryBody)GetBody());

    return numverts;
}

// Copies single frame of vertices (default is state 0, frame 0)
void T3DImagery::GetVerts(LPVOID vertbuf, int32_t state, int32_t frame, D3DVERTEXTYPE verttype,
    int32_t beg, int32_t len)
{
    if (!meshinitialized)
        InitializeMesh((PS3DImageryBody)GetBody());

    int32_t c;

    if (beg < 0)
        beg = 0;
    if (len < 0)
        len = numverts;

    LPD3DVERTEX v;
    if (flags & I3D_ISMORPH)
        v = verts[state][frame];
    else
        v = verts[0][0];

    if (verttype == D3DVT_VERTEX)
        memcpy(vertbuf, v + beg, sizeof(hmm_vec3) * len);
    else if (verttype == D3DVT_LVERTEX)                     // Convert to LVERTEX
    {
        LPD3DVERTEX s = v + beg;
        LPD3DLVERTEX d = (LPD3DLVERTEX)vertbuf;
        for (c = 0; c < len; c++, s++, d++)
        {
            d->x = s->x;
            d->y = s->y;
            d->z = s->z;
            d->color = D3DRGBA(8.0, 8.0, 8.0, 1.0);
            d->specular = D3DRGBA(0, 0, 0, 0);
            d->tu = s->tu;
            d->tv = s->tv;
        }
    }
    else if (verttype == D3DVT_TLVERTEX)                    // Convert to TLVERTEX
    {
        LPD3DVERTEX s = v + beg;
        LPD3DTLVERTEX d = (LPD3DTLVERTEX)vertbuf;
        for (c = 0; c < len; c++, s++, d++)
        {
            d->sx = (float)100 + s->x;
            d->sy = (float)100 + s->y;
            d->sz = (float)500 + s->z;
            d->rhw = (float)1.0;
            d->color = D3DRGBA(1.0, 1.0, 1.0, 1.0);
            d->specular = D3DRGBA(0, 0, 0, 0);
            d->tu = s->tu;
            d->tv = s->tv;
        }
    }
}

int32_t T3DImagery::NumObjVerts(int32_t objnum)
{
    if (!meshinitialized)
        InitializeMesh((PS3DImageryBody)GetBody());

  // Sanity check for object num
    if (objnum < 0 || objnum > objects.NumItems() || !objects.Used(objnum))
        return 0;

    return objects[objnum].numverts;
}

// Copies single frame of vertices (default is state 0, frame 0)
void T3DImagery::GetObjVerts(int32_t objnum, LPVOID vertbuf, int32_t state, int32_t frame, D3DVERTEXTYPE verttype)
{
    if (!meshinitialized)
        InitializeMesh((PS3DImageryBody)GetBody());

    GetVerts(vertbuf, state, frame, verttype, objects[objnum].startvert, objects[objnum].numverts);
}


// Returns the number of faces for this imagery object
int32_t T3DImagery::NumFaces()
{
    if (!meshinitialized)
        InitializeMesh((PS3DImageryBody)GetBody());

    return numfaces;
}

// Copies all faces for this imagery object
void T3DImagery::GetFaces(PS3DFace facesbuf)
{
    if (!meshinitialized)
        InitializeMesh((PS3DImageryBody)GetBody());

    memcpy(facesbuf, faces, sizeof(S3DFace) * numfaces);
}

// Returns the size of the face buffer for a particular object
int32_t T3DImagery::NumObjFaces(int32_t objnum)
{
    if (!meshinitialized)
        InitializeMesh((PS3DImageryBody)GetBody());

  // Sanity check for object num
    if (objnum < 0 || objnum > objects.NumItems() || !objects.Used(objnum))
        return 0;

    return objects[objnum].numfaces;
}

// Copies all object faces
void T3DImagery::GetObjFaces(int32_t objnum, PS3DFace facesbuf, int32_t *texfaces, int32_t *numtexfaces)
{
    if (!meshinitialized)
        InitializeMesh((PS3DImageryBody)GetBody());

  // Sanity check for object num
    if (objnum < 0 || objnum > objects.NumItems() || !objects.Used(objnum))
        return;

    memcpy(facesbuf, faces + objects[objnum].startface, sizeof(S3DFace) * objects[objnum].numfaces);

    if (texfaces)
        memcpy(texfaces, objects[objnum].texfaces, sizeof(int32_t) * min(MAXTEXTURES, textures.NumItems()));

    if (numtexfaces)
        memcpy(numtexfaces, objects[objnum].numtexfaces, sizeof(int32_t) * min(MAXTEXTURES, textures.NumItems()));
}

// ********************
// * Object functions *
// ********************

// Returns number of objects in object list
int32_t T3DImagery::NumObjects()
{
    if (!meshinitialized)
        InitializeMesh((PS3DImageryBody)GetBody());

    return objects.NumItems();
}

// Adds a new object to object list
int32_t T3DImagery::AddObject(PS3DObj obj)
{
    if (!meshinitialized)
        InitializeMesh((PS3DImageryBody)GetBody());

    return objects.Add(*obj);
}

// Removes object from object list
void T3DImagery::RemoveObject(int32_t objnum)
{
    if (!meshinitialized)
        InitializeMesh((PS3DImageryBody)GetBody());

  // Sanity check for object rendering
    if (objnum < 0 || objnum > objects.NumItems() || !objects.Used(objnum))
        return;

    PS3DObj obj = &(objects[objnum]);

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

// Clears all objects
void T3DImagery::ClearObjects()
{
    for (int32_t c = 0; c < objects.NumItems(); c++)
    {
        RemoveObject(c);
    }
    objects.Clear();
}

// Gets the object number given the object name
int32_t T3DImagery::GetObjectNum(char *objname)
{
    if (!meshinitialized)
        InitializeMesh((PS3DImageryBody)GetBody());

    for (int32_t c = 0; c < objects.NumItems(); c++)
    {
        if (objects.Used(c) && !stricmp(objects[c].name, objname))
            return c;
    }
    return -1;
}

// Gets the name of an object
char *T3DImagery::GetObjectName(int32_t objnum)
{
    if (!meshinitialized)
        InitializeMesh((PS3DImageryBody)GetBody());

  // Sanity check for object rendering
    if (objnum < 0 || objnum > objects.NumItems() || !objects.Used(objnum))
        return nullptr;

    return objects[objnum].name;
}

// Gets data for the given object
void T3DImagery::GetObject(int32_t objnum, PS3DObj objbuf)
{
    if (!meshinitialized)
        InitializeMesh((PS3DImageryBody)GetBody());

  // Sanity check for object rendering
    if (objnum < 0 || objnum > objects.NumItems() || !objects.Used(objnum))
        return;

    memcpy(objbuf, &objects[objnum], sizeof(S3DObj));
}

// Calculates a matrix based on the given position, rotation, and scale

void MakeMatrix(hmm_mat4 &m, hmm_vec3 &pos, hmm_vec3 &rot, hmm_vec3 &scl)
{
    if (rot.x != (float)0.0)
        D3DMATRIXRotateX(&m, rot.x);
    if (rot.y != (float)0.0)
        D3DMATRIXRotateY(&m, rot.y);
    if (rot.z != (float)0.0)
        D3DMATRIXRotateZ(&m, rot.z);
    if (pos.x != (float)0.0 || pos.y != (float)0.0 || pos.z != (float)0.0)
        D3DMATRIXTranslate(&m, &pos);
    if (scl.x != (float)1.0 || scl.y != (float)1.0 || scl.z != (float)1.0)
        D3DMATRIXScale(&m, &scl);
}

// Returns whether the object is hidden for the given state
bool T3DImagery::IsHidden(int32_t objnum, int32_t state)
{
    if (!meshinitialized)
        InitializeMesh((PS3DImageryBody)GetBody());

  // Sanity check for object rendering
    if (objnum < 0 || objnum > objects.NumItems() || !objects.Used(objnum))
        return true;

    if (objects[objnum].anikeys[state] == nullptr)
        return true;

    return false;
}

#define POSCHANGED 1
#define ROTCHANGED 2
#define SCLCHANGED 4

// Skips the key for this frame and returns the number of keys skipped.
inline int32_t SkipAniKey32(int32_t key, PSAniKey32 keys, int32_t numkeys, int32_t &frames, int32_t &flags,
    int32_t &posidx, int32_t &rotidx, int32_t &sclidx)
{
    int32_t num = 0;
    PSAniKey32 ak32 = &(keys[key]);
    frames = 1;
    flags = 0;

    // Note: frame numbers are NOT stored in keys.. instead, the next frame
    // is implied by a repeated pos, rot, or scl key, or the presence of
    // a SKIP key.  Repeats for each frame are garanteed by the exporter.

    while (key < numkeys)
    {
        int32_t flags32 = ak32->flags;

        switch (flags32)
        {
          case ANIFLAG32_CODE:
          {
            int32_t code = ak32->code;
            switch (code)
            {
              case ANICODE32_NEXTFRAME:
              {
                if (!(flags & (POSCHANGED | ROTCHANGED | SCLCHANGED))) // No frame before this skip
                    frames = 0;                       // Set frame count to 0
                key++;
                ak32++;
                num++;
                return num;
              }
              case ANICODE32_SKIP:
              {
                if (!(flags & (POSCHANGED | ROTCHANGED | SCLCHANGED))) // No frame before this skip
                    frames = 0;                       // Set frame count to 0
                frames += ak32->value - 1;                // Then add frames to skip
                key++;
                ak32++;
                num++;
                return num;
              }
              case ANICODE32_POSX:
              case ANICODE32_POSY:
              case ANICODE32_POSZ:
              {
                if (flags & (POSCHANGED | ROTCHANGED | SCLCHANGED))
                    return num;
                if (code == ANICODE32_POSX)
                    posidx = key;
                else if (code == ANICODE32_POSZ)    // Toggle done on last pos key (z)
                    flags |= POSCHANGED;
                break;
              }
              case ANICODE32_ROTX:
              case ANICODE32_ROTY:
              case ANICODE32_ROTZ:
              {
                if (flags & (ROTCHANGED | SCLCHANGED))
                    return num;
                if (code == ANICODE32_ROTX)
                    rotidx = key;
                else if (code == ANICODE32_ROTZ) // Toggle done on last rot key (z)
                    flags |= ROTCHANGED;
                break;
              }
              case ANICODE32_SCLX:
              case ANICODE32_SCLY:
              case ANICODE32_SCLZ:
              {
                if (flags & SCLCHANGED)
                    return num;
                if (code == ANICODE32_SCLX)
                    sclidx = key;
                else if (code == ANICODE32_SCLZ) // Toggle done on last scl key (z)
                    flags |= SCLCHANGED;
                break;
              }
            }
            break;
          }
          case ANIFLAG32_POS:
          {
            if (flags & (POSCHANGED | ROTCHANGED | SCLCHANGED))
                return num;
            posidx = key;
            flags |= POSCHANGED;
            break;
          }
          case ANIFLAG32_ROT:
          {
            if (flags & (ROTCHANGED | SCLCHANGED))
                return num;
            rotidx = key;
            flags |= ROTCHANGED;
            break;
          }
          case ANIFLAG32_SCL:
          {
            if (flags & SCLCHANGED)
                return num;
            sclidx = key;
            flags |= SCLCHANGED;
            break;
          }
        }

        key++;
        ak32++;
        num++;
    }

    return num;
}

// Skips the key for this frame and returns the number of keys skipped.
inline void GetAniKey32(int32_t key, PSAniKey32 keys, int32_t numkeys,  
    int32_t posidx, int32_t rotidx, int32_t sclidx,
    hmm_vec3 &pos, hmm_vec3 &rot, hmm_vec3 &scl)
{
    
  // Get position key
    if ((uint32_t)posidx < (uint32_t)numkeys)
    {
        PSAniKey32 ak32 = &(keys[posidx]);

        if (ak32->flags == ANIFLAG32_CODE && ak32->code == ANICODE32_POSX)
        {
            pos.x = (float)ak32[0].value / (float)ANIKEY32_CODESCALE;
            pos.y = (float)ak32[1].value / (float)ANIKEY32_CODESCALE;
            pos.z = (float)ak32[2].value / (float)ANIKEY32_CODESCALE;
        }
        else
        {
            pos.x = (float)ak32->x / (float)ANIKEY32_POSSCALE;
            pos.y = (float)ak32->y / (float)ANIKEY32_POSSCALE;
            pos.z = (float)ak32->z / (float)ANIKEY32_POSSCALE;
        }
    }
    else
        pos.x = pos.y = pos.z = 0.0f;

  // Get rotation key
    if ((uint32_t)rotidx < (uint32_t)numkeys)
    {
        PSAniKey32 ak32 = &(keys[rotidx]);

        if (ak32->flags == ANIFLAG32_CODE && ak32->code == ANICODE32_ROTX)
        {
            rot.x = (float)ak32[0].value / (float)ANIKEY32_CODESCALE;
            rot.y = (float)ak32[1].value / (float)ANIKEY32_CODESCALE;
            rot.z = (float)ak32[2].value / (float)ANIKEY32_CODESCALE;
        }
        else
        {
            rot.x = (float)ak32->x / (float)ANIKEY32_ROTSCALE;
            rot.y = (float)ak32->y / (float)ANIKEY32_ROTSCALE;
            rot.z = (float)ak32->z / (float)ANIKEY32_ROTSCALE;
        }
    }
    else
        rot.x = rot.y = rot.z = 0.0f;

  // Get scale key
    if ((uint32_t)sclidx < (uint32_t)numkeys)
    {
        PSAniKey32 ak32 = &(keys[sclidx]);

        if (ak32->flags == ANIFLAG32_CODE && ak32->code == ANICODE32_SCLX)
        {
            scl.x = (float)ak32[0].value / (float)ANIKEY32_CODESCALE;
            scl.y = (float)ak32[1].value / (float)ANIKEY32_CODESCALE;
            scl.z = (float)ak32[2].value / (float)ANIKEY32_CODESCALE;
        }
        else
        {
            scl.x = (float)ak32->x / (float)ANIKEY32_SCLSCALE;
            scl.y = (float)ak32->y / (float)ANIKEY32_SCLSCALE;
            scl.z = (float)ak32->z / (float)ANIKEY32_SCLSCALE;
        }
    }
    else
        scl.x = scl.y = scl.z = 1.0f;
}

// Gets the position, rotation, and scale for the given state and frame, or returns false
// if there is no animation for that frame.  This is the uninterpolated version.

bool T3DImagery::GetUninterpolatedAniKey(int32_t objnum, int32_t state, int32_t frame, 
    hmm_vec3 &pos, hmm_vec3 &rot, hmm_vec3 &scl)
{
    PS3DObj obj = &objects[objnum];

    if (!meshinitialized)
        InitializeMesh((PS3DImageryBody)GetBody());

    int32_t numaniframes = GetAniLength(state);
    if ((uint32_t)state > (uint32_t)NumStates() || (uint32_t)frame >= (uint32_t)GetAniLength(state))
        return false;

    // Set animation matrices for object
    if (obj->anikeys[state])
    {
      // Do old AniKey extraction
        if (!(flags & I3D_ANIKEY32))
        {   
            SAniKey &ak = ((PSAniKey)(obj->anikeys[state]))[frame];
            rot.x = (float)ak_rx(ak) / (float)ANIKEY_ANGSCALE;
            rot.y = (float)ak_ry(ak) / (float)ANIKEY_ANGSCALE;
            rot.z = (float)ak_rz(ak) / (float)ANIKEY_ANGSCALE;
            pos.x = (float)ak_x(ak) / (float)ANIKEY_POSSCALE;
            pos.y = (float)ak_y(ak) / (float)ANIKEY_POSSCALE;
            pos.z = (float)ak_z(ak) / (float)ANIKEY_POSSCALE;
            scl.x = scl.y = scl.z = (float)1.0;
        }

      // Do new AniKey32 extraction 
        else
        {
            // NOTE: The new AniKey32 key system uses a list of pos, rot, scale 32
            // bit compressed 10 bits per x,y,z keys, and code keys which may be
            // a single 24 bit x,y,z key, or a skip code which indicates how many frames
            // to skip before the next key.  There are no frame numbers in the key
            // lists, instead, a NEXTFRAME is implied by the repeat of a pos/rot/scale
            // key, or the occurence of SKIP key.  POS keys, ROT keys, and SCL keys are
            // added to the list when their values change sufficiently to warrant a new
            // key, thus the key array is sparse, containing only the information
            // necessary to animate the object.

            int32_t curframe = 0;
            int32_t curkey = 0;
            int32_t numkeys = obj->numanikeys[state]; // Number of animation keys
            PSAniKey32 keys = &(((PSAniKey32)obj->anikeys[state])[curkey]);
            int32_t posidx, rotidx, sclidx;
            posidx = rotidx = sclidx = -1;

          // This little search loop should be BLAZING fast in practice.. so the fact
          // that you have to search for a key should not be a big problem.
            for (;;)
            {
                int32_t num, frames, flags;
                if (curkey < numkeys)
                    num = SkipAniKey32(curkey, keys, numkeys, frames, flags,
                        posidx, rotidx, sclidx);  // Find where keys are set
                else
                    num = 0;
                
                if ((num <= 0) || (curframe + frames >= frame))
                {
                    GetAniKey32(curkey, keys, numkeys,
                        posidx, rotidx, sclidx, // Last key where these are set
                        pos, rot, scl);         // Return pos,rot,scl values

                  // Interpolate between keys here...
/*                  if ((frame > curframe) && (curkey + num < numkeys))
                    {
                        hmm_vec3 pos2, rot2, scl2;
                        int32_t frames2, flags2;

                        num += GetAniKey32(curkey + num, keys, numkeys,
                            frames2, flags2,    // Get frames for keys
                            pos2, rot2, scl2);  // Get pos,rot,scl

                      // Got next key, now do linear interpolation between keys
                        float i = 
                            (float)frame - (float)curframe / (float)frames; 

                        if (flags2 & POSCHANGED)
                        {
                            pos.x = pos.x * (1.0f - i) + (pos2.x * i);
                            pos.y = pos.y * (1.0f - i) + (pos2.y * i);
                            pos.z = pos.z * (1.0f - i) + (pos2.z * i);
                        }

                        if (flags2 & ROTCHANGED)
                        {
                            rot.x = rot.x * (1.0f - i) + (rot2.x * i);
                            rot.y = rot.y * (1.0f - i) + (rot2.y * i);
                            rot.z = rot.z * (1.0f - i) + (rot2.z * i);
                        }

                        if (flags2 & SCLCHANGED)
                        {
                            scl.x = scl.x * (1.0f - i) + (scl2.x * i);
                            scl.y = scl.y * (1.0f - i) + (scl2.y * i);
                            scl.z = scl.z * (1.0f - i) + (scl2.z * i);
                        }
                    }
*/
                    break; // Break out of key search loop
                }

                curkey += num;
                curframe += frames;

            } // While loop
        }
    }
    else // No ani keys (object is hidden!)
    {
        rot.x = rot.y = rot.z = pos.x = pos.y = pos.z = (float)0.0;
    }

    return true;
}


// Gets the position, rotation, and scale for the given state and frame, or returns false
// if there is no animation for that frame.  This is the interpolated version

#define INTERFRAMES 5

void InterpolatePoints(hmm_vec3 &v1, hmm_vec3 &v2, float &i)
{
    if (v1.x != v2.x || v1.y != v2.y || v1.z != v2.z)
    {
        v1.x = v1.x * i + v2.x * ((float)1.0 - i);
        v1.y = v1.y * i + v2.y * ((float)1.0 - i);
        v1.z = v1.z * i + v2.z * ((float)1.0 - i);
    }
}

// Adjusts differences in rotations so that all rotations are less than 180 degrees
void NormalizeRot(hmm_vec3 &v1, hmm_vec3 &v2)
{
    if (v2.x - v1.x > M_PI)         // Greater than 180
        v2.x -= (float)M_2PI;
    else if (v2.x - v1.x < -M_PI)   // Less than -180
        v2.x += (float)M_2PI;
    if (v2.y - v1.y > M_PI)         // Greater than 180
        v2.y -= (float)M_2PI;
    else if (v2.y - v1.y < -M_PI)   // Less than -180
        v2.y += (float)M_2PI;
    if (v2.z - v1.z > M_PI)         // Greater than 180
        v2.z -= (float)M_2PI;
    else if (v2.z - v1.z < -M_PI)   // Less than -180
        v2.z += (float)M_2PI;

    if (abs(v2.x - v1.x) > M_PI / 2 ||
        abs(v2.y - v1.y) > M_PI / 2 ||
        abs(v2.z - v1.z) > M_PI / 2)
    {
        v2 = v1; // Quaternion interpolation goes here
    }
}

bool T3DImagery::GetAniKey(int32_t objnum, int32_t state, int32_t frame, 
    hmm_vec3 &pos, hmm_vec3 &rot, hmm_vec3 &scl)
{
    if (!meshinitialized)
        InitializeMesh((PS3DImageryBody)GetBody());

    if (!GetUninterpolatedAniKey(objnum, state, frame, pos, rot, scl))
        return false;

    PS3DObj obj = &objects[objnum];

    if (!Interpolate)                                               // No interpolation period
        return true;

    if (GetHeader()->states[state].aniflags & AF_NOINTERPOLATION)   // No interpolation this state
        return true;

    if ((uint32_t)prevstate > (uint32_t)GetHeader()->numstates)           // Doesn't have previous state
        return true;

    if (GetHeader()->states[prevstate].aniflags & AF_NOINTERPOLATION) // Previous state no interpolation
        return true;

    char *begstate = FindTag("beg", state);
    char *endstate = FindTag("end", prevstate);

    if (begstate && endstate &&                                     // Has begin/end state frames   
        prevframe >= (GetHeader()->states[prevstate].frames - 1) && // and last state went to last frame
        !stricmp(endstate, begstate))                               // and beg pose for this state matches end pose of prev
        return true;

    int32_t iframes = INTERFRAMES;
    int32_t anilength = GetAniLength(state);

    float i = (float)1.0;
    hmm_vec3 ipos, irot, iscl;
    if (prevstate >= 0 && frame < iframes)   // From previous state
    {
        i = (float)(frame + 1) / (float)iframes;
        GetUninterpolatedAniKey(objnum, prevstate, prevframe, ipos, irot, iscl);
        NormalizeRot(rot, irot); // Makes sure rotations are all less than 180 degrees
        InterpolatePoints(pos, ipos, i);
        InterpolatePoints(rot, irot, i);
        InterpolatePoints(scl, iscl, i);
    }

    return true;
}

// Calculates the given objects matrix
bool T3DImagery::CalcObjectMatrix(PS3DAnimObj animobj, int32_t state, int32_t frame, hmm_mat4* pos, bool calcparents)
{
    int32_t objnum = animobj->objnum;
    PS3DObj obj = &objects[objnum];
    hmm_mat4 &m = animobj->matrix;

    D3DMATRIXClear(&m);

    bool hastrans = animobj->flags & (OBJ3D_ROTMASK | OBJ3D_POSMASK | OBJ3D_SCLMASK);

    if (!(flags & I3D_ISMORPH) && (!hastrans || animobj->flags & OBJ3D_ADDTOANI))
    {
        int32_t animtrack;
        if (animobj->flags & OBJ3D_ANIMTRACK)
            animtrack = animobj->animtrack;
        else
            animtrack = animobj->objnum;

        if (GetAniKey(animtrack, state, frame, animobj->pos, animobj->rot, animobj->scl))
            MakeMatrix(m, animobj->pos, animobj->rot, animobj->scl);
    }

    if (hastrans)  // One of the object transform flags was set, so do the pos, rot, scl
    {
      // Do first transform
        if ((animobj->flags & OBJ3D_POSMASK) == OBJ3D_POS1)
            D3DMATRIXTranslate(&m, &animobj->pos);
        else if ((animobj->flags & OBJ3D_ROTMASK) == OBJ3D_ROT1)
        {
            D3DMATRIXRotateX(&m, animobj->rot.x);
            D3DMATRIXRotateY(&m, animobj->rot.y);
            D3DMATRIXRotateZ(&m, animobj->rot.z);
        }
        else if ((animobj->flags & OBJ3D_SCLMASK) == OBJ3D_SCL1)
            D3DMATRIXScale(&m, &animobj->scl);

      // Do second transform
        if ((animobj->flags & OBJ3D_POSMASK) == OBJ3D_POS2)
            D3DMATRIXTranslate(&m, &animobj->pos);
        else if ((animobj->flags & OBJ3D_ROTMASK) == OBJ3D_ROT2)
        {
            D3DMATRIXRotateX(&m, animobj->rot.x);
            D3DMATRIXRotateY(&m, animobj->rot.y);
            D3DMATRIXRotateZ(&m, animobj->rot.z);
        }
        else if ((animobj->flags & OBJ3D_SCLMASK) == OBJ3D_SCL2)
            D3DMATRIXScale(&m, &animobj->scl);

      // Do third transform
        if ((animobj->flags & OBJ3D_POSMASK) == OBJ3D_POS3)
            D3DMATRIXTranslate(&m, &animobj->pos);
        else if ((animobj->flags & OBJ3D_ROTMASK) == OBJ3D_ROT3)
        {
            D3DMATRIXRotateX(&m, animobj->rot.x);
            D3DMATRIXRotateY(&m, animobj->rot.y);
            D3DMATRIXRotateZ(&m, animobj->rot.z);
        }
        else if ((animobj->flags & OBJ3D_SCLMASK) == OBJ3D_SCL3)
            D3DMATRIXScale(&m, &animobj->scl);
    }

  // If has parent, concatenate local position with parents position..
    if (animobj->parent)
    {
        if (calcparents)
            CalcObjectMatrix(animobj->parent, state, frame, pos, calcparents);
        MultiplyD3DMATRIX(&m, &m, &animobj->parent->matrix);
    }

    return true;
}

// Calculates the given objects matrix
// re-write of the original that a) deals only with the matrix and b) actually copies the result into pos...
bool T3DImagery::CalcObjectMatrixCopy(PS3DAnimObj animobj, int32_t state, int32_t frame, hmm_mat4* pos, bool calcparents)
{
    int32_t objnum = animobj->objnum;
    PS3DObj obj = &objects[objnum];
    hmm_mat4 m;
    
    memcpy(&m, &animobj->matrix, sizeof(hmm_mat4));

    // If has parent, concatenate local position with parents position..
    if (animobj->parent && calcparents)
    {
        if (calcparents)
            CalcObjectMatrixCopy(animobj->parent, state, frame, pos, calcparents);
        MultiplyD3DMATRIX(&m, &m, &animobj->parent->matrix);
    }

    memcpy(pos, &m, sizeof(hmm_mat4));

    return true;
}


// Renders an object individually
bool T3DImagery::RenderObject(PS3DAnimObj animobj, int32_t state, int32_t frame, hmm_mat4* pos, 
    int32_t tex, bool uselastmatrix)
{
    int32_t t;
    int32_t objnum = animobj->objnum;

  // Sanity check for object rendering
    if (objnum < 0 || objnum > objects.NumItems() || !objects.Used(objnum))
        return false;

  // Sanity check for state
    if (state >= NumStates())
        return false;

  // If textures turned off, reload mesh
    if (hastextures) // Turn textures on/off
    {
        if ((!UseTextures && textures.NumItems() > 0) || (UseTextures && textures.NumItems() <= 0))
            ClearMesh();
    }

  // If mesh not initialized, attempt to initialize it now
    if (!meshinitialized)
        if (!InitializeMesh((PS3DImageryBody)GetBody()))
            return false;

    PS3DObj obj = &objects[objnum];

  // **** CALCULATE POSITION ****

  // Make the objects matrix if object isn't morph, and we aren't using the last matrix 
    hmm_mat4 &m = animobj->matrix;
    if (!(animobj->flags & OBJ3D_MATRIX) && !uselastmatrix)
        CalcObjectMatrix(animobj, state, frame, pos, false); // Don't calculate parent matrices 

  // This is where we check to see if we're hidden (after calculating position and
  // before we change any render states)
    if (animobj->flags & OBJ3D_HIDE)
        return true;
 
  // **** SET RENDER STATES ****

  // Ok, we have the object matrix, now multiply it by the world, and set it into
  // the D3D render state
    if (!(flags & I3D_ISMORPH) ||  // If not a morph (uses animation keys)
        (animobj->flags & (OBJ3D_POSMASK | OBJ3D_ROTMASK | OBJ3D_SCLMASK | OBJ3D_MATRIX)))
         // Or is morph but overriding pos or matrix
    {
        // See if we need to transform the position to the objects world space position
        if (animobj->flags & OBJ3D_ABSPOS)
            TRY_D3D(Scene3D.SetTransform(D3DTRANSFORMSTATE_WORLD, &animobj->matrix))
        else
        {
            hmm_mat4 world;

            if (Double3D || Triple3D)
            {
                hmm_mat4 temp;
                memcpy(&temp, &animobj->matrix, sizeof(hmm_mat4));
                hmm_vec3 s;
                s.x = s.y = s.z = Triple3D ? 3.0f : 2.0f;
                D3DMATRIXScale(&temp, &s);

                MultiplyD3DMATRIX(&world, &temp, pos);
                TRY_D3D(Scene3D.SetTransform(D3DTRANSFORMSTATE_WORLD, &world))
            }
            else
            {
                MultiplyD3DMATRIX(&world, &animobj->matrix, pos);
                TRY_D3D(Scene3D.SetTransform(D3DTRANSFORMSTATE_WORLD, &world))
            }

//          hmm_vec3 v1, v2;           // Test if hip position less than 0!!!!
//          v1.x = v1.y = v1.z = 0.0f;
//          D3DMATRIXTransform(&world, &v1, &v2);
//          if (v2.z < 30.0f && !stricmp(obj->name, "hips"))
//          {
//              v2.z = v2.z * 1.0f;
//          } 

        }
    }
    else
    {
        TRY_D3D(Scene3D.SetTransform(D3DTRANSFORMSTATE_WORLD, pos))
    }

  // If we have already cached an execute buffer, we can leave right here after matrix stuff    
    if (cacheexbuf && executebuf != nullptr)
        return true;

  // Set the object material
    D3DMATERIALHANDLE hmaterial;
    if (animobj->flags & OBJ3D_MAT)
        hmaterial = animobj->hmaterial;
    else
        hmaterial = materials[obj->material].hmaterial;
    D3DMATERIALHANDLE holdmat;
    TRY_D3D(Scene3D.GetLightState(D3DLIGHTSTATE_MATERIAL, &holdmat));
    if (holdmat != hmaterial)
        TRY_D3D(Scene3D.SetLightState(D3DLIGHTSTATE_MATERIAL, hmaterial))

  // Now draw the objects faces by texture
    for (t = 0; t < textures.NumItems() + 1; t++)
    {
        int32_t numvrt;
        D3DVERTEXTYPE vrttype;
        LPVOID vrt;
        int32_t numfac;
        S3DFace *fac;

        if (tex >= 0 && tex != t) // Do only textures specified in the command line
            continue;

      // Get vertices to draw
        if (animobj->flags & OBJ3D_VERTS)
        {
            numvrt = animobj->numverts;                     // Use override vertex array
            vrttype = animobj->verttype;
            vrt = animobj->verts;
        }
        else
        {
            numvrt = obj->numverts;
            vrttype = D3DVT_VERTEX;
            if (flags & I3D_ISMORPH)
                vrt = &verts[state][frame] + obj->startvert;
            else
                vrt = verts[0][0] + obj->startvert;
        }

      // Get faces to draw
        if (animobj->flags & OBJ3D_FACES)
        {
            numfac = animobj->numtexfaces[t];               // Use override face array
            fac = animobj->faces + animobj->texfaces[t];
        }
        else
        {
            numfac = obj->numtexfaces[t];
            fac = &faces[obj->startface + obj->texfaces[t]];
        }

      // Nothing to do!
        if (numvrt <= 0 || numfac <= 0)
                continue;

      // Set multiframe texture frame (does nothing if overriding texture handles)
        if (hastextures && UseTextures &&
            t > 0 && !(animobj->flags & OBJ3D_TEX)) // Can't have custom textures and control frames
        {
            int32_t texframe;
            if (animobj->flags & OBJ3D_TEXFRAME)
                texframe = animobj->textureframe[t];
            else
                texframe = frame;
            SetTextureFrame(t - 1, texframe);
        }

      // Set texture
        D3DTEXTUREHANDLE htexture = nullptr;;
        LPDIRECTDRAWSURFACE texsurface = nullptr;
        if (!hastextures || !UseTextures)
        {
            htexture = nullptr;
            texsurface = nullptr;
        }
        else if (animobj->flags & OBJ3D_TEX)
        {
            htexture = animobj->htextures[t];
            texsurface = animobj->surfaces[t];
        }
        else
        {
            if (t == 0)
            {
                htexture = nullptr;
                texsurface = nullptr;
            }
            else
            {
                htexture = textures[t - 1].htexture;
                texsurface = textures[t - 1].surface;
            }
        }
        D3DTEXTUREHANDLE holdtex;
        TRY_D3D(Scene3D.GetRenderState(D3DRENDERSTATE_TEXTUREHANDLE, &holdtex));
        if (materials[obj->material].matdesc.hTexture != htexture)
        {
            materials[obj->material].matdesc.hTexture = htexture;
            materials[obj->material].material->SetMaterial(&materials[obj->material].matdesc);
        }
        if (holdtex != htexture)
        {
            //TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE, htexture));
            //>>>>>>>>>>>>>>>>>>>>>>>>>>
            TRY_D3D(Scene3D.SetTexture( htexture, texsurface ));
            //<<<<<<<<<<<<<<<<<<<<<<<<<<
        }

      // Uhhhh.... ok, draw it now..
        TRY_D3D(Scene3D.DrawIndexedPrimitive(
            D3DPT_TRIANGLELIST,
            vrttype,
            vrt,
            numvrt,
            (uint16_t *)fac,
            numfac * 3,
            /*D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP*/ 0));
    }

    return true;
}

void T3DImagery::BeginRender(bool cachebuf, bool clearbuf)
{
    if (clearbuf && executebuf != nullptr)
    {
        delete executebuf;
        executebuf = nullptr;
    }

    cacheexbuf = cachebuf && !UseDrawPrimitive && CacheExBufs;  // Only attempt to cache if we can

    if (cacheexbuf && executebuf != nullptr)
        Scene3D.UseExecuteBuf(executebuf);  // Tell scene we want to use this buf..
        
  // Initialize rendering stuff for scene (sets up execute buf stuff, etc.)
    Scene3D.BeginRender();
}


void T3DImagery::EndRender()
{
  // Tell scene we're done rendering (wraps up execute buffer stuff, etc.)  
    Scene3D.EndRender();

  // If we want to cache a buf, and we haven't yet, and there were no calls to 
  // GetExtents() from within the previous render, copy the previous execute
  // buffer into our cache.
    if (cacheexbuf && !executebuf && !Scene3D.WasRenderFlushed())
        executebuf = Scene3D.CopyExecuteBuf();
}

void T3DImagery::PlaySound(PTObjectInstance inst, int32_t state, int32_t frame)
{
    char *soundlist = FindTag("play", state, frame);
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

int32_t T3DImagery::AddMaterial(LPD3DMATERIAL newmat, int32_t tex)
{
    S3DMat mat;
    D3DMATERIAL &m = mat.matdesc;
    memcpy(&m, newmat, sizeof(D3DMATERIAL));

    TRY_D3D(Direct3D->CreateMaterial(&mat.material, nullptr));

    bool isalpha = false;
    if (tex >= 0 && textures[tex].ddsd.ddpfPixelFormat.dwRGBAlphaBitMask != 0)
        isalpha = true;

    if (UseTextures && tex >= 0 && tex < textures.NumItems() &&
        textures.Used(tex) && textures[tex].htexture)
    {
        if (isalpha)
        {
            m.ambient.r = m.ambient.g = m.ambient.b = 0.0f;
            m.ambient.a = 1.0f;
            m.diffuse.r = m.diffuse.g = m.diffuse.b = 0.0f;
            m.diffuse.a = 1.0f;
            m.specular.r = m.specular.g = m.specular.b = 0.0f;
            m.specular.a = 1.0f;
        }
/*      m.ambient.r = 1.0f;
        m.ambient.g = 1.0f;
        m.ambient.b = 1.0f;
        m.ambient.a = 1.0f;
        m.diffuse.r = 1.0f;
        m.diffuse.g = 1.0f;
        m.diffuse.b = 1.0f;
        m.diffuse.a = 1.0f;
        m.specular.r = 1.0f;
        m.specular.g = 1.0f;
        m.specular.b = 1.0f;
        m.specular.a = 1.0f;
*/      m.hTexture = textures[tex].htexture;
        mat.texture = tex;
    }
    else
    {
        m.hTexture = nullptr;
        mat.texture = -1;
    }

//  m.power = 30.0f;
    mat.material->SetMaterial(&m);
    mat.material->GetHandle(Device, &(mat.hmaterial));

    return materials.Add(mat);
}

void T3DImagery::GetMaterial(int32_t matnum, PS3DMat mat)
{
    if (matnum < 0 || matnum >= materials.NumItems() || !materials.Used(matnum))
        return;

    memcpy(mat, &materials[matnum], sizeof(S3DMat));
}

D3DMATERIALHANDLE T3DImagery::GetMaterialHandle(int32_t matnum)
{
    if (matnum < 0 || matnum >= materials.NumItems() || !materials.Used(matnum))
        return nullptr;

    return materials[matnum].hmaterial;
}

void T3DImagery::SetMaterial(int32_t matnum, PS3DMat mat)
{
    if (matnum < 0 || matnum >= materials.NumItems() || !materials.Used(matnum))
        return;

    memcpy(&materials[matnum].matdesc, &mat->matdesc, sizeof(D3DMATERIAL));
    if (mat->texture < 0 || mat->texture >= textures.NumItems() || !textures.Used(mat->texture))
    {
        materials[matnum].matdesc.hTexture = nullptr;
        materials[matnum].texture = -1;
    }
    else
    {
        materials[matnum].matdesc.hTexture = textures[mat->texture].htexture;
        materials[matnum].texture = mat->texture;
    }
    materials[matnum].material->SetMaterial(&materials[matnum].matdesc);
}

void T3DImagery::RemoveMaterial(int32_t matnum)
{
    if (!materials.Used(matnum) || !materials[matnum].material)
         return;
    RELEASE(materials[matnum].material);
    materials[matnum].material = nullptr;
    materials[matnum].hmaterial = nullptr;

    materials.Remove(matnum);
}

// Release materials
void T3DImagery::ClearMaterials()
{
    for (int32_t c = 0; c < materials.NumItems(); c++)
    {
        RemoveMaterial(c);
    }
    materials.Clear();
}

// *********************
// * Texture Functions *
// *********************

/*
 * NewTexture
 * Loads a pixel buffer into a texture map DD surface of the given format.  If
 * lpTexture is not nullptr, the source texture will be reloaded into lpTexture.
 */

extern LPDIRECTDRAWSURFACE back;

int32_t T3DImagery::AddTexture(LPDDSURFACEDESC srcsd,
    OFFSET *pixels, int32_t frames, LPPALETTEENTRY palette)
{
    S3DTex tex;
    memset(&tex, 0, sizeof(S3DTex));
    if (!LoadTexture(&tex, srcsd, pixels, frames, palette, false))
        return -1;

    return textures.Add(tex);
}

bool T3DImagery::LoadTexture(PS3DTex tex,
    LPDDSURFACEDESC srcsd, OFFSET *pixels, int32_t frames, LPPALETTEENTRY palette,
    bool copyframes)
{
    DDSURFACEDESC ddsd;
    PALETTEENTRY *dstpal = nullptr;
    uint32_t pcaps;

//  if (texnum <= 0) // Use back surface as texture.. COOL!
//  {
//      TRY_D3D(back->QueryInterface(IID_IDirect3DTexture, (LPVOID*)&texture[texnum]));
//      TRY_D3D(texture[texnum]->GetHandle(Device, &htexture[texnum]));
//      return true;
//  }

    if (srcsd->dwWidth != srcsd->dwHeight)
        Error("%s has non square texture", GetResFilename());

    if (srcsd->dwWidth != 128 &&    // From most common to least common sizes
        srcsd->dwWidth != 64 &&
        srcsd->dwWidth != 256 &&
        srcsd->dwWidth != 32 &&
        srcsd->dwWidth != 512 &&
        srcsd->dwWidth != 16 &&
        srcsd->dwWidth != 8)
            Error("%s texture is not a power of two (8-512)", GetResFilename());

    DDSURFACEDESC dstsd;
    Scene3D.GetClosestTextureFormat(srcsd, &dstsd);

  // Setup palette
    int32_t dstpalcolors = 0;
    if (dstsd.ddpfPixelFormat.dwFlags & (DDPF_PALETTEINDEXED8 | DDPF_PALETTEINDEXEDTO8))
        dstpalcolors = 256;
    else if (dstsd.ddpfPixelFormat.dwFlags &
        (DDPF_PALETTEINDEXED1 | DDPF_PALETTEINDEXED2 | DDPF_PALETTEINDEXED4))
        dstpalcolors = 1 << dstsd.ddpfPixelFormat.dwRGBBitCount;

  // Setup surface descs
    srcsd->lPitch = srcsd->dwWidth;

    dstsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_PIXELFORMAT;
    dstsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
    dstsd.dwWidth = srcsd->dwWidth;
    dstsd.lPitch = srcsd->lPitch;
    dstsd.dwHeight = srcsd->dwHeight;

  // Save a copy of the surface desc in texture array
    memcpy(&tex->ddsd, &dstsd, sizeof(DDSURFACEDESC));

  // Allocate palette
    if (dstpalcolors > 0)
        dstpal = new PALETTEENTRY[dstpalcolors];
    else
        dstpal = nullptr;

  // Check for memory!
    int32_t bufsize = (dstsd.ddpfPixelFormat.dwRGBBitCount * dstsd.lPitch * dstsd.dwHeight) >> 3;
    int32_t mem = GetFreeTextureMem();
//  if (mem - bufsize < 10 * 1024)    // Not enough video memory for texture
//      return false;                 // Don't use because some video cards don't support this function!!
    int32_t animsize = bufsize * frames;
    if (mem - animsize < 100 * 1024)
        copyframes = true;              // Forces copyframes to be true if not enough memory

  // ***** Do Texture Frames *****
    if (frames <= 1)
        copyframes = false;  // Can't copy frames if there's only one frame
    tex->copyframes = copyframes;
    tex->numframes = frames;
    tex->framenum = 0;
    if (frames > 1)
    {
        tex->framesurfs = new LPDIRECTDRAWSURFACE[frames];
        tex->frametexs = new LPDIRECT3DTEXTURE[frames];
        tex->framehtexs = new D3DTEXTUREHANDLE[frames];
    }
    else
    {
        tex->framesurfs = nullptr;
        tex->frametexs = nullptr;
        tex->framehtexs = nullptr;
    }
    tex->surface = nullptr;
    tex->texture = nullptr;
    tex->htexture = nullptr;

    LPDIRECTDRAWSURFACE memsurf = nullptr;
    LPDIRECT3DTEXTURE memtex = nullptr;
    D3DTEXTUREHANDLE memhtex = nullptr;
    LPDIRECTDRAWPALETTE mempal = nullptr;

    LPDIRECTDRAWSURFACE vidsurf = nullptr;
    LPDIRECT3DTEXTURE vidtex = nullptr;
    D3DTEXTUREHANDLE vidhtex = nullptr;
    LPDIRECTDRAWPALETTE vidpal = nullptr;

    for (int32_t f = 0; f < frames; f++)
    {
      // Create the memory surface
        if (memsurf == nullptr)
        {
            dstsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_SYSTEMMEMORY;
            DirectDraw->CreateSurface(&dstsd, &memsurf, nullptr);
            if (memsurf == nullptr)
                return false;
        }

      // Convert the surface
        memset(&ddsd, 0, sizeof(DDSURFACEDESC));
        ddsd.dwSize = sizeof(DDSURFACEDESC);
        TRY_DD(memsurf->Lock(nullptr, &ddsd, DDLOCK_NOSYSLOCK | DDLOCK_WAIT, nullptr));
        uint8_t *dstpixels = (uint8_t *)ddsd.lpSurface;
        Scene3D.ConvertTexture(srcsd, pixels[f], palette, &dstsd, dstpixels, dstpal); // Should already have keycolor encoded as 1 bit alpha
        TRY_DD(memsurf->Unlock(nullptr));

      // Create palette (if needed)
        if (dstpal)
        {
            if (dstsd.ddpfPixelFormat.dwFlags & (DDPF_PALETTEINDEXED8 | DDPF_PALETTEINDEXEDTO8))
                pcaps = DDPCAPS_8BIT | DDPCAPS_ALLOW256;
            else if (dstsd.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED4)
                pcaps = DDPCAPS_4BIT;
            else if (dstsd.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED2)
                pcaps = DDPCAPS_2BIT;
            else if (dstsd.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED1)
                pcaps = DDPCAPS_1BIT;
            TRY_DD(DirectDraw->CreatePalette(DDPCAPS_INITIALIZE | pcaps, dstpal, &mempal, nullptr));
            TRY_DD(memsurf->SetPalette(mempal));
        }

        TRY_D3D(memsurf->QueryInterface(IID_IDirect3DTexture, (LPVOID*)&memtex));
        TRY_D3D(memtex->GetHandle(Device, &memhtex));

      // Setup primary texture buffer
        if (!vidsurf)
        {

            memset(&ddsd, 0, sizeof(DDSURFACEDESC));
            ddsd.dwSize = sizeof(DDSURFACEDESC);
            TRY_D3D(memsurf->GetSurfaceDesc(&ddsd));
            ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
            ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_ALLOCONLOAD;
            if (!IsUsingHardware())
                ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
            DirectDraw->CreateSurface(&ddsd, &vidsurf, nullptr);
            if (vidsurf == nullptr)
                return false;

            if (ddsd.ddpfPixelFormat.dwFlags & (DDPF_PALETTEINDEXED8 | DDPF_PALETTEINDEXEDTO8))
                pcaps = DDPCAPS_8BIT | DDPCAPS_ALLOW256;
            else if (ddsd.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED4)
                pcaps = DDPCAPS_4BIT;
            else if (ddsd.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED2)
                pcaps = DDPCAPS_2BIT;
            else if (ddsd.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED1)
                pcaps = DDPCAPS_1BIT;
            else
                pcaps = 0;

            if (pcaps) {
                TRY_D3D(DirectDraw->CreatePalette(pcaps, dstpal, &vidpal, nullptr));
                TRY_D3D(vidsurf->SetPalette(vidpal));
            }

            TRY_D3D(vidsurf->QueryInterface(IID_IDirect3DTexture, (LPVOID*)&vidtex));
            HRESULT err = vidtex->Load(memtex);
            if (err != DD_OK)
                return false;
            TRY_D3D(vidtex->GetHandle(Device, &vidhtex));

            // Set pointer to buffer in frame array (if more than one frame)
        }

        if (frames <= 1)    // If only one frame, put only video surface into main pointers and go
        {
            tex->surface = vidsurf;  // Sets to only vidsurf.. no frame array
            tex->texture = vidtex;
            tex->htexture = vidhtex;
            // Only 1 memsurf will be allocated, then freed after loop
        }
        else if (copyframes && tex->framesurfs) // If multiframe and using copy, put multi mem surf to array, and only one vid to main
        {
            tex->framesurfs[f] = memsurf;
            tex->frametexs[f] = memtex;
            tex->framehtexs[f] = memhtex;
            memsurf = nullptr;     // Causes memsurf to be reallocated next frame
            memtex = nullptr;
            memhtex = nullptr;
            tex->surface = vidsurf; // Sets to single vidsurf allocated
            tex->texture = vidtex;
            tex->htexture = vidhtex;
        }
        else if (tex->framesurfs)           // If multiframe and not copy, put multi vid surf to array and first vid surf to main
        {
            tex->framesurfs[f] = vidsurf;
            tex->frametexs[f] = vidtex;
            tex->framehtexs[f] = vidhtex;
            vidsurf = nullptr;     // Causes new vidsurf to be reallocated next frame
            vidtex = nullptr;
            vidhtex = nullptr;
            tex->surface = tex->framesurfs[0];  // Set to very first vidsurf
            tex->texture = tex->frametexs[0];
            tex->htexture = tex->framehtexs[0];
            // Only 1 memsurf will be allocated, then freed after loop
        }
    }

    if (memtex)     // If these are still here now, delete them
    {
        RELEASE(memtex);
        RELEASE(memsurf);
    }

    if (dstpal)
        delete dstpal;

    return true;
}

// Releases texture surface and texture object
void T3DImagery::RemoveTexture(int32_t texnum)
{
    PS3DTex t = &textures[texnum];

    if (t->numframes <= 1 || t->copyframes)
    {
        RELEASE(t->texture);
        RELEASE(t->surface);
    }
    t->texture = nullptr;
    t->surface = nullptr;
    t->htexture = nullptr;
    if (t->framesurfs)
    {
        for (int32_t c = 0; c < t->numframes; c++)
        {
            RELEASE(t->frametexs[c]);
            RELEASE(t->framesurfs[c]);
        }
        delete t->framesurfs;
        delete t->frametexs;
        delete t->framehtexs;
        t->framesurfs = nullptr;
        t->frametexs = nullptr;
        t->framehtexs = nullptr;
    }
    t->numframes = 0;
    t->framenum = 0;

    textures.Remove(texnum);
}

// Returns number of textures
int32_t T3DImagery::NumTextures()
{
    return textures.NumItems();
}

// Gets the current texture info
void T3DImagery::GetTexture(int32_t texnum, PS3DTex tex)
{
    if (texnum < 0 || texnum >= textures.NumItems() || !textures.Used(texnum))
        return;

    memcpy(tex, &textures[texnum], sizeof(S3DTex));
}

D3DTEXTUREHANDLE T3DImagery::GetTextureHandle(int32_t texnum)
{
    if (texnum < 0 || texnum >= textures.NumItems() || !textures.Used(texnum))
        return nullptr;

    return textures[texnum].htexture;
}

// Kill the textures
void T3DImagery::ClearTextures()
{
    for (int32_t c = 0; c < textures.NumItems(); c++)
    {
        RemoveTexture(c);
    }

    textures.Clear();
}

// Gets the current texture frame
int32_t T3DImagery::GetTextureFrame(int32_t texnum)
{
    if (texnum < 0 || texnum >= textures.NumItems() || !textures.Used(texnum))
        return 0;

    return textures[texnum].framenum;
}

// Copies texture data for given texture frame
bool T3DImagery::SetTextureFrame(int32_t texnum, int32_t framenum)
{
    if (texnum < 0 || texnum >= textures.NumItems() || !textures.Used(texnum))
        return false;

    if (textures[texnum].numframes <= 1)
        return true;

    if (framenum > textures[texnum].numframes)
        framenum = framenum % textures[texnum].numframes;

    if (textures[texnum].framenum == framenum) // Texture already set to correct frame
        return true;

    if (!textures[texnum].copyframes)
    {
        textures[texnum].surface = textures[texnum].framesurfs[framenum];
        textures[texnum].texture = textures[texnum].frametexs[framenum];
        textures[texnum].htexture = textures[texnum].framehtexs[framenum];
        textures[texnum].framenum = framenum;
        return true;
    }

    HRESULT err = textures[texnum].texture->Load(textures[texnum].frametexs[framenum]);
    if (err != DD_OK)
        return false;

    textures[texnum].framenum = framenum;

    return true;
}

// Returns true if any surfaces are lost
bool T3DImagery::SurfacesLost()
{
    for (int32_t c = 0; c < textures.NumItems(); c++)
    {
        if (textures.Used(c) && 
            textures[c].surface && 
            textures[c].surface->IsLost() == DDERR_SURFACELOST)
              return true;
    }

    return false;
}

// If SurfacesLost() reports true, is called to cause the surfaces to be reloaded.
void T3DImagery::RestoreSurfaces()
{
    if (!SurfacesLost())
        return;

    Restore();
}

// *****************************
// * General Purpose Functions *
// *****************************

// Finds a tag in the tag list given the tag name, state, and optional frame.  If
// state or frame are less < 0, will find any state, or any frame.  Optional
// foundstate and foundframe will return the state and frame actually found.
char *T3DImagery::FindTag(char *name, int32_t state, int32_t frame,
    int32_t *foundstate, int32_t *foundframe)
{
    for (int32_t c = 0; c < tags.NumItems(); c++)
    {
        S3DTag &tag = tags[c];

        if (state >= 0 && tag.state > state ||
           (frame >= 0 && tag.state == state && tag.frame > frame))
            break;

        if ((state >= 0 && tag.state != state) ||
            (frame >= 0 && tag.frame != frame) ||
            stricmp(tag.name, name) != 0)
          continue;
          
        if (foundstate)
            *foundstate = tag.state;
        if (foundframe)
            *foundframe = tag.frame;
        
        return tag.str;
    }
    
    return nullptr;
}

// Resets the extents registers in the 3D renderer to default values
void T3DImagery::ResetExtents()
{
    D3DCLIPSTATUS cs;
    cs.dwFlags = D3DCLIPSTATUS_EXTENTS2;
    cs.dwStatus = 0;
    cs.minx = cs.miny = cs.minz = +9999.0f;
    cs.maxx = cs.maxy = cs.maxz = -9999.0f;
    HRESULT ok = Scene3D.SetClipStatus(&cs);
}

// Retrieves the extents of the imagery drawn since the ClearExtents() function was called
void T3DImagery::GetExtents(LPD3DRECT extents)
{
    D3DCLIPSTATUS cs;
    cs.dwFlags = D3DCLIPSTATUS_EXTENTS2;
    Scene3D.GetClipStatus(&cs);

    extents->x1 = (int32_t)cs.minx;
    extents->y1 = (int32_t)cs.miny;
    extents->x2 = (int32_t)cs.maxx;
    extents->y2 = (int32_t)cs.maxy;
}

void T3DImagery::AddUpdateRect(LPD3DRECT extents, int32_t uflags)
{
    if (extents->x2 < 0 || extents->x1 >= WIDTH ||
        extents->y2 < 0 || extents->y1 >= HEIGHT ||
        extents->x2 - extents->x1 <= 0 ||
        extents->y2 - extents->y1 <= 0)
        return;

    SRect r;
    r.left = (int32_t)extents->x1;
    r.top = (int32_t)extents->y1;
    r.right = (int32_t)extents->x2;
    r.bottom = (int32_t)extents->y2;

    if (!NoUpdateRects)
        Display->AddUpdateRect(r.left, r.top, r.right - r.left + 1, r.bottom - r.top + 1, uflags);
}

// This function sets the maximum bounding box for a state given the latest extents rect
void T3DImagery::UpdateBoundingRect(PTObjectInstance oi, int32_t state, LPD3DRECT extents)
{
    if (extents->x2 <= 0 || extents->x1 <= 0 || extents->x1 >= WIDTH ||
        extents->y2 <= 0 || extents->y1 <= 0 || extents->y1 >= HEIGHT ||
        extents->x2 - extents->x1 <= 0 ||
        extents->y2 - extents->y1 <= 0)
        return;

    S3DPoint p;
    oi->GetScreenPos(p);
    SRect nr, r;
    p.x = p.x - MapPane.GetScrollX() + MapPane.GetPosX(); // Get screen relative position
    p.y = p.y - MapPane.GetScrollY() + MapPane.GetPosY();
    r.left = -GetRegX(state);
    r.top = -GetRegY(state);
    r.right = r.left + GetWidth(state) - 1;
    r.bottom = r.top + GetHeight(state) - 1;
    nr.left = (int32_t)extents->x1 - p.x;
    nr.top = (int32_t)extents->y1 - p.y;
    nr.right = (int32_t)extents->x2 - p.x;
    nr.bottom = (int32_t)extents->y2 - p.y;
    if (nr.left < r.left)
        r.left = nr.left;
    if (nr.top < r.top)
        r.top = nr.top;
    if (nr.right > r.right)
        r.right = nr.right;
    if (nr.bottom > r.bottom)
        r.bottom = nr.bottom;
    SetReg(state, -r.left, -r.top, 0);
    SetWidthHeight(state, r.right - r.left + 1, r.bottom - r.top + 1);
}

// Refreshes Z buffer
void T3DImagery::RefreshZBuffer(PTObjectInstance oi)
{
    SRect r;

  // Draw ZBuffer behind character when NoScrollZBuffer is on.
  // NoScrollZBuffer causes the update system to NOT update the screen ZBuffer, which
  // means that there is garbage in there when the 3D objects draw to it.  This little
  // function copies only the rectangle of the ZBuffer that this 3D object needs to the
  // screen so that the zbuffer is correct for the object when it draws.
    GetScreenRect(oi, r);
    Scene3D.RestoreZBuffer(r);
}


// Gets the current motion for the given state,frame or returns false if invalid
// state,frame or no motion available.
bool T3DImagery::GetMotion(int32_t state, int32_t frame, 
    int32_t &dist, int32_t &vert, int32_t &ang, int32_t &rotx, int32_t &roty, int32_t &rotz)
{
    if ((uint32_t)state >= (uint32_t)GetHeader()->numstates ||
        (uint32_t)frame >= (uint32_t)GetHeader()->states[state].frames ||
        !motion[state])
    {
        dist = vert = ang = rotx = roty = rotz = 0;
        return false;
    }

    PSMotionData md = &(motion[state][frame]); 

    dist = md_dist(*md);
    vert = md_vert(*md);
    ang = md->ang;
    rotx = md->rotx;
    roty = md->roty;
    rotz = md->rotz;

    return true;
}

// Sets the motion for an object based on its state, frame, and 3D motion data
void T3DImagery::SetObjectMotion(PTObjectInstance inst)
{
    int32_t state = inst->GetState();
    int32_t frame = inst->GetFrame();
    int32_t oldstate = inst->GetPrevState();
    int32_t oldframe = inst->GetPrevFrame();

    if (MeshInitialized() && 
        !(GetAniFlags(state) & AF_NOMOTION) && 
        !inst->CommandDone())
    {
        if ((state != oldstate) && (GetAniFlags(state) & AF_ROOT))
            inst->ClearAccum(); // Clear accumulator if we went to the root state

        int32_t dist, vert, ang, rotx, roty, rotz;
        bool motion = GetMotion(state, frame, dist, vert, ang, rotx, roty, rotz);
        if (motion)
        {
            ang = (ang + inst->GetFace()) & 255;
            rotx = (rotx + inst->GetRotateX()) & 255;
            roty = (roty + inst->GetRotateY()) & 255;
            rotz = (rotz + inst->GetRotateZ()) & 255;
            inst->SetMoveDist(dist);        // Set distance for next move
            inst->SetMoveVert(vert);        // Set vertical distance for next move
            inst->SetMoveAngle(ang);        // Sets movement angle
            inst->SetRotateX(rotx);         // Rotate X
            inst->SetRotateY(roty);         // Rotate Y
            inst->SetRotateZ(rotz);         // Rotate Z (also known as Face!)
        }
    }
    else
        inst->Halt();
}

// ****************** NORMAL IMAGERY STUFF **********************

// Returns the inventory image for the item
PTBitmap T3DImagery::GetInvImage(int32_t state, int32_t num)
{
    if (!meshinitialized)
        if (!InitializeMesh((PS3DImageryBody)GetBody()))
            FatalError("Unable to initialize 3D imagery");

    if (!icons)
        return nullptr;
    else
        return icons[state].invitem;
}

// Get inventory animation for state
PTAnimation T3DImagery::GetInvAnimation(int32_t state)
{
    if (!meshinitialized)
        InitializeMesh((PS3DImageryBody)GetBody());

    if (!icons)
        return nullptr;
    else
        return icons[state].invanim;
}

PTObjectAnimator T3DImagery::NewObjectAnimator(PTObjectInstance oi)
{
  // If mesh not initialized, attempt to initialize it now
    if (!meshinitialized)
        if (!InitializeMesh((PS3DImageryBody)GetBody()))
            FatalError("Unable to initialize 3D imagery for %s", oi->GetName());

  // Create the animator
    char *name = oi->GetTypeName();
    PT3DAnimatorBuilder builder = T3DAnimatorBuilder::GetBuilder(name);

    if (builder == &T3DAnimatorBuilderInstance)
    {
        name = oi->GetClassName();
        builder = T3DAnimatorBuilder::GetBuilder(name);
    }

    return (PTObjectAnimator)builder->Build(oi);
}

// Returns whether or not an animator is necessary
bool T3DImagery::NeedsAnimator(PTObjectInstance)
{
    return true;  // 3D imagery always needs an animator
}

// *******************************************************************************
// * T3DControllerBuilder - Used to register and automatically build controllers *
// *******************************************************************************

int32_t T3DControllerBuilder::numconttypes = 0;
PT3DControllerBuilder T3DControllerBuilder::builders[MAX3DCONTROLLERTYPES];

T3DControllerBuilder::T3DControllerBuilder(char *name)
{
    if (numconttypes < MAX3DCONTROLLERTYPES)
        builders[numconttypes++] = this;

    controllername = _strdup(name);
}

PT3DControllerBuilder T3DControllerBuilder::GetBuilder(char *name)
{
  // Eliminate standard animation tags
    if (!stricmp(name, "play") || !stricmp(name, "beg") || !stricmp(name, "end"))
        return nullptr;

  // Find controller tags
    for (int32_t i = 0; i < numconttypes; i++)
        if (stricmp(name, builders[i]->controllername) == 0)
            return builders[i];

    return nullptr;
}


// *********************************************************************
// * T3DController - Controls Animated Effects Based on Animation Tags *
// *********************************************************************

// 3D Controllers allow animators to add special effects to their animations
// by adding 'controller' tags into the animation note track.  When the 
// animation is loaded, a controller is instantiated for every recognized
// controller tag in the animation note track, and each controller parses
// its own parameters.  Controllers are useful for adding 'procedural'
// animation such as scrolling textures, morphing points, blood, etc.

// Parses the parameters for a controller
bool T3DController::ParseParams(TToken &t)
{
    while (t.Type() != TKN_EOF)
    {
        if (t.Type() != TKN_IDENT)
            return false;

        char param[80];
        strncpyz(param, t.Text(), 80);
        t.WhiteGet();

        if (t.Is("="))
            t.WhiteGet();

        if (!ParseItem(param, t))
            return false;
            
        if (!(t.Is(",") || t.Type() == TKN_EOF))
            return false;

        if (t.Is(","))
            t.WhiteGet();
    }

    return true;
}

// Parses a single parameter item in the parameter list
bool T3DController::ParseItem(char *name, TToken &t)
{
    if (!stricmp(name, "obj"))  // Get object list for this controller
    {
        if (t.Is("("))
        {
            t.WhiteGet();
            while (t.Type() != TKN_EOF && !t.Is(")"))
            {
                if (t.Type() != TKN_IDENT && t.Type() != TKN_TEXT)
                    return false;

                int32_t objnum = animator->GetObjectNum(t.Text());
                if (objnum >= 0)
                    animobjs.Add(animator->GetObject(objnum));

                t.WhiteGet();

                if (t.Is(","))
                    t.WhiteGet();
            }
            if (t.Is(")"))
                t.WhiteGet();
        }
        else if (t.Type() == TKN_IDENT || t.Type() == TKN_TEXT)
        {
            int32_t objnum = animator->GetObjectNum(t.Text());
            if (objnum >= 0)
                animobjs.Add(animator->GetObject(objnum));
            t.WhiteGet();
        }
        else
            return false;

        return true;
    }

    return false;
}

// Called to initialize the controller and parse controller parameters.
bool T3DController::Initialize(char *params)
{
  // Setup stream and token
    TStringParseStream s(params, strlen(params));
    TToken t(s);
    
  // Parse parameters
    t.WhiteGet();
    if (ParseParams(t) != true)
        return false;

    return true;
}

// Kills the controller
void T3DController::Close()
{
    animobjs.Clear();
}

// *******************************
// * T3DAnimatorBuilder Funtions *
// *******************************

int32_t T3DAnimatorBuilder::numanimtypes = 0;
PT3DAnimatorBuilder T3DAnimatorBuilder::builders[MAX3DANIMATORTYPES];

T3DAnimatorBuilder::T3DAnimatorBuilder(char *name)
{
    if (numanimtypes < MAX3DANIMATORTYPES)
        builders[numanimtypes++] = this;

    animatorname = _strdup(name);
}

T3DAnimatorBuilder::T3DAnimatorBuilder()
{
    if (numanimtypes < MAX3DANIMATORTYPES)
        builders[numanimtypes++] = this;

    animatorname = "default";
}

PT3DAnimator T3DAnimatorBuilder::Build(PTObjectInstance oi)
{
    return new T3DAnimator(oi);
}

PT3DAnimatorBuilder T3DAnimatorBuilder::GetBuilder(char *name)
{
    for (int32_t i = 0; i < numanimtypes; i++)
        if (stricmp(name, builders[i]->animatorname) == 0)
            return builders[i];

    return &T3DAnimatorBuilderInstance;  // Return default builder (for T3DAnimator object)
}

// ************************
// * T3DAnimator Funtions *
// ************************

T3DAnimator::~T3DAnimator()
{
    Close();
}

// Initializes animator and calls the SetupObjects() function to setup
void T3DAnimator::Initialize()
{
    TObjectAnimator::Initialize();

    pos.x = 100000.0f;
    pos.y = 100000.0f;
    pos.z = 100000.0f;
    rot.x = 0.0f;
    rot.y = 0.0f;
    rot.z = 0.0f;
    changed = true;
//  memset(&worldextents, 0, sizeof(S3DRect));
//  bpsaved = false;

    flags = ANI3D_ADDUPDATERECT | ANI3D_CACHEEXECUTEBUFFER;
    
    inst->SetFlags(OF_MOVING);          // don't bother refreshing background

  // Setup 3DAnimObj list now
    SetupObjects();

  // Create controller objects
    contprevstate = -1;
    RefreshControllers(inst->GetState());

    animid = Scene3D.AddAnimator(this);
}

// Deletes all objects, etc.
void T3DAnimator::Close()
{
    controllers.DeleteAll();

    Scene3D.RemoveAnimator(animid);

    for (int32_t c = 0; c < animobjs.NumItems(); c++)
    {
        RemoveObject(c);
    }

    animobjs.DeleteAll();

    TObjectAnimator::Close();
}

// Refresh controllers
void T3DAnimator::RefreshControllers(int32_t newstate)
{
    int32_t c;

  // We're already doing this state?
    if (newstate == contprevstate)
        return;

  // Remove controllers for other states
    for (c = controllers.NumItems() - 1; c >= 0; c--)
    {
        int32_t tagstate = controllers[c]->TagState();
        if (tagstate != -1 && tagstate != newstate)
            RemoveController(c);
    }

  // Add controllers for this state
    int32_t numtags = Get3DImagery()->NumTags();
    for (c = 0; c < numtags; c++)
    {
        PS3DTag tag = Get3DImagery()->GetTag(c);
        if (!((contprevstate == -1 && tag->state == -1) || // First time.. do ALL-STATES controllers
               tag->state == newstate))                    // Other times, do only controllers for this state
            continue;

        PT3DControllerBuilder cbuilder = T3DControllerBuilder::GetBuilder(tag->name);
        if (cbuilder)
        {
            PT3DController cont = cbuilder->Build(tag->state, tag->frame,
                this, Get3DImagery(), inst);
            if (cont)
            {
                if (!cont->Initialize(tag->str))
                    delete cont;
                else
                    controllers.Add(cont);
            }
        }
    }

  // Set the previous state values
    contprevstate = newstate;
}

void T3DAnimator::RecordNewExtents(PTObjectInstance oi, int32_t state, bool frontonly)
{
    if (state < 0)
    {
        for (int32_t i = 0; i < oi->NumStates(); i++)
        {
            PSImageryStateHeader st = image->GetState(i);
            st->regx = st->regy = 0;
            st->width = st->height = 0;
        }

        flags |= ANI3D_UPDATEALLSTATES;
        state = 0;
    }
    else
    {
        PSImageryStateHeader st = image->GetState(state);
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

    // from here on out the Animate() function will handle the updates
}

void T3DAnimator::AnimateResetBoundRect()
{
    int32_t face = inst->GetFace();
    if (flags & ANI3D_UPDATEFRONTONLY)
        face += 256;
    else
        face += 64;
    inst->Face(face & 255);
    if (face < 1 || face >= 256)
    {
        bool done = false;
        if (flags & ANI3D_UPDATEALLSTATES)
        {
            if (flags & ANI3D_UPDATEFRONTONLY)
                inst->Face(0);
            else
                inst->Face(32);

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
 // Refresh controller list for the current state
    RefreshControllers(inst->GetState());

 // Pulse all controllers
    for (int32_t c = 0; c < controllers.NumItems(); c++)
        controllers[c]->Pulse();

  // Now try to play a sound
    ((PT3DImagery)image)->PlaySound(inst, state, frame);
}

void T3DAnimator::Animate(bool draw)
{
    int32_t oldframe = frame;
    int32_t oldstate = state;
//  bpsaved = false;

    TObjectAnimator::Animate(draw);

    S3DPoint p;
    inst->GetPos(p);

    changed = false;
    if (pos.x != p.xf || pos.y != p.yf || pos.z != p.zf)
    {
        pos.x = p.xf;
        pos.y = p.yf;
        pos.z = FIX_Z_VALUE(p.z);
        changed = true;
    }

    float newrotz = inst->GetFace(f) / 256.0f * M_2PIf;
    if (abs(rot.z - newrotz) > 0.0001)
    {
        rot.z = newrotz;
        changed = true;
    }

    // if recording extents, continue spinning 'em around
    if (flags & ANI3D_UPDATEBOUNDRECT && inst->CommandDone())
        AnimateResetBoundRect();
}

// Returns true if any surfaces are lost for this animator
bool T3DAnimator::SurfacesLost()
{
    return ((PT3DImagery)image)->SurfacesLost();
}

// If SurfacesLost() reports true, is called to cause the surfaces to be reloaded.
void T3DAnimator::RestoreSurfaces()
{
    ((PT3DImagery)image)->RestoreSurfaces();
}

// Calculates an object matrix based on current position
void T3DAnimator::MakeMatrix(hmm_mat4* m)
{
    hmm_mat4 m2;

    D3DMATRIXClear(m);
    D3DMATRIXRotateZ(m, rot.z); // Yaw rotation
    D3DMATRIXClear(&m2);
    D3DMATRIXRotateX(&m2, rot.x);       // Pitch rotation
    MultiplyD3DMATRIX(m, m, &m2);
    D3DMATRIXClear(&m2);
    D3DMATRIXRotateY(&m2, rot.y);       // Roll rotation
    MultiplyD3DMATRIX(m, m, &m2);
    D3DMATRIXClear(&m2);
    D3DMATRIXTranslate(&m2, &pos);      // Translate
    MultiplyD3DMATRIX(m, m, &m2);
}

// Called to set up lighting, model, etc. for this object before virtual Render called
void T3DAnimator::PreRender()    
{
    /*
     * Execute the instruction buffer
     */
    S3DPoint curpos;
    inst->GetPos(curpos);
    Scene3D.LightAffectObject(curpos.x, curpos.y, curpos.z + LIGHTINGCHARHEIGHT); // Add 50 to get middle of guy

  // Make the object matrix for this frame
    if (changed)
        MakeMatrix(&matrix);

  // Sets the state info for next render (so interpolation will work
    ((PT3DImagery)image)->SetPrevState(inst->GetPrevState(), inst->GetPrevFrame());

  // Begins rendering
    ((PT3DImagery)image)->BeginRender((flags & ANI3D_CACHEEXECUTEBUFFER) != 0, false);

  // Resets the render extents rectangle so user can get size of object he just drew.
    ResetExtents();
}

// Causes the 3D imagery to be rendered to the screen.  Called between Pre and Post Render
bool T3DAnimator::Render()
{
    PS3DAnimObj objstack[MAXOBJECTS];  // Use this stack instead of a recursive function
    int32_t stackpos;
    int32_t c;

  // Render all controllers
    for (c = 0; c < controllers.NumItems(); c++)
        controllers[c]->Render();

  // Reset parents for this state (NOTE: assumes object order is same as 3DImage object order)
    for (c = 0; c < animobjs.NumItems(); c++)
    {
        if (!(animobjs[c]->flags & OBJ3D_PARENT))  // Don't reset if parent is being overrided
        {
            int32_t parent = ((PT3DImagery)image)->GetObjectParent(animobjs[c]->objnum, state);
            if ((uint32_t)parent < (uint32_t)animobjs.NumItems() && animobjs[parent]->objnum == parent)
                animobjs[c]->parent = animobjs[parent];
            else
                animobjs[c]->parent = nullptr;
        }
    }

  // Render mesh by textures..
    for (int32_t t = 0; t < Get3DImagery()->NumTextures() + 1; t++)
    {
      for (c = 0; c < animobjs.NumItems(); c++)
        animobjs[c]->flags &= ~OBJ3D_RENDERED;

    // Render objects by parent..
      for (c = 0; c < animobjs.NumItems(); c++)  // Go through all objects
      {
        if (!animobjs[c] || (animobjs[c]->flags & OBJ3D_RENDERED))
            continue;  // Already drawn this

        objstack[0] = animobjs[c];  // Current object is always the bottom of stack
        stackpos = 1;

       // Get the objects furthest undrawn parent
        PS3DAnimObj parent = animobjs[c]->parent;
        while (parent && !(parent->flags & OBJ3D_RENDERED) && stackpos < MAXOBJECTS)
        {
            objstack[stackpos++] = parent;
            parent = parent->parent;
        }

      // Draw from the undrawn parent down to the current object  
        while (stackpos)
        {
            stackpos--;
            PS3DAnimObj obj = objstack[stackpos];

            if (Get3DImagery()->IsHidden(obj->objnum, state))
                obj->flags |= OBJ3D_HIDE;

            Get3DImagery()->RenderObject(obj, state, frame, &matrix, t, t > 0);

            obj->flags |= OBJ3D_RENDERED;
        }

      }
    }

    return true;
}

// Called to restore lighting state, etc. etc.
void T3DAnimator::PostRender()
{
  // End rendering (save cached buffer if flags are set for it)
    ((PT3DImagery)image)->EndRender();

  // Update extents (if they haven't been updated yet)
  // NOTE: Extents need to be updated AFTER EndRender() is called so we don't reset the
  // execute buffer while it is being recorded.  If UpdateExtents is called during the
  // render, it will cause the current execute buffer to be rendered (flushed), and a fresh
  // empty one to be created, which is ok if we're just drawing stuff, but bad if we're
  // trying to build a cachable buffer.
    if (!WasUpdated())
        UpdateExtents();

  // Reset lights   
    Scene3D.ResetAllLights();

  // Kill any completed controllers
    for (int32_t c = 0; c < controllers.NumItems(); c++)
    {
        if (controllers[c]->KillMe())
            controllers.Collapse(c, true);
    }
}

// *****************************
// * Animator Object Functions *
// *****************************

// Sets up the objects to animate (simply uses whatever objects are in the imagery)
// Override this function to set up whatever objects you need for your effect, etc.
void T3DAnimator::SetupObjects()
{
    for (int32_t c = 0; c < Get3DImagery()->NumObjects(); c++)
    {
        PS3DAnimObj o = NewObject(c);
//      GetVerts(o, D3DVT_LVERTEX);
        AddObject(o);
    }

  // Initialize mesh hierarchy
    for (c = 0; c < animobjs.NumItems(); c++)
    {
        int32_t parent = ((PT3DImagery)image)->GetObjectParent(animobjs[c]->objnum, 0);
        if ((uint32_t)parent < (uint32_t)animobjs.NumItems() && animobjs[parent]->objnum == parent)
            animobjs[c]->parent = animobjs[parent];
        else
            animobjs[c]->parent = nullptr;
    }
}

// Creates a 3DAnimObj structure to be used with the T3DImagery RenderObject() function
// and initializes
PS3DAnimObj T3DAnimator::NewObject(int32_t objnum, int32_t flags)
{
    PS3DAnimObj obj = new S3DAnimObj;
    memset(obj, 0, sizeof(S3DAnimObj));
    obj->objnum = objnum;
    obj->parent = nullptr;

    S3DObj o;
    Get3DImagery()->GetObject(objnum, &o);

    obj->animtrack = min(objnum, Get3DImagery()->NumObjects() - 1);
    obj->primtype = D3DPT_TRIANGLELIST;
    obj->verttype = D3DVT_VERTEX; 
    obj->hmaterial = Get3DImagery()->GetMaterialHandle(o.material);
    for (int32_t c = 0; c < MAXTEXTURES; c++)
    {
        obj->htextures[c] = Get3DImagery()->GetTextureHandle(c);
        //>>>>>>>>>>>>>>>>>>>>>>>>
        S3DTex tex;
        Get3DImagery()->GetTexture(c, &tex);
        obj->surfaces[c] = tex.surface;
        //<<<<<<<<<<<<<<<<<<<<<<<<
    }

    if (flags & OBJ3D_COPYVERTS)
        GetVerts(obj, obj->verttype); 

    if (flags & OBJ3D_COPYFACES)
        GetFaces(obj);

    return obj;
}

void T3DAnimator::GetVerts(PS3DAnimObj obj, D3DVERTEXTYPE verttype)
{
    if (obj->verts)
        FreeVerts(obj);

    obj->flags |= (OBJ3D_VERTS | OBJ3D_COPYVERTS | OBJ3D_OWNSVERTS);
    obj->verttype = verttype;

    obj->numverts = Get3DImagery()->NumObjVerts(obj->objnum);
    if (obj->verttype == D3DVT_VERTEX)
        obj->verts = new hmm_vec3[obj->numverts];
    else if (obj->verttype == D3DVT_LVERTEX)
        obj->lverts = new D3DLVERTEX[obj->numverts];
    else if (obj->verttype == D3DVT_TLVERTEX)
        obj->tlverts = new D3DTLVERTEX[obj->numverts];
    Get3DImagery()->GetObjVerts(obj->objnum, obj->verts, 0, 0, obj->verttype);
}

void T3DAnimator::FreeVerts(PS3DAnimObj obj)
{
    if (obj->verts && (obj->flags & OBJ3D_OWNSVERTS))
        delete obj->verts;

    obj->flags &= ~(OBJ3D_VERTS | OBJ3D_COPYVERTS | OBJ3D_OWNSVERTS);
    obj->verts = nullptr;
    obj->numverts = 0;
}

void T3DAnimator::GetFaces(PS3DAnimObj obj)
{
    if (obj->faces)
        FreeFaces(obj);

    obj->flags |= (OBJ3D_FACES | OBJ3D_COPYFACES | OBJ3D_OWNSFACES);

    obj->numfaces = Get3DImagery()->NumObjFaces(obj->objnum);
    obj->faces = new S3DFace[obj->numfaces];
    Get3DImagery()->GetObjFaces(obj->objnum, obj->faces);
}

void T3DAnimator::FreeFaces(PS3DAnimObj obj)
{
    if (obj->faces && (obj->flags & OBJ3D_OWNSFACES))
        delete obj->faces;

    obj->flags &= ~(OBJ3D_FACES | OBJ3D_COPYFACES | OBJ3D_OWNSFACES);
    obj->faces = nullptr;
    obj->numfaces = 0;
}

// Adds a S3DAnimObj to this animators internal object array
// Note that the array owns the objects data, and will attempt to delete it
int32_t T3DAnimator::AddObject(PS3DAnimObj obj)
{
    return animobjs.Add(obj);
}

void T3DAnimator::RemoveObject(int32_t objnum)
{
    if (objnum < 0 || objnum >= MAX3DANIMOBJECTS || animobjs[objnum] == nullptr)
        return;

    if ((animobjs[objnum]->flags & OBJ3D_OWNSVERTS) && animobjs[objnum]->verts)
        delete animobjs[objnum]->verts;

    if ((animobjs[objnum]->flags & OBJ3D_OWNSFACES) && animobjs[objnum]->faces)
        delete animobjs[objnum]->faces;

    animobjs.Delete(objnum);
}

// Returns object number for first object based on the imagery object with given name
// For default SetupObjects(), the animator and imagery obj num will be the same.
int32_t T3DAnimator::GetObjectNum(char *name)
{
    for (int32_t c = 0; c < animobjs.NumItems(); c++)
    {
        if (animobjs[c] &&
          !stricmp(Get3DImagery()->GetObjectName(animobjs[c]->objnum), name))
            return c;
    }
    return -1;
}

// Returns object override data
PS3DAnimObj T3DAnimator::GetObject(int32_t objnum)
{
    if (objnum < 0 || objnum >= MAX3DANIMOBJECTS || animobjs[objnum] == nullptr)
        return nullptr;

    return animobjs[objnum];
}

// Returns true if object is enabled
bool T3DAnimator::IsObjectEnabled(int32_t objnum)
{
    if (objnum < 0 || objnum >= MAX3DANIMOBJECTS || animobjs[objnum] == nullptr)
        return false;

    return !(animobjs[objnum]->flags & OBJ3D_HIDE);
}


// Enables or disables object
void T3DAnimator::EnableObject(int32_t objnum, bool enable)
{
    if (objnum < 0 || objnum >= MAX3DANIMOBJECTS || animobjs[objnum] == nullptr)
        return;

    if (enable)
        animobjs[objnum]->flags &= ~OBJ3D_HIDE;
    else
        animobjs[objnum]->flags |= OBJ3D_HIDE;
}

bool T3DAnimator::GetObjectMatrix(int32_t objnum, hmm_mat4 *m)
{
    if (objnum < 0)
        return false;

    PS3DAnimObj animobj = GetObject(objnum);
    Get3DImagery()->CalcObjectMatrixCopy(animobj, inst->GetState(), inst->GetFrame(), m, 0);

    return true;
}

bool T3DAnimator::GetObjectPos(int32_t objnum, hmm_vec3 &v, hmm_vec3 *s)
{
    hmm_mat4 m;

    if (objnum < 0)
        return false;

    if (!GetObjectMatrix(objnum, &m))
        return false;

    hmm_vec3 p;
    if (s)
      { p.x = s->x; p.y = s->y; p.z = s->z; }
    else
      { p.x = p.y = p.z = 0.0f; }

    D3DMATRIXTransform(&m, &p, &v);
    return true;
}

bool T3DAnimator::GetObjectPos(char *objname, hmm_vec3 &v, hmm_vec3 *s)
{
    int32_t objnum = GetObjectNum(objname);

    if (objnum < 0)
        return false;

    return GetObjectPos(objnum, v, s);
}

bool T3DAnimator::GetObjectMapPos(int32_t objnum, S3DPoint &pos)
{
    hmm_vec3 v;

    if (!GetObjectPos(objnum, v))
        return false;

    pos.x = (int32_t)v.x;
    pos.y = (int32_t)v.y;
    pos.z = (int32_t)REV_FIX_Z_VALUE(v.z);

    return true;
}

bool T3DAnimator::GetObjectMapPos(char *objname, S3DPoint &pos)
{
    int32_t objnum = GetObjectNum(objname);

    if (objnum < 0)
        return false;

    return GetObjectMapPos(objnum, pos);
}

