// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 bitmapdata.h - Bitmap data objects                    *
// *************************************************************************

#pragma once

#include "revenant.h"
#include "graphics.h"

_STRUCTDEF(SPalette);

struct SPalette
{
    uint16_t colors[256];
    uint32_t rgbcolors[256];
};

#ifndef _WINGDI_
typedef struct tagRGBTRIPLE {
        uint8_t    rgbtBlue;
        uint8_t    rgbtGreen;
        uint8_t    rgbtRed;
} RGBTRIPLE;
#endif

// *******************************************
// * TBitmapData Class Varible/Function List *
// *******************************************

_CLASSDEF(TBitmapData)

class TBitmapData
{
  public:

    int32_t    width;     // Bitmap Width
    int32_t    height;    // Bitmap Height
    int32_t    regx;      // Registration point x
    int32_t    regy;      // Registration point y
    uint32_t  flags;     // Bitmap Flags. See exiledef.h for Bitmap flags.
    uint32_t  drawmode;  // Default drawing mode of bitmap. See exiledef.h
                      // for drawing mode flags.
    uint32_t  keycolor;  // Color to use as Transparent color.
    uint32_t  aliassize; // Size of Alias Buffer
    OFFSET alias;     // Relative Offset to alias data.
    uint32_t  alphasize;
    OFFSET alpha;   
    uint32_t  zbuffersize;
    OFFSET zbuffer;
    uint32_t  normalsize;
    OFFSET normal;
    uint32_t  palettesize;
    OFFSET palette;
    uint32_t  datasize;

    union            // Bitmap Data in 8/15/24/32 bit
    {
        uint8_t      data8[1];
        uint16_t      data16[1];
        RGBTRIPLE data24[1];
        uint32_t     data32[1];
    };

};

