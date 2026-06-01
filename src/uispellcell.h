// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uispellcell.h - TSpellIconSlot: shared spell-icon cell control        *
// *************************************************************************
//
// ONE per-cell sub-control covering every spell-icon slot in the HUD —
// exactly analogous to TInvSlot (src/invslot.h) which covers item slots.
//
// Used in TWO places:
//   - TQuickSpellPane (bottom-bar 4-slot ring strip) — each ring = one
//     TSpellIconSlot. The ring sprite is stamped UNDER the 40×40 icon.
//   - Spell-info scroll header (uiscrollpanetest.cpp) — when viewing a
//     spell-scroll, the top-left of the body area shows one TSpellIconSlot
//     as a "this is the spell" thumbnail without the ring overlay.
//
// What each cell draws (compose-to-target contract):
//   1. (optional) Ring base sprite (RingU/D/G from SpellIcons.dat, 48×48,
//      BM_16BIT + BM_ALPHA) centered on the cell. Ring is BELOW the icon.
//   2. 40×40 spell-circle icon from SpellIcons.dat, keyed by spell name,
//      centered in the cell. Alpha-blended (BM_ALPHA).
//   3. (optional) Disabled overlay: the ring is replaced by RingG.
//   4. (optional) Pressed state: ring is replaced by RingD.
//
// The cell does NOT draw the spell name text — that is the caller's
// responsibility (matching TInvSlot which doesn't draw item-name text).
//
// Drag/drop support:
//   - The cell can act as a DRAG SOURCE (left-click-down starts a drag
//     with EDragSource::SpellPane and the spell index).
//   - The cell can act as a DROP TARGET for EDragSource::SpellPane (only).
//     On drop, it calls the bound `onDrop(slotIdx, spellIdx)` callback.
//
// Coordination note (Agent B — uidragstate.{h,cpp}):
//   EDragSource::SpellPane = 4 is already defined in uidragstate.h
//   (added in an earlier pass). No additional changes needed there.
//   However the drag-source item reference (`SUIDragState::item`) holds
//   a `TObjectInstance*` — for spell slots we store the spell index as
//   a reinterpret_cast<TObjectInstance*>(intptr_t(spellIdx)) sentinel.
//   This is a harness-only hack; the production path will store a proper
//   TSpell* (or equivalent) when TPlayScreen routes the drag.
//
// Coordination note (Agent A — hudstate.{h,cpp}):
//   For persistent quickspell bindings (survive save/load), add:
//     char quickspellBindings[4][64];  // spell name per slot, empty = none
//   to SHudState. The TSpellIconSlot writes/reads from there in production.
//   In the test harness we use a local binding array.
//
// *************************************************************************

#pragma once

#include "bitmap.h"   // PTBitmap
#include "surface.h"  // TSurface (fwd reference via compiler's knowledge)

#include <cstdint>
#include <functional>

// Forward-declare to keep the header minimal.
class TMulti;
struct SFontAtlas;

// State flags — mirrors the retail `mbr_0x14` bits for TQuickSpellPane.
struct SSpellCellState
{
    bool pressed  = false;   // ring shows RingD
    bool disabled = false;   // ring shows RingG (slot uncastable / empty)
    bool selected = false;   // highlight ring (future selection state)
};

// Callback type for drop events.
using FOnSpellDrop = std::function<void(int32_t destSlot, int32_t srcSpellIdx)>;

// -----------------------------------------------------------------------
// TSpellIconSlot — the per-cell spell-icon sub-control.
// -----------------------------------------------------------------------
class TSpellIconSlot
{
public:
    // Construct a spell-icon slot. The slot owns its position in the
    // render-target (pane-local x, y). ringW/ringH is the ring bitmap
    // size (48×48 for QuickSpell ring); iconW/iconH the inner icon size
    // (40×40). The ring may be nullptr if the slot is ring-less (spell
    // scroll header variant).
    TSpellIconSlot(int32_t x, int32_t y,
                   int32_t ringW, int32_t ringH,
                   int32_t iconW, int32_t iconH)
        : x_(x), y_(y)
        , ringW_(ringW), ringH_(ringH)
        , iconW_(iconW), iconH_(iconH)
    {}

    // --- binding -------------------------------------------------------

    // Set the ring sprites (U=base, D=pressed, G=disabled). Pass nullptr
    // to omit the ring layer entirely (ring-less variant for scroll header).
    void SetRingSprites(PTBitmap ringU, PTBitmap ringD, PTBitmap ringG)
    {
        ringU_ = ringU; ringD_ = ringD; ringG_ = ringG;
    }

    // Bind a spell to this slot. iconBm is the 40×40 spell-circle icon
    // (resolved from SpellIcons.dat by name); spellIdx is the harness
    // spell-index (used as drag payload). spellName is kept for logging.
    void SetSpell(PTBitmap iconBm, int32_t spellIdx, const char* spellName)
    {
        icon_     = iconBm;
        spellIdx_ = spellIdx;
        spellName_ = spellName;
        state_.disabled = (iconBm == nullptr);
    }

    // Clear the spell binding (empty slot).
    void ClearSpell()
    {
        icon_      = nullptr;
        spellIdx_  = -1;
        spellName_ = nullptr;
        state_.disabled = true;
    }

    // Set the callback invoked when a spell is dropped onto this slot.
    void SetOnDrop(FOnSpellDrop cb) { onDrop_ = cb; }

    // --- state -------------------------------------------------------

    SSpellCellState& State()       { return state_; }
    const SSpellCellState& State() const { return state_; }

    // --- draw --------------------------------------------------------

    // Draw into the currently-active render-target pass (compose-to-target
    // contract). tw/th are the render-target dimensions for bounds clipping.
    void Draw(int32_t tw, int32_t th) const;

    // --- hit-test + drag/drop ----------------------------------------

    // Returns true if (mx, my) falls within the cell's click rect
    // (the inner icon area, not the full ring bounds).
    bool HitTest(int32_t mx, int32_t my) const;

    // Handle a mouse-down event at (mx, my). Starts a drag if the cell
    // has a bound spell and the coordinates hit the cell.
    // Returns true if the event was consumed.
    bool HandleMouseDown(int32_t mx, int32_t my);

    // Handle a mouse-up event at (mx, my). Completes a drop if there is
    // an active SpellPane drag and the coordinates hit the cell.
    // Returns true if a drop was committed.
    bool HandleMouseUp(int32_t mx, int32_t my);

    // --- accessors ---------------------------------------------------

    int32_t     X()          const { return x_; }
    int32_t     Y()          const { return y_; }
    int32_t     SpellIndex() const { return spellIdx_; }
    const char* SpellName()  const { return spellName_; }
    PTBitmap    Icon()       const { return icon_; }

private:
    int32_t     x_       = 0;
    int32_t     y_       = 0;
    int32_t     ringW_   = 48;
    int32_t     ringH_   = 48;
    int32_t     iconW_   = 40;
    int32_t     iconH_   = 40;

    PTBitmap    ringU_    = nullptr;
    PTBitmap    ringD_    = nullptr;
    PTBitmap    ringG_    = nullptr;
    PTBitmap    icon_     = nullptr;

    int32_t     spellIdx_  = -1;
    const char* spellName_ = nullptr;

    SSpellCellState  state_;
    FOnSpellDrop     onDrop_;
};
