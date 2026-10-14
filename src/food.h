// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                        food.h - TFood module                          *
// *************************************************************************

#pragma once

#include "revenant.h"
#include "object.h"

// Something to eat or drink. Retail's POTION class is a TFood too (potion.h).
// docs/gameplay/forensics/CONSUMABLES.md
_CLASSDEF(TFood)
class TFood : public TObjectInstance
{
  public:
    TFood(TObjectImagery* newim) : TObjectInstance(newim) {}
    TFood(SObjectDef* def, TObjectImagery* newim) : TObjectInstance(def, newim) {}

    bool Use(TObjectInstance* user, int32_t with = -1) override;
        // REVSYNC: TFood::Use @ 0x0050e780 (vtable 0xbc; POTION's too). The
        // user eats or drinks one: its health and mana go up by the item's,
        // held to their maxima, and an item with a Poison stat cures poison;
        // the main player hears it and reads FOODGOOD1; one is used up. On
        // true the item may be gone: don't touch it.
    int32_t CursorType(TObjectInstance* inst = nullptr) override { if (inst) return CURSOR_NONE; return CURSOR_MOUTH; }
        // Yummy

    void Load(RTInputStream is, int32_t version, int32_t objversion) override;
        // A loaded stack holds at least one
    bool MergeInto(TObjectInstance* newowner) override;
        // REVSYNC: 0x0050ea80. Joins newowner's stack of the same name

    // How many are in this stack (retail object stat 0)
    OBJSTATFUNC(Amount)

    // Food statistics, by retail's ids (CONSUMABLES.md §3). Eating gives
    // Health and Mana; a Poison stat cures poison. Fatigue, Cure and Fill are
    // class.def's, and nothing reads them.
    STATFUNC(Value)
    STATFUNC(Health)
    STATFUNC(Mana)
    STATFUNC(Fatigue)
    STATFUNC(Poison)
    STATFUNC(Cure)
    STATFUNC(Fill)
};

DEFINE_BUILDER("FOOD", TFood)

extern TObjectClass FoodClass;
