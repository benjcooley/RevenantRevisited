// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *     light.glsl.h  - GLSL deferred lighting (GL 3.3 core variant)      *
// *************************************************************************
//
// Linux / SOKOL_GLCORE33 port of light.metal.h. TRenderer prepends
// lightmodel.glsl.h (the #version line, the params block and
// shade_surface) to kLightFsGlsl. Lighting consumes the precomputed
// sun-shadow mask; it does not ray-march.
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
float sample_scene_depth(vec2 uv) {
    ivec2 ts = textureSize(depth_tex, 0);
    ivec2 p = clamp(ivec2(uv * vec2(ts)), ivec2(0), ts - ivec2(1));
    return texelFetch(depth_tex, p, 0).r;
}
void main() {
    vec4 alb = texture(albedo_tex, v_uv);
    if (alb.a < 0.01) discard;
    float d   = sample_scene_depth(v_uv);
    vec4  nrm = texture(normal_tex, v_uv);
    vec3  np  = nrm.xyz;
    vec3  N   = normalize(np * 2.0 - 1.0);
    bool  is_mesh = (nrm.a < 0.5 || nrm.a > 1.5);     // G-buffer surface class: 1 tile, 0 mesh
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
    float cast_shadow = (shadow.w > 0.5) ? texture(shadow_tex, v_uv).r : 1.0;
    surface_light s = shade_surface(alb.rgb, W, N, is_mesh, ao, cast_shadow);
    // kObjFlagSelfLit (bit 0x20 of the id's top byte): imagery retail drew
    // after the light transfer keeps its own colours.
    {
        ivec2 its = textureSize(id_tex, 0);
        ivec2 ip  = clamp(ivec2(v_uv * vec2(its)), ivec2(0), its - 1);
        if ((uint(texelFetch(id_tex, ip, 0).a * 255.0 + 0.5) & 0x20u) != 0u)
            s.lit = alb.rgb;
    }
    // Marker2 is already vertex-lit RGB565 or source-unlit ARGB; Classic must not light it twice.
    if (int(settings.z) == 0 && nrm.a > 1.5) s.lit = alb.rgb;
    if (vm == 4) { frag_color = vec4(s.points, 1.0); return; }
    if (vm == 6) { frag_color = vec4(vec3(s.sun_shadow), 1.0); return; }
    frag_color = vec4(s.lit, 1.0);
}
)GLSL";
