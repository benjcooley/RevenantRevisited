#pragma once

#include "maprenderer.h"

#include "gamemap.h"
#include "object.h"
#include "renderer.h"
#include "revenant.h"
#include "sector.h"

#include <sokol_gfx.h>

#include <unordered_map>
#include <unordered_set>
#include <vector>

struct SSectorTileTex {
    struct SDepthDump {
        uint16_t u16_min = 0xFFFF;
        uint16_t u16_max = 0;
        int32_t  i16_min =  32767;
        int32_t  i16_max = -32768;
        int32_t  sample_count = 0;
        int32_t  high_bit_count = 0;
        int32_t  bbox_x0 =  0x7FFFFFFF;
        int32_t  bbox_y0 =  0x7FFFFFFF;
        int32_t  bbox_x1 = -0x7FFFFFFF;
        int32_t  bbox_y1 = -0x7FFFFFFF;
    };
    void*    bm_key = nullptr;
    sg_image color = {};
    sg_image depth = {};
    int32_t  w = 0, h = 0;
    float    z_local_min = 0.0f;
    float    z_local_max = 0.0f;
    bool     has_alpha = false;
    uint32_t bm_flags = 0;
    int32_t  opaque_count = 0;
    int32_t  pixel_count = 0;
    std::vector<float>   cpu_depth_local;
    std::vector<uint8_t> cpu_opaque;
    SDepthDump z_dump;
    const char* debug_classname = nullptr;
    const char* debug_typename = nullptr;
    S3DPoint    debug_world_pos = {0,0,0};
    int32_t     debug_regx = 0;
    int32_t     debug_regy = 0;
    int32_t     debug_regz = 0;
};

struct SSectorTileInst {
    int32_t  tex_idx = -1;
    S3DPoint world_pos = {0,0,0};
    int32_t  regx = 0, regy = 0, regz = 0;
    bool     has_authored_local_dz = false;
    float    authored_local_dz_min = 0.0f;
    float    authored_local_dz_max = 0.0f;
    int32_t  wwidth = 0, wlength = 0, wheight = 0;
    int32_t  wregx = 0, wregy = 0, wregz = 0;
    TSafeRef<> src;
};

struct SSectorLight {
    TSafeRef<> ref;
    bool       enabled = true;
};

enum class ESectorDrawableKind : uint8_t {
    Tile,
    Mesh,
    Billboard,
};

struct SSectorMeshAsset {
    TObjectImagery* imagery_key = nullptr;
    int32_t         objnum = -1;
    int32_t         texslot = -1;
    int32_t         uv_variant = 0;
    MeshHandle      handle = 0;
    bool            helper_material = false;
    bool            helper_shadow_plane = false;
    float           diffuse[4] = {1,1,1,1};
    float           ambient[4] = {1,1,1,1};
    float           specular[4] = {0,0,0,1};
    float           emissive[4] = {0,0,0,1};
    float           power = 1.0f;
    float           bbox_min[3] = { 0, 0, 0 };
    float           bbox_max[3] = { 0, 0, 0 };
};

struct SMapRenderContext;
struct SMapRenderStats;

struct SSectorDrawableInst {
    ESectorDrawableKind kind = ESectorDrawableKind::Tile;
    int32_t   asset_idx = -1;
    S3DPoint  world_pos = {0,0,0};
    int32_t   regx = 0, regy = 0, regz = 0;
    bool      has_authored_local_dz = false;
    float     authored_local_dz_min = 0.0f;
    float     authored_local_dz_max = 0.0f;
    int32_t   wwidth = 0, wlength = 0, wheight = 0;
    int32_t   wregx = 0, wregy = 0, wregz = 0;
    int32_t   state = 0;
    int32_t   frame = 0;
    int32_t   debug_sector_level = 0;
    int32_t   debug_sector_x = 0;
    int32_t   debug_sector_y = 0;
    int32_t   debug_sector_slot = -1;
    TSafeRef<> src;

    void UpdateFromInstance();
    void AccumulateSceneZ(const SMapRenderContext& ctx, float& scene_z_min_fit,
                          float& scene_z_max_fit, int32_t& fit_tiles) const;
    void Submit(const SMapRenderContext& ctx, SMapRenderStats& stats, uint32_t obj_id) const;
};

struct SMapRenderContext {
    const std::vector<SSectorTileTex>* tile_assets = nullptr;
    const std::vector<SSectorMeshAsset>* mesh_assets = nullptr;
    sg_image debug_green_img = {};
    S3DPoint sectorCameraWorld = {0,0,0};
    int32_t cam_ox = 0, cam_oy = 0;
    int32_t vw = 0, vh = 0;
    float cam_forward = 2750.0f;
    float camera_zoom = 1.0f;
    float tile_scale = 1.0f;
    float z_near = 0.0f, z_far = 1.0f, zspan = 1.0f;
    float depth_mul = 1.0f;
    bool perspective_camera = false;
    bool show_gizmos = true;
    bool show_tiles = true;
    bool show_meshes = true;
    bool force_mesh_preview_pose = false;
    float mesh_scale_x = 1.0f;
    float mesh_scale_y = 1.0f;
    float mesh_scale_z = 1.0f;
    int32_t cov_cell_px = 32;
    int32_t cov_cw = 0, cov_ch = 0;
    std::vector<uint8_t>* cov = nullptr;
    bool* logged_suppressed_legacy_flame_mesh = nullptr;
};

struct SMapRenderStats {
    int32_t draw_submitted = 0;
    int32_t draw_invalid_img = 0;
    int32_t draw_offscreen = 0;
    int32_t mesh_submitted = 0;
    int32_t mesh_skipped = 0;
    int32_t mesh_project_logged = 0;
    int32_t char_mesh_logged = 0;
    int32_t char_mesh_skip_logged = 0;
};

// Persisted across frames so the editor can read what was actually drawn
// last frame without re-running the render. Updated at the end of each
// RenderFrame (see maprenderer.cpp Submit / EndTilePass).
struct SMapDrawCounts {
    int32_t total_drawables  = 0;
    int32_t tiles_submitted  = 0;
    int32_t meshes_submitted = 0;
    int32_t offscreen_culled = 0;
};

struct FVec3 {
    float x, y, z;
};

constexpr float kMapRendererIsoCos30     = 867.0f / 1000.0f;
constexpr float kMapRendererCamForwardWU = 2750.0f;

inline float MapRendererCameraDepth(const S3DPoint& rel)
{
    return kMapRendererCamForwardWU
         - (float(rel.x + rel.y) * kMapRendererIsoCos30 + float(rel.z) * 0.5f);
}

// 3D-mesh object world positions are in the common world space
// (pos.z pre-scaled by WORLD3D_Z_SCALE at sector load), so this just
// passes world coordinates through. The scale_* args are kept for
// existing callers that historically applied per-axis scales but now
// default to identity -- the only previously meaningful scale (Z=1.5)
// has moved to the sector-load step + a mesh-local matrix scale.
inline S3DPoint MapRendererMeshWorld(const S3DPoint& world,
                                     float scale_x = 1.0f,
                                     float scale_y = 1.0f,
                                     float scale_z = 1.0f)
{
    return {
        int32_t(float(world.x) * scale_x),
        int32_t(float(world.y) * scale_y),
        int32_t(float(world.z) * scale_z)
    };
}

inline int32_t MapRendererFloorDiv(int32_t v, int32_t d)
{
    return (v >= 0) ? (v / d) : -(((-v) + d - 1) / d);
}

inline int64_t MapRendererSectorBinKey(int32_t sx, int32_t sy)
{
    return (int64_t(sx) << 32) ^ uint32_t(sy);
}

struct TMapRenderer::Impl
{
    std::vector<SSectorTileTex>  sectorTileTex;
    std::vector<SSectorMeshAsset> sectorMeshAsset;
    std::vector<SSectorDrawableInst> sectorDrawInst;
    std::unordered_map<int64_t, std::vector<int32_t>> sectorDrawBins;
    // Borrowed reference to the active TGameMap (owned by TMapManager).
    // Renderer iterates currentMap->Sectors() per frame; SafeRef keeps
    // it null after a map unload via the (id, gen) handle pattern.
    TSafeRef<TGameMap> currentMap;

    // Listener handle on currentMap.Get() (TGameMap::AddListener id).
    // SetMap registers; Unloaded handler / next SetMap unregisters.
    uint32_t mapListenerId = 0;

    // Camera anchor for RebuildForCurrentMap. Set from --level / --sector
    // at boot; used by the camera-anchor logic when rebuilding draw
    // caches. Defaults to "level origin" so post-boot level swaps with
    // no explicit anchor center on the new level's content.
    int32_t initialAnchorSx       = 0;
    int32_t initialAnchorSy       = 0;
    bool    useInitialLevelOrigin = true;
    std::vector<SSectorLight> sectorLights;
    int32_t dlightTexIdx = -1;
    sg_image debugGreenImage = {};
    float sectorSceneZMin = 500.0f;
    float sectorSceneZMax = 5000.0f;
    int32_t sectorCenterOx = 0;
    int32_t sectorCenterOy = 0;
    S3DPoint sectorWorldCenter = {0,0,0};
    S3DPoint sectorCameraWorld = {0,0,0};
    int32_t  cameraLevel = 0;

    // Last observed sum of TSector::ContentVer() across sectorsKept.
    // Compared at frame start; if the live sum has advanced (any
    // sector mutated its objects[] array via Add/Remove/Set), the
    // renderer rebuilds the per-instance drawable cache. Initial
    // -1 forces a build on first use after InitializeFromStartupArgs.
    int64_t lastSyncedSectorVerSum = -1;
    bool sectorShowTileBboxes = false;
    bool sectorShowTileLocators = false;
    bool sectorShowTileLabels = true;
    bool sectorShowObjectLocators = false;
    bool sectorShowObjectLabels = true;
    bool sectorShowGizmos = true;
    bool sectorShowTiles = true;
    bool sectorShowMeshes = true;
    bool sectorShowMeshLocators = false;
    bool sectorForceMeshPreviewPose = false;
    // Editor-driven selection set, stored as map indices so any
    // TSafeRef<T> (instance, light, sector entry, ...) can be marked
    // without committing to a single concrete type at this layer.
    std::unordered_set<int32_t> selectedMapIndices;
    bool sectorPerspectiveCamera = false;
    float sectorPerspectiveFovDeg = 6.0f;
    float sectorCameraZoom = 1.0f;
    float sectorPerspectiveZOffset = 0.0f;
    float sectorPerspectiveZScale = 1.0f;
    float sectorPerspectiveTileScale = 1.02f;
    int32_t sectorPerspectiveSteps = 32;
    int32_t sectorPerspectiveRefine = 5;
    int32_t sectorPerspectiveDebugMode = 0; // 0 normal, 1 checkerboard, 2 hit class
    // World-space mesh scale was historically Z=1.5 to push mesh
    // height into the tile coordinate space at draw time. That scale
    // has moved to TSector::Load (positions) + a mesh-local matrix
    // scale (mesh verts) so the renderer stops scaling per-frame.
    // Kept here at 1.0 for any caller that still references them.
    float sectorMeshScaleX = 1.0f;
    float sectorMeshScaleY = 1.0f;
    float sectorMeshScaleZ = 1.0f;
    int32_t sectorCharacterFocusIdx = -1;
    bool sectorDragging = false;
    int32_t dragStartX = 0, dragStartY = 0;
    S3DPoint dragCameraStartWorld = {0,0,0};
    int32_t lightDragIdx = -1;
    int32_t lightDragStartSX = 0, lightDragStartSY = 0;
    S3DPoint lightDragStartOiPos = {0,0,0};

    float dir[3]       = { 0.28f, -0.025f, 0.431f };
    float light_dir[3] = { 0.28f, -0.025f, 0.431f };
    float intensity    = 1.4f;
    float color[3]     = { 1.0f, 1.0f, 1.0f };
    float ambient_color[3] = { 1.0f, 1.0f, 1.0f };
    float ambient      = 0.15f;
    bool  ao_enable    = true;
    float ao_radius_px = 12.0f;
    float ao_strength  = 1.0f;
    float ao_bias      = 0.15f;
    float ao_max_dist  = 96.0f;
    float puck_u = 0.56f;
    float puck_v = 0.50f;
    float sdir_off_x = 0.0f;
    float sdir_off_y = 0.0f;
    float sdir_wz_mul = 1.0f;
    float depth_mul = 1.0f;
    float normal_hardness = 0.5f;
    float normal_radius = 1.5f;
    bool  show_light_vector = true;
    bool  show_mouse_normal = true;
    float debug_arrow_len_wu = 96.0f;
    float edge_thr = 64.0f;
    float z_near = -1024.0f;
    float z_far  = 16384.0f;
    bool  z_range_logged = false;
    bool  animate = false;
    int32_t tick = 0;
    int32_t view_mode = 0;
    bool lights_on = true;
    float radius_mul = 1.0f;
    float intensity_mul = 1.0f;
    int32_t lighting_mode = 1;
    bool  sun_shadow = true;
    SMapDrawCounts last_draw_counts;   // updated each RenderFrame, read by the editor
    float sun_shadow_step = 24.0f;
    float sun_shadow_soft = 3.0f;
    int32_t sun_shadow_max = 32;
    float debugSceneZMinFit = 0.0f;
    float debugSceneZMaxFit = 0.0f;
    int32_t debugFitTiles = 0;
    int64_t lastLegacyAnimTick = -1;

    void rebuildBins()
    {
        sectorDrawBins.clear();
        for (int32_t i = 0; i < int32_t(sectorDrawInst.size()); ++i)
        {
            const auto& inst = sectorDrawInst[i];
            const int32_t sx = MapRendererFloorDiv(inst.world_pos.x, SECTORWIDTH);
            const int32_t sy = MapRendererFloorDiv(inst.world_pos.y, SECTORHEIGHT);
            sectorDrawBins[MapRendererSectorBinKey(sx, sy)].push_back(i);
        }
    }
    S3DPoint sectorCameraRel(const S3DPoint& world) const
    {
        return { world.x - sectorCameraWorld.x, world.y - sectorCameraWorld.y, world.z - sectorCameraWorld.z };
    }
    S3DPoint sectorCameraRelMesh(const S3DPoint& world) const
    {
        const S3DPoint mesh_world = MapRendererMeshWorld(world, sectorMeshScaleX, sectorMeshScaleY, sectorMeshScaleZ);
        const S3DPoint mesh_camera = MapRendererMeshWorld(sectorCameraWorld, sectorMeshScaleX, sectorMeshScaleY, sectorMeshScaleZ);
        return { mesh_world.x - mesh_camera.x, mesh_world.y - mesh_camera.y, mesh_world.z - mesh_camera.z };
    }
    float sectorCameraForward(int32_t viewport_h) const;
    float sectorCameraDepth(const S3DPoint& rel, int32_t viewport_h) const
    {
        return sectorCameraForward(viewport_h)
             - (float(rel.x + rel.y) * kMapRendererIsoCos30 + float(rel.z) * 0.5f);
    }
    void sectorProjectWorld(const S3DPoint& world, S3DPoint& screen) const
    {
        const S3DPoint rel = sectorCameraRel(world);
        WorldToScreen(rel, screen.x, screen.y);
        screen.z = int32_t(sectorCameraDepth(rel, 0));
    }
    void sectorProjectWorldForViewport(const S3DPoint& world, int32_t viewport_h, S3DPoint& screen) const;
    void sectorProjectMeshWorld(const S3DPoint& world, S3DPoint& screen) const
    {
        const S3DPoint rel = sectorCameraRelMesh(world);
        WorldToScreen(rel, screen.x, screen.y);
        screen.z = int32_t(sectorCameraDepth(rel, 0));
    }
    float sectorCameraSceneZ(const SSectorDrawableInst& inst, int32_t viewport_h) const
    {
        return sectorCameraDepth(sectorCameraRel(inst.world_pos), viewport_h);
    }
    float sectorCameraSceneZMesh(const SSectorDrawableInst& inst, int32_t viewport_h) const
    {
        return sectorCameraDepth(sectorCameraRelMesh(inst.world_pos), viewport_h);
    }
    void sectorCameraOriginScreen(int32_t& sx, int32_t& sy) const
    {
        WorldToScreen(sectorWorldCenter, sx, sy);
        sx += sectorCenterOx;
        sy += sectorCenterOy;
    }
    S3DPoint sectorLightPos(const SSectorLight& L) const
    {
        TObjectInstance* oi = L.ref.Get();
        if (!oi) return {0,0,0};
        S3DPoint p = oi->Pos();
        if (PSLightDef ld = oi->GetLightDef()) {
            p.x += ld->pos.x; p.y += ld->pos.y; p.z += ld->pos.z;
        }
        return p;
    }
    float sectorLightRadius(const SSectorLight& L) const
    {
        TObjectInstance* oi = L.ref.Get();
        PSLightDef ld = oi ? oi->GetLightDef() : nullptr;
        return ld ? float(ld->intensity) : 0.0f;
    }
    void sectorLightColor(const SSectorLight& L, float rgb[3]) const
    {
        TObjectInstance* oi = L.ref.Get();
        PSLightDef ld = oi ? oi->GetLightDef() : nullptr;
        if (ld) {
            rgb[0] = ld->color.red / 255.0f;
            rgb[1] = ld->color.green / 255.0f;
            rgb[2] = ld->color.blue / 255.0f;
        } else {
            rgb[0] = rgb[1] = rgb[2] = 0.0f;
        }
    }
    float sectorLightIntensity(const SSectorLight& L) const
    {
        TObjectInstance* oi = L.ref.Get();
        PSLightDef ld = oi ? oi->GetLightDef() : nullptr;
        return (!ld || ld->multiplier <= 0) ? 0.0f : float(ld->multiplier) / 10.0f;
    }
    const char* sectorLightClassName(const SSectorLight& L) const
    {
        TObjectInstance* oi = L.ref.Get();
        return oi ? oi->GetClassName() : nullptr;
    }
};
