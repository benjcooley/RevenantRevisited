// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *               bmsurface.h - bitmap surface include file               *
// *************************************************************************

#pragma once

#include "revenant.h"
#include "surface.h"
#include "bitmap.h"

_CLASSDEF(TBitmapSurface)

class TBitmapSurface : public TSurface
{
  private:
    PTBitmap bitmap;      // Bitmap pointer
    bool     ownsbitmap;  // Determines if bitmap is deleted on destruction.   

  public:
    TBitmapSurface();
    TBitmapSurface(int32_t bmwidth, int32_t bmheight, int32_t bmflags)
        { Initialize(bmwidth, bmheight, bmflags); }
    TBitmapSurface(PTBitmap bitmap)
        { Initialize(bitmap); }

    virtual int32_t SurfaceType() { return SURFACE_BITMAP; }
      // Returns type of surface this is

      // Constructor for BitmapSurface.
    virtual ~TBitmapSurface();
      // Destructor for BitmapSurface.

    void Initialize(int32_t width, int32_t height, int32_t bmflags);
      // Initializes bitmap surface using width and height. Sets variables.
    void Initialize(PTBitmap bitmap);
      // Initializes bitmap surface from an existing bitmap. Sets variables.

    uint32_t GetSurface() { return nullptr; }
      // Returns Direct Draw pointer or Null if not Direct Draw Surface.
    virtual void *Lock() { stride = width; return locked = (void *)bitmap->data16; }
      // Locks surface. Returns pointer to surface or nullptr 
      // if buffer couldn't be locked.
    virtual bool Unlock() { locked = nullptr; return true;}
      // Unlocks surface.
    
    virtual bool Lost() {return false;}
      // Returns true if the surface needs to be regenerated. Never needed for bitmap.

    virtual bool Rect(int32_t x, int32_t y, int32_t w, int32_t h, SColor &color);
      // Draws a unfilled box in specified color
    virtual bool Rect(SRect r, SColor &color);
      // Draws a unfilled box in specified color
    virtual bool Box(int32_t x, int32_t y, int32_t w, int32_t h, SColor &color);
      // Draws a box in specified color
    virtual bool Line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, SColor &color);
      // Draws a line in specified color
    virtual bool Copy(PTBitmap bitmap);
      // Copies specified bitmap to current Direct Draw Surface

    virtual bool BlitPrimary(PSDrawParam dp, PTSurface surface, int32_t ddflags = 0);
      // Blits from surface to surface. RECT sets size of blit. 
      // X & Y specifies dest. origin

    PTBitmap GetBitmap() { return bitmap; }
      // Allows access to bitmap

};

#endif
