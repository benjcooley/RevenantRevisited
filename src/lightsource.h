// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 lightsource.h - TLightSource module                   *
// *************************************************************************

#pragma once

#include "revenant.h"
#include "object.h"

_CLASSDEF(TLightSource)
class TLightSource : public TObjectInstance
{
  public:
    TLightSource(TObjectImagery* newim) : TObjectInstance(newim) {}
    TLightSource(SObjectDef* def, TObjectImagery* newim) : TObjectInstance(def, newim) {}

    // Lightsource stats
    STATFUNC(EqSlot)
    STATFUNC(Value)
};

DEFINE_BUILDER("LIGHTSOURCE", TLightSource)

