// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *         uicliptest.cpp - --test=ui-clip: TPane clip-rect API          *
// *************************************************************************
//
// See uicliptest.h.
//
// *************************************************************************

#include "uicliptest.h"

#include "display.h"
#include "logging.h"
#include "screen.h"
#include "time.h"

#include <cmath>
#include <memory>

namespace {

class TClipTestPane : public TPane
{
public:
    explicit TClipTestPane(const char* nm, int32_t w, int32_t h)
      : TPane(0, 0, w, h), name(nm) {}
    const char* GetName() const { return name; }
private:
    const char* name = "";
};

std::unique_ptr<TClipTestPane> g_pane;

void LogClip(const char* label)
{
    const SSpacing& fade = g_pane->GetClipFade();
    const bool soft = fade.left || fade.top || fade.right || fade.bottom;
    if (g_pane->HasClipRect())
    {
        SRect r;
        g_pane->GetClipRect(r);
        if (soft)
            log_info("[ui-clip] %-22s has_clip=YES rect=(L%d T%d R%d B%d) fade=(L%d T%d R%d B%d)",
                     label, r.left, r.top, r.right, r.bottom,
                     fade.left, fade.top, fade.right, fade.bottom);
        else
            log_info("[ui-clip] %-22s has_clip=YES rect=(L%d T%d R%d B%d) (hard)",
                     label, r.left, r.top, r.right, r.bottom);
    }
    else
    {
        log_info("[ui-clip] %-22s has_clip=NO", label);
    }
}

}  // namespace

bool InitializeUIClipMode()
{
    log_info("[ui-clip] === TPane clip-rect API contract ===");
    g_pane = std::make_unique<TClipTestPane>("pane", 200, 100);

    LogClip("after construct");

    SRect r1{};
    r1.left = 10; r1.top = 20; r1.right = 110; r1.bottom = 80;
    g_pane->SetClipRect(r1);
    LogClip("after SetClipRect #1");

    SRect r2{};
    r2.left = 50; r2.top = 0; r2.right = 200; r2.bottom = 100;
    g_pane->SetClipRect(r2);
    LogClip("after SetClipRect #2");

    g_pane->ClearClipRect();
    LogClip("after ClearClipRect");

    g_pane->SetClipRect(r1);
    LogClip("after re-set");

    log_info("[ui-clip] === A.2g soft-edge fade ===");
    g_pane->SetClipFade(SSpacing(0, 12, 0, 12));   // soft on top + bottom
    LogClip("after fade T12 B12");

    g_pane->SetClipFade(SSpacing(8, 8, 8, 8));     // soft on all edges
    LogClip("after fade L8 T8 R8 B8");

    g_pane->SetClipFade(SSpacing{});               // reset to hard
    LogClip("after fade reset (hard)");

    log_info("[ui-clip] (renderer-side scissor + fade shader wiring deferred to first B-phase consumer)");
    return true;
}

void RenderUIClipMode()
{
    const double t = TTime::Time();
    const float r = 0.14f + 0.04f * float(std::sin(t * 0.6));
    const float g = 0.18f + 0.04f * float(std::sin(t * 0.8 + 1.0));
    const float b = 0.10f + 0.04f * float(std::sin(t * 1.0 + 2.0));
    Display.BackBuffer()->StartPass(r, g, b, 1.0f);
    Display.BackBuffer()->EndPass();
}

void CloseUIClipMode()
{
    g_pane.reset();
}
