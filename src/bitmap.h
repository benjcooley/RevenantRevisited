// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      bitmap.h - Bitmap objects                        *
// *************************************************************************

#pragma once

#include <memory.h>

#include "revenant.h"
#include "bitmapdata.h"
#include "surface.h"

// ***************************************
// * TBitmap Class Varible/Function List *
// ***************************************

_CLASSDEF(TBitmap)

class TBitmap : public TBitmapData
{
  public:
    static PTBitmap NewBitmap(int32_t width, int32_t height, int32_t flags, int32_t aliasbufsize = 0);
      // Creates bitmap 
    static PTBitmap Load(int32_t resource);
      // Loads bitmap

    bool RawPut(int32_t x, int32_t y, 
          PTBitmap bitmap, int32_t srcx, int32_t srcy, int32_t srcw, int32_t srch, int32_t drawmode,
          uint32_t color, int32_t intensity, uint16_t zpos, DRAWFUNCTION func, void *data);

    bool Put(int32_t x, int32_t y, PTBitmap bitmap, int32_t srcx, int32_t srcy, int32_t srcw, int32_t srch, 
             int32_t drawmode = 0, int32_t intensity = 31)
       { return RawPut(x, y, bitmap, 
            srcx, srcy, srcw, srch, drawmode, 0, intensity, 0, nullptr, nullptr); }
      // Draws a Rectangle of bitmap to bitmap.    
    bool Put(int32_t x, int32_t y, PTBitmap bitmap, int32_t drawmode = DM_USEDEFAULT, 
             int32_t intensity = 31)
       { return RawPut(x, y, bitmap, 
            0, 0, bitmap->width, bitmap->height, drawmode, 0, intensity, 0, nullptr, nullptr); }

      // Z Draws bitmap to bitmap
    bool ZPut(int32_t x, int32_t y, int32_t z, PTBitmap bitmap, int32_t srcx, int32_t srcy, int32_t srcw, int32_t srch, 
             int32_t drawmode  = 0)
       { return RawPut(x, y, bitmap, 
            srcx, srcy, srcw, srch, drawmode, 0, 0, (uint16_t)z, nullptr, nullptr); }
      // Z Draws a Rectangle of bitmap to bitmap.    
    bool ZPut(int32_t x, int32_t y, int32_t z, PTBitmap bitmap, int32_t drawmode = DM_USEDEFAULT)
       { return RawPut(x, y, bitmap, 
            0, 0, bitmap->width, bitmap->height, drawmode, 0, 0, (uint16_t)z, nullptr, nullptr); }
      // Draws bitmap to bitmap
    
    bool Put(int32_t x, int32_t y, PTSurface surface, int32_t srcx, int32_t srcy, int32_t srcw, int32_t srch, 
             int32_t drawmode  = nullptr, int32_t intensity = 31);
      // Draws a Rectangle of a surface to bitmap.    
    bool Put(int32_t x, int32_t y, PTSurface surface, int32_t drawmode = DM_USEDEFAULT, 
             int32_t intensity = 31)
       { return Put(x, y, surface, 0, 0, surface->Width(), surface->Height(), drawmode, 
            intensity); }
      // Draws surface to bitmap
    
    bool StretchPut(int32_t x, int32_t y, int32_t w, int32_t h, PTBitmap bitmap, int32_t drawmode);
    bool StretchPut(int32_t x, int32_t y, int32_t w, int32_t h, PTBitmap bitmap, int32_t srcx,
                    int32_t srcy, int32_t srcw, int32_t srch, int32_t drawmode);
      // Stretch versions of Normal Puts
    
    void WriteText(char *text, int32_t x = 0, int32_t y = 0, int32_t lines = 1, PTFont font = SystemFont, PSColor color = nullptr, uint32_t drawmode = DM_USEDEFAULT);
      // Draws text to the bitmap

    bool Line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, SColor &color);
    bool Rect(int32_t x, int32_t y, int32_t w, int32_t h, SColor &color);
    bool Box(int32_t x, int32_t y, int32_t w, int32_t h, SColor &color);
      // Draws a filled Rect.

  // Compression functions
    bool CacheChunks();
      // If this is a compressed bitmap, causes chunks to be decompressed into chunk cache
      // Returns true if chunks were decompressed to cache

  // Miscellaneous bitmap buffer functions
    bool Clear(SColor &color, uint32_t drawmode = DM_USEDEFAULT, uint16_t zpos = 0xffff);
      // Clears bitmap to keycolor.                 
    bool SaveBMP(char *filename);
      // Saves BMP of source bitmap.
    bool SaveZBF(char *filename);
      // Saves zbuffer file
    bool OnPixel(int32_t x, int32_t y);
      // Check to see if target pixel is not keycolor

    bool operator= (PTBitmap bitmap)
        { memcpy(bitmap, this, sizeof(this)); return true; }
      // Allows use of '=' to copy values from one variable to another
};

uint16_t TranslateColor(SColor &color);
    // Translate 24 Bit Color into 15/16 bit uint16_t value
