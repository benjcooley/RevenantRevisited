// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *           mesh.hlsl.h  - HLSL SM5 mesh G-buffer fill shaders          *
// *************************************************************************
//
// D3D11 port of mesh.metal.h.  See that header for layout + projection
// math documentation.
//
// *************************************************************************

#pragma once

inline constexpr const char* kMeshVsHlsl = R"HLSL(
#define ISO_COS30 0.867
cbuffer mesh_params : register(b0) {
    float4 vp;
    float4 camz;
    float4 camw;
    float4 retail_ambient;
    float4 retail_directional;
};
struct vs_in {
    float3 pos    : POSITION;
    float3 normal : NORMAL;
    float2 uv     : TEXCOORD0;
    float4 w0     : TEXCOORD1;
    float4 w1     : TEXCOORD2;
    float4 w2     : TEXCOORD3;
    float4 w3     : TEXCOORD4;
    float4 tint   : TEXCOORD5;
    float2 uv_offset : TEXCOORD7;
    float retail_mode : TEXCOORD8;
};
struct vs_out {
    float4 pos     : SV_Position;
    float3 wpos    : TEXCOORD0;
    float3 wnormal : TEXCOORD1;
    float2 uv      : TEXCOORD2;
    float4 tint    : TEXCOORD3;
    float  scene_z : TEXCOORD4;
    float3 retail_color : TEXCOORD5;
    float retail_mode : TEXCOORD6;
};
vs_out main_vs(vs_in i) {
    // w0..w3 are rows of a 4x4 world matrix (backend-neutral layout).
    float4 ph = float4(i.pos, 1.0);
    float3 wp = float3(dot(i.w0, ph), dot(i.w1, ph), dot(i.w2, ph));
    float3 wn = normalize(float3(dot(i.w0.xyz, i.normal),
                                 dot(i.w1.xyz, i.normal),
                                 dot(i.w2.xyz, i.normal)));

    float3 source_light = retail_ambient.xyz + retail_directional.xyz *
        max(dot(wn, float3(0.0, 0.78125, 0.625)), 0.0);
    float3 retail_color = floor(saturate(source_light) * 31.0) / 31.0;
    if (camw.w > 0.5) {
        static const float D3D_XY_SCALE = 1.4142135623730951;
        static const float D3D_Z_SCALE  = 2.1798270608996972e-5;
        float spx = vp.x + wp.x * D3D_XY_SCALE;
        float spy = vp.y - wp.y * D3D_XY_SCALE;
        float scene_z_n = clamp(wp.z * D3D_Z_SCALE, 0.0, 1.0);

        vs_out o;
        o.pos.x = 2.0 * spx / max(vp.z, 1.0) - 1.0;
        o.pos.y = 1.0 - 2.0 * spy / max(vp.w, 1.0);
        o.pos.z = scene_z_n;
        o.pos.w = 1.0;
        o.wpos    = wp;
        o.wnormal = wn;
        o.uv      = i.uv + i.uv_offset;
        o.tint    = i.tint;
        o.scene_z = scene_z_n;
    o.retail_color = retail_color;
    o.retail_mode = i.retail_mode;
        return o;
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

    vs_out o;
    o.pos.x = 2.0 * spx / max(vp.z, 1.0) - 1.0;
    o.pos.y = 1.0 - 2.0 * spy / max(vp.w, 1.0);
    o.pos.z = scene_z_n;
    o.pos.w = 1.0;
    o.wpos    = wp;
    o.wnormal = wn;
    o.uv      = i.uv + i.uv_offset;
    o.tint    = i.tint;
    o.scene_z = scene_z_n;
    o.retail_color = retail_color;
    o.retail_mode = i.retail_mode;
    return o;
}
)HLSL";

inline constexpr const char* kMeshFsHlsl = R"HLSL(
Texture2D    albedo_tex : register(t0);
SamplerState smp        : register(s0);
struct vs_out {
    float4 pos     : SV_Position;
    float3 wpos    : TEXCOORD0;
    float3 wnormal : TEXCOORD1;
    float2 uv      : TEXCOORD2;
    float4 tint    : TEXCOORD3;
    float  scene_z : TEXCOORD4;
    float3 retail_color : TEXCOORD5;
    float retail_mode : TEXCOORD6;
};
struct fs_out { float4 albedo  : SV_Target0;
                float4 normal  : SV_Target1;
                float4 scene_z : SV_Target2;
                float4 obj_id  : SV_Target3; };
fs_out main_ps(vs_out in_) {
    float4 texel = albedo_tex.Sample(smp, in_.uv);
    if (in_.retail_mode > 0.5) {
        uint width, height;
        albedo_tex.GetDimensions(width, height);
        int2 size = int2(width, height);
        int2 xy = min(int2(floor(frac(in_.uv) * float2(size))), size - int2(1, 1));
        texel = albedo_tex.Load(int3(xy, 0));
    }
    float4 c = texel * in_.tint;
    if (in_.retail_mode > 0.5 && in_.retail_mode < 1.5) c.rgb *= in_.retail_color;
    if (c.a < 0.01) discard;
    float3 N = normalize(in_.wnormal);
    fs_out o;
    o.albedo  = c;
    o.normal  = float4(N * 0.5 + 0.5, in_.retail_mode > 0.5 ? 2.0 : 1.0);
    o.scene_z = float4(in_.scene_z, 0.0, 0.0, 1.0);
    o.obj_id  = float4(0.0, 0.0, 0.0, 0.0);   // Phase 1: empty id
    return o;
}
)HLSL";
