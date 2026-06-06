// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *           mesh.metal.h  - MSL 3D mesh G-buffer fill shaders           *
// *************************************************************************
//
// Writes the same G-buffer MRT layout as tile.metal.h but from real 3D
// vertices transformed by a per-instance world matrix. Instance layout:
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
struct mesh_params { float4 vp; float4 camz; float4 camw; };
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
};
struct vs_out {
    float4 pos     [[position]];
    float3 wpos;
    float3 wnormal;
    float2 uv;
    float4 tint;
    float4 obj_id;
    float  scene_z;
};
vertex vs_out _main(vs_in in [[stage_in]],
                    constant mesh_params& p [[buffer(0)]]) {
    // w0..w3 are rows of a 4x4 world matrix (backend-neutral layout).
    float4 ph = float4(in.pos, 1.0);
    float3 wp = float3(dot(in.w0, ph), dot(in.w1, ph), dot(in.w2, ph));
    float3 wn = normalize(float3(dot(in.w0.xyz, in.normal),
                                 dot(in.w1.xyz, in.normal),
                                 dot(in.w2.xyz, in.normal)));

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
        o.uv      = in.uv;
        o.tint    = in.tint;
        o.obj_id  = in.inst_obj_id;
        o.scene_z = scene_z_n;
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
    o.uv      = in.uv;
    o.tint    = in.tint;
    o.obj_id  = in.inst_obj_id;
    o.scene_z = scene_z_n;
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
};
struct fs_out { float4 albedo  [[color(0)]];
                float4 normal  [[color(1)]];
                float4 scene_z [[color(2)]];
                float4 obj_id  [[color(3)]]; };
fragment fs_out _main(vs_out in [[stage_in]],
                      texture2d<float> albedo_tex [[texture(0)]],
                      sampler smp [[sampler(0)]]) {
    float4 c = albedo_tex.sample(smp, in.uv) * in.tint;
    if (c.a < 0.01) discard_fragment();
    float3 N = normalize(in.wnormal);
    fs_out o;
    o.albedo  = c;
    o.normal  = float4(N * 0.5 + 0.5, 1.0);
    o.scene_z = float4(in.scene_z, 0.0, 0.0, 1.0);
    o.obj_id  = in.obj_id;
    return o;
}
)MSL";
