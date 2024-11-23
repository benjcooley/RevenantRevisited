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
    static PTFont LoadFont(int32_t id)
        { return (PTFont)LoadResource("FONT", id); }
    PTBitmap GetChar(unsigned char ch);
    int32_t FirstChar()
        { return firstchar; }
    int32_t Numchars()
        { return numchars; }
    int32_t StartHeight(unsigned char ch)
        { return (int32_t) startheight[ch - firstchar]; }
    int32_t DrawRight(unsigned char ch)
        { return (int32_t) drawright[ch - firstchar]; }
    int32_t DrawLeft(unsigned char ch)
        { return (int32_t) drawleft[ch - firstchar]; }

    int32_t FindNumLinesInText(char *text, int32_t wrapwidth = -1, int32_t justify = JUSTIFY_LEFT);
        // Find the number of lines in the given text and wrapwidth - this function
        // resides in the font because it is highly variable depending on the overlap
        // and size of each character in the font.
};

#endif
