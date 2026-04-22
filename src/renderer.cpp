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
    gbuf_desc.depth_stencil_attachment.image = depth_target;
    default_pass = sg_make_pass(&gbuf_desc);

    // Pass [3] -- deferred lighting.
    sg_pass_desc lit_desc = {};
    lit_desc.color_attachments[0].image = lit_target;
    lit_pass = sg_make_pass(&lit_desc);

    // Pass [2] -- screen-space AO.
    sg_pass_desc ao_desc = {};
    ao_desc.color_attachments[0].image = ao_target;
    ao_pass = sg_make_pass(&ao_desc);

    // Reserved for a depth-only prepass in Phase 3.
    depth_pass = {};

    InitCompositePipeline();
    InitTilePipeline();
    InitAOPipeline();
    InitLightPipeline();

    return true;
}

void TRenderer::Shutdown()
{
    ShutdownLightPipeline();
    ShutdownAOPipeline();
    ShutdownTilePipeline();
    ShutdownCompositePipeline();

    if (default_pass.id) { sg_destroy_pass(default_pass); default_pass = {}; }
    if (lit_pass.id)     { sg_destroy_pass(lit_pass);     lit_pass     = {}; }
    if (ao_pass.id)      { sg_destroy_pass(ao_pass);      ao_pass      = {}; }

    if (color_target.id)   { sg_destroy_image(color_target);   color_target   = {}; }
    if (depth_target.id)   { sg_destroy_image(depth_target);   depth_target   = {}; }
    if (normal_target.id)  { sg_destroy_image(normal_target);  normal_target  = {}; }
    if (scene_z_target.id) { sg_destroy_image(scene_z_target); scene_z_target = {}; }
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
    sh.vs.uniform_blocks[0].size = sizeof(float) * 8;
    sh.vs.uniform_blocks[0].uniforms[0].name = "rect";
    sh.vs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
    sh.vs.uniform_blocks[0].uniforms[1].name = "uv_rect";
    sh.vs.uniform_blocks[0].uniforms[1].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.source                 = kCompositeFs;
    sh.fs.entry                  = kShaderFsEntry;
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

    // Variant 2: drawing into the swapchain (sokol default pass).
    pip.colors[0].pixel_format = _SG_PIXELFORMAT_DEFAULT;
    pip.depth.pixel_format     = _SG_PIXELFORMAT_DEFAULT;
    pip.label = "renderer.composite.pipeline.swap";
    composite_pip_swap = sg_make_pipeline(&pip);
}

void TRenderer::ShutdownCompositePipeline()
{
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

    // 5 vec4s: rect, zparams, tile_root, tile_sprite, filter.
    constexpr int32_t kUB_vec4s = 5;
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
    // MRT: albedo (RGBA8) + world normal (RGBA16F) + scene_z (R32F).
    pip.color_count            = 3;
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
    pip.depth.pixel_format     = SG_PIXELFORMAT_DEPTH;
    pip.depth.compare          = SG_COMPAREFUNC_LESS_EQUAL;
    pip.depth.write_enabled    = true;
    pip.label = "renderer.tile.pipeline";
    tile_pipeline = sg_make_pipeline(&pip);
}

void TRenderer::ShutdownTilePipeline()
{
    if (tile_pipeline.id) { sg_destroy_pipeline(tile_pipeline); tile_pipeline = {}; }
    if (tile_shader.id)   { sg_destroy_shader(tile_shader);     tile_shader   = {}; }
    if (tile_vbuf.id)     { sg_destroy_buffer(tile_vbuf);       tile_vbuf     = {}; }
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
    sh.fs.uniform_blocks[0].size = sizeof(float) * 12;
    sh.fs.uniform_blocks[0].uniforms[0].name = "vp";
    sh.fs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[1].name = "recon";
    sh.fs.uniform_blocks[0].uniforms[1].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[2].name = "ao_settings";
    sh.fs.uniform_blocks[0].uniforms[2].type = SG_UNIFORMTYPE_FLOAT4;
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

    float u[12] = {};
    int32_t o = 0;
    u[o++] = recon.ox; u[o++] = recon.oy; u[o++] = recon.z_near; u[o++] = recon.zspan;
    u[o++] = recon.center_wx; u[o++] = recon.center_wy; u[o++] = recon.kcam_forward; u[o++] = recon.reserved;
    u[o++] = light.ao_radius_px;
    u[o++] = light.ao_enable ? light.ao_strength : 0.0f;
    u[o++] = light.ao_bias;
    u[o++] = light.ao_max_dist_wu;
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
    constexpr int32_t kUB_vec4s = 6 + kPL + kPL + 4;
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
                                        float kcam_forward, float reserved)
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
    recon.reserved     = reserved;
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
    sg_apply_bindings(&bind);

    constexpr int32_t kPL = TRenderer::kMaxPointLights;
    constexpr int32_t kUB_vec4s = 6 + kPL + kPL + 4;
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
    u[o++] = 0.0f;
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

    const sg_range r = { u, sizeof(u) };
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, &r);
    sg_draw(0, 6, 1);
    sg_end_pass();

    lit_target_dirty = true;
}

// *************************************************************************
// * Tile Pass Submission  (BeginTilePass / DrawTile / EndTilePass)        *
// *************************************************************************

void TRenderer::BeginTilePass(float r, float g, float b, float a)
{
    if (!default_pass.id) return;
    tile_clear_rgba[0] = r; tile_clear_rgba[1] = g;
    tile_clear_rgba[2] = b; tile_clear_rgba[3] = a;
    sg_pass_action pa = {};
    pa.colors[0].action = SG_ACTION_CLEAR;
    pa.colors[0].value  = { 0.0f, 0.0f, 0.0f, 0.0f };
    pa.colors[1].action = SG_ACTION_CLEAR;
    pa.colors[1].value  = { 0.5f, 0.5f, 0.5f, 1.0f };
    pa.colors[2].action = SG_ACTION_CLEAR;
    pa.colors[2].value  = { 1.0f, 0.0f, 0.0f, 1.0f };
    pa.depth.action     = SG_ACTION_CLEAR;
    pa.depth.value      = 1.0f;
    pa.stencil.action   = SG_ACTION_DONTCARE;
    sg_begin_pass(default_pass, &pa);
    color_target_dirty = true;
}

void TRenderer::DrawTile(sg_image color_img, sg_image depth_img,
                         int32_t dst_x, int32_t dst_y, int32_t dst_w, int32_t dst_h,
                         float anchor_z, float depth_mul, float normal_mul,
                         float root_wx, float root_wy, float root_wz,
                         float anchor_px_x, float anchor_px_y,
                         float zraw_to_wu)
{
    if (!tile_pipeline.id || !color_img.id || !depth_img.id) return;
    if (width <= 0 || height <= 0) return;

    sg_apply_pipeline(tile_pipeline);
    sg_bindings bind = {};
    bind.vertex_buffers[0] = tile_vbuf;
    bind.fs_images[0]      = color_img;
    bind.fs_images[1]      = depth_img;
    sg_apply_bindings(&bind);

    const int32_t pad = kGBufPad;
    const int32_t gbw = width  + 2 * pad;
    const int32_t gbh = height + 2 * pad;
    const int32_t px  = dst_x + pad;
    const int32_t py  = dst_y + pad;
    const float nx = (2.0f * px / gbw)  - 1.0f;
    const float nw = (2.0f * dst_w) / gbw;
    const float ny = 1.0f - (2.0f * (py + dst_h) / gbh);
    const float nh = (2.0f * dst_h) / gbh;

    float uniforms[5 * 4] = {};
    int32_t off = 0;
    uniforms[off++] = nx; uniforms[off++] = ny; uniforms[off++] = nw; uniforms[off++] = nh;
    uniforms[off++] = anchor_z;
    uniforms[off++] = depth_mul;
    uniforms[off++] = normal_mul;
    uniforms[off++] = 0.0f;
    uniforms[off++] = root_wx; uniforms[off++] = root_wy;
    uniforms[off++] = root_wz; uniforms[off++] = zraw_to_wu;
    uniforms[off++] = anchor_px_x; uniforms[off++] = anchor_px_y;
    uniforms[off++] = float(dst_w); uniforms[off++] = float(dst_h);
    uniforms[off++] = light.normal_radius;
    uniforms[off++] = light.edge_threshold;
    uniforms[off++] = 0.0f;
    uniforms[off++] = 0.0f;

    const sg_range u_range = { uniforms, sizeof(uniforms) };
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &u_range);
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, &u_range);
    sg_draw(0, 6, 1);
}

void TRenderer::EndTilePass()
{
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

    const float uniforms[8] = {
        -1.0f, -1.0f, 2.0f, 2.0f,
         0.0f,  0.0f, 1.0f, 1.0f,
    };
    const sg_range u_range = { uniforms, sizeof(uniforms) };
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &u_range);
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
                          int32_t src_tex_w, int32_t src_tex_h)
{
    if (!img.id || !composite_pip_rt.id) return;
    if (target_w <= 0 || target_h <= 0) return;
    if (src_tex_w <= 0 || src_tex_h <= 0) return;

    sg_apply_pipeline(composite_pip_rt);
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

    const float uniforms[8] = { nx, ny, nw, nh,  u0, v0, uw, vh };
    const sg_range u_range = { uniforms, sizeof(uniforms) };
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &u_range);
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
    const float u[8] = { -1.0f, -1.0f, 2.0f, 2.0f,  u0, v0, uw, vh };
    const sg_range ur = { u, sizeof(u) };
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &ur);
    sg_draw(0, 6, 1);

    color_target_dirty = false;
    lit_target_dirty   = false;
    return true;
}
