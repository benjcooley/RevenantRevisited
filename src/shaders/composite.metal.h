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
struct params { float4 rect; float4 uv_rect; float4 chroma_key; float4 color_tint; };
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
struct params { float4 rect; float4 uv_rect; float4 chroma_key; float4 color_tint; };
struct vs_out { float4 pos [[position]]; float2 uv; };
fragment float4 _main(vs_out in [[stage_in]],
                      constant params& p [[buffer(0)]],
                      texture2d<float> tex [[texture(0)]],
                      sampler smp [[sampler(0)]]) {
    float4 c = tex.sample(smp, in.uv);
    if (p.chroma_key.x > 0.5 && c.r > 0.55 && c.g < 0.30 && c.b < 0.30)
        discard_fragment();
    return c * p.color_tint;
}
)MSL";

// The 2:1 reduction (TRenderer::DrawBitmapHalvedToTarget): each pixel covers
// a 2x2 block of texels and takes the mean of the opaque ones; with fewer
// than three opaque it is left empty. The mean is taken as retail takes it,
// in the RGB565 the bitmaps hold (0x004a31a0): green and blue the floor of
// the mean of their values; red sums each pixel's top byte -- red with
// green's top three bits under it -- and drops those bits after dividing, so
// green can carry red up a step. `texel` is one source texel in uv; the
// quad's uv lands on the corner the block's four texels share.
inline constexpr const char* kCompositeReduceFsMetal = R"MSL(
#include <metal_stdlib>
using namespace metal;
struct params { float4 rect; float4 uv_rect; float4 texel; float4 color_tint; };
struct vs_out { float4 pos [[position]]; float2 uv; };
fragment float4 _main(vs_out in [[stage_in]],
                      constant params& p [[buffer(0)]],
                      texture2d<float> tex [[texture(0)]],
                      sampler smp [[sampler(0)]]) {
    const float2 h = 0.5 * p.texel.xy;
    const float4 block[4] = { tex.sample(smp, in.uv + float2(-h.x, -h.y)),
                              tex.sample(smp, in.uv + float2( h.x, -h.y)),
                              tex.sample(smp, in.uv + float2(-h.x,  h.y)),
                              tex.sample(smp, in.uv + float2( h.x,  h.y)) };
    const float3 steps = float3(8.0, 4.0, 8.0);         // one RGB565 step of each channel, in 8 bits
    float3 sum = float3(0.0);                           // 8 red + green's top 3 bits, green, blue
    float opaque = 0.0;
    for (int i = 0; i < 4; ++i)
        if (block[i].a > 0.5)
        {
            const float3 c = floor(rint(block[i].rgb * 255.0) / steps);
            sum += float3(c.r * 8.0 + floor(c.g / 8.0), c.g, c.b);
            opaque += 1.0;
        }
    if (opaque < 3.0)
        discard_fragment();
    const float3 mean = floor(sum / (opaque * float3(8.0, 1.0, 1.0)));
    const float3 levels = float3(32.0, 64.0, 32.0);      // back to 8 bits by bit replication
    return float4((mean * steps + floor(mean / (levels / steps))) / 255.0, 1.0) * p.color_tint;
}
)MSL";
