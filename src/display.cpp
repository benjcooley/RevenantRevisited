// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 display.cpp  - Display Object File                    *
// *************************************************************************
//
// TDisplay is the swapchain / window surface. See renderer.h for the core
// render engine that used to live in this file.
//
// *************************************************************************

#include "display.h"

#include <sokol_app.h>
#include <sokol_gfx.h>
#include <sokol_glue.h>

#include "imgui.h"
#include "sokol_imgui.h"

#include "multisurface.h"
#include "renderer.h"
#include "revenant.h"

extern bool  Hardware3D;
extern bool  UsingHardware;
extern void  MakeColorTables();

// Low-level dirty-rect callback plumbing (legacy; retained for the
// TSurface API surface -- sokol doesn't use it).
void ResetUpdateCallbacks();
void ProcessUpdateCallbacks();
void AddUpdateCallback(SDrawBlock* db, SDrawParam* dp);

TDisplay::TDisplay()
{}

bool TDisplay::Initialize(int32_t dwidth, int32_t dheight, int32_t /*dbitsperpixel*/)
{
    if (backbuffer)
        return true;

    width  = dwidth;
    height = dheight;
    format = SG_PIXELFORMAT_RGBA8;

    // ---- sokol_gfx context ------------------------------------------------
    sg_desc desc = {};
    desc.context          = sapp_sgcontext();
    // Full-level scenes can now cache hundreds of rigid mesh assets in
    // addition to tile/backbuffer/ImGui buffers. Each mesh asset owns a vertex
    // and index buffer, so leave generous headroom.
    desc.buffer_pool_size = 8192;
    // One tile bitmap needs 2 images (color + depth); a Misthaven sector
    // has ~200 unique bitmaps, and the test harness loads a 3x3 neighborhood.
    // Plenty of headroom for UI atlases and ImGui.
    desc.image_pool_size  = 4096;
    desc.shader_pool_size = 64;
    desc.pipeline_pool_size = 128;
    desc.pass_pool_size   = 64;
    // Full-level sector tests can issue tens of thousands of draw calls
    // with small uniform uploads; Metal stages these through sokol's
    // per-frame shared buffer.
    desc.uniform_buffer_size = 16 * 1024 * 1024;
    sg_setup(&desc);

    // ---- Backbuffer TSurface ---------------------------------------------
    // The backbuffer is the target for legacy 2D blits (Put/WriteText/Box/
    // ZPut/...). Screens open backbuffer->StartPass, draw into it, then end
    // the pass; FlipPage composites it onto the swapchain when TRenderer
    // produced no 3D output this frame.
    sg_image_desc bb_desc = {};
    bb_desc.width         = width;
    bb_desc.height        = height;
    bb_desc.sample_count  = 1;
    bb_desc.render_target = true;
    bb_desc.pixel_format  = SG_PIXELFORMAT_RGBA8;
    bb_desc.min_filter    = SG_FILTER_NEAREST;
    bb_desc.mag_filter    = SG_FILTER_NEAREST;
    bb_desc.wrap_u        = SG_WRAP_CLAMP_TO_EDGE;
    bb_desc.wrap_v        = SG_WRAP_CLAMP_TO_EDGE;
    bb_desc.label         = "display.backbuffer";
    backbuffer = new TSurface(sg_make_image(&bb_desc), width, height, SG_PIXELFORMAT_RGBA8);

    // Frontbuffer is an alias of the backbuffer under sokol -- the retail
    // Windows path double-buffered at the DirectX level, we just composite
    // the backbuffer each frame. FrontBuffer() is preserved for API-compat.
    frontbuffer = backbuffer;

    // zbuffer is a legacy hook; no one drives it under sokol (TBitmap owns
    // its own per-bitmap zbuffer for tile data).
    zbuffer = nullptr;

    // ---- Bring up the core render engine ---------------------------------
    Renderer = new TRenderer();
    Renderer->Initialize(width, height);

    // ---- ImGui overlay ---------------------------------------------------
    // In-game debug / tuning panels (lighting, depth scale, camera). Draws
    // into the swapchain pass in FlipPage.
    //
    // max_vertices default is 65,536 which silently truncates the foreground
    // drawlist when a debug overlay pushes a lot of geometry (e.g. the
    // sector test's per-tile bbox wireframes, ~12 AA lines x thousands of
    // tiles). sokol_imgui.h bails with `break` once a drawlist would
    // overflow the shared vtx/idx buffer, dropping whatever hadn't been
    // copied yet -- including the foreground drawlist since it's appended
    // last. Bump to 1M vertices (~20 MB) so large overlays render intact.
    {
        simgui_desc_t d = {};
        d.ini_filename = "imgui.ini";
        d.max_vertices = 1 << 20;
        simgui_setup(&d);
        imgui_initialized = true;
        constexpr float kDebugUiScale = 0.85f;
        ImGui::GetStyle().ScaleAllSizes(kDebugUiScale);
        ImGui::GetIO().FontGlobalScale = kDebugUiScale;
    }

    return true;
}

bool TDisplay::Close()
{
    if (imgui_initialized) {
        simgui_shutdown();
        imgui_initialized = false;
    }

    if (Renderer) {
        Renderer->Shutdown();
        delete Renderer;
        Renderer = nullptr;
    }

    // frontbuffer aliases backbuffer -- null it out first to avoid a
    // double free.
    frontbuffer = nullptr;

    if (backbuffer) { delete backbuffer; backbuffer = nullptr; }
    if (zbuffer)    { delete zbuffer;    zbuffer    = nullptr; }

    width = height = 0;
    return true;
}

TDisplay::~TDisplay()
{
    Close();
}

bool TDisplay::Restore()
{
    if (!backbuffer)
        return false;

    if (backbuffer->Lost())
    {
        backbuffer->Restore();
        backbuffer->Clear();
    }

    return true;
}

// Present the frame. Ask the core renderer to composite its final image
// (lit_target, post-deferred-lighting) onto the swapchain; fall back to
// the legacy 2D backbuffer if TRenderer produced no 3D output. ImGui is
// always rendered last inside the same pass.
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

    const bool presented_3d = Renderer ? Renderer->PresentToSwapchain() : false;
    if (!presented_3d) {
        // UI/widget frame -- composite the 2D backbuffer onto the swapchain.
        if (Renderer) Renderer->Composite(backbuffer);
    }

    simgui_render();
    sg_end_pass();
    sg_commit();

    return true;
}
