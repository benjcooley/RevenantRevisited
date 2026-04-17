// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                        tool.h - TTool module                          *
// *************************************************************************

#pragma once

#include "revenant.h"
#include "object.h"

_CLASSDEF(TTool)
class TTool : public TObjectInstance
{
  public:
    TTool(TObjectImagery* newim) : TObjectInstance(newim) {}
    TTool(SObjectDef* def, TObjectImagery* newim) : TObjectInstance(def, newim) {}

    virtual bool Use(TObjectInstance* user, int32_t with = -1);
    virtual int32_t CursorType(TObjectInstance* with = nullptr);

    // Tool stats
    STATFUNC(Value)
    STATFUNC(Pick)
};

DEFINE_BUILDER("TOOL", TTool)
