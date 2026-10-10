// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *              barinv.h - The potion shelf on the bottom bar            *
// *************************************************************************

#pragma once

#include "screen.h"

#include <cstdint>
#include <memory>

class TBitmap;
class TSurface;

// REVSYNC: TBarInvPane @ 0x0065b028 (vtable 0x005a56d4; docs/ui/
// HUD_REBUILD.md §6a). The shelf of boxes on the right of the bottom bar,
// one for each belt slot that fits: (bar width - 220) / 45 boxes, 45 apart,
// from x 220. It shares the bottom bar's rect and draws over it
// (TBottomBarPane, its parent).
//
// Not yet ported: the belt's items in the boxes (paint 0x0052ca70, each
// item's own inventory draw and its count, a pouch's first item), the
// scroll offset and the hover (0x0052cd80) -- HUD_REBUILD P2b.
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
    void Compose() override;
    void Draw() override;

    // How many belt slots the shelf shows at its width.
    [[nodiscard]] int32_t NumBoxes() const;

  private:
    TBitmap* boxart = nullptr;
    std::unique_ptr<TSurface> layer;
    int32_t composedWidth = -1;     // the width the layer was composed for
};
