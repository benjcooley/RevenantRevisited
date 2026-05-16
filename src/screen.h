// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 screen.h  - EXILE Screen Include File                 *
// *************************************************************************

#pragma once

#define NUMEXCLUSIVEPANES 4

#include "revenant.h"

// ******************************
// * TPane - Screen pane object *
// ******************************

// The pane object creates a virtual view pane on a game screen.  The
// view pane has its own origin, its own clipping rectangle, and handles
// its own mouse movement, clicks, joystick, and keyboard input.

_CLASSDEF(TPane)
class TPane
{
  private:
    int32_t  x, y, width, height;             // Pane's position in screen coordinates
    int32_t  newx, newy, newwidth, newheight; // Size and position to change to on next frame
    int32_t  oldscrollx, oldscrolly;          // Previous scroll position
    int32_t  scrollx, scrolly;                // Current scroll position
    int32_t  newscrollx, newscrolly;          // Next scroll position
    TScreen* screen;                          // Screen pane is on
    bool isopen;                              // Pane is currently active
    bool hidden;                              // Flag set if Pane is hidden
    bool ignoreinput;                         // To allow hidden panes to still process input
    bool dirty;                               // Pane needs update
    int32_t  backgroundbuffer;                // Background buffer index

   public:

    TPane() {}
    TPane(int32_t px, int32_t py, int32_t pw, int32_t ph, bool phide = false)
      { newx = x = px; newy = y = py; newwidth = width = pw; newheight = height = ph; hidden = phide; }
      // Create pane

  // Init & Close functions
    virtual bool Initialize();
      // Initializes pane before it is displayed
    virtual void Close();
      // Called for pane to delete internal structures before closing
    bool IsOpen() { return isopen; }

  // Pane state variables
    int32_t GetPosX() { return x; }
      // Gets pane screen x pos
    int32_t GetPosY() { return y; }
      // Gets pane screen y pos
    int32_t GetWidth() { return width; }
      // Gets width
    int32_t GetHeight() { return height; }
      // Gets height
    void GetRect(SRect &r) { r.left = x; r.top = y; r.right = r.left + width - 1; r.bottom = r.top + height - 1; }
      // Gets the pane's rectangle
    bool InPane(int32_t x, int32_t y) { return (x >= 0 && y >= 0 && x < width && y < height); }
      // Quick bounds checker
    bool IsDirty() { return dirty; }
      // Get update status

  // Scrolling functions
    void SetScrollPos(int32_t sx, int32_t sy) { newscrollx = sx; newscrolly = sy; }
      // Sets scroll position of pane for next frame (returned with GetNewScrollX, ScrollY)         
    int32_t GetScrollX() { return scrollx; }
      // Gets pane draw origin x pos for current frame
    int32_t GetScrollY() { return scrolly; }
      // Gets pane draw origin y pos for current frame
    int32_t GetOldScrollX() { return oldscrollx; }
      // Gets pane draw origin x pos for previous frame
    int32_t GetOldScrollY() { return oldscrolly; }
      // Gets pane draw origin y pos for previous frame
    int32_t GetNewScrollX() { return newscrollx; }
      // Gets pane draw origin x pos for next frame
    int32_t GetNewScrollY() { return newscrolly; }
      // Gets pane draw origin y pos for next frame

  // Background stuff
    virtual void CreateBackgroundBuffers() {}
      // Allocates any background buffers used by the pane
    virtual void FreeBackgroundBuffers() {}
      // Frees any background buffers used by the pane
    virtual void SetBackgroundBuffer(int32_t buf) { backgroundbuffer = buf; }
      // Sets the special primary background buffer for this pane (used for scrolling)
    virtual void ClearBackgroundBuffer() { backgroundbuffer = -1; }
      // Clears the special primary background buffer for this pane
    int32_t GetBackgroundBuffer() { return backgroundbuffer; }
      // Gets restore buffer index.
    void UpdateBackgroundScrollPos();
      // Called by the screen TimerTick() function to update bg buf's scroll pos
    void DrawRestoreRect(int32_t x, int32_t y, int32_t width, int32_t height, 
        uint32_t drawmode =  DM_WRAPCLIPSRC | DM_NORESTORE | DM_ZBUFFER);
      // Restores the rectangle x,y,width,height to the screen for this pane

  // Pane display functions
    void PutToScreen();
      // Causes the pane to be immediately shown on the screen.  Useful for when
      // the pane contains a status or 'loading' bar that is updated during a single
      // timer tick.
    void Draw();
      // Manually calls the Pulse(), DrawBackground(), and Animate() functions.  Useful
      // for drawing within a timer tick (like a loading bar)
    void Resize(int32_t nx, int32_t ny, int32_t nwidth, int32_t nheight)
        { newx = nx; newy = ny; newwidth = nwidth; newheight = nheight; }
      // Resizes pane for next frame
    void SetPos(int32_t nx, int32_t ny) { newx = nx; newy = ny; }
      // Sets the pane's x, y position for next frame
    void SetSize(int32_t nwidth, int32_t nheight) { newwidth = nwidth; newheight = nheight; }
      // Sets the pane's width, height position for next frame
    bool WasResized()
        { return x != newx || y != newy || width != newwidth || height != newheight; }
      // True if any changes to position or size was made during the previous frame
    virtual void SetClipRect();
      // Sets the display's origin and clipping rectangle to clip this pane
    virtual void Update() { dirty = true; }
      // Flag the pane as needing a background update
    virtual void SetDirty(bool newdirty) { dirty = newdirty; }
      // For setting the update status explicitly
    
    virtual void Show() { hidden = false; ignoreinput = false; Update(); }
      // Draws pane's imagery to backbuffer
    virtual void Hide() { hidden = true; ignoreinput = true; }
      // Erases pane's imagery from backbuffer
    virtual void SetIgnoreInput(bool val) { ignoreinput = val; }
      // So that the child classes can set it manually
    virtual bool IsHidden() { return hidden || !IsOnScreen(); }
      // Returns the pane's visibility status
    virtual bool IsOnScreen();
      // Returns the pane's visibility status
    virtual bool IsIgnoringInput() { return ignoreinput; }
      // Whether or not the pane is processing input (keyboard only)
    TScreen* Screen() { return screen; }
      // Returns the screen this pane is currently on
    void SetScreen(TScreen* newscreen) { screen = newscreen; }
      // Called to set the pane's screen (CALLED ONLY BY SCREEN FUNCTIONS!!!)

  // Pane virtual handler functions
    virtual void PaneResized()
        { FreeBackgroundBuffers(); 
          x = newx; y = newy; width = newwidth; height = newheight; 
          CreateBackgroundBuffers(); 
         }
      // Called before anything else to resize the pane
    virtual void Pulse() {}
      // Called to cause pane to do object AI, set pane position, etc. before DrawBackground() is called
    virtual void DrawBackground() {}
      // Called before animation is drawn to allow drawing to background buf
    virtual void Animate(bool draw) {}
      // Animate function called every timer tick (draw is false if this frame is skipped)
    virtual void Overlay(bool draw) {}
      // Animate function called after 2d and 3d objects are rendered
    virtual void MouseClick(int32_t button, int32_t x, int32_t y) {}
      // Handles mouse clicks in pane.
    virtual void MouseMove(int32_t button, int32_t x, int32_t y) {}
      // Handles mouse movement in pane.
    virtual void KeyPress(int32_t key, bool down) {}
      // Handles keyboard presses in pane.
    virtual void CharPress(int32_t key, bool down) {}
      // Handles processed (ASCII) keyboard presses in pane.
    virtual void Joystick(int32_t key, bool down) {}
      // Handles joystick input in pane.

  // Converts points in pane to points on the screen
    void PaneToScreen(int32_t panex, int32_t paney, int32_t &screenx, int32_t &screeny)
      { screenx = x + panex - scrollx; screeny = y + paney - scrolly; }
    void PaneToScreen(SRect &r) 
      { r.left = x + r.left - scrollx; r.top = y + r.top - scrolly;
        r.right = x + r.right - scrollx; r.bottom = y + r.bottom - scrolly; }
};

// ********************************
// * TScreen - Game screen object *
// ********************************

// The TScreen object represents a game screen.  Each game screen can access
// the display surface and handle input directly, or it can have multiple
// TPane objects which handle various parts of the screen.

#define MAXSCREENPANES 32

typedef TPointerArray<TPane, MAXSCREENPANES> TPaneArray;
typedef TPointerIterator<TPane> TPaneIterator;

_CLASSDEF(TScreen)
class TScreen
{
  protected:
    TPaneArray panes;                   // Array of panes
    int32_t exclusive[NUMEXCLUSIVEPANES];   // Current exclusive pane list or nullptr if no exclusive.
    bool complete[NUMEXCLUSIVEPANES];   // Whether the exclusive pane is completely exclusive
    int32_t curexclusive;                   // Current exclusive pane
    int32_t numexclusive;                   // Number of exclusive panes
    TScreen* nextscreen;                // Pointer to nextscreen
    bool firstframe;                    // True just after screen is initialized before first frame
    bool dirty;                         // Needs redraw
    bool done = false;                  // Set by subclass when screen is ready to end (AppFrame transitions)
    int32_t screenframes;                   // Number of ticks since screen initialized
    int64_t lastPulseLegacyFrame = -1;  // Last TTime::LegacyFrameCount() value a Pulse was emitted at

  public:
    TScreen();
    virtual ~TScreen();

  // Initialization
    virtual bool Initialize() { return false; }
      // Initializes the screen.
    virtual void Close() {}
      // Closes the screen.
    void PutToScreen();
      // Causes the back buffer to be immediately shown on the users screen.  Useful for when
      // the screen contains a status or 'loading' bar that is updated during a single
      // timer tick.

  // Pane stuff
    int32_t FindPane(PTPane pane);
      // Returns the index to the given pane in the screen, or -1 if none
    int32_t AddPane(PTPane pane, int32_t panenum = -1);
      // Adds pane to screen at given panenum (or bottom if panenum = -1)
    bool RemovePane(PTPane pane);
      // Removes pane from the pane list (returns true if pane was actually in pane list)
    bool SetExclusivePane(int32_t panenum, bool completeexclusion = false);
      // Sets pane to handle all input/output (for error or popup panes)
      // If complete is true then *nothing* from the other panes (including
      // Animate() and DrawBackground()) will be called during exclusive mode.
    bool SetExclusivePane(PTPane pane, bool completeexclusion = false)
        { return SetExclusivePane(FindPane(pane), completeexclusion); }
      // Sets pane to handle all input/output (for error or popup panes)
    void ReleaseExclusivePane(int32_t panenum);
      // Releases exclusive pane
    void ReleaseExclusivePane(PTPane pane) { ReleaseExclusivePane(FindPane(pane)); }
      // Releases exclusive pane
    bool InCompleteExclusion() { return (numexclusive > 0 && complete[curexclusive]); }
      // Whether or not all i/o is stopped except for one pane
    bool FirstFrame() { return firstframe; }
      // Is this the first frame for this screen?
    void RedrawAllPanes();
      // Redraw all non-hidden panes

   // Next screen stuff
    void SetNextScreen(TScreen* screen) {nextscreen = screen;}
      // Sets nextscreen variable
    TScreen* GetNextScreen() {return nextscreen;}
      // Gets nextscreen variable

  // Virtual handlers
    virtual void Pulse();
      // Called to cause pane to do object AI, set pane position, etc. before DrawBackground() is called
    virtual void DrawBackground();
      // Draws to background before animation is drawm
    virtual void Animate(bool draw);
      // Animate function called every timer tick.
    virtual void MouseClick(int32_t button, int32_t x, int32_t y);
      // Handles mouse clicks to screen. Calls TPane MouseClick functions.
    virtual void MouseMove(int32_t button, int32_t x, int32_t y);
      // Handles mouse movement on screen.  Calls TPane MouseMove functions.
    virtual void KeyPress(int32_t key, bool down);
      // Handles keyboard presses. Calls TPane KeyPress functions.
    virtual void CharPress(int32_t key, bool down);
      // Handles shifted keypresses.
    virtual void Joystick(int32_t key, bool down);
      // Handles joystick input. Calls TPane Joystick functions.
    virtual void Redraw() { dirty = true; }
      // Redraw the current screen

  // Screen loops. See docs/FRAME_PIPELINE.md for the full picture.
    virtual void Tick();
      // Advance simulation. Catches up any pending 24Hz Pulses via
      // TTime::LegacyFrameCount. Pure logic, no drawing. Called by
      // AppFrame once per real frame.
    virtual void DrawFrame();
      // Render one frame. Opens the Overlay2D pass on the backbuffer,
      // calls Animate(true), draws the cursor, closes the pass. Does
      // NOT mutate gameplay state. Called by AppFrame after Tick().
    [[deprecated("call Tick() and DrawFrame() instead")]]
    virtual bool TimerTick(bool draw);
      // Legacy wrapper: Tick() + (draw ? DrawFrame() : nothing). Kept
      // for test modes and any caller not yet migrated. Returns !done.
      // TODO(frame-pipeline): remove once last caller migrates.

    static TScreen* ShowScreen(TScreen* screen, int32_t ticks);
      // Begins a screen: sets CurrentScreen and calls Initialize. Returns the
      // screen on success (still running; driven by AppFrame) or nullptr on
      // init failure. `ticks` is legacy and ignored. AppFrame is responsible
      // for detecting IsDone() and calling EndCurrentScreen.
    static void EndCurrentScreen();
      // Tears down CurrentScreen and clears the global pointer.

    [[nodiscard]] bool IsDone() const { return done; }
    void SetDone(bool v = true) { done = v; }

  // Get screen frames
    int32_t FrameCount() { return screenframes; }
      // Get the current frame number since this screen was initialized
    void ResetFrameCount() { screenframes = 0; }
      // Resets the screen framecount

  private:
    bool BeginScreen();
      // Calls all pane and screen initialize functions
    void EndScreen();
      // Calls all pane and screen close functions
};
