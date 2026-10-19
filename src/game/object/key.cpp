// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                        key.cpp - TKey module                          *
// *************************************************************************

#include "game/app/revenant.h"
#include "game/object/key.h"
#include "game/object/object.h"
#include "game/world/mappane.h"
#include "game/object/player.h"

REGISTER_OBJECT_DEF(TKey)
TObjectClass KeyClass("KEY", OBJCLASS_KEY, 0);

// Hard coded class stats
DEFSTAT(Key, Value, VAL, 0, 0, 0, 1000000)

// Hard codes object stats
DEFOBJSTAT(Key, KeyId, KEY, 0, 0, 0, 1000000)


