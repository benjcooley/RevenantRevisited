// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uiquickspelltest.cpp - --test=ui-quickspell                          *
// *************************************************************************
//
// Clean-room reconstruction of TQuickSpellPane (cls_0x5a5a30) — the 4-slot
// quick-spell ring strip on the left of the bottom utility bar. Built ONLY
// from docs/ui/forensics/QuickSpellPane_SPEC.md (with NOMENCLATURE,
// UI_METHOD_MAP §12, RECONSTRUCTION_PROTOCOL). Every coordinate, asset,
// drawmode, and shadow cites its spec section inline.
//
// What this pane draws (spec §1 / §4 / §5):
//   - Four 48x48 gold ring buttons in a row at pane-local x = {10, 60, 110,
//     160}, y = 10 (spec §3 frame table, init.cpp:31/46/61/78 + per-slot
//     DAT_005e4fe8/fec/ff0/ff4 / 005e4ff8/ffc/5000/5004). Ring sprite is
//     SpellIcons.dat:RingU/D/G (48x48, flags=0x104 = BM_16BIT + BM_ALPHA;
//     kc=0 — alpha-keyed, NOT magenta-keyed; do NOT apply the global kc —
//     spec §2 / §7 / project-retail-shadow-semantics).
//   - Base sprite: RingU (mbr_0x9c). Disabled slot: RingG (mbr_0xa8 → set
//     when `flag & 0x4`). Pressed slot: RingD (mbr_0xa0 → set when
//     `flag & 0x10000`). Selection logic in slot 21 `0x5444c0` toggles
//     the disabled bit based on whether the player knows + can cast the
//     bound spell — spec §5/§6b.
//   - **UNCONFIRMED-D** content: per ring, the centered 40x40 spell-circle
//     icon (SpellIcons.dat keyed by spell name; spec §2 / §4 / §14-D),
//     and a 2-line cream spell-name label under each ring (per
//     `docs/ui/sample_screen_1.jpg` reference image — spec §8 / §14-D).
//     Slot 20 `0x5444a0` (per-tick relayout) is NOT extracted in the recon
//     snapshot, so the precise compose host for these is unknown. The
//     reference image proves they exist; this harness draws them in the
//     same Overlay pass to match the visible retail panel.
//
// Architecture (spec §3 surfaces): compose the strip into ONE offscreen
// TSurface RT via the *ToTarget primitive family, then DrawSurface it once
// in the HUD pass at (0, display_h - 60). Same direct-renderer contract
// as uibarinvtest / uiplyrstatusbartest / uisidetabstest.
//
// Primitives (UI_METHOD_MAP §12 — canonical shared toolbox only):
//   Renderer->DrawBitmapToTarget        — alpha-keyed ring stamp + icon stamp
//   font.h DrawTextShadowedToTarget     — 3-pass black shadow + cream top
// No hand-rolled shadow passes / glyph walks; no procedural stand-ins
// (feedback_no_standins / feedback_ui_no_mock_use_retail).
//
// Test harness driver:
//   - Synthetic 4-slot binding (Advanced Healing, Iron Skin, Fire Flash,
//     Ice Bolt — matching the reference image labels exactly so visual
//     verification against retail is direct).
//   - A "pressed" slot cycles every 1.5s (exercises RingD state per slot)
//     and a "disabled" slot cycles every 2.0s offset (exercises RingG +
//     greyed-out icon — same path the retail slot-21 sets when the player
//     can't cast a bound spell).
//
// *************************************************************************

#include "uiquickspelltest.h"

#include "bitmap.h"
#include "display.h"
#include "font.h"
#include "logging.h"
#include "multi.h"
#include "renderer.h"
#include "surface.h"
#include "time.h"

#include <cstdint>
#include <cstdio>
#include <cstring>

namespace {

// =====================================================================
// Geometry constants — all cited to QuickSpellPane_SPEC.md.
// =====================================================================

// --- pane rect (spec §3 ctor + frame table) --------------------------
// ctor cls_0x5a5a30.cpp:32-39 / FUN_00488580:5-12 sets
//   mbr_0x4=0, mbr_0x8=0x1a4(420), mbr_0xc=0x280(640), mbr_0x10=0x3c(60)
// → bottom-anchored 60-tall strip, x=0, w follows the bar (greedy).
constexpr int32_t kPaneH = 0x3c;   // 60 — pane height (spec §3, retail bar plate)

// Test-mode RT height extends above the bar so the 2-line labels (which
// sit ABOVE the rings in the reference image — `docs/ui/sample_screen_1.jpg`
// shows the cream text labels on the playfield, NOT on the bar plate)
// have room to render. The bar-plate portion stays the bottom 60 px;
// the labels live in the extra top margin and DrawSurface bottom-anchors
// the whole RT to display_h - kStripH.
// Two-line label layout: per user 2026-05-30 ("spell names split by word,
// first word above ring, second word below"), the strip RT extends both
// ABOVE the bar plate (for line 1) and BELOW the ring (for line 2). Line 2
// sits at the bottom of the bar plate / partially below it on the playfield.
constexpr int32_t kLabelMarginAbove = 22;   // RT room above bar for line 1
constexpr int32_t kLabelMarginBelow = 18;   // RT room below ring for line 2
constexpr int32_t kStripH      = kPaneH + kLabelMarginAbove;
// kStripH used for RT allocation; the below-bar label overlap is handled
// via overdraw onto the bar's bottom edge / playfield (no extra RT pixels).

// The 4 rings only occupy pane-x [10, 208]; the rest of pane width is
// owned by the host bar's BarInv slots (spec §3 spacing arithmetic).
// We size the RT to cover the used left segment plus a margin so the
// 2-line text labels under each ring fit.
// Width = 10 (left margin) + 4*50 (slot pitch) + 2 (right slack) = 212.
constexpr int32_t kStripW = 212;

// --- per-slot button layout (spec §3 frame table / §4) ---------------
// init.cpp:31/46/61/78 — per-slot x literals DAT_005e4fe8/fec/ff0/ff4
// (0x0a/0x3c/0x6e/0xa0 = 10/60/110/160). All four y = 0x0a (10) within
// the bar-pane. Since our RT is taller (label margin on top), offset
// the ring y by the label margin so the bar-relative coord stays the
// retail literal.
constexpr int32_t kBtnCount = 4;
constexpr int32_t kBtnX[kBtnCount] = { 0x0a, 0x3c, 0x6e, 0xa0 };  // 10, 60, 110, 160
// Retail bar-y literal is 0x0a (10). We deviate to 0 so the 48-tall ring
// occupies the upper portion of the 60-tall bar plate, freeing the lower
// 12 px for line 2 of the spell-name label (per user 2026-05-30: "second
// word goes below the ring"). With the retail literal of 10 the ring at
// bar-y 10..58 leaves only 2 px below — line 2 gets clipped.
constexpr int32_t kBtnYInBar       = 0;
constexpr int32_t kBtnY            = kBtnYInBar + kLabelMarginAbove; // RT-local y

// --- ring sprite + click rect (spec §3 hit-rect-vs-sprite note) ------
// Ring sprites are 48x48 stamped at (mbr_0x60, mbr_0x64); the inner
// 32x32 is the TButton click-rect (cls_0x5a3ab8 ctor passes w=h=0x20).
constexpr int32_t kRingW = 48;
constexpr int32_t kRingH = 48;

// --- spell icon inside the ring (spec §4 UNCONFIRMED-D / §2 table) ---
// 40x40 from SpellIcons.dat keyed by spell name (SpellbookPane_SPEC §2;
// dump idxs 0..51). Centered in the 48x48 ring → offset (+4, +4) from
// the ring TL.
constexpr int32_t kIconW       = 40;
constexpr int32_t kIconH       = 40;
constexpr int32_t kIconOffset  = 4;    // (48 - 40) / 2 — center in ring

// --- spell-name label cell ABOVE each ring (spec §8 UNCONFIRMED-D /
// reference docs/ui/sample_screen_1.jpg) ------------------------------
// The reference shows a 2-line cream label tightly above each ring —
// the labels float on the playfield ABOVE the bar plate (not on the
// bar itself), so the label region lives in the RT's top margin
// (RT-local y < kLabelMargin). Cell width is slightly wider than the
// ring so short names like "IronSkin" can fit on one line.
constexpr int32_t kLabelCellW  = 56;
constexpr int32_t kLabelCellH  = 14;   // ~= one line height for 10px font
constexpr int32_t kLabelDX     = -4;   // label cell starts 4px left of btn TL

// Cream/pink text color matching SpellbookPane spell-name (spec §8 of
// SpellbookPane_SPEC, RGB(0xff,0xe7,0xf2)) — reference image labels are
// the same cream as the spellbook headings.
constexpr float kLabelR = 0xFF / 255.0f;
constexpr float kLabelG = 0xE7 / 255.0f;
constexpr float kLabelB = 0xF2 / 255.0f;

// Font: same Arimo atlas the rest of the HUD uses — Arial-metric-compatible
// TTF; size picked to fit the reference image's ~10px label height.
constexpr const char* kFontPath = "thirdparty/fonts/Arimo-Regular.ttf";
constexpr int32_t     kFontPx   = 10;

// =====================================================================
// Asset roster — spec §2 (all from SpellIcons.dat).
// =====================================================================
constexpr const char* kArchive = "SpellIcons.dat";
constexpr const char* kRingUName = "RingU";   // spec §2 idx 52 — base
constexpr const char* kRingDName = "RingD";   // spec §2 idx 53 — pressed
constexpr const char* kRingGName = "RingG";   // spec §2 idx 54 — disabled

TMulti*  g_spellIconsDat = nullptr;
PTBitmap g_ringU         = nullptr;
PTBitmap g_ringD         = nullptr;
PTBitmap g_ringG         = nullptr;

const SFontAtlas* g_font = nullptr;

TSurface* g_pane = nullptr;
double    g_lastTickMs = 0.0;

// =====================================================================
// Per-slot synthetic binding — spec §6b reads these from the player's
// quick-spell array (`field_0x2cc + slot*6`, meth_0x51b560). This
// harness binds directly so the icon/text branches all render without
// needing a live Player object.
//
// Names MUST be SpellIcons.dat entry names (the asset is keyed by name —
// spec §2). Reference image labels are visual wraps of the same names:
// "Advanced healing" wraps "Advanced Healing", "IronSkin" is a compact
// alias for "Iron Skin", etc.
// =====================================================================
struct SQuickSlot
{
    const char* spellName;    // SpellIcons.dat entry name
    const char* labelLine1;   // text-wrapped 2-line label (reference image)
    const char* labelLine2;
    PTBitmap    icon;         // cached on init
};

SQuickSlot g_slots[kBtnCount] = {
    // Reference image bottom-left labels: "Advanced/healing", "IronSkin",
    // "Fire/Flash", "Ice/Bolt".
    { "Advanced Healing", "Advanced", "healing", nullptr },
    { "Iron Skin",        "IronSkin", "",        nullptr },
    { "Fire Flash",       "Fire",     "Flash",   nullptr },
    { "Ice Bolt",         "Ice",      "Bolt",    nullptr },
};

// =====================================================================
// Per-slot state — spec §5/§6c flag word (mbr_0x14 bits 0x4 = disabled,
// 0x10000 = pressed). The retail per-frame Overlay (slot 21) toggles
// these based on player.knows-spell / pressed-input. Cycled here so a
// single capture exercises Up/Down/Glow all three sprites.
//
// Synthetic driver advances at 24Hz protocol-rule-6 sim tick.
// =====================================================================
constexpr double kSimTickMs = 1000.0 / 24.0;

struct SSlotState
{
    bool pressed  = false;   // RingD overlays the base RingU when set
    bool disabled = false;   // RingG replaces RingU when set
};

SSlotState g_slotState[kBtnCount];

// =====================================================================
// Asset lookup helper — same shape as the other ui*test panes.
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

// =====================================================================
// HUD drawable — composes the 4-ring strip RT then DrawSurface's it
// once at the bottom-anchored screen origin (spec §3).
// =====================================================================
class TQuickSpellHud : public THudDrawable
{
public:
    void Draw() override
    {
        if (!g_pane) return;
        // Spec §3: pane_y = display_h - 60 (BL-anchored to bottom bar).
        // Pane x = 0 (left-anchored, ctor mbr_0x4 = 0). Our RT is taller
        // than the retail pane (extra top margin for the labels which
        // float above the bar plate) — anchor the RT so its bar-region
        // bottom edge lands at display_h, which means RT-y = display_h
        // − kStripH (the labels extend kLabelMargin px ABOVE the bar).
        const int32_t dh = Display.Height();
        const int32_t y  = (dh > 0 ? dh : kStripH) - kStripH;
        Renderer->DrawSurface(g_pane, 0, y);
    }

    void Refresh()
    {
        if (!g_ringU) return;
        EnsurePane();
        if (!g_pane) return;

        AdvanceTicks();
        UpdateSyntheticState();

        const int32_t tw = g_pane->Width();
        const int32_t th = g_pane->Height();

        // Transparent clear — the strip composites onto the bar plate in
        // production; in test mode it lays over a backdrop (spec §3).
        g_pane->StartPass(0.0f, 0.0f, 0.0f, 0.0f);

        // Spec §5 step 3 (base draw children pass): for each of the 4
        // buttons, the TButton paint stamps the active ring sprite at
        // (mbr_0x60, mbr_0x64). Per §6c the active sprite is:
        //   flag & 0x4     → mbr_0xa8 = RingG (disabled)
        //   flag & 0x10000 → mbr_0xa0 = RingD (pressed)
        //   else           → mbr_0x9c = RingU (default base)
        //
        // After the ring, we composite the spell-circle icon centered
        // inside (UNCONFIRMED-D, per docs/ui/sample_screen_1.jpg).
        for (int32_t i = 0; i < kBtnCount; ++i)
        {
            const int32_t rx = kBtnX[i];
            const int32_t ry = kBtnY;

            // (a) Ring sprite — pick by flag word (spec §6c table).
            PTBitmap ring = g_ringU;
            if (g_slotState[i].disabled)     ring = g_ringG ? g_ringG : g_ringU;
            else if (g_slotState[i].pressed) ring = g_ringD ? g_ringD : g_ringU;
            if (ring)
                Renderer->DrawBitmapToTarget(ring, rx, ry, tw, th);

            // (b) Spell icon centered in the ring (UNCONFIRMED-D). 40x40
            //     from SpellIcons.dat keyed by spell name (spec §2 / §4).
            //     The Heal/etc icons are flags=0x2 (BM_15BIT) with
            //     chroma-keyed black, decoded transparent by the bitmap
            //     cache; the renderer's alpha pipeline composites them
            //     on top of the ring with no kc bleed.
            if (PTBitmap icon = g_slots[i].icon)
            {
                const int32_t ix = rx + kIconOffset;
                const int32_t iy = ry + kIconOffset;
                if (g_slotState[i].disabled)
                {
                    // Disabled slot reads greyed-out — tint the icon at
                    // 35% white to dim it. (Retail path is unknown for
                    // the icon-on-disabled-ring composite; this matches
                    // the reference visual where disabled slots look
                    // washed.)
                    Renderer->DrawBitmapSubrectTintedToTarget(
                        icon, ix, iy, 0, 0, icon->width, icon->height,
                        tw, th, 0.35f, 0.35f, 0.35f, 1.0f);
                }
                else
                {
                    Renderer->DrawBitmapToTarget(icon, ix, iy, tw, th);
                }
            }

            // (c) Two-line spell name label SPLIT across the ring (per
            //     user 2026-05-30: "spell names split by word, first
            //     word above the ring, second word below"). Cream color,
            //     centered, 3-pass black shadow (font flag 0x400 — same
            //     SpellbookPane convention, spec §7/§8). Single-word
            //     spells (e.g. "IronSkin") render line 1 only, placed
            //     directly above the ring.
            if (g_font)
            {
                const int32_t lineH  = (int32_t)(TextLineHeight(g_font) + 0.5f);
                const bool hasLine2  = g_slots[i].labelLine2 && g_slots[i].labelLine2[0];
                const int32_t cellX  = rx + kLabelDX;
                const int32_t line1Y = kBtnY - lineH - 1;          // above ring
                const int32_t line2Y = kBtnY + kRingH + 1;         // below ring

                if (g_slots[i].labelLine1 && g_slots[i].labelLine1[0])
                    DrawTextShadowedToTarget(
                        g_font, g_slots[i].labelLine1,
                        cellX, line1Y, kLabelCellW, kLabelCellH,
                        ETextAlign::Center,
                        kLabelR, kLabelG, kLabelB, tw, th);
                if (hasLine2)
                    DrawTextShadowedToTarget(
                        g_font, g_slots[i].labelLine2,
                        cellX, line2Y, kLabelCellW, kLabelCellH,
                        ETextAlign::Center,
                        kLabelR, kLabelG, kLabelB, tw, th);
            }
        }

        g_pane->EndPass();
    }

private:
    void EnsurePane()
    {
        if (g_pane) return;
        // Spec §3: strip occupies the left segment of the 60-tall bar.
        // RT height is kPaneH + kLabelMargin so the cream labels (which
        // sit above the rings per the reference image) have room; the
        // rest of the bar belongs to TBarInvPane / the bar chrome
        // (drawn separately, not in this isolated test mode).
        g_pane = new TSurface(kStripW, kStripH, SG_PIXELFORMAT_RGBA8);
    }

    // Sim tick advance (24Hz, protocol rule 6 + spec §9 dirty model).
    static void AdvanceTicks()
    {
        const double nowMs = TTime::Time() * 1000.0;
        if (g_lastTickMs == 0.0) g_lastTickMs = nowMs;
        int32_t guard = 0;
        while (nowMs - g_lastTickMs >= kSimTickMs && guard < 64)
        {
            g_lastTickMs += kSimTickMs;
            ++guard;
        }
    }

    // Synthetic driver: cycle pressed + disabled across slots so a
    // single capture exercises RingU / RingD / RingG art and the
    // disabled-icon greyed-tint branch. Real pane sources these from
    // the player (slot 21 sets disabled bit per spec §5/§6b).
    static void UpdateSyntheticState()
    {
        const double t = TTime::Time();

        // Pressed slot advances every 1.5 s (~6 s for the full cycle).
        const int32_t pressedIdx = int32_t(t / 1.5) % kBtnCount;
        // Disabled slot advances every 2.0 s offset (so pressed +
        // disabled phase apart and exercise all 3 ring states).
        const int32_t disabledIdx = (int32_t(t / 2.0) + 2) % kBtnCount;

        for (int32_t i = 0; i < kBtnCount; ++i)
        {
            g_slotState[i].pressed  = (i == pressedIdx) && (i != disabledIdx);
            g_slotState[i].disabled = (i == disabledIdx);
        }
    }
};

TQuickSpellHud g_hud;

}  // namespace

// =====================================================================
// Entry points (testmodes.cpp links these — keep the signatures).
// =====================================================================
bool InitializeUIQuickSpellMode()
{
    log_info("[ui-quickspell] === TQuickSpellPane (clean-room from spec) ===");

    // Spec §2: SpellIcons.dat carries Ring{U,D,G} (idxs 52/53/54) AND
    // the 40x40 spell-circle icons (idxs 0..51) keyed by spell name.
    // Init resolves both by name via FUN_0046d710 (init.cpp:28-30).
    g_spellIconsDat = TMulti::LoadMulti((char*)kArchive);

    if (g_spellIconsDat)
    {
        g_ringU = LookupByName(g_spellIconsDat, kRingUName);
        g_ringD = LookupByName(g_spellIconsDat, kRingDName);
        g_ringG = LookupByName(g_spellIconsDat, kRingGName);
        for (int32_t i = 0; i < kBtnCount; ++i)
            g_slots[i].icon = LookupByName(g_spellIconsDat, g_slots[i].spellName);
    }

    log_info("[ui-quickspell] rings: RingU=%s RingD=%s RingG=%s",
             g_ringU ? "OK" : "MISS",
             g_ringD ? "OK" : "MISS",
             g_ringG ? "OK" : "MISS");
    if (g_ringU)
        log_info("[ui-quickspell] RingU %dx%d (expect 48x48 flags=0x104)",
                 g_ringU->width, g_ringU->height);
    for (int32_t i = 0; i < kBtnCount; ++i)
        log_info("[ui-quickspell] slot[%d] spell='%s' icon=%s",
                 i, g_slots[i].spellName,
                 g_slots[i].icon ? "OK" : "MISS");

    // Font: small Arimo for the 2-line spell-name labels (spec §8 /
    // SpellbookPane spec §8 — same convention as the spellbook text).
    g_font = BuildTTFAtlas(kFontPath, kFontPx);
    log_info("[ui-quickspell] font %s @%dpx = %s",
             kFontPath, kFontPx, g_font ? "OK" : "MISS");

    delete g_pane;
    g_pane       = nullptr;
    g_lastTickMs = 0.0;
    for (int32_t i = 0; i < kBtnCount; ++i)
        g_slotState[i] = SSlotState{};

    Renderer->AddHud(&g_hud, 0.0f);
    return true;
}

void RenderUIQuickSpellMode()
{
    g_hud.Refresh();

    // Muted slate backdrop — same family as ui-barinv / ui-bottombar so
    // the ring strip's bottom-anchored placement reads in isolation
    // (no playfield behind the HUD in test mode).
    Display.BackBuffer()->StartPass(0.18f, 0.20f, 0.26f, 1.0f);
    Display.BackBuffer()->EndPass();
}

void CloseUIQuickSpellMode()
{
    Renderer->RemoveHud(&g_hud);
    delete g_pane;
    g_pane          = nullptr;
    g_ringU = g_ringD = g_ringG = nullptr;
    g_spellIconsDat = nullptr;
    g_font          = nullptr;
    g_lastTickMs    = 0.0;
    for (int32_t i = 0; i < kBtnCount; ++i)
    {
        g_slots[i].icon = nullptr;
        g_slotState[i]  = SSlotState{};
    }
}
