// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uibarinvtest.cpp - --test=ui-barinv                                  *
// *************************************************************************
//
// Clean-room reconstruction of TBarInvPane (the row of 42x42 quick-use slot
// boxes stamped onto the bottom bar). Built ONLY from
// docs/ui/forensics/BarInvPane_SPEC.md (with FORENSICS_PROTOCOL,
// UI_METHOD_MAP §12, RECONSTRUCTION_PROTOCOL).
//
// What this pane draws (spec §1 / §4 / §5):
//   - BarInvBox (bottombar.dat idx 1, 42x42 RGB555 opaque) stamped N times
//     at slot N starting at bar-pane-local (0xdc=220 + 45*N, 10). Count =
//     (pane_w - 220) / 45 — at Classic 640 → 9 slots; greedy at hi-res.
//   - For each populated slot, the item's icon stamped at the same x as the
//     box. The icon comes from the baked .i3d icon path
//     (oi->InventoryImage() = imagery->GetInvImage(state); the +0x130 getter
//     PlyrStatusBar uses for portraits + the inventory pane uses for items).
//   - For each item with value > 1: a small "%d" label at the slot top
//     (40x2*lineH cell, font_id 0x404 — see spec §8). Color in spec is
//     UNCONFIRMED-B; we use white as a sensible default.
//   - For each Pouch-style item: an inner-item icon overlay 20x20 at
//     bar-local (slot_x, 30), plus a quantity "%d" label in the lower-right
//     (20-wide cell at (slot_x+1, 36), white, font_id 0x400 — spec §8).
//
// Architecture (spec §3 direct-renderer contract): compose the entire row
// (boxes + per-slot icon + per-slot text) into ONE offscreen TSurface RT via
// *ToTarget primitives, then DrawSurface it once in the HUD pass at
// (0, display_h - 60). This mirrors uibottombartest / uiplyrstatusbartest /
// uisidetabstest — the canonical compose-to-target contract.
//
// Primitives (UI_METHOD_MAP §12 — canonical shared toolbox only):
//   Renderer->DrawBitmapToTarget        — opaque box stamp + opaque icon
//   Renderer->DrawBitmapSubrectStretchedToTarget — icon stretched to 40x40
//                                          (canonical "stretch to dst" twin)
//   font.h DrawTextShadowedToTarget     — text with 3-pass black shadow
//                                          (font flag 0x400, spec §8)
// No hand-rolled shadow passes / glyph walks; no procedural stand-ins.
//
// Test-harness items (spec §1 plain language):
//   - The pane is interesting only when populated. We spawn a handful of
//     real retail items via the object class registries (Potion, Food,
//     Key) and run them through InventoryImage() so the icon path matches
//     the pane's retail draw exactly. Spawn imagery loads asynchronously,
//     so per-frame TryExtract pulls the icon once it lands (same pattern
//     uiplyrstatusbartest uses for the portrait).
//
// *************************************************************************

#include "uibarinvtest.h"

#include "bitmap.h"
#include "display.h"
#include "font.h"
#include "imagery.h"
#include "logging.h"
#include "multi.h"
#include "object.h"
#include "renderer.h"
#include "surface.h"
#include "invslot.h"

#include <cstdint>
#include <cstdio>
#include <cstring>

// Item-class registries (defined in their respective .cpp files). The pane
// is content-agnostic — these are sample real items so the test mode has
// something to draw; the production pane gets its items from the player's
// inventory (spec §5 step 6/7).
extern TObjectClass PotionClass;
extern TObjectClass FoodClass;
extern TObjectClass KeyClass;
extern TObjectClass InvContainerClass;

namespace {

// =====================================================================
// Geometry constants — all cited to BarInvPane_SPEC.md.
// =====================================================================

// --- pane (spec §3 / §4) ---------------------------------------------
// Pane height tracks the bottom bar at 60 (`0x3c`). The pane sits at
// bar-pane-local origin (0,0); its on-screen y is display_h - 60.
constexpr int32_t kBarH       = 0x3c;   // 60 — bottom-bar / pane height (§3)

// --- slot row layout (spec §4 "static element layout" / §6 cell-origin) -
// Triple-confirmed (forward in Draw `:1442`, inverse in Hover `:cdf6`, and
// Drag `:1559`): row origin (220, 10), pitch x 45, box bitmap 42x42, slot
// count = (pane_w - 220) / 45.
constexpr int32_t kSlotX0     = 0xdc;   // 220 — row origin x (slot 0 TL)
constexpr int32_t kSlotY      = 0x0a;   // 10  — row origin y (slot TL within pane)
constexpr int32_t kSlotPitchX = 0x2d;   // 45  — slot stride
constexpr int32_t kBoxW       = 42;     // BarInvBox bitmap (§2; measured)
constexpr int32_t kBoxH       = 42;

// --- Pouch inner-item icon overlay (spec §4 / §6 Pouch branch) -------
// `meth_0x4bd680(target, slot_x, 0x1e, tint_scratch, 0x100)` — 20x20 stamp
// at bar-local (slot_x, 30). The retail path tints into a 20x20 scratch via
// chroma-key; we let the bitmap's own alpha drive the composite (no stand-in).
constexpr int32_t kInnerIconW = 20;
constexpr int32_t kInnerIconH = 20;
constexpr int32_t kInnerIconY = 0x1e;   // 30 (§4)

// --- per-slot item icon (the item's self-draw) -----------------------
// Retail calls `item.vtable+0x108(slot_x, 10, target)` — the item draws its
// own face into the slot. The Pouch-as-bag uses a 42x42 face; everyday
// items (potions/keys/food) use a smaller icon stretched into the slot
// interior. The slot's interior hit rect is 40x40 (Hover §4: `cmp $0x28`
// on y), so we draw the icon into a 40x40 box centred on the slot for the
// non-Pouch items. The retail pane defers to the item's own draw method;
// the InventoryImage() bitmap is the same source used by every other
// inventory render path — see [[project-ui-portrait-and-icons]].
constexpr int32_t kItemIconInset = 1;   // small inset so the chrome bezel reads

// --- text cells (spec §8) --------------------------------------------
// Pouch quantity (per Pouch slot): (slot_x + 1, 36) cell (20, lineH+2),
// font_id 0x400 (HDC index 0, drop-shadow bit 0x400), white.
// Value text (any item, val>1): (slot_x, 10) cell (40, 2*lineH), font_id
// 0x404 (HDC index 4, drop-shadow bit 0x400), color UNCONFIRMED-B (we use
// a sensible white-ish since the precise color source is not pinned).
constexpr int32_t kQtyCellX0   = 0xdc + 1;       // 221 — slot 0 (slot_x + 1)
constexpr int32_t kQtyCellY    = 0x24;           // 36
constexpr int32_t kQtyCellW    = 0x14;           // 20
constexpr int32_t kQtyCellHPad = 2;              // h = lineH + 2 (spec §8 / §6)

constexpr int32_t kValCellX0   = 0xdc;           // 220 — slot 0
constexpr int32_t kValCellY    = 0x0a;           // 10
constexpr int32_t kValCellW    = 0x28;           // 40
constexpr int32_t kValCellHMul = 2;              // h = lineH * 2 (spec §8 / §6)

// Font: Arimo-Regular at ~12 px — the Arial-metric-compatible font used
// throughout the HUD. spec §8 calls out two HDC indices (0 / 4); we use
// the single shared Arimo atlas (UI_METHOD_MAP §12) for both, since
// font-id 0x404's exact pt-size mapping is UNCONFIRMED-I and the visible
// difference at HUD scale is negligible.
constexpr const char* kFontPath = "thirdparty/fonts/Arimo-Regular.ttf";
constexpr int32_t     kFontPx   = 11;            // tiny labels in a 42-tall box

// =====================================================================
// BarInv slot style (per-pane visual conventions). Routed through
// the shared TInvSlot class (src/invslot.{h,cpp}).
//
// Style choices (per-pane visual contract for BarInv):
//   - icon STRETCHED into a 40x40 box (cell_w-2*inset square) centered
//     in the slot — BarInv's longstanding "fit icon to box" behavior.
//   - regular qty: WHITE, LEFT-aligned, at cell TL (slot-x, kValCellY)
//     with cell width 40 — BarInv's existing convention (spec §8 row 2).
//   - bag-count: WHITE, LEFT-aligned at (slot_x+1, kQtyCellY) with
//     cell width 20 — BarInv's pouch quantity (spec §8 row 1).
//   - no empty-cell placeholder.
// =====================================================================
SInvSlotStyle MakeBarInvSlotStyle()
{
    SInvSlotStyle s;

    // BarInv stretches icons into the 40x40 box interior (inset 1).
    s.icon_fit_to_cell     = true;
    s.icon_fit_inset       = kItemIconInset;

    // Regular qty: WHITE left, cell (slot_x, kValCellY=10) w=40.
    // qty_rect_dy from cell TL — BarInv's cell TL is (slot_x, kSlotY=10),
    // and the value cell IS at (slot_x, kValCellY=10), so dy = 0.
    s.draw_qty             = true;
    s.qty_r                = 1.0f;
    s.qty_g                = 1.0f;
    s.qty_b                = 1.0f;
    s.qty_align            = ETextAlign::Left;
    s.qty_rect_dx          = 0;
    s.qty_rect_dy          = 0;
    s.qty_rect_w           = kValCellW;     // 40
    s.qty_rect_h_pad       = 0;             // lineH is already enough

    // Bag-contents count: WHITE left at (slot_x+1, kQtyCellY=36) cell 20.
    // BarInv cell TL is (slot_x, kSlotY=10), so bag rect dx=+1, dy=26.
    s.draw_bag_count       = true;
    s.bag_r                = 1.0f;
    s.bag_g                = 1.0f;
    s.bag_b                = 1.0f;
    s.bag_align            = ETextAlign::Left;
    s.bag_rect_dx          = 1;
    s.bag_rect_dy          = kQtyCellY - kSlotY;   // 36 - 10 = 26
    s.bag_rect_w           = kQtyCellW;
    s.bag_rect_h_pad       = kQtyCellHPad;

    // Pouch overlay: stretched 20x20 at (slot_x, 30) → dy from cell TL = 20.
    s.pouch_overlay_stretch = true;
    s.pouch_inner_dx        = 0;
    s.pouch_inner_dy        = kInnerIconY - kSlotY;  // 30 - 10 = 20
    s.pouch_inner_w         = kInnerIconW;
    s.pouch_inner_h         = kInnerIconH;

    return s;
}

SInvSlotStyle g_barInvSlotStyle = MakeBarInvSlotStyle();

// =====================================================================
// Asset roster — spec §2.
// =====================================================================
constexpr const char* kArchive    = "bottombar.dat";
constexpr const char* kBoxName    = "BarInvBox";   // §2 idx 1

TMulti*  g_bottombarDat = nullptr;
PTBitmap g_barInvBox    = nullptr;
const SFontAtlas* g_font = nullptr;

TSurface* g_pane  = nullptr;
int32_t   g_paneW = 0;        // live pane width (= display width, spec §3)

// =====================================================================
// Per-slot synthetic state.
// =====================================================================
//
// Each entry represents one populated slot. The real pane walks the
// player's inventory and filters on slot_id (`field_7c - 0x10b`); since
// this test harness has no Player, we just bind a real spawned item per
// slot. The Pouch overlay branch in the retail pane is keyed on the item
// name being "Pouch" — for the test we drive the overlay synthetically on
// the slot index, so the rendering paths exercise without relying on real
// pouch contents.
//
// Item categories (spec §1 "what shows where"):
//   - kIconOnly:    a plain icon (most items — potions, keys, food)
//   - kIconValue:   icon + value label (val>1; e.g. food-stack, ammo)
//   - kIconPouch:   icon + inner-item overlay + quantity label
//
// The "value" and "quantity" sources in the real pane are vtable getters
// on the item (vtable+0x198 / TPlayScreen::meth_0x470040). We drive them
// from the harness so the visual exercise is complete.
// =====================================================================
enum ESlotKind { kIconOnly, kIconValue, kIconPouch };

struct SSlotItem
{
    int32_t          objclass = -1;
    int32_t          objtype  = -1;
    TObjectInstance* inst     = nullptr;
    PTBitmap         icon     = nullptr;
    PTBitmap         inner    = nullptr;     // populated for Pouch slots
    ESlotKind        kind     = kIconOnly;
    int32_t          value    = 1;           // for kIconValue
    int32_t          qty      = 1;           // for kIconPouch
    const char*      label    = "?";
    TInvSlot*        slot     = nullptr;     // shared per-cell sub-control
};

// 4-6 demo slots — chosen to populate the leftmost cells with a mix of
// kinds so the icon-only / icon+value / pouch+qty branches all render.
SSlotItem g_slots[6];
int32_t   g_slotCount = 0;

// =====================================================================
// Asset lookup helper (same shape as the other ui*test panes).
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

// Per-frame: pull the spawned items' baked icons once their imagery has
// streamed in. Mirrors uiplyrstatusbartest's TryExtractPortrait pattern
// ([[project-ui-portrait-and-icons]] — the imagery body loads async, so we
// retry each frame until it lands). The state-0 icon is the canonical
// "inventory image" for non-character objects.
PTBitmap TryExtractIcon(TObjectInstance* inst)
{
    if (!inst) return nullptr;
    TObjectImagery* img = inst->GetImagery();
    if (!img || img->NumStates() <= 0) return nullptr;
    // GetInvImage(state, num=0) — spec [[project-ui-portrait-and-icons]];
    // the same getter every inventory pane / paperdoll uses. State 0 is the
    // baked icon for static items (no animation states).
    if (PTBitmap bm = img->GetInvImage(0))
        return bm;
    // Probe further states defensively (some items have the icon on a
    // non-zero state — same defensive sweep PlyrStatusBar uses).
    for (int32_t s = 1; s < img->NumStates(); ++s)
        if (PTBitmap bm = img->GetInvImage(s))
            return bm;
    return nullptr;
}

// =====================================================================
// HUD drawable — composes the slot row RT then DrawSurface's it once
// at the bottom-anchored screen origin (spec §3 / §5 / §6).
// =====================================================================
class TBarInvHud : public THudDrawable
{
public:
    void Draw() override
    {
        if (!g_pane) return;
        // Spec §3: pane_y = display_h - 60 (BL-anchored to bottom bar).
        const int32_t dh = Display.Height();
        const int32_t y  = (dh > 0 ? dh : kBarH) - kBarH;
        Renderer->DrawSurface(g_pane, 0, y);
    }

    void Refresh()
    {
        if (!g_barInvBox) return;
        EnsurePane();
        if (!g_pane) return;

        // Per-frame icon extraction (async imagery load — [[project-ui-
        // portrait-and-icons]]).
        for (int32_t i = 0; i < g_slotCount; ++i)
        {
            SSlotItem& s = g_slots[i];
            if (!s.icon)  s.icon  = TryExtractIcon(s.inst);
            if (s.kind == kIconPouch && !s.inner) s.inner = s.icon; // overlay
        }

        const int32_t tw = g_pane->Width();
        const int32_t th = g_pane->Height();

        // Transparent clear so only the chrome + icons read against the
        // playfield. The real pane drops onto the bottom-bar plate (which
        // is itself opaque); the test mode draws against a backdrop, so
        // transparent here is correct.
        g_pane->StartPass(0.0f, 0.0f, 0.0f, 0.0f);

        // Spec §5 step 2: recompute slot count = (pane_w - 220) / 45.
        const int32_t visibleCount = (tw - kSlotX0) / kSlotPitchX;
        if (visibleCount <= 0) { g_pane->EndPass(); return; }

        // Spec §5 step 3 (box row, `:1439-1446`): for N in [0..count):
        //   meth_0x4bd680(target, 0xdc + 0x2d*N, 0xa, BarInvBox, opaque).
        // Opaque copy → DrawBitmapToTarget (UI_METHOD_MAP §4 / §12).
        for (int32_t N = 0; N < visibleCount; ++N)
        {
            const int32_t boxX = kSlotX0 + kSlotPitchX * N;
            Renderer->DrawBitmapToTarget(g_barInvBox, boxX, kSlotY, tw, th);
        }

        // Spec §5 step 7 (per-item loop). The real pane iterates the
        // player's inventory and decides which slot each item belongs to
        // via `field_7c - 0x10b`. The test mode binds slot index directly.
        // Each slot routes through the shared TInvSlot class
        // (src/invslot.{h,cpp}) — same primitive Inventory + Equip
        // (next pass) use, per the FONT/COLOR MATRIX in invslot.h.
        for (int32_t N = 0; N < g_slotCount && N < visibleCount; ++N)
        {
            SSlotItem& s = g_slots[N];
            if (!s.slot) continue;

            // Bind the live content. BarInv's three kinds map onto
            // TInvSlot's two-kind model:
            //   kIconOnly   → Regular with qty=1 (qty branch skipped)
            //   kIconValue  → Regular with qty=value (qty branch draws)
            //   kIconPouch  → Pouch (pouch_inner + bag-count branches)
            const int32_t qtyOrCount = (s.kind == kIconPouch) ? s.qty
                                       : (s.kind == kIconValue ? s.value : 1);
            s.slot->SetItem(s.inst, s.icon, qtyOrCount);
            if (s.kind == kIconPouch)
                s.slot->SetPouchOverlay(s.inner);
            s.slot->Draw(g_pane, tw, th, g_font);
        }

        g_pane->EndPass();
    }

private:
    void EnsurePane()
    {
        // Spec §3: live pane width tracks display width. The slot row is
        // a sparse chunk of that width starting at x=220; the empty space
        // to the left (the spell-ring area) is left untouched by this
        // pane (it belongs to TQuickSpellPane / the bottom bar chrome).
        const int32_t dw   = Display.Width();
        const int32_t want = (dw > 0) ? dw : 640;
        if (g_pane && g_paneW == want) return;
        delete g_pane;
        g_paneW = want;
        g_pane = new TSurface(g_paneW, kBarH, SG_PIXELFORMAT_RGBA8);
    }
};

TBarInvHud g_hud;

// =====================================================================
// Spawn a single item from a given class. The class registries (Potion,
// Food, Key) are populated at boot from the IMAGERY.DAT / OBJECTS.DAT
// archives; we walk the class's type table and pick the first type whose
// imagery loads with a non-null inv-image. Same shape as the portrait-
// instance spawn in uiplyrstatusbartest.
// =====================================================================
TObjectInstance* SpawnItemFromClass(TObjectClass& cls, int32_t objclass,
                                    const char** name_out)
{
    const int32_t numTypes = cls.NumTypes();
    for (int32_t i = 0; i < numTypes; ++i)
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

void SpawnDemoSlots()
{
    g_slotCount = 0;
    auto add = [&](TObjectClass& cls, int32_t objclass, ESlotKind kind,
                   int32_t v, int32_t q) {
        if (g_slotCount >= (int32_t)(sizeof(g_slots) / sizeof(g_slots[0])))
            return;
        SSlotItem& s = g_slots[g_slotCount];
        const char* nm = nullptr;
        s.inst = SpawnItemFromClass(cls, objclass, &nm);
        if (!s.inst) return;
        s.objclass = objclass;
        s.kind     = kind;
        s.value    = v;
        s.qty      = q;
        s.label    = nm ? nm : cls.ClassName();

        // Construct the per-cell TInvSlot at this slot's pane-local rect.
        const int32_t slotX = kSlotX0 + kSlotPitchX * g_slotCount;
        s.slot = new TInvSlot(slotX, kSlotY, kBoxW, kBoxH,
                              /*allowed_type*/ 0,
                              /*placeholder*/  nullptr,
                              /*style*/        &g_barInvSlotStyle);

        ++g_slotCount;
    };

    // Slot 0: a plain potion (icon only).
    add(PotionClass, OBJCLASS_POTION, kIconOnly, 1, 1);
    // Slot 1: an icon + value label (e.g. food stack of 12).
    add(FoodClass, OBJCLASS_FOOD, kIconValue, 12, 1);
    // Slot 2: a Pouch-style slot (icon + inner overlay + qty 7).
    add(InvContainerClass, OBJCLASS_INVCONTAINER, kIconPouch, 1, 7);
    // Slot 3: a key (icon only — typical solo quick-use).
    add(KeyClass, OBJCLASS_KEY, kIconOnly, 1, 1);
}

}  // namespace

// =====================================================================
// Entry points (testmodes.cpp links these — keep the signatures).
// =====================================================================
bool InitializeUIBarInvMode()
{
    log_info("[ui-barinv] === TBarInvPane (clean-room from spec) ===");

    // Spec §2: real retail bottombar.dat — BarInvBox at idx 1.
    g_bottombarDat = TMulti::LoadMulti((char*)kArchive);
    if (g_bottombarDat)
        g_barInvBox = LookupByName(g_bottombarDat, kBoxName);

    log_info("[ui-barinv] assets: BarInvBox=%s", g_barInvBox ? "OK" : "MISS");
    if (g_barInvBox)
        log_info("[ui-barinv] BarInvBox %dx%d (expect 42x42)",
                 g_barInvBox->width, g_barInvBox->height);

    g_font = BuildTTFAtlas(kFontPath, kFontPx);
    log_info("[ui-barinv] font %s @%dpx = %s",
             kFontPath, kFontPx, g_font ? "OK" : "MISS");

    // Spawn a small set of real items so the icon + text branches all
    // render. The imagery streams in asynchronously; per-frame Refresh
    // calls TryExtractIcon to pull the baked .i3d face once it lands.
    SpawnDemoSlots();
    log_info("[ui-barinv] spawned %d demo slot items", g_slotCount);

    delete g_pane;
    g_pane  = nullptr;
    g_paneW = 0;

    Renderer->AddHud(&g_hud, 0.0f);
    return true;
}

void RenderUIBarInvMode()
{
    g_hud.Refresh();

    // Same muted slate backdrop as ui-bottombar so the slot row's
    // bottom-anchored placement reads in isolation (no playfield behind
    // it in test mode).
    Display.BackBuffer()->StartPass(0.18f, 0.20f, 0.26f, 1.0f);
    Display.BackBuffer()->EndPass();
}

void CloseUIBarInvMode()
{
    Renderer->RemoveHud(&g_hud);
    delete g_pane;
    g_pane  = nullptr;
    g_paneW = 0;
    g_barInvBox    = nullptr;
    g_bottombarDat = nullptr;
    g_font         = nullptr;

    for (int32_t i = 0; i < g_slotCount; ++i)
    {
        SSlotItem& s = g_slots[i];
        if (s.inst)
        {
            s.inst->OffScreen();
            delete s.inst;
        }
        delete s.slot;
        s = SSlotItem{};
    }
    g_slotCount = 0;
}
