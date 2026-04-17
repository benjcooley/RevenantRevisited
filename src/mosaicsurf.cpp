// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *              mosaicsurf.cpp - 3D Surface Include File                 *
// *************************************************************************

#include <windows.h>

#include "revenant.h"
#include "mosaicsurf.h"
#include "directdraw.h"
#include "ddsurface.h"
#include "multisurface.h"
#include "bitmap.h"
#include "bmsurface.h"

TMosaicSurface::TMosaicSurface()
{
    tilex = tiley = numtilex = numtiley = 0;
    flags = 0;
    tiles = nullptr;
}

TMosaicSurface::~TMosaicSurface()
{
    Close();
}

bool TMosaicSurface::Initialize(
    int32_t ntilex, int32_t ntiley, int32_t nnumtilex, int32_t nnumtiley, uint32_t ncreateflags)
{
    tilex = ntilex;
    tiley = ntiley;
    numtilex = nnumtilex;
    numtiley = nnumtiley;
    createflags = ncreateflags;

    if (createflags & MOSAICSURF_CLONEGRAPHICS)
        createflags &= ~(MOSAICSURF_SYSTEMMEM | 
            MOSAICSURF_VIDEOMEM | MOSAICSURF_VIDEOMEMONLY | MOSAICSURF_BMSURFACE |
            MOSAICSURF_8BIT | MOSAICSURF_16BIT | MOSAICSURF_24BIT | MOSAICSURF_32BIT);

    if (createflags & MOSAICSURF_CLONEZBUFFER)
        createflags &= ~(MOSAICSURF_ZBUFFER | MOSAICSURF_ZSYSTEMMEM | MOSAICSURF_ZVIDEOMEM | MOSAICSURF_ZVIDEOMEMONLY);

    if (createflags & MOSAICSURF_CLONENORMALS)
        createflags &= ~(MOSAICSURF_NORMALS | MOSAICSURF_NSYSTEMMEM | MOSAICSURF_NVIDEOMEM | MOSAICSURF_NVIDEOMEMONLY);

    if (!(createflags & (MOSAICSURF_BMSURFACE | MOSAICSURF_CLONEGRAPHICS))
       && !(createflags & (MOSAICSURF_SYSTEMMEM | MOSAICSURF_VIDEOMEM)))
        return false;  // Must be one or other

    if ((createflags & MOSAICSURF_ZBUFFER) 
       && !(createflags & (MOSAICSURF_ZSYSTEMMEM | MOSAICSURF_ZVIDEOMEM)))
        return false;  // Must be one or other

    if ((createflags & MOSAICSURF_NORMALS) 
       && !(createflags & (MOSAICSURF_NSYSTEMMEM | MOSAICSURF_NVIDEOMEM)))
        return false;  // Must be one or other

    bitsperpixel = 16;
    if (createflags & MOSAICSURF_8BIT)
    {
        bitsperpixel = 8;
        createflags |= MOSAICSURF_BMSURFACE;
    }
    else if (createflags & MOSAICSURF_16BIT)
    {
        bitsperpixel = 16;
    }
    else if (createflags & MOSAICSURF_24BIT)
    {
        bitsperpixel = 24;
        createflags |= MOSAICSURF_BMSURFACE;
    }
    else if (createflags & MOSAICSURF_32BIT)
    {
        bitsperpixel = 32;
        createflags |= MOSAICSURF_BMSURFACE;
    }
    int32_t bytesperpixel = bitsperpixel >> 3;

    int32_t freevidmem = GetFreeVideoMem();

    if (createflags & MOSAICSURF_VIDEOMEM)
    {
        freevidmem -= tilex * tiley * numtilex * numtiley * bytesperpixel;

        if (freevidmem < TEXTURERESERVE)
        {
            if (createflags & MOSAICSURF_VIDEOMEMONLY)
                return false;
            createflags &= ~MOSAICSURF_VIDEOMEM;
            createflags |= MOSAICSURF_SYSTEMMEM;
        }
    }

    if ((createflags & MOSAICSURF_ZBUFFER) && (createflags & MOSAICSURF_ZVIDEOMEM))
    {
        freevidmem -= tilex * tiley * numtilex * numtiley * 2;

        if (freevidmem < TEXTURERESERVE)
        {
            if (createflags & MOSAICSURF_ZVIDEOMEMONLY)
                return false;
            createflags &= ~MOSAICSURF_ZVIDEOMEM;
            createflags |= MOSAICSURF_ZSYSTEMMEM;
        }
    }

    if ((createflags & MOSAICSURF_NORMALS) && (createflags & MOSAICSURF_NVIDEOMEM))
    {
        freevidmem -= tilex * tiley * numtilex * numtiley * 2;

        if (freevidmem < TEXTURERESERVE)
        {
            if (createflags & MOSAICSURF_NVIDEOMEMONLY)
                return false;
            createflags &= ~MOSAICSURF_NVIDEOMEM;
            createflags |= MOSAICSURF_NSYSTEMMEM;
        }
    }
    
    int32_t vflags;
    if (createflags & MOSAICSURF_VIDEOMEM)
        vflags = VSURF_VIDEOMEM;
    else
        vflags = VSURF_SYSTEMMEM;

    int32_t zflags;
    if (createflags & MOSAICSURF_ZVIDEOMEM)
        zflags = VSURF_VIDEOMEM;
    else
        zflags = VSURF_SYSTEMMEM;

    int32_t nflags;
    if (createflags & MOSAICSURF_NVIDEOMEM)
        nflags = VSURF_VIDEOMEM;
    else
        nflags = VSURF_SYSTEMMEM;

    int32_t bmbits;
    if (createflags & MOSAICSURF_8BIT)
        bmbits = BM_8BIT;
    else if (createflags & MOSAICSURF_16BIT)
        bmbits = BM_16BIT;
    else if (createflags & MOSAICSURF_24BIT)
        bmbits = BM_24BIT;
    else if (createflags & MOSAICSURF_32BIT)
        bmbits = BM_32BIT;
    else
        bmbits = BM_16BIT;

    tiles = new TSurface*[numtilex * numtiley];

    TSurface* vsurf, zsurf, nsurf;
    TSurface **surf = tiles;

    int32_t x, y, offx, offy;
    for (y = 0, offy = 0; y < numtiley; y++, offy += tiley)
    {
      for (x = 0, offx = 0; x < numtilex; x++, surf++, offx += tiley)
      {
        vsurf = zsurf = nsurf = nullptr;

        if (!(createflags & MOSAICSURF_CLONEGRAPHICS))
        {
            if (createflags & MOSAICSURF_BMSURFACE)
                vsurf = new TBitmapSurface(tilex, tiley, bmbits);
            else
            {
                vsurf = new TSurface(tilex, tiley, vflags);
                if (vsurf->Stride() != tilex && NoWideBuffers)
                {
                    delete vsurf;
                    vflags = (vflags & (~(uint32_t)VSURF_VIDEOMEM)) | VSURF_SYSTEMMEM;
                    vsurf = new TSurface(tilex, tiley, vflags);
                }
            }
        }
        if (createflags & MOSAICSURF_ZBUFFER)
        {
            zsurf = new TSurface(tilex, tiley, zflags | VSURF_ZBUFFER); // Voodoo doesn't like this
            if (zsurf->Stride() != tilex && NoWideBuffers)
            {
                delete zsurf;
                zflags = (zflags & (~(uint32_t)VSURF_VIDEOMEM)) | VSURF_SYSTEMMEM;
                zsurf = new TSurface(tilex, tiley, zflags | VSURF_ZBUFFER); // Voodoo doesn't like this
            }
        }
//      if (createflags & MOSAICSURF_NORMALS)
//          nsurf = new TSurface(tilex, tiley, nflags, stride);

        *surf = new TMultiSurface(vsurf, zsurf, nsurf, true);
      }
    }

    width        = tilex * numtilex;
    height       = tiley * numtiley;
    stride       = width;
    bitsperpixel = tiles[0]->BitsPerPixel();
    flags        = tiles[0]->flags;

    Reset();

    return true;
}

// Initializes a cloned mosaic surface
// 
// A Cloned surface uses the same surface buffers as the original surface, but stores
// its own local origin and clipping rectangle.  This is very useful for... say.. 
// a surface used simultaneously by two different threads.

bool TMosaicSurface::Initialize(PTMosaicSurface clone, uint32_t ncreateflags)
{
    Initialize(clone->tilex, clone->tiley, 
        clone->numtilex, clone->numtiley, ncreateflags | MOSAICSURF_ISCLONE);

    TSurface* vsurf, zsurf, nsurf;
    TMultiSurface **surf = (TMultiSurface **)tiles;

    int32_t x, y, offx, offy;
    for (y = 0, offy = 0; y < numtiley; y++, offy += tiley)
    {
      for (x = 0, offx = 0; x < numtilex; x++, surf++, offx += tiley)
      {
    
        PTMultiSurface tile = clone->GetTile(x, y);
        vsurf = zsurf = nsurf = nullptr;

        if ((createflags & MOSAICSURF_CLONEGRAPHICS) && tile->GetGraphicsBuffer())
        {
            if (clone->createflags & MOSAICSURF_BMSURFACE)
                vsurf = new TBitmapSurface(
                    ((PTBitmapSurface)tile->GetGraphicsBuffer())->GetBitmap());
            else
                vsurf = new TSurface(tile->GetGraphicsBuffer()->GetSGImage());
            (*surf)->SetGraphicsBuffer(vsurf);
        }
        if ((createflags & MOSAICSURF_CLONEZBUFFER) && tile->GetZBuffer())
        {
            zsurf = new TSurface(tile->GetZBuffer()->GetSGImage());
            (*surf)->SetZBuffer(zsurf);
        }
        if ((createflags & MOSAICSURF_CLONENORMALS) && tile->GetNormalBuffer())
        {
            nsurf = new TSurface(tile->GetNormalBuffer()->GetSGImage());
            (*surf)->SetNormalBuffer(nsurf);
        }

      }
    }

    width        = tilex * numtilex;
    height       = tiley * numtiley;
    stride       = width;
    bitsperpixel = tiles[0]->BitsPerPixel();
    flags        = tiles[0]->flags;

    Reset();

    return true;
}

void TMosaicSurface::Close()
{
    if (tiles == nullptr)
        return;

    TSurface **surf = tiles;
    for (int32_t y = 0; y < numtiley; y++)
    {
      for (int32_t x = 0; x < numtilex; x++, surf++)
      {
        delete (*surf);
      }
    }

    delete tiles;
    tiles = nullptr;
}

void TMosaicSurface::SetOrigin(int32_t x, int32_t y)
{
    TSurface::SetOrigin(x, y);

    TSurface **surf = tiles;
    int32_t tx, ty, offx, offy;
    for (ty = 0, offy = 0; ty < numtiley; ty++, offy += tiley)
    {
      for (tx = 0, offx = 0; tx < numtilex; tx++, surf++, offx += tilex)
      {
        (*surf)->SetOrigin(x - offx, y - offy);
      }
    }
}

void TMosaicSurface::SetClipRect(int32_t x, int32_t y, int32_t w, int32_t h)
{
    TSurface::SetClipRect(x, y, w, h);

    TSurface **surf = tiles;
    int32_t tx, ty, offx, offy;
    for (ty = 0, offy = 0; ty < numtiley; ty++, offy += tiley)
    {
      for (tx = 0, offx = 0; tx < numtilex; tx++, surf++, offx += tilex)
      {
        (*surf)->SetClipRect(0, 0, width, height); // Use standard clip rect for subtiles
      }
    }
}

// NOTE: Mosaic surfaces don't support CLIP_WRAP
void TMosaicSurface::SetClipMode(int32_t mode)
{
    TSurface::SetClipMode(mode);

    // Note: Tile surfaces are ALWAYS CLIP_EDGES, wrap clipping is implemented by this object
    // for tiles, as a wrapped blits can span multiple tiles
    TSurface **surf = tiles;
    for (int32_t ty = 0; ty < numtiley; ty++)
    {
      for (int32_t tx = 0; tx < numtilex; tx++, surf++)
      {
        (*surf)->SetClipMode(CLIP_EDGES);       // Use standard clip mode for subtiles
      }
    }
}

// Copies specified bitmap to current bitmap
bool TMosaicSurface::ParamDraw(PSDrawParam dp, PTBitmap bitmap)
{
    SDrawParam dpv = *dp;
    bool drew = false;
    SDrawParam dparray[4];
    SDrawBlock db;
    int32_t numrects;

    if (!ParamDrawSetup(dpv, bitmap))
        return false;

    memset(&db, 0, sizeof(SDrawBlock));

    db.dbufwidth  = width;
    db.dbufheight = height;
    db.dstride    = stride;

    if (bitmap)
    {
        db.sbufwidth  = bitmap->width;
        db.sbufheight = bitmap->height;
        db.sstride    = bitmap->width;
    }
    else
    {
        db.sbufwidth = db.sbufheight = db.sstride = 0;
    }

    if (!Clip(&db, &dpv, dparray, numrects))
        return false;

    for (int32_t cliploop = 0; cliploop < numrects; cliploop++)
    {
        RSDrawParam dpa = dparray[cliploop];
                    
        dpa.drawmode &= ~(uint32_t)(DM_WRAPCLIP | DM_WRAPCLIPSRC);

        TSurface **surf = tiles;
        for (int32_t ty = 0; ty < numtiley; ty++)
        {
          for (int32_t tx = 0; tx < numtilex; tx++, surf++)
          {
            drew |= (*surf)->ParamDraw(&dpa, bitmap);
          }
        }
    }

    return drew;
}

// Blits from surface to surface. RECT sets size of blit. 
// X & Y specifies dest. origin
bool TMosaicSurface::ParamBlit(PSDrawParam dp, TSurface* surface, int32_t ddflags, LPDDBLTFX fx)
{
    SDrawParam dpv = *dp;
    bool drew = false;
    SDrawParam dparray[4];
    SDrawBlock db;
    int32_t numrects;

  // Allow special case of blitting from one mosaic surface with identical size and tile
  // layout to another!!
    PTMosaicSurface mosaicsrc = nullptr;
    if (surface && surface->SurfaceType() == SURFACE_MOSAIC)
    {
        mosaicsrc = (PTMosaicSurface)surface;
        if (mosaicsrc->tilex != tilex || mosaicsrc->tiley != tiley ||
            mosaicsrc->numtilex != numtilex || mosaicsrc->numtiley != numtiley)
                FatalError("Attempt to blit between non-identical mosaic surfaces");
    }

    if (!ParamBlitSetup(dpv, surface, ddflags, fx))
        return false;

    memset(&db, 0, sizeof(SDrawBlock));

    db.dbufwidth  = width;
    db.dbufheight = height;
    db.dstride    = stride;

    if (surface)
    {
        db.sbufwidth  = surface->Width();
        db.sbufheight = surface->Height();
        db.sstride    = surface->Stride();
    }
    else
    {
        db.sbufwidth = db.sbufheight = db.sstride = 0;
    }

    if (!Clip(&db, &dpv, dparray, numrects))
        return false;

    int32_t soriginx, soriginy;
    surface->GetOrigin(soriginx, soriginy);

    TSurface* src = surface;
    for (int32_t cliploop = 0; cliploop < numrects; cliploop++)
    {
        RSDrawParam dpa = dparray[cliploop];
        
      // Re-add originx and originy since Clip ParamBlitSetup will subtract these out               
        dpa.sx -= soriginx;
        dpa.sy -= soriginy;

        dpa.drawmode &= ~(uint32_t)(DM_WRAPCLIP | DM_WRAPCLIPSRC);

        TSurface **surf = tiles;
        for (int32_t ty = 0; ty < numtiley; ty++)
        {
          for (int32_t tx = 0; tx < numtilex; tx++, surf++)
          {
            if (mosaicsrc)
                src = mosaicsrc->GetTile(tx, ty);

            drew |= (*surf)->ParamBlit(&dpa, src, ddflags, fx);
          }
        }
    }

    return drew;
}

// Blits from surface to surface. RECT sets size of blit. 
// X & Y specifies dest. origin
bool TMosaicSurface::ParamGetBlit(PSDrawParam dp, TSurface* surface, int32_t ddflags, LPDDBLTFX fx)
{
    SDrawParam dpv = *dp;
    bool drew = false;
    SDrawParam dparray[4];
    SDrawBlock db;
    int32_t numrects;

  // Get blits always have two surfaces
    if (!surface)
        return false;

  // Allow special case of blitting from one mosaic surface with identical size and tile
  // layout to another!!
    PTMosaicSurface mosaicdest = nullptr;
    if (surface->UseGetBlit())
    {
        bool err = false;
        if (surface->SurfaceType() != SURFACE_MOSAIC)
            err = true;
        if (!err)
        {
            mosaicdest = (PTMosaicSurface)surface;
            if (mosaicdest->tilex != tilex || mosaicdest->tiley != tiley ||
                mosaicdest->numtilex != numtilex || mosaicdest->numtiley != numtiley)
                err = true;
        }
        if (err)
            FatalError("Attempt to blit between non-identical mosaic surfaces");
    }

    if (!surface->ParamBlitSetup(dpv, this, ddflags, fx))
        return false;

    memset(&db, 0, sizeof(SDrawBlock));

    db.dbufwidth  = surface->Width();
    db.dbufheight = surface->Height();
    db.dstride    = surface->Stride();

    db.sbufwidth  = width;
    db.sbufheight = height;
    db.sstride    = stride;

    if (!Clip(&db, &dpv, dparray, numrects))
        return false;

    TSurface* dest = surface;
    for (int32_t cliploop = 0; cliploop < numrects; cliploop++)
    {
        RSDrawParam dpa = dparray[cliploop];

        dpa.drawmode &= ~(uint32_t)(DM_WRAPCLIP | DM_WRAPCLIPSRC);
                    
        TSurface **surf = tiles;
        for (int32_t ty = 0; ty < numtiley; ty++)
        {
          for (int32_t tx = 0; tx < numtilex; tx++, surf++)
          {
            if (mosaicdest)
                dest = mosaicdest->GetTile(tx, ty);

            drew |= dest->ParamBlit(&dpa, *surf, ddflags, fx);
          }
        }
    }

    return drew;
}

