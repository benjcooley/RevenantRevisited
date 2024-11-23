// *************************************************************************
// *                           Cinematix EXILE                             *
// *                    Copyright (C) 1996 Cinematix                       *
// *                rangedweapon.h - TRangedWeapon module                  *
// *************************************************************************

#pragma once

#include "revenant.h"

#include "object.h"

_CLASSDEF(TRangedWeapon)
class TRangedWeapon : public TObjectInstance
{
  public:
    TRangedWeapon(PTObjectImagery newim) : TObjectInstance(newim) {}
    TRangedWeapon(PSObjectDef def, PTObjectImagery newim) : TObjectInstance(def, newim) {}

    // Ranged weapon stats
    STATFUNC(EqSlot)
    STATFUNC(Value)

};

DEFINE_BUILDER("RANGEDWEAPON", TRangedWeapon)

