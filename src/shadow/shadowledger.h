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
//   land before it teleports him, and the jump must land there.
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

    // Compares the player with the last snapshot and logs every change with
    // its cause. Returns the number of violations found.
    int32_t Audit(TPlayer& player, int64_t tick, TShadowLog& log);

    [[nodiscard]] int32_t Violations() const { return violations; }
    [[nodiscard]] const SShadowSnapshot& Last() const { return last; }

    // TScript::IStepObserver: which scripts ran lines since the last audit.
    void BeforeLine(TScript& script, int32_t offset) override;
    void AfterLine(TScript& script, int32_t bits) override {}

  private:
    void Violation(TShadowLog& log, const char* what, const std::string& text);
    [[nodiscard]] std::string ScriptCause() const;
    [[nodiscard]] bool LandedOnExit(const SShadowSnapshot& now, std::string& exitname) const;
    [[nodiscard]] bool LandedOnTravel(const SShadowSnapshot& now);

    struct STravel
    {
        S3DPoint landing{};
        int32_t  level = 0;
        int64_t  until = 0;
    };

    SShadowSnapshot               last;
    bool                          started    = false;
    std::set<std::string>         scripts;          // run since the last audit
    std::vector<STravel>          travels;          // proven, not yet landed
    int32_t                       violations = 0;
};
