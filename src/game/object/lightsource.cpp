// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                lightsource.cpp - TLightSource module                  *
// *************************************************************************

#include "game/app/revenant.h"
#include "game/object/lightsource.h"

REGISTER_OBJECT_DEF(TLightSource)
TObjectClass LightSourceClass("LIGHTSOURCE", OBJCLASS_LIGHTSOURCE, 0);

// Hard coded class stats
DEFSTAT(LightSource, EqSlot, EQSL, 0, 4, 0, 6)
DEFSTAT(LightSource, Value,  VAL,  1, 0, 0, 1000000)
