// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *               display.h  - EXILE Display Include File                 *
// *************************************************************************

#pragma once

#include "revenant.h"

#include "graphics.h"
#include "surface.h"

#define UPDATE_THISFRAME        0x01        // Adds a dirty rectangle update rect for this frame
#define UPDATE_NEXTFRAME        0x02        // Adds a dirty rectangle update rect for the next frame
#define UPDATE_SCREENTOBUFFER   0x04        // Immediately copies the screen rect to the background
#define UPDATE_BUFFERTOSCREEN   0x08        // Immediately copies the background rect to the screen
#define UPDATE_NOMERGERECT      0x10        // Prevents dirty rectangle system from merging this rect

#define UPDATE_RESTORE          (UPDATE_THISFRAME)
#define UPDATE_BACKGROUND       (UPDATE_SCREENTOBUFFER | UPDATE_NEXTFRAME)

_STRUCTDEF(RestoreRect)
_CLASSDEF(TDisplay)

class TDisplay : public TSurface
{
  public:
    // Creates Display Structures and Surfaces
    TDisplay();
    // Destructor
    virtual ~TDisplay();

    // Legacy dirty-rect API — no-op under sokol_gfx; screens just redraw
    void AddUpdateRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t flags = 0) {
        (void)x; (void)y; (void)w; (void)h; (void)flags;
    }

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

    // Composites a surface's sokol image as a fullscreen quad in the current
    // render pass. Caller must be inside an active sg_begin_*_pass.
    void Composite(TSurface* src);

    // Composites a sokol image as a textured quad at pixel rect (dst_x, dst_y,
    // dst_w, dst_h) within a render target of size (target_w, target_h). Caller
    // must be inside an active pass whose color attachment is that target.
    void Composite(sg_image img,
                   int32_t dst_x, int32_t dst_y, int32_t dst_w, int32_t dst_h,
                   int32_t target_w, int32_t target_h);

    // Atlas-friendly variant: also takes a pixel-space source rect within the
    // image (src_w x src_h starting at src_x, src_y, image size src_tex_w x
    // src_tex_h). Used for per-glyph draws out of a font atlas.
    void Composite(sg_image img,
                   int32_t dst_x, int32_t dst_y, int32_t dst_w, int32_t dst_h,
                   int32_t target_w, int32_t target_h,
                   int32_t src_x, int32_t src_y, int32_t src_w, int32_t src_h,
                   int32_t src_tex_w, int32_t src_tex_h);

    // ---- Phase A1 tile rendering ----
    // Begin a pass on the color+depth render targets (color_target + depth_target)
    // clearing both. After this, DrawTile can issue any number of sprite draws;
    // the fragment shader writes per-pixel gl_FragDepth from the depth image
    // sample plus anchor_z, so sprites interpenetrate correctly in world space.
    // EndTilePass closes the pass. FlipPage composites color_target to the
    // swapchain when it was written to this frame.
    void BeginTilePass(float r, float g, float b, float a);
    void DrawTile(sg_image color_img, sg_image depth_img,
                  int32_t dst_x, int32_t dst_y, int32_t dst_w, int32_t dst_h,
                  float anchor_z, float z_scale);
    void EndTilePass();

    // Scene light used by the tile fragment shader. Normals are reconstructed
    // from each tile's authored per-pixel depthmap via dfdx/dfdy, and albedo
    // is modulated by (ambient + diffuse). dir is lit-space (x right, y down
    // on screen, z out of the screen); deriv_scale maps a unit of sampled
    // depth across a screen pixel to a unit of surface slope — tune to taste.
    void SetLight(float dx, float dy, float dz, float intensity,
                  float r, float g, float b, float ambient,
                  float deriv_scale);

    // Debug view modes for the tile fragment shader:
    //   0 = lit (default)  1 = albedo only  2 = depth  3 = normals
    void SetTileViewMode(int32_t mode);

    // Screen-space point lights. Rebuild the list each frame — ClearPointLights()
    // at the top, AddPointLight(...) per light. Position is in pixels (x, y
    // screen-space, z above the screen plane); radius is also pixels (linear
    // falloff to radius, squared for a softer knee). Up to kMaxPointLights
    // per draw; extras silently dropped.
    static constexpr int32_t kMaxPointLights = 16;
    void ClearPointLights();
    void AddPointLight(float x, float y, float z_above, float radius,
                       float r, float g, float b, float intensity);

    // The color render target that BeginTilePass draws into. Exposed so
    // callers can composite it onto the backbuffer or swapchain.
    sg_image ColorTarget() const { return color_target; }

private:
    int32_t currentpage = 0;          // Currently Displayed Front/Back Surface
    bool updateenabled = true;        // Whether restore system is enabled
    TSurface* frontbuffer = nullptr;  // Front buffer surface
    TSurface* backbuffer = nullptr;   // Back buffer surface
    TSurface* zbuffer = nullptr;      // Depth buffer surface
    TSurface* savezbuffer = nullptr;  // Where the real zbuffer goes when we're using a secondary z

    // Shared composite primitive (textured quad). The shader + vbuf are
    // shared; we need one pipeline per target-format combo because sokol
    // validates that the pipeline's attachment formats match the pass's.
    //   composite_pip_rt  — targets our RGBA8 backbuffer (no depth)
    //   composite_pip_swap — targets the sokol_app swapchain (default pass)
    sg_shader   composite_shader = {};
    sg_buffer   composite_vbuf   = {};
    sg_pipeline composite_pip_rt   = {};
    sg_pipeline composite_pip_swap = {};
    void InitCompositePipeline();
    void ShutdownCompositePipeline();

    // Sokol graphics resources
    sg_pass default_pass;             // Default render pass (color_target + depth_target)
    sg_pass depth_pass;               // Pass for depth pre-pass
    sg_shader   tile_shader   = {};
    sg_pipeline tile_pipeline = {};
    sg_buffer   tile_vbuf     = {};
    bool        color_target_dirty = false;  // Set by BeginTilePass, read by FlipPage

    // Scene light used by the tile fragment shader. Defaults are tuned so
    // a flat surface (normal = +Z) doesn't clamp to 1.0 — without that,
    // flat ground looks identical to the unlit pass. A mostly-horizontal
    // light gives flat ground ~0.5 and lets sloped faces read brighter.
    struct {
        float dir[3]     = { 0.6f, -0.6f, 0.4f };
        float intensity  = 1.0f;
        float color[3]   = { 1.0f, 1.0f, 1.0f };
        float ambient    = 0.25f;
        float deriv_scale = 8192.0f;
        int32_t view_mode = 0;
        // Screen-space point lights. Each light is (pos xy, z_above, radius)
        // + (rgb, intensity); shader loops to `plight_count`.
        int32_t plight_count = 0;
        float plight_pos[kMaxPointLights][4] = {};
        float plight_col[kMaxPointLights][4] = {};
    } light;
    void InitTilePipeline();
    void ShutdownTilePipeline();

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
    
    // Display dimensions/bitsperpixel live on the TSurface base class.
};
