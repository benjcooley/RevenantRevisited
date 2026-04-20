// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   mapscroll.h - TMapScroll module                     *
// *                                                                       *
// *  Retail-only class (OBJCLASS_MAPSCROLL = 26). Registered by the      *
// *  retail Revenant.exe `FindClassByID` switch at VA 0x00483850 but     *
// *  absent from the 1998 source snapshot. See recon/docs/OBJCLASS_IDS.md*
// *  for the full retail class-id table.                                  *
// *                                                                       *
// *  Retail's `TObjectInstance::LoadObject` explicitly drops class-26    *
// *  instances during per-sector load (they live in the player save      *
// *  instead), but the class must still be registered so                 *
// *  TObjectClass::classes[26] != nullptr for any tool that walks the    *
// *  numclasses range (editor, save-loader, FindClass-by-name).          *
// *************************************************************************

#pragma once

#include "revenant.h"
#include "object.h"

_CLASSDEF(TMapScroll)
class TMapScroll : public TObjectInstance
{
  public:
    TMapScroll(TObjectImagery* newim) : TObjectInstance(newim) {}
    TMapScroll(SObjectDef* def, TObjectImagery* newim) : TObjectInstance(def, newim) {}
};

DEFINE_BUILDER("MAPSCROLL", TMapScroll)
