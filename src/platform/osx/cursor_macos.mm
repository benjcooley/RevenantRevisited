// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *      cursor_macos.mm - NSCursor backing for the game pointer          *
// *************************************************************************
//
// We hand our cursor pixels to AppKit's NSCursor so macOS handles every
// edge case automatically: mouse exits window -> default arrow shows
// over the desktop / next app, window goes to background -> cursor
// behaves like every other app's, ImGui captures the mouse -> we just
// reset to the arrow and the OS draws it. No more sapp_show_mouse
// dance, no more "stuck game cursor" bugs.

#include "../../cursor_os.h"

#include "../../bitmap.h"
#include "../../bitmapdecode.h"
#include "../../logging.h"

#import <AppKit/AppKit.h>

namespace rev_platform
{

bool SetOSCursor(const TBitmap *bm, int32_t hot_x, int32_t hot_y)
{
    if (!bm) return false;
    if (bm->width <= 0 || bm->height <= 0) return false;

    const int32_t w = bm->width;
    const int32_t h = bm->height;

    // Decode game bitmap (8-bit palette + keycolor transparency in the
    // common case, but DecodeBitmapToRGBA handles every format the
    // engine ships). Output pitch = w*4 RGBA bytes.
    const size_t pitch = (size_t)w * 4;
    std::vector<uint8_t> rgba(pitch * (size_t)h, 0);
    if (!DecodeBitmapToRGBA(const_cast<TBitmap*>(bm), rgba.data(),
                            (int32_t)pitch, 0, 0))
    {
        log_warn("[cursor-os] DecodeBitmapToRGBA failed (%dx%d, flags=0x%x)",
                 w, h, bm->flags);
        return false;
    }

    @autoreleasepool {
        // NSBitmapImageRep wants planes; we pass a single RGBA8 plane.
        // NS_PIXELS_WIDE / HIGH come from our bitmap; samplesPerPixel=4
        // (RGBA), hasAlpha=YES, isPlanar=NO, colorSpaceName=device RGB,
        // bitmapFormat=alpha-non-premultiplied so the OS doesn't try to
        // premultiply twice.
        unsigned char *planes[1] = { rgba.data() };
        NSBitmapImageRep *rep =
            [[NSBitmapImageRep alloc]
                initWithBitmapDataPlanes:planes
                              pixelsWide:w
                              pixelsHigh:h
                           bitsPerSample:8
                         samplesPerPixel:4
                                hasAlpha:YES
                                isPlanar:NO
                          colorSpaceName:NSDeviceRGBColorSpace
                            bitmapFormat:NSBitmapFormatAlphaNonpremultiplied
                             bytesPerRow:(NSInteger)pitch
                            bitsPerPixel:32];
        if (!rep)
        {
            log_warn("[cursor-os] NSBitmapImageRep alloc failed");
            return false;
        }

        NSImage *img = [[NSImage alloc] initWithSize:NSMakeSize(w, h)];
        [img addRepresentation:rep];

        // Clamp hotspot to bitmap bounds -- callers occasionally pass
        // regx/regy that are out-of-image (negative or > size). AppKit
        // doesn't validate; an out-of-range hotspot can wedge clicking.
        const NSPoint hot = NSMakePoint(
            (CGFloat)std::clamp((int32_t)hot_x, (int32_t)0, w - 1),
            (CGFloat)std::clamp((int32_t)hot_y, (int32_t)0, h - 1));

        NSCursor *cursor = [[NSCursor alloc] initWithImage:img hotSpot:hot];
        [cursor set];
    }
    return true;
}

void ResetOSCursor()
{
    @autoreleasepool {
        [[NSCursor arrowCursor] set];
    }
}

} // namespace rev_platform
