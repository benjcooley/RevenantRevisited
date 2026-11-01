// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   objectdefs.h - the defs of the classes with a record of their own   *
// *************************************************************************
//
// The archetype classes (objectdef.h TObjectDef) for the object classes
// whose retail stream carries fields of their own beyond the base's:
// each applies its record (objectstates.h) on top of the base and
// captures one. The character's and the player's are in characterdef.h.
// A C++ subclass with no fields of its own (TLever is an exit) uses its
// base's codec through TObjectDefFor.
//
// *************************************************************************

#pragma once

#include "game/object/objectdef.h"
#include "game/object/objectstates.h"

class TExitDef : public TObjectDef
{
  public:
    SExitState defaultState;
    [[nodiscard]] const SObjectState& DefaultState() const override { return defaultState; }
    [[nodiscard]] SObjectState& DefaultState() override { return defaultState; }
    TObjectInstance* Create(SObjectState& state, TObjectImagery* img) const override;
    TObjectInstance* Create(TObjectImagery* img) const override;
    void Apply(const SObjectState& state, TObjectInstance& inst) const override;
    void Capture(const TObjectInstance& inst, SObjectState& state) const override;
};

class TScrollDef final : public TObjectDef
{
  public:
    SScrollState defaultState;
    [[nodiscard]] const SObjectState& DefaultState() const override { return defaultState; }
    [[nodiscard]] SObjectState& DefaultState() override { return defaultState; }
    TObjectInstance* Create(SObjectState& state, TObjectImagery* img) const override;
    TObjectInstance* Create(TObjectImagery* img) const override;
    void Apply(const SObjectState& state, TObjectInstance& inst) const override;
    void Capture(const TObjectInstance& inst, SObjectState& state) const override;
};

class TWeaponDef final : public TObjectDef
{
  public:
    SWeaponState defaultState;
    [[nodiscard]] const SObjectState& DefaultState() const override { return defaultState; }
    [[nodiscard]] SObjectState& DefaultState() override { return defaultState; }
    TObjectInstance* Create(SObjectState& state, TObjectImagery* img) const override;
    TObjectInstance* Create(TObjectImagery* img) const override;
    void Apply(const SObjectState& state, TObjectInstance& inst) const override;
    void Capture(const TObjectInstance& inst, SObjectState& state) const override;
};

class TDripDef final : public TObjectDef
{
  public:
    SDripState defaultState;
    [[nodiscard]] const SObjectState& DefaultState() const override { return defaultState; }
    [[nodiscard]] SObjectState& DefaultState() override { return defaultState; }
    TObjectInstance* Create(SObjectState& state, TObjectImagery* img) const override;
    TObjectInstance* Create(TObjectImagery* img) const override;
    void Apply(const SObjectState& state, TObjectInstance& inst) const override;
    void Capture(const TObjectInstance& inst, SObjectState& state) const override;
};

class TMonsterGenDef final : public TObjectDef
{
  public:
    SMonsterGenState defaultState;
    [[nodiscard]] const SObjectState& DefaultState() const override { return defaultState; }
    [[nodiscard]] SObjectState& DefaultState() override { return defaultState; }
    TObjectInstance* Create(SObjectState& state, TObjectImagery* img) const override;
    TObjectInstance* Create(TObjectImagery* img) const override;
    void Apply(const SObjectState& state, TObjectInstance& inst) const override;
    void Capture(const TObjectInstance& inst, SObjectState& state) const override;
};
