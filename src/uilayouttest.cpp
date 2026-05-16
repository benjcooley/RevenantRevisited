// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *     uilayouttest.cpp - --test=ui-layout: TPane 2-pass layout proof    *
// *************************************************************************
//
// See uilayouttest.h.
//
// Synthetic tree:
//
//   Root: Vertical, 800x600, padding 16, spacing 8
//   +-- Header:  Fixed h=40,  Greedy w
//   +-- Body:    Greedy h,    Greedy w, weight=1
//   |   +-- BodyLeft:  Greedy w (weight=1), Greedy h, margin.right=4
//   |   +-- BodyRight: Greedy w (weight=2), Greedy h, margin.left=4
//   +-- Footer:  Fixed h=24,  Greedy w
//
// Expected math (window 800x600, padding 16 -> content 768x568):
//
//   Header:   x=16, y=16,                  w=768, h=40
//   Body:     x=16, y=16+40+8=64,          w=768, h=568-40-24-16=488
//             BodyLeft (weight 1 of 3):    w=(768-8)/3=253ish, h=488
//             BodyRight (weight 2 of 3):   w=506ish,            h=488
//   Footer:   x=16, y=16+40+8+488+8=560,   w=768, h=24
//
// log_info each rect on init; the test passes by inspection.
//
// *************************************************************************

#include "uilayouttest.h"

#include "display.h"
#include "logging.h"
#include "screen.h"
#include "time.h"

#include <cmath>
#include <memory>

namespace {

// Bare TPane subclass that just keeps a name so log output is readable.
// No draw override -- the test mode renders a backdrop, not the panes.
class TLayoutTestPane : public TPane
{
public:
    explicit TLayoutTestPane(const char* nm, int32_t x = 0, int32_t y = 0,
                             int32_t w = 0, int32_t h = 0)
      : TPane(x, y, w, h), name(nm) {}

    const char* GetName() const { return name; }

private:
    const char* name = "";
};

std::unique_ptr<TLayoutTestPane> g_root;
std::unique_ptr<TLayoutTestPane> g_header;
std::unique_ptr<TLayoutTestPane> g_body;
std::unique_ptr<TLayoutTestPane> g_bodyLeft;
std::unique_ptr<TLayoutTestPane> g_bodyRight;
std::unique_ptr<TLayoutTestPane> g_footer;

void LogPane(const TLayoutTestPane* p, int32_t indent = 0)
{
    if (!p) return;
    char pad[32] = {};
    for (int32_t i = 0; i < indent && i < 30; ++i) pad[i] = ' ';
    log_info("[ui-layout] %s%-10s x=%d y=%d w=%d h=%d",
             pad, p->GetName(),
             p->GetPosX(), p->GetPosY(),
             p->GetWidth(), p->GetHeight());
    for (TPane* c : p->Children())
        LogPane(static_cast<TLayoutTestPane*>(c), indent + 2);
}

}  // namespace

bool InitializeUILayoutMode()
{
    log_info("[ui-layout] building synthetic tree (root 800x600, padding 16, spacing 8)");

    g_root      = std::make_unique<TLayoutTestPane>("Root",       0, 0, 800, 600);
    g_header    = std::make_unique<TLayoutTestPane>("Header");
    g_body      = std::make_unique<TLayoutTestPane>("Body");
    g_bodyLeft  = std::make_unique<TLayoutTestPane>("BodyLeft");
    g_bodyRight = std::make_unique<TLayoutTestPane>("BodyRight");
    g_footer    = std::make_unique<TLayoutTestPane>("Footer");

    // Root: vertical, padded, spaced.
    g_root->SetLayoutKind(SLayoutKind::Vertical);
    g_root->SetPadding(SSpacing(16));
    g_root->SetSpacing(8);

    // Header: fixed 40 tall, greedy wide.
    g_header->Resize(0, 0, 0, 40);
    g_header->PaneResized();
    g_header->SetSizePolicy(SSizePolicy::Greedy, SSizePolicy::Fixed);

    // Body: greedy both axes; contains a horizontal split.
    g_body->SetSizePolicy(SSizePolicy::Greedy, SSizePolicy::Greedy);
    g_body->SetGreedyWeight(1.0f);
    g_body->SetLayoutKind(SLayoutKind::Horizontal);

    // BodyLeft: greedy weight 1.
    g_bodyLeft->SetSizePolicy(SSizePolicy::Greedy, SSizePolicy::Greedy);
    g_bodyLeft->SetGreedyWeight(1.0f);
    g_bodyLeft->SetMargin(SSpacing(0, 0, 4, 0));

    // BodyRight: greedy weight 2 -- gets twice the share of remaining width.
    g_bodyRight->SetSizePolicy(SSizePolicy::Greedy, SSizePolicy::Greedy);
    g_bodyRight->SetGreedyWeight(2.0f);
    g_bodyRight->SetMargin(SSpacing(4, 0, 0, 0));

    // Footer: fixed 24 tall, greedy wide.
    g_footer->Resize(0, 0, 0, 24);
    g_footer->PaneResized();
    g_footer->SetSizePolicy(SSizePolicy::Greedy, SSizePolicy::Fixed);

    // Wire the tree.
    g_root->AddChild(g_header.get());
    g_root->AddChild(g_body.get());
    g_root->AddChild(g_footer.get());
    g_body->AddChild(g_bodyLeft.get());
    g_body->AddChild(g_bodyRight.get());

    g_root->RunLayoutPass();

    log_info("[ui-layout] post-layout rects:");
    LogPane(g_root.get());

    return true;
}

void RenderUILayoutMode()
{
    // No visual yet -- A.2d 9-slice will give us a real way to draw
    // colored boxes per pane. For now: gently cycling backdrop so we
    // can tell the test mode is running.
    const double t = TTime::Time();
    const float r = 0.10f + 0.05f * float(std::sin(t * 0.7));
    const float g = 0.12f + 0.05f * float(std::sin(t * 0.9 + 1.0));
    const float b = 0.18f + 0.05f * float(std::sin(t * 1.1 + 2.0));
    Display.BackBuffer()->StartPass(r, g, b, 1.0f);
    Display.BackBuffer()->EndPass();
}

void CloseUILayoutMode()
{
    // unique_ptrs unlink themselves via TPane::~TPane (children's parent
    // ptrs nulled, this pane removed from its parent's vector).
    g_footer.reset();
    g_bodyRight.reset();
    g_bodyLeft.reset();
    g_body.reset();
    g_header.reset();
    g_root.reset();
}
