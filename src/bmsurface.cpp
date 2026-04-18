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

int32_t TBitmapSurface::BitsPerPixel()
{
    if (!bitmap) return 0;
    if (bitmap->flags & BM_8BIT)  return 8;
    if (bitmap->flags & BM_15BIT) return 15;
    if (bitmap->flags & BM_16BIT) return 16;
    if (bitmap->flags & BM_24BIT) return 24;
    if (bitmap->flags & BM_32BIT) return 32;
    return 0;
}

void TBitmapSurface::Initialize(int32_t bmwidth, int32_t bmheight, int32_t bmflags)
{
    bitmap = TBitmap::NewBitmap(bmwidth, bmheight, bmflags);
    width  = bitmap->width;
    height = bitmap->height;
    flags  = bmflags;
    stride = width;
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
