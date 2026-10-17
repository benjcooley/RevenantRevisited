// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *       shadowledger.h - Shadow Smoke's audit of every change to Locke   *
// *************************************************************************
//
// Locke may not cheat (docs/shadow/ARCHITECTURE.md §1.1, §4). The ledger
// takes a snapshot of him every tick and matches each change since the
// last one to a cause:
//
// - a script line ran (the script engine's step observer): scripts are the
//   game, and may give, take, move and raise anything;
// - the game clock: health, mana and fatigue regenerate at the rules.def
//   rates, and a level-up refills them;
// - an exit: a jump that lands on an exit.def target;
// - a travel Shadow proved (TShadowTravel): it registers where Locke will
//   land before it teleports him, and the jump must land there;
// - a take from a chest (and later a pick-up or a purchase): it registers
//   what it will bring, and gains beyond that are violations.
//
// The story variables (state.def) are part of the snapshot too: each change
// is logged with the script that made it, which is how a run shows where
// the story advanced (docs/shadow/ARCHITECTURE.md §9.5). Only a script may
// change one.
//
// Losses (damage taken, items gone) never help Locke: they are logged, and
// an item lost without a cause is a finding, not a violation. A gain or a
// jump with no cause is a violation. So is any cheat flag.

#pragma once

#include "shadowcoverage.h"

#include "../charstats.h"
#include "../script.h"

#include <array>
#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <vector>

class TPlayer;
class TShadowLog;

// Locke as the ledger compares him, tick to tick.
struct SShadowSnapshot
{
    int64_t  tick        = 0;
    S3DPoint pos{};
    int32_t  maplevel    = 0;
    int32_t  health      = 0;
    int32_t  mana        = 0;
    int32_t  fatigue     = 0;
    int32_t  gold        = 0;
    int32_t  level       = 0;
    int32_t  exp         = 0;
    int32_t  attacklevel = 0;
    std::array<int32_t, NUM_PLRSTATS> stats{};
    std::array<int32_t, NUM_SKILLS>   skills{};
    std::array<int32_t, NUM_SKILLS>   skillexp{};
    std::array<int32_t, 3>            levelup{};
    // Everything he carries, bags included, by lower-case name; gold is
    // `gold`, not an item here.
    std::map<std::string, int32_t> items;
    // The story variables, by their index in the game state list.
    std::vector<int32_t> states;

    static SShadowSnapshot Take(TPlayer& player, int64_t tick);
};

class TShadowLedger final : public TScript::IStepObserver
{
  public:
    // From the player as he is now: the first snapshot. Installs the step
    // observer.
    void Start(TPlayer& player, int64_t tick);
    // Removes the step observer.
    void Stop();

    // A proven travel is about to put Locke at `landing` on `level`; the
    // jump is accepted until tick `until`.
    void AllowTravel(const S3DPoint& landing, int32_t level, int64_t until);
    // A verb is about to bring Locke these items (by lower-case name) and
    // this much gold: gains up to them are accepted until tick `until`.
    void AllowItems(std::string cause, std::map<std::string, int32_t> items, int32_t gold, int64_t until);
    // Training is setting the level-up picks: they may change until `until`.
    void AllowTraining(int64_t until) { traininguntil = until; }
    // A fight's kills earn experience, skill experience and level-ups (and
    // a level-up's attributes and refill) until `until`.
    void AllowProgress(std::string cause, int64_t until)
    {
        progresscause = std::move(cause);
        progressuntil = until;
    }
    // Food or a potion is about to give health and mana: up to these, above
    // regeneration, until `until`.
    void AllowVitals(std::string cause, int32_t health, int32_t mana, int64_t until);

    // Compares the player with the last snapshot and logs every change with
    // its cause. Returns the number of violations found.
    int32_t Audit(TPlayer& player, int64_t tick, TShadowLog& log);

    [[nodiscard]] int32_t Violations() const { return violations; }
    // Clicked whenever the count of something he carries changes (a move
    // between slots or bags isn't one).
    [[nodiscard]] int32_t ItemsVersion() const { return itemsversion; }
    [[nodiscard]] const SShadowSnapshot& Last() const { return last; }
    // Every script line run and story variable changed (§9.4, §9.5).
    [[nodiscard]] const TShadowCoverage& Coverage() const { return coverage; }

    // TScript::IStepObserver: which scripts ran lines since the last audit,
    // and each line for the coverage.
    void BeforeLine(TScript& script, int32_t offset) override;
    void AfterLine(TScript& script, int32_t bits) override {}

  private:
    void Violation(TShadowLog& log, const char* what, const std::string& text);
    [[nodiscard]] std::string ScriptCause() const;
    [[nodiscard]] bool LandedOnExit(const SShadowSnapshot& now, std::string& exitname) const;
    [[nodiscard]] bool LandedOnTravel(const SShadowSnapshot& now);

    // How much of a gain open allowances explain: takes it from them and
    // returns the rest, and the causes it came from.
    int32_t Explain(const std::string& item, int32_t gain, std::string& causes);

    struct STravel
    {
        S3DPoint landing{};
        int32_t  level = 0;
        int64_t  until = 0;
    };
    struct SItems
    {
        std::string                    cause;
        std::map<std::string, int32_t> items;   // "gold" for gold
        int64_t                        until = 0;
    };

    SShadowSnapshot               last;
    TShadowCoverage               coverage;
    int64_t                       audittick  = 0;    // the tick being run: the last audited
    bool                          started    = false;
    std::set<std::string>         scripts;          // run since the last audit
    std::vector<STravel>          travels;          // proven, not yet landed
    std::vector<SItems>           gifts;            // registered, not yet arrived
    int64_t                       traininguntil = -1;
    int64_t                       progressuntil = -1;
    std::string                   progresscause;
    struct SVitals
    {
        std::string cause;
        int32_t     health = 0;
        int32_t     mana   = 0;
        int64_t     until  = 0;
    };
    std::vector<SVitals>          meals;
    int32_t                       violations = 0;
    int32_t                       itemsversion = 0;
};
