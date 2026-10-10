// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      SpellPane.h - Spell Pane                         *
// *************************************************************************

#pragma once

#include "revenant.h"
#include "screen.h"
#include "object.h"
#include "button.h"
#include "spell.h"

#include <memory>
#include <string>

class TMulti;
struct SFontAtlas;

extern char *Talismans[];
extern char *Old[];

// *******************
// * TTalismanButton *
// *******************

_CLASSDEF(TTalismanButton)
class TTalismanButton : public TButton
{
  public:
    TTalismanButton(char *bname, int32_t bx, int32_t by, int32_t bw, int32_t bh, uint16_t keypr,
            void (*bfunc)(), PTBitmap dbm = nullptr, PTBitmap ubm = nullptr,
            bool rad = false, bool tog = false, bool notsquare = false,
            int32_t qspellid = -1, int32_t xoff = 0) :
        TButton(bname, bx, by, bw, bh, keypr, bfunc, dbm, ubm, rad, tog, notsquare, -1, 0)
            { quickspellid = qspellid; xoffset = xoff; Clear(); }

    void Compose(int32_t target_w, int32_t target_h) override;
        // The button, then the talismans of its spell

    void AddTalisman(char t);
        // Add a talisman to the button
    void Backspace();
        // Backspace a single talsiman
    void Clear();
        // Clear all talismans
    void Invoke();
        // Invoke the spell on the button
    char *GetSpell();
        // get the talismans for this button
    void SetSpell(char *talismans);
        // Get the talismans for this button
    bool HasTalismans();
        // Checks to see if player has talismans for this spell

  protected:
    int32_t quickspellid;               // Player quickspell for this button (-1 is no player spell)
    int32_t xoffset;                    // Offset 
};

// **************
// * TSpellPane *
// **************

// This pane is where the user assembles spells with various talismans and invokes them.

_CLASSDEF(TSpellPane)
class TSpellPane : public TButtonPane
{
  public:
    TSpellPane() : TButtonPane(MULTIPANEX, MULTIPANEY, MULTIPANEWIDTH, MULTIPANEHEIGHT) {}
    ~TSpellPane() {}

    virtual bool Initialize();
    virtual void DrawBackground();
    virtual void MouseClick(int32_t button, int32_t x, int32_t y);
    virtual void MouseMove(int32_t button, int32_t x, int32_t y);

    void Scroll(int32_t numlines);
        // Scroll the pane's contents

    void ToggleTalismanNames();
        // Change between showing the english name and just the symbol
    bool ShowTalismanNames() { return shownames; }
        // Whether to show the name of the talisman next to its icon

    bool AddTal(int32_t tal);
        // Add a talisman to the current spell
    bool RemoveTal(int32_t numtals = 1);
        // Backspace numtals of talismans

    void Invoke();
        // Invoke the spell in the button

    char *GetSpell();
        // get the spell info

  private:
    int32_t OnTal(int32_t x, int32_t y);
        // Which talisman mouse is on

    bool shownames;                     // expand names
    int32_t startline;                      // for scrolling

    int32_t clickedtal;                     // which talisman is currently clicked
    bool onclickedtal;                  // whether mouse arrow is still on the clicked talisman
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
    TBitmap *greybitmap = nullptr;
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
//
// Not yet ported: dropping a spell dragged from the spellbook onto a ring
// (MouseClick 0x00544890 sets that quick spell). It arrives with the
// spellbook pane, which owns the drag (HUD_REBUILD P4).
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

    std::unique_ptr<TMulti> icons;                  // SpellIcons.dat (retail DAT_0065bc3c)
};
