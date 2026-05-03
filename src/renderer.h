// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *           renderer.h  - Core Render Engine (Game-Agnostic)            *
// *************************************************************************
//
// TRenderer is Revenant's core render engine. It owns every piece of GPU
// state the game draws through: sokol pipelines, shaders, render targets,
// the G-buffer, pass objects, the composite quad, directional+point light
// state, and reconstruction params. Everything above it (TMapRenderer,
// TMeshRenderer, TScreen UI) is a *producer* that submits draws into
// TRenderer's API. TRenderer itself knows NOTHING about sectors, tiles,
// object instances, imagery, animators, or any T3D*/TObject*-anything --
// it speaks only in pipelines, images, uniforms, and screen pixels.
//
// *************************************************************************
// * ARCHITECTURE --------------------------------------------------------- *
// *************************************************************************
//
// Three layers, strictly enforced:
//
//   (1)  CORE RENDERER (this class, TRenderer)
//        - Sokol pipelines, shaders, vertex buffers
//        - Render targets: albedo / normal / scene_z / depth / ao / lit
//        - Pass objects and frame-graph glue
//        - Composite quad (UI + scene blit)
//        - Directional + point light state, AO, sun shadow
//        - Reconstruction params for the deferred light pass
//        - Atlas / mesh / material registry (handle-based, forthcoming)
//        Submission API:  BeginTilePass / DrawTile / EndTilePass,
//                         RunLightingPass, Composite(...)
//        Knows nothing about game data.
//
//   (2)  GAME CONSUMERS (TMapRenderer, future TMeshRenderer, ...)
//        - Walk game-side data (TSector/TObjectInstance, T3DImagery, ...)
//        - Cull, sort, curate draw lists
//        - Call TRenderer's submission API
//        - Do NOT own pipelines, render targets, or pass objects
//
//   (3)  UI  (TScreen / TPane)
//        - Widget composition, font atlases, ImGui backend
//        - Submits via TRenderer::Composite or runs its own pass layer
//        - Never routes UI draws through TMapRenderer
//
// Typical frame (pass pipeline, in order):
//
//    [1] G-BUFFER FILL       -- default_pass  (MRT: albedo, normal, scene_z)
//    [2] AMBIENT OCCLUSION   -- ao_pass       (R32F  screen-space AO)
//    [3] DEFERRED LIGHTING   -- lit_pass      (albedo * shading -> lit_target)
//    [4] WATER / REFRACTION  -- (future, ping-pong sceneColor A<->B)
//    [5] TRANSPARENT EFFECTS -- (future)
//    [6] DEBUG 3D            -- (future, line/triangle primitives)
//    [7] GAME UI             -- composite onto backbuffer or swapchain
//    [8] DEBUG UI            -- ImGui overlay in swapchain pass
//
// The TRenderer G-buffer is allocated larger than the display (kGBufPad on
// every side) so screen-space effects that sample neighbors -- particularly
// the sun-shadow ray march -- find occluders that are just off the visible
// edge. PresentToSwapchain composites only the centered display-sized
// sub-rect of lit_target to the final swapchain pass.
//
// *************************************************************************
// * RELATIONSHIP TO TDisplay -------------------------------------------- *
// *************************************************************************
//
// TDisplay has been demoted to a swapchain / TSurface wrapper. It holds the
// backbuffer/frontbuffer/zbuffer TSurfaces, handles sokol context setup,
// and its FlipPage delegates the final blit to TRenderer::PresentToSwap-
// chain. All pipeline state, render targets, and submission APIs that used
// to live on TDisplay are now owned here. Most direct Display-> calls in
// game code should migrate to Renderer-> over time; only TSurface-style
// legacy blits (Put/WriteText/Box/ZPut/...) stay on TDisplay.
//
// *************************************************************************

#pragma once

#include "revenant.h"

#include <sokol_gfx.h>

#include <vector>

_CLASSDEF(TRenderer)
_CLASSDEF(TSurface)

// obj_id layout: low 28 bits hold the per-drawable id (drawable index +
// 1); the top 4 bits are state flags the editor can set per-submission
// to drive shader-side overlays (outline, hover tint, ...). The lit
// shader reads the alpha channel of id_target and tests the flag bits;
// click-pick reads the same pixel and masks them off.
constexpr uint32_t kObjFlagSelected = 1u << 31;
constexpr uint32_t kObjFlagHovered  = 1u << 30;
constexpr uint32_t kObjIdMask       = 0x0FFFFFFFu;
constexpr uint32_t kObjFlagMask     = 0xF0000000u;

// Per-tile submission payload. Scene code fills one of these per visible
// tile and hands it to TRenderer::SubmitTile; TRenderer accumulates them
// during a tile pass and emits draws at EndTilePass. Scene code never
// issues GPU work directly.
struct STileSubmit
{
    sg_image color_img;
    sg_image depth_img;
    int32_t  dst_x, dst_y, dst_w, dst_h;
    float    anchor_z;        // normalized scene-z of the tile anchor [0..1]
    float    depth_mul;       // bitmap-z -> normalized-scene-z scale
    float    normal_mul;      // normal-reconstruction depth scale
    float    root_wx, root_wy, root_wz;   // world xyz of the tile anchor
    float    anchor_px_x, anchor_px_y;    // source-image anchor pixel
    float    zraw_to_wu;      // bitmap-z -> world-units scale
    float    zraw_min = 0.0f;
    float    zraw_max = 0.0f;
    float    sort_depth = 0.0f;
    uint32_t obj_id = 0;      // packed into id_target (RGBA8) for picking
};

struct SOverlaySubmit
{
    sg_image color_img;
    int32_t  dst_x, dst_y, dst_w, dst_h;
    int32_t  src_x = 0, src_y = 0, src_w = 1, src_h = 1;
    int32_t  src_tex_w = 1, src_tex_h = 1;
    bool     additive_blend = false;
    bool     chroma_key = false;
    float    chroma_key_rgb[3] = {1.0f, 0.0f, 0.0f};
};

// Opaque handle to a mesh registered with TRenderer. 0 is invalid.
using MeshHandle = uint32_t;

// Per-vertex layout for rigid meshes (mesh slot 0). Tight-packed, 32 bytes.
struct SMeshVertex
{
    float pos[3];
    float normal[3];
    float uv[2];
};

// Per-instance submission payload. Scene code fills one of these per visible
// mesh instance and hands it to TRenderer::SubmitMesh. TRenderer sorts by
// mesh handle, uploads a dynamic instance vertex buffer, and emits one
// instanced draw per mesh at EndTilePass (instancing is always on; a single
// instance just means count=1).
struct SMeshSubmit
{
    MeshHandle mesh;
    float      world[16];     // row-major 4x4
    float      tint[4];       // rgba multiplier
    uint32_t   obj_id = 0;    // packed into id_target (RGBA8) for picking
};

struct SHelperMeshSubmit
{
    MeshHandle mesh;
    float      world[16];
    bool       shadow_plane = false;
    bool       additive_blend = false;
    float      diffuse[4];
    float      ambient[4];
    float      specular[4];
    float      emissive[4];
    float      power;
    float      sort_depth = 0.0f;
};

enum class ETransparentWorldKind : uint8_t
{
    Tile,
    Helper,
};

struct STransparentWorldSubmit
{
    ETransparentWorldKind kind = ETransparentWorldKind::Tile;
    float                 sort_depth = 0.0f;
    STileSubmit           tile = {};
    SHelperMeshSubmit     helper = {};
};

class TRenderer
{
public:
    // Pad the G-buffer by this many pixels on every side so off-screen
    // geometry can still feed screen-space effects (sun-shadow ray-march).
    static constexpr int32_t kGBufPad = 128;

    // Max simultaneous point lights per deferred light pass. Extras are
    // silently dropped by AddPointLight.
    static constexpr int32_t kMaxPointLights = 16;

    TRenderer();
    ~TRenderer();

    // Bring the core renderer online. Allocates the G-buffer, AO, and
    // lit render targets at (width + 2*kGBufPad) x (height + 2*kGBufPad),
    // creates every pipeline, and sets up ImGui. Assumes sg_setup() has
    // already run (TDisplay::Initialize drives that).
    bool Initialize(int32_t width, int32_t height);
    // Tear down pipelines, shaders, RTs. Leaves sg_shutdown to TDisplay.
    void Shutdown();

    [[nodiscard]] int32_t Width()  const { return width; }
    [[nodiscard]] int32_t Height() const { return height; }

    // ---- G-buffer fill (pass [1]) ---------------------------------------
    // Begin a pass on the MRT G-buffer (albedo/normal/scene_z + depth),
    // clearing to the caller-supplied backdrop color. Scene code calls
    // SubmitTile per visible tile -- TRenderer accumulates them and emits
    // draws at EndTilePass. The fragment shader writes per-pixel depth
    // from the sampled bitmap-z plus anchor_z, so tiles interpenetrate
    // correctly in world space.
    void BeginTilePass(float r, float g, float b, float a);
    void SubmitTile(const STileSubmit& t);
    void SubmitTransparentTile(const STileSubmit& t);
    void SubmitOverlay(const SOverlaySubmit& t);
    void SubmitMesh(const SMeshSubmit& m);
    void SubmitHelperMesh(const SHelperMeshSubmit& m);
    void EndTilePass();

    // ---- Mesh registry -------------------------------------------------
    // Register a rigid mesh. TRenderer owns the resulting GPU buffers for
    // the life of the renderer; there is no unregister today (meshes
    // accumulate across a session). Returns 0 on failure.
    MeshHandle RegisterMesh(const SMeshVertex* verts, int32_t num_verts,
                            const uint16_t*  indices, int32_t num_indices,
                            sg_image albedo);
    void SetMeshAlbedo(MeshHandle mesh, sg_image albedo);

    // ---- Scene lighting state -------------------------------------------
    // Directional sun. Normals are reconstructed per-fragment from the tile
    // depth map in the light shader, so a near-horizontal direction gives
    // a flat ground about 0.5 and lets angled surfaces read brighter.
    void SetLight(float dx, float dy, float dz, float intensity,
                  float r, float g, float b, float ambient);
    // Ambient floor color, applied independently of the sun's tint.
    void SetAmbientColor(float r, float g, float b);
    // Z-buffer-based screen-space ambient occlusion.
    void SetAmbientOcclusion(bool enable, float radius_px, float strength,
                             float bias, float max_dist_wu);
    // Normal-reconstruction central-difference stencil radius (texels).
    void SetNormalRadius(float texels);
    // Bilateral edge threshold for normal reconstruction, in uploaded
    // bitmap-z units. Neighbor samples farther than this from the center
    // get clamped to the center -- kills the "bevel" seam artifact.
    void SetEdgeThreshold(float zraw_units);
    // Blend between flat sun tint/intensity (0) and full N.L directional
    // lighting (1). Tones down the speckled look of reconstructed normals.
    void SetNormalLightingHardness(float hardness);
    // Debug view modes for the lighting pass:
    //   0 = lit          1 = albedo     2 = depth       3 = normals
    //   4 = point-only   5 = recon heat 6 = shadow mask 7 = AO only
    void SetTileViewMode(int32_t mode);
    // 0 = retail 1998 (ambient + distance-only point lights, no sun,
    //                  no shadows, no AO)
    // 1 = modern (adds directional sun + screen-space contact shadows)
    void SetLightingMode(int32_t mode);
    // Sun contact-shadow ray march.
    void SetSunShadow(bool enable, float step_wu, float softness_px,
                      int32_t max_steps);
    // Toggle just the enable flag (params untouched). Editor uses this
    // to suppress shadows while panels are open.
    void SetSunShadowEnabled(bool enable);
    [[nodiscard]] bool SunShadowEnabled() const;
    // Base world-space direction for the sun-shadow ray. Decoupled from
    // light direction so shadows can cast toward the viewer when lighting
    // comes from behind the scene.
    void SetShadowWorldDir(float dx, float dy, float dz);
    // Debug variance applied on top of the derived shadow ray.
    void SetShadowVariance(float sx, float sy, float sz);

    // ---- Point lights ---------------------------------------------------
    // Rebuild each frame: ClearPointLights() then AddPointLight(...) per.
    // Position is world xyz, radius is world units (linear-then-squared
    // falloff to 0 at radius). Up to kMaxPointLights per frame; extras
    // silently dropped.
    void ClearPointLights();
    void AddPointLight(float wx, float wy, float wz, float radius_wu,
                       float r, float g, float b, float intensity);

    // ---- Deferred reconstruction (pass [3]) -----------------------------
    // Packed per-frame params for the light shader's iso inverse. Call
    // once per frame before RunLightingPass.
    //   ox, oy       -- screen-pixel camera-origin (display space)
    //   z_near/z_far -- camera-depth range in wu (maps to scene_z [0,1])
    //   center_wx/y  -- world camera-center xy
    //   kcam_forward -- camera-forward distance (MapRenderer kCamForwardWU)
    //   perspective  -- non-zero enables perspective projection
    //   zoom         -- camera-plane scale multiplier (1 = authored scale)
    //   z_offset/z_scale -- debug tweaks for perspective tile relief depth
    void SetReconstructionParams(float ox, float oy,
                                 float z_near, float z_far,
                                 float center_wx, float center_wy,
                                 float kcam_forward, float perspective,
                                 float zoom = 1.0f,
                                 float z_offset = 0.0f,
                                 float z_scale = 1.0f,
                                 float tile_scale = 1.0f);
    void SetPerspectiveRaycastParams(int32_t steps, int32_t refine);
    void SetPerspectiveDebugMode(int32_t mode) { perspective_debug_mode = mode; }

    // Run the deferred lighting pass over the current G-buffer. Writes
    // lit_target (passes [2] + [3]).
    void RunLightingPass();

    // ---- Composite (UI blit + swapchain present) ------------------------
    // Fullscreen blit of a TSurface into the current render pass (used by
    // TDisplay::FlipPage to paint the backbuffer onto the swapchain).
    void Composite(TSurface* src);
    // Sub-rect blit of an image into the current render pass (RGBA8 RT
    // variant -- caller is inside that RT's pass).
    void Composite(sg_image img,
                   int32_t dst_x, int32_t dst_y, int32_t dst_w, int32_t dst_h,
                   int32_t target_w, int32_t target_h);
    // Atlas-friendly sub-rect blit: also takes a pixel-space source rect
    // within the image.
    void Composite(sg_image img,
                   int32_t dst_x, int32_t dst_y, int32_t dst_w, int32_t dst_h,
                   int32_t target_w, int32_t target_h,
                   int32_t src_x, int32_t src_y, int32_t src_w, int32_t src_h,
                   int32_t src_tex_w, int32_t src_tex_h,
                   bool additive_blend = false,
                   bool chroma_key = false,
                   const float* chroma_key_rgb = nullptr);

    // Called from TDisplay::FlipPage inside sg_begin_default_pass. Picks
    // the best final image (lit_target > color_target > nothing) and
    // composites it onto the swapchain using the centered display-sized
    // sub-rect of the padded G-buffer. Returns true if it drew anything.
    bool PresentToSwapchain();

    // Where to composite the game image onto the swapchain. Default is
    // the entire window. Editor mode disables this (the game render
    // arrives in the editor's Game View panel via ImGui::Image of the
    // lit_target sokol image) so the present blit can be skipped.
    void SetPresentNDCRect(float x, float y, float w, float h);
    void ResetPresentNDCRect() { SetPresentNDCRect(-1.0f, -1.0f, 2.0f, 2.0f); }
    void SuppressPresent(bool on) { suppress_present = on; }

    // ---- Debug accessors ------------------------------------------------
    [[nodiscard]] sg_image ColorTarget() const { return color_target; }
    [[nodiscard]] sg_image LitTarget()   const { return lit_target; }
    [[nodiscard]] sg_image IdTarget()    const { return id_target; }

    // Editor selection-outline plumbing. Setting this >0 turns on the
    // post-process outline glow in the lit shader for pixels whose
    // id_target value matches `id`.
    void SetSelectedObjectId(uint32_t id) { selected_obj_id = id; }
    [[nodiscard]] uint32_t SelectedObjectId() const { return selected_obj_id; }
    [[nodiscard]] int32_t  GBufPad()     const { return kGBufPad; }

private:
    int32_t width  = 0;
    int32_t height = 0;

    // ---- Composite primitive (shared quad) ------------------------------
    sg_shader   composite_shader   = {};
    sg_buffer   composite_vbuf     = {};
    sg_pipeline composite_pip_rt   = {};   // RGBA8 RT variant
    sg_pipeline composite_pip_add_rt = {}; // RGBA8 additive/lighten RT variant
    sg_pipeline composite_pip_swap = {};   // Swapchain variant

    // ---- Passes ---------------------------------------------------------
    sg_pass default_pass = {};   // G-buffer MRT fill
    sg_pass ao_pass      = {};   // AO -> ao_target
    sg_pass lit_pass     = {};   // Deferred lighting -> lit_target
    sg_pass depth_pass   = {};   // Reserved for a depth-only prepass

    // ---- Tile (G-buffer fill) pipeline ----------------------------------
    sg_shader   tile_shader   = {};
    sg_pipeline tile_pipeline = {};
    sg_buffer   tile_vbuf     = {};

    // ---- Screen-space AO pipeline ---------------------------------------
    sg_shader   ao_shader   = {};
    sg_pipeline ao_pipeline = {};

    // ---- Deferred light pipeline ----------------------------------------
    sg_shader   light_shader   = {};
    sg_pipeline light_pipeline = {};

    // Accumulated tile submissions for the current tile pass; drained in
    // EndTilePass. Capacity is preserved across frames to avoid churn.
    std::vector<STileSubmit> tile_queue;
    std::vector<STransparentWorldSubmit> transparent_world_queue;
    std::vector<SOverlaySubmit> overlay_queue;
    std::vector<SMeshSubmit> mesh_queue;

    // ---- Mesh pipeline -------------------------------------------------
    sg_shader   mesh_shader      = {};
    sg_pipeline mesh_pipeline    = {};
    sg_shader   transparent_tile_shader   = {};
    sg_pipeline transparent_tile_pipeline = {};
    sg_shader   helper_mesh_shader        = {};
    sg_pipeline helper_mesh_back_pipeline = {};
    sg_pipeline helper_mesh_front_pipeline = {};
    sg_pipeline helper_mesh_add_back_pipeline = {};
    sg_pipeline helper_mesh_add_front_pipeline = {};
    sg_buffer   mesh_instance_vb = {};   // dynamic, rebuilt each frame
    static constexpr int32_t kMaxMeshInstances = 2048;

    struct SMeshEntry {
        sg_buffer vbuf;
        sg_buffer ibuf;
        int32_t   num_indices;
        sg_image  albedo;
    };
    std::vector<SMeshEntry> meshes;   // index+1 is MeshHandle

    // Dirty flags -- read by PresentToSwapchain.
    bool color_target_dirty = false;
    bool lit_target_dirty   = false;

    // NDC sub-rect for the present blit. Default fills the swapchain.
    float present_ndc[4] = { -1.0f, -1.0f, 2.0f, 2.0f };
    bool  suppress_present = false;
    float tile_clear_rgba[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    int32_t perspective_debug_mode = 0;
    int32_t perspective_steps = 32;
    int32_t perspective_refine = 5;

    // ---- Render targets -------------------------------------------------
    sg_image color_target   = {};   // G-buffer: albedo   (RGBA8)
    sg_image depth_target   = {};   // G-buffer: HW depth (DEPTH)
    sg_image normal_target  = {};   // G-buffer: normal   (RGBA16F)
    sg_image scene_z_target = {};   // G-buffer: scene-z  (R32F)
    sg_image id_target      = {};   // G-buffer: obj id   (RGBA8 packed)
    uint32_t selected_obj_id = 0;   // editor selection (0 = no outline)
    sg_image ao_target      = {};   // AO pass output     (R32F)
    sg_image lit_target     = {};   // Lit output         (RGBA8)
    sg_pass  helper_pass    = {};   // Forward helper/material pass into lit_target

    // ---- Lighting state (uploaded by RunLightingPass) -------------------
    struct SLightState {
        float dir[3]           = { 0.6f, -0.6f, 0.4f };
        float intensity        = 1.0f;
        float color[3]         = { 1.0f, 1.0f, 1.0f };
        float ambient_color[3] = { 1.0f, 1.0f, 1.0f };
        float ambient          = 0.25f;
        bool  ao_enable        = true;
        float ao_radius_px     = 12.0f;
        float ao_strength      = 1.0f;
        float ao_bias          = 0.15f;
        float ao_max_dist_wu   = 96.0f;
        float normal_radius    = 1.5f;    // texels
        float edge_threshold   = 0.001f;  // zraw units
        float normal_lighting_hardness = 0.5f;
        int32_t view_mode      = 0;
        int32_t mode           = 1;       // 0 retail, 1 modern
        bool    sun_shadow_enable     = true;
        float   sun_shadow_step_wu    = 24.0f;
        float   sun_shadow_softness_px = 3.0f;
        int32_t sun_shadow_max_steps  = 32;
        float   shadow_world_dir[3] = { 0.6f, -0.6f, 0.4f };
        float   shadow_dir[3]       = { 0.0f, 0.0f, 1.0f };
        int32_t plight_count = 0;
        float   plight_pos[kMaxPointLights][4] = {};
        float   plight_col[kMaxPointLights][4] = {};
    } light;

    // ---- Reconstruction params (per frame) ------------------------------
    struct SReconState {
        float ox = 0, oy = 0;
        float z_near = 0, zspan = 1;
        float center_wx = 0, center_wy = 0;
        float kcam_forward = 0;
        float reserved = 0;
        float zoom = 1.0f;
        float z_offset = 0.0f;
        float z_scale = 1.0f;
        float tile_scale = 1.0f;
    } recon;

    // ---- Pipeline setup / teardown --------------------------------------
    void InitCompositePipeline();
    void ShutdownCompositePipeline();
    void InitTilePipeline();
    void ShutdownTilePipeline();
    void InitAOPipeline();
    void ShutdownAOPipeline();
    void InitLightPipeline();
    void ShutdownLightPipeline();
    void RunAOPass();

    // Emit one queued tile as a single sg_draw (no instancing yet).
    void EmitTile(const STileSubmit& t);
    void DrainOverlayQueue();

    // Mesh pipeline lifecycle + per-frame drain.
    void InitMeshPipeline();
    void ShutdownMeshPipeline();
    void DrainMeshQueue();
    void EmitTransparentTile(const STileSubmit& t);
    void EmitTransparentHelper(const SHelperMeshSubmit& s);
    void DrainTransparentWorldQueue();
};

// Global renderer instance -- created by TDisplay::Initialize, destroyed by
// TDisplay::Close. All game-side renderer-specific calls (tile submission,
// lighting state, composites) go through this pointer. Non-renderer surface
// calls (Put, WriteText, Box, ZPut, ...) still go through Display->.
extern TRenderer* Renderer;
