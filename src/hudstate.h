// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  hudstate.h - Serializable HUD state                                  *
// *************************************************************************
//
// Mirror of the retail HUD's persistent state — the modal slot selections,
// the sidebar/HUD open flags, and a few derived display modes that must
// survive Save/Load.  Per user note 2026-05-30: this was a forensics gap,
// the per-panel SPECs did not flag that this state belongs in the savegame.
//
// Retail evidence (from `recon/discovered/FUN_0047cf40_TPlayScreen_DispatchCommand.cpp`):
//   DAT_0065d1b8 — upper-region modal: 0=Equip, 1=Stats, 2=Book (Spellbook)
//   DAT_0065d1bc — lower-region modal: 0=Inv,   1=Map,   2=Spell (composer)
//   DAT_0065d190/0065d194/0065d198 — sidebar visibility flag set (state
//     machine: 0=closed, 1=opening, 2=open; case 7-12 dispatch path
//     differs depending on which flag is set, so all three matter)
//
// Natural pairings (cases 7 / 9 set BOTH slots at once):
//   case 7  — "Spell view"   → upper=2 (Book), lower=2 (Spell)
//   case 9  — "Equip view"   → upper=0 (Equip), lower=0 (Inv)
//   case 8  — "Stats"        → upper=1 (Stats), lower unchanged
//   case 10 — "Spell (open)" → variant of case 7 with FUN_0047c500 path
//   case 11 — "Inv only"     → lower=0 (Inv), upper unchanged
//   case 12 — "Map only"     → lower=1 (Map), upper unchanged
//
// Independent top/bottom slot selection is supported architecturally even
// though retail's keyboard cases tend to pair them; sidebar tab clicks
// route through SetTopSlot/SetBottomSlot directly.
//
// *************************************************************************

#pragma once

#include <cstdint>

enum EHudTopSlot : int32_t {
    HUD_TOP_EQUIP    = 0,
    HUD_TOP_STATS    = 1,
    HUD_TOP_BOOK     = 2,
};

enum EHudBottomSlot : int32_t {
    HUD_BOT_INV      = 0,
    HUD_BOT_MAP      = 1,
    HUD_BOT_SPELL    = 2,
};

enum EHudSidebarState : int32_t {
    HUD_SIDEBAR_CLOSED  = 0,
    HUD_SIDEBAR_OPENING = 1,
    HUD_SIDEBAR_OPEN    = 2,
};

struct SHudState
{
    int32_t topSlot         = HUD_TOP_EQUIP;
    int32_t bottomSlot      = HUD_BOT_INV;
    int32_t sidebarState    = HUD_SIDEBAR_OPEN;
    int32_t bottomBarOpen   = 1;  // case 4 toggle (mbr_0x5b8 in retail)
    int32_t textBarVisible  = 1;
    int32_t statsBarVisible = 1;
    // Inventory page index — driven by the scrollleft/scrollright arrows
    // at pane-local (140,11)/(161,12). 0 = first page. Retail upper bound
    // is 0xf3 per InventoryPane_SPEC `mbr_0x188` analysis.
    int32_t inventoryPage   = 0;
    // Currently-open container: 0 = root inventory; >0 = bag slot index
    // (1 = bag in slot 0, 2 = bag in slot 1, …) — clicked-into-bag stack
    // is single-level for now. Per user 2026-05-30: "Clicking on a bag in
    // the inventory replaces the pack icon with a miniature image of the
    // bag, and the inventory shows the contents of the bag."
    int32_t inventoryContainer = 0;
    // Spellbook scroll offset in pixels — driven by the up/down arrows
    // at pane-local (169, 150) and (169, 174). Per SpellbookPane_SPEC:
    // ±40 per tick (one spell row = ~91px; 40 is a half-row scroll).
    int32_t spellbookScroll = 0;

    static constexpr uint32_t kMagic   = 0x52485644;  // 'RHVD'
    static constexpr uint32_t kVersion = 4;
};

bool SaveHudState(const SHudState& s, const char* path);
bool LoadHudState(SHudState& s, const char* path);

SHudState& GetHudState();
