// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *     tile.hlsl.h  - HLSL tile G-buffer fill (SM5 / D3D11 variant)      *
// *************************************************************************
//
// Windows / SOKOL_D3D11 port of tile.metal.h.
//
// *************************************************************************

#pragma once

inline constexpr const char* kTileVsHlsl = R"HLSL(
cbuffer params : register(b0) {
    float4 rect;
    float4 zparams;
    float4 tile_root;
    float4 tile_sprite;
    float4 filter_;
};
struct vs_in  { float2 pos : POSITION; float2 uv : TEXCOORD0; };
struct vs_out { float4 pos : SV_Position; float2 uv : TEXCOORD0; };
vs_out main_vs(vs_in i) {
    vs_out o;
    o.pos = float4(rect.xy + i.pos * rect.zw, 0.0, 1.0);
    o.uv  = i.uv;
    return o;
}
)HLSL";

inline constexpr const char* kTileFsHlsl = R"HLSL(
#define ISO_COS30 0.867
#define ISO_WZ_DENOM 2.003378
#define EDGE_EPS 1e-6
cbuffer params : register(b0) {
    float4 rect;
    float4 zparams;
    float4 tile_root;
    float4 tile_sprite;
    float4 filter_;
};
Texture2D    color_tex : register(t0);
Texture2D    depth_tex : register(t1);
SamplerState smp       : register(s0);
struct vs_out { float4 pos : SV_Position; float2 uv : TEXCOORD0; };
struct fs_out {
    float4 albedo  : SV_Target0;
    float4 normal  : SV_Target1;
    float4 scene_z : SV_Target2;
    float  depth   : SV_Depth;
};
fs_out main_ps(vs_out in_) {
    fs_out o;
    float4 c = color_tex.Sample(smp, in_.uv);
    if (c.a < 0.01) discard;
    float zraw = depth_tex.Sample(smp, in_.uv).r;
    float d    = zparams.x + zraw * zparams.y;
    if (d < 0.0 || d > 1.0) discard;
    float nr  = max(filter_.x, 0.5);
    float thr = max(filter_.y, 0.0);
    float tw, th; depth_tex.GetDimensions(tw, th);
    float2 tex = float2(nr / max(tw, 1.0), nr / max(th, 1.0));
    float sx = in_.uv.x * tile_sprite.z - tile_sprite.x;
    float sy = in_.uv.y * tile_sprite.w - tile_sprite.y;
    float zl  = depth_tex.Sample(smp, in_.uv - float2(tex.x, 0.0)).r;
    float zrs = depth_tex.Sample(smp, in_.uv + float2(tex.x, 0.0)).r;
    float zt  = depth_tex.Sample(smp, in_.uv - float2(0.0, tex.y)).r;
    float zb  = depth_tex.Sample(smp, in_.uv + float2(0.0, tex.y)).r;
    if (abs(zl  - zraw) > thr + EDGE_EPS) zl  = zraw;
    if (abs(zrs - zraw) > thr + EDGE_EPS) zrs = zraw;
    if (abs(zt  - zraw) > thr + EDGE_EPS) zt  = zraw;
    if (abs(zb  - zraw) > thr + EDGE_EPS) zb  = zraw;
    float dzl = zl  * tile_root.w;
    float dzr = zrs * tile_root.w;
    float dzt = zt  * tile_root.w;
    float dzb = zb  * tile_root.w;
    float nrx = nr;
    float nry = nr;
    float lz_l = (-dzl - 2.0 * sy * ISO_COS30) / ISO_WZ_DENOM;
    float su_l = 2.0 * (sy + lz_l * ISO_COS30);
    float3 Wl = float3(tile_root.x + 0.5 * (su_l + (sx - nrx)),
                       tile_root.y + 0.5 * (su_l - (sx - nrx)),
                       tile_root.z + lz_l);
    float lz_r = (-dzr - 2.0 * sy * ISO_COS30) / ISO_WZ_DENOM;
    float su_r = 2.0 * (sy + lz_r * ISO_COS30);
    float3 Wr = float3(tile_root.x + 0.5 * (su_r + (sx + nrx)),
                       tile_root.y + 0.5 * (su_r - (sx + nrx)),
                       tile_root.z + lz_r);
    float lz_t = (-dzt - 2.0 * (sy - nry) * ISO_COS30) / ISO_WZ_DENOM;
    float su_t = 2.0 * ((sy - nry) + lz_t * ISO_COS30);
    float3 Wt = float3(tile_root.x + 0.5 * (su_t + sx),
                       tile_root.y + 0.5 * (su_t - sx),
                       tile_root.z + lz_t);
    float lz_b = (-dzb - 2.0 * (sy + nry) * ISO_COS30) / ISO_WZ_DENOM;
    float su_b = 2.0 * ((sy + nry) + lz_b * ISO_COS30);
    float3 Wb = float3(tile_root.x + 0.5 * (su_b + sx),
                       tile_root.y + 0.5 * (su_b - sx),
                       tile_root.z + lz_b);
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
)HLSL";
