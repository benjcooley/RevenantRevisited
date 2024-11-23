// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 statusbar.h - Status bars (health, stamina)           *
// *************************************************************************

#pragma once

#include "revenant.h"
#include "multi.h"
#include "screen.h"

_CLASSDEF(TStatusBar)
class TStatusBar : public TPane
{
  public:
    TStatusBar(int32_t x, int32_t y, int32_t w, int32_t h) : TPane(x, y, w, h) { }

    virtual bool Initialize();
    virtual void Close();

    virtual void DrawBackground();

    virtual int32_t GetHue() { return 0; }
        // Redefine this for any bar to return the color at the current level

    int32_t GetLevel() { return level; }
    void SetLevel(int32_t lev) { level = targetlevel = max(min(lev, 1000), 0); }
    void ChangeLevel(int32_t tlev) { targetlevel = max(min(tlev, 1000), 0); }

  protected:
    PTMulti tubedata;

    int32_t level;              // level of fluid (0-1000)
    int32_t targetlevel;        // target level (animating towards)
    bool animating;         // whether it was drawn to the background last frame
};

_CLASSDEF(THealthBar)
class THealthBar : public TStatusBar
{
  public:
    THealthBar() : TStatusBar(HEALTHBARX, HEALTHBARY, HEALTHBARWIDTH, HEALTHBARHEIGHT) { }

    virtual bool Initialize();
    virtual int32_t GetHue();
};

_CLASSDEF(TStaminaBar)
class TStaminaBar : public TStatusBar
{
  public:
    TStaminaBar() : TStatusBar(STAMINABARX, STAMINABARY, STAMINABARWIDTH, STAMINABARHEIGHT) { }

    virtual bool Initialize();
    virtual int32_t GetHue();
};
