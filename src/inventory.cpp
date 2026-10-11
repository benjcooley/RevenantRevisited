// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  Inventory.cpp - TInvetory object                     *
// *************************************************************************

#include "inventory.h"

#include "bitmap.h"
#include "fonttable.h"
#include "logging.h"
#include "multi.h"
#include "object.h"
#include "player.h"
#include "playscreen.h"
#include "renderer.h"
#include "revdefs.h"
#include "uidragstate.h"

#include <algorithm>
#include <cstdio>
#include <iterator>

namespace {

// REVSYNC: 0x00537a70 -- the art and where it goes.
constexpr int32_t kContainerIconLeft = 10;
constexpr int32_t kContainerIconTop = 10;
constexpr int32_t kGoldPileLeft = 0x28;
constexpr int32_t kGoldPileTop = 5;
constexpr int32_t kGoldTextLeft = 0x50;
constexpr int32_t kGoldTextTop = 0xd;
constexpr const char* kGoldFormat = "%d$";          // "Gold" draws '$' as "gp" (0x005e41a8)
constexpr const char* kGoldName = "Gold";           // the item the readout totals (0x005e41a0)
constexpr int32_t kCellsLeft = 8;
constexpr int32_t kCellsTop = 0x2a;
constexpr int32_t kCellPitchX = 0x2d;
constexpr int32_t kCellPitchY = 0x2c;

// REVSYNC: 0x00537650 -- the arrows, 24 x 24: "scrollleft" at (140, 11),
// "scrollright" at (161, 12), with up, down and grey art.
constexpr int32_t kArrowSize = 0x18;
struct SArrow
{
    const char* name;
    int32_t x, y;
    const char* up;
    const char* down;
    const char* grey;
    int32_t columns;
};
constexpr SArrow kArrows[] = {
    { "scrollleft", 0x8c, 0xb, "InvArwLU", "InvArwLD", "InvArwLG", -1 },
    { "scrollright", 0xa1, 0xc, "InvArwRU", "InvArwRD", "InvArwRG", 1 },
};
void (*const kArrowFunctions[])() = { [] { Inventory.Scroll(-1); }, [] { Inventory.Scroll(1); } };
static_assert(std::size(kArrows) == std::size(kArrowFunctions));

}  // namespace

TInventory::TInventory() : TButtonPane(0, 0, kWidth, kHeight) {}
TInventory::~TInventory() = default;

// REVSYNC: 0x00537650. Its host places it in the side panel's lower slot.
bool TInventory::Initialize()
{
    if (IsOpen())
        return true;
    archive.reset(TMulti::LoadMulti("Inventory.dat"));
    goldFont = FontTable ? FontTable->Atlas("Gold") : nullptr;
    style = SInvSlotStyle::Retail();
    if (!archive || !goldFont || !style.amount.font || !style.pouchCount.font)
    {
        log_error("[inventory] Inventory.dat, or the \"Gold\", \"Numbers\" or \"Small\" font is missing");
        archive.reset();
        return false;
    }
    background = archive->Bitmap("Inventory");
    backpack = archive->Bitmap("Backpack");
    goldpile = archive->Bitmap("GoldPile");
    if (!TButtonPane::Initialize())
        return false;
    for (size_t i = 0; i < std::size(kArrows); ++i)
    {
        const SArrow& arrow = kArrows[i];
        auto* button = new TButton(arrow.name, arrow.x, arrow.y, kArrowSize, kArrowSize, 0, kArrowFunctions[i],
                                   archive->Bitmap(arrow.down), archive->Bitmap(arrow.up));
        button->SetGreyBitmap(archive->Bitmap(arrow.grey));
        NewButton(button);
    }
    cells.clear();
    cells.reserve(kColumns * kRows);
    for (int32_t column = 0; column < kColumns; ++column)
        for (int32_t row = 0; row < kRows; ++row)
            cells.emplace_back(kCellsLeft + column * kCellPitchX, kCellsTop + row * kCellPitchY, INVITEMREALWIDTH,
                               INVITEMREALHEIGHT, kInvSlotAcceptAny, nullptr, &style);
    firstSlot = 0;
    RefreshArrows();
    return true;
}

void TInventory::Close()
{
    if (!IsOpen())
        return;
    TButtonPane::Close();
    cells.clear();
    goldText.clear();
    containerIcon = nullptr;
    archive.reset();
}

TObjectInstance* TInventory::GetTopContainer() const
{
    TObjectInstance* top = container;
    while (top && top->GetOwner())
        top = top->GetOwner();
    return top;
}

// REVSYNC: 0x005391a0 -- a new container opens on its first page.
void TInventory::SetContainer(TObjectInstance* cont)
{
    if (container == cont)
        return;
    container = cont;
    firstSlot = 0;
    RefreshArrows();
    SetDirty(true);
}

// REVSYNC: 0x00537520. A column at a time; an arrow that can't move greys out.
void TInventory::Scroll(int32_t columns)
{
    const int32_t first = std::clamp(firstSlot + columns * kRows, 0, kLastFirstSlot);
    if (first == firstSlot)
        return;
    firstSlot = first;
    RefreshArrows();
    SetDirty(true);
}

void TInventory::RefreshArrows()
{
    if (Buttons.NumItems() < static_cast<int32_t>(std::size(kArrows)))
        return;
    Button(0)->SetDisabled(firstSlot <= 0);
    Button(1)->SetDisabled(firstSlot >= kLastFirstSlot);
}

// REVSYNC: 0x00537a70 -- cell n shows slot first + n (column-major), the
// item being dragged off the grid left empty (+0x18c, +0x180; the drag is
// UIDragState's); the gold readout is the main player's.
void TInventory::Pulse()
{
    TButtonPane::Pulse();
    const int32_t frame = PlayScreen.GameFrame();
    const SUIDragState& drag = UIDragState::Get();
    const int32_t dragged = UIDragState::IsDragging() && drag.source == EDragSource::Inventory ? drag.source_idx : -1;
    bool changed = false;
    for (size_t n = 0; n < cells.size(); ++n)
    {
        const int32_t slot = firstSlot + static_cast<int32_t>(n);
        TObjectInstance* item = container && slot != dragged ? container->GetInventorySlot(slot) : nullptr;
        changed |= cells[n].BindItem(item, frame);
    }
    char gold[32] = {};
    if (Player)
        std::snprintf(gold, sizeof(gold), kGoldFormat, Player->GetInventoryAmount(kGoldName));
    TBitmap* icon = !container ? nullptr : container == Player ? backpack : container->InventoryImage();
    if (changed || goldText != gold || containerIcon != icon)
    {
        goldText = gold;
        containerIcon = icon;
        SetDirty(true);
    }
}

void TInventory::Compose()
{
    for (TInvSlot& cell : cells)
        cell.Prepare();
    TButtonPane::Compose();
}

// REVSYNC: 0x00537a70 -- the background (opaque), the container's icon (the
// backpack, or the bag's icon reduced to 20 x 20, 0x00538e40), the gold
// readout and its pile (with alpha), then the cells. The arrows follow as
// the pane's buttons.
void TInventory::ComposeBackground(int32_t target_w, int32_t target_h)
{
    if (!Renderer)
        return;
    Renderer->DrawBitmapToTarget(background, 0, 0, target_w, target_h, EBitmapDecode::Unkeyed);
    if (containerIcon == backpack)
        Renderer->DrawBitmapToTarget(backpack, kContainerIconLeft, kContainerIconTop, target_w, target_h);
    else if (containerIcon)
        Renderer->DrawBitmapHalvedToTarget(containerIcon, kContainerIconLeft, kContainerIconTop, target_w, target_h,
                                           EHalving::BoxOverBlack);
    if (!goldText.empty())
    {
        DrawTextToTarget(goldFont, goldText.c_str(), kGoldTextLeft, kGoldTextTop, kWidth - kGoldTextLeft,
                         static_cast<int32_t>(TextLineHeight(goldFont)), ETextAlign::Left, 1.0f, 1.0f, 1.0f,
                         target_w, target_h);
        Renderer->DrawBitmapToTarget(goldpile, kGoldPileLeft, kGoldPileTop, target_w, target_h,
                                     EBitmapDecode::Unkeyed);
    }
    for (const TInvSlot& cell : cells)
        cell.Draw(target_w, target_h);
}
