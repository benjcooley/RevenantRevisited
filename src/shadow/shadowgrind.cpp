// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *        shadowgrind.cpp - Shadow Smoke grinds for levels and loot        *
// *************************************************************************
//
// `grind until level <n> | skill <name> <n> [restock <routine>]`
// (docs/shadow/ARCHITECTURE.md §8.5; the facts behind it are
// docs/playtest/GRIND.md §1-3 and §9). Locke fights around where he is
// until he reaches the level or the skill -- a skill grows with the kills
// made with it (LandBlow's AwardSkillExp), so Hands is practised by
// fighting with nothing in his hands:
//
// - **Sites.** The level's hostile monsters and its generators, gathered
//   into sites 600 units across, each scored by how well its monsters'
//   Value suits his level (experience wants Value near his level, and a
//   monster over level + 1 is one to stay away from) and by how far it is.
// - **Clearing.** At a site he sizes up each monster with trial duels on
//   copies (§6.4) and fights the ones he would beat, nearest first, then
//   picks up what they drop.
// - **Farming.** Placed monsters don't come back; generators do, but only
//   while they are loaded and off screen (GRIND.md §3.1). So with a site
//   cleared he waits 650-800 units from its generators, inside the sector
//   window but out of sight, and goes back when something has spawned.
// - **Keeping going.** Below 60% health he eats or drinks what he carries;
//   with nothing left he rests somewhere no monster sees him until his
//   health has come back (TCharacter::PulseRecovery) -- he never sets off
//   hurt, which is how a walk's fights kill him. Out of healing and with
//   the gold for it, he runs the plan's restock routine (to town and the
//   potion shop) and comes back.

#include "shadowsteps.h"

#include "shadowduel.h"
#include "shadowencounters.h"
#include "shadowledger.h"
#include "shadowlog.h"
#include "shadowplan.h"
#include "shadowtravel.h"

#include "../character.h"
#include "../mappane.h"
#include "../monstergen.h"
#include "../player.h"
#include "../revenant.h"
#include "../textencoding.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iterator>
#include <memory>
#include <set>

namespace {

constexpr int32_t kTicksPerSecond = 24;
// Monsters and generators this close together are one site (grind.py's
// SITE_RADIUS).
constexpr int32_t kSiteRadius = 600;
// A site cleared is left alone this long, so its generators can refill.
constexpr int32_t kRevisitTicks = 2 * 60 * kTicksPerSecond;
// Farming: where to wait from the generators -- out of sight (the animate
// pass reaches about 450-500 units, GRIND.md §3.1) but in the sector window
// that keeps them pulsing -- and the longest wait (the slowest slot is 91 s).
constexpr int32_t kFarmNear = 650;
constexpr int32_t kFarmFar = 800;
constexpr int32_t kFarmTicks = 100 * kTicksPerSecond;
// Sizing a fight up: this many trial duels, and what makes it one to take.
constexpr int32_t kTrials = 24;
constexpr int32_t kWinsNeeded = 22;             // of kTrials
constexpr int32_t kWorstLossPct = 70;           // of his health, on average
// He tops up below this share of his health, food first out of a fight;
// with nothing to, he rests to this share. Health comes back about a point
// a minute (GRIND.md §6.1): a rest is long, and has stuck when this long
// passes without a point.
constexpr int32_t kTopUpPct = 60;
constexpr int32_t kRestedPct = 90;
constexpr int32_t kRestStuckTicks = 5 * 60 * kTicksPerSecond;
// No grind goes on longer than this much game time.
constexpr int32_t kGrindLimit = 6 * 60 * 60 * kTicksPerSecond;

constexpr const char* kHealing[] = {"Lesser Healing", "Lesser Restoration", "Greater Healing",
                                    "Greater Restoration", "Enchanted Healing", "Enchanted Restoration"};
constexpr const char* kFood[] = {"Bread", "Apple", "Cheese", "Meat", "Fish", "Orange", "Banana", "Carrot",
                                 "Celery", "Grapes", "Watermelon", "Muffin", "Sausage", "Drum Stick", "Lettuce",
                                 "Chocolate"};

int32_t Distance2D(const S3DPoint& a, const S3DPoint& b)
{
    return (int32_t)std::lround(std::hypot(double(a.x - b.x), double(a.y - b.y)));
}

// A character type's Value (class.def), by name: what a generator slot
// makes is worth.
int32_t TypeValue(const char* type)
{
    const TObjectClass* cl = TObjectClass::GetClass(OBJCLASS_CHARACTER);
    const int32_t objtype = cl && type ? cl->FindObjType(type) : -1;
    return objtype >= 0 ? cl->GetStat(objtype, "Value") : 0;
}

// How well a monster of `value` suits a level-`level` Locke (grind.py fit):
// experience wants Value at or above his level; well below it is worth
// little; above level + 1 is not for him.
double Fit(int32_t value, int32_t level)
{
    if (value > level + 1)
        return 0.0;
    if (value >= level)
        return 1.0;
    if (value >= level - 3)
        return 0.5;
    return 0.05;
}

bool Hostile(TCharacter& who)
{
    return who.Health() > 0 && who.Aggressive() && !(who.Flags() & OF_INVISIBLE) && who.IsEnemy(Player);
}

struct SSite
{
    S3DPoint                            centre{};
    std::vector<TSafeRef<TCharacter>>   monsters;
    std::vector<TSafeRef<TObjectInstance>> generators;
    double                              score = 0.0;
};

class TGrindRunner final : public TShadowStepRunner
{
  public:
    using TShadowStepRunner::TShadowStepRunner;

    EShadowStatus Pulse(SShadowContext& context) override
    {
        if (++age > kGrindLimit)
            return Fail("still short of " + step.goal.Describe() + " after " +
                        std::to_string(kGrindLimit / kTicksPerSecond / 3600) + " hours of game time");
        switch (stage)
        {
            case EStage::Assess:   return Assess(context);
            case EStage::Restock:  return Restock(context);
            case EStage::Go:       return Go(context);
            case EStage::Clear:    return Clear(context);
            case EStage::Fight:    return Fight(context);
            case EStage::Linger:   return Linger(context);
            case EStage::Rest:     return Rest(context);
        }
        return EShadowStatus::Running;
    }

  private:
    enum class EStage : uint8_t { Assess, Restock, Go, Clear, Fight, Linger, Rest };

    // The goal reached? A goal the game can't read fails the step.
    [[nodiscard]] bool Reached(std::string& why) const
    {
        int32_t actual = 0;
        return ShadowRead(step.goal, actual, why) && step.goal.Holds(actual);
    }

    [[nodiscard]] static bool Hurt() { return Player->Health() * 100 < Player->MaxHealth() * kTopUpPct; }

    [[nodiscard]] static bool CarriesHealing()
    {
        return std::any_of(std::begin(kHealing), std::end(kHealing),
                           [](const char* potion) { return Player->FindObjInventory(potion) != nullptr; });
    }

    // ---- what next ----
    EShadowStatus Assess(SShadowContext& context)
    {
        if (!ShadowGameIdle())
            return EShadowStatus::Running;
        if (home.z == INT32_MIN)
            home = Player->Pos();
        std::string why;
        if (Reached(why))
        {
            context.log.Event("grind", step.goal.Describe() + " reached, at level " + std::to_string(Player->Level()));
            return EShadowStatus::Done;
        }
        if (!why.empty())
            return Fail(why);
        // A level up makes the monsters he left worth another look.
        if (Player->Level() != sizedat)
        {
            hard.clear();
            sizedat = Player->Level();
        }
        if (Hurt())
            return TopUp(context) ? EShadowStatus::Running : StartRest(context);
        if (!step.routine.empty() && !restocked && !CarriesHealing() &&
            Player->GetInventoryAmount("Gold") >= kPotionPrice)
        {
            StartRoutine(context);
            return EShadowStatus::Running;
        }
        // The best site; with none to clear, the best one to farm.
        std::vector<SSite> sites = Survey();
        SSite* best = nullptr;
        for (SSite& site : sites)
            if (!Recent(site, context) && !site.monsters.empty() && (!best || site.score > best->score))
                best = &site;
        if (best)
        {
            site = *best;
            context.log.Event("grind", "to the site at (" + std::to_string(site.centre.x) + ", " +
                                           std::to_string(site.centre.y) + "): " + std::to_string(site.monsters.size()) +
                                           " monsters, score " + std::to_string((int32_t)(site.score * 100)) + "%");
            travel = std::make_unique<TShadowTravel>(site.centre, "the site", false);
            stage = EStage::Go;
            return EShadowStatus::Running;
        }
        // Farming is going back to a site cleared, so recent ones count.
        for (SSite& candidate : sites)
            if (!candidate.generators.empty() && (!best || candidate.score > best->score))
                best = &candidate;
        if (!best)
            return Fail("nothing left to fight that Locke would beat, at level " + std::to_string(Player->Level()));
        site = *best;
        return Farm(context);
    }

    // ---- eat, drink, restock ----
    static constexpr int32_t kPotionPrice = 250;    // Lesser Healing (GRIND.md §6.2)

    bool TopUp(SShadowContext& context)
    {
        auto eat = [&](const char* name) {
            TObjectInstance* item = Player->FindObjInventory(name);
            if (!item)
                return false;
            const int32_t health = item->FindStat("Health") >= 0 ? item->GetStat("Health") : 0;
            const int32_t mana = item->FindStat("Mana") >= 0 ? item->GetStat("Mana") : 0;
            context.ledger.AllowVitals(std::string("consume ") + name, health, mana, context.tick + 2);
            context.log.Event("consume", name);
            item->Use(Player, -1);
            return true;
        };
        // Food first (it can't be sold back, GRIND.md §6.2), then potions.
        for (const char* food : kFood)
            if (eat(food))
                return true;
        for (const char* potion : kHealing)
            if (eat(potion))
                return true;
        return false;
    }

    void StartRoutine(SShadowContext& context)
    {
        context.log.Event("grind", "restock: " + step.routine);
        routinestep = 0;
        routinerunner.reset();
        restocked = true;       // once per spell of need; a fight that goes well clears it
        stage = EStage::Restock;
    }

    EShadowStatus Restock(SShadowContext& context)
    {
        const SShadowStage* routine = context.plan.FindRoutine(step.routine);
        while (true)
        {
            if (!routinerunner)
            {
                if (routinestep >= (int32_t)routine->steps.size())
                {
                    stage = EStage::Assess;
                    return EShadowStatus::Running;
                }
                context.log.Event("step", routine->steps[routinestep].Describe() + " (" + step.routine + ")");
                routinerunner = MakeStepRunner(routine->steps[routinestep]);
            }
            const EShadowStatus status = routinerunner->Pulse(context);
            if (status == EShadowStatus::Running)
                return status;
            if (status == EShadowStatus::Failed)
                return Fail("restocking: " + routinerunner->Failure());
            routinerunner.reset();
            ++routinestep;
        }
    }

    // ---- sites ----
    std::vector<SSite> Survey() const
    {
        const int32_t level = Player->Level();
        std::vector<SSite> sites;
        auto siteFor = [&](const S3DPoint& at) -> SSite& {
            for (SSite& s : sites)
                if (Distance2D(s.centre, at) <= kSiteRadius)
                    return s;
            sites.push_back(SSite{at});
            return sites.back();
        };
        for (TMapIterator i(Player->GetLevel(), nullptr, CHECK_NOINVENT); i; i++)
        {
            if (i->ObjClass() == OBJCLASS_CHARACTER)
            {
                auto* monster = static_cast<TCharacter*>(i.Item());
                if (!Hostile(*monster))
                    continue;
                SSite& s = siteFor(monster->Pos());
                s.monsters.push_back(monster);
                const int32_t value = monster->GetStat("Value");
                s.score += Fit(value, level);
                if (value > level + 1)
                    s.score -= 2.0;     // one too strong costs the site (grind.py)
            }
            else if (auto* generator = dynamic_cast<TMonsterGen*>(i.Item()); generator && generator->NumTypes() > 0)
            {
                SSite& s = siteFor(generator->Pos());
                s.generators.push_back(generator);
                for (int32_t k = 0; k < TMonsterGen::kMaxTypes; ++k)
                    if (generator->TypeName(k)[0])
                        s.score += 0.5 * Fit(TypeValue(generator->TypeName(k)), level);
            }
        }
        // Nearer is better: a site twice as far scores two thirds as well.
        for (SSite& s : sites)
            s.score /= 1.0 + Distance2D(s.centre, Player->Pos()) / 4000.0;
        return sites;
    }

    [[nodiscard]] bool Recent(const SSite& candidate, const SShadowContext& context) const
    {
        for (const auto& [centre, when] : visited)
            if (Distance2D(centre, candidate.centre) <= kSiteRadius / 2 && context.tick - when < kRevisitTicks)
                return true;
        return false;
    }

    // ---- going, clearing, fighting ----
    EShadowStatus Go(SShadowContext& context)
    {
        std::string why;
        const EShadowStatus status = travel->Pulse(context, why);
        if (status == EShadowStatus::Failed)
        {
            // Somewhere he can't walk to: leave the site be for a while.
            context.log.Event("grind", "can't reach the site: " + why);
            visited.push_back({site.centre, context.tick});
            stage = EStage::Assess;
            return EShadowStatus::Running;
        }
        if (status == EShadowStatus::Running)
            return status;
        travel.reset();
        stage = EStage::Clear;
        return EShadowStatus::Running;
    }

    EShadowStatus Clear(SShadowContext& context)
    {
        // The nearest monster of the site he would beat.
        TCharacter* next = nullptr;
        for (const TSafeRef<TCharacter>& ref : site.monsters)
        {
            TCharacter* monster = ref.Get();
            if (!monster || !Hostile(*monster) || hard.count(monster->GetMapIndex()))
                continue;
            if (!next || Distance2D(monster->Pos(), Player->Pos()) < Distance2D(next->Pos(), Player->Pos()))
                next = monster;
        }
        if (!next)
        {
            // Cleared: back to the start, which sees whether he is done, needs
            // healing, or should farm this site's generators.
            visited.push_back({site.centre, context.tick});
            stage = EStage::Assess;
            return EShadowStatus::Running;
        }
        if (!SizeUp(*next, context))
        {
            hard.insert(next->GetMapIndex());
            return EShadowStatus::Running;
        }
        fightstep = SShadowStep{};
        fightstep.verb = EShadowVerb::Fight;
        fight = MakeFightRunner(fightstep, {next});
        stage = EStage::Fight;
        return EShadowStatus::Running;
    }

    // Would he beat it as he is? Trial duels on copies (§6.4).
    bool SizeUp(TCharacter& monster, SShadowContext& context)
    {
        int32_t wins = 0, lost = 0;
        for (int32_t k = 0; k < kTrials; ++k)
        {
            TShadowDuel trial(TShadowDuel::EMode::Trial, SShadowTactics{}, uint32_t(context.tick * 31 + k));
            const SShadowFightReport report = trial.Fight(*Player, {&monster}, context.tick, nullptr, nullptr);
            wins += report.won;
            lost += report.suffered;
        }
        const int32_t meanloss = lost / kTrials;
        const bool take = wins >= kWinsNeeded && meanloss * 100 <= Player->Health() * kWorstLossPct;
        const std::string name = ToUtf8(monster.GetName() && monster.GetName()[0] ? monster.GetName()
                                                                                    : monster.GetTypeName());
        context.log.Event("sizeup", name + ": won " + std::to_string(wins) + "/" + std::to_string(kTrials) +
                                        ", losing " + std::to_string(meanloss) + " health on average: " +
                                        (take ? "fight" : "leave it"));
        return take;
    }

    EShadowStatus Fight(SShadowContext& context)
    {
        const EShadowStatus status = fight->Pulse(context);
        if (status == EShadowStatus::Running)
            return status;
        if (status == EShadowStatus::Failed)
            return Fail(fight->Failure());
        fight.reset();
        restocked = false;
        // Top up between fights if he needs it, then on with the site --
        // unless he is done, or hurt with nothing left (Assess rests him).
        if (Hurt())
            TopUp(context);
        std::string why;
        stage = Reached(why) || !why.empty() || Hurt() ? EStage::Assess : EStage::Clear;
        return EShadowStatus::Running;
    }

    // ---- farming the generators ----
    EShadowStatus Farm(SShadowContext& context)
    {
        // A spot in reach of the generators' sector window but out of sight
        // of them: the first standable one he can walk to, around the site.
        const TShadowWalkField* field = context.nav.Field(context.log);
        const int32_t region = field ? field->RegionAt(Player->Pos()) : 0;
        for (int32_t k = 0; k < 16 && field; ++k)
        {
            const double angle = k * (2.0 * M_PI / 16.0);
            for (const int32_t dist : {kFarmNear, (kFarmNear + kFarmFar) / 2, kFarmFar})
            {
                const S3DPoint probe(site.centre.x + int32_t(std::cos(angle) * dist),
                                     site.centre.y + int32_t(std::sin(angle) * dist), site.centre.z);
                if (const std::optional<S3DPoint> spot = field->NearestStandable(probe, 32, region))
                {
                    context.log.Event("grind", "farming the generators at (" + std::to_string(site.centre.x) +
                                                   ", " + std::to_string(site.centre.y) + ")");
                    travel = std::make_unique<TShadowTravel>(*spot, "the farming spot", false);
                    waited = 0;
                    stage = EStage::Linger;
                    return EShadowStatus::Running;
                }
            }
        }
        visited.push_back({site.centre, context.tick});
        stage = EStage::Assess;
        return EShadowStatus::Running;
    }

    EShadowStatus Linger(SShadowContext& context)
    {
        if (travel)
        {
            std::string why;
            const EShadowStatus status = travel->Pulse(context, why);
            if (status == EShadowStatus::Running)
                return status;
            travel.reset();
            if (status == EShadowStatus::Failed)
            {
                visited.push_back({site.centre, context.tick});
                stage = EStage::Assess;
                return EShadowStatus::Running;
            }
        }
        // Once a second: has something spawned at the site?
        ++waited;
        if (waited % kTicksPerSecond == 0)
        {
            int32_t found[16];
            const int32_t n = MapPane.FindObjectsInRange(site.centre, Player->GetLevel(), found, kSiteRadius, 0,
                                                         OBJCLASS_CHARACTER, 16, OBJSET_ALL);
            site.monsters.clear();
            for (int32_t i = 0; i < n; ++i)
                if (auto* monster = dynamic_cast<TCharacter*>(MapPane.GetInstance(found[i])); monster && Hostile(*monster))
                    site.monsters.push_back(monster);
            if (!site.monsters.empty())
            {
                context.log.Event("grind", std::to_string(site.monsters.size()) + " spawned; back to the site");
                travel = std::make_unique<TShadowTravel>(site.centre, "the site", false);
                stage = EStage::Go;
                return EShadowStatus::Running;
            }
        }
        if (waited > kFarmTicks)
        {
            visited.push_back({site.centre, context.tick});
            stage = EStage::Assess;
        }
        return EShadowStatus::Running;
    }

    // ---- resting ----
    // Where he is, if no monster would see him; else back where the grind
    // began (the plan brought him there, so the way is his), running.
    EShadowStatus StartRest(SShadowContext& context)
    {
        const bool seen = !ShadowLegEncounters({Player->Pos()}, Player->GetLevel()).empty();
        context.log.Event("grind", "resting at " + std::to_string(Player->Health()) + "/" +
                                       std::to_string(Player->MaxHealth()) + " health" +
                                       (seen ? ", away from the monsters" : ""));
        if (seen)
            travel = std::make_unique<TShadowTravel>(home, "a quiet spot", true);
        restfrom = restgain = context.tick;
        resthealth = Player->Health();
        stage = EStage::Rest;
        return EShadowStatus::Running;
    }

    EShadowStatus Rest(SShadowContext& context)
    {
        if (travel)
        {
            std::string why;
            const EShadowStatus status = travel->Pulse(context, why);
            if (status == EShadowStatus::Running)
                return status;
            travel.reset();         // got there, or rests where he stands
        }
        if (Player->Health() * 100 >= Player->MaxHealth() * kRestedPct)
        {
            context.log.Event("grind", "rested in " + std::to_string((context.tick - restfrom) / kTicksPerSecond) +
                                           " s");
            hard.clear();           // he sized them up hurt
            stage = EStage::Assess;
            return EShadowStatus::Running;
        }
        if (Player->Health() > resthealth)
        {
            resthealth = Player->Health();
            restgain = context.tick;
        }
        else if (context.tick - restgain > kRestStuckTicks)
            return Fail("his health stopped coming back resting (" + std::to_string(Player->Health()) + "/" +
                        std::to_string(Player->MaxHealth()) + ")");
        return EShadowStatus::Running;
    }

    EStage  stage = EStage::Assess;
    int32_t age = 0;
    int32_t waited = 0;
    S3DPoint home{0, 0, INT32_MIN};             // where the grind began
    int64_t restfrom = 0;
    int64_t restgain = 0;                       // when the rest last gave a point
    int32_t resthealth = 0;
    int32_t sizedat = 0;                        // the level `hard` was sized at
    SSite   site;
    std::vector<std::pair<S3DPoint, int64_t>> visited;     // sites cleared, and when
    std::set<int32_t>                         hard;        // monsters he wouldn't beat
    std::unique_ptr<TShadowTravel>            travel;
    SShadowStep                               fightstep;
    std::unique_ptr<TShadowStepRunner>        fight;
    std::unique_ptr<TShadowStepRunner>        routinerunner;
    int32_t                                   routinestep = 0;
    bool                                      restocked = false;
};

} // namespace

std::unique_ptr<TShadowStepRunner> MakeGrindRunner(const SShadowStep& step)
{
    return std::make_unique<TGrindRunner>(step);
}
