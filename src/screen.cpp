// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                screen.cpp  - EXILE Screen Object File                 *
// *************************************************************************

#include <stdio.h>

#include <algorithm>

#include "revenant.h"
#include "bitmap.h"
#include "display.h"
#include "mainwnd.h"
#include "timer.h"
#include "mappane.h"
#include "screen.h"
#include "sound.h"
#include "time.h"

int32_t cursorx = 0;        // Mouse cursor positions
int32_t cursory = 0;
int32_t mousebutton = 0;    // Mouse button

extern bool LoaderWait;

// Screen Display Functions

// ----------------------------------------------------------------------------
// TPane retained-mode hierarchy (A.2a). See screen.h for ownership notes.
// ----------------------------------------------------------------------------

TPane::~TPane()
{
    if (parent)
        parent->RemoveChild(this);
    for (TPane* c : children)
        if (c && c->parent == this)
            c->parent = nullptr;
    children.clear();
}

void TPane::AddChild(TPane* child)
{
    if (!child || child == this)
        return;
    // If the child is already parented somewhere (including us), unlink it
    // from its previous parent first so the back-pointer stays single-valued.
    if (child->parent)
        child->parent->RemoveChild(child);
    child->parent = this;
    children.push_back(child);
    // A new child counts as a change in this subtree; propagate dirty up so
    // any cached layout / draw work in ancestors gets re-evaluated.
    SetDirty(true);
}

void TPane::RemoveChild(TPane* child)
{
    if (!child)
        return;
    for (auto it = children.begin(); it != children.end(); ++it)
    {
        if (*it == child)
        {
            children.erase(it);
            // Only clear the back-pointer if it still references us; protects
            // against the child having been re-parented before we got here.
            if (child->parent == this)
                child->parent = nullptr;
            SetDirty(true);
            return;
        }
    }
}

// ----------------------------------------------------------------------------
// TPane two-pass layout (A.2b)
//
// Measure: bottom-up.  Each pane reports its preferred size for the given
//          parent constraint. Leaves use their explicit (newwidth, newheight);
//          containers recurse and aggregate along their axis.
// Arrange: top-down.    Container parents assign final rects to children
//          inside their own content rect, honoring per-child margin and
//          sizing policy.
// ----------------------------------------------------------------------------

SSize TPane::MeasureSelf(const SSize& parentConstraint)
{
    if (layoutKind == SLayoutKind::None)
    {
        // Leaf / explicit-rect pane. Preferred size = the rect the caller
        // configured. parentConstraint is ignored intentionally; non-container
        // panes don't react to their parent.
        //
        // Still recurse into children so anchored descendants (A.2c) have a
        // valid `measured` size for LayoutChildren to read. Children's
        // measured sizes don't affect this pane's own measurement.
        for (TPane* c : children)
        {
            if (!c) continue;
            const SSize cConstraint{
                parentConstraint.w - c->margin.Horizontal(),
                parentConstraint.h - c->margin.Vertical()
            };
            c->MeasureSelf(cConstraint);
        }
        measured = SSize(newwidth, newheight);
        return measured;
    }

    // Container. Subtract this pane's own padding from the available
    // constraint passed to children.
    const SSize childConstraint{
        parentConstraint.w - padding.Horizontal(),
        parentConstraint.h - padding.Vertical()
    };

    int32_t totalAlong = 0;   // sum of child sizes along the layout axis
    int32_t maxCross   = 0;   // max child size on the cross axis
    int32_t visible    = 0;   // counted children (for spacing)

    for (TPane* c : children)
    {
        if (!c)
            continue;
        // Margin reduces what the child can ask for; pass through the rest.
        const SSize cConstraint{
            childConstraint.w - c->margin.Horizontal(),
            childConstraint.h - c->margin.Vertical()
        };
        const SSize cm = c->MeasureSelf(cConstraint);

        if (layoutKind == SLayoutKind::Vertical)
        {
            totalAlong += cm.h + c->margin.Vertical();
            maxCross    = (std::max)(maxCross, cm.w + c->margin.Horizontal());
        }
        else  // Horizontal
        {
            totalAlong += cm.w + c->margin.Horizontal();
            maxCross    = (std::max)(maxCross, cm.h + c->margin.Vertical());
        }
        ++visible;
    }

    if (visible > 1)
        totalAlong += spacing * (visible - 1);

    if (layoutKind == SLayoutKind::Vertical)
        measured = SSize(maxCross + padding.Horizontal(),
                         totalAlong + padding.Vertical());
    else
        measured = SSize(totalAlong + padding.Horizontal(),
                         maxCross + padding.Vertical());

    return measured;
}

void TPane::LayoutChildren()
{
    if (children.empty())
        return;

    // Content rect = own rect minus padding (origin in this pane's
    // coordinate space; child positions are relative to the pane in the
    // same screen-coords that x/y use).
    const int32_t contentX = x + padding.left;
    const int32_t contentY = y + padding.top;
    const int32_t contentW = (std::max)(0, width  - padding.Horizontal());
    const int32_t contentH = (std::max)(0, height - padding.Vertical());

    // None-layout parents (A.2c): position any anchored children against the
    // content rect. Non-anchored children keep their explicit (x, y) so this
    // path stays vanilla for everything that hasn't opted in.
    if (layoutKind == SLayoutKind::None)
    {
        for (TPane* c : children)
        {
            if (!c || c->anchor == SAnchor::None)
                continue;

            // Use the child's measured size (populated by MeasureSelf) as
            // the anchored box; falls back to the explicit width/height
            // for leaves that never set anything.
            const int32_t cw = c->measured.w > 0 ? c->measured.w : c->GetWidth();
            const int32_t ch = c->measured.h > 0 ? c->measured.h : c->GetHeight();

            int32_t cx = contentX, cy = contentY;
            switch (c->anchor)
            {
              case SAnchor::TopLeft:
              case SAnchor::CenterLeft:
              case SAnchor::BottomLeft:
                cx = contentX + c->margin.left;
                break;
              case SAnchor::TopCenter:
              case SAnchor::Center:
              case SAnchor::BottomCenter:
                cx = contentX + (contentW - cw) / 2;
                break;
              case SAnchor::TopRight:
              case SAnchor::CenterRight:
              case SAnchor::BottomRight:
                cx = contentX + contentW - cw - c->margin.right;
                break;
              default:
                break;
            }
            switch (c->anchor)
            {
              case SAnchor::TopLeft:
              case SAnchor::TopCenter:
              case SAnchor::TopRight:
                cy = contentY + c->margin.top;
                break;
              case SAnchor::CenterLeft:
              case SAnchor::Center:
              case SAnchor::CenterRight:
                cy = contentY + (contentH - ch) / 2;
                break;
              case SAnchor::BottomLeft:
              case SAnchor::BottomCenter:
              case SAnchor::BottomRight:
                cy = contentY + contentH - ch - c->margin.bottom;
                break;
              default:
                break;
            }

            c->Resize(cx, cy, cw, ch);
            c->PaneResized();
            if (c->layoutKind != SLayoutKind::None)
                c->LayoutChildren();
        }
        return;
    }

    // Phase 1 of arrange: along-axis sizing.
    //
    // For Vertical: along=h, cross=w. Each child contributes its measured
    // along-axis size if Fixed, plus its along-axis margin. Greedy children
    // contribute 0 to the fixed total; the leftover is divided among them
    // by greedyWeight after all fixeds are tallied.

    auto alongOf = [&](TPane* c) -> int32_t {
        return layoutKind == SLayoutKind::Vertical ? c->measured.h : c->measured.w;
    };
    auto alongMarginOf = [&](TPane* c) -> int32_t {
        return layoutKind == SLayoutKind::Vertical ? c->margin.Vertical()
                                                   : c->margin.Horizontal();
    };
    auto alongPolicyOf = [&](TPane* c) -> SSizePolicy {
        return layoutKind == SLayoutKind::Vertical ? c->vsizePolicy : c->hsizePolicy;
    };

    int32_t visible = 0;
    int32_t fixedAlong = 0;
    float   weightSum  = 0.0f;

    for (TPane* c : children)
    {
        if (!c) continue;
        ++visible;
        fixedAlong += alongMarginOf(c);
        if (alongPolicyOf(c) == SSizePolicy::Greedy)
            weightSum += (std::max)(0.0f, c->greedyWeight);
        else
            fixedAlong += alongOf(c);
    }
    if (visible > 1)
        fixedAlong += spacing * (visible - 1);

    const int32_t alongAvail = (layoutKind == SLayoutKind::Vertical ? contentH : contentW);
    const int32_t greedyAvail = (std::max)(0, alongAvail - fixedAlong);

    // Phase 2 of arrange: walk children in order, place each.
    int32_t cursor = (layoutKind == SLayoutKind::Vertical ? contentY : contentX);
    bool first = true;
    for (TPane* c : children)
    {
        if (!c) continue;

        if (!first)
            cursor += spacing;
        first = false;

        // Per-child along/cross sizes after policy.
        int32_t childAlong;
        if (alongPolicyOf(c) == SSizePolicy::Greedy && weightSum > 0.0f)
            childAlong = static_cast<int32_t>(
                (std::max)(0.0f, c->greedyWeight) / weightSum * float(greedyAvail));
        else
            childAlong = alongOf(c);

        // Cross sizing: Greedy fills the cross-axis content; Fixed uses measured.
        int32_t childCrossAvail = (layoutKind == SLayoutKind::Vertical
            ? contentW - c->margin.Horizontal()
            : contentH - c->margin.Vertical());
        childCrossAvail = (std::max)(0, childCrossAvail);

        int32_t childCross;
        auto crossPolicy = (layoutKind == SLayoutKind::Vertical
            ? c->hsizePolicy : c->vsizePolicy);
        auto crossMeasured = (layoutKind == SLayoutKind::Vertical
            ? c->measured.w : c->measured.h);
        if (crossPolicy == SSizePolicy::Greedy)
            childCross = childCrossAvail;
        else
            childCross = (std::min)(crossMeasured, childCrossAvail);

        // Place the child. Vertical: cursor is Y; cross-axis is X (with margin.left).
        int32_t cx, cy, cw, ch;
        if (layoutKind == SLayoutKind::Vertical)
        {
            cx = contentX + c->margin.left;
            cy = cursor   + c->margin.top;
            cw = childCross;
            ch = childAlong;
            cursor += childAlong + c->margin.Vertical();
        }
        else
        {
            cx = cursor   + c->margin.left;
            cy = contentY + c->margin.top;
            cw = childAlong;
            ch = childCross;
            cursor += childAlong + c->margin.Horizontal();
        }

        // Apply through the same channel as explicit caller mutation, so
        // WasResized() and the next-frame-commit path stay consistent.
        c->Resize(cx, cy, cw, ch);
        c->PaneResized();   // commit immediately -- layout passes don't defer

        // Recurse into the child's own children if it's a container.
        if (c->layoutKind != SLayoutKind::None)
            c->LayoutChildren();
    }
}

void TPane::RunLayoutPass()
{
    // Pass 1: measure (bottom-up). The root pane measures against its own
    // current size as a constraint -- callers that want the root to size to
    // a viewport (window, sokol_app drawable, etc.) should Resize/PaneResized
    // the root first.
    MeasureSelf(SSize(width, height));
    // Pass 2: arrange (top-down).
    LayoutChildren();
    SetDirty(false);   // any layout work for this subtree is now current
}

void TPane::OnCanvasResize(int32_t newCanvasW, int32_t newCanvasH)
{
    // Default screen-root behavior: stretch to fill the new canvas at
    // (0,0), commit the resize, and re-run layout (which anchors any
    // anchored children against the new content rect). Subclasses that
    // want different semantics (centered fixed-size, no-op, ...) override.
    Resize(0, 0, newCanvasW, newCanvasH);
    PaneResized();
    RunLayoutPass();
}

// ----------------------------------------------------------------------------
// TScreen
// ----------------------------------------------------------------------------

TScreen::TScreen()
{
    nextscreen = nullptr;
}

void TScreen::OnCanvasResize(int32_t newCanvasW, int32_t newCanvasH)
{
    // Broadcast to all registered panes. Hidden panes still receive the
    // event so their layout stays current for when they're shown again.
    for (int32_t i = 0; i < panes.NumItems(); ++i)
    {
        if (panes.Used(i) && panes[i])
            panes[i]->OnCanvasResize(newCanvasW, newCanvasH);
    }
}

TScreen::~TScreen()
{
}

bool TScreen::BeginScreen()
{
    // TODO(port): InitBackgroundSystem removed — CPU background caching obsolete under sokol GPU compositor.
    firstframe = true;  // Tell timer tick function not to flip a page the first time
    done = false;
    lastPulseLegacyFrame = TTime::LegacyFrameCount() - 1;  // catch up to current on first tick

    panes.Clear();
    screenframes = 0;
    for (int32_t loop = 0; loop < NUMEXCLUSIVEPANES; loop++)
        exclusive[loop] = 0;
    numexclusive = 0;

    return Initialize();
}

void TScreen::EndScreen()
{
    int32_t loop;

    Close();

    panes.Clear();
    screenframes = 0;
    for (loop = 0; loop < NUMEXCLUSIVEPANES; loop++)
        exclusive[loop] = 0;
    numexclusive = 0;
}

// Copies contents of display's back buffer to the user's display immediately. (For
// 'loading' screens which have to update from within a single timer tick).
void TScreen::PutToScreen()
{
    // TODO(port): Display.PutToScreen removed — sokol handles presentation.
}

// *************************
// * Screen Pane Functions *
// *************************

int32_t TScreen::FindPane(PTPane pane)
{
    for (TPaneIterator p(&panes); p; p++)
        if (pane == p.Item())
            return p.ItemNum();

    return -1;
}

int32_t TScreen::AddPane(PTPane pane, int32_t panenum)
{
  // Do pane list
    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
    {
        if (panes[loop] == pane)
        {
            if (panenum < 0)
                return loop;
            else
                panes.Remove(loop);
        }
    }

    int32_t newnum;
    if (panenum < 0)
        newnum = panes.Set(pane, panes.NumItems());
    else
        newnum = panes.Set(pane, panenum);

    pane->SetScreen(this);

    return newnum;
}

bool TScreen::RemovePane(PTPane pane)
{
    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
    {
        if (panes[loop] == pane)
        {
            panes.Remove(loop);
            pane->SetScreen(nullptr);
            return true;
        }
    }

    return false;
}

bool TScreen::SetExclusivePane(int32_t panenum, bool completeexclusion)
{
    if (!panes[panenum] || numexclusive >= NUMEXCLUSIVEPANES)
        return false;

    exclusive[numexclusive] = panenum;
    complete[numexclusive] = completeexclusion;
    numexclusive++;

    return true;
}

void TScreen::ReleaseExclusivePane(int32_t panenum)
{
    int32_t pos = 0;
    for (int32_t c = 0; c < numexclusive; c++)
    {
        if (exclusive[c] != panenum)
        {
            exclusive[pos] = exclusive[c];
            pos++;
        }
    }

    numexclusive = pos;
}

void TScreen::RedrawAllPanes()
{
    Display.Reset();

  // Do exclusive
    if (numexclusive > 0)
    {
        PTPane pane = panes[exclusive[numexclusive - 1]];
        if (pane && !pane->IsHidden())
            pane->SetDirty(true);

        if (complete[numexclusive - 1])
        {
            Display.Reset();
            return;
        }
    }

  // Do pane list
    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
        if (panes.Used(loop) && !panes[loop]->IsHidden())
            panes[loop]->SetDirty(true);

    Display.Reset();
}

// *****************************
// * Virtual Handler Functions *
// *****************************

void TScreen::DrawBackground()
{
    Display.Reset();

  // Do exclusive
    if (numexclusive > 0)
    {
        PTPane pane = panes[exclusive[numexclusive - 1]];
        if (pane && !pane->IsHidden())
        {
            if (dirty)
                pane->Update();

            pane->SetClipRect();
            pane->DrawBackground();
        }

        if (complete[numexclusive - 1])
        {
            dirty = false;
            Display.Reset();
            return;
        }
    }

  // Do pane list
    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop) || panes[loop]->IsHidden())
            continue;

        if (dirty)
            panes[loop]->Update();

        panes[loop]->SetClipRect();
        panes[loop]->DrawBackground();
    }

    dirty = false;
    Display.Reset();
}

void TScreen::Pulse()
{
    Display.Reset();

  // Do exclusive
    if (numexclusive > 0)
    {
        PTPane pane = panes[exclusive[numexclusive - 1]];
        if (pane && !pane->IsHidden())
        {
            pane->SetClipRect();
            pane->Pulse();
        }

        if (complete[numexclusive - 1])
        {
            Display.Reset();
            return;
        }
    }

  // Do pane list
    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop) || panes[loop]->IsHidden())
            continue;
        panes[loop]->SetClipRect();
        panes[loop]->Pulse();
    }

    Display.Reset();
}

void TScreen::Animate(bool draw)
{
    Display.Reset();

  // Do exclusive
    if (numexclusive > 0)
    {
        PTPane pane = panes[exclusive[numexclusive - 1]];
        if (pane && !pane->IsHidden())
        {
            pane->SetClipRect();
            pane->Animate(draw);
        }

        if (complete[numexclusive - 1])
        {
            Display.Reset();
            return;
        }
    }

  // Do pane list
    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop) || panes[loop]->IsHidden())
            continue;
        panes[loop]->SetClipRect();
        panes[loop]->Animate(draw);
    }

    Display.Reset();
}

void TScreen::MouseClick(int32_t button, int32_t x, int32_t y)
{
    Display.Reset();

  // Do exclusive
    if (numexclusive > 0)
    {
        PTPane pane = panes[exclusive[numexclusive - 1]];
        if (pane && !pane->IsHidden())
        {
            pane->SetClipRect();
            pane->MouseClick(button, x - pane->GetPosX(), y - pane->GetPosY());
        }
        Display.Reset();
        return;
    }

  // Do pane list
    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop) || panes[loop]->IsHidden() || panes[loop]->IsIgnoringInput())
            continue;

        panes[loop]->SetClipRect();
        int32_t nx = x - panes[loop]->GetPosX();
        int32_t ny = y - panes[loop]->GetPosY();

      // send mouseup buttons to all panes, not just the owner of that screen space
        if (panes[loop]->InPane(nx, ny) ||
            (button == MB_LEFTUP || button == MB_RIGHTUP || button == MB_MIDDLEUP))
            panes[loop]->MouseClick(button, nx, ny);
    }

    Display.Reset();
}

void TScreen::MouseMove(int32_t button, int32_t x, int32_t y)
{
    Display.Reset();

  // Do exclusive
    if (numexclusive > 0)
    {
        PTPane pane = panes[exclusive[numexclusive - 1]];
        if (pane && !pane->IsHidden())
        {
            pane->SetClipRect();
            pane->MouseMove(button, x - pane->GetPosX(), y - pane->GetPosY());
        }
        Display.Reset();
        return;
    }

  // Do pane list
    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop) || panes[loop]->IsHidden() || panes[loop]->IsIgnoringInput())
            continue;

        panes[loop]->SetClipRect();
        int32_t nx = x - panes[loop]->GetPosX();
        int32_t ny = y - panes[loop]->GetPosY();
        panes[loop]->MouseMove(button, nx, ny);
    }

    Display.Reset();
}

void TScreen::KeyPress(int32_t key, bool down)
{
    Display.Reset();

  // Do exclusive
    if (numexclusive > 0)
    {
        PTPane pane = panes[exclusive[numexclusive - 1]];
        if (pane && !pane->IsHidden())
            pane->KeyPress(key, down);
        Display.Reset();
        return;
    }

    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop) || panes[loop]->IsHidden() || panes[loop]->IsIgnoringInput())
            continue;
        panes[loop]->SetClipRect();
        panes[loop]->KeyPress(key, down);
    }

    Display.Reset();
}

void TScreen::CharPress(int32_t key, bool down)
{
    Display.Reset();

  // Do exclusive
    if (numexclusive > 0)
    {
        PTPane pane = panes[exclusive[numexclusive - 1]];
        if (pane && !pane->IsHidden())
            pane->CharPress(key, down);
        Display.Reset();
        return;
    }

    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop) || panes[loop]->IsHidden() || panes[loop]->IsIgnoringInput())
            continue;
        panes[loop]->SetClipRect();
        panes[loop]->CharPress(key, down);
    }

    Display.Reset();
}

void TScreen::Joystick(int32_t key, bool down)
{
    Display.Reset();

  // Do exclusive
    if (numexclusive > 0)
    {
        PTPane pane = panes[exclusive[numexclusive - 1]];
        if (pane && !pane->IsHidden())
            pane->Joystick(key, down);
        Display.Reset();
        return;
    }

    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop) || panes[loop]->IsHidden() || panes[loop]->IsIgnoringInput())
            continue;

        panes[loop]->SetClipRect();
        panes[loop]->Joystick(key, down);
    }

    Display.Reset();
}

// ***************************
// * Screen System Functions *
// ***************************

// Non-blocking screen startup. Under sokol_app there is no message pump for us
// to own, so ShowScreen no longer runs a TimerLoop — it just Initialize()s the
// screen and hands control back to AppFrame, which drives TimerTick() per sokol
// frame and calls EndCurrentScreen() once IsDone() flips true.
TScreen* TScreen::ShowScreen(TScreen* screen, int32_t /*ticks*/)
{
    if (!screen)
        return nullptr;

    CurrentScreen = screen;
    if (!screen->BeginScreen())
    {
        CurrentScreen = nullptr;
        return nullptr;
    }
    return screen;
}

void TScreen::EndCurrentScreen()
{
    if (!CurrentScreen)
        return;
    CurrentScreen->EndScreen();
    CurrentScreen = nullptr;
}

// One non-blocking tick. Called once per sokol AppFrame.
//
// Pulses catch up to the 24Hz legacy frame counter in scaled simulation time.
// Slow-motion scales leave extra render frames for interpolation; fast scales
// may process more than one fixed legacy tick before drawing.
// DrawBackground/Animate run every call since sokol presents at vsync.
// Input is delivered out-of-band via AppEvent, so there is no message pump
// here any more. Returns false when the screen has set `done` — AppFrame
// uses that as the signal to EndCurrentScreen and advance.
// Sim half of the frame: catch up any pending 24Hz Pulses. Pure logic;
// must not produce sokol draw calls. See docs/FRAME_PIPELINE.md.
void TScreen::Tick()
{
    if (Closing)
        return;

    // Resize panes that asked for it before we pulse (panes may set new
    // map position in their Pulse, which depends on post-resize dims).
    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop))
            continue;
        if (panes[loop]->WasResized())
            panes[loop]->PaneResized();
    }

    // Catch up missed Pulses. At 1x / 60Hz this is 0 or 1 per call;
    // debug time scales deliberately alter that cadence.
    const int64_t lf = TTime::LegacyFrameCount();
    while (lastPulseLegacyFrame < lf)
    {
        Pulse();
        lastPulseLegacyFrame++;
        screenframes++;
    }
}

// Draw half of the frame. Two-phase to keep sokol's "one pass active
// at a time" rule:
//
//   Phase 1 — Animate (3D scene + screen-owned passes). The screen
//             manages its own sokol passes here (TMapRenderer opens
//             G-buffer, light, etc. on its own offscreen targets).
//             NO overlay pass is open during Animate, so screens are
//             free to call sg_begin_pass.
//
//   Phase 2 — Overlay2D pass on the backbuffer. While this is open
//             every legacy Display.Put / Blit / Line lands in the
//             backbuffer texture. We bracket DrawBackground (pane
//             backdrops drawn on top of 3D) and DrawMouseCursor
//             inside this pass. FlipPage then composites the
//             backbuffer over the 3D scene with alpha blend.
//
// See docs/FRAME_PIPELINE.md.
void TScreen::DrawFrame()
{
    if (Closing || !Display.IsActive())
        return;

    // Propagate dirty + update pane scroll. Cheap; safe to run draw-side
    // since Pulse already happened.
    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop))
            continue;
        if (dirty)
            panes[loop]->SetDirty(true);
        panes[loop]->UpdateBackgroundScrollPos();
    }

    // --- Phase 1: 3D scene + screen-owned passes -------------------------
    Display.Reset();
    Animate(true);

    // --- Phase 2: Overlay2D -- legacy 2D blits land in the backbuffer ----
    // Overlay2D pass (legacy Display.Put backbuffer fallback). HUD
    // drawing now goes through Renderer's HUD items (registered in the
    // runtime mode's OnEnter, drawn by Renderer->DrawHud inside the
    // swapchain pass in FlipPage). The overlay backbuffer is still
    // opened so any not-yet-migrated Display.Put caller (DrawBackground,
    // etc.) has a pass to draw into, even if those draws don't currently
    // surface visually under sokol.
    Display.BeginOverlay();

    Display.Reset();
    if (!firstframe)
    {
        DrawBackground();
    }

    Display.EndOverlay();

    firstframe = false;
}

// Legacy entry point. Kept so callers in test modes and anywhere we
// haven't migrated yet continue to work. New code should call Tick()
// and DrawFrame() separately. TODO(frame-pipeline): remove once last
// caller migrates.
bool TScreen::TimerTick(bool draw)
{
    if (Closing)
        return false;

    Tick();
    if (draw)
        DrawFrame();

    return !done;
}

// *******************
// * TPane Functions *
// *******************

bool TPane::Initialize() 
{ 
    if (isopen)
        return true;
    
    x = newx; y = newy; width = newwidth; height = newheight;

    backgroundbuffer = -1; 
    oldscrollx = scrollx = newscrollx = oldscrolly = scrolly = newscrolly = 0;
    dirty = true;
    ignoreinput = false;

  // Create any background buffers for this pane
    CreateBackgroundBuffers();

    isopen = true;

    return true; 
}

void TPane::Close()
{
  // Free all background buffers for this pane  
    FreeBackgroundBuffers();

    isopen = false;
}

// Causes the pane to be immediately shown on the screen.  Useful for when
// the pane contains a status or 'loading' bar that is updated during a single
// timer tick.
void TPane::PutToScreen()
{
    // TODO(port): Display.PutToScreen removed — sokol handles presentation.
}

// This function can be called to draw a pane immediately (instead of waiting for the
// screen TimerTick() function to call the various draw routines.  Note that no user
// input will be processed, and only the Pulse(), DrawBackground(), and Animate() functions
// are called.  The PutToScreen() function can be called immediately after this function
// to render the results to the display.
void TPane::Draw()
{
    if (!IsOpen() || IsHidden())
        return;

    SClipState cs;
    Display.SaveClipState(cs);

    SetClipRect();

    Pulse();
    DrawBackground();
    Animate(true);

    Display.RestoreClipState(cs);
}


void TPane::SetClipRect()
{
    Display.SetOrigin(x - scrollx, y - scrolly);
    Display.SetClipRect(x, y, width, height);
    Display.SetClipMode(CLIP_EDGES);
}

void TPane::UpdateBackgroundScrollPos()
{
    oldscrollx = scrollx; oldscrolly = scrolly;
    scrollx = newscrollx; scrolly = newscrolly;
    // TODO(port): Display.ScrollBackground removed — CPU background caching obsolete under sokol GPU compositor.
}

void TPane::DrawRestoreRect(int32_t x, int32_t y, int32_t width, int32_t height, uint32_t drawmode)
{
    (void)x; (void)y; (void)width; (void)height; (void)drawmode;
    // TODO(port): Display.DrawRestoreRect removed — CPU background caching obsolete under sokol GPU compositor.
}

bool TPane::IsOnScreen()
{
    return screen == CurrentScreen;
}
