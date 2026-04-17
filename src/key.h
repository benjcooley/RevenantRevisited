// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                         key.h - TKey module                           *
// *************************************************************************

#pragma once

#include "revenant.h"
#include "object.h"

_CLASSDEF(TKey)
class TKey : public TObjectInstance
{
  public:
    TKey(TObjectImagery* newim) : TObjectInstance(newim) {}
    TKey(SObjectDef* def, TObjectImagery* newim) : TObjectInstance(def, newim) {}

    // Key stats
    STATFUNC(Value)
    OBJSTATFUNC(KeyId)
};

DEFINE_BUILDER("KEY", TKey)

#endif