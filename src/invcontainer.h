// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                invcontainer.h - TInvContainer module                  *
// *                                                                       *
// *  Retail-only class (OBJCLASS_INVCONTAINER = 17). Registered by the    *
// *  retail Revenant.exe `FindClassByID` switch at VA 0x00483850 but      *
// *  absent from the 1998 source snapshot. See                            *
// *  recon/docs/OBJCLASS_IDS.md for the full retail class-id table.       *
// *                                                                       *
// *  The retail vtable for INVCONTAINER inherits from TContainer (in-     *
// *  world lockable containers), so bags / inventory chests reuse the     *
// *  Load/Save / Use path from TContainer. Stats and any class-specific   *
// *  overrides come from the retail `class.def`, which is not part of     *
// *  the 1998 tree.                                                       *
// *************************************************************************

#pragma once

#include "revenant.h"
#include "object.h"
#include "container.h"

_CLASSDEF(TInvContainer)
class TInvContainer : public TContainer
{
  public:
    TInvContainer(TObjectImagery* newim) : TContainer(newim) {}
    TInvContainer(SObjectDef* def, TObjectImagery* newim) : TContainer(def, newim) {}
};

DEFINE_BUILDER("INVCONTAINER", TInvContainer)
