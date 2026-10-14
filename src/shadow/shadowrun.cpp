// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *          shadowrun.cpp - the Shadow Smoke run (--shadow=<plan>)        *
// *************************************************************************

#include "shadowrun.h"

#include "buildinfo.h"

#include "shadowloot.h"
#include "shadowpack.h"
#include "shadowpurse.h"
#include "shadowstate.h"

#include "../gameflow.h"
#include "../logging.h"
#include "../module.h"
#include "../player.h"
#include "../playscreen.h"
#include "../revenant.h"
#include "../revutils.h"

#include <sokol_app.h>          // sapp_request_quit once the run ends

#include <algorithm>
#include <cctype>
#include <ctime>
#include <filesystem>
#include <fstream>
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
    if (!options.verify.empty())
    {
        SetDeterministicRun(true);
        active = true;
        log_info("[smoke] verifying the save: Locke's state to %s", options.verify.c_str());
        return true;
    }
    if (!options.duel.empty())
    {
        // The duel harness: no plan, no ledger (the scenario's console lines
        // set Locke up, which no verb explains), the same turbo.
        if (options.duelruns < 1)
        {
            log_error("[smoke] --shadow-duel-runs must be at least 1");
            return false;
        }
        bench = std::make_unique<TShadowDuelBench>(options.duel, options.duelruns, options.dueltrace);
        if (options.out.empty())
            options.out = (std::filesystem::path(SavePath) / "shadow" / ("duel-s" + std::to_string(options.seed) + "-" +
                                                                         Timestamp())).string();
        SetDeterministicRun(true);
        active = true;
        log_info("[smoke] duel harness: %d trials against %d foe(s), out %s", options.duelruns,
                 (int32_t)options.duel.size(), options.out.c_str());
        return true;
    }
    std::string error;
    if (!plan.Load(options.plan, error))
    {
        log_error("[smoke] %s", error.c_str());
        return false;
    }
    if (!options.style.empty())
    {
        const std::optional<EShadowStyle> style = ShadowParseStyle(options.style);
        if (!style)
        {
            log_error("[smoke] --shadow-style: '%s' isn't melee, hands, bow, spells or mixed", options.style.c_str());
            return false;
        }
        plan.SetStyle(*style);
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
        if (!options.startslot.empty())
            lastsave = {options.startslot, stage, 0};
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
    if (!options.verify.empty())
    {
        Verify();
        return;
    }
    log.SetTick(tick);
    if (!begun)
        Begin();
    if (bench)
    {
        RunBench();
        return;
    }

    if (Player != player)
    {
        ledger.Start(*Player, tick);
        player = Player;
        reloading = false;
    }
    else if (reloading)
        return;             // the load hasn't begun yet: the dead Locke is still the player
    else if (ledger.Audit(*Player, tick, log) > 0 && !options.lenient)
    {
        Finish(2, "the ledger found a change with no cause");
        return;
    }

    // A death: back to the newest save, as a player (§6.6).
    if (Player->Health() <= 0)
    {
        Died("Locke died");
        return;
    }

    // Whatever came into or left the pack, it is put in order as a player
    // keeps it, and the best armour is put on -- not mid-trade, while the
    // shop's drawer lists the pack (§8.9, §8.10). (The shop pane stays
    // armed after a visit: its script sets it up again with each menu.)
    if (!finished && ledger.ItemsVersion() != tidied && ShadowGameIdle() &&
        PlayScreen.Drawer() != TPlayScreen::EDrawer::BuySell)
    {
        ShadowTidyPack(*Player, log);
        ShadowReadyArmour(*Player, log);
        tidied = ledger.ItemsVersion();
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
    if (bench)
    {
        log.Event("run", "duel harness, seed " + std::to_string(options.seed) + ", build " + kBuildName,
                  [&](JsonOut& out) {
                      out.Field("seed", options.seed);
                      out.FieldString("module", modulename);
                      out.FieldString("build", kBuildName);
                      out.FieldString("sha", kBuildSha);
                  });
        return;
    }
    const std::string style = kShadowStyleNames[size_t(plan.Style())];
    log.Event("run", plan.Path() + ", seed " + std::to_string(options.seed) + ", style " + style + ", build " +
                         kBuildName,
              [&](JsonOut& out) {
                  out.FieldString("plan", plan.Path());
                  out.Field("seed", options.seed);
                  out.FieldString("style", style);
                  out.FieldString("module", modulename);
                  out.FieldString("build", kBuildName);
                  out.FieldString("sha", kBuildSha);
              });
    if (!SameId(modulename, plan.Module()))
        Finish(1, "the plan is for module " + plan.Module() + ", the game runs " + modulename);
}

void TShadowRun::RunBench()
{
    const EShadowStatus status = bench->Pulse(log);
    if (status == EShadowStatus::Done)
        Finish(0, "the trial duels are done");
    else if (status == EShadowStatus::Failed)
        Finish(1, "duel harness: " + bench->Failure());
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
            const std::string why = "a step the plan didn't ask for (" + interruptstep.Describe() + "): " +
                                    interrupt->Failure();
            if (Player->Health() <= 0)
                Died(why);
            else
                Finish(1, why);
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
            // Between steps, with something new in the pack: a combination
            // of his talismans that makes a spell he doesn't know is tried,
            // as a player experiments in the spell pane (§8.6).
            if (ledger.ItemsVersion() != inventedat && ShadowGameIdle())
            {
                inventedat = ledger.ItemsVersion();
                if (ShadowSpellToInvent({}, nullptr))
                {
                    interruptstep = SShadowStep{};
                    interruptstep.verb = EShadowVerb::Invent;
                    interrupt = MakeInventRunner(interruptstep);
                    return;
                }
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
            const std::string why = plan.Path() + ":" + std::to_string(failed.line) + ": " + failed.Describe() +
                                    ": " + runner->Failure();
            if (Player->Health() <= 0)
                Died(why);
            else
                Finish(1, why);
            return;
        }
        // The newest save is where a death goes back to: the step after it.
        if (stages[stage].steps[step].verb == EShadowVerb::Save)
            lastsave = {"Shadow " + stages[stage].steps[step].name, stage, step + 1};
        runner.reset();
        ++step;
    }
}

void TShadowRun::Verify()
{
    // The game's own settling first: the load done, control Locke's, and a
    // second more for whatever the level's scripts do on arrival.
    constexpr int32_t kSettleTicks = 24;
    if (!ShadowGameIdle())
    {
        settled = 0;
        return;
    }
    if (++settled < kSettleTicks)
        return;
    std::ofstream file(options.verify, std::ios::binary);
    file << ShadowStateJson(*Player) << "\n";
    finished = true;
    exitcode = file ? 0 : 1;
    if (!file)
        log_error("[smoke] can't write %s", options.verify.c_str());
    sapp_request_quit();
}

void TShadowRun::Died(const std::string& how)
{
    const std::string where = stage < (int32_t)plan.Stages().size() ? plan.Stages()[stage].id : "the end";
    const int32_t times = ++deaths[where];
    log.Event("death", how, [&](JsonOut& out) {
        out.FieldString("stage", where);
        out.Field("times", times);
    });
    // A balanced plan doesn't lose a fight it chose twice (§6.6).
    if (lastsave.slot.empty())
    {
        Finish(1, how + " (no save to go back to)");
        return;
    }
    if (times > kDeathsAllowed)
    {
        Finish(1, how + " (the second death in stage " + where + ")");
        return;
    }
    log.Event("reload", "\"" + lastsave.slot + "\", back to stage " + plan.Stages()[lastsave.stage].id);
    GameFlow.Session().RequestLoad(lastsave.slot, false);
    reloading = true;
    runner.reset();
    interrupt.reset();
    stage = lastsave.stage;
    step = lastsave.step;
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
    if (!bench)
    {
        log.WriteFile("coverage.json", ledger.Coverage().Json() + "\n");
        WriteWaypoints();
        WriteSummary(why);
    }
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
        << "style     " << kShadowStyleNames[size_t(plan.Style())] << "\n"
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
        out.Key("state").Raw(w.state);
        out.End('}');
    }
    out.End(']');
    out.End('}');
    log.WriteFile("waypoints.json", out.str() + "\n");
}
