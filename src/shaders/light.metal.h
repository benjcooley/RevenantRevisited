// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *        light.metal.h  - MSL deferred lighting fullscreen pass         *
// *************************************************************************
//
// Reads the G-buffer (albedo + world normal + scene_z + AO) and runs:
//   * directional sun (hard normal-facing visibility, tunable N.L falloff)
//   * blurred screen-space sun-shadow mask (mode 1 only)
//   * up to KPL point lights with retail's pow-based falloff curve
//
// Fragment world position is reconstructed from scene_z via the retail iso
// inverse (same math as ao.metal.h and tile.metal.h). See
// docs/DEFERRED_LIGHTING.md for the derivation.
//
// NOTE: KPL must match TRenderer::kMaxPointLights in renderer.h. If you
// change one, change the other.
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
#include <metal_stdlib>
using namespace metal;
#define ISO_COS30 0.867
#define ISO_WZ_DENOM 2.003378
#define KPL 16
struct params {
    float4 vp; float4 recon; float4 light_dir; float4 light_col;
    float4 ambient_col; float4 settings;
    float4 plight_pos[KPL]; float4 plight_col[KPL];
    float4 shadow;
    float4 shadow_dir;
    float4 shadow_world_dir;
    float4 normal_lighting;
    float4 selected_obj_id;
};
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
static float2 project_world_uv(float3 W, constant params& p, float fbw, float fbh) {
    float dx = W.x - p.recon.x;
    float dy = W.y - p.recon.y;
    float S = dx - dy;
    float T = 0.5 * (dx + dy) - W.z * ISO_COS30;
    float scale = max(p.settings.w, 0.0001);
    if (p.recon.w > 0.5) {
        float scene_z = p.recon.z - ISO_COS30 * (dx + dy) - 0.5 * W.z;
        scale *= p.recon.z / max(scene_z, 1.0);
    }
    S *= scale;
    T *= scale;
    return float2((S + p.vp.x) / fbw, (T + p.vp.y) / fbh);
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
static float shadow_tap_weight(int tap, int sampleCount) {
    if (sampleCount <= 1) return 1.0;
    float u = float(tap) / float(sampleCount - 1);
    return mix(0.5, 1.0, 1.0 - fabs(u * 2.0 - 1.0));
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
    float3 np  = normal_tex.sample(smp, in.uv).xyz;
    float3 N   = normalize(np * 2.0 - 1.0);
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
    int nl_dbg = int(p.settings.y);
    if (vm == 4) {
        float3 accum = float3(0.0);
        const float kMP = 0.0022436f;
        const float kSC = 50.0 / (1.0 - kMP);
        for (int i = 0; i < nl_dbg; ++i) {
            float3 delta = p.plight_pos[i].xyz - W;
            float  dist  = length(delta);
            float  rad   = p.plight_pos[i].w;
            if (rad > 0.0 && dist < rad) {
                float3 Lp    = delta / max(dist, 1e-5);
                float  pterm = mix(1.0, max(dot(N, Lp), 0.0), clamp(p.normal_lighting.x, 0.0, 1.0));
                float normd = dist * 254.0 / rad;
                float pw    = pow(normd + 1.0, -1.1) - kMP;
                float attn  = clamp(pw * kSC, 0.0, 1.0);
                accum += p.plight_col[i].rgb * p.plight_col[i].w * attn * pterm;
            }
        }
        return float4(accum, 1.0);
    }
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
    float3 light = p.ambient_col.rgb * p.light_col.w;
    int mode = int(p.settings.z);
    if (mode == 1) light *= ao;
    float3 Ldir = normalize(p.light_dir.xyz);
    float  raw_sun_ndotl = dot(N, Ldir);
    float  sun_ndotl = max(raw_sun_ndotl, 0.0);
    float  normal_hardness = clamp(p.normal_lighting.x, 0.0, 1.0);
    float  sun_term = mix(1.0, sun_ndotl, normal_hardness);
    // Direct sun visibility is a composition of two intentionally separate
    // facts: normal-facing geometry and the blurred cast-shadow mask. The
    // lighting pass never ray-marches; the mask is produced once per frame by
    // the shadow pass, then blurred as an image operation.
    float normal_visibility = (raw_sun_ndotl > 0.0) ? 1.0 : 0.0;
    float cast_shadow = (mode == 1 && p.shadow.w > 0.5) ? shadow_tex.sample(smp, in.uv).r : 1.0;
    float sun_shadow = normal_visibility * cast_shadow;
    if (mode == 1) {
        light += p.light_col.rgb * p.light_dir.w * sun_term * sun_shadow;
    }
    if (vm == 6) return float4(float3(sun_shadow), 1.0);
    const float kMinPower = 0.0022436f;
    const float kScale    = 50.0 / (1.0 - kMinPower);
    int nl = int(p.settings.y);
    for (int i = 0; i < KPL; ++i) {
        if (i >= nl) break;
        float3 delta = p.plight_pos[i].xyz - W;
        float  dist  = length(delta);
        float  rad   = p.plight_pos[i].w;
        if (rad > 0.0 && dist < rad) {
            float3 Lp    = delta / max(dist, 1e-5);
            float  pterm = mix(1.0, max(dot(N, Lp), 0.0), normal_hardness);
            float  normd = dist * 254.0 / rad;
            float  pw    = pow(normd + 1.0, -1.1) - kMinPower;
            float  attn  = clamp(pw * kScale, 0.0, 1.0);
            light += p.plight_col[i].rgb * p.plight_col[i].w * attn * pterm;
        }
    }
  // Brightness cap (see light.glsl.h for rationale).
    float light_ceiling = max(p.ambient_col.w, 1e-3f);
    light = min(light, float3(light_ceiling));
    float3 col = alb.rgb * light;

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
