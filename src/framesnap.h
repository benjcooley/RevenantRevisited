// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  framesnap.h - Headless snapshot + filmstrip output                   *
// *************************************************************************
//
// CLI:
//   --snap=<path.png>             single-frame PNG of the backbuffer after
//                                 a short warmup, then exit.
//   --filmstrip=<N>,<DURATION>    capture N frames DURATION seconds apart
//                                 in sim time. Writes individual frames as
//                                 <prefix><idx>.png plus an NxN grid
//                                 composite <prefix>filmstrip.png.
//                                 DURATION==0 enables MANUAL mode: the
//                                 game must call FrameSnap::TriggerSnapshot()
//                                 to capture each frame (used by tester /
//                                 input-simulator agents to snap after
//                                 each driven action). Once N captures are
//                                 in, a filmstrip is written and a NEW
//                                 batch starts (<prefix>filmstrip-002.png,
//                                 <prefix>filmstrip-003.png, ...). Auto
//                                 mode (DURATION>0) exits after one batch.
//   --snapprefix=<PREFIX>         prefix for filmstrip output files; lets
//                                 parallel agents not stomp output, AND
//                                 subfolder under filmstrips/ so the
//                                 PNG count per folder stays sane:
//                                   (omitted)         → filmstrips/
//                                   tabs_             → filmstrips/tabs_
//                                   sidebar/tabs_     → filmstrips/sidebar/tabs_
//                                   hud/v3/run-       → filmstrips/hud/v3/run-
//                                   /tmp/foo_         → /tmp/foo_ (absolute)
//                                 Parent dirs are mkdir'd recursively on
//                                 first write. (Single --snap=PATH is
//                                 unaffected — PATH is used verbatim.)
//
// On capture, reads the Display.BackBuffer() sg_image via the Metal
// readback path (src/platform/macosx/renderer_readback.mm) and saves PNG
// via stb_image_write. The window stays on-screen (sokol_app still owns
// the run loop) but focus is not required; the readback is from GPU
// memory, not the screen.
//
// *************************************************************************

#pragma once

#include <cstdint>
#include <string>

namespace FrameSnap {

// Parsed-CLI state for the active run.
struct SConfig {
    bool        active        = false;
    bool        is_filmstrip  = false;   // false = single --snap
    int32_t     frames        = 1;       // total frames to capture
    double      interval_sec  = 0.0;     // gap between captures (filmstrip)
    std::string out_path;                // --snap path or filmstrip basename
    std::string prefix;                  // --snapprefix value
    int32_t     warmup_frames = 6;       // discard frames before first capture

    // --snaprect=x,y,w,h — capture only this subrect of the display.
    // (-1, -1, -1, -1) means "whole display".
    int32_t     rect_x        = -1;
    int32_t     rect_y        = -1;
    int32_t     rect_w        = -1;
    int32_t     rect_h        = -1;
};

// Parse the CLI args; returns true if any snap flag was found and `cfg`
// was populated. Called once near the top of GetParameters().
bool ParseArgs(int argc, char** argv, SConfig& cfg);

// Set the current config. Call after ParseArgs returns true.
void SetConfig(const SConfig& cfg);
const SConfig& GetConfig();
bool Active();

// Called once per frame from the engine's per-frame entry. In auto mode
// (--snap or --filmstrip with DURATION>0) it decides whether to capture
// this frame, runs the readback, and on the Nth capture writes PNG(s)
// and signals sapp_request_quit. In manual mode (--filmstrip with
// DURATION==0) it does nothing — captures only fire via TriggerSnapshot.
void TickAfterRender();

// Manually capture one frame into the active filmstrip. Intended to be
// called by a tester / input-simulator after it has driven a state change
// and wants that state preserved as the next filmstrip cell. No-op when
// not active or not in manual mode. After N captures the filmstrip is
// written and a fresh batch begins; the process keeps running so the
// tester can produce multiple filmstrips.
//
// `label` is optional — appended to the per-frame filename (kebab-cased)
// so saved PNGs are self-describing: `prefix001-002-click-upper-book.png`.
// Pass nullptr / empty string for no label.
//
// Returns true if a capture happened this call, false otherwise.
bool TriggerSnapshot(const char* label = nullptr);

// Internal: write a single RGBA8 image to disk via stb_image_write.
bool WritePngRGBA(const char* path, int32_t w, int32_t h, const uint8_t* pixels);

} // namespace FrameSnap
