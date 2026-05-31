// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uiinventorytest.cpp - --test=ui-inventory                            *
// *************************************************************************
//
// Clean-room reconstruction of TInventory (the right-sidebar Inventory
// content pane: the 188x174 4x3 column-major item grid that hangs below the
// upper sidebar content area). Built ONLY from
// docs/ui/forensics/InventoryPane_SPEC.md (with FORENSICS_PROTOCOL,
// NOMENCLATURE, UI_METHOD_MAP §12, RECONSTRUCTION_PROTOCOL).
//
// What this pane draws (spec §1 / §4 / §5 — InventoryPane_SPEC.md):
//   - Inventory chrome (inventory.dat[0], 188x174 RGB555 opaque) at pane
//     local (0,0) — the cells are baked into the chrome bitmap, so the
//     engine does NOT draw per-cell boxes (spec §2 notes).
//   - Container art surface 20x20 at pane (10,10) DM_TRANSPARENT (spec §4).
//     The retail surfaces (mbr_0x1a0 / 0x1a8) carry the live-container
//     portrait; in this isolated harness we draw the Backpack bitmap into
//     the container slot as a stand-in placeholder for "the container being
//     viewed is the player's pack" (spec §14 UNCONFIRMED-F leaves the live
//     content open). The icon stays the real Backpack asset.
//   - Gold "%d$" total at pane (0x50,0xd) = (80,13), shadowed UI font,
//     LEFT-aligned, TOP (spec §8 row 1).
//   - Backpack 20x20 icon at pane (0x28,5) = (40,5), DM_ALPHA (spec §4).
//   - Per occupied cell: the item's InventoryImage() (spec §6 step 6.1 +
//     §5 step 8b), key-transparent at cell (col*45+8, row*44+42), clipped
//     to 40x40 interior.
//   - Per stacked item (Amount > 1): "%d" count text at the cell's top-left
//     corner, shadowed font 0x404, LEFT-aligned, 40-px cell (spec §8 row 2).
//   - Left/Right page arrows (24x24) at pane (0x8c,0xb)=(140,11) and
//     (0xa1,0xc)=(161,12), DM_ALPHA (spec §4). Per-arrow state: enabled
//     when paging is possible (spec §5 step 10). For this isolated harness
//     we render the Up (idle) art for each arrow — there is no real
//     container with > 12 items so the page state stays at 0.
//
// Architecture (spec §3 direct-renderer contract): compose the WHOLE pane
// (chrome + container icon + arrows + gold + 4x3 grid + counts) into ONE
// offscreen TSurface render target via the *ToTarget primitive family, then
// DrawSurface it once in the HUD pass. Mirrors uiplyrstatusbartest /
// uibarinvtest / uibottombartest — the canonical compose-to-target contract.
//
// Primitives (UI_METHOD_MAP §12 — canonical shared toolbox only):
//   Renderer->DrawBitmapToTarget                 — opaque chrome + opaque arrow
//   Renderer->DrawBitmapSubrectStretchedToTarget — per-item icon clipped/sized
//                                                  into the 40x40 cell interior
//   font.h DrawTextShadowedToTarget              — gold + count text (3-pass
//                                                  black shadow, spec §7/§8)
// No hand-rolled glyph walks or shadow passes; no procedural stand-ins.
// (feedback-ui-no-mock-use-retail — the icons MUST come from real spawned
//  item instances through InventoryImage(); no hand-drawn placeholders.)
//
// Test-harness items (spec §1 plain language):
//   The pane is interesting only when populated. We spawn a handful of real
//   retail items via the existing object class registries (Potion, Food,
//   Key, Money) and run them through InventoryImage() so the icon path
//   matches the pane's retail draw exactly. Spawn imagery loads
//   asynchronously, so per-frame TryExtract pulls the icon once it lands
//   (same pattern uiplyrstatusbartest::TryExtractPortrait and
//   uibarinvtest::TryExtractIcon use — [[project-ui-portrait-and-icons]]).
//
// Test placement: the production pane lives at screen (452, 306) — bottom-
// right of a 640x480 Classic screen (spec §3). For this isolated test mode
// at the live display size we right/bottom-anchor with the same intent so
// the chrome reads in the corner where the player will see it in-engine.
//
// *************************************************************************

#include "uiinventorytest.h"

#include "animation.h"
#include "bitmap.h"
#include "display.h"
#include "font.h"
#include "fonttable.h"
#include "imagery.h"
#include "logging.h"
#include "multi.h"
#include "invslot.h"
#include "object.h"
#include "renderer.h"
#include "surface.h"

#include <cstdint>
#include <cstdio>
#include <cstring>

// Item-class registries (defined in their respective .cpp files). The pane
// is content-agnostic — these are sample real items so the test mode has
// something to draw; the production pane gets its items from the player's
// inventory walk (spec §5 step 8 / §6.4 iterator).
extern TObjectClass PotionClass;
extern TObjectClass FoodClass;
extern TObjectClass KeyClass;
extern TObjectClass MoneyClass;

namespace {

// =====================================================================
// Geometry constants — all cited to InventoryPane_SPEC.md.
// =====================================================================

// --- pane rect (spec §3 "Pane on screen") ----------------------------
// Chrome bitmap == pane rect (spec §2 notes: "no chip-model overhang"); the
// chrome IS 188x174. Pane-local origin = (0,0) for every element below.
constexpr int32_t kPaneW = 0xbc;        // 188 — chrome / pane width  (§3)
constexpr int32_t kPaneH = 0xae;        // 174 — chrome / pane height (§3)

// --- grid (spec §4 "Item-slot grid table", §6.1 cell kernel) ---------
// Triple-confirmed: forward (DrawGrid `:537cfc-537d0e`), inverse (MouseClick
// `:406-413`), and Animate (`:537fa0`). Column-major 4 cols x 3 rows,
// pane-local origin (8, 42), pitch 45x44, interior 40x40, slot = col*3 + row.
constexpr int32_t kGridX0     = 0x08;   // 8  — grid origin x (cell 0,0 TL)  (§4)
constexpr int32_t kGridY0     = 0x2a;   // 42 — grid origin y                (§4)
constexpr int32_t kCellPitchX = 0x2d;   // 45 — x stride                     (§4)
constexpr int32_t kCellPitchY = 0x2c;   // 44 — y stride                     (§4)
constexpr int32_t kCellInner  = 0x28;   // 40 — interior (icon clip)         (§4)
constexpr int32_t kGridCols   = 4;
constexpr int32_t kGridRows   = 3;
constexpr int32_t kGridCells  = kGridCols * kGridRows;   // 12

// --- container-icon panel (spec §4 row "container art surf") ---------
// Surface stamp at pane-local (10,10), 20x20, DM_TRANSPARENT. The retail
// surfaces hold the live container portrait; the test harness binds the
// Backpack bitmap into this slot as a content-agnostic placeholder.
constexpr int32_t kContSurfX  = 0x0a;   // 10 — container surf TL x (§4)
constexpr int32_t kContSurfY  = 0x0a;   // 10 — container surf TL y (§4)
constexpr int32_t kContSurfW  = 20;
constexpr int32_t kContSurfH  = 20;

// --- Top-strip sprite at pane (40, 5) — `mbr_0x19c` blit ---------------
//
// Retail truth, verified DIRECTLY from the shipped Revenant.exe (objdump
// -d -M intel --start-address=0x537650 --stop-address=0x537700):
//
//   :53767c  push 0x5e411c     ; "Inventory"   (chrome string)
//   :537693  call 0x46d710     ; meth_0x46d710("Inventory")
//   :5376a3  mov [esi+0x198], eax    ; mbr_0x198 = Inventory chrome
//
//   :53769e  push 0x5e4128     ; "GoldPile"    ← KEY LITERAL
//   :5376a9  call 0x46d710     ; meth_0x46d710("GoldPile")
//   :5376b0  mov [esi+0x19c], eax    ; mbr_0x19c = GoldPile bitmap
//
// So `mbr_0x19c` is unambiguously **GoldPile (32x32)** — NOT Backpack.
// The §2/§4 spec mislabeled the field; Ghidra's OOAnalyzer used the
// numeric load-order naming and the spec author guessed wrong about
// which name maps to which entry. `meth_0x46d710` is FindByName, not
// a sequential iterator (cls_0x46d6b0.cpp:46-78 — walks the multi-
// resource's name table via stricmp). Each call literally takes the
// asset name as its string arg.
//
// Then later at draw time (disasm `…DrawGrid_537a70.disasm.txt:90-96`):
//
//   00537b6a  MOV  ECX, [EDI + 0x19c]          ; load GoldPile pointer
//   00537b70  PUSH EBX                          ; arg6 = 0
//   00537b71  PUSH 0x2000                       ; arg5 = DM_ALPHA
//   00537b76  PUSH ECX                          ; arg4 = bitmap
//   00537b77  PUSH 0x5                          ; arg3 = y = 5
//   00537b79  PUSH 0x28                         ; arg2 = x = 40 (0x28)
//   00537b7b  MOV  ECX, EBP                     ; this = dst surface
//   00537b7d  CALL 0x004bd680                   ; FUN_004bd680(x,y,bm,mode,0)
//
// `FUN_004bd680(x, y, bitmap, drawmode, ?)` is the bare TL-stamp
// primitive — no width/height args, no centering, no alignment math.
// Caller-side centering is the convention (UI_METHOD_MAP §4 example:
// `Ring` icon centered via `(0x1a - bm.w/2, 0x1f - bm.h/2)` in TLogo-
// Screen). For the GoldPile call here, args are bare literals `(0x28,
// 0x5)` — TL stamp at (40, 5) with the 32×32 sprite extending to
// (72, 37). No bottom-align math exists in the retail code path.
//
// `mbr_0x19c` has exactly ONE assignment site (Initialize line 128 of
// cls_0x5a58c0.cpp + binary disasm :5376b0 above) and is never re-
// assigned conditionally — so the sprite identity does NOT change with
// gold amount. The "small/medium gold" gradient hinted at by the
// `smallgold*`/`medgold*` asset families and the `Gold`/`GoldMed`
// BMFONT pair is latent in the inventory pane: only `mbr_0x19c =
// GoldPile` (the single 32×32 sprite) is consumed here.
constexpr int32_t kGoldIconX = 0x28;   // 40 — disasm `:537b79 PUSH 0x28`
constexpr int32_t kGoldIconY = 0x05;   //  5 — disasm `:537b77 PUSH 0x5`

// --- Scroll arrow buttons (spec §4 / §10 hit rects + Init lines) -----
// Each arrow 24x24, DM_ALPHA. Test mode draws the "Up" (idle) art only —
// there is no actual page state to drive Down/Glow in this harness.
constexpr int32_t kArrowLX = 0x8c;      // 140 (Init ctor `:165`)
constexpr int32_t kArrowLY = 0x0b;      // 11
constexpr int32_t kArrowRX = 0xa1;      // 161 (Init ctor `:185`)
constexpr int32_t kArrowRY = 0x0c;      // 12
constexpr int32_t kArrowW  = 0x18;      // 24
constexpr int32_t kArrowH  = 0x18;      // 24

// --- Gold "%d$" text cell (spec §8 row 1) ----------------------------
// Pane-local (0x50,0xd)=(80,13), LEFT, TOP, shadowed UI font. Width is
// auto, but the canonical DrawTextShadowedToTarget takes a cell width for
// alignment; the gold reads "left-aligned starting at x=80", so any cell
// width that fits the digits works. We pass the remaining width to the
// right arrow as the alignment cell so any overflow is visible (not the
// likely case for a synthetic gold value).
constexpr int32_t kGoldX = 0x50;        // 80 (§8)
constexpr int32_t kGoldY = 0x0d;        // 13 (§8)

// --- Count text (per stacked item, spec §8 row 2) --------------------
// Cell (cellX, cellY, 0x28, ~lineH), LEFT, TOP, font 0x404 (shadowed).
// Width = the 40-px cell interior (spec §8 confirms `w=0x28=40` arg at
// `:537e4f`). White over black-shadow renders the count digit cleanly over
// the icon. (UNCONFIRMED-A 2x scaling + UNCONFIRMED-B color source — we use
// white left/top per the cell math; the 2x scale was deemed unlikely to be
// intended (spec §11 retail-bug bullet 2).)
constexpr int32_t kCountCellW = 0x28;   // 40 (§8 / §4)

// --- Fonts ----------------------------------------------------------
// Two distinct fonts here:
//   - Gold readout "<n>g" in the header: BMFONT `Gold` (FONT.DEF line 63
//     — atom `smallgold` + baked shadow `smallgoldshad`).
//
//     Retail truth (binary disasm of FUN_00485870 — the font-loader):
//       :485e3d  push 0x5d8878   ; "Gold"
//       :485e47  mov  [0x65b7d8], eax    ; stored at the small-gold global
//       :485e51  push 0x5d8880   ; "GoldMed"
//       :485e5b  mov  [0x6663dc], eax    ; stored at the medium-gold global
//     The inventory DrawGrid at :537b43 reads `[0x6663dc]` → so retail
//     literally uses **GoldMed** (the medium face). But the medium font's
//     glyphs are ornate runes that don't read as digits at small sizes;
//     the user-preferred visual is the SMALL `Gold` face whose glyphs
//     are conventional gold-colored digits. Per user instruction (this
//     pass), use `Gold` — and document the retail truth here so future
//     edits don't think this was a transcription error.
//
//     The glyphs are baked gold-colored; tint white so the baked color
//     shows through. Drawn via the canonical bitmap-atlas path
//     (BuildFontAtlas(TFont*) + DrawTextToTarget — no extra shadow pass;
//     the bitmap atom includes the shadow already).
//
//   - Stack-count "%d" digits on a grid cell: the canonical Arimo TTF at
//     the stats-bar px size (same face/size uiplyrstatusbartest uses).
//     Two distinct count types share this font but render differently:
//
//       (i) Regular item quantity (potions/weapons/food/etc., Amount>1)
//           → RED text at the cell's TOP-RIGHT corner, w=40 cell width.
//           Retail draw at `:537e4f FUN_004be2b0(cellX, cellY, 0x28,
//           …, font 0x404)`.
//
//      (ii) Bag/pack/pouch container contents count (count of items
//           inside the bag — only on items that ARE containers)
//           → WHITE text CENTERED on the BOTTOM of the bag icon.
//           Retail draw at `:537def FUN_004be2b0(cellX+0x14, cellY+0x1a,
//           textW+2, 0x14, …)` with `FUN_00429950(0xff,0xff,0xff,…)`
//           building the WHITE color pack at `:537db4-:537db9`.
//           The cellX+20 / cellY+26 origin centers the text around the
//           bag-icon's bottom; same arimo font, white with the
//           canonical 3-pass black drop shadow.
//
//     Both use `DrawTextShadowedToTarget` (3 offset black-shadow passes)
//     so the digits read cleanly over the cell's baked chrome bezel and
//     the item icon underneath.
constexpr const char* kGoldFontName  = "Gold";   // user override — retail is GoldMed
constexpr const char* kCountFontPath = "thirdparty/fonts/Arimo-Regular.ttf";
constexpr int32_t     kCountFontPx   = 12;       // same as uiplyrstatusbartest

// =====================================================================
// Asset roster — spec §2.
// =====================================================================
constexpr const char* kArchive       = "inventory.dat";
constexpr const char* kChromeName    = "Inventory";    // §2 idx 0  (188x174)
constexpr const char* kBackpackName  = "Backpack";     // §2 idx 1  (20x20)
constexpr const char* kArrowRUName   = "InvArwRU";     // §2 idx 2  (right Up)
constexpr const char* kArrowLUName   = "InvArwLU";     // §2 idx 5  (left  Up)
constexpr const char* kGoldPileName  = "GoldPile";     // §2 idx 8  (32x32)

// =====================================================================
// Loaded assets + cached RT.
// =====================================================================
TMulti*  g_inventoryDat = nullptr;
PTBitmap g_chrome       = nullptr;
PTBitmap g_backpack     = nullptr;
PTBitmap g_arrowRU      = nullptr;
PTBitmap g_arrowLU      = nullptr;
PTBitmap g_goldPile     = nullptr;
const SFontAtlas* g_goldFont  = nullptr;   // BMFONT "Gold" bitmap face
const SFontAtlas* g_countFont = nullptr;   // Arimo TTF for stack counts

TSurface* g_pane = nullptr;   // composed RT (188x174 — chrome-sized)

// =====================================================================
// Inventory slot style (per-pane visual conventions). Routed through
// the shared TInvSlot class (src/invslot.{h,cpp}).
//
// Style choices (per-pane visual contract for Inventory):
//   - icon stamped at NATIVE size at cell TL (retail Inventory literal)
//   - regular qty: RED, RIGHT-aligned at cell top-right, w=40 (cell)
//   - bag-count : WHITE, CENTER-aligned at cell bottom (cellY + 26)
//   - no empty-cell placeholder (Inventory shows chrome's baked border)
// =====================================================================
SInvSlotStyle MakeInventorySlotStyle()
{
    SInvSlotStyle s;
    s.icon_fit_to_cell     = false;        // retail-faithful TL stamp
    s.icon_fit_inset       = 0;

    // Regular qty: RED top-right, width = cell interior (40)
    s.draw_qty             = true;
    s.qty_r                = 1.00f;
    s.qty_g                = 0.18f;
    s.qty_b                = 0.14f;
    s.qty_align            = ETextAlign::Right;
    s.qty_rect_dx          = 0;
    s.qty_rect_dy          = 0;
    s.qty_rect_w           = kCellInner;   // 40 (the cell interior)
    s.qty_rect_h_pad       = 0;

    // Bag-contents count: WHITE center, at cellY + 26 (retail :537de2
    // LEA EDX, [EBX + 0x1a]). cellX + 20 / textW + 2 / h = 0x14.
    s.draw_bag_count       = true;
    s.bag_r                = 1.0f;
    s.bag_g                = 1.0f;
    s.bag_b                = 1.0f;
    s.bag_align            = ETextAlign::Center;
    s.bag_rect_dx          = 0;
    s.bag_rect_dy          = 26;
    s.bag_rect_w           = kCellInner;
    s.bag_rect_h_pad       = 0;

    // Pouch overlay: native stamp at cellY + 20 (retail :537d83
    // FUN_004bd680(cellX, cellY+0x14, ...)).
    s.pouch_overlay_stretch = false;
    s.pouch_inner_dx        = 0;
    s.pouch_inner_dy        = 20;
    s.pouch_inner_w         = 20;
    s.pouch_inner_h         = 20;

    return s;
}

SInvSlotStyle g_invSlotStyle = MakeInventorySlotStyle();

// =====================================================================
// Per-slot synthetic state.
// =====================================================================
// Mirrors uibarinvtest's pattern. Each populated slot binds one real
// spawned item; per-frame Refresh() lazily extracts InventoryImage() once
// the imagery body has streamed in (async; [[project-ui-portrait-and-icons]]).
//
// Items can carry either:
//   - `invitem` (static bitmap) — `icon` set, `anim` null.
//   - `invanim` (TAnimation)    — `anim` set, `icon` set to the active frame
//                                 each Refresh() (kAnimFramesPerTick stride).
// Some classes (notably potions) carry only `invanim`, so the per-tick
// extraction path is the only way they show up at all.
struct SSlotItem
{
    int32_t          objclass = -1;
    int32_t          objtype  = -1;
    TObjectInstance* inst     = nullptr;
    PTBitmap         icon     = nullptr;     // current frame (or static bitmap)
    PTAnimation      anim     = nullptr;     // non-null => animated
    int32_t          amount   = 1;           // > 1 triggers the "%d" count text
    const char*      label    = "?";
    TInvSlot*        slot     = nullptr;     // shared per-cell sub-control
};

// Animation pacing. 24 frames/sec is the retail asset cadence (matches the
// 24 Hz tick gate the rest of the engine animations use). With Refresh()
// called once per Display.Update() (~60-120 Hz live), divide by an integer
// frame stride so the animation reads slow + smooth in the harness.
constexpr int32_t kRefreshFramesPerAnim = 3; // advance every 3rd Refresh tick

// Up to 12 visible cells per page (spec §4 / §6.1). The first ~6 slots
// populate so the column-major layout reads (slot 0..2 = col 0; slot 3..5 =
// col 1; etc.) — enough variety to exercise icon + count branches without
// requiring a full 12-item catalog.
SSlotItem g_slots[kGridCells];
int32_t   g_slotCount = 0;

// Synthetic gold readout (the retail readout pulls `player.stat("Gold")`,
// spec §5 step 6). Demo value — not load-bearing for the visual contract.
// Retail draws this in the single `GoldMed` BMFONT regardless of amount
// (the latent small `Gold` face is never selected by the inventory pane —
// see kGoldFontName note above), so any non-trivial value exercises the
// readout the same way. 14475 picks a 5-digit value to show the readout's
// rightmost reach inside the 60px cell between the icon and left arrow.
constexpr int32_t kGoldValue = 14475;

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
// streamed in (the imagery body loads async — [[project-ui-portrait-and-
// icons]]). Returns true if it bound either a static `invitem` (icon out)
// OR an animated `invanim` (anim out). State 0 is the canonical inventory
// image; the retail InventoryImage() (FUN_0046f190) tries invitem first
// then falls back to invanim's frame 0, so this helper mirrors that order.
bool TryExtractIcon(TObjectInstance* inst, PTBitmap& icon_out, PTAnimation& anim_out)
{
    icon_out = nullptr;
    anim_out = nullptr;
    if (!inst) return false;
    TObjectImagery* img = inst->GetImagery();
    if (!img || img->NumStates() <= 0) return false;

    // Pass 1: static invitem on any state.
    if (PTBitmap bm = img->GetInvImage(0)) { icon_out = bm; return true; }
    for (int32_t s = 1; s < img->NumStates(); ++s)
        if (PTBitmap bm = img->GetInvImage(s)) { icon_out = bm; return true; }

    // Pass 2: invanim fallback (potions etc. ship only this).
    if (PTAnimation a = img->GetInvAnimation(0))
    {
        anim_out = a;
        icon_out = a->GetFrame(0);  // seed with frame 0; tick advances
        return true;
    }
    for (int32_t s = 1; s < img->NumStates(); ++s)
        if (PTAnimation a = img->GetInvAnimation(s))
        {
            anim_out = a;
            icon_out = a->GetFrame(0);
            return true;
        }

    return false;
}

// =====================================================================
// HUD drawable — composes the pane RT then DrawSurface's it once at the
// bottom-right anchored screen origin (spec §3: production pane sits at
// screen (452,306) in Classic 640x480; the chrome is 188x174 so the right
// edge = 640, bottom = 480 — i.e. flush bottom-right). The test mode keeps
// the same anchor intent for the live display size.
// =====================================================================
class TInventoryHud : public THudDrawable
{
public:
    void Draw() override
    {
        if (!g_pane) return;
        // Spec §3: the chrome's right edge sits at the display right edge,
        // and its bottom sits at the display bottom (Classic literal: pane
        // at (640-188, 480-174) = (452, 306)). Bottom-right anchor.
        const int32_t dw = Display.Width();
        const int32_t dh = Display.Height();
        const int32_t x  = (dw > 0 ? dw : kPaneW) - kPaneW;
        const int32_t y  = (dh > 0 ? dh : kPaneH) - kPaneH;
        Renderer->DrawSurface(g_pane, x, y);
    }

    void Refresh()
    {
        if (!g_chrome) return;
        EnsurePane();
        if (!g_pane) return;

        // Advance the harness tick counter (drives invanim playback).
        ++m_refreshCount;

        // Lazy icon extraction (async imagery load — same retry pattern
        // uibarinvtest + uiplyrstatusbartest use). On first acquire we
        // also detect whether the item is animated (carries `invanim`) so
        // the per-frame loop below can step through frames.
        for (int32_t i = 0; i < g_slotCount; ++i)
        {
            SSlotItem& s = g_slots[i];
            if (s.icon || s.anim) continue;
            TryExtractIcon(s.inst, s.icon, s.anim);
            // Log on first successful bind so the test output reads
            // invitem vs invanim vs neither for the whole roster.
            if (s.icon || s.anim)
            {
                const char* kind = s.anim ? "invanim" : "invitem";
                const int32_t nf = s.anim ? s.anim->NumFrames() : 1;
                log_info("[ui-inventory] slot %d '%s' bound %s (%d frame%s)",
                         i, s.label, kind, nf, nf == 1 ? "" : "s");
            }
        }

        // Advance any active animation's current frame. Stride is
        // kRefreshFramesPerAnim Refresh ticks per animation frame so the
        // motion reads at the asset's intended pace (the harness Refresh
        // cadence is the renderer's frame rate, which is much faster than
        // the 24Hz animation cadence the assets were authored for).
        if ((m_refreshCount % kRefreshFramesPerAnim) == 0)
        {
            for (int32_t i = 0; i < g_slotCount; ++i)
            {
                SSlotItem& s = g_slots[i];
                if (!s.anim) continue;
                const int32_t nf = s.anim->NumFrames();
                if (nf <= 0) continue;
                const int32_t f = (m_refreshCount / kRefreshFramesPerAnim) % nf;
                if (PTBitmap bm = s.anim->GetFrame(f))
                    s.icon = bm;
            }
        }

        const int32_t tw = g_pane->Width();
        const int32_t th = g_pane->Height();

        // Transparent clear so the chrome alone reads against the playfield
        // (the chrome IS opaque, but transparent here is the safe default —
        // the chrome stamps over the cleared pixels in step 4 below).
        g_pane->StartPass(0.0f, 0.0f, 0.0f, 0.0f);

        // Spec §5 step 4: chrome `Inventory` opaque at pane-local (0,0).
        // The cells are baked INTO the chrome art (spec §2 notes: "the
        // engine draws icons on top, it does not draw cell borders").
        Renderer->DrawBitmapToTarget(g_chrome, 0, 0, tw, th);

        // Spec §5 step 5: container art surface. Retail call (disasm
        // `…DrawGrid_537a70.disasm.txt:47-60`):
        //     00537aff  MOV  EDX, [EAX+0x4]      ; surface width
        //     00537afc  MOV  ECX, [EAX+0x8]      ; surface height
        //     00537b04  PUSH 0x100                ; arg7 = DM_TRANSPARENT
        //     00537b09  PUSH ECX                  ; arg6 = surface h
        //     00537b0a  PUSH EDX                  ; arg5 = surface w
        //     00537b0d  PUSH EAX                  ; arg4 = surface ptr
        //     00537b0e  PUSH 0xa                  ; arg3 = y = 10
        //     00537b10  PUSH 0xa                  ; arg2 = x = 10
        //     00537b14  CALL 0x00438df0           ; surface blit wrapper
        // Source is `mbr_0x1a0` (player) or `mbr_0x1a8` (target) per
        // `:537aee/:537af6` — UNCONFIRMED-F what gets painted INTO those
        // surfaces. For this isolated harness, drawing the Backpack
        // bitmap into the slot is the closest content-faithful
        // placeholder without a live game state.
        if (g_backpack)
        {
            Renderer->DrawBitmapSubrectStretchedToTarget(
                g_backpack,
                /*dst*/ kContSurfX, kContSurfY, kContSurfW, kContSurfH,
                /*src*/ 0, 0, g_backpack->width, g_backpack->height,
                tw, th);
        }

        // Spec §5 step 6: gold readout in the header. Retail call
        // (disasm `…DrawGrid_537a70.disasm.txt:73-88`):
        //     00537b43  MOV  EDX, [0x006663dc]   ; font ptr (GoldMed)
        //     00537b62  PUSH 0x50                ; arg2 = x = 80
        //     00537b60  PUSH 0xd                 ; arg3 = y = 13
        //     00537b5e  PUSH 0x1                 ; arg4 = align = LEFT
        //     00537b5d  PUSH EDX                 ; arg5 = font
        //     ...
        //     00537b65  CALL 0x004be110          ; text draw wrapper
        //
        // Format string at `0x5e41a8` is literally `"%d$"` (binary-verified
        // via objdump; the dollar sign IS the retail suffix). My earlier
        // "live-verified %dg" was an unsubstantiated guess — fixed.
        //
        // Font is `[DAT_006663dc]` = retail "GoldMed" (binary-verified at
        // FUN_00485870:485e51). User override here uses the small `Gold`
        // face instead (see kGoldFontName note above) — the medium face's
        // ornate runes don't read as digits at small sizes.
        //
        // The glyphs are pre-tinted gold with the drop shadow painted
        // into the bitmap; tint white so the baked color shows through.
        // NO extra shadow pass: the bitmap atom already includes the
        // shadow.
        if (g_goldFont)
        {
            const int32_t cellW = kArrowLX - kGoldX;     // 140 - 80 = 60 px
            const int32_t cellH = (int32_t)(TextLineHeight(g_goldFont) + 0.5f);
            char buf[16];
            std::snprintf(buf, sizeof(buf), "%d$", kGoldValue);  // retail literal at 0x5e41a8
            DrawTextToTarget(g_goldFont, buf,
                             kGoldX, kGoldY, cellW, cellH,
                             ETextAlign::Left,
                             1.0f, 1.0f, 1.0f, tw, th);   // pass-through tint
        }

        // Spec §5 step 6 (cont): top-strip sprite `mbr_0x19c` at pane
        // (40, 5), DM_ALPHA. Retail call is
        //   FUN_004bd680(0x28, 5, [EDI+0x19c], 0x2000, 0)
        // (disasm `…DrawGrid_537a70.disasm.txt:90-96`) — TL stamp at
        // (40, 5), bitmap's native size, no centering or alignment math.
        // Live-verified the bitmap is GoldPile (32x32), so the blit
        // covers (40, 5)..(72, 37).
        if (g_goldPile)
        {
            Renderer->DrawBitmapToTarget(g_goldPile, kGoldIconX, kGoldIconY, tw, th);
        }

        // Spec §5 step 8: per-cell item loop. The retail pane iterates the
        // container's inventory list (FUN_0046dfb0); each item maps to a
        // slot via its InventNum() field. This test mode binds slot index
        // directly so the column-major layout (slot 0..2 = col 0; 3..5 =
        // col 1; …) reads end-to-end. Each cell routes through the shared
        // TInvSlot class (src/invslot.{h,cpp}) — same primitive BarInv +
        // Equip (next pass) use, per the FONT/COLOR MATRIX in invslot.h.
        for (int32_t slot = 0; slot < g_slotCount && slot < kGridCells; ++slot)
        {
            SSlotItem& s = g_slots[slot];
            // Bind the live item content into the pre-constructed slot.
            // Demo items are all regular (no pouches in the test roster);
            // pane integration (the real game's container walk) will set
            // EInvSlotKind::Pouch via slot.SetPouchOverlay(inner) when
            // a container item carries a bag-contents preview.
            if (s.slot) {
                s.slot->SetItem(s.inst, s.icon, s.amount);
                s.slot->Draw(g_pane, tw, th, g_countFont);
            }
        }

        // Spec §5 step 10: scroll-arrow buttons (24x24, DM_ALPHA). Retail
        // toggles enabled/disabled based on the `mbr_0x188` page offset
        // (left = page>0; right = page<right_limit). The isolated harness
        // has < 12 items so the page state stays at 0 → both arrows render
        // their idle "Up" art; no Down/Glow art is needed here.
        if (g_arrowLU)
            Renderer->DrawBitmapToTarget(g_arrowLU, kArrowLX, kArrowLY, tw, th);
        if (g_arrowRU)
            Renderer->DrawBitmapToTarget(g_arrowRU, kArrowRX, kArrowRY, tw, th);

        g_pane->EndPass();
    }

private:
    void EnsurePane()
    {
        // The chrome is fixed-size (spec §3 "chrome bitmap == pane rect");
        // no scaling, no greedy width.
        if (g_pane) return;
        g_pane = new TSurface(kPaneW, kPaneH, SG_PIXELFORMAT_RGBA8);
    }

    int32_t m_refreshCount = 0;
};

TInventoryHud g_hud;

// =====================================================================
// Spawn a single item from a given class registry. Same shape as
// uibarinvtest::SpawnItemFromClass + uiplyrstatusbartest::SpawnPortraitInstance.
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
    auto add = [&](TObjectClass& cls, int32_t objclass, int32_t amount) {
        if (g_slotCount >= kGridCells) return;
        SSlotItem& s = g_slots[g_slotCount];
        const char* nm = nullptr;
        s.inst = SpawnItemFromClass(cls, objclass, &nm);
        if (!s.inst) return;
        s.objclass = objclass;
        s.amount   = amount;
        s.label    = nm ? nm : cls.ClassName();

        // Construct the per-cell TInvSlot at this slot's pane-local rect.
        // Column-major: slot = col*3 + row (spec §6.1).
        const int32_t col = g_slotCount / kGridRows;
        const int32_t row = g_slotCount % kGridRows;
        const int32_t cx  = col * kCellPitchX + kGridX0;
        const int32_t cy  = row * kCellPitchY + kGridY0;
        s.slot = new TInvSlot(cx, cy, kCellInner, kCellInner,
                              /*allowed_type*/ 0,
                              /*placeholder*/  nullptr,
                              /*style*/        &g_invSlotStyle);

        ++g_slotCount;
    };

    // Choose items that exercise the icon + count branches. Spec §6.1
    // slot order is column-major (slot 0..2 = col 0; 3..5 = col 1; etc.).
    // Slot 0..2 in column 0 will appear stacked vertically.
    //
    // Potions appear twice on purpose: in retail many potion subtypes ship
    // with `invanim` rather than `invitem`, so these slots exercise the
    // animated-icon fallback path (TObjectInstance::InventoryImage's
    // GetInvAnimation branch). Per-Refresh tick stepping advances the
    // current frame (kRefreshFramesPerAnim stride). If GetInvAnimation
    // also returns null for the chosen type the slot is simply blank (no
    // stand-in) — same behavior the retail blit had for un-iconed items.
    add(PotionClass, OBJCLASS_POTION, /*amount*/ 1);   // slot 0, col 0 row 0 (potion — anim?)
    add(FoodClass,   OBJCLASS_FOOD,   /*amount*/ 5);   // slot 1, col 0 row 1 (count)
    add(KeyClass,    OBJCLASS_KEY,    /*amount*/ 1);   // slot 2, col 0 row 2
    add(MoneyClass,  OBJCLASS_MONEY,  /*amount*/ 42);  // slot 3, col 1 row 0 (count)
    add(PotionClass, OBJCLASS_POTION, /*amount*/ 3);   // slot 4, col 1 row 1 (count + anim?)
    add(KeyClass,    OBJCLASS_KEY,    /*amount*/ 1);   // slot 5, col 1 row 2
}

}  // namespace

// =====================================================================
// Entry points (testmodes.cpp links these — keep the signatures).
// =====================================================================
bool InitializeUIInventoryMode()
{
    log_info("[ui-inventory] === TInventory (clean-room from spec) ===");

    // Spec §2: real retail inventory.dat — chrome / backpack / arrows / gold.
    g_inventoryDat = TMulti::LoadMulti((char*)kArchive);
    if (g_inventoryDat)
    {
        g_chrome   = LookupByName(g_inventoryDat, kChromeName);
        g_backpack = LookupByName(g_inventoryDat, kBackpackName);
        g_arrowRU  = LookupByName(g_inventoryDat, kArrowRUName);
        g_arrowLU  = LookupByName(g_inventoryDat, kArrowLUName);
        g_goldPile = LookupByName(g_inventoryDat, kGoldPileName);
    }

    log_info("[ui-inventory] assets: Inventory=%s Backpack=%s "
             "InvArwLU=%s InvArwRU=%s GoldPile=%s",
             g_chrome ? "OK" : "MISS", g_backpack ? "OK" : "MISS",
             g_arrowLU ? "OK" : "MISS", g_arrowRU ? "OK" : "MISS",
             g_goldPile ? "OK" : "MISS");
    if (g_chrome)
        log_info("[ui-inventory] chrome %dx%d (expect 188x174)",
                 g_chrome->width, g_chrome->height);

    // Gold font: BMFONT "Gold" (FONT.DEF — atom `smallgold`). BuildFontAtlas
    // packs the per-glyph bitmap atoms into one GPU atlas; the colored
    // glyphs render via the shared DrawTextToTarget path.
    if (FontTable)
        if (TFont* f = FontTable->Bitmap(kGoldFontName))
            g_goldFont = BuildFontAtlas(f);

    // Stack-count font: Arimo TTF at the stats-bar size.
    g_countFont = BuildTTFAtlas(kCountFontPath, kCountFontPx);

    log_info("[ui-inventory] fonts: Gold(BMFONT)=%s  count(TTF %s @%dpx)=%s",
             g_goldFont ? "OK" : "MISS",
             kCountFontPath, kCountFontPx,
             g_countFont ? "OK" : "MISS");

    SpawnDemoSlots();
    log_info("[ui-inventory] spawned %d demo slot items", g_slotCount);

    delete g_pane;
    g_pane = nullptr;

    Renderer->AddHud(&g_hud, 0.0f);
    return true;
}

void RenderUIInventoryMode()
{
    g_hud.Refresh();

    // Muted slate backdrop so the chrome's bottom-right placement reads in
    // isolation (no playfield behind the pane in test mode).
    Display.BackBuffer()->StartPass(0.18f, 0.20f, 0.26f, 1.0f);
    Display.BackBuffer()->EndPass();
}

void CloseUIInventoryMode()
{
    Renderer->RemoveHud(&g_hud);
    delete g_pane;
    g_pane = nullptr;
    g_chrome = g_backpack = g_arrowRU = g_arrowLU = g_goldPile = nullptr;
    g_inventoryDat = nullptr;
    g_goldFont  = nullptr;
    g_countFont = nullptr;

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
