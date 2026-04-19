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
    // Draw a tile. `anchor_z` is the normalized scene-depth of the billboard's
    // anchor pixel before the bitmap-local z sample is added. Per-tile params:
    //   tile_root_wu   — world position of the tile anchor (regx, regy pixel)
    //   anchor_px      — which sprite pixel is the anchor (usually regx, regy)
    //   dst_wh         — sprite size in destination pixels (= dst_w, dst_h)
    //   zraw_to_wu     — scale factor applied to sampled bitmap-z differences
    //                    for normal reconstruction.
    void DrawTile(sg_image color_img, sg_image depth_img,
                  int32_t dst_x, int32_t dst_y, int32_t dst_w, int32_t dst_h,
                  float anchor_z, float depth_mul, float normal_mul,
                  float root_wx, float root_wy, float root_wz,
                  float anchor_px_x, float anchor_px_y,
                  float zraw_to_wu);
    void EndTilePass();

    // Scene light used by the deferred fullscreen light pass. Normals are
    // reconstructed from each tile's authored per-pixel depthmap, and albedo
    // is modulated by (ambient + diffuse). dir is lit-space (x right, y down
    // on screen, z out of the screen).
    void SetLight(float dx, float dy, float dz, float intensity,
                  float r, float g, float b, float ambient);
    // Normal-reconstruction denoise radius in texels (central-difference
    // stencil width). Wider = smoother normals, loses surface detail.
    void SetNormalRadius(float texels);
    // Bilateral-reject threshold for normal reconstruction, in the same units
    // as the uploaded tile depth texture. Neighbor samples that differ from
    // the center by more than this get clamped to the center — kills the
    // "bevel" artifact where the filter bleeds across wall/floor seams.
    void SetEdgeThreshold(float zraw_units);
    // Debug view modes for the tile fragment shader:
    //   0 = lit (default)  1 = albedo only  2 = depth  3 = normals
    void SetTileViewMode(int32_t mode);

    // World-space point lights. Rebuild the list each frame — ClearPointLights()
    // at the top, AddPointLight(...) per light. Position is world xyz in
    // world units (100 wu = 1 m), radius is also in world units: linear
    // falloff to `radius`, squared for a softer knee. The tile fragment
    // shader reconstructs each fragment's world position, so falloff is a
    // true sphere in world space (rendered as an iso-foreshortened ellipse
    // on-screen). Up to kMaxPointLights per draw; extras silently dropped.
    static constexpr int32_t kMaxPointLights = 16;
    void ClearPointLights();
    void AddPointLight(float wx, float wy, float wz, float radius_wu,
                       float r, float g, float b, float intensity);

    // The color render target that BeginTilePass draws into. Exposed so
    // callers can composite it onto the backbuffer or swapchain.
    sg_image ColorTarget() const { return color_target; }

    // Deferred-shading reconstruction params. The light pass reads only the
    // G-buffer (albedo + normal + depth) and reconstructs per-fragment world
    // position from (screen xy, depth) via the iso inverse. Call once per
    // frame before RunLightingPass().
    //   ox, oy         — screen-pixel position of the camera origin on screen
    //                    (sector center plus debug pan in the test harness)
    //   z_near, z_far  — camera-depth range in world units (maps to [0,1])
    //   center_wx/y    — world-space camera center xy
    //   kcam_forward   — camera-forward distance (kCamForwardWU)
    //   reserved       — unused, reserved for future reconstruction tweaks
    void SetReconstructionParams(float ox, float oy,
                                 float z_near, float z_far,
                                 float center_wx, float center_wy,
                                 float kcam_forward, float reserved);

    // Run the deferred lighting pass over the G-buffer filled by the
    // preceding BeginTilePass/DrawTile/EndTilePass calls. Output lands in
    // the internal lit_target, which FlipPage composites to the swapchain.
    void RunLightingPass();

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

    // Sokol graphics resources. default_pass is now a G-buffer fill pass:
    //   color attachment 0 → color_target  (albedo, RGBA8)
    //   color attachment 1 → normal_target (world normal packed *0.5+0.5
    //                                       into RGBA16F)
    //   depth attachment   → depth_target  (camera-depth 0..1)
    // Lighting runs in a second pass reading those three, writing to
    // lit_target, which FlipPage composites to the swapchain.
    sg_pass default_pass;             // G-buffer fill pass
    sg_pass lit_pass;                 // Light accumulation pass (-> lit_target)
    sg_pass depth_pass;               // Pass for depth pre-pass (unused)
    sg_shader   tile_shader   = {};
    sg_pipeline tile_pipeline = {};
    sg_buffer   tile_vbuf     = {};
    sg_shader   light_shader    = {};
    sg_pipeline light_pipeline  = {};
    bool        color_target_dirty = false;  // Set by BeginTilePass, read by FlipPage
    bool        lit_target_dirty   = false;  // Set by RunLightingPass, read by FlipPage
    // BeginTilePass's clear color is the scene backdrop. Stored here so the
    // light pass clears lit_target to it — tile fragments overwrite, anything
    // outside a drawn tile stays the backdrop color.
    float       tile_clear_rgba[4] = { 0, 0, 0, 1 };

    // Scene light used by the tile fragment shader. Defaults are tuned so
    // a flat surface (normal = +Z) doesn't clamp to 1.0 — without that,
    // flat ground looks identical to the unlit pass. A mostly-horizontal
    // light gives flat ground ~0.5 and lets sloped faces read brighter.
    struct {
        float dir[3]     = { 0.6f, -0.6f, 0.4f };
        float intensity  = 1.0f;
        float color[3]   = { 1.0f, 1.0f, 1.0f };
        float ambient    = 0.25f;
        float normal_radius = 1.5f;   // texels; wider = smoother normals
        float edge_threshold = 0.001f; // zraw units; bilateral reject cutoff
        int32_t view_mode = 0;
        // Screen-space point lights. Each light is (pos xy, z_above, radius)
        // + (rgb, intensity); shader loops to `plight_count`.
        int32_t plight_count = 0;
        float plight_pos[kMaxPointLights][4] = {};
        float plight_col[kMaxPointLights][4] = {};
    } light;
    void InitTilePipeline();
    void ShutdownTilePipeline();
    void InitLightPipeline();
    void ShutdownLightPipeline();

    // Reconstruction params for the light pass — updated once per frame via
    // SetReconstructionParams(). Packed into 2 vec4s for the light shader's
    // uniform block. See SetReconstructionParams() docs for units.
    struct {
        float ox = 0, oy = 0;
        float z_near = 0, zspan = 1;
        float center_wx = 0, center_wy = 0;
        float kcam_forward = 0;
        float reserved = 0;
    } recon;

    // Render targets
    sg_image color_target;            // G-buffer RT0: albedo (RGBA8)
    sg_image depth_target;            // G-buffer depth-stencil (DEPTH) for HW z-test
    sg_image normal_target;           // G-buffer RT1: world normal (RGBA16F)
    sg_image scene_z_target;          // G-buffer RT2: scene_z in [0,1] (R32F).
                                      //   Separate from depth_target because
                                      //   sokol's Metal backend doesn't
                                      //   support sampling the depth-stencil
                                      //   attachment as a regular texture.
    sg_image lit_target;              // Light pass output (RGBA8) -> swapchain
    
    // Uniform buffers
    struct {
        sg_buffer view_proj;          // View-projection matrix
        sg_buffer model;              // Model matrix
        sg_buffer light_params;       // Light position, color, etc
    } uniforms;
    
    // Display dimensions/bitsperpixel live on the TSurface base class.
};
