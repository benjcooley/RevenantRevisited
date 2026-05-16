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
#include "mappane.h"
#include "screen.h"
#include "sound.h"
#include "time.h"

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
    // TODO(port): InitBackgroundSystem removed — CPU background caching obsolete under sokol GPU compositor.
    firstframe = true;  // Tell timer tick function not to flip a page the first time
    done = false;
    lastPulseLegacyFrame = TTime::LegacyFrameCount() - 1;  // catch up to current on first tick

    panes.Clear();
    screenframes = 0;
    for (int32_t loop = 0; loop < NUMEXCLUSIVEPANES; loop++)
        exclusive[loop] = 0;
    numexclusive = 0;

    return Initialize();
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
    // TODO(port): Display.PutToScreen removed — sokol handles presentation.
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
    Display.Reset();

  // Do exclusive
    if (numexclusive > 0)
    {
        PTPane pane = panes[exclusive[numexclusive - 1]];
        if (pane && !pane->IsHidden())
            pane->SetDirty(true);

        if (complete[numexclusive - 1])
        {
            Display.Reset();
            return;
        }
    }

  // Do pane list
    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
        if (panes.Used(loop) && !panes[loop]->IsHidden())
            panes[loop]->SetDirty(true);

    Display.Reset();
}

// *****************************
// * Virtual Handler Functions *
// *****************************

void TScreen::DrawBackground()
{
    Display.Reset();

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
            Display.Reset();
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
    Display.Reset();
}

void TScreen::Pulse()
{
    Display.Reset();

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
            Display.Reset();
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

    Display.Reset();
}

void TScreen::Animate(bool draw)
{
    Display.Reset();

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
            Display.Reset();
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

    Display.Reset();
}

void TScreen::MouseClick(int32_t button, int32_t x, int32_t y)
{
    Display.Reset();

  // Do exclusive
    if (numexclusive > 0)
    {
        PTPane pane = panes[exclusive[numexclusive - 1]];
        if (pane && !pane->IsHidden())
        {
            pane->SetClipRect();
            pane->MouseClick(button, x - pane->GetPosX(), y - pane->GetPosY());
        }
        Display.Reset();
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

    Display.Reset();
}

void TScreen::MouseMove(int32_t button, int32_t x, int32_t y)
{
    Display.Reset();

  // Do exclusive
    if (numexclusive > 0)
    {
        PTPane pane = panes[exclusive[numexclusive - 1]];
        if (pane && !pane->IsHidden())
        {
            pane->SetClipRect();
            pane->MouseMove(button, x - pane->GetPosX(), y - pane->GetPosY());
        }
        Display.Reset();
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

    Display.Reset();
}

void TScreen::KeyPress(int32_t key, bool down)
{
    Display.Reset();

  // Do exclusive
    if (numexclusive > 0)
    {
        PTPane pane = panes[exclusive[numexclusive - 1]];
        if (pane && !pane->IsHidden())
            pane->KeyPress(key, down);
        Display.Reset();
        return;
    }

    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop) || panes[loop]->IsHidden() || panes[loop]->IsIgnoringInput())
            continue;
        panes[loop]->SetClipRect();
        panes[loop]->KeyPress(key, down);
    }

    Display.Reset();
}

void TScreen::CharPress(int32_t key, bool down)
{
    Display.Reset();

  // Do exclusive
    if (numexclusive > 0)
    {
        PTPane pane = panes[exclusive[numexclusive - 1]];
        if (pane && !pane->IsHidden())
            pane->CharPress(key, down);
        Display.Reset();
        return;
    }

    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop) || panes[loop]->IsHidden() || panes[loop]->IsIgnoringInput())
            continue;
        panes[loop]->SetClipRect();
        panes[loop]->CharPress(key, down);
    }

    Display.Reset();
}

void TScreen::Joystick(int32_t key, bool down)
{
    Display.Reset();

  // Do exclusive
    if (numexclusive > 0)
    {
        PTPane pane = panes[exclusive[numexclusive - 1]];
        if (pane && !pane->IsHidden())
            pane->Joystick(key, down);
        Display.Reset();
        return;
    }

    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop) || panes[loop]->IsHidden() || panes[loop]->IsIgnoringInput())
            continue;

        panes[loop]->SetClipRect();
        panes[loop]->Joystick(key, down);
    }

    Display.Reset();
}

// ***************************
// * Screen System Functions *
// ***************************

// Non-blocking screen startup. Under sokol_app there is no message pump for us
// to own, so ShowScreen no longer runs a TimerLoop — it just Initialize()s the
// screen and hands control back to AppFrame, which drives TimerTick() per sokol
// frame and calls EndCurrentScreen() once IsDone() flips true.
TScreen* TScreen::ShowScreen(TScreen* screen, int32_t /*ticks*/)
{
    if (!screen)
        return nullptr;

    CurrentScreen = screen;
    if (!screen->BeginScreen())
    {
        CurrentScreen = nullptr;
        return nullptr;
    }
    return screen;
}

void TScreen::EndCurrentScreen()
{
    if (!CurrentScreen)
        return;
    CurrentScreen->EndScreen();
    CurrentScreen = nullptr;
}

// One non-blocking tick. Called once per sokol AppFrame.
//
// Pulses catch up to the 24Hz legacy frame counter in scaled simulation time.
// Slow-motion scales leave extra render frames for interpolation; fast scales
// may process more than one fixed legacy tick before drawing.
// DrawBackground/Animate run every call since sokol presents at vsync.
// Input is delivered out-of-band via AppEvent, so there is no message pump
// here any more. Returns false when the screen has set `done` — AppFrame
// uses that as the signal to EndCurrentScreen and advance.
// Sim half of the frame: catch up any pending 24Hz Pulses. Pure logic;
// must not produce sokol draw calls. See docs/FRAME_PIPELINE.md.
void TScreen::Tick()
{
    if (Closing)
        return;

    // Resize panes that asked for it before we pulse (panes may set new
    // map position in their Pulse, which depends on post-resize dims).
    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop))
            continue;
        if (panes[loop]->WasResized())
            panes[loop]->PaneResized();
    }

    // Catch up missed Pulses. At 1x / 60Hz this is 0 or 1 per call;
    // debug time scales deliberately alter that cadence.
    const int64_t lf = TTime::LegacyFrameCount();
    while (lastPulseLegacyFrame < lf)
    {
        Pulse();
        lastPulseLegacyFrame++;
        screenframes++;
    }
}

// Draw half of the frame. Two-phase to keep sokol's "one pass active
// at a time" rule:
//
//   Phase 1 — Animate (3D scene + screen-owned passes). The screen
//             manages its own sokol passes here (TMapRenderer opens
//             G-buffer, light, etc. on its own offscreen targets).
//             NO overlay pass is open during Animate, so screens are
//             free to call sg_begin_pass.
//
//   Phase 2 — Overlay2D pass on the backbuffer. While this is open
//             every legacy Display.Put / Blit / Line lands in the
//             backbuffer texture. We bracket DrawBackground (pane
//             backdrops drawn on top of 3D) and DrawMouseCursor
//             inside this pass. FlipPage then composites the
//             backbuffer over the 3D scene with alpha blend.
//
// See docs/FRAME_PIPELINE.md.
void TScreen::DrawFrame()
{
    if (Closing || !Display.IsActive())
        return;

    // Propagate dirty + update pane scroll. Cheap; safe to run draw-side
    // since Pulse already happened.
    for (int32_t loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop))
            continue;
        if (dirty)
            panes[loop]->SetDirty(true);
        panes[loop]->UpdateBackgroundScrollPos();
    }

    // --- Phase 1: 3D scene + screen-owned passes -------------------------
    Display.Reset();
    Animate(true);

    // --- Phase 2: Overlay2D -- legacy 2D blits land in the backbuffer ----
    // Overlay2D pass (legacy Display.Put backbuffer fallback). HUD
    // drawing now goes through Renderer's HUD items (registered in the
    // runtime mode's OnEnter, drawn by Renderer->DrawHud inside the
    // swapchain pass in FlipPage). The overlay backbuffer is still
    // opened so any not-yet-migrated Display.Put caller (DrawBackground,
    // etc.) has a pass to draw into, even if those draws don't currently
    // surface visually under sokol.
    Display.BeginOverlay();

    Display.Reset();
    if (!firstframe)
    {
        DrawBackground();
    }

    Display.EndOverlay();

    firstframe = false;
}

// Legacy entry point. Kept so callers in test modes and anywhere we
// haven't migrated yet continue to work. New code should call Tick()
// and DrawFrame() separately. TODO(frame-pipeline): remove once last
// caller migrates.
bool TScreen::TimerTick(bool draw)
{
    if (Closing)
        return false;

    Tick();
    if (draw)
        DrawFrame();

    return !done;
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
    // TODO(port): Display.PutToScreen removed — sokol handles presentation.
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
    Display.SaveClipState(cs);

    SetClipRect();

    Pulse();
    DrawBackground();
    Animate(true);

    Display.RestoreClipState(cs);
}


void TPane::SetClipRect()
{
    Display.SetOrigin(x - scrollx, y - scrolly);
    Display.SetClipRect(x, y, width, height);
    Display.SetClipMode(CLIP_EDGES);
}

void TPane::UpdateBackgroundScrollPos()
{
    oldscrollx = scrollx; oldscrolly = scrolly;
    scrollx = newscrollx; scrolly = newscrolly;
    // TODO(port): Display.ScrollBackground removed — CPU background caching obsolete under sokol GPU compositor.
}

void TPane::DrawRestoreRect(int32_t x, int32_t y, int32_t width, int32_t height, uint32_t drawmode)
{
    (void)x; (void)y; (void)width; (void)height; (void)drawmode;
    // TODO(port): Display.DrawRestoreRect removed — CPU background caching obsolete under sokol GPU compositor.
}

bool TPane::IsOnScreen()
{
    return screen == CurrentScreen;
}
