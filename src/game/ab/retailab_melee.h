// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *   retailab_melee.h - the melee fixture's world, for its callers       *
// *************************************************************************
//
// The world a melee case describes (retailab_melee.cpp; the retail side is
// tools/retail_runtime/slots/combat/melee_attack.py): its characters with
// their whole character data, attack and AI state, the rules, and the seams
// and the tick's globals installed for as long as it lives. The melee
// targets run one call on it; the scenario kata (retailab_scenario.cpp)
// ticks it.

#pragma once

#include "game/ab/retailab_fixture.h"

#include <memory>

namespace RetailAB::Melee
{

class TMeleeWorld
{
  public:
    explicit TMeleeWorld(const JsonValue& cs);    // `cs` outlives the world
    ~TMeleeWorld();
    TMeleeWorld(const TMeleeWorld&) = delete;
    TMeleeWorld& operator=(const TMeleeWorld&) = delete;

    [[nodiscard]] Fixture::TFixtureWorld& World() { return *world; }
    // Whether the case's call runs the character's tick (C6, C7, the
    // scenario kata): the tick's seams and globals are installed.
    [[nodiscard]] bool Ticking() const { return ticking; }
    // The end of the melee dump: each character's AI state ("ai") and, for a
    // ticking case, the tick state of those that give one ("pulse").
    void WriteEnd(JsonOut& j) const;

  private:
    struct SScopes;
    const JsonValue* cs = nullptr;
    bool ticking = false;
    std::unique_ptr<Fixture::TFixtureWorld> world;
    std::unique_ptr<SScopes> scopes;
};

}  // namespace RetailAB::Melee
