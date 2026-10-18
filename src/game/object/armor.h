// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                       armor.h - TArmor module                         *
// *************************************************************************

#pragma once

#include "game/app/revenant.h"
#include "game/object/object.h"

_CLASSDEF(TArmor)
class TArmor : public TObjectInstance
{
  public:
    TArmor(TObjectImagery* newim) : TObjectInstance(newim) {}
    TArmor(SObjectDef* def, TObjectImagery* newim) : TObjectInstance(def, newim) {}

    // Armor stats
    STATFUNC(EqSlot)
    STATFUNC(Value)
    STATFUNC(Protection)
    STATFUNC(Combining)
    STATFUNC(ResistPoison)
    STATFUNC(Stealth)
    STATFUNC(MinStrength)
    STATFUNC(MinConstitution)
};

DEFINE_BUILDER("ARMOR", TArmor)
