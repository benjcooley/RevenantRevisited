// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     scroll.cpp - TScroll module                       *
// *************************************************************************

#include "game/app/revenant.h"
#include "game/object/scroll.h"
#include "engine/render/display.h"
#include "engine/assets/bitmap.h"
#include "engine/assets/font.h"
#include "engine/assets/fonttable.h"
#include "game/ui/playscreen.h"

REGISTER_BUILDER(TScroll)
TObjectClass ScrollClass("SCROLL", OBJCLASS_SCROLL, 0);

// Hard coded class stats
DEFSTAT(Scroll, Value, VAL, 0, 0, 0, 1000000)

TScrollPane ScrollPane;
TBookPane BookPane;

void TScroll::SetText(char *newtext)
{
    if (text)
        delete text;

    text = new char[strlen(newtext)+1];
    if (text)
        strcpy(text, newtext);
}

bool TScroll::Use(TObjectInstance* user, int32_t with)
{
    if (with < 0)
    {
        PTScrollPane pane = &ScrollPane;

        if (!stricmp(GetTypeName(), "Book"))
            pane = (PTScrollPane)&BookPane;

        PlayScreen.SetNextPane(pane);
        pane->SetScroll(this);
    }

    return false;
}

// ***************
// * TScrollPane *
// ***************

#define NUMLINES        11      // depends on font and scroll height
#define SCROLLWIDTH     430     // depends on font and scroll width

void ScrollBtnScrollUp()
{
    ScrollPane.Scroll(-NUMLINES);
}

void ScrollBtnScrollDown()
{
    ScrollPane.Scroll(NUMLINES);
}

void ScrollBtnExit()
{
    ScrollPane.Close();
}

bool TScrollPane::Initialize()
{
    TButtonPane::Initialize();

    line = 0;
    numlines = 0;

    scrolldata = TMulti::LoadMulti("scroll.dat");
    scrollfont = FontTable->Bitmap("Scroll");

    SetScroll(scroll);      // recompute number of lines now that font is loaded up

    if (scrolldata)
    {
        PTBitmap bitmap = scrolldata->Bitmap("scroll");
        int32_t x = (WIDTH - bitmap->width) / 2;
        int32_t y = (HEIGHT - bitmap->height) / 4;

        NewButton("down", x+452, y+330, 28, 31, VK_NEXT, ScrollBtnScrollDown, scrolldata->Bitmap("downdown"), scrolldata->Bitmap("downup"));
        NewButton("up", x+484, y+323, 28, 31, VK_PRIOR, ScrollBtnScrollUp, scrolldata->Bitmap("updown"), scrolldata->Bitmap("upup"));
        NewButton("exit", x+518, y+321, 28, 31, VK_ESCAPE, ScrollBtnExit, scrolldata->Bitmap("exitdown"), scrolldata->Bitmap("exitup"));

        PlayScreen.AddPane(this);
        PlayScreen.SetExclusivePane(this, true);
        return true;
    }

    return false;
}

void TScrollPane::Close()
{
    TButtonPane::Close();

    if (scrolldata)
        delete scrolldata;

    PlayScreen.ReleaseExclusivePane(this);
    PlayScreen.RemovePane(this);
    PlayScreen.Redraw();

    scroll = nullptr;
}

void TScrollPane::DrawBackground()
{
    if (IsDirty())
    {
        PTBitmap bitmap = scrolldata->Bitmap("scroll");

        int32_t x = (WIDTH - bitmap->width) / 2;
        int32_t y = (HEIGHT - bitmap->height) / 4;
        Display.Put(x, y, bitmap, DM_TRANSPARENT | DM_BACKGROUND);

        if (scroll && scroll->GetText())
            Display.WriteText(scroll->GetText(), x+65, y+68, NUMLINES, scrollfont, nullptr, DM_USEDEFAULT, SCROLLWIDTH, line);

        // decide which buttons should be visisble
        if (line < 1)
            Button(1)->Hide();
        else
            Button(1)->Show();

        if ((line + NUMLINES - 1) >= numlines)
            Button(0)->Hide();
        else
            Button(0)->Show();

        Button(2)->Show();

        RedrawButtons();
        SetDirty(false);
    }

    // buttons shouldn't draw until after the rest is placed down
    TButtonPane::DrawBackground();
}

void TScrollPane::KeyPress(int32_t key, bool down)
{
    TButtonPane::KeyPress(key, down);

    if (down)
    {
        switch (key)
        {
            case VK_UP:
                Scroll(-1);
                break;
            case VK_DOWN:
                Scroll(1);
                break;
            case VK_HOME:
                Scroll(-100000);
                break;
            case VK_END:
                Scroll(100000);
                break;
        }
    }
}

void TScrollPane::SetScroll(PTScroll s)
{
    scroll = s;

    if (scroll && scroll->GetText() && scrollfont)
        numlines = scrollfont->FindNumLinesInText(scroll->GetText(), SCROLLWIDTH);
    else
        numlines = 0;
}

void TScrollPane::Scroll(int32_t numscrolllines)
{
    int32_t oldline = line;

    line += numscrolllines;

    if (line < 0)
        line = 0;
    else if (line > numlines)
        line = numlines;

    if (oldline != line)
        SetDirty(true);
}

// *************
// * TBookPane *
// *************

#define BOOKLINES       16      // depends on font and scroll height
#define BOOKWIDTH       216     // depends on font and scroll width

void BookBtnScrollUp()
{
    BookPane.Scroll(-(BOOKLINES * 2));
}

void BookBtnScrollDown()
{
    BookPane.Scroll(BOOKLINES * 2);
}

void BookBtnExit()
{
    BookPane.Close();
}

bool TBookPane::Initialize()
{
    TButtonPane::Initialize();

    line = 0;
    numlines = 0;

    scrolldata = TMulti::LoadMulti("book.dat");
    scrollfont = FontTable->Bitmap("Book");

    SetScroll(scroll);          // recompute number of lines now that font is loaded up

    if (scrolldata)
    {
        PTBitmap bitmap = scrolldata->Bitmap("book");
        int32_t x = (WIDTH - bitmap->width) / 2;
        int32_t y = (HEIGHT - bitmap->height) / 3;

        NewButton("down", x+440, y+386, 28, 22, VK_NEXT, BookBtnScrollDown, scrolldata->Bitmap("downdown"), scrolldata->Bitmap("downup"));
        NewButton("up", x+472, y+379, 28, 23, VK_PRIOR, BookBtnScrollUp, scrolldata->Bitmap("updown"), scrolldata->Bitmap("upup"));
        NewButton("exit", x+505, y+377, 28, 30, VK_ESCAPE, BookBtnExit, scrolldata->Bitmap("exitdown"), scrolldata->Bitmap("exitup"));

        PlayScreen.AddPane(this);
        PlayScreen.SetExclusivePane(this, true);
        return true;
    }

    return false;
}

void TBookPane::DrawBackground()
{
    if (IsDirty())
    {
        PTBitmap bitmap = scrolldata->Bitmap("book");

        int32_t x = (WIDTH - bitmap->width) / 2;
        int32_t y = (HEIGHT - bitmap->height) / 3;
        Display.Put(x, y, bitmap, DM_TRANSPARENT | DM_BACKGROUND);

        if (scroll && scroll->GetText())
        {
            char buf[80];
            for (int32_t side = 0; side < 2; side++)
            {
                sprintf(buf, "%d", (line / BOOKLINES) + side + 1);
                Display.WriteText(buf, x+180+(side*232), y+21+(side*2), 1, scrollfont, nullptr, DM_USEDEFAULT, -1, 0, JUSTIFY_CENTER);

                Display.WriteText(scroll->GetText(), x+82+(side*232), y+40+(side*2), BOOKLINES, scrollfont, nullptr, DM_USEDEFAULT, BOOKWIDTH, line + (side*BOOKLINES));
            }
        }

        // decide which buttons should be visisble
        if (line < 1)
            Button(1)->Hide();
        else
            Button(1)->Show();

        if ((line + (BOOKLINES*2) - 1) >= numlines)
            Button(0)->Hide();
        else
            Button(0)->Show();

        Button(2)->Show();

        RedrawButtons();
        SetDirty(false);
    }

    // buttons shouldn't draw until after the rest is placed down
    TButtonPane::DrawBackground();
}

