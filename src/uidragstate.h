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
// *************************************************************************

#pragma once

#include <cstdint>

class TObjectInstance;

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

// Begin a drag from a pane's slot. Records the item + source for the
// duration. Safe to call when already dragging — overwrites.
void BeginDrag(EDragSource src, int32_t slot_idx,
               TObjectInstance* item,
               int32_t mouse_x, int32_t mouse_y);

// Attempt a drop on the given (pane, slot). Caller has already checked
// CanAcceptDrop on the destination's TInvSlot. `commit` indicates the
// move was accepted. Either way, the drag state returns to idle.
//
// Returns true if a drop actually committed (i.e. there WAS an active
// drag AND the caller said commit). Pure status.
bool CompleteDrag(EDragSource dest, int32_t dest_slot, bool commit);

// Force-cancel the active drag (e.g. ESC pressed, mouse-up over empty
// area, etc.). No-op when not dragging.
void Cancel();

// Quick predicate.
bool IsActive();

} // namespace UIDragState
