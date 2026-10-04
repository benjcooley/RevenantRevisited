// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uihudtest.cpp - --test=ui-hud                                        *
// *************************************************************************
//
// Composes the full in-game HUD by delegating to the existing per-panel
// test modes. The sidebar owns only tab selection state in composed mode;
// each selected content pane owns and refreshes its own render texture.
//
// 2026-05-31: Added Inventory pane initialization so page-offset (#7a),
// drag visual (#7b), and arrow gray-out (#7e) are visible in ui-hud
// filmstrips.  The live Equip paperdoll is embedded when the upper
// sidebar slot is Equip.
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

#include "uibarinvtest.h"
#include "uibottombartest.h"
#include "uiequiptest.h"
#include "uiinventorytest.h"   // Inventory pane (#7a-#7e — populates harness_inv[])
#include "uimaptest.h"
#include "uiplyrstatusbartest.h"
#include "uiquickspelltest.h"
#include "uisidebartest.h"
#include "uispellbooktest.h"
#include "uispellcreatetest.h"
#include "uistatstest.h"
#include "uitextbartest.h"

namespace {

bool g_initStatusBar = false;
bool g_initTextBar   = false;
bool g_initBottomBar = false;
bool g_initBarInv    = false;
bool g_initQuickSp   = false;
bool g_initSidebar   = false;
bool g_initInventory = false;   // Inventory pane (populates harness_inv[])
bool g_initMap       = false;
bool g_initSpell     = false;
bool g_initStats     = false;
bool g_initBook      = false;
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

    // Anchored to top of screen
    SafeInit("PlyrStatusBar", g_initStatusBar, &InitializeUIPlyrStatusBarMode);

    // Anchored to bottom of screen — three layered pieces
    SafeInit("BottomBar",     g_initBottomBar, &InitializeUIBottomBarMode);
    SafeInit("BarInv",        g_initBarInv,    &InitializeUIBarInvMode);
    SafeInit("QuickSpell",    g_initQuickSp,   &InitializeUIQuickSpellMode);

    // Right-anchored side-tab selector. In composed HUD mode the selected
    // content panes below draw their own chrome and dynamic contents.
    SafeInit("Sidebar",       g_initSidebar,   &InitializeUISidebarMode);
    if (g_initSidebar)
        SetUISidebarContentChromeEnabled(false);

    // Content panes. Each owns one render surface and is visibility-gated
    // by SHudState, matching the retail upper/lower region selectors.
    SafeInit("Stats",         g_initStats,     &InitializeUIStatsMode);
    SafeInit("Spellbook",     g_initBook,      &InitializeUISpellbookMode);
    SafeInit("Inventory",     g_initInventory, &InitializeUIInventoryMode);
    SafeInit("Map",           g_initMap,       &InitializeUIMapMode);
    SafeInit("SpellCreate",   g_initSpell,     &InitializeUISpellCreateMode);

    // Equip pane — live 3D paperdoll + equipment slots.
    SafeInit("Equip",         g_initEquip,     &InitializeUIEquipMode);

    // Overlay (transparent)
    SafeInit("TextBar",       g_initTextBar,   &InitializeUITextBarMode);

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
    if (g_initStatusBar) RenderUIPlyrStatusBarModeEmbedded();
    if (g_initBottomBar) RenderUIBottomBarModeEmbedded();
    if (g_initBarInv)    RenderUIBarInvModeEmbedded();
    if (g_initQuickSp)   RenderUIQuickSpellModeEmbedded();
    if (g_initSidebar)   RenderUISidebarModeEmbedded();
    const SHudState& s = GetHudState();
    const bool upperVisible = (s.sidebarState == HUD_SIDEBAR_OPEN);
    const bool lowerVisible = (s.sidebarState == HUD_SIDEBAR_OPEN);
    if (g_initStats)
    {
        SetUIStatsModeVisible(upperVisible && s.topSlot == HUD_TOP_STATS);
        RenderUIStatsModeEmbedded();
    }
    if (g_initBook)
    {
        SetUISpellbookModeVisible(upperVisible && s.topSlot == HUD_TOP_BOOK);
        RenderUISpellbookModeEmbedded();
    }
    if (g_initInventory)
    {
        SetUIInventoryModeVisible(lowerVisible && s.bottomSlot == HUD_BOT_INV);
        RenderUIInventoryModeEmbedded();
    }
    if (g_initMap)
    {
        SetUIMapModeVisible(lowerVisible && s.bottomSlot == HUD_BOT_MAP);
        RenderUIMapModeEmbedded();
    }
    if (g_initSpell)
    {
        SetUISpellCreateModeVisible(lowerVisible && s.bottomSlot == HUD_BOT_SPELL);
        RenderUISpellCreateModeEmbedded();
    }
    if (g_initEquip)
    {
        SetUIEquipModeVisible(upperVisible && s.topSlot == HUD_TOP_EQUIP);
        RenderUIEquipModeEmbedded();
    }
    if (g_initTextBar)   RenderUITextBarModeEmbedded();
}

void CloseUIHudMode()
{
    log_info("[ui-hud] closing");
    if (g_cursorOverlayRegistered && Renderer)
    {
        Renderer->RemoveHud(&g_cursorHud);
        g_cursorOverlayRegistered = false;
    }
    SafeClose("TextBar",       g_initTextBar,   &CloseUITextBarMode);
    SafeClose("Equip",         g_initEquip,     &CloseUIEquipMode);
    SafeClose("SpellCreate",   g_initSpell,     &CloseUISpellCreateMode);
    SafeClose("Map",           g_initMap,       &CloseUIMapMode);
    SafeClose("Inventory",     g_initInventory, &CloseUIInventoryMode);
    SafeClose("Spellbook",     g_initBook,      &CloseUISpellbookMode);
    SafeClose("Stats",         g_initStats,     &CloseUIStatsMode);
    SafeClose("Sidebar",       g_initSidebar,   &CloseUISidebarMode);
    SafeClose("QuickSpell",    g_initQuickSp,   &CloseUIQuickSpellMode);
    SafeClose("BarInv",        g_initBarInv,    &CloseUIBarInvMode);
    SafeClose("BottomBar",     g_initBottomBar, &CloseUIBottomBarMode);
    SafeClose("PlyrStatusBar", g_initStatusBar, &CloseUIPlyrStatusBarMode);
    ClearUIBitmapAtlas();
}
