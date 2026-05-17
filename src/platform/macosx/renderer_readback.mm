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
    out_rgba[0] = px[0];
    out_rgba[1] = px[1];
    out_rgba[2] = px[2];
    out_rgba[3] = px[3];
    return true;
}

} // namespace RendererReadback
