// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *       tile.glsl.h  - GLSL tile G-buffer fill (GL 3.3 core variant)    *
// *************************************************************************
//
// Linux / SOKOL_GLCORE33 port of tile.metal.h. See that file and
// docs/DEFERRED_LIGHTING.md for the derivation; this is a straight
// transcription.
//
// *************************************************************************

#pragma once

inline constexpr const char* kTileVsGlsl = R"GLSL(
#version 330 core
#define ISO_COS30 0.867
#define ISO_WZ_DENOM 2.003378
layout(std140) uniform params {
    vec4 rect;
    vec4 zparams;
    vec4 tile_root;
    vec4 tile_sprite;
    vec4 filter_;
    vec4 obj_id;
    vec4 camera;
};
layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 uv;
out vec2 v_uv;
void main() {
    float sx = uv.x * tile_sprite.z - tile_sprite.x;
    float sy = uv.y * tile_sprite.w - tile_sprite.y;
    float lz = (-2.0 * sy * ISO_COS30) / ISO_WZ_DENOM;
    float su = 2.0 * (sy + lz * ISO_COS30);
    float wx = tile_root.x + 0.5 * (su + sx);
    float wy = tile_root.y + 0.5 * (su - sx);
    float wz = tile_root.z + lz;
    float relx = wx - camera.x;
    float rely = wy - camera.y;
    float sum = relx + rely;
    float S = relx - rely;
    float T = 0.5 * sum - wz * ISO_COS30;
    float scene_z_wu = camera.z - ISO_COS30 * sum - 0.5 * wz;
    float zoom = max(zparams.w, 0.0001);
    float persp_scale = ((camera.w > 0.5) ? (camera.z / max(scene_z_wu, 1.0)) : 1.0) * zoom;
    float spx = rect.x + S * persp_scale;
    float spy = rect.y + T * persp_scale;
    gl_Position = vec4(2.0 * spx / max(rect.z, 1.0) - 1.0,
                       1.0 - 2.0 * spy / max(rect.w, 1.0),
                       0.0, 1.0);
    v_uv = uv;
}
)GLSL";

inline constexpr const char* kTileFsGlsl = R"GLSL(
#version 330 core
#define ISO_COS30 0.867
#define ISO_WZ_DENOM 2.003378
#define EDGE_EPS 1e-6
layout(std140) uniform params {
    vec4 rect;
    vec4 zparams;
    vec4 tile_root;
    vec4 tile_sprite;
    vec4 filter_;
    vec4 obj_id;
    vec4 camera;
};
in vec2 v_uv;
uniform sampler2D color_tex;
uniform sampler2D depth_tex;
layout(location = 0) out vec4 out_albedo;
layout(location = 1) out vec4 out_normal;
layout(location = 2) out vec4 out_scene_z;
layout(location = 3) out vec4 out_obj_id;
void main() {
    vec4 c = texture(color_tex, v_uv);
    if (c.a < 0.01) discard;
    float zraw = texture(depth_tex, v_uv).r;
    float d    = zparams.x + zraw * zparams.y;
    if (d < 0.0 || d > 1.0) discard;
    float nr  = max(filter_.x, 0.5);
    float thr = max(filter_.y, 0.0);
    vec2  ts  = vec2(textureSize(depth_tex, 0));
    vec2  tex = vec2(nr / max(ts.x, 1.0), nr / max(ts.y, 1.0));
    float sx = v_uv.x * tile_sprite.z - tile_sprite.x;
    float sy = v_uv.y * tile_sprite.w - tile_sprite.y;
    float zl  = texture(depth_tex, v_uv - vec2(tex.x, 0.0)).r;
    float zrs = texture(depth_tex, v_uv + vec2(tex.x, 0.0)).r;
    float zt  = texture(depth_tex, v_uv - vec2(0.0, tex.y)).r;
    float zb  = texture(depth_tex, v_uv + vec2(0.0, tex.y)).r;
    if (abs(zl  - zraw) > thr + EDGE_EPS) zl  = zraw;
    if (abs(zrs - zraw) > thr + EDGE_EPS) zrs = zraw;
    if (abs(zt  - zraw) > thr + EDGE_EPS) zt  = zraw;
    if (abs(zb  - zraw) > thr + EDGE_EPS) zb  = zraw;
    float dzl = zl  * tile_root.w;
    float dzr = zrs * tile_root.w;
    float dzt = zt  * tile_root.w;
    float dzb = zb  * tile_root.w;
    float nrx = nr;
    float nry = nr;
    float lz_l = (-dzl - 2.0 * sy * ISO_COS30) / ISO_WZ_DENOM;
    float su_l = 2.0 * (sy + lz_l * ISO_COS30);
    vec3  Wl = vec3(tile_root.x + 0.5 * (su_l + (sx - nrx)),
                    tile_root.y + 0.5 * (su_l - (sx - nrx)),
                    tile_root.z + lz_l);
    float lz_r = (-dzr - 2.0 * sy * ISO_COS30) / ISO_WZ_DENOM;
    float su_r = 2.0 * (sy + lz_r * ISO_COS30);
    vec3  Wr = vec3(tile_root.x + 0.5 * (su_r + (sx + nrx)),
                    tile_root.y + 0.5 * (su_r - (sx + nrx)),
                    tile_root.z + lz_r);
    float lz_t = (-dzt - 2.0 * (sy - nry) * ISO_COS30) / ISO_WZ_DENOM;
    float su_t = 2.0 * ((sy - nry) + lz_t * ISO_COS30);
    vec3  Wt = vec3(tile_root.x + 0.5 * (su_t + sx),
                    tile_root.y + 0.5 * (su_t - sx),
                    tile_root.z + lz_t);
    float lz_b = (-dzb - 2.0 * (sy + nry) * ISO_COS30) / ISO_WZ_DENOM;
    float su_b = 2.0 * ((sy + nry) + lz_b * ISO_COS30);
    vec3  Wb = vec3(tile_root.x + 0.5 * (su_b + sx),
                    tile_root.y + 0.5 * (su_b - sx),
                    tile_root.z + lz_b);
    vec3 t_u = Wr - Wl;
    vec3 t_v = Wb - Wt;
    vec3 N   = normalize(cross(t_v, t_u));
    vec3 Vc  = normalize(vec3(ISO_COS30, ISO_COS30, 1.0));
    if (dot(N, Vc) < 0.0) N = -N;
    out_albedo  = c;
    out_normal  = vec4(N * 0.5 + 0.5, 1.0);
    out_scene_z = vec4(d, 0.0, 0.0, 1.0);
    out_obj_id  = obj_id;
    gl_FragDepth = d;
}
)GLSL";
