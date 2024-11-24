// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 Surface.h - 3D Surface Include File                   *
// *************************************************************************

#pragma once

#include "revenant.h"

#include "blit.h"
#include "graphics.h"

#define SURFACE_UNKNOWN 0
#define SURFACE_VIDEO   1
#define SURFACE_DISPLAY 2
#define SURFACE_BITMAP  3
#define SURFACE_MULTI   4
#define SURFACE_MOSAIC  5

_STRUCTDEF(SClipState)
struct SClipState
{
    int32_t clipmode;
    int32_t originx, originy;
    int32_t clipx, clipy, clipwidth, clipheight;
};

_CLASSDEF(TSurface)
class TSurface
{
  protected:
    int32_t width;           // Width of Surface
    int32_t height;          // Height of Surface
    int32_t bitsperpixel;    // Surface Color Depth
    int32_t stride;          // Size of one horizontal screen line in pixels
    int32_t originx;         // Current drawing origin
    int32_t originy;
    int32_t clipmode;        // Current clipmode
    int32_t clipx;           // Current clipping rectangle
    int32_t clipy;
    int32_t clipwidth;
    int32_t clipheight;
    uint32_t keycolor;       // Surface transparent color
    void* locked;            // True if surface locked

    // Buffers
    uint8_t* cpu_buffer;     // CPU-side staging buffer for modifications
    size_t buffer_size;      // Size of staging buffer in bytes
    sg_image image;          // Sokol GPU texture
    sg_image_desc img_desc;  // Sokol image description/config
    sg_pipeline pipeline;    // Sokol render pipeline
    sg_bindings bindings;    // Sokol resource bindings

  public:
    uint32_t flags;         // 'BM' flags for surface

    TSurface();
      // Initializes Surface
    virtual ~TSurface();
      // Releases Surface

    virtual int32_t SurfaceType() { return SURFACE_VIDEO; }
      // Returns type of surface this is

    virtual sg_image GetSGImage() { return image; }
      // Returns sokol image handle

    virtual int32_t BitsPerPixel() {return bitsperpixel;}
      // Returns current bits per pixel
    int32_t  Stride() {return stride;}
      // Returns current stride
    int32_t  Width() {return width;}
      // Returns current screen width
    int32_t  Height() {return height;}
      // Returns current screen height
    int32_t  KeyColor() {return keycolor;}
    void SetKeyColor(uint32_t key){keycolor = key;}
      // Sets KeyColor for surface.

    virtual void Reset();
      // Resets originx, originy, cliprect and clipmode to screen defaults.
    virtual bool Lost() = 0;
      // Returns true if the surface needs to be regenerated.
    
    virtual void* Lock();
    virtual bool Unlock();

    bool IsLocked() { return locked != nullptr; }
    
    
    virtual TSurface* GetGraphicsBuffer() { return this; }
      // Returns ZBuffer surface for this surface (if it has one)
    virtual TSurface* GetZBuffer() { return nullptr; }
      // Returns ZBuffer surface for this surface (if it has one)
    virtual TSurface* GetNormalBuffer() { return nullptr; }
      // Returns the normal buffer for this surface (if it has one)

    virtual void SetOrigin(int32_t x, int32_t y) { originx = x; originy = y; }
      // Sets drawing origin
    void GetOrigin(int32_t &x, int32_t &y) { x = originx; y = originy; }
      // Gets drawing origin

    virtual void SetClipRect(int32_t x, int32_t y, int32_t w, int32_t h) {clipx = x; clipy = y; clipwidth = w; 
                     clipheight = h;}
      // Sets display clipping rectangle
    void SetClipRect(RSRect r) 
      { SetClipRect(r.left, r.top, r.right - r.left + 1, r.bottom - r.top + 1); }
      // Alternative cliping rect set function
    void ResetClipRect() { SetClipRect(0, 0, width, height); } 
      // Resets the clip rect to origin and width height of surface
    void GetClipRect(int32_t &x, int32_t &y, int32_t &w, int32_t &h)
        { x = clipx;  y = clipy; w = clipwidth; h = clipheight; }
      // Gets display clipping rectangle
    void GetClipRect(RSRect r)
        { GetClipRect(r.left, r.top, r.right, r.bottom); r.right += r.left - 1; r.bottom += r.top - 1; }    
      // Alternative clipping rect get function

    virtual void SetClipMode(int32_t mode) { clipmode = mode; }
      // Sets clipping to normal of wrap around
    void GetClipMode(int32_t &mode) { mode = clipmode; }
      // Gets clipping mode.

    void SaveClipState(SClipState &cs)
      { cs.clipmode = clipmode; cs.originx = originx; cs.originy = originy;
        cs.clipx = clipx; cs.clipy = clipy; cs.clipwidth = clipwidth; cs.clipheight = clipheight; }
      // Saves the current clipping state to the SClipState structure
    void RestoreClipState(SClipState &cs)
      { SetClipMode(cs.clipmode); SetOrigin(cs.originx, cs.originy); 
        SetClipRect(cs.clipx, cs.clipy, cs.clipwidth, cs.clipheight); }
      // Restores the current clipping state from the SClipState structure

    virtual DRAWCALLBACK GetDrawCallBack() { return nullptr; }
      // Allows surfaces to specify a draw rectangle callback routine for
      // the low level Draw() function (supports adding update rectangles to
      // the Display)

  // These two functions are used to setup the clipping rect, origin, drawmode, and
  // do simple clipping for a draw param structure being passed to the ParamDraw() or
  // BlitHandler() functions.  A function can call these functions before a call to a
  // ParamDraw or ParamBlit function to get a copy of the DrawParam structure as it will be when
  // the lower level blits or draws are actually done.  The resulting DrawParam can then
  // be clipped with the Clip() function, or used directly with the ParamDraw or ParamBlit
  // functions.
    bool ParamDrawSetup(RSDrawParam dpv, PTBitmap bitmap);
      // Sets up a DrawParam structure for a call to DrawParam().  Returns false if fails
    bool ParamBlitSetup(RSDrawParam tmpdp, TSurface* srcsurface, int32_t flags);
      // Sets up a DrawParam structure for a call to BlitHandler.  Returns false if fails

  // Low level Put and Blit which ONLY do primary surface (no Z or Normal Buffer)
    virtual bool BlitHandler(PSDrawParam dp, TSurface* surface, int32_t flags = 0);
      // Blits from surface to surface. RECT sets size of blit. 
      // X & Y specifies dest. origin

// --------------------------------------------------------------------------------
// CORE SURFACE DRAW ROUTINES
//
// These routines are called by ALL surface drawing functions, and allow the 
// special surfaces such as TMultiSurface, and TMosaicSurface to redirect the
// drawing calls to one or more ordinary surface drawing calls.  It is therefore
// extremely important that NO drawing functions bypass these calls below.  All
// drawing functions MUST be called by the following functions below!!!!!!!

    virtual bool UseGetBlit() { return false; }
      // true when blitting from a complex surface like a TMosaicSurface.  This
      // causes the surface to use the complex source surfaces' GetBlit function 
      // instead of the ordinary destination surfaces Blit function when blitting
      // from a complex (mosaic) surface to an ordinary surface.

  // Put and blit functions which do primary, zbuffer, and normal buffer surface
    virtual bool ParamDraw(PSDrawParam dp, PTBitmap bitmap = nullptr);
      // Copies specified bitmap to current bitmap
    virtual bool ParamBlit(PSDrawParam dp, TSurface* surface, int32_t flags = 0);
      // Blits from surface to this surface. RECT sets size of blit.
      // X & Y specifies dest origin.  If no hardware available, uses software
      // blitting
    virtual bool ParamGetBlit(PSDrawParam dp, TSurface* surface, int32_t flags = 0);
      // Blits from this surface to surface. RECT sets size of blit. 
      // X & Y specifies dest origin.  If no hardware available, uses software
      // blitting.  Called by ParamBlit when blitting from a complex surface
      // to an ordinary surface.

// --------------------------------------------------------------------------------

  // Shortcut blit functions
    bool Blit(int32_t x, int32_t y, int32_t w, int32_t h,
        uint32_t drawmode = DM_USEDEFAULT, int32_t flags = 0)
    {
        SDrawParam dp;
        MakeDPNoSrc(dp, x, y, w, h, drawmode);
        return ParamBlit(&dp, nullptr, flags);
    }

  // Shortcut blit functions
    bool Blit(int32_t x, int32_t y, TSurface* surface, int32_t sx, int32_t sy, int32_t swidth,
               int32_t sheight, uint32_t drawmode = DM_USEDEFAULT, int32_t flags = 0)
    {
        SDrawParam dp;
        MakeDP(dp, x, y, sx, sy, swidth, sheight, drawmode);
        return ParamBlit(&dp, surface, flags);
    }
    
    bool Blit(int32_t x, int32_t y, TSurface* surface, uint32_t drawmode = DM_USEDEFAULT,
               int32_t flags = 0)
    { 
        return Blit(x, y, surface, 0, 0, surface->Width(), surface->Height(), drawmode, flags);
    }

  // Shortcut blit functions
    bool GetBlit(int32_t x, int32_t y, TSurface* surface, int32_t sx, int32_t sy, int32_t swidth,
               int32_t sheight, uint32_t drawmode = DM_USEDEFAULT, int32_t flags = 0)
    {
        SDrawParam dp;
        MakeDP(dp, x, y, sx, sy, swidth, sheight, drawmode);
        return ParamGetBlit(&dp, surface, flags);
    }
    
    bool GetBlit(int32_t x, int32_t y, TSurface* surface, uint32_t drawmode = DM_USEDEFAULT,
               int32_t flags = 0)
    { 
        return GetBlit(x, y, surface, 0, 0, surface->Width(), surface->Height(), drawmode, flags);
    }

  // Shortcut put functions
    bool Put(int32_t x, int32_t y, PTBitmap bitmap, int32_t sx, int32_t sy, int32_t swidth, int32_t sheight, 
              uint32_t drawmode = DM_USEDEFAULT)
    { 
        SDrawParam dp; 
        MakeDP(dp, x, y, sx, sy, swidth, sheight, drawmode);
        return ParamDraw(&dp, bitmap); 
    }
    
    bool Put(int32_t x, int32_t y, PTBitmap bitmap, uint32_t drawmode = DM_USEDEFAULT, PSColor color = nullptr);

    bool PutHue(int32_t x, int32_t y, PTBitmap bitmap, uint32_t drawmode = DM_USEDEFAULT, int32_t hue = 0);

    bool PutSV(int32_t x, int32_t y, PTBitmap bitmap, uint32_t drawmode = DM_USEDEFAULT, int32_t saturation = 100, int32_t brightness = 100);

    bool PutDim(int32_t x, int32_t y, PTBitmap bitmap, uint32_t drawmode = DM_USEDEFAULT, int32_t dim = 0);

    bool ZPut(int32_t x, int32_t y, int32_t z, PTBitmap bitmap, int32_t sx, int32_t sy, int32_t swidth, 
              int32_t sheight, uint32_t drawmode = DM_USEDEFAULT)
    { 
        SDrawParam dp; 
        MakeDP(dp, x, y, sx, sy, swidth, sheight, drawmode);
        dp.zpos = (uint16_t)z;
        return ParamDraw(&dp, bitmap); 
    }
    
    bool ZPut(int32_t x, int32_t y, int32_t z, PTBitmap bitmap, uint32_t drawmode = DM_USEDEFAULT);

    bool ZPutDim(int32_t x, int32_t y, int32_t z, PTBitmap bitmap, uint32_t drawmode = DM_USEDEFAULT, int32_t dim = 0, PSColor color = nullptr);

    uint32_t ZFind(int32_t x, int32_t y, int32_t z, PTBitmap bitmap, uint32_t drawmode = DM_USEDEFAULT);

    virtual bool Box(int32_t dx, int32_t dy, int32_t dwidth, int32_t dheight, 
        uint32_t color = 0, uint16_t zpos = 0xFFFF, 
        uint16_t normal = 0x7F7F, uint32_t drawmode = DM_USEDEFAULT);

    bool Clear(uint32_t color = 0, uint16_t zpos = 0xFFFF, uint16_t normal = 0x7F7F, uint32_t drawmode = DM_USEDEFAULT)
    {
        return Box(0, 0, width, height, color, zpos, normal, drawmode);
    }

    bool Line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, 
        SColor &color, uint32_t drawmode = DM_USEDEFAULT);

    bool Rect(int32_t x, int32_t y, int32_t w, int32_t h, 
        SColor &color, uint32_t drawmode = DM_USEDEFAULT);

    int32_t WriteText(char *text, int32_t x = 0, int32_t y = 0, 
        int32_t numlines = 1, TFont* font = SystemFont, 
        PSColor color = nullptr, uint32_t drawmode = DM_USEDEFAULT, 
        int32_t wrapwidth = -1, int32_t startline = 0, 
        int32_t justify = JUSTIFY_LEFT, int32_t hue = -1, int32_t linespace = 0);
        
    int32_t WriteTextShadow(char *text, int32_t x = 0, int32_t y = 0, 
        int32_t numlines = 1, TFont* font = SystemFont, 
        PSColor color = nullptr, uint32_t drawmode = DM_USEDEFAULT, 
        int32_t wrapwidth = -1, int32_t startline = 0, 
        int32_t justify = JUSTIFY_LEFT, int32_t hue = -1, int32_t linespace = 0);


    // *************************
    // ** NOTE TO EVERYBODY!! **
    // *************************

    // DON'T PUT GAME SPECIFIC DRAW ROUTINES IN THIS SURFACE HEADER.....

    // I set up the draw system so that low level draw routines can be passed in the 
    // draw param structure in the 'func' member.  This means that you shouldn't
    // glom new game specific functions like ZFind in here, instead make a class or
    // global function in your own module which sets up a drawparam structure and
    // calls the ParamDraw() function.  This will make it easier to keep the low
    // level system somewhat clean.

    // o Low level blit functions and general purpose drawing functions should go in 
    //   graphics.cpp
    //
    // o Surface versions of these general purpose functions should go here.
    //
    // o Game specific low level surface or bitmap functions should go in the module
    //   they're most closely connected with, and should have their own functions which
    //   use the 'func' member and the ParamDraw() function.
};

