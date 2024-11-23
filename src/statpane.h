// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   statpane.h - FORSAKEN Stat Pane                     *
// *************************************************************************

#pragma once

#include "revenant.h"
#include "button.h"
#include "player.h"

// *************
// * TStatPane *
// *************

// The stat pane is a display of the main characters abilities/skills/stats.

_CLASSDEF(TStatPane)
class TStatPane : public TButtonPane
{
  public:
    TStatPane() : TButtonPane(MULTIPANEX, MULTIPANEY, MULTIPANEWIDTH, MULTIPANEHEIGHT) {}
    ~TStatPane() {}

    virtual bool Initialize();
    virtual void Close();
    virtual void DrawBackground();
    virtual void MouseClick(int32_t button, int32_t x, int32_t y);
    virtual void MouseMove(int32_t button, int32_t x, int32_t y);

    void Scroll(int32_t numlines);
    void ExpandAll() { SetAllExpanded(true); }
    void ContractAll() { SetAllExpanded(false); }

    PTButton ScrollUpButton() { return Button(2); }
    PTButton ScrollDownButton() { return Button(3); }

  protected:
    void SetAllExpanded(bool value);
    int32_t OnSlot(int32_t x, int32_t y);

    int32_t startline;                      // for scrolling
    bool expanded[NUM_SKILLS];          // whether each level is expanded
};


