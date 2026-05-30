// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  invslot.cpp - shared item-cell sub-control (TInvSlot)                *
// *************************************************************************

#include "invslot.h"

#include "renderer.h"
#include "logging.h"

#include <cstdio>
#include <cstring>

// ----------------------------------------------------------------------
// Constructor + content setters.
// ----------------------------------------------------------------------
TInvSlot::TInvSlot(int32_t x, int32_t y, int32_t w, int32_t h,
                   int32_t allowed_type,
                   PTBitmap empty_placeholder,
                   const SInvSlotStyle* style)
    : x_(x), y_(y), w_(w), h_(h),
      allowed_type_(allowed_type),
      empty_placeholder_(empty_placeholder),
      style_(style)
{
}

void TInvSlot::SetItem(TObjectInstance* item, PTBitmap icon, int32_t qty)
{
    item_ = item;
    icon_ = icon;
    qty_  = qty;
    if (!item || !icon)
        kind_ = EInvSlotKind::Empty;
    else if (kind_ != EInvSlotKind::Pouch)   // pouch is set explicitly
        kind_ = EInvSlotKind::Regular;
}

void TInvSlot::SetPouchOverlay(PTBitmap inner)
{
    pouch_inner_ = inner;
    if (inner) kind_ = EInvSlotKind::Pouch;
}

// ----------------------------------------------------------------------
// Hit test (TButton-style — interior of [x, x+w) × [y, y+h)).
// ----------------------------------------------------------------------
bool TInvSlot::OnSlot(int32_t mx, int32_t my) const
{
    return (mx >= x_) && (mx < x_ + w_)
        && (my >= y_) && (my < y_ + h_);
}

// ----------------------------------------------------------------------
// Drop-target policy (stubbed).
//
// Real implementation needs TPlayer::CanEquip(item, allowed_type_) to
// reject mismatched equipment. Inventory + BarInv (allowed_type_ == 0)
// accept anything. Lands with the cross-pane drag-state owner port.
// ----------------------------------------------------------------------
bool TInvSlot::CanAcceptDrop(TObjectInstance* /*dragged*/) const
{
    return allowed_type_ == 0;
}

// ----------------------------------------------------------------------
// Paint helpers (file-local).
// ----------------------------------------------------------------------
namespace {

void StampIcon(int32_t tw, int32_t th,
               PTBitmap bm,
               int32_t cellX, int32_t cellY,
               int32_t cellW, int32_t cellH,
               const SInvSlotStyle& style)
{
    if (!bm) return;
    if (style.icon_fit_to_cell) {
        const int32_t inset = style.icon_fit_inset;
        const int32_t dw = cellW - 2 * inset;
        const int32_t dh = cellH - 2 * inset;
        if (dw <= 0 || dh <= 0) return;
        Renderer->DrawBitmapSubrectStretchedToTarget(
            bm,
            /*dst*/ cellX + inset, cellY + inset, dw, dh,
            /*src*/ 0, 0, bm->width, bm->height,
            tw, th);
    } else {
        // Retail-faithful native stamp at cell TL (Inventory).
        Renderer->DrawBitmapToTarget(bm, cellX, cellY, tw, th);
    }
}

void StampPouchOverlay(int32_t tw, int32_t th,
                       PTBitmap inner,
                       int32_t cellX, int32_t cellY,
                       const SInvSlotStyle& style)
{
    if (!inner) return;
    const int32_t dx = cellX + style.pouch_inner_dx;
    const int32_t dy = cellY + style.pouch_inner_dy;
    if (style.pouch_overlay_stretch) {
        Renderer->DrawBitmapSubrectStretchedToTarget(
            inner,
            /*dst*/ dx, dy, style.pouch_inner_w, style.pouch_inner_h,
            /*src*/ 0, 0, inner->width, inner->height,
            tw, th);
    } else {
        Renderer->DrawBitmapToTarget(inner, dx, dy, tw, th);
    }
}

void StampCount(int32_t tw, int32_t th,
                int32_t value,
                int32_t cellX, int32_t cellY,
                int32_t dx, int32_t dy, int32_t w, int32_t hPad,
                ETextAlign align,
                float r, float g, float b,
                const SFontAtlas* font)
{
    if (!font) return;
    const int32_t lineH = (int32_t)(TextLineHeight(font) + 0.5f);
    const int32_t cellH = lineH + hPad;
    char buf[16];
    std::snprintf(buf, sizeof(buf), "%d", value);
    DrawTextShadowedToTarget(font, buf,
                             cellX + dx, cellY + dy, w, cellH,
                             align,
                             r, g, b, tw, th);
}

}  // namespace

// ----------------------------------------------------------------------
// Draw — the canonical per-cell paint.
//
// Composition order (matches retail's per-cell sequence in
// `cls_0x5a58c0::DrawGrid :537d19..:537e4f` and the same shape used by
// TBarInv's slot loop):
//   1. Empty-cell placeholder (Equip slot pictograms; null for inv/bar)
//   2. Main item icon (native stamp OR stretch-fit to cell)
//   3. Pouch overlay (Pouch kind only; the bag drawn over the cell)
//   4. Regular item quantity text (qty > 1, NOT pouches) — Inventory
//      paints RED top-right; BarInv paints WHITE top-left.
//   5. Pouch contents-count text — Inventory paints WHITE bottom-center;
//      BarInv paints WHITE left of cell.
// ----------------------------------------------------------------------
void TInvSlot::Draw(TSurface* /*target*/, int32_t tw, int32_t th,
                    const SFontAtlas* font)
{
    if (!style_) return;
    const SInvSlotStyle& style = *style_;

    // (1) Empty-cell placeholder. Only painted when there's no item;
    //     lets EquipPane show "Head", "Hand2", … pictograms in unfilled
    //     slots. Inventory + BarInv leave this null.
    if (kind_ == EInvSlotKind::Empty) {
        if (empty_placeholder_) {
            StampIcon(tw, th, empty_placeholder_,
                      x_, y_, w_, h_, style);
        }
        return;
    }

    // (2) Main item icon. Native TL stamp (Inventory) or stretch-fit
    //     into the cell interior (BarInv convention).
    if (icon_) {
        StampIcon(tw, th, icon_, x_, y_, w_, h_, style);
    }

    // (3) Pouch overlay (the bag drawn over the cell's contents).
    if (kind_ == EInvSlotKind::Pouch && pouch_inner_) {
        StampPouchOverlay(tw, th, pouch_inner_, x_, y_, style);
    }

    // (4) Regular item quantity text (qty > 1, NOT pouches).
    //     Inventory: RED top-right. BarInv: WHITE top-left.
    if (kind_ == EInvSlotKind::Regular
        && style.draw_qty && qty_ > 1)
    {
        StampCount(tw, th, qty_, x_, y_,
                   style.qty_rect_dx, style.qty_rect_dy,
                   style.qty_rect_w, style.qty_rect_h_pad,
                   style.qty_align,
                   style.qty_r, style.qty_g, style.qty_b,
                   font);
    }

    // (5) Pouch contents-count text.
    //     Inventory: WHITE bottom-center on bag's bottom edge.
    //     BarInv:    WHITE left of cell.
    if (kind_ == EInvSlotKind::Pouch && style.draw_bag_count)
    {
        StampCount(tw, th, qty_, x_, y_,
                   style.bag_rect_dx, style.bag_rect_dy,
                   style.bag_rect_w, style.bag_rect_h_pad,
                   style.bag_align,
                   style.bag_r, style.bag_g, style.bag_b,
                   font);
    }
}

// ----------------------------------------------------------------------
// Interaction dispatch (stubbed).
//
// Routes to the cross-pane drag-state owner once that lands. The retail
// owner is `cls_0x5a5320_TPlayScreen` via its `virt_meth_0x44f140`
// (12 read/write XREFs to the drag globals listed in the header banner).
// In the port: a singleton owned by TPlayScreen.
// ----------------------------------------------------------------------
bool TInvSlot::HandleEvent(EInvSlotEvent /*kind*/,
                           int32_t /*mouse_x*/, int32_t /*mouse_y*/)
{
    // Stubbed — wires through to the cross-pane drag-state owner in a
    // follow-up pass. Return false so the pane's existing event paths
    // remain authoritative until the dispatcher lands.
    return false;
}
