// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                        tile.h - TTile module                          *
// *************************************************************************

#pragma once

#include "revenant.h"

#include "object.h"

_CLASSDEF(TTile)
class TTile : public TObjectInstance
{
  public:
    TTile(TObjectImagery* newim) : TObjectInstance(newim) { flags |= OF_IMMOBILE; }
    TTile(SObjectDef* def, TObjectImagery* newim) : TObjectInstance(def, newim) { flags |= OF_IMMOBILE; }

    // Tile stats
    STATFUNC(Code)
    STATFUNC(Extra)
    STATFUNC(Supertile)
    STATFUNC(Width)
    STATFUNC(Height)
};

DEFINE_BUILDER("TILE", TTile)
