// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   Inventory.h - TInventory Object                     *
// *************************************************************************

#pragma once

#include "button.h"
#include "invslot.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class TMulti;
class TObjectInstance;

// REVSYNC: TInventory @ 0x0065d4f8 (vtable 0x005a58c0; docs/ui/
// HUD_REBUILD.md §6b). The side panel's inventory, in its lower slot: the
// container's icon, the player's gold, twelve item cells and two arrows that
// page through the container one column at a time. The container is the
// main player's inventory, set as the player becomes main, or a bag the
// player opened.
//
// Not yet ported (P4, interactions): retail's own clicks and drags
// (MouseClick 0x00538210) -- opening a bag, climbing back out through the
// container icon, using and swapping items. The side panel harness still
// hit-tests the cells for drags (UIDragState), through FirstSlot.
class TInventory final : public TButtonPane
{
  public:
    static constexpr int32_t kWidth = 0xbc;
    static constexpr int32_t kHeight = 0xae;
    static constexpr int32_t kColumns = 4;
    static constexpr int32_t kRows = 3;
    static constexpr int32_t kLastFirstSlot = 0xf3;     // the last column's first slot keeps 12 in 0..255

    TInventory();
    ~TInventory() override;
    TInventory(const TInventory&) = delete;
    TInventory& operator=(const TInventory&) = delete;

    bool Initialize() override;     // 0x00537650
    void Close() override;
    void Pulse() override;          // binds the cells, the gold readout and the container icon
    void Compose() override;

    // The container whose inventory the pane shows (retail +0x17c).
    [[nodiscard]] TObjectInstance* GetContainer() const { return container; }
    // Its outermost owner: usually the player.
    [[nodiscard]] TObjectInstance* GetTopContainer() const;
    void SetContainer(TObjectInstance* cont);

    // The first slot the grid shows (retail +0x188): slot first + col * 3 +
    // row is in column col, row row.
    [[nodiscard]] int32_t FirstSlot() const { return firstSlot; }
    // Pages one column left (-1) or right (+1), within 0..kLastFirstSlot
    // (0x00537520); the arrows' functions.
    void Scroll(int32_t columns);

    // The screen centre of the cell showing `slot`; false when the slot is
    // not among the twelve in view or the pane is hidden. For the playtest
    // director, which right-clicks an item to equip it as a player does.
    [[nodiscard]] bool CellCenter(int32_t slot, int32_t& x, int32_t& y);

    // The slot an item is being dragged from, for the panes it is dropped
    // on; -1 while nothing is (retail +0x180, set by its drag, P4).
    [[nodiscard]] int32_t GetHeldSlot() const { return -1; }

  protected:
    void ComposeBackground(int32_t target_w, int32_t target_h) override;    // 0x00537a70

  private:
    void RefreshArrows();

    std::unique_ptr<TMulti> archive;        // Inventory.dat (retail DAT_0065c130)
    TBitmap* background = nullptr;
    TBitmap* backpack = nullptr;
    TBitmap* goldpile = nullptr;
    const SFontAtlas* goldFont = nullptr;   // "Gold" (retail's font 9, DAT_006663dc)
    TObjectInstance* container = nullptr;
    int32_t firstSlot = 0;
    SInvSlotStyle style;
    std::vector<TInvSlot> cells;            // the grid, column-major
    std::string goldText;                   // what the gold readout shows
    TBitmap* containerIcon = nullptr;       // what the container icon shows
};
