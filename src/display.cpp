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
    desc.image_pool_size = 256;
    desc.shader_pool_size = 64;
    desc.pipeline_pool_size = 64;
    desc.pass_pool_size = 32;
    sg_setup(&desc);

    // Create render target images
    sg_image_desc rt_desc = {};
    rt_desc.width = width;
    rt_desc.height = height;
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

    // Create render passes
    sg_pass_desc pass_desc = {};

    // Main render pass
    pass_desc.color_attachments[0].image = color_target;
    pass_desc.depth_stencil_attachment.image = depth_target;
    default_pass = sg_make_pass(&pass_desc);

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

    // Dear ImGui overlay — used for in-game debug/tuning panels (lighting,
    // depth scale, camera). Draws into the swapchain pass in FlipPage.
    {
        simgui_desc_t d = {};
        d.ini_filename = "imgui.ini";
        simgui_setup(&d);
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

// ---- Tile rendering pipeline ---------------------------------------------
// Draws a sprite into (color_target, depth_target) with per-pixel depth
// written from a companion depthmap texture. The fragment shader samples
// depth_tex (normalized [0,1] delta) and writes `[[depth(any)]] = z_base +
// sample * z_scale`, so overlapping sprites interpenetrate at the sample
// level rather than sprite-level painter order. Matches Revenant's 2.5D
// composition where each tile/object ships with a per-pixel depthmap.

namespace {

// zparams.zw carries the depth-derivative scale used to convert per-pixel
// dfdx/dfdy of the sampled depth into a surface-space slope. light_dir.w is
// the diffuse intensity; light_col.w is the ambient term. Normal is
// reconstructed from the tile's own authored depthmap (per-pixel depth is
// shipped with every tile bitmap), so lighting happens inline — no separate
// g-buffer or fullscreen light pass is needed for this first tier.
// Matches TDisplay::kMaxPointLights. Keep in sync — the Metal struct layout
// is a straight memcpy from DrawTile's uniforms buffer.
#define TILE_MAX_POINT_LIGHTS 16

const char* kTileVsMetal =
    "#include <metal_stdlib>\n"
    "using namespace metal;\n"
    "#define MAX_PL 16\n"
    "struct params { float4 rect; float4 zparams; float4 light_dir; float4 light_col;\n"
    "                float4 plight_count;\n"
    "                float4 plight_pos[MAX_PL]; float4 plight_col[MAX_PL]; };\n"
    "struct vs_in  { float2 pos [[attribute(0)]]; float2 uv [[attribute(1)]]; };\n"
    "struct vs_out { float4 pos [[position]]; float2 uv; };\n"
    "vertex vs_out _main(vs_in in [[stage_in]], constant params& p [[buffer(0)]]) {\n"
    "    vs_out o;\n"
    "    o.pos = float4(p.rect.xy + in.pos * p.rect.zw, 0.0, 1.0);\n"
    "    o.uv  = in.uv;\n"
    "    return o;\n"
    "}\n";

const char* kTileFsMetal =
    "#include <metal_stdlib>\n"
    "using namespace metal;\n"
    "#define MAX_PL 16\n"
    "struct params { float4 rect; float4 zparams; float4 light_dir; float4 light_col;\n"
    "                float4 plight_count;\n"
    "                float4 plight_pos[MAX_PL]; float4 plight_col[MAX_PL]; };\n"
    "struct vs_out { float4 pos [[position]]; float2 uv; };\n"
    "struct fs_out { float4 color [[color(0)]]; float depth [[depth(any)]]; };\n"
    "fragment fs_out _main(vs_out in [[stage_in]],\n"
    "                      texture2d<float> color_tex [[texture(0)]],\n"
    "                      texture2d<float> depth_tex [[texture(1)]],\n"
    "                      sampler smp              [[sampler(0)]],\n"
    "                      constant params& p       [[buffer(0)]]) {\n"
    "    fs_out o;\n"
    "    float4 c = color_tex.sample(smp, in.uv);\n"
    "    if (c.a < 0.01) discard_fragment();\n"
    "    float zd = depth_tex.sample(smp, in.uv).r;\n"
    "    float d  = clamp(p.zparams.x + zd * p.zparams.y, 0.0, 1.0);\n"
    "    float s  = p.zparams.z;\n"
    "    float3 N = normalize(float3(-dfdx(zd)*s, -dfdy(zd)*s, 1.0));\n"
    "    float3 L = normalize(p.light_dir.xyz);\n"
    "    float ndotl = max(dot(N, L), 0.0) * p.light_dir.w;\n"
    "    float3 lit = c.rgb * (p.light_col.w + p.light_col.rgb * ndotl);\n"
    "    // Screen-space point lights. All coords in pixels; fragment sits on\n"
    "    // the screen plane (z=0). Lights float above at plight_pos[i].z.\n"
    "    // Linear falloff squared for a softer knee.\n"
    "    int n = int(p.plight_count.x);\n"
    "    if (n > MAX_PL) n = MAX_PL;\n"
    "    float3 frag_p = float3(in.pos.xy, 0.0);\n"
    "    for (int i = 0; i < n; ++i) {\n"
    "        float3 Lv    = p.plight_pos[i].xyz - frag_p;\n"
    "        float  rad   = max(p.plight_pos[i].w, 1e-4);\n"
    "        float  dist  = length(Lv);\n"
    "        if (dist >= rad) continue;\n"
    "        float3 Ldir  = Lv / max(dist, 1e-4);\n"
    "        float  pdotn = max(dot(N, Ldir), 0.0);\n"
    "        float  atten = 1.0 - dist / rad;\n"
    "        atten *= atten;\n"
    "        lit += c.rgb * p.plight_col[i].rgb * p.plight_col[i].w * pdotn * atten;\n"
    "    }\n"
    "    int mode = int(p.zparams.w);\n"
    "    float3 rgb;\n"
    "    if      (mode == 1) rgb = c.rgb;          // albedo only\n"
    "    else if (mode == 2) rgb = float3(d);      // depth actually written to zbuffer\n"
    "    else if (mode == 3) rgb = N * 0.5 + 0.5;  // normals\n"
    "    else                rgb = lit;                    // lit (default)\n"
    "    o.color = float4(rgb, c.a);\n"
    "    o.depth = d;\n"
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

    // Uniform block: rect, zparams, light_dir, light_col, plight_count (5
    // vec4s) + plight_pos[N] + plight_col[N]. sokol's Metal path memcpys
    // the raw bytes into a constant buffer, so this matches the Metal
    // struct layout in kTile*Metal exactly.
    constexpr int32_t kPL = TDisplay::kMaxPointLights;
    constexpr int32_t kUB_vec4s = 5 + kPL + kPL;
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
    sh.vs.uniform_blocks[0].uniforms[2].name = "light_dir";
    sh.vs.uniform_blocks[0].uniforms[2].type = SG_UNIFORMTYPE_FLOAT4;
    sh.vs.uniform_blocks[0].uniforms[3].name = "light_col";
    sh.vs.uniform_blocks[0].uniforms[3].type = SG_UNIFORMTYPE_FLOAT4;
    sh.vs.uniform_blocks[0].uniforms[4].name = "plight_count";
    sh.vs.uniform_blocks[0].uniforms[4].type = SG_UNIFORMTYPE_FLOAT4;
    sh.vs.uniform_blocks[0].uniforms[5].name = "plight_pos";
    sh.vs.uniform_blocks[0].uniforms[5].type = SG_UNIFORMTYPE_FLOAT4;
    sh.vs.uniform_blocks[0].uniforms[5].array_count = kPL;
    sh.vs.uniform_blocks[0].uniforms[6].name = "plight_col";
    sh.vs.uniform_blocks[0].uniforms[6].type = SG_UNIFORMTYPE_FLOAT4;
    sh.vs.uniform_blocks[0].uniforms[6].array_count = kPL;
    sh.fs.source = kTileFsMetal;
    sh.fs.entry  = "_main";
    sh.fs.uniform_blocks[0].size = kUB_bytes;
    sh.fs.uniform_blocks[0].uniforms[0].name = "rect";
    sh.fs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[1].name = "zparams";
    sh.fs.uniform_blocks[0].uniforms[1].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[2].name = "light_dir";
    sh.fs.uniform_blocks[0].uniforms[2].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[3].name = "light_col";
    sh.fs.uniform_blocks[0].uniforms[3].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[4].name = "plight_count";
    sh.fs.uniform_blocks[0].uniforms[4].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[5].name = "plight_pos";
    sh.fs.uniform_blocks[0].uniforms[5].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[5].array_count = kPL;
    sh.fs.uniform_blocks[0].uniforms[6].name = "plight_col";
    sh.fs.uniform_blocks[0].uniforms[6].type = SG_UNIFORMTYPE_FLOAT4;
    sh.fs.uniform_blocks[0].uniforms[6].array_count = kPL;
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
    pip.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
    pip.colors[0].blend.enabled = true;
    pip.colors[0].blend.src_factor_rgb   = SG_BLENDFACTOR_SRC_ALPHA;
    pip.colors[0].blend.dst_factor_rgb   = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    pip.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
    pip.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
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

void TDisplay::BeginTilePass(float r, float g, float b, float a)
{
    if (!default_pass.id) return;
    sg_pass_action pa = {};
    pa.colors[0].action = SG_ACTION_CLEAR;
    pa.colors[0].value  = { r, g, b, a };
    pa.depth.action     = SG_ACTION_CLEAR;
    pa.depth.value      = 1.0f;
    pa.stencil.action   = SG_ACTION_DONTCARE;
    sg_begin_pass(default_pass, &pa);
    color_target_dirty = true;
}

void TDisplay::DrawTile(sg_image color_img, sg_image depth_img,
                        int32_t dst_x, int32_t dst_y, int32_t dst_w, int32_t dst_h,
                        float anchor_z, float z_scale)
{
    if (!tile_pipeline.id || !color_img.id || !depth_img.id) return;
    if (width <= 0 || height <= 0) return;

    sg_apply_pipeline(tile_pipeline);
    sg_bindings bind = {};
    bind.vertex_buffers[0] = tile_vbuf;
    bind.fs_images[0]      = color_img;
    bind.fs_images[1]      = depth_img;
    sg_apply_bindings(&bind);

    // Dest pixel rect -> NDC. Pixel Y is down, NDC Y is up.
    const float nx = (2.0f * dst_x / width)  - 1.0f;
    const float nw = (2.0f * dst_w) / width;
    const float ny = 1.0f - (2.0f * (dst_y + dst_h) / height);
    const float nh = (2.0f * dst_h) / height;

    // 5 vec4s of scalar params + kMaxPointLights vec4s of plight_pos + same
    // again for plight_col. Layout must match the Metal struct in kTile*Metal.
    constexpr int32_t kUB_vec4s = 5 + kMaxPointLights + kMaxPointLights;
    float uniforms[kUB_vec4s * 4] = {};
    int32_t off = 0;
    // rect
    uniforms[off++] = nx; uniforms[off++] = ny; uniforms[off++] = nw; uniforms[off++] = nh;
    // zparams: anchor_z, z_scale, deriv_scale, view_mode
    uniforms[off++] = anchor_z;
    uniforms[off++] = z_scale;
    uniforms[off++] = light.deriv_scale;
    uniforms[off++] = float(light.view_mode);
    // light_dir: dir + intensity
    uniforms[off++] = light.dir[0]; uniforms[off++] = light.dir[1];
    uniforms[off++] = light.dir[2]; uniforms[off++] = light.intensity;
    // light_col: rgb + ambient
    uniforms[off++] = light.color[0]; uniforms[off++] = light.color[1];
    uniforms[off++] = light.color[2]; uniforms[off++] = light.ambient;
    // plight_count.x
    uniforms[off++] = float(light.plight_count);
    uniforms[off++] = 0.0f; uniforms[off++] = 0.0f; uniforms[off++] = 0.0f;
    // plight_pos[kMaxPointLights]
    for (int32_t i = 0; i < kMaxPointLights; ++i)
        for (int32_t k = 0; k < 4; ++k) uniforms[off++] = light.plight_pos[i][k];
    // plight_col[kMaxPointLights]
    for (int32_t i = 0; i < kMaxPointLights; ++i)
        for (int32_t k = 0; k < 4; ++k) uniforms[off++] = light.plight_col[i][k];

    const sg_range u_range = { uniforms, sizeof(uniforms) };
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &u_range);
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, &u_range);
    sg_draw(0, 6, 1);
}

void TDisplay::SetLight(float dx, float dy, float dz, float intensity,
                        float r, float g, float b, float ambient,
                        float deriv_scale)
{
    light.dir[0] = dx; light.dir[1] = dy; light.dir[2] = dz;
    light.intensity   = intensity;
    light.color[0] = r; light.color[1] = g; light.color[2] = b;
    light.ambient     = ambient;
    light.deriv_scale = deriv_scale;
}

void TDisplay::SetTileViewMode(int32_t mode)
{
    light.view_mode = mode;
}

void TDisplay::ClearPointLights()
{
    light.plight_count = 0;
}

void TDisplay::AddPointLight(float x, float y, float z_above, float radius,
                             float r, float g, float b, float intensity)
{
    if (light.plight_count >= kMaxPointLights) return;
    const int32_t i = light.plight_count++;
    light.plight_pos[i][0] = x;
    light.plight_pos[i][1] = y;
    light.plight_pos[i][2] = z_above;
    light.plight_pos[i][3] = radius;
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
    if (color_target_dirty)
    {
        // Tile pass output is the final image for this frame. The
        // backbuffer in this mode is undefined (never cleared/drawn), so
        // compositing it on top would paint garbage (initial pink) over
        // the tiles. UI/widget modes will reintroduce the backbuffer
        // composite when they start writing to it.
        sg_apply_pipeline(composite_pip_swap);
        sg_bindings bind = {};
        bind.vertex_buffers[0] = composite_vbuf;
        bind.fs_images[0]      = color_target;
        sg_apply_bindings(&bind);
        const float u[8] = { -1.0f, -1.0f, 2.0f, 2.0f,  0.0f, 0.0f, 1.0f, 1.0f };
        const sg_range ur = { u, sizeof(u) };
        sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &ur);
        sg_draw(0, 6, 1);
        color_target_dirty = false;
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
