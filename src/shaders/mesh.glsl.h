// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *           mesh.glsl.h  - GLSL 330 mesh G-buffer fill shaders          *
// *************************************************************************
//
// GL core port of mesh.metal.h.  See that header for layout + projection
// math documentation, and for the translucent pass's depth and colour
// shaders.
//
// *************************************************************************

#pragma once

inline constexpr const char* kMeshVsGlsl = R"GLSL(
#version 330
#define ISO_COS30 0.867
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec4 w0;
layout(location = 4) in vec4 w1;
layout(location = 5) in vec4 w2;
layout(location = 6) in vec4 w3;
layout(location = 7) in vec4 tint;
layout(location = 9) in vec2 uv_offset;
layout(location = 10) in float retail_mode;
layout(std140) uniform mesh_params {
    vec4 vp;
    vec4 camz;
    vec4 camw;
    vec4 retail_ambient;
    vec4 retail_directional;
};
out vec3  v_wpos;
out vec3  v_wnormal;
out vec2  v_uv;
out vec4  v_tint;
out float v_scene_z;
out vec3 v_retail_color;
out float v_retail_mode;
void main() {
    // w0..w3 are rows of a 4x4 world matrix (backend-neutral layout).
    vec4 ph = vec4(pos, 1.0);
    vec3 wp = vec3(dot(w0, ph), dot(w1, ph), dot(w2, ph));
    vec3 wn = normalize(vec3(dot(w0.xyz, normal),
                             dot(w1.xyz, normal),
                             dot(w2.xyz, normal)));

    vec3 source_light = retail_ambient.xyz + retail_directional.xyz *
        max(dot(wn, vec3(0.0, 0.78125, 0.625)), 0.0);
    vec3 retail_color = floor(clamp(source_light, 0.0, 1.0) * 31.0) / 31.0;
    if (camw.w > 0.5) {
        const float D3D_XY_SCALE = 1.4142135623730951;
        const float D3D_Z_SCALE  = 2.1798270608996972e-5;
        float spx = vp.x + wp.x * D3D_XY_SCALE;
        float spy = vp.y - wp.y * D3D_XY_SCALE;
        float scene_z_n = clamp(wp.z * D3D_Z_SCALE, 0.0, 1.0);

        gl_Position.x = 2.0 * spx / max(vp.z, 1.0) - 1.0;
        gl_Position.y = 1.0 - 2.0 * spy / max(vp.w, 1.0);
        gl_Position.z = scene_z_n;
        gl_Position.w = 1.0;
        v_wpos    = wp;
        v_wnormal = wn;
        v_uv      = uv + uv_offset;
        v_tint    = tint;
        v_scene_z = scene_z_n;
        v_retail_color = retail_color;
        v_retail_mode = retail_mode;
        return;
    }

    float wx = wp.x - camw.x;
    float wy = wp.y - camw.y;
    float wz = wp.z;
    float sum = wx + wy;
    float S   = wx - wy;
    float T   = 0.5 * sum - wz * ISO_COS30;

    float scene_z_wu = camz.z - ISO_COS30 * sum - 0.5 * wz;
    float scene_z_n  = (scene_z_wu - camz.x) / max(camz.y, 1e-6);
    float zoom = max(camw.z, 0.0001);
    float persp_scale = ((camz.w > 0.5) ? (camz.z / max(scene_z_wu, 1.0)) : 1.0) * zoom;
    float spx = vp.x + S * persp_scale;
    float spy = vp.y + T * persp_scale;

    gl_Position.x = 2.0 * spx / max(vp.z, 1.0) - 1.0;
    gl_Position.y = 1.0 - 2.0 * spy / max(vp.w, 1.0);
    gl_Position.z = scene_z_n;
    gl_Position.w = 1.0;
    v_wpos    = wp;
    v_wnormal = wn;
    v_uv      = uv + uv_offset;
    v_tint    = tint;
    v_scene_z = scene_z_n;
    v_retail_color = retail_color;
    v_retail_mode = retail_mode;
}
)GLSL";

inline constexpr const char* kMeshFsGlsl = R"GLSL(
#version 330
in vec3  v_wpos;
in vec3  v_wnormal;
in vec2  v_uv;
in vec4  v_tint;
in float v_scene_z;
in vec3 v_retail_color;
in float v_retail_mode;
uniform sampler2D albedo_tex;
layout(location = 0) out vec4 o_albedo;
layout(location = 1) out vec4 o_normal;
layout(location = 2) out vec4 o_scene_z;
layout(location = 3) out vec4 o_obj_id;
void main() {
    vec4 texel = texture(albedo_tex, v_uv);
    if (v_retail_mode > 0.5) {
        ivec2 size = textureSize(albedo_tex, 0);
        ivec2 xy = min(ivec2(floor(fract(v_uv) * vec2(size))), size - ivec2(1));
        texel = texelFetch(albedo_tex, xy, 0);
        if (v_retail_mode > 2.5) {
            // Native 56ca90 RGB565 table; 54fbf2 alpha nibble plus one.
            vec4 nibble = floor(texel * 15.0 + 0.5);
            texel = vec4(nibble.rgb * vec3(2.0 / 31.0, 4.0 / 63.0, 2.0 / 31.0),
                           (nibble.a + 1.0) / 16.0);
        }
    }
    vec4 c = texel * v_tint;
    if (v_retail_mode > 0.5 && v_retail_mode < 1.5) c.rgb *= v_retail_color;
    if (c.a < 0.01) discard;
    vec3 N = normalize(v_wnormal);
    o_albedo  = c;
    o_normal  = vec4(N * 0.5 + 0.5, v_retail_mode > 0.5 ? 2.0 : 0.0);
    o_scene_z = vec4(v_scene_z, 0.0, 0.0, 1.0);
    o_obj_id  = vec4(0.0, 0.0, 0.0, 0.0);   // Phase 1: empty id
}
)GLSL";

// Translucent pass, step 1: depth only (colour writes are masked off). Covers
// exactly the fragments step 2 shades.
inline constexpr const char* kMeshDepthFsGlsl = R"GLSL(
#version 330
in vec3  v_wpos;
in vec3  v_wnormal;
in vec2  v_uv;
in vec4  v_tint;
in float v_scene_z;
in vec3 v_retail_color;
in float v_retail_mode;
uniform sampler2D albedo_tex;
void main() {
    vec4 texel = texture(albedo_tex, v_uv);
    if (v_retail_mode > 0.5) {
        ivec2 size = textureSize(albedo_tex, 0);
        ivec2 xy = min(ivec2(floor(fract(v_uv) * vec2(size))), size - ivec2(1));
        texel = texelFetch(albedo_tex, xy, 0);
        if (v_retail_mode > 2.5) {
            // Native 56ca90 RGB565 table; 54fbf2 alpha nibble plus one.
            vec4 nibble = floor(texel * 15.0 + 0.5);
            texel = vec4(nibble.rgb * vec3(2.0 / 31.0, 4.0 / 63.0, 2.0 / 31.0),
                           (nibble.a + 1.0) / 16.0);
        }
    }
    if (texel.a * v_tint.a < 0.01) discard;
}
)GLSL";

// Translucent pass, step 2: lit colour at the surface's nearest depth,
// blended over lit_target. Appended to kLightModelGlsl. The surface isn't in
// the G-buffer, so it has no SSAO or sun cast shadow (both 1).
inline constexpr const char* kMeshTranslucentFsGlsl = R"GLSL(
in vec3  v_wpos;
in vec3  v_wnormal;
in vec2  v_uv;
in vec4  v_tint;
in float v_scene_z;
in vec3 v_retail_color;
in float v_retail_mode;
uniform sampler2D albedo_tex;
out vec4 frag_color;
void main() {
    vec4 texel = texture(albedo_tex, v_uv);
    if (v_retail_mode > 0.5) {
        ivec2 size = textureSize(albedo_tex, 0);
        ivec2 xy = min(ivec2(floor(fract(v_uv) * vec2(size))), size - ivec2(1));
        texel = texelFetch(albedo_tex, xy, 0);
        if (v_retail_mode > 2.5) {
            // Native 56ca90 RGB565 table; 54fbf2 alpha nibble plus one.
            vec4 nibble = floor(texel * 15.0 + 0.5);
            texel = vec4(nibble.rgb * vec3(2.0 / 31.0, 4.0 / 63.0, 2.0 / 31.0),
                           (nibble.a + 1.0) / 16.0);
        }
    }
    vec4 c = texel * v_tint;
    if (v_retail_mode > 0.5 && v_retail_mode < 1.5) c.rgb *= v_retail_color;
    if (c.a < 0.01) discard;
    vec3 N = normalize(v_wnormal);
    int vm = int(settings.x);
    if (vm == 1) { frag_color = vec4(c.rgb, c.a); return; }
    if (vm == 3) { frag_color = vec4(N * 0.5 + 0.5, c.a); return; }
    surface_light s = shade_surface(c.rgb, v_wpos, N, true, 1.0, 1.0);
    if (int(settings.z) == 0 && v_retail_mode > 0.5) s.lit = c.rgb;
    if (vm == 4) { frag_color = vec4(s.points, c.a); return; }
    if (vm == 6) { frag_color = vec4(vec3(s.sun_shadow), c.a); return; }
    frag_color = vec4(s.lit, c.a);
}
)GLSL";
