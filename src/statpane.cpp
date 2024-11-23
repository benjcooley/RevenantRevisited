// ************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   statpane.cpp - FORSAKEN Stat Pane                   *
// *************************************************************************

#include "revenant.h"
#include "statpane.h"
#include "display.h"
#include "playscreen.h"
#include "player.h"
#include "multi.h"
#include "font.h"

void BtnStatStats()
{
    StatPane.Update();
}

void BtnStatSkills()
{
    StatPane.Update();
}

void BtnStatMin()
{
    StatPane.ContractAll();
    StatPane.Scroll(-10000);
}

void BtnStatMax()
{
    StatPane.ExpandAll();
}

void BtnStatUp()
{
    StatPane.Scroll(-1);
}

void BtnStatDown()
{
    StatPane.Scroll(1);
}

#define NUMLINES    6
#define SCROLLRATE  8
#define TEXT_POS_Y  23

bool TStatPane::Initialize()
{
    TButtonPane::Initialize();

    NewButton("stats", 19, 5, 64, 19, VK_RETURN, BtnStatStats, GameData->Bitmap("statsdown"), GameData->Bitmap("statsup"), false, true, false, 1);
    NewButton("skills", 84, 5, 64, 19, VK_RETURN, BtnStatSkills, GameData->Bitmap("skillsdown"), GameData->Bitmap("skillsup"), false, true, false, 1);

    NewButton("up", 149, 66, 16, 14, 0, BtnStatUp, GameData->Bitmap("spellupdown"), GameData->Bitmap("spellupup"), false, false, false, -1, SCROLLRATE);
    NewButton("down", 149, 83, 16, 14, 0, BtnStatDown, GameData->Bitmap("spelldowndown"), GameData->Bitmap("spelldownup"), false, false, false, -1, SCROLLRATE);

    NewButton("min", 149, 49, 16, 14, 0, BtnStatMin, GameData->Bitmap("statmindown"), GameData->Bitmap("statminup"));
    NewButton("max", 149, 34, 16, 14, 0, BtnStatMax, GameData->Bitmap("statmaxdown"), GameData->Bitmap("statmaxup"));

    startline = 0;
    memset(&expanded, 0, sizeof(bool) * NUM_SKILLS);

    Button(0)->SetState(true);

    return true;
}

void TStatPane::Close()
{
    TButtonPane::Close();
}

#define GEM         (char)('~' + 13)
#define GEM_LINE    (char)(GEM + 1)
#define LINE        (char)(GEM_LINE + 1)
#define LINE_BRANCH (char)(LINE + 1)
#define LINE_END    (char)(LINE_BRANCH + 1)

void TStatPane::DrawBackground()
{
    if (IsDirty())
    {
        Display->Put(0, 0, GameData->Bitmap("statpane"), DM_BACKGROUND);

        RedrawButtons();

        char buf1[512] = "";
        char buf2[512] = "";

        bool showdownbutton = true;

        if (Button(0)->GetState() == true)
        {
            for (int32_t i = 0; i < NUM_PLRSTATS; i++)
                sprintf(buf1, "%s   %s\n", buf1, Player->ObjStatName(PLRSTAT_FIRST + i));

            if (Player)
                for (int32_t i = 0; i < NUM_PLRSTATS; i++)
                    sprintf(buf2, "%s%d\n", buf2, Player->GetObjStat(PLRSTAT_FIRST + i));

            showdownbutton = false;
        }
        else
        {
            int32_t line = 0;
            for (int32_t i = 0; i < NUM_SKILLS; i++, line++)
            {
                bool isline = true;

                if (SkillTree[i].ancestor < 0)
                {
                    if (line >= startline)
                    {
                        if (!SkillTree[i].children)
                            sprintf(buf1, "%s   %s\n", buf1, SkillTree[i].name);
                        else
                        {
                            if (expanded[i])
                                sprintf(buf1, "%s%c %s\n", buf1, GEM_LINE, SkillTree[i].name);
                            else
                                sprintf(buf1, "%s%c %s\n", buf1, GEM, SkillTree[i].name);
                        }
                    }
                }
                else
                {
                    int32_t lastlen = strlen(buf1);

                    // check for ancestor of the ancestor...
                    int32_t grandparent = SkillTree[i].ancestor;
                    while (grandparent >= 0)
                    {
                        if (!expanded[grandparent])
                        {
                            isline = false;
                            break;
                        }

                        char fillchar;

                        if (SkillTree[grandparent].lastchild)
                            fillchar = ' ';
                        else
                            fillchar = LINE;

                        grandparent = SkillTree[grandparent].ancestor;
                        if (grandparent < 0)
                            break;

                        if (fillchar == ' ')
                            sprintf(buf1, "%s  ", buf1);
                        else
                            sprintf(buf1, "%s%c", buf1, fillchar);
                    }

                    if (!isline || line < startline)
                        buf1[lastlen] = 0;
                    else
                    {
                        if (!SkillTree[i].children)
                        {
                            if (SkillTree[i].lastchild)
                                sprintf(buf1, "%s%c %s\n", buf1, LINE_END, SkillTree[i].name);
                            else
                                sprintf(buf1, "%s%c %s\n", buf1, LINE_BRANCH, SkillTree[i].name);
                        }
                        else
                        {
                            int32_t gemchar = expanded[i] ? GEM_LINE : GEM;

                            if (SkillTree[i].lastchild)
                                sprintf(buf1, "%s%c%c %s\n", buf1, LINE_END, gemchar, SkillTree[i].name);
                            else
                                sprintf(buf1, "%s%c%c %s\n", buf1, LINE_BRANCH, gemchar, SkillTree[i].name);
                        }
                    }
                }

                if (!isline)
                    line--;     // the line is hidden, don't count it
                else if (line >= startline && Player)
                    sprintf(buf2, "%s%d\n", buf2, Player->Skill(i));
            }

            if ((line - startline) <= NUMLINES)
                showdownbutton = false;
        }

        Display->WriteTextShadow(buf1, 12, TEXT_POS_Y, NUMLINES, GameData->Font("goldfont"));

        if (Player)
            Display->WriteTextShadow(buf2, 138, TEXT_POS_Y, NUMLINES, GameData->Font("goldfont"), nullptr, FONT_DRAWMODE, -1, 0, JUSTIFY_RIGHT);

        // determine which scroll buttons are showing
        if (startline < 1 || Button(0)->GetState())
        {
            ScrollUpButton()->SetState(false);
            ScrollUpButton()->Hide();
        }
        else
            ScrollUpButton()->Show();

        if (showdownbutton)
            ScrollDownButton()->Show();
        else
        {
            ScrollDownButton()->SetState(false);
            ScrollDownButton()->Hide();
        }
        
        SetDirty(false);
        PlayScreen.MultiUpdate();
    }

    TButtonPane::DrawBackground();
}

void TStatPane::MouseClick(int32_t button, int32_t x, int32_t y)
{
    TButtonPane::MouseClick(button, x, y);

    if (button == MB_LEFTDOWN)
    {
        int32_t slot = OnSlot(x, y);
        if (slot >= 0 && SkillTree[slot].children)
        {
            expanded[slot] = !expanded[slot];
            SetDirty(true);
        }
    }
}

void TStatPane::MouseMove(int32_t button, int32_t x, int32_t y)
{
    TButtonPane::MouseMove(button, x, y);
}

void TStatPane::Scroll(int32_t numlines)
{
    int32_t oldstartline = startline;

    startline += numlines;

    if (startline < 0)
        startline = 0;
    if ((startline + NUMLINES) > NUM_SKILLS)
        startline = NUM_SKILLS - NUMLINES;

    if (startline != oldstartline)
        SetDirty(true);
}

void TStatPane::SetAllExpanded(bool value)
{
    bool changed = false;

    for (int32_t i = 0; i < NUM_SKILLS; i++)
        if (SkillTree[i].children)
            if (expanded[i] != value)
            {
                expanded[i] = value;
                changed = true;
            }

    if (changed)
        SetDirty(true);
}

int32_t TStatPane::OnSlot(int32_t x, int32_t y)
{
    if (x > 113)            // buttons are to the right of this value
        return -1;

    y -= TEXT_POS_Y + 4;
    if (y < 0)
        return -1;

    int32_t slot = y / GameData->Font("goldfont")->height;
    if (slot < 0 || slot >= NUMLINES)
        return -1;

    slot += startline;

    int32_t line = 0;
    for (int32_t i = 0; i < NUM_SKILLS; i++)
    {
        bool isline = true;
        int32_t grandparent = SkillTree[i].ancestor;
        while (grandparent >= 0)
        {
            if (!expanded[grandparent])
            {
                isline = false;
                break;
            }
            grandparent = SkillTree[grandparent].ancestor;
        }

        if (isline && line++ == slot)
            return i;
    }

    return -1;
}
