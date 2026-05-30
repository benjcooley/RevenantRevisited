// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *      renderer_readback.mm - Metal RGBA8 single-pixel readback          *
// *************************************************************************

#include "../../renderer_readback.h"

#define SOKOL_METAL
#include <sokol_gfx.h>

#import <Metal/Metal.h>

namespace RendererReadback {

bool ReadPixel(sg_image img, int32_t x, int32_t y, uint8_t out_rgba[4])
{
    out_rgba[0] = out_rgba[1] = out_rgba[2] = out_rgba[3] = 0;
    if (img.id == SG_INVALID_ID || x < 0 || y < 0) return false;

    sg_image_info info = sg_query_image_info(img);
    if (x >= info.width || y >= info.height) return false;

    // sg_mtl_query_image_handle returns the MTLTexture for the active
    // slot. NULL on non-Metal backends or for invalid handles.
    id<MTLTexture> tex = (__bridge id<MTLTexture>) sg_mtl_query_image_handle(img);
    if (!tex) return false;

    id<MTLDevice> device = tex.device;
    if (!device) return false;

    // 1x1 RGBA8 staging buffer (4 bytes), shared so CPU can read after
    // blit completes. Created fresh each call -- 4 bytes is cheap.
    id<MTLBuffer> stage = [device newBufferWithLength:4
                                              options:MTLResourceStorageModeShared];
    if (!stage) return false;

    id<MTLCommandQueue> queue = [device newCommandQueue];
    id<MTLCommandBuffer> cmd  = [queue commandBuffer];
    id<MTLBlitCommandEncoder> blit = [cmd blitCommandEncoder];

    [blit copyFromTexture:tex
              sourceSlice:0
              sourceLevel:0
             sourceOrigin:MTLOriginMake(NSUInteger(x), NSUInteger(y), 0)
               sourceSize:MTLSizeMake(1, 1, 1)
                 toBuffer:stage
        destinationOffset:0
   destinationBytesPerRow:4
 destinationBytesPerImage:4];

    [blit endEncoding];
    [cmd commit];
    [cmd waitUntilCompleted];

    const uint8_t* px = (const uint8_t*) stage.contents;
    // Swap R<->B for BGRA8 sources (the Metal swapchain) so callers get RGBA;
    // RGBA8 sources are copied as-is. See ReadRect for the rationale.
    const MTLPixelFormat pf = tex.pixelFormat;
    const bool bgra = (pf == MTLPixelFormatBGRA8Unorm ||
                       pf == MTLPixelFormatBGRA8Unorm_sRGB);
    out_rgba[0] = bgra ? px[2] : px[0];
    out_rgba[1] = px[1];
    out_rgba[2] = bgra ? px[0] : px[2];
    out_rgba[3] = px[3];
    return true;
}

bool ReadRect(sg_image img, int32_t x, int32_t y, int32_t w, int32_t h,
              uint8_t* out_rgba)
{
    if (img.id == SG_INVALID_ID || !out_rgba) return false;
    if (x < 0 || y < 0 || w <= 0 || h <= 0)   return false;

    sg_image_info info = sg_query_image_info(img);
    if (x + w > info.width || y + h > info.height) return false;

    id<MTLTexture> tex = (__bridge id<MTLTexture>) sg_mtl_query_image_handle(img);
    if (!tex) return false;

    id<MTLDevice> device = tex.device;
    if (!device) return false;

    const NSUInteger bytesPerRow   = NSUInteger(w) * 4;
    const NSUInteger totalBytes    = bytesPerRow * NSUInteger(h);

    id<MTLBuffer> stage = [device newBufferWithLength:totalBytes
                                              options:MTLResourceStorageModeShared];
    if (!stage) return false;

    id<MTLCommandQueue> queue = [device newCommandQueue];
    id<MTLCommandBuffer> cmd  = [queue commandBuffer];
    id<MTLBlitCommandEncoder> blit = [cmd blitCommandEncoder];

    [blit copyFromTexture:tex
              sourceSlice:0
              sourceLevel:0
             sourceOrigin:MTLOriginMake(NSUInteger(x), NSUInteger(y), 0)
               sourceSize:MTLSizeMake(NSUInteger(w), NSUInteger(h), 1)
                 toBuffer:stage
        destinationOffset:0
   destinationBytesPerRow:bytesPerRow
 destinationBytesPerImage:totalBytes];

    [blit endEncoding];
    [cmd commit];
    [cmd waitUntilCompleted];

    memcpy(out_rgba, stage.contents, totalBytes);

    // The Metal blit is a raw byte copy — no channel swizzle. The swapchain
    // backbuffer is BGRA8 (sapp_color_format on Metal), so its bytes land as
    // B,G,R,A; callers want RGBA. Swap R<->B for BGRA8 sources so saved PNGs
    // have correct colors. RGBA8 sources (offscreen TSurface targets) are
    // left untouched.
    const MTLPixelFormat pf = tex.pixelFormat;
    if (pf == MTLPixelFormatBGRA8Unorm || pf == MTLPixelFormatBGRA8Unorm_sRGB)
    {
        for (NSUInteger i = 0; i + 3 < totalBytes; i += 4)
        {
            const uint8_t b = out_rgba[i];
            out_rgba[i]     = out_rgba[i + 2];
            out_rgba[i + 2] = b;
        }
    }
    return true;
}

} // namespace RendererReadback
