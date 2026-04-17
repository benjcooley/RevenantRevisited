// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   talisman.h - TTalisman module                       *
// *************************************************************************

#pragma once

#include "revenant.h"

#include "object.h"

_CLASSDEF(TTalisman)
class TTalisman : public TObjectInstance
{
  public:
    TTalisman(TObjectImagery* newim) : TObjectInstance(newim) {}
    TTalisman(SObjectDef* def, TObjectImagery* newim) : TObjectInstance(def, newim) {}

    virtual void RemoveFromInventory();
        // Remove this object from whatever inventory it is in

    // Talisman stats
    STATFUNC(Code)
};

DEFINE_BUILDER("TALISMAN", TTalisman)
