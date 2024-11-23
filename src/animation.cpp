// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *            animation.cpp - EXILE Animation Objects File               *
// *************************************************************************

#include "revenant.h"
#include "display.h"
#include "bitmap.h"
#include "graphics.h"
#include "resource.h"
#include "animation.h"

TAnimation::TAnimation()
{
}

TAnimation::~TAnimation()
{
}

PTAnimation TAnimation::Load(int32_t resource)
{
    return (PTAnimation)LoadResource("animation", resource);
}    

bool TAnimation::Decompress(PSDrawBlock db, int32_t frame, bool Transparent)
{
    return true;
}

void TAnimation::Put(PTSurface surface, void *decbuf, int32_t frame, PSDrawParam dp)
{
    SDrawBlock  db;
    SDrawParam  dp2 = *dp;

    if (surface == nullptr || (decbuf == nullptr && (flags & AF_INTERFRAME))) 
        return;

    if (frame > numframes) 
        return;

    PTBitmap framebmp = frames[frame].bitmap;
    surface->ParamDraw(dp, framebmp);
    return;
    
    if (flags & AF_INTERFRAME) 
    {

        framebmp      = (PTBitmap)(frames[frame].bitmap.ptr());

        db.sbufwidth  = db.dbufwidth  = framebmp->width;
        db.sbufheight = db.dbufheight = framebmp->height;
        db.sstride    = db.dstride    = framebmp->width;

        db.source     = &framebmp->data16;
        db.dest       = decbuf;

        db.szbuffer   = (uint16_t *)framebmp->zbuffer.ptr();
        db.snormals   = (uint16_t *)framebmp->normal.ptr();

        db.dzbuffer   = db.dnormals = nullptr;
        
        db.palette    = (uint16_t *)framebmp->palette.ptr();
        db.alpha      = (uint8_t *)framebmp->alpha.ptr();
        db.alias      = (uint8_t *)framebmp->alias.ptr();

        db.keycolor   = framebmp->keycolor;

        dp2.sx        = 0;  
        dp2.sy        = 0;

        dp2.dx       += frames[frame].x;    
        dp2.dy       += frames[frame].y;

        dp2.swidth    = framebmp->width;
        dp2.sheight   = framebmp->height;

        dp2.drawmode  = drawmode | DM_NORESTORE;

        Draw(&db, &dp2);

        surface->GetOrigin(dp->originx, dp->originy);
        surface->GetClipRect(dp->clipx, dp->clipy, dp->clipwidth, dp->clipheight);

        db.source     = decbuf;
        db.dest       = surface->Lock();
        surface->Unlock();

        db.dbufwidth  = surface->Width();
        db.dbufheight = surface->Height();
        db.dstride    = surface->Stride();
        Draw(&db, dp);
    }
}

void TAnimation::Stretch(PTSurface surface, PTSurface decbuf, int32_t x, int32_t y, int32_t size, 
                         int32_t frame)
{
}

int32_t TAnimation::NumFrames()
{
    int32_t n = numframes;

//  if (flags & AF_HALFSPEED) // Not supported
//      n <<= 1;
//  else if (flags & AF_THIRDSPEED)
//      n *= 3;
//  else if (flags & AF_QUARTERSPEED)
//      n <<= 2;

    return n;
}

PTBitmap TAnimation::GetFrame(int32_t frame)
{
//  if (flags & AF_HALFSPEED)       // Not supported
//      frame >>= 1;
//  else if (flags & AF_THIRDSPEED)
//      frame /= 3;
//  else if (flags & AF_QUARTERSPEED)
//      frame >>= 2;

    if (frame >= numframes)
        return nullptr;

    return frames[frame].bitmap;
}

void TAnimation::Move(int32_t &x, int32_t &y, int32_t frame, int32_t orientation)
{
}

void TAnimation::SizeMove(int32_t &x, int32_t &y, int32_t size, int32_t frame, int32_t orientation)
{
}

