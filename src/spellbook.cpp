// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  spellbook.cpp - the side panel's spell book                          *
// *************************************************************************
//
// See spellbook.h. Every constant is retail's, cited by the instruction that
// holds it in docs/ui/forensics/SpellBookPane_RETAIL.md.
//
// *************************************************************************

#include "spellbook.h"

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
#include <cstdlib>
#include <cstring>
#include <string>

extern TObjectClass TalismanClass;

namespace {

// The window onto the rows, in the pane (0x545cbd / 0x545cc8): the book's
// parchment and rows, 148 x 229, between its caps.
constexpr int32_t kWindowX = 0x12;
constexpr int32_t kWindowY = 0x2f;
constexpr int32_t kWindowW = 0x94;
constexpr int32_t kWindowH = 0xe5;
constexpr int32_t kPaperStep = 0x50;        // a parchment tile's height (0x54533f)
constexpr int32_t kPaperEnd = 0x140;        // tiles down to here (0x54535a)
constexpr int32_t kCapTopX = 0x4f, kCapTopY = 0x2d;             // 0x545dc3 / 0x545dce
constexpr int32_t kCapBottomX = 0x4b, kCapBottomY = 0x105;      // 0x545e69 / 0x545e74

// A row, from its top: the circle and its ring at (12, 0), the name beside
// it, the description from y 44 and its lines, the skill and mana under it,
// the talismans 17 px further, 10 px to the next row.
constexpr int32_t kRowChrome = 0x5b;        // a row's height besides its description (0x546955)
constexpr int32_t kCircleX = 0x0c;
constexpr int32_t kCircleSize = 0x28;
constexpr int32_t kPressShift = -2;         // the pressed circle of a spell the player holds (0x5457a7)
constexpr int32_t kNameShown = 0x32;        // the name and circle, while the row's top is this near (0x545584)
constexpr int32_t kNameX = 0x36, kNameW = 0x55, kNameH = 0x56;
constexpr int32_t kNameTop = 0x04, kNameBottom = 0x10, kNameSingle = 0x0a;
constexpr size_t kNameChars = 10;           // a longer part shows 8 characters and ".." (0x545614)
constexpr size_t kNameCut = 8;
constexpr int32_t kDescX = 0x0a, kDescY = 0x2c, kDescW = 0x84;
constexpr int32_t kManaX = 0x5a;
constexpr int32_t kStatsReach = 0x2f;       // the skill and mana, while the row's bottom is this near (0x5459fc)
constexpr int32_t kGlyphX = 0x0c, kGlyphStep = 0x10, kGlyphTop = 0x3d;
constexpr int32_t kGlyphReach = 0x1e;       // 0x5458d4

constexpr int32_t kScrollStep = 0x28;       // the most a pulse scrolls (0x546652)
// Dragging inside this part of the pane scrolls the parchment (0x5463b6).
constexpr int32_t kDragLeft = 0x12, kDragRight = 0xa6, kDragTop = 0x12, kDragBottom = 0x114;
constexpr int32_t kDragHotspot = 0x14;      // the dragged circle's point under the pointer (0x5464bb)

// The arrows (0x544d08 / 0x544dc2), 24 x 24: they overhang the pane by 5 px.
constexpr int32_t kArrowX = 0xa9, kArrowUpY = 0x96, kArrowDownY = 0xae, kArrowSize = 0x18;

// The text's colours (PackRGB 0x00429950): the description, the skill, the mana.
constexpr float kDescColour[3] = { 0xf2 / 255.0f, 0xe7 / 255.0f, 0xff / 255.0f };
constexpr float kSkillColour[3] = { 0x05 / 255.0f, 0xf4 / 255.0f, 0xf4 / 255.0f };
constexpr float kManaColour[3] = { 0xe9 / 255.0f, 0xff / 255.0f, 0x25 / 255.0f };

// A spell's description: the dialog line "SPELL<talismans>" (0x5e52e4).
std::string DescriptionTag(const char *talismans)
{
    return std::string("SPELL") + talismans;
}

// A part of a name as the book shows it: over 10 characters, the first 8
// and "..".
std::string NamePart(std::string part)
{
    if (part.size() > kNameChars)
        part = part.substr(0, kNameCut) + "..";
    return part;
}

}  // namespace

TSpellBookPane::TSpellBookPane() : TButtonPane(0, 0, kWidth, kHeight) {}
TSpellBookPane::~TSpellBookPane() = default;

// REVSYNC: 0x005449e0. Its host places it in the side panel's upper slot.
bool TSpellBookPane::Initialize()
{
    if (IsOpen())
        return true;
    archive.reset(TMulti::LoadMulti("SpellScroll.dat"));
    icons = AcquireSpellIcons();
    textfont = FontTable ? FontTable->Atlas("Small") : nullptr;
    titlefont = FontTable ? FontTable->Atlas("SpellTitle") : nullptr;
    if (!archive || !icons || !textfont || !titlefont)
    {
        log_error("[spellbook] SpellScroll.dat, SpellIcons.dat or the \"Small\" / \"SpellTitle\" font is missing");
        archive.reset();
        icons.reset();
        return false;
    }
    scrollart = archive->Bitmap("Scroll");
    paper = archive->Bitmap("ScrollPaper");
    captop = archive->Bitmap("ScrlTop");
    capbottom = archive->Bitmap("ScrlBottom");
    for (int32_t type = 0; type < kNumTalismanTypes; ++type)
    {
        codes[type] = type < TalismanClass.NumTypes() ? static_cast<char>(TalismanClass.GetStat(type, "Code")) : 0;
        glyphs[type] = archive->Bitmap((std::string("S") + kTalismanArtNames[type]).c_str());
    }
    if (!TButtonPane::Initialize())
        return false;

    // No hotkeys, no repeat: a click steps one row. The arrows draw through
    // their bitmaps' alpha (+0xd4 |= DM_ALPHA, 0x544d6b).
    struct SArrow { const char *name; int32_t y; void (*func)(); const char *down, *up, *grey; };
    static constexpr SArrow kArrows[] = {
        { "up", kArrowUpY, [] { SpellBook.StepRow(-1); }, "ScrlArwUD", "ScrlArwUU", "ScrlArwUG" },
        { "down", kArrowDownY, [] { SpellBook.StepRow(1); }, "ScrlArwDD", "ScrlArwDU", "ScrlArwDG" },
    };
    for (const SArrow &a : kArrows)
    {
        auto *arrow = new TButton(a.name, kArrowX, a.y, kArrowSize, kArrowSize, 0, a.func, archive->Bitmap(a.down),
                                  archive->Bitmap(a.up), false, false, true);
        arrow->SetGreyBitmap(archive->Bitmap(a.grey));
        arrow->SetDrawMode(DM_USEDEFAULT | DM_ALPHA);
        NewButton(arrow);
    }
    scroll = pending = 0;
    pressed = -1;
    steppable = true;
    Rebuild();
    return true;
}

// REVSYNC: 0x00544e40
void TSpellBookPane::Close()
{
    if (!IsOpen())
        return;
    TButtonPane::Close();
    heights.clear();
    rings.clear();
    total = 0;
    scrollart = paper = captop = capbottom = nullptr;
    glyphs = {};
    icons.reset();
    archive.reset();
}

int32_t TSpellBookPane::RowCount() const
{
    return Player ? (std::min)(Player->NumKnownSpells(), static_cast<int32_t>(heights.size())) : 0;
}

int32_t TSpellBookPane::RowTop(int32_t row) const
{
    int32_t top = 0;
    for (int32_t i = 0; i < row && i < static_cast<int32_t>(heights.size()); ++i)
        top += heights[i];
    return top;
}

// The row height a description takes: its lines in "Small" across 132 px
// (0x004acb80) and the row's chrome.
int32_t TSpellBookPane::RowHeight(const char *description) const
{
    std::vector<std::string> lines;
    return WrapTextLines(textfont, description, static_cast<float>(kDescW), lines)
           * static_cast<int32_t>(textfont->lineStep) + kRowChrome;
}

// Retail's clamp (0x5466c4): to the last full window, then to the top.
int32_t TSpellBookPane::ClampScroll(int32_t pos) const
{
    return (std::max)((std::min)(pos, total - kWindowH), 0);
}

void TSpellBookPane::ScrollTo(int32_t pos)
{
    scroll = ClampScroll(pos);
    pending = 0;
    SetDirty(true);
}

// REVSYNC: 0x00545846 -- lit while the player holds the talismans and has the
// mana (vtable +0x1d0), pressed while the circle is.
TSpellBookPane::ERing TSpellBookPane::RingOf(int32_t row) const
{
    const char *talismans = Player->KnownSpell(row);
    const SSpellVariant *variant = *talismans ? SpellList.GetVariantDataByTalismans(talismans) : nullptr;
    if (variant && Player->HasTalismans(talismans) && variant->mana <= Player->Mana())
        return pressed == row ? ERing::Down : ERing::Up;
    return ERing::Grey;
}

// REVSYNC: 0x00546830
void TSpellBookPane::Rebuild()
{
    heights.clear();
    total = 0;
    builtfor = Player;
    builtversion = Player ? Player->KnownSpellsVersion() : -1;
    if (!Player || !textfont)
        return;
    for (int32_t i = 0; i < Player->NumKnownSpells(); ++i)
    {
        const char *talismans = Player->KnownSpell(i);
        if (!SpellList.GetSpellDataByTalismans(talismans))
        {
            Player->ForgetSpell(i--);
            continue;
        }
        heights.push_back(RowHeight(DialogList.GetLine(DescriptionTag(talismans).c_str())));
        total += heights.back();
    }
    builtversion = Player->KnownSpellsVersion();
}

// REVSYNC: 0x00544fb0
void TSpellBookPane::Reveal(const char *talismans)
{
    if (!Player || !talismans || !*talismans)
        return;
    const SSpellData *spell = SpellList.GetSpellDataByTalismans(talismans);
    if (!spell || !spell->iconname[0] || DialogList.FindLine(DescriptionTag(talismans).c_str()) < 0)
        return;
    if (!IsOpen())
    {
        Player->LearnSpell(talismans);      // the rows follow when the book opens
        return;
    }
    int32_t top = 0;
    for (int32_t row = 0; row < RowCount(); top += heights[row++])
        if (strcmp(Player->KnownSpell(row), talismans) == 0)
        {
            pending = top - scroll;
            SetDirty(true);
            return;
        }
    Player->LearnSpell(talismans);
    Rebuild();
    pending = RowTop(RowCount() - 1) - scroll;
    SetDirty(true);
    if (RowTop(RowCount() - 1) < scroll + kWindowH)
        steppable = false;          // the new row shows before the scroll moves (0x545273)
}

// REVSYNC: 0x00544f00 -- from the first row whose top, or 40 px below it, is
// in the window; nothing when none is.
void TSpellBookPane::StepRow(int32_t direction)
{
    int32_t previous = 0, top = 0;
    for (int32_t row = 0; row < RowCount(); previous = top, top += heights[row++])
    {
        const auto shown = [&](int32_t y) { return y >= scroll && y <= scroll + kWindowH; };
        if (shown(top) || shown(top + kCircleSize))
        {
            pending = (direction > 0 ? top + heights[row] : previous) - scroll;
            return;
        }
    }
}

// REVSYNC: 0x00546620. The scroll steps only once the book has been
// composed since it last moved; a ring that changes recomposes the book
// (retail left it stale until a repaint).
void TSpellBookPane::Pulse()
{
    TButtonPane::Pulse();
    if (!IsOpen())
        return;
    if (Player != builtfor || (Player && Player->KnownSpellsVersion() != builtversion))
    {
        Rebuild();
        SetDirty(true);
    }
    if (steppable && pending != 0)
    {
        const int32_t step = std::abs(pending) >= kScrollStep ? (pending > 0 ? kScrollStep : -kScrollStep) : pending;
        pending -= step;
        const int32_t moved = ClampScroll(scroll + step);
        if (moved != scroll)
        {
            scroll = moved;
            steppable = false;
            SetDirty(true);
        }
    }
    if (static_cast<int32_t>(rings.size()) != RowCount())
        SetDirty(true);
    for (int32_t row = 0; row < RowCount() && !IsDirty(); ++row)
        if (RingOf(row) != rings[row])
            SetDirty(true);
}

// REVSYNC: 0x005452d0 -- the scroll's chrome; the parchment and the rows in
// the window; its caps over the window's edges. The arrows follow (the
// buttons). The window is the scissor rect: retail composed it on a keyed
// surface first, which shows the same but where its own translucent art
// blended with the key (the pink fringe, not reproduced).
void TSpellBookPane::ComposeBackground(int32_t target_w, int32_t target_h)
{
    steppable = true;
    rings.assign(RowCount(), ERing::Grey);
    if (!Renderer)
        return;
    Renderer->DrawBitmapToTarget(scrollart, 0, 0, target_w, target_h, EBitmapDecode::Unkeyed);
    Renderer->SetTargetClip(kWindowX, kWindowY, kWindowW, kWindowH);
    const int32_t first = scroll >= 0 ? -(scroll % kPaperStep) : -kPaperStep - scroll % kPaperStep;
    for (int32_t y = first; y < kPaperEnd; y += kPaperStep)
        Renderer->DrawBitmapToTarget(paper, kWindowX, kWindowY + y, target_w, target_h, EBitmapDecode::MagentaKeyed);
    for (int32_t row = 0, top = 0; row < RowCount(); top += heights[row++])
    {
        rings[row] = RingOf(row);
        if (top + heights[row] > scroll && top < scroll + kWindowH)
            ComposeRow(row, top, target_w, target_h);
    }
    Renderer->ResetTargetClip(target_w, target_h);
    Renderer->DrawBitmapToTarget(captop, kCapTopX, kCapTopY, target_w, target_h, EBitmapDecode::MagentaKeyed);
    Renderer->DrawBitmapToTarget(capbottom, kCapBottomX, kCapBottomY, target_w, target_h, EBitmapDecode::MagentaKeyed);
}

// One row, its top `top` px down the rows; each part only while it can
// reach the window (0x5454db, 0x545584, 0x5458d4, 0x5459fc).
void TSpellBookPane::ComposeRow(int32_t row, int32_t top, int32_t target_w, int32_t target_h)
{
    const char *talismans = Player->KnownSpell(row);
    const SSpellData *spell = SpellList.GetSpellDataByTalismans(talismans);
    const SSpellVariant *variant = SpellList.GetVariantDataByTalismans(talismans);
    const int32_t height = heights[row];
    const int32_t desch = height - kRowChrome;
    const int32_t over = top + height - scroll - kWindowH;     // how far the row's bottom is under the window
    const int32_t x0 = kWindowX;
    const int32_t y0 = kWindowY + top - scroll;                 // the row's top in the pane
    const int32_t k = scroll - top;

    if (k < desch + kDescY && k + kWindowH > kDescY)
        DrawTextWrappedToTarget(textfont, DialogList.GetLine(DescriptionTag(talismans).c_str()), x0 + kDescX,
                                y0 + kDescY, kDescW, desch, ETextAlign::Left, kDescColour[0], kDescColour[1],
                                kDescColour[2], true, target_w, target_h);

    if (k < kNameShown && variant)
    {
        const std::string name = DialogList.DisplayName(variant->name);
        const size_t space = name.find(' ');
        const auto title = [&](const std::string &part, int32_t y) {
            DrawTextWrappedToTarget(titlefont, NamePart(part).c_str(), x0 + kNameX, y0 + y, kNameW, kNameH,
                                    ETextAlign::Center, 1.0f, 1.0f, 1.0f, true, target_w, target_h);
        };
        if (space != std::string::npos)
        {
            title(name.substr(0, space), kNameTop);
            title(name.substr(space + 1), kNameBottom);
        }
        else
            title(name, kNameSingle);
        if (TBitmap *circle = spell ? SpellCircle(*icons, *spell) : nullptr)
        {
            const int32_t shift = pressed == row && Player->HasTalismans(talismans) ? kPressShift : 0;
            static constexpr const char *kRingNames[] = { "RingG", "RingU", "RingD" };
            Renderer->DrawBitmapToTarget(circle, x0 + kCircleX + shift, y0 + shift, target_w, target_h,
                                         EBitmapDecode::MagentaKeyed);
            Renderer->DrawBitmapToTarget(icons->Bitmap(kRingNames[static_cast<int32_t>(rings[row])]),
                                         x0 + kCircleX + shift, y0 + shift, target_w, target_h,
                                         EBitmapDecode::Unkeyed);
        }
    }

    if (over < kGlyphReach)
        for (int32_t i = 0, x = kGlyphX; talismans[i]; ++i, x += kGlyphStep)
            for (int32_t type = 0; type < kNumTalismanTypes; ++type)
                if (codes[type] && std::toupper(static_cast<unsigned char>(codes[type]))
                                       == std::toupper(static_cast<unsigned char>(talismans[i])))
                {
                    Renderer->DrawBitmapToTarget(glyphs[type], x0 + x, y0 + desch + kGlyphTop, target_w, target_h,
                                                 EBitmapDecode::Unkeyed);
                    break;
                }

    if (over < kStatsReach && variant)
    {
        const int32_t y = y0 + desch + kDescY;
        const std::string skill = std::string(DialogList.GetLine("SPANESKILLS")) + ": "
                                + std::to_string(variant->skilllevel);
        const std::string mana = std::string(DialogList.GetLine("SPANEMANA")) + ": " + std::to_string(variant->mana);
        DrawTextWrappedToTarget(textfont, skill.c_str(), x0 + kDescX, y, kDescW, desch, ETextAlign::Left,
                                kSkillColour[0], kSkillColour[1], kSkillColour[2], true, target_w, target_h);
        DrawTextWrappedToTarget(textfont, mana.c_str(), x0 + kManaX, y, kDescW, desch, ETextAlign::Left,
                                kManaColour[0], kManaColour[1], kManaColour[2], true, target_w, target_h);
    }
}

// The row whose circle (40 x 40 at (12, 0) in the row) holds the pane's
// (x, y), or -1; the last when two would (0x545fa2).
int32_t TSpellBookPane::RowAtCircle(int32_t x, int32_t y) const
{
    const int32_t cx = x - kWindowX, cy = y - kWindowY;
    int32_t hit = -1;
    for (int32_t row = 0, top = 0; row < RowCount(); top += heights[row++])
        if (top < scroll + cy && scroll + cy < top + heights[row] && kCircleX < cx && cx < kCircleX + kCircleSize
            && top - scroll < cy && cy < top - scroll + kCircleSize)
            hit = row;
    return hit;
}

bool TSpellBookPane::CirclePoint(int32_t row, int32_t &x, int32_t &y) const
{
    if (row < 0 || row >= RowCount())
        return false;
    x = kWindowX + kCircleX + kCircleSize / 2;
    y = kWindowY + RowTop(row) - scroll + kCircleSize / 2;
    return true;
}

// REVSYNC: 0x00545f10's cast -- with no target; a spell the player can't put
// together, or one that fails, fizzles, and the text bar says so.
void TSpellBookPane::Cast(int32_t row)
{
    char fizzle[] = "Fizzle";
    std::string talismans = Player->KnownSpell(row);
    if (talismans.empty())
        Player->CastByName(fizzle);
    else if (!Player->HasTalismans(talismans.c_str()))
    {
        TextBar.Print("%s", DialogList.GetLine("SPLMISTAL"));
        Player->CastByName(fizzle);
    }
    else if (!Player->CastByTalismans(talismans.data()))
    {
        Player->CastByName(fizzle);
        TextBar.Print("%s", DialogList.GetLine("SPLCASTFAIL"));
    }
    else
        TextBar.Print("%s", DialogList.GetLine("SPLCASTOK"));
}

// Every button-up on the book (0x005462fc): the press ends, and a spell
// carried over the book goes no further.
void TSpellBookPane::Release()
{
    if (pressed != -1)
    {
        pressed = -1;
        SetDirty(true);
    }
    if (UIDragState::IsActive() && UIDragState::Get().source == EDragSource::SpellPane)
        UIDragState::CompleteDrag(EDragSource::SpellPane, -1, false);
}

// REVSYNC: 0x00545f10 -- only while the player has control: a press on a
// circle picks its row; a release on the same circle casts it; a spell let
// go over the book scrolls to its row.
void TSpellBookPane::MouseClick(int32_t button, int32_t x, int32_t y)
{
    if (!PlayScreen.IsControlOn())
        return;
    TButtonPane::MouseClick(button, x, y);
    if (!InPane(x, y) || !Player)
        return;
    if (button == MB_LEFTDOWN)
    {
        const int32_t row = RowAtCircle(x, y);
        if (row >= 0)
        {
            pressed = row;
            SetDirty(true);
        }
    }
    else if (button == MB_LEFTUP)
    {
        const SUIDragState &drag = UIDragState::Get();
        if (drag.source == EDragSource::SpellPane && !drag.spell.empty())
        {
            if (SpellList.GetSpellDataByTalismans(drag.spell.c_str()))
                for (int32_t row = 0; row < RowCount(); ++row)
                    if (drag.spell == Player->KnownSpell(row))
                    {
                        pending = RowTop(row) - scroll;
                        break;
                    }
        }
        else if (pressed >= 0 && RowAtCircle(x, y) == pressed)
            Cast(pressed);
        Release();
    }
}

// REVSYNC: 0x00546390 -- with the button held: on the parchment with no
// circle pressed, the rows follow the pointer; with a circle pressed, the
// spell is picked up at once (no threshold), its circle under the pointer.
void TSpellBookPane::MouseMove(int32_t button, int32_t x, int32_t y)
{
    TButtonPane::MouseMove(button, x, y);
    const bool onpaper = InPane(x, y) && kDragLeft < x && x < kDragRight && kDragTop < y && y < kDragBottom;
    if (onpaper && button == MB_LEFTDOWN && pressed == -1)
    {
        pending += lasty - y;
        lasty = y;
        return;
    }
    if (button == MB_LEFTDOWN && pressed >= 0 && Player && !UIDragState::IsActive())
    {
        const char *talismans = Player->KnownSpell(pressed);
        if (const SSpellData *spell = *talismans ? SpellList.GetSpellDataByTalismans(talismans) : nullptr)
        {
            UIDragState::BeginSpellDrag(pressed, talismans, SpellCircle(*icons, *spell), GetPosX() + x,
                                        GetPosY() + y, kDragHotspot, kDragHotspot);
            pressed = -1;
            SetDirty(true);
        }
    }
    lasty = y;
}
