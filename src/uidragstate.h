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
//   1. Mouse-down on a slot or playfield item -> BeginDrag(source, index, item)
//      records a pending click/grab, but does not yet remove the item from its
//      slot or show a drag ghost.
//   2. Mouse motion past the wiggle threshold -> UpdateDrag(...) promotes the
//      pending grab to a real drag and attaches the icon at the original
//      click offset.
//   3. Mouse-up before promotion -> CompleteClick() clears the pending grab
//      and leaves use/click behavior distinct from drag/drop.
//   4. Mouse-up after promotion -> CompleteDrag(dest, dest_slot) resolves the
//      transfer and asks the destination policy whether it can accept the item.
//   5. Either way, state returns to idle.
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
// In the production game TPlayer/map ownership own the world, inventory, bar,
// and equipment arrays; the panes read from them. In the isolated --test=ui-hud
// harness no TPlayer
// exists, so we keep a minimal mock here so inventory ↔ equip drag-drop can
// demonstrate committed item moves (#14) without hard-coding per-pane state.
//
// SHarnessSlot describes one "slot" in the mock — which TObjectInstance
// lives there plus the bitmap to display.
//
// Layout:
//   harness_inv[0..11]   — Inventory 4×3 grid, column-major (slot=col*3+row)
//   harness_barinv[0..8] — Bottom-bar quick-use row
//   harness_equip[0..10] — Equip paperdoll, indexed by EQ_* enum value
//
// On CompleteDrag the harness performs:
//   Inventory, BarInv, and Equip slots all swap through the same manager.
//
// Production path: slots initiate drags; TPlayScreen/UIDragState owns the
// transaction. It must handle playfield pickup, inventory/bar/equip drops,
// map drops, use-on-object, replacement/swap/return, and the pickup/drop sound
// effect from the same commit point. The retail snapshot split this across
// TMapPane/TInventory/TEquipPane with TPlayScreen globals; the port keeps the
// policy centralized instead of teaching each slot every possible destination.
//
// *************************************************************************

#pragma once

#include "bitmap.h"    // PTBitmap
#include "cursor.h"    // SDragBitmapLayer

#include <cstdint>

class TObjectInstance;

// Slot counts matching retail geometry constants.
constexpr int32_t kHarnessInvSlots   = 12;   // 4 cols × 3 rows
constexpr int32_t kHarnessBarInvSlots = 9;   // bottom-bar quick shelf
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
    Playfield  = 5,   // map/world object source or drop destination
};

struct SUIDragState
{
    EDragSource      source     = EDragSource::None;
    int32_t          source_idx = -1;   // pane-local cell index (slot number)
    TObjectInstance* item       = nullptr;   // the in-flight item ref
    int32_t          start_x    = 0;    // cursor at mouse-down (for revert)
    int32_t          start_y    = 0;
    int32_t          grab_x     = 0;    // cursor offset within source slot/icon
    int32_t          grab_y     = 0;
    PTBitmap         icon       = nullptr;
    SDragBitmapLayer layers[kMaxDragBitmapLayers] = {};
    int32_t          layer_count = 0;
    bool             pending    = false; // mouse is down, below wiggle threshold
    bool             dragging   = false; // promoted to a real drag/drop
};

namespace UIDragState {

SUIDragState& Get();

// Shared test-harness item state.  Panes read these arrays each Refresh()
// to know which slots have items and what icon to display.  Inventory pane
// populates harness_inv[] in its Initialize; equip pane reads harness_equip[].
// Both are valid to write from any pane — the coordinator is this TU.
extern SHarnessSlot harness_inv  [kHarnessInvSlots];
extern SHarnessSlot harness_barinv[kHarnessBarInvSlots];
extern SHarnessSlot harness_equip[kHarnessEquipSlots];

// Begin a pending slot grab. Returns false for empty harness slots.
// Safe to call when already tracking — overwrites.
// `icon` is the bitmap to show as the drag ghost after the wiggle threshold.
bool BeginDrag(EDragSource src, int32_t slot_idx,
               TObjectInstance* item,
               int32_t mouse_x, int32_t mouse_y,
               PTBitmap icon = nullptr,
               int32_t grab_x = 0, int32_t grab_y = 0);

bool BeginDragLayers(EDragSource src, int32_t slot_idx,
                     TObjectInstance* item,
                     int32_t mouse_x, int32_t mouse_y,
                     const SDragBitmapLayer* layers, int32_t layer_count,
                     int32_t grab_x = 0, int32_t grab_y = 0);

// Promote the pending grab to a real drag once the cursor moves far enough.
// Returns true when the manager is tracking this pointer sequence.
bool UpdateDrag(int32_t mouse_x, int32_t mouse_y);

// Finish a pending click without converting it into a drag/drop.
bool CompleteClick();

// Attempt a drop on the given (pane, slot). `commit` indicates the
// move was accepted. Either way, the drag state returns to idle.
// Performs the harness_inv/harness_equip swap and plays a sound.
// Returns true if a drop was committed.
bool CompleteDrag(EDragSource dest, int32_t dest_slot, bool commit);

// Force-cancel the active drag (e.g. ESC pressed, mouse-up over empty
// area, etc.). No-op when not dragging.
void Cancel();

// Quick predicates. IsActive() and IsTracking() include pending clicks;
// IsDragging() is true only after promotion past the wiggle threshold.
bool IsTracking();
bool IsPending();
bool IsActive();
bool IsDragging();

// Reset all harness slots to empty (call before re-populating at init).
void ResetHarness();
void ResetInventoryHarness();
void ResetBarInvHarness();
void ResetEquipHarness();

} // namespace UIDragState
