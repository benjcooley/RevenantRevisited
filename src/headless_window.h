// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  headless_window.h - Hide main window for --headless / --snap mode   *
// *************************************************************************
//
// `--headless` on the CLI: don't show the sokol_app window. On macOS,
// sokol_main sets our sokol_app patch's desc.hidden / desc.no_dock_icon so
// the window is never put on screen; HideAllWindows() (implemented in
// `platform/macosx/headless_window.mm`) keeps it out of sight should
// anything order it in. On other platforms the helper is a no-op; the snap
// pipeline still works because the GPU readback reads our own offscreen
// render target, not the window contents.
//
// Snap / filmstrip do not auto-imply --headless; the user opts into the
// hidden window explicitly. Typical pairing: `--headless --snap=foo.png`.
//
// *************************************************************************

#pragma once

namespace HeadlessWindow {

// Parse `--headless` (and `--no-headless`) flags from the CLI argv.
// Returns true if --headless was found. Stores the parsed value in the
// internal flag so IsActive() returns it.
bool ParseArgs(int argc, char** argv);

// Returns true if --headless was set.
bool IsActive();

// Platform-specific. macOS: make every NSWindow transparent, click-through
// and (almost) off-screen, and keep it out of Mission Control / Cmd-Tab.
// Leaves the window closable, so a quit request still ends the app. Safe
// to call every frame; cheap. On non-Apple builds it is a no-op (build
// still links).
void HideAllWindows();

// Platform-specific. macOS: a headless run is a hidden, silent app with no
// Dock icon -- what App Nap throttles, which can hold back the frame timer
// for minutes (seen: a run that never reached its first frame, another
// stalled ~10 min). Opts the process out for the rest of the run with an
// NSProcessInfo activity that also holds off idle system sleep (an
// unattended run froze when the Mac idle-slept on battery); the display may
// still sleep. Call once, before sokol_app runs (sokol_main).
// No-op on other platforms.
void KeepAwake();

} // namespace HeadlessWindow
