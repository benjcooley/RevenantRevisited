// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   uinineslicetest.cpp - --test=ui-nineslice: 9-slice rect math       *
// *************************************************************************
//
// See uinineslicetest.h.
//
// *************************************************************************

#include "uinineslicetest.h"

#include "display.h"
#include "logging.h"
#include "time.h"

#include <algorithm>
#include <cmath>

namespace {

// Mirrors the slicing math in TRenderer::DrawNineSlice so we can log what
// the renderer would issue for a given (source, insets, dest). Keeps the
// test mode self-contained and lets the renderer impl stay focused on the
// blit path. If the algorithm in renderer.cpp ever diverges from this,
// the test mode log breakdown will visibly drift from the runtime behavior
// when we visualize it in B.x -- that's the intended detector.
void DumpNineSlice(int32_t sw, int32_t sh,
                   int32_t l, int32_t t, int32_t r, int32_t b,
                   int32_t dx, int32_t dy, int32_t dw, int32_t dh,
                   const char* label)
{
    l = (std::max)(0, (std::min)(l, sw / 2));
    r = (std::max)(0, (std::min)(r, sw / 2));
    t = (std::max)(0, (std::min)(t, sh / 2));
    b = (std::max)(0, (std::min)(b, sh / 2));

    const int32_t dw1 = dw - l - r;
    const int32_t dh1 = dh - t - b;

    log_info("[ui-nineslice] === %s: src %dx%d insets L%d T%d R%d B%d "
             "-> dst %dx%d at (%d,%d) ===", label, sw, sh, l, t, r, b,
             dw, dh, dx, dy);
    log_info("[ui-nineslice]   TL %dx%d   T %dx%d (edge w=%d)   TR %dx%d",
             l, t,    dw1, t,  dw1, r, t);
    log_info("[ui-nineslice]   L  %dx%d   C %dx%d (edge h=%d)   R  %dx%d",
             l, dh1,  dw1, dh1, dh1, r, dh1);
    log_info("[ui-nineslice]   BL %dx%d   B %dx%d              BR %dx%d",
             l, b,    dw1, b,  r, b);
    if (dw1 <= 0)
        log_info("[ui-nineslice]   ! dest narrower than (L+R) -- center & top/bottom edges skipped");
    if (dh1 <= 0)
        log_info("[ui-nineslice]   ! dest shorter than (T+B) -- center & left/right edges skipped");
}

}  // namespace

bool InitializeUINineSliceMode()
{
    log_info("[ui-nineslice] synthetic source: 32x32 RGBA, insets {8,8,8,8}");
    log_info("[ui-nineslice] (no real bitmap loaded yet -- this is a math-dump verification; "
             "visual blit verifies when first B-phase pane wires a widget atlas)");

    constexpr int32_t SW = 32, SH = 32, L = 8, T = 8, R = 8, B = 8;

    DumpNineSlice(SW, SH, L, T, R, B,  0,   0,    64,  64,  "1x-ish 64x64");
    DumpNineSlice(SW, SH, L, T, R, B,  0,   0,   128,  64,  "wide 128x64 (button)");
    DumpNineSlice(SW, SH, L, T, R, B,  0,   0,   256, 128,  "large 256x128 (panel)");
    DumpNineSlice(SW, SH, L, T, R, B,  0,   0,   320,  80,  "very-wide 320x80 (toolbar)");
    DumpNineSlice(SW, SH, L, T, R, B,  0,   0,    80, 320,  "very-tall 80x320 (sidebar)");
    DumpNineSlice(SW, SH, L, T, R, B,  0,   0,    20,  20,  "degenerate 20x20 (smaller than 2*inset)");

    return true;
}

void RenderUINineSliceMode()
{
    const double tt = TTime::Time();
    const float r = 0.14f + 0.05f * float(std::sin(tt * 0.8));
    const float g = 0.10f + 0.05f * float(std::sin(tt * 1.0 + 1.0));
    const float b = 0.18f + 0.05f * float(std::sin(tt * 1.2 + 2.0));
    Display.BackBuffer()->StartPass(r, g, b, 1.0f);
    Display.BackBuffer()->EndPass();
}

void CloseUINineSliceMode()
{
}
