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
};
in vec2 v_uv;
uniform sampler2D albedo_tex;
uniform sampler2D normal_tex;
uniform sampler2D depth_tex;
uniform sampler2D ao_tex;
uniform sampler2D id_tex;
uniform sampler2D shadow_tex;
out vec4 frag_color;
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
vec2 project_world_uv(vec3 W, float fbw, float fbh) {
    float dx = W.x - recon.x;
    float dy = W.y - recon.y;
    float S = dx - dy;
    float T = 0.5 * (dx + dy) - W.z * ISO_COS30;
    float scale = max(settings.w, 0.0001);
    if (recon.w > 0.5) {
        float scene_z = recon.z - ISO_COS30 * (dx + dy) - 0.5 * W.z;
        scale *= recon.z / max(scene_z, 1.0);
    }
    S *= scale;
    T *= scale;
    return vec2((S + vp.x) / fbw, (T + vp.y) / fbh);
}
float scene_depth_world(vec3 W) {
    float dx = W.x - recon.x;
    float dy = W.y - recon.y;
    return recon.z - ISO_COS30 * (dx + dy) - 0.5 * W.z;
}
float interpolate_ray_depth(float z0, float z1, float a) {
    if (recon.w > 0.5) {
        float inv_z = mix(1.0 / max(z0, 1.0), 1.0 / max(z1, 1.0), a);
        return 1.0 / max(inv_z, 1e-6);
    }
    return mix(z0, z1, a);
}
float sample_scene_depth(vec2 uv) {
    ivec2 ts = textureSize(depth_tex, 0);
    ivec2 p = clamp(ivec2(uv * vec2(ts)), ivec2(0), ts - ivec2(1));
    return texelFetch(depth_tex, p, 0).r;
}
float shadow_tap_weight(int tap, int sampleCount) {
    if (sampleCount <= 1) return 1.0;
    float u = float(tap) / float(sampleCount - 1);
    return mix(0.5, 1.0, 1.0 - abs(u * 2.0 - 1.0));
}
void main() {
    vec4 alb = texture(albedo_tex, v_uv);
    if (alb.a < 0.01) discard;
    float d  = sample_scene_depth(v_uv);
    vec3  np = texture(normal_tex, v_uv).xyz;
    vec3  N  = normalize(np * 2.0 - 1.0);
    float ao = texture(ao_tex, v_uv).r;
    vec2  ts = vec2(textureSize(albedo_tex, 0));
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
    int nl_dbg = int(settings.y);
    if (vm == 4) {
        vec3 accum = vec3(0.0);
        const float kMP = 0.0022436;
        const float kSC = 50.0 / (1.0 - kMP);
        for (int i = 0; i < nl_dbg; ++i) {
            vec3  delta = plight_pos[i].xyz - W;
            float dist  = length(delta);
            float rad   = plight_pos[i].w;
            if (rad > 0.0 && dist < rad) {
                vec3  Lp    = delta / max(dist, 1e-5);
                float pterm = mix(1.0, max(dot(N, Lp), 0.0), clamp(normal_lighting.x, 0.0, 1.0));
                float normd = dist * 254.0 / rad;
                float pw    = pow(normd + 1.0, -1.1) - kMP;
                float attn  = clamp(pw * kSC, 0.0, 1.0);
                accum += plight_col[i].rgb * plight_col[i].w * attn * pterm;
            }
        }
        frag_color = vec4(accum, 1.0); return;
    }
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
    vec3 light = ambient_col.rgb * light_col.w;
    int mode = int(settings.z);
    if (mode == 1) light *= ao;
    vec3  Ldir = normalize(light_dir.xyz);
    float raw_sun_ndotl = dot(N, Ldir);
    float sun_ndotl = max(raw_sun_ndotl, 0.0);
    float normal_hardness = clamp(normal_lighting.x, 0.0, 1.0);
    float sun_term = mix(1.0, sun_ndotl, normal_hardness);
    // Direct sun visibility is a composition of two intentionally separate
    // facts: normal-facing geometry and the blurred cast-shadow mask. The
    // lighting pass never ray-marches; the mask is produced once per frame by
    // the shadow pass, then blurred as an image operation.
    float normal_visibility = (raw_sun_ndotl > 0.0) ? 1.0 : 0.0;
    float cast_shadow = (mode == 1 && shadow.w > 0.5) ? texture(shadow_tex, v_uv).r : 1.0;
    float sun_shadow = normal_visibility * cast_shadow;
    if (mode == 1) {
        light += light_col.rgb * light_dir.w * sun_term * sun_shadow;
    }
    if (vm == 6) { frag_color = vec4(vec3(sun_shadow), 1.0); return; }
    const float kMinPower = 0.0022436;
    const float kScale    = 50.0 / (1.0 - kMinPower);
    int nl = int(settings.y);
    for (int i = 0; i < KPL; ++i) {
        if (i >= nl) break;
        vec3  delta = plight_pos[i].xyz - W;
        float dist  = length(delta);
        float rad   = plight_pos[i].w;
        if (rad > 0.0 && dist < rad) {
            vec3  Lp    = delta / max(dist, 1e-5);
            float pterm = mix(1.0, max(dot(N, Lp), 0.0), normal_hardness);
            float normd = dist * 254.0 / rad;
            float pw    = pow(normd + 1.0, -1.1) - kMinPower;
            float attn  = clamp(pw * kScale, 0.0, 1.0);
            light += plight_col[i].rgb * plight_col[i].w * attn * pterm;
        }
    }
    frag_color = vec4(alb.rgb * light, 1.0);
}
)GLSL";
