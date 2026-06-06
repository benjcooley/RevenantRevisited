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
//   1. 40×40 spell-circle icon. Retail spellbook stamps the spell bitmap and
//      ring at the same origin; the icon art is already inset inside its own
//      bitmap and includes the dark circular field.
//   2. Ring sprite (RingU/D/G) drawn ON TOP at (x_, y_). The transparent inner
//      hole reveals the icon art underneath; the golden outer frame overlaps
//      the icon edges (giving the "icon inside the ring" look).
//      Skipped if all ring ptrs null.
//
// Retail source: cls_0x5a3c68::virt_meth_0x42cc30:628-631 stamps the ring
// sprite at (mbr_0x60, mbr_0x64) via FUN_004bd680. The spellbook draw path
// stamps its spell icon and ring at the same computed origin, confirming the
// slot should not add an automatic +4/+4 inset.
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
    // Retail spellbook draws icon and ring at the same origin; callers can
    // override iconOff* only when an extracted path proves a different inset.
    if (icon_)
    {
        if (state_.disabled && disabledTint_)
        {
            Renderer->DrawBitmapSubrectTintedToTarget(
                icon_,
                x_ + iconOffX_, y_ + iconOffY_,
                0, 0, icon_->width, icon_->height,
                tw, th,
                disabledR_, disabledG_, disabledB_, disabledA_);
        }
        else
        {
            Renderer->DrawBitmapSubrectStretchedToTarget(
                icon_,
                x_ + iconOffX_, y_ + iconOffY_,
                iconW_, iconH_,
                0, 0, icon_->width, icon_->height,
                tw, th);
        }
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

    // --- (3) Optional label layer ---
    if (labelVisible_ && labelFont_)
    {
        if (labelLine1_ && labelLine1_[0])
            DrawTextShadowedToTarget(
                labelFont_, labelLine1_,
                x_ + labelX_, y_ + labelLine1Y_,
                labelW_, labelH_,
                labelAlign_,
                labelR_, labelG_, labelB_,
                tw, th);

        if (labelLine2_ && labelLine2_[0])
            DrawTextShadowedToTarget(
                labelFont_, labelLine2_,
                x_ + labelX_, y_ + labelLine2Y_,
                labelW_, labelH_,
                labelAlign_,
                labelR_, labelG_, labelB_,
                tw, th);
    }
}

// -----------------------------------------------------------------------
// HitTest — returns true if (mx, my) is within the cell's click area.
// QuickSpell overrides this to the retail 32x32 TButton rect. Spellbook
// cells use a 40x40 icon hit rect.
// -----------------------------------------------------------------------
bool TSpellIconSlot::HitTest(int32_t mx, int32_t my) const
{
    int32_t hx = hitX_;
    int32_t hy = hitY_;
    int32_t hw = hitW_;
    int32_t hh = hitH_;
    if (!customHitRect_)
    {
        hx = iconOffX_;
        hy = iconOffY_;
        hw = iconW_;
        hh = iconH_;
    }
    return (mx >= x_ + hx && mx < x_ + hx + hw &&
            my >= y_ + hy && my < y_ + hy + hh);
}

// -----------------------------------------------------------------------
// HandleMouseDown — start a spell drag from this slot.
// -----------------------------------------------------------------------
bool TSpellIconSlot::HandleMouseDown(int32_t mx, int32_t my)
{
    if (!HitTest(mx, my)) return false;
    if (spellIdx_ < 0 || !icon_) return false;

    const bool hasRing = (ringU_ || ringD_ || ringG_);
    const int32_t originX = hasRing ? x_ : x_ + iconOffX_;
    const int32_t originY = hasRing ? y_ : y_ + iconOffY_;

    SDragBitmapLayer layers[kMaxDragBitmapLayers] = {};
    int32_t layerCount = 0;

    // Preserve the same layer order as Draw(): icon/dark disk first, ring
    // on top. The dark disk appears to be baked into the spell icon art in
    // the current assets; if retail proves it is a separate bitmap, add it
    // before this icon layer.
    layers[layerCount++] = {
        icon_,
        hasRing ? iconOffX_ : 0,
        hasRing ? iconOffY_ : 0,
    };

    PTBitmap ringBm = nullptr;
    if (hasRing)
    {
        if (state_.disabled && ringG_)
            ringBm = ringG_;
        else if (ringD_)
            ringBm = ringD_;
        else
            ringBm = ringU_;
    }
    if (ringBm && layerCount < kMaxDragBitmapLayers)
        layers[layerCount++] = { ringBm, 0, 0 };

    // Start a SpellPane drag. The item pointer carries the spell index as
    // an index+1 integer cast — harness-only sentinel (see header note).
    // Coordination: EDragSource::SpellPane = 4 is already in uidragstate.h
    // (no Agent-B change needed).
    UIDragState::BeginDragLayers(
        EDragSource::SpellPane,
        spellIdx_,
        reinterpret_cast<TObjectInstance*>(static_cast<intptr_t>(spellIdx_ + 1)),
        mx, my,
        layers, layerCount,
        mx - originX, my - originY);

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
        reinterpret_cast<intptr_t>(drag.item)) - 1;

    if (UIDragState::CompleteDrag(EDragSource::SpellPane, spellIdx_, true) && onDrop_)
        onDrop_(spellIdx_, srcIdx);

    return true;
}
