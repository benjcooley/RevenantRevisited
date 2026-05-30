// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  headless_window.h - Hide main window for --headless / --snap mode   *
// *************************************************************************
//
// `--headless` on the CLI: don't show the sokol_app window. Implemented
// macOS-side in `platform/macosx/headless_window.mm` (orderOut + offscreen
// frame + ActivationPolicyProhibited). On other platforms the helper is a
// no-op; the snap pipeline still works because the GPU readback reads our
// own offscreen render target, not the window contents.
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

// Platform-specific. macOS: orderOut all NSWindows + move them off-screen +
// set the process activation policy to "background only" so the window
// never shows in Dock / Mission Control. Safe to call every frame; cheap.
// On non-Apple builds it is a no-op (build still links).
void HideAllWindows();

} // namespace HeadlessWindow
