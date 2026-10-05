// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uiinventorytest.cpp - --test=ui-inventory                            *
// *************************************************************************
//
// Clean-room reconstruction of TInventory (the right-sidebar Inventory
// content pane: the 188x174 4x3 column-major item grid that hangs below the
// upper sidebar content area). Built from
// docs/ui/forensics/InventoryPane_SPEC.md (FORENSICS_PROTOCOL, NOMENCLATURE,
// UI_METHOD_MAP §12, RECONSTRUCTION_PROTOCOL).
//
// Contents (spec §5 step 8): the main player's carried items, slot
// page + column*3 + row of its inventory (Player->GetInventorySlot), each
// bound through the shared TInvSlot; gold is the player's "Gold" items
// (retail (*player+0x84)("Gold") = GetInventoryAmount). The page comes from
// HudState.inventoryPage; a dragged item's cell paints empty while it
// follows the cursor (UIDragState). The --test=ui-inventory host supplies a
// demo player.
//
// Architecture (spec §3): compose the whole pane into one offscreen TSurface
// RT via the *ToTarget primitive family, then DrawSurface it once in the HUD.
//
// *************************************************************************

#include "uiinventorytest.h"

#include "bitmap.h"
#include "bitmapatlas.h"
#include "display.h"
#include "font.h"
#include "fonttable.h"
#include "hudstate.h"
#include "invslot.h"
#include "logging.h"
#include "multi.h"
#include "player.h"
#include "renderer.h"
#include "surface.h"
#include "uidragstate.h"

#include <cstdint>
#include <cstdio>
#include <cstring>

namespace {

// =====================================================================
// Geometry constants — all cited to InventoryPane_SPEC.md.
// =====================================================================

constexpr int32_t kPaneW = 0xbc;        // 188 — chrome / pane width  (§3)
constexpr int32_t kPaneH = 0xae;        // 174 — chrome / pane height (§3)

// Grid (spec §4 "Item-slot grid table", §6.1 cell kernel).
// Column-major: slot = col*3 + row + page (page = HudState.inventoryPage).
constexpr int32_t kGridX0     = 0x08;   // 8  — grid origin x (§4)
constexpr int32_t kGridY0     = 0x2a;   // 42 — grid origin y (§4)
constexpr int32_t kCellPitchX = 0x2d;   // 45 — x stride (§4)
constexpr int32_t kCellPitchY = 0x2c;   // 44 — y stride (§4)
constexpr int32_t kCellInner  = 0x28;   // 40 — interior (icon clip) (§4)
constexpr int32_t kGridCols   = 4;
constexpr int32_t kGridRows   = 3;
constexpr int32_t kGridCells  = kGridCols * kGridRows;   // 12

// Container-icon panel (spec §4 row "container art surf").
constexpr int32_t kContSurfX  = 0x0a;   // 10 (§4)
constexpr int32_t kContSurfY  = 0x0a;   // 10 (§4)
constexpr int32_t kContSurfW  = 20;
constexpr int32_t kContSurfH  = 20;

// GoldPile icon (spec §0 LIVE-VERIFIED #1; mbr_0x19c IS GoldPile, NOT Backpack).
constexpr int32_t kGoldIconX = 0x28;   // 40 — disasm `:537b79 PUSH 0x28`
constexpr int32_t kGoldIconY = 0x05;   //  5 — disasm `:537b77 PUSH 0x5`

// Scroll arrows (spec §4; Init ctor :165 / :185).
constexpr int32_t kArrowLX = 0x8c;    // 140
constexpr int32_t kArrowLY = 0x0b;    // 11
constexpr int32_t kArrowRX = 0xa1;    // 161
constexpr int32_t kArrowRY = 0x0c;    // 12
constexpr int32_t kArrowW  = 0x18;    // 24
constexpr int32_t kArrowH  = 0x18;    // 24

// Gold text cell (spec §8 row 1).
constexpr int32_t kGoldX = 0x50;      // 80
constexpr int32_t kGoldY = 0x0d;      // 13

// Stack-count cell width (spec §8 row 2).
constexpr int32_t kCountCellW = 0x28; // 40

// Fonts.
constexpr const char* kGoldFontName  = "Gold";   // user override — retail is GoldMed
constexpr const char* kCountFontFile = "Arimo-Regular.ttf";
constexpr int32_t     kCountFontPx   = 12;

// Page step — retail spec UNCONFIRMED-D says per-click delta is extracted
// from LAB_00537500/537510. The hit-test and HudState.inventoryPage counter
// in uisidebartest.cpp already increments by 1 per arrow click; we use 1
// here and match on that.
constexpr int32_t kMaxPage = 0xf3;    // retail upper bound per spec §5 step 10

// The money type the gold readout totals (`0x5e41a0`).
constexpr const char* kGoldItemName = "Gold";

// =====================================================================
// Asset roster — spec §2.
// =====================================================================
constexpr const char* kArchive       = "inventory.dat";
constexpr const char* kChromeName    = "Inventory";
constexpr const char* kBackpackName  = "Backpack";
constexpr const char* kArrowRUName   = "InvArwRU";   // right Up (idle / enabled)
constexpr const char* kArrowLUName   = "InvArwLU";   // left  Up (idle / enabled)
constexpr const char* kArrowRDName   = "InvArwRD";   // right Down (pressed / grayed)
constexpr const char* kArrowLDName   = "InvArwLD";   // left  Down (pressed / grayed)
constexpr const char* kGoldPileName  = "GoldPile";

// =====================================================================
// Loaded assets + render target.
// =====================================================================
TMulti*  g_inventoryDat = nullptr;
PTBitmap g_chrome       = nullptr;
PTBitmap g_backpack     = nullptr;
PTBitmap g_arrowRU      = nullptr;   // right enabled
PTBitmap g_arrowRD      = nullptr;   // right disabled/down
PTBitmap g_arrowLU      = nullptr;   // left  enabled
PTBitmap g_arrowLD      = nullptr;   // left  disabled/down
PTBitmap g_goldPile     = nullptr;
const SFontAtlas* g_goldFont  = nullptr;
const SFontAtlas* g_countFont = nullptr;

TSurface* g_pane = nullptr;
bool g_hudVisible = true;

// =====================================================================
// Inventory slot style (Inventory-pane visual conventions).
// =====================================================================
SInvSlotStyle MakeInventorySlotStyle()
{
    SInvSlotStyle s;
    s.icon_fit_to_cell  = false;   // native TL stamp — retail literal

    // Regular qty: RED top-right (spec §8 row 2 LIVE-VERIFIED #6)
    s.draw_qty          = true;
    s.qty_r             = 1.00f;
    s.qty_g             = 0.18f;
    s.qty_b             = 0.14f;
    s.qty_align         = ETextAlign::Right;
    s.qty_rect_dx       = 0;
    s.qty_rect_dy       = 0;
    s.qty_rect_w        = kCellInner;
    s.qty_rect_h_pad    = 0;

    // Bag-contents count: WHITE centered at cellY + 26 (spec §0 disasm
    // `:537de2 LEA EDX, [EBX + 0x1a]`; spec §5 step 8b).
    s.draw_bag_count    = true;
    s.bag_r             = 1.0f;
    s.bag_g             = 1.0f;
    s.bag_b             = 1.0f;
    s.bag_align         = ETextAlign::Center;
    s.bag_rect_dx       = 0;
    s.bag_rect_dy       = 26;
    s.bag_rect_w        = kCellInner;
    s.bag_rect_h_pad    = 0;

    // Pouch overlay at cellY + 20 (spec §5 step 8b `:537d83`).
    s.pouch_overlay_stretch = false;
    s.pouch_inner_dx    = 0;
    s.pouch_inner_dy    = 20;
    s.pouch_inner_w     = 20;
    s.pouch_inner_h     = 20;

    return s;
}

SInvSlotStyle g_invSlotStyle = MakeInventorySlotStyle();

// One TInvSlot per visible cell (geometry; content bound each Refresh).
TInvSlot* g_invSlots[kGridCells] = {};

// =====================================================================
// Asset lookup helper.
// =====================================================================
PTBitmap LookupByName(TMulti* m, const char* name)
{
    if (!m || !name) return nullptr;
    for (int32_t i = 0; i < m->numoffsets; ++i)
    {
        const char* nm = (const char*)m->names[i].ptr();
        if (nm && !std::strcmp(nm, name))
            return m->Bitmap(i);
    }
    return nullptr;
}

void BuildInvSlots()
{
    // Construct per-cell TInvSlot objects at the correct pane-local rects.
    // The geometry is fixed regardless of page; the page offset shifts which
    // inventory slot each cell shows.
    for (int32_t cell = 0; cell < kGridCells; ++cell)
    {
        delete g_invSlots[cell];
        const int32_t col = cell / kGridRows;
        const int32_t row = cell % kGridRows;
        const int32_t cx  = col * kCellPitchX + kGridX0;
        const int32_t cy  = row * kCellPitchY + kGridY0;
        g_invSlots[cell] = new TInvSlot(cx, cy, kCellInner, kCellInner,
                                        /*allowed_type*/ kInvSlotAcceptAny,
                                        /*placeholder*/  nullptr,
                                        /*style*/        &g_invSlotStyle);
    }
}

// =====================================================================
// HUD drawable.
// =====================================================================
class TInventoryHud : public THudDrawable
{
public:
    void Draw() override
    {
        if (!g_hudVisible || !g_pane) return;
        // Bottom-right anchor — spec §3: pane at (640-188, 480-174) in Classic.
        const int32_t dw = Display.Width();
        const int32_t dh = Display.Height();
        const int32_t x  = (dw > 0 ? dw : kPaneW) - kPaneW;
        const int32_t y  = (dh > 0 ? dh : kPaneH) - kPaneH;
        Renderer->DrawSurface(g_pane, x, y);
    }

    void Refresh()
    {
        if (!g_hudVisible) return;
        if (!g_chrome) return;
        EnsurePane();
        if (!g_pane) return;

        // HudState page (#7a — which 12-slot window is visible).
        const int32_t page = GetHudState().inventoryPage;

        // Active drag state (#7b — which slot is being dragged).
        const SUIDragState& drag = UIDragState::Get();
        const bool dragging = UIDragState::IsDragging()
                           && drag.source == EDragSource::Inventory;

        const int32_t tw = g_pane->Width();
        const int32_t th = g_pane->Height();

        g_pane->StartPass(0.0f, 0.0f, 0.0f, 0.0f);

        // Chrome (spec §5 step 4).
        Renderer->DrawBitmapToTarget(g_chrome, 0, 0, tw, th);

        // Container art (spec §5 step 5) — Backpack as harness placeholder.
        if (g_backpack)
            Renderer->DrawBitmapSubrectStretchedToTarget(
                g_backpack,
                kContSurfX, kContSurfY, kContSurfW, kContSurfH,
                0, 0, g_backpack->width, g_backpack->height,
                tw, th);

        // Gold readout + GoldPile icon, only with a player (spec §5 step 6 /
        // §8 row 1 / §0 LIVE-VERIFIED #1+#4).
        if (Player)
        {
            if (g_goldFont)
            {
                const int32_t cellW = kArrowLX - kGoldX;
                const int32_t cellH = (int32_t)(TextLineHeight(g_goldFont) + 0.5f);
                char buf[16];
                std::snprintf(buf, sizeof(buf), "%d$",
                              Player->GetInventoryAmount(kGoldItemName));
                DrawTextToTarget(g_goldFont, buf,
                                 kGoldX, kGoldY, cellW, cellH,
                                 ETextAlign::Left,
                                 1.0f, 1.0f, 1.0f, tw, th);
            }
            if (g_goldPile)
                Renderer->DrawBitmapToTarget(g_goldPile, kGoldIconX, kGoldIconY, tw, th);
        }

        // Per-cell item loop (spec §5 step 8 / §6.1 cell kernel): the cell
        // at column col, row row shows carried slot page + col*3 + row.
        for (int32_t cell = 0; Player && cell < kGridCells; ++cell)
        {
            const int32_t slot = page + cell;

            // #7b: the dragged item's cell paints empty while it follows
            // the cursor.
            if (dragging && drag.source_idx == slot) continue;

            TObjectInstance* item = Player->GetInventorySlot(slot);
            if (!item || !g_invSlots[cell]) continue;   // chrome border shows through

            g_invSlots[cell]->BindItem(item);
            g_invSlots[cell]->Draw(g_pane, tw, th, g_countFont);
        }

        // #7e: Scroll arrows with gray-out at page boundaries (spec §5 step 10).
        // Left arrow: grayed (Down art) at page == 0; enabled (Up art) at page > 0.
        // Right arrow: grayed (Down art) at page >= kMaxPage; enabled otherwise.
        // Per spec §2: Down/Glow variants are InvArwLD/RD (flags 0x104, alpha).
        // Using the Down art for the disabled state gives a visual dim without
        // requiring a separate tint pass.
        {
            const bool leftEnabled  = (page > 0);
            const bool rightEnabled = (page < kMaxPage);

            PTBitmap leftArt  = leftEnabled  ? g_arrowLU : g_arrowLD;
            PTBitmap rightArt = rightEnabled ? g_arrowRU : g_arrowRD;

            // Fallback: if Down variant not loaded, still show Up art (not blank).
            if (!leftArt)  leftArt  = g_arrowLU;
            if (!rightArt) rightArt = g_arrowRU;

            if (leftArt)
                Renderer->DrawBitmapToTarget(leftArt,  kArrowLX, kArrowLY, tw, th);
            if (rightArt)
                Renderer->DrawBitmapToTarget(rightArt, kArrowRX, kArrowRY, tw, th);
        }

        g_pane->EndPass();
    }

private:
    void EnsurePane()
    {
        if (g_pane) return;
        g_pane = new TSurface(kPaneW, kPaneH, SG_PIXELFORMAT_RGBA8);
    }
};

TInventoryHud g_hud;

}  // namespace

// =====================================================================
// Entry points.
// =====================================================================
bool InitializeUIInventoryMode()
{
    log_info("[ui-inventory] === TInventory reconstruction ===");

    // Load retail assets from inventory.dat (spec §2).
    g_inventoryDat = TMulti::LoadMulti((char*)kArchive);
    RegisterUIBitmapAtlasArchive(g_inventoryDat);
    if (g_inventoryDat)
    {
        g_chrome   = LookupByName(g_inventoryDat, kChromeName);
        g_backpack = LookupByName(g_inventoryDat, kBackpackName);
        g_arrowRU  = LookupByName(g_inventoryDat, kArrowRUName);
        g_arrowRD  = LookupByName(g_inventoryDat, kArrowRDName);
        g_arrowLU  = LookupByName(g_inventoryDat, kArrowLUName);
        g_arrowLD  = LookupByName(g_inventoryDat, kArrowLDName);
        g_goldPile = LookupByName(g_inventoryDat, kGoldPileName);
    }

    log_info("[ui-inventory] assets: chrome=%s back=%s "
             "ArwLU=%s ArwLD=%s ArwRU=%s ArwRD=%s GoldPile=%s",
             g_chrome   ? "OK" : "MISS",
             g_backpack ? "OK" : "MISS",
             g_arrowLU  ? "OK" : "MISS",
             g_arrowLD  ? "OK" : "MISS",
             g_arrowRU  ? "OK" : "MISS",
             g_arrowRD  ? "OK" : "MISS",
             g_goldPile ? "OK" : "MISS");
    if (g_chrome)
        log_info("[ui-inventory] chrome %dx%d (expect 188x174)",
                 g_chrome->width, g_chrome->height);

    // Gold font (BMFONT "Gold"; user override of retail "GoldMed").
    if (FontTable)
        if (TFont* f = FontTable->Bitmap(kGoldFontName))
            g_goldFont = BuildFontAtlas(f);

    // Stack-count font (Arimo TTF).
    g_countFont = BuildTTFAtlas(TTFFilePath(kCountFontFile).c_str(), kCountFontPx);

    log_info("[ui-inventory] fonts: Gold=%s  count=%s",
             g_goldFont ? "OK" : "MISS", g_countFont ? "OK" : "MISS");

    BuildInvSlots();

    delete g_pane;
    g_pane = nullptr;
    g_hudVisible = true;

    Renderer->AddHud(&g_hud, 0.0f);
    return true;
}

void RenderUIInventoryMode()
{
    RenderUIInventoryModeEmbedded();

    // Muted slate backdrop for isolated test view.
    Display.BackBuffer()->StartPass(0.18f, 0.20f, 0.26f, 1.0f);
    Display.BackBuffer()->EndPass();
}

void RenderUIInventoryModeEmbedded()
{
    if (!g_hudVisible) return;
    g_hud.Refresh();
}

void SetUIInventoryModeVisible(bool visible)
{
    g_hudVisible = visible;
}

void CloseUIInventoryMode()
{
    Renderer->RemoveHud(&g_hud);
    delete g_pane;
    g_pane = nullptr;

    g_chrome = g_backpack = nullptr;
    g_arrowRU = g_arrowRD = g_arrowLU = g_arrowLD = nullptr;
    g_goldPile = nullptr;
    g_inventoryDat = nullptr;
    g_goldFont  = nullptr;
    g_countFont = nullptr;

    for (int32_t i = 0; i < kGridCells; ++i)
    {
        delete g_invSlots[i];
        g_invSlots[i] = nullptr;
    }
    g_hudVisible = true;
}
