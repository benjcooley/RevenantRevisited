// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                        trap.h - TTrap module                          *
// *************************************************************************

#pragma once

#include "game/app/revenant.h"
#include "game/object/object.h"

_CLASSDEF(TTrap)
class TTrap : public TObjectInstance
{
  public:
    TTrap(TObjectImagery* newim) : TObjectInstance(newim) { flags |= OF_IMMOBILE; }
    TTrap(SObjectDef* def, TObjectImagery* newim) : TObjectInstance(def, newim) { flags |= OF_IMMOBILE; }
};

DEFINE_BUILDER("TRAP", TTrap)
