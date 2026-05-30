// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  headless_window.mm - Best-effort window hide for --headless mode    *
// *************************************************************************
//
// sokol_app's macOS impl creates an NSWindow + CAMetalLayer + Metal
// drawable, then calls `makeKeyAndOrderFront:` from
// `applicationDidFinishLaunching:`. Hiding this window cleanly is harder
// than it should be — Cocoa's standard hide vocabulary (orderOut, alpha=0,
// setFrameOrigin off-screen, setStyleMask:Borderless) does NOT remove the
// window from the system compositor; the CAMetalLayer is still visible at
// its original position regardless of what we do to the NSWindow.
//
// Verified 2026-05-30: orderOut + alpha=0 + offscreen-position + delegate
// swizzle (to prevent terminate-on-last-window-closed) all run cleanly,
// but a `screencapture` of the desktop still shows the Revenant title
// bar / dark content at sokol's chosen position. The Quartz CGWindowList
// also continues to report the window at its original X,Y.
//
// What DOES work today:
//   - The snap pipeline reads our own offscreen capture RT (see TDisplay
//     SnapCaptureImage), so PNG/filmstrip output is correct regardless
//     of window visibility.
//   - The --headless flag + delegate swizzle keep the app alive past the
//     attempted hide so the snap completes cleanly.
//
// True no-window-popup needs one of these (deferred):
//   - Patch sokol_app to support a `hidden_at_startup` desc flag that
//     skips `makeKeyAndOrderFront:`.
//   - Replace sokol_app's NSApp/NSWindow creation with our own (SOKOL_NO_ENTRY)
//     that creates a borderless 1x1 NSWindow and renders into it.
//
// *************************************************************************

#include "../../headless_window.h"

#import <Cocoa/Cocoa.h>
#import <objc/runtime.h>

namespace HeadlessWindow {

static int  s_hideCalls = 0;
static bool s_swizzled   = false;

// Replacement method bound to sokol_app's NSApplicationDelegate via swizzle.
// Returning NO keeps the app alive after orderOut:; sokol_app's frame timer
// keeps firing and our snap pipeline keeps reading the offscreen RT.
static BOOL stayAliveAfterLastWindowClosed(id /*self*/, SEL /*cmd*/, NSApplication* /*sender*/)
{
    return NO;
}

static void SwizzleTerminateAfterLastWindow()
{
    if (s_swizzled || !NSApp) return;
    id delegate = [NSApp delegate];
    if (!delegate) return;

    Class cls = object_getClass(delegate);
    SEL sel = @selector(applicationShouldTerminateAfterLastWindowClosed:);

    Method existing = class_getInstanceMethod(cls, sel);
    if (existing)
    {
        method_setImplementation(existing, (IMP) stayAliveAfterLastWindowClosed);
    }
    else
    {
        class_addMethod(cls, sel,
                        (IMP) stayAliveAfterLastWindowClosed,
                        "B@:@");
    }
    s_swizzled = true;
    NSLog(@"[headless] swizzled NSApplicationDelegate to keep app alive after orderOut");
}

void HideAllWindows()
{
    if (!NSApp) return;
    @autoreleasepool {
        // Swizzle the delegate's terminate-after-last-window-closed so the
        // app keeps running even when the window is mostly off-screen.
        SwizzleTerminateAfterLastWindow();

        NSArray<NSWindow*>* wins = [NSApp windows];
        const int count = (int) wins.count;
        for (NSWindow* w in wins) {
            // Strategy: keep the window the SAME SIZE (sapp_width/height
            // drive the snap-capture RT, so we must not resize), but:
            //   - strip the title bar / buttons (borderless),
            //   - reposition so only a 1-px corner is on the visible
            //     screen — the OS clamps fully off-screen windows back
            //     on, but tolerates "almost off-screen" placements.
            //   - tag as transient + ignores-cycle so Mission Control,
            //     Cmd-Tab, and Dock don't surface it.
            //   - ignore mouse events so a stray click can't focus it.
            //   - alphaValue 0 hides what little does show (1 pixel).
            const NSRect cur = [w frame];
            // Place the window so its bottom-right corner aligns with
            // the screen's top-left; only the bottom-right pixel sits on
            // visible screen. Coordinate system: macOS Y grows up.
            NSScreen* mainScreen = [NSScreen mainScreen];
            const NSRect screenRect = mainScreen ? [mainScreen frame] : NSMakeRect(0,0,1920,1080);
            const CGFloat x = -(cur.size.width  - 1.0);
            const CGFloat y = screenRect.size.height - 1.0;
            [w setStyleMask:NSWindowStyleMaskBorderless];
            [w setFrameOrigin:NSMakePoint(x, y)];
            [w setAlphaValue:0.0];
            [w setIgnoresMouseEvents:YES];
            [w setCollectionBehavior:(NSWindowCollectionBehaviorTransient
                                      | NSWindowCollectionBehaviorIgnoresCycle)];
            [w setLevel:NSScreenSaverWindowLevel + 1];  // above normal; never gets focus
        }
        if (++s_hideCalls <= 3) {
            NSLog(@"[headless] HideAllWindows call #%d (%d window(s) in list)",
                  s_hideCalls, count);
        }
    }
}

} // namespace HeadlessWindow
