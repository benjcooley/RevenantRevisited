// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                screen.cpp  - EXILE Screen Object File                 *
// *************************************************************************

#include <stdio.h>

#include "revenant.h"
#include "bitmap.h"
#include "display.h"
#include "mainwnd.h"
#include "timer.h"
#include "cursor.h"
#include "mappane.h"
#include "screen.h"
#include "sound.h"
#include "videocap.h"

int32_t cursorx = 0;        // Mouse cursor positions
int32_t cursory = 0;
int32_t mousebutton = 0;    // Mouse button

extern bool LoaderWait;

// Screen Display Functions

TScreen::TScreen()
{
    nextscreen = nullptr;
}

TScreen::~TScreen()
{
}

bool TScreen::BeginScreen()
{
    int32_t loop;

    Display->InitBackgroundSystem();
    firstframe = true;  // Tell timer tick function not to flip a page the first time

    panes.Clear();
    screenframes = 0;
    for (loop = 0; loop < NUMEXCLUSIVEPANES; loop++)
        exclusive[loop] = 0;
    numexclusive = 0;

    bool ok = Initialize();
    if (!ok)
        return false;

    return true;
}

void TScreen::EndScreen()
{
    int32_t loop;

    Close();

    panes.Clear();
    screenframes = 0;
    for (loop = 0; loop < NUMEXCLUSIVEPANES; loop++)
        exclusive[loop] = 0;
    numexclusive = 0;
}

// Copies contents of display's back buffer to the user's display immediately. (For
// 'loading' screens which have to update from within a single timer tick).
void TScreen::PutToScreen()
{
    Display->PutToScreen(0, 0, Display->Width(), Display->Height());
}

// *************************
// * Screen Pane Functions *
// *************************

int32_t TScreen::FindPane(PTPane pane)
{
    for (TPaneIterator p(&panes); p; p++)
        if (pane == p.Item())
            return p.ItemNum();

    return -1;
}

int32_t TScreen::AddPane(PTPane pane, int32_t panenum)
{
  // Do pane list
    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
    {
        if (panes[loop] == pane)
        {
            if (panenum < 0)
                return loop;
            else
                panes.Remove(loop);
        }
    }

    int32_t newnum;
    if (panenum < 0)
        newnum = panes.Set(pane, panes.NumItems());
    else
        newnum = panes.Set(pane, panenum);

    pane->SetScreen(this);

    return newnum;
}

bool TScreen::RemovePane(PTPane pane)
{
    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
    {
        if (panes[loop] == pane)
        {
            panes.Remove(loop);
            pane->SetScreen(nullptr);
            return true;
        }
    }

    return false;
}

bool TScreen::SetExclusivePane(int32_t panenum, bool completeexclusion)
{
    if (!panes[panenum] || numexclusive >= NUMEXCLUSIVEPANES)
        return false;

    exclusive[numexclusive] = panenum;
    complete[numexclusive] = completeexclusion;
    numexclusive++;

    return true;
}

void TScreen::ReleaseExclusivePane(int32_t panenum)
{
    int32_t pos = 0;
    for (int32_t c = 0; c < numexclusive; c++)
    {
        if (exclusive[c] != panenum)
        {
            exclusive[pos] = exclusive[c];
            pos++;
        }
    }

    numexclusive = pos;
}

void TScreen::RedrawAllPanes()
{
    Display->Reset();

  // Do exclusive
    if (numexclusive > 0)
    {
        PTPane pane = panes[exclusive[numexclusive - 1]];
        if (pane && !pane->IsHidden())
            pane->SetDirty(true);

        if (complete[numexclusive - 1])
        {
            Display->Reset();
            return;
        }
    }

  // Do pane list
    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
        if (panes.Used(loop) && !panes[loop]->IsHidden())
            panes[loop]->SetDirty(true);

    Display->Reset();
}

// *****************************
// * Virtual Handler Functions *
// *****************************

void TScreen::DrawBackground()
{
    Display->Reset();

  // Do exclusive
    if (numexclusive > 0)
    {
        PTPane pane = panes[exclusive[numexclusive - 1]];
        if (pane && !pane->IsHidden())
        {
            if (dirty)
                pane->Update();

            pane->SetClipRect();
            pane->DrawBackground();
        }

        if (complete[numexclusive - 1])
        {
            dirty = false;
            Display->Reset();
            return;
        }
    }

  // Do pane list
    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop) || panes[loop]->IsHidden())
            continue;

        if (dirty)
            panes[loop]->Update();

        panes[loop]->SetClipRect();
        panes[loop]->DrawBackground();
    }

    dirty = false;
    Display->Reset();
}

void TScreen::Pulse()
{
    Display->Reset();

  // Do exclusive
    if (numexclusive > 0)
    {
        PTPane pane = panes[exclusive[numexclusive - 1]];
        if (pane && !pane->IsHidden())
        {
            pane->SetClipRect();
            pane->Pulse();
        }

        if (complete[numexclusive - 1])
        {
            Display->Reset();
            return;
        }
    }

  // Do pane list
    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop) || panes[loop]->IsHidden())
            continue;
        panes[loop]->SetClipRect();
        panes[loop]->Pulse();
    }

    Display->Reset();
}

void TScreen::Animate(bool draw)
{
    Display->Reset();

  // Do exclusive
    if (numexclusive > 0)
    {
        PTPane pane = panes[exclusive[numexclusive - 1]];
        if (pane && !pane->IsHidden())
        {
            pane->SetClipRect();
            pane->Animate(draw);
        }

        if (complete[numexclusive - 1])
        {
            Display->Reset();
            return;
        }
    }

  // Do pane list
    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop) || panes[loop]->IsHidden())
            continue;
        panes[loop]->SetClipRect();
        panes[loop]->Animate(draw);
    }

    Display->Reset();
}

void TScreen::MouseClick(int32_t button, int32_t x, int32_t y)
{
    Display->Reset();

  // Do exclusive
    if (numexclusive > 0)
    {
        PTPane pane = panes[exclusive[numexclusive - 1]];
        if (pane && !pane->IsHidden())
        {
            pane->SetClipRect();
            pane->MouseClick(button, x - pane->GetPosX(), y - pane->GetPosY());
        }
        Display->Reset();
        return;
    }

  // Do pane list
    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop) || panes[loop]->IsHidden() || panes[loop]->IsIgnoringInput())
            continue;

        panes[loop]->SetClipRect();
        int32_t nx = x - panes[loop]->GetPosX();
        int32_t ny = y - panes[loop]->GetPosY();

      // send mouseup buttons to all panes, not just the owner of that screen space
        if (panes[loop]->InPane(nx, ny) ||
            (button == MB_LEFTUP || button == MB_RIGHTUP || button == MB_MIDDLEUP))
            panes[loop]->MouseClick(button, nx, ny);
    }

    Display->Reset();
}

void TScreen::MouseMove(int32_t button, int32_t x, int32_t y)
{
    Display->Reset();

  // Do exclusive
    if (numexclusive > 0)
    {
        PTPane pane = panes[exclusive[numexclusive - 1]];
        if (pane && !pane->IsHidden())
        {
            pane->SetClipRect();
            pane->MouseMove(button, x - pane->GetPosX(), y - pane->GetPosY());
        }
        Display->Reset();
        return;
    }

  // Do pane list
    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop) || panes[loop]->IsHidden() || panes[loop]->IsIgnoringInput())
            continue;

        panes[loop]->SetClipRect();
        int32_t nx = x - panes[loop]->GetPosX();
        int32_t ny = y - panes[loop]->GetPosY();
        panes[loop]->MouseMove(button, nx, ny);
    }

    Display->Reset();
}

void TScreen::KeyPress(int32_t key, bool down)
{
    Display->Reset();

  // Do exclusive
    if (numexclusive > 0)
    {
        PTPane pane = panes[exclusive[numexclusive - 1]];
        if (pane && !pane->IsHidden())
            pane->KeyPress(key, down);
        Display->Reset();
        return;
    }

    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop) || panes[loop]->IsHidden() || panes[loop]->IsIgnoringInput())
            continue;
        panes[loop]->SetClipRect();
        panes[loop]->KeyPress(key, down);
    }

    Display->Reset();
}

void TScreen::CharPress(int32_t key, bool down)
{
    Display->Reset();

  // Do exclusive
    if (numexclusive > 0)
    {
        PTPane pane = panes[exclusive[numexclusive - 1]];
        if (pane && !pane->IsHidden())
            pane->CharPress(key, down);
        Display->Reset();
        return;
    }

    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop) || panes[loop]->IsHidden() || panes[loop]->IsIgnoringInput())
            continue;
        panes[loop]->SetClipRect();
        panes[loop]->CharPress(key, down);
    }

    Display->Reset();
}

void TScreen::Joystick(int32_t key, bool down)
{
    Display->Reset();

  // Do exclusive
    if (numexclusive > 0)
    {
        PTPane pane = panes[exclusive[numexclusive - 1]];
        if (pane && !pane->IsHidden())
            pane->Joystick(key, down);
        Display->Reset();
        return;
    }

    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop) || panes[loop]->IsHidden() || panes[loop]->IsIgnoringInput())
            continue;

        panes[loop]->SetClipRect();
        panes[loop]->Joystick(key, down);
    }

    Display->Reset();
}

// ***************************
// * Screen System Functions *
// ***************************

TScreen* TScreen::ShowScreen(TScreen* screen, int32_t ticks)
{
    if (!screen)
        return nullptr;

    CurrentScreen = screen;

    CurrentScreen->screenframes = 0;

    // clear the screen
//  Display->Clear(0, 0xffff, 0, DM_NOCLIP | DM_NORESTORE);
//  Display->FlipPage();
//  Display->Clear(0, 0xffff, 0, DM_NOCLIP | DM_NORESTORE);

    bool init = CurrentScreen->BeginScreen();

    if (init)
    {
        CurrentScreen->TimerLoop(ticks);
        CurrentScreen->EndScreen();
    }

    CurrentScreen = nullptr;

    if (init && !Closing)
        return screen->GetNextScreen();
    else
        return nullptr;
}

bool TScreen::TimerTick(bool draw)
{
    // Make sure we pick up where we left off if TimerTick() is called recursively
    // from the message handling functions!
    static bool washandlingmessage = false;
    MSG  Message;

    HWND hwnd = MainWindow.Hwnd();

    if (hwnd == nullptr || Closing || !Display->Width())
        return false;

  // Update game toggles here...
  // ***************************

  // Allow scrolling of ZBuffer if game speed is high enough
    if (GameSpeed >= 4)
        NoScrollZBuffer = false;

  // ***************************
  // End update game toggles

    if (!washandlingmessage)
    {
      // Process windows messages
        washandlingmessage = true;
        while (PeekMessage(&Message, hwnd, 0, 0, PM_REMOVE))
        {
            if (Message.message == WM_QUIT)
            {   
                Closing = true;
                return false;
            }
            if (Message.message == WM_PAINT)
                ValidateRect(hwnd, nullptr);
            else
            {
                TranslateMessage(&Message);
                DispatchMessage(&Message);
            }
        }

      // If no current screen, forget it!
        if (CurrentScreen != this || Closing)
            return false;

      // Simulate mouse messages by polling the mouse
        POINT cursorpos;
        GetCursorPos(&cursorpos);
        RECT r;
        GetClientRect(MainWindow.Hwnd(), &r);
        ClientToScreen(MainWindow.Hwnd(), (LPPOINT)&r);
        cursorpos.x -= r.left;
        cursorpos.y -= r.top;

        // only bother if it's changed
        if (cursorx != cursorpos.x || cursory != cursorpos.y)
        {
            cursorx = cursorpos.x;
            cursory = cursorpos.y;

            CurrentScreen->MouseMove(mousebutton, cursorx, cursory);
        }

      // Get joystick state and send down to screen
        uint32_t joystate, joychanged, dbljoystate, dbljoychanged;
        GetJoystickState(0, &joystate, &joychanged, &dbljoystate, &dbljoychanged);
        if (joychanged || dbljoychanged)
        {
            int32_t joykey = -1;
            bool joydown = false;

            while (GetJoystickKeyCode(joystate, joychanged, dbljoystate, dbljoychanged, joykey, joydown))
                Joystick(joykey, joydown);
        }

      // Reset clipping stuff
        Display->Reset();

      // Resize pane before pulsing (and possibly setting new map position)
        for (int32_t loop = 0; loop < panes.NumItems(); loop++)
        {
            if (!panes.Used(loop))
                continue;

          // Update pane sizes
            if (panes[loop]->WasResized())
                panes[loop]->PaneResized();
        }

      // Pulse the screen (and it's panes) before we do any drawring....
        Pulse();    

      // Now update pane scroll position (after pulsing and possibly setting new map pos)
        for (loop = 0; loop < panes.NumItems(); loop++)
        {
            if (!panes.Used(loop))
                continue;

          // Set panes do dirty if screen is dirty
            if (dirty)
                panes[loop]->SetDirty(true);    

          // Update the scrollpos for panes 
            panes[loop]->UpdateBackgroundScrollPos();
        }

      // Calls the DrawBackground() function
      // Note: All drawing is done BEFORE the dirty rectangle restore functions are called.
      // This means that you can actually override what the restore system would do before
      // it has a chance to do it.  Any background draws will merge their update rectangles
      // with the restore rects in the restore system, thus reducing the total amount of the
      // display to restore.  If you update the entire background, all restore rects are
      // deleted (this actually happens during screen scrolling).

        if (draw)
            DrawBackground();

      // Restores the video buffer
        if (draw)
            Display->RestoreBackgroundAreas();

    }

    washandlingmessage = false;

 // Call screen's Animate() virtual function
    Display->Reset();
    Animate(draw);

 // Draw new mouse cursor
    Display->Reset();
    if (draw)
        DrawMouseCursor();  

    return true;
}

bool TScreen::TimerLoop(int32_t ticks)
{
    uint32_t totalticks, totalframes, skipframes;
    float showrate, framerate, frameaccum;
    bool lastskipped;

    showrate = framerate = (float)FRAMERATE;
    frameaccum = (float)0.0;
    totalticks = totalframes = skipframes = 0;
    lastskipped = false;

    while (true)
    {
      // If clock has ticked, call timertick
        if (!DisableTimer)
            Timer.WaitForTick();

        while (PauseWhenNotActive && !AppActive)
        {
          // Process windows messages
            MSG  Message;
            HWND hwnd = MainWindow.Hwnd();

            GetMessage(&Message, hwnd, 0, 0);       // use getmessage instead of peekmessage for pause

            if (Message.message == WM_QUIT)
            {
                Closing = true;
                return false;
            }
            if (Message.message == WM_PAINT)
                ValidateRect(hwnd, nullptr);
            else
            {
                TranslateMessage(&Message);
                DispatchMessage(&Message);
            }
        }
        
        if (CurrentScreen)
        {
            static uint32_t lastcount;

            LastFrameTicks = GetTickCount() - lastcount;
            lastcount = GetTickCount();

            totalticks += LastFrameTicks;
            totalframes++;
            if (lastskipped)
                skipframes++;

            if (PauseFrameSkip)
                totalframes = totalticks = skipframes = 0;

            if (totalframes >= 5)
            {
                showrate = (float)1000.0 / (float)(totalticks / totalframes);
                int32_t realframes = totalframes - skipframes;
                if (realframes <= 0)
                    realframes = 1; 
                framerate = (float)1000.0 / (float)(totalticks / realframes);

                if (skipframes >= totalframes)
                    PauseFrameSkip = true;

                totalticks = 0;
                totalframes = 0;
                skipframes = 0;
            }

            if (ShowFramesPerSecond)
            {
                char buf[80];
                int32_t usleep = MapPane.GetUpdateSleep();
                sprintf(buf, "Frame rate: %4.1f %4.1f %d - %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s", showrate, framerate, usleep,
                    (SmoothScroll)?"Scr ":"",
                    (NoFrameSkip)?"NSkp ":"",
                    (NoScrollZBuffer)?"NSZb ":"",
                    (ShowDrawing)?"Shw ":"",
                    (NoNormals)?"NNrm ":"",
                    (ClearBeforeDraw)?"Clr ":"",
                    (FlatShade)?"FSh ":"",
                    (DitherEnable)?"Dth ":"",
                    (BlendEnable)?"Bln ":"",
                    (ZEnable)?"Zbf ":"",
                    (SpecularEnable)?"Spc ":"",
                    (UseTextures)?"Tex ":"",
                    (BilinearFilter)?"Flt ":"",
                    (NoUpdateRects)?"NUp ":"",
                    (Show3D)?"3D ":"",
                    (GridSnap)?"Grd ":"",
                    (UseDrawPrimitive)?"Dpr ":"",
                    (NoPulseObjs)?"NPls ":"",
                    (NoAnimateObjs)?"NAni ":"",
                    (Interpolate)?"":"NInt ",
                    (NoAI)?"NAI ":"");
                Display->Reset(); // Reset display clipping rect
                Display->WriteText(buf, 
                    MapPane.GetPosX() + 10,
                    MapPane.GetPosY() + 10,
                    1, SystemFont, nullptr, DM_TRANSPARENT | DM_ALIAS);
            }
            
            frameaccum += framerate; // Accumulator is greater than framerate to draw a frame

          // Do we draw this frame?
            if (frameaccum > (FRAMERATE - 0.5) ||               // Frame rate caught up or..
                firstframe || PauseFrameSkip || NoFrameSkip ||  // game flags say draw every frame or..
                VideoCapture.IsCapturing())                     // if capturing video frames
            {
                PauseFrameSkip = false;         // resume normal frame skipping

                Timer.ResetTick();
                if (frameaccum > (float)100.0)
                    frameaccum = (float)0.0;
                else
                    frameaccum -= FRAMERATE;

                if (!firstframe)
                    Display->FlipPage(true);  // Don't flip page the first time
                firstframe = false;

                if (!CurrentScreen->TimerTick(true))
                    return false;

                if (VideoCapture.IsCapturing())
                    VideoCapture.SaveFrame();  

                screenframes++;

                lastskipped = false;
            }
            else
            {
                if (!CurrentScreen->TimerTick(false))
                    return false;

                screenframes++;

                lastskipped = true;
            }
        }

        if (ticks)
        {
            ticks--;
            if (!ticks)
                return true;
        }
    }

    return false;
}

// *******************
// * TPane Functions *
// *******************

bool TPane::Initialize() 
{ 
    if (isopen)
        return true;
    
    x = newx; y = newy; width = newwidth; height = newheight;

    backgroundbuffer = -1; 
    oldscrollx = scrollx = newscrollx = oldscrolly = scrolly = newscrolly = 0;
    dirty = true;
    ignoreinput = false;

  // Create any background buffers for this pane
    CreateBackgroundBuffers();

    isopen = true;

    return true; 
}

void TPane::Close()
{
  // Free all background buffers for this pane  
    FreeBackgroundBuffers();

    isopen = false;
}

// Causes the pane to be immediately shown on the screen.  Useful for when
// the pane contains a status or 'loading' bar that is updated during a single
// timer tick.
void TPane::PutToScreen()
{
    Display->PutToScreen(x, y, width, height);
}

// This function can be called to draw a pane immediately (instead of waiting for the
// screen TimerTick() function to call the various draw routines.  Note that no user
// input will be processed, and only the Pulse(), DrawBackground(), and Animate() functions
// are called.  The PutToScreen() function can be called immediately after this function
// to render the results to the display.
void TPane::Draw()
{
    if (!IsOpen() || IsHidden())
        return;

    SClipState cs;
    Display->SaveClipState(cs);

    SetClipRect();

    Pulse();
    DrawBackground();
    Animate(true);

    Display->RestoreClipState(cs);
}


void TPane::SetClipRect()
{
    Display->SetOrigin(x - scrollx, y - scrolly);
    Display->SetClipRect(x, y, width, height);
    Display->SetClipMode(CLIP_EDGES);
}

void TPane::UpdateBackgroundScrollPos()
{
    oldscrollx = scrollx; oldscrolly = scrolly;
    scrollx = newscrollx; scrolly = newscrolly;
    if (backgroundbuffer >= 0)
        Display->ScrollBackground(backgroundbuffer, scrollx, scrolly);
}   

void TPane::DrawRestoreRect(int32_t x, int32_t y, int32_t width, int32_t height, uint32_t drawmode)
{
    Display->DrawRestoreRect(backgroundbuffer, x, y, width, height, drawmode);
}

bool TPane::IsOnScreen()
{
    return screen == CurrentScreen;
}
