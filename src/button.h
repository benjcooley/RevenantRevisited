// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  button.h - EXILE button routines                     *
// *************************************************************************

#ifndef _BUTTON_H
#define _BUTTON_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _SCREEN_H
#include "screen.h"
#endif

#ifndef _BITMAP_H
#include "bitmap.h"
#endif

_CLASSDEF(TButton)
class TButton
{
  public:
    TButton(char *bname, int32_t bx, int32_t by, int32_t bw, int32_t bh, uint16_t keypr,
            void (*bfunc)(), PTBitmap dbm = nullptr, PTBitmap ubm = nullptr,
            bool rad = false, bool tog = false, bool notsquare = false, int32_t group = -1, int32_t repeat = 0);

    char *GetName() { return name; }
        // Name of the button
    bool OnButton(int32_t bx, int32_t by);
        // Check whether x, y is on the button
    bool IsKey(int32_t keypr, bool keydown);
        // Check whether the given keypress is that of the button
    bool IsDirty() { return dirty; }
        // Check update status
    bool IsToggle() { return toggle; }
        // Returns whether button is a toggle-type
    bool RadioGroup() { return radiogroup; }
        // Returns the radio group or -1 if none
    bool Repeats() { return (repeatrate > 0); }
        // Returns if the button is a repeater
    void SetDirty() { dirty = true; }
        // Sets update status
    bool IsHidden() { return hidden; }
    void Hide() { if (!hidden) { hidden = true; SetDirty(); } }
    void Show() { if (hidden) { hidden = false; SetDirty(); } }
    void SetState(bool newstate) { down = newstate; counter = 0; SetDirty(); }
        // Sets the button state
    bool GetState() { return down; }
        // Returns true if the button is down, false if not
    void Invert() { if (down) down = false; else down = true; SetDirty(); }
        // Invert button value
    void SetPosX(int32_t nx) { x = nx; }
    void SetPosY(int32_t ny) { y = ny; }
    void ButtonFunc() { if (buttonfunc) (*(buttonfunc))(); }
        // Call the button's function
    void SetLevel(int32_t lev) { if (lev != level) { level = lev; SetDirty(); } }
        // Set SV level
    void SetUpBitmap(PTBitmap ubm) { upbitmap = ubm; SetDirty(); }
        // Set the up bitmap
    void SetDownBitmap(PTBitmap dbm) { downbitmap = dbm; SetDirty(); }
        // Set the down bitmap

    virtual void Draw();
        // Draw the button to the screen
    void Animate(bool draw = true);
        // Animation pulse

  protected:
    char name[NAMELEN];                 // Button name
    int32_t x, y, w, h;                     // Button position
    bool radial;                        // Circular buttons
    bool toggle;                        // Toggle buttons
    bool pixelcheck;                    // Check pixels on click
    int32_t level;                          // SV level
    int32_t radiogroup;                     // Radio buttons, or -1 if not radio
    int32_t repeatrate;                     // Repeats like a keypress, or 0 for none
    int32_t counter;                        // For repeating
    PTBitmap upbitmap;                  // Button up bitmap
    PTBitmap downbitmap;                // Button down bitmap
    uint16_t key;                           // Hotkey for button
    bool down;                          // Is button down
    bool dirty;                         // Needs to be redrawn
    bool hidden;                        // Whether button is visible or not
    void (*buttonfunc)();               // Function associated with button
};

class TButtonPane : public TPane
{
  public:
    TButtonPane(int32_t px, int32_t py, int32_t pw, int32_t ph) : TPane(px, py, pw, ph) {}
      // Create pane

    virtual bool Initialize();
    virtual void Close();
    virtual void KeyPress(int32_t key, bool down);
    virtual void MouseClick(int32_t button, int32_t x, int32_t y);
    virtual void MouseMove(int32_t button, int32_t x, int32_t y);
    virtual void DrawBackground();
    virtual void Animate(bool draw = true);

    void RedrawButtons();
        // Redraw all the buttons
    virtual void Update() { TPane::Update(); RedrawButtons(); }

    void ClearGroup(int32_t group);
        // Clear a radio group
    void CheckGroup(int32_t group);
        // Called on init to set the first button of a radio group down

    bool NewButton(PTButton b);
        // Add a new button to the list
    bool NewButton(char *bname, int32_t bx, int32_t by, int32_t bw, int32_t bh, uint16_t key, void (*bfunc)(),
                        PTBitmap dbm = nullptr, PTBitmap ubm = nullptr, bool radial = false,
                        bool tog = false, bool notsquare = false, int32_t radiogroup = -1, int32_t repeat = 0)
    { return NewButton(new TButton(bname, bx, by, bw, bh, key, bfunc, dbm, ubm, radial, tog, notsquare, radiogroup, repeat)); }
        // Shortcut for normal buttons

    PTButton Button(int32_t b) { return Buttons[b]; }

  protected:
    TPointerArray<TButton, MAXBUTTONS> Buttons;
    int32_t clicked;                        // Index to last clicked button
};

#endif