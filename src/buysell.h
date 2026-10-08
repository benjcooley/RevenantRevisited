// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  buysell.h - the shop (retail TBuySellScreen)                         *
// *************************************************************************
//
// REVSYNC: TBuySellScreen = cls_0x5a5d64, one global at 0x0065a3b8.
// docs/ui/forensics/BuySellScreen_SPEC.md is the forensic spec; the buysell*
// commands that drive it are in src/cmd_buysell.cpp
// (docs/gameflow/forensics/COMMAND_SYSTEM.md §6.6).
//
// A shop script fills the pane with rows -- a merchant's stock (Buy) or the
// customer's sellable items (Sell) -- and opens it. The pane is PlayScreen's
// bottom drawer in its third mode, not a modal: TPlayScreen::UpdateDrawer
// adds and removes it, and the world keeps running under it.

#pragma once

#include "defpane.h"
#include "saferef.h"

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class TObjectClass;
class TObjectImagery;
class TObjectInstance;

// One row. REVSYNC: the 0x48-byte record BuildItem (0x0052da90) fills.
struct SBuySellItem
{
    struct SStat
    {
        std::string label;
        int32_t     value = 0;
    };

    // Releases the counted imagery reference the row holds for its icon.
    struct SImageryRelease
    {
        void operator()(TObjectImagery* imagery) const;
    };

    std::string description;                    // +0x00
    std::string name;                           // +0x04 display name
    std::string type;                           // +0x08 the object type's name
    std::array<SStat, 5> stats{};               // +0x0c..+0x30
    int32_t     numstats = 0;                   // +0x34
    int32_t     price    = 0;                   // +0x38
    int32_t     amount   = 1;                   // +0x3c
    std::unique_ptr<TObjectImagery, SImageryRelease> imagery;   // +0x40 the icon's source
};

class TBuySellPane : public TDefPane
{
  public:
    // The shop type, retail +0x17c (buysellshoptype, COMMAND_SYSTEM §6.6).
    static constexpr uint32_t kBuy    = 0x01;
    static constexpr uint32_t kSell   = 0x02;
    static constexpr uint32_t kArmor  = 0x04;
    static constexpr uint32_t kWeapon = 0x08;
    static constexpr uint32_t kMisc   = 0x10;

    // The drawer's height (retail 0xb2, TPlayScreen Pulse 0x0047b4d0).
    static constexpr int32_t kHeight = 178;

    // REVSYNC: Initialize @ 0x0052f390 -- buysellinit, and PlayScreen before
    // it opens the drawer. The shop is in use from here until Exit or Reset;
    // the first row shown is 0, nothing is selected. The first call builds
    // the pane from buysell.dat and forgets salesperson, tags and customer;
    // later calls keep them and the rows.
    bool Initialize() override;
    // REVSYNC: Close @ 0x0052f6f0 (the play screen closing).
    void Close() override;
    // REVSYNC: Reset @ 0x00530600 (slot 13) -- the drawer closing: the rows go
    // and the shop is no longer in use.
    void Reset();
    // REVSYNC: 0x00532f40 -- LoadGame drops the rows.
    void Clear();

    // REVSYNC: +0x1b0, the script's `wait buysell` (wait 7, 0x0049301d).
    [[nodiscard]] bool IsActive() const { return active; }

    // Script API (the buysell* commands).
    void SetShopType(uint32_t type);                                   // +0x17c
    void SetCustomer(TObjectInstance* customer);                       // +0x1b4
    void SetSalesperson(TObjectInstance* salesperson);                 // 0x00532fb0
    void SetPurchaseDialog(const char* tag);                           // 0x00532fe0
    void SetNoGoldDialog(const char* tag);                             // 0x00533040
    void Add(const char* type, int32_t amount);                        // 0x00530670
    void AddCriteria(const char* stat, int32_t min, int32_t max);      // 0x00530af0
    void AddBuyItems();                                                // 0x00531b70
    void AddBuyItem(const char* name);                                 // 0x00531d70
    void AddBuyCriteria(const char* stat, int32_t min, int32_t max);   // 0x00531fc0
    void Remove(const char* name);                                     // 0x00532210
    void RemoveCriteria(const char* stat, int32_t min, int32_t max);   // 0x00532340

    [[nodiscard]] const std::vector<SBuySellItem>& Items() const { return items; }

    // TPane
    void Pulse() override;
    void Compose() override;
    void MouseClick(int32_t button, int32_t x, int32_t y) override;   // 0x0052fd50
    void MouseMove(int32_t button, int32_t x, int32_t y) override;    // 0x0052fca0
    void KeyPress(int32_t key, bool down) override;                    // 0x0052fa60
    void Joystick(int32_t key, bool down) override;                    // 0x0052fb30

  protected:
    void Paint() override;                                             // 0x0052f7d0
    void OnActivate(const SDefWidget& widget, int32_t buttonIndex) override;

  private:
    // Which customer item a sell list takes (AddBuyItem / AddBuyCriteria).
    struct SCustomerFilter
    {
        const char* name = nullptr;     // only items of this name
        const char* stat = nullptr;     // only items with this stat in [min, max]
        int32_t     min  = 0;
        int32_t     max  = 0;
    };

    void LayOut();
    [[nodiscard]] bool BuildItem(const char* name, int32_t amount, SBuySellItem& item) const;   // 0x0052da90
    [[nodiscard]] bool Stocks(const TObjectClass& cl, int32_t objtype) const;
    void AddCustomerItems(const SCustomerFilter& filter);
    [[nodiscard]] bool SellsToShop(TObjectInstance& item) const;
    void Append(SBuySellItem&& item);
    void RemoveAt(int32_t index);

    void DrawRow(int32_t index, int32_t state);                        // 0x0052f040
    [[nodiscard]] int32_t RowAt(int32_t x, int32_t y) const;

    void ScrollUp();                                                   // 0x0052fe90
    void ScrollDown();                                                 // 0x0052fee0
    void Transact();                                                   // 0x0052ff40
    void Buy(const SBuySellItem& item);
    void Sell(int32_t index);
    void Exit();                                                       // 0x00530570
    void Say(const std::string& tag) const;
    void ClampSelection();

    uint32_t shoptype  = 0;             // +0x17c
    int32_t  top       = 0;             // +0x180 first row shown
    int32_t  shown     = 3;             // +0x184 rows shown
    int32_t  selected  = -1;            // +0x188
    int32_t  hover     = -1;            // +0x18c
    std::vector<SBuySellItem> items;    // +0x194 / +0x198
    TSafeRef<TObjectInstance> salesperson;   // +0x1a0
    std::string purchasetag;            // +0x1a4
    std::string nogoldtag;              // +0x1a8
    bool     built     = false;         // +0x1ac
    bool     active    = false;         // +0x1b0
    TSafeRef<TObjectInstance> customer;      // +0x1b4
    int32_t  goldshown = -1;            // the gold the panel last drew (recompose when it changes)
};

extern TBuySellPane BuySellPane;
