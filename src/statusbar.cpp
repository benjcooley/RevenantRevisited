// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *           statusbar.cpp - The player and target status bar            *
// *************************************************************************

#include "statusbar.h"

#include "bitmap.h"
#include "character.h"
#include "dialog.h"
#include "font.h"
#include "fonttable.h"
#include "logging.h"
#include "multi.h"
#include "player.h"
#include "playscreen.h"
#include "renderer.h"
#include "surface.h"
#include "time.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <vector>

namespace {

// REVSYNC: TPlayScreen::Pulse (0x0047b4d0) sizes the pane: the width of the
// map view (the display less the side pane), 0x70 tall.
constexpr int32_t kPaneHeight = 0x70;

// REVSYNC: a chip's fade runs 0..6, one step a tick (Pulse 0x00549da0). Draw
// (0x0054ab80) tints the chip by fade * 255 / 6; 0x0054af20 blits its text
// once that passes 128, from fade 4.
constexpr int32_t kFadeSteps = 6;
constexpr bool TextShown(int32_t fade) { return fade * 255 / kFadeSteps > 128; }

// The tint's alpha for a fade, in retail's whole steps of 1/255: 42/255 at
// fade 1. Between ticks the fade is fractional and so is the step it lands on.
float FadeAlpha(float fade)
{
    return std::floor(fade * 255.0f / float(kFadeSteps)) / 255.0f;
}

// Where a chip's pieces go. The player's x values run from the pane's left
// edge, the opponent's from its right edge (FromEdge); x values inside a
// layer run from the layer's left.
struct SChipLayout
{
    int32_t artEdge;        // the art layer's left edge
    int32_t ringCenterX;    // in the art layer
    int32_t iconX;          // in the art layer
    int32_t cellsEdge;      // the text cells layer's left edge
    int32_t valueX;         // the value cells, in the cells layer
    ETextAlign valueAlign;
    int32_t nameX;          // the name cell, in the cells layer
};

// REVSYNC: the art at (4, 4) and (pane_w - 0x88, 4) (Draw 0x0054ab80); ring
// centres at x 0x1a and 0x62, icons at x 0x2b and 0x3a (0x0054a0a0,
// 0x0054a310); value cells at x 0x47, left-aligned, and pane_w - 0x80,
// right-aligned; name cells at x 0 and pane_w - 0x44 (0x0054af20).
constexpr SChipLayout kLayouts[] = {
    { 0x04, 0x1a, 0x2b, 0x00, 0x47, ETextAlign::Left,  0x00 },     // player
    { 0x88, 0x62, 0x3a, 0x80, 0x00, ETextAlign::Right, 0x3c },     // opponent
};

// REVSYNC: the art layer is one of Initialize's 0x80 x 0x40 ARGB4444
// surfaces (0x00549740). The portrait goes through a 0x28 x 0x28 16-bit
// scratch surface: copied in whole, then blitted with its centre on the
// ring's and the surface's key into the art, so only its magenta is
// transparent (its own key, black for the character icons, stays opaque)
// and its colour drops to 4 bits a channel: EBitmapDecode::Overlay4444. The
// icons' rows are 3, 0x11 and 0x20 (0x0054a0a0).
constexpr int32_t kArtTop = 4;
constexpr int32_t kArtWidth = 0x80;
constexpr int32_t kArtHeight = 0x40;
constexpr int32_t kRingCenterY = 0x1f;
constexpr int32_t kPortraitSize = 0x28;
constexpr int32_t kIconTop[] = { 0x03, 0x11, 0x20 };      // health, mana, fatigue

// REVSYNC: the bars as Draw (0x0054ab80) hands them to the bar kernel
// FUN_0054a5d0. "bars" holds each one twice, full and empty, from texel
// column 2. The value fills `length` texels: the near cap (`cap` wide), the
// middle, then the first `cap` texels of the far cap, whose remaining texels
// close the bar. The player's bars start 0x44 from the pane's left edge; the
// opponent's end 0x44 from its right (retail's pane_w - 0xc1 / 0x91 / 0x79).
struct SBarArt
{
    int32_t top;            // pane y
    int32_t fullRow;        // "bars" texel row of the full bar
    int32_t emptyRow;       // and of the empty one
    int32_t width;          // the whole bar
    int32_t height;
    int32_t length;         // the texels the value fills
    int32_t cap;            // the caps' fill width
};
constexpr int32_t kBarsColumn = 2;
constexpr int32_t kBarInset = 0x44;
constexpr SBarArt kBarArt[] = {
    { 0x0f, 0x01, 0x31, 0x7d, 0x11, 0x77, 6 },      // health: vtable +0x1c0 of +0x1d8
    { 0x1f, 0x13, 0x42, 0x4d, 0x0c, 0x45, 4 },      // mana: +0x1d0 of +0x1e8
    { 0x2c, 0x22, 0x51, 0x35, 0x0c, 0x2d, 4 },      // fatigue: +0x1c8 of +0x1e0
};

// REVSYNC: 0x0054af20's cells: the values 0x32 x 0x0e at rows 7, 0x17 and
// 0x24, the name 0x40 x 0x40 at row 0x36, in white over the three-pass black
// shadow (font flag 0x400). The values are "%d"; the name is word-wrapped
// and centred (DT_WORDBREAK | DT_CENTER).
constexpr int32_t kValueTop[] = { 0x07, 0x17, 0x24 };
constexpr int32_t kValueWidth = 0x32;
constexpr int32_t kValueHeight = 0x0e;
constexpr int32_t kNameTop = 0x36;
constexpr int32_t kNameSize = 0x40;
constexpr int32_t kCellsWidth = 0x80;
constexpr int32_t kCellsHeight = kNameTop + kNameSize;

static_assert(std::size(kBarArt) == TPlyrStatusBar::kNumBars && std::size(kIconTop) == TPlyrStatusBar::kNumBars
              && std::size(kValueTop) == TPlyrStatusBar::kNumBars);

// REVSYNC: 0x0054af20 -- an opponent goes by the dialog line tagged with its
// name less everything but ASCII letters and digits ("Lizard Man" ->
// LIZARDMAN), else by the name itself.
std::string DisplayName(const char* name)
{
    std::string tag;
    for (const char* c = name; *c; ++c)
        if ((*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z') || (*c >= '0' && *c <= '9'))
            tag += *c;
    const int32_t id = DialogList.FindLine(tag.c_str());
    return id >= 0 ? DialogList.GetLine(id) : name;
}

// REVSYNC: 0x0054ae10 -- a player's name cell is the STATBARFMT line
// ("%s\nLevel %d" without one) filled with its name and level; any other
// character's is its name.
std::string NameCell(const std::string& name, int32_t level)
{
    if (level < 0)
        return name;
    const int32_t id = DialogList.FindLine("STATBARFMT");
    const char* format = id >= 0 ? DialogList.GetLine(id) : "%s\nLevel %d";
    char text[128];
    std::snprintf(text, sizeof(text), format, name.c_str(), int(level));
    return text;
}

}  // namespace

TPlyrStatusBar::TPlyrStatusBar() = default;
TPlyrStatusBar::~TPlyrStatusBar() = default;

// REVSYNC: 0x00549740. Retail makes its surfaces here and takes StatusBar.dat
// from TPlayScreen, which loads it (DAT_0065a9d0); the pane loads its own
// archive and makes its layers as Compose first needs them.
bool TPlyrStatusBar::Initialize()
{
    if (IsOpen())
        return true;

    archive.reset(TMulti::LoadMulti("StatusBar.dat"));
    font = FontTable ? FontTable->Atlas("Small") : nullptr;
    const TGenericFont* small = FontTable ? FontTable->FindFont("Small") : nullptr;
    if (!archive || !font || !small)
    {
        log_error("[statusbar] StatusBar.dat or the \"Small\" font is missing");
        archive.reset();
        font = nullptr;
        return false;
    }
    bars = archive->Bitmap("bars");
    backpanel = archive->Bitmap("BackPanel");
    ring = archive->Bitmap("Ring");
    icons = { archive->Bitmap("HealthIcon"), archive->Bitmap("ManaIcon"), archive->Bitmap("FatigueIcon") };
    // GDI's DrawText steps a wrapped line by the font's cell height.
    lineheight = small->height;

    LayOut();
    if (!TPane::Initialize())
        return false;
    chips = {};
    return true;
}

// REVSYNC: 0x00549d40
void TPlyrStatusBar::Close()
{
    if (!IsOpen())
        return;
    chips = {};
    archive.reset();
    bars = backpanel = ring = nullptr;
    icons = {};
    font = nullptr;
    lineheight = 0;
    TPane::Close();
}

void TPlyrStatusBar::LayOut()
{
    int32_t mapx = 0, mapy = 0, mapw = 0, maph = 0;
    PlayScreen.GetMapViewRect(mapx, mapy, mapw, maph);
    Resize(mapx, mapy, mapw, kPaneHeight);
}

// REVSYNC: 0x00549da0 (slot 19), once a tick from the screen's pane pass.
// The player's chip fades in while there is a player and goes with it; the
// opponent's fades in while the player fights (the root action is combat or
// the bow: Fighting()) and out once the fight ends. Retail reads the
// characters as it composes and draws; the port reads them here.
void TPlyrStatusBar::Pulse()
{
    LayOut();
    TCharacter* player = Player;
    ReadPlayer(player);
    ReadTarget(player);
}

void TPlyrStatusBar::ReadPlayer(TCharacter* player)
{
    SChip& chip = Chip(ESide::Player);
    chip.fadeGoal = player ? kFadeSteps : 0;
    chip.fade = player ? (std::min)(chip.fade + 1, kFadeSteps) : 0;
    ReadCharacter(ESide::Player, player, true);
}

// The chip keeps the last opponent while it fades out: retail keeps its map
// index (+0xb8) and finds it again (0x00452690). Its text stays as the fight
// left it (0x0054af20 refreshes it from the current opponent only); its bars
// follow the character.
void TPlyrStatusBar::ReadTarget(TCharacter* player)
{
    SChip& chip = Chip(ESide::Target);
    TCharacter* opponent = player ? player->Fighting() : nullptr;
    if (opponent == player)
        opponent = nullptr;
    chip.fadeGoal = opponent ? kFadeSteps : 0;
    if (!player)
        chip.fade = 0;
    else if (chip.fade != chip.fadeGoal)
        chip.fade += chip.fade < chip.fadeGoal ? 1 : -1;
    if (opponent)
        ReadCharacter(ESide::Target, opponent, true);
    else
        ReadCharacter(ESide::Target, player ? chip.character.Get() : nullptr, false);
}

void TPlyrStatusBar::ReadCharacter(ESide side, TCharacter* character, bool readText)
{
    SChip& chip = Chip(side);
    chip.present = character != nullptr;
    if (!character)
        return;

    const TSafeRef<TCharacter> ref(character);
    const bool changed = ref != chip.character;
    TBitmap* portrait = character->InventoryImage();
    if (changed || portrait != chip.portrait)
        ++chip.artVersion;
    chip.character = ref;
    chip.portrait = portrait;
    chip.stats = { SStat{ character->Health(), character->MaxHealth() },
                   SStat{ character->Mana(), character->MaxMana() },
                   SStat{ character->Fatigue(), character->MaxFatigue() } };
    if (!readText)
        return;

    SText text;
    for (size_t bar = 0; bar < kNumBars; ++bar)
        text.values[bar] = chip.stats[bar].value;
    if (changed)
    {
        const char* name = character->GetName() ? character->GetName() : "";
        text.name = side == ESide::Target ? DisplayName(name) : name;
    }
    else
        text.name = chip.text.name;
    if (character->ObjClass() == OBJCLASS_PLAYER)
        text.level = static_cast<TPlayer*>(character)->Level();
    if (text != chip.text)
    {
        chip.text = std::move(text);
        ++chip.textVersion;
    }
}

// REVSYNC: 0x00549e60 (slot 20). A dirty pane rebuilds both chips' art;
// otherwise a chip's art is rebuilt when its character changes (0x0054a0a0,
// 0x0054a310) -- or, in the port, when its portrait does. The text cells are
// 0x0054af20's, rebuilt when a value or the name changes.
void TPlyrStatusBar::Compose()
{
    if (IsDirty())
    {
        for (SChip& chip : chips)
        {
            ++chip.artVersion;
            ++chip.textVersion;
        }
        SetDirty(false);
    }
    if (!Renderer || !archive)
        return;
    for (ESide side : kSides)
    {
        const SChip& chip = Chip(side);
        if (!chip.present)
            continue;
        if (chip.composedArt != chip.artVersion)
            ComposeArt(side);
        if (chip.composedText != chip.textVersion)
            ComposeCells(side);
    }
}

// REVSYNC: 0x0054a0a0 (player) / 0x0054a310 (opponent), in retail's order:
// the BackPanel; the portrait, through its window centred on the ring; the
// Ring; then the fatigue, mana and health icons, each over the one below.
void TPlyrStatusBar::ComposeArt(ESide side)
{
    SChip& chip = Chip(side);
    const SChipLayout& layout = kLayouts[size_t(side)];
    if (!chip.art)
        chip.art = std::make_unique<TSurface>(kArtWidth, kArtHeight, SG_PIXELFORMAT_RGBA8);

    chip.art->StartPass(0.0f, 0.0f, 0.0f, 0.0f);
    Renderer->DrawBitmapToTarget(backpanel, 0, 0, kArtWidth, kArtHeight);
    if (TBitmap* portrait = chip.portrait)
        Renderer->DrawBitmapSubrectToTarget(portrait, layout.ringCenterX - kPortraitSize / 2,
                                            kRingCenterY - kPortraitSize / 2, 0, 0,
                                            (std::min)(portrait->width, kPortraitSize),
                                            (std::min)(portrait->height, kPortraitSize),
                                            kArtWidth, kArtHeight, EBitmapDecode::Overlay4444);
    Renderer->DrawBitmapToTarget(ring, layout.ringCenterX - ring->width / 2, kRingCenterY - ring->height / 2,
                                 kArtWidth, kArtHeight);
    for (size_t bar = kNumBars; bar-- > 0;)
        Renderer->DrawBitmapToTarget(icons[bar], layout.iconX, kIconTop[bar], kArtWidth, kArtHeight);
    chip.art->EndPass();
    chip.composedArt = chip.artVersion;
}

void TPlyrStatusBar::ComposeCells(ESide side)
{
    SChip& chip = Chip(side);
    const SChipLayout& layout = kLayouts[size_t(side)];
    if (!chip.cells)
        chip.cells = std::make_unique<TSurface>(kCellsWidth, kCellsHeight, SG_PIXELFORMAT_RGBA8);

    chip.cells->StartPass(0.0f, 0.0f, 0.0f, 0.0f);
    for (size_t bar = 0; bar < kNumBars; ++bar)
        DrawTextShadowedToTarget(font, std::to_string(chip.text.values[bar]).c_str(), layout.valueX,
                                 kValueTop[bar], kValueWidth, kValueHeight, layout.valueAlign,
                                 1.0f, 1.0f, 1.0f, kCellsWidth, kCellsHeight);
    std::vector<std::string> lines;
    WrapTextLines(font, NameCell(chip.text.name, chip.text.level).c_str(), float(kNameSize), lines);
    for (size_t line = 0; line < lines.size(); ++line)
        DrawTextShadowedToTarget(font, lines[line].c_str(), layout.nameX, kNameTop + int32_t(line) * lineheight,
                                 kNameSize, lineheight, ETextAlign::Center, 1.0f, 1.0f, 1.0f,
                                 kCellsWidth, kCellsHeight);
    chip.cells->EndPass();
    chip.composedText = chip.textVersion;
}

// REVSYNC: 0x0054ab80 (slot 7): each chip's art at its fade, then its bars;
// then, after the overlay quads, the text cells of each chip past half fade
// (0x0054af20, slot 23). Between ticks a chip's fade eases toward the next
// tick's: the ramp is smooth and reads retail's at each tick.
void TPlyrStatusBar::Draw()
{
    if (!Renderer || !archive)
        return;
    const float frac = float(TTime::LegacyFrameFraction());
    for (ESide side : kSides)
        DrawChip(side, frac);
    for (ESide side : kSides)
        DrawCells(side);
}

void TPlyrStatusBar::DrawChip(ESide side, float frac) const
{
    const SChip& chip = Chip(side);
    if (!chip.present || !chip.art)
        return;
    const float alpha = FadeAlpha(StepTowardPerTick(float(chip.fade), float(chip.fadeGoal),
                                                    frac * TTime::LegacyFrameSeconds));
    if (alpha <= 0.0f)
        return;
    Renderer->DrawSurfaceTinted(chip.art.get(), GetPosX() + FromEdge(side, kLayouts[size_t(side)].artEdge),
                                GetPosY() + kArtTop, 1.0f, 1.0f, 1.0f, alpha);
    for (size_t bar = 0; bar < kNumBars; ++bar)
        DrawBar(side, bar, alpha);
}

void TPlyrStatusBar::DrawCells(ESide side) const
{
    const SChip& chip = Chip(side);
    if (!chip.present || !chip.cells || !TextShown(chip.fade))
        return;
    Renderer->DrawSurface(chip.cells.get(), GetPosX() + FromEdge(side, kLayouts[size_t(side)].cellsEdge),
                          GetPosY());
}

// REVSYNC: the bar kernel FUN_0054a5d0, overlay branch. The value (clamped to
// 0..maximum, the maximum at least 1) fills length * value / maximum texels:
// from the left on the player's chip, from the right on the opponent's. The
// bar goes down as four slices. The middle splits where the fill ends; each
// cap is full or empty whole: the near one (where the fill starts) once the
// fill covers half of it, the far one once the fill reaches it.
void TPlyrStatusBar::DrawBar(ESide side, size_t bar, float alpha) const
{
    const SBarArt& art = kBarArt[bar];
    const SStat& stat = Chip(side).stats[bar];
    const int32_t maximum = (std::max)(stat.maximum, 1);
    const int32_t filled = std::clamp(stat.value, 0, maximum) * art.length / maximum;
    const int32_t middle = art.length - 2 * art.cap;
    const int32_t litMiddle = std::clamp(filled - art.cap, 0, middle);
    const bool nearLit = filled >= art.cap / 2;
    const bool farLit = filled >= art.length - art.cap;

    const bool rightward = side == ESide::Player;
    const int32_t left = GetPosX() + (rightward ? kBarInset : GetWidth() - kBarInset - art.width);
    const int32_t top = GetPosY() + art.top;
    const auto slice = [&](int32_t offset, int32_t width, bool full) {
        if (width > 0)
            Renderer->DrawBitmapSubrectTinted(bars, left + offset, top, kBarsColumn + offset,
                                              full ? art.fullRow : art.emptyRow, width, art.height,
                                              1.0f, 1.0f, 1.0f, alpha);
    };
    const int32_t firstRun = rightward ? litMiddle : middle - litMiddle;
    slice(0, art.cap, rightward ? nearLit : farLit);
    slice(art.cap, firstRun, rightward);
    slice(art.cap + firstRun, middle - firstRun, !rightward);
    slice(art.length - art.cap, art.width - art.length + art.cap, rightward ? farLit : nearLit);
}

int32_t TPlyrStatusBar::FromEdge(ESide side, int32_t edge) const
{
    return side == ESide::Player ? edge : GetWidth() - edge;
}
