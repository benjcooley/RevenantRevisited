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

#define UPDATE_THISFRAME        0x01        // Adds a dirty rectangle update rect for this frame
#define UPDATE_NEXTFRAME        0x02        // Adds a dirty rectangle update rect for the next frame
#define UPDATE_SCREENTOBUFFER   0x04        // Immediately copies the screen rect to the background
#define UPDATE_BUFFERTOSCREEN   0x08        // Immediately copies the background rect to the screen
#define UPDATE_NOMERGERECT      0x10        // Prevents dirty rectangle system from merging this rect

#define UPDATE_RESTORE          (UPDATE_THISFRAME)
#define UPDATE_BACKGROUND       (UPDATE_SCREENTOBUFFER | UPDATE_NEXTFRAME)

_STRUCTDEF(RestoreRect)
_CLASSDEF(TDisplay)

class TDisplay 
{
  public:
    // Creates Display Structures and Surfaces
    TDisplay();
    // Destructor
    virtual ~TDisplay();

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

    // Creates a clear zbuffer for use with the Viewport->Clear() function
    // and swaps the actual zbuffer with the clear buffer so the rest of the program
    // thinks the clear buffer is the actual zbuffer
    void InitClearZBuffer();
    // Closes the clear zbuffer and puts the real video zbuffer back as the main zbuffer
    void CloseClearZBuffer();
    // True if we're currently using a secondary zbuffer
    bool UsingClearZBuffer() { return savezbuffer != nullptr; }
    // Returns the real display zbuffer (used by the Scene3D.RestoreZBuffer() function)
    TSurface* GetRealZBuffer() { if (savezbuffer) return savezbuffer; else return zbuffer; }

    // Returns the back buffer surface
    TSurface* BackBuffer() const { return backbuffer; }
    // Returns the front buffer surface 
    TSurface* FrontBuffer() const { return frontbuffer; }

    // Initializes the Display Structures and Sets up the screen.
    bool Initialize(int32_t dwidth, int32_t dheight, int32_t dbitsperpixel);
    // Shuts down and frees the display
    virtual bool Close();
    // Restores the display device after having been tabbed out of
    bool Restore();

    // Flips front and back surfaces
    bool FlipPage(bool Wait = true);

private:
    int32_t currentpage = 0;          // Currently Displayed Front/Back Surface
    bool updateenabled = true;        // Whether restore system is enabled
    TSurface* frontbuffer = nullptr;  // Front buffer surface
    TSurface* backbuffer = nullptr;   // Back buffer surface  
    TSurface* zbuffer = nullptr;      // Depth buffer surface
    TSurface* savezbuffer = nullptr;  // Where the real zbuffer goes when we're using a secondary z
    // Sokol graphics resources
    sg_pass default_pass;             // Default render pass
    sg_pass depth_pass;               // Pass for depth pre-pass
    sg_pipeline tile_pipeline;        // Pipeline for tile rendering
    sg_pipeline_desc tile_pip_desc;   // Tile pipeline config
    
    // Render targets
    sg_image color_target;            // Main color render target
    sg_image depth_target;            // Depth render target
    sg_image normal_target;           // Normal render target for lighting
    
    // Uniform buffers
    struct {
        sg_buffer view_proj;          // View-projection matrix
        sg_buffer model;              // Model matrix
        sg_buffer light_params;       // Light position, color, etc
    } uniforms;
    
    // Display properties  
    int32_t width = 0;               // Display width
    int32_t height = 0;              // Display height
    int32_t bitsperpixel = 0;        // Color depth
};
