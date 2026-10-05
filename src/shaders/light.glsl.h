// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *     light.glsl.h  - GLSL deferred lighting (GL 3.3 core variant)      *
// *************************************************************************
//
// Linux / SOKOL_GLCORE33 port of light.metal.h. Lighting consumes the
// precomputed sun-shadow mask; it does not ray-march.
//
// NOTE: KPL must match TRenderer::kMaxPointLights in renderer.h. If you
// change one, change the other.
//
// *************************************************************************

#pragma once

inline constexpr const char* kLightVsGlsl = R"GLSL(
#version 330 core
layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 uv;
out vec2 v_uv;
void main() {
    gl_Position = vec4(pos * 2.0 - 1.0, 0.0, 1.0);
    v_uv = uv;
}
)GLSL";

inline constexpr const char* kLightFsGlsl = R"GLSL(
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
    vec4 point_model;       // .x retail light count, .y modern gain/multiplier, .z modern range scale
};
in vec2 v_uv;
uniform sampler2D albedo_tex;
uniform sampler2D normal_tex;
uniform sampler2D depth_tex;
uniform sampler2D ao_tex;
uniform sampler2D id_tex;
uniform sampler2D shadow_tex;
out vec4 frag_color;
const int CM_TILE = 0, CM_MESH_AMB = 1, CM_MESH_DIR = 2, CM_MESH_KEY = 3;
const float kMinPower    = 0.0022436;                  // pow(256, -1.1)
const float kBrightScale = 50.0 / (1.0 - 0.0022436);   // maxbrightness / (1 - minpower)
vec3 reconstruct_world(vec2 uv, float d, float fbw, float fbh) {
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
    return vec3(recon.x + (sum_r + S) * 0.5,
                recon.y + (sum_r - S) * 0.5,
                wz);
}
float sample_scene_depth(vec2 uv) {
    ivec2 ts = textureSize(depth_tex, 0);
    ivec2 p = clamp(ivec2(uv * vec2(ts)), ivec2(0), ts - ivec2(1));
    return texelFetch(depth_tex, p, 0).r;
}
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
// Classic tiles: DLS. Each authored light adds int(63*B)/63 to a 6-bit
// intensity that saturates at 1; the light table turns it into
// gain * colour. Retail keeps one colour per pixel (the light drawn last);
// the B-weighted average is the same for one light or same-coloured lights.
vec3 classic_tile_points(vec3 W, int n_retail) {
    float sum_b = 0.0;
    vec3  sum_c = vec3(0.0);
    for (int i = 0; i < KPL; ++i) {
        if (i >= n_retail) break;
        float r = plight_pos[i].w;
        if (r <= 0.0) continue;
        float d = retail_iso_distance(plight_pos[i].xyz - W);
        if (d >= r) continue;
        float b6 = floor(63.0 * retail_brightness(d * 254.0 / r)) / 63.0;
        sum_b += b6;
        sum_c += b6 * max_normalized(plight_col[i].rgb) * plight_col[i].w;
    }
    if (sum_b <= 0.0) return vec3(0.0);
    return sum_c / sum_b * min(sum_b, 1.0) * classic_model[CM_TILE].w;
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
void main() {
    vec4 alb = texture(albedo_tex, v_uv);
    if (alb.a < 0.01) discard;
    float d   = sample_scene_depth(v_uv);
    vec4  nrm = texture(normal_tex, v_uv);
    vec3  np  = nrm.xyz;
    vec3  N   = normalize(np * 2.0 - 1.0);
    bool  is_mesh = nrm.a < 0.5;     // G-buffer surface class: 1 tile, 0 mesh
    float ao  = texture(ao_tex, v_uv).r;
    vec2  ts  = vec2(textureSize(albedo_tex, 0));
    float fbw = ts.x, fbh = ts.y;
    vec3  W = reconstruct_world(v_uv, d, fbw, fbh);
    int vm = int(settings.x);
    if (vm == 1) { frag_color = vec4(alb.rgb, 1.0); return; }
    if (vm == 2) {
        // Depth diagnostic, not presentation art. This is scene-z only,
        // shown as repeated world-unit bands so narrow ranges are visible.
        // No edge detection is mixed into this mode.
        float z_wu = d * vp.w + vp.z;
        float coarse = fract(z_wu / 1024.0);
        float fine = fract(z_wu / 128.0);
        frag_color = vec4(fine, coarse, 1.0 - coarse, 1.0);
        return;
    }
    if (vm == 3) { frag_color = vec4(np, 1.0); return; }
    if (vm == 5) {
        float ws = max(settings.z, 1.0);
        float zs = max(settings.w, 1.0);
        frag_color = vec4(fract(W.x / ws), fract(W.y / ws), fract(W.z / zs), 1.0);
        return;
    }
    if (vm == 7) { frag_color = vec4(ao, ao, ao, 1.0); return; }
    if (vm == 8) {
        // Scene-z discontinuity diagnostic. Red here is an explicit
        // derivative overlay, not raw depth.
        float z_wu = d * vp.w + vp.z;
        float edge = clamp((abs(dFdx(z_wu)) + abs(dFdy(z_wu))) / 48.0, 0.0, 1.0);
        frag_color = vec4(edge, edge * 0.05, 0.0, 1.0);
        return;
    }
    if (vm == 9) {
        // Ground-plane-relative height diagnostic. This is reconstructed
        // world Z (W.z): flat ground should be flat color, while walls and
        // raised surfaces should form coherent equal-height bands. Blue is
        // near/below ground, warm is higher; red bands are 128 wu intervals.
        float h = W.z;
        float norm_h = clamp((h + 256.0) / 2048.0, 0.0, 1.0);
        float band = (fract(abs(h) / 128.0) < 0.04) ? 1.0 : 0.0;
        frag_color = vec4(max(norm_h, band), norm_h * (1.0 - 0.5 * band), 1.0 - norm_h, 1.0);
        return;
    }
    int   mode     = int(settings.z);
    int   nl       = min(int(settings.y), KPL);
    int   n_retail = min(int(point_model.x), nl);
    float normal_hardness = clamp(normal_lighting.x, 0.0, 1.0);
    vec3  base;           // ambient + directional
    vec3  points;         // point lights
    float ceiling;        // per-channel cap on base + points
    float sun_shadow = 1.0;
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
        vec3  Ldir = normalize(light_dir.xyz);
        float raw_sun_ndotl = dot(N, Ldir);
        float sun_term = mix(1.0, max(raw_sun_ndotl, 0.0), normal_hardness);
        // Direct sun visibility is a composition of two intentionally separate
        // facts: normal-facing geometry and the blurred cast-shadow mask. The
        // lighting pass never ray-marches; the mask is produced once per frame by
        // the shadow pass, then blurred as an image operation.
        float normal_visibility = (raw_sun_ndotl > 0.0) ? 1.0 : 0.0;
        float cast_shadow = (shadow.w > 0.5) ? texture(shadow_tex, v_uv).r : 1.0;
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
    for (int i = 0; i < KPL; ++i) {
        if (i >= nl) break;
        if (i < n_retail) continue;
        points += pow_point_light(W, N, normal_hardness, plight_pos[i].xyz, plight_pos[i].w,
                                  plight_col[i].rgb, plight_col[i].w);
    }
    if (vm == 4) { frag_color = vec4(points, 1.0); return; }
    if (vm == 6) { frag_color = vec4(vec3(sun_shadow), 1.0); return; }
    vec3 light = min(base + points, vec3(ceiling));
    frag_color = vec4(clamp(alb.rgb * light, 0.0, 1.0), 1.0);
}
)GLSL";
