// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *               invcontainer.cpp - TInvContainer module                 *
// *                                                                       *
// *  Retail class registration for INVCONTAINER (id 17). Body lives in    *
// *  the retail Revenant.exe only; this TU exists so the class is        *
// *  present in `TObjectClass::classes[17]` at sector-load time.          *
// *************************************************************************

#include "revenant.h"
#include "invcontainer.h"
#include "object.h"

REGISTER_BUILDER(TInvContainer)
TObjectClass InvContainerClass("INVCONTAINER", OBJCLASS_INVCONTAINER, 0);
