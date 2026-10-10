// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *             barinv.cpp - The potion shelf on the bottom bar           *
// *************************************************************************

#include "barinv.h"

#include "bitmap.h"
#include "logging.h"
#include "renderer.h"
#include "surface.h"

#include <algorithm>

namespace {

// REVSYNC: 0x0052ca70 -- a box (BarInvBox, 42 x 42) every 45 pixels from x
// 0xdc, at y 10, as many as fit: (width - 0xdc) / 0x2d.
constexpr int32_t kBoxesLeft = 0xdc;
constexpr int32_t kBoxPitch = 0x2d;
constexpr int32_t kBoxTop = 10;

}  // namespace

TBarInvPane::TBarInvPane() : TPane(0, 0, 0, 0) {}
TBarInvPane::~TBarInvPane() = default;

// REVSYNC: 0x0052c970. The pane takes its rect from the bottom bar
// (TBottomBarPane::LayOut).
bool TBarInvPane::Initialize()
{
    if (IsOpen())
        return true;
    if (!boxart)
    {
        log_error("[barinv] no box art");
        return false;
    }
    composedWidth = -1;
    return TPane::Initialize();
}

void TBarInvPane::Close()
{
    if (!IsOpen())
        return;
    layer.reset();
    composedWidth = -1;
    TPane::Close();
}

int32_t TBarInvPane::NumBoxes() const
{
    return (std::max)(0, (GetWidth() - kBoxesLeft) / kBoxPitch);
}

void TBarInvPane::Compose()
{
    if (IsDirty())
    {
        composedWidth = -1;
        SetDirty(false);
    }
    if (composedWidth == GetWidth() || GetWidth() <= 0 || GetHeight() <= 0 || !Renderer)
        return;
    if (!layer || layer->Width() != GetWidth() || layer->Height() != GetHeight())
        layer = std::make_unique<TSurface>(GetWidth(), GetHeight(), SG_PIXELFORMAT_RGBA8);
    layer->StartPass(0.0f, 0.0f, 0.0f, 0.0f);
    for (int32_t box = 0; box < NumBoxes(); ++box)
        Renderer->DrawBitmapToTarget(boxart, kBoxesLeft + box * kBoxPitch, kBoxTop, GetWidth(), GetHeight());
    layer->EndPass();
    composedWidth = GetWidth();
}

void TBarInvPane::Draw()
{
    if (layer && composedWidth >= 0 && Renderer)
        Renderer->DrawSurface(layer.get(), GetPosX(), GetPosY());
}
