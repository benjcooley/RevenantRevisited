// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *           mesh.glsl.h  - GLSL 330 mesh G-buffer fill shaders          *
// *************************************************************************
//
// GL core port of mesh.metal.h.  See that header for layout + projection
// math documentation.
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
layout(std140) uniform mesh_params {
    vec4 vp;
    vec4 camz;
    vec4 camw;
};
out vec3  v_wpos;
out vec3  v_wnormal;
out vec2  v_uv;
out vec4  v_tint;
out float v_scene_z;
void main() {
    // w0..w3 are rows of a 4x4 world matrix (backend-neutral layout).
    vec4 ph = vec4(pos, 1.0);
    vec3 wp = vec3(dot(w0, ph), dot(w1, ph), dot(w2, ph));
    vec3 wn = normalize(vec3(dot(w0.xyz, normal),
                             dot(w1.xyz, normal),
                             dot(w2.xyz, normal)));

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
    v_uv      = uv;
    v_tint    = tint;
    v_scene_z = scene_z_n;
}
)GLSL";

inline constexpr const char* kMeshFsGlsl = R"GLSL(
#version 330
in vec3  v_wpos;
in vec3  v_wnormal;
in vec2  v_uv;
in vec4  v_tint;
in float v_scene_z;
uniform sampler2D albedo_tex;
layout(location = 0) out vec4 o_albedo;
layout(location = 1) out vec4 o_normal;
layout(location = 2) out vec4 o_scene_z;
layout(location = 3) out vec4 o_obj_id;
void main() {
    vec4 c = texture(albedo_tex, v_uv) * v_tint;
    if (c.a < 0.01) discard;
    vec3 N = normalize(v_wnormal);
    o_albedo  = c;
    o_normal  = vec4(N * 0.5 + 0.5, 1.0);
    o_scene_z = vec4(v_scene_z, 0.0, 0.0, 1.0);
    o_obj_id  = vec4(0.0, 0.0, 0.0, 0.0);   // Phase 1: empty id
}
)GLSL";
