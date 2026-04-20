// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                    potion.cpp - TPotion module                        *
// *                                                                       *
// *  Retail class registration for POTION (id 18). Body lives in the     *
// *  retail Revenant.exe only; this TU exists so the class is present    *
// *  in `TObjectClass::classes[18]` at sector-load time.                  *
// *************************************************************************

#include "revenant.h"
#include "potion.h"
#include "object.h"

REGISTER_BUILDER(TPotion)
TObjectClass PotionClass("POTION", OBJCLASS_POTION, 0);
