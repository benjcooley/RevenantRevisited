// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                       tool.cpp - TTool module                         *
// *************************************************************************

#include "game/app/revenant.h"
#include "game/object/tool.h"
#include "game/object/exit.h"
#include "game/object/container.h"
#include "game/world/mappane.h"

REGISTER_OBJECT_DEF(TTool)
TObjectClass ToolClass("TOOL", OBJCLASS_TOOL, 0);

// Hard coded class stats
DEFSTAT(Tool, Value, VAL, 0, 0, 0, 1000000)
DEFSTAT(Tool, Pick, PICK, 1, 0, 0, 100)

// REVSYNC: TTool::Use @ 0x005240c0 -- only the scripts (the base Use); a
// pick works as what a locked container is used with (CheckKeyUse).
bool TTool::Use(TObjectInstance* user, int32_t with)
{
    return TObjectInstance::Use(user, with);
}

int32_t TTool::CursorType(TObjectInstance* with)
{
    return CURSOR_NONE;
}

