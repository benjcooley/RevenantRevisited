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
    TShadow(TObjectImagery* newim) : TObjectInstance(newim) {}
    TShadow(SObjectDef* def, TObjectImagery* newim) : TObjectInstance(def, newim) {}
};

DEFINE_BUILDER("SHADOW", TShadow)
