// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      textbar.h - Text Bar Pane                        *
// *************************************************************************

#pragma once

#include "revenant.h"
#include "screen.h"

// This is the lame little text bar that goes just under the map
// map and tells the player completely useless information like,
// "The door is now open" or "Got the Hammer of Wounding" in an
// attempt to make them feel like complete idiots.
// It also now does something kind of useful - displaying the name
// and health of the creature Locke is fighting when there is nothing
// else to print.

_CLASSDEF(TTextBar)
class TTextBar : public TPane
{
  public:
    TTextBar() : TPane(TEXTBARX, TEXTBARY, TEXTBARWIDTH, TEXTBARHEIGHT) { }

    virtual bool Initialize();
    virtual void Close();

    virtual void DrawBackground();

    void Print(char *txt, ...); // printf style output!!!
    void Clear() { text[0] = 0; SetDirty(true); }

    // Opponent health display functions
    void SetHealthDisplay(char *n, int32_t l);
    void ClearHealthDisplay();

    // Sets the target and current levels
    void SetLevels(int32_t newlevel, int32_t newtargetlevel);

  protected:
    char text[80];                  // text to display

    char name[80];                  // name of monster's health to display
    int32_t level;                  // current health level
    int32_t targetlevel;            // target health level
    bool animating;                 // whether last frame was drawn to background
    bool pulsecheck;                // must be set every tick to keep health bar up
};

