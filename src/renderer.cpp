// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *           renderer.cpp  - Core Render Engine Implementation           *
// *************************************************************************
//
// This file implements TRenderer -- Revenant's game-agnostic core render
// engine. See renderer.h for the full architecture doc.
//
// Pass pipeline per-frame (see renderer.h ARCHITECTURE):
//
//      [1] G-buffer fill          -> default_pass (MRT)
//      [2] Ambient occlusion      -> ao_pass
//      [3] Deferred lighting      -> lit_pass
//      [7] Composite to swapchain <- TDisplay::FlipPage via PresentToSwapchain
//
// Passes [4]-[6] (water, effects, debug 3D) are reserved.
// Passes [8] (debug UI) is the sokol_imgui overlay, rendered by TDisplay
// inside the swapchain pass after PresentToSwapchain.
//
// *************************************************************************

#include "renderer.h"

#include <sokol_gfx.h>

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <cstring>

#include "revenant.h"
#include "shaders/shaders.h"
#include "surface.h"

// *************************************************************************
// * Global Renderer Pointer                                               *
// *************************************************************************
TRenderer* Renderer = nullptr;

// *************************************************************************
// * Shared GPU Resources (Unit Quad)                                      *
// *************************************************************************
//
// Shader source text lives in src/shaders/*.metal.h (plus future *.glsl.h
// and *.hlsl.h siblings). shaders.h picks the right backend at compile
// time and exposes kCompositeVs / kCompositeFs / kTileVs / kTileFs /
// kAOVs / kAOFs / kLightVs / kLightFs.

namespace {

struct SCompositeVertex { float x, y, u, v; };

#if defined(SOKOL_METAL)
inline constexpr const char* kHelperMeshVs = R"MSL(
#include <metal_stdlib>
using namespace metal;
#define ISO_COS30 0.867
struct helper_vs_params {
    float4 w0; float4 w1; float4 w2; float4 w3;
    float4 vp; float4 camz; float4 camw;
};
struct vs_in {
    float3 pos    [[attribute(0)]];
    float3 normal [[attribute(1)]];
    float2 uv     [[attribute(2)]];
};
struct vs_out {
    float4 pos     [[position]];
    float3 wpos;
    float3 wnormal;
    float2 uv;
};
vertex vs_out _main(vs_in in [[stage_in]],
                    constant helper_vs_params& p [[buffer(0)]]) {
    float4 ph = float4(in.pos, 1.0);
    float3 wp = float3(dot(p.w0, ph), dot(p.w1, ph), dot(p.w2, ph));
    float3 wn = normalize(float3(dot(p.w0.xyz, in.normal),
                                 dot(p.w1.xyz, in.normal),
                                 dot(p.w2.xyz, in.normal)));
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
    o.wpos = wp;
    o.wnormal = wn;
    o.uv = in.uv;
    return o;
}
)MSL";
inline constexpr const char* kHelperMeshFs = R"MSL(
#include <metal_stdlib>
using namespace metal;
struct vs_out {
    float4 pos     [[position]];
    float3 wpos;
    float3 wnormal;
    float2 uv;
    float4 tint;
    float  scene_z;
};
struct helper_params {
    float4 diffuse;
    float4 ambient;
    float4 specular;
    float4 emissive;
    float4 light_dir_i;
    float4 light_col_a;
    float4 view_dir_power;
};
fragment float4 _main(vs_out in [[stage_in]],
                      constant helper_params& hp [[buffer(0)]],
                      texture2d<float> albedo_tex [[texture(0)]],
                      sampler smp [[sampler(0)]]) {
    float4 tex = albedo_tex.sample(smp, in.uv);
    float alpha = tex.a * max(max(hp.diffuse.a, hp.ambient.a), max(hp.specular.a, hp.emissive.a));
    if (alpha < 0.01) discard_fragment();
    float3 N = normalize(in.wnormal);
    float3 L = normalize(hp.light_dir_i.xyz);
    float3 V = normalize(hp.view_dir_power.xyz);
    float3 H = normalize(L + V);
    float ndl = max(dot(N, L), 0.0);
    float spec = (ndl > 0.0) ? pow(max(dot(N, H), 0.0), max(hp.view_dir_power.w, 1.0)) : 0.0;
    float3 base = tex.rgb * hp.diffuse.rgb;
    float3 col = base * (hp.ambient.rgb * hp.light_col_a.w + hp.light_col_a.rgb * hp.light_dir_i.w * ndl)
               + hp.specular.rgb * spec * hp.light_dir_i.w
               + hp.emissive.rgb;
    return float4(col, alpha);
}
)MSL";
inline constexpr const char* kTransparentTileFs = R"MSL(
#include <metal_stdlib>
using namespace metal;
struct params { float4 rect; float4 zparams; float4 tile_root; float4 tile_sprite;
                float4 filter; float4 obj_id; float4 camera; float4 proj; float4 tile_rect;
                float4 cam_tile; };
struct vs_out { float4 pos [[position]]; float2 uv; };
struct fs_out { float4 color [[color(0)]];
                float  depth [[depth(any)]]; };
fragment fs_out _main(vs_out in [[stage_in]],
                      texture2d<float> color_tex [[texture(0)]],
                      texture2d<float> depth_tex [[texture(1)]],
                      sampler smp [[sampler(0)]],
                      constant params& p [[buffer(0)]]) {
    fs_out o;
    float4 c = color_tex.sample(smp, in.uv);
    if (c.a < 0.01) discard_fragment();
    float zraw = depth_tex.sample(smp, in.uv).r;
    float d    = p.zparams.x + zraw * p.zparams.y;
    if (d < 0.0 || d > 1.0) discard_fragment();
    o.color = c;
    o.depth = d;
    return o;
}
)MSL";
#elif defined(SOKOL_GLCORE33) || defined(SOKOL_GLES3)
inline constexpr const char* kHelperMeshVs = R"GLSL(
#version 330
#define ISO_COS30 0.867
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(std140) uniform helper_vs_params {
    vec4 w0; vec4 w1; vec4 w2; vec4 w3;
    vec4 vp; vec4 camz; vec4 camw;
};
out vec3  v_wpos;
out vec3  v_wnormal;
out vec2  v_uv;
void main() {
    vec4 ph = vec4(pos, 1.0);
    vec3 wp = vec3(dot(w0, ph), dot(w1, ph), dot(w2, ph));
    vec3 wn = normalize(vec3(dot(w0.xyz, normal), dot(w1.xyz, normal), dot(w2.xyz, normal)));
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
    gl_Position = vec4(2.0 * spx / max(vp.z, 1.0) - 1.0,
                       1.0 - 2.0 * spy / max(vp.w, 1.0),
                       scene_z_n, 1.0);
    v_wpos = wp;
    v_wnormal = wn;
    v_uv = uv;
}
)GLSL";
inline constexpr const char* kHelperMeshFs = R"GLSL(
#version 330
in vec3  v_wpos;
in vec3  v_wnormal;
in vec2  v_uv;
in vec4  v_tint;
in float v_scene_z;
layout(std140) uniform helper_params {
    vec4 diffuse;
    vec4 ambient;
    vec4 specular;
    vec4 emissive;
    vec4 light_dir_i;
    vec4 light_col_a;
    vec4 view_dir_power;
};
uniform sampler2D albedo_tex;
out vec4 fragColor;
void main() {
    vec4 tex = texture(albedo_tex, v_uv);
    float alpha = tex.a * max(max(diffuse.a, ambient.a), max(specular.a, emissive.a));
    if (alpha < 0.01) discard;
    vec3 N = normalize(v_wnormal);
    vec3 L = normalize(light_dir_i.xyz);
    vec3 V = normalize(view_dir_power.xyz);
    vec3 H = normalize(L + V);
    float ndl = max(dot(N, L), 0.0);
    float spec = (ndl > 0.0) ? pow(max(dot(N, H), 0.0), max(view_dir_power.w, 1.0)) : 0.0;
    vec3 base = tex.rgb * diffuse.rgb;
    vec3 col = base * (ambient.rgb * light_col_a.w + light_col_a.rgb * light_dir_i.w * ndl)
             + specular.rgb * spec * light_dir_i.w
             + emissive.rgb;
    fragColor = vec4(col, alpha);
}
)GLSL";
inline constexpr const char* kTransparentTileFs = R"GLSL(
#version 330
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
out vec4 fragColor;
void main() {
    vec4 c = texture(color_tex, v_uv);
    if (c.a < 0.01) discard;
    float zraw = texture(depth_tex, v_uv).r;
    float d    = zparams.x + zraw * zparams.y;
    if (d < 0.0 || d > 1.0) discard;
    fragColor = c;
    gl_FragDepth = d;
}
)GLSL";
#elif defined(SOKOL_D3D11)
inline constexpr const char* kHelperMeshVs = R"HLSL(
#define ISO_COS30 0.867
cbuffer helper_vs_params : register(b0) {
    float4 w0; float4 w1; float4 w2; float4 w3;
    float4 vp; float4 camz; float4 camw;
};
struct vs_in {
    float3 pos    : POSITION;
    float3 normal : NORMAL;
    float2 uv     : TEXCOORD0;
};
struct vs_out {
    float4 pos     : SV_Position;
    float3 wpos    : TEXCOORD0;
    float3 wnormal : TEXCOORD1;
    float2 uv      : TEXCOORD2;
};
vs_out main_vs(vs_in i) {
    float4 ph = float4(i.pos, 1.0);
    float3 wp = float3(dot(w0, ph), dot(w1, ph), dot(w2, ph));
    float3 wn = normalize(float3(dot(w0.xyz, i.normal), dot(w1.xyz, i.normal), dot(w2.xyz, i.normal)));
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
    o.pos = float4(2.0 * spx / max(vp.z, 1.0) - 1.0,
                   1.0 - 2.0 * spy / max(vp.w, 1.0),
                   scene_z_n, 1.0);
    o.wpos = wp;
    o.wnormal = wn;
    o.uv = i.uv;
    return o;
}
)HLSL";
inline constexpr const char* kHelperMeshFs = R"HLSL(
cbuffer helper_params : register(b0) {
    float4 diffuse;
    float4 ambient;
    float4 specular;
    float4 emissive;
    float4 light_dir_i;
    float4 light_col_a;
    float4 view_dir_power;
};
Texture2D albedo_tex : register(t0);
SamplerState smp : register(s0);
struct ps_in {
    float4 pos : SV_POSITION;
    float3 wpos : TEXCOORD0;
    float3 wnormal : TEXCOORD1;
    float2 uv : TEXCOORD2;
    float4 tint : TEXCOORD3;
    float  scene_z : TEXCOORD4;
};
float4 main_ps(ps_in input) : SV_Target0 {
    float4 tex = albedo_tex.Sample(smp, input.uv);
    float alpha = tex.a * max(max(diffuse.a, ambient.a), max(specular.a, emissive.a));
    clip(alpha - 0.01);
    float3 N = normalize(input.wnormal);
    float3 L = normalize(light_dir_i.xyz);
    float3 V = normalize(view_dir_power.xyz);
    float3 H = normalize(L + V);
    float ndl = max(dot(N, L), 0.0);
    float specPow = (ndl > 0.0) ? pow(max(dot(N, H), 0.0), max(view_dir_power.w, 1.0)) : 0.0;
    float3 base = tex.rgb * diffuse.rgb;
    float3 col = base * (ambient.rgb * light_col_a.w + light_col_a.rgb * light_dir_i.w * ndl)
               + specular.rgb * specPow * light_dir_i.w
               + emissive.rgb;
    return float4(col, alpha);
}
)HLSL";
inline constexpr const char* kTransparentTileFs = R"HLSL(
cbuffer params : register(b0) {
    float4 rect;
    float4 zparams;
    float4 tile_root;
    float4 tile_sprite;
    float4 filter_;
    float4 obj_id;
    float4 camera;
};
Texture2D    color_tex : register(t0);
Texture2D    depth_tex : register(t1);
SamplerState smp       : register(s0);
struct vs_out { float4 pos : SV_Position; float2 uv : TEXCOORD0; };
struct fs_out {
    float4 color : SV_Target0;
    float  depth : SV_Depth;
};
fs_out main_ps(vs_out in_) {
    fs_out o;
    float4 c = color_tex.Sample(smp, in_.uv);
    if (c.a < 0.01) discard;
    float zraw = depth_tex.Sample(smp, in_.uv).r;
    float d    = zparams.x + zraw * zparams.y;
    if (d < 0.0 || d > 1.0) discard;
    o.color = c;
    o.depth = d;
    return o;
}
)HLSL";
#endif

// Unit quad in [0,1]^2 with UVs. Shared by composite, tile, AO, and light
// pipelines. UV.y is flipped so row 0 of a source image lands at the top.
const SCompositeVertex kCompositeQuad[6] = {
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 1.0f, 0.0f, 1.0f, 1.0f },
    { 0.0f, 1.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f, 0.0f },
    { 1.0f, 0.0f, 1.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f, 0.0f },
};


}  // namespace

// *************************************************************************
// * TRenderer - Lifecycle                                                 *
// *************************************************************************

TRenderer::TRenderer() = default;

TRenderer::~TRenderer()
{
    Shutdown();
}

bool TRenderer::Initialize(int32_t dwidth, int32_t dheight)
{
    if (width > 0)
        return true;

    width  = dwidth;
    height = dheight;

    // The G-buffer is allocated larger than the display (kGBufPad on every
    // side) so off-screen geometry can still contribute to effects that
    // sample neighbors -- sun-shadow ray march in particular.
    const int32_t pad = kGBufPad;
    const int32_t gbw = width  + 2 * pad;
    const int32_t gbh = height + 2 * pad;

    sg_image_desc rt_desc = {};
    rt_desc.width         = gbw;
    rt_desc.height        = gbh;
    rt_desc.sample_count  = 1;
    rt_desc.render_target = true;
    rt_desc.min_filter    = SG_FILTER_LINEAR;
    rt_desc.mag_filter    = SG_FILTER_LINEAR;

    rt_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    color_target = sg_make_image(&rt_desc);

    rt_desc.pixel_format = SG_PIXELFORMAT_DEPTH;
    depth_target = sg_make_image(&rt_desc);

    rt_desc.pixel_format = SG_PIXELFORMAT_RGBA16F;
    normal_target = sg_make_image(&rt_desc);

    rt_desc.pixel_format = SG_PIXELFORMAT_R32F;
    scene_z_target = sg_make_image(&rt_desc);

    // id_target -- per-pixel object instance id, packed into RGBA8.
    // Used by the editor for pixel-perfect picking + selection outline.
    // Pixels covered by no drawable read back as (0,0,0,0) which we
    // reserve for "no object".
    rt_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    rt_desc.min_filter   = SG_FILTER_NEAREST;
    rt_desc.mag_filter   = SG_FILTER_NEAREST;
    id_target = sg_make_image(&rt_desc);
    rt_desc.min_filter   = SG_FILTER_LINEAR;
    rt_desc.mag_filter   = SG_FILTER_LINEAR;

    // lit_target -- final shaded RGBA8. Nearest filter because we composite
    // 1:1 onto the swapchain and don't want resample blur.
    rt_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    rt_desc.min_filter   = SG_FILTER_NEAREST;
    rt_desc.mag_filter   = SG_FILTER_NEAREST;
    lit_target = sg_make_image(&rt_desc);

    rt_desc.pixel_format = SG_PIXELFORMAT_R32F;
    rt_desc.min_filter   = SG_FILTER_LINEAR;
    rt_desc.mag_filter   = SG_FILTER_LINEAR;
    ao_target = sg_make_image(&rt_desc);

    // Pass [1] -- G-buffer MRT fill.
    sg_pass_desc gbuf_desc = {};
    gbuf_desc.color_attachments[0].image     = color_target;
    gbuf_desc.color_attachments[1].image     = normal_target;
    gbuf_desc.color_attachments[2].image     = scene_z_target;
    gbuf_desc.color_attachments[3].image     = id_target;
    gbuf_desc.depth_stencil_attachment.image = depth_target;
    default_pass = sg_make_pass(&gbuf_desc);

    // Pass [3] -- deferred lighting.
    sg_pass_desc lit_desc = {};
    lit_desc.color_attachments[0].image = lit_target;
    lit_pass = sg_make_pass(&lit_desc);

    sg_pass_desc helper_desc = {};
    helper_desc.color_attachments[0].image = lit_target;
    helper_desc.depth_stencil_attachment.image = depth_target;
    helper_pass = sg_make_pass(&helper_desc);

    // Pass [2] -- screen-space AO.
    sg_pass_desc ao_desc = {};
    ao_desc.color_attachments[0].image = ao_target;
    ao_pass = sg_make_pass(&ao_desc);

    // Reserved for a depth-only prepass in Phase 3.
    depth_pass = {};

    InitCompositePipeline();
    InitTilePipeline();
    InitMeshPipeline();
    InitAOPipeline();
    InitLightPipeline();

    return true;
}

void TRenderer::Shutdown()
{
    ShutdownLightPipeline();
    ShutdownAOPipeline();
    ShutdownMeshPipeline();
    ShutdownTilePipeline();
    ShutdownCompositePipeline();

    if (default_pass.id) { sg_destroy_pass(default_pass); default_pass = {}; }
    if (lit_pass.id)     { sg_destroy_pass(lit_pass);     lit_pass     = {}; }
    if (helper_pass.id)  { sg_destroy_pass(helper_pass);  helper_pass  = {}; }
    if (ao_pass.id)      { sg_destroy_pass(ao_pass);      ao_pass      = {}; }

    if (color_target.id)   { sg_destroy_image(color_target);   color_target   = {}; }
    if (depth_target.id)   { sg_destroy_image(depth_target);   depth_target   = {}; }
    if (normal_target.id)  { sg_destroy_image(normal_target);  normal_target  = {}; }
    if (scene_z_target.id) { sg_destroy_image(scene_z_target); scene_z_target = {}; }
    if (id_target.id)      { sg_destroy_image(id_target);      id_target      = {}; }
    if (ao_target.id)      { sg_destroy_image(ao_target);      ao_target      = {}; }
    if (lit_target.id)     { sg_destroy_image(lit_target);     lit_target     = {}; }

    width = height = 0;
    color_target_dirty = false;
    lit_target_dirty   = false;
}

// *************************************************************************
// * Composite Pipeline  (textured quad, UI + swapchain present)           *
// *************************************************************************

void TRenderer::InitCompositePipeline()
{
    sg_buffer_desc vb = {};
    vb.size  = sizeof(kCompositeQuad);
    vb.data  = SG_RANGE(kCompositeQuad);
    vb.label = "renderer.composite.vbuf";
    composite_vbuf = sg_make_buffer(&vb);

    sg_shader_desc sh = {};
    sh.attrs[0].name             = "pos";
    sh.attrs[0].sem_name    = "POSITION";
    sh.attrs[0].sem_index   = 0;
    sh.attrs[1].name             = "uv";
    sh.attrs[1].sem_name    = "TEXCOORD";
    sh.attrs[1].sem_index   = 0;
    sh.vs.source                 = kCompositeVs;
    sh.vs.entry                  = kShaderVsEntry;
    sh.vs.uniform_blocks[0].size = sizeof(float) * 12;
    sh.vs.uniform_blocks[0].uniforms[0].name = "rect";
    sh.vs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
    sh.vs.uniform_blocks[0].uniforms[1].name = "uv_rect";
    sh.vs.uniform_blocks[0].uniforms[1].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.source                 = kCompositeFs;
    sh.fs.entry                  = kShaderFsEntry;
    sh.fs.uniform_blocks[0].size = sizeof(float) * 12;
    sh.fs.uniform_blocks[0].uniforms[0].name = "rect";
    sh.fs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[1].name = "uv_rect";
    sh.fs.uniform_blocks[0].uniforms[1].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[2].name = "chroma_key";
    sh.fs.uniform_blocks[0].uniforms[2].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.images[0].name         = "tex";
    sh.fs.images[0].image_type   = SG_IMAGETYPE_2D;
    sh.fs.images[0].sampler_type = SG_SAMPLERTYPE_FLOAT;
    sh.label = "renderer.composite.shader";
    composite_shader = sg_make_shader(&sh);

    sg_pipeline_desc pip = {};
    pip.shader = composite_shader;
    pip.layout.attrs[0].format  = SG_VERTEXFORMAT_FLOAT2;
    pip.layout.attrs[1].format  = SG_VERTEXFORMAT_FLOAT2;
    pip.primitive_type          = SG_PRIMITIVETYPE_TRIANGLES;
    pip.colors[0].blend.enabled = true;
    pip.colors[0].blend.src_factor_rgb   = SG_BLENDFACTOR_SRC_ALPHA;
    pip.colors[0].blend.dst_factor_rgb   = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    pip.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
    pip.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;

    // Variant 1: drawing into an RGBA8 render target (e.g. the backbuffer).
    pip.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
    pip.depth.pixel_format     = SG_PIXELFORMAT_NONE;
    pip.label = "renderer.composite.pipeline.rt";
    composite_pip_rt = sg_make_pipeline(&pip);

    pip.colors[0].blend.src_factor_rgb   = SG_BLENDFACTOR_ONE;
    pip.colors[0].blend.dst_factor_rgb   = SG_BLENDFACTOR_ONE;
    pip.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
    pip.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE;
    pip.label = "renderer.composite.pipeline.rt.add";
    composite_pip_add_rt = sg_make_pipeline(&pip);

    // Variant 2: drawing into the swapchain (sokol default pass).
    pip.colors[0].blend.src_factor_rgb   = SG_BLENDFACTOR_SRC_ALPHA;
    pip.colors[0].blend.dst_factor_rgb   = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    pip.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
    pip.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    pip.colors[0].pixel_format = _SG_PIXELFORMAT_DEFAULT;
    pip.depth.pixel_format     = _SG_PIXELFORMAT_DEFAULT;
    pip.label = "renderer.composite.pipeline.swap";
    composite_pip_swap = sg_make_pipeline(&pip);
}

void TRenderer::ShutdownCompositePipeline()
{
    if (composite_pip_add_rt.id) { sg_destroy_pipeline(composite_pip_add_rt); composite_pip_add_rt = {}; }
    if (composite_pip_rt.id)   { sg_destroy_pipeline(composite_pip_rt);   composite_pip_rt   = {}; }
    if (composite_pip_swap.id) { sg_destroy_pipeline(composite_pip_swap); composite_pip_swap = {}; }
    if (composite_shader.id)   { sg_destroy_shader(composite_shader);     composite_shader   = {}; }
    if (composite_vbuf.id)     { sg_destroy_buffer(composite_vbuf);       composite_vbuf     = {}; }
}

// *************************************************************************
// * Tile (G-buffer Fill) Pipeline  --  Pass [1]                           *
// *************************************************************************

void TRenderer::InitTilePipeline()
{
    sg_buffer_desc vb = {};
    vb.size  = sizeof(kCompositeQuad);
    vb.data  = SG_RANGE(kCompositeQuad);
    vb.label = "renderer.tile.vbuf";
    tile_vbuf = sg_make_buffer(&vb);

    // 12 vec4s: rect, zparams, tile_root, tile_sprite, filter, obj_id,
    // camera, proj, tile_rect, cam_tile, debug, raycast.
    constexpr int32_t kUB_vec4s = 12;
    constexpr int32_t kUB_bytes = kUB_vec4s * 16;

    sg_shader_desc sh = {};
    sh.attrs[0].name = "pos";
    sh.attrs[0].sem_name    = "POSITION";
    sh.attrs[0].sem_index   = 0;
    sh.attrs[1].name = "uv";
    sh.attrs[1].sem_name    = "TEXCOORD";
    sh.attrs[1].sem_index   = 0;
    sh.vs.source = kTileVs;
    sh.vs.entry  = kShaderVsEntry;
    sh.vs.uniform_blocks[0].size = kUB_bytes;
    sh.vs.uniform_blocks[0].uniforms[0].name = "rect";
    sh.vs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
    sh.vs.uniform_blocks[0].uniforms[1].name = "zparams";
    sh.vs.uniform_blocks[0].uniforms[1].type = SG_UNIFORMTYPE_FLOAT4;
    sh.vs.uniform_blocks[0].uniforms[2].name = "tile_root";
    sh.vs.uniform_blocks[0].uniforms[2].type = SG_UNIFORMTYPE_FLOAT4;
    sh.vs.uniform_blocks[0].uniforms[3].name = "tile_sprite";
    sh.vs.uniform_blocks[0].uniforms[3].type = SG_UNIFORMTYPE_FLOAT4;
    sh.vs.uniform_blocks[0].uniforms[4].name = "filter";
    sh.vs.uniform_blocks[0].uniforms[4].type = SG_UNIFORMTYPE_FLOAT4;
    sh.vs.uniform_blocks[0].uniforms[5].name = "obj_id";
    sh.vs.uniform_blocks[0].uniforms[5].type = SG_UNIFORMTYPE_FLOAT4;
    sh.vs.uniform_blocks[0].uniforms[6].name = "camera";
    sh.vs.uniform_blocks[0].uniforms[6].type = SG_UNIFORMTYPE_FLOAT4;
    sh.vs.uniform_blocks[0].uniforms[7].name = "proj";
    sh.vs.uniform_blocks[0].uniforms[7].type = SG_UNIFORMTYPE_FLOAT4;
    sh.vs.uniform_blocks[0].uniforms[8].name = "tile_rect";
    sh.vs.uniform_blocks[0].uniforms[8].type = SG_UNIFORMTYPE_FLOAT4;
    sh.vs.uniform_blocks[0].uniforms[9].name = "cam_tile";
    sh.vs.uniform_blocks[0].uniforms[9].type = SG_UNIFORMTYPE_FLOAT4;
    sh.vs.uniform_blocks[0].uniforms[10].name = "debug";
    sh.vs.uniform_blocks[0].uniforms[10].type = SG_UNIFORMTYPE_FLOAT4;
    sh.vs.uniform_blocks[0].uniforms[11].name = "raycast";
    sh.vs.uniform_blocks[0].uniforms[11].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.source = kTileFs;
    sh.fs.entry  = kShaderFsEntry;
    sh.fs.uniform_blocks[0].size = kUB_bytes;
    sh.fs.uniform_blocks[0].uniforms[0].name = "rect";
    sh.fs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[1].name = "zparams";
    sh.fs.uniform_blocks[0].uniforms[1].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[2].name = "tile_root";
    sh.fs.uniform_blocks[0].uniforms[2].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[3].name = "tile_sprite";
    sh.fs.uniform_blocks[0].uniforms[3].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[4].name = "filter";
    sh.fs.uniform_blocks[0].uniforms[4].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[5].name = "obj_id";
    sh.fs.uniform_blocks[0].uniforms[5].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[6].name = "camera";
    sh.fs.uniform_blocks[0].uniforms[6].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[7].name = "proj";
    sh.fs.uniform_blocks[0].uniforms[7].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[8].name = "tile_rect";
    sh.fs.uniform_blocks[0].uniforms[8].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[9].name = "cam_tile";
    sh.fs.uniform_blocks[0].uniforms[9].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[10].name = "debug";
    sh.fs.uniform_blocks[0].uniforms[10].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[11].name = "raycast";
    sh.fs.uniform_blocks[0].uniforms[11].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.images[0].name         = "color_tex";
    sh.fs.images[0].image_type   = SG_IMAGETYPE_2D;
    sh.fs.images[0].sampler_type = SG_SAMPLERTYPE_FLOAT;
    sh.fs.images[1].name         = "depth_tex";
    sh.fs.images[1].image_type   = SG_IMAGETYPE_2D;
    sh.fs.images[1].sampler_type = SG_SAMPLERTYPE_FLOAT;
    sh.label = "renderer.tile.shader";
    tile_shader = sg_make_shader(&sh);

    sg_pipeline_desc pip = {};
    pip.shader = tile_shader;
    pip.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT2;
    pip.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT2;
    pip.primitive_type         = SG_PRIMITIVETYPE_TRIANGLES;
    // MRT: albedo (RGBA8) + world normal (RGBA16F) + scene_z (R32F) +
    // obj_id (RGBA8 packed instance id).
    pip.color_count            = 4;
    pip.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
    pip.colors[0].blend.enabled = true;
    pip.colors[0].blend.src_factor_rgb   = SG_BLENDFACTOR_SRC_ALPHA;
    pip.colors[0].blend.dst_factor_rgb   = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    pip.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
    pip.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    pip.colors[1].pixel_format = SG_PIXELFORMAT_RGBA16F;
    pip.colors[1].blend.enabled = false;
    pip.colors[2].pixel_format = SG_PIXELFORMAT_R32F;
    pip.colors[2].blend.enabled = false;
    pip.colors[3].pixel_format = SG_PIXELFORMAT_RGBA8;
    pip.colors[3].blend.enabled = false;
    pip.depth.pixel_format     = SG_PIXELFORMAT_DEPTH;
    pip.depth.compare          = SG_COMPAREFUNC_LESS_EQUAL;
    pip.depth.write_enabled    = true;
    pip.label = "renderer.tile.pipeline";
    tile_pipeline = sg_make_pipeline(&pip);

    sg_shader_desc tsh = sh;
    tsh.fs.source = kTransparentTileFs;
    tsh.fs.entry  = kShaderFsEntry;
    tsh.label = "renderer.transparent_tile.shader";
    transparent_tile_shader = sg_make_shader(&tsh);

    sg_pipeline_desc tpip = {};
    tpip.shader = transparent_tile_shader;
    tpip.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT2;
    tpip.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT2;
    tpip.primitive_type         = SG_PRIMITIVETYPE_TRIANGLES;
    tpip.color_count            = 1;
    tpip.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
    tpip.colors[0].blend.enabled = true;
    tpip.colors[0].blend.src_factor_rgb   = SG_BLENDFACTOR_SRC_ALPHA;
    tpip.colors[0].blend.dst_factor_rgb   = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    tpip.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
    tpip.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    tpip.depth.pixel_format     = SG_PIXELFORMAT_DEPTH;
    tpip.depth.compare          = SG_COMPAREFUNC_LESS_EQUAL;
    tpip.depth.write_enabled    = false;
    tpip.label = "renderer.transparent_tile.pipeline";
    transparent_tile_pipeline = sg_make_pipeline(&tpip);
}

void TRenderer::ShutdownTilePipeline()
{
    if (transparent_tile_pipeline.id) { sg_destroy_pipeline(transparent_tile_pipeline); transparent_tile_pipeline = {}; }
    if (transparent_tile_shader.id)   { sg_destroy_shader(transparent_tile_shader);     transparent_tile_shader   = {}; }
    if (tile_pipeline.id) { sg_destroy_pipeline(tile_pipeline); tile_pipeline = {}; }
    if (tile_shader.id)   { sg_destroy_shader(tile_shader);     tile_shader   = {}; }
    if (tile_vbuf.id)     { sg_destroy_buffer(tile_vbuf);       tile_vbuf     = {}; }
}

// *************************************************************************
// * Mesh Pipeline  --  3D rigid mesh into G-buffer                        *
// *************************************************************************
//
// Writes the same MRT targets as the tile pipeline (albedo / world normal /
// scene_z + HW depth) from real 3D vertices. Per-instance data (world matrix
// + tint) comes through a dynamic instance vertex buffer filled every
// DrainMeshQueue; sorting by MeshHandle groups instances of the same asset
// into single instanced draws.
//
// *************************************************************************

void TRenderer::InitMeshPipeline()
{
    // Shader: 8 attribute slots. Slot 0..2 per-vertex; slot 3..7 per-instance
    // (4 rows of the world matrix + tint). Single uniform block of 3 vec4s
    // (vp / camz / camw) shared VS-side.
    sg_shader_desc sh = {};
    sh.attrs[0].name = "pos";       sh.attrs[0].sem_name = "POSITION"; sh.attrs[0].sem_index = 0;
    sh.attrs[1].name = "normal";    sh.attrs[1].sem_name = "NORMAL";   sh.attrs[1].sem_index = 0;
    sh.attrs[2].name = "uv";        sh.attrs[2].sem_name = "TEXCOORD"; sh.attrs[2].sem_index = 0;
    sh.attrs[3].name = "w0";        sh.attrs[3].sem_name = "TEXCOORD"; sh.attrs[3].sem_index = 1;
    sh.attrs[4].name = "w1";        sh.attrs[4].sem_name = "TEXCOORD"; sh.attrs[4].sem_index = 2;
    sh.attrs[5].name = "w2";        sh.attrs[5].sem_name = "TEXCOORD"; sh.attrs[5].sem_index = 3;
    sh.attrs[6].name = "w3";        sh.attrs[6].sem_name = "TEXCOORD"; sh.attrs[6].sem_index = 4;
    sh.attrs[7].name = "tint";        sh.attrs[7].sem_name = "TEXCOORD"; sh.attrs[7].sem_index = 5;
    sh.attrs[8].name = "inst_obj_id"; sh.attrs[8].sem_name = "TEXCOORD"; sh.attrs[8].sem_index = 6;

    sh.vs.source = kMeshVs;
    sh.vs.entry  = kShaderVsEntry;
    sh.vs.uniform_blocks[0].size = 3 * sizeof(float) * 4;
    sh.vs.uniform_blocks[0].uniforms[0].name = "vp";
    sh.vs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
    sh.vs.uniform_blocks[0].uniforms[1].name = "camz";
    sh.vs.uniform_blocks[0].uniforms[1].type = SG_UNIFORMTYPE_FLOAT4;
    sh.vs.uniform_blocks[0].uniforms[2].name = "camw";
    sh.vs.uniform_blocks[0].uniforms[2].type = SG_UNIFORMTYPE_FLOAT4;

    sh.fs.source = kMeshFs;
    sh.fs.entry  = kShaderFsEntry;
    sh.fs.images[0].name         = "albedo_tex";
    sh.fs.images[0].image_type   = SG_IMAGETYPE_2D;
    sh.fs.images[0].sampler_type = SG_SAMPLERTYPE_FLOAT;
    sh.label = "renderer.mesh.shader";
    mesh_shader = sg_make_shader(&sh);

    sg_pipeline_desc pip = {};
    pip.shader = mesh_shader;
    // Vertex buffer 0: per-vertex (pos/normal/uv).
    pip.layout.buffers[0].stride    = sizeof(SMeshVertex);
    pip.layout.buffers[0].step_func = SG_VERTEXSTEP_PER_VERTEX;
    // Vertex buffer 1: per-instance (world rows + tint + obj_id rgba8).
    pip.layout.buffers[1].stride    = sizeof(float) * 24;   // 4*vec4 world + vec4 tint + vec4 obj_id
    pip.layout.buffers[1].step_func = SG_VERTEXSTEP_PER_INSTANCE;
    pip.layout.attrs[0].buffer_index = 0;
    pip.layout.attrs[0].offset       = offsetof(SMeshVertex, pos);
    pip.layout.attrs[0].format       = SG_VERTEXFORMAT_FLOAT3;
    pip.layout.attrs[1].buffer_index = 0;
    pip.layout.attrs[1].offset       = offsetof(SMeshVertex, normal);
    pip.layout.attrs[1].format       = SG_VERTEXFORMAT_FLOAT3;
    pip.layout.attrs[2].buffer_index = 0;
    pip.layout.attrs[2].offset       = offsetof(SMeshVertex, uv);
    pip.layout.attrs[2].format       = SG_VERTEXFORMAT_FLOAT2;
    for (int32_t i = 0; i < 6; ++i) {
        pip.layout.attrs[3 + i].buffer_index = 1;
        pip.layout.attrs[3 + i].offset       = i * int32_t(sizeof(float)) * 4;
        pip.layout.attrs[3 + i].format       = SG_VERTEXFORMAT_FLOAT4;
    }
    pip.index_type     = SG_INDEXTYPE_UINT16;
    pip.primitive_type = SG_PRIMITIVETYPE_TRIANGLES;
    pip.cull_mode      = SG_CULLMODE_NONE;
    pip.color_count            = 4;
    pip.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
    pip.colors[0].blend.enabled = true;
    pip.colors[0].blend.src_factor_rgb   = SG_BLENDFACTOR_SRC_ALPHA;
    pip.colors[0].blend.dst_factor_rgb   = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    pip.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
    pip.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    pip.colors[1].pixel_format = SG_PIXELFORMAT_RGBA16F;
    pip.colors[1].blend.enabled = false;
    pip.colors[2].pixel_format = SG_PIXELFORMAT_R32F;
    pip.colors[2].blend.enabled = false;
    pip.colors[3].pixel_format = SG_PIXELFORMAT_RGBA8;   // obj_id
    pip.colors[3].blend.enabled = false;
    pip.depth.pixel_format  = SG_PIXELFORMAT_DEPTH;
    pip.depth.compare       = SG_COMPAREFUNC_LESS_EQUAL;
    pip.depth.write_enabled = true;
    pip.label = "renderer.mesh.pipeline";
    mesh_pipeline = sg_make_pipeline(&pip);

    sg_shader_desc hsh = {};
    hsh.attrs[0].name = "pos";       hsh.attrs[0].sem_name = "POSITION"; hsh.attrs[0].sem_index = 0;
    hsh.attrs[1].name = "normal";    hsh.attrs[1].sem_name = "NORMAL";   hsh.attrs[1].sem_index = 0;
    hsh.attrs[2].name = "uv";        hsh.attrs[2].sem_name = "TEXCOORD"; hsh.attrs[2].sem_index = 0;
    hsh.vs.source = kHelperMeshVs;
    hsh.vs.entry  = kShaderVsEntry;
    hsh.vs.uniform_blocks[0].size = 7 * sizeof(float) * 4;
    hsh.vs.uniform_blocks[0].uniforms[0].name = "w0";
    hsh.vs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
    hsh.vs.uniform_blocks[0].uniforms[1].name = "w1";
    hsh.vs.uniform_blocks[0].uniforms[1].type = SG_UNIFORMTYPE_FLOAT4;
    hsh.vs.uniform_blocks[0].uniforms[2].name = "w2";
    hsh.vs.uniform_blocks[0].uniforms[2].type = SG_UNIFORMTYPE_FLOAT4;
    hsh.vs.uniform_blocks[0].uniforms[3].name = "w3";
    hsh.vs.uniform_blocks[0].uniforms[3].type = SG_UNIFORMTYPE_FLOAT4;
    hsh.vs.uniform_blocks[0].uniforms[4].name = "vp";
    hsh.vs.uniform_blocks[0].uniforms[4].type = SG_UNIFORMTYPE_FLOAT4;
    hsh.vs.uniform_blocks[0].uniforms[5].name = "camz";
    hsh.vs.uniform_blocks[0].uniforms[5].type = SG_UNIFORMTYPE_FLOAT4;
    hsh.vs.uniform_blocks[0].uniforms[6].name = "camw";
    hsh.vs.uniform_blocks[0].uniforms[6].type = SG_UNIFORMTYPE_FLOAT4;
    hsh.fs.source = kHelperMeshFs;
    hsh.fs.entry  = kShaderFsEntry;
    hsh.fs.uniform_blocks[0].size = 7 * sizeof(float) * 4;
    hsh.fs.uniform_blocks[0].uniforms[0].name = "diffuse";
    hsh.fs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
    hsh.fs.uniform_blocks[0].uniforms[1].name = "ambient";
    hsh.fs.uniform_blocks[0].uniforms[1].type = SG_UNIFORMTYPE_FLOAT4;
    hsh.fs.uniform_blocks[0].uniforms[2].name = "specular";
    hsh.fs.uniform_blocks[0].uniforms[2].type = SG_UNIFORMTYPE_FLOAT4;
    hsh.fs.uniform_blocks[0].uniforms[3].name = "emissive";
    hsh.fs.uniform_blocks[0].uniforms[3].type = SG_UNIFORMTYPE_FLOAT4;
    hsh.fs.uniform_blocks[0].uniforms[4].name = "light_dir_i";
    hsh.fs.uniform_blocks[0].uniforms[4].type = SG_UNIFORMTYPE_FLOAT4;
    hsh.fs.uniform_blocks[0].uniforms[5].name = "light_col_a";
    hsh.fs.uniform_blocks[0].uniforms[5].type = SG_UNIFORMTYPE_FLOAT4;
    hsh.fs.uniform_blocks[0].uniforms[6].name = "view_dir_power";
    hsh.fs.uniform_blocks[0].uniforms[6].type = SG_UNIFORMTYPE_FLOAT4;
    hsh.fs.images[0].name         = "albedo_tex";
    hsh.fs.images[0].image_type   = SG_IMAGETYPE_2D;
    hsh.fs.images[0].sampler_type = SG_SAMPLERTYPE_FLOAT;
    hsh.label = "renderer.helper.shader";
    helper_mesh_shader = sg_make_shader(&hsh);

    sg_pipeline_desc hpip = {};
    hpip.shader = helper_mesh_shader;
    hpip.layout.buffers[0].stride    = sizeof(SMeshVertex);
    hpip.layout.buffers[0].step_func = SG_VERTEXSTEP_PER_VERTEX;
    hpip.layout.attrs[0].buffer_index = 0;
    hpip.layout.attrs[0].offset       = offsetof(SMeshVertex, pos);
    hpip.layout.attrs[0].format       = SG_VERTEXFORMAT_FLOAT3;
    hpip.layout.attrs[1].buffer_index = 0;
    hpip.layout.attrs[1].offset       = offsetof(SMeshVertex, normal);
    hpip.layout.attrs[1].format       = SG_VERTEXFORMAT_FLOAT3;
    hpip.layout.attrs[2].buffer_index = 0;
    hpip.layout.attrs[2].offset       = offsetof(SMeshVertex, uv);
    hpip.layout.attrs[2].format       = SG_VERTEXFORMAT_FLOAT2;
    hpip.index_type     = SG_INDEXTYPE_UINT16;
    hpip.primitive_type = SG_PRIMITIVETYPE_TRIANGLES;
    hpip.cull_mode      = SG_CULLMODE_FRONT;
    hpip.color_count = 1;
    hpip.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
    hpip.colors[0].blend.enabled = true;
    hpip.colors[0].blend.src_factor_rgb   = SG_BLENDFACTOR_SRC_ALPHA;
    hpip.colors[0].blend.dst_factor_rgb   = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    hpip.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
    hpip.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    hpip.depth.pixel_format  = SG_PIXELFORMAT_DEPTH;
    hpip.depth.compare       = SG_COMPAREFUNC_LESS_EQUAL;
    hpip.depth.write_enabled = false;
    hpip.label = "renderer.helper.back.pipeline";
    helper_mesh_back_pipeline = sg_make_pipeline(&hpip);

    hpip.cull_mode = SG_CULLMODE_BACK;
    hpip.label = "renderer.helper.front.pipeline";
    helper_mesh_front_pipeline = sg_make_pipeline(&hpip);

    hpip.colors[0].blend.src_factor_rgb   = SG_BLENDFACTOR_ONE;
    hpip.colors[0].blend.dst_factor_rgb   = SG_BLENDFACTOR_ONE;
    hpip.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
    hpip.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE;
    hpip.cull_mode = SG_CULLMODE_FRONT;
    hpip.label = "renderer.helper.add.back.pipeline";
    helper_mesh_add_back_pipeline = sg_make_pipeline(&hpip);

    hpip.cull_mode = SG_CULLMODE_BACK;
    hpip.label = "renderer.helper.add.front.pipeline";
    helper_mesh_add_front_pipeline = sg_make_pipeline(&hpip);

    sg_buffer_desc ivb = {};
    ivb.size  = kMaxMeshInstances * int32_t(sizeof(float)) * 24;   // 4*vec4 world + tint + obj_id
    ivb.usage = SG_USAGE_STREAM;
    ivb.label = "renderer.mesh.instance_vbuf";
    mesh_instance_vb = sg_make_buffer(&ivb);
}

void TRenderer::ShutdownMeshPipeline()
{
    for (auto& m : meshes) {
        if (m.vbuf.id) sg_destroy_buffer(m.vbuf);
        if (m.ibuf.id) sg_destroy_buffer(m.ibuf);
    }
    meshes.clear();
    if (mesh_instance_vb.id) { sg_destroy_buffer(mesh_instance_vb); mesh_instance_vb = {}; }
    if (helper_mesh_back_pipeline.id) { sg_destroy_pipeline(helper_mesh_back_pipeline); helper_mesh_back_pipeline = {}; }
    if (helper_mesh_front_pipeline.id) { sg_destroy_pipeline(helper_mesh_front_pipeline); helper_mesh_front_pipeline = {}; }
    if (helper_mesh_add_back_pipeline.id) { sg_destroy_pipeline(helper_mesh_add_back_pipeline); helper_mesh_add_back_pipeline = {}; }
    if (helper_mesh_add_front_pipeline.id) { sg_destroy_pipeline(helper_mesh_add_front_pipeline); helper_mesh_add_front_pipeline = {}; }
    if (helper_mesh_shader.id)   { sg_destroy_shader(helper_mesh_shader); helper_mesh_shader = {}; }
    if (mesh_pipeline.id)    { sg_destroy_pipeline(mesh_pipeline);  mesh_pipeline    = {}; }
    if (mesh_shader.id)      { sg_destroy_shader(mesh_shader);      mesh_shader      = {}; }
}

MeshHandle TRenderer::RegisterMesh(const SMeshVertex* verts, int32_t num_verts,
                                   const uint16_t*  indices, int32_t num_indices,
                                   sg_image albedo)
{
    if (!verts || !indices || num_verts <= 0 || num_indices <= 0) return 0;

    SMeshEntry e = {};
    sg_buffer_desc vbd = {};
    vbd.size  = int(num_verts) * int(sizeof(SMeshVertex));
    vbd.data  = { verts, size_t(vbd.size) };
    vbd.label = "renderer.mesh.asset.vbuf";
    e.vbuf = sg_make_buffer(&vbd);

    sg_buffer_desc ibd = {};
    ibd.type  = SG_BUFFERTYPE_INDEXBUFFER;
    ibd.size  = int(num_indices) * int(sizeof(uint16_t));
    ibd.data  = { indices, size_t(ibd.size) };
    ibd.label = "renderer.mesh.asset.ibuf";
    e.ibuf = sg_make_buffer(&ibd);

    e.num_indices = num_indices;
    e.albedo      = albedo;

    meshes.push_back(e);
    return MeshHandle(meshes.size());   // index+1
}

void TRenderer::SetMeshAlbedo(MeshHandle mesh, sg_image albedo)
{
    if (mesh == 0 || mesh > meshes.size() || !albedo.id)
        return;
    meshes[mesh - 1].albedo = albedo;
}

void TRenderer::SubmitMesh(const SMeshSubmit& m)
{
    if (m.mesh == 0 || m.mesh > meshes.size()) return;
    mesh_queue.push_back(m);
}

void TRenderer::SubmitTransparentTile(const STileSubmit& t)
{
    if (!t.color_img.id || !t.depth_img.id) return;
    STransparentWorldSubmit sub = {};
    sub.kind = ETransparentWorldKind::Tile;
    sub.sort_depth = t.sort_depth;
    sub.tile = t;
    transparent_world_queue.push_back(sub);
}

void TRenderer::SubmitHelperMesh(const SHelperMeshSubmit& m)
{
    if (m.mesh == 0 || m.mesh > meshes.size()) return;
    STransparentWorldSubmit sub = {};
    sub.kind = ETransparentWorldKind::Helper;
    sub.sort_depth = m.sort_depth;
    sub.helper = m;
    transparent_world_queue.push_back(sub);
}

void TRenderer::DrainMeshQueue()
{
    if (mesh_queue.empty() || !mesh_pipeline.id) return;

    // Cap to what the instance buffer can hold.
    if (int32_t(mesh_queue.size()) > kMaxMeshInstances)
        mesh_queue.resize(kMaxMeshInstances);

    // Sort by mesh handle so same-asset instances are contiguous.
    std::sort(mesh_queue.begin(), mesh_queue.end(),
              [](const SMeshSubmit& a, const SMeshSubmit& b) { return a.mesh < b.mesh; });

    // Pack instance rows (24 floats each: w0..w3 + tint + obj_id rgba8).
    std::vector<float> inst;
    inst.resize(mesh_queue.size() * 24);
    for (size_t i = 0; i < mesh_queue.size(); ++i) {
        const SMeshSubmit& s = mesh_queue[i];
        float* dst = &inst[i * 24];
        std::memcpy(dst,      s.world, sizeof(s.world));
        std::memcpy(dst + 16, s.tint,  sizeof(s.tint));
        dst[20] = float((s.obj_id >>  0) & 0xFFu) / 255.0f;
        dst[21] = float((s.obj_id >>  8) & 0xFFu) / 255.0f;
        dst[22] = float((s.obj_id >> 16) & 0xFFu) / 255.0f;
        dst[23] = float((s.obj_id >> 24) & 0xFFu) / 255.0f;
    }
    const sg_range r = { inst.data(), inst.size() * sizeof(float) };
    sg_update_buffer(mesh_instance_vb, &r);

    sg_apply_pipeline(mesh_pipeline);

    // Uniform block: vp/camz/camw.
    float u[12] = {};
    u[0] = recon.ox;
    u[1] = recon.oy;
    u[2] = float(width  + 2 * kGBufPad);
    u[3] = float(height + 2 * kGBufPad);
    u[4] = recon.z_near;
    u[5] = recon.zspan;
    u[6] = recon.kcam_forward;
    u[7] = recon.reserved;
    u[8] = recon.center_wx;
    u[9] = recon.center_wy;
    u[10] = recon.zoom;
    u[11] = 0.0f;
    const sg_range u_range = { u, sizeof(u) };
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &u_range);

    // Emit one instanced draw per contiguous run of equal mesh handles.
    size_t i = 0;
    while (i < mesh_queue.size()) {
        size_t j = i + 1;
        while (j < mesh_queue.size() && mesh_queue[j].mesh == mesh_queue[i].mesh) ++j;

        const SMeshEntry& me = meshes[mesh_queue[i].mesh - 1];
        sg_bindings bind = {};
        bind.vertex_buffers[0]        = me.vbuf;
        bind.vertex_buffers[1]        = mesh_instance_vb;
        bind.vertex_buffer_offsets[1] = int(i) * int(sizeof(float)) * 24;
        bind.index_buffer             = me.ibuf;
        bind.fs_images[0]             = me.albedo;
        sg_apply_bindings(&bind);
        sg_draw(0, me.num_indices, int(j - i));

        i = j;
    }
}

void TRenderer::EmitTransparentTile(const STileSubmit& t)
{
    if (!transparent_tile_pipeline.id || !t.color_img.id || !t.depth_img.id) return;

    sg_apply_pipeline(transparent_tile_pipeline);
    sg_bindings bind = {};
    bind.vertex_buffers[0] = tile_vbuf;
    bind.fs_images[0]      = t.color_img;
    bind.fs_images[1]      = t.depth_img;
    sg_apply_bindings(&bind);

    const int32_t gbw = width  + 2 * kGBufPad;
    const int32_t gbh = height + 2 * kGBufPad;
    const int32_t px  = t.dst_x + kGBufPad;
    const int32_t py  = t.dst_y + kGBufPad;
    float rect_x = float(px);
    float rect_y = float(py);
    float rect_w = float(t.dst_w);
    float rect_h = float(t.dst_h);
    if (recon.reserved > 0.5f)
    {
        const float focal_zoom = recon.kcam_forward * recon.zoom;
        const float anchor_cam_x = float(px) + t.anchor_px_x - recon.ox;
        const float anchor_cam_y = float(py) + t.anchor_px_y - recon.oy;
        float z0 = t.anchor_z * recon.zspan + recon.z_near + recon.z_offset + t.zraw_min * t.zraw_to_wu * recon.z_scale;
        float z1 = t.anchor_z * recon.zspan + recon.z_near + recon.z_offset + t.zraw_max * t.zraw_to_wu * recon.z_scale;
        if (z1 < z0) std::swap(z0, z1);
        z0 = (std::max)(z0, 1.0f);
        z1 = (std::max)(z1, z0 + 1.0f);
        const float tile_scale = recon.tile_scale > 0.0f ? recon.tile_scale : 1.0f;
        const float sx_min = -t.anchor_px_x * tile_scale;
        const float sx_max = (float(t.dst_w) - t.anchor_px_x) * tile_scale;
        const float sy_min = -t.anchor_px_y * tile_scale;
        const float sy_max = (float(t.dst_h) - t.anchor_px_y) * tile_scale;
        const float sxv[2] = { sx_min, sx_max };
        const float syv[2] = { sy_min, sy_max };
        const float zv[2] = { z0, z1 };
        float min_x =  FLT_MAX, min_y =  FLT_MAX;
        float max_x = -FLT_MAX, max_y = -FLT_MAX;
        for (float z : zv)
        for (float sx : sxv)
        for (float sy : syv)
        {
            const float qx = recon.ox + (anchor_cam_x + sx) * focal_zoom / z;
            const float qy = recon.oy + (anchor_cam_y + sy) * focal_zoom / z;
            min_x = (std::min)(min_x, qx);
            min_y = (std::min)(min_y, qy);
            max_x = (std::max)(max_x, qx);
            max_y = (std::max)(max_y, qy);
        }
        constexpr float kPerspectiveTilePadPx = 2.0f;
        rect_x = std::floor(min_x - kPerspectiveTilePadPx);
        rect_y = std::floor(min_y - kPerspectiveTilePadPx);
        rect_w = std::ceil(max_x + kPerspectiveTilePadPx) - rect_x;
        rect_h = std::ceil(max_y + kPerspectiveTilePadPx) - rect_y;
    }
    const float nx = (2.0f * rect_x / gbw)  - 1.0f;
    const float nw = (2.0f * rect_w) / gbw;
    const float ny = 1.0f - (2.0f * (rect_y + rect_h) / gbh);
    const float nh = (2.0f * rect_h) / gbh;

    // Keep layout identical to the opaque tile shader. Transparent tiles do
    // not write obj_id, but still need the shared camera projection params.
    float uniforms[12 * 4] = {};
    int32_t off = 0;
    uniforms[off++] = nx; uniforms[off++] = ny; uniforms[off++] = nw; uniforms[off++] = nh;
    uniforms[off++] = t.anchor_z;
    uniforms[off++] = t.depth_mul;
    uniforms[off++] = recon.tile_scale;
    uniforms[off++] = recon.zoom;
    uniforms[off++] = t.root_wx; uniforms[off++] = t.root_wy;
    uniforms[off++] = t.root_wz; uniforms[off++] = t.zraw_to_wu;
    uniforms[off++] = t.anchor_px_x; uniforms[off++] = t.anchor_px_y;
    uniforms[off++] = float(t.dst_w); uniforms[off++] = float(t.dst_h);
    uniforms[off++] = light.normal_radius;
    uniforms[off++] = light.edge_threshold;
    uniforms[off++] = t.zraw_min;
    uniforms[off++] = t.zraw_max;
    // obj_id slot: zeros for transparent tiles (no need to pick them).
    uniforms[off++] = 0.0f; uniforms[off++] = 0.0f;
    uniforms[off++] = 0.0f; uniforms[off++] = 0.0f;
    uniforms[off++] = recon.center_wx; uniforms[off++] = recon.center_wy;
    uniforms[off++] = recon.kcam_forward; uniforms[off++] = recon.reserved;
    uniforms[off++] = recon.ox; uniforms[off++] = recon.oy;
    uniforms[off++] = float(gbw); uniforms[off++] = float(gbh);
    uniforms[off++] = float(px); uniforms[off++] = float(py);
    uniforms[off++] = float(t.dst_w); uniforms[off++] = float(t.dst_h);
    uniforms[off++] = float(px) + t.anchor_px_x - recon.ox;
    uniforms[off++] = float(py) + t.anchor_px_y - recon.oy;
    uniforms[off++] = t.anchor_z * recon.zspan + recon.z_near + recon.z_offset;
    uniforms[off++] = t.zraw_to_wu * recon.z_scale;
    {
        const uint32_t h = t.obj_id * 1103515245u + 12345u;
        const float r = 0.25f + 0.70f * float((h >>  0) & 0xFFu) / 255.0f;
        const float g = 0.25f + 0.70f * float((h >>  8) & 0xFFu) / 255.0f;
        const float b = 0.25f + 0.70f * float((h >> 16) & 0xFFu) / 255.0f;
        uniforms[off++] = float(perspective_debug_mode);
        uniforms[off++] = r; uniforms[off++] = g; uniforms[off++] = b;
    }
    uniforms[off++] = float(perspective_steps);
    uniforms[off++] = float(perspective_refine);
    uniforms[off++] = 0.0f;
    uniforms[off++] = 0.0f;

    const sg_range u_range = { uniforms, sizeof(uniforms) };
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &u_range);
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, &u_range);
    sg_draw(0, 6, 1);
}

void TRenderer::EmitTransparentHelper(const SHelperMeshSubmit& s)
{
    sg_pipeline back_pipeline = s.additive_blend ? helper_mesh_add_back_pipeline : helper_mesh_back_pipeline;
    sg_pipeline front_pipeline = s.additive_blend ? helper_mesh_add_front_pipeline : helper_mesh_front_pipeline;
    if (!back_pipeline.id || !front_pipeline.id) return;

    const float view_dir[4] = { -0.5f, -0.5f, 1.0f, 1.0f };
    const SMeshEntry& me = meshes[s.mesh - 1];

    sg_apply_pipeline(back_pipeline);
    sg_bindings bind = {};
    bind.vertex_buffers[0] = me.vbuf;
    bind.index_buffer = me.ibuf;
    bind.fs_images[0] = me.albedo;
    sg_apply_bindings(&bind);

    float vsu[28] = {};
    int vo = 0;
    std::memcpy(&vsu[vo], s.world + 0,  sizeof(float) * 4); vo += 4;
    std::memcpy(&vsu[vo], s.world + 4,  sizeof(float) * 4); vo += 4;
    std::memcpy(&vsu[vo], s.world + 8,  sizeof(float) * 4); vo += 4;
    std::memcpy(&vsu[vo], s.world + 12, sizeof(float) * 4); vo += 4;
    vsu[vo++] = recon.ox;
    vsu[vo++] = recon.oy;
    vsu[vo++] = float(width  + 2 * kGBufPad);
    vsu[vo++] = float(height + 2 * kGBufPad);
    vsu[vo++] = recon.z_near;
    vsu[vo++] = recon.zspan;
    vsu[vo++] = recon.kcam_forward;
    vsu[vo++] = recon.reserved;
    vsu[vo++] = recon.center_wx;
    vsu[vo++] = recon.center_wy;
    vsu[vo++] = recon.zoom;
    vsu[vo++] = 0.0f;
    const sg_range vsr = { vsu, sizeof(vsu) };
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &vsr);

    float fsu[28] = {};
    int o = 0;
    std::memcpy(&fsu[o], s.diffuse, sizeof(s.diffuse)); o += 4;
    std::memcpy(&fsu[o], s.ambient, sizeof(s.ambient)); o += 4;
    std::memcpy(&fsu[o], s.specular, sizeof(s.specular)); o += 4;
    std::memcpy(&fsu[o], s.emissive, sizeof(s.emissive)); o += 4;
    fsu[o++] = light.dir[0]; fsu[o++] = light.dir[1]; fsu[o++] = light.dir[2]; fsu[o++] = light.intensity;
    fsu[o++] = light.color[0]; fsu[o++] = light.color[1]; fsu[o++] = light.color[2]; fsu[o++] = light.ambient;
    fsu[o++] = view_dir[0]; fsu[o++] = view_dir[1]; fsu[o++] = view_dir[2]; fsu[o++] = s.power;
    const sg_range fsr = { fsu, sizeof(fsu) };
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, &fsr);

    sg_draw(0, me.num_indices, 1);
    sg_apply_pipeline(front_pipeline);
    sg_apply_bindings(&bind);
    sg_draw(0, me.num_indices, 1);
}

void TRenderer::DrainTransparentWorldQueue()
{
    if (transparent_world_queue.empty() || !helper_pass.id) return;

    std::stable_sort(transparent_world_queue.begin(), transparent_world_queue.end(),
                     [](const STransparentWorldSubmit& a, const STransparentWorldSubmit& b)
                     {
                         return a.sort_depth < b.sort_depth;
                     });

    sg_pass_action pa = {};
    pa.colors[0].action = SG_ACTION_LOAD;
    pa.depth.action = SG_ACTION_LOAD;
    pa.stencil.action = SG_ACTION_DONTCARE;
    sg_begin_pass(helper_pass, &pa);
    for (const auto& s : transparent_world_queue)
    {
        if (s.kind == ETransparentWorldKind::Tile)
            EmitTransparentTile(s.tile);
        else
            EmitTransparentHelper(s.helper);
    }
    sg_end_pass();
    transparent_world_queue.clear();
    lit_target_dirty = true;
}

// *************************************************************************
// * Screen-Space Ambient Occlusion  --  Pass [2]                          *
// *************************************************************************

void TRenderer::InitAOPipeline()
{
    sg_shader_desc sh = {};
    sh.attrs[0].name = "pos";
    sh.attrs[0].sem_name    = "POSITION";
    sh.attrs[0].sem_index   = 0;
    sh.attrs[1].name = "uv";
    sh.attrs[1].sem_name    = "TEXCOORD";
    sh.attrs[1].sem_index   = 0;
    sh.vs.source = kAOVs;
    sh.vs.entry  = kShaderVsEntry;
    sh.fs.source = kAOFs;
    sh.fs.entry  = kShaderFsEntry;
    sh.fs.uniform_blocks[0].size = sizeof(float) * 16;
    sh.fs.uniform_blocks[0].uniforms[0].name = "vp";
    sh.fs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[1].name = "recon";
    sh.fs.uniform_blocks[0].uniforms[1].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[2].name = "ao_settings";
    sh.fs.uniform_blocks[0].uniforms[2].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[3].name = "camera";
    sh.fs.uniform_blocks[0].uniforms[3].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.images[0].name         = "albedo_tex";
    sh.fs.images[0].image_type   = SG_IMAGETYPE_2D;
    sh.fs.images[0].sampler_type = SG_SAMPLERTYPE_FLOAT;
    sh.fs.images[1].name         = "normal_tex";
    sh.fs.images[1].image_type   = SG_IMAGETYPE_2D;
    sh.fs.images[1].sampler_type = SG_SAMPLERTYPE_FLOAT;
    sh.fs.images[2].name         = "depth_tex";
    sh.fs.images[2].image_type   = SG_IMAGETYPE_2D;
    sh.fs.images[2].sampler_type = SG_SAMPLERTYPE_FLOAT;
    sh.label = "renderer.ao.shader";
    ao_shader = sg_make_shader(&sh);

    sg_pipeline_desc pip = {};
    pip.shader = ao_shader;
    pip.layout.attrs[0].format  = SG_VERTEXFORMAT_FLOAT2;
    pip.layout.attrs[1].format  = SG_VERTEXFORMAT_FLOAT2;
    pip.primitive_type          = SG_PRIMITIVETYPE_TRIANGLES;
    pip.colors[0].pixel_format  = SG_PIXELFORMAT_R32F;
    pip.colors[0].blend.enabled = false;
    pip.depth.pixel_format      = SG_PIXELFORMAT_NONE;
    pip.label = "renderer.ao.pipeline";
    ao_pipeline = sg_make_pipeline(&pip);
}

void TRenderer::ShutdownAOPipeline()
{
    if (ao_pipeline.id) { sg_destroy_pipeline(ao_pipeline); ao_pipeline = {}; }
    if (ao_shader.id)   { sg_destroy_shader(ao_shader);     ao_shader   = {}; }
}

void TRenderer::RunAOPass()
{
    if (!ao_pass.id || !ao_pipeline.id) return;

    sg_pass_action pa = {};
    pa.colors[0].action = SG_ACTION_CLEAR;
    pa.colors[0].value  = { 1.0f, 0.0f, 0.0f, 1.0f };
    sg_begin_pass(ao_pass, &pa);

    sg_apply_pipeline(ao_pipeline);
    sg_bindings bind = {};
    bind.vertex_buffers[0] = composite_vbuf;
    bind.fs_images[0]      = color_target;
    bind.fs_images[1]      = normal_target;
    bind.fs_images[2]      = scene_z_target;
    sg_apply_bindings(&bind);

    float u[16] = {};
    int32_t o = 0;
    u[o++] = recon.ox; u[o++] = recon.oy; u[o++] = recon.z_near; u[o++] = recon.zspan;
    u[o++] = recon.center_wx; u[o++] = recon.center_wy; u[o++] = recon.kcam_forward; u[o++] = recon.reserved;
    u[o++] = light.ao_radius_px;
    u[o++] = light.ao_enable ? light.ao_strength : 0.0f;
    u[o++] = light.ao_bias;
    u[o++] = light.ao_max_dist_wu;
    u[o++] = recon.zoom;
    u[o++] = 0.0f;
    u[o++] = 0.0f;
    u[o++] = 0.0f;
    const sg_range r = { u, sizeof(u) };
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, &r);
    sg_draw(0, 6, 1);
    sg_end_pass();
}

// *************************************************************************
// * Deferred Lighting Pipeline  --  Pass [3]                              *
// *************************************************************************

void TRenderer::InitLightPipeline()
{
    constexpr int32_t kPL = TRenderer::kMaxPointLights;
    // 6 scalar vec4s + kPL plight_pos + kPL plight_col
    //                + shadow + shadow_dir + shadow_world_dir + normal_lighting
    constexpr int32_t kUB_vec4s = 6 + kPL + kPL + 4 + 1;   // +1 = selected_obj_id (editor outline)
    constexpr int32_t kUB_bytes = kUB_vec4s * 16;

    sg_shader_desc sh = {};
    sh.attrs[0].name = "pos";
    sh.attrs[0].sem_name    = "POSITION";
    sh.attrs[0].sem_index   = 0;
    sh.attrs[1].name = "uv";
    sh.attrs[1].sem_name    = "TEXCOORD";
    sh.attrs[1].sem_index   = 0;
    sh.vs.source = kLightVs;
    sh.vs.entry  = kShaderVsEntry;
    sh.fs.source = kLightFs;
    sh.fs.entry  = kShaderFsEntry;
    sh.fs.uniform_blocks[0].size = kUB_bytes;
    sh.fs.uniform_blocks[0].uniforms[0].name = "vp";
    sh.fs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[1].name = "recon";
    sh.fs.uniform_blocks[0].uniforms[1].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[2].name = "light_dir";
    sh.fs.uniform_blocks[0].uniforms[2].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[3].name = "light_col";
    sh.fs.uniform_blocks[0].uniforms[3].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[4].name = "ambient_col";
    sh.fs.uniform_blocks[0].uniforms[4].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[5].name = "settings";
    sh.fs.uniform_blocks[0].uniforms[5].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[6].name = "plight_pos";
    sh.fs.uniform_blocks[0].uniforms[6].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[6].array_count = kPL;
    sh.fs.uniform_blocks[0].uniforms[7].name = "plight_col";
    sh.fs.uniform_blocks[0].uniforms[7].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[7].array_count = kPL;
    sh.fs.uniform_blocks[0].uniforms[8].name = "shadow";
    sh.fs.uniform_blocks[0].uniforms[8].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[9].name = "shadow_dir";
    sh.fs.uniform_blocks[0].uniforms[9].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[10].name = "shadow_world_dir";
    sh.fs.uniform_blocks[0].uniforms[10].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[11].name = "normal_lighting";
    sh.fs.uniform_blocks[0].uniforms[11].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[12].name = "selected_obj_id";
    sh.fs.uniform_blocks[0].uniforms[12].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.images[0].name         = "albedo_tex";
    sh.fs.images[0].image_type   = SG_IMAGETYPE_2D;
    sh.fs.images[0].sampler_type = SG_SAMPLERTYPE_FLOAT;
    sh.fs.images[1].name         = "normal_tex";
    sh.fs.images[1].image_type   = SG_IMAGETYPE_2D;
    sh.fs.images[1].sampler_type = SG_SAMPLERTYPE_FLOAT;
    sh.fs.images[2].name         = "depth_tex";
    sh.fs.images[2].image_type   = SG_IMAGETYPE_2D;
    sh.fs.images[2].sampler_type = SG_SAMPLERTYPE_FLOAT;
    sh.fs.images[3].name         = "ao_tex";
    sh.fs.images[3].image_type   = SG_IMAGETYPE_2D;
    sh.fs.images[3].sampler_type = SG_SAMPLERTYPE_FLOAT;
    sh.fs.images[4].name         = "id_tex";
    sh.fs.images[4].image_type   = SG_IMAGETYPE_2D;
    sh.fs.images[4].sampler_type = SG_SAMPLERTYPE_FLOAT;
    sh.label = "renderer.light.shader";
    light_shader = sg_make_shader(&sh);

    sg_pipeline_desc pip = {};
    pip.shader = light_shader;
    pip.layout.attrs[0].format  = SG_VERTEXFORMAT_FLOAT2;
    pip.layout.attrs[1].format  = SG_VERTEXFORMAT_FLOAT2;
    pip.primitive_type          = SG_PRIMITIVETYPE_TRIANGLES;
    pip.colors[0].pixel_format  = SG_PIXELFORMAT_RGBA8;
    pip.colors[0].blend.enabled = false;
    pip.depth.pixel_format      = SG_PIXELFORMAT_NONE;
    pip.label = "renderer.light.pipeline";
    light_pipeline = sg_make_pipeline(&pip);
}

void TRenderer::ShutdownLightPipeline()
{
    if (light_pipeline.id) { sg_destroy_pipeline(light_pipeline); light_pipeline = {}; }
    if (light_shader.id)   { sg_destroy_shader(light_shader);     light_shader   = {}; }
}

void TRenderer::SetReconstructionParams(float ox, float oy,
                                        float z_near, float z_far,
                                        float center_wx, float center_wy,
                                        float kcam_forward, float perspective,
                                        float zoom,
                                        float z_offset,
                                        float z_scale,
                                        float tile_scale)
{
    // Callers pass the world-origin screen offset in display-space pixels.
    // The light shader samples the padded G-buffer, so shift by +pad to
    // keep the world->screen math consistent.
    recon.ox           = ox + float(kGBufPad);
    recon.oy           = oy + float(kGBufPad);
    recon.z_near       = z_near;
    recon.zspan        = z_far - z_near;
    recon.center_wx    = center_wx;
    recon.center_wy    = center_wy;
    recon.kcam_forward = kcam_forward;
    recon.reserved     = perspective;
    recon.zoom         = zoom > 0.0f ? zoom : 1.0f;
    recon.z_offset     = z_offset;
    recon.z_scale      = z_scale > 0.0f ? z_scale : 1.0f;
    recon.tile_scale   = tile_scale > 0.0f ? tile_scale : 1.0f;
}

void TRenderer::SetPerspectiveRaycastParams(int32_t steps, int32_t refine)
{
    perspective_steps = std::clamp(steps, 4, 128);
    perspective_refine = std::clamp(refine, 0, 8);
}

void TRenderer::RunLightingPass()
{
    if (!lit_pass.id || !light_pipeline.id) return;

    RunAOPass();

    sg_pass_action pa = {};
    pa.colors[0].action = SG_ACTION_CLEAR;
    pa.colors[0].value  = { tile_clear_rgba[0], tile_clear_rgba[1],
                            tile_clear_rgba[2], tile_clear_rgba[3] };
    sg_begin_pass(lit_pass, &pa);

    sg_apply_pipeline(light_pipeline);
    sg_bindings bind = {};
    bind.vertex_buffers[0] = composite_vbuf;
    bind.fs_images[0]      = color_target;
    bind.fs_images[1]      = normal_target;
    bind.fs_images[2]      = scene_z_target;
    bind.fs_images[3]      = ao_target;
    bind.fs_images[4]      = id_target;
    sg_apply_bindings(&bind);

    constexpr int32_t kPL = TRenderer::kMaxPointLights;
    constexpr int32_t kUB_vec4s = 6 + kPL + kPL + 4 + 1;   // +1 = selected_obj_id (editor outline)
    float u[kUB_vec4s * 4] = {};
    int32_t o = 0;
    u[o++] = recon.ox;    u[o++] = recon.oy;
    u[o++] = recon.z_near; u[o++] = recon.zspan;
    u[o++] = recon.center_wx; u[o++] = recon.center_wy;
    u[o++] = recon.kcam_forward; u[o++] = recon.reserved;
    u[o++] = light.dir[0]; u[o++] = light.dir[1]; u[o++] = light.dir[2]; u[o++] = light.intensity;
    u[o++] = light.color[0]; u[o++] = light.color[1]; u[o++] = light.color[2]; u[o++] = light.ambient;
    u[o++] = light.ambient_color[0]; u[o++] = light.ambient_color[1];
    u[o++] = light.ambient_color[2]; u[o++] = 0.0f;
    u[o++] = float(light.view_mode);
    u[o++] = float(light.plight_count);
    u[o++] = float(light.mode);
    u[o++] = recon.zoom;
    for (int32_t i = 0; i < kPL; ++i)
        for (int32_t k = 0; k < 4; ++k) u[o++] = light.plight_pos[i][k];
    for (int32_t i = 0; i < kPL; ++i)
        for (int32_t k = 0; k < 4; ++k) u[o++] = light.plight_col[i][k];
    u[o++] = light.sun_shadow_step_wu;
    u[o++] = light.sun_shadow_softness_px;
    u[o++] = float(light.sun_shadow_max_steps);
    u[o++] = light.sun_shadow_enable ? 1.0f : 0.0f;
    u[o++] = light.shadow_dir[0];
    u[o++] = light.shadow_dir[1];
    u[o++] = light.shadow_dir[2];
    u[o++] = 0.0f;
    u[o++] = light.shadow_world_dir[0];
    u[o++] = light.shadow_world_dir[1];
    u[o++] = light.shadow_world_dir[2];
    u[o++] = 0.0f;
    u[o++] = light.normal_lighting_hardness;
    u[o++] = 0.0f;
    u[o++] = 0.0f;
    u[o++] = 0.0f;
    // selected_obj_id packed into RGBA8 channels for the outline shader.
    // 0 means "no selection -- skip outline".
    u[o++] = float((selected_obj_id >>  0) & 0xFFu) / 255.0f;
    u[o++] = float((selected_obj_id >>  8) & 0xFFu) / 255.0f;
    u[o++] = float((selected_obj_id >> 16) & 0xFFu) / 255.0f;
    u[o++] = float((selected_obj_id >> 24) & 0xFFu) / 255.0f;

    const sg_range r = { u, sizeof(u) };
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, &r);
    sg_draw(0, 6, 1);
    sg_end_pass();

    DrainTransparentWorldQueue();
    DrainOverlayQueue();
    lit_target_dirty = true;
}

// *************************************************************************
// * Tile Pass Submission  (BeginTilePass / SubmitTile / EndTilePass)      *
// *************************************************************************

void TRenderer::BeginTilePass(float r, float g, float b, float a)
{
    if (!default_pass.id) return;
    tile_clear_rgba[0] = r; tile_clear_rgba[1] = g;
    tile_clear_rgba[2] = b; tile_clear_rgba[3] = a;
    tile_queue.clear();
    transparent_world_queue.clear();
    overlay_queue.clear();
    sg_pass_action pa = {};
    pa.colors[0].action = SG_ACTION_CLEAR;
    pa.colors[0].value  = { 0.0f, 0.0f, 0.0f, 0.0f };
    pa.colors[1].action = SG_ACTION_CLEAR;
    pa.colors[1].value  = { 0.5f, 0.5f, 0.5f, 1.0f };
    pa.colors[2].action = SG_ACTION_CLEAR;
    pa.colors[2].value  = { 1.0f, 0.0f, 0.0f, 1.0f };
    pa.colors[3].action = SG_ACTION_CLEAR;
    pa.colors[3].value  = { 0.0f, 0.0f, 0.0f, 0.0f };  // id=0 means "empty"
    pa.depth.action     = SG_ACTION_CLEAR;
    pa.depth.value      = 1.0f;
    pa.stencil.action   = SG_ACTION_DONTCARE;
    sg_begin_pass(default_pass, &pa);
    color_target_dirty = true;
}

void TRenderer::SubmitTile(const STileSubmit& t)
{
    tile_queue.push_back(t);
}

void TRenderer::SubmitOverlay(const SOverlaySubmit& t)
{
    if (!t.color_img.id) return;
    overlay_queue.push_back(t);
}

void TRenderer::EmitTile(const STileSubmit& t)
{
    if (!tile_pipeline.id || !t.color_img.id || !t.depth_img.id) return;
    if (width <= 0 || height <= 0) return;

    sg_apply_pipeline(tile_pipeline);
    sg_bindings bind = {};
    bind.vertex_buffers[0] = tile_vbuf;
    bind.fs_images[0]      = t.color_img;
    bind.fs_images[1]      = t.depth_img;
    sg_apply_bindings(&bind);

    const int32_t gbw = width  + 2 * kGBufPad;
    const int32_t gbh = height + 2 * kGBufPad;
    const int32_t px  = t.dst_x + kGBufPad;
    const int32_t py  = t.dst_y + kGBufPad;
    float rect_x = float(px);
    float rect_y = float(py);
    float rect_w = float(t.dst_w);
    float rect_h = float(t.dst_h);
    if (recon.reserved > 0.5f)
    {
        const float focal_zoom = recon.kcam_forward * recon.zoom;
        const float anchor_cam_x = float(px) + t.anchor_px_x - recon.ox;
        const float anchor_cam_y = float(py) + t.anchor_px_y - recon.oy;
        float z0 = t.anchor_z * recon.zspan + recon.z_near + recon.z_offset + t.zraw_min * t.zraw_to_wu * recon.z_scale;
        float z1 = t.anchor_z * recon.zspan + recon.z_near + recon.z_offset + t.zraw_max * t.zraw_to_wu * recon.z_scale;
        if (z1 < z0) std::swap(z0, z1);
        z0 = (std::max)(z0, 1.0f);
        z1 = (std::max)(z1, z0 + 1.0f);
        const float tile_scale = recon.tile_scale > 0.0f ? recon.tile_scale : 1.0f;
        const float sx_min = -t.anchor_px_x * tile_scale;
        const float sx_max = (float(t.dst_w) - t.anchor_px_x) * tile_scale;
        const float sy_min = -t.anchor_px_y * tile_scale;
        const float sy_max = (float(t.dst_h) - t.anchor_px_y) * tile_scale;
        const float sxv[2] = { sx_min, sx_max };
        const float syv[2] = { sy_min, sy_max };
        const float zv[2] = { z0, z1 };
        float min_x =  FLT_MAX, min_y =  FLT_MAX;
        float max_x = -FLT_MAX, max_y = -FLT_MAX;
        for (float z : zv)
        for (float sx : sxv)
        for (float sy : syv)
        {
            const float qx = recon.ox + (anchor_cam_x + sx) * focal_zoom / z;
            const float qy = recon.oy + (anchor_cam_y + sy) * focal_zoom / z;
            min_x = (std::min)(min_x, qx);
            min_y = (std::min)(min_y, qy);
            max_x = (std::max)(max_x, qx);
            max_y = (std::max)(max_y, qy);
        }
        constexpr float kPerspectiveTilePadPx = 2.0f;
        rect_x = std::floor(min_x - kPerspectiveTilePadPx);
        rect_y = std::floor(min_y - kPerspectiveTilePadPx);
        rect_w = std::ceil(max_x + kPerspectiveTilePadPx) - rect_x;
        rect_h = std::ceil(max_y + kPerspectiveTilePadPx) - rect_y;
    }
    const float nx = (2.0f * rect_x / gbw)  - 1.0f;
    const float nw = (2.0f * rect_w) / gbw;
    const float ny = 1.0f - (2.0f * (rect_y + rect_h) / gbh);
    const float nh = (2.0f * rect_h) / gbh;

    float uniforms[12 * 4] = {};
    int32_t off = 0;
    uniforms[off++] = nx; uniforms[off++] = ny; uniforms[off++] = nw; uniforms[off++] = nh;
    uniforms[off++] = t.anchor_z;
    uniforms[off++] = t.depth_mul;
    uniforms[off++] = recon.tile_scale;
    uniforms[off++] = recon.zoom;
    uniforms[off++] = t.root_wx; uniforms[off++] = t.root_wy;
    uniforms[off++] = t.root_wz; uniforms[off++] = t.zraw_to_wu;
    uniforms[off++] = t.anchor_px_x; uniforms[off++] = t.anchor_px_y;
    uniforms[off++] = float(t.dst_w); uniforms[off++] = float(t.dst_h);
    uniforms[off++] = light.normal_radius;
    uniforms[off++] = light.edge_threshold;
    uniforms[off++] = t.zraw_min;
    uniforms[off++] = t.zraw_max;
    // obj_id packed into 4 bytes (RGBA8) for the gbuffer ID target.
    uniforms[off++] = float((t.obj_id >>  0) & 0xFFu) / 255.0f;
    uniforms[off++] = float((t.obj_id >>  8) & 0xFFu) / 255.0f;
    uniforms[off++] = float((t.obj_id >> 16) & 0xFFu) / 255.0f;
    uniforms[off++] = float((t.obj_id >> 24) & 0xFFu) / 255.0f;
    uniforms[off++] = recon.center_wx; uniforms[off++] = recon.center_wy;
    uniforms[off++] = recon.kcam_forward; uniforms[off++] = recon.reserved;
    uniforms[off++] = recon.ox; uniforms[off++] = recon.oy;
    uniforms[off++] = float(gbw); uniforms[off++] = float(gbh);
    uniforms[off++] = float(px); uniforms[off++] = float(py);
    uniforms[off++] = float(t.dst_w); uniforms[off++] = float(t.dst_h);
    uniforms[off++] = float(px) + t.anchor_px_x - recon.ox;
    uniforms[off++] = float(py) + t.anchor_px_y - recon.oy;
    uniforms[off++] = t.anchor_z * recon.zspan + recon.z_near + recon.z_offset;
    uniforms[off++] = t.zraw_to_wu * recon.z_scale;
    {
        const uint32_t h = t.obj_id * 1103515245u + 12345u;
        const float r = 0.25f + 0.70f * float((h >>  0) & 0xFFu) / 255.0f;
        const float g = 0.25f + 0.70f * float((h >>  8) & 0xFFu) / 255.0f;
        const float b = 0.25f + 0.70f * float((h >> 16) & 0xFFu) / 255.0f;
        uniforms[off++] = float(perspective_debug_mode);
        uniforms[off++] = r; uniforms[off++] = g; uniforms[off++] = b;
    }
    uniforms[off++] = float(perspective_steps);
    uniforms[off++] = float(perspective_refine);
    uniforms[off++] = 0.0f;
    uniforms[off++] = 0.0f;

    const sg_range u_range = { uniforms, sizeof(uniforms) };
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &u_range);
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, &u_range);
    sg_draw(0, 6, 1);
}

void TRenderer::DrainOverlayQueue()
{
    if (overlay_queue.empty() || !lit_pass.id) return;
    sg_pass_action pa = {};
    pa.colors[0].action = SG_ACTION_LOAD;
    pa.depth.action = SG_ACTION_DONTCARE;
    pa.stencil.action = SG_ACTION_DONTCARE;
    sg_begin_pass(lit_pass, &pa);
    const int32_t gbw = width + 2 * kGBufPad;
    const int32_t gbh = height + 2 * kGBufPad;
    for (const auto& t : overlay_queue)
        Composite(t.color_img,
                  t.dst_x + kGBufPad, t.dst_y + kGBufPad,
                  t.dst_w, t.dst_h,
                  gbw, gbh,
                  t.src_x, t.src_y, t.src_w, t.src_h,
                  t.src_tex_w, t.src_tex_h,
                  t.additive_blend,
                  t.chroma_key,
                  t.chroma_key_rgb);
    sg_end_pass();
    overlay_queue.clear();
    lit_target_dirty = true;
}

void TRenderer::EndTilePass()
{
    for (const auto& t : tile_queue)
        EmitTile(t);
    DrainMeshQueue();
    mesh_queue.clear();
    sg_end_pass();
}

// *************************************************************************
// * Lighting State Setters                                                *
// *************************************************************************

void TRenderer::SetLight(float dx, float dy, float dz, float intensity,
                         float r, float g, float b, float ambient)
{
    light.dir[0] = dx; light.dir[1] = dy; light.dir[2] = dz;
    light.shadow_world_dir[0] = dx;
    light.shadow_world_dir[1] = dy;
    light.shadow_world_dir[2] = dz;
    light.intensity   = intensity;
    light.color[0] = r; light.color[1] = g; light.color[2] = b;
    light.ambient     = ambient;
}

void TRenderer::SetAmbientColor(float r, float g, float b)
{
    light.ambient_color[0] = r;
    light.ambient_color[1] = g;
    light.ambient_color[2] = b;
}

void TRenderer::SetAmbientOcclusion(bool enable, float radius_px, float strength,
                                    float bias, float max_dist_wu)
{
    light.ao_enable      = enable;
    light.ao_radius_px   = radius_px;
    light.ao_strength    = strength;
    light.ao_bias        = bias;
    light.ao_max_dist_wu = max_dist_wu;
}

void TRenderer::SetShadowWorldDir(float dx, float dy, float dz)
{
    light.shadow_world_dir[0] = dx;
    light.shadow_world_dir[1] = dy;
    light.shadow_world_dir[2] = dz;
}

void TRenderer::SetShadowVariance(float sx, float sy, float sz)
{
    light.shadow_dir[0] = sx;
    light.shadow_dir[1] = sy;
    light.shadow_dir[2] = sz;
}

void TRenderer::SetTileViewMode(int32_t mode)  { light.view_mode = mode; }
void TRenderer::SetLightingMode(int32_t mode)  { light.mode = mode; }

void TRenderer::SetSunShadow(bool enable, float step_wu, float softness_px,
                             int32_t max_steps)
{
    light.sun_shadow_enable      = enable;
    light.sun_shadow_step_wu     = step_wu;
    light.sun_shadow_softness_px = softness_px;
    light.sun_shadow_max_steps   = max_steps;
}

void TRenderer::SetSunShadowEnabled(bool enable) { light.sun_shadow_enable = enable; }
bool TRenderer::SunShadowEnabled() const         { return light.sun_shadow_enable; }

void TRenderer::SetNormalRadius(float texels)   { light.normal_radius = texels; }
void TRenderer::SetEdgeThreshold(float zraw_units) { light.edge_threshold = zraw_units; }

void TRenderer::SetNormalLightingHardness(float hardness)
{
    if (hardness < 0.0f) hardness = 0.0f;
    if (hardness > 1.0f) hardness = 1.0f;
    light.normal_lighting_hardness = hardness;
}

void TRenderer::ClearPointLights()
{
    light.plight_count = 0;
}

void TRenderer::AddPointLight(float wx, float wy, float wz, float radius_wu,
                              float r, float g, float b, float intensity)
{
    if (light.plight_count >= kMaxPointLights) return;
    const int32_t i = light.plight_count++;
    light.plight_pos[i][0] = wx;
    light.plight_pos[i][1] = wy;
    light.plight_pos[i][2] = wz;
    light.plight_pos[i][3] = radius_wu;
    light.plight_col[i][0] = r;
    light.plight_col[i][1] = g;
    light.plight_col[i][2] = b;
    light.plight_col[i][3] = intensity;
}

// *************************************************************************
// * Composite Submission                                                  *
// *************************************************************************

void TRenderer::Composite(TSurface* src)
{
    if (!src) return;
    const sg_image img = src->GetSGImage();
    if (!img.id || !composite_pip_swap.id) return;

    sg_apply_pipeline(composite_pip_swap);
    sg_bindings bind = {};
    bind.vertex_buffers[0] = composite_vbuf;
    bind.fs_images[0]      = img;
    sg_apply_bindings(&bind);

    const float uniforms[12] = {
        -1.0f, -1.0f, 2.0f, 2.0f,
         0.0f,  0.0f, 1.0f, 1.0f,
         0.0f,  0.0f, 0.0f, 0.0f,
    };
    const sg_range u_range = { uniforms, sizeof(uniforms) };
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &u_range);
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, &u_range);
    sg_draw(0, 6, 1);
}

void TRenderer::Composite(sg_image img,
                          int32_t dst_x, int32_t dst_y, int32_t dst_w, int32_t dst_h,
                          int32_t target_w, int32_t target_h)
{
    Composite(img,
              dst_x, dst_y, dst_w, dst_h,
              target_w, target_h,
              0, 0, 1, 1,
              1, 1);
}

void TRenderer::Composite(sg_image img,
                          int32_t dst_x, int32_t dst_y, int32_t dst_w, int32_t dst_h,
                          int32_t target_w, int32_t target_h,
                          int32_t src_x, int32_t src_y, int32_t src_w, int32_t src_h,
                          int32_t src_tex_w, int32_t src_tex_h,
                          bool additive_blend,
                          bool chroma_key,
                          const float* chroma_key_rgb)
{
    const sg_pipeline pip = additive_blend ? composite_pip_add_rt : composite_pip_rt;
    if (!img.id || !pip.id) return;
    if (target_w <= 0 || target_h <= 0) return;
    if (src_tex_w <= 0 || src_tex_h <= 0) return;

    sg_apply_pipeline(pip);
    sg_bindings bind = {};
    bind.vertex_buffers[0] = composite_vbuf;
    bind.fs_images[0]      = img;
    sg_apply_bindings(&bind);

    const float nx = (2.0f * dst_x / target_w) - 1.0f;
    const float nw = (2.0f * dst_w) / target_w;
    const float ny = 1.0f - (2.0f * (dst_y + dst_h) / target_h);
    const float nh = (2.0f * dst_h) / target_h;

    const float u0 = float(src_x) / float(src_tex_w);
    const float v0 = float(src_y) / float(src_tex_h);
    const float uw = float(src_w) / float(src_tex_w);
    const float vh = float(src_h) / float(src_tex_h);

    float uniforms[12] = { nx, ny, nw, nh,  u0, v0, uw, vh, 0.0f, 0.0f, 0.0f, 0.0f };
    if (chroma_key && chroma_key_rgb)
    {
        uniforms[8] = 1.0f;
        uniforms[9] = chroma_key_rgb[0];
        uniforms[10] = chroma_key_rgb[1];
        uniforms[11] = chroma_key_rgb[2];
    }
    const sg_range u_range = { uniforms, sizeof(uniforms) };
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &u_range);
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, &u_range);
    sg_draw(0, 6, 1);
}

// *************************************************************************
// * Present to Swapchain  --  Pass [7]                                    *
// *************************************************************************

bool TRenderer::PresentToSwapchain()
{
    // Caller (TDisplay::FlipPage) has already opened sokol's default pass
    // for this frame. Nothing to do if neither the tile pass nor lighting
    // pass wrote anything; let FlipPage fall back to the backbuffer.
    if (!lit_target_dirty && !color_target_dirty)
        return false;

    // Editor mode opts out: the game image is composited inside the
    // ImGui Game View panel via ImGui::Image of lit_target. Just mark
    // the targets clean so the next frame's pass actions don't fire.
    if (suppress_present) {
        color_target_dirty = false;
        lit_target_dirty   = false;
        return false;
    }

    sg_apply_pipeline(composite_pip_swap);
    sg_bindings bind = {};
    bind.vertex_buffers[0] = composite_vbuf;
    bind.fs_images[0]      = lit_target_dirty ? lit_target : color_target;
    sg_apply_bindings(&bind);

    // Sample only the centered display-sized sub-rect of the padded
    // G-buffer / lit target. See kGBufPad.
    const int32_t pad = kGBufPad;
    const float   gbw = float(width  + 2 * pad);
    const float   gbh = float(height + 2 * pad);
    const float   u0  = float(pad)   / gbw;
    const float   v0  = float(pad)   / gbh;
    const float   uw  = float(width) / gbw;
    const float   vh  = float(height) / gbh;
    const float u[12] = {
        present_ndc[0], present_ndc[1], present_ndc[2], present_ndc[3],
        u0, v0, uw, vh,
        0.0f, 0.0f, 0.0f, 0.0f,
    };
    const sg_range ur = { u, sizeof(u) };
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &ur);
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, &ur);
    sg_draw(0, 6, 1);

    color_target_dirty = false;
    lit_target_dirty   = false;
    return true;
}

void TRenderer::SetPresentNDCRect(float x, float y, float w, float h)
{
    present_ndc[0] = x;
    present_ndc[1] = y;
    present_ndc[2] = w;
    present_ndc[3] = h;
}
