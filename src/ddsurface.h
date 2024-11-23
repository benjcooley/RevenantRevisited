// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *            ddsurface.h - Direct Draw Surface Include File             *
// *************************************************************************

#ifndef _DDSURFACE_H
#define _DDSURFACE_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _SURFACE_H
#include "surface.h"
#endif

#define VSURF_VIDEOMEM     1       // Attempts to grab video, if non available, uses system
#define VSURF_SYSTEMMEM    2       // Grabs system memory
#define VSURF_VIDEOONLY    4       // Forces system to only grab video memory or exit with error
#define VSURF_ZBUFFER      8       // This surface will be a zbuffer

_CLASSDEF(TDDSurface)
class TDDSurface : public TSurface
{
  protected:
    bool                lost;          // Indicates if a surface was lost
    LPDIRECTDRAWSURFACE surface;       // DirectDraw surface pointer
    bool                ownssurface;   // Determines if surface is released
    int32_t                 vsflags;       // VSURF flags for surface (SYSTEMMEM or VIDEOMEM)

  public:
  // Constructor
    TDDSurface();
      // No initialize - must call Initialize() and Close() 
    TDDSurface(int32_t width, int32_t height, int32_t flags = VSURF_VIDEOMEM, int32_t stride = 0)
        { surface = nullptr; Initialize(width, height, flags, stride); ownssurface = true;}
      // Initializes Video Surface
    TDDSurface(PTBitmap bitmap, int32_t intensity, int32_t flags = VSURF_VIDEOMEM)
        { surface = nullptr; Initialize(bitmap, intensity, flags);  ownssurface = true;}
      // Initializes Video Surface
    TDDSurface(LPDIRECTDRAWSURFACE ddsurface)
        { surface = nullptr; Initialize(ddsurface);  ownssurface = false;}
      // Initializes Video Surface from DirectDraw surface.
    virtual ~TDDSurface();
      // Closes Video Surface.

    virtual int32_t SurfaceType() { return SURFACE_VIDEO; }
      // Returns type of surface this is

    virtual LPDIRECTDRAWSURFACE GetDDSurface() { return surface; }
      // Returns LPDIRECTDRAWSURFACE pointer or Null if not Direct Draw Surface.

  // Initialize functions
    void Initialize(int32_t width, int32_t height, bool usevideomem = true, int32_t stride = 0);
      // Initializes Video Surface
    void Initialize(PTBitmap bitmap, int32_t intensity, bool usevideomem = true);
      // Initializes Video Surface
    void Initialize(LPDIRECTDRAWSURFACE ddsurface);
      // Initializes Video Surface from DirectDraw surface.

    void Close();
      // Close video surface

    virtual bool Lost() {return lost;}
      // Returns true if the surface needs to be regenerated.

    virtual bool Restore();
      // Checks and Restores Surface if needed

    int32_t VSFlags() { return vsflags; }
      // Returns the direct draw video surface specific flags
    
    virtual void *Lock();
      // Locks surface. Returns pointer to surface or nullptr 
      // if buffer couldn't be locked.
    virtual bool Unlock();
      // Unlocks surface.

  // Low level Put and Blit which ONLY do primary surface (no Z or Normal Buffer)
    virtual bool BlitHandler(PSDrawParam dp, PTSurface surface, int32_t ddflags = 0, LPDDBLTFX fx = nullptr);
      // Blits from surface to surface. RECT sets size of blit. 
      // X & Y specifies dest. origin

    virtual bool Box(int32_t dx, int32_t dy, int32_t dwidth, int32_t dheight, 
        uint32_t color = 0, uint16_t zpos = 0xFFFF, 
        uint16_t normal = 0x7F7F, uint32_t drawmode = DM_USEDEFAULT);
      // Check caps structure and do hardware clear if available..
};

#endif
