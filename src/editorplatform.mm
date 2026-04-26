// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           editorplatform.mm - macOS window-resize shim for editor     *
// *************************************************************************
//
// sokol_app.h doesn't expose a sapp_set_window_size helper, but the
// editor needs to grow / shrink the OS window when F12 toggles in/out
// of editor mode. On macOS we have sapp_macos_get_window() returning
// the underlying NSWindow* which we can poke directly.
//
// Cross-platform variants (Windows / Linux) live alongside this file
// when those builds are exercised; for now this is a no-op stub on
// non-Apple platforms and the editor falls back to laying its panels
// out inside the existing window size.
//
// *************************************************************************

#include "editorplatform.h"

#if defined(__APPLE__)
#  include <AppKit/AppKit.h>
#  include <sokol_app.h>
#endif

namespace EditorPlatform {

void GetWindowSize(int32_t& w, int32_t& h)
{
#if defined(__APPLE__)
    NSWindow* nsw = (__bridge NSWindow*)sapp_macos_get_window();
    if (nsw) {
        NSRect cr = [nsw contentRectForFrameRect:[nsw frame]];
        w = int32_t(cr.size.width);
        h = int32_t(cr.size.height);
        return;
    }
#endif
    w = sapp_width();
    h = sapp_height();
}

void GetWindowOrigin(int32_t& x, int32_t& y)
{
#if defined(__APPLE__)
    NSWindow* nsw = (__bridge NSWindow*)sapp_macos_get_window();
    if (nsw) {
        NSRect f = [nsw frame];
        x = int32_t(f.origin.x);
        y = int32_t(f.origin.y);
        return;
    }
#endif
    x = 0;
    y = 0;
}

void SetWindowFrame(int32_t origin_x, int32_t origin_y, int32_t w, int32_t h)
{
#if defined(__APPLE__)
    NSWindow* nsw = (__bridge NSWindow*)sapp_macos_get_window();
    if (!nsw) return;
    NSRect contentRect = NSMakeRect(origin_x, origin_y, w, h);
    NSRect frameRect   = [nsw frameRectForContentRect:contentRect];
    [nsw setFrame:frameRect display:YES animate:NO];
#else
    (void)origin_x; (void)origin_y; (void)w; (void)h;
#endif
}

void SetWindowSizeCenteredOnCurrent(int32_t w, int32_t h)
{
#if defined(__APPLE__)
    NSWindow* nsw = (__bridge NSWindow*)sapp_macos_get_window();
    if (!nsw) return;
    NSRect cur_frame   = [nsw frame];
    NSRect cur_content = [nsw contentRectForFrameRect:cur_frame];
    const float old_cx = float(cur_content.origin.x) + float(cur_content.size.width)  * 0.5f;
    const float old_cy = float(cur_content.origin.y) + float(cur_content.size.height) * 0.5f;
    // setContentSize is the canonical way to resize the content area; it
    // propagates to subviews (MTKView) so sokol_app's drawableSize
    // callback fires and sapp_width/sapp_height pick up the new
    // dimensions next frame. Then re-anchor the frame so the centre
    // stays put.
    [nsw setContentSize:NSMakeSize(float(w), float(h))];
    NSRect new_frame   = [nsw frame];
    new_frame.origin.x = old_cx - new_frame.size.width  * 0.5f;
    new_frame.origin.y = old_cy - new_frame.size.height * 0.5f;
    [nsw setFrameOrigin:new_frame.origin];
#else
    (void)w; (void)h;
#endif
}

} // namespace EditorPlatform
