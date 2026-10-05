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
// State at idle: source == eSrcNone, item empty.
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
//   4. Mouse-up after promotion -> CompleteDrag(dest, dest_slot) moves the
//      item in the main player's inventory, if the destination takes it.
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
// The items are the main player's (`Player`): one inventory holds the
// carried items, the equipment and the belt, told apart by slot number
// (revdefs.h kInvSlot*). A slot index below means:
//   Inventory  carried slot number (page + column*3 + row)
//   BarInv     belt position n (slot 0x10b + n)
//   Equip      EQ_* index (slot 0x100 + index)
//   SpellPane  spell-book row
//
// The manager is the one place a drop mutates the inventory and plays the
// action sound; slots only start drags and name destinations.
//
// *************************************************************************

#pragma once

#include "bitmap.h"    // PTBitmap
#include "cursor.h"    // SDragBitmapLayer
#include "saferef.h"

#include <cstdint>

class TObjectInstance;

enum class EDragSource : int32_t
{
    None       = 0,   // idle
    Inventory  = 1,   // sidebar 4x3 inventory grid
    BarInv     = 2,   // bottom-bar belt
    Equip      = 3,   // sidebar 11-slot paperdoll
    SpellPane  = 4,   // spell book row (no item)
    Playfield  = 5,   // map/world object source or drop destination
};

struct SUIDragState
{
    EDragSource      source     = EDragSource::None;
    int32_t          source_idx = -1;   // slot index, see the banner
    TSafeRef<TObjectInstance> item;     // the in-flight item
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

// Begin a pending grab of `item` from a slot. Returns false when there is
// no item (an empty slot), except for spell-book rows, which carry none.
// `icon` is the drag ghost; by default the item's slot icon.
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

// Drop on (dest, dest_slot). With `commit`, an item drag moves the item in
// the main player's inventory when the destination takes it. Either way
// the drag state returns to idle. Returns true if the drop was committed.
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

} // namespace UIDragState
