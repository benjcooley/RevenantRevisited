// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *       light.hlsl.h  - HLSL deferred lighting (SM5 / D3D11 variant)    *
// *************************************************************************
//
// Windows / SOKOL_D3D11 port of light.metal.h.
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
SamplerState smp        : register(s0);
struct vs_out { float4 pos : SV_Position; float2 uv : TEXCOORD0; };
float4 main_ps(vs_out in_) : SV_Target0 {
    float4 alb = albedo_tex.Sample(smp, in_.uv);
    if (alb.a < 0.01) discard;
    float  d   = depth_tex.Sample(smp, in_.uv).r;
    float3 np  = normal_tex.Sample(smp, in_.uv).xyz;
    float3 N   = normalize(np * 2.0 - 1.0);
    float  ao  = ao_tex.Sample(smp, in_.uv).r;
    float  fbw, fbh; albedo_tex.GetDimensions(fbw, fbh);
    float  S   = in_.uv.x * fbw - vp.x;
    float  T   = in_.uv.y * fbh - vp.y;
    float  scene_z = d * vp.w + vp.z;
    float  K   = recon.z - scene_z;
    float  wz  = (K - 2.0 * T * ISO_COS30) / ISO_WZ_DENOM;
    float  sum_r = 2.0 * (T + wz * ISO_COS30);
    float3 W = float3(recon.x + (sum_r + S) * 0.5,
                      recon.y + (sum_r - S) * 0.5,
                      wz);
    int vm = (int)settings.x;
    if (vm == 1) return float4(alb.rgb, 1.0);
    if (vm == 2) { float vd = saturate(1.0 - d); return float4(vd, vd, vd, 1.0); }
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
    float3 light = ambient_col.rgb * light_col.w;
    int mode = (int)settings.z;
    if (mode == 1) light *= ao;
    float sun_shadow = 1.0;
    float3 Ldir = normalize(light_dir.xyz);
    float  sun_ndotl = max(dot(N, Ldir), 0.0);
    float  normal_hardness = saturate(normal_lighting.x);
    float  sun_term = lerp(1.0, sun_ndotl, normal_hardness);
    if (mode == 1 && shadow.w > 0.5 && sun_term > 0.0) {
        float3 Sdir = normalize(shadow_world_dir.xyz);
        if (length(Sdir) < 1e-5) Sdir = Ldir;
        float S_per = (-Sdir.x + Sdir.y) + shadow_dir.x;
        float T_per = ((-Sdir.x - Sdir.y) * 0.5 + Sdir.z * ISO_COS30)
                    + shadow_dir.y;
        float Sz    = Sdir.z * shadow_dir.z;
        float step_wu  = shadow.x;
        float soft_px  = shadow.y;
        int   maxSteps = (int)shadow.z;
        const float kBiasWu = 2.0;
        float2 sun_uv  = float2(S_per, T_per);
        float2 perp_uv = normalize(float2(-sun_uv.y, sun_uv.x));
        float h = frac(sin(dot(in_.uv, float2(12.9898, 78.233))) * 43758.5453);
        const int kRays = 4;
        float hits = 0.0;
        [loop] for (int r = 0; r < kRays; ++r) {
            float rf = (float(r) + h) / float(kRays);
            float off_px = (rf - 0.5) * 2.0 * soft_px;
            float2 perp_off = perp_uv * off_px / float2(fbw, fbh);
            float rh = frac(h + float(r) * 0.6180339);
            bool hit = false;
            [loop] for (int i = 1; i <= maxSteps; ++i) {
                float t = (float(i) - 0.5 + rh) * step_wu;
                float2 suv = in_.uv + perp_off +
                             float2(S_per * t / fbw, T_per * t / fbh);
                if (suv.x < 0.0 || suv.x > 1.0 ||
                    suv.y < 0.0 || suv.y > 1.0) break;
                float4 albs = albedo_tex.Sample(smp, suv);
                if (albs.a < 0.01) continue;
                float drs = depth_tex.Sample(smp, suv).r;
                float szs = drs * vp.w + vp.z;
                float Tt  = suv.y * fbh - vp.y;
                float Ks  = recon.z - szs;
                float wzs = (Ks - 2.0 * Tt * ISO_COS30) / ISO_WZ_DENOM;
                float ray_wz = W.z + t * Sz;
                if (wzs > ray_wz + kBiasWu) { hit = true; break; }
            }
            if (hit) hits += 1.0;
        }
        float occ = hits / float(kRays);
        sun_shadow = 1.0 - occ * 0.8;
    }
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
    return float4(alb.rgb * light, 1.0);
}
)HLSL";
