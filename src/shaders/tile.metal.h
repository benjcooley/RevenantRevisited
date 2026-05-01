// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *           tile.metal.h  - MSL tile G-buffer fill shaders              *
// *************************************************************************
//
// Deferred shading G-buffer fill. Each tile sprite writes:
//   color(0)  albedo  (RGBA8)
//   color(1)  world normal, packed x*0.5+0.5 (RGBA16F)
//   color(2)  scene_z  (R32F, maps tile-local z to camera-depth [0..1])
//   depth     same scene_z, for hardware depth test
//
// Normals are reconstructed per-fragment from the tile depth map via a
// bilateral central-difference stencil. The iso inverse below matches the
// retail iso projection (cos30 = 867/1000). See docs/DEFERRED_LIGHTING.md
// for the full derivation.
//
// *************************************************************************

#pragma once

inline constexpr const char* kTileVsMetal = R"MSL(
#include <metal_stdlib>
using namespace metal;
struct params { float4 rect; float4 zparams; float4 tile_root; float4 tile_sprite;
                float4 filter; float4 obj_id; float4 camera; float4 proj; float4 tile_rect;
                float4 cam_tile; float4 debug; float4 raycast; };
struct vs_in  { float2 pos [[attribute(0)]]; float2 uv [[attribute(1)]]; };
struct vs_out { float4 pos [[position]]; float2 uv; };
vertex vs_out _main(vs_in in [[stage_in]], constant params& p [[buffer(0)]]) {
    vs_out o;
    o.pos = float4(p.rect.xy + in.pos * p.rect.zw, 0.0, 1.0);
    o.uv  = in.uv;
    return o;
}
)MSL";

// G-buffer fill:
//   zparams      .x = anchor_z, .y = depth_mul, .z = tile coverage scale, .w = zoom
//   tile_root    .xyz = anchor pixel world xyz, .w = zraw_to_wu scale
//   tile_sprite  .xy = anchor pixel (regx,regy), .zw = sprite size
//   filter       .x = normal_radius (texels), .y = bilateral edge threshold
//                .z/.w = tile local zraw min/max, relative to the anchor
//   cam_tile     .xy = tile anchor in old camera plane, .z = anchor camera depth,
//                .w = zraw_to_camera_depth scale
inline constexpr const char* kTileFsMetal = R"MSL(
#include <metal_stdlib>
using namespace metal;
#define ISO_COS30 0.867
#define ISO_WZ_DENOM 2.003378
#define EDGE_EPS 1e-6
struct params { float4 rect; float4 zparams; float4 tile_root; float4 tile_sprite;
                float4 filter; float4 obj_id; float4 camera; float4 proj; float4 tile_rect;
                float4 cam_tile; float4 debug; float4 raycast; };
struct vs_out { float4 pos [[position]]; float2 uv; };
struct fs_out { float4 albedo   [[color(0)]];
                float4 normal   [[color(1)]];
                float4 scene_z  [[color(2)]];
                float4 obj_id   [[color(3)]];
                float  depth    [[depth(any)]]; };
static bool tile_depth_valid(float2 uv, texture2d<float> depth_tex) {
    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0)
        return false;
    uint w = depth_tex.get_width();
    uint h = depth_tex.get_height();
    uint2 p = uint2(uint(clamp(uv.x * float(w), 0.0, float(w - 1))),
                    uint(clamp(uv.y * float(h), 0.0, float(h - 1))));
    return fabs(depth_tex.read(p).r) > 0.0001;
}
static float tile_depth_read(float2 uv, texture2d<float> depth_tex, sampler smp) {
    uint w = depth_tex.get_width();
    uint h = depth_tex.get_height();
    uint2 p = uint2(uint(clamp(uv.x * float(w), 0.0, float(w - 1))),
                    uint(clamp(uv.y * float(h), 0.0, float(h - 1))));
    return depth_tex.read(p).r;
}
fragment fs_out _main(vs_out in [[stage_in]],
                      texture2d<float> color_tex [[texture(0)]],
                      texture2d<float> depth_tex [[texture(1)]],
                      sampler smp              [[sampler(0)]],
                      constant params& p       [[buffer(0)]]) {
    fs_out o;
    float2 hit_uv = in.uv;
    bool hit_valid_depth = p.camera.w <= 0.5;
    if (p.camera.w > 0.5) {
        const float2 frag_px = in.pos.xy;
        const float zoom = max(p.zparams.w, 0.0001);
        const float2 ray_cam_xy = (frag_px - p.proj.xy) / max(p.camera.z * zoom, 1.0);
        float z0 = p.cam_tile.z + min(p.filter.z, p.filter.w) * p.cam_tile.w;
        float z1 = p.cam_tile.z + max(p.filter.z, p.filter.w) * p.cam_tile.w;
        if (z1 < z0) { float tmp = z0; z0 = z1; z1 = tmp; }
        z0 = max(z0, 1.0);
        z1 = max(z1, z0 + 1.0);

        bool found = false;
        float best_z = z0;
        float2 best_uv = float2(0.0);
        float prev_z = z0;
        float prev_diff = 0.0;
        float2 prev_uv = float2(0.0);
        bool prev_valid = false;
        int valid_run = 0;
        const float tile_scale = max(p.zparams.z, 0.0001);

        int volume_steps = clamp(int(p.raycast.x), 4, 128);
        int refine_steps = clamp(int(p.raycast.y), 0, 8);
        for (int i = 0; i < 128; ++i) {
            if (i >= volume_steps) break;
            float cam_z = mix(z0, z1, float(i) / float(volume_steps - 1));
            float2 cam_xy = ray_cam_xy * cam_z;
            float sx = (cam_xy.x - p.cam_tile.x) / tile_scale;
            float sy = (cam_xy.y - p.cam_tile.y) / tile_scale;
            float2 uv = float2((sx + p.tile_sprite.x) / max(p.tile_sprite.z, 1.0),
                               (sy + p.tile_sprite.y) / max(p.tile_sprite.w, 1.0));
            if (!tile_depth_valid(uv, depth_tex)) {
                prev_valid = false;
                valid_run = 0;
                continue;
            }
            float zraw_surf = tile_depth_read(uv, depth_tex, smp);
            float surf_z = p.cam_tile.z + zraw_surf * p.cam_tile.w;
            float diff = surf_z - cam_z;
            if (prev_valid && valid_run >= 2 && prev_diff >= 0.0 && diff <= 0.0) {
                float lo_z = prev_z;
                float hi_z = cam_z;
                float2 lo_uv = prev_uv;
                float2 hi_uv = uv;
                bool refine_ok = true;
                // Refine only within the already valid front-surface bracket.
                // The alpha boundary is not geometry; invalid samples never
                // create a bracket, so tile sides stay non-renderable.
                for (int r = 0; r < 8; ++r) {
                    if (r >= refine_steps) break;
                    float mid_z = 0.5 * (lo_z + hi_z);
                    float2 mid_xy = ray_cam_xy * mid_z;
                    float mid_sx = (mid_xy.x - p.cam_tile.x) / tile_scale;
                    float mid_sy = (mid_xy.y - p.cam_tile.y) / tile_scale;
                    float2 mid_uv = float2((mid_sx + p.tile_sprite.x) / max(p.tile_sprite.z, 1.0),
                                           (mid_sy + p.tile_sprite.y) / max(p.tile_sprite.w, 1.0));
                    if (!tile_depth_valid(mid_uv, depth_tex)) {
                        // Leaving valid depth means this bracket is crossing
                        // the tile silhouette, not the front surface.
                        refine_ok = false;
                        break;
                    }
                    float mid_raw = tile_depth_read(mid_uv, depth_tex, smp);
                    float mid_surface_z = p.cam_tile.z + mid_raw * p.cam_tile.w;
                    float mid_diff = mid_surface_z - mid_z;
                    if (mid_diff >= 0.0) {
                        lo_z = mid_z;
                        lo_uv = mid_uv;
                    } else {
                        hi_z = mid_z;
                        hi_uv = mid_uv;
                    }
                }
                if (refine_ok) {
                    best_z = 0.5 * (lo_z + hi_z);
                    best_uv = 0.5 * (lo_uv + hi_uv);
                    found = true;
                    break;
                }
            }
            prev_valid = true;
            ++valid_run;
            prev_z = cam_z;
            prev_diff = diff;
            prev_uv = uv;
        }
        (void)prev_uv;
        if (!found || best_uv.x < 0.0 || best_uv.x > 1.0 ||
            best_uv.y < 0.0 || best_uv.y > 1.0)
            discard_fragment();
        hit_uv = best_uv;
        hit_valid_depth = tile_depth_valid(hit_uv, depth_tex);
        if (!hit_valid_depth) discard_fragment();
    }

    float4 c = color_tex.sample(smp, hit_uv);
    if (p.camera.w <= 0.5 && c.a < 0.01) discard_fragment();
    if (p.camera.w > 0.5) c.a = 1.0;
    if (p.debug.x > 1.5) {
        c.rgb = hit_valid_depth ? float3(0.1, 0.2, 1.0) : float3(0.0, 1.0, 0.1);
        c.a = 1.0;
    } else if (p.debug.x > 0.5) {
        c.rgb = p.debug.yzw;
        c.a = 1.0;
    }
    float zraw = (p.camera.w > 0.5) ? tile_depth_read(hit_uv, depth_tex, smp)
                                    : depth_tex.sample(smp, hit_uv).r;
    float d    = p.zparams.x + zraw * p.zparams.y;
    if (d < 0.0 || d > 1.0) discard_fragment();
    float  nr  = max(p.filter.x, 0.5);
    float  thr = max(p.filter.y, 0.0);
    float  tw  = float(depth_tex.get_width());
    float  th  = float(depth_tex.get_height());
    float2 tex = float2(nr / max(tw, 1.0), nr / max(th, 1.0));
    float  sx  = hit_uv.x * p.tile_sprite.z - p.tile_sprite.x;
    float  sy  = hit_uv.y * p.tile_sprite.w - p.tile_sprite.y;
    float  zl  = depth_tex.sample(smp, hit_uv - float2(tex.x, 0.0)).r;
    float  zrs = depth_tex.sample(smp, hit_uv + float2(tex.x, 0.0)).r;
    float  zt  = depth_tex.sample(smp, hit_uv - float2(0.0, tex.y)).r;
    float  zb  = depth_tex.sample(smp, hit_uv + float2(0.0, tex.y)).r;
    if (fabs(zl  - zraw) > thr + EDGE_EPS) zl  = zraw;
    if (fabs(zrs - zraw) > thr + EDGE_EPS) zrs = zraw;
    if (fabs(zt  - zraw) > thr + EDGE_EPS) zt  = zraw;
    if (fabs(zb  - zraw) > thr + EDGE_EPS) zb  = zraw;
    float  dzl = zl  * p.tile_root.w;
    float  dzr = zrs * p.tile_root.w;
    float  dzt = zt  * p.tile_root.w;
    float  dzb = zb  * p.tile_root.w;
    float  nrx = nr;
    float  nry = nr;
    float  lz_l = (-dzl - 2.0 * sy * ISO_COS30) / ISO_WZ_DENOM;
    float  su_l = 2.0 * (sy + lz_l * ISO_COS30);
    float3 Wl = float3(p.tile_root.x + 0.5 * (su_l + (sx - nrx)),
                      p.tile_root.y + 0.5 * (su_l - (sx - nrx)),
                      p.tile_root.z + lz_l);
    float  lz_r = (-dzr - 2.0 * sy * ISO_COS30) / ISO_WZ_DENOM;
    float  su_r = 2.0 * (sy + lz_r * ISO_COS30);
    float3 Wr = float3(p.tile_root.x + 0.5 * (su_r + (sx + nrx)),
                      p.tile_root.y + 0.5 * (su_r - (sx + nrx)),
                      p.tile_root.z + lz_r);
    float  lz_t = (-dzt - 2.0 * (sy - nry) * ISO_COS30) / ISO_WZ_DENOM;
    float  su_t = 2.0 * ((sy - nry) + lz_t * ISO_COS30);
    float3 Wt = float3(p.tile_root.x + 0.5 * (su_t + sx),
                      p.tile_root.y + 0.5 * (su_t - sx),
                      p.tile_root.z + lz_t);
    float  lz_b = (-dzb - 2.0 * (sy + nry) * ISO_COS30) / ISO_WZ_DENOM;
    float  su_b = 2.0 * ((sy + nry) + lz_b * ISO_COS30);
    float3 Wb = float3(p.tile_root.x + 0.5 * (su_b + sx),
                      p.tile_root.y + 0.5 * (su_b - sx),
                      p.tile_root.z + lz_b);
    float3 t_u = Wr - Wl;
    float3 t_v = Wb - Wt;
    float3 N   = normalize(cross(t_v, t_u));
    float3 Vc  = normalize(float3(ISO_COS30, ISO_COS30, 1.0));
    if (dot(N, Vc) < 0.0) N = -N;
    o.albedo  = c;
    o.normal  = float4(N * 0.5 + 0.5, 1.0);
    o.scene_z = float4(d, 0.0, 0.0, 1.0);
    o.obj_id  = p.obj_id;
    o.depth   = d;
    return o;
}
)MSL";
