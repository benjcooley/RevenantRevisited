// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *          ao.hlsl.h  - HLSL SSAO shader (SM5 / D3D11 variant)          *
// *************************************************************************
//
// Windows / SOKOL_D3D11 port of ao.metal.h.
//
// *************************************************************************

#pragma once

inline constexpr const char* kAOVsHlsl = R"HLSL(
struct vs_in  { float2 pos : POSITION; float2 uv : TEXCOORD0; };
struct vs_out { float4 pos : SV_Position; float2 uv : TEXCOORD0; };
vs_out main_vs(vs_in i) {
    vs_out o;
    o.pos = float4(i.pos * 2.0 - 1.0, 0.0, 1.0);
    o.uv  = i.uv;
    return o;
}
)HLSL";

inline constexpr const char* kAOFsHlsl = R"HLSL(
#define ISO_COS30 0.867
#define ISO_WZ_DENOM 2.003378
cbuffer params : register(b0) {
    float4 vp;
    float4 recon;
    float4 ao_settings;
};
Texture2D    albedo_tex : register(t0);
Texture2D    normal_tex : register(t1);
Texture2D    depth_tex  : register(t2);
SamplerState smp        : register(s0);
struct vs_out { float4 pos : SV_Position; float2 uv : TEXCOORD0; };
float3 reconstruct_world(float2 uv, float d, float fbw, float fbh) {
    float S = uv.x * fbw - vp.x;
    float T = uv.y * fbh - vp.y;
    float scene_z = d * vp.w + vp.z;
    float K = recon.z - scene_z;
    float wz = (K - 2.0 * T * ISO_COS30) / ISO_WZ_DENOM;
    float sum_r = 2.0 * (T + wz * ISO_COS30);
    return float3(recon.x + (sum_r + S) * 0.5,
                  recon.y + (sum_r - S) * 0.5,
                  wz);
}
float main_ps(vs_out in_) : SV_Target0 {
    float4 alb = albedo_tex.Sample(smp, in_.uv);
    if (alb.a < 0.01) return 1.0;
    float strength = max(ao_settings.y, 0.0);
    if (strength <= 0.0) return 1.0;
    float radius_px = max(ao_settings.x, 1.0);
    float bias = ao_settings.z;
    float max_dist = max(ao_settings.w, 1.0);
    float d = depth_tex.Sample(smp, in_.uv).r;
    float3 N = normalize(normal_tex.Sample(smp, in_.uv).xyz * 2.0 - 1.0);
    float fbw, fbh; albedo_tex.GetDimensions(fbw, fbh);
    float3 W = reconstruct_world(in_.uv, d, fbw, fbh);
    float h = frac(sin(dot(in_.uv, float2(12.9898, 78.233))) * 43758.5453);
    const int kSamples = 8;
    float occ = 0.0;
    float wsum = 0.0;
    [loop] for (int i = 0; i < kSamples; ++i) {
        float a = h * 6.2831853 + float(i) * (6.2831853 / float(kSamples));
        float ring = radius_px * (0.35 + 0.65 * frac(h + float(i) * 0.6180339));
        float2 suv = in_.uv + float2(cos(a), sin(a)) * ring / float2(fbw, fbh);
        if (suv.x < 0.0 || suv.x > 1.0 || suv.y < 0.0 || suv.y > 1.0) continue;
        float4 as = albedo_tex.Sample(smp, suv);
        if (as.a < 0.01) continue;
        float ds = depth_tex.Sample(smp, suv).r;
        float3 Ws = reconstruct_world(suv, ds, fbw, fbh);
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
    float occ_n = saturate(occ / wsum);
    occ_n = pow(occ_n, 0.75);
    return saturate(1.0 - strength * occ_n);
}
)HLSL";
