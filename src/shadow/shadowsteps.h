// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *       shadowsteps.h - Shadow Smoke's plan steps, as they run           *
// *************************************************************************
//
// Each step of a plan (shadowplan.h) runs as a TShadowStepRunner, pulsed
// once a tick until it is done or fails. Steps act only through what a
// player does: keys through the input queue, the functions a click on the
// world reaches, and the save request (docs/shadow/ARCHITECTURE.md §3).

#pragma once

#include "shadowplan.h"

#include "../revtypes.h"

#include <array>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

class TCharacter;
class TShadowLedger;
class TShadowLog;
class TShadowNav;
class TShadowPlan;

enum class EShadowStatus : uint8_t { Running, Done, Failed };

// A milestone save the run made, for the waypoint catalogue
// (tools/shadow/shadow.py files it).
struct SShadowWaypoint
{
    std::string           name;
    std::string           slot;
    std::filesystem::path dir;
    std::string           stage;
    int64_t               tick = 0;
    // Locke as he was saved, for the catalogue's manifest.
    int32_t               maplevel = 0;
    S3DPoint              pos{};
    std::array<int32_t, 2> health{}, mana{}, fatigue{};   // value, maximum
    std::array<int32_t, 3> xp{};                          // level, experience, next level's
    int32_t               gold = 0;
};

// What a step works with while it runs.
struct SShadowContext
{
    TShadowLog&                   log;
    TShadowLedger&                ledger;
    TShadowNav&                   nav;
    const TShadowPlan&            plan;
    int64_t                       tick = 0;
    const SShadowStage&           stage;
    std::vector<SShadowWaypoint>& waypoints;
};

class TShadowStepRunner
{
  public:
    explicit TShadowStepRunner(const SShadowStep& step) : step(step) {}
    virtual ~TShadowStepRunner() = default;
    TShadowStepRunner(const TShadowStepRunner&) = delete;
    TShadowStepRunner& operator=(const TShadowStepRunner&) = delete;

    // One tick of the step.
    virtual EShadowStatus Pulse(SShadowContext& context) = 0;
    // Why it failed.
    [[nodiscard]] const std::string& Failure() const { return failure; }

  protected:
    EShadowStatus Fail(std::string why)
    {
        failure = std::move(why);
        return EShadowStatus::Failed;
    }

    const SShadowStep& step;

  private:
    std::string failure;
};

[[nodiscard]] std::unique_ptr<TShadowStepRunner> MakeStepRunner(const SShadowStep& step);

// Nothing holds the game: control is Locke's, and no movie, fade, load,
// menu or choice list is up.
[[nodiscard]] bool ShadowGameIdle();

// What `condition` reads, from the game, into `actual`; false with `why`
// when there is nothing to read (a story variable state.def doesn't have).
[[nodiscard]] bool ShadowRead(const SShadowCondition& condition, int32_t& actual, std::string& why);

// A key pressed and released, as a player's keyboard sends it: through the
// input queue, which turbo drains before every tick.
void ShadowPressKey(int32_t vk);

// The shop steps (shadowshop.cpp).
[[nodiscard]] std::unique_ptr<TShadowStepRunner> MakeShopRunner(const SShadowStep& step);
// A fight, planned or not (shadowfight.cpp): the step's target (none: no
// one in particular), `foes` (the monsters a walk met), and whoever is
// fighting Locke.
[[nodiscard]] std::unique_ptr<TShadowStepRunner> MakeFightRunner(const SShadowStep& step,
                                                                 std::vector<TCharacter*> foes = {});
// Grinding for a level (shadowgrind.cpp).
[[nodiscard]] std::unique_ptr<TShadowStepRunner> MakeGrindRunner(const SShadowStep& step);
// Whether anything near Locke is fighting him (§6.5).
[[nodiscard]] bool ShadowUnderAttack();
