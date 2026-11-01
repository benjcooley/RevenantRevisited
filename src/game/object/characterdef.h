// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   characterdef.h - the complex object's and the character's defs      *
// *************************************************************************
//
// The archetype classes (objectdef.h TObjectDef) for the classes with
// state of their own: each applies its class's record (objectstates.h)
// to a live object on top of its base's, and captures one. The player's
// def derives from the character's so a player is applied as a character
// first.
//
// *************************************************************************

#pragma once

#include "game/object/objectdef.h"
#include "game/object/objectstates.h"

// TComplexObject: the root action block. Not registered by itself: no
// type is a bare complex object.
class TComplexObjectDef : public TObjectDef
{
  public:
    void Apply(const SObjectState& state, TObjectInstance& inst) const override;
    void Capture(const TObjectInstance& inst, SObjectState& state) const override;
};

// TCharacter ("Character" in class.def).
class TCharacterDef : public TComplexObjectDef
{
  public:
    SCharacterState defaultState;
    [[nodiscard]] const SObjectState& DefaultState() const override { return defaultState; }
    [[nodiscard]] SObjectState& DefaultState() override { return defaultState; }

    TObjectInstance* Create(SObjectState& state, TObjectImagery* img) const override;
    TObjectInstance* Create(TObjectImagery* img) const override;
    void Apply(const SObjectState& state, TObjectInstance& inst) const override;
    void Capture(const TObjectInstance& inst, SObjectState& state) const override;
};

// TPlayer ("Player"): a character with the player's record on top.
class TPlayerDef final : public TCharacterDef
{
  public:
    SPlayerState defaultState;
    [[nodiscard]] const SObjectState& DefaultState() const override { return defaultState; }
    [[nodiscard]] SObjectState& DefaultState() override { return defaultState; }

    TObjectInstance* Create(SObjectState& state, TObjectImagery* img) const override;
    TObjectInstance* Create(TObjectImagery* img) const override;
    void Apply(const SObjectState& state, TObjectInstance& inst) const override;
    void Capture(const TObjectInstance& inst, SObjectState& state) const override;
};

// The automap record as the game holds it <-> as the record stores it
// (words as little-endian bytes). The legacy codec and the def share them.
struct SAutoMapRecord;
[[nodiscard]] SAutoMapRecordState AutoMapToState(const SAutoMapRecord& record);
[[nodiscard]] SAutoMapRecord AutoMapFromState(const SAutoMapRecordState& state);
