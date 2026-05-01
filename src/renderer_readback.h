// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           renderer_readback.h - GPU-to-CPU pixel readback             *
// *************************************************************************
//
// Pulls a single RGBA8 pixel out of a sokol-managed render target back to
// CPU memory. Used by the editor for the OBJID pixel-pick path. The
// implementation is platform-specific (renderer_readback.mm on Apple);
// callers must be prepared for a not-implemented return when the build
// targets a backend without a readback helper.
//
// *************************************************************************

#pragma once

#include <sokol_gfx.h>
#include <cstdint>

namespace RendererReadback {

// Read a single RGBA8 pixel at (x, y) in `img`'s native storage. Returns
// false if the readback isn't supported (non-Apple build), if the image
// handle is invalid, or if the request is out of bounds.
//
// Synchronous: blocks until the GPU has finished the most recent frame
// touching `img`. Cheap when called rarely (selection clicks).
bool ReadPixel(sg_image img, int32_t x, int32_t y, uint8_t out_rgba[4]);

} // namespace RendererReadback
