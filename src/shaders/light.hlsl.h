// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *       light.hlsl.h  - HLSL deferred lighting (SM5 / D3D11 variant)    *
// *************************************************************************
//
// Windows / SOKOL_D3D11 port of light.metal.h. TRenderer prepends
// lightmodel.hlsl.h (the params cbuffer and shade_surface) to
// kLightFsHlsl. Lighting consumes the precomputed sun-shadow mask; it
// does not ray-march.
//
// *************************************************************************

#pragma once

inline constexpr const char* kLightVsHlsl = R"HLSL(
struct vs_in  { float2 pos : POSITION; float2 uv : TEXCOORD0; };
struct vs_out { float4 pos : SV_Position; float2 uv : TEXCOORD0; };
vs_out main_vs(vs_in i) {
    vs_out o;
    o.pos = float4(i.pos * 2.0 - 1.0, 0.0, 1.0);
    o.uv  = i.uv;
    return o;
}
)HLSL";

inline constexpr const char* kLightFsHlsl = R"HLSL(
Texture2D    albedo_tex : register(t0);
Texture2D    normal_tex : register(t1);
Texture2D    depth_tex  : register(t2);
Texture2D    ao_tex     : register(t3);
Texture2D    id_tex     : register(t4);
Texture2D    shadow_tex : register(t5);
SamplerState smp        : register(s0);
struct vs_out { float4 pos : SV_Position; float2 uv : TEXCOORD0; };
float3 reconstruct_world(float2 uv, float d, float fbw, float fbh) {
    float scene_z = d * vp.w + vp.z;
    // Convert framebuffer pixels back to Revenant's authored iso screen
    // units before applying the inverse. This is required even in ortho:
    // high-resolution render targets scale the sprite pass, but they do not
    // change the world camera.
    float zoom = max(settings.w, 0.0001);
    float S = (uv.x * fbw - vp.x) / zoom;
    float T = (uv.y * fbh - vp.y) / zoom;
    if (recon.w > 0.5) {
        float focal = max(recon.z, 1.0);
        S = (S / focal) * scene_z;
        T = (T / focal) * scene_z;
    }
    float K = recon.z - scene_z;
    float wz = (K - 2.0 * T * ISO_COS30) / ISO_WZ_DENOM;
    float sum_r = 2.0 * (T + wz * ISO_COS30);
    return float3(recon.x + (sum_r + S) * 0.5,
                  recon.y + (sum_r - S) * 0.5,
                  wz);
}
float sample_scene_depth(float2 uv) {
    uint w, h;
    depth_tex.GetDimensions(w, h);
    int2 p = int2(clamp(uv * float2(float(w), float(h)),
                         float2(0.0, 0.0),
                         float2(float(w - 1), float(h - 1))));
    return depth_tex.Load(int3(p, 0)).r;
}
float4 main_ps(vs_out in_) : SV_Target0 {
    float4 alb = albedo_tex.Sample(smp, in_.uv);
    if (alb.a < 0.01) discard;
    float  d   = sample_scene_depth(in_.uv);
    float4 nrm = normal_tex.Sample(smp, in_.uv);
    float3 np  = nrm.xyz;
    float3 N   = normalize(np * 2.0 - 1.0);
    bool   is_mesh = nrm.a < 0.5;     // G-buffer surface class: 1 tile, 0 mesh
    float  ao  = ao_tex.Sample(smp, in_.uv).r;
    float  fbw, fbh; albedo_tex.GetDimensions(fbw, fbh);
    float3 W = reconstruct_world(in_.uv, d, fbw, fbh);
    int vm = (int)settings.x;
    if (vm == 1) return float4(alb.rgb, 1.0);
    if (vm == 2) {
        // Depth diagnostic, not presentation art. This is scene-z only,
        // shown as repeated world-unit bands so narrow ranges are visible.
        // No edge detection is mixed into this mode.
        float z_wu = d * vp.w + vp.z;
        float coarse = frac(z_wu / 1024.0);
        float fine = frac(z_wu / 128.0);
        return float4(fine, coarse, 1.0 - coarse, 1.0);
    }
    if (vm == 3) return float4(np, 1.0);
    if (vm == 5) {
        float ws = max(settings.z, 1.0);
        float zs = max(settings.w, 1.0);
        return float4(frac(W.x / ws), frac(W.y / ws), frac(W.z / zs), 1.0);
    }
    if (vm == 7) return float4(ao, ao, ao, 1.0);
    if (vm == 8) {
        // Scene-z discontinuity diagnostic. Red here is an explicit
        // derivative overlay, not raw depth.
        float z_wu = d * vp.w + vp.z;
        float edge = saturate((abs(ddx(z_wu)) + abs(ddy(z_wu))) / 48.0);
        return float4(edge, edge * 0.05, 0.0, 1.0);
    }
    if (vm == 9) {
        // Ground-plane-relative height diagnostic. This is reconstructed
        // world Z (W.z): flat ground should be flat color, while walls and
        // raised surfaces should form coherent equal-height bands. Blue is
        // near/below ground, warm is higher; red bands are 128 wu intervals.
        float h = W.z;
        float norm_h = saturate((h + 256.0) / 2048.0);
        float band = (frac(abs(h) / 128.0) < 0.04) ? 1.0 : 0.0;
        return float4(max(norm_h, band), norm_h * (1.0 - 0.5 * band), 1.0 - norm_h, 1.0);
    }
    float cast_shadow = (shadow.w > 0.5) ? shadow_tex.Sample(smp, in_.uv).r : 1.0;
    surface_light s = shade_surface(alb.rgb, W, N, is_mesh, ao, cast_shadow);
    // kObjFlagSelfLit (bit 0x20 of the id's top byte): imagery retail drew
    // after the light transfer keeps its own colours.
    {
        uint iw, ih;
        id_tex.GetDimensions(iw, ih);
        int2 ip = int2(min(uint(in_.uv.x * float(iw)), iw - 1), min(uint(in_.uv.y * float(ih)), ih - 1));
        if ((uint(id_tex.Load(int3(ip, 0)).a * 255.0 + 0.5) & 0x20u) != 0u)
            s.lit = alb.rgb;
    }
    if (vm == 4) return float4(s.points, 1.0);
    if (vm == 6) return float4(s.sun_shadow, s.sun_shadow, s.sun_shadow, 1.0);
    return float4(s.lit, 1.0);
}
)HLSL";
