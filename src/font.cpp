// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 font.cpp - EXILE Font Objects File                    *
// *************************************************************************

#include "font.h"

#include "bitmap.h"
#include "graphics.h"

PTBitmap TFont::GetChar(unsigned char ch)
{
    if (ch < firstchar || ch > (firstchar + numchars))
        return nullptr;

    PTBitmap character = (PTBitmap)chars[ch - firstchar].ptr();
    return character;
}

int32_t TFont::FindNumLinesInText(char *text, int32_t wrapwidth, int32_t justify)
{
    SDrawBlock db;
    SDrawParam dp;
    STextParam tp;
    dp.func = TextDraw;
    dp.dx = dp.dy = 0;
    dp.data = (void *)&tp;

    tp.text = text;
    tp.numlines = 100000;
    tp.startline = 0;
    tp.wrapwidth = wrapwidth;
    tp.font = this;
    tp.justify = justify;
    tp.draw = false;
    tp.length = 0;

    TextDraw(&db, &dp);
    return tp.length;
}
