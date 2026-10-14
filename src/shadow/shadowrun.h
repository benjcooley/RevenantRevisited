// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *          shadowrun.h - the Shadow Smoke run (--shadow=<plan>)          *
// *************************************************************************
//
// Shadow Smoke plays the game from a plan, headless and deterministic
// (docs/shadow/ARCHITECTURE.md). TShadowRun is the driver: the play screen
// pulses it once a tick, before anything else in the tick. Each pulse it
// audits Locke (TShadowLedger), then runs the plan's current step until the
// plan is done or a step fails. It also sets the pace: under --shadow each
// drawn frame runs as many simulation steps as fit a budget (turbo).

#pragma once

#include "shadowbench.h"
#include "shadowledger.h"
#include "shadowlog.h"
#include "shadowplan.h"
#include "shadowsteps.h"
#include "shadowtravel.h"

#include <chrono>
#include <cstdint>
#include <memory>
#include <map>
#include <set>
#include <string>
#include <vector>

struct SShadowOptions
{
    std::string plan;               // --shadow=<plan>
    std::string out;                // --shadow-out=<dir>; default <SavePath>/shadow/<run>
    std::string from;               // --shadow-stage=<stage>: start at it (with --quickstart=<its save>)
    std::string startslot;          // that --quickstart save: the newest save a death goes back to
    std::string until;              // --shadow-until=<stage>: stop after it
    uint32_t    seed    = 1;        // --seed
    bool        lenient = false;    // --shadow-lenient: log violations, don't stop
    // --shadow-verify=<file>: no plan; once the save --quickstart names has
    // loaded and settled, Locke's state (shadowstate.h) goes to <file> and
    // the game quits -- the reload half of the save round trip (§10).
    std::string verify;
    std::string style;              // --shadow-style=<melee|hands|bow|spells|mixed>: over the plan's
    // --shadow-duel=<foe type>[,...]: the calibration's duel harness in place
    // of a plan (shadowbench.h), --shadow-duel-runs=N trial fights.
    std::vector<std::string> duel;
    int32_t     duelruns = 200;
    int32_t     dueltrace = 0;      // --shadow-duel-trace=N: the first N fights' timelines
};

class TShadowRun
{
  public:
    // From the command line, before the game boots: reads the plan. False
    // (logged) when the run can't go ahead.
    bool Configure(const SShadowOptions& options);
    [[nodiscard]] bool Active() const { return active; }

    // revmain: how many simulation steps the coming frame runs before it
    // draws, and that they are done. The count adapts so a frame's
    // simulation takes about kFrameBudget of wall time.
    [[nodiscard]] int32_t StepsThisFrame();
    void StepsDone();

    // TPlayScreen::Update, first thing each tick.
    void Pulse();

    // The process's exit code once the run has ended: 0 passed, 1 a step
    // failed, 2 a ledger violation.
    [[nodiscard]] int32_t ExitCode() const { return exitcode; }

  private:
    void Begin();
    void RunSteps();
    void RunBench();
    void Finish(int32_t code, const std::string& why);
    void WriteSummary(const std::string& why) const;
    void WriteWaypoints() const;
    void Verify();
    // Locke is dead: the newest save is loaded and the plan goes on after
    // it, or the run fails (§6.6).
    void Died(const std::string& how);

    // The window is there to be watched, not played: a draw costs tens of
    // milliseconds on a big level, so the simulation gets most of the frame
    // and the run draws a handful of frames a second.
    static constexpr std::chrono::milliseconds kFrameBudget{100};
    static constexpr int32_t kMaxStepsPerFrame = 4096;

    SShadowOptions options;
    TShadowPlan    plan;
    TShadowLog     log;
    TShadowLedger  ledger;
    TShadowNav     nav;
    bool           active   = false;
    bool           begun    = false;
    bool           finished = false;
    int32_t        exitcode = 0;
    int64_t        tick     = 0;
    TPlayer*       player   = nullptr;     // the one the ledger follows

    int32_t        stage = 0;
    int32_t        step  = -1;
    std::unique_ptr<TShadowStepRunner> runner;
    // A step the plan didn't ask for -- a fight (§6.5), trying new spells
    // (§8.6): it runs first, then the plan's step goes on.
    SShadowStep                        interruptstep;
    std::unique_ptr<TShadowStepRunner> interrupt;
    // --shadow-duel: the duel harness runs instead of the plan.
    std::unique_ptr<TShadowDuelBench>  bench;
    std::vector<SShadowWaypoint>       waypoints;
    std::vector<std::string>           stagesdone;
    std::set<int32_t>                  surveyed;   // levels whose chests are logged
    int32_t                            tidied = -1; // the ledger's items version the pack was put in order at
    int32_t                            settled = 0; // verifying: idle ticks since the load
    int32_t                            inventedat = -1; // the items version spells were last looked for at
    // The newest save: its slot and the step after it.
    struct SSavePoint
    {
        std::string slot;
        int32_t     stage = 0;
        int32_t     step  = 0;
    };
    SSavePoint                         lastsave;
    bool                               reloading = false;  // a death's load is under way
    bool                               settling  = false;  // ... done; waiting for the game to be idle
    std::map<std::string, int32_t>     deaths;     // by stage
    static constexpr int32_t           kDeathsAllowed = 1;     // a stage's second death fails the run

    // Turbo: the steps a frame runs, and when this frame's began.
    int32_t                               steps = 1;
    std::chrono::steady_clock::time_point stepsstart{};
    std::chrono::steady_clock::time_point wallstart{};
};

extern TShadowRun Shadow;
