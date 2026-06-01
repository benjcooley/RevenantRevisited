// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uihudtest.cpp - --test=ui-hud                                        *
// *************************************************************************
//
// Composes the full in-game HUD by delegating to the existing per-panel
// test modes. Each Initialize* sub-call registers its own THudDrawable at
// its correct screen-anchored position; the renderer paints them all every
// frame, so this orchestrator does nothing per-frame beyond the backdrop.
//
// 2026-05-31: Added Inventory pane initialization so page-offset (#7a),
// drag visual (#7b), and arrow gray-out (#7e) are visible in ui-hud
// filmstrips.  Equip pane initialization is owned by the Opus agent.
//
// *************************************************************************

#include "uihudtest.h"

#include "display.h"
#include "logging.h"
#include "renderer.h"
#include "surface.h"

#include "uibarinvtest.h"
#include "uibottombartest.h"
#include "uiinventorytest.h"   // Inventory pane (#7a-#7e — populates harness_inv[])
#include "uiplyrstatusbartest.h"
#include "uiquickspelltest.h"
#include "uisidebartest.h"
#include "uitextbartest.h"

namespace {

bool g_initStatusBar = false;
bool g_initTextBar   = false;
bool g_initBottomBar = false;
bool g_initBarInv    = false;
bool g_initQuickSp   = false;
bool g_initSidebar   = false;
bool g_initInventory = false;   // Inventory pane (populates harness_inv[])

void SafeInit(const char* name, bool& flag, bool (*init)())
{
    if (!init) return;
    flag = init();
    log_info("[ui-hud] %s init = %s", name, flag ? "OK" : "FAIL");
}

void SafeClose(const char* name, bool& flag, void (*close)())
{
    if (!flag || !close) return;
    close();
    flag = false;
    log_info("[ui-hud] %s closed", name);
}

}  // namespace

bool InitializeUIHudMode()
{
    log_info("[ui-hud] === full HUD assembly ===");

    // Anchored to top of screen
    SafeInit("PlyrStatusBar", g_initStatusBar, &InitializeUIPlyrStatusBarMode);

    // Anchored to bottom of screen — three layered pieces
    SafeInit("BottomBar",     g_initBottomBar, &InitializeUIBottomBarMode);
    SafeInit("BarInv",        g_initBarInv,    &InitializeUIBarInvMode);
    SafeInit("QuickSpell",    g_initQuickSp,   &InitializeUIQuickSpellMode);

    // Right-anchored sidebar = tabs + top-slot + bottom-slot chrome
    SafeInit("Sidebar",       g_initSidebar,   &InitializeUISidebarMode);

    // Inventory pane — items + page state + drag-drop source.
    // (Equip pane initialization is owned by the Opus agent via uiequiptest.)
    SafeInit("Inventory",     g_initInventory, &InitializeUIInventoryMode);

    // Overlay (transparent)
    SafeInit("TextBar",       g_initTextBar,   &InitializeUITextBarMode);

    return true;
}

void RenderUIHudMode()
{
    // Each per-panel test mode's RenderUI*Mode does two things:
    //   1) Calls its private Refresh() — rebuilds the panel's internal
    //      TSurface RT so its registered THudDrawable::Draw has something
    //      to blit.
    //   2) Clears the backbuffer to its own isolated-test backdrop color
    //      via Display.BackBuffer()->StartPass(...).
    //
    // We want (1) for every sub-panel but only ONE final backdrop clear
    // (our own). The sub-Renders are called in order — each one's clear
    // is overwritten by the next. Our final StartPass below is the one
    // that survives; all the registered HUDs paint on top of THAT clear.
    if (g_initStatusBar) RenderUIPlyrStatusBarMode();
    if (g_initBottomBar) RenderUIBottomBarMode();
    if (g_initBarInv)    RenderUIBarInvMode();
    if (g_initQuickSp)   RenderUIQuickSpellMode();
    if (g_initSidebar)   RenderUISidebarMode();
    if (g_initInventory) RenderUIInventoryMode();
    if (g_initTextBar)   RenderUITextBarMode();

    // Final backdrop = dark-gray (simulates the empty playfield behind the
    // HUD until the full game scene is composed here).
    Display.BackBuffer()->StartPass(0.15f, 0.16f, 0.18f, 1.0f);
    Display.BackBuffer()->EndPass();
}

void CloseUIHudMode()
{
    log_info("[ui-hud] closing");
    SafeClose("TextBar",       g_initTextBar,   &CloseUITextBarMode);
    SafeClose("Inventory",     g_initInventory, &CloseUIInventoryMode);
    SafeClose("Sidebar",       g_initSidebar,   &CloseUISidebarMode);
    SafeClose("QuickSpell",    g_initQuickSp,   &CloseUIQuickSpellMode);
    SafeClose("BarInv",        g_initBarInv,    &CloseUIBarInvMode);
    SafeClose("BottomBar",     g_initBottomBar, &CloseUIBottomBarMode);
    SafeClose("PlyrStatusBar", g_initStatusBar, &CloseUIPlyrStatusBarMode);
}
