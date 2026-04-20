// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  mapscroll.cpp - TMapScroll module                    *
// *                                                                       *
// *  Retail class registration for MAPSCROLL (id 26). Body lives in the  *
// *  retail Revenant.exe only; this TU exists so the class is present    *
// *  in `TObjectClass::classes[26]` at sector-load time.                  *
// *************************************************************************

#include "revenant.h"
#include "mapscroll.h"
#include "object.h"

REGISTER_BUILDER(TMapScroll)
TObjectClass MapScrollClass("MAPSCROLL", OBJCLASS_MAPSCROLL, 0);
