// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                        trap.h - TTrap module                          *
// *************************************************************************

#pragma once

#include "revenant.h"
#include "object.h"

_CLASSDEF(TTrap)
class TTrap : public TObjectInstance
{
  public:
    TTrap(PTObjectImagery newim) : TObjectInstance(newim) { flags |= OF_IMMOBILE; }
    TTrap(PSObjectDef def, PTObjectImagery newim) : TObjectInstance(def, newim) { flags |= OF_IMMOBILE; }
};

DEFINE_BUILDER("TRAP", TTrap)
