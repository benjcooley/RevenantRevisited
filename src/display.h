// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *               display.h  - EXILE Display Include File                 *
// *************************************************************************

#pragma once

#include "revenant.h"

#include "graphics.h"
#include "surface.h"
#include "ddsurface.h"

_STRUCTDEF(RestoreRect)
_CLASSDEF(TDisplay)

class TDisplay
{
private:
    int32_t currentpage;            // Currently Displayed Front/Back Surface
    bool updateenabled;             // Whether restore system is enabled
    TSurface* Front;                // Front buffer surface
    TSurface* Back;                 // Back buffer surface  
    TSurface* ZBuffer;              // Depth buffer surface
    TSurface* SaveZBuffer;          // Where the real zbuffer goes when we're using a secondary z
    sg_pass default_pass;           // Default render pass for Sokol
    
    int32_t width;                  // Display width
    int32_t height;                 // Display height
    int32_t bitsperpixel;           // Color depth

  public:
    TDisplay();
      // Creates Display Structures and Surfaces
    ~TDisplay();
      // Destructor

      // Clear ZBuffer Info:
      // -------------------
      // Several cards will NOT allow you to access the video ZBuffer directly (i.e. the
      // voodoo cards).  This means that the scrolling restore buffers which use the 
      // dirty rectangle system CAN'T update the display ZBuffer.  Luckily, Microsoft
      // has a specific ZBuffer updating system inside DirectX itself.. SetBackgroundDepth()
      // function and the Clear() function for Viewports.  What we do here is create an
      // extra ZBuffer (yes, it does take up a lot more memory), and quietly swap the zbuffer
      // in the Display class (here) with this backround z buffer.  Other than taking up
      // more video memory, this doesn't slow the system down since the dirty rectangle system
      // never draws to the video zbuffer except when drawing 3D objects, and the Viewport->Clear()
      // function is actually faster than our dirty rectangle routine anyway.

    void InitClearZBuffer();
      // Creates a clear zbuffer for use with the Viewport->Clear() function
      // and swaps the actual zbuffer with the clear buffer so the rest of the program
      // thinks the clear buffer is the actual zbuffer
    void CloseClearZBuffer();
      // Closes the clear zbuffer and puts the real video zbuffer back as the main zbuffer
    bool UsingClearZBuffer() { return SaveZBuffer != nullptr; }
      // True if we're currently using a secondary zbuffer
    TSurface* GetRealZBuffer() { if (SaveZBuffer) return SaveZBuffer; else return ZBuffer; }
      // Returns the real display zbuffer (used by the Scene3D.RestoreZBuffer() function)

    virtual int32_t SurfaceType() { return SURFACE_DISPLAY; }
      // Returns type of surface this is


    TSurface* BackBuffer() { return Back; }
      // Returns the back buffer surface
    TSurface* FrontBuffer() { return Front; }
      // Returns the front buffer surface

    bool Initialize(int32_t dwidth, int32_t dheight, int32_t dbitsperpixel);
      // Initializes the Display Structures and Sets up the screen.
    virtual bool Close();
      // Shuts down and frees the display
    bool Restore();
      // Restores the display device after having been tabbed out of
  
    virtual TSurface* GetZBuffer() { return ZBuffer; }  // Returns nullptr if display ZBuffer disabled
      // Returns ZBuffer surface for this surface (if it has one)
    virtual TSurface* GetNormalBuffer() { return nullptr; }
      // Returns the normal buffer for this surface (if it has one)

    bool FlipPage(bool Wait = true);
      // Flips front and back surfaces

    bool PutToScreen(int32_t x, int32_t y, int32_t width, int32_t height);
      // Copies specific area from back buffer to front buffer (or window) so it can
      // be seen immediately on the screen without having to wait for the FlipPage() 
      // function to be called.  This is useful for when you are drawing (Loading...)
      // indicators and can't call the update Tick() functions.
    bool PutToScreen(SRect &r)
      { return PutToScreen(r.x(), r.y(), r.w(), r.h()); }
      // Copies specific area from back buffer to front buffer (or window).  Calls the
      // above function.

  // Override blit functions to call dirty rectangle update routines
  // ---------------------------------------------------------------

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

    // ***************************************************
    // Background System & Dirty Rectangle Update Routines
    // ***************************************************

    // A background area is a rectangle of the display for which there is a background buffer.
    // A background buffer stores the background imagery of the screen for that rectangle, and
    // is used to restore the background to the screen via dirty rectangle updates when
    // graphics are drawn to the screen (i.e. 3D or animation).  Areas of the screen without
    // background areas associated with them will not be restored when graphics are drawn to them.
    //
    // Background areas can be larger than the screen rectangle they update, and the origin for
    // the background buffer can be changed relative to the upper left corner of the screen rectangle, 
    // which means that scrolling screen backgrounds are easy to implement.  When a background 
    // area scrolls, an update rectangle is added for the whole screen rectangle.  In fact, this 
    // is how the main map scrolling system is implemented.  Also, when the origin of the background
    // area is such that the background buffer will not cover the entire screen buffer, the
    // dirty rectangle update routines will automatically wrap clip the updates so that the 
    // restore rectangles actually wrap around when the overlap the edge of the background 
    // buffer.  This allows the user to scroll the background buffer relative to the screen
    // rectangle, and constantly draw the unseen edges of the background to implement
    // a large virtual map (like we use in our game), without having to pay attention to 
    // any clipping or wrapping issues.

    // Note: Screen TPanes can be associated with a specific update area in order to simpify the
    // scrolling system.  When a screen pane is associated with a scrolling background area, all
    // the user need do is scroll the screen pane, and draw background graphics in the DrawBackground()
    // pane function.

    bool InitBackgroundSystem();
      // Initializes restore system.
    bool CloseBackgroundSystem();
      // Closes the background system
    void PauseBackgroundSystem() { updateenabled = false; }
      // Temporarily toggle off background update system
    void UnPauseBackgroundSystem() { updateenabled = true; }
      // Restart background update system
    int32_t CreateBackgroundArea(int32_t x, int32_t y, int32_t width, int32_t height,
        bool createzbuf = false, int32_t vsflags = VSURF_SYSTEMMEM);
      // Uses surface as background area
    int32_t UseBackgroundArea(int32_t x, int32_t y, int32_t width, int32_t height, TSurface* surface);
      // Creates a background area.   
    void FreeBackgroundArea(int32_t index);
      // Frees the given background area
    bool ClearBackgroundAreas();
      // Clears all restore areas. Releases surfaces.
    bool RestoreBackgroundAreas();
      // Draws all restore areas to screen.
    bool DrawRestoreRect(int32_t index, int32_t x, int32_t y, int32_t width, int32_t height,
        uint32_t drawmode =  DM_WRAPCLIPSRC | DM_NORESTORE | DM_ZBUFFER);
      // Called by RestoreBackgroundAreas to draw a background rect to the display
    bool ScrollBackground(int32_t index, int32_t originx, int32_t originy);
      // Sets origin of background Buffer

    #define UPDATE_THISFRAME        0x01        // Adds a dirty rectangle update rect for this frame
    #define UPDATE_NEXTFRAME        0x02        // Adds a dirty rectangle update rect for the next frame
    #define UPDATE_SCREENTOBUFFER   0x04        // Immediately copies the screen rect to the background
    #define UPDATE_BUFFERTOSCREEN   0x08        // Immediately copies the background rect to the screen
    #define UPDATE_NOMERGERECT      0x10        // Prevents dirty rectangle system from merging this rect

    #define UPDATE_RESTORE          (UPDATE_THISFRAME)
    #define UPDATE_BACKGROUND       (UPDATE_SCREENTOBUFFER | UPDATE_NEXTFRAME)

    void AddUpdateRect(int32_t x, int32_t y, int32_t width, int32_t height, int32_t flags);
      // Adds restore rects for all screen buffers (uses screen coords)
    bool AddBackgroundUpdateRect(int32_t index, int32_t x, int32_t y, int32_t width, int32_t height, int32_t flags); 
      // Adds update rect to a given restore buffer (using buffer coordinates)

    virtual DRAWCALLBACK GetDrawCallBack();
      // Returns the AddUpdateRect() function as a draw callback to the Draw() routine

//  NOTE: This code is part of surface.cpp now (BEN)
//  virtual bool ZPut(int32_t x, int32_t y, int32_t z, PTBitmap bitmap, uint32_t drawmode = DM_USEDEFAULT);

  
    void AddSubRect(int32_t index, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t flags);
      // Calls AddUpdateRect with parameters of a smaller rect.
};
