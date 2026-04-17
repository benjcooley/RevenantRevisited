// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 display.cpp  - Display Object File                    *
// *************************************************************************

#include "display.h"

#include <sokol_app.h>
#include <sokol_gfx.h>
#include <sokol_glue.h>

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
    bitsperpixel = 32; // Always use 32-bit RGBA color

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

    // Create front/back buffers as surfaces
    frontbuffer = new TSurface(color_target, width, height, bitsperpixel);
    backbuffer = new TSurface(sg_make_image(&rt_desc), width, height, bitsperpixel);
    zbuffer = new TSurface(depth_target, width, height, 32);

    return true;
}

bool TDisplay::Close()
{
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

// Flips the front/back buffer
bool TDisplay::FlipPage(bool /*Wait*/)
{
    if (!frontbuffer || frontbuffer->Lost())
        return false;

    // Set up pass action for clearing
    sg_pass_action pass_action = {};
    pass_action.colors[0] = { .action = SG_ACTION_DONTCARE };
    pass_action.depth = { .action = SG_ACTION_DONTCARE };
    pass_action.stencil = { .action = SG_ACTION_DONTCARE };

    // Begin default pass with viewport matching window size
    sg_begin_default_pass(&pass_action, sapp_width(), sapp_height());

#if 0 // TODO(port): real sokol_gfx path — Phase 3
    // Draw backbuffer to screen. The per-surface pipeline handle used to live
    // on TSurface; it's being moved onto TDisplay in the Phase 3 rewrite, so
    // the actual fullscreen blit call gets re-wired there.
    if (backbuffer) {
        sg_apply_pipeline(backbuffer->pipeline);

        sg_bindings bind = {};
        bind.fs.images[0] = backbuffer->GetSGImage();
        sg_apply_bindings(&bind);

        sg_draw(0, 6, 1);
    }
#endif

    // End pass and commit frame
    sg_end_pass();
    sg_commit();

    // Swap front/back buffers
    if (!SingleBuffer) {
        TSurface* tmp = frontbuffer;
        frontbuffer = backbuffer;
        backbuffer = tmp;
        currentpage = !currentpage;
    }

    return true;
}
