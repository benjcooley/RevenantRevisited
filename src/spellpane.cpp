// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     SpellPane.cpp - Spell Pane                        *
// *************************************************************************

#include "spellpane.h"

#include "bitmap.h"
#include "dialog.h"
#include "font.h"
#include "fonttable.h"
#include "logging.h"
#include "multi.h"
#include "object.h"
#include "player.h"
#include "playscreen.h"
#include "renderer.h"
#include "spell.h"
#include "textbar.h"
#include "uidragstate.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <iterator>
#include <string>

extern TObjectClass TalismanClass;

namespace {

// REVSYNC: 0x005e4f80 -- the talismans' art names, in type order: GameData's
// "<name>" icon and "<name>candy" plate, SpellPane.dat's "S<name>" glyph
// (0x005e4fb4).
constexpr const char *kTalismanArtNames[kNumTalismanTypes] = {
    "Sun", "Life", "Ocean", "Law", "Soul", "Stars", "Death", "Chaos", "Sky", "Earth", "Ward", "Moon",
};

// REVSYNC: 0x00542640 / 0x005424a0 -- the spell pane keeps a spell of 5
// talismans at most.
constexpr size_t kSpellTalismans = 5;

// REVSYNC: 0x005424a0 -- the glyphs from x + glyphleft + (5 - n) * 14, 25
// apart, at y + 5, a pixel down while pressed.
constexpr int32_t kGlyphShift = 14;
constexpr int32_t kGlyphStep = 0x19;
constexpr int32_t kGlyphTop = 5;

}  // namespace

// *******************
// * TTalismanButton *
// *******************

TTalismanButton::TTalismanButton(const char *bname, int32_t bx, int32_t by, int32_t bw, int32_t bh, void (*bfunc)(),
                                 TBitmap *dbm, TBitmap *ubm, const TTalismanArt *talismanart, int32_t left)
    : TButton(bname, bx, by, bw, bh, 0, bfunc, dbm, ubm, false, false, true)
    , art(talismanart)
    , glyphleft(left)
{
}

void TTalismanButton::SetSpell(const std::string &talismans)
{
    if (talismans == spell)
        return;
    spell = talismans;
    SetDirty();
}

// REVSYNC: 0x005424a0 -- the button, then each talisman's glyph with alpha
// (every type whose code matches, case aside).
void TTalismanButton::Compose(int32_t target_w, int32_t target_h)
{
    TButton::Compose(target_w, target_h);
    if (hidden || !art || !Renderer)
        return;
    const size_t count = (std::min)(spell.size(), kSpellTalismans);
    int32_t left = x + glyphleft + static_cast<int32_t>(kSpellTalismans - count) * kGlyphShift;
    const int32_t top = y + kGlyphTop + (down ? 1 : 0);
    for (size_t i = 0; i < count; ++i, left += kGlyphStep)
        for (const STalismanArt &talisman : *art)
            if (talisman.glyph && std::toupper(static_cast<unsigned char>(talisman.code))
                                      == std::toupper(static_cast<unsigned char>(spell[i])))
                Renderer->DrawBitmapToTarget(talisman.glyph, left, top, target_w, target_h, EBitmapDecode::Unkeyed);
}

// **************
// * TSpellPane *
// **************

namespace {

// REVSYNC: 0x005432a0 -- the buttons: "spell" (the talisman button, its
// glyphs 7 in), "back" (key VK_BACK), "up" and "down" (repeating at 8),
// "min".
constexpr int32_t kRepeatRate = 8;
constexpr int32_t kSpellGlyphLeft = 7;
enum EButton : int32_t { kSpell, kBack, kUp, kDown, kNames, kNumButtons };
struct SButton
{
    const char *name;
    int32_t x, y, w, h;
    uint16_t key;
    int32_t repeat;
    const char *down;
    const char *up;
};
constexpr SButton kButtons[kNumButtons] = {
    { "spell", 0x18, 0x88, 0x8e, 0x24, 0, 0, "SpellInvD", "SpellInvU" },
    { "back", 0xa4, 0x8f, 0x16, 0x14, VK_BACK, 0, "SpellBackD", "SpellBackU" },
    { "up", 0xa6, 0x52, 0x14, 0x1a, 0, kRepeatRate, "SpellArwUD", "SpellArwUU" },
    { "down", 0xa6, 0x70, 0x14, 0x1a, 0, kRepeatRate, "SpellArwDD", "SpellArwDU" },
    { "min", 0xa6, 0x25, 0x12, 0x12, 0, 0, "SpellNameD", "SpellNameU" },
};
void (*const kButtonFunctions[kNumButtons])() = {
    [] { SpellPane.Invoke(); },
    [] { SpellPane.RemoveTalisman(); },
    [] { SpellPane.Scroll(-1); },
    [] { SpellPane.Scroll(1); },
    [] { SpellPane.ToggleNames(); },
};
// The names button's art: names on, names off (0x005435f0).
constexpr const char *kNamesOn[2] = { "SpellNameD", "SpellNameU" };
constexpr const char *kNamesOff[2] = { "SpellTalD", "SpellTalU" };

// REVSYNC: 0x005435f0 / 0x00543ca0 -- the list: from (33, 10), 30 px a
// talisman (60 with names), wrapping past x 130 to 30 px down; rows from y
// 126 left out. Hits fall in (33..162, 10..135), a talisman 30 x 30.
constexpr int32_t kListLeft = 0x21;
constexpr int32_t kListTop = 0xa;
constexpr int32_t kListStep = 0x1e;
constexpr int32_t kListWrap = 0x82;
constexpr int32_t kListBottom = 0x7e;
constexpr int32_t kHitRight = 0xa3;
constexpr int32_t kHitBottom = 0x88;
constexpr int32_t kTalismansPerLine = 2;
constexpr int32_t kLinesShown = 6;          // "down" shows past this many from the first line

// REVSYNC: 0x005435f0 -- a talisman's plate at (x, y - 2) and icon at
// (x + 2, y + 1), keyed, each over its black silhouette (a Put with a colour)
// 2 px right (the plate) or 1 px right and down (the icon); pressed, both
// sit 1 px down and right. Its name from (x + 29, y + 10), a baseline
// (0x00438ed0 draws with flag 0x80).
constexpr int32_t kNameLeft = 0x1d;
constexpr int32_t kNameTop = 0xa;

constexpr const char *kPouchName = "spell pouch";   // retail's second look, "spellpouch", is thrown away

}  // namespace

TSpellPane::TSpellPane() : TButtonPane(0, 0, kWidth, kHeight) {}
TSpellPane::~TSpellPane() = default;

// REVSYNC: 0x005432a0. Its host places it in the side panel's lower slot.
bool TSpellPane::Initialize()
{
    if (IsOpen())
        return true;
    archive.reset(TMulti::LoadMulti("SpellPane.dat"));
    namefont = FontTable ? FontTable->Atlas("Small") : nullptr;
    if (!archive || !GameData || !namefont)
    {
        log_error("[spellpane] SpellPane.dat, GameData or the \"Small\" font is missing");
        archive.reset();
        return false;
    }
    background = archive->Bitmap("spellconstr");
    for (int32_t type = 0; type < kNumTalismanTypes; ++type)
    {
        const std::string name = kTalismanArtNames[type];
        STalismanArt &talisman = art[type];
        talisman.code = type < TalismanClass.NumTypes() ? static_cast<char>(TalismanClass.GetStat(type, "Code")) : 0;
        talisman.plate = GameData->Bitmap((name + "candy").c_str());
        talisman.icon = GameData->Bitmap(name.c_str());
        talisman.glyph = archive->Bitmap(("S" + name).c_str());
    }
    if (!TButtonPane::Initialize())
        return false;
    for (int32_t i = 0; i < kNumButtons; ++i)
    {
        const SButton &b = kButtons[i];
        TBitmap *down = archive->Bitmap(b.down);
        TBitmap *up = archive->Bitmap(b.up);
        NewButton(i == kSpell ? new TTalismanButton(b.name, b.x, b.y, b.w, b.h, kButtonFunctions[i], down, up, &art,
                                                    kSpellGlyphLeft)
                              : new TButton(b.name, b.x, b.y, b.w, b.h, b.key, kButtonFunctions[i], down, up, false,
                                            false, false, -1, b.repeat));
    }
    talismans.clear();
    heldtypes = 0;
    shownames = true;
    firstline = 0;
    pressed = -1;
    onpressed = false;
    RefreshButtons();
    SetDirty(true);
    return true;
}

void TSpellPane::Close()
{
    if (!IsOpen())
        return;
    TButtonPane::Close();
    talismans.clear();
    heldtypes = 0;
    art = {};
    background = nullptr;
    archive.reset();
}

TTalismanButton *TSpellPane::SpellButton()
{
    return Buttons.NumItems() > kSpell ? static_cast<TTalismanButton *>(Button(kSpell)) : nullptr;
}

std::string TSpellPane::ConstructSpell() const
{
    const char *spell = Player ? Player->GetQuickSpell(QSPELL_CONSTRUCT) : "";
    return std::string(spell, (std::min)(std::strlen(spell), kSpellTalismans));
}

// REVSYNC: 0x005435f0 -- the talismans in the player's spell pouch, in type
// order, named as the game names them (the dialog list's line, else the type
// name); the construct spell on the "spell" button.
void TSpellPane::Pulse()
{
    TButtonPane::Pulse();
    const int32_t types = (std::min)(TalismanClass.NumTypes(), kNumTalismanTypes);
    uint32_t held = 0;
    if (TObjectInstance *pouch = Player ? Player->FindObjInventory(kPouchName) : nullptr)
        for (int32_t type = 0; type < types; ++type)
            if (pouch->FindObjInventory(TalismanClass.GetObjType(type)->name))
                held |= 1u << type;
    if (held != heldtypes)
    {
        heldtypes = held;
        talismans.clear();
        for (int32_t type = 0; type < types; ++type)
            if (held & (1u << type))
            {
                const char *name = TalismanClass.GetObjType(type)->name;
                const char *line = DialogList.GetLine(name);
                talismans.push_back({ type, line[0] == '[' ? name : line });
            }
        RefreshButtons();
        SetDirty(true);
    }
    if (TTalismanButton *button = SpellButton())
        button->SetSpell(ConstructSpell());
}

template <typename Fn>
void TSpellPane::ForEachShown(Fn &&fn) const
{
    int32_t x = kListLeft, y = kListTop, line = 0;
    for (const STalisman &talisman : talismans)
    {
        if ((line++ < firstline && shownames) || y >= kListBottom)
            continue;
        fn(talisman, x, y);
        x += shownames ? 2 * kListStep : kListStep;
        if (x > kListWrap)
        {
            x = kListLeft;
            y += kListStep;
        }
    }
}

int32_t TSpellPane::TalismanAt(int32_t x, int32_t y) const
{
    if (x < kListLeft || x >= kHitRight || y < kListTop || y >= kHitBottom)
        return -1;
    int32_t hit = -1;
    ForEachShown([&](const STalisman &talisman, int32_t left, int32_t top) {
        if (hit < 0 && x >= left && x < left + kListStep && y >= top && y < top + kListStep)
            hit = talisman.type;
    });
    return hit;
}

// REVSYNC: 0x005435f0 -- "up" shows with names on and a first line past 0,
// "down" with names on and more than six talismans from it; a hidden arrow
// is released. "min" shows the names art with names on, the talismans art
// off. (Retail sets the arrows only while the player has a spell pouch: with
// none they stay as they were, shown from the start with nothing to page.
// A retail bug, not reproduced: with no talismans both are hidden.)
void TSpellPane::RefreshButtons()
{
    if (Buttons.NumItems() < kNumButtons)
        return;
    const auto show = [](TButton *button, bool shown) {
        if (shown)
            button->Show();
        else if (!button->IsHidden())
        {
            button->SetState(false);
            button->Hide();
        }
    };
    show(Button(kUp), shownames && firstline >= 1);
    show(Button(kDown), shownames && static_cast<int32_t>(talismans.size()) - firstline > kLinesShown);
    const char *const *names = shownames ? kNamesOn : kNamesOff;
    TBitmap *down = archive->Bitmap(names[0]);
    TBitmap *up = archive->Bitmap(names[1]);
    TButton *toggle = Button(kNames);
    if (toggle->DownBitmap() != down || toggle->UpBitmap() != up)
    {
        toggle->SetDownBitmap(down);
        toggle->SetUpBitmap(up);
    }
}

void TSpellPane::ComposeBackground(int32_t target_w, int32_t target_h)
{
    if (!Renderer)
        return;
    Renderer->DrawBitmapToTarget(background, 0, 0, target_w, target_h, EBitmapDecode::Unkeyed);
    ForEachShown([&](const STalisman &talisman, int32_t x, int32_t y) {
        const STalismanArt &look = art[talisman.type];
        const int32_t sink = PressedTalisman() == talisman.type ? 1 : 0;
        if (look.plate)
        {
            Renderer->DrawBitmapTintedToTarget(look.plate, x + 2, y, target_w, target_h, 0.0f, 0.0f, 0.0f, 1.0f);
            Renderer->DrawBitmapToTarget(look.plate, x + sink, y - 2 + sink, target_w, target_h);
        }
        if (look.icon)
        {
            Renderer->DrawBitmapTintedToTarget(look.icon, x + 3, y + 2, target_w, target_h, 0.0f, 0.0f, 0.0f, 1.0f);
            Renderer->DrawBitmapToTarget(look.icon, x + 2 + sink, y + 1 + sink, target_w, target_h);
        }
        if (shownames)
            DrawTextShadowedAtBaseline(namefont, talisman.name.c_str(), static_cast<float>(x + kNameLeft),
                                       static_cast<float>(y + kNameTop), 1.0f, 1.0f, 1.0f, target_w, target_h);
    });
}

// REVSYNC: 0x00543b40 -- only while the player has control: a press on a
// talisman picks it, a release on it adds it.
void TSpellPane::MouseClick(int32_t button, int32_t x, int32_t y)
{
    if (!PlayScreen.IsControlOn())
        return;
    TButtonPane::MouseClick(button, x, y);
    if (button == MB_LEFTDOWN)
    {
        pressed = TalismanAt(x, y);
        if (pressed >= 0)
        {
            onpressed = true;
            SetDirty(true);
        }
    }
    else if (button == MB_LEFTUP && pressed >= 0)
    {
        if (onpressed)
            AddTalisman(pressed);
        pressed = -1;
        onpressed = false;
        SetDirty(true);
    }
}

// REVSYNC: 0x00543c40 -- the pressed talisman follows the pointer on and off.
void TSpellPane::MouseMove(int32_t button, int32_t x, int32_t y)
{
    TButtonPane::MouseMove(button, x, y);
    if (button != MB_LEFTDOWN || pressed < 0)
        return;
    const bool on = TalismanAt(x, y) == pressed;
    if (on != onpressed)
    {
        onpressed = on;
        SetDirty(true);
    }
}

// REVSYNC: 0x00542d50
void TSpellPane::Invoke()
{
    if (Player)
        Player->InvokeQuickSpell(QSPELL_CONSTRUCT);
}

// REVSYNC: 0x00542640 -- a full spell stays as it is; a talisman the player
// holds no more of comes off again, and the text bar says so.
void TSpellPane::AddTalisman(int32_t type)
{
    if (!Player || type < 0 || type >= kNumTalismanTypes)
        return;
    std::string spell = ConstructSpell();
    if (spell.size() < kSpellTalismans)
    {
        spell += art[type].code;
        Player->SetQuickSpell(QSPELL_CONSTRUCT, spell.c_str());
        if (!Player->HasTalismans(spell.c_str()))
        {
            spell.pop_back();
            TextBar.Print("%s", DialogList.GetLine("SPLOUTTAL"));
        }
    }
    Player->SetQuickSpell(QSPELL_CONSTRUCT, spell.c_str());
    SetDirty(true);
}

// REVSYNC: 0x00542e30
void TSpellPane::RemoveTalisman()
{
    if (Player)
    {
        std::string spell = ConstructSpell();
        if (!spell.empty())
            spell.pop_back();
        Player->SetQuickSpell(QSPELL_CONSTRUCT, spell.c_str());
    }
    SetDirty(true);
}

// REVSYNC: 0x00542ed0 / 0x00542f00 -- not below the first talisman.
void TSpellPane::Scroll(int32_t lines)
{
    const int32_t first = (std::max)(firstline + lines * kTalismansPerLine, 0);
    if (first == firstline)
        return;
    firstline = first;
    RefreshButtons();
    SetDirty(true);
}

// REVSYNC: 0x00542f30
void TSpellPane::ToggleNames()
{
    shownames = !shownames;
    RefreshButtons();
    SetDirty(true);
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
    , font(labelfont)
    , lineheight(labelline)
{
    greybitmap = ringgrey;
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

// REVSYNC: 0x00542900 -- the circle, keyed on magenta alone (retail sets its
// key to the display's, 0x00542ab3), then the ring with alpha: grey while the
// spell can't be cast, else down or up; pressed, both sink a pixel. Then the
// label. (Retail first restores the bar under the ring; the port's ring layer
// lies over the bar's.)
void TQuickSpellButton::Compose(int32_t target_w, int32_t target_h)
{
    dirty = false;
    if (hidden || !Renderer)
        return;
    const int32_t sink = down ? 1 : 0;
    if (icon)
        Renderer->DrawBitmapToTarget(icon, x + sink, y + sink, target_w, target_h, EBitmapDecode::MagentaKeyed);
    if (TBitmap *ring = !castable ? greybitmap : down ? downbitmap : upbitmap)
        Renderer->DrawBitmapToTarget(ring, x + sink, y + sink, target_w, target_h, EBitmapDecode::Unkeyed);
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
        const char *talismans = Player ? Player->GetQuickSpell(ring) : nullptr;
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

// REVSYNC: 0x00544890. A spell dragged from the spell book and let go over a
// ring goes on that ring, if spell.def has it (TPlayer::SetQuickSpell
// 0x0051b580); a spell let go anywhere on the pane ends its drag there. Any
// other click is the rings' own.
void TQuickSpellPane::MouseClick(int32_t button, int32_t x, int32_t y)
{
    if (button == MB_LEFTUP && UIDragState::IsDragging() && UIDragState::Get().source == EDragSource::SpellPane
        && InPane(x, y))
    {
        const int32_t ring = RingAt(x, y);
        const int32_t row = UIDragState::Get().source_idx;     // the spell book's row: a known spell
        std::string talismans = Player && row >= 0 && row < Player->NumKnownSpells() ? Player->KnownSpell(row) : "";
        const bool dropped = ring > 0 && !talismans.empty() && SpellList.GetSpellDataByTalismans(talismans.data());
        if (dropped)
            Player->SetQuickSpell(ring, talismans.data());
        UIDragState::CompleteDrag(EDragSource::QuickSpell, ring, dropped);
        if (dropped)
            return;
    }
    TButtonPane::MouseClick(button, x, y);
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

int32_t TQuickSpellPane::RingAt(int32_t x, int32_t y)
{
    for (int32_t ring = 1; ring <= kNumRings; ++ring)
        if (Ring(ring)->OnButton(x, y))
            return ring;
    return 0;
}
