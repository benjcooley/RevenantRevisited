// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *               display.h  - EXILE Display Include File                 *
// *************************************************************************
//
// TDisplay is the swapchain / window surface. It wraps sokol's context
// setup, owns the front/back TSurface buffers that legacy blits draw
// into (Put / WriteText / Box / ZPut / ...), and handles FlipPage.
//
// The core render engine -- pipelines, G-buffer, lighting, tile pass,
// composite submission -- now lives in TRenderer (renderer.h). TDisplay
// creates a TRenderer instance during Initialize and delegates the final
// swapchain composite to Renderer->PresentToSwapchain. All new game-side
// code that wants to submit draws should use Renderer-> directly; only
// legacy TSurface-style blits still route through Display->.
//
// *************************************************************************

#pragma once

#include "revenant.h"

#include "graphics.h"
#include "surface.h"

#define UPDATE_THISFRAME        0x01        // Adds a dirty rectangle update rect for this frame
#define UPDATE_NEXTFRAME        0x02        // Adds a dirty rectangle update rect for the next frame
#define UPDATE_SCREENTOBUFFER   0x04        // Immediately copies the screen rect to the background
#define UPDATE_BUFFERTOSCREEN   0x08        // Immediately copies the background rect to the screen
#define UPDATE_NOMERGERECT      0x10        // Prevents dirty rectangle system from merging this rect

#define UPDATE_RESTORE          (UPDATE_THISFRAME)
#define UPDATE_BACKGROUND       (UPDATE_SCREENTOBUFFER | UPDATE_NEXTFRAME)

_STRUCTDEF(RestoreRect)
_CLASSDEF(TDisplay)

class TDisplay : public TSurface
{
  public:
    TDisplay();
    virtual ~TDisplay();

    // Legacy dirty-rect API -- no-op under sokol_gfx; screens just redraw.
    void AddUpdateRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t flags = 0) {
        (void)x; (void)y; (void)w; (void)h; (void)flags;
    }

    // Clear-zbuffer swap dance from the retail DirectX path. Not needed on
    // sokol (no video-memory zbuffer restriction) -- stubs kept for callers.
    void InitClearZBuffer()  {}
    void CloseClearZBuffer() {}
    bool UsingClearZBuffer() { return savezbuffer != nullptr; }
    TSurface* GetRealZBuffer() { if (savezbuffer) return savezbuffer; else return zbuffer; }

    [[nodiscard]] TSurface* BackBuffer()  const { return backbuffer; }
    [[nodiscard]] TSurface* FrontBuffer() const { return frontbuffer; }

    // Brings up the sokol_gfx context, builds the TRenderer core engine,
    // wires up ImGui, and allocates the backbuffer/front/z TSurfaces.
    bool Initialize(int32_t dwidth, int32_t dheight, int32_t dbitsperpixel);
    // Tears down everything Initialize brought up (including the TRenderer).
    virtual bool Close();
    // Restore after alt-tab -- legacy stub on sokol.
    bool Restore();

    // Present the frame. If TRenderer produced any G-buffer / lit output
    // this frame, PresentToSwapchain composites it onto the swapchain;
    // otherwise the backbuffer is blitted. ImGui is always rendered last.
    bool FlipPage(bool Wait = true);

  private:
    int32_t currentpage = 0;
    bool    updateenabled = true;
    TSurface* frontbuffer = nullptr;
    TSurface* backbuffer  = nullptr;
    TSurface* zbuffer     = nullptr;
    TSurface* savezbuffer = nullptr;
    bool      imgui_initialized = false;
};
