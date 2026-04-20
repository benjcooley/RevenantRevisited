// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     potion.h - TPotion module                         *
// *                                                                       *
// *  Retail-only class (OBJCLASS_POTION = 18). Registered by the retail  *
// *  Revenant.exe `FindClassByID` switch at VA 0x00483850 but absent      *
// *  from the 1998 source snapshot. See recon/docs/OBJCLASS_IDS.md for    *
// *  the full retail class-id table.                                      *
// *                                                                       *
// *  Potions are use-on-self consumables; the retail vtable derives from *
// *  TObjectInstance directly (there is no TFood intermediary — FOOD is  *
// *  class 4, an independent class hierarchy). Stats and Use() behaviour *
// *  come from the retail `class.def`, which is not part of the 1998     *
// *  tree.                                                                *
// *************************************************************************

#pragma once

#include "revenant.h"
#include "object.h"

_CLASSDEF(TPotion)
class TPotion : public TObjectInstance
{
  public:
    TPotion(TObjectImagery* newim) : TObjectInstance(newim) {}
    TPotion(SObjectDef* def, TObjectImagery* newim) : TObjectInstance(def, newim) {}
};

DEFINE_BUILDER("POTION", TPotion)
