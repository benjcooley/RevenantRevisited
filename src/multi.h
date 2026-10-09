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
    static TMulti* LoadMulti(const char *name)
      { return (TMulti*)LoadResource(name); }
    static TMulti* LoadMulti(const char *name, int32_t id)
      { return (TMulti*)LoadResource(name, id); }
    void operator delete(void *p)
      { free(p); }

    void *Object(int32_t i)
      { return offsets[i]; }
    void *Object(const char *name);
      // The named entry; a missing name is fatal ("Unable to find ... in
      // multiresource"), as in retail.

    PTAnimation Animation(int32_t i)
      { return (PTAnimation)(void *)offsets[i]; }
    PTBitmap Bitmap(int32_t i)
      { return (PTBitmap)(void *)offsets[i]; }
    TFont* Font(int32_t i)
      { return (TFont*)(void *)offsets[i]; }

    PTAnimation Animation(const char *name)
      { return (PTAnimation)Object(name); }
    PTBitmap Bitmap(const char *name)
      { return (PTBitmap)Object(name); }
    TFont* Font(const char *name)
      { return (TFont*)Object(name); }
    PTWaveData Wave(const char *name)
      { return (PTWaveData)Object(name); }
};
