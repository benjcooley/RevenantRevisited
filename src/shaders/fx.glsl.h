// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *           fx.glsl.h  - GLSL 330 VFX billboard / particle / strip      *
// *************************************************************************
//
// GL port of fx.metal.h. See that header for documentation of the FX
// submission API, pipeline layout, and the LitFlat lighting branch.
// Phase 1 is Mac-first; this port has not been visually validated yet --
// shader source is included so the shader umbrella compiles on
// Linux/Win builds.
//
// *************************************************************************

#pragma once

inline constexpr const char* kFxBillboardVsGlsl = R"GLSL(
#version 330
layout(location = 0) in vec2 corner;
layout(location = 1) in vec3 world_pos;
layout(location = 2) in vec2 size_wu;
layout(location = 3) in vec4 uv_rect;
layout(location = 4) in vec4 color_rgba;
layout(location = 5) in float debug_mode;
layout(location = 6) in float light_mode;
layout(location = 7) in float orientation;
layout(std140) uniform fx_params {
    vec4 vp;
    vec4 camz;
    vec4 camw;
    vec4 sun_dir;
    vec4 sun_color;
    vec4 ambient_color;
};
out vec2  v_corner;
out vec4  v_uv_rect;
out vec4  v_color;
out vec3  v_lit;
out float v_debug;
void main() {
    int omode = int(orientation + 0.5);
    vec3 wp = world_pos;
    if (omode == 1) {
        wp.x += corner.x * size_wu.x;
        wp.y += corner.y * size_wu.y;
    }
    float wx = wp.x - camw.x;
    float wy = wp.y - camw.y;
    float wz = wp.z;
    float sum = wx + wy;
    float S   = wx - wy;
    float T   = 0.5 * sum - wz * 0.867;
    float scene_z_wu = camz.z - 0.867 * sum - 0.5 * wz;
    float scene_z_n  = (scene_z_wu - camz.x) / max(camz.y, 1e-6);
    float zoom = max(camw.z, 0.0001);
    float persp_scale = ((camz.w > 0.5) ? (camz.z / max(scene_z_wu, 1.0)) : 1.0) * zoom;
    float spx, spy;
    if (omode == 1) {
        spx = vp.x + S * persp_scale;
        spy = vp.y + T * persp_scale;
    } else {
        spx = vp.x + S * persp_scale + corner.x * size_wu.x * persp_scale;
        spy = vp.y + T * persp_scale - corner.y * size_wu.y * persp_scale;
    }
    gl_Position.x = 2.0 * spx / max(vp.z, 1.0) - 1.0;
    gl_Position.y = 1.0 - 2.0 * spy / max(vp.w, 1.0);
    gl_Position.z = scene_z_n;
    gl_Position.w = 1.0;
    int lmode = int(light_mode + 0.5);
    vec3 lit = vec3(1.0);
    if (lmode == 1) {
        lit = ambient_color.rgb + max(0.0, sun_dir.z) * sun_color.rgb;
    }
    v_corner  = corner;
    v_uv_rect = uv_rect;
    v_color   = color_rgba;
    v_lit     = lit;
    v_debug   = debug_mode;
}
)GLSL";

inline constexpr const char* kFxBillboardFsGlsl = R"GLSL(
#version 330
in vec2  v_corner;
in vec4  v_uv_rect;
in vec4  v_color;
in vec3  v_lit;
in float v_debug;
uniform sampler2D atlas;
out vec4 o_color;
void main() {
    int mode = int(v_debug + 0.5);
    vec2 uv01 = v_corner + vec2(0.5, 0.5);
    vec2 uv_n = vec2(v_uv_rect.x + uv01.x * v_uv_rect.z,
                     v_uv_rect.y + uv01.y * v_uv_rect.w);
    vec4 c;
    if      (mode == 1) c = v_color;
    else if (mode == 2) c = texture(atlas, uv01);
    else if (mode == 3) c = texture(atlas, uv_n);
    else                c = texture(atlas, uv_n) * v_color;
    c.rgb *= v_lit;
    if (c.a < 0.002) discard;
    o_color = c;
}
)GLSL";

inline constexpr const char* kFxParticleVsGlsl = R"GLSL(
#version 330
layout(location = 0) in vec2 corner;
layout(location = 1) in vec3 world_pos;
layout(location = 2) in vec2 size_wu;
layout(location = 3) in vec4 uv_rect;
layout(location = 4) in vec4 color_rgba;
layout(location = 5) in float debug_mode;
layout(location = 6) in float rotation_rad;
layout(location = 7) in float light_mode;
layout(location = 8) in float orientation;
layout(std140) uniform fx_params {
    vec4 vp;
    vec4 camz;
    vec4 camw;
    vec4 sun_dir;
    vec4 sun_color;
    vec4 ambient_color;
};
out vec2  v_corner;
out vec4  v_uv_rect;
out vec4  v_color;
out vec3  v_lit;
out float v_debug;
void main() {
    float ca = cos(rotation_rad), sa = sin(rotation_rad);
    vec2 rc = vec2(ca * corner.x - sa * corner.y, sa * corner.x + ca * corner.y);
    int omode = int(orientation + 0.5);
    vec3 wp = world_pos;
    if (omode == 1) {
        wp.x += rc.x * size_wu.x;
        wp.y += rc.y * size_wu.y;
    }
    float wx = wp.x - camw.x;
    float wy = wp.y - camw.y;
    float wz = wp.z;
    float sum = wx + wy;
    float S   = wx - wy;
    float T   = 0.5 * sum - wz * 0.867;
    float scene_z_wu = camz.z - 0.867 * sum - 0.5 * wz;
    float scene_z_n  = (scene_z_wu - camz.x) / max(camz.y, 1e-6);
    float zoom = max(camw.z, 0.0001);
    float persp_scale = ((camz.w > 0.5) ? (camz.z / max(scene_z_wu, 1.0)) : 1.0) * zoom;
    float spx, spy;
    if (omode == 1) {
        spx = vp.x + S * persp_scale;
        spy = vp.y + T * persp_scale;
    } else {
        spx = vp.x + S * persp_scale + rc.x * size_wu.x * persp_scale;
        spy = vp.y + T * persp_scale - rc.y * size_wu.y * persp_scale;
    }
    gl_Position.x = 2.0 * spx / max(vp.z, 1.0) - 1.0;
    gl_Position.y = 1.0 - 2.0 * spy / max(vp.w, 1.0);
    gl_Position.z = scene_z_n;
    gl_Position.w = 1.0;
    int lmode = int(light_mode + 0.5);
    vec3 lit = vec3(1.0);
    if (lmode == 1) {
        lit = ambient_color.rgb + max(0.0, sun_dir.z) * sun_color.rgb;
    }
    v_corner  = corner;
    v_uv_rect = uv_rect;
    v_color   = color_rgba;
    v_lit     = lit;
    v_debug   = debug_mode;
}
)GLSL";

inline constexpr const char* kFxParticleFsGlsl = kFxBillboardFsGlsl;

inline constexpr const char* kFxStripVsGlsl = R"GLSL(
#version 330
layout(location = 0) in vec3 world_pos;
layout(location = 1) in vec3 world_tan;
layout(location = 2) in float half_width;
layout(location = 3) in vec2 uv;
layout(location = 4) in vec4 color;
layout(location = 5) in float debug_mode;
layout(location = 6) in float light_mode;
layout(std140) uniform fx_params {
    vec4 vp;
    vec4 camz;
    vec4 camw;
    vec4 sun_dir;
    vec4 sun_color;
    vec4 ambient_color;
};
layout(std140) uniform fx_cam    { vec4 cam_pos; vec4 cam_fwd; };
out vec2  v_uv;
out vec4  v_color;
out vec3  v_lit;
out float v_debug;
void main() {
    vec3 vd = normalize(world_pos - cam_pos.xyz);
    vec3 t = world_tan;
    float tl = length(t);
    if (tl > 1e-4) t /= tl;
    vec3 side = cross(t, vd);
    float sl = length(side);
    if (sl > 1e-4) side /= sl;
    vec3 wp = world_pos + side * half_width;
    float wx = wp.x - camw.x;
    float wy = wp.y - camw.y;
    float wz = wp.z;
    float sum = wx + wy;
    float S   = wx - wy;
    float T   = 0.5 * sum - wz * 0.867;
    float scene_z_wu = camz.z - 0.867 * sum - 0.5 * wz;
    float scene_z_n  = (scene_z_wu - camz.x) / max(camz.y, 1e-6);
    float zoom = max(camw.z, 0.0001);
    float persp_scale = ((camz.w > 0.5) ? (camz.z / max(scene_z_wu, 1.0)) : 1.0) * zoom;
    float spx = vp.x + S * persp_scale;
    float spy = vp.y + T * persp_scale;
    gl_Position.x = 2.0 * spx / max(vp.z, 1.0) - 1.0;
    gl_Position.y = 1.0 - 2.0 * spy / max(vp.w, 1.0);
    gl_Position.z = scene_z_n;
    gl_Position.w = 1.0;
    int lmode = int(light_mode + 0.5);
    vec3 lit = vec3(1.0);
    if (lmode == 1) {
        lit = ambient_color.rgb + max(0.0, sun_dir.z) * sun_color.rgb;
    }
    v_uv    = uv;
    v_color = color;
    v_lit   = lit;
    v_debug = debug_mode;
}
)GLSL";

inline constexpr const char* kFxStripFsGlsl = R"GLSL(
#version 330
in vec2  v_uv;
in vec4  v_color;
in vec3  v_lit;
in float v_debug;
uniform sampler2D atlas;
out vec4 o_color;
void main() {
    int mode = int(v_debug + 0.5);
    vec4 c;
    if      (mode == 1) c = v_color;
    else if (mode == 2) c = texture(atlas, v_uv);
    else if (mode == 3) { c = texture(atlas, v_uv); c = vec4(1.0, 1.0, 1.0, c.a); }
    else                c = texture(atlas, v_uv) * v_color;
    c.rgb *= v_lit;
    if (c.a < 0.002) discard;
    o_color = c;
}
)GLSL";
