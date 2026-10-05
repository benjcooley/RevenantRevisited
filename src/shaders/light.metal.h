// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *        light.metal.h  - MSL deferred lighting fullscreen pass         *
// *************************************************************************
//
// Reads the G-buffer (albedo + world normal + scene_z + AO) and lights it
// with one of two models (settings.z):
//   0 Classic -- the retail lighting model. Static tiles (normal.a = 1) use
//     the DLS light table: ambient gain, pow-curve falloff in the retail iso
//     screen metric, 6-bit saturating intensity, no N.L. Meshes
//     (normal.a = 0) use T3DScene's vertex lighting: split ambient + key
//     light, linear per-light brightness, N.L. docs/LIGHTING_FIDELITY.md.
//   1 modern -- ambient * AO, directional sun with the blurred sun-shadow
//     mask, world-space pow falloff with tunable N.L.
// Point lights 0..point_model.x-1 are authored map lights (retail radius,
// colour, multiplier); the rest are direct (VFX) lights, the same in both
// models.
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
    // Classic model, indexed by CM_*:
    //   CM_TILE      .rgb tile ambient gain, .w light gain per multiplier
    //   CM_MESH_AMB  .rgb mesh ambient, .w ambient intensity subtracted per light
    //   CM_MESH_DIR  .rgb key-light colour, .w light brightness per multiplier
    //   CM_MESH_KEY  .xyz key-light direction (to light), .w overbright ceiling
    float4 classic_model[4];
    float4 point_model;       // .x retail light count, .y modern gain/multiplier, .z modern range scale
};
struct vs_out { float4 pos [[position]]; float2 uv; };
constant int CM_TILE = 0, CM_MESH_AMB = 1, CM_MESH_DIR = 2, CM_MESH_KEY = 3;
constant float kMinPower   = 0.0022436;                  // pow(256, -1.1)
constant float kBrightScale = 50.0 / (1.0 - 0.0022436);  // maxbrightness / (1 - minpower)
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
// Retail BrightnessTable[normd] (colortable.cpp MakeColorTables).
static float retail_brightness(float normd) {
    return clamp((pow(normd + 1.0, -1.1) - kMinPower) * kBrightScale, 0.0, 1.0);
}
// Length of a world delta in the retail iso screen basis: screen x/y pixels
// plus 16-bit screen z (object.cpp WorldToScreen / WorldToScreenZ). The DLS
// light block measures light reach in this metric.
static float retail_iso_distance(float3 d) {
    float sx = d.x - d.y;
    float sy = 0.5 * (d.x + d.y) - ISO_COS30 * d.z;
    float sz = -0.5 * d.z - ISO_COS30 * (d.x + d.y);
    return sqrt(sx * sx + sy * sy + sz * sz);
}
// Retail light tables scale every colour so its largest channel is 1.
static float3 max_normalized(float3 c) {
    float m = max(max(c.r, c.g), c.b);
    return (m > 0.0) ? c / m : float3(0.0);
}
// Light i with the world-space pow falloff: direct lights in every model,
// and authored lights in the modern model (rad/intensity pre-scaled).
static float3 pow_point_light(float3 W, float3 N, float hardness, float3 pos, float rad,
                              float3 col, float intensity) {
    float3 delta = pos - W;
    float  dist  = length(delta);
    if (rad <= 0.0 || dist >= rad) return float3(0.0);
    float3 Lp    = delta / max(dist, 1e-5);
    float  pterm = mix(1.0, max(dot(N, Lp), 0.0), hardness);
    return col * intensity * retail_brightness(dist * 254.0 / rad) * pterm;
}
// Classic tiles: DLS. Each authored light adds int(63*B)/63 to a 6-bit
// intensity that saturates at 1; the light table turns it into
// gain * colour. Retail keeps one colour per pixel (the light drawn last);
// the B-weighted average is the same for one light or same-coloured lights.
static float3 classic_tile_points(float3 W, int n_retail, constant params& p) {
    float  sum_b = 0.0;
    float3 sum_c = float3(0.0);
    for (int i = 0; i < KPL; ++i) {
        if (i >= n_retail) break;
        float r = p.plight_pos[i].w;
        if (r <= 0.0) continue;
        float d = retail_iso_distance(p.plight_pos[i].xyz - W);
        if (d >= r) continue;
        float b6 = floor(63.0 * retail_brightness(d * 254.0 / r)) / 63.0;
        sum_b += b6;
        sum_c += b6 * max_normalized(p.plight_col[i].rgb) * p.plight_col[i].w;
    }
    if (sum_b <= 0.0) return float3(0.0);
    return sum_c / sum_b * min(sum_b, 1.0) * p.classic_model[CM_TILE].w;
}
// Classic meshes: T3DLight::GetBrightness (linear, saturating) per light,
// applied as a D3D point light with N.L. Light inputs are divided by the
// overbright scale s and clamped to [0,1] the way the vertex colour is.
static float3 classic_mesh_points(float3 W, float3 N, int n_retail, constant params& p) {
    float  s   = max(p.classic_model[CM_MESH_KEY].w, 1.0);
    float3 sum = float3(0.0);
    for (int i = 0; i < KPL; ++i) {
        if (i >= n_retail) break;
        float r = p.plight_pos[i].w;
        if (r <= 0.0) continue;
        float3 delta = p.plight_pos[i].xyz - W;
        float  d     = length(delta);
        if (d >= r) continue;
        float raw = p.classic_model[CM_MESH_DIR].w * p.plight_col[i].w * (1.0 - d / r);
        float b   = s * clamp(raw / s - p.classic_model[CM_MESH_AMB].w, 0.0, 1.0);
        sum += max_normalized(p.plight_col[i].rgb) * b * max(dot(N, delta / max(d, 1e-5)), 0.0);
    }
    return sum;
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
    int    mode     = int(p.settings.z);
    int    nl       = min(int(p.settings.y), KPL);
    int    n_retail = min(int(p.point_model.x), nl);
    float  normal_hardness = clamp(p.normal_lighting.x, 0.0, 1.0);
    float3 base;          // ambient + directional
    float3 points;        // point lights
    float  ceiling;       // per-channel cap on base + points
    float  sun_shadow = 1.0;
    if (mode == 0) {
        if (is_mesh) {
            base    = p.classic_model[CM_MESH_AMB].rgb
                    + p.classic_model[CM_MESH_DIR].rgb * max(dot(N, p.classic_model[CM_MESH_KEY].xyz), 0.0);
            points  = classic_mesh_points(W, N, n_retail, p);
            ceiling = max(p.classic_model[CM_MESH_KEY].w, 1.0);
        } else {
            base    = p.classic_model[CM_TILE].rgb;
            points  = classic_tile_points(W, n_retail, p);
            ceiling = 1.0e4;    // the light table has no gain cap; the output saturates
        }
    } else {
        base = p.ambient_col.rgb * p.light_col.w * ao;
        float3 Ldir = normalize(p.light_dir.xyz);
        float  raw_sun_ndotl = dot(N, Ldir);
        float  sun_term = mix(1.0, max(raw_sun_ndotl, 0.0), normal_hardness);
        // Direct sun visibility is a composition of two intentionally separate
        // facts: normal-facing geometry and the blurred cast-shadow mask. The
        // lighting pass never ray-marches; the mask is produced once per frame by
        // the shadow pass, then blurred as an image operation.
        float normal_visibility = (raw_sun_ndotl > 0.0) ? 1.0 : 0.0;
        float cast_shadow = (p.shadow.w > 0.5) ? shadow_tex.sample(smp, in.uv).r : 1.0;
        sun_shadow = normal_visibility * cast_shadow;
        base += p.light_col.rgb * p.light_dir.w * sun_term * sun_shadow;
        points = float3(0.0);
        for (int i = 0; i < KPL; ++i) {
            if (i >= n_retail) break;
            points += pow_point_light(W, N, normal_hardness, p.plight_pos[i].xyz,
                                      p.plight_pos[i].w * p.point_model.z,
                                      p.plight_col[i].rgb, p.plight_col[i].w * p.point_model.y);
        }
        ceiling = max(p.ambient_col.w, 1e-3);
    }
    for (int i = 0; i < KPL; ++i) {
        if (i >= nl) break;
        if (i < n_retail) continue;
        points += pow_point_light(W, N, normal_hardness, p.plight_pos[i].xyz, p.plight_pos[i].w,
                                  p.plight_col[i].rgb, p.plight_col[i].w);
    }
    if (vm == 4) return float4(points, 1.0);
    if (vm == 6) return float4(float3(sun_shadow), 1.0);
    float3 light = min(base + points, float3(ceiling));
    float3 col = clamp(alb.rgb * light, 0.0, 1.0);

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
