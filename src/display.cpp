// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 display.cpp  - Display Object File                    *
// *************************************************************************

#include "display.h"

#include "directdraw.h"
#include "graphics.h"
#include "mainwnd.h"
#include "bitmap.h"
#include "multisurface.h"

LPDIRECTDRAWSURFACE front;      // Pointer to DirectDraw Surfaces for the display.   
LPDIRECTDRAWSURFACE back;
LPDIRECTDRAWCLIPPER clipper;    
LPDIRECTDRAWSURFACE zbuffer;

extern uint32_t ZBufferBitDepth;
extern LPDIRECTDRAW         DirectDraw;       // DirectDraw pointer

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
void AddUpdateCallback(PSDrawBlock db, PSDrawParam dp);

TDisplay::TDisplay()
{}

bool TDisplay::Initialize(int32_t dwidth, int32_t dheight, int32_t /*dbitsperpixel*/)
{
    if (frontbuffer)
        return true;

    width = dwidth;
    height = dheight;
    bitsperpixel = 32; // Always use 32-bit RGBA color
    savezbuffer = nullptr;
    ResetUpdateCallbacks();

    // Initialize Sokol graphics context
    sg_desc desc = {};
    desc.context = sapp_sgcontext();
    desc.buffer_pool_size = 128;
    desc.image_pool_size = 128;
    desc.shader_pool_size = 32;
    desc.pipeline_pool_size = 32;
    desc.pass_pool_size = 16;
    sg_setup(&desc);

    // Create front and back buffer images
    sg_image_desc img_desc = {};
    img_desc.width = width;
    img_desc.height = height;
    img_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    img_desc.usage = SG_USAGE_DYNAMIC;
    img_desc.render_target = true;

    sg_image front_img = sg_make_image(&img_desc);
    sg_image back_img = sg_make_image(&img_desc);

    // Create front and back buffer surfaces
    frontbuffer = new TSurface(front_img, width, height, bitsperpixel);
    backbuffer = new TSurface(back_img, width, height, bitsperpixel);

    // Verify required graphics capabilities
    sg_features features = sg_query_features();
    if (!features.image_float || !features.instancing) {
        FatalError("GPU does not support required features");
    }

    // Create depth buffer
    zbuffer = new TSurface();
    zbuffer->Initialize(dwidth, dheight, SG_PIXELFORMAT_DEPTH_STENCIL);

    // Create pipeline for surface rendering
    pip_desc = {};
    pip_desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;
    pip_desc.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT2;
    pip_desc.shader = sg_make_shader(blit_shader_desc());
    pip_desc.primitive_type = SG_PRIMITIVETYPE_TRIANGLES;
    pip_desc.colors[0].blend.enabled = true;
    pip_desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
    pip_desc.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    pipeline = sg_make_pipeline(&pip_desc);

    // Create default render pass
    sg_pass_desc pass_desc = {};
    pass_desc.color_attachments[0].image = backbuffer->GetSGImage();
    pass_desc.depth_stencil_attachment.image = zbuffer->GetSGImage();
    default_pass = sg_make_pass(&pass_desc);

    // Clear buffers
    frontbuffer->Clear();
    backbuffer->Clear();
    zbuffer->Clear();

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
        backbuffer    = nullptr;
        back    = nullptr;
    }

    // Destroy zbuffer surface
    if (zbuffer)
    {

        delete zbuffer;
        zbuffer = nullptr;
        zbuffer = nullptr;
    }

    // Destroy front surface
    if (frontbuffer)
    {

        delete frontbuffer;
        frontbuffer = nullptr;
        front = nullptr;
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
        front->Restore();
        if (!Windowed)
            frontbuffer->Clear();
    }

    if (back->IsLost())
    {
        back->Restore();
        backbuffer->Clear();
    }


    return true;
}

// Flips the front/back buffer
bool TDisplay::FlipPage(bool Wait)
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

    // Draw backbuffer to screen
    if (backbuffer) {
        // Set up pipeline state
        sg_apply_pipeline(backbuffer->pipeline);
        
        // Set up bindings
        sg_bindings bind = {};
        bind.fs.images[0] = backbuffer->GetSGImage();
        sg_apply_bindings(&bind);

        // Draw fullscreen quad
        sg_draw(0, 6, 1);
    }

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
