// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *               invcontainer.cpp - TInvContainer module                 *
// *                                                                       *
// *  Retail class registration for INVCONTAINER (id 17). Body lives in    *
// *  the retail Revenant.exe only; this TU exists so the class is        *
// *  present in `TObjectClass::classes[17]` at sector-load time.          *
// *************************************************************************

#include "game/app/revenant.h"
#include "game/object/invcontainer.h"
#include "game/object/object.h"

REGISTER_BUILDER(TInvContainer)
extern TObjectClass ContainerClass;
TObjectClass InvContainerClass("INVCONTAINER", OBJCLASS_INVCONTAINER, 0, &ContainerClass);  // retail: based on CONTAINER
