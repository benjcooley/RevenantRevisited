// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *    lightmodel.glsl.h  - GLSL lighting model shared by lit passes      *
// *************************************************************************
//
// GL 3.3 core port of lightmodel.metal.h (see that header): the light
// uniform block, the Classic and modern lighting models, and
// shade_surface(), shared by the deferred light pass and the translucent
// mesh pass. TRenderer prepends this chunk to their fragment shaders, so it
// carries the #version line.
//
// NOTE: KPL must match TRenderer::kMaxPointLights in renderer.h. If you
// change one, change the other.
//
// *************************************************************************

#pragma once

inline constexpr const char* kLightModelGlsl = R"GLSL(
#version 330 core
#define ISO_COS30 0.867
#define ISO_WZ_DENOM 2.003378
#define KPL 16
layout(std140) uniform params {
    vec4 vp;
    vec4 recon;
    vec4 light_dir;
    vec4 light_col;
    vec4 ambient_col;
    vec4 settings;
    vec4 plight_pos[KPL];
    vec4 plight_col[KPL];
    vec4 shadow;
    vec4 shadow_dir;
    vec4 shadow_world_dir;
    vec4 normal_lighting;
    vec4 selected_obj_id;
    // Classic model, indexed by CM_*:
    //   CM_TILE      .rgb tile ambient gain, .w light gain per multiplier
    //   CM_MESH_AMB  .rgb mesh ambient, .w ambient intensity subtracted per light
    //   CM_MESH_DIR  .rgb key-light colour, .w light brightness per multiplier
    //   CM_MESH_KEY  .xyz key-light direction (to light), .w overbright ceiling
    vec4 classic_model[4];
    vec4 point_model;       // .x retail light count, .y modern gain/multiplier, .z modern range scale, .w Classic map lights on meshes
};
const int CM_TILE = 0, CM_MESH_AMB = 1, CM_MESH_DIR = 2, CM_MESH_KEY = 3;
const float kMinPower    = 0.0022436;                  // pow(256, -1.1)
const float kBrightScale = 50.0 / (1.0 - 0.0022436);   // maxbrightness / (1 - minpower)
// Retail BrightnessTable[normd] (colortable.cpp MakeColorTables).
float retail_brightness(float normd) {
    return clamp((pow(normd + 1.0, -1.1) - kMinPower) * kBrightScale, 0.0, 1.0);
}
// Length of a world delta in the retail iso screen basis: screen x/y pixels
// plus 16-bit screen z (object.cpp WorldToScreen / WorldToScreenZ). The DLS
// light block measures light reach in this metric.
float retail_iso_distance(vec3 d) {
    float sx = d.x - d.y;
    float sy = 0.5 * (d.x + d.y) - ISO_COS30 * d.z;
    float sz = -0.5 * d.z - ISO_COS30 * (d.x + d.y);
    return sqrt(sx * sx + sy * sy + sz * sz);
}
// Retail light tables scale every colour so its largest channel is 1.
vec3 max_normalized(vec3 c) {
    float m = max(max(c.r, c.g), c.b);
    return (m > 0.0) ? c / m : vec3(0.0);
}
// Light with the world-space pow falloff: direct lights in every model,
// and authored lights in the modern model (rad/intensity pre-scaled).
vec3 pow_point_light(vec3 W, vec3 N, float hardness, vec3 pos, float rad,
                     vec3 col, float intensity) {
    vec3  delta = pos - W;
    float dist  = length(delta);
    if (rad <= 0.0 || dist >= rad) return vec3(0.0);
    vec3  Lp    = delta / max(dist, 1e-5);
    float pterm = mix(1.0, max(dot(N, Lp), 0.0), hardness);
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
vec3 classic_tile_lit(vec3 alb, vec3 W, int n_retail, out vec3 light_gain) {
    float sum_i = 0.0;
    vec3  sum_c = vec3(0.0);
    for (int i = 0; i < KPL; ++i) {
        if (i >= n_retail) break;
        float r = plight_pos[i].w;
        if (r <= 0.0) continue;
        float d = retail_iso_distance(plight_pos[i].xyz - W);
        if (d >= r) continue;
        float i6 = floor(63.0 * retail_brightness(d * 254.0 / r));
        sum_i += i6;
        sum_c += i6 * max_normalized(plight_col[i].rgb) * plight_col[i].w;
    }
    vec3 color_mult = (sum_i > 0.0) ? sum_c / sum_i : vec3(0.0);
    vec3 m  = floor(min(8.0 * classic_model[CM_TILE].rgb +
                        8.0 * classic_model[CM_TILE].w * (min(sum_i, 63.0) / 63.0) * color_mult,
                        vec3(255.0)));
    vec3 c5 = floor(floor(alb * 255.0 + 0.5) / 8.0);
    vec3 o5 = floor(min(c5 * m, vec3(255.0)) / 8.0);
    light_gain = m / 8.0;
    return vec3(o5.r / 31.0, 2.0 * o5.g / 63.0, o5.b / 31.0);
}
// Classic meshes: T3DLight::GetBrightness (linear, saturating) per light,
// applied as a D3D point light with N.L. Light inputs are divided by the
// overbright scale s and clamped to [0,1] the way the vertex colour is.
vec3 classic_mesh_points(vec3 W, vec3 N, int n_retail) {
    float s   = max(classic_model[CM_MESH_KEY].w, 1.0);
    vec3  sum = vec3(0.0);
    for (int i = 0; i < KPL; ++i) {
        if (i >= n_retail) break;
        float r = plight_pos[i].w;
        if (r <= 0.0) continue;
        vec3  delta = plight_pos[i].xyz - W;
        float d     = length(delta);
        if (d >= r) continue;
        float raw = classic_model[CM_MESH_DIR].w * plight_col[i].w * (1.0 - d / r);
        float b   = s * clamp(raw / s - classic_model[CM_MESH_AMB].w, 0.0, 1.0);
        sum += max_normalized(plight_col[i].rgb) * b * max(dot(N, delta / max(d, 1e-5)), 0.0);
    }
    return sum;
}
// One lit surface sample. `points` and `sun_shadow` are kept for the light
// pass's diagnostic view modes.
struct surface_light { vec3 lit; vec3 points; float sun_shadow; };
// Lights albedo `alb` at world position W with world normal N. is_mesh picks
// the Classic surface class (tile or mesh). ao and cast_shadow are the
// screen-space terms of the modern model; 1 where a pass has none.
surface_light shade_surface(vec3 alb, vec3 W, vec3 N, bool is_mesh,
                            float ao, float cast_shadow) {
    int   mode     = int(settings.z);
    int   nl       = min(int(settings.y), KPL);
    int   n_retail = min(int(point_model.x), nl);
    float normal_hardness = clamp(normal_lighting.x, 0.0, 1.0);
    vec3  base    = vec3(0.0);      // ambient + directional
    vec3  points  = vec3(0.0);      // point lights (a Classic tile: its table gain)
    float ceiling = 1.0;            // per-channel cap on base + points
    float sun_shadow = 1.0;
    bool  table_lit = false;        // a Classic tile: lit by the light table
    vec3  table_out = vec3(0.0);
    if (mode == 0) {
        if (is_mesh) {
            base    = classic_model[CM_MESH_AMB].rgb
                    + classic_model[CM_MESH_DIR].rgb * max(dot(N, classic_model[CM_MESH_KEY].xyz), 0.0);
            if (point_model.w > 0.5)      // map lights reach meshes only with RealTimeLight
                points = classic_mesh_points(W, N, n_retail);
            ceiling = max(classic_model[CM_MESH_KEY].w, 1.0);
        } else {
            table_lit = true;
            table_out = classic_tile_lit(alb, W, n_retail, points);
        }
    } else {
        base = ambient_col.rgb * light_col.w * ao;
        vec3  Ldir = normalize(light_dir.xyz);
        float raw_sun_ndotl = dot(N, Ldir);
        float sun_term = mix(1.0, max(raw_sun_ndotl, 0.0), normal_hardness);
        // Direct sun visibility is a composition of two intentionally separate
        // facts: normal-facing geometry and the blurred cast-shadow mask. The
        // lighting pass never ray-marches; the mask is produced once per frame by
        // the shadow pass, then blurred as an image operation.
        float normal_visibility = (raw_sun_ndotl > 0.0) ? 1.0 : 0.0;
        sun_shadow = normal_visibility * cast_shadow;
        base += light_col.rgb * light_dir.w * sun_term * sun_shadow;
        points = vec3(0.0);
        for (int i = 0; i < KPL; ++i) {
            if (i >= n_retail) break;
            points += pow_point_light(W, N, normal_hardness, plight_pos[i].xyz,
                                      plight_pos[i].w * point_model.z,
                                      plight_col[i].rgb, plight_col[i].w * point_model.y);
        }
        ceiling = max(ambient_col.w, 1e-3);
    }
    vec3 direct = vec3(0.0);
    for (int i = 0; i < KPL; ++i) {
        if (i >= nl) break;
        if (i < n_retail) continue;
        direct += pow_point_light(W, N, normal_hardness, plight_pos[i].xyz, plight_pos[i].w,
                                  plight_col[i].rgb, plight_col[i].w);
    }
    surface_light s;
    if (table_lit) {
        s.lit = clamp(table_out + alb * direct, 0.0, 1.0);
    } else {
        points += direct;
        s.lit = clamp(alb * min(base + points, vec3(ceiling)), 0.0, 1.0);
    }
    s.points     = points;
    s.sun_shadow = sun_shadow;
    return s;
}
)GLSL";
