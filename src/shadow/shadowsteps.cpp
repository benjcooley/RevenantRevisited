// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *       shadowsteps.cpp - Shadow Smoke's plan steps, as they run         *
// *************************************************************************

#include "shadowsteps.h"

#include "shadowlog.h"
#include "shadowtravel.h"

#include "../dialog.h"
#include "../gameflow.h"
#include "../inputqueue.h"
#include "../mappane.h"
#include "../player.h"
#include "../playscreen.h"
#include "../revenant.h"
#include "../revtypes.h"
#include "../savegame.h"
#include "../script.h"
#include "../textencoding.h"

#include <cmath>

namespace {

constexpr int32_t kTicksPerSecond = 24;
// A scene that hasn't begun this long after its step started never will.
constexpr int32_t kSceneStartTicks = 20 * kTicksPerSecond;
// Control back for this long, nothing up: the scene is over. Scenes give
// control back for a moment between their parts (STORY.md S4).
constexpr int32_t kSettleTicks = 2 * kTicksPerSecond;
// No scene in the game runs this long.
constexpr int32_t kSceneLimitTicks = 30 * 60 * kTicksPerSecond;

void PressKey(int32_t vk)
{
    InputQueue.PushKey(vk, true);
    InputQueue.PushKey(vk, false);
}

// The choices of the menu up now, as the game shows them.
std::vector<std::string> MenuTexts()
{
    const auto& entries = DialogPane.Entries();
    for (auto it = entries.rbegin(); it != entries.rend(); ++it)
        if ((*it)->Mode() == TDialogEntry::EMode::Responses)
            return (*it)->Texts();
    return {};
}

// Watches a scene through: skips its movies as a player does, answers its
// menus from a list, and ends once control has been back for a while.
class TSceneWatch
{
  public:
    // `mustplay`: a scene has to start (a `scene` step); otherwise none
    // starting is fine (what a `use` sets off may be no scene at all).
    explicit TSceneWatch(std::vector<int32_t> answers, bool mustplay = true)
        : answers(std::move(answers)), mustplay(mustplay)
    {
    }

    EShadowStatus Pulse(SShadowContext& context, std::string& failure)
    {
        ++age;
        if (age > kSceneLimitTicks)
        {
            failure = "the scene ran past " + std::to_string(kSceneLimitTicks / kTicksPerSecond / 60) + " minutes";
            return EShadowStatus::Failed;
        }

        // A movie: Escape, once each.
        if (PlayScreen.PlayingMovie())
        {
            started = true;
            if (!skipping)
            {
                context.log.Event("movie", "skipped");
                PressKey(VK_ESCAPE);
                skipping = true;
            }
            return EShadowStatus::Running;
        }
        skipping = false;

        // A menu: the next answer, once each.
        if (DialogPane.IsShowingResponses())
        {
            started = true;
            settled = 0;
            if (answering)
                return EShadowStatus::Running;
            const std::vector<std::string> texts = MenuTexts();
            if (next >= answers.size())
            {
                failure = "a menu with no answer planned: " + Joined(texts);
                return EShadowStatus::Failed;
            }
            const int32_t answer = answers[next++];
            if (answer > (int32_t)texts.size())
            {
                failure = "answer " + std::to_string(answer) + " to a menu of " + std::to_string(texts.size()) +
                          ": " + Joined(texts);
                return EShadowStatus::Failed;
            }
            const std::string chosen = ToUtf8(texts[answer - 1].c_str());
            context.log.Event("choose", std::to_string(answer) + " " + chosen, [&](JsonOut& out) {
                out.Field("answer", answer);
                out.Key("menu").Begin('[');
                for (const std::string& text : texts)
                    out.String(ToUtf8(text.c_str()));
                out.End(']');
            });
            PressKey('0' + answer);
            answering = true;
            return EShadowStatus::Running;
        }
        answering = false;

        if (!ShadowGameIdle())
        {
            started = true;
            settled = 0;
            return EShadowStatus::Running;
        }

        if (!started)
        {
            if (!mustplay && age > kSettleTicks)
                return EShadowStatus::Done;
            if (age > kSceneStartTicks)
            {
                failure = "no scene started";
                return EShadowStatus::Failed;
            }
            return EShadowStatus::Running;
        }
        if (++settled < kSettleTicks)
            return EShadowStatus::Running;
        if (next < answers.size())
        {
            failure = std::to_string(answers.size() - next) + " planned answers left over";
            return EShadowStatus::Failed;
        }
        return EShadowStatus::Done;
    }

  private:
    static std::string Joined(const std::vector<std::string>& texts)
    {
        std::string out;
        for (size_t i = 0; i < texts.size(); ++i)
            out += (i ? " | " : "") + std::to_string(i + 1) + " " + ToUtf8(texts[i].c_str());
        return out;
    }

    std::vector<int32_t> answers;
    bool    mustplay  = true;
    size_t  next      = 0;
    int32_t age       = 0;
    int32_t settled   = 0;
    bool    started   = false;
    bool    skipping  = false;
    bool    answering = false;
};

class TStartRunner final : public TShadowStepRunner
{
  public:
    using TShadowStepRunner::TShadowStepRunner;
    EShadowStatus Pulse(SShadowContext&) override { return EShadowStatus::Done; }
};

class TSceneRunner final : public TShadowStepRunner
{
  public:
    explicit TSceneRunner(const SShadowStep& step) : TShadowStepRunner(step), watch(step.choices) {}

    EShadowStatus Pulse(SShadowContext& context) override
    {
        std::string why;
        const EShadowStatus status = watch.Pulse(context, why);
        return status == EShadowStatus::Failed ? Fail(why) : status;
    }

  private:
    TSceneWatch watch;
};

class TTravelRunner final : public TShadowStepRunner
{
  public:
    explicit TTravelRunner(const SShadowStep& step)
        : TShadowStepRunner(step), travel(step.target, step.evade, TShadowTravel::EArrive::Onto)
    {
    }

    EShadowStatus Pulse(SShadowContext& context) override
    {
        std::string why;
        const EShadowStatus status = travel.Pulse(context, why);
        return status == EShadowStatus::Failed ? Fail(why) : status;
    }

  private:
    TShadowTravel travel;
};

// Goes to an object, then acts on it: talks to an NPC (and answers its
// menus), or uses a door, a lever, a chest, with an item or without.
class TApproachRunner final : public TShadowStepRunner
{
  public:
    explicit TApproachRunner(const SShadowStep& step)
        : TShadowStepRunner(step), travel(step.target, false, TShadowTravel::EArrive::Beside),
          watch(step.choices, step.verb == EShadowVerb::Talk)
    {
    }

    EShadowStatus Pulse(SShadowContext& context) override
    {
        std::string why;
        if (!acted)
        {
            const EShadowStatus status = travel.Pulse(context, why);
            if (status == EShadowStatus::Failed)
                return Fail(why);
            if (status == EShadowStatus::Running)
                return status;
            if (!Act(context, why))
                return Fail(why);
            acted = true;
            return EShadowStatus::Running;
        }
        const EShadowStatus status = watch.Pulse(context, why);
        return status == EShadowStatus::Failed ? Fail(step.Describe() + ": " + why) : status;
    }

  private:
    bool Act(SShadowContext& context, std::string& why)
    {
        TObjectInstance* target = travel.Object();
        if (!target)
            target = ShadowFindObject(step.target, Player->GetLevel(), Player->Pos());
        if (!target)
        {
            why = "no " + step.target + " here";
            return false;
        }
        int32_t with = -1;
        if (!step.item.empty())
        {
            TObjectInstance* item = Player->FindObjInventory(step.item.c_str());
            if (!item)
            {
                why = "Locke has no " + step.item;
                return false;
            }
            with = item->GetMapIndex();
        }
        context.log.Event(step.verb == EShadowVerb::Talk ? "talk" : "use", step.Describe());
        target->Use(Player, with);
        return true;
    }

    TShadowTravel travel;
    TSceneWatch   watch;
    bool          acted = false;
};

class TExpectRunner final : public TShadowStepRunner
{
  public:
    using TShadowStepRunner::TShadowStepRunner;

    EShadowStatus Pulse(SShadowContext& context) override
    {
        const SShadowCondition& c = step.condition;
        int32_t actual = 0;
        switch (c.subject)
        {
            case EShadowSubject::GameState:
                actual = ScriptManager.GameState(c.name.c_str());
                if (actual == STATE_INVALID)
                    return Fail("no story variable " + c.name);
                break;
            case EShadowSubject::Level:
                actual = Player->Level();
                break;
            case EShadowSubject::MapLevel:
                actual = Player->GetLevel();
                break;
            case EShadowSubject::Have:
                actual = Player->GetInventoryAmount(c.name.c_str());
                break;
        }
        const std::string text = c.Describe() + " (is " + std::to_string(actual) + ")";
        if (!c.Holds(actual))
            return Fail("expected " + text);
        context.log.Event("expect", text);
        return EShadowStatus::Done;
    }
};

class TSaveRunner final : public TShadowStepRunner
{
  public:
    using TShadowStepRunner::TShadowStepRunner;

    EShadowStatus Pulse(SShadowContext& context) override
    {
        const std::string slot = "Shadow " + step.name;
        if (!requested)
        {
            // Retail refuses a save while control is off (SAVE_GAME.md §7);
            // wait until the game has been Locke's for a moment.
            if (!ShadowGameIdle())
            {
                idle = 0;
                return EShadowStatus::Running;
            }
            if (++idle < kSettleTicks)
                return EShadowStatus::Running;
            GameFlow.Session().RequestSave(slot);
            requested = true;
            return EShadowStatus::Running;
        }
        if (SaveGame.LastSaved() != slot)
        {
            if (++waited > kTicksPerSecond)
                return Fail("the save to \"" + slot + "\" didn't happen");
            return EShadowStatus::Running;
        }
        SShadowWaypoint waypoint;
        waypoint.name = step.name;
        waypoint.slot = slot;
        waypoint.dir = TSaveGame::SlotsDir() / slot;
        waypoint.stage = context.stage.id;
        waypoint.tick = context.tick;
        waypoint.maplevel = Player->GetLevel();
        waypoint.pos = Player->Pos();
        waypoint.health = {Player->Health(), Player->MaxHealth()};
        waypoint.mana = {Player->Mana(), Player->MaxMana()};
        waypoint.fatigue = {Player->Fatigue(), Player->MaxFatigue()};
        waypoint.xp = {Player->Level(), Player->Exp(), Player->NextExp()};
        waypoint.gold = Player->GetInventoryAmount("Gold");
        context.waypoints.push_back(waypoint);
        context.log.Event("save", "\"" + slot + "\"", [&](JsonOut& out) {
            out.FieldString("name", step.name);
            out.FieldString("slot", slot);
            out.FieldString("dir", waypoint.dir.string());
        });
        return EShadowStatus::Done;
    }

  private:
    bool    requested = false;
    int32_t idle      = 0;
    int32_t waited    = 0;
};

} // namespace

bool ShadowGameIdle()
{
    TGameSession& session = GameFlow.Session();
    return PlayScreen.IsControlOn() && !PlayScreen.PlayingMovie() && !PlayScreen.IsFading() &&
           !PlayScreen.InGameMenuOpen() && !DialogPane.IsShowingResponses() && !session.Loading() &&
           !session.LevelLoading();
}

std::unique_ptr<TShadowStepRunner> MakeStepRunner(const SShadowStep& step)
{
    switch (step.verb)
    {
        case EShadowVerb::Start:  return std::make_unique<TStartRunner>(step);
        case EShadowVerb::Scene:  return std::make_unique<TSceneRunner>(step);
        case EShadowVerb::Travel: return std::make_unique<TTravelRunner>(step);
        case EShadowVerb::Talk:   return std::make_unique<TApproachRunner>(step);
        case EShadowVerb::Use:    return std::make_unique<TApproachRunner>(step);
        case EShadowVerb::Expect: return std::make_unique<TExpectRunner>(step);
        case EShadowVerb::Save:   return std::make_unique<TSaveRunner>(step);
    }
    return nullptr;
}
