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
    TButton(const char *bname, int32_t bx, int32_t by, int32_t bw, int32_t bh, uint16_t keypr,
            void (*bfunc)(), PTBitmap dbm = nullptr, PTBitmap ubm = nullptr,
            bool rad = false, bool tog = false, bool notsquare = false, int32_t group = -1, int32_t repeat = 0);
    virtual ~TButton() = default;

    const char *GetName() const { return name; }
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
    void Hide() { if (!hidden) { hidden = true; hover = false; SetDirty(); } }
    void Show() { if (hidden) { hidden = false; SetDirty(); } }
    void SetState(bool newstate) { down = newstate; counter = 0; SetDirty(); }
        // Sets the button state
    bool GetState() { return down; }
        // Returns true if the button is down, false if not
    void Invert() { if (down) down = false; else down = true; SetDirty(); }
        // Invert button value
    void SetPosX(int32_t nx) { x = nx; }
    void SetPosY(int32_t ny) { y = ny; }
    void SetRect(int32_t nx, int32_t ny, int32_t nw, int32_t nh) { x = nx; y = ny; w = nw; h = nh; }
        // Moves and sizes the button (retail vtable 0x28)
    void ButtonFunc() { if (buttonfunc) (*(buttonfunc))(); }
        // Call the button's function
    void SetLevel(int32_t lev) { if (lev != level) { level = lev; SetDirty(); } }
        // Set SV level
    void SetUpBitmap(PTBitmap ubm) { upbitmap = ubm; SetDirty(); }
        // Set the up bitmap
    void SetDownBitmap(PTBitmap dbm) { downbitmap = dbm; SetDirty(); }
        // Set the down bitmap

  // Hover (retail button flags 0x10 "can hover" and 8 "hovered"). The pane
  // sets it (TButtonPane::SetHover); only a visible, hoverable button takes it.
    void SetHoverable(bool on) { hoverable = on; if (!on) hover = false; }
    [[nodiscard]] bool IsHoverable() const { return hoverable; }
    [[nodiscard]] bool IsHover() const { return hover; }
    void SetHover(bool on) { hover = on && hoverable && !hidden; SetDirty(); }

    virtual void Draw();
        // Draw the button to the screen
    void Animate(bool draw = true);
        // Animation pulse

  protected:
    char name[NAMELEN] = {};                // Button name
    int32_t x = 0, y = 0, w = 0, h = 0;     // Button position
    bool radial = false;                    // Circular buttons
    bool toggle = false;                    // Toggle buttons
    bool pixelcheck = false;                // Check pixels on click
    int32_t level = 0;                      // SV level
    int32_t radiogroup = -1;                // Radio buttons, or -1 if not radio
    int32_t repeatrate = 0;                 // Repeats like a keypress, or 0 for none
    int32_t counter = 0;                    // For repeating
    PTBitmap upbitmap = nullptr;            // Button up bitmap
    PTBitmap downbitmap = nullptr;          // Button down bitmap
    uint16_t key = 0;                       // Hotkey for button
    bool down = false;                      // Is button down
    bool dirty = true;                      // Needs to be redrawn
    bool hidden = false;                    // Whether button is visible or not
    bool hoverable = false;                 // Takes the hover (retail flag 0x10)
    bool hover = false;                     // The pane's hovered button (retail flag 8)
    void (*buttonfunc)() = nullptr;         // Function associated with button
};

class TButtonPane : public TPane
{
  public:
  // OnControl message: a button was pressed (retail 3000).
    static constexpr int32_t CONTROL_CLICKED = 3000;

  // Pane flags (retail TPane +0x60 bits the button pane reads).
    static constexpr uint32_t BPF_HOVER     = 0x2;  // track the button under the pointer
    static constexpr uint32_t BPF_KEEPHOVER = 0x4;  // keep it when the pointer leaves the buttons
    static constexpr uint32_t BPF_KEYFOCUS  = 0x8;  // arrows move the hover, Enter presses it

    TButtonPane(int32_t px, int32_t py, int32_t pw, int32_t ph) : TPane(px, py, pw, ph) {}
      // Create pane

    bool Initialize() override;
    void Close() override;
    void KeyPress(int32_t key, bool down) override;
    void MouseClick(int32_t button, int32_t x, int32_t y) override;
    void MouseMove(int32_t button, int32_t x, int32_t y) override;
    void DrawBackground() override;
    void Animate(bool draw = true) override;

    virtual void OnControl(TButton *button, int32_t msg) { (void)button; (void)msg; }
      // A button reports to its pane (retail vtable 0x94); CONTROL_CLICKED
      // when it is pressed, after its function runs.

    void RedrawButtons();
        // Redraw all the buttons
    void Update() override { TPane::Update(); RedrawButtons(); }

    void ClearGroup(int32_t group);
        // Clear a radio group
    void CheckGroup(int32_t group);
        // Called on init to set the first button of a radio group down

    bool NewButton(PTButton b);
        // Add a new button to the list
    bool NewButton(const char *bname, int32_t bx, int32_t by, int32_t bw, int32_t bh, uint16_t key, void (*bfunc)(),
                        PTBitmap dbm = nullptr, PTBitmap ubm = nullptr, bool radial = false,
                        bool tog = false, bool notsquare = false, int32_t radiogroup = -1, int32_t repeat = 0)
    { return NewButton(new TButton(bname, bx, by, bw, bh, key, bfunc, dbm, ubm, radial, tog, notsquare, radiogroup, repeat)); }
        // Shortcut for normal buttons
    bool DeleteButton(TButton *b);
        // Remove the button from the list and delete it (retail 0x004367d0)

    PTButton Button(int32_t b) { return Buttons[b]; }

    void SetHover(TButton *b);
        // Make `b` the hovered button, or none (retail 0x004369f0)
    [[nodiscard]] TButton *HoverButton() const { return hover; }
    void SetPaneFlags(uint32_t flags) { paneflags = flags; }
    [[nodiscard]] uint32_t PaneFlags() const { return paneflags; }

  protected:
    void Activate(TButton *b);
        // The button was pressed: click sound, its function, OnControl

    TPointerArray<TButton, MAXBUTTONS> Buttons;
    int32_t clicked = -1;                   // Index to last clicked button
    TButton *hover = nullptr;               // Hovered button (retail +0x9c)
    uint32_t paneflags = 0;                 // BPF_*
};

#endif
