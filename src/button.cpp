// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     button.cpp - TButton object                       *
// *************************************************************************

#include <ctype.h>
#include <cstdio>

#include "revenant.h"
#include "display.h"
#include "button.h"
#include "font.h"
#include "sound.h"

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

TButton::TButton(const char *bname, int32_t bx, int32_t by, int32_t bw, int32_t bh, uint16_t keypr,
                 void (*bfunc)(), PTBitmap dbm, PTBitmap ubm, bool rad,
                 bool tog, bool notsquare, int32_t group, int32_t repeat)
{
    std::snprintf(name, sizeof(name), "%s", bname ? bname : "");   // retail keeps 31 chars
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
        upbitmap->flags |= BM_REGPOINT;
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
            //Display.PutSV(x, y, down ? downbitmap : upbitmap, DM_USEREG | DM_BACKGROUND | DM_TRANSPARENT, level * 2, level);
            Display.Put(x, y, down ? downbitmap : upbitmap, DM_USEREG | DM_BACKGROUND | DM_TRANSPARENT);
        else
            Display.Put(x, y, down ? downbitmap : upbitmap, DM_USEREG | DM_BACKGROUND);
    }
    else
    {
        // generate a generic button
        int32_t add = down ? 1 : 0;
        int32_t nx = x + (w / 2) - ((strlen(name) * SystemFont->GetChar(SystemFont->FirstChar())->width) / 2);
        int32_t ny = y + (h / 2) - (SystemFont->height / 2) - 3;
        DrawFrame(&Display, x, y, w, h, down);
        SColor color;
        color.red = color.blue = color.green = 40;
        Display.WriteText(name, nx + add, ny + add, 1, SystemFont, &color, DM_TRANSPARENT | DM_ALIAS | DM_BACKGROUND);
        Display.AddUpdateRect(x, y, w, h, UPDATE_RESTORE);
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

// REVSYNC: 0x0042d0d0 (rectangular: x <= bx < x + w)
bool TButton::OnButton(int32_t bx, int32_t by)
{
    if (hidden || (radiogroup >= 0 && down))
        return false;

    if (radial)
        return ((sqr(absval(x - bx)) + sqr(absval(y - by))) <= sqr(w));
    else
    {
        bool insquare = (bx >= x && by >= y && bx < (x + w) && by < (y + h));
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
    hover = nullptr;
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
    clicked = -1;
    hover = nullptr;
    TPane::Close();
}

// REVSYNC: 0x004369f0. Retail plays "frontend_move" whenever the hover is
// asked to move to another button, before checking that the button took it.
void TButtonPane::SetHover(TButton *b)
{
    if (!b)
    {
        if (hover)
            hover->SetHover(false);
        hover = nullptr;
        return;
    }
    if (b == hover)
        return;
    PLAY("frontend_move");
    b->SetHover(true);
    if (!b->IsHover())
        return;
    if (hover)
        hover->SetHover(false);
    hover = b;
}

// The pressed button reports: retail TButton plays "click1" and sends its
// pane CONTROL_CLICKED (0x0042d4b0 release, 0x0042d390 key press).
void TButtonPane::Activate(TButton *b)
{
    PLAY("click1");
    b->ButtonFunc();
    OnControl(b, CONTROL_CLICKED);
}

// REVSYNC: 0x004367d0
bool TButtonPane::DeleteButton(TButton *b)
{
    for (TPointerIterator<TButton> i(&Buttons); i; i++)
    {
        if (i.Item() != b)
            continue;
        if (hover == b)
            hover = nullptr;
        if (clicked == i.ItemNum())
            clicked = -1;
        Buttons.Delete(i);
        return true;
    }
    return false;
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

// REVSYNC: 0x004361f0. With BPF_KEYFOCUS the arrows move the hover through
// the buttons (wrapping) and Enter presses the hovered button -- or the last
// one, when none is hovered -- on key down.
void TButtonPane::KeyPress(int32_t key, bool down)
{
    const int32_t count = Buttons.NumItems();
    if ((paneflags & BPF_KEYFOCUS) && count > 0)
    {
        int32_t at = count - 1;
        for (int32_t n = 0; n < count; ++n)
            if (hover && Buttons[n] == hover)
                at = n;
        if (TButton *focus = Buttons[at])
        {
            const bool back = key == VK_UP || key == VK_LEFT;
            if (down && (back || key == VK_DOWN || key == VK_RIGHT))
            {
                const int32_t step = back ? count - 1 : 1;
                for (int32_t n = (at + step) % count; n != at; n = (n + step) % count)
                {
                    if (!Buttons[n])
                        continue;
                    SetHover(Buttons[n]);
                    if (hover == Buttons[n])
                        break;
                }
            }
            else if (down && key == VK_RETURN)
                Activate(focus);
        }
    }

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
                    Activate(i.Item());
            }
    }
}

// REVSYNC: 0x00436530 (pane) / 0x0042d4b0 (button). A press on a button
// gives it the hover; the release over the same button activates it.
void TButtonPane::MouseClick(int32_t button, int32_t x, int32_t y)
{
    if (button == MB_LEFTDOWN)
    {
        for (TPointerIterator<TButton> i(&Buttons); i; i++)
            if (i.Item() && i.Item()->OnButton(x, y) &&
                (i.Item()->GetState() == false || i.Item()->IsToggle()) &&
                (i.Item()->RadioGroup() < 0 || i.Item()->GetState() == false))
            {
                SetHover(i.Item());
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
        TButton *pressed = clicked >= 0 ? Buttons[clicked] : nullptr;
        clicked = -1;
        if (pressed && pressed->OnButton(x, y) && !pressed->IsToggle())
        {
            pressed->SetState(false);
            if (!pressed->Repeats())
                Activate(pressed);
        }
    }
}

// REVSYNC: 0x00436660. With BPF_HOVER the button under the pointer takes the
// hover; off every button the hover clears unless BPF_KEEPHOVER is set.
void TButtonPane::MouseMove(int32_t button, int32_t x, int32_t y)
{
    TButton *pressed = clicked >= 0 ? Buttons[clicked] : nullptr;
    if (pressed)
    {
        if (button == MB_LEFTDOWN)
        {
            if (pressed->OnButton(x, y))
            {
                if (pressed->GetState() == false && !pressed->IsToggle())
                    pressed->SetState(true);
            }
            else
            {
                if (pressed->GetState() == true || !pressed->IsToggle())
                    pressed->SetState(false);
            }
        }
        return;
    }

    if (!(paneflags & BPF_HOVER))
        return;
    TButton *under = nullptr;
    for (TPointerIterator<TButton> i(&Buttons); i; i++)
        if (i.Item() && i.Item()->IsHoverable() && i.Item()->OnButton(x, y))
            under = i.Item();
    if (under)
        SetHover(under);
    else if (!(paneflags & BPF_KEEPHOVER))
        SetHover(nullptr);
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

