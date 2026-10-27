// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *    uihudmockuptest.cpp - --test=ui-hud-mockup: full HUD wireframe     *
// *************************************************************************
//
// See uihudmockuptest.h.
//
// *************************************************************************

#include "uihudmockuptest.h"

#include "display.h"
#include "inventory.h"
#include "logging.h"
#include "multictrl.h"
#include "renderer.h"
#include "spellpane.h"     // TQuickSpellPane
#include "statpane.h"
#include "statusbar.h"
#include "textbar.h"
#include "time.h"

#include <cmath>
#include <memory>

namespace {

// All the HUD panes, default-constructed. Their constructors set the
// retail rects from revdefs.h. We intentionally do NOT call Initialize()
// -- that would require GameData (playscrn.dat) and per-class data files;
// the mockup is about layout topology, not content.
std::unique_ptr<TTextBar>       g_textbar;
std::unique_ptr<THealthBar>     g_health;
std::unique_ptr<TStaminaBar>    g_stamina;
std::unique_ptr<TInventory>     g_inventory;
std::unique_ptr<TQuickSpellPane> g_quickspells;
std::unique_ptr<TMultiCtrlPane> g_multictrl;
std::unique_ptr<TStatPane>      g_statpane;     // representative multipane

struct SHudColor
{
    const char* label;
    TPane*      pane;
    uint8_t     r, g, b, a;
};

SHudColor PaneRow(const char* label, TPane* p, uint8_t r, uint8_t g, uint8_t b)
{
    return SHudColor{label, p, r, g, b, 160};
}

void DrawOutlinedFill(TPane* p, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    if (!p) return;
    const int32_t x = p->GetPosX();
    const int32_t y = p->GetPosY();
    const int32_t w = p->GetWidth();
    const int32_t h = p->GetHeight();
    if (w <= 0 || h <= 0) return;

    Renderer->DrawSolidRect(x, y, w, h, r, g, b, a);
    Renderer->DrawSolidRect(x,         y,         w, 1, 230, 230, 230, 255);
    Renderer->DrawSolidRect(x,         y + h - 1, w, 1, 230, 230, 230, 255);
    Renderer->DrawSolidRect(x,         y,         1, h, 230, 230, 230, 255);
    Renderer->DrawSolidRect(x + w - 1, y,         1, h, 230, 230, 230, 255);
}

// Visualizer registered with the HUD pipeline. Iterates the constructed
// panes and paints each with its assigned color so the user sees the
// full retail HUD silhouette.
class THudMockupHud : public THudDrawable
{
public:
    void Draw() override
    {
        // Animate the gauges to demonstrate they're alive on the HUD path,
        // not just static rects.
        if (g_health && g_stamina)
        {
            const double t = TTime::Time();
            g_health ->SetLevel(int32_t(500.0 + 500.0 * std::sin(t * 0.8)));
            g_stamina->SetLevel(int32_t(500.0 + 500.0 * std::sin(t * 0.8 + 1.5)));
        }

        // Status bars draw with a bottom-up fill (gauge style).
        for (TStatusBar* bar : {static_cast<TStatusBar*>(g_health.get()),
                                static_cast<TStatusBar*>(g_stamina.get())})
        {
            if (!bar) continue;
            const int32_t x = bar->GetPosX();
            const int32_t y = bar->GetPosY();
            const int32_t w = bar->GetWidth();
            const int32_t h = bar->GetHeight();
            Renderer->DrawSolidRect(x, y, w, h, 40, 40, 40, 200);
            const int32_t lev = (std::max)(0, (std::min)(bar->GetLevel(), 1000));
            const int32_t fillH = (lev * h) / 1000;
            if (fillH > 0)
            {
                const bool is_health = (bar == g_health.get());
                if (is_health)
                    Renderer->DrawSolidRect(x, y + (h - fillH), w, fillH, 220, 60, 60, 220);
                else
                    Renderer->DrawSolidRect(x, y + (h - fillH), w, fillH, 60, 120, 220, 220);
            }
            Renderer->DrawSolidRect(x,         y,         w, 1, 230, 230, 230, 255);
            Renderer->DrawSolidRect(x,         y + h - 1, w, 1, 230, 230, 230, 255);
            Renderer->DrawSolidRect(x,         y,         1, h, 230, 230, 230, 255);
            Renderer->DrawSolidRect(x + w - 1, y,         1, h, 230, 230, 230, 255);
        }

        // The rest are outlined fills colored by role.
        DrawOutlinedFill(g_textbar.get(),     180, 160,  40, 180);  // textbar  -- yellow
        DrawOutlinedFill(g_inventory.get(),    90,  60,  30, 180);  // inventory -- brown
        DrawOutlinedFill(g_quickspells.get(), 100,  50, 140, 180);  // quick spells -- purple
        DrawOutlinedFill(g_multictrl.get(),    80,  80,  90, 180);  // ctrl buttons -- gray
        DrawOutlinedFill(g_statpane.get(),     60, 120,  60, 180);  // multipane (stat) -- green
    }
};
THudMockupHud g_viz;

void LogPane(const char* label, TPane* p)
{
    if (!p) return;
    log_info("[ui-hud-mockup]   %-12s rect=(x=%d y=%d w=%d h=%d)",
             label, p->GetPosX(), p->GetPosY(), p->GetWidth(), p->GetHeight());
}

}  // namespace

bool InitializeUIHudMockupMode()
{
    log_info("[ui-hud-mockup] === full retail HUD layout wireframe ===");
    log_info("[ui-hud-mockup] (constructors set retail rects from revdefs.h; "
             "Initialize() skipped -- needs GameData)");

    g_textbar     = std::make_unique<TTextBar>();
    g_health      = std::make_unique<THealthBar>();
    g_stamina     = std::make_unique<TStaminaBar>();
    g_inventory   = std::make_unique<TInventory>();
    g_quickspells = std::make_unique<TQuickSpellPane>();
    g_multictrl   = std::make_unique<TMultiCtrlPane>();
    g_statpane    = std::make_unique<TStatPane>();

    LogPane("TextBar",      g_textbar.get());
    LogPane("HealthBar",    g_health.get());
    LogPane("StaminaBar",   g_stamina.get());
    LogPane("Inventory",    g_inventory.get());
    LogPane("QuickSpells",  g_quickspells.get());
    LogPane("MultiCtrl",    g_multictrl.get());
    LogPane("StatPane",     g_statpane.get());

    Renderer->AddHud(&g_viz, 0.0f);
    log_info("[ui-hud-mockup] wireframe registered with renderer HUD pipeline");
    return true;
}

void RenderUIHudMockupMode()
{
    // Slate-gray backdrop -- neutral so the colored panes stand out.
    Display.BackBuffer()->StartPass(0.18f, 0.20f, 0.22f, 1.0f);
    Display.BackBuffer()->EndPass();
}

void CloseUIHudMockupMode()
{
    Renderer->RemoveHud(&g_viz);
    g_statpane.reset();
    g_multictrl.reset();
    g_quickspells.reset();
    g_inventory.reset();
    g_stamina.reset();
    g_health.reset();
    g_textbar.reset();
}
