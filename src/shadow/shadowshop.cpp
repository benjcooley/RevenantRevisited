// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *       shadowshop.cpp - Shadow Smoke buys and sells at a merchant        *
// *************************************************************************
//
// A purchase (docs/playtest/GRIND.md §7, docs/shadow/ARCHITECTURE.md §8.1):
// Locke walks up to the merchant and talks; the shop script greets him and
// offers Buy, Sell and Exit (BSBUY2, BSSELL2, BSEXIT2). The answer runs the
// script's half of the shop: its stock or the customer's sellable items go
// into TBuySellPane, and `buysellscreen` puts them up. The trade itself is
// the shop's own Buy and Sell, called per item rather than through the
// drawer's keys -- the stock, the prices, the gold and free-slot checks and
// the sold-unique bookkeeping are all theirs. Then Exit, and the menu's
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
#include "../textencoding.h"

#include <algorithm>
#include <cctype>

namespace {

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

// The row of `name` (display or type name), -1 for none.
int32_t FindRow(const std::string& name)
{
    const std::vector<SBuySellItem>& items = BuySellPane.Items();
    const std::string wanted = Lower(name);
    for (size_t i = 0; i < items.size(); ++i)
        if (Lower(items[i].name) == wanted || Lower(items[i].type) == wanted)
            return int32_t(i);
    return -1;
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
            // Holding enough already: nothing to buy.
            if (step.upto && wanted() <= 0)
                return EShadowStatus::Done;
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
            if (!DialogPane.IsShowingResponses())
                return EShadowStatus::Running;
            ShadowPressKey('0' + (buying ? kAnswerBuy : kAnswerSell));
            stage = EStage::Opening;
            return EShadowStatus::Running;

        case EStage::Opening:
            // The shop is in use from `buysellinit`, before the script adds
            // its rows; they are all in once `buysellscreen` puts them up.
            if (!BuySellPane.IsActive() || PlayScreen.Drawer() != TPlayScreen::EDrawer::BuySell)
                return EShadowStatus::Running;
            if (FindRow(step.item) < 0)
                return Close(context, step.target + (buying ? " doesn't sell " : " won't buy ") + step.item);
            target = step.upto ? std::max(0, wanted()) : step.count;
            Announce(context);
            stage = EStage::Trading;
            return EShadowStatus::Running;

        case EStage::Trading:
        {
            // One a tick, each checked against the gold and the pack.
            const int32_t row = FindRow(step.item);
            if (done >= target || row < 0)
                return Close(context, done >= target ? "" : "none left to sell");
            const int32_t gold = Player->GetInventoryAmount("Gold");
            const int32_t held = Player->GetInventoryAmount(step.item.c_str());
            if (buying)
                BuySellPane.Buy(BuySellPane.Items()[size_t(row)]);
            else
                BuySellPane.Sell(row);
            if (Player->GetInventoryAmount("Gold") == gold && Player->GetInventoryAmount(step.item.c_str()) == held)
            {
                // Buying up to a stock, what the gold allows is enough.
                if (step.upto)
                    return Close(context, "");
                return Close(context, buying ? "the purchase didn't go through (gold? a free slot?)"
                                             : "the sale didn't go through");
            }
            ++done;
            return EShadowStatus::Running;
        }

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
    enum class EStage : uint8_t { Go, Greeting, Opening, Trading, Leaving, Settling };

    // The ledger is told the whole trade before the first of it.
    void Announce(SShadowContext& context)
    {
        const SBuySellItem& item = BuySellPane.Items()[size_t(FindRow(step.item))];
        if (buying)
            context.ledger.AllowItems("buy " + step.item,
                                      {{Lower(item.type.empty() ? item.name : item.type), target}}, 0,
                                      context.tick + kShopLimitTicks);
        else
            context.ledger.AllowItems("sell " + step.item, {}, item.price * target, context.tick + kShopLimitTicks);
        context.log.Event(buying ? "buy" : "sell", ToUtf8(item.name.c_str()) + " x" + std::to_string(target) +
                                                       " at " + std::to_string(item.price) + " gold each");
    }

    EShadowStatus Close(SShadowContext& context, const std::string& why)
    {
        failure = why;
        context.log.Event(buying ? "bought" : "sold",
                          step.item + " x" + std::to_string(done) + (why.empty() ? "" : ": " + why));
        BuySellPane.Exit();
        stage = EStage::Leaving;
        return EShadowStatus::Running;
    }

    // Up to a stock: how many more he needs to hold `count`.
    [[nodiscard]] int32_t wanted() const { return step.count - Player->GetInventoryAmount(step.item.c_str()); }

    TShadowTravel travel;
    bool          buying  = true;
    int32_t       target  = 0;      // how many to trade
    EStage        stage   = EStage::Go;
    int32_t       age     = 0;
    int32_t       done    = 0;
    int32_t       settled = 0;
    std::string   failure;
};

} // namespace

std::unique_ptr<TShadowStepRunner> MakeShopRunner(const SShadowStep& step)
{
    return std::make_unique<TShopRunner>(step);
}
