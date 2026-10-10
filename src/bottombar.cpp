// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *                   bottombar.cpp - The bottom bar                      *
// *************************************************************************

#include "bottombar.h"

#include "bitmap.h"
#include "logging.h"
#include "multi.h"
#include "playscreen.h"
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

    LayOut();
    if (!TButtonPane::Initialize() || !shelf.Initialize() || !QuickSpells.Initialize())
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

// The bar lies along the bottom of the map view, as wide as it (retail:
// TPlayScreen::Pulse 0x0047b4d0 gives it the display less the side pane).
void TBottomBarPane::LayOut()
{
    int32_t mapx = 0, mapy = 0, mapw = 0, maph = 0;
    PlayScreen.GetMapViewRect(mapx, mapy, mapw, maph);
    Resize(mapx, mapy + maph, mapw, kHeight);
    shelf.Resize(mapx, mapy + maph, mapw, kHeight);
    QuickSpells.Resize(mapx, mapy + maph, mapw, kHeight);
}

// The screen pulses its own panes; the bar pulses its children.
void TBottomBarPane::Pulse()
{
    LayOut();
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
// stretched), then BarEndCap at its right end.
void TBottomBarPane::ComposeBackground(int32_t target_w, int32_t target_h)
{
    if (!Renderer)
        return;
    Renderer->DrawBitmapSubrectToTarget(utilitybar, 0, 0, 0, 0, target_w, kHeight, target_w, target_h);
    Renderer->DrawBitmapToTarget(endcap, target_w - kEndCapWidth, 0, target_w, target_h);
}
