// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// * uistatusbartest.cpp - --test=ui-statusbars: status-bar pane bring-up  *
// *************************************************************************
//
// See uistatusbartest.h.
//
// *************************************************************************

#include "uistatusbartest.h"

#include "display.h"
#include "logging.h"
#include "renderer.h"
#include "statusbar.h"
#include "time.h"

#include <cmath>
#include <memory>

namespace {

std::unique_ptr<THealthBar>  g_health;
std::unique_ptr<TStaminaBar> g_stamina;

// Visualizer: a THudDrawable that draws each gauge as a filled rect at the
// pane's retail rect, with the filled portion sized to (level / 1000) of
// the pane height. Drains/fills over time so the cycling is visible. This
// stands in for the legacy Display.PutHue tube rendering until that path
// gets ported to a real Renderer-> primitive.
class TStatusBarHud : public THudDrawable
{
public:
    void Draw() override;
};
TStatusBarHud g_viz;

void DrawGauge(TStatusBar* bar, uint8_t r, uint8_t g, uint8_t b)
{
    if (!bar) return;
    const int32_t x = bar->GetPosX();
    const int32_t y = bar->GetPosY();
    const int32_t w = bar->GetWidth();
    const int32_t h = bar->GetHeight();

    // Frame (dim) -- shows where the gauge lives even at level=0.
    Renderer->DrawSolidRect(x, y, w, h, 40, 40, 40, 200);

    // Filled portion grows from the bottom up (retail health tubes
    // fill upward). Level is 0..1000; clamp for safety.
    const int32_t lev = (std::max)(0, (std::min)(bar->GetLevel(), 1000));
    const int32_t fillH = (lev * h) / 1000;
    if (fillH > 0)
        Renderer->DrawSolidRect(x, y + (h - fillH), w, fillH, r, g, b, 220);

    // 1-px outline for definition.
    Renderer->DrawSolidRect(x,         y,         w, 1, 200, 200, 200, 255);
    Renderer->DrawSolidRect(x,         y + h - 1, w, 1, 200, 200, 200, 255);
    Renderer->DrawSolidRect(x,         y,         1, h, 200, 200, 200, 255);
    Renderer->DrawSolidRect(x + w - 1, y,         1, h, 200, 200, 200, 255);
}

void TStatusBarHud::Draw()
{
    // Animate the levels over a slow cycle (~6s period) so the gauges
    // visibly fill and drain. We bypass the in-pane interpolation by
    // calling SetLevel directly each frame -- ChangeLevel would only
    // step toward the target through DrawBackground's loop which we
    // aren't running.
    if (g_health && g_stamina)
    {
        const double t = TTime::Time();
        const int32_t h_lev = int32_t(500.0 + 500.0 * std::sin(t * 1.0));
        const int32_t s_lev = int32_t(500.0 + 500.0 * std::sin(t * 1.0 + 1.5));
        g_health ->SetLevel(h_lev);
        g_stamina->SetLevel(s_lev);
    }
    DrawGauge(g_health.get(),  220,  60,  60);   // red-ish health
    DrawGauge(g_stamina.get(),  60, 120, 220);   // blue-ish stamina
}

void LogBar(const char* tag, TStatusBar* bar)
{
    if (!bar) return;
    log_info("[ui-statusbars] %-15s level=%4d  hue=%3d  rect=(x=%d y=%d w=%d h=%d) dirty=%s open=%s",
             tag, bar->GetLevel(), bar->GetHue(),
             bar->GetPosX(), bar->GetPosY(),
             bar->GetWidth(), bar->GetHeight(),
             bar->IsDirty() ? "YES" : "no",
             bar->IsOpen()  ? "YES" : "no");
}

void Sweep(TStatusBar* bar, const char* tag)
{
    log_info("[ui-statusbars] === %s SetLevel sweep ===", tag);
    for (int32_t lev : {0, 250, 500, 750, 1000})
    {
        bar->SetLevel(lev);
        char label[64];
        std::snprintf(label, sizeof(label), "after SetLevel(%d)", lev);
        LogBar(label, bar);
    }
    // Clamp behavior at extremes (SetLevel clamps to [0,1000]).
    bar->SetLevel(-100);
    LogBar("after SetLevel(-100) (clamps to 0)", bar);
    bar->SetLevel(5000);
    LogBar("after SetLevel(5000) (clamps to 1000)", bar);
    // ChangeLevel sets only targetlevel; observable GetLevel() still
    // shows the current (un-animated) level until DrawBackground walks
    // its HEALTH_INCREMENT steps. We can't visualize that without
    // running DrawBackground, but we can show the API was accepted.
    bar->SetLevel(500);
    bar->ChangeLevel(900);
    LogBar("after ChangeLevel(900) [target=900, level=500 still]", bar);
}

}  // namespace

bool InitializeUIStatusBarMode()
{
    log_info("[ui-statusbars] === THealthBar / TStaminaBar bring-up ===");

    g_health  = std::make_unique<THealthBar>();
    g_stamina = std::make_unique<TStaminaBar>();
    LogBar("health  (construct)",  g_health.get());
    LogBar("stamina (construct)",  g_stamina.get());

    // Initialize loads tubedata from health.dat / stamina.dat via TMulti.
    // Returns false if the asset isn't reachable -- on a real GOG install
    // these are present; on a clean dev tree they may be missing. Log
    // whichever outcome -- both are valid for this test.
    const bool h_ok = g_health->Initialize();
    log_info("[ui-statusbars] THealthBar::Initialize() -> %s",
             h_ok ? "OK (health.dat loaded)" : "tubedata=null (health.dat not reachable)");
    const bool s_ok = g_stamina->Initialize();
    log_info("[ui-statusbars] TStaminaBar::Initialize() -> %s",
             s_ok ? "OK (stamina.dat loaded)" : "tubedata=null (stamina.dat not reachable)");

    LogBar("health  (post-init)", g_health.get());
    LogBar("stamina (post-init)", g_stamina.get());

    Sweep(g_health.get(),  "HEALTH");
    Sweep(g_stamina.get(), "STAMINA");

    log_info("[ui-statusbars] (DrawBackground/visual render uses tubedata + Display.Box -- "
             "ports incrementally with TTextBar)");

    // Wire the visualizer into the HUD pipeline. Renderer->DrawHud() will
    // call its Draw() each frame during the swapchain pass; it animates
    // the gauges and emits DrawSolidRect calls at the panes' retail rects.
    Renderer->AddHud(&g_viz, 0.0f);
    log_info("[ui-statusbars] visualizer registered with renderer HUD pipeline");

    return true;
}

void RenderUIStatusBarMode()
{
    const double t = TTime::Time();
    const float r = 0.16f + 0.04f * float(std::sin(t * 0.6));
    const float g = 0.10f + 0.04f * float(std::sin(t * 0.8 + 1.0));
    const float b = 0.10f + 0.04f * float(std::sin(t * 1.0 + 2.0));
    Display.BackBuffer()->StartPass(r, g, b, 1.0f);
    Display.BackBuffer()->EndPass();
}

void CloseUIStatusBarMode()
{
    Renderer->RemoveHud(&g_viz);
    if (g_stamina) g_stamina->Close();
    if (g_health)  g_health->Close();
    g_stamina.reset();
    g_health.reset();
}
