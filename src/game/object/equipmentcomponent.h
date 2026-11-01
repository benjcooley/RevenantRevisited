// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   equipmentcomponent.h - what a player wears and wields               *
// *************************************************************************
//
// The equip slots: the item in each, or nullptr. Every player has one from
// construction, reached through Equipment(). Nothing here is serialized:
// an equipped item is an inventory item in an equip slot, and the slots
// are rebuilt from the inventory on load (TPlayer::Equip), as in retail.
//
// *************************************************************************

#pragma once

#include "engine/scene/component.h"

#include <array>

_CLASSDEF(TObjectInstance)

// Equipment slots
constexpr int32_t NUM_EQ_SLOTS    = 11;
constexpr int32_t EQ_HEAD         = 0;
constexpr int32_t EQ_NECK         = 1;
constexpr int32_t EQ_BODY         = 2;
constexpr int32_t EQ_OFFHAND      = 3;
constexpr int32_t EQ_PRIMEHAND    = 4;
constexpr int32_t EQ_R_ACCESSORY  = 5;
constexpr int32_t EQ_L_ACCESSORY  = 6;
constexpr int32_t EQ_RANGEDWEAPON = 7;
constexpr int32_t EQ_AMMO         = 8;
constexpr int32_t EQ_LEGS         = 9;
constexpr int32_t EQ_FEET         = 10;

class TEquipmentComponent final : public TComponent
{
  public:
    [[nodiscard]] const char* ComponentName() const override { return "equipment"; }

    std::array<TObjectInstance*, NUM_EQ_SLOTS> slots{};   // the item in each slot, or nullptr

    [[nodiscard]] TObjectInstance* At(int32_t slot) const
        { return (uint32_t)slot < (uint32_t)NUM_EQ_SLOTS ? slots[size_t(slot)] : nullptr; }
};
