// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   statscomponent.h - an object's own statistics                       *
// *************************************************************************
//
// Retail kept an object's statistics in a sizable array inside every
// object (health, fatigue, mana, the player's attributes and skills). They
// are a component now: an object of a class that defines object stats
// gets one from its constructor and keeps it for life, so the object
// reaches it through a plain pointer. Values sit in the class's stat
// order (the id every GetObjStat / SetObjStat call site uses); the
// record (SObjectState::stats) names them by unique id, and the
// archetype's Apply / Capture translate.
//
// *************************************************************************

#pragma once

#include "engine/scene/component.h"

#include <vector>

class TStatsComponent final : public TComponent
{
  public:
    [[nodiscard]] const char* ComponentName() const override { return "stats"; }

    [[nodiscard]] int32_t Count() const { return int32_t(values_.size()); }
    [[nodiscard]] int32_t Get(int32_t statid) const
        { return (uint32_t)statid < values_.size() ? values_[size_t(statid)] : 0; }
    void Set(int32_t statid, int32_t value)
        { if ((uint32_t)statid < values_.size()) values_[size_t(statid)] = value; }
    void Resize(int32_t count) { values_.resize(size_t(count < 0 ? 0 : count), 0); }
    void Remove(int32_t statid)
        { if ((uint32_t)statid < values_.size()) values_.erase(values_.begin() + statid); }
        // The class dropped a stat: later ids move down (TObjectClass::DeleteObjStat)

  private:
    std::vector<int32_t> values_;   // By class stat id
};
