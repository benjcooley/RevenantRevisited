// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 lightsource.h - TLightSource module                   *
// *************************************************************************

#pragma once

#include "game/app/revenant.h"
#include "game/object/object.h"

_CLASSDEF(TLightSource)
class TLightSource : public TObjectInstance
{
  public:
    TLightSource(TObjectImagery* newim) : TObjectInstance(newim) {}
    TLightSource(SObjectState* def, TObjectImagery* newim) : TObjectInstance(def, newim) {}

    // Lightsource stats
    STATFUNC(EqSlot)
    STATFUNC(Value)
};

DEFINE_OBJECT_DEF("LIGHTSOURCE", TLightSource)

