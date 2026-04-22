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
                float4 filter; };
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
//   zparams      .x = anchor_z, .y = depth_mul, .z unused, .w unused
//   tile_root    .xyz = anchor pixel world xyz, .w = zraw_to_wu scale
//   tile_sprite  .xy = anchor pixel (regx,regy), .zw = sprite size
//   filter       .x = normal_radius (texels), .y = bilateral edge threshold
inline constexpr const char* kTileFsMetal = R"MSL(
#include <metal_stdlib>
using namespace metal;
#define ISO_COS30 0.867
#define ISO_WZ_DENOM 2.003378
#define EDGE_EPS 1e-6
struct params { float4 rect; float4 zparams; float4 tile_root; float4 tile_sprite;
                float4 filter; };
struct vs_out { float4 pos [[position]]; float2 uv; };
struct fs_out { float4 albedo   [[color(0)]];
                float4 normal   [[color(1)]];
                float4 scene_z  [[color(2)]];
                float  depth    [[depth(any)]]; };
fragment fs_out _main(vs_out in [[stage_in]],
                      texture2d<float> color_tex [[texture(0)]],
                      texture2d<float> depth_tex [[texture(1)]],
                      sampler smp              [[sampler(0)]],
                      constant params& p       [[buffer(0)]]) {
    fs_out o;
    float4 c = color_tex.sample(smp, in.uv);
    if (c.a < 0.01) discard_fragment();
    float zraw = depth_tex.sample(smp, in.uv).r;
    float d    = p.zparams.x + zraw * p.zparams.y;
    if (d < 0.0 || d > 1.0) discard_fragment();
    float  nr  = max(p.filter.x, 0.5);
    float  thr = max(p.filter.y, 0.0);
    float  tw  = float(depth_tex.get_width());
    float  th  = float(depth_tex.get_height());
    float2 tex = float2(nr / max(tw, 1.0), nr / max(th, 1.0));
    float  sx  = in.uv.x * p.tile_sprite.z - p.tile_sprite.x;
    float  sy  = in.uv.y * p.tile_sprite.w - p.tile_sprite.y;
    float  zl  = depth_tex.sample(smp, in.uv - float2(tex.x, 0.0)).r;
    float  zrs = depth_tex.sample(smp, in.uv + float2(tex.x, 0.0)).r;
    float  zt  = depth_tex.sample(smp, in.uv - float2(0.0, tex.y)).r;
    float  zb  = depth_tex.sample(smp, in.uv + float2(0.0, tex.y)).r;
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
    o.depth   = d;
    return o;
}
)MSL";
