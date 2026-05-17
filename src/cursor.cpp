// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  cursor.cpp - Mouse cursor handling                   * 
// *************************************************************************

#include "cursor.h"

#include "bitmap.h"
#include "bmsurface.h"
#include "cursor_os.h"
#include "display.h"
#include "imgui.h"
#include "logging.h"
#include "mainwnd.h"
#include "multi.h"
#include "object.h"
#include "playscreen.h"
#include "renderer.h"

// Globals for storing current mouse imagery
PTBitmap MouseCursor = nullptr;
PTBitmap MouseShadow = nullptr;
int32_t shadowoffsetx = 0, shadowoffsety = 0;
PTBitmap MouseCursorAdd = nullptr;     // Little picture that goes in the corner
bool priority;                      // Priority of mouse image

// True once the OS has accepted our cursor pixels via rev_platform::
// SetOSCursor (macOS path). TCursorHud::Draw consults this to suppress
// the in-game main cursor render so we don't double-draw on top of the
// OS pointer. On platforms without the OS handoff it stays false and
// the original in-game draw runs.
static bool g_os_cursor_owns_pixels = false;

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

    // Hand the new pixels to the OS so AppKit draws our cursor wherever
    // it draws every other app's -- including out-of-window. regx/regy
    // is the registration point Display.Put uses (DM_USEREG), so it's
    // also the right NSCursor hotspot. macOS-only today; the stub
    // returns false on other platforms and we fall back to the in-game
    // draw in TCursorHud::Draw.
    if (cursor)
    {
        g_os_cursor_owns_pixels = rev_platform::SetOSCursor(cursor, cursor->regx, cursor->regy);
        if (!g_os_cursor_owns_pixels)
            log_warn("[cursor] SetOSCursor declined; falling back to in-game draw");
    }
}

void RefreshOSCursor()
{
    if (MouseCursor)
        g_os_cursor_owns_pixels = rev_platform::SetOSCursor(
            MouseCursor, MouseCursor->regx, MouseCursor->regy);
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
    Display.SetClipRect(x, y, width, height);
    Display.Put(cursorx + shadowoffsetx, cursory + shadowoffsety, MouseShadow, DM_TRANSPARENT | DM_USEREG | DM_ALIAS);
}

void TCursorHud::Draw()
{
    // Renderer invokes this inside the swapchain pass between the
    // 3D scene present and the ImGui debug overlay. We're inside an
    // active pass -- can issue Renderer->DrawBitmap calls directly.
    if (!Renderer) return;

    // Suppress the game-side overlay while ImGui owns the mouse. The
    // OS cursor still draws the pointer pixel (we reset it to the
    // arrow from runtimemode while ImGui captures); we just don't add
    // drag bitmaps or corner overlays on top.
    if (ImGui::GetIO().WantCaptureMouse)
        return;

    // Drag bitmap (e.g. inventory item being dragged) goes first so
    // the cursor sits above it. Still drawn here even when the OS owns
    // the cursor pixel -- the drag bitmap is a separate visual asset
    // (the item being moved), not part of the cursor sprite.
    if (DragBitmap)
        Renderer->DrawBitmap(DragBitmap, cursorx - grabx, cursory - graby);

    // Main cursor pixel: only draw it ourselves if the OS didn't take
    // it. On macOS SetMouseBitmap routes through rev_platform::SetOSCursor
    // and g_os_cursor_owns_pixels = true; on other platforms (and the
    // brief window before the first SetMouseBitmap call) we fall back
    // to the in-game draw.
    if (MouseCursor && !MouseCursorAdd && !g_os_cursor_owns_pixels)
        Renderer->DrawBitmap(MouseCursor, cursorx, cursory);

    // Corner-bitmap overlay (the little add-on icon: hand, eye, sword,
    // etc.) is always our render, even when OS owns the main cursor --
    // it's a per-target hint laid over the cursor pixel that the OS
    // pointer has no awareness of.
    if (MouseCursorAdd)
        Renderer->DrawBitmap(MouseCursorAdd, cursorx, cursory);

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
        Display.SetOrigin(0, 0);

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

        Display.ResetClipRect();
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
#if 0 // TODO(port): Win32 cursor clipping; sokol_app has no equivalent yet
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
#endif
}

void ReleaseCursor()
{
    // save the old position for restore after reactivation
    oldcursorx = cursorx;
    oldcursory = cursory;

#if 0 // TODO(port): Win32 cursor clipping; sokol_app has no equivalent yet
    // clear out the clip rect (cursor can move anywhere)
    ClipCursor(nullptr);
#endif
}

