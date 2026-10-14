// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uidragstate.h - Cross-pane drag-state singleton                      *
// *************************************************************************
//
// Singleton owned by TPlayScreen in retail (per the pane specs in
// docs/ui/forensics/: `cls_0x5a5320_TPlayScreen` is the sole writer of
// `DAT_0065b878` + friends across all sidebar / barinv / equip panes; P4
// measures it). In the port we keep an identical-shaped singleton in its
// own TU so any pane / TInvSlot can read it without pulling in
// TPlayScreen's whole header.
//
// State at idle: source == eSrcNone, item empty.
//
// Lifecycle:
//   1. Mouse-down on a slot or playfield item -> BeginDrag(source, index, item)
//      records a pending click/grab, but does not yet remove the item from its
//      slot or show a drag ghost (the doll's grab is a drag at once).
//   2. Mouse motion past the wiggle threshold -> UpdateDrag(...) promotes the
//      pending grab to a real drag, attaches the icon at the original click
//      offset and holds the item (SetDragObj).
//   3. Mouse-up -> Release(dest, dest_slot) does what retail's item panes do
//      with it: a click uses the item, a drag uses the item it lands on with
//      it, moves it, equips it or takes it off. A spell is dropped through
//      CompleteDrag.
//   4. Either way, state returns to idle.
//
// Retail-cited fields (per the pane specs):
//   DAT_0065b878 ↔ source_slot      currently-dragged EQ-slot index
//   (DAT_0065b088 is not drag state: it is TBarInvPane +0x60, the shelf's
//   container, which the main player sets as it becomes main.)
//   DAT_00667fcc ↔ default_player   default Player target (unchanged here)
//   DAT_00668518 ↔ mouse_down_flag  Win32 WndProc mouse-down latch
//   mbr_0x180..0x194 (per-pane)    starting slot / start XY / drag-active flag
//                                  — we hold the cross-pane subset here, the
//                                  per-pane local state stays on the pane.
//
// The belt's and the doll's items are the main player's (`Player`): one
// inventory holds his carried items, his equipment and his belt, told apart
// by slot number (revdefs.h kInvSlot*). The inventory pane's are those of
// the container it shows: his, or a bag he opened. A slot index below means:
//   Inventory  slot number in the shown container (page + column*3 + row)
//   BarInv     belt position n (slot 0x10b + n)
//   Equip      EQ_* index (slot 0x100 + index)
//   SpellPane  spell-book row (the spell itself is `spell`: retail's drag
//              payload 0x00656398, the talisman code)
//   QuickSpell quick-spell ring 1..4 (a destination only: spells drop there)
//
// The manager is the one place a click, a drop or a right click acts on an
// item (docs/gameflow/forensics/INVENTORY_ACTIONS.md); the panes only start
// grabs and name where the button came up. A spell drop moves nothing: the
// ring it lands on copies the spell, silently.
//
// *************************************************************************

#pragma once

#include "bitmap.h"    // PTBitmap
#include "cursor.h"    // SDragBitmapLayer
#include "saferef.h"

#include <cstdint>
#include <string>

class TObjectInstance;

enum class EDragSource : int32_t
{
    None       = 0,   // idle
    Inventory  = 1,   // sidebar 4x3 inventory grid
    BarInv     = 2,   // bottom-bar belt
    Equip      = 3,   // sidebar 11-slot paperdoll
    SpellPane  = 4,   // spell book row (no item)
    Playfield  = 5,   // map/world object source or drop destination
    QuickSpell = 6,   // bottom-bar quick-spell ring (a spell's destination)
};

struct SUIDragState
{
    EDragSource      source     = EDragSource::None;
    int32_t          source_idx = -1;   // slot index, see the banner
    TSafeRef<TObjectInstance> item;     // the in-flight item
    TSafeRef<TObjectInstance> container; // the inventory the source pane showed
    int32_t          start_x    = 0;    // cursor at mouse-down (for revert)
    int32_t          start_y    = 0;
    int32_t          grab_x     = 0;    // cursor offset within source slot/icon
    int32_t          grab_y     = 0;
    std::string      spell;             // a spell-book drag's talisman code
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

// A spell picked up from spell-book row `row`: a drag at once, with no
// threshold (retail 0x00546390 starts it on the first move), carrying the
// talisman code; `icon` follows the pointer at its point (grab_x, grab_y).
bool BeginSpellDrag(int32_t row, const char* talismans, PTBitmap icon,
                    int32_t mouse_x, int32_t mouse_y, int32_t grab_x, int32_t grab_y);

// Promote the pending grab to a real drag once the cursor moves far enough.
// Returns true when the manager is tracking this pointer sequence.
bool UpdateDrag(int32_t mouse_x, int32_t mouse_y);

// The left button came up over cell `dest_slot` of item pane `dest`
// (Inventory, BarInv; anywhere on the Equip pane, whose slot is ignored), or
// over none of them (EDragSource::None), with an item grabbed in one of
// them: a click (no drag) uses the item, a drag onto another item uses that
// one with it, and what isn't used moves, comes off or is equipped. The grab
// ends either way.
void Release(EDragSource dest, int32_t dest_slot);

// The left button came up on the inventory pane's container icon: up to the
// inventory the shown bag is in, or the item grabbed in the inventory up
// into it. Ends a grab of an item pane.
void ReleaseOnContainerIcon();

// The right button came up over an item pane's cell with nothing grabbed:
// `item` (null for an empty cell) of `container`. A talisman goes into the
// Spell Pouch, an item is equipped, an empty cell closes up the container.
void RightClick(TObjectInstance* container, TObjectInstance* item);

// A spell-book drag let go on (dest, dest_slot); `commit` when the
// destination took the spell. The drag state returns to idle. Returns true
// if the drop was committed.
bool CompleteDrag(EDragSource dest, int32_t dest_slot, bool commit);

// Equip a carried item in its own equipment slot (its EqSlot stat), as a
// drop on the equip pane or a right click on the item does. A slot of -1
// puts EQUIPUNABLE on the text bar. Returns true if the item was equipped.
bool EquipInOwnSlot(TObjectInstance* item);

// Force-cancel the active drag (e.g. ESC pressed, mouse-up over empty
// area, etc.). No-op when not dragging.
void Cancel();

// Ends what is still tracked when the button comes up and no pane took the
// drop: its item stays where it was. The screen that owns the drag calls it
// after routing the button-up.
void ReleaseUnclaimed();

// The item grabbed in the inventory, the belt or the doll, else null. The
// map reads it when the button comes up over it (retail's map pane reads the
// item panes' held slots, 0x0044fce3, and gets the release first).
[[nodiscard]] TObjectInstance* HeldItem();

// Quick predicates. IsActive() includes a grab not yet dragged;
// IsDragging() is true only after promotion past the wiggle threshold.
bool IsActive();
bool IsDragging();

} // namespace UIDragState
