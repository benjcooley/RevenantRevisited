// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     textbar.cpp - Text Bar Pane                       *
// *************************************************************************

#include <stdlib.h>
#include <stdarg.h>

#include "revenant.h"
#include "textbar.h"
#include "display.h"
#include "multi.h"
#include "object.h"

bool TTextBar::Initialize()
{
    TPane::Initialize();

    text[0] = 0;
    name[0] = 0;
    animating = false;
    SetDirty(true);
    return true;
}

void TTextBar::Close()
{
}

#define HEALTH_INCREMENT    4

void TTextBar::DrawBackground()
{
    bool drawhealth = name[0];

    if (animating || (drawhealth && targetlevel != level) || IsDirty())
    {
        uint32_t drawmode;

        if (drawhealth)
        {
            if (absval(targetlevel - level) <= HEALTH_INCREMENT)
                level = targetlevel;
            else if (targetlevel > level)
                level += HEALTH_INCREMENT;
            else if (level > targetlevel)
                level -= HEALTH_INCREMENT;
        }

        if (drawhealth && targetlevel != level)
        {
            animating = true;
            drawmode = DM_NORESTORE;
        }
        else
        {
            animating = false;
            drawmode = DM_BACKGROUND;
        }

        Display->Box(0, 0, GetWidth(), GetHeight(), 0, 0xffff, 0, drawmode);

        if (drawhealth)
        {
            int32_t hue = (level * 155) / 176;
            if (hue > 16)
                hue -= 16;
            else
                hue = 0;

            Display->PutHue(min(0, -(186 - level)), 1, GameData->Bitmap("texthealthbar"), drawmode, hue);
            Display->WriteTextShadow(name, 2, -6, 1, GameData->Font("silverfont"), nullptr, drawmode | DM_TRANSPARENT);
        }
        else
            Display->WriteText(text, 2, -6, 1, GameData->Font("silverfont"));

        SetDirty(false);
    }

    if (pulsecheck)
        pulsecheck = false;
    else
        ClearHealthDisplay();
}

void TTextBar::Print(char *txt, ...)
{
    va_list marker;
    va_start(marker, txt);

    name[0] = '\0';
    vsprintf(text, txt, marker);
    SetDirty(true);
}

void TTextBar::SetHealthDisplay(const char *n, int32_t l)
{
    pulsecheck = true;

    if (strcmp(name, n) == 0)
        targetlevel = l;
    else
    {
        strcpy(name, n);
        level = targetlevel = l;
        SetDirty(true);
    }
}

void TTextBar::SetLevels(int32_t newlevel, int32_t newtargetlevel)
{
    if (!name[0])
        strcpy(name, text);
    level = newlevel;
    targetlevel = newtargetlevel;
}

void TTextBar::ClearHealthDisplay()
{
    if (name[0])
    {
        name[0] = 0;
        SetDirty(true);
    }
}

