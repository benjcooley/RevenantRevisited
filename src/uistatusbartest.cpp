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
#include "statusbar.h"
#include "time.h"

#include <cmath>
#include <memory>

namespace {

std::unique_ptr<THealthBar>  g_health;
std::unique_ptr<TStaminaBar> g_stamina;

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
    if (g_stamina) g_stamina->Close();
    if (g_health)  g_health->Close();
    g_stamina.reset();
    g_health.reset();
}
