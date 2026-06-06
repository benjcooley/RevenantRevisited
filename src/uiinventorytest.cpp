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
// Scope of this file (sub-items #7a–#7f and #14):
//   #7a  — 4×3 = 12 slots, reading HudState.inventoryPage for correct offset
//   #7b  — Drag visual: dragged slot skipped in paint; icon follows cursor
//          via SetDragBitmap (cursor.h); begins drag via UIDragState::BeginDrag
//   #7c  — Bags: harness_inv slots marked is_bag show mini inner-item icon;
//          right-click open/close handled in uisidebartest.cpp (not owned here)
//   #7d  — Inv→Equip drop handled by CompleteDrag swap in uidragstate.cpp;
//          uiequiptest.cpp reads harness_equip[] to show dropped items
//   #7e  — Arrow gray-out: Left gray at page=0, Right gray at max page.
//          Uses InvArwLD (Down art = grayed) for disabled side; Up for enabled.
//   #7f  — Use cursor: right-click detection in uisidebartest.cpp (not owned
//          here — this pane just renders); reported as sidebar dispatch issue.
//   #14  — Swap committed in UIDragState::CompleteDrag with sound effect.
//
// Architecture (spec §3): compose the whole pane into one offscreen TSurface
// RT via the *ToTarget primitive family, then DrawSurface it once in the HUD.
//
// Shared state: reads UIDragState::harness_inv[] for per-slot item content
// (written here at Initialize, updated by CompleteDrag swaps). Reads
// HudState.inventoryPage for the visible slot window.
//
// *************************************************************************

#include "uiinventorytest.h"

#include "animation.h"
#include "bitmap.h"
#include "bitmapatlas.h"
#include "display.h"
#include "font.h"
#include "fonttable.h"
#include "hudstate.h"
#include "imagery.h"
#include "invslot.h"
#include "logging.h"
#include "multi.h"
#include "object.h"
#include "renderer.h"
#include "surface.h"
#include "uidragstate.h"

#include <cstdint>
#include <cstdio>
#include <cstring>

// Item-class registries (defined in their respective .cpp files).
extern TObjectClass PotionClass;
extern TObjectClass FoodClass;
extern TObjectClass KeyClass;
extern TObjectClass MoneyClass;
extern TObjectClass ArmorClass;

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
constexpr const char* kCountFontPath = "thirdparty/fonts/Arimo-Regular.ttf";
constexpr int32_t     kCountFontPx   = 12;

// Page step — retail spec UNCONFIRMED-D says per-click delta is extracted
// from LAB_00537500/537510. The hit-test and HudState.inventoryPage counter
// in uisidebartest.cpp already increments by 1 per arrow click; we use 1
// here and match on that.
constexpr int32_t kMaxPage = 0xf3;    // retail upper bound per spec §5 step 10

// Synthetic gold value (no real TPlayer in harness).
constexpr int32_t kGoldValue = 14475;

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

// =====================================================================
// Per-spawn tracking (for icon lazy-extraction and anim stepping).
// harness_inv[] is the shared cross-pane item state; g_spawned[] is the
// per-instance animation/label metadata that only this pane needs.
// =====================================================================
struct SSpawnMeta
{
    PTAnimation  anim       = nullptr;   // non-null if animated
    int32_t      animFrames = 0;
    const char*  label      = "?";
};
// One meta entry per harness_inv slot populated at Initialize.
SSpawnMeta   g_spawnMeta[kHarnessInvSlots] = {};
int32_t      g_spawnCount = 0;

// Per-slot TInvSlot objects (geometry only; content comes from harness_inv[]).
TInvSlot*    g_invSlots[kHarnessInvSlots] = {};

// Refresh tick counter (drives invanim stepping).
int32_t      g_refreshCount = 0;
// Advance animation every 3 Refresh ticks (24 fps asset cadence / ~60+ fps
// harness cadence; matches the existing animation gate pattern).
constexpr int32_t kAnimStride = 3;

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

// Try to extract a static or animated inventory icon from a spawned instance.
// Returns true once bound; retried each Refresh() until imagery streams in.
bool TryExtractIcon(int32_t idx)
{
    SHarnessSlot& slot = UIDragState::harness_inv[idx];
    SSpawnMeta&   meta = g_spawnMeta[idx];
    if (slot.icon || meta.anim) return true;   // already bound
    if (!slot.inst) return false;

    TObjectImagery* img = slot.inst->GetImagery();
    if (!img || img->NumStates() <= 0) return false;

    // Static invitem first (mirrors InventoryImage() retail logic).
    for (int32_t s = 0; s < img->NumStates(); ++s)
        if (PTBitmap bm = img->GetInvImage(s))
        {
            slot.icon = bm;
            log_info("[ui-inventory] slot %d '%s' bound invitem %dx%d",
                     idx, meta.label ? meta.label : "?", bm->width, bm->height);
            return true;
        }

    // invanim fallback (potions etc. carry only animation).
    for (int32_t s = 0; s < img->NumStates(); ++s)
        if (TAnimation* a = img->GetInvAnimation(s))
        {
            meta.anim       = a;
            meta.animFrames = a->NumFrames();
            slot.icon       = a->GetFrame(0);
            log_info("[ui-inventory] slot %d '%s' bound invanim %d frames",
                     idx, meta.label ? meta.label : "?", meta.animFrames);
            return true;
        }

    return false;
}

// =====================================================================
// Spawn helper — same pattern as uibarinvtest + uiequiptest.
// =====================================================================
TObjectInstance* SpawnItem(TObjectClass& cls, int32_t objclass,
                           const char** name_out)
{
    const int32_t n = cls.NumTypes();
    for (int32_t i = 0; i < n; ++i)
    {
        SObjectInfo* info = cls.GetObjType(i);
        if (!info) continue;

        SObjectDef def = {};
        def.objclass = (short)objclass;
        def.objtype  = (short)i;
        def.state    = 0;
        def.level    = 0;
        def.pos      = { 0, 0, 0 };
        def.vel      = { 0, 0, 0 };
        def.accum    = { 0, 0, 0 };
        def.rotatex  = 0;
        def.rotatey  = 0;
        def.rotatez  = 0;
        def.group    = 0;

        TObjectInstance* inst = cls.NewObject(&def);
        if (!inst) continue;
        inst->OnScreen();
        if (name_out) *name_out = info->name ? info->name : cls.ClassName();
        return inst;
    }
    return nullptr;
}

TObjectInstance* SpawnNamedItem(TObjectClass& cls, int32_t objclass,
                                const char* typeName,
                                const char** name_out)
{
    const int32_t objtype = cls.FindObjType(typeName);
    if (objtype < 0) return nullptr;
    SObjectInfo* info = cls.GetObjType(objtype);
    if (!info) return nullptr;

    SObjectDef def = {};
    def.objclass = (short)objclass;
    def.objtype  = (short)objtype;
    def.state    = 0;
    def.level    = 0;
    def.pos      = { 0, 0, 0 };
    def.vel      = { 0, 0, 0 };
    def.accum    = { 0, 0, 0 };
    def.rotatex  = 0;
    def.rotatey  = 0;
    def.rotatez  = 0;
    def.group    = 0;

    TObjectInstance* inst = cls.NewObject(&def);
    if (!inst) return nullptr;
    inst->OnScreen();
    if (name_out) *name_out = info->name ? info->name : typeName;
    return inst;
}

void PopulateHarness()
{
    // Fill the first few harness_inv[] slots with real spawned items.
    // This also seeds their metadata in g_spawnMeta[].  harness_inv[] is
    // readable by any pane (uiequiptest, uidragstate, etc.).
    //
    // Layout: column-major (slot = col*3 + row).
    //   Slot 0 (col0, row0): Helmet — equips into EQ_HEAD for drag tests
    //   Slot 1 (col0, row1): Food   — exercises qty count (5)
    //   Slot 2 (col0, row2): Key    — single item
    //   Slot 3 (col1, row0): Money  — exercises qty count (42)
    //   Slot 4 (col1, row1): Potion — second potion (qty 3 + potential anim)
    //   Slot 5 (col1, row2): Key    — single item
    UIDragState::ResetInventoryHarness();
    g_spawnCount = 0;

    auto add = [&](TObjectClass& cls, int32_t objclass, int32_t qty,
                   bool is_bag = false)
    {
        if (g_spawnCount >= kHarnessInvSlots) return;
        const int32_t idx = g_spawnCount;
        const char* nm = nullptr;
        TObjectInstance* inst = SpawnItem(cls, objclass, &nm);
        if (!inst) return;

        SHarnessSlot& slot = UIDragState::harness_inv[idx];
        slot.inst   = inst;
        slot.qty    = qty;
        slot.is_bag = is_bag;
        slot.label  = nm ? nm : cls.ClassName();

        SSpawnMeta& meta = g_spawnMeta[idx];
        meta.label  = slot.label;

        ++g_spawnCount;
    };

    auto addNamed = [&](TObjectClass& cls, int32_t objclass, const char* name,
                        int32_t qty)
    {
        if (g_spawnCount >= kHarnessInvSlots) return;
        const int32_t idx = g_spawnCount;
        const char* nm = nullptr;
        TObjectInstance* inst = SpawnNamedItem(cls, objclass, name, &nm);
        if (!inst) return;

        SHarnessSlot& slot = UIDragState::harness_inv[idx];
        slot.inst   = inst;
        slot.qty    = qty;
        slot.label  = nm ? nm : name;

        SSpawnMeta& meta = g_spawnMeta[idx];
        meta.label  = slot.label;

        ++g_spawnCount;
    };

    addNamed(ArmorClass, OBJCLASS_ARMOR, "Brown Leather Helmet", 1); // slot 0 — col0 row0
    add(FoodClass,   OBJCLASS_FOOD,   5);          // slot 1 — col0 row1 (qty)
    add(KeyClass,    OBJCLASS_KEY,    1);           // slot 2 — col0 row2
    add(MoneyClass,  OBJCLASS_MONEY,  42);         // slot 3 — col1 row0 (qty)
    add(PotionClass, OBJCLASS_POTION, 3);          // slot 4 — col1 row1 (qty)
    add(KeyClass,    OBJCLASS_KEY,    1);           // slot 5 — col1 row2

    log_info("[ui-inventory] harness populated with %d items", g_spawnCount);
}

void BuildInvSlots()
{
    // Construct per-cell TInvSlot objects at the correct pane-local rects.
    // The geometry is fixed regardless of page; the page offset shifts which
    // harness_inv[] entry maps to which cell during paint.
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

        ++g_refreshCount;

        // Lazy icon extraction for each spawned item.
        for (int32_t i = 0; i < g_spawnCount; ++i)
            TryExtractIcon(i);

        // Advance animated icons (24Hz asset cadence).
        if ((g_refreshCount % kAnimStride) == 0)
        {
            for (int32_t i = 0; i < g_spawnCount; ++i)
            {
                SSpawnMeta& meta = g_spawnMeta[i];
                if (!meta.anim || meta.animFrames <= 0) continue;
                const int32_t f = (g_refreshCount / kAnimStride) % meta.animFrames;
                if (PTBitmap bm = meta.anim->GetFrame(f))
                    UIDragState::harness_inv[i].icon = bm;
            }
        }

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

        // Gold readout (spec §5 step 6 / §8 row 1).
        if (g_goldFont)
        {
            const int32_t cellW = kArrowLX - kGoldX;
            const int32_t cellH = (int32_t)(TextLineHeight(g_goldFont) + 0.5f);
            char buf[16];
            std::snprintf(buf, sizeof(buf), "%d$", kGoldValue);
            DrawTextToTarget(g_goldFont, buf,
                             kGoldX, kGoldY, cellW, cellH,
                             ETextAlign::Left,
                             1.0f, 1.0f, 1.0f, tw, th);
        }

        // GoldPile icon (spec §4 / §0 LIVE-VERIFIED #1+#4).
        if (g_goldPile)
            Renderer->DrawBitmapToTarget(g_goldPile, kGoldIconX, kGoldIconY, tw, th);

        // Per-cell item loop (spec §5 step 8 / §6.1 cell kernel).
        // Grid maps cells 0..11 to harness_inv[] slots page..page+11.
        // Each visible cell = column-major position; slot index into harness_inv
        // is page + col*3 + row (spec §6.1: `slot = col*3 + row + page`).
        for (int32_t cell = 0; cell < kGridCells; ++cell)
        {
            const int32_t col  = cell / kGridRows;
            const int32_t row  = cell % kGridRows;
            const int32_t hidx = page + cell;   // harness_inv[] index

            // Bounds check (harness only has kHarnessInvSlots slots).
            if (hidx < 0 || hidx >= kHarnessInvSlots) continue;

            const SHarnessSlot& hs = UIDragState::harness_inv[hidx];

            // #7b: skip drawing this cell if it's the active drag source
            // (the item is following the cursor — its origin shows empty).
            if (dragging && drag.source_idx == hidx) continue;

            if (!hs.inst) continue;   // empty slot — chrome baked border shows through

            // Bind slot content and paint via the shared TInvSlot.
            if (!g_invSlots[cell]) continue;
            TInvSlot* isl = g_invSlots[cell];

            isl->SetItem(hs.inst, hs.icon, hs.qty);

            // #7c: bag contents overlay — if the harness slot is a bag,
            // set the pouch overlay so the inner icon renders.
            if (hs.is_bag && hs.bag_inner)
                isl->SetPouchOverlay(hs.bag_inner);
            else
                isl->SetPouchOverlay(nullptr);

            isl->Draw(g_pane, tw, th, g_countFont);
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
    g_countFont = BuildTTFAtlas(kCountFontPath, kCountFontPx);

    log_info("[ui-inventory] fonts: Gold=%s  count=%s",
             g_goldFont ? "OK" : "MISS", g_countFont ? "OK" : "MISS");

    // Populate harness_inv[] and build per-cell TInvSlot geometry.
    PopulateHarness();
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

    // Clean up per-cell slots.
    for (int32_t i = 0; i < kGridCells; ++i)
    {
        delete g_invSlots[i];
        g_invSlots[i] = nullptr;
    }

    // Clean up spawned instances.
    for (int32_t i = 0; i < g_spawnCount; ++i)
    {
        SHarnessSlot& hs = UIDragState::harness_inv[i];
        if (hs.inst)
        {
            hs.inst->OffScreen();
            delete hs.inst;
        }
        hs = SHarnessSlot{};
        g_spawnMeta[i] = SSpawnMeta{};
    }
    g_spawnCount = 0;
    g_refreshCount = 0;
    g_hudVisible = true;
}
