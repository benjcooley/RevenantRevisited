// *************************************************************************
// *                         Cinematix Revenant                            *
// *        shadow.hlsl.h - HLSL sun shadow mask + blur passes             *
// *************************************************************************

#pragma once

inline constexpr const char* kShadowVsHlsl = R"HLSL(
struct vs_in  { float2 pos : POSITION; float2 uv : TEXCOORD0; };
struct vs_out { float4 pos : SV_Position; float2 uv : TEXCOORD0; };
vs_out main_vs(vs_in i) {
    vs_out o;
    o.pos = float4(i.pos * 2.0 - 1.0, 0.0, 1.0);
    o.uv  = i.uv;
    return o;
}
)HLSL";

inline constexpr const char* kShadowFsHlsl = R"HLSL(
#define ISO_COS30 0.867
#define ISO_WZ_DENOM 2.003378
cbuffer params : register(b0) {
    float4 vp;
    float4 recon;
    float4 camera;
    float4 shadow;
    float4 shadow_dir;
    float4 shadow_world_dir;
    float4 normal_lighting;
};
Texture2D    albedo_tex : register(t0);
Texture2D    depth_tex  : register(t1);
SamplerState smp        : register(s0);
struct vs_out { float4 pos : SV_Position; float2 uv : TEXCOORD0; };
float3 reconstruct_world(float2 uv, float d, float fbw, float fbh) {
    float scene_z = d * vp.w + vp.z;
    float zoom = max(camera.x, 0.0001);
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
    float scale = max(camera.x, 0.0001);
    if (recon.w > 0.5) {
        float scene_z = recon.z - ISO_COS30 * (dx + dy) - 0.5 * W.z;
        scale *= recon.z / max(scene_z, 1.0);
    }
    return float2((S * scale + vp.x) / fbw, (T * scale + vp.y) / fbh);
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
float main_ps(vs_out in_) : SV_Target0 {
    float4 alb = albedo_tex.Sample(smp, in_.uv);
    if (alb.a < 0.01 || shadow.w <= 0.5) return 1.0;
    float d = sample_scene_depth(in_.uv);
    if (d >= 0.9999) return 1.0;

    float fbw, fbh; albedo_tex.GetDimensions(fbw, fbh);
    float3 W = reconstruct_world(in_.uv, d, fbw, fbh);
    float3 Sdir = shadow_world_dir.xyz;
    if (length(Sdir) < 1e-5) return 1.0;
    Sdir.z *= shadow_dir.z;
    float3 Rdir = normalize(Sdir);

    float step_wu = shadow.x;
    int maxSteps = (int)shadow.z;
    float reach_wu = step_wu * (float)maxSteps;
    float3 rayEndW = W + Rdir * reach_wu;
    float2 rayEndUv = project_world_uv(rayEndW, fbw, fbh);
    float2 ray_uv = rayEndUv - in_.uv;
    if (length(ray_uv) <= 1e-6 || reach_wu <= 0.0) return 1.0;

    float ray_z0_wu = scene_depth_world(W);
    float ray_z1_wu = scene_depth_world(rayEndW);
    float depth_cutoff_wu = max(shadow_dir.y, 1.0);
    float bias_wu = normal_lighting.y;
    [loop] for (int i = 1; i <= 512; ++i) {
        if (i > maxSteps) break;
        float a = ((float)i - 0.5) / (float)maxSteps;
        float2 suv = lerp(in_.uv, rayEndUv, a);
        if (suv.x < 0.0 || suv.x > 1.0 || suv.y < 0.0 || suv.y > 1.0) break;
        float drs = sample_scene_depth(suv);
        if (drs >= 0.9999) continue;
        float scene_z_wu = drs * vp.w + vp.z;
        float ray_z_wu = interpolate_ray_depth(ray_z0_wu, ray_z1_wu, a);
        float depth_delta_wu = scene_z_wu - ray_z_wu;
        if (depth_delta_wu >= -depth_cutoff_wu && depth_delta_wu <= bias_wu) return 0.0;
    }
    return 1.0;
}
)HLSL";

inline constexpr const char* kShadowBlurFsHlsl = R"HLSL(
cbuffer params : register(b0) {
    float4 blur;
};
Texture2D    shadow_tex : register(t0);
SamplerState smp        : register(s0);
struct vs_out { float4 pos : SV_Position; float2 uv : TEXCOORD0; };
float main_ps(vs_out in_) : SV_Target0 {
    float tw, th; shadow_tex.GetDimensions(tw, th);
    float2 dir = blur.xy / max(float2(tw, th), float2(1.0, 1.0));
    float radius = max(blur.z, 0.0);
    float c = 0.38774 * shadow_tex.Sample(smp, in_.uv).r;
    c += 0.24477 * shadow_tex.Sample(smp, in_.uv + dir * radius).r;
    c += 0.24477 * shadow_tex.Sample(smp, in_.uv - dir * radius).r;
    c += 0.06136 * shadow_tex.Sample(smp, in_.uv + dir * radius * 2.0).r;
    c += 0.06136 * shadow_tex.Sample(smp, in_.uv - dir * radius * 2.0).r;
    return saturate(c);
}
)HLSL";
