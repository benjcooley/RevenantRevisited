// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      helper.h - THelper module                        *
// *************************************************************************

#pragma once

#include "game/app/revenant.h"

#include "game/object/object.h"

_CLASSDEF(THelper)
class THelper : public TObjectInstance
{
  public:
    THelper(TObjectImagery* newim) : TObjectInstance(newim) { flags |= OF_IMMOBILE; }
    THelper(SObjectState* def, TObjectImagery* newim) : TObjectInstance(def, newim) { flags |= OF_IMMOBILE; }
};

DEFINE_OBJECT_DEF("HELPER", THelper)

