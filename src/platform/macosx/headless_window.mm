// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  headless_window.mm - Keep the window out of sight in --headless mode *
// *************************************************************************
//
// The primary mechanism is our sokol_app patch: --headless sets
// desc.hidden + desc.no_dock_icon (see sokol_main), so sokol never calls
// `makeKeyAndOrderFront:` and the NSWindow is never on screen. The Metal
// device, drawable and frame timer still run, and the snap pipeline reads
// our own offscreen capture RT (TDisplay::SnapCaptureImage), so
// PNG/filmstrip output does not depend on the window being visible.
//
// HideAllWindows() is belt-and-suspenders on top of that: should anything
// order the window in, it stays transparent, off-screen and click-through.
//
// It must not get in the way of quitting. sokol quits by calling
// `performClose:` on its window, which does nothing for a window without a
// close button, so the style mask is left alone; and the app has to be
// allowed to terminate once that window closes (the sokol patch terminates
// explicitly for hidden windows, see _sapp_macos_window_delegate).
//
// *************************************************************************

#include "../../headless_window.h"

#import <Cocoa/Cocoa.h>

namespace HeadlessWindow {

static int s_hideCalls = 0;

void HideAllWindows()
{
    if (!NSApp) return;
    @autoreleasepool {
        NSArray<NSWindow*>* wins = [NSApp windows];
        const int count = (int) wins.count;
        for (NSWindow* w in wins) {
            // Strategy: keep the window the SAME SIZE (sapp_width/height
            // drive the snap-capture RT, so we must not resize), but:
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

void KeepAwake()
{
    static id s_activity = nil;     // held for the life of the process
    if (s_activity)
        return;
    @autoreleasepool {
        s_activity = [[NSProcessInfo processInfo]
            beginActivityWithOptions:(NSActivityUserInitiatedAllowingIdleSystemSleep
                                      | NSActivityLatencyCritical)
                              reason:@"Revenant --headless run"];
#if !__has_feature(objc_arc)
        [s_activity retain];
#endif
    }
}

} // namespace HeadlessWindow
