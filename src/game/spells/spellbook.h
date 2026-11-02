// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  spellbook.h - the side panel's spell book                            *
// *************************************************************************

#pragma once

#include "game/ui/button.h"
#include "game/ui/spellpane.h"      // kNumTalismanTypes

#include <array>
#include <cstdint>
#include <memory>
#include <vector>

class TMulti;
class TPlayer;
struct SFontAtlas;

// REVSYNC: TSpellBookPane @ 0x0065a9d8 (vtable 0x005a5ae8; docs/ui/
// forensics/SpellBookPane_RETAIL.md, docs/ui/HUD_REBUILD.md §6e). The side
// panel's spell book, in its upper slot: the spells the main player has
// learned (TPlayer::KnownSpell), in the order learned, down a scroll of
// parchment. A row shows the spell's circle in its ring, its name, its
// description, the skill and mana it takes and its talismans; the ring is
// lit while the player holds the talismans and has the mana.
//
// A click on a circle casts its spell; dragging a circle takes the spell to
// a quick-spell ring (TQuickSpellPane); dragging the parchment, or the
// arrows, scroll the book. Casting a combination in the spell pane teaches
// it (Reveal).
//
// The scroll moves toward its target at most 40 px a pulse, and not again
// until the book has been composed. Retail repainted only the strip a step
// exposed, so rings elsewhere went stale; the port recomposes the whole
// window, and a ring that changes recomposes it.
class TSpellBookPane final : public TButtonPane
{
  public:
    static constexpr int32_t kWidth = 0xbc;
    static constexpr int32_t kHeight = 0x132;

    TSpellBookPane();
    ~TSpellBookPane() override;
    TSpellBookPane(const TSpellBookPane&) = delete;
    TSpellBookPane& operator=(const TSpellBookPane&) = delete;

    bool Initialize() override;     // 0x005449e0
    void Close() override;          // 0x00544e40
    void Pulse() override;          // 0x00546620: the scroll's step; the rings
    void MouseClick(int32_t button, int32_t x, int32_t y) override;    // 0x00545f10
    void MouseMove(int32_t button, int32_t x, int32_t y) override;     // 0x00546390

    // REVSYNC: 0x00546830 -- each known spell's row, its height from its
    // description; a code spell.def doesn't know is forgotten. Retail runs
    // it as a player becomes main (TPlayerManager::SetMainPlayerNum
    // 0x0051f048) and after learning; the book runs it whenever the main
    // player or its list of spells changes (Pulse).
    void Rebuild();
    // REVSYNC: 0x00544fb0 -- a spell the player knows scrolls into view; a
    // spell.def spell with a circle and a description is learned first.
    void Reveal(const char *talismans);
    // Whether Reveal would teach this combination: a spell.def spell with a
    // circle and a description (0x00544fb0's test).
    [[nodiscard]] static bool Learnable(const char *talismans);
    // REVSYNC: 0x00544f00 -- the arrows: the next (+1) or previous (-1)
    // row's top to the top of the window.
    void StepRow(int32_t direction);

    // The window onto the rows: its top in px down them (0: the first row at
    // the top) and what is left of the scroll to it; the row whose circle is
    // pressed (-1: none); the rows' height.
    [[nodiscard]] int32_t ScrollPos() const { return scroll; }
    [[nodiscard]] int32_t PendingScroll() const { return pending; }
    [[nodiscard]] int32_t PressedRow() const { return pressed; }
    [[nodiscard]] int32_t ContentHeight() const { return total; }
    // The window at `pos` at once (clamped), as a finished scroll leaves it.
    void ScrollTo(int32_t pos);
    // The centre of row `row`'s circle in the pane, where a click casts it.
    [[nodiscard]] bool CirclePoint(int32_t row, int32_t& x, int32_t& y) const;

  protected:
    void ComposeBackground(int32_t target_w, int32_t target_h) override;    // 0x005452d0

  private:
    enum class ERing : uint8_t { Grey, Up, Down };

    [[nodiscard]] int32_t RowCount() const;
    [[nodiscard]] int32_t RowTop(int32_t row) const;
    [[nodiscard]] int32_t RowAtCircle(int32_t x, int32_t y) const;
    [[nodiscard]] int32_t RowHeight(const char *description) const;
    [[nodiscard]] int32_t ClampScroll(int32_t pos) const;
    [[nodiscard]] ERing RingOf(int32_t row) const;
    void ComposeRow(int32_t row, int32_t top, int32_t target_w, int32_t target_h);
    void Cast(int32_t row);
    void Release();

    std::unique_ptr<TMulti> archive;            // SpellScroll.dat (retail DAT_00666640)
    std::shared_ptr<TMulti> icons;              // SpellIcons.dat
    TBitmap *scrollart = nullptr;               // "Scroll" (+0x190)
    TBitmap *paper = nullptr;                   // "ScrollPaper" (+0x18c)
    TBitmap *captop = nullptr;                  // "ScrlTop" (+0x184)
    TBitmap *capbottom = nullptr;               // "ScrlBottom" (+0x188)
    std::array<TBitmap *, kNumTalismanTypes> glyphs{};      // "S<name>"
    std::array<char, kNumTalismanTypes> codes{};            // each type's Code
    const SFontAtlas *textfont = nullptr;       // "Small"
    const SFontAtlas *titlefont = nullptr;      // "SpellTitle" (retail DAT_00667c2c)

    const TPlayer *builtfor = nullptr;          // the rows are this player's list
    int32_t builtversion = -1;                  // at this version (TPlayer::KnownSpellsVersion)
    std::vector<int32_t> heights;               // each row's (+0x1a8)
    int32_t total = 0;                          // their sum (+0x1a0)
    int32_t scroll = 0;                         // +0x17c
    int32_t pending = 0;                        // the scroll still to come (+0x1d0)
    int32_t pressed = -1;                       // +0x1c8
    int32_t lasty = 0;                          // the pointer's last y (+0x194)
    bool steppable = true;                      // composed since the scroll last moved (+0x198 / +0x19c)
    std::vector<ERing> rings;                   // each row's ring as last composed
};

extern TSpellBookPane SpellBook;
