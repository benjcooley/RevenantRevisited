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

#include "../object.h"
#include "../saferef.h"

#include <optional>
#include <string>

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

  private:
    TShadowWalkField           field;
    std::optional<SShadowGait> gait;
};

// The object `name` names on `level` nearest `near`: an instance name, else
// a type name, compared without case. nullptr when there is none.
[[nodiscard]] TObjectInstance* ShadowFindObject(const std::string& name, int32_t level, const S3DPoint& near);

class TShadowTravel
{
  public:
    // To `target`: a plan place, or an object by name. `evade` runs past
    // the leg's monsters (§5.5; with the duel model, P2).
    TShadowTravel(std::string target, bool evade) : target(std::move(target)), evade(evade) {}

    EShadowStatus Pulse(SShadowContext& context, std::string& failure);
    // The object travelled to (nullptr for a point, or once it is gone).
    [[nodiscard]] TObjectInstance* Object() const { return object.Get(); }

  private:
    enum class EStage : uint8_t { Plan, Walk, Arrive, Done };

    // Where Locke should end up for the target as it stands now: next to an
    // object, or on the place. False with `failure` set.
    bool Destination(SShadowContext& context, S3DPoint& dest, std::string& failure);

    std::string      target;
    bool             evade  = false;
    EStage           stage  = EStage::Plan;
    TSafeRef<TObjectInstance> object;
    int32_t          region = 0;        // Locke's, where the walk starts
    S3DPoint         from{};
    S3DPoint         dest{};
    SShadowPace      pace;
    int32_t          waited = 0;
};
