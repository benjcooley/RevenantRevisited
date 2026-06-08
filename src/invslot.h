// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  invslot.h - shared item-cell sub-control (TInvSlot : TButton)        *
// *************************************************************************
//
// ONE per-cell sub-control covering every inventory-style slot in the HUD:
//   - TBarInv      (bottom-bar 9-slot shelf)
//   - TInventory   (sidebar 4x3 grid)
//   - TEquipPane   (paperdoll 11 EQ_* slots)
//
// Each pane owns N×TInvSlot, configured per-instance via the slot rect +
// `allowedType` filter + `emptyPlaceholder` art + style flags. The slot
// handles paint, hit-test, and "start/accept" policy. UIDragState/TPlayScreen
// owns the drag transaction itself: playfield pickup, inventory/bar/equip
// transfer, map drops, swaps/replacements, returns, and the drag/drop sound.
// Differences between consumers are data (config), not branching code.
//
// Naming follows the user's vocabulary ("common inv slot class") and the
// snapshot's per-cell-as-sub-control precedent (TTalismanButton :
// TButton — src/spellpane.h:23). Equip + Inventory duplicate `grabslot`/
// `heldslot`/`OnSlot`/`DrawAnim` independently in the 1998 snapshot —
// THAT inline-state duplication is the evidence justifying one shared
// abstraction; the snapshot has no shared item-cell class. Per
// [[feedback-evolve-dont-replace]] we evolve the existing TButton with
// this sub-control rather than build a parallel module.
//
// ========================================================================
// RETAIL PROTOCOL — what this class mirrors (forensics summary)
// ========================================================================
//
// MouseClick dispatch (the retail panes share one unified shape — verified
// directly from `cls_0x5a58c0::virt_meth_0x538210` + binary disasm of
// `cls_0x5a5320_TPlayScreen::virt_meth_0x44f140`):
//   void MouseClick(this, int eventType, int paramA, int paramB)
//     - eventType == 1 → mouse-DOWN (record grabbed slot + click pos)
//     - eventType == 4 → mouse-UP   (commit drop or revert)
//     - eventType == 5 → right-click / use (dispatched to Use/Equip)
//
// Per-pane drag state (cls_0x5a58c0 InventorySidebar):
//   mbr_0x180 = grabbed slot index (-1 idle)
//   mbr_0x184 = starting slot (same at click-down)
//   mbr_0x188 = page offset
//   mbr_0x18c = drag-active flag (set after motion threshold)
//   mbr_0x190, 0x194 = click-down x, y
//
// Cross-pane drag state (globals — read by every pane during drop):
//   DAT_00668518 = mouse-button-down flag (Win32 WndProc-maintained)
//   DAT_00668510, DAT_00668514 = current mouse x, y
//   DAT_0065b878 = currently-dragged EQ-slot index (-1 idle) — written
//                   by TEquipPane on drag-start, READ by every other
//                   pane during drop (14 read sites across InvSidebar /
//                   TBarInvPane / others — verified in _data.txt XREFs)
//   DAT_0065b088, 0065b090 = drag source character + slot reference
//   DAT_0066829c = swap-allowed flag
//   DAT_00667fcc = the Player global (default drag target)
//
// The cross-pane drag-state OWNER is `cls_0x5a5320_TPlayScreen` itself
// (per its `virt_meth_0x44f140` — 12 read/write XREFs to the globals
// above). TPlayScreen dispatches MouseClicks to its child panes and
// maintains the global drag state across them. In the port, this maps
// to a singleton `UIDragState` accessed through TPlayScreen.
//
// On drop (`MouseClick eventType==4`):
//   - call `meth_0x4701f0(slot)` (= TPlayer::GetInventory(slot)) on both
//     source and destination panes to fetch the items
//   - if dest is a pouch and source can be put in it, call
//     `[container+0x58](destItem, slot)` (= AddToInventory)
//   - otherwise swap via `meth_0x519300` / `meth_0x5199b0`
//   - common helper `FUN_00538e40` re-syncs container art surfaces
//
// On use (`MouseClick eventType==5`):
//   - if item is a pouch → recurse into container
//   - if item is equipment (eqslot >= 0) → call `meth_0x5199b0(player,
//     item)` to equip into the appropriate paperdoll slot
//   - otherwise call `[item+vtbl]->Use(player)` (the item's own Use)
//
// Full production drag/drop is a manager concern, not a slot concern. The API
// below lets panes route slot events through UIDragState; the manager remains
// the only place that should mutate Player/map ownership or play action audio.
//
// ========================================================================
// FONT/COLOR MATRIX (per-consumer visual conventions)
// ========================================================================
//
// | Cell text type         | Color | Position           | Consumer       |
// |------------------------|-------|--------------------|----------------|
// | Gold readout           | gold  | top-strip (panel)  | Inventory only |
// |   (bitmap font "Gold")                                                |
// | Regular item qty (>1)  | RED   | top-right of cell  | Inventory      |
// | Regular item qty (>1)  | WHITE | top-left of cell   | BarInv         |
// | Bag-contents count     | WHITE | BOTTOM-CENTER      | Inventory      |
// |   (centered on bag's bottom edge)                                     |
// | Pouch quantity         | WHITE | left of cell       | BarInv         |
//
// Encoded as data on `SInvSlotStyle`; the slot's Draw() consumes it
// without per-consumer branches.
// *************************************************************************

#pragma once

#include "font.h"      // ETextAlign + SFontAtlas + DrawTextShadowedToTarget
#include "bitmap.h"    // PTBitmap
#include "surface.h"   // TSurface

#include <cstdint>

class TObjectInstance;

constexpr int32_t kInvSlotAcceptAny = -1;

// ----------------------------------------------------------------------
// Per-cell kind tag. Set by the pane when binding an item into a slot.
// ----------------------------------------------------------------------
enum class EInvSlotKind : int32_t
{
    Empty   = 0,     // no item — paint the placeholder (if configured)
    Regular,         // single-or-stacked item (potion/weapon/food/etc.)
    Pouch,           // bag/pack/pouch — inner-contents count + overlay
};

// ----------------------------------------------------------------------
// Per-consumer visual conventions. Each pane builds this once at init
// and shares it across its slots (or per-slot if needed).
// ----------------------------------------------------------------------
struct SInvSlotStyle
{
    // ----- icon stamp options ---------------------------------------
    // If true, fit the icon to the cell interior (stretch into a box)
    // — BarInv's "icon stretched into 41x41 box" convention.
    // If false, stamp the icon at its native size at the cell TL —
    // retail Inventory behavior (FUN_004bd680(cellX, cellY, ...) has
    // no size args — see InventoryPane_SPEC.md §0 LIVE-VERIFIED #5).
    bool       icon_fit_to_cell  = false;
    int32_t    icon_fit_inset    = 0;     // pixel inset when fitting

    // ----- regular-item quantity text (qty > 1, NOT pouches) --------
    bool       draw_qty          = true;
    float      qty_r             = 1.0f;
    float      qty_g             = 1.0f;
    float      qty_b             = 1.0f;
    ETextAlign qty_align         = ETextAlign::Right;
    int32_t    qty_rect_dx       = 0;
    int32_t    qty_rect_dy       = 0;
    int32_t    qty_rect_w        = 40;
    int32_t    qty_rect_h_pad    = 0;

    // ----- pouch-contents count (WHITE, centered on bag bottom) -----
    bool       draw_bag_count    = false;
    float      bag_r             = 1.0f;
    float      bag_g             = 1.0f;
    float      bag_b             = 1.0f;
    ETextAlign bag_align         = ETextAlign::Center;
    int32_t    bag_rect_dx       = 0;
    int32_t    bag_rect_dy       = 26;
    int32_t    bag_rect_w        = 40;
    int32_t    bag_rect_h_pad    = 0;

    // ----- pouch overlay icon (the bag drawn on top of the cell) ----
    bool       pouch_overlay_stretch = false;
    int32_t    pouch_inner_dx    = 0;
    int32_t    pouch_inner_dy    = 20;
    int32_t    pouch_inner_w     = 20;
    int32_t    pouch_inner_h     = 20;
};

// ----------------------------------------------------------------------
// MouseClick event-type discriminator (mirrors retail eventType).
// ----------------------------------------------------------------------
enum class EInvSlotEvent : int32_t
{
    MouseDown   = 1,    // retail eventType == 1
    MouseUp     = 4,    // retail eventType == 4 (commit drop or revert)
    UseOrEquip  = 5,    // retail eventType == 5 (right-click / use)
    HoverEnter,
    HoverLeave,
};

// ----------------------------------------------------------------------
// TInvSlot — the per-cell sub-control. One instance per slot; the pane
// owns N×TInvSlot, calls Draw() inside its compose pass and routes its
// MouseClick virtual through the slot's HandleEvent.
//
// Constructor signature mirrors TButton's (bname/x/y/w/h) — TInvSlot
// inherits the TButton hit-test, dirty-flag, and visibility machinery
// for free. TButton's keypress/buttonfunc/down/toggle members are unused
// on inv slots (the slot's behavior is item-driven, not key-bound),
// but the base class is small enough that the extra members are
// negligible and the inheritance keeps the snapshot's button-as-control
// vocabulary intact.
//
// NOTE: This class currently does NOT inherit from TButton because the
// TButton header pulls in screen.h/bitmap.h transitively and the test
// modes use TInvSlot as a free-standing renderer. The class is shaped
// to be promoted to `class TInvSlot : public TButton` once the pane
// integration wave wires it through the existing TButtonPane click
// dispatch. The member layout + virtuals match TButton's expected
// pattern (Draw() override, dirty flag, hit rect) so the promotion is
// mechanical.
// ----------------------------------------------------------------------
class TInvSlot
{
public:
    // Construct + configure a slot. The slot keeps a const-ref to the
    // shared style table the pane owns.
    TInvSlot(int32_t x, int32_t y, int32_t w, int32_t h,
             int32_t allowed_type        = kInvSlotAcceptAny,
             PTBitmap empty_placeholder  = nullptr,
             const SInvSlotStyle* style  = nullptr);

    // ----- content binding (pane sets this each frame or on change) -
    void SetItem(TObjectInstance* item, PTBitmap icon, int32_t qty);
    void SetPouchOverlay(PTBitmap inner);

    // ----- accessors --------------------------------------------------
    int32_t          PosX()        const { return x_; }
    int32_t          PosY()        const { return y_; }
    int32_t          Width()       const { return w_; }
    int32_t          Height()      const { return h_; }
    int32_t          AllowedType() const { return allowed_type_; }
    TObjectInstance* Item()        const { return item_; }
    PTBitmap         Icon()        const { return icon_; }
    EInvSlotKind     Kind()        const { return kind_; }

    // ----- hit test (TButton-style) ---------------------------------
    bool OnSlot(int32_t mx, int32_t my) const;

    // ----- drop-target policy ---------------------------------------
    // Inventory + BarInv accept anything (allowed_type_ == -1).
    // Equip cells reject items whose eqslot doesn't match.
    bool CanAcceptDrop(TObjectInstance* dragged) const;

    // ----- paint -----------------------------------------------------
    // Compose this slot into the given target surface. The target is
    // typically the pane's offscreen RT (the same `g_pane` every other
    // *ToTarget primitive takes); `font` is the count-text font (may
    // be null to skip text branches).
    virtual void Draw(TSurface* target, int32_t tw, int32_t th,
                      const SFontAtlas* font);

    // ----- interaction dispatch (stubbed) ---------------------------
    // Routes a mouse event through the cross-pane drag-state owner.
    // Returns true if the event was consumed.
    virtual bool HandleEvent(EInvSlotEvent kind,
                             int32_t mouse_x, int32_t mouse_y);

protected:
    // Rect (pane-local). TButton parity: x_/y_/w_/h_.
    int32_t          x_              = 0;
    int32_t          y_              = 0;
    int32_t          w_              = 0;
    int32_t          h_              = 0;

    // Per-instance config.
    int32_t          allowed_type_   = kInvSlotAcceptAny;
    PTBitmap         empty_placeholder_ = nullptr;
    const SInvSlotStyle* style_      = nullptr;

    // Per-cell content (pane sets via SetItem).
    EInvSlotKind     kind_           = EInvSlotKind::Empty;
    PTBitmap         icon_           = nullptr;
    PTBitmap         pouch_inner_    = nullptr;
    int32_t          qty_            = 1;
    TObjectInstance* item_           = nullptr;
};
