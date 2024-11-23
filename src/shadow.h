// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      shadow.h - TShadow module                        *
// *************************************************************************

#pragma once

#include "revenant.h"
#include "object.h"

_CLASSDEF(TShadow)
class TShadow : public TObjectInstance
{
  public:
    TShadow(PTObjectImagery newim) : TObjectInstance(newim) {}
    TShadow(PSObjectDef def, PTObjectImagery newim) : TObjectInstance(def, newim) {}
};

DEFINE_BUILDER("SHADOW", TShadow)
