// *************************************************************************
// *                         Cinematix Revenant                            *
// *        shadow.metal.h - MSL sun shadow mask + blur passes             *
// *************************************************************************

#pragma once

inline constexpr const char* kShadowVsMetal = R"MSL(
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

inline constexpr const char* kShadowFsMetal = R"MSL(
#include <metal_stdlib>
using namespace metal;
#define ISO_COS30 0.867
#define ISO_WZ_DENOM 2.003378
struct params {
    float4 vp; float4 recon; float4 camera; float4 shadow;
    float4 shadow_dir; float4 shadow_world_dir; float4 normal_lighting;
};
struct vs_out { float4 pos [[position]]; float2 uv; };
static float3 reconstruct_world(float2 uv, float d, constant params& p, float fbw, float fbh) {
    float scene_z = d * p.vp.w + p.vp.z;
    float zoom = max(p.camera.x, 0.0001);
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
static float2 project_world_uv(float3 W, constant params& p, float fbw, float fbh) {
    float dx = W.x - p.recon.x;
    float dy = W.y - p.recon.y;
    float S = dx - dy;
    float T = 0.5 * (dx + dy) - W.z * ISO_COS30;
    float scale = max(p.camera.x, 0.0001);
    if (p.recon.w > 0.5) {
        float scene_z = p.recon.z - ISO_COS30 * (dx + dy) - 0.5 * W.z;
        scale *= p.recon.z / max(scene_z, 1.0);
    }
    return float2((S * scale + p.vp.x) / fbw, (T * scale + p.vp.y) / fbh);
}
static float scene_depth_world(float3 W, constant params& p) {
    float dx = W.x - p.recon.x;
    float dy = W.y - p.recon.y;
    return p.recon.z - ISO_COS30 * (dx + dy) - 0.5 * W.z;
}
static float interpolate_ray_depth(float z0, float z1, float a, constant params& p) {
    if (p.recon.w > 0.5) {
        float inv_z = mix(1.0 / max(z0, 1.0), 1.0 / max(z1, 1.0), a);
        return 1.0 / max(inv_z, 1e-6);
    }
    return mix(z0, z1, a);
}
static float sample_scene_depth(texture2d<float> depth_tex, float2 uv) {
    uint w = depth_tex.get_width();
    uint h = depth_tex.get_height();
    uint2 p = uint2(uint(clamp(uv.x * float(w), 0.0, float(w - 1))),
                    uint(clamp(uv.y * float(h), 0.0, float(h - 1))));
    return depth_tex.read(p).r;
}
fragment float _main(vs_out in [[stage_in]],
                     texture2d<float> albedo_tex [[texture(0)]],
                     texture2d<float> depth_tex  [[texture(1)]],
                     sampler smp                [[sampler(0)]],
                     constant params& p         [[buffer(0)]]) {
    float4 alb = albedo_tex.sample(smp, in.uv);
    if (alb.a < 0.01 || p.shadow.w <= 0.5) return 1.0;
    float d = sample_scene_depth(depth_tex, in.uv);
    if (d >= 0.9999) return 1.0;

    float fbw = float(albedo_tex.get_width());
    float fbh = float(albedo_tex.get_height());
    float3 W = reconstruct_world(in.uv, d, p, fbw, fbh);
    float3 Sdir = p.shadow_world_dir.xyz;
    if (length(Sdir) < 1e-5) return 1.0;
    Sdir.z *= p.shadow_dir.z;
    float3 Rdir = normalize(Sdir);

    float step_wu = p.shadow.x;
    int maxSteps = int(p.shadow.z);
    float reach_wu = step_wu * float(maxSteps);
    float3 rayEndW = W + Rdir * reach_wu;
    float2 rayEndUv = project_world_uv(rayEndW, p, fbw, fbh);
    float2 ray_uv = rayEndUv - in.uv;
    if (length(ray_uv) <= 1e-6 || reach_wu <= 0.0) return 1.0;

    float ray_z0_wu = scene_depth_world(W, p);
    float ray_z1_wu = scene_depth_world(rayEndW, p);
    float depth_cutoff_wu = max(p.shadow_dir.y, 1.0);
    float bias_wu = p.normal_lighting.y;
    for (int i = 1; i <= 512; ++i) {
        if (i > maxSteps) break;
        float a = (float(i) - 0.5) / float(maxSteps);
        float2 suv = mix(in.uv, rayEndUv, a);
        if (suv.x < 0.0 || suv.x > 1.0 || suv.y < 0.0 || suv.y > 1.0) break;
        float drs = sample_scene_depth(depth_tex, suv);
        if (drs >= 0.9999) continue;
        float scene_z_wu = drs * p.vp.w + p.vp.z;
        float ray_z_wu = interpolate_ray_depth(ray_z0_wu, ray_z1_wu, a, p);
        float depth_delta_wu = scene_z_wu - ray_z_wu;
        if (depth_delta_wu >= -depth_cutoff_wu && depth_delta_wu <= bias_wu) return 0.0;
    }
    return 1.0;
}
)MSL";

inline constexpr const char* kShadowBlurFsMetal = R"MSL(
#include <metal_stdlib>
using namespace metal;
struct params { float4 blur; };
struct vs_out { float4 pos [[position]]; float2 uv; };
fragment float _main(vs_out in [[stage_in]],
                     texture2d<float> shadow_tex [[texture(0)]],
                     sampler smp                [[sampler(0)]],
                     constant params& p         [[buffer(0)]]) {
    float2 ts = float2(float(shadow_tex.get_width()), float(shadow_tex.get_height()));
    float2 dir = p.blur.xy / max(ts, float2(1.0));
    float radius = max(p.blur.z, 0.0);
    float c = 0.38774 * shadow_tex.sample(smp, in.uv).r;
    c += 0.24477 * shadow_tex.sample(smp, in.uv + dir * radius).r;
    c += 0.24477 * shadow_tex.sample(smp, in.uv - dir * radius).r;
    c += 0.06136 * shadow_tex.sample(smp, in.uv + dir * radius * 2.0).r;
    c += 0.06136 * shadow_tex.sample(smp, in.uv - dir * radius * 2.0).r;
    return clamp(c, 0.0, 1.0);
}
)MSL";
