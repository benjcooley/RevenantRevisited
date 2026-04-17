// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *              multi.h - TMulti multiple resource class                 *
// *************************************************************************

#pragma once

#include "revenant.h"

// ******************************************
// * TMulti - Multiple resource array class *
// ******************************************

// The TMulti class is basically a big buffer of resources accessed by an
// offset array.

#include "revenant.h"
#include "resource.h"
#include "multidat.h"

_CLASSDEF(TMulti)
class TMulti : public TMultiData
{
  public:

  // Functions
    static TMulti* LoadMulti(char *name)
      { return (TMulti*)LoadResource(name); }
    static TMulti* LoadMulti(char *name, int32_t id)
      { return (TMulti*)LoadResource(name, id); }
    void operator delete(void *p)
      { free(p); }

    void *Object(int32_t i)
      { return offsets[i]; }
    void *Object(char *name);

    PTAnimation Animation(int32_t i)
      { return (PTAnimation)(void *)offsets[i]; }
    PTBitmap Bitmap(int32_t i)
      { return (PTBitmap)(void *)offsets[i]; }
    TFont* Font(int32_t i)
      { return (TFont*)(void *)offsets[i]; }

    PTAnimation Animation(char *name)
      { return (PTAnimation)Object(name); }
    PTBitmap Bitmap(char *name)
      { return (PTBitmap)Object(name); }
    TFont* Font(char *name)
      { return (TFont*)Object(name); }
    PTWaveData Wave(char *name)
      { return (PTWaveData)Object(name); }
};
