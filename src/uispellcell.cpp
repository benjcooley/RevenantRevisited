// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uispellcell.cpp - TSpellIconSlot implementation                      *
// *************************************************************************
//
// See uispellcell.h for the full contract.
//
// Draw order (compose-to-target contract) — retail confirmed via recon:
//   The Ring sprites (RingU/D/G, flags=0x104 BM_16BIT+BM_ALPHA) have a
//   TRANSPARENT center hole. The spell icon sits BELOW the ring and shows
//   through the transparent center. So the order is:
//   1. 40×40 spell-circle icon, centered within the ring area. The icon's own
//      bitmap art includes a dark circular background painted in — the dark
//      area IS the disc that shows inside the ring well. Skipped if null.
//   2. Ring sprite (RingU/D/G) drawn ON TOP at (x_, y_). The transparent inner
//      hole reveals the icon art underneath; the golden outer frame overlaps
//      the icon edges (giving the "icon inside the ring" look).
//      Skipped if all ring ptrs null.
//
// Retail source: cls_0x5a3c68::virt_meth_0x42cc30:628-631 stamps the ring
// sprite at (mbr_0x60, mbr_0x64) via FUN_004bd680. The spell icon stamp is
// UNCONFIRMED-D in QuickSpellPane_SPEC.md §4 — assumed to happen before the
// ring stamp in the per-tick slot-20 (0x5444a0) body, which is not yet
// extracted. Visual verification against retail (sample_screen_1.jpg) confirms
// icon is BELOW the ring (golden ring frame overlaps icon edges).
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
    // --- (1) Spell-circle icon FIRST (below the ring) ---
    // The ring has a transparent center; the icon shows through it.
    // Offset from ring TL: (ringW_ - iconW_) / 2 = (48-40)/2 = 4 for the
    // retail 48×48 ring + 40×40 icon combination.
    // Retail cite: QuickSpellPane_SPEC.md §4 UNCONFIRMED-D: "ring-TL
    // (x_i + 4, y_i + 4) to seat a 40×40 icon inside the 48×48 ring".
    if (icon_)
    {
        const int32_t iconOffX = (ringW_ - iconW_) / 2;   // = 4 for 48/40
        const int32_t iconOffY = (ringH_ - iconH_) / 2;   // = 4 for 48/40
        Renderer->DrawBitmapToTarget(
            icon_,
            x_ + iconOffX, y_ + iconOffY,
            tw, th);
    }

    // --- (2) Ring sprite ON TOP (transparent center reveals icon) ---
    // Choose which ring based on state (disabled → G, pressed → D, else U).
    // Retail cite: TButton paint cls_0x5a3c68.cpp:628-631 stamps ring at
    // (mbr_0x60, mbr_0x64) = (x_, y_) via FUN_004bd680.
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
        Renderer->DrawBitmapToTarget(ringBm, x_, y_, tw, th);
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
