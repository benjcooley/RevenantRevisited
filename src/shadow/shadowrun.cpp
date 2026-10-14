// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *          shadowrun.cpp - the Shadow Smoke run (--shadow=<plan>)        *
// *************************************************************************

#include "shadowrun.h"

#include "buildinfo.h"

#include "shadowloot.h"

#include "../gameflow.h"
#include "../logging.h"
#include "../module.h"
#include "../player.h"
#include "../revenant.h"
#include "../revutils.h"

#include <sokol_app.h>          // sapp_request_quit once the run ends

#include <algorithm>
#include <cctype>
#include <ctime>
#include <filesystem>
#include <sstream>

TShadowRun Shadow;

namespace {

bool SameId(const std::string& a, const std::string& b)
{
    return a.size() == b.size() &&
           std::equal(a.begin(), a.end(), b.begin(),
                      [](char x, char y) { return std::tolower((unsigned char)x) == std::tolower((unsigned char)y); });
}

std::string Timestamp()
{
    const std::time_t now = std::time(nullptr);
    char text[32];
    std::strftime(text, sizeof(text), "%Y%m%d-%H%M%S", std::localtime(&now));
    return text;
}

} // namespace

bool TShadowRun::Configure(const SShadowOptions& given)
{
    options = given;
    std::string error;
    if (!plan.Load(options.plan, error))
    {
        log_error("[smoke] %s", error.c_str());
        return false;
    }
    if (!options.until.empty() && plan.FindStage(options.until) < 0)
    {
        log_error("[smoke] --shadow-until: the plan has no stage '%s'", options.until.c_str());
        return false;
    }
    if (!options.from.empty())
    {
        stage = plan.FindStage(options.from);
        if (stage < 0)
        {
            log_error("[smoke] --shadow-stage: the plan has no stage '%s'", options.from.c_str());
            return false;
        }
    }
    if (options.out.empty())
    {
        const std::string stem = std::filesystem::path(options.plan).stem().string();
        options.out = (std::filesystem::path(SavePath) / "shadow" /
                       (stem + "-s" + std::to_string(options.seed) + "-" + Timestamp())).string();
    }
    // Whole levels load at once, area sound stays off, and the game clock
    // follows the simulation, not the wall (revutils.h).
    SetDeterministicRun(true);
    active = true;
    log_info("[smoke] plan %s: %d stages, seed %u, out %s", options.plan.c_str(),
             (int32_t)plan.Stages().size(), options.seed, options.out.c_str());
    return true;
}

int32_t TShadowRun::StepsThisFrame()
{
    stepsstart = std::chrono::steady_clock::now();
    return finished ? 1 : steps;
}

void TShadowRun::StepsDone()
{
    // The frame's simulation alone, against the budget: double the steps
    // while well under it, halve them when over. The draw and the wait for
    // the display stay out of it.
    const auto took = std::chrono::steady_clock::now() - stepsstart;
    if (took < kFrameBudget / 2)
        steps = std::min(steps * 2, kMaxStepsPerFrame);
    else if (took > kFrameBudget)
        steps = std::max(steps / 2, 1);
}

void TShadowRun::Pulse()
{
    if (!active || finished || !Player)
        return;

    // A load replaces the world and the player: the ledger picks up the new
    // one once it's in.
    if (GameFlow.Session().Loading())
    {
        player = nullptr;
        return;
    }

    ++tick;
    log.SetTick(tick);
    if (!begun)
        Begin();

    if (Player != player)
    {
        ledger.Start(*Player, tick);
        player = Player;
    }
    else if (ledger.Audit(*Player, tick, log) > 0 && !options.lenient)
    {
        Finish(2, "the ledger found a change with no cause");
        return;
    }

    // The chests of each level Locke comes to, once: where the plan's chest
    // tours come from (ARCHITECTURE.md §8.8).
    if (!finished && !surveyed.count(Player->GetLevel()) && ShadowGameIdle())
    {
        surveyed.insert(Player->GetLevel());
        ShadowLogChests(Player->GetLevel(), log);
    }

    RunSteps();
}

void TShadowRun::Begin()
{
    begun = true;
    wallstart = std::chrono::steady_clock::now();
    if (!log.Open(options.out))
    {
        Finish(1, "can't write the run's log to " + options.out);
        return;
    }
    const TModule* module = ModuleManager.Active();
    const std::string modulename = module ? module->dirname : "";
    log.Event("run", plan.Path() + ", seed " + std::to_string(options.seed) + ", build " + kBuildName,
              [&](JsonOut& out) {
                  out.FieldString("plan", plan.Path());
                  out.Field("seed", options.seed);
                  out.FieldString("module", modulename);
                  out.FieldString("build", kBuildName);
                  out.FieldString("sha", kBuildSha);
              });
    if (!SameId(modulename, plan.Module()))
        Finish(1, "the plan is for module " + plan.Module() + ", the game runs " + modulename);
}

void TShadowRun::RunSteps()
{
    const std::vector<SShadowStage>& stages = plan.Stages();

    // Something fighting Locke comes first, whatever the step: he answers it.
    if (!interrupt && ShadowGameIdle() && ShadowUnderAttack() &&
        !(runner && step >= 0 && stage < (int32_t)stages.size() &&
          stages[stage].steps[step].verb == EShadowVerb::Fight))
    {
        interruptstep = SShadowStep{};
        interruptstep.verb = EShadowVerb::Fight;
        log.Event("interrupt", "something is fighting Locke");
        interrupt = MakeFightRunner(interruptstep);
    }
    if (interrupt)
    {
        SShadowContext context{log, ledger, nav, plan, tick,
                               stages[std::min<int32_t>(stage, (int32_t)stages.size() - 1)], waypoints};
        const EShadowStatus status = interrupt->Pulse(context);
        if (status == EShadowStatus::Running)
            return;
        if (status == EShadowStatus::Failed)
        {
            Finish(1, "a fight the plan didn't ask for: " + interrupt->Failure());
            return;
        }
        interrupt.reset();
    }

    while (!finished)
    {
        if (!runner)
        {
            if (stage >= (int32_t)stages.size())
            {
                Finish(0, "the plan is done");
                return;
            }
            const SShadowStage& current = stages[stage];
            if (step < 0)
            {
                log.Event("stage", current.id + " \"" + current.title + "\"", [&](JsonOut& out) {
                    out.FieldString("id", current.id);
                    out.FieldString("title", current.title);
                });
                step = 0;
            }
            if (step >= (int32_t)current.steps.size())
            {
                log.Event("stage-done", current.id, [&](JsonOut& out) { out.FieldString("id", current.id); });
                stagesdone.push_back(current.id);
                if (!options.until.empty() && SameId(current.id, options.until))
                {
                    Finish(0, "stopped after stage " + current.id);
                    return;
                }
                ++stage;
                step = -1;
                continue;
            }
            const SShadowStep& next = current.steps[step];
            log.Event("step", next.Describe(), [&](JsonOut& out) {
                out.Field("line", next.line);
                out.FieldString("step", next.Describe());
            });
            runner = MakeStepRunner(next);
        }

        SShadowContext context{log, ledger, nav, plan, tick, stages[stage], waypoints};
        const EShadowStatus status = runner->Pulse(context);
        if (status == EShadowStatus::Running)
            return;
        if (status == EShadowStatus::Failed)
        {
            const SShadowStep& failed = stages[stage].steps[step];
            Finish(1, plan.Path() + ":" + std::to_string(failed.line) + ": " + failed.Describe() + ": " +
                          runner->Failure());
            return;
        }
        runner.reset();
        ++step;
    }
}

void TShadowRun::Finish(int32_t code, const std::string& why)
{
    if (finished)
        return;
    finished = true;
    exitcode = code;
    static constexpr const char* kResults[] = {"pass", "fail", "violation"};
    log.Event("end", std::string(kResults[std::clamp(code, 0, 2)]) + ": " + why, [&](JsonOut& out) {
        out.Field("code", code);
        out.FieldString("why", why);
        out.Field("violations", ledger.Violations());
    });
    ledger.Stop();
    WriteWaypoints();
    WriteSummary(why);
    log.Close();
    sapp_request_quit();
}

void TShadowRun::WriteSummary(const std::string& why) const
{
    const double wall = std::chrono::duration<double>(std::chrono::steady_clock::now() - wallstart).count();
    std::ostringstream out;
    out << "Shadow Smoke: " << (exitcode == 0 ? "PASS" : exitcode == 1 ? "FAIL" : "VIOLATION") << "\n"
        << "  " << why << "\n\n"
        << "plan      " << plan.Path() << "\n"
        << "seed      " << options.seed << "\n"
        << "build     " << kBuildName << "\n"
        << "game time " << tick / 24 / 60 << " min " << tick / 24 % 60 << " s (" << tick << " ticks)\n"
        << "wall time " << (int64_t)wall << " s\n";
    if (player)
    {
        const SShadowSnapshot& locke = ledger.Last();
        out << "Locke     level " << locke.level << ", " << locke.exp << " exp, " << locke.gold << " gold, health "
            << locke.health << ", map level " << locke.maplevel << " at (" << locke.pos.x << ", " << locke.pos.y
            << ", " << locke.pos.z << ")\n";
    }
    out << "ledger    " << ledger.Violations() << " violations\n\nstages done:";
    for (const std::string& id : stagesdone)
        out << ' ' << id;
    out << "\n\nwaypoints:\n";
    for (const SShadowWaypoint& w : waypoints)
        out << "  " << w.stage << ": \"" << w.name << "\" in " << w.dir.string() << " (tick " << w.tick << ")\n";
    log.WriteFile("summary.txt", out.str());
}

void TShadowRun::WriteWaypoints() const
{
    JsonOut out;
    out.Begin('{');
    out.FieldString("plan", plan.Path());
    out.Field("seed", options.seed);
    out.FieldString("build", kBuildName);
    out.FieldString("sha", kBuildSha);
    out.Key("waypoints").Begin('[');
    for (const SShadowWaypoint& w : waypoints)
    {
        out.Begin('{');
        out.FieldString("name", w.name);
        out.FieldString("slot", w.slot);
        out.FieldString("dir", w.dir.string());
        out.FieldString("stage", w.stage);
        out.Field("tick", w.tick);
        out.Key("player").Begin('{');
        out.Field("level", w.maplevel);
        out.Key("pos").Begin('[').Value(w.pos.x).Value(w.pos.y).Value(w.pos.z).End(']');
        out.Key("health").Begin('[').Value(w.health[0]).Value(w.health[1]).End(']');
        out.Key("mana").Begin('[').Value(w.mana[0]).Value(w.mana[1]).End(']');
        out.Key("fatigue").Begin('[').Value(w.fatigue[0]).Value(w.fatigue[1]).End(']');
        out.Key("xp").Begin('[').Value(w.xp[0]).Value(w.xp[1]).Value(w.xp[2]).End(']');
        out.Field("gold", w.gold);
        out.End('}');
        out.End('}');
    }
    out.End(']');
    out.End('}');
    log.WriteFile("waypoints.json", out.str() + "\n");
}
