// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *        shadowtravel.h - Shadow Smoke's travel: a proven walk, then go   *
// *************************************************************************
//
// How Locke gets anywhere in a Shadow Smoke run (docs/shadow/
// ARCHITECTURE.md §5). A travel finds where he should stand, proves on the
// level's walk field that he could walk there, lets the walk's time pass
// (at his real walking and running speeds, paced by fatigue), and only
// then teleports him, telling the ledger where he will land.

#pragma once

#include "shadownav.h"
#include "shadowsteps.h"
#include "shadowzones.h"

#include "../object.h"
#include "../saferef.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>

class TCharacter;
class TExit;

// Locke's pace, from his own animations and char.def, per game tick.
struct SShadowGait
{
    double walk     = 0.0;      // units a tick walking: the "walkf" state's motion
    double run      = 0.0;      // and running: "runf"
    double rundrain = 0.0;      // fatigue a tick running (RUNFATIGUE)
    double recovery = 0.0;      // fatigue a tick not running (rules.def FATIGUEDATA)
};

// A leg as Locke would walk it: run while fatigue lasts above a reserve,
// walk while it comes back.
struct SShadowPace
{
    int32_t ticks   = 0;
    int32_t fatigue = 0;        // at arrival
    int32_t ran     = 0;        // units run
    int32_t walked  = 0;        // units walked
};

[[nodiscard]] SShadowPace PaceLeg(int32_t length, int32_t fatigue, int32_t maxfatigue, const SShadowGait& gait);

// A character's speed in units a tick moving forward: the root motion of
// the first of `states` it has ("runf", "combatf", "walkf"; one frame a
// tick), 0 when it has none.
[[nodiscard]] double ShadowMoveSpeed(TObjectInstance& who, std::initializer_list<const char*> states);

// What travel keeps across steps: the walk field of Locke's level and his
// gait, both built when first needed.
class TShadowNav
{
  public:
    // The walk field of Locke's level, rebuilt (and logged) when stale;
    // nullptr when it can't be built.
    const TShadowWalkField* Field(TShadowLog& log);
    // Locke's gait, measured once (logged).
    const SShadowGait& Gait(TShadowLog& log);
    // The trigger zones of Locke's level, read when he comes to it.
    const TShadowZones& Zones(TShadowLog& log);

  private:
    TShadowWalkField           field;
    std::optional<SShadowGait> gait;
    TShadowZones               zones;
};

// The object `name` names on `level` nearest `near`: an instance name, else
// a type name, compared without case. nullptr when there is none.
[[nodiscard]] TObjectInstance* ShadowFindObject(const std::string& name, int32_t level, const S3DPoint& near);

class TShadowTravel
{
  public:
    // Where the travel ends at an object: beside it (to talk to it or use
    // it), or onto it. Onto an exit means through it, as a player goes: onto
    // its strip, which sets it off, or up to a scripted door and using it,
    // whose master.s block takes him through.
    enum class EArrive : uint8_t { Beside, Onto };

    // To `target`: a plan place, or an object by name. `evade` runs past
    // the leg's monsters (§5.5; with the duel model, P2).
    TShadowTravel(std::string target, bool evade, EArrive arrive)
        : target(std::move(target)), evade(evade), arrive(arrive)
    {
    }
    // To a point on Locke's level (a grind site, a farming spot), named
    // `label` in the log.
    TShadowTravel(const S3DPoint& point, std::string label, bool evade)
        : target(std::move(label)), evade(evade), point(point)
    {
    }

    EShadowStatus Pulse(SShadowContext& context, std::string& failure);
    // The object travelled to (nullptr for a point, or once it is gone).
    [[nodiscard]] TObjectInstance* Object() const { return object.Get(); }
    // Failed because the way is held by monsters Locke wouldn't beat, even
    // running past them: a player turns back there (§5.5).
    [[nodiscard]] bool Guarded() const { return guarded; }

  private:
    enum class EStage : uint8_t { Plan, Walk, Arrive, Through, Crossing, Fighting, Done };

    // Where Locke should end up for the target as it stands now: next to an
    // object, on an exit's strip, or on the place. False with `failure` set.
    bool Destination(SShadowContext& context, S3DPoint& dest, std::string& failure);
    // A standable cell of `exit`'s strip in Locke's region, nearest its
    // middle.
    bool OntoStrip(const TShadowWalkField& field, TExit& exit, S3DPoint& dest, std::string& failure) const;

    std::string      target;
    bool             evade  = false;
    bool             guarded = false;
    EArrive          arrive = EArrive::Beside;
    bool             viaexit = false;   // through an exit: it takes him on
    bool             viadoor = false;   // ... a scripted door, used once he is there
    int32_t          startlevel = -1;
    std::optional<S3DPoint> point;      // a bare point to go to, not a place or object
    EStage           stage  = EStage::Plan;
    TSafeRef<TObjectInstance> object;
    int32_t          region = 0;        // Locke's, where the walk starts
    S3DPoint         from{};
    S3DPoint         dest{};
    SShadowPace      pace;
    int32_t          waited = 0;
    // A zone the walk can't go around: the leg stops in it, lets it fire as
    // a walker would, and travel goes on from there.
    std::string      crossing;
    int32_t          crossings = 0;
    // Monsters the walk meets (§5.5): the leg stops where the first sees
    // Locke, and he fights it (and what else sees him there) before going
    // on. Evading, he fights a share of the leg's, once.
    std::vector<TSafeRef<TCharacter>>  meeting;
    SShadowStep                        fightstep;
    std::unique_ptr<TShadowStepRunner> fight;
    bool                               evaded = false;
};
