// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *     composite.hlsl.h  - HLSL composite quad (SM5 / D3D11 variant)     *
// *************************************************************************
//
// Windows / SOKOL_D3D11 port of composite.metal.h. Vertex entry: main_vs;
// pixel entry: main_ps (matches the sh.vs.entry / sh.fs.entry strings set
// in the D3D11 branch of renderer.cpp's pipeline init).
//
// *************************************************************************

#pragma once

inline constexpr const char* kCompositeVsHlsl = R"HLSL(
cbuffer params : register(b0) {
    float4 rect;
    float4 uv_rect;
    float4 chroma_key;
};
struct vs_in  { float2 pos : POSITION; float2 uv : TEXCOORD0; };
struct vs_out { float4 pos : SV_Position; float2 uv : TEXCOORD0; };
vs_out main_vs(vs_in i) {
    vs_out o;
    o.pos = float4(rect.xy + i.pos * rect.zw, 0.0, 1.0);
    o.uv  = uv_rect.xy + i.uv * uv_rect.zw;
    return o;
}
)HLSL";

inline constexpr const char* kCompositeFsHlsl = R"HLSL(
Texture2D    tex : register(t0);
SamplerState smp : register(s0);
struct vs_out { float4 pos : SV_Position; float2 uv : TEXCOORD0; };
float4 main_ps(vs_out i) : SV_Target0 {
    float4 c = tex.Sample(smp, i.uv);
    if (chroma_key.x > 0.5 && c.r > 0.55 && c.g < 0.30 && c.b < 0.30)
        discard;
    return c;
}
)HLSL";
