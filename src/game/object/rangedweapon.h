// *************************************************************************
// *                           Cinematix EXILE                             *
// *                    Copyright (C) 1996 Cinematix                       *
// *                rangedweapon.h - TRangedWeapon module                  *
// *************************************************************************

#pragma once

#include "game/app/revenant.h"

#include "game/object/object.h"

_CLASSDEF(TRangedWeapon)
class TRangedWeapon : public TObjectInstance
{
  public:
    TRangedWeapon(TObjectImagery* newim) : TObjectInstance(newim) {}
    TRangedWeapon(SObjectDef* def, TObjectImagery* newim) : TObjectInstance(def, newim) {}

    // Ranged weapon stats
    STATFUNC(EqSlot)
    STATFUNC(Value)

};

DEFINE_BUILDER("RANGEDWEAPON", TRangedWeapon)

