// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *        shadowfight.cpp - Shadow Smoke's fight step, and the loot        *
// *************************************************************************
//
// `fight <target> [win]` and the fights the plan didn't ask for
// (docs/shadow/ARCHITECTURE.md §6.5): Locke walks up, everything fighting
// him joins in, the duel model resolves it (shadowduel.h), the fight's
// duration passes with the world running while the bodies drop what they
// carried (TCharacter::DropLoot), and Locke picks it all up (Pickup, what a
// click on the floor reaches). The ledger is told what each foe carries
// before the first blow, so loot can't appear any other way.

#include "shadowsteps.h"

#include "shadowduel.h"
#include "shadowledger.h"
#include "shadowlog.h"
#include "shadowtravel.h"

#include "../character.h"
#include "../mappane.h"
#include "../player.h"
#include "../revenant.h"
#include "../textencoding.h"

#include <algorithm>
#include <cctype>
#include <set>

namespace {

constexpr int32_t kTicksPerSecond = 24;
// Who counts as in the fight: characters this close fighting Locke.
constexpr int32_t kFightRange = 640;
// A body drops what it carried on its first dead pulse; give it this long.
constexpr int32_t kDropTicks = 2 * kTicksPerSecond;
// Drops land within about twice a body's radius, gold 30 off (DropLoot).
constexpr int32_t kLootRange = 128;
constexpr int32_t kMaxFoes = 32;
// Sizing a fight up: this many trial duels, and what makes it one to take.
constexpr int32_t kTrials = 24;
constexpr int32_t kWinsNeeded = 22;             // of kTrials
constexpr int32_t kWorstLossPct = 70;           // of his health, on average

std::string Lower(const char* s)
{
    std::string out = s ? s : "";
    std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) { return (char)std::tolower(c); });
    return out;
}

// Every living character near Locke that is fighting him.
std::vector<TCharacter*> Attackers()
{
    std::vector<TCharacter*> found;
    int32_t indices[kMaxFoes];
    const int32_t n = MapPane.FindObjectsInRange(Player->Pos(), Player->GetLevel(), indices, kFightRange, 0,
                                                 OBJCLASS_CHARACTER, kMaxFoes, OBJSET_ALL);
    for (int32_t i = 0; i < n; ++i)
        if (auto* who = dynamic_cast<TCharacter*>(MapPane.GetInstance(indices[i])))
            if (who->Health() > 0 && who->Fighting() == Player)
                found.push_back(who);
    return found;
}

class TFightRunner final : public TShadowStepRunner
{
  public:
    TFightRunner(const SShadowStep& step, const std::vector<TCharacter*>& met)
        : TShadowStepRunner(step), travel(step.target, false, TShadowTravel::EArrive::Beside)
    {
        travel.OnlyLiving();
        for (TCharacter* foe : met)
            given.push_back(foe);
    }

    EShadowStatus Pulse(SShadowContext& context) override
    {
        switch (stage)
        {
        case EStage::Go:
            if (!step.target.empty())
            {
                // No one of that name alive -- beaten, or his body gone --
                // is a fight already won; the plan's checks after it say so.
                // By a type name, the nearest one still standing.
                TCharacter* foe = dynamic_cast<TCharacter*>(
                    ShadowFindObject(step.target, Player->GetLevel(), Player->Pos(), true));
                if (!foe)
                {
                    context.log.Event("fight", step.target + " is already down");
                    return EShadowStatus::Done;
                }
                std::string why;
                const EShadowStatus status = travel.Pulse(context, why);
                if (status == EShadowStatus::Failed)
                    return Fail(why);
                if (status == EShadowStatus::Running)
                    return status;
            }
            return Resolve(context);

        case EStage::Settle:
            if (++waited < settle)
                return EShadowStatus::Running;
            Loot(context);
            return EShadowStatus::Done;
        }
        return EShadowStatus::Done;
    }

  private:
    enum class EStage : uint8_t { Go, Settle };

    EShadowStatus Resolve(SShadowContext& context)
    {
        std::vector<TCharacter*> foes = Attackers();
        for (const TSafeRef<TCharacter>& ref : given)
            if (TCharacter* foe = ref.Get(); foe && foe->Health() > 0 &&
                                             std::find(foes.begin(), foes.end(), foe) == foes.end())
                foes.push_back(foe);
        TCharacter* target = nullptr;
        if (!step.target.empty())
        {
            target = dynamic_cast<TCharacter*>(travel.Object());
            if (!target)
                return Fail(step.target + " isn't someone to fight");
            if (std::find(foes.begin(), foes.end(), target) == foes.end())
                foes.insert(foes.begin(), target);
        }
        if (foes.empty())
            return EShadowStatus::Done;

        // What they carry, and what lies about already: only what the bodies
        // drop is picked up, and the ledger knows it beforehand.
        std::map<std::string, int32_t> carried;
        int32_t gold = 0;
        for (TCharacter* foe : foes)
        {
            places.push_back(foe->Pos());
            for (TInventoryIterator i(foe, EInvWalk::Nested); i; i++)
            {
                const std::string name = Lower(i.Item()->GetName());
                const int32_t amount = std::max(i.Item()->Amount(), 1);
                if (name == "gold")
                    gold += amount;
                else
                    carried[name] += amount;
            }
        }
        for (const TObjectInstance* item : FloorItems())
            before.insert(item->GetMapIndex());

        std::string names;
        for (TCharacter* foe : foes)
            names += (names.empty() ? "" : ", ") +
                     ToUtf8(foe->GetName() && foe->GetName()[0] ? foe->GetName() : foe->GetTypeName());
        context.log.Event("fight", names, [&](JsonOut& out) {
            out.FieldString("foes", names);
            out.Field("gold", gold);
        });

        TShadowDuel duel(TShadowDuel::EMode::Live, SShadowTactics{}, uint32_t(context.tick));
        const SShadowFightReport report = duel.Fight(*Player, foes, context.tick, &context.ledger, &context.log);
        if (report.died)
            return Fail("Locke died fighting " + names);
        if (report.stalled)
            return Fail("the fight with " + names + " stalled");
        if (step.mustwin && target && target->Health() > 0)
            return Fail(step.target + " is still standing");
        if (report.dealt > 0 || !report.killed.empty())
            context.ledger.AllowItems("loot " + names, carried, gold,
                                      context.tick + std::max(report.ticks, kDropTicks) + kTicksPerSecond * 5);
        settle = std::max(report.ticks, kDropTicks);
        stage = EStage::Settle;
        return EShadowStatus::Running;
    }

    // The items lying on the floor around the fight.
    std::vector<TObjectInstance*> FloorItems() const
    {
        std::vector<TObjectInstance*> items;
        for (const S3DPoint& at : places)
            for (TMapIterator i(at, kLootRange, CHECK_NOINVENT); i; i++)
                if (i->IsInventoryItem() && !i->IsInInventory() &&
                    std::find(items.begin(), items.end(), i.Item()) == items.end())
                    items.push_back(i.Item());
        return items;
    }

    void Loot(SShadowContext& context)
    {
        int32_t taken = 0;
        for (TObjectInstance* item : FloorItems())
        {
            if (before.count(item->GetMapIndex()))
                continue;           // it was there before the fight
            context.log.Event("pickup", ToUtf8(item->GetName()));
            Player->Pickup(item);
            ++taken;
        }
        if (taken)
            context.log.Event("looted", std::to_string(taken) + " things");
    }

    TShadowTravel                     travel;
    std::vector<TSafeRef<TCharacter>> given;    // the monsters a walk met
    EStage                stage   = EStage::Go;
    std::vector<S3DPoint> places;
    std::set<int32_t>     before;
    int32_t               settle  = 0;
    int32_t               waited  = 0;
};

} // namespace

std::unique_ptr<TShadowStepRunner> MakeFightRunner(const SShadowStep& step, std::vector<TCharacter*> foes)
{
    return std::make_unique<TFightRunner>(step, foes);
}

SShadowOdds ShadowSizeUp(const std::vector<TCharacter*>& foes, int64_t tick)
{
    SShadowOdds odds;
    int32_t lost = 0;
    for (int32_t k = 0; k < kTrials; ++k)
    {
        TShadowDuel trial(TShadowDuel::EMode::Trial, SShadowTactics{}, uint32_t(tick * 31 + k));
        const SShadowFightReport report = trial.Fight(*Player, foes, tick, nullptr, nullptr);
        odds.wins += report.won;
        lost += report.suffered;
    }
    odds.trials = kTrials;
    odds.meanloss = lost / kTrials;
    odds.take = odds.wins >= kWinsNeeded && odds.meanloss * 100 <= Player->Health() * kWorstLossPct;
    return odds;
}

bool ShadowUnderAttack()
{
    return Player && !Attackers().empty();
}
