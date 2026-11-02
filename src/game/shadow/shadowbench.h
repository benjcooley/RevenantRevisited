// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *     shadowbench.h - many trial duels from one game state (calibration)  *
// *************************************************************************
//
// `--shadow-duel=<foe type>[,<foe type>...] --shadow-duel-runs=N`: the duel
// harness of the calibration suite (docs/shadow/CALIBRATION.md). The game
// is set up by the same console lines the combat arena's scenario uses
// (--exec: Locke's level and gear, the monsters added); once they have run
// and the monsters are there, the duel model throws N trial fights between
// Locke and them, seeds 1..N, and writes each fight's report as a line of
// duels.jsonl. Nothing in the world changes: trial fights work on copies.
// tools/shadow/calibrate.py sets the numbers beside the real fights'.

#pragma once

#include "game/shadow/shadowsteps.h"

#include <cstdint>
#include <string>
#include <vector>

class TCharacter;
class TShadowLog;

class TShadowDuelBench
{
  public:
    // `traced`: the first fights whose timelines are written beside the
    // reports (trace-<seed>.txt), 0 for none.
    TShadowDuelBench(std::vector<std::string> foes, int32_t runs, int32_t traced)
        : wanted(std::move(foes)), runs(runs), traced(traced)
    {
    }

    // Once a tick: waits for the set-up, then fights the trials at once.
    EShadowStatus Pulse(TShadowLog& log);
    [[nodiscard]] const std::string& Failure() const { return failure; }

  private:
    // The nearest living character of each wanted type around Locke; fewer
    // than wanted while the set-up hasn't made them all.
    [[nodiscard]] std::vector<TCharacter*> FindFoes() const;
    void Run(const std::vector<TCharacter*>& foes, TShadowLog& log) const;

    std::vector<std::string> wanted;
    int32_t                  runs    = 0;
    int32_t                  traced  = 0;
    int32_t                  waited  = 0;     // ticks the world has been idle without the foes
    std::string              failure;
};
