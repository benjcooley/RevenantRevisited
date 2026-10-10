// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     SpellPane.cpp - Spell Pane                        *
// *************************************************************************

#include "revenant.h"
#include "bitmap.h"
#include "character.h"
#include "dialog.h"
#include "display.h"
#include "fonttable.h"
#include "logging.h"
#include "mappane.h"
#include "multi.h"
#include "player.h"
#include "playscreen.h"
#include "renderer.h"
#include "spell.h"
#include "font.h"
#include "spellpane.h"
#include "textbar.h"

#include <iterator>

extern TObjectClass TalismanClass;

char *Old[] =
{ "Sun", "Life", "Ocean", "Law", "Soul", "Stars", "Death", "Chaos", "Sky", "Earth", "Ward", "Moon", nullptr };

// *******************
// * TTalismanButton *
// *******************

void TTalismanButton::Compose(int32_t target_w, int32_t target_h)
{
    TButton::Compose(target_w, target_h);
    if (hidden || !Player || !Renderer)
        return;

    char spell[SPELLSIZE + 1];
    strncpyz(spell, Player->GetQuickSpell(quickspellid), MAXTALISMANLEN);
    const int32_t len = int32_t(strlen(spell));
    const int32_t start = x + xoffset + ((SPELLSIZE - len) * 11) - 4;

    for (int32_t i = 0; i < len; i++)
        for (int32_t t = 0; t < TalismanClass.NumTypes(); t++)
            if (toupper(spell[i]) == toupper(TalismanClass.GetStat(t, "Code")))
                Renderer->DrawBitmapToTarget(GameData->Bitmap(Old[t]), start + i * 23, y + 3 + (down ? 1 : 0),
                                             target_w, target_h);
}

void TTalismanButton::AddTalisman(char t)
{
    if (!Player)
        return;

    char spell[SPELLSIZE + 1];
    strncpyz(spell, Player->GetQuickSpell(quickspellid), MAXTALISMANLEN);
    int32_t len = strlen(spell);

    if (len < SPELLSIZE)
    {
        // add it
        spell[len++] = t;
        spell[len] = '\0';
                
        // now check for if you got that many
        if (!HasTalismans())
        {
            len--;
            spell[len] = '\0';
            TextBar.Print("No more talismans of that type.");
        }

        SetDirty();
    }
    
    Player->SetQuickSpell(quickspellid, spell);
}

void TTalismanButton::Backspace()
{
    if (!Player)
        return;

    char spell[SPELLSIZE + 1];
    strncpyz(spell, Player->GetQuickSpell(quickspellid), MAXTALISMANLEN);
    int32_t len = strlen(spell);

    if (len > 0)
    {
        len--;
        spell[len] = '\0';
        SetDirty();
    }

    Player->SetQuickSpell(quickspellid, spell);
}

void TTalismanButton::Clear()
{
    if (!Player)
        return;

    Player->SetQuickSpell(quickspellid, "");
}

char *TTalismanButton::GetSpell()
{
    if (!Player)
        return "";

    return Player->GetQuickSpell(quickspellid);
}

void TTalismanButton::SetSpell(char *talismans)
{
    if (!Player)
        return;

    Player->SetQuickSpell(quickspellid, talismans);

    SetDirty();
}

// Checks to see if player has talismans for this spell
bool TTalismanButton::HasTalismans()
{
    if (!Player)
        return false;

    return Player->HasTalismans(Player->GetQuickSpell(quickspellid));
}

void TTalismanButton::Invoke()
{
    if (Player)
        Player->InvokeQuickSpell(quickspellid);
}

// **************
// * TSpellPane *
// **************

void BtnSpellPane()
{
    SpellPane.Invoke();
}

void BtnSpellBook()
{
}

void BtnSpellAdd()
{
}

void BtnSpellBack()
{
    SpellPane.RemoveTal();
}

void BtnSpellDown()
{
    SpellPane.Scroll(1);
}

void BtnSpellUp()
{
    SpellPane.Scroll(-1);
}

void BtnSpellMin()
{
    SpellPane.ToggleTalismanNames();
}

#define REPEATRATE      8

bool TSpellPane::Initialize()
{
    TButtonPane::Initialize();

    NewButton(new TTalismanButton("spell",  24,  95, 120, 34, 0, BtnSpellPane, GameData->Bitmap("spellinvokedown"), GameData->Bitmap("spellinvokeup"), false, false, true, QSPELL_CONSTRUCT));
    NewButton("book",   3,   62, 16, 14, 0, BtnSpellBook, GameData->Bitmap("spellbookdown"), GameData->Bitmap("spellbookup"));
    NewButton("add",    3,   84, 16, 14, 0, BtnSpellAdd, GameData->Bitmap("spelladddown"), GameData->Bitmap("spelladdup"));
    NewButton("back",   142, 94, 12, 11, VK_BACK, BtnSpellBack, GameData->Bitmap("spellbackdown"), GameData->Bitmap("spellbackup"));
    NewButton("down",   149, 83, 16, 14, 0, BtnSpellDown, GameData->Bitmap("spelldowndown"), GameData->Bitmap("spelldownup"), false, false, false, -1, REPEATRATE);
    NewButton("up",     149, 66, 16, 14, 0, BtnSpellUp, GameData->Bitmap("spellupdown"), GameData->Bitmap("spellupup"), false, false, false, -1, REPEATRATE);
    NewButton("min",    149, 45, 16, 14, 0, BtnSpellMin, GameData->Bitmap("spellmindown"), GameData->Bitmap("spellminup"));

    shownames = true;
    startline = 0;
    clickedtal = -1;

    Update();

    return true;
}

#define TAL_STARTX      25
#define TAL_STARTY      9
#define TAL_WIDTH       30
#define TAL_HEIGHT      30
#define TAL_WRAPWIDTH   130
#define TAL_WRAPHEIGHT  76
#define TAL_NONAMEGAP   30

void TSpellPane::DrawBackground()
{
    bool wasdirty = IsDirty();

    if (wasdirty)
    {
        Display.Put(0, 0, GameData->Bitmap("spell"), DM_BACKGROUND);

        RedrawButtons();

        if (Player)
        {
            // find their spell pouch
            TObjectInstance* pouch = Player->FindObjInventory("spell pouch");
            if (!pouch)
                TObjectInstance* pouch = Player->FindObjInventory("spellpouch");
            if (pouch)
            {
                int32_t x = TAL_STARTX, y = TAL_STARTY;

                SColor color = { 0, 0, 0 };

                int32_t line = 0;
                for (int32_t i = 0; i<TalismanClass.NumTypes(); i++)
                {
                    char *name = TalismanClass.GetObjType(i)->name;

                    if (pouch->FindObjInventory(name))
                    {
                        if ((line++ >= startline || !ShowTalismanNames()) && y < TAL_WRAPHEIGHT)
                        {
                            int32_t add = 0;
                            if (onclickedtal && i == clickedtal)
                                add = 1;

                            char buf[80];
                            sprintf(buf, "%scandy", Old[i]);
                            Display.Put(x+2, y-2+2, GameData->Bitmap(buf), DM_BACKGROUND | DM_TRANSPARENT, &color);
                            Display.Put(x+add, y-2+add, GameData->Bitmap(buf), DM_BACKGROUND | DM_TRANSPARENT);
                            Display.Put(x+2+1, y+1+1, GameData->Bitmap(Old[i]), DM_BACKGROUND | DM_TRANSPARENT, &color);
                            Display.Put(x+2+add, y+1+add, GameData->Bitmap(Old[i]), DM_BACKGROUND | DM_TRANSPARENT);

                            if (ShowTalismanNames())
                                Display.WriteTextShadow(name, x + TAL_WIDTH - 1, y, 1, GameData->Font("goldfont"));

                            x += TAL_WIDTH;
                            if (ShowTalismanNames())
                                x += TAL_NONAMEGAP;

                            if (x > TAL_WRAPWIDTH)
                            {
                                x = TAL_STARTX;
                                y += TAL_HEIGHT;
                            }
                        }
                    }
                }

                //color.red = 255;
                //Display.WriteTextShadow("Greater Slurpee", 32, 79, 1, GameData->Font("tinyfont"), &color);

                // set up button visibility
                if (!ShowTalismanNames() || startline < 1)
                {
                    Button(5)->SetState(false);
                    Button(5)->Hide();
                }
                else
                    Button(5)->Show();

                if (!ShowTalismanNames() || (line - startline) <= 6)
                {
                    Button(4)->SetState(false);
                    Button(4)->Hide();
                }
                else
                    Button(4)->Show();
            }
        }

        if (shownames)
        {
            Button(6)->SetUpBitmap(GameData->Bitmap("spellminup"));
            Button(6)->SetDownBitmap(GameData->Bitmap("spellmindown"));
        }
        else
        {
            Button(6)->SetUpBitmap(GameData->Bitmap("spellmaxup"));
            Button(6)->SetDownBitmap(GameData->Bitmap("spellmaxdown"));
        }

        PlayScreen.MultiUpdate();
        SetDirty(false);
    }


    if (wasdirty)
        TButtonPane::DrawBackground();
}

void TSpellPane::MouseClick(int32_t button, int32_t x, int32_t y)
{
    TButtonPane::MouseClick(button, x, y);

    if (button == MB_LEFTDOWN)
    {
        clickedtal = OnTal(x, y);
        if (clickedtal >= 0)
        {
            onclickedtal = true;
            Update();
        }
    }
    else if (button == MB_LEFTUP && clickedtal >= 0)
    {
        if (onclickedtal)
            AddTal(clickedtal);

        clickedtal = -1;
        Update();
    }
}

void TSpellPane::MouseMove(int32_t button, int32_t x, int32_t y)
{
    TButtonPane::MouseMove(button, x, y);

    if (button == MB_LEFTDOWN && clickedtal >= 0)
    {
        bool oldonclickedtal = onclickedtal;

        if (OnTal(x, y) == clickedtal)
            onclickedtal = true;
        else
            onclickedtal = false;

        if (oldonclickedtal != onclickedtal)
            Update();
    }
}

int32_t TSpellPane::OnTal(int32_t x, int32_t y)
{
    if (x < TAL_STARTX || x >= (TAL_STARTX + TAL_WRAPWIDTH) ||
        y < TAL_STARTY || y >= (TAL_STARTY + TAL_WRAPHEIGHT))
        return -1;

    if (Player)
    {
        TObjectInstance* pouch = Player->FindObjInventory("spell pouch");
        if (!pouch)
            TObjectInstance* pouch = Player->FindObjInventory("spellpouch");
        if (pouch)
        {
            int32_t x0 = TAL_STARTX, y0 = TAL_STARTY;

            int32_t line = 0;
            for (int32_t i = 0; Old[i]; i++)
                if (pouch->FindObjInventory(Old[i]))
                    if ((line++ >= startline || !ShowTalismanNames()) && y0 < TAL_WRAPHEIGHT)
                    {
                        if (x >= x0 && x < (x0 + TAL_WIDTH) && y >= y0 && y < (y0 + TAL_HEIGHT))
                            return i;

                        x0 += TAL_WIDTH;
                        if (ShowTalismanNames())
                            x0 += TAL_NONAMEGAP;

                        if (x0 > TAL_WRAPWIDTH)
                        {
                            x0 = TAL_STARTX;
                            y0 += TAL_HEIGHT;
                        }
                    }
        }
    }

    return -1;
}

void TSpellPane::Scroll(int32_t numlines)
{
    int32_t oldstartline = startline;

    startline += numlines * 2;      // two talismans on a line

    if (startline < 0)
        startline = 0;

    if (startline != oldstartline)
        SetDirty(true);
}

void TSpellPane::ToggleTalismanNames()
{
    shownames = !shownames;
    SetDirty(true);
}

bool TSpellPane::AddTal(int32_t tal)
{
    char code = TalismanClass.GetStat(tal, "Code");
    ((PTTalismanButton)Button(0))->AddTalisman(code);
    SetDirty(true);
    return false;
}

bool TSpellPane::RemoveTal(int32_t numtals)
{
    for(int32_t i = 0; i < numtals; ++i)
        ((PTTalismanButton)Button(0))->Backspace();

    SetDirty(true);

    return true;
}

void TSpellPane::Invoke()
{
    ((PTTalismanButton)Button(0))->Invoke();
}

char *TSpellPane::GetSpell()
{
    return ((PTTalismanButton)Button(0))->GetSpell();
}

// *********************
// * TQuickSpellButton *
// *********************

namespace {

// REVSYNC: 0x00542900 -- the label is the spell's name as the game shows names
// (0x0046e7f0), split at its first space; a part over 9 characters shows its
// first 7 and "..". In white "Small", centred, no shadow: the first part in
// (x - 6, y - 10, 52 x two lines), the rest in (x - 6, y + 36, 52 x a line
// + 4).
constexpr int32_t kLabelLeft = -6;
constexpr int32_t kLabelWidth = 0x34;
constexpr int32_t kTopLabelTop = -10;
constexpr int32_t kBottomLabelTop = 0x24;
constexpr size_t kLabelChars = 9;
constexpr size_t kLabelCut = 7;

std::string LabelPart(std::string part)
{
    if (part.size() > kLabelChars)
        part = part.substr(0, kLabelCut) + "..";
    return part;
}

}  // namespace

TQuickSpellButton::TQuickSpellButton(const char *bname, int32_t bx, int32_t by, int32_t bw, int32_t bh,
                                     void (*bfunc)(), TBitmap *ringdown, TBitmap *ringup, TBitmap *ringgrey,
                                     const SFontAtlas *labelfont, int32_t labelline)
    : TButton(bname, bx, by, bw, bh, 0, bfunc, ringdown, ringup)
    , greybitmap(ringgrey)
    , font(labelfont)
    , lineheight(labelline)
{
}

void TQuickSpellButton::SetSpell(TBitmap *circle, std::string top, std::string bottom, bool cancast)
{
    if (circle == icon && top == toplabel && bottom == bottomlabel && cancast == castable)
        return;
    icon = circle;
    toplabel = std::move(top);
    bottomlabel = std::move(bottom);
    castable = cancast;
    SetDirty();
}

// REVSYNC: 0x00542900 -- the circle (magenta-keyed), then the ring with alpha:
// grey while the spell can't be cast, else down or up; pressed, both sink a
// pixel. Then the label. (Retail first restores the bar under the ring; the
// port's ring layer lies over the bar's.)
void TQuickSpellButton::Compose(int32_t target_w, int32_t target_h)
{
    dirty = false;
    if (hidden || !Renderer)
        return;
    const int32_t sink = down ? 1 : 0;
    if (icon)
        Renderer->DrawBitmapToTarget(icon, x + sink, y + sink, target_w, target_h);
    if (TBitmap *ring = !castable ? greybitmap : down ? downbitmap : upbitmap)
        Renderer->DrawBitmapToTarget(ring, x + sink, y + sink, target_w, target_h);
    if (!font)
        return;
    if (!toplabel.empty())
        DrawTextToTarget(font, toplabel.c_str(), x + kLabelLeft, y + kTopLabelTop, kLabelWidth, 2 * lineheight,
                         ETextAlign::Center, 1.0f, 1.0f, 1.0f, target_w, target_h);
    if (!bottomlabel.empty())
        DrawTextToTarget(font, bottomlabel.c_str(), x + kLabelLeft, y + kBottomLabelTop, kLabelWidth, lineheight + 4,
                         ETextAlign::Center, 1.0f, 1.0f, 1.0f, target_w, target_h);
}

// *******************
// * TQuickSpellPane *
// *******************

namespace {

// REVSYNC: TQuickSpellPane::Initialize (0x00544160) -- buttons "1".."4" at
// (10 + 50k, 10) (the tables at 0x005e4fe8 / 0x005e4ff8), 32 x 32, from
// SpellIcons.dat's RingD / RingU / RingG; each casts its quick spell.
constexpr int32_t kRingLeft = 10;
constexpr int32_t kRingStep = 50;
constexpr int32_t kRingTop = 10;
constexpr int32_t kRingHit = 0x20;
constexpr const char *kRingNames[] = { "1", "2", "3", "4" };
void (*const kRingFunctions[])() = {
    [] { QuickSpells.Invoke(1); }, [] { QuickSpells.Invoke(2); },
    [] { QuickSpells.Invoke(3); }, [] { QuickSpells.Invoke(4); },
};
static_assert(std::size(kRingNames) == TQuickSpellPane::kNumRings
              && std::size(kRingFunctions) == TQuickSpellPane::kNumRings);

// REVSYNC: the circle of a spell that names none in SpellIcons.dat (0x005e50bc).
constexpr const char *kDefaultCircle = "Aura";

}  // namespace

TQuickSpellPane::TQuickSpellPane() : TButtonPane(0, 0, 0, 0) {}
TQuickSpellPane::~TQuickSpellPane() = default;

// The pane takes its rect from the bottom bar (TBottomBarPane::LayOut).
bool TQuickSpellPane::Initialize()
{
    if (IsOpen())
        return true;
    icons.reset(TMulti::LoadMulti("SpellIcons.dat"));
    const SFontAtlas *font = FontTable ? FontTable->Atlas("Small") : nullptr;
    const TGenericFont *small = FontTable ? FontTable->FindFont("Small") : nullptr;
    if (!icons || !font || !small)
    {
        log_error("[quickspell] SpellIcons.dat or the \"Small\" font is missing");
        icons.reset();
        return false;
    }
    if (!TButtonPane::Initialize())
        return false;
    TBitmap *ringdown = icons->Bitmap("RingD");
    TBitmap *ringup = icons->Bitmap("RingU");
    TBitmap *ringgrey = icons->Bitmap("RingG");
    for (int32_t k = 0; k < kNumRings; ++k)
        NewButton(new TQuickSpellButton(kRingNames[k], kRingLeft + k * kRingStep, kRingTop, kRingHit, kRingHit,
                                        kRingFunctions[k], ringdown, ringup, ringgrey, font, small->height));
    return true;
}

void TQuickSpellPane::Close()
{
    if (!IsOpen())
        return;
    TButtonPane::Close();
    icons.reset();
}

// REVSYNC: 0x005444c0's loop. A ring shows the player's quick spell: the
// spell's circle (its ICONNAME, else "Aura") and its variant's name. It can
// be cast when the spell exists and the player holds its talismans
// (0x0051b7c0); otherwise, or with no quick spell there, it is grey.
void TQuickSpellPane::Pulse()
{
    for (int32_t ring = 1; ring <= kNumRings; ++ring)
    {
        TQuickSpellButton *button = Ring(ring);
        char *talismans = Player ? Player->GetQuickSpell(ring) : nullptr;
        SSpellData *spell = talismans && *talismans ? SpellList.GetSpellDataByTalismans(talismans) : nullptr;
        SSpellVariant *variant = spell ? SpellList.GetVariantDataByTalismans(talismans) : nullptr;
        if (!variant)
        {
            button->SetSpell(nullptr, {}, {}, false);
            continue;
        }
        TBitmap *circle = spell->iconname[0] ? icons->FindBitmap(spell->iconname) : nullptr;
        if (!circle)
            circle = icons->Bitmap(kDefaultCircle);
        const std::string name = DialogList.DisplayName(variant->name);
        const size_t space = name.find(' ');
        button->SetSpell(circle, LabelPart(name.substr(0, space)),
                         space == std::string::npos ? std::string() : LabelPart(name.substr(space + 1)),
                         Player->HasTalismans(talismans));
    }
}

void TQuickSpellPane::Invoke(int32_t ring)
{
    if (ring >= 1 && ring <= kNumRings && Player)
        Player->InvokeQuickSpell(ring);
}

TQuickSpellButton *TQuickSpellPane::Ring(int32_t ring)
{
    return static_cast<TQuickSpellButton *>(Button(ring - 1));
}
