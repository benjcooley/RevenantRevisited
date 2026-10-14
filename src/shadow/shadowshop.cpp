// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *       shadowshop.cpp - Shadow Smoke buys and sells at a merchant        *
// *************************************************************************
//
// A purchase as a player makes one (docs/playtest/GRIND.md §7.3,
// docs/shadow/ARCHITECTURE.md §8.1): walk up to the merchant and talk; the
// shop script greets and offers Buy, Sell and Exit (BSBUY2, BSSELL2,
// BSEXIT2); the answer opens the shop drawer (TBuySellPane) on its first
// page with nothing selected. Its own keys then do the rest: the arrow pages
// three rows at a time, 1-3 selects a row of the page, A buys or sells the
// selection (a buy keeps it selected), E closes the drawer, and the menu's
// third answer ends the conversation.

#include "shadowsteps.h"

#include "shadowledger.h"
#include "shadowlog.h"
#include "shadowtravel.h"

#include "../buysell.h"
#include "../dialog.h"
#include "../player.h"
#include "../playscreen.h"
#include "../revenant.h"
#include "../revtypes.h"
#include "../textencoding.h"

#include <algorithm>
#include <cctype>

namespace {

constexpr int32_t kRowsShown = 3;               // TBuySellPane::shown
constexpr int32_t kAnswerBuy = 1;               // the shop menu: Buy, Sell, Exit
constexpr int32_t kAnswerSell = 2;
constexpr int32_t kAnswerLeave = 3;
// A shop step that has gone this long has stuck.
constexpr int32_t kShopLimitTicks = 2 * 60 * 24;
constexpr int32_t kSettleTicks = 2 * 24;

std::string Lower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return (char)std::tolower(c); });
    return s;
}

class TShopRunner final : public TShadowStepRunner
{
  public:
    explicit TShopRunner(const SShadowStep& step)
        : TShadowStepRunner(step), travel(step.target, false, TShadowTravel::EArrive::Beside),
          buying(step.verb == EShadowVerb::Buy)
    {
    }

    EShadowStatus Pulse(SShadowContext& context) override
    {
        if (stage != EStage::Go && ++age > kShopLimitTicks)
            return Fail("the shop didn't finish");
        switch (stage)
        {
        case EStage::Go:
        {
            std::string why;
            const EShadowStatus status = travel.Pulse(context, why);
            if (status == EShadowStatus::Failed)
                return Fail(why);
            if (status == EShadowStatus::Running)
                return status;
            TObjectInstance* merchant = travel.Object();
            if (!merchant)
                return Fail("no " + step.target + " here");
            context.log.Event("shop", step.Describe());
            merchant->Use(Player, -1);
            stage = EStage::Greeting;
            return EShadowStatus::Running;
        }

        case EStage::Greeting:
            // The shop's three answers; buy or sell.
            if (!DialogPane.IsShowingResponses())
                return EShadowStatus::Running;
            ShadowPressKey('0' + (buying ? kAnswerBuy : kAnswerSell));
            stage = EStage::Opening;
            return EShadowStatus::Running;

        case EStage::Opening:
            // The shop is in use from `buysellinit`, before the script adds
            // its rows; it's open once `buysellscreen` has the drawer up.
            if (!BuySellPane.IsActive() || PlayScreen.Drawer() != TPlayScreen::EDrawer::BuySell)
                return EShadowStatus::Running;
            return FindRow(context);

        case EStage::Paging:
            if (pages != 0)
            {
                ShadowPressKey(pages > 0 ? VK_DOWN : VK_UP);
                pages += pages > 0 ? -1 : 1;
                return EShadowStatus::Running;
            }
            ShadowPressKey('1' + row % kRowsShown);
            gold = Player->GetInventoryAmount("Gold");
            held = Player->GetInventoryAmount(step.item.c_str());
            stage = EStage::Trading;
            return EShadowStatus::Running;

        case EStage::Trading:
            if (done < step.count)
            {
                // One a tick, each checked: a buy needs the gold and a free
                // slot; a sale takes the row away, so each sale reselects.
                const int32_t nowgold = Player->GetInventoryAmount("Gold");
                const int32_t nowheld = Player->GetInventoryAmount(step.item.c_str());
                if (pressed && nowgold == gold && nowheld == held)
                    return StopTrading(context, buying ? "the purchase didn't go through (gold? a free slot?)"
                                                       : "the sale didn't go through");
                gold = nowgold;
                held = nowheld;
                if (pressed)
                    ++done;
                if (done >= step.count)
                    return StopTrading(context, "");
                if (!buying && pressed)
                    return FindRow(context);      // the sold row is gone; find the next
                ShadowPressKey('A');
                pressed = true;
                return EShadowStatus::Running;
            }
            return StopTrading(context, "");

        case EStage::Closing:
            // Exit ends the shop (the drawer keeps its mode as it closes).
            if (BuySellPane.IsActive())
                return EShadowStatus::Running;
            stage = EStage::Leaving;
            [[fallthrough]];

        case EStage::Leaving:
            if (!DialogPane.IsShowingResponses())
                return EShadowStatus::Running;
            ShadowPressKey('0' + kAnswerLeave);
            stage = EStage::Settling;
            return EShadowStatus::Running;

        case EStage::Settling:
            if (!ShadowGameIdle())
            {
                settled = 0;
                return EShadowStatus::Running;
            }
            if (++settled < kSettleTicks)
                return EShadowStatus::Running;
            if (!failure.empty())
                return Fail(failure);
            return EShadowStatus::Done;
        }
        return EShadowStatus::Done;
    }

  private:
    enum class EStage : uint8_t { Go, Greeting, Opening, Paging, Trading, Closing, Leaving, Settling };

    // The row of the item, and the pages down to it from the first.
    EShadowStatus FindRow(SShadowContext& context)
    {
        const std::vector<SBuySellItem>& items = BuySellPane.Items();
        const std::string wanted = Lower(step.item);
        row = -1;
        for (size_t i = 0; i < items.size(); ++i)
            if (Lower(items[i].name) == wanted || Lower(items[i].type) == wanted)
            {
                row = int32_t(i);
                break;
            }
        if (row < 0)
        {
            std::string rows;
            for (const SBuySellItem& item : items)
                rows += (rows.empty() ? "" : ", ") + ToUtf8(item.name.c_str()) + " [" + ToUtf8(item.type.c_str()) + "]";
            return StopTrading(context, step.target + (buying ? " doesn't sell " : " won't buy ") + step.item +
                                            " (rows: " + rows + ")");
        }
        const SBuySellItem& item = items[size_t(row)];
        if (!announced)
        {
            const int32_t left = step.count - done;
            if (buying)
                context.ledger.AllowItems("buy " + step.item, {{Lower(item.type.empty() ? item.name : item.type), left}},
                                          0, context.tick + kShopLimitTicks);
            else
                context.ledger.AllowItems("sell " + step.item, {}, item.price * left, context.tick + kShopLimitTicks);
            context.log.Event(buying ? "buy" : "sell",
                              ToUtf8(item.name.c_str()) + " x" + std::to_string(left) + " at " +
                                  std::to_string(item.price) + " gold each");
            announced = true;
        }
        // The drawer opens on its first page; a sale clears the selection and
        // leaves the page where it was. Page from where it is to the row's.
        pages = row / kRowsShown - page;
        page = row / kRowsShown;
        pressed = false;
        stage = EStage::Paging;
        return EShadowStatus::Running;
    }

    EShadowStatus StopTrading(SShadowContext& context, const std::string& why)
    {
        failure = why;
        context.log.Event(buying ? "bought" : "sold",
                          step.item + " x" + std::to_string(done) + (why.empty() ? "" : ": " + why));
        ShadowPressKey('E');
        stage = EStage::Closing;
        return EShadowStatus::Running;
    }

    TShadowTravel travel;
    bool          buying    = true;
    EStage        stage     = EStage::Go;
    int32_t       age       = 0;
    int32_t       row       = -1;
    int32_t       page      = 0;
    int32_t       pages     = 0;
    int32_t       done      = 0;
    int32_t       gold      = 0;
    int32_t       held      = 0;
    int32_t       settled   = 0;
    bool          pressed   = false;
    bool          announced = false;
    std::string   failure;
};

} // namespace

std::unique_ptr<TShadowStepRunner> MakeShopRunner(const SShadowStep& step)
{
    return std::make_unique<TShopRunner>(step);
}
