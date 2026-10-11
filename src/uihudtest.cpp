// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uihudtest.cpp - --test=ui-hud                                        *
// *************************************************************************
//
// Composes the side of the in-game HUD not yet rebuilt by delegating to the
// existing per-panel test modes. The sidebar owns only tab selection state in
// composed mode; each selected content pane owns and refreshes its own render
// texture. The rebuilt panes are the play screen's own (the status bar, the
// bottom bar, the inventory, the spell pane, the automap); --test=ui-hud
// hosts them beside this
// (testmodes.cpp).
//
// The live Equip paperdoll is embedded when the upper sidebar slot is Equip.
//
// *************************************************************************

#include "uihudtest.h"

#include "bitmapatlas.h"
#include "cursor.h"
#include "display.h"
#include "hudstate.h"
#include "logging.h"
#include "renderer.h"
#include "revenant.h"
#include "surface.h"

#include "uiequiptest.h"
#include "uisidebartest.h"

namespace {

bool g_initSidebar   = false;
bool g_initEquip     = false;
bool g_cursorOverlayRequested = false;
bool g_cursorOverlayRegistered = false;
TCursorHud g_cursorHud;

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

void SetUIHudCursorOverlayEnabled(bool enabled)
{
    g_cursorOverlayRequested = enabled;
}

bool InitializeUIHudMode()
{
    log_info("[ui-hud] === full HUD assembly ===");
    BeginUIBitmapAtlasBuild();
    RegisterUIBitmapAtlasArchive(GameData);

    // Right-anchored side-tab selector. In composed HUD mode the selected
    // content panes below draw their own chrome and dynamic contents.
    SafeInit("Sidebar",       g_initSidebar,   &InitializeUISidebarMode);
    if (g_initSidebar)
        SetUISidebarContentChromeEnabled(false);

    // Content panes. Each owns one render surface and is visibility-gated
    // by SHudState, matching the retail upper/lower region selectors.

    // Equip pane — live 3D paperdoll + equipment slots.
    SafeInit("Equip",         g_initEquip,     &InitializeUIEquipMode);

    RegisterUIBitmapAtlasInventoryImagery();
    const bool atlasOk = BuildUIBitmapAtlas();
    log_info("[ui-hud] shared bitmap atlas = %s", atlasOk ? "OK" : "FAIL");

    if (g_cursorOverlayRequested && Renderer && !g_cursorOverlayRegistered)
    {
        Renderer->AddHud(&g_cursorHud, 1000.0f);
        g_cursorOverlayRegistered = true;
    }

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
    RenderUIHudModeEmbedded();

    // Final backdrop = dark-gray (simulates the empty playfield behind the
    // HUD until the full game scene is composed here).
    Display.BackBuffer()->StartPass(0.15f, 0.16f, 0.18f, 1.0f);
    Display.BackBuffer()->EndPass();
}

void RenderUIHudModeEmbedded()
{
    if (g_initSidebar)   RenderUISidebarModeEmbedded();
    const SHudState& s = GetHudState();
    const bool upperVisible = (s.sidebarState == HUD_SIDEBAR_OPEN);
    if (g_initEquip)
    {
        SetUIEquipModeVisible(upperVisible && s.topSlot == HUD_TOP_EQUIP);
        RenderUIEquipModeEmbedded();
    }
}

void CloseUIHudMode()
{
    log_info("[ui-hud] closing");
    if (g_cursorOverlayRegistered && Renderer)
    {
        Renderer->RemoveHud(&g_cursorHud);
        g_cursorOverlayRegistered = false;
    }
    SafeClose("Equip",         g_initEquip,     &CloseUIEquipMode);
    SafeClose("Sidebar",       g_initSidebar,   &CloseUISidebarMode);
    ClearUIBitmapAtlas();
}
