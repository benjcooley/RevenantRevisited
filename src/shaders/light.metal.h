// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *        light.metal.h  - MSL deferred lighting fullscreen pass         *
// *************************************************************************
//
// Reads the G-buffer (albedo + world normal + scene_z + AO) and runs:
//   * directional sun (N.L, toggled by normal_lighting_hardness)
//   * screen-space sun-shadow ray march (mode 1 only)
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
    float S = uv.x * fbw - p.vp.x;
    float T = uv.y * fbh - p.vp.y;
    if (p.recon.w > 0.5) {
        float focal_zoom = max(p.recon.z * max(p.settings.w, 0.0001), 1.0);
        S = (S / focal_zoom) * scene_z;
        T = (T / focal_zoom) * scene_z;
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
    if (p.recon.w > 0.5) {
        float scene_z = p.recon.z - ISO_COS30 * (dx + dy) - 0.5 * W.z;
        float focal_zoom = max(p.recon.z * max(p.settings.w, 0.0001), 1.0);
        float inv_z = focal_zoom / max(scene_z, 1.0);
        S *= inv_z;
        T *= inv_z;
    }
    return float2((S + p.vp.x) / fbw, (T + p.vp.y) / fbh);
}
fragment float4 _main(vs_out in [[stage_in]],
                      texture2d<float> albedo_tex [[texture(0)]],
                      texture2d<float> normal_tex [[texture(1)]],
                      texture2d<float> depth_tex  [[texture(2)]],
                      texture2d<float> ao_tex     [[texture(3)]],
                      texture2d<float> id_tex     [[texture(4)]],
                      sampler smp                [[sampler(0)]],
                      constant params& p         [[buffer(0)]]) {
    float4 alb = albedo_tex.sample(smp, in.uv);
    if (alb.a < 0.01) discard_fragment();
    float  d   = depth_tex.sample(smp, in.uv).r;
    float3 np  = normal_tex.sample(smp, in.uv).xyz;
    float3 N   = normalize(np * 2.0 - 1.0);
    float  ao  = ao_tex.sample(smp, in.uv).r;
    float  fbw = float(albedo_tex.get_width());
    float  fbh = float(albedo_tex.get_height());
    float3 W = reconstruct_world(in.uv, d, p, fbw, fbh);
    int vm = int(p.settings.x);
    if (vm == 1) return float4(alb.rgb, 1.0);
    if (vm == 2) { float vd = clamp(1.0 - d, 0.0, 1.0); return float4(vd, vd, vd, 1.0); }
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
    float3 light = p.ambient_col.rgb * p.light_col.w;
    int mode = int(p.settings.z);
    if (mode == 1) light *= ao;
    float sun_shadow = 1.0;
    float3 Ldir = normalize(p.light_dir.xyz);
    float  sun_ndotl = max(dot(N, Ldir), 0.0);
    float  normal_hardness = clamp(p.normal_lighting.x, 0.0, 1.0);
    float  sun_term = mix(1.0, sun_ndotl, normal_hardness);
    if (mode == 1 && p.shadow.w > 0.5 && sun_term > 0.0) {
        float3 Sdir = normalize(p.shadow_world_dir.xyz);
        if (length(Sdir) < 1e-5) Sdir = Ldir;
        float3 Rdir = normalize(float3(-Sdir.x, -Sdir.y, Sdir.z * p.shadow_dir.z));
        float step_wu  = p.shadow.x;
        float soft_px  = p.shadow.y;
        int   maxSteps = int(p.shadow.z);
        const float kBiasWu = 2.0;
        float3 rayStepW = W + Rdir * step_wu;
        float2 ray_uv = project_world_uv(float3(rayStepW.xy, 0.0), p, fbw, fbh) - in.uv;
        float2 perp_uv = (length(ray_uv) > 1e-6) ? normalize(float2(-ray_uv.y, ray_uv.x)) : float2(0.0, 1.0);
        float h = fract(sin(dot(in.uv, float2(12.9898, 78.233))) * 43758.5453);
        const int kRays = 4;
        float hits = 0.0;
        for (int r = 0; r < kRays; ++r) {
            float rf = (float(r) + h) / float(kRays);
            float off_px = (rf - 0.5) * 2.0 * soft_px;
            float2 perp_off = perp_uv * off_px / float2(fbw, fbh);
            float rh = fract(h + float(r) * 0.6180339);
            bool hit = false;
            for (int i = 1; i <= maxSteps; ++i) {
                float t = (float(i) - 0.5 + rh) * step_wu;
                float3 rayW = W + Rdir * t;
                float2 suv = project_world_uv(float3(rayW.xy, 0.0), p, fbw, fbh) + perp_off +
                             float2(p.shadow_dir.x * t / fbw, p.shadow_dir.y * t / fbh);
                if (suv.x < 0.0 || suv.x > 1.0 ||
                    suv.y < 0.0 || suv.y > 1.0) break;
                float4 albs = albedo_tex.sample(smp, suv);
                if (albs.a < 0.01) continue;
                float drs = depth_tex.sample(smp, suv).r;
                float3 Ws = reconstruct_world(suv, drs, p, fbw, fbh);
                if (Ws.z > rayW.z + kBiasWu) { hit = true; break; }
            }
            if (hit) hits += 1.0;
        }
        float occ = hits / float(kRays);
        sun_shadow = 1.0 - occ * 0.8;
    }
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
