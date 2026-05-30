// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *      cursor.mm - NSCursor backing for the game pointer (macOS)        *
// *************************************************************************
//
// Hand the game cursor pixels to AppKit so macOS handles every edge
// case: mouse exits the window -> default arrow shows over the desktop
// / next app, ImGui captures the mouse -> we set the arrow and AppKit
// draws it, etc.
//
// The non-obvious bit: sokol_app installs its own NSTrackingArea with
// NSTrackingActiveInKeyWindow. That tracking area goes inactive the
// moment another window becomes key, which is exactly when the mouse
// leaves us -- so calling [NSCursor set] from sokol's MOUSE_LEAVE
// handler is too late, and the last [set] from inside the window
// persists visually over the desktop. The reliable fix is to install
// OUR OWN tracking area with NSTrackingActiveAlways + a cursorUpdate:
// owner: AppKit then calls us back any time the mouse enters our view
// area regardless of key-window state, and naturally pops to the
// destination's cursor when the mouse leaves.

#include "../cursor.h"

#include "../../bitmap.h"
#include "../../bitmapdecode.h"
#include "../../logging.h"

#import <AppKit/AppKit.h>

#include <sokol_app.h>

// ---------- Cached state (file-scope so both the ObjC class and the
//            facade impl can see it; not in the rev_platform namespace
//            because ObjC method bodies can't read C++ namespaces) ------

static NSCursor          *g_currentCursor = nil;
static NSTrackingArea    *g_trackingArea  = nil;
static NSView            *g_trackedView   = nil;

// ---------- Tracking-area owner: NSResponder that handles cursorUpdate: ----

@interface RevCursorTracker : NSResponder
@end

@implementation RevCursorTracker
- (void)cursorUpdate:(NSEvent * __unused)event
{
    // Mouse just entered our tracked rect (or AppKit is asking what
    // cursor to show here). Pin the current game cursor; if we have no
    // game cursor (game-mode not active, or ImGui captured), fall
    // through to the system arrow so AppKit can do its normal thing.
    if (g_currentCursor)
        [g_currentCursor set];
    else
        [[NSCursor arrowCursor] set];
}
@end

static RevCursorTracker *g_tracker = nil;

// ---------- Platform-facade impl ------------------------------------------

namespace rev_platform
{

static void EnsureTrackingArea()
{
    if (g_trackingArea) return;
    const void *raw_win = sapp_macos_get_window();
    if (!raw_win)
    {
        log_warn("[cursor-os] sapp_macos_get_window returned null; tracking area deferred");
        return;
    }
    NSWindow *window = (__bridge NSWindow*) raw_win;
    NSView *view = [window contentView];
    if (!view)
    {
        log_warn("[cursor-os] window has no contentView; tracking area deferred");
        return;
    }

    g_tracker = [[RevCursorTracker alloc] init];
    g_trackingArea = [[NSTrackingArea alloc]
        initWithRect:[view bounds]
        options:(NSTrackingCursorUpdate |
                 NSTrackingActiveAlways |     // <-- key: stay active even when not key window
                 NSTrackingInVisibleRect)     // <-- ignore initWithRect; auto-resize with view
        owner:g_tracker
        userInfo:nil];
    [view addTrackingArea:g_trackingArea];
    g_trackedView = view;
    log_info("[cursor-os] installed tracking area on sokol contentView (%dx%d)",
             (int)view.bounds.size.width, (int)view.bounds.size.height);
}

bool SetOSCursor(const TBitmap *bm, int32_t hot_x, int32_t hot_y)
{
    // Always sync g_currentCursor with the outcome so the cursorUpdate:
    // callback never shows a stale cursor: if any step below fails,
    // g_currentCursor goes nil and cursorUpdate: falls through to the
    // system arrow. That keeps cursor.mm and cursor.cpp's
    // g_os_cursor_owns_pixels (which mirrors our bool return) in lockstep
    // -- without this, a failed decode left the OS showing the previous
    // cursor while cursor.cpp re-enabled the in-game draw, producing the
    // "OS cursor + app cursor underneath" double-vision bug.
    [g_currentCursor release];
    g_currentCursor = nil;

    if (!bm) return false;
    if (bm->width <= 0 || bm->height <= 0) return false;

    const int32_t w = bm->width;
    const int32_t h = bm->height;

    const size_t pitch = (size_t)w * 4;
    std::vector<uint8_t> rgba(pitch * (size_t)h, 0);
    if (!DecodeBitmapToRGBA(const_cast<TBitmap*>(bm), rgba.data(),
                            (int32_t)pitch, 0, 0))
    {
        log_warn("[cursor-os] DecodeBitmapToRGBA failed (%dx%d, flags=0x%x)",
                 w, h, bm->flags);
        [[NSCursor arrowCursor] set];
        return false;
    }

    @autoreleasepool {
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
            [[NSCursor arrowCursor] set];
            return false;
        }

        NSImage *img = [[NSImage alloc] initWithSize:NSMakeSize(w, h)];
        [img addRepresentation:rep];
        // NSImage retained rep; we can drop our +1 from alloc.
        [rep release];

        const NSPoint hot = NSMakePoint(
            (CGFloat)std::clamp((int32_t)hot_x, (int32_t)0, w - 1),
            (CGFloat)std::clamp((int32_t)hot_y, (int32_t)0, h - 1));

        // initWithImage:hotSpot: retains img -- drop our +1 from alloc
        // after the NSCursor is built. g_currentCursor holds the cursor's
        // own +1 from alloc; released next time we replace it (above) or
        // in ResetOSCursor.
        g_currentCursor = [[NSCursor alloc] initWithImage:img hotSpot:hot];
        [img release];

        // Install the tracking area lazily on the first SetOSCursor call
        // (window may not exist yet at static-init time). After that,
        // AppKit calls our cursorUpdate: whenever the mouse needs a
        // cursor decision in our view.
        EnsureTrackingArea();

        // Apply immediately for the case where the mouse is already
        // inside our view at this call: cursorUpdate: won't fire again
        // until the mouse moves. AppKit ignores the redundant call when
        // the cursor is already current.
        [g_currentCursor set];
    }
    return true;
}

void ResetOSCursor()
{
    @autoreleasepool {
        // Drop the cached game cursor so cursorUpdate: falls through to
        // the arrow on the next mouse enter (e.g. while ImGui captures
        // the mouse, we want the arrow inside our window). The next
        // SetOSCursor call repopulates the cache.
        [g_currentCursor release];
        g_currentCursor = nil;
        [[NSCursor arrowCursor] set];
    }
}

void ReassertOSCursor()
{
    if (!g_currentCursor) return;
    [g_currentCursor set];
}

} // namespace rev_platform
