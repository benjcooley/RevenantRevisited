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

#include "editorfonts.h"
#include "editoricons.h"
#include "framesnap.h"
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
        d.no_default_font = true;          // we build our own atlas next
        // sokol_imgui swallows Cmd+V on macOS by default (routes through
        // its own SAPP_EVENTTYPE_CLIPBOARD_PASTED instead). Disable that
        // override so our Edit.Paste hotkey gets the key event normally;
        // we read the OS clipboard in the command's execute callback.
        d.disable_paste_override = true;
        simgui_setup(&d);
        imgui_initialized = true;
        ImGuiIO& io = ImGui::GetIO();
        // KEEP trickle enabled (ImGui's default). On macOS, Cmd-modified
        // key shortcuts fire KEY_DOWN + KEY_UP in the same NSEvent (per
        // sokol_app's "Cmd-held auto-keyup" stuck-key workaround). Without
        // trickling, both apply in the same ImGui frame and IsKeyPressed
        // never sees a transition -- Cmd+Z silently does nothing. See
        // sokol issue #233 + memory/project_imgui_mac_modifiers.md.
        io.ConfigInputTrickleEventQueue = true;
        ImGuiStyle& st = ImGui::GetStyle();
        st.HoverDelayShort  = 0.0f;
        st.HoverDelayNormal = 0.0f;

        EditorFonts::Build();
        EditorIcons::Build();
    }

    return true;
}

bool TDisplay::Close()
{
    if (imgui_initialized) {
        EditorIcons::Shutdown();
        EditorFonts::Shutdown();
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

// Trivial dtor: explicit Close() runs from ShutdownGlobals before the
// global destructs. Calling sokol_gfx / TRenderer teardown from a
// process-exit path is unsafe because sokol's own static state and
// the surfaces it references live in other TUs.
TDisplay::~TDisplay() = default;

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
// Open the Overlay2D sokol pass on the 2D backbuffer with a transparent
// clear. The backbuffer image is RGBA8 with an alpha channel; FlipPage's
// composite step expects alpha = 0 to mean "no overlay here, show the
// 3D scene through." Idempotent (a second Begin without an End between
// is a no-op so internal helpers can bracket sections without fear).
// See docs/FRAME_PIPELINE.md.
void TDisplay::BeginOverlay()
{
    if (overlay_pass_open || !backbuffer)
        return;

    backbuffer->StartPass(0.0f, 0.0f, 0.0f, 0.0f);
    overlay_pass_open = true;
}

void TDisplay::EndOverlay()
{
    if (!overlay_pass_open || !backbuffer)
        return;

    backbuffer->EndPass();
    overlay_pass_open = false;
}

bool TDisplay::FlipPage(bool /*Wait*/)
{
    if (!backbuffer)
        return false;

    // Defensive: if a screen forgot to close the overlay pass before
    // FlipPage, close it now. Better to recover than crash.
    if (overlay_pass_open)
        EndOverlay();

    sg_pass_action pa = {};
    pa.colors[0].action = SG_ACTION_CLEAR;
    pa.colors[0].value  = { 0.0f, 0.0f, 0.0f, 1.0f };
    pa.depth.action     = SG_ACTION_DONTCARE;
    pa.stencil.action   = SG_ACTION_DONTCARE;

    sg_begin_default_pass(&pa, sapp_width(), sapp_height());

    // Layered composite: Scene3D, then Overlay2D, then DebugUI. The
    // composite pipeline alpha-blends, so a fully-transparent overlay
    // leaves the 3D scene visible. See docs/FRAME_PIPELINE.md.
    if (Renderer) {
        Renderer->PresentToSwapchain();   // Scene3D (no-op if nothing drew)
        Renderer->Composite(backbuffer);  // Overlay2D (legacy CPU-raster fallback; mostly empty)
        Renderer->DrawHud();              // HUD items (cursor, stat panels, ...)
    }
    simgui_render();                      // DebugUI

    sg_end_pass();
    sg_commit();

    // ---- Snap-mode mirror pass -------------------------------------------
    // Re-run the composite into an offscreen RT we own so framesnap can
    // read it back via Metal blit. Adds one extra composite per frame but
    // only when --snap / --filmstrip is active. The RT, depth, and pass
    // are lazily allocated and re-created on display-size changes.
    //
    // Both color + depth must match the SWAPCHAIN formats, because the
    // renderer's pipelines were created against them — sokol-gfx will
    // assert if pipeline.color_format != pass.color_format (or depth).
    if (FrameSnap::Active())
    {
        const int32_t w = sapp_width();
        const int32_t h = sapp_height();
        const sg_pixel_format scColor = (sg_pixel_format) sapp_color_format();
        const sg_pixel_format scDepth = (sg_pixel_format) sapp_depth_format();

        if (snap_capture_color.id == SG_INVALID_ID
            || snap_capture_w != w
            || snap_capture_h != h)
        {
            if (snap_capture_color.id != SG_INVALID_ID)
                sg_destroy_image(snap_capture_color);

            sg_image_desc cd = {};
            cd.render_target = true;
            cd.width  = w;
            cd.height = h;
            cd.pixel_format = scColor;
            cd.min_filter = SG_FILTER_NEAREST;
            cd.mag_filter = SG_FILTER_NEAREST;
            cd.wrap_u = SG_WRAP_CLAMP_TO_EDGE;
            cd.wrap_v = SG_WRAP_CLAMP_TO_EDGE;
            cd.label = "snap-capture-color";
            snap_capture_color = sg_make_image(&cd);

            snap_capture_w = w;
            snap_capture_h = h;
        }

        // Build a transient depth attachment + pass each frame. Cheap
        // relative to the composite work itself; keeps display.cpp from
        // owning more long-lived sokol resources.
        sg_image_desc dd = {};
        dd.render_target = true;
        dd.width  = w;
        dd.height = h;
        dd.pixel_format = scDepth;
        dd.min_filter = SG_FILTER_NEAREST;
        dd.mag_filter = SG_FILTER_NEAREST;
        dd.wrap_u = SG_WRAP_CLAMP_TO_EDGE;
        dd.wrap_v = SG_WRAP_CLAMP_TO_EDGE;
        dd.label = "snap-capture-depth";
        sg_image snapDepth = sg_make_image(&dd);

        sg_pass_desc pd = {};
        pd.color_attachments[0].image = snap_capture_color;
        pd.depth_stencil_attachment.image = snapDepth;
        sg_pass capturePass = sg_make_pass(&pd);

        sg_pass_action snapPa = {};
        snapPa.colors[0].action = SG_ACTION_CLEAR;
        snapPa.colors[0].value  = { 0.0f, 0.0f, 0.0f, 1.0f };
        snapPa.depth.action     = SG_ACTION_CLEAR;
        snapPa.depth.value      = 1.0f;
        snapPa.stencil.action   = SG_ACTION_DONTCARE;

        sg_begin_pass(capturePass, &snapPa);
        if (Renderer) {
            // Mirror pass: use PresentForSnap (dirty-flag-free variant)
            // because PresentToSwapchain already cleared the flags this
            // frame. Without this the snap RT misses Scene3D entirely
            // and the filmstrip captures only the HUD over black —
            // which broke vfx test snaps (the effects ARE the scene).
            Renderer->PresentForSnap();
            Renderer->Composite(backbuffer);
            Renderer->DrawHud();
        }
        // simgui_render() omitted from snap path — ImGui in a non-default
        // pass needs careful pipeline setup; HUD-test modes don't use it.
        sg_end_pass();
        sg_commit();

        sg_destroy_pass(capturePass);
        sg_destroy_image(snapDepth);
    }

    return true;
}
