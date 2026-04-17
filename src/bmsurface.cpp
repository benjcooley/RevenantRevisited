// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                bmsurface.cpp - EXILE Surface Object                   *
// *************************************************************************

#include "revenant.h"
#include "bitmap.h"
#include "bmsurface.h"

TBitmapSurface::TBitmapSurface()
{
}

TBitmapSurface::~TBitmapSurface()
{
    if (ownsbitmap && bitmap)
        delete bitmap;
}

void TBitmapSurface::Initialize(int32_t bmwidth, int32_t bmheight, int32_t bmflags)
{
    bitmap = TBitmap::NewBitmap(bmwidth, bmheight, bmflags);
    width  = bitmap->width;
    height = bitmap->height;
    flags  = bmflags;
    stride = width;

    if (bitmap->flags & BM_8BIT)
    {
        bitsperpixel = 8;
    }

    else if (bitmap->flags & BM_15BIT)
    {
        bitsperpixel = 15;
    }
    
    else if (bitmap->flags & BM_16BIT)
    {
        bitsperpixel = 16;
    }
    
    else if (bitmap->flags & BM_24BIT)
    {
        bitsperpixel = 24;
    }
    
    else if (bitmap->flags & BM_32BIT)
    {
        bitsperpixel = 32;
    }
    
    ownsbitmap = true;
}

void TBitmapSurface::Initialize(PTBitmap newbitmap)
{
    bitmap     = newbitmap;
    ownsbitmap = false;

    width      = bitmap->width;
    height     = bitmap->height;
    clipwidth  = bitmap->width;
    clipheight = bitmap->height;
    flags      = bitmap->flags;
    stride     = width;

    if (bitmap->flags & BM_8BIT)
    {
        bitsperpixel = 8;
    }

    else if (bitmap->flags & BM_15BIT)
    {
        bitsperpixel = 15;
    }
    
    else if (bitmap->flags & BM_16BIT)
    {
        bitsperpixel = 16;
    }
    
    else if (bitmap->flags & BM_24BIT)
    {
        bitsperpixel = 24;
    }
    
    else if (bitmap->flags & BM_32BIT)
    {
        bitsperpixel = 32;
    }
}

bool TBitmapSurface::Rect(int32_t x, int32_t y, int32_t w, int32_t h, SColor &color)
{
    return true;
}

bool TBitmapSurface::Rect(SRect r, SColor &color)
{
    return true;
}

bool TBitmapSurface::Box(int32_t x, int32_t y, int32_t w, int32_t h, SColor &color)
{
    return true;
}

bool TBitmapSurface::Line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, SColor &color)
{
    return true;
}

bool TBitmapSurface::Copy(PTBitmap bitmap)
{
    return true;
}

bool TBitmapSurface::BlitPrimary(PSDrawParam dp, TSurface* surface, int32_t ddflags)
{
    return true;
}
