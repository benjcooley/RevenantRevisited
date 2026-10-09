// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *            statusbar.h - The player and target status bar             *
// *************************************************************************

#pragma once

#include "saferef.h"
#include "screen.h"

#include <array>
#include <cstdint>
#include <memory>
#include <string>

class TBitmap;
class TCharacter;
class TMulti;
class TSurface;
struct SFontAtlas;

// REVSYNC: TPlyrStatusBar @ 0x0065a8c0 (vtable 0x005a54e4; docs/ui/
// HUD_REBUILD.md, docs/ui/forensics/RETAIL_HUD_CODE_MAP.md). The strip along
// the top of the map view. The player's chip sits at its left edge: the
// portrait in its ring, the health, mana and fatigue bars with their values,
// and the name and level. While the player fights, the opponent's chip is
// mirrored at the right edge. Each chip fades in over six ticks as its
// character appears and out as it goes; the text shows past half fade.
//
// The 1998 pane was two fluid tubes (THealthBar, TStaminaBar) that gameplay
// pushed levels into. Retail replaced them with this pane, which reads the
// characters itself.
//
// Retail composes each chip's art into a texture when its character
// changes, draws the art and the bar slices as quads at the chip's fade,
// then blits the text cells over them. The port draws the same layers from
// the same assets at the same places: the art and the text as cached
// layers, the bars as sprites. tools/retail_ab/hud_ab.py holds it to
// retail's own code, run in the emulator.
//
// Not ported: the NoTexOverlay path (StatusBarNoTex.dat, every layer blitted
// in 2D), which retail took only on cards it listed as broken, and the
// multiplayer readout ("%s\nLv:%d P:%d M:%d", STATBARDMFMT, 0x0054ae10).
class TPlyrStatusBar final : public TPane
{
  public:
    TPlyrStatusBar() = default;
    ~TPlyrStatusBar() override;
    TPlyrStatusBar(const TPlyrStatusBar&) = delete;
    TPlyrStatusBar& operator=(const TPlyrStatusBar&) = delete;

    bool Initialize() override;     // 0x00549740 (slot 0)
    void Close() override;          // 0x00549d40 (slot 1)
    void Pulse() override;          // 0x00549da0 (slot 19)
    void Compose() override;        // 0x00549e60 (slot 20); the text cells of 0x0054af20
    void Draw() override;           // 0x0054ab80 (slot 7); the cell blits of 0x0054af20 (slot 23)

    // The bars, in retail's order: health, mana, fatigue.
    static constexpr size_t kNumBars = 3;

  private:
    enum class ESide : uint8_t { Player, Target };
    static constexpr ESide kSides[] = { ESide::Player, ESide::Target };

    struct SStat
    {
        int32_t value = 0;
        int32_t maximum = 0;
    };

    // What a chip's text cells say: the bars' values and the name cell's
    // makings. Compose formats them.
    struct SText
    {
        std::array<int32_t, kNumBars> values{};
        std::string name;                   // the player's name; the opponent's display name
        int32_t level = -1;                 // a player's level; -1 for anyone else (the bare name)

        bool operator==(const SText& other) const
            { return values == other.values && level == other.level && name == other.name; }
        bool operator!=(const SText& other) const { return !(*this == other); }
    };

    // One chip. Pulse reads its character into the model each tick; Compose
    // rebuilds the layers when the model's versions move on; Draw submits.
    struct SChip
    {
        TSafeRef<TCharacter> character;     // the player; the opponent, or the last one as it fades out
        bool present = false;               // `character` was alive at the last tick
        int32_t fade = 0;                   // 0..kFadeSteps: retail +0xd4 (player), +0xdc (target)
        int32_t fadeGoal = 0;               // +0xd8 / +0xe0
        std::array<SStat, kNumBars> stats{};
        TBitmap* portrait = nullptr;        // the character's InventoryImage (vtable +0x130)
        SText text;
        uint32_t artVersion = 1;            // bumped when the character or its portrait changes
        uint32_t textVersion = 1;           // bumped when `text` changes

        std::unique_ptr<TSurface> art;      // retail +0x70 (player) / +0x74 (target)
        std::unique_ptr<TSurface> cells;    // the chip's cells of retail's text surface (+0x64)
        uint32_t composedArt = 0;
        uint32_t composedText = 0;
    };

    void LayOut();
    void ReadPlayer(TCharacter* player);
    void ReadTarget(TCharacter* player);
    void ReadCharacter(ESide side, TCharacter* character, bool readText);
    void ComposeArt(ESide side);
    void ComposeCells(ESide side);
    void DrawChip(ESide side, float frac) const;
    void DrawCells(ESide side) const;
    void DrawBar(ESide side, size_t bar, float alpha) const;
    [[nodiscard]] int32_t FromEdge(ESide side, int32_t edge) const;
    [[nodiscard]] SChip& Chip(ESide side) { return chips[size_t(side)]; }
    [[nodiscard]] const SChip& Chip(ESide side) const { return chips[size_t(side)]; }

    std::array<SChip, std::size(kSides)> chips{};

    std::unique_ptr<TMulti> archive;                   // StatusBar.dat (retail DAT_0065a9d0)
    TBitmap* bars = nullptr;
    TBitmap* backpanel = nullptr;
    TBitmap* ring = nullptr;
    std::array<TBitmap*, kNumBars> icons{};            // health, mana, fatigue
    const SFontAtlas* font = nullptr;                  // "Small" (DAT_0065abc4)
    int32_t lineheight = 0;                            // the font's GDI line height
};
