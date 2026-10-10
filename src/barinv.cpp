// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *             barinv.cpp - The potion shelf on the bottom bar           *
// *************************************************************************

#include "barinv.h"

#include "bitmap.h"
#include "fonttable.h"
#include "logging.h"
#include "player.h"
#include "playscreen.h"
#include "renderer.h"
#include "revdefs.h"
#include "surface.h"
#include "uidragstate.h"

#include <algorithm>

namespace {

// REVSYNC: 0x0052ca70 -- a box (BarInvBox, 42 x 42, drawn opaque: mode
// DM_BACKGROUND) every 45 pixels from x 0xdc, at y 10, as many as fit:
// (width - 0xdc) / 0x2d. An item's icon sits at the box's top-left.
constexpr int32_t kBoxesLeft = 0xdc;
constexpr int32_t kBoxPitch = 0x2d;
constexpr int32_t kBoxTop = 10;
constexpr int32_t kCellSize = 0x28;
constexpr int32_t kBeltSlots = kInvSlotLast - kInvSlotBeltFirst + 1;

// REVSYNC: 0x0052ca70 / 0x0052cd80 -- an amount over 1 right-aligned across
// the cell's top in "Numbers" (FONT.DEF: Arial 12, red), shadowed; a pouch's
// slot-0 item at half size 20 down, and the pouch's count beside it in white
// "Small", shadowed, in a cell 20 wide at (20, 26).
constexpr int32_t kPouchItemTop = 0x14;
constexpr int32_t kPouchCountLeft = 0x14;
constexpr int32_t kPouchCountTop = 0x24 - kBoxTop;
constexpr int32_t kPouchCountWidth = 0x14;
constexpr SColor kPouchCountColor = { 0xff, 0xff, 0xff };

}  // namespace

TBarInvPane::TBarInvPane() : TPane(0, 0, 0, 0) {}
TBarInvPane::~TBarInvPane() = default;

// REVSYNC: 0x0052c970. The pane takes its rect from the bottom bar
// (TBottomBarPane::LayOut).
bool TBarInvPane::Initialize()
{
    if (IsOpen())
        return true;
    const TGenericFont* numbers = FontTable ? FontTable->FindFont("Numbers") : nullptr;
    style.amount = { FontTable ? FontTable->Atlas("Numbers") : nullptr, numbers ? numbers->color : SColor{},
                     0, 0, kCellSize, ETextAlign::Right };
    style.pouchItemX = 0;
    style.pouchItemY = kPouchItemTop;
    style.pouchCount = { FontTable ? FontTable->Atlas("Small") : nullptr, kPouchCountColor,
                         kPouchCountLeft, kPouchCountTop, kPouchCountWidth, ETextAlign::Left };
    if (!boxart || !style.amount.font || !style.pouchCount.font)
    {
        log_error("[barinv] no box art, or the \"Numbers\" or \"Small\" font is missing");
        return false;
    }
    if (!TPane::Initialize())
        return false;
    LayOutCells();
    return true;
}

void TBarInvPane::Close()
{
    if (!IsOpen())
        return;
    layer.reset();
    cells.clear();
    composedVersion = -1;
    TPane::Close();
}

int32_t TBarInvPane::NumBoxes() const
{
    return (std::max)(0, (GetWidth() - kBoxesLeft) / kBoxPitch);
}

void TBarInvPane::LayOutCells()
{
    cells.clear();
    const int32_t count = (std::min)(NumBoxes(), kBeltSlots);
    cells.reserve(count);
    for (int32_t box = 0; box < count; ++box)
        cells.emplace_back(kBoxesLeft + box * kBoxPitch, kBoxTop, kCellSize, kCellSize, kInvSlotAcceptAny,
                           nullptr, &style);
    ++contentVersion;
}

void TBarInvPane::PaneResized()
{
    TPane::PaneResized();
    if (IsOpen())
        LayOutCells();
}

// REVSYNC: 0x0052ca70 -- box n shows the item in the main player's belt slot
// 0x10b + n (retail walks the player, pane +0x60, set as it becomes the main
// player). The box of an item being dragged off the shelf is left empty
// (+0x6c, +0x64); the drag is UIDragState's.
void TBarInvPane::Pulse()
{
    const int32_t frame = PlayScreen.GameFrame();
    const SUIDragState& drag = UIDragState::Get();
    const int32_t dragged = UIDragState::IsDragging() && drag.source == EDragSource::BarInv ? drag.source_idx : -1;
    for (size_t box = 0; box < cells.size(); ++box)
    {
        TObjectInstance* item = Player && int32_t(box) != dragged
                              ? Player->GetInventorySlot(kInvSlotBeltFirst + int32_t(box)) : nullptr;
        if (cells[box].BindItem(item, frame))
            ++contentVersion;
    }
}

void TBarInvPane::Compose()
{
    if (IsDirty())
    {
        ++contentVersion;
        SetDirty(false);
    }
    if (composedVersion == contentVersion || GetWidth() <= 0 || GetHeight() <= 0 || !Renderer)
        return;
    for (TInvSlot& cell : cells)
        cell.Prepare();
    if (!layer || layer->Width() != GetWidth() || layer->Height() != GetHeight())
        layer = std::make_unique<TSurface>(GetWidth(), GetHeight(), SG_PIXELFORMAT_RGBA8);
    layer->StartPass(0.0f, 0.0f, 0.0f, 0.0f);
    for (int32_t box = 0; box < NumBoxes(); ++box)
        Renderer->DrawBitmapToTarget(boxart, kBoxesLeft + box * kBoxPitch, kBoxTop, GetWidth(), GetHeight(),
                                     EBitmapDecode::Unkeyed);
    for (const TInvSlot& cell : cells)
        cell.Draw(GetWidth(), GetHeight());
    layer->EndPass();
    composedVersion = contentVersion;
}

void TBarInvPane::Draw()
{
    if (layer && composedVersion >= 0 && Renderer)
        Renderer->DrawSurface(layer.get(), GetPosX(), GetPosY());
}
