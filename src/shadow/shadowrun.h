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

#include "shadowledger.h"
#include "shadowlog.h"
#include "shadowplan.h"
#include "shadowsteps.h"
#include "shadowtravel.h"

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

struct SShadowOptions
{
    std::string plan;               // --shadow=<plan>
    std::string out;                // --shadow-out=<dir>; default <SavePath>/shadow/<run>
    std::string from;               // --shadow-stage=<stage>: start at it (with --quickstart=<its save>)
    std::string until;              // --shadow-until=<stage>: stop after it
    uint32_t    seed    = 1;        // --seed
    bool        lenient = false;    // --shadow-lenient: log violations, don't stop
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
    void Finish(int32_t code, const std::string& why);
    void WriteSummary(const std::string& why) const;
    void WriteWaypoints() const;

    static constexpr std::chrono::milliseconds kFrameBudget{12};
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
    std::vector<SShadowWaypoint>       waypoints;
    std::vector<std::string>           stagesdone;

    // Turbo: the steps a frame runs, and when this frame's began.
    int32_t                               steps = 1;
    std::chrono::steady_clock::time_point stepsstart{};
    std::chrono::steady_clock::time_point wallstart{};
};

extern TShadowRun Shadow;
