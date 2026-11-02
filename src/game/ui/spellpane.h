// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      SpellPane.h - Spell Pane                         *
// *************************************************************************

#pragma once

#include "game/ui/button.h"

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class TMulti;
struct SFontAtlas;
struct SSpellData;

// The talismans, in TalismanClass type order, as retail's spell panes draw
// them: each one's code and art (the name tables at 0x005e4f80 and
// 0x005e4fb4). A modded class with more types shows only these.
inline constexpr int32_t kNumTalismanTypes = 12;
// REVSYNC: 0x005e4f80 -- the talismans' art names, in type order: GameData's
// "<name>" icon and "<name>candy" plate, the spell panes' "S<name>" glyph
// (0x005e4fb4).
inline constexpr const char *kTalismanArtNames[kNumTalismanTypes] = {
    "Sun", "Life", "Ocean", "Law", "Soul", "Stars", "Death", "Chaos", "Sky", "Earth", "Ward", "Moon",
};

// SpellIcons.dat (retail DAT_0065bc3c, which TPlayScreen loads once): the
// spells' circles and the rings, shared by the panes that draw them for as
// long as any holds it. Null if the file is missing.
[[nodiscard]] std::shared_ptr<TMulti> AcquireSpellIcons();
// A spell's circle in SpellIcons.dat: its ICONNAME, else "Aura"
// (0x005e50bc / 0x005e52ec).
[[nodiscard]] TBitmap *SpellCircle(TMulti &icons, const SSpellData &spell);
struct STalismanArt
{
    char code = 0;                  // its character in a spell (the type's Code stat)
    TBitmap *plate = nullptr;       // GameData "<name>candy"
    TBitmap *icon = nullptr;        // GameData "<name>"
    TBitmap *glyph = nullptr;       // SpellPane.dat "S<name>", on the talisman button
};
using TTalismanArt = std::array<STalismanArt, kNumTalismanTypes>;
static_assert(kNumTalismanTypes <= 32, "TSpellPane keeps the held types a bit each");

// *******************
// * TTalismanButton *
// *******************

// REVSYNC: retail's talisman button (vtable 0x005b9bf0; draw 0x005424a0): a
// button that shows a spell as its talismans' glyphs, the spell pane's
// "spell" button. Its pane binds the spell to it (SetSpell).
class TTalismanButton final : public TButton
{
  public:
    TTalismanButton(const char *bname, int32_t bx, int32_t by, int32_t bw, int32_t bh, void (*bfunc)(),
                    TBitmap *dbm, TBitmap *ubm, const TTalismanArt *talismanart, int32_t glyphleft);

    // The talisman codes shown; marks the button dirty when they change.
    void SetSpell(const std::string &talismans);
    void Compose(int32_t target_w, int32_t target_h) override;

  private:
    const TTalismanArt *art = nullptr;
    int32_t glyphleft = 0;          // retail +0x14c
    std::string spell;
};

// **************
// * TSpellPane *
// **************

// REVSYNC: TSpellPane @ 0x006661b0 (vtable 0x005a5978; docs/ui/
// HUD_REBUILD.md §6c). The side panel's spell pane, in its lower slot, where
// the player puts a spell together from the talismans in the spell pouch
// and casts it: the pouch's talismans (with their names, or packed without),
// the spell being put together -- the player's construct quick spell -- on
// the "spell" button, which casts it, and buttons to drop its last
// talisman, page the list and toggle the names.
//
class TSpellPane final : public TButtonPane
{
  public:
    static constexpr int32_t kWidth = 0xbc;
    static constexpr int32_t kHeight = 0xae;

    TSpellPane();
    ~TSpellPane() override;
    TSpellPane(const TSpellPane&) = delete;
    TSpellPane& operator=(const TSpellPane&) = delete;

    bool Initialize() override;     // 0x005432a0
    void Close() override;
    void Pulse() override;          // binds the pouch's talismans and the construct spell
    void MouseClick(int32_t button, int32_t x, int32_t y) override;    // 0x00543b40
    void MouseMove(int32_t button, int32_t x, int32_t y) override;     // 0x00543c40

    // The buttons' functions.
    void Invoke();                  // "spell" (0x00542d50): cast the construct spell; the book learns it
    void RemoveTalisman();          // "back" (0x00542e30): drop its last talisman
    void Scroll(int32_t lines);     // "up" / "down" (0x00542f00 / 0x00542ed0): two talismans a line
    void ToggleNames();             // "min" (0x00542f30)

    // Adds a talisman of TalismanClass type `type` to the construct spell
    // (0x00542640), unless it is full or the player holds no more of it.
    void AddTalisman(int32_t type);

    [[nodiscard]] bool ShowNames() const { return shownames; }
    [[nodiscard]] int32_t FirstLine() const { return firstline; }
    // The talisman type under (x, y) in the pane, or -1 (0x00543ca0).
    [[nodiscard]] int32_t TalismanAt(int32_t x, int32_t y) const;
    // The talisman type pressed and still under the pointer, or -1.
    [[nodiscard]] int32_t PressedTalisman() const { return onpressed ? pressed : -1; }

  protected:
    void ComposeBackground(int32_t target_w, int32_t target_h) override;    // 0x005435f0

  private:
    struct STalisman
    {
        int32_t type = -1;
        std::string name;           // as the game shows it
    };

    // Each talisman shown, at its place in the pane, in the order the draw
    // and the hit test lay them out.
    template <typename Fn> void ForEachShown(Fn &&fn) const;
    void RefreshButtons();          // the arrows and the names art, as 0x005435f0 sets them
    [[nodiscard]] TTalismanButton *SpellButton();
    [[nodiscard]] std::string ConstructSpell() const;

    std::unique_ptr<TMulti> archive;            // SpellPane.dat (retail DAT_0065a574)
    TBitmap *background = nullptr;
    const SFontAtlas *namefont = nullptr;       // "Small"
    TTalismanArt art;
    std::vector<STalisman> talismans;           // the pouch's, in type order
    uint32_t heldtypes = 0;                     // which types those are, a bit each
    bool shownames = true;                      // retail +0x17c
    int32_t firstline = 0;                      // +0x180: the first talisman listed, with names
    int32_t pressed = -1;                       // +0x184: the talisman type pressed
    bool onpressed = false;                     // +0x188: the pointer still on it
};

// *********************
// * TQuickSpellButton *
// *********************

// REVSYNC: retail's quick-spell ring (vtable 0x005b9c54; draw 0x00542900).
// The spell bound to the ring shows as its circle from SpellIcons.dat inside
// the ring, with its name above and below; the ring is grey while the
// player can't cast it, and the ring and circle sink a pixel while pressed.
class TQuickSpellButton final : public TButton
{
  public:
    TQuickSpellButton(const char *bname, int32_t bx, int32_t by, int32_t bw, int32_t bh, void (*bfunc)(),
                      TBitmap *ringdown, TBitmap *ringup, TBitmap *ringgrey,
                      const SFontAtlas *labelfont, int32_t labelline);

    // What the ring shows: the spell's circle (null: none), the two parts of
    // its label, and whether the player can cast it (else the grey ring).
    // Marks the ring dirty when any of it changes.
    void SetSpell(TBitmap *circle, std::string top, std::string bottom, bool castable);
    void Compose(int32_t target_w, int32_t target_h) override;

  private:
    const SFontAtlas *font = nullptr;
    int32_t lineheight = 0;
    TBitmap *icon = nullptr;
    std::string toplabel;
    std::string bottomlabel;
    bool castable = false;
};

// *******************
// * TQuickSpellPane *
// *******************

// REVSYNC: TQuickSpellPane @ 0x0065c6f8 (vtable 0x005a5a30; docs/ui/
// HUD_REBUILD.md §6a). The four quick-spell rings at the left of the bottom
// bar: rings 1..4 show the player's quick spells (TPlayer::GetQuickSpell),
// and a click casts one. It shares the bottom bar's rect and draws over it
// (TBottomBarPane, its parent).
// A spell dragged from the spell book and let go on a ring goes on it.
_CLASSDEF(TQuickSpellPane)
class TQuickSpellPane final : public TButtonPane
{
  public:
    static constexpr int32_t kNumRings = 4;         // quick spells QSPELL_1..QSPELL_4

    TQuickSpellPane();
    ~TQuickSpellPane() override;
    TQuickSpellPane(const TQuickSpellPane&) = delete;
    TQuickSpellPane& operator=(const TQuickSpellPane&) = delete;

    bool Initialize() override;     // 0x00544160
    void Close() override;
    void Pulse() override;          // each ring's spell and state, as 0x005444c0 sets them
    void MouseClick(int32_t button, int32_t x, int32_t y) override;    // 0x00544890: a spell dropped on a ring

    void Invoke(int32_t ring);
        // Cast the spell on ring 1..4 (the rings' functions, 0x005440a0 ...)

  private:
    [[nodiscard]] TQuickSpellButton *Ring(int32_t ring);
    [[nodiscard]] int32_t RingAt(int32_t x, int32_t y);    // 1..4, or 0 off the rings

    std::shared_ptr<TMulti> icons;                  // SpellIcons.dat (AcquireSpellIcons)
};
