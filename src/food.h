// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                        food.h - TFood module                          *
// *************************************************************************

#ifndef _FOOD_H
#define _FOOD_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _OBJECT_H
#include "object.h"
#endif

_CLASSDEF(TFood)
class TFood : public TObjectInstance
{
  public:
    TFood(TObjectImagery* newim) : TObjectInstance(newim) {}
    TFood(SObjectDef* def, TObjectImagery* newim) : TObjectInstance(def, newim) {}

    virtual bool Use(TObjectInstance* user, int32_t with = -1);
        // Munch munch munch
    int32_t CursorType(TObjectInstance* inst = nullptr) override { if (inst) return CURSOR_NONE; return CURSOR_MOUTH; }
        // Yummy

    void Load(RTInputStream is, int32_t version, int32_t objversion) override;
        // A loaded stack holds at least one
    bool MergeInto(TObjectInstance* newowner) override;
        // REVSYNC: 0x0050ea80. Joins newowner's stack of the same name

    // How many are in this stack (retail object stat 0)
    OBJSTATFUNC(Amount)

    // Food statistics
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

#endif