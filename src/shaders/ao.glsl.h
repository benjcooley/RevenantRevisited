// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *          ao.glsl.h  - GLSL SSAO shader (GL 3.3 core variant)          *
// *************************************************************************
//
// Linux / SOKOL_GLCORE33 port of ao.metal.h.
//
// *************************************************************************

#pragma once

inline constexpr const char* kAOVsGlsl = R"GLSL(
#version 330 core
layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 uv;
out vec2 v_uv;
void main() {
    gl_Position = vec4(pos * 2.0 - 1.0, 0.0, 1.0);
    v_uv = uv;
}
)GLSL";

inline constexpr const char* kAOFsGlsl = R"GLSL(
#version 330 core
#define ISO_COS30 0.867
#define ISO_WZ_DENOM 2.003378
layout(std140) uniform params {
    vec4 vp;
    vec4 recon;
    vec4 ao_settings;
    vec4 camera;
};
in vec2 v_uv;
uniform sampler2D albedo_tex;
uniform sampler2D normal_tex;
uniform sampler2D depth_tex;
layout(location = 0) out float out_ao;
vec3 reconstruct_world(vec2 uv, float d, float fbw, float fbh) {
    // Framebuffer pixels are scaled by the active camera zoom/resolution.
    // Undo that first so the iso inverse sees the same units as WorldToScreen.
    float zoom = max(camera.x, 0.0001);
    float S = (uv.x * fbw - vp.x) / zoom;
    float T = (uv.y * fbh - vp.y) / zoom;
    float scene_z = d * vp.w + vp.z;
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
void main() {
    vec4 alb = texture(albedo_tex, v_uv);
    if (alb.a < 0.01) { out_ao = 1.0; return; }
    float strength = max(ao_settings.y, 0.0);
    if (strength <= 0.0) { out_ao = 1.0; return; }
    float radius_px = max(ao_settings.x, 1.0);
    float bias = ao_settings.z;
    float max_dist = max(ao_settings.w, 1.0);
    float d = texture(depth_tex, v_uv).r;
    vec3  N = normalize(texture(normal_tex, v_uv).xyz * 2.0 - 1.0);
    vec2  ts = vec2(textureSize(albedo_tex, 0));
    float fbw = ts.x, fbh = ts.y;
    vec3  W = reconstruct_world(v_uv, d, fbw, fbh);
    float h = fract(sin(dot(v_uv, vec2(12.9898, 78.233))) * 43758.5453);
    const int kSamples = 8;
    float occ = 0.0;
    float wsum = 0.0;
    for (int i = 0; i < kSamples; ++i) {
        float a = h * 6.2831853 + float(i) * (6.2831853 / float(kSamples));
        float ring = radius_px * (0.35 + 0.65 * fract(h + float(i) * 0.6180339));
        vec2 suv = v_uv + vec2(cos(a), sin(a)) * ring / vec2(fbw, fbh);
        if (suv.x < 0.0 || suv.x > 1.0 || suv.y < 0.0 || suv.y > 1.0) continue;
        vec4 as = texture(albedo_tex, suv);
        if (as.a < 0.01) continue;
        float ds = texture(depth_tex, suv).r;
        vec3 Ws = reconstruct_world(suv, ds, fbw, fbh);
        vec3 V = Ws - W;
        float dist = length(V);
        if (dist < 1e-4 || dist > max_dist) continue;
        float hemi = pow(max(dot(N, normalize(V)) - bias, 0.0), 1.35);
        float attn = 1.0 - dist / max_dist;
        attn *= attn;
        occ += hemi * attn;
        wsum += attn;
    }
    if (wsum <= 1e-5) { out_ao = 1.0; return; }
    float occ_n = clamp(occ / wsum, 0.0, 1.0);
    occ_n = pow(occ_n, 0.75);
    out_ao = clamp(1.0 - strength * occ_n, 0.0, 1.0);
}
)GLSL";
