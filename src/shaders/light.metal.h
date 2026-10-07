// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *        light.metal.h  - MSL deferred lighting fullscreen pass         *
// *************************************************************************
//
// Reads the G-buffer (albedo + world normal + scene_z + AO) and lights each
// sample with shade_surface() from lightmodel.metal.h, which TRenderer
// prepends to kLightFsMetal (the lighting models are described there). The
// G-buffer normal's alpha carries the surface class the Classic model keys
// on: 1 tile, 0 mesh.
//
// Fragment world position is reconstructed from scene_z via the retail iso
// inverse (same math as ao.metal.h and tile.metal.h). See
// docs/DEFERRED_LIGHTING.md for the derivation.
//
// *************************************************************************

#pragma once

inline constexpr const char* kLightVsMetal = R"MSL(
#include <metal_stdlib>
using namespace metal;
struct vs_in  { float2 pos [[attribute(0)]]; float2 uv [[attribute(1)]]; };
struct vs_out { float4 pos [[position]];     float2 uv; };
vertex vs_out _main(vs_in in [[stage_in]]) {
    vs_out o;
    o.pos = float4(in.pos * 2.0 - 1.0, 0.0, 1.0);
    o.uv  = in.uv;
    return o;
}
)MSL";

inline constexpr const char* kLightFsMetal = R"MSL(
struct vs_out { float4 pos [[position]]; float2 uv; };
static float3 reconstruct_world(float2 uv, float d, constant params& p, float fbw, float fbh) {
    float scene_z = d * p.vp.w + p.vp.z;
    // Convert framebuffer pixels back to Revenant's authored iso screen
    // units before applying the inverse. This is required even in ortho:
    // high-resolution render targets scale the sprite pass, but they do not
    // change the world camera.
    float zoom = max(p.settings.w, 0.0001);
    float S = (uv.x * fbw - p.vp.x) / zoom;
    float T = (uv.y * fbh - p.vp.y) / zoom;
    if (p.recon.w > 0.5) {
        float focal = max(p.recon.z, 1.0);
        S = (S / focal) * scene_z;
        T = (T / focal) * scene_z;
    }
    float K = p.recon.z - scene_z;
    float wz = (K - 2.0 * T * ISO_COS30) / ISO_WZ_DENOM;
    float sum_r = 2.0 * (T + wz * ISO_COS30);
    return float3(p.recon.x + (sum_r + S) * 0.5,
                  p.recon.y + (sum_r - S) * 0.5,
                  wz);
}
static float sample_scene_depth(texture2d<float> depth_tex, float2 uv) {
    uint w = depth_tex.get_width();
    uint h = depth_tex.get_height();
    uint2 p = uint2(uint(clamp(uv.x * float(w), 0.0, float(w - 1))),
                    uint(clamp(uv.y * float(h), 0.0, float(h - 1))));
    return depth_tex.read(p).r;
}
fragment float4 _main(vs_out in [[stage_in]],
                      texture2d<float> albedo_tex [[texture(0)]],
                      texture2d<float> normal_tex [[texture(1)]],
                      texture2d<float> depth_tex  [[texture(2)]],
                      texture2d<float> ao_tex     [[texture(3)]],
                      texture2d<float> id_tex     [[texture(4)]],
                      texture2d<float> shadow_tex [[texture(5)]],
                      sampler smp                [[sampler(0)]],
                      constant params& p         [[buffer(0)]]) {
    float4 alb = albedo_tex.sample(smp, in.uv);
    if (alb.a < 0.01) discard_fragment();
    float  d   = sample_scene_depth(depth_tex, in.uv);
    float4 nrm = normal_tex.sample(smp, in.uv);
    float3 np  = nrm.xyz;
    float3 N   = normalize(np * 2.0 - 1.0);
    bool   is_mesh = nrm.a < 0.5;     // G-buffer surface class: 1 tile, 0 mesh
    float  ao  = ao_tex.sample(smp, in.uv).r;
    float  fbw = float(albedo_tex.get_width());
    float  fbh = float(albedo_tex.get_height());
    float3 W = reconstruct_world(in.uv, d, p, fbw, fbh);
    int vm = int(p.settings.x);
    if (vm == 1) return float4(alb.rgb, 1.0);
    if (vm == 2) {
        // Depth diagnostic, not presentation art. This is scene-z only,
        // shown as repeated world-unit bands so narrow ranges are visible.
        // No edge detection is mixed into this mode.
        float z_wu = d * p.vp.w + p.vp.z;
        float coarse = fract(z_wu / 1024.0);
        float fine = fract(z_wu / 128.0);
        return float4(fine, coarse, 1.0 - coarse, 1.0);
    }
    if (vm == 3) return float4(np, 1.0);
    if (vm == 5) {
        float ws = max(p.settings.z, 1.0);
        float zs = max(p.settings.w, 1.0);
        return float4(fract(W.x / ws), fract(W.y / ws), fract(W.z / zs), 1.0);
    }
    if (vm == 7) return float4(ao, ao, ao, 1.0);
    if (vm == 8) {
        // Scene-z discontinuity diagnostic. Red here is an explicit
        // derivative overlay, not raw depth.
        float z_wu = d * p.vp.w + p.vp.z;
        float edge = clamp((fabs(dfdx(z_wu)) + fabs(dfdy(z_wu))) / 48.0, 0.0, 1.0);
        return float4(edge, edge * 0.05, 0.0, 1.0);
    }
    if (vm == 9) {
        // Ground-plane-relative height diagnostic. This is reconstructed
        // world Z (W.z): flat ground should be flat color, while walls and
        // raised surfaces should form coherent equal-height bands. Blue is
        // near/below ground, warm is higher; red bands are 128 wu intervals.
        float h = W.z;
        float norm_h = clamp((h + 256.0) / 2048.0, 0.0, 1.0);
        float band = (fract(fabs(h) / 128.0) < 0.04) ? 1.0 : 0.0;
        return float4(max(norm_h, band), norm_h * (1.0 - 0.5 * band), 1.0 - norm_h, 1.0);
    }
    float cast_shadow = (p.shadow.w > 0.5) ? shadow_tex.sample(smp, in.uv).r : 1.0;
    surface_light s = shade_surface(alb.rgb, W, N, is_mesh, ao, cast_shadow, p);
    // kObjFlagSelfLit (bit 0x20 of the id's top byte): imagery retail drew
    // after the light transfer keeps its own colours.
    {
        uint2 ip = uint2(min(uint(in.uv.x * float(id_tex.get_width())),  id_tex.get_width()  - 1),
                         min(uint(in.uv.y * float(id_tex.get_height())), id_tex.get_height() - 1));
        if ((uint(id_tex.read(ip).a * 255.0 + 0.5) & 0x20u) != 0u)
            s.lit = alb.rgb;
    }
    if (vm == 4) return float4(s.points, 1.0);
    if (vm == 6) return float4(float3(s.sun_shadow), 1.0);
    float3 col = s.lit;

    // Editor outline: each id_target pixel carries flag bits in the top
    // 4 bits of its packed obj_id (alpha channel). Bit 0x80 of A is
    // kObjFlagSelected. Multi-select draws all selected drawables for
    // free -- we just check the bit per-pixel.
    {
        uint  iw = id_tex.get_width();
        uint  ih = id_tex.get_height();
        uint2 px = uint2(uint(in.uv.x * float(iw)),
                         uint(in.uv.y * float(ih)));
        if (px.x >= iw) px.x = iw - 1;
        if (px.y >= ih) px.y = ih - 1;
        uint xL = (px.x > 0) ? px.x - 1 : 0;
        uint xR = (px.x + 1 < iw) ? px.x + 1 : iw - 1;
        uint yT = (px.y > 0) ? px.y - 1 : 0;
        uint yB = (px.y + 1 < ih) ? px.y + 1 : ih - 1;
        // SelBit() returns true iff the alpha channel of the read texel
        // has bit 0x80 set (kObjFlagSelected).
        auto SelBit = [&](uint2 q) {
            float a = id_tex.read(q).a;
            return uint(a * 255.0 + 0.5) >= 128u;
        };
        bool matchC = SelBit(px);
        int  matchN = (SelBit(uint2(xL, px.y)) ? 1 : 0)
                    + (SelBit(uint2(xR, px.y)) ? 1 : 0)
                    + (SelBit(uint2(px.x, yT)) ? 1 : 0)
                    + (SelBit(uint2(px.x, yB)) ? 1 : 0);
        if (matchC && matchN < 4) {
            const float3 outline_col = float3(1.0, 0.85, 0.20);
            col = mix(col, outline_col, 0.85);
        } else if (matchC) {
            col = mix(col, float3(1.0, 0.92, 0.55), 0.18);
        }
    }
    return float4(col, 1.0);
}
)MSL";
