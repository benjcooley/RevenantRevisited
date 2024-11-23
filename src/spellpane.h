// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      SpellPane.h - Spell Pane                         *
// *************************************************************************

#pragma once

#include "revenant.h"
#include "screen.h"
#include "object.h"
#include "button.h"
#include "spell.h"

extern char *Talismans[];
extern char *Old[];

// *******************
// * TTalismanButton *
// *******************

_CLASSDEF(TTalismanButton)
class TTalismanButton : public TButton
{
  public:
    TTalismanButton(char *bname, int32_t bx, int32_t by, int32_t bw, int32_t bh, uint16_t keypr,
            void (*bfunc)(), PTBitmap dbm = nullptr, PTBitmap ubm = nullptr,
            bool rad = false, bool tog = false, bool notsquare = false,
            int32_t qspellid = -1, int32_t xoff = 0) :
        TButton(bname, bx, by, bw, bh, keypr, bfunc, dbm, ubm, rad, tog, notsquare, -1, 0)
            { quickspellid = qspellid; xoffset = xoff; Clear(); }

    virtual void Draw();
        // Draw the button to the screen

    void AddTalisman(char t);
        // Add a talisman to the button
    void Backspace();
        // Backspace a single talsiman
    void Clear();
        // Clear all talismans
    void Invoke();
        // Invoke the spell on the button
    char *GetSpell();
        // get the talismans for this button
    void SetSpell(char *talismans);
        // Get the talismans for this button
    bool HasTalismans();
        // Checks to see if player has talismans for this spell

  protected:
    int32_t quickspellid;               // Player quickspell for this button (-1 is no player spell)
    int32_t xoffset;                    // Offset 
};

// **************
// * TSpellPane *
// **************

// This pane is where the user assembles spells with various talismans and invokes them.

_CLASSDEF(TSpellPane)
class TSpellPane : public TButtonPane
{
  public:
    TSpellPane() : TButtonPane(MULTIPANEX, MULTIPANEY, MULTIPANEWIDTH, MULTIPANEHEIGHT) {}
    ~TSpellPane() {}

    virtual bool Initialize();
    virtual void DrawBackground();
    virtual void MouseClick(int32_t button, int32_t x, int32_t y);
    virtual void MouseMove(int32_t button, int32_t x, int32_t y);

    void Scroll(int32_t numlines);
        // Scroll the pane's contents

    void ToggleTalismanNames();
        // Change between showing the english name and just the symbol
    bool ShowTalismanNames() { return shownames; }
        // Whether to show the name of the talisman next to its icon

    bool AddTal(int32_t tal);
        // Add a talisman to the current spell
    bool RemoveTal(int32_t numtals = 1);
        // Backspace numtals of talismans

    void Invoke();
        // Invoke the spell in the button

    char *GetSpell();
        // get the spell info

  private:
    int32_t OnTal(int32_t x, int32_t y);
        // Which talisman mouse is on

    bool shownames;                     // expand names
    int32_t startline;                      // for scrolling

    int32_t clickedtal;                     // which talisman is currently clicked
    bool onclickedtal;                  // whether mouse arrow is still on the clicked talisman
};

// *******************
// * TQuickSpellPane *
// *******************

#define NUMBUTTONS      4

// A bunch of buttons above the inventory giving the player quicker access to
// their spells.

_CLASSDEF(TQuickSpellPane)
class TQuickSpellPane : public TButtonPane
{
  public:
    TQuickSpellPane() : TButtonPane(QUICKSPELLX, QUICKSPELLY, QUICKSPELLWIDTH, QUICKSPELLHEIGHT) {}
    ~TQuickSpellPane() {}

    virtual bool Initialize();
    virtual void DrawBackground();
    virtual void MouseClick(int32_t button, int32_t x, int32_t y);

    void Invoke(int32_t button);
        // Invoke the spell on the given button
    void Clear(int32_t button);
    void Backspace(int32_t button);
    void AddTalisman(int32_t button, char tal);
    void Set(int32_t button);
};
