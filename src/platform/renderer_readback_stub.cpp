// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   renderer_readback_stub.cpp - Non-macOS pixel-readback fallback      *
// *************************************************************************
//
// GPU->CPU single-pixel readback is implemented for Metal in
// platform/macosx/renderer_readback.mm. The D3D11 / GL backends don't have
// a readback helper yet, so this returns "not supported" -- callers (the
// editor's OBJID pixel pick) already handle a false result. See
// renderer_readback.h.
//
// Guarded so the macOS build (which compiles the .mm) doesn't get a second
// definition -- the *.cpp glob picks this file up on every platform.
// *************************************************************************

#if !defined(__APPLE__)

#include "../renderer_readback.h"

namespace RendererReadback {

bool ReadPixel(sg_image /*img*/, int32_t /*x*/, int32_t /*y*/, uint8_t out_rgba[4])
{
    out_rgba[0] = out_rgba[1] = out_rgba[2] = out_rgba[3] = 0;
    return false;
}

} // namespace RendererReadback

#endif // !__APPLE__
