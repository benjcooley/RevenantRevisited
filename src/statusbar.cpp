// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *           statusbar.cpp - Status bars (health, stamina)               *
// *************************************************************************

#include "revenant.h"
#include "statusbar.h"
#include "display.h"

// **************
// * TStatusBar *
// **************

bool TStatusBar::Initialize()
{
    tubedata = nullptr;
    level = targetlevel = 1000;
    animating = false;
    SetDirty(true);
    return true;
}

void TStatusBar::Close()
{
    if (tubedata)
        delete tubedata;
}

#define HEALTH_INCREMENT    10

void TStatusBar::DrawBackground()
{
    if (animating || level != targetlevel || IsDirty())
    {
        if (absval(level - targetlevel) <= HEALTH_INCREMENT)
            level = targetlevel;
        else if (level < targetlevel)
            level += HEALTH_INCREMENT;
        else if (level > targetlevel)
            level -= HEALTH_INCREMENT;

        bool drawmode = 0;

        if (level != targetlevel)
        {
            animating = true;
            drawmode |= DM_NORESTORE;
        }
        else
        {
            animating = false;
            drawmode |= DM_BACKGROUND;
        }

        Display->PutHue(0, -(176 * level / 1000), tubedata->Bitmap("tube"), drawmode, GetHue());

        drawmode |= DM_TRANSPARENT;

        Display->Put(0, 0, tubedata->Bitmap("topoverlay"), drawmode);
        Display->Put(0, 59, tubedata->Bitmap("middleoverlay"), drawmode);
        Display->Put(0, 161, tubedata->Bitmap("bottomoverlay"), drawmode);

        SetDirty(false);
    }
}

// **************
// * THealthBar *
// **************

bool THealthBar::Initialize()
{
    TStatusBar::Initialize();

    tubedata = TMulti::LoadMulti("health.dat");

    return tubedata != nullptr;
}

int32_t THealthBar::GetHue()
{
    int32_t hue = level * 155 / 1000;
    if (hue > 16)
        hue -= 16;
    else
        hue = 0;

    return hue;
}

// ***************
// * TStaminaBar *
// ***************

bool TStaminaBar::Initialize()
{
    TStatusBar::Initialize();

    tubedata = TMulti::LoadMulti("stamina.dat");

    return tubedata != nullptr;
}

int32_t TStaminaBar::GetHue()
{
    return ((1000 - level) * 65 / 1000) + 240;
}

