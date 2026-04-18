// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 font.h - EXILE Font Definition File                   *
// *************************************************************************

#pragma once

#include "revenant.h"

#include "bitmap.h"
#include "fontdata.h"
#include "resource.h"

#define FONT_DRAWMODE   (DM_TRANSPARENT | DM_ALIAS | DM_BACKGROUND)

_CLASSDEF(TFont)

class TFont : public TFontData
{
  public:
    TFont() {}
    PTBitmap GetChar(unsigned char ch);
    int32_t FirstChar() const
        { return firstchar; }
    int32_t Numchars() const
        { return numchars; }
    int32_t StartHeight(unsigned char ch) const
        { return (int32_t) startheight[ch - firstchar]; }
    int32_t DrawRight(unsigned char ch) const
        { return (int32_t) drawright[ch - firstchar]; }
    int32_t DrawLeft(unsigned char ch) const
        { return (int32_t) drawleft[ch - firstchar]; }

    int32_t FindNumLinesInText(char *text, int32_t wrapwidth = -1, int32_t justify = JUSTIFY_LEFT);
        // Find the number of lines in the given text and wrapwidth - this function
        // resides in the font because it is highly variable depending on the overlap
        // and size of each character in the font.
};

// GPU atlas for a TFont: every glyph baked into one RGBA8 sg_image with a
// per-glyph UV rect. Keycolor pixels become alpha=0 so straight-alpha blend
// composites correctly. Stored in a parallel cache keyed by TFont* rather
// than on the TFont object itself, because TFont instances are raw
// resource-file memory (malloc'd to exact on-disk size by LoadResource) —
// appending member fields would corrupt the bitmap data that follows.
struct SFontAtlasRect
{
    uint16_t x, y, w, h;          // pixel rect inside the atlas image
    float xoff = 0, yoff = 0;     // top-left offset from pen (TTF only)
    float xoff2 = 0, yoff2 = 0;   // bottom-right offset from pen (TTF only)
    float xadvance = 0;           // pen advance after glyph (TTF only)
};

struct SFontAtlas
{
    sg_image       image = {};
    int32_t        width = 0;
    int32_t        height = 0;
    int16_t        firstchar = 0;
    int16_t        numchars  = 0;
    SFontAtlasRect rects[MAXFONTCHARS] = {};

    const SFontAtlasRect& Rect(unsigned char ch) const
        { return rects[ch - firstchar]; }
};

const SFontAtlas* BuildFontAtlas(TFont* font);
  // Returns the cached atlas for `font`, building it on first call.
  // Null if the font is null or has no glyphs.
const SFontAtlas* FindFontAtlas(TFont* font);
  // Cache lookup only; returns null if never built.
void DestroyAllFontAtlases();
  // Releases every cached atlas image. Call during shutdown.

// stb_truetype-backed atlas for WINFONT entries. Rasterizes ASCII 32..127
// at `pixel_height` and packs via stbtt_PackFontRange. Uses a separate
// cache (keyed by path+size) from BuildFontAtlas's TFont* cache because
// TTF fonts have no TFont resource to key against.
const SFontAtlas* BuildTTFAtlas(const char* path, int pixel_height);
void DestroyAllTTFAtlases();
