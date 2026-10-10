// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *                   bottombar.cpp - The bottom bar                      *
// *************************************************************************

#include "bottombar.h"

#include "bitmap.h"
#include "logging.h"
#include "multi.h"
#include "renderer.h"
#include "spellpane.h"

namespace {

// REVSYNC: 0x0052c880 -- the end cap's 10 pixels close the bar at its right.
constexpr int32_t kEndCapWidth = 10;

}  // namespace

TBottomBarPane::TBottomBarPane() : TButtonPane(0, 0, 0, 0) {}
TBottomBarPane::~TBottomBarPane() = default;

// REVSYNC: 0x0052c780. Retail looks its art up in the BottomBar.dat
// TPlayScreen keeps (DAT_0065a570); the bar loads its own and hands the
// shelf its box. The rings load SpellIcons.dat themselves.
bool TBottomBarPane::Initialize()
{
    if (IsOpen())
        return true;
    archive.reset(TMulti::LoadMulti("BottomBar.dat"));
    if (!archive)
    {
        log_error("[bottombar] BottomBar.dat is missing");
        return false;
    }
    utilitybar = archive->Bitmap("UtilityBar");
    endcap = archive->Bitmap("BarEndCap");
    shelf.SetBoxArt(archive->Bitmap("BarInvBox"));

    if (!TButtonPane::Initialize())
    {
        Close();
        return false;
    }
    for (TPane* child : { static_cast<TPane*>(&shelf), static_cast<TPane*>(&QuickSpells) })
        child->Resize(GetPosX(), GetPosY(), GetWidth(), GetHeight());
    if (!shelf.Initialize() || !QuickSpells.Initialize())
    {
        log_error("[bottombar] the shelf or the quick-spell rings did not initialize");
        Close();
        return false;
    }
    AddChild(&shelf);
    AddChild(&QuickSpells);
    return true;
}

void TBottomBarPane::Close()
{
    RemoveChild(&QuickSpells);
    RemoveChild(&shelf);
    QuickSpells.Close();
    shelf.Close();
    TButtonPane::Close();
    utilitybar = endcap = nullptr;
    archive.reset();
}

// REVSYNC: 0x0052c930 (SetRect) -- the bar and its two panes take the rect.
// Before Initialize it is the rect the bar opens with; once open it applies
// at once.
void TBottomBarPane::Place(int32_t x, int32_t y, int32_t width)
{
    Resize(x, y, width, kHeight);
    if (IsOpen() && WasResized())
        PaneResized();
}

// The screen pulses its own panes; the bar pulses its children.
void TBottomBarPane::Pulse()
{
    shelf.Pulse();
    QuickSpells.Pulse();
}

void TBottomBarPane::PaneResized()
{
    TButtonPane::PaneResized();
    for (TPane* child : Children())
    {
        child->Resize(GetPosX(), GetPosY(), GetWidth(), GetHeight());
        child->PaneResized();
    }
    SetDirty(true);
}

// REVSYNC: 0x0052c880 -- UtilityBar cropped to the bar's width (not
// stretched), then BarEndCap at its right end; both opaque (their own draw
// mode, 0).
void TBottomBarPane::ComposeBackground(int32_t target_w, int32_t target_h)
{
    if (!Renderer)
        return;
    Renderer->DrawBitmapSubrectToTarget(utilitybar, 0, 0, 0, 0, target_w, kHeight, target_w, target_h,
                                        EBitmapDecode::Unkeyed);
    Renderer->DrawBitmapToTarget(endcap, target_w - kEndCapWidth, 0, target_w, target_h, EBitmapDecode::Unkeyed);
}
