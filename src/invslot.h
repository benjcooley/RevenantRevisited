// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  invslot.h - shared item-cell sub-control (TInvSlot)                  *
// *************************************************************************
//
// One cell of an item pane: the potion shelf on the bottom bar (TBarInvPane),
// the sidebar inventory and the paperdoll. A pane owns its cells, binds each
// to the item it shows once a pulse, and draws them into its composed layer.
// What differs between panes is data: the cell rects and an SInvSlotStyle.
//
// What a cell shows -- retail's shelf (paint 0x0052ca70, Animate 0x0052cd80),
// measured in the emulator (docs/ui/HUD_REBUILD.md §7, P2b):
//   - the item's icon at the cell's top-left, clipped to the cell: the frame
//     of its inventory animation that the game frame picks, else its
//     inventory icon (SInvIcon: for ammo and money a stack of copies);
//   - its amount, when over 1: "%d", shadowed, in the style's amount text;
//   - for a "Pouch", the item in the pouch's slot 0 at half size (the 2:1
//     reduction, TRenderer::DrawBitmapHalvedToTarget), and next to it how
//     many items the pouch holds, shadowed.
// The sidebar inventory and the paperdoll draw their cells with the same
// calls; their styles are P4's to measure.
//
// Drag and drop between cells goes through UIDragState (uidragstate.h);
// the panes' drag protocol is in docs/ui/forensics/*_SPEC.md, P4 measures it.
// *************************************************************************

#pragma once

#include "font.h"
#include "graphics.h"
#include "invicon.h"

#include <cstdint>
#include <memory>

class TBitmap;
class TObjectInstance;
class TSurface;

constexpr int32_t kInvSlotAcceptAny = -1;

// A line of text in a cell: its rect's left, top and width relative to the
// cell's top-left, in a font and colour.
struct SInvSlotText
{
    const SFontAtlas* font = nullptr;   // none: not drawn
    SColor color = { 0xff, 0xff, 0xff };
    int32_t x = 0;
    int32_t y = 0;
    int32_t w = 0;
    ETextAlign align = ETextAlign::Left;
};

// Where a pane puts a cell's text and a pouch's item, relative to the cell's
// top-left (the icon sits there at its own size).
struct SInvSlotStyle
{
    SInvSlotText amount;            // the item's amount, when over 1
    int32_t pouchItemX = 0;         // a pouch: its slot-0 item at half size
    int32_t pouchItemY = 0;
    SInvSlotText pouchCount;        //          and how many items it holds
};

// What a cell shows of its item. A pane compares it from one pulse to the
// next to know when to recompose.
struct SInvSlotContent
{
    TObjectInstance* item = nullptr;
    SInvIcon icon;
    int32_t amount = 0;             // drawn when over 1
    SInvIcon pouchItem;             // a pouch: the icon of the item in its slot 0
    int32_t pouchCount = 0;         //          and how many items it holds

    [[nodiscard]] bool operator==(const SInvSlotContent& other) const;
    [[nodiscard]] bool operator!=(const SInvSlotContent& other) const { return !(*this == other); }
};

// Mouse events a cell routes to the drag owner (retail's MouseClick event
// numbers where it has one).
enum class EInvSlotEvent : int32_t
{
    MouseDown   = 1,
    MouseUp     = 4,    // commit the drop or put the item back
    UseOrEquip  = 5,    // right click
    HoverEnter,
    HoverLeave,
};

class TInvSlot
{
  public:
    // `allowedType` filters drops (an equipment slot's EQ_* number, or any);
    // `emptyPlaceholder` is drawn while the cell is empty (the paperdoll's
    // pictograms). The pane owns `style` and keeps it alive.
    TInvSlot(int32_t x, int32_t y, int32_t w, int32_t h,
             int32_t allowedType = kInvSlotAcceptAny,
             TBitmap* emptyPlaceholder = nullptr,
             const SInvSlotStyle* style = nullptr);
    ~TInvSlot();
    TInvSlot(TInvSlot&&) noexcept;
    TInvSlot& operator=(TInvSlot&&) noexcept;

    // Shows `item` (null: nothing), an animated icon on game frame `frame`.
    // True when what the cell shows changed.
    bool BindItem(TObjectInstance* item, int32_t frame);

    [[nodiscard]] int32_t PosX() const { return x; }
    [[nodiscard]] int32_t PosY() const { return y; }
    [[nodiscard]] int32_t Width() const { return w; }
    [[nodiscard]] int32_t Height() const { return h; }
    [[nodiscard]] int32_t AllowedType() const { return allowedType; }
    [[nodiscard]] TObjectInstance* Item() const { return content.item; }
    [[nodiscard]] const SInvSlotContent& Content() const { return content; }

    [[nodiscard]] bool OnSlot(int32_t mx, int32_t my) const;

    // Inventory and shelf cells take anything; an equipment cell takes what
    // fits its slot (the drop owner makes the final Player::Equip check).
    [[nodiscard]] bool CanAcceptDrop(TObjectInstance* dragged) const;

    // Puts together what the cell draws from a layer of its own -- a pouch's
    // stacked item, which is halved as a whole -- when it changed. Call it
    // outside any pass, before the pass Draw goes into.
    void Prepare();

    // Draws the cell into the active TSurface pass.
    void Draw(int32_t target_w, int32_t target_h) const;

    // Routes a mouse event to the drag owner; true if consumed. Not wired
    // yet: the panes' own event paths stay in charge until P4.
    bool HandleEvent(EInvSlotEvent kind, int32_t mouse_x, int32_t mouse_y);

  private:
    int32_t x = 0;
    int32_t y = 0;
    int32_t w = 0;
    int32_t h = 0;
    int32_t allowedType = kInvSlotAcceptAny;
    TBitmap* emptyPlaceholder = nullptr;
    const SInvSlotStyle* style = nullptr;
    SInvSlotContent content;
    std::unique_ptr<TSurface> pouchStack;   // content.pouchItem put together, when it is a stack
    SInvIcon pouchStackIcon;                // what pouchStack holds
};
