// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *            animdata.h - EXILE Animation Definition File               *
// *************************************************************************

#pragma once

#include "revenant.h"

_STRUCTDEF(SAnimationFrame)
_CLASSDEF(TAnimationData)

struct SAnimationFrame
{
    int32_t      dx;                // Delta Offsets to next frame
    int32_t      dy;
    int32_t      regx;              // Bitmap registration offset from path point
    int32_t      regy;
    int32_t      x;                 // Position of frame in decompression buffer
    int32_t      y;
    int32_t      width;
    int32_t      height;
    uint32_t    decbufsize;        // Size of Decompression buffer.
    OTBitmap bitmap;            // Long offset to compressed bitmap data
};

class TAnimationData
{
  public:
    int32_t             flags;      // Animation flags
    int32_t             maxwidth;   // Animation frames max width and height
    int32_t             maxheight;  
    int32_t             numframes;  // Number of frames in the animation
    uint32_t           drawmode;   // Default drawmode for animation
    SAnimationFrame frames[1];  // Long offset to animation data record
};

