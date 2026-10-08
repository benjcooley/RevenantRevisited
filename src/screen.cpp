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
#include "logging.h"
#include "mappane.h"
#include "renderer.h"
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

void ClassicCanvasOrigin(int32_t& x, int32_t& y)
{
    x = std::max<int32_t>(0, (Display.Width()  - WIDTH)  / 2);
    y = std::max<int32_t>(0, (Display.Height() - HEIGHT) / 2);
}

// ----------------------------------------------------------------------------
// TPane draw contract, routing, modal end
// ----------------------------------------------------------------------------

void TPane::ComposeTree()
{
    if (IsHidden())
        return;
    for (TPane* child : children)
        if (child)
            child->ComposeTree();
    Compose();
}

void TPane::DrawTree()
{
    if (IsHidden())
        return;
    Draw();
    for (TPane* child : children)
        if (child)
            child->DrawTree();
}

void TPane::EndModal(int32_t result)
{
    if (screen)
        screen->RequestModalEnd(this, result);
}

namespace {

bool IsButtonUp(int32_t button)
{
    return button == MB_LEFTUP || button == MB_RIGHTUP || button == MB_MIDDLEUP;
}

}  // namespace

void TPane::RouteMouseClick(int32_t button, int32_t lx, int32_t ly)
{
    // Children last-added first (topmost first). Like the screen's flat pane
    // loop, every pane under the point gets the click and every pane gets
    // button-up, so a drag that ends elsewhere still releases.
    for (auto it = children.rbegin(); it != children.rend(); ++it)
    {
        TPane* child = *it;
        if (!child || child->IsHidden() || child->IsIgnoringInput())
            continue;
        const int32_t cx = lx + x - child->x;
        const int32_t cy = ly + y - child->y;
        if (child->InPane(cx, cy) || IsButtonUp(button))
            child->RouteMouseClick(button, cx, cy);
    }
    MouseClick(button, lx, ly);
}

void TPane::RouteMouseMove(int32_t button, int32_t lx, int32_t ly)
{
    for (auto it = children.rbegin(); it != children.rend(); ++it)
    {
        TPane* child = *it;
        if (!child || child->IsHidden() || child->IsIgnoringInput())
            continue;
        child->RouteMouseMove(button, lx + x - child->x, ly + y - child->y);
    }
    MouseMove(button, lx, ly);
}

void TPane::RouteKeyPress(int32_t key, bool down)
{
    for (auto it = children.rbegin(); it != children.rend(); ++it)
        if (*it && !(*it)->IsHidden() && !(*it)->IsIgnoringInput())
            (*it)->RouteKeyPress(key, down);
    KeyPress(key, down);
}

void TPane::RouteCharPress(int32_t key, bool down)
{
    for (auto it = children.rbegin(); it != children.rend(); ++it)
        if (*it && !(*it)->IsHidden() && !(*it)->IsIgnoringInput())
            (*it)->RouteCharPress(key, down);
    CharPress(key, down);
}

// The screen's pane tree reaches the swapchain through one HUD drawable
// (z TScreen::kPaneLayerZ), registered for as long as the screen runs
// (docs/gameflow/ARCHITECTURE.md §4.1). Panes never register drawables of
// their own.
class TScreenPaneLayer final : public THudDrawable
{
  public:
    explicit TScreenPaneLayer(TScreen* owner) : screen(owner) {}
    void Draw() override { screen->DrawPanes(); }

  private:
    TScreen* screen;
};

// The fade's black cover is the last thing in the frame: retail draws it at
// the end of TScreen::TimerTick (0x00490bd0), after the panes and the
// cursor (0x0043a480). Above the cursor HUD (z 1000); the debug UI still
// draws over it.
constexpr float kScreenFadeHudZ = 2000.0f;

class TScreenFadeLayer final : public THudDrawable
{
  public:
    explicit TScreenFadeLayer(TScreen* owner) : screen(owner) {}
    void Draw() override { screen->DrawFade(); }

  private:
    TScreen* screen;
};

// ----------------------------------------------------------------------------
// TScreenFade
// ----------------------------------------------------------------------------

void TScreenFade::Setup(int32_t numsteps)
{
    steps = numsteps;
    level = 0.0f;
    target = 0.0f;
    flags = 0;
    clock = -1.0;
}

// Both start one step back (`cur -= 1` here, `+= 1` in FadeOut): the step
// after the pulse that asked for the fade brings the level back to where it
// was, so that tick still shows the starting level and the fade takes
// `steps` ticks.
void TScreenFade::FadeIn()
{
    if (flags & kFadingIn)
        return;
    target = float(steps);
    if (level == target)
        return;
    level -= 1.0f;
    flags = (flags & ~kFadingOut) | kFadingIn;
    log_debug("[screenfade] fade in from %.2f/%d", level + 1.0f, steps);
}

void TScreenFade::FadeOut()
{
    if (flags & kFadingOut)
        return;
    target = 0.0f;
    if (level == target)
        return;
    level += 1.0f;
    flags = (flags & ~kFadingIn) | kFadingOut;
    log_debug("[screenfade] fade out from %.2f/%d", level - 1.0f, steps);
}

namespace {

float StepToward(float from, float to, double seconds)
{
    const float delta = float(seconds * TTime::LegacyFramerate);
    return from < to ? (std::min)(from + delta, to) : (std::max)(from - delta, to);
}

}  // namespace

// Retail's draw clears the busy flags once the level reaches its target
// (0x00491cb0's tail); here the advance does. The first advance counts as
// one tick, as retail's first step.
void TScreenFade::AdvanceTo(double time)
{
    const double elapsed = clock < 0.0 ? TTime::LegacyFrameSeconds : (std::max)(time - clock, 0.0);
    clock = (std::max)(time, clock);
    if (!IsBusy())
        return;
    level = StepToward(level, target, elapsed);
    if (level == target)
    {
        flags &= ~(kFadingIn | kFadingOut);
        log_debug("[screenfade] %s", level == 0.0f ? "black" : "clear");
    }
}

// Retail draws nothing at the last step and above, clears the display to
// black at step 0, and in between blends a black 640x480 quad with alpha
// 1 - step / (steps - 1), the step first quantized to 31 levels
// (`step * 31 / (steps - 1)`, then `1 - level / 31`). The port keeps the
// curve, drops the quantization and moves the level on from the last tick.
//
// Retail held each step's cover until the next tick, whose pulse runs
// before the cover changes: a fade-in started black and first showed a
// lighter cover over that next tick's picture. Scripts rely on it --
// `fadescreenin` then `player.pos` moves the player while the screen is
// still black. Interpolating ahead of the tick would show the old picture
// through a lightening cover, so while fading in the cover is drawn one
// step behind the level: the reveal trails retail's by a tick. Fading
// out, the interpolated cover is never lighter than retail's.
float TScreenFade::Opacity(double now) const
{
    float shown = level;
    if (IsBusy() && clock >= 0.0)
        shown = StepToward(level, target, (std::max)(now - clock, 0.0));
    if (flags & kFadingIn)
        shown -= 1.0f;
    if (steps < 2)
        return shown < float(steps) ? 1.0f : 0.0f;
    return std::clamp(1.0f - shown / float(steps - 1), 0.0f, 1.0f);
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
    {
        exclusive[loop] = 0;
        exclusiveflags[loop] = 0;
        modaldone[loop] = nullptr;
    }
    numexclusive = 0;
    modalends.clear();

    if (Renderer)
    {
        if (!panelayer)
            panelayer = std::make_unique<TScreenPaneLayer>(this);
        Renderer->AddHud(panelayer.get(), kPaneLayerZ);
        if (!fadelayer)
            fadelayer = std::make_unique<TScreenFadeLayer>(this);
        Renderer->AddHud(fadelayer.get(), kScreenFadeHudZ);
    }

    // REVSYNC: BeginScreen 0x0048e8f0 -- a screen that fades comes up black
    // (its Initialize set the fader up) and fades in.
    if (!Initialize())
        return false;
    if (fade)
        fade->FadeIn();
    return true;
}

void TScreen::EndScreen()
{
    int32_t loop;

    // Retail TScreen close (0x0048ea40) notifies every pane before teardown.
    BroadcastEvent(SCREENEVENT_CLOSING);
    if (Renderer && panelayer)
        Renderer->RemoveHud(panelayer.get());
    if (Renderer && fadelayer)
        Renderer->RemoveHud(fadelayer.get());

    Close();

    panes.Clear();
    screenframes = 0;
    for (loop = 0; loop < NUMEXCLUSIVEPANES; loop++)
    {
        exclusive[loop] = 0;
        exclusiveflags[loop] = 0;
        modaldone[loop] = nullptr;
    }
    numexclusive = 0;
    modalends.clear();
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
    if (panenum < 0 || !panes[panenum] || numexclusive >= NUMEXCLUSIVEPANES)
        return false;

    exclusive[numexclusive] = panenum;
    exclusiveflags[numexclusive] = completeexclusion ? MODAL_INPUT | MODAL_PAUSE | MODAL_ANIMATE
                                                     : MODAL_INPUT;
    modaldone[numexclusive] = nullptr;
    numexclusive++;

    // REVSYNC: TScreen::SetExclusivePane @ 0x0048eea0 broadcasts 0x101.
    BroadcastEvent(SCREENEVENT_MODALPUSHED, panes[panenum]);
    return true;
}

void TScreen::ReleaseExclusivePane(int32_t panenum)
{
    int32_t pos = 0;
    for (int32_t c = 0; c < numexclusive; c++)
    {
        if (exclusive[c] != panenum)
        {
            // Entries below the released one stay where they are: moving a
            // completion onto itself would empty it.
            if (pos != c)
            {
                exclusive[pos] = exclusive[c];
                exclusiveflags[pos] = exclusiveflags[c];
                modaldone[pos] = std::move(modaldone[c]);
            }
            pos++;
        }
    }
    for (int32_t c = pos; c < numexclusive; c++)
        modaldone[c] = nullptr;

    numexclusive = pos;
}

// REVSYNC: RunModal @ 0x0048f040 (AddPane + SetExclusivePane with the
// enclosing modal's flags OR'd in). REVSYNC-DIVERGENCE: retail then re-entered
// the frame loop (TimerLoop(1) until the pane closed) and returned the pane's
// result; here the result goes to `done` on the tick after EndModal, because
// sokol owns the frame loop. Player-visible behavior is the same.
bool TScreen::PushModal(PTPane pane, uint32_t flags, TModalDone done)
{
    const uint32_t inherited = numexclusive > 0 ? exclusiveflags[numexclusive - 1] : 0;
    return PushExclusive(pane, inherited | flags, std::move(done));
}

bool TScreen::PushExclusive(PTPane pane, uint32_t flags, TModalDone done)
{
    if (!pane)
        return false;
    if (numexclusive >= NUMEXCLUSIVEPANES)
    {
        log_error("[screen] PushModal: modal stack full (%d)", NUMEXCLUSIVEPANES);
        return false;
    }
    const int32_t panenum = AddPane(pane);
    exclusive[numexclusive] = panenum;
    exclusiveflags[numexclusive] = flags;
    modaldone[numexclusive] = std::move(done);
    numexclusive++;
    BroadcastEvent(SCREENEVENT_MODALPUSHED, pane);
    return true;
}

PTPane TScreen::TopModal()
{
    return numexclusive > 0 ? panes[exclusive[numexclusive - 1]] : nullptr;
}

void TScreen::RequestModalEnd(PTPane pane, int32_t result)
{
    for (const SModalEnd& end : modalends)
        if (end.pane == pane)
            return;
    modalends.push_back({pane, result});
}

void TScreen::ProcessModalEnds()
{
    if (modalends.empty())
        return;
    std::vector<SModalEnd> ends;
    ends.swap(modalends);
    for (const SModalEnd& end : ends)
    {
        const int32_t panenum = FindPane(end.pane);
        TModalDone done;
        for (int32_t c = 0; c < numexclusive; c++)
            if (exclusive[c] == panenum)
                done = std::move(modaldone[c]);
        if (panenum >= 0)
        {
            ReleaseExclusivePane(panenum);
            RemovePane(end.pane);
        }
        if (done)
            done(end.result);
    }
}

void TScreen::BroadcastEvent(int32_t code, void* param)
{
    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
        if (panes.Used(loop) && panes[loop])
            panes[loop]->OnScreenEvent(code, param);
}

PTPane TScreen::ModalFor(uint32_t flag)
{
    return ModalHas(flag) ? panes[exclusive[numexclusive - 1]] : nullptr;
}

void TScreen::ComposePanes()
{
    if (ModalHas(MODAL_ANIMATE))
    {
        if (PTPane pane = ModalFor(MODAL_ANIMATE))
            pane->ComposeTree();
        return;
    }
    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
        if (panes.Used(loop) && panes[loop])
            panes[loop]->ComposeTree();
}

void TScreen::DrawPanes()
{
    if (ModalHas(MODAL_ANIMATE))
    {
        if (PTPane pane = ModalFor(MODAL_ANIMATE))
            pane->DrawTree();
        return;
    }
    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
        if (panes.Used(loop) && panes[loop])
            panes[loop]->DrawTree();
}

void TScreen::RedrawAllPanes()
{
    Display.Reset();

    if (ModalHas(MODAL_ANIMATE))
    {
        PTPane pane = ModalFor(MODAL_ANIMATE);
        if (pane && !pane->IsHidden())
            pane->SetDirty(true);
        Display.Reset();
        return;
    }

    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
        if (panes.Used(loop) && !panes[loop]->IsHidden())
            panes[loop]->SetDirty(true);

    Display.Reset();
}

// *****************************
// * Virtual Handler Functions *
// *****************************

// Each pass below goes to the innermost exclusive pane alone when its entry
// carries the pass's MODAL_* bit, and otherwise to every pane once -- the
// modal included, since it is in the pane array (retail 0x0048fda0,
// 0x0048ff00, 0x00490530, 0x00490660, 0x00490760, 0x00490860).

void TScreen::DrawBackground()
{
    Display.Reset();

    if (ModalHas(MODAL_ANIMATE))
    {
        PTPane pane = ModalFor(MODAL_ANIMATE);
        if (pane && !pane->IsHidden())
        {
            if (dirty)
                pane->Update();
            pane->SetClipRect();
            pane->DrawBackground();
        }
        dirty = false;
        Display.Reset();
        return;
    }

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

// REVSYNC: TScreen pane pulse @ 0x0048fda0. Under MODAL_PAUSE only the modal
// pulses, so a pane-driven world (the map pane, in retail) stands still.
void TScreen::Pulse()
{
    Display.Reset();

    if (ModalHas(MODAL_PAUSE))
    {
        PTPane pane = ModalFor(MODAL_PAUSE);
        if (pane && !pane->IsHidden())
        {
            pane->SetClipRect();
            pane->Pulse();
        }
        Display.Reset();
        return;
    }

    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop) || panes[loop]->IsHidden())
            continue;
        panes[loop]->SetClipRect();
        panes[loop]->Pulse();
    }

    Display.Reset();
}

// REVSYNC: TScreen::Animate @ 0x0048ff00
void TScreen::Animate(bool draw)
{
    Display.Reset();

    if (ModalHas(MODAL_ANIMATE))
    {
        PTPane pane = ModalFor(MODAL_ANIMATE);
        if (pane && !pane->IsHidden())
        {
            pane->SetClipRect();
            pane->Animate(draw);
        }
        Display.Reset();
        return;
    }

    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop) || panes[loop]->IsHidden())
            continue;
        panes[loop]->SetClipRect();
        panes[loop]->Animate(draw);
    }

    Display.Reset();
}

// REVSYNC: 0x00490530 (MODAL_MOUSE). The modal gets the click wherever it
// lands, in its own coordinates; without the bit, a pane gets it when it is
// under the point, and every pane gets a button-up.
void TScreen::MouseClick(int32_t button, int32_t x, int32_t y)
{
    Display.Reset();

    if (ModalHas(MODAL_MOUSE))
    {
        PTPane pane = ModalFor(MODAL_MOUSE);
        if (pane && !pane->IsHidden())
        {
            pane->SetClipRect();
            pane->RouteMouseClick(button, x - pane->GetPosX(), y - pane->GetPosY());
        }
        Display.Reset();
        return;
    }

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
            panes[loop]->RouteMouseClick(button, nx, ny);
    }

    Display.Reset();
}

void TScreen::MouseMove(int32_t button, int32_t x, int32_t y)
{
    Display.Reset();

    if (ModalHas(MODAL_MOUSE))
    {
        PTPane pane = ModalFor(MODAL_MOUSE);
        if (pane && !pane->IsHidden())
        {
            pane->SetClipRect();
            pane->RouteMouseMove(button, x - pane->GetPosX(), y - pane->GetPosY());
        }
        Display.Reset();
        return;
    }

    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop) || panes[loop]->IsHidden() || panes[loop]->IsIgnoringInput())
            continue;

        panes[loop]->SetClipRect();
        int32_t nx = x - panes[loop]->GetPosX();
        int32_t ny = y - panes[loop]->GetPosY();
        panes[loop]->RouteMouseMove(button, nx, ny);
    }

    Display.Reset();
}

// REVSYNC: 0x00490660 (MODAL_KEYS)
void TScreen::KeyPress(int32_t key, bool down)
{
    Display.Reset();

    if (ModalHas(MODAL_KEYS))
    {
        PTPane pane = ModalFor(MODAL_KEYS);
        if (pane && !pane->IsHidden())
            pane->RouteKeyPress(key, down);
        Display.Reset();
        return;
    }

    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop) || panes[loop]->IsHidden() || panes[loop]->IsIgnoringInput())
            continue;
        panes[loop]->SetClipRect();
        panes[loop]->RouteKeyPress(key, down);
    }

    Display.Reset();
}

// REVSYNC: 0x00490760 (MODAL_KEYS)
void TScreen::CharPress(int32_t key, bool down)
{
    Display.Reset();

    if (ModalHas(MODAL_KEYS))
    {
        PTPane pane = ModalFor(MODAL_KEYS);
        if (pane && !pane->IsHidden())
            pane->RouteCharPress(key, down);
        Display.Reset();
        return;
    }

    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop) || panes[loop]->IsHidden() || panes[loop]->IsIgnoringInput())
            continue;
        panes[loop]->SetClipRect();
        panes[loop]->RouteCharPress(key, down);
    }

    Display.Reset();
}

// REVSYNC: 0x00490860 (MODAL_JOYSTICK)
void TScreen::Joystick(int32_t key, bool down)
{
    Display.Reset();

    if (ModalHas(MODAL_JOYSTICK))
    {
        PTPane pane = ModalFor(MODAL_JOYSTICK);
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

    // Modals that called EndModal since the last tick pop now and report
    // their results (the port's equivalent of retail RunModal returning).
    ProcessModalEnds();

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
        // Retail steps the fade right after the screen pulse (0x0048f180).
        if (fade)
            fade->AdvanceTo(double(lastPulseLegacyFrame) * TTime::LegacyFrameSeconds);
    }
}

void TScreen::RequestClose()
{
    done = true;
    if (fade)
        fade->FadeOut();
}

bool TScreen::ReadyToEnd() const
{
    return done && (!fade || !fade->IsBusy() || fade->IsFadedOut());
}

void TScreen::DrawFade()
{
    // Retail's editor runs without a fader (PlayScreen sets one up only
    // outside it, 0x0047b10c). The port's editor opens over a running game
    // and shows it in its own view, so the cover stays off while it's up.
    if (!fade || Editor || CurrentScreen != this || !Renderer)
        return;
    Renderer->FillScreen(0.0f, 0.0f, 0.0f, fade->Opacity(TTime::Time()));
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
    ComposePanes();

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
void TPane::DrawImmediate()
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
    // Children are on screen when their root pane is (only root panes are
    // registered with a screen through AddPane).
    if (parent)
        return parent->IsOnScreen();
    return screen == CurrentScreen;
}
