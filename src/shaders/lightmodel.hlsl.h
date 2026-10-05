// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *    lightmodel.hlsl.h  - HLSL lighting model shared by lit passes      *
// *************************************************************************
//
// D3D11 port of lightmodel.metal.h (see that header): the light cbuffer,
// the Classic and modern lighting models, and shade_surface(), shared by
// the deferred light pass and the translucent mesh pass. TRenderer
// prepends this chunk to their pixel shaders.
//
// NOTE: KPL must match TRenderer::kMaxPointLights in renderer.h. If you
// change one, change the other.
//
// *************************************************************************

#pragma once

inline constexpr const char* kLightModelHlsl = R"HLSL(
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
    float4 selected_obj_id;
    // Classic model, indexed by CM_*:
    //   CM_TILE      .rgb tile ambient gain, .w light gain per multiplier
    //   CM_MESH_AMB  .rgb mesh ambient, .w ambient intensity subtracted per light
    //   CM_MESH_DIR  .rgb key-light colour, .w light brightness per multiplier
    //   CM_MESH_KEY  .xyz key-light direction (to light), .w overbright ceiling
    float4 classic_model[4];
    float4 point_model;       // .x retail light count, .y modern gain/multiplier, .z modern range scale
};
static const int CM_TILE = 0, CM_MESH_AMB = 1, CM_MESH_DIR = 2, CM_MESH_KEY = 3;
static const float kMinPower    = 0.0022436;                  // pow(256, -1.1)
static const float kBrightScale = 50.0 / (1.0 - 0.0022436);   // maxbrightness / (1 - minpower)
// Retail BrightnessTable[normd] (colortable.cpp MakeColorTables).
float retail_brightness(float normd) {
    return saturate((pow(normd + 1.0, -1.1) - kMinPower) * kBrightScale);
}
// Length of a world delta in the retail iso screen basis: screen x/y pixels
// plus 16-bit screen z (object.cpp WorldToScreen / WorldToScreenZ). The DLS
// light block measures light reach in this metric.
float retail_iso_distance(float3 d) {
    float sx = d.x - d.y;
    float sy = 0.5 * (d.x + d.y) - ISO_COS30 * d.z;
    float sz = -0.5 * d.z - ISO_COS30 * (d.x + d.y);
    return sqrt(sx * sx + sy * sy + sz * sz);
}
// Retail light tables scale every colour so its largest channel is 1.
float3 max_normalized(float3 c) {
    float m = max(max(c.r, c.g), c.b);
    return (m > 0.0) ? c / m : float3(0.0, 0.0, 0.0);
}
// Light with the world-space pow falloff: direct lights in every model,
// and authored lights in the modern model (rad/intensity pre-scaled).
float3 pow_point_light(float3 W, float3 N, float hardness, float3 pos, float rad,
                       float3 col, float intensity) {
    float3 delta = pos - W;
    float  dist  = length(delta);
    if (rad <= 0.0 || dist >= rad) return float3(0.0, 0.0, 0.0);
    float3 Lp    = delta / max(dist, 1e-5);
    float  pterm = lerp(1.0, max(dot(N, Lp), 0.0), hardness);
    return col * intensity * retail_brightness(dist * 254.0 / rad) * pterm;
}
// Classic tiles: DLS. Each authored light adds int(63*B)/63 to a 6-bit
// intensity that saturates at 1; the light table turns it into
// gain * colour. Retail keeps one colour per pixel (the light drawn last);
// the B-weighted average is the same for one light or same-coloured lights.
float3 classic_tile_points(float3 W, int n_retail) {
    float  sum_b = 0.0;
    float3 sum_c = float3(0.0, 0.0, 0.0);
    [loop] for (int i = 0; i < KPL; ++i) {
        if (i >= n_retail) break;
        float r = plight_pos[i].w;
        if (r <= 0.0) continue;
        float d = retail_iso_distance(plight_pos[i].xyz - W);
        if (d >= r) continue;
        float b6 = floor(63.0 * retail_brightness(d * 254.0 / r)) / 63.0;
        sum_b += b6;
        sum_c += b6 * max_normalized(plight_col[i].rgb) * plight_col[i].w;
    }
    if (sum_b <= 0.0) return float3(0.0, 0.0, 0.0);
    return sum_c / sum_b * min(sum_b, 1.0) * classic_model[CM_TILE].w;
}
// Classic meshes: T3DLight::GetBrightness (linear, saturating) per light,
// applied as a D3D point light with N.L. Light inputs are divided by the
// overbright scale s and clamped to [0,1] the way the vertex colour is.
float3 classic_mesh_points(float3 W, float3 N, int n_retail) {
    float  s   = max(classic_model[CM_MESH_KEY].w, 1.0);
    float3 sum = float3(0.0, 0.0, 0.0);
    [loop] for (int i = 0; i < KPL; ++i) {
        if (i >= n_retail) break;
        float r = plight_pos[i].w;
        if (r <= 0.0) continue;
        float3 delta = plight_pos[i].xyz - W;
        float  d     = length(delta);
        if (d >= r) continue;
        float raw = classic_model[CM_MESH_DIR].w * plight_col[i].w * (1.0 - d / r);
        float b   = s * saturate(raw / s - classic_model[CM_MESH_AMB].w);
        sum += max_normalized(plight_col[i].rgb) * b * max(dot(N, delta / max(d, 1e-5)), 0.0);
    }
    return sum;
}
// One lit surface sample. `points` and `sun_shadow` are kept for the light
// pass's diagnostic view modes.
struct surface_light { float3 lit; float3 points; float sun_shadow; };
// Lights albedo `alb` at world position W with world normal N. is_mesh picks
// the Classic surface class (tile or mesh). ao and cast_shadow are the
// screen-space terms of the modern model; 1 where a pass has none.
surface_light shade_surface(float3 alb, float3 W, float3 N, bool is_mesh,
                            float ao, float cast_shadow) {
    int    mode     = (int)settings.z;
    int    nl       = min((int)settings.y, KPL);
    int    n_retail = min((int)point_model.x, nl);
    float  normal_hardness = saturate(normal_lighting.x);
    float3 base;          // ambient + directional
    float3 points;        // point lights
    float  ceiling;       // per-channel cap on base + points
    float  sun_shadow = 1.0;
    if (mode == 0) {
        if (is_mesh) {
            base    = classic_model[CM_MESH_AMB].rgb
                    + classic_model[CM_MESH_DIR].rgb * max(dot(N, classic_model[CM_MESH_KEY].xyz), 0.0);
            points  = classic_mesh_points(W, N, n_retail);
            ceiling = max(classic_model[CM_MESH_KEY].w, 1.0);
        } else {
            base    = classic_model[CM_TILE].rgb;
            points  = classic_tile_points(W, n_retail);
            ceiling = 1.0e4;    // the light table has no gain cap; the output saturates
        }
    } else {
        base = ambient_col.rgb * light_col.w * ao;
        float3 Ldir = normalize(light_dir.xyz);
        float  raw_sun_ndotl = dot(N, Ldir);
        float  sun_term = lerp(1.0, max(raw_sun_ndotl, 0.0), normal_hardness);
        // Direct sun visibility is a composition of two intentionally separate
        // facts: normal-facing geometry and the blurred cast-shadow mask. The
        // lighting pass never ray-marches; the mask is produced once per frame by
        // the shadow pass, then blurred as an image operation.
        float normal_visibility = (raw_sun_ndotl > 0.0) ? 1.0 : 0.0;
        sun_shadow = normal_visibility * cast_shadow;
        base += light_col.rgb * light_dir.w * sun_term * sun_shadow;
        points = float3(0.0, 0.0, 0.0);
        [loop] for (int i = 0; i < KPL; ++i) {
            if (i >= n_retail) break;
            points += pow_point_light(W, N, normal_hardness, plight_pos[i].xyz,
                                      plight_pos[i].w * point_model.z,
                                      plight_col[i].rgb, plight_col[i].w * point_model.y);
        }
        ceiling = max(ambient_col.w, 1e-3);
    }
    [loop] for (int j = 0; j < KPL; ++j) {
        if (j >= nl) break;
        if (j < n_retail) continue;
        points += pow_point_light(W, N, normal_hardness, plight_pos[j].xyz, plight_pos[j].w,
                                  plight_col[j].rgb, plight_col[j].w);
    }
    surface_light s;
    s.lit        = saturate(alb * min(base + points, float3(ceiling, ceiling, ceiling)));
    s.points     = points;
    s.sun_shadow = sun_shadow;
    return s;
}
)HLSL";
