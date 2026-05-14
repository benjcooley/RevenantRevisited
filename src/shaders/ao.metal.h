// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *         ao.metal.h  - MSL screen-space ambient occlusion              *
// *************************************************************************
//
// Z-buffer-driven SSAO. Reconstructs each fragment's world position from
// scene_z via the retail iso inverse (matches tile.metal.h), then samples
// a jittered ring of neighbors to estimate hemispherical occlusion against
// the fragment normal. Writes a single R32F value in [0..1] (1 = unoccluded).
//
// *************************************************************************

#pragma once

inline constexpr const char* kAOVsMetal = R"MSL(
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

inline constexpr const char* kAOFsMetal = R"MSL(
#include <metal_stdlib>
using namespace metal;
#define ISO_COS30 0.867
#define ISO_WZ_DENOM 2.003378
struct params { float4 vp; float4 recon; float4 ao_settings; float4 camera; };
struct vs_out { float4 pos [[position]]; float2 uv; };
float3 reconstruct_world(float2 uv, float d, constant params& p, float fbw, float fbh) {
    // Framebuffer pixels are scaled by the active camera zoom/resolution.
    // Undo that first so the iso inverse sees the same units as WorldToScreen.
    float zoom = max(p.camera.x, 0.0001);
    float S = (uv.x * fbw - p.vp.x) / zoom;
    float T = (uv.y * fbh - p.vp.y) / zoom;
    float scene_z = d * p.vp.w + p.vp.z;
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
fragment float _main(vs_out in [[stage_in]],
                     texture2d<float> albedo_tex [[texture(0)]],
                     texture2d<float> normal_tex [[texture(1)]],
                     texture2d<float> depth_tex  [[texture(2)]],
                     sampler smp                [[sampler(0)]],
                     constant params& p         [[buffer(0)]]) {
    float4 alb = albedo_tex.sample(smp, in.uv);
    if (alb.a < 0.01) return 1.0;
    float strength = max(p.ao_settings.y, 0.0);
    if (strength <= 0.0) return 1.0;
    float radius_px = max(p.ao_settings.x, 1.0);
    float bias = p.ao_settings.z;
    float max_dist = max(p.ao_settings.w, 1.0);
    float d = depth_tex.sample(smp, in.uv).r;
    float3 N = normalize(normal_tex.sample(smp, in.uv).xyz * 2.0 - 1.0);
    float fbw = float(albedo_tex.get_width());
    float fbh = float(albedo_tex.get_height());
    float3 W = reconstruct_world(in.uv, d, p, fbw, fbh);
    float h = fract(sin(dot(in.uv, float2(12.9898, 78.233))) * 43758.5453);
    const int kSamples = 8;
    float occ = 0.0;
    float wsum = 0.0;
    for (int i = 0; i < kSamples; ++i) {
        float a = h * 6.2831853 + float(i) * (6.2831853 / float(kSamples));
        float ring = radius_px * (0.35 + 0.65 * fract(h + float(i) * 0.6180339));
        float2 suv = in.uv + float2(cos(a), sin(a)) * ring / float2(fbw, fbh);
        if (suv.x < 0.0 || suv.x > 1.0 || suv.y < 0.0 || suv.y > 1.0) continue;
        float4 as = albedo_tex.sample(smp, suv);
        if (as.a < 0.01) continue;
        float ds = depth_tex.sample(smp, suv).r;
        float3 Ws = reconstruct_world(suv, ds, p, fbw, fbh);
        float3 V = Ws - W;
        float dist = length(V);
        if (dist < 1e-4 || dist > max_dist) continue;
        float hemi = pow(max(dot(N, normalize(V)) - bias, 0.0), 1.35);
        float attn = 1.0 - dist / max_dist;
        attn *= attn;
        occ += hemi * attn;
        wsum += attn;
    }
    if (wsum <= 1e-5) return 1.0;
    float occ_n = clamp(occ / wsum, 0.0, 1.0);
    occ_n = pow(occ_n, 0.75);
    return clamp(1.0 - strength * occ_n, 0.0, 1.0);
}
)MSL";
