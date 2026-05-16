// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 screen.h  - EXILE Screen Include File                 *
// *************************************************************************

#pragma once

#define NUMEXCLUSIVEPANES 4

#include "revenant.h"

#include <vector>

// ****************************************************************************
// * Retained-mode layout primitives (A.2b)                                   *
// *                                                                          *
// * Rudimentary 2-pass measure/layout (Unity IMGUI-ish / single-axis flex).  *
// * See docs/ui/ARCHITECTURE.md sec 2.1 and memory project-ui-layout-system  *
// * for the "what NOT to build" list. These types are intentionally small.   *
// *                                                                          *
// * Container panes (layoutKind != None) measure children bottom-up, then    *
// * arrange them top-down inside their content rect (own rect minus padding).*
// * Per-child sizing policy controls how leftover space is distributed:      *
// * Fixed children get their preferred size; Greedy children share the       *
// * remainder weighted by greedyWeight.                                      *
// *                                                                          *
// * Non-container panes (layoutKind == None, the default) ignore all of      *
// * this and continue using their explicit x/y/w/h. Vanilla retail panes     *
// * pay zero cost.                                                           *
// ****************************************************************************

enum class SLayoutKind  : uint8_t { None, Vertical, Horizontal };
enum class SSizePolicy  : uint8_t { Fixed, Greedy };

// 3x3 grid of anchor points + None default. An anchored child is positioned
// against its parent's content rect (own rect minus padding), keeping its
// own size; margin acts as inset from the anchored edge. Only applies when
// the parent's layoutKind == None -- V/H containers position children via
// the flex pass. Default None preserves vanilla "use explicit (x,y)".
enum class SAnchor : uint8_t {
    None,
    TopLeft,    TopCenter,    TopRight,
    CenterLeft, Center,       CenterRight,
    BottomLeft, BottomCenter, BottomRight,
};

struct SSpacing  // left/top/right/bottom in pixels; default zero
{
    int32_t left   = 0;
    int32_t top    = 0;
    int32_t right  = 0;
    int32_t bottom = 0;

    constexpr SSpacing() = default;
    constexpr SSpacing(int32_t l, int32_t t, int32_t r, int32_t b)
      : left(l), top(t), right(r), bottom(b) {}
    constexpr explicit SSpacing(int32_t uniform)
      : left(uniform), top(uniform), right(uniform), bottom(uniform) {}

    [[nodiscard]] constexpr int32_t Horizontal() const { return left + right; }
    [[nodiscard]] constexpr int32_t Vertical()   const { return top + bottom; }
};

struct SSize     // preferred / arranged size for a layout node
{
    int32_t w = 0;
    int32_t h = 0;

    constexpr SSize() = default;
    constexpr SSize(int32_t pw, int32_t ph) : w(pw), h(ph) {}
};

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

    // Retained-mode tree (A.2a). Non-owning — lifetime of children is managed
    // by whoever created them, matching TScreen's TPaneArray convention.
    // Empty by default; vanilla screens that never call AddChild still walk
    // through TScreen's flat pane array exactly as before.
    TPane* parent = nullptr;
    std::vector<TPane*> children;

    // Layout policy (A.2b). Defaults below make the pane a leaf with no
    // container behavior and Fixed sizing -- vanilla retail panes behave
    // exactly as before until something explicitly opts in.
    SLayoutKind layoutKind   = SLayoutKind::None;
    SSizePolicy hsizePolicy  = SSizePolicy::Fixed;
    SSizePolicy vsizePolicy  = SSizePolicy::Fixed;
    SAnchor     anchor       = SAnchor::None;  // anchor against None-layout parent
    float       greedyWeight = 1.0f;   // share when Greedy among siblings
    SSpacing    padding;               // inside container, around children
    SSpacing    margin;                // outside this pane, inside parent (and inset from anchored edge)
    int32_t     spacing      = 0;      // between siblings in V/H container

    // Cached measure result -- populated by MeasureSelf() during pass 1,
    // consumed by parents during pass 2 to compute arrange rects. Not part
    // of the public API; do not read directly.
    SSize       measured;

    // Clip rect (A.2f). When set, the pane signals that its draw output
    // should be clipped to this screen-space rectangle. Renderer-side
    // scissoring is a TODO -- it lands when the first B-phase pane that
    // wants real clipping wires it through DrawHud. The API exists now
    // so consumers can opt in cleanly when the renderer side arrives.
    bool        hasClip = false;
    SRect       clipRect{};

    // Alpha (soft-edge) clip widths (A.2g). Per-edge fade-in distance from
    // each clip-rect edge in pixels. 0 = hard clip on that edge (the
    // default; behaves exactly like A.2f hard-clip). Non-zero values
    // signal a shader-side fade: content alpha ramps 0->1 across that
    // distance from the edge. Render-side implementation is TODO (lands
    // with the first scroll consumer that wants soft visual edges).
    SSpacing    clipFade{};

   public:

    TPane() {}
    TPane(int32_t px, int32_t py, int32_t pw, int32_t ph, bool phide = false)
      { newx = x = px; newy = y = py; newwidth = width = pw; newheight = height = ph; hidden = phide; }
      // Create pane

    virtual ~TPane();
      // Unlinks from parent + nulls children's back-pointers. Children are not
      // deleted (non-owning). See AddChild for ownership notes.

  // Init & Close functions
    virtual bool Initialize();
      // Initializes pane before it is displayed
    virtual void Close();
      // Called for pane to delete internal structures before closing
    bool IsOpen() const { return isopen; }

  // Pane state variables
    int32_t GetPosX() const { return x; }
      // Gets pane screen x pos
    int32_t GetPosY() const { return y; }
      // Gets pane screen y pos
    int32_t GetWidth() const { return width; }
      // Gets width
    int32_t GetHeight() const { return height; }
      // Gets height
    void GetRect(SRect &r) const { r.left = x; r.top = y; r.right = r.left + width - 1; r.bottom = r.top + height - 1; }
      // Gets the pane's rectangle
    bool InPane(int32_t px, int32_t py) const { return (px >= 0 && py >= 0 && px < width && py < height); }
      // Quick bounds checker
    bool IsDirty() const { return dirty; }
      // Get update status

  // Scrolling functions
    void SetScrollPos(int32_t sx, int32_t sy) { newscrollx = sx; newscrolly = sy; }
      // Sets scroll position of pane for next frame (returned with GetNewScrollX, ScrollY)
    int32_t GetScrollX() const { return scrollx; }
      // Gets pane draw origin x pos for current frame
    int32_t GetScrollY() const { return scrolly; }
      // Gets pane draw origin y pos for current frame
    int32_t GetOldScrollX() const { return oldscrollx; }
      // Gets pane draw origin x pos for previous frame
    int32_t GetOldScrollY() const { return oldscrolly; }
      // Gets pane draw origin y pos for previous frame
    int32_t GetNewScrollX() const { return newscrollx; }
      // Gets pane draw origin x pos for next frame
    int32_t GetNewScrollY() const { return newscrolly; }
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
    int32_t GetBackgroundBuffer() const { return backgroundbuffer; }
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
    bool WasResized() const
        { return x != newx || y != newy || width != newwidth || height != newheight; }
      // True if any changes to position or size was made during the previous frame
    virtual void SetClipRect();
      // Sets the display's origin and clipping rectangle to clip this pane
    virtual void Update() { SetDirty(true); }
      // Flag the pane as needing a background update
    virtual void SetDirty(bool newdirty)
      { dirty = newdirty; if (newdirty && parent) parent->SetDirty(true); }
      // For setting the update status explicitly. Marking dirty propagates
      // up the parent chain so the renderer / layout cache knows the subtree
      // needs work; marking clean does NOT propagate (siblings may still be
      // dirty).
    
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

  // Retained-mode hierarchy (A.2a).
  //
  // Ownership is non-owning: children pointers are weak references, exactly
  // like TScreen's TPaneArray. The caller that allocated the child is also
  // responsible for deleting it. AddChild / RemoveChild only maintain the
  // parent/child pointer relationship.
  //
  // If a pane gets deleted (its dtor runs), it unlinks itself from its
  // parent's children vector and nulls its children's parent pointers, so
  // neither side ends up dangling.
  //
  // No existing pane in the codebase uses this today -- screens still
  // manage a flat TPaneArray. Children only come into play for panes that
  // explicitly opt in (Phase A.2b/A.2c containers, A.2f scrollable
  // content viewports, etc.).
    void AddChild(TPane* child);
    void RemoveChild(TPane* child);
    TPane* GetParent() const { return parent; }
    const std::vector<TPane*>& Children() const { return children; }

  // Layout configuration (A.2b). Defaults match SLayoutKind::None, Fixed,
  // zero padding/margin -- vanilla pane behavior. Setters bump dirty so the
  // next layout pass re-evaluates.
    SLayoutKind GetLayoutKind() const   { return layoutKind; }
    void SetLayoutKind(SLayoutKind k)   { layoutKind = k; SetDirty(true); }

    SSizePolicy GetHSizePolicy() const  { return hsizePolicy; }
    SSizePolicy GetVSizePolicy() const  { return vsizePolicy; }
    void SetSizePolicy(SSizePolicy h, SSizePolicy v)
        { hsizePolicy = h; vsizePolicy = v; SetDirty(true); }

    float GetGreedyWeight() const       { return greedyWeight; }
    void  SetGreedyWeight(float w)      { greedyWeight = w; SetDirty(true); }

    const SSpacing& GetPadding() const  { return padding; }
    void SetPadding(const SSpacing& p)  { padding = p; SetDirty(true); }

    const SSpacing& GetMargin() const   { return margin; }
    void SetMargin(const SSpacing& m)   { margin = m; SetDirty(true); }

    int32_t GetSpacing() const          { return spacing; }
    void    SetSpacing(int32_t s)       { spacing = s; SetDirty(true); }

    SAnchor GetAnchor() const           { return anchor; }
    void    SetAnchor(SAnchor a)        { anchor = a; SetDirty(true); }

  // Clip rect (A.2f). Screen-space rectangle the pane wants clipped to.
  // No-op for vanilla panes that never opt in. Renderer-side scissoring
  // is the next consumer's job -- the API is the A.2f deliverable.
    bool HasClipRect() const            { return hasClip; }
    void GetClipRect(SRect& out) const  { out = clipRect; }
    void SetClipRect(const SRect& r)    { clipRect = r; hasClip = true; SetDirty(true); }
    void ClearClipRect()                { hasClip = false; SetDirty(true); }

  // Alpha (soft-edge) clip widths (A.2g). Per-edge fade-in pixel distances.
  // Zero on an edge = hard clip on that edge. Only meaningful when a clip
  // rect is set. Render-side fade shader is TODO.
    const SSpacing& GetClipFade() const { return clipFade; }
    void SetClipFade(const SSpacing& f) { clipFade = f; SetDirty(true); }

  // Two-pass layout (A.2b).
  //
  // MeasureSelf: pass 1, bottom-up. Returns this pane's preferred size given
  // a parent constraint. Default: leaf panes return their explicit
  // (newwidth, newheight); container panes recurse into children, accumulate
  // along their axis, and add padding. Subclasses override when preferred
  // size depends on content (text label, scaled image, etc.).
  //
  // LayoutChildren: pass 2, top-down. Called on container panes after their
  // own rect is set. Computes each child's arrange rect inside this pane's
  // content rect, applying child margin / sizing policy. Default no-op when
  // layoutKind == None.
  //
  // RunLayoutPass: convenience entry point. Performs MeasureSelf with the
  // pane's current size as the constraint, then LayoutChildren recursively.
  // Callers invoke this when they've changed the tree or this pane's size
  // and want layout up to date. No auto-invocation from TScreen yet -- A.2b
  // is purely additive.
    virtual SSize MeasureSelf(const SSize& parentConstraint);
    virtual void  LayoutChildren();
    void          RunLayoutPass();
    SSize         GetMeasured() const { return measured; }
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
