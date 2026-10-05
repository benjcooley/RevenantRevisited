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
#include "food.h"

// Retail's POTION class is based on FOOD (TObjectClass registration @
// 0x0050e5a3) and its vtable shares TFood's Load and Amount accessors
// (docs/gameflow/forensics/SAVE_GAME.md §11.3), so a potion is a food.
_CLASSDEF(TPotion)
class TPotion : public TFood
{
  public:
    TPotion(TObjectImagery* newim) : TFood(newim) {}
    TPotion(SObjectDef* def, TObjectImagery* newim) : TFood(def, newim) {}
};

DEFINE_BUILDER("POTION", TPotion)
