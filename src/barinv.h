// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *              barinv.h - The potion shelf on the bottom bar            *
// *************************************************************************

#pragma once

#include "invslot.h"
#include "screen.h"

#include <cstdint>
#include <memory>
#include <vector>

class TBitmap;
class TSurface;

// REVSYNC: TBarInvPane @ 0x0065b028 (vtable 0x005a56d4; docs/ui/
// HUD_REBUILD.md §6a). The shelf of boxes on the right of the bottom bar,
// one for each belt slot that fits: (bar width - 220) / 45 boxes, 45 apart,
// from x 220. Box n holds what the main player carries in belt slot
// 0x10b + n, drawn as every item cell is (TInvSlot). It shares the bottom
// bar's rect and draws over it (TBottomBarPane, its parent).
//
// Retail draws still items with the boxes (paint 0x0052ca70) and animated
// ones each frame (Animate 0x0052cd80); here both go into one composed layer,
// recomposed when what a box shows changes.
//
// Not yet ported (P4, with the other item panes' drag and drop): starting a
// drag off the shelf (0x0052d6e0) and taking a drop, which the side panel's
// harness still does by hit-testing the boxes, and the hovered item's name
// (Animate's first half, 0x0043a820).
class TBarInvPane final : public TPane
{
  public:
    TBarInvPane();
    ~TBarInvPane() override;
    TBarInvPane(const TBarInvPane&) = delete;
    TBarInvPane& operator=(const TBarInvPane&) = delete;

    // The box art, BottomBar.dat's "BarInvBox" (retail looks it up in the
    // archive TPlayScreen keeps, DAT_0065a570); set before Initialize.
    void SetBoxArt(TBitmap* box) { boxart = box; }

    bool Initialize() override;     // 0x0052c970
    void Close() override;
    void Pulse() override;          // binds the boxes to the main player's belt
    void PaneResized() override;    // a cell for each box
    void Compose() override;
    void Draw() override;

    // How many boxes the shelf shows at its width.
    [[nodiscard]] int32_t NumBoxes() const;

  private:
    void LayOutCells();

    TBitmap* boxart = nullptr;
    SInvSlotStyle style;
    std::vector<TInvSlot> cells;    // box n's cell, for belt slots that exist
    std::unique_ptr<TSurface> layer;
    int32_t contentVersion = 0;     // clicked when the boxes or what they show change
    int32_t composedVersion = -1;   // the version the layer holds
};
