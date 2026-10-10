// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *                    bottombar.h - The bottom bar                       *
// *************************************************************************

#pragma once

#include "barinv.h"
#include "button.h"

#include <cstdint>
#include <memory>

class TMulti;

// REVSYNC: TBottomBarPane @ 0x0065b638 (vtable 0x005a5808; docs/ui/
// HUD_REBUILD.md §6a). The strip along the bottom of the map view: the bar
// itself, then over it the quick-spell rings (QuickSpells) and the potion
// shelf. Retail's paint (slot 20 0x0052c800) draws the bar and then the other
// two, and its SetRect (0x0052c930) sizes all three; here they are the bar's
// children, sharing its rect.
//
// The play screen shows the bar while the HUD's lower panel is open
// (TPlayScreen::Pulse); it lays itself out against the map view.
class TBottomBarPane final : public TButtonPane
{
  public:
    static constexpr int32_t kHeight = 0x3c;

    TBottomBarPane();
    ~TBottomBarPane() override;
    TBottomBarPane(const TBottomBarPane&) = delete;
    TBottomBarPane& operator=(const TBottomBarPane&) = delete;

    bool Initialize() override;     // 0x0052c780; the shelf's 0x0052c970, the rings' 0x00544160
    void Close() override;
    void Pulse() override;
    void PaneResized() override;    // the children take the bar's rect (0x0052c930)

    [[nodiscard]] TBarInvPane& Shelf() { return shelf; }

  protected:
    void ComposeBackground(int32_t target_w, int32_t target_h) override;    // slot 21, 0x0052c880

  private:
    void LayOut();

    std::unique_ptr<TMulti> archive;    // BottomBar.dat (retail DAT_0065a570)
    TBitmap* utilitybar = nullptr;
    TBitmap* endcap = nullptr;
    TBarInvPane shelf;
};
