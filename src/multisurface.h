// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *             Multisurface.h - Multi Surface Include File               *
// *************************************************************************

#pragma once

#include "revenant.h"

#include "surface.h"

_CLASSDEF(TMultiSurface)

class TMultiSurface : public TSurface
{
  protected:
    PTSurface graphics;     // Graphics surface
    PTSurface zbuffer;      // ZBuffer surface
    PTSurface normals;      // Normal surface
    bool      deletesurf;   // Do we delete these surfaces when we die
    bool      lost;         // Indicates if a surface was lost

  public:
    TMultiSurface();
      // Initializes Multi surfaces
    TMultiSurface(PTSurface surface1, PTSurface surface2,
        PTSurface surface3, bool delsurf = false)
      { Initialize(surface1, surface2, surface3, delsurf); }
      // Initializes Multi surfaces
    virtual ~TMultiSurface();
      // Destroys Multi surfaces.

    virtual int32_t SurfaceType() { return SURFACE_MULTI; }
      // Returns type of surface this is

    void Initialize(PTSurface surface1,
         PTSurface surface2, PTSurface surface3, bool delsurf = false);
      // Causes the multisurface to be initialized
    void Close();
      // Closes the multi surface

    void SetGraphicsBuffer(PTSurface surf);
      // Sets the graphics buffer surface manually
    void SetZBuffer(PTSurface surf) { zbuffer = surf; }
      // Sets the zbuffer surface manually
    void SetNormalBuffer(PTSurface surf) { normals = surf; }
      // Sets the normal buffer surface manually

    virtual PTSurface GetGraphicsBuffer() { return graphics; }
      // Returns ZBuffer surface for this surface (if it has one)
    virtual PTSurface GetZBuffer() { return zbuffer; }
      // Returns ZBuffer surface for this surface (if it has one)
    virtual PTSurface GetNormalBuffer() { return normals; }
      // Returns the normal buffer for this surface (if it has one)

    virtual LPDIRECTDRAWSURFACE GetDDSurface() { return graphics->GetDDSurface(); }
      // Returns LPDIRECTDRAWSURFACE pointer or Null if not Direct Draw Surface.

    virtual bool Lost() { return lost; }
      // Returns true if the surface needs to be regenerated.
    
    virtual void Reset();
      // Resets originx, originy, cliprect and clipmode to screen defaults.
    virtual void *Lock() { return locked = graphics->Lock(); }
      // Locks surface. Returns pointer to surface or nullptr 
      // if buffer couldn't be locked.
    virtual bool Unlock() { locked = nullptr; return graphics->Unlock(); }
      // Unlocks surface.
    
    virtual void SetOrigin(int32_t x, int32_t y);
      // Sets drawing origin
    virtual void SetClipRect(int32_t x, int32_t y, int32_t w, int32_t h);
      // Sets display clipping rectangle
    virtual void SetClipMode(int32_t mode);
      // Sets clipping to normal of wrap around

  // Low level Put and Blit which ONLY do primary surface (no Z or Normal Buffer)
    virtual bool BlitHandler(PSDrawParam dp, PTSurface surface, int32_t ddflags = 0, LPDDBLTFX fx = nullptr)
      { return graphics->BlitHandler(dp, surface, ddflags, fx); }
      // Blits from surface to surface. RECT sets size of blit. 
      // X & Y specifies dest. origin
};

