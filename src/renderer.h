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
//    [3] SUN SHADOW MASK     -- shadow_pass   (R32F, low-res + blur)
//    [4] DEFERRED LIGHTING   -- lit_pass      (albedo * shading -> lit_target)
//    [5] WATER / REFRACTION  -- (future, ping-pong sceneColor A<->B)
//    [6] TRANSPARENT EFFECTS -- (future)
//    [7] DEBUG 3D            -- (future, line/triangle primitives)
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
// to live on TDisplay are now owned here. Most direct Display. calls in
// game code should migrate to Renderer-> over time; only TSurface-style
// legacy blits (Put/WriteText/Box/ZPut/...) stay on TDisplay.
//
// *************************************************************************

#pragma once

#include "revenant.h"
#include "render3d_types.h"

#include <sokol_gfx.h>

#include <cstddef>
#include <cstdint>
#include <unordered_map>
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

// Opaque handles to renderer-owned resources. 0 is invalid.
using MeshHandle = uint32_t;
using RendererImagePairHandle = uint32_t;

// Per-tile submission payload. Scene code fills one of these per visible
// tile and hands it to TRenderer::SubmitTile; TRenderer accumulates them
// during a tile pass and emits draws at EndTilePass. Scene code never
// issues GPU work directly.
struct STileSubmit
{
    RendererImagePairHandle image_pair = 0;
    int32_t  dst_x, dst_y, dst_w, dst_h;
    int32_t  src_w = 0, src_h = 0;  // source sprite dimensions before viewport scaling
    float    anchor_z;        // normalized scene-z of the tile anchor [0..1]
    float    depth_mul;       // bitmap-z -> normalized-scene-z scale
    float    normal_mul;      // normal-reconstruction depth scale
    float    root_wx, root_wy, root_wz;   // world xyz of the tile anchor
    float    anchor_px_x, anchor_px_y;    // source-image anchor pixel
    // Conservative source-pixel coverage of valid heightfield data. Perspective
    // relief projection uses this to shrink the proxy draw rect; the fragment
    // shader still samples the full source texture, so this is only a safe
    // rasterization bound, not a UV remap.
    float    coverage_px_x0 = 0.0f, coverage_px_y0 = 0.0f;
    float    coverage_px_x1 = 0.0f, coverage_px_y1 = 0.0f;
    float    anchor_cam_x = 0.0f, anchor_cam_y = 0.0f; // logical camera-plane anchor
    float    zraw_to_wu;      // bitmap-z -> world-units scale
    float    zraw_min = 0.0f;
    float    zraw_max = 0.0f;
    float    sort_depth = 0.0f;
    uint32_t obj_id = 0;      // packed into id_target (RGBA8) for picking
};

struct SOverlaySubmit
{
    TTextureHandle texture = kInvalidTexture;
    RendererImagePairHandle image_pair = 0;
    int32_t  dst_x, dst_y, dst_w, dst_h;
    int32_t  src_x = 0, src_y = 0, src_w = 1, src_h = 1;
    int32_t  src_tex_w = 1, src_tex_h = 1;
    bool     additive_blend = false;
    bool     chroma_key = false;
    float    chroma_key_rgb[3] = {1.0f, 0.0f, 0.0f};
};

// *************************************************************************
// * FX submission API  (Phase 1 VFX spine, see docs/vfx/PHASE1_SPINE.md)  *
// *************************************************************************
//
// World-space billboards, particles, and strips/ribbons all flow through
// these three submit calls plus the bucket batcher on TRenderer. The
// caller authors world-unit sizes; perspective / camera basis is built in
// the renderer's fx_billboard / fx_particle / fx_strip vertex shaders.
// Game-side effect code (TFlipbookBillboardComponent, TParticleEffect-
// Component, future strip components) calls SubmitFx* from inside the
// existing per-instance Submit walk in maprenderer.cpp -- no new walk.
// DrainFxQueue runs after RunLightingPass in a dedicated fx_pass that
// reads scene depth and writes the lit color target.
//
// *************************************************************************

// Canonical per-bucket FX blend modes. Mirror of EParticleBlendMode in
// particlefx.h -- producers fill in EParticleBlendMode, the renderer maps
// it to EFxBlend at submission time. AdditiveStraight (ONE/ONE) is the
// retail "self-lit overlay" mode that TBloodSystem::Render uses for its
// second pass; Additive (SRC_ALPHA/ONE) is the alpha-weighted additive
// the existing smoke/spark buckets use.
enum class EFxBlend     : uint8_t { Alpha = 0, Additive = 1, AdditiveStraight = 2, PremulAlpha = 3 };
// Depth-buffer interaction. TestWrite is for alpha-tested / mostly-opaque
// particles (decal impostors) that should occlude later transparent fx;
// TestNoWrite is the standard transparent path; None is for always-on-top.
enum class EFxDepthMode : uint8_t { TestNoWrite = 0, None = 1, TestWrite = 2 };
enum class EFxLightMode : uint8_t { Unlit = 0, LitFlat = 1 };
enum class EFxDebugMode : uint8_t { Normal = 0, SolidColor = 1, FullTexture = 2, CurrentFrame = 3, Wireframe = 4 };

// Global wireframe override — when true, every FX submit (Billboard /
// Particle / Strip) renders as a thin quad outline regardless of its
// per-instance debug_mode. Set from CLI --vfx-wireframe at startup.
// Useful for verifying rotation, projection, screen-space distortion etc.
extern bool g_fx_wireframe_override;

// Per-billboard / per-particle quad orientation. Standard particle-engine
// knob (cf. Unity Billboard / Stretched / Horizontal / Vertical / Mesh
// and Unreal "Camera Position" / "Custom" alignment). The default
// (ScreenAligned) matches the original Phase 1 behaviour: the 4 corners
// expand along the camera right/up basis after iso projection, so the
// quad always faces the viewer.
//
// WorldXY expands the 4 corners along world +X / +Y at the billboard's
// world position BEFORE iso projection -- the resulting quad lies flat
// on the world XY plane and foreshortens correctly under the iso /
// perspective camera, which is what ground-projected effects (ripples
// on water, halos on the ground, AoE rings, decal-style overlays) want.
//
// Default ScreenAligned preserves existing behaviour for every effect
// that does not opt in (F01 flame, F03 fire, X17 flare, etc.) -- no
// migration required for screen-aligned glow/spark sprites.
enum class EFxBillboardOrientation : uint8_t {
    ScreenAligned = 0,   // 4 corners expand along camera basis (default)
    WorldXY       = 1,   // 4 corners expand along world +X / +Y (flat on
                         // XY plane; correct foreshortening under iso/persp)
    // Future:
    //   WorldUpAligned        -- Z-axis-aligned, flame-style
    //   StretchedAlongVelocity -- motion-streak particles
};

// Identifies which fx pipeline owns a submission. The renderer keeps one
// dynamic vertex buffer per pipeline; equality of (texture, pipeline_id,
// blend, depth_mode) lets adjacent draws coalesce into a single instanced
// draw call.
enum class EFxPipeline  : uint16_t { Billboard = 1, Particle = 2, Strip = 3 };

struct SFxBatchKey
{
    TTextureHandle texture     = kInvalidTexture;
    uint16_t       pipeline_id = uint16_t(EFxPipeline::Billboard);
    uint8_t        blend       = uint8_t(EFxBlend::Alpha);
    uint8_t        depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);

    [[nodiscard]] bool Equals(const SFxBatchKey& rhs) const
    {
        return texture == rhs.texture
            && pipeline_id == rhs.pipeline_id
            && blend == rhs.blend
            && depth_mode == rhs.depth_mode;
    }
};

// One persistent screen-aligned world-space billboard. Sizes are in world
// units (NOT pixels) -- the renderer projects per-vertex.
struct SBillboardDrawItem
{
    float        world_pos[3]   = {0.0f, 0.0f, 0.0f};
    float        size_wu[2]     = {1.0f, 1.0f};
    float        color_rgba[4]  = {1.0f, 1.0f, 1.0f, 1.0f};
    float        uv_rect[4]     = {0.0f, 0.0f, 1.0f, 1.0f};   // x,y,w,h normalized
    SFxBatchKey  key            = {};
    EFxDebugMode debug_mode     = EFxDebugMode::Normal;
    // Per-instance per PHASE1_SPINE.md §6: lighting is a runtime
    // multiplier in the FS, not a pipeline variant -- avoids fanning
    // the pipeline count out by 2x.
    EFxLightMode light_mode     = EFxLightMode::Unlit;
    // Per-instance quad orientation. Default ScreenAligned keeps the
    // existing screen-aligned billboard expansion (no migration needed
    // for any pre-orientation-knob effect). Ground/water-projected
    // effects (H03 ripple, L02 halo, future AoE rings, decal overlays)
    // opt in to WorldXY.
    EFxBillboardOrientation orientation = EFxBillboardOrientation::ScreenAligned;
};

// Same fields as SBillboardDrawItem plus per-instance rotation. Bulk
// submission is by bucket pointer (see SubmitFxParticleBucket); this
// struct exists for one-off submission and as a logical wire shape.
struct SParticleDrawItem
{
    float        world_pos[3]   = {0.0f, 0.0f, 0.0f};
    float        size_wu[2]     = {1.0f, 1.0f};
    float        color_rgba[4]  = {1.0f, 1.0f, 1.0f, 1.0f};
    float        uv_rect[4]     = {0.0f, 0.0f, 1.0f, 1.0f};
    float        rotation_rad   = 0.0f;
    SFxBatchKey  key            = {};
    EFxDebugMode debug_mode     = EFxDebugMode::Normal;
    EFxLightMode light_mode     = EFxLightMode::Unlit;
    // Per-instance quad orientation. Same shape as SBillboardDrawItem
    // -- lets PE effects opt into WorldXY for ground-projected particle
    // swarms (e.g. a ring of glow puffs on the floor). Default
    // ScreenAligned keeps every existing PE bucket untouched.
    EFxBillboardOrientation orientation = EFxBillboardOrientation::ScreenAligned;
};

// A single screen-aligned ribbon segment (world A -> world B). Strips
// expand to a camera-aligned quad in the fx_strip vertex shader.
struct SStripSegment
{
    float world_a[3]   = {0.0f, 0.0f, 0.0f};
    float world_b[3]   = {0.0f, 0.0f, 0.0f};
    float width_a_wu   = 1.0f;
    float width_b_wu   = 1.0f;
    float color_a[4]   = {1.0f, 1.0f, 1.0f, 1.0f};
    float color_b[4]   = {1.0f, 1.0f, 1.0f, 1.0f};
    float u_a          = 0.0f;
    float u_b          = 1.0f;
    // V sub-range — left edge of the strip's quad samples `v_left`,
    // right edge samples `v_right`. Default 0..1 spans the full
    // texture height (the pre-existing behaviour). Used by the
    // LightStrip lightning bolt to pick one of 8 horizontally-stacked
    // crackle patterns per tick (flipbook-via-V-cell) — added 2026-05-31
    // for S04 TLightningAnimator_Bespoke iter5.
    float v_left       = 0.0f;
    float v_right      = 1.0f;
};

struct SStripDrawItem
{
    const SStripSegment* segments     = nullptr;
    int32_t              num_segments = 0;
    SFxBatchKey          key          = {};
    EFxDebugMode         debug_mode   = EFxDebugMode::Normal;
    EFxLightMode         light_mode   = EFxLightMode::Unlit;
};

class TParticleBucket;   // forward, see particlefx.h

// Renderer-owned paired images, currently used by world sprites that need both
// color and depth textures. The renderer owns the sg_image lifetimes and ref
// counts; game systems keep only RendererImagePairHandle values.
struct SRendererImagePairInfo
{
    uint64_t key = 0;
    int32_t width = 0;
    int32_t height = 0;
    int32_t height_minmax_mips = 0;
    uint32_t tight_proxy_points = 0;
    uint64_t gpu_bytes = 0;
    uint32_t ref_count = 0;
};

// Renderer-owned single texture image. TTextureHandle is shared with the
// legacy 3D imagery vocabulary, but the image lifetime belongs here.
struct SRendererTextureInfo
{
    uint64_t key = 0;
    int32_t width = 0;
    int32_t height = 0;
    uint64_t gpu_bytes = 0;
    uint32_t ref_count = 0;
};

struct SRendererAssetStats
{
    uint32_t image_pair_count = 0;
    uint32_t texture_count = 0;
    uint32_t mesh_count = 0;

    uint32_t image_pair_zero_ref_count = 0;
    uint32_t texture_zero_ref_count = 0;
    uint32_t mesh_zero_ref_count = 0;
    uint32_t keyed_mesh_count = 0;
    uint32_t unkeyed_mesh_count = 0;

    uint64_t image_pair_ref_total = 0;
    uint64_t texture_ref_total = 0;
    uint64_t mesh_ref_total = 0;

    uint64_t image_pair_gpu_bytes = 0;
    uint64_t texture_gpu_bytes = 0;
    uint64_t mesh_vertex_bytes = 0;
    uint64_t mesh_index_bytes = 0;
    uint64_t mesh_gpu_bytes = 0;

    uint32_t renderer_buffer_count = 0;
    uint32_t renderer_image_count = 0;
    int32_t buffer_pool_size = 0;
    int32_t image_pool_size = 0;
};

struct SRendererTilePassStats
{
    uint32_t tile_draws = 0;
    uint32_t tile_proxy_draws = 0;
    uint32_t tile_tight_proxy_draws = 0;
    uint32_t tile_tight_proxy_points = 0;
    uint32_t tile_rects_culled = 0;
    uint64_t tile_projected_pixels = 0;
    uint64_t tile_clipped_pixels = 0;
};

enum class ERendererTextureFormat : uint8_t
{
    RGBA8,
    R32F,
};

enum class ERendererTextureFilter : uint8_t
{
    Linear,
    Nearest,
};

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

// THudDrawable -- base class for everything that draws into the HUD layer.
//
// HUD widgets (cursor, status bars, dialog box, hover highlights, ...)
// subclass THudDrawable and override Draw(). Register with
// Renderer->AddHud(this, z); remove with Renderer->RemoveHud(this).
// The renderer owns the registration metadata (z, ordering) -- the
// drawable just knows how to draw itself.
//
// The renderer iterates registered drawables in z-order each frame
// and invokes Draw() inside the active swapchain pass; Draw() issues
// Renderer->DrawBitmap / DrawSurface / other renderer calls directly.
//
// Two common patterns:
//   - Cached panel: hold a TSurface, refresh it outside the draw path
//                   when its contents change, and just call
//                   Renderer->DrawSurface(...) from Draw().
//   - Live overlay: do per-frame work in Draw() and issue draw calls
//                   directly (e.g. cursor, debug arrows, floating text).
class THudDrawable
{
  public:
    virtual ~THudDrawable() = default;
    virtual void Draw() = 0;
};

class TRenderer
{
public:
    // Pad the G-buffer by this many pixels on every side so off-screen
    // geometry can still feed screen-space effects (sun-shadow ray-march).
    // This needs to cover the projected shadow reach, not merely a small
    // texture-filter neighborhood.
    // Map-side occluder/depth visibility tests must cull against the display
    // rect expanded by this amount, otherwise the padded border is allocated
    // but never populated.
    static constexpr int32_t kGBufPad = 512;

    // Max simultaneous point lights per deferred light pass. Extras are
    // silently dropped by AddPointLight.
    static constexpr int32_t kMaxPointLights = 16;

    TRenderer();
    ~TRenderer();

    // Bring the core renderer online. Allocates the G-buffer, AO, and
    // lit render targets at (width + 2*kGBufPad) x (height + 2*kGBufPad),
    // creates every pipeline, and sets up ImGui. Assumes sg_setup() has
    // already run (TDisplay::Init drives that).
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
    [[nodiscard]] SRendererTilePassStats GetLastTilePassStats() const
    {
        return last_tile_pass_stats;
    }

    // ---- Renderer-owned asset registry ---------------------------------
    // This is the GPU-realization cache. It owns sg_image/sg_buffer lifetimes;
    // map, sector, and draw systems keep opaque handles only. TAssetCache owns
    // source/CPU assets (I3D, bitmap, animation, sound, etc.) and passes
    // stable source keys here when those assets need renderer resources.
    //
    // Producers register source-keyed GPU assets. CPU/source asset wrappers
    // should balance renderer refs when they create GPU realizations; draw
    // records and visible instances must not own GPU residency. Refcount zero
    // is retained for now so we do not hitch during play; a future explicit
    // eviction path can free zero-ref assets here under renderer memory policy.
    RendererImagePairHandle RegisterImagePairAsset(uint64_t key,
                                                   const void* color_rgba8,
                                                   size_t color_bytes,
                                                   const void* depth_r32f,
                                                   size_t depth_bytes,
                                                   int32_t width,
                                                   int32_t height,
                                                   uint64_t gpu_bytes);
    const SRendererImagePairInfo* ImagePairInfo(RendererImagePairHandle handle) const;
    void AddImagePairAssetRef(RendererImagePairHandle handle, uint32_t count = 1);
    void ReleaseImagePairAssetRef(RendererImagePairHandle handle, uint32_t count = 1);

    TTextureHandle RegisterTextureAsset(uint64_t key,
                                        const void* pixels,
                                        size_t pixel_bytes,
                                        int32_t width,
                                        int32_t height,
                                        ERendererTextureFormat format,
                                        uint64_t gpu_bytes,
                                        ERendererTextureFilter filter = ERendererTextureFilter::Linear);
    const SRendererTextureInfo* TextureInfo(TTextureHandle handle) const;
    void AddTextureAssetRef(TTextureHandle handle, uint32_t count = 1);
    void ReleaseTextureAssetRef(TTextureHandle handle, uint32_t count = 1);
    void ResetAssetRefCounts();

    // Dynamic (streamable) RGBA8 texture for per-frame-updated content such as
    // video playback. Create once at a fixed size; call UpdateDynamicTexture
    // once per frame (outside any render pass) with a full width*height*4 RGBA
    // buffer, then present it with DrawTextureFit. DestroyDynamicTexture frees
    // the GPU image. Not key-cached or ref-counted -- the caller owns the
    // lifetime.
    TTextureHandle CreateDynamicTexture(int32_t width, int32_t height);
    void UpdateDynamicTexture(TTextureHandle handle, const void* rgba, size_t bytes);
    void DestroyDynamicTexture(TTextureHandle handle);

    // Aspect-preserving "contain" fit of a TTextureHandle into the swapchain.
    // Used by the cinematic player (--test=ui-cinematic) to present a video
    // frame regardless of source aspect.
    void DrawTextureFit(TTextureHandle texture);

    // Register a rigid mesh. TRenderer owns the resulting GPU buffers and
    // resolves albedo_texture internally. Returns 0 on failure.
    MeshHandle RegisterMeshAsset(uint64_t key,
                                 const SMeshVertex* verts, int32_t num_verts,
                                 const uint16_t*  indices, int32_t num_indices,
                                 TTextureHandle albedo_texture);
    MeshHandle RegisterMesh(const SMeshVertex* verts, int32_t num_verts,
                            const uint16_t*  indices, int32_t num_indices,
                            TTextureHandle albedo_texture)
    {
        return RegisterMeshAsset(0, verts, num_verts, indices, num_indices, albedo_texture);
    }
    void AddMeshAssetRef(MeshHandle mesh, uint32_t count = 1);
    void ReleaseMeshAssetRef(MeshHandle mesh, uint32_t count = 1);
    void SetMeshAlbedo(MeshHandle mesh, TTextureHandle albedo_texture);
    TTextureHandle WhiteTextureHandle();
    TTextureHandle SolidColorTexture(uint64_t key, uint32_t rgba, const char* debug_name = nullptr);
    [[nodiscard]] SRendererAssetStats GetAssetStats() const;
    [[nodiscard]] uintptr_t TextureImGuiId(TTextureHandle texture) const;

    // ---- Scene lighting state -------------------------------------------
    // Directional sun. Normals are reconstructed per-fragment from the tile
    // depth map in the light shader, so a near-horizontal direction gives
    // a flat ground about 0.5 and lets angled surfaces read brighter.
    void SetLight(float dx, float dy, float dz, float intensity,
                  float r, float g, float b, float ambient);
    // Ambient floor color, applied independently of the sun's tint.
    void SetAmbientColor(float r, float g, float b);
    // Ceiling on summed deferred light (ambient + per-point-lights),
    // applied per-channel before multiplying albedo. Default 1.5 lets
    // ambient push slightly overbright (helps the linear-space GPU
    // path match retail's non-linear palette boost) while still capping
    // stacked point-light blow-out. Range typical [0.5..4.0].
    void SetLightCeiling(float ceiling);
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
    //   8 = z edges      9 = ground/world height
    void SetTileViewMode(int32_t mode);
    // 0 = retail 1998 (ambient + distance-only point lights, no sun,
    //                  no shadows, no AO)
    // 1 = modern (adds directional sun + screen-space contact shadows)
    void SetLightingMode(int32_t mode);
    // Sun contact-shadow mask. The expensive receiver->sun ray march writes
    // a low-resolution R32F visibility buffer once; softness is then a cheap
    // separable blur of that buffer before deferred lighting samples it.
    // Debug controls expose step spacing, reach, blur radius, depth cutoff,
    // and bias. The old "samples" parameter is accepted for compatibility
    // but no longer multiplies ray work.
    void SetSunShadow(bool enable, float step_wu, float softness_px,
                      int32_t max_steps);
    void SetSunShadowRaycast(int32_t samples, float depth_cutoff_wu,
                             float bias_wu, float wz_scale);
    // Toggle just the enable flag (params untouched). Editor uses this
    // to suppress shadows while panels are open.
    void SetSunShadowEnabled(bool enable);
    [[nodiscard]] bool SunShadowEnabled() const;
    // World-space direction for the sun-shadow ray. This is the receiver ->
    // sun vector, and should normally be the same vector passed to SetLight.
    // The lighting pass marches along it and compares vertical world height,
    // so the visible cast direction is the consequence of the light vector,
    // not a second independently-authored shadow vector.
    void SetShadowWorldDir(float dx, float dy, float dz);
    // Legacy debug hook kept for old callers. XY drift is no longer part of
    // the shadow model; z scales the ray's vertical rise.
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
    void SetPerspectiveProjectionMode(int32_t mode);
    void SetPerspectiveProxyRasterScale(float scale);
    void SetPerspectiveDebugMode(int32_t mode) { perspective_debug_mode = mode; }

    // Run the deferred lighting pass over the current G-buffer. Writes
    // lit_target (passes [2] + [3]).
    void RunLightingPass();

    // ---- Backdrop pre-fill (optional, between EndTilePass + RunLightingPass)
    //
    // Pre-fills lit_target with a fullscreen color + optional image BEFORE
    // the deferred light shader runs over it. The light shader discards on
    // empty (alb.a < 0.01) pixels, so any pixel without scene contribution
    // keeps the backdrop; lit geometry composites over it; post-light fx
    // (fx_pass) then blends against the result. Pass kInvalidTexture for a
    // solid-color backdrop. With a texture: lit_target is cleared to
    // (r,g,b,a) for the letterbox bars, then the image is composited into
    // the visible rect preserving its aspect ratio.
    //
    // No-op if not called this frame -- RunLightingPass falls back to its
    // historical CLEAR-to-tile_clear_rgba behaviour for every other caller.
    void DrawBackdrop(TTextureHandle backdrop,
                      float r, float g, float b, float a);

    // ---- FX submission (Phase 1 VFX spine) ------------------------------
    // The fx pipelines need a camera basis (right/up) to expand
    // screen-aligned quads from world-space anchor points. Producers fill
    // SetFxCamera once per frame before submitting any fx item; the
    // shared fx_camera UBO is uploaded at DrainFxQueue time.
    //   right_wu / up_wu : world-space camera basis (unit length)
    //   forward_wu       : world-space camera forward (unit length)
    //   pos_wu           : world-space camera origin
    //   view_proj[16]    : row-major MVP that turns world xyz into NDC.
    //                      Phase 1 leaves this empty and uses the same
    //                      iso projection as the tile pipeline.
    void SetFxCamera(const float right_wu[3], const float up_wu[3],
                     const float forward_wu[3], const float pos_wu[3]);
    void SubmitFxBillboard(const SBillboardDrawItem& item);
    void SubmitFxParticle(const SParticleDrawItem& item);
    // Bulk submit every live particle in a TParticleBucket into one fx
    // bucket. The renderer reads draw_pos / draw_scl / draw_color /
    // draw_uv_rect off the bucket's SoA columns; bucket lifetime stays
    // with the caller. blend / texture come from the bucket desc.
    void SubmitFxParticleBucket(const TParticleBucket& bucket,
                                EFxDebugMode debug_mode = EFxDebugMode::Normal);
    void SubmitFxStrip(const SStripDrawItem& item);

    // ---- Composite (UI blit + swapchain present) ------------------------
    // Fullscreen blit of a TSurface into the current render pass (used by
    // TDisplay::FlipPage to paint the backbuffer onto the swapchain).
    void Composite(TSurface* src);
    // Sub-rect blit of a renderer texture into the current render pass.
    void Composite(TTextureHandle texture,
                   int32_t dst_x, int32_t dst_y, int32_t dst_w, int32_t dst_h,
                   int32_t target_w, int32_t target_h);
    // Atlas-friendly sub-rect blit: also takes a pixel-space source rect
    // within the texture.
    void Composite(TTextureHandle texture,
                   int32_t dst_x, int32_t dst_y, int32_t dst_w, int32_t dst_h,
                   int32_t target_w, int32_t target_h,
                   int32_t src_x, int32_t src_y, int32_t src_w, int32_t src_h,
                   int32_t src_tex_w, int32_t src_tex_h,
                   bool additive_blend = false,
                   bool chroma_key = false,
                   const float* chroma_key_rgb = nullptr);
    // Tinted render-target variant. Use while a TSurface::StartPass render
    // target is active; HUD swapchain drawing should use CompositeSwapchain*
    // or Draw* helpers below.
    void CompositeTinted(TTextureHandle texture,
                         int32_t dst_x, int32_t dst_y, int32_t dst_w, int32_t dst_h,
                         int32_t target_w, int32_t target_h,
                         int32_t src_x, int32_t src_y, int32_t src_w, int32_t src_h,
                         int32_t src_tex_w, int32_t src_tex_h,
                         float tr, float tg, float tb, float ta);

    // Swapchain-pass variant of the atlas sub-rect composite. Use this
    // from HUD Draw() callbacks (where the active pass is the swapchain,
    // not an offscreen render target) to blit glyphs from a font atlas
    // (SFontAtlas::texture) or any other TTextureHandle on top of the
    // 3D scene + backbuffer. The non-swap Composite() above targets the
    // render-target pipeline and trips sg_apply_pipeline validation when
    // called from a swapchain pass.
    void CompositeSwapchain(TTextureHandle texture,
                            int32_t dst_x, int32_t dst_y, int32_t dst_w, int32_t dst_h,
                            int32_t target_w, int32_t target_h,
                            int32_t src_x, int32_t src_y, int32_t src_w, int32_t src_h,
                            int32_t src_tex_w, int32_t src_tex_h);
    // Tinted swapchain-pass variant. Multiplies the texture sample by
    // (tr, tg, tb, ta) before output — lets HUD code colour or black-out
    // a TTF coverage atlas (white RGB, coverage in alpha), or apply a uniform tint
    // to a pre-coloured texture. Used by the bar-value / name-label text
    // path to render the retail white text plus its 3-pass black drop shadow
    // (FUN_004be2b0 lines 389-407).
    void CompositeSwapchainTinted(TTextureHandle texture,
                                  int32_t dst_x, int32_t dst_y, int32_t dst_w, int32_t dst_h,
                                  int32_t target_w, int32_t target_h,
                                  int32_t src_x, int32_t src_y, int32_t src_w, int32_t src_h,
                                  int32_t src_tex_w, int32_t src_tex_h,
                                  float tr, float tg, float tb, float ta);

    // Called from TDisplay::FlipPage inside sg_begin_default_pass. Picks
    // the best final image (lit_target > color_target > nothing) and
    // composites it onto the swapchain using the centered display-sized
    // sub-rect of the padded G-buffer. Returns true if it drew anything.
    bool PresentToSwapchain();

    // Same composite as PresentToSwapchain, but ignores the dirty flags so
    // the framesnap mirror pass can re-emit the last frame's final image
    // into its own offscreen RT *after* PresentToSwapchain already cleared
    // the dirty state for the real swapchain pass. The lit_target /
    // color_target textures still hold valid pixels at that point; the
    // dirty flags only signal "we've already presented this frame". No-op
    // if neither target has ever been written.
    bool PresentForSnap();

    // -------- HUD layer ----------------------------------------------------
    // HUD layer composites on top of the 3D scene during the swapchain
    // pass. Each HUD widget subclasses THudDrawable and implements
    // Draw(); the renderer iterates the registered drawables in z-order
    // (ascending; lower draws first, higher paints over) and calls
    // Draw() on each. Inside Draw(), the widget issues DrawBitmap /
    // DrawSurface calls -- it's running inside an active swapchain
    // pass. See docs/FRAME_PIPELINE.md.
    //
    // Ownership: the caller owns the drawable. AddHud just registers
    // the pointer; RemoveHud unregisters. The drawable's lifetime is
    // the caller's responsibility (typical pattern: unique_ptr member
    // on the owning object; add in ctor / OnEnter, remove in dtor /
    // OnExit).

    // PTBitmap -> cached GPU texture, then quad blit. The renderer
    // caches by bitmap identity so repeat calls cost an unordered_map
    // lookup. Caller never sees TTextureHandle for HUD purposes.
    void DrawBitmap (PTBitmap bm,    int32_t x, int32_t y, bool prefer_alias = false);
    // Subrect variant — blits the (src_x, src_y, src_w, src_h) region of
    // bm to (dst_x, dst_y). Used for sprite-atlas panels (e.g. the
    // TPlyrStatusBar `Bars` 128x128 atlas that holds 3 bar colours
    // stacked vertically).
    void DrawBitmapSubrect(PTBitmap bm,
                           int32_t dst_x, int32_t dst_y,
                           int32_t src_x, int32_t src_y,
                           int32_t src_w, int32_t src_h);

    // Color-tinted blit — multiplies texture sample by (tr, tg, tb, ta).
    // Used for dropshadow / glow / silhouette effects where a bitmap is
    // rendered in a uniform color while preserving its alpha/key shape.
    //   tint (0, 0, 0, 1)   = pure black silhouette (dropshadow)
    //   tint (0, 0, 0, 0.6) = semi-transparent shadow
    //   tint (1, 1, 1, 1)   = no change (use DrawBitmap instead)
    void DrawBitmapTinted(PTBitmap bm, int32_t x, int32_t y,
                          float tr, float tg, float tb, float ta);
    void DrawBitmapSubrectTinted(PTBitmap bm,
                                 int32_t dst_x, int32_t dst_y,
                                 int32_t src_x, int32_t src_y,
                                 int32_t src_w, int32_t src_h,
                                 float tr, float tg, float tb, float ta);
    // Render-target variants for cached HUD surfaces. These use the same
    // bitmap cache as the swapchain DrawBitmap helpers, but emit through the
    // RGBA8 render-target composite pipeline into the currently active
    // TSurface pass.
    void DrawBitmapToTarget(PTBitmap bm, int32_t x, int32_t y,
                            int32_t target_w, int32_t target_h);
    void DrawBitmapSubrectToTarget(PTBitmap bm,
                                   int32_t dst_x, int32_t dst_y,
                                   int32_t src_x, int32_t src_y,
                                   int32_t src_w, int32_t src_h,
                                   int32_t target_w, int32_t target_h);
    // Subrect → arbitrary dest-size variant — the bitmap's (sx,sy,sw,sh) region
    // is stretched to fill (dst_w,dst_h) at (dst_x,dst_y) in the target. This
    // is the to-target twin of the legacy retail `meth_0x4bd5e0` (the 7-arg
    // subrect→stretch surface blit, UI_METHOD_MAP §14 method-map gap). Used
    // for X-stretched chrome plates whose source bitmap width != target width
    // (e.g. TBottomBarPane UtilityBar 640px source stretched to display width).
    void DrawBitmapSubrectStretchedToTarget(PTBitmap bm,
                                            int32_t dst_x, int32_t dst_y,
                                            int32_t dst_w, int32_t dst_h,
                                            int32_t src_x, int32_t src_y,
                                            int32_t src_w, int32_t src_h,
                                            int32_t target_w, int32_t target_h);
    void DrawBitmapTintedToTarget(PTBitmap bm, int32_t x, int32_t y,
                                  int32_t target_w, int32_t target_h,
                                  float tr, float tg, float tb, float ta);
    void DrawBitmapSubrectTintedToTarget(PTBitmap bm,
                                         int32_t dst_x, int32_t dst_y,
                                         int32_t src_x, int32_t src_y,
                                         int32_t src_w, int32_t src_h,
                                         int32_t target_w, int32_t target_h,
                                         float tr, float tg, float tb, float ta);
    // Render-target dropshadow — the to-target twin of DrawBitmap{,Subrect}-
    // Shadowed below. Draws the darkened silhouette at (dst + off) then the
    // bitmap on top, into the active TSurface render-target pass. This is the
    // canonical "image with shadow" for the compose-to-target contract — UI
    // panels MUST use it rather than hand-rolling a tinted-black pass + a
    // bitmap pass. Per-side shadow direction (player +dx, target -dx) is the
    // caller's choice via off_x sign (NOMENCLATURE §4, project-retail-shadow-
    // semantics: the retail recon coord is where the SHADOW lands, so pass the
    // bitmap dst and let this offset the shadow).
    void DrawBitmapShadowedToTarget(PTBitmap bm, int32_t x, int32_t y,
                                    int32_t target_w, int32_t target_h,
                                    int32_t off_x = 4, int32_t off_y = 4,
                                    float shadow_a = 0.6f);
    void DrawBitmapSubrectShadowedToTarget(PTBitmap bm,
                                           int32_t dst_x, int32_t dst_y,
                                           int32_t src_x, int32_t src_y,
                                           int32_t src_w, int32_t src_h,
                                           int32_t target_w, int32_t target_h,
                                           int32_t off_x = 4, int32_t off_y = 4,
                                           float shadow_a = 0.6f);

    // Dropshadow convenience — draws a darkened silhouette of `bm` at
    // (x + off_x, y + off_y), then the normal bitmap on top. The shadow
    // alpha is `shadow_a` (default 0.6); shadow color defaults to black.
    // This is the modern equivalent of retail's blit-effect-pipeline
    // shadow pass (FUN_004aa280_BlitWithEffects + FUN_00438d80_Setup).
    // See memory project-retail-blit-effect-pipeline for the recon
    // analysis that informed this API.
    void DrawBitmapShadowed(PTBitmap bm, int32_t x, int32_t y,
                            int32_t off_x = 4, int32_t off_y = 4,
                            float shadow_a = 0.6f);
    void DrawBitmapSubrectShadowed(PTBitmap bm,
                                   int32_t dst_x, int32_t dst_y,
                                   int32_t src_x, int32_t src_y,
                                   int32_t src_w, int32_t src_h,
                                   int32_t off_x = 4, int32_t off_y = 4,
                                   float shadow_a = 0.6f);
    // TSurface -> quad blit, sized to the surface. Used for cached HUD
    // panels (char stats, game log, ...) that own their own surface
    // and refresh outside the draw path.
    void DrawSurface(TSurface* surf, int32_t x, int32_t y);
    void DrawSurfaceTinted(TSurface* surf, int32_t x, int32_t y,
                           float tr, float tg, float tb, float ta);

    // Solid-color filled rect (B-phase visual). Backed by a per-color 1x1
    // texture cache so the existing composite pipeline handles the blit
    // (no shader-side tint needed). Useful as a debug visualization for
    // panes that haven't ported their full DrawBackground yet, and as a
    // primitive any pane can use directly for flat-colored backgrounds /
    // separators / debug overlays.
    void DrawSolidRect(int32_t x, int32_t y, int32_t w, int32_t h,
                       uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

    // Compose-to-target variant of DrawSolidRect (emits into the active TSurface
    // render-target pass). `target_w/target_h` are the RT dims. Used by the DEF
    // widget engine for listbox selection fills.
    void DrawSolidRectToTarget(int32_t x, int32_t y, int32_t w, int32_t h,
                               int32_t target_w, int32_t target_h,
                               uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

    // 9-slice blit (A.2d). Treats `bm` as a stretchable panel: the four
    // corner pieces render verbatim at their source size; the four edges
    // stretch along their long axis; the center fills. `l/t/r/b` are inset
    // pixel counts into the source bitmap that mark the inner rect (the
    // edges/center region). `(dx, dy, dw, dh)` is the screen-space target
    // rect to draw into.
    //
    // If insets sum to more than the bitmap (e.g. l+r > bm->width) the
    // insets are clamped to bm->width/2 and bm->height/2 so the math
    // stays sane. Zero / degenerate dest pieces are skipped silently.
    //
    // Used by panes that want stretchable backgrounds (buttons, dialog
    // frames, panels). Source atoms typically come from retail widget
    // atlases (widgetstex.dat, medgold.dat, ...).
    void DrawNineSlice(PTBitmap bm,
                       int32_t l, int32_t t, int32_t r, int32_t b,
                       int32_t dx, int32_t dy, int32_t dw, int32_t dh);

    // Compose-to-target variant of DrawNineSlice: same 9-piece decomposition,
    // but emitted through the to-target Composite path into the active TSurface
    // render-target pass (the compose-to-target panel contract, NOMENCLATURE
    // §3) rather than straight to the swapchain. `target_w/target_h` are the RT
    // dimensions. Used by the DEF widget engine to stretch button / frame /
    // listbox chrome into a panel RT.
    void DrawNineSliceToTarget(PTBitmap bm,
                               int32_t l, int32_t t, int32_t r, int32_t b,
                               int32_t dx, int32_t dy, int32_t dw, int32_t dh,
                               int32_t target_w, int32_t target_h);

    void AddHud   (class THudDrawable* d, float z = 0.0f);
    void RemoveHud(class THudDrawable* d);
    // Re-register with a new z (cheaper than remove+add). No-op if d
    // wasn't registered.
    void SetHudZ  (class THudDrawable* d, float z);

    // Called by TDisplay::FlipPage between Scene3D present and ImGui.
    // Sorts registered drawables by z ascending (ties keep insertion
    // order) and invokes Draw() on each inside the active swapchain
    // pass.
    void DrawHud();

    // Where to composite the game image onto the swapchain. Default is
    // the entire window. Editor mode disables this (the game render
    // arrives in the editor's Game View panel via ImGui::Image of the
    // lit target texture id) so the present blit can be skipped.
    void SetPresentNDCRect(float x, float y, float w, float h);
    void ResetPresentNDCRect() { SetPresentNDCRect(-1.0f, -1.0f, 2.0f, 2.0f); }
    void SuppressPresent(bool on) { suppress_present = on; }

    // ---- Debug/editor accessors -----------------------------------------
    [[nodiscard]] uintptr_t LitTargetTextureId() const;
    [[nodiscard]] bool ReadIdTargetPixel(int32_t x, int32_t y, uint8_t out_rgba[4]) const;

    // Editor selection-outline plumbing. Setting this >0 turns on the
    // post-process outline glow in the lit shader for pixels whose
    // id_target value matches `id`.
    void SetSelectedObjectId(uint32_t id) { selected_obj_id = id; }
    [[nodiscard]] uint32_t SelectedObjectId() const { return selected_obj_id; }
    [[nodiscard]] int32_t  GBufPad()     const { return kGBufPad; }

private:
    // ---- HUD state (DrawBitmap/DrawSurface/AddHud/DrawHud) --------------
    // PTBitmap -> cached GPU texture. Key is the bitmap pointer; cache
    // entry holds the registered TTextureHandle so successive DrawBitmap
    // calls for the same bitmap are O(1).
    std::unordered_map<uintptr_t, TTextureHandle> bitmap_texture_cache;
    // Get-or-create a TTextureHandle for the given bitmap.
    TTextureHandle BitmapAsTexture(PTBitmap bm, bool prefer_alias = false);

    // Registered HUD drawables + their z-order. Renderer owns this
    // metadata; the drawable itself doesn't carry z. Sorted on demand
    // in DrawHud; insertion is O(1) push_back; removal / SetHudZ
    // search linearly (HUD count is small in practice).
    struct SHudRegistration
    {
        THudDrawable* drawable = nullptr;
        float         z        = 0.0f;
    };
    std::vector<SHudRegistration> hud_drawables;

    // Solid-color rect cache: one 1x1 sg_image per unique RGBA tuple.
    // Created on demand by DrawSolidRect, never freed until renderer
    // shutdown -- the per-color cost is 4 bytes of texture memory.
    std::unordered_map<uint32_t, sg_image> solid_color_cache;
    sg_image GetOrCreateSolidColorImage(uint32_t rgba);

    [[nodiscard]] sg_image TextureImage(TTextureHandle handle) const;
    [[nodiscard]] sg_image ImagePairColor(RendererImagePairHandle handle) const;
    [[nodiscard]] sg_image ImagePairDepth(RendererImagePairHandle handle) const;
    [[nodiscard]] sg_image ImagePairHeightMinMax(RendererImagePairHandle handle) const;
    void Composite(sg_image img,
                   int32_t dst_x, int32_t dst_y, int32_t dst_w, int32_t dst_h,
                   int32_t target_w, int32_t target_h);
    void Composite(sg_image img,
                   int32_t dst_x, int32_t dst_y, int32_t dst_w, int32_t dst_h,
                   int32_t target_w, int32_t target_h,
                   int32_t src_x, int32_t src_y, int32_t src_w, int32_t src_h,
                   int32_t src_tex_w, int32_t src_tex_h,
                   bool additive_blend = false,
                   bool chroma_key = false,
                   const float* chroma_key_rgb = nullptr);
    void CompositeTinted(sg_image img,
                         int32_t dst_x, int32_t dst_y, int32_t dst_w, int32_t dst_h,
                         int32_t target_w, int32_t target_h,
                         int32_t src_x, int32_t src_y, int32_t src_w, int32_t src_h,
                         int32_t src_tex_w, int32_t src_tex_h,
                         float tr, float tg, float tb, float ta);
    // Swapchain-target variant of the sub-rect composite. Same math
    // as the RT variant but uses composite_pip_swap so the pipeline's
    // color attachment pixel format matches the default pass. Used by
    // the HUD layer (DrawBitmap / DrawSurface from inside DrawHud).
    void CompositeSwapchain(sg_image img,
                            int32_t dst_x, int32_t dst_y, int32_t dst_w, int32_t dst_h,
                            int32_t target_w, int32_t target_h,
                            int32_t src_x, int32_t src_y, int32_t src_w, int32_t src_h,
                            int32_t src_tex_w, int32_t src_tex_h);
    // Tinted variant — multiplies texture sample by (tr, tg, tb, ta)
    // before output. Backs DrawBitmapTinted + the shadow draw helpers.
    void CompositeSwapchainTinted(sg_image img,
                                  int32_t dst_x, int32_t dst_y, int32_t dst_w, int32_t dst_h,
                                  int32_t target_w, int32_t target_h,
                                  int32_t src_x, int32_t src_y, int32_t src_w, int32_t src_h,
                                  int32_t src_tex_w, int32_t src_tex_h,
                                  float tr, float tg, float tb, float ta);

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
    sg_pass shadow_pass  = {};   // sun shadow -> shadow_target
    sg_pass shadow_blur_pass = {}; // horizontal blur -> shadow_blur_target
    sg_pass lit_pass     = {};   // Deferred lighting -> lit_target
    sg_pass depth_pass   = {};   // Reserved for a depth-only prepass

    // ---- Tile (G-buffer fill) pipeline ----------------------------------
    sg_shader   tile_shader   = {};
    sg_pipeline tile_pipeline = {};
    sg_buffer   tile_vbuf     = {};
    sg_buffer   tile_proxy_vbuf = {}; // stream convex proxies for projected relief tiles

    // ---- Screen-space AO pipeline ---------------------------------------
    sg_shader   ao_shader   = {};
    sg_pipeline ao_pipeline = {};

    // ---- Low-resolution sun shadow mask + blur pipelines ----------------
    sg_shader   shadow_shader      = {};
    sg_pipeline shadow_pipeline    = {};
    sg_shader   shadow_blur_shader = {};
    sg_pipeline shadow_blur_pipeline = {};

    // ---- Deferred light pipeline ----------------------------------------
    sg_shader   light_shader   = {};
    sg_pipeline light_pipeline = {};

    // Accumulated tile submissions for the current tile pass; drained in
    // EndTilePass. Capacity is preserved across frames to avoid churn.
    std::vector<STileSubmit> tile_queue;
    std::vector<STransparentWorldSubmit> transparent_world_queue;
    std::vector<SOverlaySubmit> overlay_queue;
    std::vector<SMeshSubmit> mesh_queue;
    SRendererTilePassStats current_tile_pass_stats = {};
    SRendererTilePassStats last_tile_pass_stats = {};

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
    std::vector<float> mesh_instance_scratch;
    static constexpr int32_t kMaxMeshInstances = 2048;

    struct SMeshEntry {
        sg_buffer vbuf;
        sg_buffer ibuf;
        int32_t   num_indices;
        TTextureHandle albedo = kInvalidTexture;
        uint32_t  ref_count = 0;
        uint64_t  key = 0;
        uint64_t  vertex_bytes = 0;
        uint64_t  index_bytes = 0;
    };
    std::vector<SMeshEntry> meshes;   // index+1 is MeshHandle
    std::unordered_map<uint64_t, MeshHandle> mesh_by_key;
    struct SRendererImagePairEntry : SRendererImagePairInfo {
        sg_image color = {};
        sg_image depth = {};
        sg_image height_minmax = {};
        // Source-space convex silhouette for perspective relief tiles. Two
        // floats per point: x, y in source pixels. Built once from the
        // immutable depth-valid mask; per-frame tile draws transform the
        // 3D outline slab into the stream proxy buffer.
        std::vector<float> tight_proxy_hull_px;
    };
    struct SRendererTextureEntry : SRendererTextureInfo {
        sg_image image = {};
    };
    std::vector<SRendererImagePairEntry> image_pair_assets; // index+1 handle
    std::unordered_map<uint64_t, RendererImagePairHandle> image_pair_by_key;
    std::vector<SRendererTextureEntry> texture_assets; // index+1 is TTextureHandle
    std::unordered_map<uint64_t, TTextureHandle> texture_by_key;
    TTextureHandle white_texture = kInvalidTexture;

    // Dirty flags -- read by PresentToSwapchain.
    bool color_target_dirty = false;
    bool lit_target_dirty   = false;
    // Sticky flag: set true once any Scene3D pass has written to a target
    // in any frame, so PresentForSnap won't re-emit uninitialized GPU
    // memory in HUD-only test modes (was tinting snap output magenta).
    bool any_target_ever_written = false;

    // Set by DrawBackdrop; consumed (and reset) by RunLightingPass to flip
    // its lit_target clear action from CLEAR -> LOAD so the backdrop pixels
    // survive under the light shader's discard.
    bool backdrop_filled = false;

    // NDC sub-rect for the present blit. Default fills the swapchain.
    float present_ndc[4] = { -1.0f, -1.0f, 2.0f, 2.0f };
    bool  suppress_present = false;
    float tile_clear_rgba[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    int32_t perspective_debug_mode = 0;
    int32_t perspective_projection_mode = 2; // 0 flat, 1 volume ref, 2 relief/SPOM.
    int32_t perspective_steps = 12;
    int32_t perspective_refine = 4;
    float perspective_proxy_raster_scale = 1.0f; // debug: scales proxy coverage only

    // ---- Render targets -------------------------------------------------
    sg_image color_target   = {};   // G-buffer: albedo   (RGBA8)
    sg_image depth_target   = {};   // G-buffer: HW depth (DEPTH)
    sg_image normal_target  = {};   // G-buffer: normal   (RGBA16F)
    sg_image scene_z_target = {};   // G-buffer: scene-z  (R32F)
    sg_image id_target      = {};   // G-buffer: obj id   (RGBA8 packed)
    uint32_t selected_obj_id = 0;   // editor selection (0 = no outline)
    sg_image ao_target      = {};   // AO pass output     (R32F)
    sg_image shadow_target  = {};   // sun visibility mask (R32F, low-res)
    sg_image shadow_blur_target = {}; // blur ping-pong target (R32F, low-res)
    sg_image lit_target     = {};   // Lit output         (RGBA8)
    sg_pass  helper_pass    = {};   // Forward helper/material pass into lit_target
    int32_t shadow_width = 0;
    int32_t shadow_height = 0;

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
        float   light_ceiling  = 1.5f;    // max summed light per channel; uploaded as ambient_col.w
        bool    sun_shadow_enable     = true;
        // Interactive default: one correct low-res ray plus a cheap separable
        // mask blur. Softness never multiplies the ray-march cost.
        float   sun_shadow_step_wu    = 32.0f;
        float   sun_shadow_softness_px = 3.0f;
        int32_t sun_shadow_max_steps  = 64;
        int32_t sun_shadow_samples    = 1; // compatibility only; ignored
        float   sun_shadow_depth_cutoff_wu = 16.0f;
        float   sun_shadow_bias_wu    = 2.0f;
        float   sun_shadow_wz_scale   = 1.0f;
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
    void InitShadowPipeline();
    void ShutdownShadowPipeline();
    void RunShadowPass();
    void RunShadowBlurPass(sg_pass pass, sg_image source, float dir_x, float dir_y);

    // Emit one queued tile as a single sg_draw (no instancing yet).
    bool ClipTileRasterRect(float& rect_x, float& rect_y,
                            float& rect_w, float& rect_h,
                            bool uv_independent_projection);
    void EmitTile(const STileSubmit& t);
    void DrainOverlayQueue();

    // Mesh pipeline lifecycle + per-frame drain.
    void InitMeshPipeline();
    void ShutdownMeshPipeline();
    void DrainMeshQueue();
    void EmitTransparentTile(const STileSubmit& t);
    void EmitTransparentHelper(const SHelperMeshSubmit& s);
    void DrainTransparentWorldQueue();

    // ---- FX (billboards / particles / strips) ---------------------------
    void InitFxPipeline();
    void ShutdownFxPipeline();
    void DrainFxQueue();   // runs inside fx_pass after RunLightingPass

    // Per-pipeline scratch instance data, SoA, materialized at drain time.
    struct SFxBucketScratch {
        SFxBatchKey         key = {};
        std::vector<float>  pos_xyz;       // 3 floats/instance
        std::vector<float>  size_wh;       // 2
        std::vector<float>  uv_xywh;       // 4
        std::vector<float>  color_rgba;    // 4
        std::vector<float>  rotation;      // particle only (1)
        std::vector<float>  debug_mode;    // 1 (float so we share the instance VB)
        int32_t             count = 0;
    };

    // Submission queue. Each variant entry is converted into a per-bucket
    // SFxBucketScratch in DrainFxQueue; the scratch then drives one
    // sg_draw per bucket against the shared instance vbuf for that
    // pipeline.
    struct SFxBillboardQueueEntry { SBillboardDrawItem item; float sort_z = 0.0f; };
    struct SFxParticleQueueEntry  { SParticleDrawItem  item; float sort_z = 0.0f; };
    struct SFxStripQueueEntry {
        SFxBatchKey                key;
        EFxDebugMode               debug_mode = EFxDebugMode::Normal;
        EFxLightMode               light_mode = EFxLightMode::Unlit;
        std::vector<SStripSegment> segments;
        float                      sort_z = 0.0f;
    };

    std::vector<SFxBillboardQueueEntry> fx_billboard_queue;
    std::vector<SFxParticleQueueEntry>  fx_particle_queue;
    std::vector<SFxStripQueueEntry>     fx_strip_queue;

    sg_pass     fx_pass         = {};   // color = lit_target, depth = scene_z_target (read)
    // Pipeline cross-product: [blend][depth_mode]. Light mode is
    // per-instance (uniform/attribute), not a pipeline variant -- keeps
    // the variant count manageable (4 blends * 3 depth modes = 12 per
    // shader, 36 total across billboard/particle/strip).
    static constexpr int32_t kFxBlendCount     = 4;   // matches EFxBlend
    static constexpr int32_t kFxDepthModeCount = 3;   // matches EFxDepthMode
    sg_shader   fx_billboard_shader = {};
    sg_pipeline fx_billboard_pip[kFxBlendCount][kFxDepthModeCount] = {};
    sg_shader   fx_particle_shader  = {};
    sg_pipeline fx_particle_pip[kFxBlendCount][kFxDepthModeCount] = {};
    sg_shader   fx_strip_shader     = {};
    sg_pipeline fx_strip_pip[kFxBlendCount][kFxDepthModeCount] = {};
    sg_buffer   fx_corner_vb     = {};   // static 4-vert quad corners
    sg_buffer   fx_billboard_ivb = {};   // dynamic per-frame, instance data
    sg_buffer   fx_particle_ivb  = {};
    sg_buffer   fx_strip_vb      = {};   // dynamic per-frame, expanded strip corners
    static constexpr int32_t kMaxFxInstances = 16384;
    static constexpr int32_t kMaxFxStripVerts = 8192;

    struct SFxCameraState {
        float right[3]   = {1.0f, 0.0f, 0.0f};
        float up[3]      = {0.0f, 1.0f, 0.0f};
        float forward[3] = {0.0f, 0.0f, 1.0f};
        float pos[3]     = {0.0f, 0.0f, 0.0f};
        bool  set        = false;
    } fx_camera;
};

// Global renderer instance -- created by TDisplay::Init, destroyed by
// TDisplay::Shutdown. All game-side renderer-specific calls (tile submission,
// lighting state, composites) go through this pointer. Non-renderer surface
// calls (Put, WriteText, Box, ZPut, ...) still go through Display..
extern TRenderer* Renderer;
