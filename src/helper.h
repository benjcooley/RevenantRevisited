// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      helper.h - THelper module                        *
// *************************************************************************

#pragma once

#include "revenant.h"

#include "object.h"

_CLASSDEF(THelper)
class THelper : public TObjectInstance
{
  public:
    THelper(PTObjectImagery newim) : TObjectInstance(newim) { flags |= OF_IMMOBILE; }
    THelper(PSObjectDef def, PTObjectImagery newim) : TObjectInstance(def, newim) { flags |= OF_IMMOBILE; }
};

DEFINE_BUILDER("HELPER", THelper)

