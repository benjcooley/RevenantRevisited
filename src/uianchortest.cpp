// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   uianchortest.cpp - --test=ui-anchors: TPane anchor positioning      *
// *************************************************************************
//
// See uianchortest.h. The test exercises the same anchor mechanism that
// Revisited 1920x1080 HUD layout (project-resolution-modes, A.2h) will
// use to follow window resize events.
//
// *************************************************************************

#include "uianchortest.h"

#include "display.h"
#include "logging.h"
#include "screen.h"
#include "time.h"

#include <cmath>
#include <memory>

namespace {

class TAnchorTestPane : public TPane
{
public:
    explicit TAnchorTestPane(const char* nm, int32_t w, int32_t h)
      : TPane(0, 0, w, h), name(nm) {}
    const char* GetName() const { return name; }
private:
    const char* name = "";
};

std::unique_ptr<TAnchorTestPane> g_root;
std::unique_ptr<TAnchorTestPane> g_topLeft;
std::unique_ptr<TAnchorTestPane> g_topRight;
std::unique_ptr<TAnchorTestPane> g_bottomLeft;
std::unique_ptr<TAnchorTestPane> g_bottomRight;
std::unique_ptr<TAnchorTestPane> g_center;

void LogTree(const TAnchorTestPane* p, int32_t indent = 0)
{
    if (!p) return;
    char pad[32] = {};
    for (int32_t i = 0; i < indent && i < 30; ++i) pad[i] = ' ';
    log_info("[ui-anchors] %s%-14s x=%4d y=%4d w=%4d h=%4d",
             pad, p->GetName(),
             p->GetPosX(), p->GetPosY(),
             p->GetWidth(), p->GetHeight());
    for (TPane* c : p->Children())
        LogTree(static_cast<const TAnchorTestPane*>(c), indent + 2);
}

void RunAtCanvas(int32_t w, int32_t h, const char* label)
{
    log_info("[ui-anchors] === %s: canvas %dx%d ===", label, w, h);
    g_root->Resize(0, 0, w, h);
    g_root->PaneResized();
    g_root->RunLayoutPass();
    LogTree(g_root.get());
}

}  // namespace

bool InitializeUIAnchorMode()
{
    log_info("[ui-anchors] building tree: root + 5 anchored children (4 corners + center)");

    g_root        = std::make_unique<TAnchorTestPane>("Root",         640, 480);
    g_topLeft     = std::make_unique<TAnchorTestPane>("TopLeft",      120, 32);
    g_topRight    = std::make_unique<TAnchorTestPane>("TopRight",     120, 32);
    g_bottomLeft  = std::make_unique<TAnchorTestPane>("BottomLeft",   140, 40);
    g_bottomRight = std::make_unique<TAnchorTestPane>("BottomRight",  140, 40);
    g_center      = std::make_unique<TAnchorTestPane>("Center",       200, 60);

    // Root is None-layout -- it positions anchored children, doesn't flex.
    // (default layoutKind, just being explicit.)
    g_root->SetLayoutKind(SLayoutKind::None);

    g_topLeft    ->SetAnchor(SAnchor::TopLeft);
    g_topRight   ->SetAnchor(SAnchor::TopRight);
    g_bottomLeft ->SetAnchor(SAnchor::BottomLeft);
    g_bottomRight->SetAnchor(SAnchor::BottomRight);
    g_center     ->SetAnchor(SAnchor::Center);

    // Give a couple a margin so we can see the inset-from-edge behavior.
    g_topRight   ->SetMargin(SSpacing(0, 4, 8, 0));
    g_bottomLeft ->SetMargin(SSpacing(8, 0, 0, 4));

    g_root->AddChild(g_topLeft.get());
    g_root->AddChild(g_topRight.get());
    g_root->AddChild(g_bottomLeft.get());
    g_root->AddChild(g_bottomRight.get());
    g_root->AddChild(g_center.get());

    RunAtCanvas(640,  480,  "retail 4:3");
    RunAtCanvas(1920, 1080, "revisited 16:9");
    RunAtCanvas(3840, 2160, "HiDPI 4K");

    return true;
}

void RenderUIAnchorMode()
{
    const double t = TTime::Time();
    const float r = 0.10f + 0.05f * float(std::sin(t * 0.7));
    const float g = 0.16f + 0.05f * float(std::sin(t * 0.9 + 1.0));
    const float b = 0.20f + 0.05f * float(std::sin(t * 1.1 + 2.0));
    Display.BackBuffer()->StartPass(r, g, b, 1.0f);
    Display.BackBuffer()->EndPass();
}

void CloseUIAnchorMode()
{
    g_center.reset();
    g_bottomRight.reset();
    g_bottomLeft.reset();
    g_topRight.reset();
    g_topLeft.reset();
    g_root.reset();
}
