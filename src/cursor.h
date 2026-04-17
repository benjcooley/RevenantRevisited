// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   cursor.h - Cursor handler file                      *
// *************************************************************************

#pragma once

#include "revenant.h"

extern int32_t cursorx;
extern int32_t cursory;
extern int32_t mousebutton;

void SetMouseBitmap(PTBitmap cursor);
void SetMouseShadow(PTBitmap shadow, int32_t offsetx = 1, int32_t offsety = 3);
void SetMouseCornerBitmap(PTBitmap corner, bool toppriority = false);
void SetMouseCornerBitmap(int32_t type, bool toppriority = false);
void CursorOverObject(TObjectInstance* inst, bool toppriority = false);

void SetDragBitmap(PTBitmap drag, int32_t x = 0, int32_t y = 0);
void ClearDragBitmap();

void SetDragObj(TObjectInstance* inst);
TObjectInstance* GetDragObj();

void DrawMouseCursor();
void DrawMouseShadow();

void RestrictCursor();
void ReleaseCursor();
