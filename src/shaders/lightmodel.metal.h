// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *     lightmodel.metal.h  - MSL lighting model shared by lit passes     *
// *************************************************************************
//
// The light uniform block and the two lighting models (settings.z):
//   0 Classic -- the retail lighting model. Static tiles use the DLS light
//     table in its MMX form: pow-curve falloff in the retail iso screen
//     metric, 6-bit saturating intensity, no N.L, integer multiplier bytes
//     on 5-bit colour. Meshes use T3DScene's
//     vertex lighting: split ambient + key light, linear per-light
//     brightness, N.L. docs/LIGHTING_FIDELITY.md.
//   1 modern -- ambient * AO, directional sun with the blurred sun-shadow
//     mask, world-space pow falloff with tunable N.L.
// Point lights 0..point_model.x-1 are authored map lights (retail radius,
// colour, multiplier); the rest are direct (VFX) lights, the same in both
// models.
//
// shade_surface() lights one surface sample from its world position and
// normal. TRenderer prepends this chunk to the two passes that light
// surfaces: the deferred light pass (light.metal.h), which shades G-buffer
// samples, and the translucent mesh pass (mesh.metal.h), which shades mesh
// fragments directly. Both call the same function, so a fading character
// is lit exactly like an opaque one. docs/RENDERER_ARCHITECTURE.md.
//
// NOTE: KPL must match TRenderer::kMaxPointLights in renderer.h. If you
// change one, change the other. The params layout must match
// TRenderer::PackLightUniforms.
//
// *************************************************************************

#pragma once

inline constexpr const char* kLightModelMetal = R"MSL(
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
    float4 point_model;       // .x retail light count, .y modern gain/multiplier, .z modern range scale, .w Classic map lights on meshes
};
constant int CM_TILE = 0, CM_MESH_AMB = 1, CM_MESH_DIR = 2, CM_MESH_KEY = 3;
constant float kMinPower   = 0.0022436;                  // pow(256, -1.1)
constant float kBrightScale = 50.0 / (1.0 - 0.0022436);  // maxbrightness / (1 - minpower)
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
// Classic tiles: DLS through retail's MMX transfer. Each authored light
// adds int(63*B) to a 6-bit intensity I that saturates at 63. The light
// byte picks per-channel multiplier bytes M = int(min(255, ambient byte +
// light term)), and each 5-bit colour channel becomes min(255, c5*M) >> 3;
// green then doubles into the 6-bit field. CM_TILE carries M/8 gains.
// Retail keeps one colour per pixel (the light drawn last); the I-weighted
// average is the same for one light or same-coloured lights.
// light_gain receives M/8 for the diagnostic view.
static float3 classic_tile_lit(float3 alb, float3 W, int n_retail, constant params& p,
                               thread float3& light_gain) {
    float  sum_i = 0.0;
    float3 sum_c = float3(0.0);
    for (int i = 0; i < KPL; ++i) {
        if (i >= n_retail) break;
        float r = p.plight_pos[i].w;
        if (r <= 0.0) continue;
        float d = retail_iso_distance(p.plight_pos[i].xyz - W);
        if (d >= r) continue;
        float i6 = floor(63.0 * retail_brightness(d * 254.0 / r));
        sum_i += i6;
        sum_c += i6 * max_normalized(p.plight_col[i].rgb) * p.plight_col[i].w;
    }
    float3 color_mult = (sum_i > 0.0) ? sum_c / sum_i : float3(0.0);
    float3 m  = floor(min(8.0 * p.classic_model[CM_TILE].rgb +
                          8.0 * p.classic_model[CM_TILE].w * (min(sum_i, 63.0) / 63.0) * color_mult,
                          255.0));
    float3 c5 = floor(floor(alb * 255.0 + 0.5) / 8.0);
    float3 o5 = floor(min(c5 * m, 255.0) / 8.0);
    light_gain = m / 8.0;
    return float3(o5.r / 31.0, 2.0 * o5.g / 63.0, o5.b / 31.0);
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
// One lit surface sample. `points` and `sun_shadow` are kept for the light
// pass's diagnostic view modes.
struct surface_light { float3 lit; float3 points; float sun_shadow; };
// Lights albedo `alb` at world position W with world normal N. is_mesh picks
// the Classic surface class (tile or mesh). ao and cast_shadow are the
// screen-space terms of the modern model; 1 where a pass has none.
static surface_light shade_surface(float3 alb, float3 W, float3 N, bool is_mesh,
                                   float ao, float cast_shadow, constant params& p) {
    int    mode     = int(p.settings.z);
    int    nl       = min(int(p.settings.y), KPL);
    int    n_retail = min(int(p.point_model.x), nl);
    float  normal_hardness = clamp(p.normal_lighting.x, 0.0, 1.0);
    float3 base    = float3(0.0);   // ambient + directional
    float3 points  = float3(0.0);   // point lights (a Classic tile: its table gain)
    float  ceiling = 1.0;           // per-channel cap on base + points
    float  sun_shadow = 1.0;
    bool   table_lit = false;       // a Classic tile: lit by the light table
    float3 table_out = float3(0.0);
    if (mode == 0) {
        if (is_mesh) {
            base    = p.classic_model[CM_MESH_AMB].rgb
                    + p.classic_model[CM_MESH_DIR].rgb * max(dot(N, p.classic_model[CM_MESH_KEY].xyz), 0.0);
            if (p.point_model.w > 0.5)    // map lights reach meshes only with RealTimeLight
                points = classic_mesh_points(W, N, n_retail, p);
            ceiling = max(p.classic_model[CM_MESH_KEY].w, 1.0);
        } else {
            table_lit = true;
            table_out = classic_tile_lit(alb, W, n_retail, p, points);
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
    float3 direct = float3(0.0);
    for (int i = 0; i < KPL; ++i) {
        if (i >= nl) break;
        if (i < n_retail) continue;
        direct += pow_point_light(W, N, normal_hardness, p.plight_pos[i].xyz, p.plight_pos[i].w,
                                  p.plight_col[i].rgb, p.plight_col[i].w);
    }
    surface_light s;
    if (table_lit) {
        s.lit = clamp(table_out + alb * direct, 0.0, 1.0);
    } else {
        points += direct;
        s.lit = clamp(alb * min(base + points, float3(ceiling)), 0.0, 1.0);
    }
    s.points     = points;
    s.sun_shadow = sun_shadow;
    return s;
}
)MSL";
