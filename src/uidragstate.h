// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uidragstate.h - Cross-pane drag-state singleton                      *
// *************************************************************************
//
// Singleton owned by TPlayScreen in retail (per the recon trail captured
// in src/invslot.h's header banner: `cls_0x5a5320_TPlayScreen` is the
// sole writer of `DAT_0065b878` + friends across all sidebar / barinv /
// equip panes). In the port we keep an identical-shaped singleton in
// its own TU so any pane / TInvSlot can read it without pulling in
// TPlayScreen's whole header.
//
// State at idle: source == eSrcNone, item == nullptr.
//
// Lifecycle:
//   1. Mouse-down on a slot with an item → BeginDrag(pane, slot, item)
//   2. (mouse moves; the dragged-item ghost can follow cursorx/cursory)
//   3. Mouse-up over a slot → CompleteDrag(dest_pane, dest_slot) — pane
//      checks CanAcceptDrop first; if false the drag is cancelled.
//   4. Either way, state returns to idle.
//
// Retail-cited fields (per invslot.h banner):
//   DAT_0065b878 ↔ source_slot      currently-dragged EQ-slot index
//   DAT_0065b088 ↔ source_obj       drag source character ref
//   DAT_00667fcc ↔ default_player   default Player target (unchanged here)
//   DAT_00668518 ↔ mouse_down_flag  Win32 WndProc mouse-down latch
//   mbr_0x180..0x194 (per-pane)    starting slot / start XY / drag-active flag
//                                  — we hold the cross-pane subset here, the
//                                  per-pane local state stays on the pane.
//
// =========================================================================
// SHARED ITEM STATE (test-harness mock, #7b / #7c / #7d / #14)
// =========================================================================
//
// In the production game TPlayer owns the inventory and equipment arrays;
// the panes read from it.  In the isolated --test=ui-hud harness no TPlayer
// exists, so we keep a minimal mock here so inventory ↔ equip drag-drop can
// demonstrate committed item moves (#14) without hard-coding per-pane state.
//
// SHarnessSlot describes one "slot" in the mock — which TObjectInstance
// lives there plus the bitmap to display.
//
// Layout:
//   harness_inv[0..11]   — Inventory 4×3 grid, column-major (slot=col*3+row)
//   harness_equip[0..10] — Equip paperdoll, indexed by EQ_* enum value
//
// On CompleteDrag the harness performs:
//   Inv→Inv   : swap harness_inv[src]   ↔ harness_inv[dst]
//   Inv→Equip : move harness_inv[src]   → harness_equip[dst], src cleared
//   Equip→Inv : move harness_equip[src] → harness_inv[dst],   src cleared
//   Equip→Equip: swap harness_equip[src]↔ harness_equip[dst]
//
// Production path: swap logic lives in TPlayScreen / TPlayer::Equip /
// TPlayer::Unequip / TPlayer::SwapInventory (retail src/player.cpp,
// src/equip.cpp).  The harness mock is NOT compiled into release builds.
//
// *************************************************************************

#pragma once

#include "bitmap.h"    // PTBitmap

#include <cstdint>

class TObjectInstance;

// Slot counts matching retail geometry constants.
constexpr int32_t kHarnessInvSlots   = 12;   // 4 cols × 3 rows
constexpr int32_t kHarnessEquipSlots = 11;   // NUM_EQ_SLOTS

// One "slot" in the mock inventory / equip state used by the test harness.
struct SHarnessSlot
{
    TObjectInstance* inst      = nullptr;   // null = empty slot
    PTBitmap         icon      = nullptr;   // current display bitmap
    int32_t          qty       = 1;         // stack count (>1 → count text)
    bool             is_bag    = false;     // true = Pouch container
    PTBitmap         bag_inner = nullptr;   // if is_bag: icon of first inner item
    const char*      label     = nullptr;   // debug name
};

enum class EDragSource : int32_t
{
    None       = 0,   // idle
    Inventory  = 1,   // sidebar 4x3 inventory grid
    BarInv     = 2,   // bottom-bar 9-slot quick shelf
    Equip      = 3,   // sidebar 11-slot paperdoll
    SpellPane  = 4,   // sidebar talisman composer (future)
};

struct SUIDragState
{
    EDragSource      source     = EDragSource::None;
    int32_t          source_idx = -1;   // pane-local cell index (slot number)
    TObjectInstance* item       = nullptr;   // the in-flight item ref
    int32_t          start_x    = 0;    // cursor at mouse-down (for revert)
    int32_t          start_y    = 0;
};

namespace UIDragState {

SUIDragState& Get();

// Shared test-harness item state.  Panes read these arrays each Refresh()
// to know which slots have items and what icon to display.  Inventory pane
// populates harness_inv[] in its Initialize; equip pane reads harness_equip[].
// Both are valid to write from any pane — the coordinator is this TU.
extern SHarnessSlot harness_inv  [kHarnessInvSlots];
extern SHarnessSlot harness_equip[kHarnessEquipSlots];

// Begin a drag from a pane's slot. Records the item + source for the
// duration. Safe to call when already dragging — overwrites.
// `icon` is the bitmap to show as the drag ghost under the cursor.
void BeginDrag(EDragSource src, int32_t slot_idx,
               TObjectInstance* item,
               int32_t mouse_x, int32_t mouse_y,
               PTBitmap icon = nullptr);

// Attempt a drop on the given (pane, slot). `commit` indicates the
// move was accepted. Either way, the drag state returns to idle.
// Performs the harness_inv/harness_equip swap and plays a sound.
// Returns true if a drop was committed.
bool CompleteDrag(EDragSource dest, int32_t dest_slot, bool commit);

// Force-cancel the active drag (e.g. ESC pressed, mouse-up over empty
// area, etc.). No-op when not dragging.
void Cancel();

// Quick predicate.
bool IsActive();

// Reset all harness slots to empty (call before re-populating at init).
void ResetHarness();

} // namespace UIDragState
