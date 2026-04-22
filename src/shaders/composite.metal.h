// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *     composite.metal.h  - MSL composite quad (UI + swapchain blit)     *
// *************************************************************************
//
// Textured unit-quad shader used by TRenderer::Composite. A small push
// constant (rect + uv_rect) lets the same quad blit any sub-rect of any
// source image into any screen-space destination -- the backbuffer ->
// swapchain blit, the lit_target -> swapchain composite, and all the
// atlas sub-rect blits go through this pipeline.
//
// *************************************************************************

#pragma once

inline constexpr const char* kCompositeVsMetal = R"MSL(
#include <metal_stdlib>
using namespace metal;
struct params { float4 rect; float4 uv_rect; };
struct vs_in  { float2 pos [[attribute(0)]]; float2 uv [[attribute(1)]]; };
struct vs_out { float4 pos [[position]];      float2 uv; };
vertex vs_out _main(vs_in in [[stage_in]], constant params& p [[buffer(0)]]) {
    vs_out o;
    o.pos = float4(p.rect.xy + in.pos * p.rect.zw, 0.0, 1.0);
    o.uv  = p.uv_rect.xy + in.uv * p.uv_rect.zw;
    return o;
}
)MSL";

inline constexpr const char* kCompositeFsMetal = R"MSL(
#include <metal_stdlib>
using namespace metal;
struct vs_out { float4 pos [[position]]; float2 uv; };
fragment float4 _main(vs_out in [[stage_in]],
                      texture2d<float> tex [[texture(0)]],
                      sampler smp [[sampler(0)]]) {
    return tex.sample(smp, in.uv);
}
)MSL";
