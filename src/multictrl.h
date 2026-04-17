// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                multictrl.h - FORSAKEN Multipane Control Panel         *
// *************************************************************************

#pragma once

#include "revenant.h"

#include "button.h"
#include "multi.h"

// Defines for multi control panel
#define NUMMCBUTTONS    4
#define BUTTONRADIUS    16

// **************
// * TMultiCtrl *
// **************

// The pane which contains the buttons which flip through the various multifunction panels.

_CLASSDEF(TMultiCtrlPane)
class TMultiCtrlPane : public TButtonPane
{

  // Function Members

  public:

  // Constructor & Destructor
    TMultiCtrlPane() : TButtonPane(MULTICTRLPANEX, MULTICTRLPANEY, MULTICTRLPANEWIDTH, MULTICTRLPANEHEIGHT) {}
    ~TMultiCtrlPane() {}

    virtual bool Initialize();
    virtual void Close();

    // These hide and show all the multifunction panes
    virtual void Hide();
    virtual void Show();

    int32_t GetActivePane() { return curpane; }
        // Return the currently active pane
    void ActivatePane(int32_t pane);
        // Activate one of the multipanes and hide the others
    void RedrawCurPane();
        // Redraw current pane
    void RedrawOverhangButtons() { Button(0)->SetDirty(); Button(3)->SetDirty(); }

// Data Members
  private:
    int32_t curpane;                // pane currently showing
    TMulti* buttondata;         // bitmaps for buttons
};


