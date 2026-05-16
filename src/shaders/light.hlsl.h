// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *       light.hlsl.h  - HLSL deferred lighting (SM5 / D3D11 variant)    *
// *************************************************************************
//
// Windows / SOKOL_D3D11 port of light.metal.h. Lighting consumes the
// precomputed sun-shadow mask; it does not ray-march.
//
// NOTE: KPL must match TRenderer::kMaxPointLights in renderer.h. If you
// change one, change the other.
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
#define ISO_COS30 0.867
#define ISO_WZ_DENOM 2.003378
#define KPL 16
cbuffer params : register(b0) {
    float4 vp;
    float4 recon;
    float4 light_dir;
    float4 light_col;
    float4 ambient_col;
    float4 settings;
    float4 plight_pos[KPL];
    float4 plight_col[KPL];
    float4 shadow;
    float4 shadow_dir;
    float4 shadow_world_dir;
    float4 normal_lighting;
};
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
float2 project_world_uv(float3 W, float fbw, float fbh) {
    float dx = W.x - recon.x;
    float dy = W.y - recon.y;
    float S = dx - dy;
    float T = 0.5 * (dx + dy) - W.z * ISO_COS30;
    float scale = max(settings.w, 0.0001);
    if (recon.w > 0.5) {
        float scene_z = recon.z - ISO_COS30 * (dx + dy) - 0.5 * W.z;
        scale *= recon.z / max(scene_z, 1.0);
    }
    S *= scale;
    T *= scale;
    return float2((S + vp.x) / fbw, (T + vp.y) / fbh);
}
float scene_depth_world(float3 W) {
    float dx = W.x - recon.x;
    float dy = W.y - recon.y;
    return recon.z - ISO_COS30 * (dx + dy) - 0.5 * W.z;
}
float interpolate_ray_depth(float z0, float z1, float a) {
    if (recon.w > 0.5) {
        float inv_z = lerp(1.0 / max(z0, 1.0), 1.0 / max(z1, 1.0), a);
        return 1.0 / max(inv_z, 1e-6);
    }
    return lerp(z0, z1, a);
}
float sample_scene_depth(float2 uv) {
    uint w, h;
    depth_tex.GetDimensions(w, h);
    int2 p = int2(clamp(uv * float2(float(w), float(h)),
                         float2(0.0, 0.0),
                         float2(float(w - 1), float(h - 1))));
    return depth_tex.Load(int3(p, 0)).r;
}
float shadow_tap_weight(int tap, int sampleCount) {
    if (sampleCount <= 1) return 1.0;
    float u = (float)tap / (float)(sampleCount - 1);
    return lerp(0.5, 1.0, 1.0 - abs(u * 2.0 - 1.0));
}
float4 main_ps(vs_out in_) : SV_Target0 {
    float4 alb = albedo_tex.Sample(smp, in_.uv);
    if (alb.a < 0.01) discard;
    float  d   = sample_scene_depth(in_.uv);
    float3 np  = normal_tex.Sample(smp, in_.uv).xyz;
    float3 N   = normalize(np * 2.0 - 1.0);
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
    int nl_dbg = (int)settings.y;
    if (vm == 4) {
        float3 accum = float3(0.0, 0.0, 0.0);
        const float kMP = 0.0022436;
        const float kSC = 50.0 / (1.0 - kMP);
        [loop] for (int i = 0; i < nl_dbg; ++i) {
            float3 delta = plight_pos[i].xyz - W;
            float  dist  = length(delta);
            float  rad   = plight_pos[i].w;
            if (rad > 0.0 && dist < rad) {
                float3 Lp    = delta / max(dist, 1e-5);
                float  pterm = lerp(1.0, max(dot(N, Lp), 0.0), saturate(normal_lighting.x));
                float normd = dist * 254.0 / rad;
                float pw    = pow(normd + 1.0, -1.1) - kMP;
                float attn  = saturate(pw * kSC);
                accum += plight_col[i].rgb * plight_col[i].w * attn * pterm;
            }
        }
        return float4(accum, 1.0);
    }
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
    float3 light = ambient_col.rgb * light_col.w;
    int mode = (int)settings.z;
    if (mode == 1) light *= ao;
    float3 Ldir = normalize(light_dir.xyz);
    float  raw_sun_ndotl = dot(N, Ldir);
    float  sun_ndotl = max(raw_sun_ndotl, 0.0);
    float  normal_hardness = saturate(normal_lighting.x);
    float  sun_term = lerp(1.0, sun_ndotl, normal_hardness);
    // Direct sun visibility is a composition of two intentionally separate
    // facts: normal-facing geometry and the blurred cast-shadow mask. The
    // lighting pass never ray-marches; the mask is produced once per frame by
    // the shadow pass, then blurred as an image operation.
    float normal_visibility = (raw_sun_ndotl > 0.0) ? 1.0 : 0.0;
    float cast_shadow = (mode == 1 && shadow.w > 0.5) ? shadow_tex.Sample(smp, in_.uv).r : 1.0;
    float sun_shadow = normal_visibility * cast_shadow;
    if (mode == 1) {
        light += light_col.rgb * light_dir.w * sun_term * sun_shadow;
    }
    if (vm == 6) return float4(sun_shadow, sun_shadow, sun_shadow, 1.0);
    const float kMinPower = 0.0022436;
    const float kScale    = 50.0 / (1.0 - kMinPower);
    int nl = (int)settings.y;
    [loop] for (int i = 0; i < KPL; ++i) {
        if (i >= nl) break;
        float3 delta = plight_pos[i].xyz - W;
        float  dist  = length(delta);
        float  rad   = plight_pos[i].w;
        if (rad > 0.0 && dist < rad) {
            float3 Lp    = delta / max(dist, 1e-5);
            float  pterm = lerp(1.0, max(dot(N, Lp), 0.0), normal_hardness);
            float normd = dist * 254.0 / rad;
            float pw    = pow(normd + 1.0, -1.1) - kMinPower;
            float attn  = saturate(pw * kScale);
            light += plight_col[i].rgb * plight_col[i].w * attn * pterm;
        }
    }
  // Brightness cap (see light.glsl.h for rationale).
    float light_ceiling = max(ambient_col.w, 1e-3);
    light = min(light, float3(light_ceiling, light_ceiling, light_ceiling));
    return float4(alb.rgb * light, 1.0);
}
)HLSL";
