#pragma once

#include "maprenderer.h"

#include "object.h"
#include "revenant.h"
#include "sector.h"

#include <sokol_gfx.h>

#include <unordered_map>
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
    std::vector<float>   cpu_depth_local;
    std::vector<uint8_t> cpu_opaque;
    SDepthDump z_dump;
    const char* debug_classname = nullptr;
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
    std::vector<SSectorTileInst> sectorTileInst;
    std::unordered_map<int64_t, std::vector<int32_t>> sectorTileBins;
    std::vector<TSector*> sectorsKept;
    std::vector<SSectorLight> sectorLights;
    float sectorSceneZMin = 500.0f;
    float sectorSceneZMax = 5000.0f;
    int32_t sectorCenterOx = 0;
    int32_t sectorCenterOy = 0;
    S3DPoint sectorWorldCenter = {0,0,0};
    S3DPoint sectorCameraWorld = {0,0,0};
    bool sectorShowTileBboxes = false;
    bool sectorShowGizmos = true;
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
    float sun_shadow_step = 24.0f;
    float sun_shadow_soft = 3.0f;
    int32_t sun_shadow_max = 32;
    float debugSceneZMinFit = 0.0f;
    float debugSceneZMaxFit = 0.0f;
    int32_t debugFitTiles = 0;

    void rebuildBins()
    {
        sectorTileBins.clear();
        for (int32_t i = 0; i < int32_t(sectorTileInst.size()); ++i)
        {
            const auto& inst = sectorTileInst[i];
            const int32_t sx = MapRendererFloorDiv(inst.world_pos.x, SECTORWIDTH);
            const int32_t sy = MapRendererFloorDiv(inst.world_pos.y, SECTORHEIGHT);
            sectorTileBins[MapRendererSectorBinKey(sx, sy)].push_back(i);
        }
    }
    S3DPoint sectorCameraRel(const S3DPoint& world) const
    {
        return { world.x - sectorCameraWorld.x, world.y - sectorCameraWorld.y, world.z - sectorCameraWorld.z };
    }
    void sectorProjectWorld(const S3DPoint& world, S3DPoint& screen) const
    {
        const S3DPoint rel = sectorCameraRel(world);
        WorldToScreen(rel, screen.x, screen.y);
        screen.z = int32_t(MapRendererCameraDepth(rel));
    }
    float sectorCameraSceneZ(const SSectorTileInst& inst) const
    {
        return MapRendererCameraDepth(sectorCameraRel(inst.world_pos));
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
