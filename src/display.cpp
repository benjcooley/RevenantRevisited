// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 display.cpp  - Display Object File                    *
// *************************************************************************

#include "display.h"

#include <sokol_app.h>
#include <sokol_gfx.h>
#include <sokol_glue.h>

#include "imgui.h"
#include "sokol_imgui.h"

#include "multisurface.h"
#include "revenant.h"

// Initializes Color Tables
extern bool  Hardware3D;
extern bool  UsingHardware;
extern void  MakeColorTables();

// These functions are used to record a list of update ares on the screen.
// The low level graphics functions all are passed the address of a function called
// 'callback' in their drawparam structures.  For the display, this funciton is set
// to AddUpdateCallback(), which logs the rectangle drawn in a list.  When all the
// drawing is complete, the low level draw functions return, and the list is added
// to the display dirty rectangle update list with a call to ProcessUpdateCallbacks().

void ResetUpdateCallbacks();
void ProcessUpdateCallbacks();
void AddUpdateCallback(SDrawBlock* db, SDrawParam* dp);

TDisplay::TDisplay()
{}

bool TDisplay::Initialize(int32_t dwidth, int32_t dheight, int32_t /*dbitsperpixel*/)
{
    if (frontbuffer)
        return true;

    width = dwidth;
    height = dheight;
    format = SG_PIXELFORMAT_RGBA8;

    // Initialize Sokol graphics context
    sg_desc desc = {};
    desc.context = sapp_sgcontext();
    desc.buffer_pool_size = 256;
    // One tile bitmap needs 2 images (color + depth); a single Misthaven
    // sector already has ~200 unique bitmaps, and the test harness loads a
    // 3x3 neighborhood. Headroom for UI atlases, render targets, and ImGui.
    desc.image_pool_size = 4096;
    desc.shader_pool_size = 64;
    desc.pipeline_pool_size = 64;
    desc.pass_pool_size = 32;
    // Large full-level sector tests can issue tens of thousands of draw calls,
    // each with small uniform uploads. Metal stages these through sokol's
    // shared per-frame uniform buffer; the default size overflows on
    // `--test=sector --level=0`.
    desc.uniform_buffer_size = 16 * 1024 * 1024;
    sg_setup(&desc);

    // Create render target images. The G-buffer is allocated larger than
    // the display so off-screen geometry can still contribute to effects
    // that sample neighbors (sun-shadow ray march in particular). FlipPage
    // composites only the centered (width × height) sub-rect to the swap-
    // chain. See TDisplay::kGBufPad.
    const int32_t pad = kGBufPad;
    const int32_t gbw = width  + 2 * pad;
    const int32_t gbh = height + 2 * pad;
    sg_image_desc rt_desc = {};
    rt_desc.width = gbw;
    rt_desc.height = gbh;
    rt_desc.sample_count = 1;
    rt_desc.render_target = true;
    rt_desc.min_filter = SG_FILTER_LINEAR;
    rt_desc.mag_filter = SG_FILTER_LINEAR;

    // Color target (RGBA8)
    rt_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    color_target = sg_make_image(&rt_desc);

    // Depth target (Depth32F)
    rt_desc.pixel_format = SG_PIXELFORMAT_DEPTH;
    depth_target = sg_make_image(&rt_desc);

    // Normal target (RGBA16F for normals/material data)
    rt_desc.pixel_format = SG_PIXELFORMAT_RGBA16F;
    normal_target = sg_make_image(&rt_desc);

    // Scene-z color target. Tile fragment shader writes the same normalized
    // depth here as to the depth attachment; the light pass samples this
    // (depth attachments aren't cleanly sampleable on sokol's Metal path).
    rt_desc.pixel_format = SG_PIXELFORMAT_R32F;
    scene_z_target = sg_make_image(&rt_desc);

    // Lit target — final shaded RGBA8 that FlipPage composites to the
    // swapchain. Separate from color_target because color_target is now
    // the G-buffer albedo channel and must survive across the lighting
    // pass that reads it.
    rt_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    rt_desc.min_filter   = SG_FILTER_NEAREST;
    rt_desc.mag_filter   = SG_FILTER_NEAREST;
    lit_target = sg_make_image(&rt_desc);

    // AO target — single-channel screen-space occlusion mask. The AO pass
    // writes 1 = unoccluded, 0 = fully occluded. The light pass samples it
    // and modulates only the ambient term.
    rt_desc.pixel_format = SG_PIXELFORMAT_R32F;
    rt_desc.min_filter   = SG_FILTER_LINEAR;
    rt_desc.mag_filter   = SG_FILTER_LINEAR;
    ao_target = sg_make_image(&rt_desc);

    // G-buffer fill pass — MRT: albedo + world normal + scene_z color.
    // Depth attachment is for HW Z-testing only; the light pass samples
    // scene_z_target instead (sokol/Metal depth-attachment sampling quirks).
    sg_pass_desc gbuf_desc = {};
    gbuf_desc.color_attachments[0].image     = color_target;
    gbuf_desc.color_attachments[1].image     = normal_target;
    gbuf_desc.color_attachments[2].image     = scene_z_target;
    gbuf_desc.depth_stencil_attachment.image = depth_target;
    default_pass = sg_make_pass(&gbuf_desc);

    // Light accumulation pass — fullscreen quad reading the G-buffer,
    // outputting lit RGBA8 into lit_target. No depth attachment.
    sg_pass_desc lit_desc = {};
    lit_desc.color_attachments[0].image = lit_target;
    lit_pass = sg_make_pass(&lit_desc);

    sg_pass_desc ao_desc = {};
    ao_desc.color_attachments[0].image = ao_target;
    ao_pass = sg_make_pass(&ao_desc);

    // Depth pre-pass — wired up with the 3D renderer in Phase 3. sokol_gfx
    // does not permit a pass with no color attachment, so we skip creation
    // until the depth-only prepass actually has a consumer.
    depth_pass = {};

#if 0 // TODO(port): real sokol_gfx path — Phase 3
    // sokol_gfx has no SG_BUFFERTYPE_UNIFORM (no UBOs); uniforms are pushed
    // via sg_apply_uniforms each draw call. These sg_buffer slots go away
    // when the tile pipeline gets wired up in Phase 3.
    sg_buffer_desc buf_desc = {};
    buf_desc.type = SG_BUFFERTYPE_UNIFORM;
    buf_desc.usage = SG_USAGE_STREAM;

    buf_desc.size = sizeof(float) * 16; // 4x4 matrix
    uniforms.view_proj = sg_make_buffer(&buf_desc);
    uniforms.model = sg_make_buffer(&buf_desc);

    buf_desc.size = sizeof(float) * 8; // light pos (vec3) + padding + color (vec3) + intensity
    uniforms.light_params = sg_make_buffer(&buf_desc);
#endif

#if 0 // TODO(port): real sokol_gfx path — Phase 3
    // Tile rendering pipeline needs a shader module to be wired up; deferred
    // along with the rest of the tile-draw rewrite.
    tile_pip_desc = {};
    tile_pip_desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;  // position
    tile_pip_desc.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT2;  // texcoord
    tile_pip_desc.layout.attrs[2].format = SG_VERTEXFORMAT_FLOAT;   // depth
    tile_pip_desc.layout.attrs[3].format = SG_VERTEXFORMAT_FLOAT3;  // normal
    tile_pip_desc.shader = sg_make_shader(tile_shader_desc());
    tile_pip_desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
    tile_pip_desc.depth.write_enabled = true;
    tile_pip_desc.colors[0].blend.enabled = true;
    tile_pip_desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
    tile_pip_desc.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    tile_pipeline = sg_make_pipeline(&tile_pip_desc);
#endif

    // Frontbuffer wraps the main render-target image. Unused for now; kept
    // alive so Close/Restore don't need to branch. The 3D scene will render
    // into it when Phase 3 comes online.
    frontbuffer = new TSurface(color_target, width, height, SG_PIXELFORMAT_RGBA8);

    // Backbuffer is the display's final composite target. Widget surfaces
    // (render textures) get drawn onto it via TDisplay::Composite() inside a
    // pass opened by backbuffer->StartPass(). FlipPage then composites the
    // backbuffer onto the swapchain.
    // NOTE: explicit desc — rt_desc's pixel_format was last set to RGBA16F
    // for normal_target, so reusing it here would create an RGBA16F image.
    sg_image_desc bb_desc = {};
    bb_desc.width        = width;
    bb_desc.height       = height;
    bb_desc.sample_count = 1;
    bb_desc.render_target = true;
    bb_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    bb_desc.min_filter   = SG_FILTER_NEAREST;
    bb_desc.mag_filter   = SG_FILTER_NEAREST;
    bb_desc.wrap_u       = SG_WRAP_CLAMP_TO_EDGE;
    bb_desc.wrap_v       = SG_WRAP_CLAMP_TO_EDGE;
    bb_desc.label        = "display.backbuffer";
    backbuffer = new TSurface(sg_make_image(&bb_desc), width, height, SG_PIXELFORMAT_RGBA8);

    zbuffer = new TSurface(depth_target, width, height, SG_PIXELFORMAT_DEPTH);

    InitCompositePipeline();
    InitTilePipeline();
    InitAOPipeline();
    InitLightPipeline();

    // Dear ImGui overlay — used for in-game debug/tuning panels (lighting,
    // depth scale, camera). Draws into the swapchain pass in FlipPage.
    //
    // max_vertices default is 65,536 which silently truncates the foreground
    // drawlist when a debug overlay pushes a lot of geometry (e.g. the sector
    // test's per-tile bbox wireframes — ~12 AA lines × thousands of tiles).
    // sokol_imgui.h bails with `break` once a drawlist would overflow the
    // shared vtx/idx buffer, dropping whatever hadn't been copied yet —
    // including the foreground drawlist since it's appended last. Bump to
    // 1M vertices (~20 MB) so large debug overlays render intact.
    {
        simgui_desc_t d = {};
        d.ini_filename = "imgui.ini";
        d.max_vertices = 1 << 20; // 1,048,576
        simgui_setup(&d);
        // Keep the debug overlay compact enough that the sector tuning panel
        // doesn't dominate the viewport.
        constexpr float kDebugUiScale = 0.85f;
        ImGui::GetStyle().ScaleAllSizes(kDebugUiScale);
        ImGui::GetIO().FontGlobalScale = kDebugUiScale;
    }

    return true;
}

namespace {

struct CompositeVertex { float x, y, u, v; };

// Unit quad in [0,1]² with UVs. The vertex shader transforms (x,y) into NDC
// using the per-draw `rect` uniform (rect.xy = NDC origin, rect.zw = NDC size).
// UV.y is flipped so row 0 of the source image lands at the top of the quad.
const CompositeVertex kCompositeQuad[6] = {
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 1.0f, 0.0f, 1.0f, 1.0f },
    { 0.0f, 1.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f, 0.0f },
    { 1.0f, 0.0f, 1.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f, 0.0f },
};

const char* kCompositeVsMetal =
    "#include <metal_stdlib>\n"
    "using namespace metal;\n"
    "struct params { float4 rect; float4 uv_rect; };\n"
    "struct vs_in  { float2 pos [[attribute(0)]]; float2 uv [[attribute(1)]]; };\n"
    "struct vs_out { float4 pos [[position]];      float2 uv; };\n"
    "vertex vs_out _main(vs_in in [[stage_in]], constant params& p [[buffer(0)]]) {\n"
    "    vs_out o;\n"
    "    o.pos = float4(p.rect.xy + in.pos * p.rect.zw, 0.0, 1.0);\n"
    "    o.uv  = p.uv_rect.xy + in.uv * p.uv_rect.zw;\n"
    "    return o;\n"
    "}\n";

const char* kCompositeFsMetal =
    "#include <metal_stdlib>\n"
    "using namespace metal;\n"
    "struct vs_out { float4 pos [[position]]; float2 uv; };\n"
    "fragment float4 _main(vs_out in [[stage_in]],\n"
    "                      texture2d<float> tex [[texture(0)]],\n"
    "                      sampler smp [[sampler(0)]]) {\n"
    "    return tex.sample(smp, in.uv);\n"
    "}\n";

}  // namespace

void TDisplay::InitCompositePipeline()
{
    sg_buffer_desc vb = {};
    vb.size  = sizeof(kCompositeQuad);
    vb.data  = SG_RANGE(kCompositeQuad);
    vb.label = "display.composite.vbuf";
    composite_vbuf = sg_make_buffer(&vb);

    sg_shader_desc sh = {};
    sh.attrs[0].name             = "pos";
    sh.attrs[1].name             = "uv";
    sh.vs.source                 = kCompositeVsMetal;
    sh.vs.entry                  = "_main";
    sh.vs.uniform_blocks[0].size = sizeof(float) * 8;
    sh.vs.uniform_blocks[0].uniforms[0].name = "rect";
    sh.vs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
    sh.vs.uniform_blocks[0].uniforms[1].name = "uv_rect";
    sh.vs.uniform_blocks[0].uniforms[1].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.source                 = kCompositeFsMetal;
    sh.fs.entry                  = "_main";
    sh.fs.images[0].name         = "tex";
    sh.fs.images[0].image_type   = SG_IMAGETYPE_2D;
    sh.fs.images[0].sampler_type = SG_SAMPLERTYPE_FLOAT;
    sh.label = "display.composite.shader";
    composite_shader = sg_make_shader(&sh);

    sg_pipeline_desc pip = {};
    pip.shader = composite_shader;
    pip.layout.attrs[0].format  = SG_VERTEXFORMAT_FLOAT2;
    pip.layout.attrs[1].format  = SG_VERTEXFORMAT_FLOAT2;
    pip.primitive_type          = SG_PRIMITIVETYPE_TRIANGLES;
    // Straight alpha blend so icons with keycolor-derived alpha=0 show through.
    pip.colors[0].blend.enabled = true;
    pip.colors[0].blend.src_factor_rgb   = SG_BLENDFACTOR_SRC_ALPHA;
    pip.colors[0].blend.dst_factor_rgb   = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    pip.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
    pip.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;

    // Variant 1: drawing into our RGBA8 backbuffer (no depth attachment).
    pip.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
    pip.depth.pixel_format     = SG_PIXELFORMAT_NONE;
    pip.label = "display.composite.pipeline.rt";
    composite_pip_rt = sg_make_pipeline(&pip);

    // Variant 2: drawing into the swapchain (sokol's default pass). Zero
    // means "use the default pass format" — sokol resolves it against the
    // context description at pipeline creation time.
    pip.colors[0].pixel_format = _SG_PIXELFORMAT_DEFAULT;
    pip.depth.pixel_format     = _SG_PIXELFORMAT_DEFAULT;
    pip.label = "display.composite.pipeline.swap";
    composite_pip_swap = sg_make_pipeline(&pip);
}

void TDisplay::ShutdownCompositePipeline()
{
    if (composite_pip_rt.id)   { sg_destroy_pipeline(composite_pip_rt);   composite_pip_rt   = {}; }
    if (composite_pip_swap.id) { sg_destroy_pipeline(composite_pip_swap); composite_pip_swap = {}; }
    if (composite_shader.id)   { sg_destroy_shader(composite_shader);     composite_shader   = {}; }
    if (composite_vbuf.id)     { sg_destroy_buffer(composite_vbuf);       composite_vbuf     = {}; }
}

// ---- Tile G-buffer fill pipeline -----------------------------------------
// Deferred shading. The tile shader fills a G-buffer (albedo + world normal +
// depth). A second fullscreen pass (InitLightPipeline) reads the G-buffer and
// runs the actual lighting math. Lifting lights out of the per-tile uniform
// block means no per-draw cap — future work will loop the light pass in
// batches of 16 for unlimited lights.

namespace {

const char* kTileVsMetal =
    "#include <metal_stdlib>\n"
    "using namespace metal;\n"
    "struct params { float4 rect; float4 zparams; float4 tile_root; float4 tile_sprite;\n"
    "                float4 filter; };\n"
    "struct vs_in  { float2 pos [[attribute(0)]]; float2 uv [[attribute(1)]]; };\n"
    "struct vs_out { float4 pos [[position]]; float2 uv; };\n"
    "vertex vs_out _main(vs_in in [[stage_in]], constant params& p [[buffer(0)]]) {\n"
    "    vs_out o;\n"
    "    o.pos = float4(p.rect.xy + in.pos * p.rect.zw, 0.0, 1.0);\n"
    "    o.uv  = in.uv;\n"
    "    return o;\n"
    "}\n";

// G-buffer fill. zparams.x = anchor_z, zparams.y = depth_mul,
// zparams.z unused, zparams.w unused. tile_root.xyz = world xyz of
// the anchor pixel, tile_root.w = scale from uploaded bitmap-z samples to
// local scene-depth delta (wu). For the sector test this should be 1.0 once
// the source data is interpreted correctly.
// tile_sprite.xy = anchor pixel (regx, regy), zw = sprite size (dst_w, dst_h).
// filter.x = normal_radius (texels), filter.y = bilateral edge threshold in
// uploaded depth-texture units. No light-related uniforms anymore — lighting runs as a
// fullscreen pass over the G-buffer.
const char* kTileFsMetal =
    "#include <metal_stdlib>\n"
    "using namespace metal;\n"
    "#define ISO_COS30 0.867\n"
    "#define ISO_WZ_DENOM 2.003378\n"
    "#define EDGE_EPS 1e-6\n"
    "struct params { float4 rect; float4 zparams; float4 tile_root; float4 tile_sprite;\n"
    "                float4 filter; };\n"
    "struct vs_out { float4 pos [[position]]; float2 uv; };\n"
    "struct fs_out { float4 albedo   [[color(0)]];\n"
    "                float4 normal   [[color(1)]];\n"
    "                float4 scene_z  [[color(2)]];\n"
    "                float  depth    [[depth(any)]]; };\n"
    "fragment fs_out _main(vs_out in [[stage_in]],\n"
    "                      texture2d<float> color_tex [[texture(0)]],\n"
    "                      texture2d<float> depth_tex [[texture(1)]],\n"
    "                      sampler smp              [[sampler(0)]],\n"
    "                      constant params& p       [[buffer(0)]]) {\n"
    "    fs_out o;\n"
    "    float4 c = color_tex.sample(smp, in.uv);\n"
    "    if (c.a < 0.01) discard_fragment();\n"
    "    float zraw = depth_tex.sample(smp, in.uv).r;\n"
    "    float d    = p.zparams.x + zraw * p.zparams.y;\n"
    "    // Fragments outside the active depth window should not collapse to\n"
    "    // 0/1 and pin the z-buffer there; reject them so they don't occlude\n"
    "    // in-range tiles incorrectly.\n"
    "    if (d < 0.0 || d > 1.0) discard_fragment();\n"
    "    // Build the normal from neighboring local world positions.\n"
    "    // BM_ZBUFFER encodes local scene-depth/native-z delta, not literal\n"
    "    // world-Z height, so reconstruct each sample by inverting the iso\n"
    "    // map from (local screen offset S/T, local scene-depth delta).\n"
    "    float  nr  = max(p.filter.x, 0.5);\n"
    "    float  thr = max(p.filter.y, 0.0);\n"
    "    float  tw  = float(depth_tex.get_width());\n"
    "    float  th  = float(depth_tex.get_height());\n"
    "    float2 tex = float2(nr / max(tw, 1.0), nr / max(th, 1.0));\n"
    "    float  sx  = in.uv.x * p.tile_sprite.z - p.tile_sprite.x;\n"
    "    float  sy  = in.uv.y * p.tile_sprite.w - p.tile_sprite.y;\n"
    "    float  zl  = depth_tex.sample(smp, in.uv - float2(tex.x, 0.0)).r;\n"
    "    float  zrs = depth_tex.sample(smp, in.uv + float2(tex.x, 0.0)).r;\n"
    "    float  zt  = depth_tex.sample(smp, in.uv - float2(0.0, tex.y)).r;\n"
    "    float  zb  = depth_tex.sample(smp, in.uv + float2(0.0, tex.y)).r;\n"
    "    if (fabs(zl  - zraw) > thr + EDGE_EPS) zl  = zraw;\n"
    "    if (fabs(zrs - zraw) > thr + EDGE_EPS) zrs = zraw;\n"
    "    if (fabs(zt  - zraw) > thr + EDGE_EPS) zt  = zraw;\n"
    "    if (fabs(zb  - zraw) > thr + EDGE_EPS) zb  = zraw;\n"
    "    float  dzl = zl  * p.tile_root.w;\n"
    "    float  dzr = zrs * p.tile_root.w;\n"
    "    float  dzt = zt  * p.tile_root.w;\n"
    "    float  dzb = zb  * p.tile_root.w;\n"
    "    float  nrx = nr;\n"
    "    float  nry = nr;\n"
    "    float  lz_l = (-dzl - 2.0 * sy * ISO_COS30) / ISO_WZ_DENOM;\n"
    "    float  su_l = 2.0 * (sy + lz_l * ISO_COS30);\n"
    "    float3 Wl = float3(p.tile_root.x + 0.5 * (su_l + (sx - nrx)),\n"
    "                      p.tile_root.y + 0.5 * (su_l - (sx - nrx)),\n"
    "                      p.tile_root.z + lz_l);\n"
    "    float  lz_r = (-dzr - 2.0 * sy * ISO_COS30) / ISO_WZ_DENOM;\n"
    "    float  su_r = 2.0 * (sy + lz_r * ISO_COS30);\n"
    "    float3 Wr = float3(p.tile_root.x + 0.5 * (su_r + (sx + nrx)),\n"
    "                      p.tile_root.y + 0.5 * (su_r - (sx + nrx)),\n"
    "                      p.tile_root.z + lz_r);\n"
    "    float  lz_t = (-dzt - 2.0 * (sy - nry) * ISO_COS30) / ISO_WZ_DENOM;\n"
    "    float  su_t = 2.0 * ((sy - nry) + lz_t * ISO_COS30);\n"
    "    float3 Wt = float3(p.tile_root.x + 0.5 * (su_t + sx),\n"
    "                      p.tile_root.y + 0.5 * (su_t - sx),\n"
    "                      p.tile_root.z + lz_t);\n"
    "    float  lz_b = (-dzb - 2.0 * (sy + nry) * ISO_COS30) / ISO_WZ_DENOM;\n"
    "    float  su_b = 2.0 * ((sy + nry) + lz_b * ISO_COS30);\n"
    "    float3 Wb = float3(p.tile_root.x + 0.5 * (su_b + sx),\n"
    "                      p.tile_root.y + 0.5 * (su_b - sx),\n"
    "                      p.tile_root.z + lz_b);\n"
    "    float3 t_u = Wr - Wl;\n"
    "    float3 t_v = Wb - Wt;\n"
    "    float3 N   = normalize(cross(t_v, t_u));\n"
    "    float3 Vc  = normalize(float3(ISO_COS30, ISO_COS30, 1.0));\n"
    "    // The depth texture describes the camera-facing side of a billboarded\n"
    "    // tile, so keep the reconstructed normal in the camera-facing\n"
    "    // hemisphere rather than blindly forcing +Z.\n"
    "    if (dot(N, Vc) < 0.0) N = -N;\n"
    "    o.albedo  = c;\n"
    "    o.normal  = float4(N * 0.5 + 0.5, 1.0);\n"
    "    o.scene_z = float4(d, 0.0, 0.0, 1.0);\n"
    "    o.depth   = d;\n"
    "    return o;\n"
    "}\n";

}  // namespace

void TDisplay::InitTilePipeline()
{
    // Reuse the composite unit-quad layout (pos+uv, 6 verts).
    sg_buffer_desc vb = {};
    vb.size  = sizeof(kCompositeQuad);
    vb.data  = SG_RANGE(kCompositeQuad);
    vb.label = "display.tile.vbuf";
    tile_vbuf = sg_make_buffer(&vb);

    // Uniform block for the G-buffer fill: rect, zparams, tile_root,
    // tile_sprite, filter (5 vec4s). Lights no longer live here — the
    // deferred light pass reads the G-buffer and runs all lighting math.
    constexpr int32_t kUB_vec4s = 5;
    constexpr int32_t kUB_bytes = kUB_vec4s * 16;

    sg_shader_desc sh = {};
    sh.attrs[0].name = "pos";
    sh.attrs[1].name = "uv";
    sh.vs.source = kTileVsMetal;
    sh.vs.entry  = "_main";
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
    sh.fs.source = kTileFsMetal;
    sh.fs.entry  = "_main";
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
    sh.label = "display.tile.shader";
    tile_shader = sg_make_shader(&sh);

    sg_pipeline_desc pip = {};
    pip.shader = tile_shader;
    pip.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT2;
    pip.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT2;
    pip.primitive_type         = SG_PRIMITIVETYPE_TRIANGLES;
    // MRT: albedo (RGBA8) + world normal (RGBA16F) + scene_z (R32F). Formats
    // must match default_pass attachments exactly or pipeline creation fails.
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
    pip.label = "display.tile.pipeline";
    tile_pipeline = sg_make_pipeline(&pip);
}

void TDisplay::ShutdownTilePipeline()
{
    if (tile_pipeline.id) { sg_destroy_pipeline(tile_pipeline); tile_pipeline = {}; }
    if (tile_shader.id)   { sg_destroy_shader(tile_shader);     tile_shader   = {}; }
    if (tile_vbuf.id)     { sg_destroy_buffer(tile_vbuf);       tile_vbuf     = {}; }
}

// ---- Screen-space ambient occlusion pass ---------------------------------
// Reads the G-buffer (albedo + world normal + scene_z), reconstructs local
// world positions in screen space, and outputs a single-channel AO term where
// 1 = unoccluded and 0 = fully occluded. The light pass applies it only to the
// ambient floor.

namespace {

const char* kAOVsMetal =
    "#include <metal_stdlib>\n"
    "using namespace metal;\n"
    "struct vs_in  { float2 pos [[attribute(0)]]; float2 uv [[attribute(1)]]; };\n"
    "struct vs_out { float4 pos [[position]];     float2 uv; };\n"
    "vertex vs_out _main(vs_in in [[stage_in]]) {\n"
    "    vs_out o;\n"
    "    o.pos = float4(in.pos * 2.0 - 1.0, 0.0, 1.0);\n"
    "    o.uv  = in.uv;\n"
    "    return o;\n"
    "}\n";

const char* kAOFsMetal =
    "#include <metal_stdlib>\n"
    "using namespace metal;\n"
    "#define ISO_COS30 0.867\n"
    "#define ISO_WZ_DENOM 2.003378\n"
    "struct params { float4 vp; float4 recon; float4 ao_settings; };\n"
    "struct vs_out { float4 pos [[position]]; float2 uv; };\n"
    "float3 reconstruct_world(float2 uv, float d, constant params& p, float fbw, float fbh) {\n"
    "    float S = uv.x * fbw - p.vp.x;\n"
    "    float T = uv.y * fbh - p.vp.y;\n"
    "    float scene_z = d * p.vp.w + p.vp.z;\n"
    "    float K = p.recon.z - scene_z;\n"
    "    float wz = (K - 2.0 * T * ISO_COS30) / ISO_WZ_DENOM;\n"
    "    float sum_r = 2.0 * (T + wz * ISO_COS30);\n"
    "    return float3(p.recon.x + (sum_r + S) * 0.5,\n"
    "                  p.recon.y + (sum_r - S) * 0.5,\n"
    "                  wz);\n"
    "}\n"
    "fragment float _main(vs_out in [[stage_in]],\n"
    "                     texture2d<float> albedo_tex [[texture(0)]],\n"
    "                     texture2d<float> normal_tex [[texture(1)]],\n"
    "                     texture2d<float> depth_tex  [[texture(2)]],\n"
    "                     sampler smp                [[sampler(0)]],\n"
    "                     constant params& p         [[buffer(0)]]) {\n"
    "    float4 alb = albedo_tex.sample(smp, in.uv);\n"
    "    if (alb.a < 0.01) return 1.0;\n"
    "    float strength = max(p.ao_settings.y, 0.0);\n"
    "    if (strength <= 0.0) return 1.0;\n"
    "    float radius_px = max(p.ao_settings.x, 1.0);\n"
    "    float bias = p.ao_settings.z;\n"
    "    float max_dist = max(p.ao_settings.w, 1.0);\n"
    "    float d = depth_tex.sample(smp, in.uv).r;\n"
    "    float3 N = normalize(normal_tex.sample(smp, in.uv).xyz * 2.0 - 1.0);\n"
    "    float fbw = float(albedo_tex.get_width());\n"
    "    float fbh = float(albedo_tex.get_height());\n"
    "    float3 W = reconstruct_world(in.uv, d, p, fbw, fbh);\n"
    "    float h = fract(sin(dot(in.uv, float2(12.9898, 78.233))) * 43758.5453);\n"
    "    const int kSamples = 8;\n"
    "    float occ = 0.0;\n"
    "    float wsum = 0.0;\n"
    "    for (int i = 0; i < kSamples; ++i) {\n"
    "        float a = h * 6.2831853 + float(i) * (6.2831853 / float(kSamples));\n"
    "        float ring = radius_px * (0.35 + 0.65 * fract(h + float(i) * 0.6180339));\n"
    "        float2 suv = in.uv + float2(cos(a), sin(a)) * ring / float2(fbw, fbh);\n"
    "        if (suv.x < 0.0 || suv.x > 1.0 || suv.y < 0.0 || suv.y > 1.0) continue;\n"
    "        float4 as = albedo_tex.sample(smp, suv);\n"
    "        if (as.a < 0.01) continue;\n"
    "        float ds = depth_tex.sample(smp, suv).r;\n"
    "        float3 Ws = reconstruct_world(suv, ds, p, fbw, fbh);\n"
    "        float3 V = Ws - W;\n"
    "        float dist = length(V);\n"
    "        if (dist < 1e-4 || dist > max_dist) continue;\n"
    "        float hemi = pow(max(dot(N, normalize(V)) - bias, 0.0), 1.35);\n"
    "        float attn = 1.0 - dist / max_dist;\n"
    "        attn *= attn;\n"
    "        occ += hemi * attn;\n"
    "        wsum += attn;\n"
    "    }\n"
    "    if (wsum <= 1e-5) return 1.0;\n"
    "    float occ_n = clamp(occ / wsum, 0.0, 1.0);\n"
    "    occ_n = pow(occ_n, 0.75);\n"
    "    return clamp(1.0 - strength * occ_n, 0.0, 1.0);\n"
    "}\n";

}  // namespace

void TDisplay::InitAOPipeline()
{
    sg_shader_desc sh = {};
    sh.attrs[0].name = "pos";
    sh.attrs[1].name = "uv";
    sh.vs.source = kAOVsMetal;
    sh.vs.entry  = "_main";
    sh.fs.source = kAOFsMetal;
    sh.fs.entry  = "_main";
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
    sh.label = "display.ao.shader";
    ao_shader = sg_make_shader(&sh);

    sg_pipeline_desc pip = {};
    pip.shader = ao_shader;
    pip.layout.attrs[0].format  = SG_VERTEXFORMAT_FLOAT2;
    pip.layout.attrs[1].format  = SG_VERTEXFORMAT_FLOAT2;
    pip.primitive_type          = SG_PRIMITIVETYPE_TRIANGLES;
    pip.colors[0].pixel_format  = SG_PIXELFORMAT_R32F;
    pip.colors[0].blend.enabled = false;
    pip.depth.pixel_format      = SG_PIXELFORMAT_NONE;
    pip.label = "display.ao.pipeline";
    ao_pipeline = sg_make_pipeline(&pip);
}

void TDisplay::ShutdownAOPipeline()
{
    if (ao_pipeline.id) { sg_destroy_pipeline(ao_pipeline); ao_pipeline = {}; }
    if (ao_shader.id)   { sg_destroy_shader(ao_shader);     ao_shader   = {}; }
}

void TDisplay::RunAOPass()
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

// ---- Deferred lighting pass ---------------------------------------------
// Reads the G-buffer (albedo + world normal + depth), reconstructs each
// fragment's world position via the iso inverse, and runs directional +
// point-light shading. Writes lit_target, which FlipPage composites.
//
// Iso inverse derivation (retail uses cos30 = 867/1000):
//   rel     = world - camera_center
//   screen S = rel.x - rel.y
//   screen T = (rel.x+rel.y)/2 - rel.z*cos30
//   scene_z  = kcam - (rel.x+rel.y)*cos30 - rel.z/2     (CameraDepth)
//
// Given (S, T, scene_z):
//   K       = kcam - scene_z
//   wz      = (K - 2*T*cos30) / (2*cos30^2 + 0.5)
//   sum_r   = 2*(T + wz*cos30)
//   rel.x/y = ((sum_r ± S) / 2)
//   world   = rel + camera_center
// Fragment pixel -> (S, T) via S = uv.x*fb_w - ox, T = uv.y*fb_h - oy,
// where (ox, oy) already includes any debug pan offset.

namespace {

const char* kLightVsMetal =
    "#include <metal_stdlib>\n"
    "using namespace metal;\n"
    "struct vs_in  { float2 pos [[attribute(0)]]; float2 uv [[attribute(1)]]; };\n"
    "struct vs_out { float4 pos [[position]];     float2 uv; };\n"
    "vertex vs_out _main(vs_in in [[stage_in]]) {\n"
    "    vs_out o;\n"
    "    o.pos = float4(in.pos * 2.0 - 1.0, 0.0, 1.0);\n"
    "    o.uv  = in.uv;\n"
    "    return o;\n"
    "}\n";

// params:
//   vp       .xy = screen origin for camera center; .zw = z_near, zspan
//   recon    .xy = world_center.xy;                 .z = kcam_forward
//             .w  = reserved
//   light_dir .xyz = dir,            .w  = intensity
//   light_col .xyz = directional color, .w = ambient scalar
//   ambient_col.xyz = ambient color, .w = reserved
//   settings .x = view_mode (0 lit, 1 albedo, 2 depth, 3 normal,
//                 4 plight dbg, 5 recon heatmap, 6 shadow dbg, 7 ao dbg),
//            .y = plight_count,
//            .z = lighting_mode (0=retail 1998, 1=modern),
//            .w = reserved
//   plight_pos[N] .xyz = world xyz,  .w  = radius (wu)
//   plight_col[N] .xyz = rgb,        .w  = intensity
//   shadow   .x = step_wu (wu advanced per ray-march step),
//            .y = softness_px (perpendicular jitter radius in pixels,
//                 drives penumbra width — 0 = hard shadow),
//            .z = max_steps (cap on ray length),
//            .w = enable
//   shadow_dir .xy = additive screen-space variance (pixels-per-wu) applied
//                    on top of the iso projection of shadow_world_dir.
//              .z  = multiplier applied to shadow_world_dir.z for the ray's
//                    world-z climb (shadow length — larger = shorter shadow).
//              .w  = reserved
//   shadow_world_dir .xyz = base world-space direction for the sun-shadow
//                           march, decoupled from light_dir
//                    .w   = reserved
//
// Ray-march is directional only; the tile depth buffer is camera-facing
// billboards with no back-side, so occluders must fall on pixels visible
// to the camera. In practice that means the sun must be oriented so the
// shadow falls toward/behind the viewer (front-and-to-the-right works
// well). Lights coming from behind the scene can't produce silhouettes —
// the pixels we'd need aren't in the G-buffer.
const char* kLightFsMetal =
    "#include <metal_stdlib>\n"
    "using namespace metal;\n"
    "#define ISO_COS30 0.867\n"
    "#define ISO_WZ_DENOM 2.003378\n"
    "#define KPL 16\n"
    "struct params {\n"
    "    float4 vp; float4 recon; float4 light_dir; float4 light_col;\n"
    "    float4 ambient_col; float4 settings;\n"
    "    float4 plight_pos[KPL]; float4 plight_col[KPL];\n"
    "    float4 shadow;\n"
    "    float4 shadow_dir;\n"
    "    float4 shadow_world_dir;\n"
    "    float4 normal_lighting;\n"
    "};\n"
    "struct vs_out { float4 pos [[position]]; float2 uv; };\n"
    "fragment float4 _main(vs_out in [[stage_in]],\n"
    "                      texture2d<float> albedo_tex [[texture(0)]],\n"
    "                      texture2d<float> normal_tex [[texture(1)]],\n"
    "                      texture2d<float> depth_tex  [[texture(2)]],\n"
    "                      texture2d<float> ao_tex     [[texture(3)]],\n"
    "                      sampler smp                [[sampler(0)]],\n"
    "                      constant params& p         [[buffer(0)]]) {\n"
    "    float4 alb = albedo_tex.sample(smp, in.uv);\n"
    "    if (alb.a < 0.01) discard_fragment();\n"
    "    float  d   = depth_tex.sample(smp, in.uv).r;\n"
    "    float3 np  = normal_tex.sample(smp, in.uv).xyz;\n"
    "    float3 N   = normalize(np * 2.0 - 1.0);\n"
    "    float  ao  = ao_tex.sample(smp, in.uv).r;\n"
    "    float  fbw = float(albedo_tex.get_width());\n"
    "    float  fbh = float(albedo_tex.get_height());\n"
    "    float  S   = in.uv.x * fbw - p.vp.x;\n"
    "    float  T   = in.uv.y * fbh - p.vp.y;\n"
    "    float  scene_z = d * p.vp.w + p.vp.z;\n"
    "    float  K   = p.recon.z - scene_z;\n"
    "    float  wz  = (K - 2.0 * T * ISO_COS30) / ISO_WZ_DENOM;\n"
    "    float  sum_r = 2.0 * (T + wz * ISO_COS30);\n"
    "    float3 W = float3(p.recon.x + (sum_r + S) * 0.5,\n"
    "                      p.recon.y + (sum_r - S) * 0.5,\n"
    "                      wz);\n"
    "    int vm = int(p.settings.x);\n"
    "    if (vm == 1) return float4(alb.rgb, 1.0);\n"
    "    if (vm == 2) { float vd = clamp(1.0 - d, 0.0, 1.0); return float4(vd, vd, vd, 1.0); }\n"
    "    if (vm == 3) return float4(np, 1.0);\n"
    "    int nl_dbg = int(p.settings.y);\n"
    "    if (vm == 4) {\n"
    "        // Point-light debug: accumulate the same attenuation * NdotL\n"
    "        // term used by lit mode, but without albedo modulation.\n"
    "        // This makes it obvious whether reconstructed normals are\n"
    "        // participating in the point-light response.\n"
    "        float3 accum = float3(0.0);\n"
    "        const float kMP = 0.0022436f;\n"
    "        const float kSC = 50.0 / (1.0 - kMP);\n"
    "        for (int i = 0; i < nl_dbg; ++i) {\n"
    "            float3 delta = p.plight_pos[i].xyz - W;\n"
    "            float  dist  = length(delta);\n"
    "            float  rad   = p.plight_pos[i].w;\n"
    "            if (rad > 0.0 && dist < rad) {\n"
    "                float normd = dist * 254.0 / rad;\n"
    "                float pw    = pow(normd + 1.0, -1.1) - kMP;\n"
    "                float attn  = clamp(pw * kSC, 0.0, 1.0);\n"
    "                accum += p.plight_col[i].rgb * p.plight_col[i].w * attn;\n"
    "            }\n"
    "        }\n"
    "        return float4(accum, 1.0);\n"
    "    }\n"
    "    if (vm == 5) {\n"
    "        // Reconstructed W as color. R=wx, G=wy, B=wz, each scaled to a\n"
    "        // readable range. settings.z = world_scale (wu per full intensity),\n"
    "        // settings.w = wz_scale. Use the sector panel sliders to tune.\n"
    "        float ws = max(p.settings.z, 1.0);\n"
    "        float zs = max(p.settings.w, 1.0);\n"
    "        return float4(fract(W.x / ws), fract(W.y / ws), fract(W.z / zs), 1.0);\n"
    "    }\n"
    "    if (vm == 7) return float4(ao, ao, ao, 1.0);\n"
    "    // Ambient (always on — retail fills unlit pixels from a global floor).\n"
    "    float3 light = p.ambient_col.rgb * p.light_col.w;\n"
    "    int mode = int(p.settings.z);  // 0=retail 1998, 1=modern\n"
    "    if (mode == 1) light *= ao;\n"
    "    // --- Sun-direction ray-march shadow -------------------------------\n"
    "    // March from the fragment toward the sun through screen space. At\n"
    "    // each step, sample the scene world-z at the ray's projected UV and\n"
    "    // compare against the ray's own world-z at that distance. If the\n"
    "    // scene is taller, a closer piece of geometry occludes us.\n"
    "    //\n"
    "    // Soft shadows: do kRays samples with a small perpendicular offset\n"
    "    // (softness_px in screen pixels, 0 = single hard ray). Averaging\n"
    "    // the hit-count gives a natural penumbra for pixel-art tiles.\n"
    "    float sun_shadow = 1.0;\n"
    "    float3 Ldir = normalize(p.light_dir.xyz);\n"
    "    float  sun_ndotl = max(dot(N, Ldir), 0.0);\n"
    "    float  normal_hardness = clamp(p.normal_lighting.x, 0.0, 1.0);\n"
    "    float  sun_term = mix(1.0, sun_ndotl, normal_hardness);\n"
    "    if (mode == 1 && p.shadow.w > 0.5 && sun_term > 0.0) {\n"
    "        float3 Sdir = normalize(p.shadow_world_dir.xyz);\n"
    "        if (length(Sdir) < 1e-5) Sdir = Ldir;\n"
    "        // Keep the legacy soft-shadowing march, but derive the projected\n"
    "        // ray from the dedicated shadow world direction.\n"
    "        float S_per = (-Sdir.x + Sdir.y) + p.shadow_dir.x;\n"
    "        float T_per = ((-Sdir.x - Sdir.y) * 0.5 + Sdir.z * ISO_COS30)\n"
    "                    + p.shadow_dir.y;\n"
    "        float Sz    = Sdir.z * p.shadow_dir.z;\n"
    "        float step_wu  = p.shadow.x;\n"
    "        float soft_px  = p.shadow.y;\n"
    "        int   maxSteps = int(p.shadow.z);\n"
    "        const float kBiasWu = 2.0;   // self-occlusion tolerance\n"
    "        // Perpendicular to sun dir in screen space — tangent for jitter.\n"
    "        float2 sun_uv  = float2(S_per, T_per);\n"
    "        float2 perp_uv = normalize(float2(-sun_uv.y, sun_uv.x));\n"
    "        float h = fract(sin(dot(in.uv, float2(12.9898, 78.233))) * 43758.5453);\n"
    "        const int kRays = 4;\n"
    "        float hits = 0.0;\n"
    "        for (int r = 0; r < kRays; ++r) {\n"
    "            // Spread rays across ±soft_px perpendicular to the sun dir.\n"
    "            float rf = (float(r) + h) / float(kRays);          // 0..1\n"
    "            float off_px = (rf - 0.5) * 2.0 * soft_px;\n"
    "            float2 perp_off = perp_uv * off_px / float2(fbw, fbh);\n"
    "            float rh = fract(h + float(r) * 0.6180339);\n"
    "            bool hit = false;\n"
    "            for (int i = 1; i <= maxSteps; ++i) {\n"
    "                float t = (float(i) - 0.5 + rh) * step_wu;       // wu\n"
    "                float2 suv = in.uv + perp_off +\n"
    "                             float2(S_per * t / fbw, T_per * t / fbh);\n"
    "                if (suv.x < 0.0 || suv.x > 1.0 ||\n"
    "                    suv.y < 0.0 || suv.y > 1.0) break;\n"
    "                float4 albs = albedo_tex.sample(smp, suv);\n"
    "                if (albs.a < 0.01) continue;\n"
    "                float drs = depth_tex.sample(smp, suv).r;\n"
    "                float szs = drs * p.vp.w + p.vp.z;\n"
    "                float Tt  = suv.y * fbh - p.vp.y;\n"
    "                float Ks  = p.recon.z - szs;\n"
    "                float wzs = (Ks - 2.0 * Tt * ISO_COS30) / ISO_WZ_DENOM;\n"
    "                float ray_wz = W.z + t * Sz;\n"
    "                if (wzs > ray_wz + kBiasWu) { hit = true; break; }\n"
    "            }\n"
    "            if (hit) hits += 1.0;\n"
    "        }\n"
    "        float occ = hits / float(kRays);\n"
    "        sun_shadow = 1.0 - occ * 0.8;  // max ~80% darken\n"
    "    }\n"
    "    if (mode == 1) {\n"
    "        light += p.light_col.rgb * p.light_dir.w * sun_term * sun_shadow;\n"
    "    }\n"
    "    // Shadow debug: white = lit, black = shadowed.\n"
    "    if (vm == 6) return float4(float3(sun_shadow), 1.0);\n"
    "    // --- Point lights (retail 1998 accurate) --------------------------\n"
    "    // colortable.cpp GetLightBrightness / MakeColorTables:\n"
    "    // BrightnessTable[d] = min(((d+1)^-1.1 - 256^-1.1) / (1 - 256^-1.1)\n"
    "    // * 50, 1). Distance is normalized to 254 by the light's radius;\n"
    "    // inside that, the exponential curve drops to 0 at the edge. Retail\n"
    "    // used pure distance (no N·L); the MMX pre-release path added\n"
    "    // Lambert but never shipped — do not reintroduce.\n"
    "    const float kMinPower = 0.0022436f;          // pow(256, -1.1)\n"
    "    const float kScale    = 50.0 / (1.0 - kMinPower);\n"
    "    int nl = int(p.settings.y);\n"
    "    for (int i = 0; i < KPL; ++i) {\n"
    "        if (i >= nl) break;\n"
    "        float3 delta = p.plight_pos[i].xyz - W;\n"
    "        float  dist  = length(delta);\n"
    "        float  rad   = p.plight_pos[i].w;\n"
    "        if (rad > 0.0 && dist < rad) {\n"
    "            float  normd = dist * 254.0 / rad;\n"
    "            float  pw    = pow(normd + 1.0, -1.1) - kMinPower;\n"
    "            float  attn  = clamp(pw * kScale, 0.0, 1.0);\n"
    "            light += p.plight_col[i].rgb * p.plight_col[i].w * attn;\n"
    "        }\n"
    "    }\n"
    "    return float4(alb.rgb * light, 1.0);\n"
    "}\n";

}  // namespace

void TDisplay::InitLightPipeline()
{
    constexpr int32_t kPL = TDisplay::kMaxPointLights;
    // 6 scalar vec4s + kPL vec4s of plight_pos + kPL vec4s of plight_col
    // + trailing shadow params + shadow_dir variance + shadow_world_dir
    // + normal-lighting controls.
    // New vec4s go at the end so adding them doesn't shift earlier offsets.
    constexpr int32_t kUB_vec4s = 6 + kPL + kPL + 4;
    constexpr int32_t kUB_bytes = kUB_vec4s * 16;

    sg_shader_desc sh = {};
    sh.attrs[0].name = "pos";
    sh.attrs[1].name = "uv";
    sh.vs.source = kLightVsMetal;
    sh.vs.entry  = "_main";
    sh.fs.source = kLightFsMetal;
    sh.fs.entry  = "_main";
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
    sh.label = "display.light.shader";
    light_shader = sg_make_shader(&sh);

    sg_pipeline_desc pip = {};
    pip.shader = light_shader;
    pip.layout.attrs[0].format  = SG_VERTEXFORMAT_FLOAT2;
    pip.layout.attrs[1].format  = SG_VERTEXFORMAT_FLOAT2;
    pip.primitive_type          = SG_PRIMITIVETYPE_TRIANGLES;
    pip.colors[0].pixel_format  = SG_PIXELFORMAT_RGBA8;
    pip.colors[0].blend.enabled = false;
    pip.depth.pixel_format      = SG_PIXELFORMAT_NONE;
    pip.label = "display.light.pipeline";
    light_pipeline = sg_make_pipeline(&pip);
}

void TDisplay::ShutdownLightPipeline()
{
    if (light_pipeline.id) { sg_destroy_pipeline(light_pipeline); light_pipeline = {}; }
    if (light_shader.id)   { sg_destroy_shader(light_shader);     light_shader   = {}; }
}

void TDisplay::SetReconstructionParams(float ox, float oy,
                                       float z_near, float z_far,
                                       float center_wx, float center_wy,
                                       float kcam_forward, float reserved)
{
    // Callers pass the world-origin screen offset in display-space pixels.
    // The light shader samples the padded G-buffer, so shift by +pad to
    // keep the world→screen math consistent.
    recon.ox           = ox + float(kGBufPad);
    recon.oy           = oy + float(kGBufPad);
    recon.z_near       = z_near;
    recon.zspan        = z_far - z_near;
    recon.center_wx    = center_wx;
    recon.center_wy    = center_wy;
    recon.kcam_forward = kcam_forward;
    recon.reserved     = reserved;
}

void TDisplay::RunLightingPass()
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
    bind.fs_images[0]      = color_target;    // albedo
    bind.fs_images[1]      = normal_target;   // world normal
    bind.fs_images[2]      = scene_z_target;  // scene_z in [0,1]
    bind.fs_images[3]      = ao_target;       // ambient occlusion
    sg_apply_bindings(&bind);

    constexpr int32_t kPL = TDisplay::kMaxPointLights;
    constexpr int32_t kUB_vec4s = 6 + kPL + kPL + 4;
    float u[kUB_vec4s * 4] = {};
    int32_t o = 0;
    // vp
    u[o++] = recon.ox;    u[o++] = recon.oy;
    u[o++] = recon.z_near; u[o++] = recon.zspan;
    // recon
    u[o++] = recon.center_wx; u[o++] = recon.center_wy;
    u[o++] = recon.kcam_forward; u[o++] = recon.reserved;
    // light_dir
    u[o++] = light.dir[0]; u[o++] = light.dir[1]; u[o++] = light.dir[2]; u[o++] = light.intensity;
    // light_col
    u[o++] = light.color[0]; u[o++] = light.color[1]; u[o++] = light.color[2]; u[o++] = light.ambient;
    // ambient_col
    u[o++] = light.ambient_color[0]; u[o++] = light.ambient_color[1];
    u[o++] = light.ambient_color[2]; u[o++] = 0.0f;
    // settings: .x=view_mode, .y=plight_count, .z=lighting_mode, .w=reserved
    u[o++] = float(light.view_mode);
    u[o++] = float(light.plight_count);
    u[o++] = float(light.mode);
    u[o++] = 0.0f;
    // plight_pos[]
    for (int32_t i = 0; i < kPL; ++i)
        for (int32_t k = 0; k < 4; ++k) u[o++] = light.plight_pos[i][k];
    // plight_col[]
    for (int32_t i = 0; i < kPL; ++i)
        for (int32_t k = 0; k < 4; ++k) u[o++] = light.plight_col[i][k];
    // shadow: .x=step_wu, .y=softness_px, .z=max_steps, .w=enable
    u[o++] = light.sun_shadow_step_wu;
    u[o++] = light.sun_shadow_softness_px;
    u[o++] = float(light.sun_shadow_max_steps);
    u[o++] = light.sun_shadow_enable ? 1.0f : 0.0f;
    // shadow_dir: .xy=screen variance (px/wu), .z=wz multiplier, .w=reserved
    u[o++] = light.shadow_dir[0];
    u[o++] = light.shadow_dir[1];
    u[o++] = light.shadow_dir[2];
    u[o++] = 0.0f;
    // shadow_world_dir: .xyz=base world-space shadow ray, .w=reserved
    u[o++] = light.shadow_world_dir[0];
    u[o++] = light.shadow_world_dir[1];
    u[o++] = light.shadow_world_dir[2];
    u[o++] = 0.0f;
    // normal_lighting: .x=flat<->normal directional blend, .yzw=reserved
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

void TDisplay::BeginTilePass(float r, float g, float b, float a)
{
    if (!default_pass.id) return;
    tile_clear_rgba[0] = r; tile_clear_rgba[1] = g;
    tile_clear_rgba[2] = b; tile_clear_rgba[3] = a;
    sg_pass_action pa = {};
    // color_target (albedo) — clear to transparent. The light pass's fragment
    // shader discards on alpha < 0.01, so pixels no tile drew to just get the
    // lit_pass clear color (the user-supplied backdrop). This is how we get
    // the backdrop through without running bogus lighting on pixels whose
    // normal_target / scene_z_target slots were never written.
    pa.colors[0].action = SG_ACTION_CLEAR;
    pa.colors[0].value  = { 0.0f, 0.0f, 0.0f, 0.0f };
    // normal_target / scene_z_target — sokol defaults to CLEAR with (0.5,…).
    // Exact values don't matter because the light pass discards those pixels.
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

void TDisplay::DrawTile(sg_image color_img, sg_image depth_img,
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

    // Caller passes display-space pixel coords; the G-buffer is padded by
    // kGBufPad on every side, so shift by +pad and use the padded dims as
    // the NDC denominator. Pixel Y is down, NDC Y is up.
    const int32_t pad = kGBufPad;
    const int32_t gbw = width  + 2 * pad;
    const int32_t gbh = height + 2 * pad;
    const int32_t px  = dst_x + pad;
    const int32_t py  = dst_y + pad;
    const float nx = (2.0f * px / gbw)  - 1.0f;
    const float nw = (2.0f * dst_w) / gbw;
    const float ny = 1.0f - (2.0f * (py + dst_h) / gbh);
    const float nh = (2.0f * dst_h) / gbh;

    // 5 vec4s: rect, zparams, tile_root, tile_sprite, filter.
    float uniforms[5 * 4] = {};
    int32_t off = 0;
    // rect
    uniforms[off++] = nx; uniforms[off++] = ny; uniforms[off++] = nw; uniforms[off++] = nh;
    // zparams: anchor_z, depth_mul, normal_mul, unused
    uniforms[off++] = anchor_z;
    uniforms[off++] = depth_mul;
    uniforms[off++] = normal_mul;
    uniforms[off++] = 0.0f;
    // tile_root: world xyz of the tile's anchor pixel, .w = zraw_to_wu scale
    uniforms[off++] = root_wx; uniforms[off++] = root_wy;
    uniforms[off++] = root_wz; uniforms[off++] = zraw_to_wu;
    // tile_sprite: xy = anchor pixel in sprite-local coords, zw = sprite size
    uniforms[off++] = anchor_px_x; uniforms[off++] = anchor_px_y;
    uniforms[off++] = float(dst_w); uniforms[off++] = float(dst_h);
    // filter: .x = normal-filter radius in texels, .y = bilateral edge threshold
    uniforms[off++] = light.normal_radius;
    uniforms[off++] = light.edge_threshold;
    uniforms[off++] = 0.0f;
    uniforms[off++] = 0.0f;

    const sg_range u_range = { uniforms, sizeof(uniforms) };
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &u_range);
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, &u_range);
    sg_draw(0, 6, 1);
}

void TDisplay::SetLight(float dx, float dy, float dz, float intensity,
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

void TDisplay::SetAmbientColor(float r, float g, float b)
{
    light.ambient_color[0] = r;
    light.ambient_color[1] = g;
    light.ambient_color[2] = b;
}

void TDisplay::SetAmbientOcclusion(bool enable, float radius_px, float strength,
                                   float bias, float max_dist_wu)
{
    light.ao_enable = enable;
    light.ao_radius_px = radius_px;
    light.ao_strength = strength;
    light.ao_bias = bias;
    light.ao_max_dist_wu = max_dist_wu;
}

void TDisplay::SetShadowWorldDir(float dx, float dy, float dz)
{
    light.shadow_world_dir[0] = dx;
    light.shadow_world_dir[1] = dy;
    light.shadow_world_dir[2] = dz;
}

void TDisplay::SetShadowVariance(float sx, float sy, float sz)
{
    light.shadow_dir[0] = sx;
    light.shadow_dir[1] = sy;
    light.shadow_dir[2] = sz;
}

void TDisplay::SetTileViewMode(int32_t mode)
{
    light.view_mode = mode;
}

void TDisplay::SetLightingMode(int32_t mode)
{
    light.mode = mode;
}

void TDisplay::SetSunShadow(bool enable, float step_wu, float softness_px,
                            int32_t max_steps)
{
    light.sun_shadow_enable     = enable;
    light.sun_shadow_step_wu    = step_wu;
    light.sun_shadow_softness_px = softness_px;
    light.sun_shadow_max_steps  = max_steps;
}

void TDisplay::SetNormalRadius(float texels)
{
    light.normal_radius = texels;
}

void TDisplay::SetEdgeThreshold(float zraw_units)
{
    light.edge_threshold = zraw_units;
}

void TDisplay::SetNormalLightingHardness(float hardness)
{
    if (hardness < 0.0f) hardness = 0.0f;
    if (hardness > 1.0f) hardness = 1.0f;
    light.normal_lighting_hardness = hardness;
}

void TDisplay::ClearPointLights()
{
    light.plight_count = 0;
}

void TDisplay::AddPointLight(float wx, float wy, float wz, float radius_wu,
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

void TDisplay::EndTilePass()
{
    sg_end_pass();
}

// Fullscreen quad across the swapchain (sokol's default pass). Only called
// from FlipPage — hence the `_swap` pipeline variant.
void TDisplay::Composite(TSurface* src)
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
        -1.0f, -1.0f, 2.0f, 2.0f,   // rect: fullscreen NDC
         0.0f,  0.0f, 1.0f, 1.0f,   // uv_rect: full image
    };
    const sg_range u_range = { uniforms, sizeof(uniforms) };
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &u_range);
    sg_draw(0, 6, 1);
}

// Sub-rect draw into an RGBA8 render-target surface (e.g. the backbuffer).
// Caller must be inside that surface's pass.
void TDisplay::Composite(sg_image img,
                         int32_t dst_x, int32_t dst_y, int32_t dst_w, int32_t dst_h,
                         int32_t target_w, int32_t target_h)
{
    // Delegate to the atlas-friendly variant with a whole-image source rect.
    Composite(img,
              dst_x, dst_y, dst_w, dst_h,
              target_w, target_h,
              0, 0, 1, 1,
              1, 1);
}

void TDisplay::Composite(sg_image img,
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

    // Dest pixel rect -> NDC. NDC Y up, pixel Y down, so flip Y.
    const float nx = (2.0f * dst_x / target_w) - 1.0f;
    const float nw = (2.0f * dst_w) / target_w;
    const float ny = 1.0f - (2.0f * (dst_y + dst_h) / target_h);
    const float nh = (2.0f * dst_h) / target_h;

    // Source pixel rect -> UV. Top-left of the subregion is (u0, v0); the
    // vertex UVs already bake in the y-flip so the glyph lands right-side up.
    const float u0 = float(src_x) / float(src_tex_w);
    const float v0 = float(src_y) / float(src_tex_h);
    const float uw = float(src_w) / float(src_tex_w);
    const float vh = float(src_h) / float(src_tex_h);

    const float uniforms[8] = { nx, ny, nw, nh,  u0, v0, uw, vh };
    const sg_range u_range = { uniforms, sizeof(uniforms) };
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &u_range);
    sg_draw(0, 6, 1);
}

bool TDisplay::Close()
{
    simgui_shutdown();
    ShutdownLightPipeline();
    ShutdownAOPipeline();
    ShutdownTilePipeline();
    ShutdownCompositePipeline();

    if (!frontbuffer)
        return true;

    if (frontbuffer && frontbuffer == backbuffer)
    {
        TSurface* tmp = frontbuffer;
        frontbuffer = backbuffer;
        backbuffer = tmp;
    }

    if (backbuffer)
    {
        delete backbuffer;
        backbuffer = nullptr;
    }

    // Destroy zbuffer surface
    if (zbuffer)
    {
        delete zbuffer;
        zbuffer = nullptr;
    }

    // Destroy front surface
    if (frontbuffer)
    {
        delete frontbuffer;
        frontbuffer = nullptr;
    }

    width = height = 0;

    return true;
}

TDisplay::~TDisplay()
{
    Close();
}

bool TDisplay::Restore()
{
    if (!frontbuffer)
        return false;

    if (frontbuffer->Lost())
    {
        frontbuffer->Restore();
        if (!Windowed)
            frontbuffer->Clear();
    }

    if (backbuffer && backbuffer->Lost())
    {
        backbuffer->Restore();
        backbuffer->Clear();
    }

    return true;
}

// Draws the backbuffer onto the swapchain. The caller is expected to have
// already issued whatever draws it wants *into* the backbuffer (inside
// backbuffer->StartPass/EndPass) earlier in the frame.
bool TDisplay::FlipPage(bool /*Wait*/)
{
    if (!backbuffer)
        return false;

    sg_pass_action pa = {};
    pa.colors[0].action = SG_ACTION_CLEAR;
    pa.colors[0].value  = { 0.0f, 0.0f, 0.0f, 1.0f };
    pa.depth.action     = SG_ACTION_DONTCARE;
    pa.stencil.action   = SG_ACTION_DONTCARE;

    sg_begin_default_pass(&pa, sapp_width(), sapp_height());
    if (lit_target_dirty || color_target_dirty)
    {
        // Tile/lighting output is the final image for this frame. The
        // backbuffer in this mode is undefined (never cleared/drawn), so
        // compositing it on top would paint garbage (initial pink) over
        // the tiles. UI/widget modes will reintroduce the backbuffer
        // composite when they start writing to it.
        //
        // Prefer lit_target (post-deferred-lighting). If the caller ran
        // BeginTilePass without RunLightingPass (e.g. a pre-lighting debug
        // build), fall back to color_target so the albedo still shows.
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
    }
    else
    {
        Composite(backbuffer);
    }
    simgui_render();
    sg_end_pass();
    sg_commit();

    return true;
}
