// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  cursor.cpp - Mouse cursor handling                   * 
// *************************************************************************

#include "cursor.h"

#include "bitmap.h"
#include "bmsurface.h"
#include "display.h"
#include "mainwnd.h"
#include "multi.h"
#include "object.h"
#include "playscreen.h"

// Globals for storing current mouse imagery
PTBitmap MouseCursor = nullptr;
PTBitmap MouseShadow = nullptr;
int32_t shadowoffsetx = 0, shadowoffsety = 0;
PTBitmap MouseCursorAdd = nullptr;     // Little picture that goes in the corner
bool priority;                      // Priority of mouse image

PTBitmap DragBitmap = nullptr;
int32_t grabx, graby;

TObjectInstance* DragObj = nullptr;

bool cleardragbitmap = false;

int32_t oldcursorx = WIDTH/2, oldcursory = HEIGHT/2;    // Save cursor pos between activations

// List of cursor types
char *CursorTypes[NUMCURSORTYPES] = { "hand", "eye", "mouth", "door", "stairs", "hourglass", "swords" };


void SetMouseBitmap(PTBitmap cursor)
{
    MouseCursor = cursor;

    if (cursor == GameData->Bitmap("cursor"))
        SetMouseShadow(GameData->Bitmap("cursorshadow"));
    else
    {
        //MouseShadow = nullptr;
        MouseCursorAdd = nullptr;      // no corner bitmaps if not in regular cursor
        priority = true;
    }
}

void SetMouseShadow(PTBitmap shadow, int32_t offsetx, int32_t offsety)
{
    MouseShadow = shadow;
    shadowoffsetx = offsetx;
    shadowoffsety = offsety;
}

void SetMouseCornerBitmap(PTBitmap corner, bool toppriority)
{
    if (corner || toppriority)
    {
        MouseCursorAdd = corner;
        priority = toppriority;
    }
}

void SetMouseCornerBitmap(int32_t type, bool toppriority)
{
    if (type != CURSOR_NONE || toppriority)
        SetMouseCornerBitmap(type != CURSOR_NONE ? GameData->Bitmap(CursorTypes[type]) : nullptr);
}

void SetDragBitmap(PTBitmap drag, int32_t x, int32_t y)
{
    DragBitmap = drag;
    grabx = x;
    graby = y;
}

void ClearDragBitmap()
{
    cleardragbitmap = true;
}

void SetDragObj(TObjectInstance* inst)
{
    DragObj = inst;
}

TObjectInstance* GetDragObj()
{
    return DragObj;
}

void DrawMouseShadow(int32_t x, int32_t y, int32_t width, int32_t height)
{
    Display->SetClipRect(x, y, width, height);
    Display->Put(cursorx + shadowoffsetx, cursory + shadowoffsety, MouseShadow, DM_TRANSPARENT | DM_USEREG | DM_ALIAS);
}

void DrawMouseCursor()
{
    // draw the bitmap first so that the cursor appears over the top of it
    if (DragBitmap)
        Display->Put(cursorx - grabx, cursory - graby, DragBitmap, DM_TRANSPARENT | DM_USEREG);

    if (MouseCursor && !MouseCursorAdd)
        Display->Put(cursorx, cursory, MouseCursor, DM_TRANSPARENT | DM_USEREG | DM_ALIAS);

    if (MouseCursorAdd)
        Display->Put(cursorx, cursory, MouseCursorAdd, DM_TRANSPARENT | DM_USEREG | DM_ALIAS);

    if (cleardragbitmap)
    {
        SetDragBitmap(nullptr);
        cleardragbitmap = false;
    }

    MouseCursorAdd = nullptr;
    priority = false;
}

void DrawMouseShadow()
{
    if (MouseShadow)
    {
        Display->SetOrigin(0, 0);

        if (MouseShadow != GameData->Bitmap("cursorshadow"))
            DrawMouseShadow(MAPPANEX, MAPPANEY, MAPPANEWIDTH, MAPPANEHEIGHT);
        else
        {
            if (PlayScreen.InCompleteExclusion())
                DrawMouseShadow(0, 0, WIDTH, HEIGHT);
            else
            {
                if (cursorx <= MAPPANEX)
                    DrawMouseShadow(0, 0, MAPPANEX, HEIGHT);
                else if ((cursorx + MouseShadow->width) >= (MAPPANEX + MAPPANEWIDTH))
                    DrawMouseShadow(MAPPANEX + MAPPANEWIDTH, 0, WIDTH - (MAPPANEX + MAPPANEWIDTH), HEIGHT);

                if (cursory <= MAPPANEY)
                    DrawMouseShadow(0, 0, WIDTH, MAPPANEY);
                else if ((cursory + MouseShadow->height) >= (MAPPANEY + MAPPANEHEIGHT))
                    DrawMouseShadow(0, MAPPANEY + MAPPANEHEIGHT, WIDTH, HEIGHT - (MAPPANEY + MAPPANEHEIGHT));
            }
        }

        Display->ResetClipRect();
    }
}

void CursorOverObject(TObjectInstance* inst, bool toppriority)
{
    if (toppriority || !priority)
    {
        int32_t type = CURSOR_NONE;

        if (inst)
            type = inst->CursorType(DragObj);

        SetMouseCornerBitmap(type, toppriority);
    }
}

void RestrictCursor()
{
    // center the cursor
    RECT r;
    GetClientRect(MainWindow.Hwnd(), &r);
    ClientToScreen(MainWindow.Hwnd(), (LPPOINT)&r);
    SetCursorPos(r.left + oldcursorx, r.top + oldcursory);

    // clip if not windowed
    if (!Windowed || Borderless)
    {
        r.right = r.left + WIDTH;
        r.bottom = r.top + HEIGHT;
        ClipCursor(&r);
    }
}

void ReleaseCursor()
{
    // save the old position for restore after reactivation
    oldcursorx = cursorx;
    oldcursory = cursory;

    // clear out the clip rect (cursor can move anywhere)
    ClipCursor(nullptr);
}

