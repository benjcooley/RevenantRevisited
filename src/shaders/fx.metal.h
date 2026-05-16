// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *           fx.metal.h  - MSL VFX billboard / particle / strip          *
// *************************************************************************
//
// Phase 1 VFX spine. Three pipeline pairs, shared math.
//
//   fx_billboard : per-instance world_pos + size_wu + uv_rect + color +
//                  debug_mode. Static 4-vertex corner VB. Screen-aligned
//                  quad expansion in the vertex shader.
//   fx_particle  : fx_billboard + per-instance rotation_rad (rotates the
//                  corner about the camera-forward axis before expand).
//   fx_strip     : CPU-expanded per-segment quads. Each segment becomes
//                  4 vertices; the vertex shader offsets corners along
//                  side = normalize(cross(tangent, view_dir)) * half_w.
//
// All three share the iso-projection from tile.metal.h / mesh.metal.h so
// fragment depth matches the deferred G-buffer (lit pass blends against
// the same coordinate system). UBO layout:
//
//   vp     .xy = camera pixel origin (padded G-buffer space)
//          .zw = padded G-buffer dims (fbw, fbh)
//   camz   .x  = z_near wu, .y = zspan wu, .z = kcam_forward focal,
//          .w  = perspective enable (0/1)
//   camw   .xy = camera-center world xy, .z = zoom, .w unused
//
// Diagnostic ladder is per-instance (EFxDebugMode):
//   0 Normal       : sample atlas at uv_rect, multiply by color
//   1 SolidColor   : skip sample, output color
//   2 FullTexture  : sample at corner.xy + 0.5 (ignore uv_rect)
//   3 CurrentFrame : sample at uv_rect, force color = white
//
// *************************************************************************

#pragma once

// ---- shared iso-projection helper ---------------------------------------
// Kept in sync with mesh.metal.h. Returns ndc.xyz + scene_z_normalized.
#define FX_PROJECT_BODY \
    "    float wx = wp.x - p.camw.x;\n"                                          \
    "    float wy = wp.y - p.camw.y;\n"                                          \
    "    float wz = wp.z;\n"                                                     \
    "    float sum = wx + wy;\n"                                                 \
    "    float S   = wx - wy;\n"                                                 \
    "    float T   = 0.5 * sum - wz * 0.867;\n"                                  \
    "    float scene_z_wu = p.camz.z - 0.867 * sum - 0.5 * wz;\n"                \
    "    float scene_z_n  = (scene_z_wu - p.camz.x) / max(p.camz.y, 1e-6);\n"    \
    "    float zoom = max(p.camw.z, 0.0001);\n"                                  \
    "    float persp_scale = ((p.camz.w > 0.5) ? (p.camz.z / max(scene_z_wu, 1.0)) : 1.0) * zoom;\n" \
    "    float spx = p.vp.x + S * persp_scale;\n"                                \
    "    float spy = p.vp.y + T * persp_scale;\n"

inline constexpr const char* kFxBillboardVsMetal = R"MSL(
#include <metal_stdlib>
using namespace metal;
struct fx_params { float4 vp; float4 camz; float4 camw; };
struct vs_in {
    float2 corner       [[attribute(0)]];   // [-0.5,+0.5]^2
    float3 world_pos    [[attribute(1)]];
    float2 size_wu      [[attribute(2)]];
    float4 uv_rect      [[attribute(3)]];
    float4 color_rgba   [[attribute(4)]];
    float  debug_mode   [[attribute(5)]];
};
struct vs_out {
    float4 pos      [[position]];
    float2 corner;
    float4 uv_rect;
    float4 color;
    float  debug_mode;
};
vertex vs_out _main(vs_in in [[stage_in]],
                    constant fx_params& p [[buffer(0)]]) {
    // Screen-aligned: in the iso projection used by the rest of the
    // pipeline, "right" is +x in screen pixels and "up" is +y (we flip y
    // at NDC). We just need the corner to shift the world anchor by
    // size_wu * 0.5 along world directions that produce a pixel-aligned
    // quad. The cheap path: project the anchor, then offset spx/spy by
    // corner * size_wu * persp_scale. This keeps the size in world units
    // and the quad screen-aligned for free.
    float3 wp = in.world_pos;
    float wx = wp.x - p.camw.x;
    float wy = wp.y - p.camw.y;
    float wz = wp.z;
    float sum = wx + wy;
    float S   = wx - wy;
    float T   = 0.5 * sum - wz * 0.867;
    float scene_z_wu = p.camz.z - 0.867 * sum - 0.5 * wz;
    float scene_z_n  = (scene_z_wu - p.camz.x) / max(p.camz.y, 1e-6);
    float zoom = max(p.camw.z, 0.0001);
    float persp_scale = ((p.camz.w > 0.5) ? (p.camz.z / max(scene_z_wu, 1.0)) : 1.0) * zoom;
    float spx = p.vp.x + S * persp_scale + in.corner.x * in.size_wu.x * persp_scale;
    float spy = p.vp.y + T * persp_scale - in.corner.y * in.size_wu.y * persp_scale;

    vs_out o;
    o.pos.x = 2.0 * spx / max(p.vp.z, 1.0) - 1.0;
    o.pos.y = 1.0 - 2.0 * spy / max(p.vp.w, 1.0);
    o.pos.z = scene_z_n;
    o.pos.w = 1.0;
    o.corner     = in.corner;
    o.uv_rect    = in.uv_rect;
    o.color      = in.color_rgba;
    o.debug_mode = in.debug_mode;
    return o;
}
)MSL";

inline constexpr const char* kFxBillboardFsMetal = R"MSL(
#include <metal_stdlib>
using namespace metal;
struct vs_out {
    float4 pos      [[position]];
    float2 corner;
    float4 uv_rect;
    float4 color;
    float  debug_mode;
};
fragment float4 _main(vs_out in [[stage_in]],
                      texture2d<float> atlas [[texture(0)]],
                      sampler smp [[sampler(0)]]) {
    // EFxDebugMode: 0 normal / 1 solid / 2 fulltex / 3 currentframe
    int mode = int(in.debug_mode + 0.5);
    float2 uv01 = in.corner + float2(0.5, 0.5);
    float2 uv_normal = float2(in.uv_rect.x + uv01.x * in.uv_rect.z,
                              in.uv_rect.y + uv01.y * in.uv_rect.w);
    float4 c;
    if (mode == 1) {
        c = in.color;
    } else if (mode == 2) {
        c = atlas.sample(smp, uv01);
    } else if (mode == 3) {
        c = atlas.sample(smp, uv_normal);
    } else {
        c = atlas.sample(smp, uv_normal) * in.color;
    }
    if (c.a < 0.002) discard_fragment();
    return c;
}
)MSL";

// Particle shader: identical layout to billboard plus per-instance
// rotation that turns the corner 2D vector before screen offset.
inline constexpr const char* kFxParticleVsMetal = R"MSL(
#include <metal_stdlib>
using namespace metal;
struct fx_params { float4 vp; float4 camz; float4 camw; };
struct vs_in {
    float2 corner       [[attribute(0)]];
    float3 world_pos    [[attribute(1)]];
    float2 size_wu      [[attribute(2)]];
    float4 uv_rect      [[attribute(3)]];
    float4 color_rgba   [[attribute(4)]];
    float  debug_mode   [[attribute(5)]];
    float  rotation_rad [[attribute(6)]];
};
struct vs_out {
    float4 pos      [[position]];
    float2 corner;
    float4 uv_rect;
    float4 color;
    float  debug_mode;
};
vertex vs_out _main(vs_in in [[stage_in]],
                    constant fx_params& p [[buffer(0)]]) {
    float ca = cos(in.rotation_rad), sa = sin(in.rotation_rad);
    float2 rc = float2(ca * in.corner.x - sa * in.corner.y,
                       sa * in.corner.x + ca * in.corner.y);
    float3 wp = in.world_pos;
    float wx = wp.x - p.camw.x;
    float wy = wp.y - p.camw.y;
    float wz = wp.z;
    float sum = wx + wy;
    float S   = wx - wy;
    float T   = 0.5 * sum - wz * 0.867;
    float scene_z_wu = p.camz.z - 0.867 * sum - 0.5 * wz;
    float scene_z_n  = (scene_z_wu - p.camz.x) / max(p.camz.y, 1e-6);
    float zoom = max(p.camw.z, 0.0001);
    float persp_scale = ((p.camz.w > 0.5) ? (p.camz.z / max(scene_z_wu, 1.0)) : 1.0) * zoom;
    float spx = p.vp.x + S * persp_scale + rc.x * in.size_wu.x * persp_scale;
    float spy = p.vp.y + T * persp_scale - rc.y * in.size_wu.y * persp_scale;

    vs_out o;
    o.pos.x = 2.0 * spx / max(p.vp.z, 1.0) - 1.0;
    o.pos.y = 1.0 - 2.0 * spy / max(p.vp.w, 1.0);
    o.pos.z = scene_z_n;
    o.pos.w = 1.0;
    o.corner     = in.corner;
    o.uv_rect    = in.uv_rect;
    o.color      = in.color_rgba;
    o.debug_mode = in.debug_mode;
    return o;
}
)MSL";

// Particle FS is identical to billboard FS; alias for clarity at the
// pipeline-setup site.
inline constexpr const char* kFxParticleFsMetal = kFxBillboardFsMetal;

// Strip shader: CPU has already expanded each segment into 4 corners.
// Per-vertex attributes carry world position, the segment tangent
// (B - A), signed half-width along the side, uv, and color. The VS
// computes side = normalize(cross(tangent, view_dir)), offsets by
// side * half_width, and re-projects through the iso math.
inline constexpr const char* kFxStripVsMetal = R"MSL(
#include <metal_stdlib>
using namespace metal;
struct fx_params { float4 vp; float4 camz; float4 camw; };
struct fx_cam    { float4 cam_pos; float4 cam_fwd; };
struct vs_in {
    float3 world_pos    [[attribute(0)]];
    float3 world_tan    [[attribute(1)]];
    float  half_width   [[attribute(2)]];
    float2 uv           [[attribute(3)]];
    float4 color        [[attribute(4)]];
    float  debug_mode   [[attribute(5)]];
};
struct vs_out {
    float4 pos      [[position]];
    float2 uv;
    float4 color;
    float  debug_mode;
};
vertex vs_out _main(vs_in in [[stage_in]],
                    constant fx_params& p [[buffer(0)]],
                    constant fx_cam& cam [[buffer(1)]]) {
    float3 view_dir = normalize(in.world_pos - cam.cam_pos.xyz);
    float3 t = in.world_tan;
    float  tlen = length(t);
    if (tlen > 1e-4) t = t / tlen;
    float3 side = cross(t, view_dir);
    float slen = length(side);
    if (slen > 1e-4) side = side / slen;
    float3 wp = in.world_pos + side * in.half_width;

    float wx = wp.x - p.camw.x;
    float wy = wp.y - p.camw.y;
    float wz = wp.z;
    float sum = wx + wy;
    float S   = wx - wy;
    float T   = 0.5 * sum - wz * 0.867;
    float scene_z_wu = p.camz.z - 0.867 * sum - 0.5 * wz;
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
    o.uv         = in.uv;
    o.color      = in.color;
    o.debug_mode = in.debug_mode;
    return o;
}
)MSL";

inline constexpr const char* kFxStripFsMetal = R"MSL(
#include <metal_stdlib>
using namespace metal;
struct vs_out {
    float4 pos      [[position]];
    float2 uv;
    float4 color;
    float  debug_mode;
};
fragment float4 _main(vs_out in [[stage_in]],
                      texture2d<float> atlas [[texture(0)]],
                      sampler smp [[sampler(0)]]) {
    int mode = int(in.debug_mode + 0.5);
    float4 c;
    if (mode == 1) {
        c = in.color;
    } else if (mode == 2 || mode == 3) {
        c = atlas.sample(smp, in.uv);
        if (mode == 3) c = float4(1.0, 1.0, 1.0, c.a);
    } else {
        c = atlas.sample(smp, in.uv) * in.color;
    }
    if (c.a < 0.002) discard_fragment();
    return c;
}
)MSL";
