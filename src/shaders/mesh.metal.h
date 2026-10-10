// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *           mesh.metal.h  - MSL 3D mesh shaders (G-buffer + translucent)*
// *************************************************************************
//
// kMeshFsMetal writes the same G-buffer MRT layout as tile.metal.h but from
// real 3D vertices transformed by a per-instance world matrix, with surface
// class 0 in normal.a so the light pass applies the mesh lighting model.
//
// Meshes whose tint alpha is below 1 (a fading character) skip the G-buffer
// and draw in the translucent pass over lit_target, one surface at a time:
//   kMeshDepthFsMetal       depth only -- leaves the surface's nearest depth
//   kMeshTranslucentFsMetal lights each fragment with shade_surface() (the
//                           light pass's function; TRenderer prepends
//                           lightmodel.metal.h) and blends it at tint alpha
// Both run behind kMeshVsMetal, so their depths match the prepass exactly.
// docs/RENDERER_ARCHITECTURE.md, "Translucent meshes".
// Instance layout:
//   slot 1  mat4 world   (4 * float4, one attribute each for cols 0..3)
//   slot 2  tint rgba    (1 * float4)
//
// Per-vertex (slot 0): position (xyz), normal (xyz), uv (xy).
//
// The vertex shader applies world, then projects world -> iso screen pixel
// using the same math that light.metal.h's world-reconstruction inverse
// assumes. See docs/DEFERRED_LIGHTING.md for the derivation.
//
//   vp      .xy = camera origin (padded G-buffer pixel space)
//           .zw = (fbw, fbh) padded G-buffer dimensions
//   camz    .x  = z_near wu, .y = zspan wu, .z = kcam_forward/focal wu,
//           .w  = perspective enable
//   camw    .xy = camera center (world xy); .z = zoom; .w unused
//
// *************************************************************************

#pragma once

inline constexpr const char* kMeshVsMetal = R"MSL(
#include <metal_stdlib>
using namespace metal;
#define ISO_COS30 0.867
struct mesh_params { float4 vp; float4 camz; float4 camw; float4 retail_ambient; float4 retail_directional; };
struct vs_in {
    float3 pos          [[attribute(0)]];
    float3 normal       [[attribute(1)]];
    float2 uv           [[attribute(2)]];
    float4 w0           [[attribute(3)]];
    float4 w1           [[attribute(4)]];
    float4 w2           [[attribute(5)]];
    float4 w3           [[attribute(6)]];
    float4 tint         [[attribute(7)]];
    float4 inst_obj_id  [[attribute(8)]];
    float2 uv_offset    [[attribute(9)]];
    float retail_mode   [[attribute(10)]];
};
struct vs_out {
    float4 pos     [[position]];
    float3 wpos;
    float3 wnormal;
    float2 uv;
    float4 tint;
    float4 obj_id;
    float  scene_z;
    float3 retail_color;
    float retail_mode;
};
vertex vs_out _main(vs_in in [[stage_in]],
                    constant mesh_params& p [[buffer(0)]]) {
    // w0..w3 are rows of a 4x4 world matrix (backend-neutral layout).
    float4 ph = float4(in.pos, 1.0);
    float3 wp = float3(dot(in.w0, ph), dot(in.w1, ph), dot(in.w2, ph));
    float3 wn = normalize(float3(dot(in.w0.xyz, in.normal),
                                 dot(in.w1.xyz, in.normal),
                                 dot(in.w2.xyz, in.normal)));

    // blue/swscene.cpp::Illuminate: source normal transform, fixed direction,
    // and five-bit vertex light before Gouraud interpolation. ARGB bypasses it.
    float3 source_light = p.retail_ambient.xyz + p.retail_directional.xyz *
        max(dot(wn, float3(0.0, 0.78125, 0.625)), 0.0);
    float3 retail_color = floor(clamp(source_light, 0.0, 1.0) * 31.0) / 31.0;

    if (p.camw.w > 0.5) {
        // Retail EquipmentPane paperdoll path:
        // D3D view = identity, projection diag = {1/65536,1/65536,0x37b6db6e,1},
        // viewport clip scale = 65536 * sqrt(2).  Net XY scale is sqrt(2)
        // pixels per world unit around the D3D viewport center.
        constexpr float D3D_XY_SCALE = 1.4142135623730951;
        constexpr float D3D_Z_SCALE  = 2.1798270608996972e-5;
        float spx = p.vp.x + wp.x * D3D_XY_SCALE;
        float spy = p.vp.y - wp.y * D3D_XY_SCALE;
        float scene_z_n = clamp(wp.z * D3D_Z_SCALE, 0.0, 1.0);

        vs_out o;
        o.pos.x = 2.0 * spx / max(p.vp.z, 1.0) - 1.0;
        o.pos.y = 1.0 - 2.0 * spy / max(p.vp.w, 1.0);
        o.pos.z = scene_z_n;
        o.pos.w = 1.0;
        o.wpos    = wp;
        o.wnormal = wn;
        o.uv      = in.uv + in.uv_offset;
        o.tint    = in.tint;
        o.obj_id  = in.inst_obj_id;
        o.scene_z = scene_z_n;
        o.retail_color = retail_color;
        o.retail_mode = in.retail_mode;
        return o;
    }

    float wx = wp.x - p.camw.x;
    float wy = wp.y - p.camw.y;
    float wz = wp.z;
    float sum = wx + wy;
    float S   = wx - wy;
    float T   = 0.5 * sum - wz * ISO_COS30;

    float scene_z_wu = p.camz.z - ISO_COS30 * sum - 0.5 * wz;
    float scene_z_n  = (scene_z_wu - p.camz.x) / max(p.camz.y, 1e-6);
    float zoom = max(p.camw.z, 0.0001);
    float persp_scale = ((p.camz.w > 0.5) ? (p.camz.z / max(scene_z_wu, 1.0)) : 1.0) * zoom;
    float spx = p.vp.x + S * persp_scale;
    float spy = p.vp.y + T * persp_scale;

    vs_out o;
    o.pos.x = 2.0 * spx / max(p.vp.z, 1.0) - 1.0;
    o.pos.y = 1.0 - 2.0 * spy / max(p.vp.w, 1.0);
    o.pos.z = scene_z_n;
    o.pos.w = 1.0;
    o.wpos    = wp;
    o.wnormal = wn;
    o.uv      = in.uv + in.uv_offset;
    o.tint    = in.tint;
    o.obj_id  = in.inst_obj_id;
    o.scene_z = scene_z_n;
    o.retail_color = retail_color;
    o.retail_mode = in.retail_mode;
    return o;
}
)MSL";

inline constexpr const char* kMeshFsMetal = R"MSL(
#include <metal_stdlib>
using namespace metal;
struct vs_out {
    float4 pos     [[position]];
    float3 wpos;
    float3 wnormal;
    float2 uv;
    float4 tint;
    float4 obj_id;
    float  scene_z;
    float3 retail_color;
    float retail_mode;
};
struct fs_out { float4 albedo  [[color(0)]];
                float4 normal  [[color(1)]];
                float4 scene_z [[color(2)]];
                float4 obj_id  [[color(3)]]; };
fragment fs_out _main(vs_out in [[stage_in]],
                      texture2d<float> albedo_tex [[texture(0)]],
                      sampler smp [[sampler(0)]]) {
    // Blue software rasterization indexes texels directly and wraps UVs.
    float4 texel = albedo_tex.sample(smp, in.uv);
    if (in.retail_mode > 0.5) {
        uint2 size = uint2(albedo_tex.get_width(), albedo_tex.get_height());
        uint2 xy = min(uint2(floor(fract(in.uv) * float2(size))), size - uint2(1));
        texel = albedo_tex.read(xy);
        if (in.retail_mode > 2.5) {
            // Native 56ca90 RGB565 table; 54fbf2 alpha nibble plus one.
            float4 nibble = floor(texel * 15.0 + 0.5);
            texel = float4(nibble.rgb * float3(2.0 / 31.0, 4.0 / 63.0, 2.0 / 31.0),
                           (nibble.a + 1.0) / 16.0);
        }
    }
    float4 c = texel * in.tint;
    if (in.retail_mode > 0.5 && in.retail_mode < 1.5) c.rgb *= in.retail_color;
    if (c.a < 0.01) discard_fragment();
    float3 N = normalize(in.wnormal);
    fs_out o;
    o.albedo  = c;
    o.normal  = float4(N * 0.5 + 0.5, in.retail_mode > 0.5 ? 2.0 : 0.0);
    o.scene_z = float4(in.scene_z, 0.0, 0.0, 1.0);
    o.obj_id  = in.obj_id;
    return o;
}
)MSL";

// Translucent pass, step 1: depth only (colour writes are masked off). Covers
// exactly the fragments step 2 shades.
inline constexpr const char* kMeshDepthFsMetal = R"MSL(
#include <metal_stdlib>
using namespace metal;
struct vs_out {
    float4 pos     [[position]];
    float3 wpos;
    float3 wnormal;
    float2 uv;
    float4 tint;
    float4 obj_id;
    float  scene_z;
    float3 retail_color;
    float retail_mode;
};
fragment void _main(vs_out in [[stage_in]],
                    texture2d<float> albedo_tex [[texture(0)]],
                    sampler smp [[sampler(0)]]) {
    float4 texel = albedo_tex.sample(smp, in.uv);
    if (in.retail_mode > 0.5) {
        uint2 size = uint2(albedo_tex.get_width(), albedo_tex.get_height());
        uint2 xy = min(uint2(floor(fract(in.uv) * float2(size))), size - uint2(1));
        texel = albedo_tex.read(xy);
        if (in.retail_mode > 2.5) {
            // Native 56ca90 RGB565 table; 54fbf2 alpha nibble plus one.
            float4 nibble = floor(texel * 15.0 + 0.5);
            texel = float4(nibble.rgb * float3(2.0 / 31.0, 4.0 / 63.0, 2.0 / 31.0),
                           (nibble.a + 1.0) / 16.0);
        }
    }
    if (texel.a * in.tint.a < 0.01) discard_fragment();
}
)MSL";

// Translucent pass, step 2: lit colour at the surface's nearest depth,
// blended over lit_target. Appended to kLightModelMetal. The surface isn't in
// the G-buffer, so it has no SSAO or sun cast shadow (both 1).
inline constexpr const char* kMeshTranslucentFsMetal = R"MSL(
struct vs_out {
    float4 pos     [[position]];
    float3 wpos;
    float3 wnormal;
    float2 uv;
    float4 tint;
    float4 obj_id;
    float  scene_z;
    float3 retail_color;
    float retail_mode;
};
fragment float4 _main(vs_out in [[stage_in]],
                      texture2d<float> albedo_tex [[texture(0)]],
                      sampler smp [[sampler(0)]],
                      constant params& p [[buffer(0)]]) {
    float4 texel = albedo_tex.sample(smp, in.uv);
    if (in.retail_mode > 0.5) {
        uint2 size = uint2(albedo_tex.get_width(), albedo_tex.get_height());
        uint2 xy = min(uint2(floor(fract(in.uv) * float2(size))), size - uint2(1));
        texel = albedo_tex.read(xy);
        if (in.retail_mode > 2.5) {
            // Native 56ca90 RGB565 table; 54fbf2 alpha nibble plus one.
            float4 nibble = floor(texel * 15.0 + 0.5);
            texel = float4(nibble.rgb * float3(2.0 / 31.0, 4.0 / 63.0, 2.0 / 31.0),
                           (nibble.a + 1.0) / 16.0);
        }
    }
    float4 c = texel * in.tint;
    if (in.retail_mode > 0.5 && in.retail_mode < 1.5) c.rgb *= in.retail_color;
    if (c.a < 0.01) discard_fragment();
    float3 N = normalize(in.wnormal);
    int vm = int(p.settings.x);
    if (vm == 1) return float4(c.rgb, c.a);
    if (vm == 3) return float4(N * 0.5 + 0.5, c.a);
    surface_light s = shade_surface(c.rgb, in.wpos, N, true, 1.0, 1.0, p);
    if (int(p.settings.z) == 0 && in.retail_mode > 0.5) s.lit = c.rgb;
    if (vm == 4) return float4(s.points, c.a);
    if (vm == 6) return float4(float3(s.sun_shadow), c.a);
    return float4(s.lit, c.a);
}
)MSL";
