// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uispellcell.cpp - TSpellIconSlot implementation                      *
// *************************************************************************
//
// See uispellcell.h for the full contract.
//
// Draw order (compose-to-target contract):
//   1. Ring base sprite (RingU/D/G) centered on (x_, y_) at (ringW_×ringH_).
//      Alpha-blended (BM_16BIT + BM_ALPHA). Skipped if all ring ptrs null.
//   2. 40×40 spell-circle icon, centered within the ring area. Alpha-blended.
//      Skipped if icon_ null.
//
// Drag payload:
//   Drag source stores spellIdx_ as an intptr_t cast to TObjectInstance*.
//   This is a harness-only sentinel; production code will store a real spell
//   reference when TPlayScreen routes the drag.
// *************************************************************************

#include "uispellcell.h"
#include "renderer.h"
#include "uidragstate.h"

#include <cstdint>

// -----------------------------------------------------------------------
// Draw
// -----------------------------------------------------------------------
void TSpellIconSlot::Draw(int32_t tw, int32_t th) const
{
    // --- (1) Ring sprite --- drawn UNDER the icon ---
    // Choose which ring based on state (disabled → G, pressed → D, else U).
    PTBitmap ringBm = nullptr;
    if (ringU_ || ringD_ || ringG_)
    {
        if (state_.disabled && ringG_)
            ringBm = ringG_;
        else if (state_.pressed && ringD_)
            ringBm = ringD_;
        else
            ringBm = ringU_;
    }

    if (ringBm)
    {
        // Center the ring at cell (x_, y_). The ring fills ringW_×ringH_.
        Renderer->DrawBitmapToTarget(ringBm, x_, y_, tw, th);
    }

    // --- (2) Spell-circle icon --- centered inside the ring ---
    if (icon_)
    {
        // Center the iconW_×iconH_ icon within the ringW_×ringH_ area.
        // (ringW_ - iconW_) / 2 gives the horizontal offset from ring TL.
        const int32_t iconOffX = (ringW_ - iconW_) / 2;
        const int32_t iconOffY = (ringH_ - iconH_) / 2;
        Renderer->DrawBitmapToTarget(
            icon_,
            x_ + iconOffX, y_ + iconOffY,
            tw, th);
    }
}

// -----------------------------------------------------------------------
// HitTest — returns true if (mx, my) is within the icon's click area.
// The retail TQuickSpellPane uses a 32×32 inner click rect (cls_0x5a3ab8
// ctor w=h=0x20). We use the icon bounds as the hit area.
// -----------------------------------------------------------------------
bool TSpellIconSlot::HitTest(int32_t mx, int32_t my) const
{
    const int32_t iconOffX = (ringW_ - iconW_) / 2;
    const int32_t iconOffY = (ringH_ - iconH_) / 2;
    const int32_t ix = x_ + iconOffX;
    const int32_t iy = y_ + iconOffY;
    return (mx >= ix && mx < ix + iconW_ &&
            my >= iy && my < iy + iconH_);
}

// -----------------------------------------------------------------------
// HandleMouseDown — start a spell drag from this slot.
// -----------------------------------------------------------------------
bool TSpellIconSlot::HandleMouseDown(int32_t mx, int32_t my)
{
    if (!HitTest(mx, my)) return false;
    if (spellIdx_ < 0 || !icon_) return false;

    // Start a SpellPane drag. The item pointer carries the spell index as
    // a raw integer cast — harness-only sentinel (see header note).
    // Coordination: EDragSource::SpellPane = 4 is already in uidragstate.h
    // (no Agent-B change needed).
    UIDragState::BeginDrag(
        EDragSource::SpellPane,
        spellIdx_,
        reinterpret_cast<TObjectInstance*>(static_cast<intptr_t>(spellIdx_)),
        mx, my);

    state_.pressed = true;
    return true;
}

// -----------------------------------------------------------------------
// HandleMouseUp — complete a drop onto this slot.
// -----------------------------------------------------------------------
bool TSpellIconSlot::HandleMouseUp(int32_t mx, int32_t my)
{
    state_.pressed = false;

    if (!UIDragState::IsActive()) return false;
    const SUIDragState& drag = UIDragState::Get();
    if (drag.source != EDragSource::SpellPane) return false;
    if (!HitTest(mx, my)) return false;

    const int32_t srcIdx = static_cast<int32_t>(
        reinterpret_cast<intptr_t>(drag.item));

    UIDragState::CompleteDrag(EDragSource::SpellPane, spellIdx_, true);

    if (onDrop_)
        onDrop_(spellIdx_, srcIdx);

    return true;
}
