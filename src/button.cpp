// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     button.cpp - TButton object                       *
// *************************************************************************

#include <ctype.h>

#include "revenant.h"
#include "display.h"
#include "button.h"
#include "font.h"

// ***********
// * TButton *
// ***********

void DrawFrame(TSurface* surface, int32_t x, int32_t y, int32_t w, int32_t h, bool down)
{
    int32_t topcolor = 216, bottomcolor = 40, leftcolor = 196, rightcolor = 70;
    int32_t centercolor = 128;
    SColor color;

    if (down)
    {
        int32_t tmp;
        tmp = topcolor; topcolor = bottomcolor; bottomcolor = tmp;
        tmp = leftcolor; leftcolor = rightcolor; rightcolor = tmp;
    }

    // center
    color.red = color.green = color.blue = centercolor;
    surface->Box(x + 1, y + 1, w - 2, h - 2, TranslateColor(color), 0, 0, DM_BACKGROUND);
    // top
    color.red = color.green = color.blue = topcolor;
    surface->Box(x, y, w, 1, TranslateColor(color), 0, 0, DM_BACKGROUND);
    // left
    color.red = color.green = color.blue = leftcolor;
    surface->Box(x, y + 1, 1, h - 1, TranslateColor(color), 0, 0, DM_BACKGROUND);
    // right
    color.red = color.green = color.blue = rightcolor;
    surface->Box(x + w - 1, y + 1, 1, h - 2, TranslateColor(color), 0, 0, DM_BACKGROUND);
    // bottom
    color.red = color.green = color.blue = bottomcolor;
    surface->Box(x + 1, y + h - 1, w - 1, 1, TranslateColor(color), 0, 0, DM_BACKGROUND);
}

TButton::TButton(char *bname, int32_t bx, int32_t by, int32_t bw, int32_t bh, uint16_t keypr,
                 void (*bfunc)(), PTBitmap dbm, PTBitmap ubm, bool rad,
                 bool tog, bool notsquare, int32_t group, int32_t repeat)
{
    strcpy(name, bname);
    x = bx;
    y = by;
    w = bw;
    h = bh;
    key = keypr;
    radial = rad;
    toggle = tog;
    buttonfunc = bfunc;
    down = false;
    dirty = true;
    hidden = false;
    pixelcheck = notsquare && (ubm != nullptr);
    level = 0;
    radiogroup = group;
    repeatrate = repeat;
    if (radiogroup >= 0)
        toggle = true;

    downbitmap = dbm;
    if (downbitmap && rad)
    {
        downbitmap->flags |= BM_REGPOINT;
        downbitmap->regx = w + 1;
        downbitmap->regy = w;
    }

    upbitmap = ubm;
    if (upbitmap && rad)
    {
        downbitmap->flags |= BM_REGPOINT;
        upbitmap->regx = w + 1;
        upbitmap->regy = w;
    }
}

void TButton::Draw()
{
    if (hidden)
        return;

    if (downbitmap)
    {
        if (pixelcheck)
            //Display->PutSV(x, y, down ? downbitmap : upbitmap, DM_USEREG | DM_BACKGROUND | DM_TRANSPARENT, level * 2, level);
            Display->Put(x, y, down ? downbitmap : upbitmap, DM_USEREG | DM_BACKGROUND | DM_TRANSPARENT);
        else
            Display->Put(x, y, down ? downbitmap : upbitmap, DM_USEREG | DM_BACKGROUND);
    }
    else
    {
        // generate a generic button
        int32_t add = down ? 1 : 0;
        int32_t nx = x + (w / 2) - ((strlen(name) * SystemFont->GetChar(SystemFont->FirstChar())->width) / 2);
        int32_t ny = y + (h / 2) - (SystemFont->height / 2) - 3;
        DrawFrame(Display, x, y, w, h, down);
        SColor color;
        color.red = color.blue = color.green = 40;
        Display->WriteText(name, nx + add, ny + add, 1, SystemFont, &color, DM_TRANSPARENT | DM_ALIAS | DM_BACKGROUND);
        Display->AddUpdateRect(x, y, w, h, UPDATE_RESTORE);
    }

    dirty = false;
}

#define REPEATWAIT      7

void TButton::Animate(bool draw)
{
    if (hidden)
        return;

    if (repeatrate > 0 && down)
        if (counter++ >= REPEATWAIT && (counter % (FRAMERATE / repeatrate)) == 0)
            ButtonFunc();
}

bool TButton::OnButton(int32_t bx, int32_t by)
{
    if (hidden || (radiogroup >= 0 && down))
        return false;

    if (radial)
        return ((sqr(absval(x - bx)) + sqr(absval(y - by))) <= sqr(w));
    else
    {
        bool insquare = (bx >= x && by >= y && bx <= (x + w) && by <= (y + h));
        if (!pixelcheck || !insquare)
            return insquare;

        return upbitmap->OnPixel(bx - x, by - y);
    }
}

inline bool TButton::IsKey(int32_t keypr, bool keydown)
{
    if (hidden || (radiogroup >= 0 && down))
        return false;

    if (Editor)
        return (keypr == key && (key < 'A' || key > 'Z' || CtrlDown || !keydown));
    else
        return (keypr == key && !CtrlDown);
}

// ***************
// * TButtonPane *
// ***************

bool TButtonPane::Initialize()
{
    TPane::Initialize();

    Buttons.Clear();
    SetDirty(true);
    clicked = -1;
    return true;
}

void TButtonPane::Close()
{
    TPointerIterator<TButton> i(&Buttons);
    
    while (i)
    {
        if (i.Item() == nullptr)
            i++;
        else
        {
            delete i.Item();
            Buttons.Remove(i);
        }
    }
}

void TButtonPane::DrawBackground()
{
    for (TPointerIterator<TButton> i(&Buttons); i; i++)
        if (i.Item() && i.Item()->IsDirty())
            i.Item()->Draw();
}

void TButtonPane::Animate(bool draw)
{
    for (TPointerIterator<TButton> i(&Buttons); i; i++)
        if (i.Item())
            i.Item()->Animate(draw);
}

void TButtonPane::RedrawButtons()
{
    for (TPointerIterator<TButton> i(&Buttons); i; i++)
        if (i.Item())
            i.Item()->SetDirty();
}

void TButtonPane::KeyPress(int32_t key, bool down)
{
    if (down)
    {
        for (TPointerIterator<TButton> i(&Buttons); i; i++)
            if (i.Item() && i.Item()->IsKey(key, down) &&
                (i.Item()->GetState() == false || i.ItemNum() != clicked) &&
                (i.Item()->RadioGroup() < 0 || i.Item()->GetState() == false))
            {
                if (i.Item()->IsToggle())
                {
                    ClearGroup(i.Item()->RadioGroup());
                    i.Item()->SetState(!(i.Item()->GetState()));
                    i.Item()->ButtonFunc();
                }
                else
                    i.Item()->SetState(true);

                if (i.Item()->Repeats())
                    i.Item()->ButtonFunc();
                break;
            }
    }
    else
    {
        for (TPointerIterator<TButton> i(&Buttons); i; i++)
            if (i.Item() && i.Item()->IsKey(key, down) && i.Item()->GetState() == true &&
                i.ItemNum() != clicked)
            {
                if (!i.Item()->IsToggle())
                    i.Item()->SetState(false);

                if (!i.Item()->Repeats())
                    i.Item()->ButtonFunc();
            }
    }
}

void TButtonPane::MouseClick(int32_t button, int32_t x, int32_t y)
{
    if (button == MB_LEFTDOWN)
    {
        for (TPointerIterator<TButton> i(&Buttons); i; i++)
            if (i.Item() && i.Item()->OnButton(x, y) &&
                (i.Item()->GetState() == false || i.Item()->IsToggle()) &&
                (i.Item()->RadioGroup() < 0 || i.Item()->GetState() == false))
            {
                if (i.Item()->IsToggle())
                {
                    ClearGroup(i.Item()->RadioGroup());
                    i.Item()->Invert();
                    i.Item()->ButtonFunc();
                }
                else
                {
                    i.Item()->SetState(true);
                    clicked = i.ItemNum();
                }

                if (i.Item()->Repeats())
                    i.Item()->ButtonFunc();
                break;
            }
    }
    else if (button == MB_LEFTUP)
    {
        if (clicked >= 0 && Buttons[clicked]->OnButton(x, y) &&
            !Buttons[clicked]->IsToggle())
        {
            if (!Buttons[clicked]->IsToggle())
                Buttons[clicked]->SetState(false);
            if (!Buttons[clicked]->Repeats())
                Buttons[clicked]->ButtonFunc();
        }
        clicked = -1;
    }
}

void TButtonPane::MouseMove(int32_t button, int32_t x, int32_t y)
{
    if (button == MB_LEFTDOWN && clicked >= 0)
    {
        if (Buttons[clicked]->OnButton(x, y))
        {
            if (Buttons[clicked]->GetState() == false &&
                !Buttons[clicked]->IsToggle())
                Buttons[clicked]->SetState(true);
        }
        else
        {
            if (Buttons[clicked]->GetState() == true ||
                !Buttons[clicked]->IsToggle())
                Buttons[clicked]->SetState(false);
        }
    }
}

bool TButtonPane::NewButton(PTButton b)
{
    if (b)
        return (Buttons.Add(b) >= 0);

    CheckGroup(b->RadioGroup());

    return false;
}

void TButtonPane::ClearGroup(int32_t group)
{
    if (group < 0)
        return;

    for (TPointerIterator<TButton> i(&Buttons); i; i++)
        if (i.Item()->RadioGroup() == group)
            i.Item()->SetState(false);
}

void TButtonPane::CheckGroup(int32_t group)
{
    if (group < 0)
        return;

    for (TPointerIterator<TButton> i(&Buttons); i; i++)
        if (i.Item()->RadioGroup() == group)
        {
            i.Item()->SetState(true);
            break;
        }
}

