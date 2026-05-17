// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *           fx.hlsl.h  - HLSL VFX billboard / particle / strip (stub)   *
// *************************************************************************
//
// HLSL port of fx.metal.h. Stub-only until a Windows D3D11 build is
// brought online for VFX validation. See fx.metal.h for documentation,
// including the LitFlat per-instance lighting branch.
//
// *************************************************************************

#pragma once

inline constexpr const char* kFxBillboardVsHlsl = R"HLSL(
cbuffer fx_params : register(b0) {
    float4 vp;
    float4 camz;
    float4 camw;
    float4 sun_dir;
    float4 sun_color;
    float4 ambient_color;
};
struct vs_in {
    float2 corner       : TEXCOORD0;
    float3 world_pos    : TEXCOORD1;
    float2 size_wu      : TEXCOORD2;
    float4 uv_rect      : TEXCOORD3;
    float4 color_rgba   : TEXCOORD4;
    float  debug_mode   : TEXCOORD5;
    float  light_mode   : TEXCOORD6;
};
struct vs_out {
    float4 pos      : SV_Position;
    float2 corner   : TEXCOORD0;
    float4 uv_rect  : TEXCOORD1;
    float4 color    : TEXCOORD2;
    float3 lit      : TEXCOORD3;
    float  debug    : TEXCOORD4;
};
vs_out main_vs(vs_in i) {
    float3 wp = i.world_pos;
    float wx = wp.x - camw.x, wy = wp.y - camw.y, wz = wp.z;
    float sum = wx + wy;
    float S = wx - wy;
    float T = 0.5 * sum - wz * 0.867;
    float scene_z_wu = camz.z - 0.867 * sum - 0.5 * wz;
    float scene_z_n  = (scene_z_wu - camz.x) / max(camz.y, 1e-6);
    float zoom = max(camw.z, 0.0001);
    float persp_scale = ((camz.w > 0.5) ? (camz.z / max(scene_z_wu, 1.0)) : 1.0) * zoom;
    float spx = vp.x + S * persp_scale + i.corner.x * i.size_wu.x * persp_scale;
    float spy = vp.y + T * persp_scale - i.corner.y * i.size_wu.y * persp_scale;
    vs_out o;
    o.pos.x = 2.0 * spx / max(vp.z, 1.0) - 1.0;
    o.pos.y = 1.0 - 2.0 * spy / max(vp.w, 1.0);
    o.pos.z = scene_z_n;
    o.pos.w = 1.0;
    int lmode = int(i.light_mode + 0.5);
    float3 lit = float3(1, 1, 1);
    if (lmode == 1) {
        lit = ambient_color.rgb + max(0.0, sun_dir.z) * sun_color.rgb;
    }
    o.corner  = i.corner;
    o.uv_rect = i.uv_rect;
    o.color   = i.color_rgba;
    o.lit     = lit;
    o.debug   = i.debug_mode;
    return o;
}
)HLSL";

inline constexpr const char* kFxBillboardFsHlsl = R"HLSL(
Texture2D atlas : register(t0);
SamplerState smp : register(s0);
struct vs_out {
    float4 pos      : SV_Position;
    float2 corner   : TEXCOORD0;
    float4 uv_rect  : TEXCOORD1;
    float4 color    : TEXCOORD2;
    float3 lit      : TEXCOORD3;
    float  debug    : TEXCOORD4;
};
float4 main_ps(vs_out i) : SV_Target {
    int mode = int(i.debug + 0.5);
    float2 uv01 = i.corner + float2(0.5, 0.5);
    float2 uv_n = float2(i.uv_rect.x + uv01.x * i.uv_rect.z,
                         i.uv_rect.y + uv01.y * i.uv_rect.w);
    float4 c;
    if      (mode == 1) c = i.color;
    else if (mode == 2) c = atlas.Sample(smp, uv01);
    else if (mode == 3) c = atlas.Sample(smp, uv_n);
    else                c = atlas.Sample(smp, uv_n) * i.color;
    c.rgb *= i.lit;
    if (c.a < 0.002) discard;
    return c;
}
)HLSL";

inline constexpr const char* kFxParticleVsHlsl = R"HLSL(
cbuffer fx_params : register(b0) {
    float4 vp;
    float4 camz;
    float4 camw;
    float4 sun_dir;
    float4 sun_color;
    float4 ambient_color;
};
struct vs_in {
    float2 corner       : TEXCOORD0;
    float3 world_pos    : TEXCOORD1;
    float2 size_wu      : TEXCOORD2;
    float4 uv_rect      : TEXCOORD3;
    float4 color_rgba   : TEXCOORD4;
    float  debug_mode   : TEXCOORD5;
    float  rotation_rad : TEXCOORD6;
    float  light_mode   : TEXCOORD7;
};
struct vs_out {
    float4 pos      : SV_Position;
    float2 corner   : TEXCOORD0;
    float4 uv_rect  : TEXCOORD1;
    float4 color    : TEXCOORD2;
    float3 lit      : TEXCOORD3;
    float  debug    : TEXCOORD4;
};
vs_out main_vs(vs_in i) {
    float ca = cos(i.rotation_rad), sa = sin(i.rotation_rad);
    float2 rc = float2(ca*i.corner.x - sa*i.corner.y, sa*i.corner.x + ca*i.corner.y);
    float3 wp = i.world_pos;
    float wx = wp.x - camw.x, wy = wp.y - camw.y, wz = wp.z;
    float sum = wx + wy;
    float S = wx - wy;
    float T = 0.5 * sum - wz * 0.867;
    float scene_z_wu = camz.z - 0.867 * sum - 0.5 * wz;
    float scene_z_n  = (scene_z_wu - camz.x) / max(camz.y, 1e-6);
    float zoom = max(camw.z, 0.0001);
    float persp_scale = ((camz.w > 0.5) ? (camz.z / max(scene_z_wu, 1.0)) : 1.0) * zoom;
    float spx = vp.x + S * persp_scale + rc.x * i.size_wu.x * persp_scale;
    float spy = vp.y + T * persp_scale - rc.y * i.size_wu.y * persp_scale;
    vs_out o;
    o.pos.x = 2.0 * spx / max(vp.z, 1.0) - 1.0;
    o.pos.y = 1.0 - 2.0 * spy / max(vp.w, 1.0);
    o.pos.z = scene_z_n;
    o.pos.w = 1.0;
    int lmode = int(i.light_mode + 0.5);
    float3 lit = float3(1, 1, 1);
    if (lmode == 1) {
        lit = ambient_color.rgb + max(0.0, sun_dir.z) * sun_color.rgb;
    }
    o.corner  = i.corner;
    o.uv_rect = i.uv_rect;
    o.color   = i.color_rgba;
    o.lit     = lit;
    o.debug   = i.debug_mode;
    return o;
}
)HLSL";

inline constexpr const char* kFxParticleFsHlsl = kFxBillboardFsHlsl;

inline constexpr const char* kFxStripVsHlsl = R"HLSL(
cbuffer fx_params : register(b0) {
    float4 vp;
    float4 camz;
    float4 camw;
    float4 sun_dir;
    float4 sun_color;
    float4 ambient_color;
};
cbuffer fx_cam    : register(b1) { float4 cam_pos; float4 cam_fwd; };
struct vs_in {
    float3 world_pos    : TEXCOORD0;
    float3 world_tan    : TEXCOORD1;
    float  half_width   : TEXCOORD2;
    float2 uv           : TEXCOORD3;
    float4 color        : TEXCOORD4;
    float  debug_mode   : TEXCOORD5;
    float  light_mode   : TEXCOORD6;
};
struct vs_out {
    float4 pos      : SV_Position;
    float2 uv       : TEXCOORD0;
    float4 color    : TEXCOORD1;
    float3 lit      : TEXCOORD2;
    float  debug    : TEXCOORD3;
};
vs_out main_vs(vs_in i) {
    float3 vd = normalize(i.world_pos - cam_pos.xyz);
    float3 t = i.world_tan;
    float tl = length(t);
    if (tl > 1e-4) t /= tl;
    float3 side = cross(t, vd);
    float sl = length(side);
    if (sl > 1e-4) side /= sl;
    float3 wp = i.world_pos + side * i.half_width;
    float wx = wp.x - camw.x, wy = wp.y - camw.y, wz = wp.z;
    float sum = wx + wy;
    float S = wx - wy;
    float T = 0.5 * sum - wz * 0.867;
    float scene_z_wu = camz.z - 0.867 * sum - 0.5 * wz;
    float scene_z_n  = (scene_z_wu - camz.x) / max(camz.y, 1e-6);
    float zoom = max(camw.z, 0.0001);
    float persp_scale = ((camz.w > 0.5) ? (camz.z / max(scene_z_wu, 1.0)) : 1.0) * zoom;
    float spx = vp.x + S * persp_scale;
    float spy = vp.y + T * persp_scale;
    vs_out o;
    o.pos.x = 2.0 * spx / max(vp.z, 1.0) - 1.0;
    o.pos.y = 1.0 - 2.0 * spy / max(vp.w, 1.0);
    o.pos.z = scene_z_n;
    o.pos.w = 1.0;
    int lmode = int(i.light_mode + 0.5);
    float3 lit = float3(1, 1, 1);
    if (lmode == 1) {
        lit = ambient_color.rgb + max(0.0, sun_dir.z) * sun_color.rgb;
    }
    o.uv    = i.uv;
    o.color = i.color;
    o.lit   = lit;
    o.debug = i.debug_mode;
    return o;
}
)HLSL";

inline constexpr const char* kFxStripFsHlsl = R"HLSL(
Texture2D atlas : register(t0);
SamplerState smp : register(s0);
struct vs_out {
    float4 pos      : SV_Position;
    float2 uv       : TEXCOORD0;
    float4 color    : TEXCOORD1;
    float3 lit      : TEXCOORD2;
    float  debug    : TEXCOORD3;
};
float4 main_ps(vs_out i) : SV_Target {
    int mode = int(i.debug + 0.5);
    float4 c;
    if      (mode == 1) c = i.color;
    else if (mode == 2) c = atlas.Sample(smp, i.uv);
    else if (mode == 3) { c = atlas.Sample(smp, i.uv); c = float4(1,1,1,c.a); }
    else                c = atlas.Sample(smp, i.uv) * i.color;
    c.rgb *= i.lit;
    if (c.a < 0.002) discard;
    return c;
}
)HLSL";
