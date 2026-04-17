// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      scroll.h - TScroll module                        *
// *************************************************************************

#pragma once

#include "revenant.h"
#include "multi.h"
#include "object.h"
#include "button.h"
#include "font.h"

_CLASSDEF(TScroll)
class TScroll final : public TObjectInstance
{
  public:
    TScroll(TObjectImagery* newim) : TObjectInstance(newim) { text = nullptr; }
    TScroll(SObjectDef* def, TObjectImagery* newim) : TObjectInstance(def, newim) { text = nullptr; }

    _NODEFAULTCONS(TScroll);

    char *GetText() { return text; }
    void SetText(char *newtext);

    virtual bool Use(TObjectInstance* user, int32_t with = -1);
        // Read scroll
    virtual int32_t CursorType(TObjectInstance* inst = nullptr) { if (inst) return CURSOR_NONE; return CURSOR_EYE; }
        // Indicate you can read the scroll

    virtual void Load(RTInputStream is, int32_t version, int32_t objversion);
    virtual void Save(RTOutputStream os);

    // Scroll stats
    STATFUNC(Value)

  private:
    char *text;             // Text written on the scroll
};

DEFINE_BUILDER("SCROLL", TScroll)

// ***************
// * TScrollPane *
// ***************

_CLASSDEF(TScrollPane)
class TScrollPane : public TButtonPane
{
  public:
    TScrollPane() : TButtonPane(0, 0, WIDTH, HEIGHT) { scroll = nullptr; }
    
    _NODEFAULTCONS(TScrollPane);

    virtual bool Initialize();
    virtual void Close();
    virtual void DrawBackground();
    virtual void KeyPress(int32_t key, bool down);

    void SetScroll(PTScroll s);
        // Set the pane to read from the scroll object given

    void Scroll(int32_t numscrolllines);
        // Can be negative or positive to scroll up or down

  protected:
    PTScroll scroll;                // Pointer to the scroll being read

    TMulti* scrolldata;             // Bitmap of scroll and buttons
    TFont* scrollfont;              // Text font

    int32_t line;                       // For scrolling through the text
    int32_t numlines;                   // Number of total lines in the text
};

// *************
// * TBookPane *
// *************

_CLASSDEF(TBookPane)
class TBookPane final : public TScrollPane
{
  public:
    TBookPane() : TScrollPane() { }

    _NODEFAULTCONS(TBookPane);

    virtual bool Initialize();
    virtual void DrawBackground();
};
