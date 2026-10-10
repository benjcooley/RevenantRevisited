// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  invslot.cpp - shared item-cell sub-control (TInvSlot)                *
// *************************************************************************

#include "invslot.h"

#include "animation.h"
#include "imagery.h"
#include "object.h"
#include "renderer.h"
#include "revdefs.h"
#include "surface.h"

#include <algorithm>
#include <string>

namespace {

// REVSYNC: 0x0052cd80 / 0x0052ca70. An item whose state has an inventory
// animation shows the frame the game frame picks (Animate); any other shows
// its inventory icon (paint, through the item's own draw).
SInvIcon ItemIcon(TObjectInstance* item, int32_t frame)
{
    SInvIcon icon;
    if (!item)
        return icon;
    if (TObjectImagery* imagery = item->GetImagery())
        if (TAnimation* anim = imagery->GetInvAnimation(item->GetState()); anim && anim->NumFrames() > 0)
        {
            icon.Add(anim->GetFrame(frame % anim->NumFrames()), 0, 0);
            return icon;
        }
    return item->InventoryIcon();
}

// The icon's images at (x, y), each clipped to the icon's INVITEMREALWIDTH x
// INVITEMREALHEIGHT, as retail's composed icons were.
void DrawIcon(const SInvIcon& icon, int32_t x, int32_t y, int32_t target_w, int32_t target_h)
{
    for (int32_t i = 0; i < icon.count; ++i)
    {
        const SInvIconPart& part = icon.parts[i];
        const int32_t left = (std::max)(0, -part.x);
        const int32_t top = (std::max)(0, -part.y);
        const int32_t right = (std::min)(part.image->width, INVITEMREALWIDTH - part.x);
        const int32_t bottom = (std::min)(part.image->height, INVITEMREALHEIGHT - part.y);
        if (right > left && bottom > top)
            Renderer->DrawBitmapSubrectToTarget(part.image, x + part.x + left, y + part.y + top, left, top,
                                                right - left, bottom - top, target_w, target_h);
    }
}

void DrawText(const SInvSlotText& text, int32_t value, int32_t cellX, int32_t cellY,
              int32_t target_w, int32_t target_h)
{
    if (!text.font)
        return;
    DrawTextShadowedToTarget(text.font, std::to_string(value).c_str(), cellX + text.x, cellY + text.y,
                             text.w, 0, text.align, text.color.red / 255.0f, text.color.green / 255.0f,
                             text.color.blue / 255.0f, target_w, target_h);
}

}  // namespace

bool SInvSlotContent::operator==(const SInvSlotContent& other) const
{
    return item == other.item && icon == other.icon && amount == other.amount
        && pouchItem == other.pouchItem && pouchCount == other.pouchCount;
}

TInvSlot::TInvSlot(int32_t x, int32_t y, int32_t w, int32_t h, int32_t allowedType,
                   TBitmap* emptyPlaceholder, const SInvSlotStyle* style)
    : x(x), y(y), w(w), h(h), allowedType(allowedType), emptyPlaceholder(emptyPlaceholder), style(style)
{
}

TInvSlot::~TInvSlot() = default;
TInvSlot::TInvSlot(TInvSlot&&) noexcept = default;
TInvSlot& TInvSlot::operator=(TInvSlot&&) noexcept = default;

// REVSYNC: 0x0052ca70. A pouch shows the item in its slot 0 (GetInventory
// 0x004701f0) by that item's inventory icon (vtable +0x130, not its
// animation), with the pouch's item count. Retail counts the pouch's
// inventory array (0x00470040, +0x68), which keeps the holes items taken out
// leave; the count here is of the items in it.
bool TInvSlot::BindItem(TObjectInstance* item, int32_t frame)
{
    SInvSlotContent shown;
    if (item)
    {
        shown.item = item;
        shown.icon = ItemIcon(item, frame);
        shown.amount = item->Amount();
        if (stricmp(item->GetName(), "Pouch") == 0)
            if (TObjectInstance* first = item->GetInventorySlot(0))
                if (!(shown.pouchItem = first->InventoryIcon()).Empty())
                    shown.pouchCount = item->RealNumInventoryItems();
    }
    if (shown == content)
        return false;
    content = shown;
    return true;
}

bool TInvSlot::OnSlot(int32_t mx, int32_t my) const
{
    return mx >= x && mx < x + w && my >= y && my < y + h;
}

bool TInvSlot::CanAcceptDrop(TObjectInstance* dragged) const
{
    if (allowedType == kInvSlotAcceptAny)
        return true;
    if (!dragged || dragged->FindStat("EqSlot") < 0)
        return false;
    return dragged->GetStat("EqSlot") == allowedType;
}

// A pouch's stacked item is halved as a whole, as retail halves the bitmap
// it composed (0x004a31a0): put it together in a layer of its own first.
void TInvSlot::Prepare()
{
    if (content.pouchItem.Empty() || content.pouchItem.Single())
    {
        pouchStack.reset();
        pouchStackIcon = {};
        return;
    }
    if (pouchStack && pouchStackIcon == content.pouchItem)
        return;
    if (!Renderer)
        return;
    if (!pouchStack)
        pouchStack = std::make_unique<TSurface>(INVITEMREALWIDTH, INVITEMREALHEIGHT, SG_PIXELFORMAT_RGBA8);
    pouchStack->StartPass(0.0f, 0.0f, 0.0f, 0.0f);
    DrawIcon(content.pouchItem, 0, 0, INVITEMREALWIDTH, INVITEMREALHEIGHT);
    pouchStack->EndPass();
    pouchStackIcon = content.pouchItem;
}

// REVSYNC: 0x0052ca70 / 0x0052cd80 -- the icon, a pouch's item and count,
// then the amount.
void TInvSlot::Draw(int32_t target_w, int32_t target_h) const
{
    if (!Renderer)
        return;
    if (!content.item)
    {
        Renderer->DrawBitmapToTarget(emptyPlaceholder, x, y, target_w, target_h);
        return;
    }
    DrawIcon(content.icon, x, y, target_w, target_h);
    if (!style)
        return;
    if (!content.pouchItem.Empty())
    {
        const int32_t px = x + style->pouchItemX, py = y + style->pouchItemY;
        if (content.pouchItem.Single())
            Renderer->DrawBitmapHalvedToTarget(content.pouchItem.parts[0].image, px, py, target_w, target_h);
        else if (pouchStack && pouchStackIcon == content.pouchItem)
            Renderer->DrawSurfaceHalvedToTarget(pouchStack.get(), px, py, target_w, target_h);
        DrawText(style->pouchCount, content.pouchCount, x, y, target_w, target_h);
    }
    if (content.amount > 1)
        DrawText(style->amount, content.amount, x, y, target_w, target_h);
}

bool TInvSlot::HandleEvent(EInvSlotEvent /*kind*/, int32_t /*mouse_x*/, int32_t /*mouse_y*/)
{
    return false;
}
