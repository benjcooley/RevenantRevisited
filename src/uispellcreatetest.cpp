// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uispellcreatetest.cpp - --test=ui-spellcreate                        *
// *************************************************************************
//
// Clean-room reconstruction of TSpellPane (cls_0x5a5978, "Spell" lower-
// region tab of the right-sidebar — the talisman composer). Built ONLY
// from docs/ui/forensics/SpellCreatePane_SPEC.md (status =
// forensics-skeleton). Every confirmed coordinate cites its spec section;
// every UNCONFIRMED placeholder is flagged inline so the next forensics
// pass can lift the real literal.
//
// What this pane draws (spec §0-§3 CONFIRMED, §4-§13 TBD):
//   - `spellconstr` 188x174 parchment chrome (spec §2 idx 0, flags 0x2 =
//     BM_15BIT opaque) at pane-local (0,0).
//   - 5 buttons wired in retail Initialize() at the confirmed literal
//     (x,y,w,h) coordinates from cls_0x5a5978.cpp:136-190 (spec §3):
//       - Invoke / spell-name long bar at (24,136) 142x36 (SpellInvU base)
//       - "back" backspace button at (164,143) 22x20 (SpellBackU base)
//       - "up" arrow at (166,82) 20x26 (SpellArwUU base)
//       - "down" arrow at (166,112) 20x26 (SpellArwDU base)
//       - "min" toggle at (166,37) 18x18 (SpellTalU base — spec §0
//         identifies SpellTal as the talisman-names toggle; the snapshot
//         names this "min" / "max" but the asset roster (§2) only ships
//         SpellTalU/D + SpellNameU/D as 18x18 toggles. We use SpellTalU
//         per the spec's call-site mapping at _data.txt:116206)
//   - 12 talisman icons (SStars/SLaw/SLife/SSky/SChaos/SDeath/SSoul/SSun/
//     SOcean/SMoon/SEarth/SWard — 32x32 BM_16BIT+BM_ALPHA, kc=0) laid out
//     in a 4-col x 3-row grid in the upper-left interior of the pane.
//     (UNCONFIRMED — see GAP-1 below.)
//
// Architecture (spec §3 direct-renderer contract): compose the WHOLE pane
// (chrome + 5 buttons + 12 talisman icons) into ONE offscreen TSurface RT
// via the *ToTarget primitive family, then DrawSurface it once in the HUD
// pass. Same direct-renderer contract as uispellbooktest /
// uiplyrstatusbartest / uiinventorytest.
//
// Primitives (UI_METHOD_MAP §12 — canonical shared toolbox only):
//   Renderer->DrawBitmapToTarget                 - opaque chrome / opaque
//                                                  button base / alpha
//                                                  talisman icon stamp
// No hand-rolled shadow passes / glyph walks; no procedural stand-ins.
//
// === SPEC GAPS (forensics-skeleton — to lift in next pass) ============
//
// GAP-1 (talisman grid origin + pitch): The 12 S* icon positions are NOT
//       in the confirmed spec §3 (those are only the 5 button literals
//       from Initialize). The icons are emitted by the slot-84 DrawBackground
//       (FUN_005435f0) body which is NOT YET EXTRACTED into
//       recon/discovered/. The snapshot DrawBackground uses
//       TAL_STARTX=25 TAL_STARTY=9 TAL_WIDTH=30 TAL_HEIGHT=30 TAL_WRAPWIDTH=130
//       (spellpane.cpp:207-213) — but those are pre-release literals on
//       the 168x128 snapshot pane, not the 188x174 retail pane.
//
//       Our placeholder layout: 4-col x 3-row grid at pane-local origin
//       (10, 8) with 38px x-pitch (32px icon + 6px padding) and 33px y-pitch
//       (32px icon + 1px padding). This fits inside the upper-left interior
//       of the pane (x range [10, 162], y range [8, 107]) — well clear of
//       the right-column buttons at x=164-166 and the wide Invoke bar at
//       y=136. The layout is purely a CALL-TO-ARMS for visual verification
//       that all 12 icons load + decode correctly; the actual retail
//       positions almost certainly differ.
//
// GAP-2 (spell name text region): No font is wired into this test mode
//       because the spec does not yet identify which font the retail pane
//       uses for the spell-name display text inside the long Invoke bar.
//       The snapshot uses `goldfont` / `tinyfont` (spellpane.cpp:258, 274).
//       Skipped here pending the slot-84 extraction that would pin down
//       (cell rect, font atom, color, shadow).
//
// GAP-3 (spell pouch / composition cells): The "spell pouch" area is the
//       middle region where the currently-composed talismans are shown
//       (spec §1 / §3 frame table row "spell-pouch"). Position + cell
//       count is UNCONFIRMED (spec §14-3). Not rendered here.
//
// GAP-4 (button state cycling): Test mode shows the resting Up state for
//       every button. Down/Glow states (SpellInvD/SpellBackD/etc.) would
//       need a synthetic press-state cycler analogous to uiquickspelltest;
//       not implemented here because the click handler body (`0x543c40`)
//       isn't fully decoded.
//
// GAP-5 (snapshot uses x_ofs=2 shadow stamp pre-pass): The snapshot
//       TTalismanButton::Draw at spellpane.cpp:51-55 stamps each talisman
//       icon TWICE (once at +2,+2 with a flat black color, then once at
//       +0,+0) — a per-icon drop-shadow. We do NOT replicate this until
//       the retail slot-84 body confirms it; the canonical shadow API
//       (DrawBitmapShadowedToTarget) is available if needed.
//
// === END SPEC GAPS ====================================================
//
// *************************************************************************

#include "uispellcreatetest.h"

#include "bitmap.h"
#include "display.h"
#include "font.h"
#include "logging.h"
#include "multi.h"
#include "renderer.h"
#include "surface.h"
#include "uidragstate.h"

#include <cstdint>
#include <cstdio>
#include <cstring>

namespace {

// =====================================================================
// Geometry constants — all cited to SpellCreatePane_SPEC.md.
// =====================================================================

// --- pane rect (spec §3 ctor :32-43) ---------------------------------
// 0xbc x 0xae = 188 x 174. Right-anchored lower MULTIPANE slot at
// Classic screen (452, 306).
constexpr int32_t kPaneW = 0xbc;        // 188 — pane width  (§3 ctor :39)
constexpr int32_t kPaneH = 0xae;        // 174 — pane height (§3 ctor :42)

// --- button positions (spec §3 confirmed literals) -------------------
// All five are emitted by Initialize @ 0x5432a0 via cls_0x5a3c68 ctor
// calls; spec §3 lifts the (x,y,w,h) tuples directly from the decomp.

// Invoke / spell-name long bar (spec §3 row 1)
//   cls_0x5a5978.cpp:136-138 → (0x18=24, 0x88=136, 0x8e=142, 0x24=36)
constexpr int32_t kInvokeX = 0x18;       // 24
constexpr int32_t kInvokeY = 0x88;       // 136
constexpr int32_t kInvokeW = 0x8e;       // 142
constexpr int32_t kInvokeH = 0x24;       // 36

// "back" backspace (spec §3 row 2)
//   cls_0x5a5978.cpp:152-154 → (0xa4=164, 0x8f=143, 0x16=22, 0x14=20)
constexpr int32_t kBackX   = 0xa4;       // 164
constexpr int32_t kBackY   = 0x8f;       // 143
constexpr int32_t kBackW   = 0x16;       // 22
constexpr int32_t kBackH   = 0x14;       // 20

// "up" arrow (spec §3 row 3)
//   cls_0x5a5978.cpp:164-166 → (0xa6=166, 0x52=82, 0x14=20, 0x1a=26)
constexpr int32_t kUpArwX  = 0xa6;       // 166
constexpr int32_t kUpArwY  = 0x52;       // 82
constexpr int32_t kUpArwW  = 0x14;       // 20
constexpr int32_t kUpArwH  = 0x1a;       // 26

// "down" arrow (spec §3 row 4)
//   cls_0x5a5978.cpp:176-178 → (0xa6=166, 0x70=112, 0x14=20, 0x1a=26)
constexpr int32_t kDnArwX  = 0xa6;       // 166
constexpr int32_t kDnArwY  = 0x70;       // 112
constexpr int32_t kDnArwW  = 0x14;       // 20
constexpr int32_t kDnArwH  = 0x1a;       // 26

// "min" toggle (spec §3 row 5)
//   cls_0x5a5978.cpp:188-190 → (0xa6=166, 0x25=37, 0x12=18, 0x12=18)
// spec §0 maps this to SpellTal (talisman-names toggle) via the
// _data.txt:116206/116211 string-anchor xrefs to SpellTalU/D.
constexpr int32_t kMinX    = 0xa6;       // 166
constexpr int32_t kMinY    = 0x25;       // 37
constexpr int32_t kMinW    = 0x12;       // 18
constexpr int32_t kMinH    = 0x12;       // 18

// --- talisman icon grid (UNCONFIRMED — see GAP-1) --------------------
// 12 icons of 32x32 in a 4-col x 3-row grid, upper-left interior of pane.
// Origin (10,8); pitch 38x33 (icon + small gap). Total grid extent:
//   x: 10 .. 10 + 3*38 + 32 = 156      (well clear of right column @ x=164)
//   y:  8 ..  8 + 2*33 + 32 = 106      (well clear of Invoke bar @ y=136)
constexpr int32_t kTalIconW    = 32;      // §2 — every S* talisman is 32x32
constexpr int32_t kTalIconH    = 32;      // §2
constexpr int32_t kTalGridX0   = 10;      // GAP-1 — placeholder origin
constexpr int32_t kTalGridY0   = 8;       // GAP-1 — placeholder origin
constexpr int32_t kTalPitchX   = 38;      // GAP-1 — placeholder pitch
constexpr int32_t kTalPitchY   = 33;      // GAP-1 — placeholder pitch
constexpr int32_t kTalGridCols = 4;       // GAP-1 — placeholder shape
constexpr int32_t kTalGridRows = 3;       // GAP-1 — placeholder shape
constexpr int32_t kTalCount    = 12;      // §2 — exactly 12 S* icons

// =====================================================================
// Asset roster — spec §2 (all 25 entries are in spellpane.dat).
// =====================================================================
constexpr const char* kArchive       = "spellpane.dat";

// Font for composed-word display (GAP-2 — retail font unknown;
// approximated with Arimo at 10px for the pouch area).
constexpr const char* kFontPath  = "thirdparty/fonts/Arimo-Regular.ttf";
constexpr int32_t     kFontPx    = 10;

// Composed-word pouch area (UNCONFIRMED — §14-3; harness placement below
// the talisman grid, above the invoke button — spec §1 "middle area").
// Approximate: x=10, y=114, w=148, h=20 (between grid-bottom and Invoke).
constexpr int32_t kPouchX        = 10;
constexpr int32_t kPouchY        = 114;   // below grid (y0=8 + 3*33=107 ≈ 114)
constexpr int32_t kPouchW        = 150;

// Spell match result text: at the Invoke bar position (24,136) w=142.
// Showing the matched spell name inside the Invoke bar area.
constexpr float kMatchR = 1.0f, kMatchG = 0.85f, kMatchB = 0.3f;  // gold-ish

constexpr const char* kChromeName    = "spellconstr";   // §2 idx 0 (188x174)
constexpr const char* kInvokeUName   = "SpellInvU";     // §2 idx 2 (142x36)
constexpr const char* kBackUName     = "SpellBackU";    // §2 idx 4 (22x20)
constexpr const char* kArwUUName     = "SpellArwUU";    // §2 idx 8 (20x26)
constexpr const char* kArwDUName     = "SpellArwDU";    // §2 idx 6 (20x26)
constexpr const char* kTalUName      = "SpellTalU";     // §2 idx 12 (18x18)

// 12 talisman names in dat order (idx 13..24 per §2 / dump_dat.py).
// Spec §2 footnote ties these 1:1 to the snapshot Old[] table
// (spellpane.cpp:22-23) and the spell.def TALISMANS recipes
// (spell.def:14-26). Order MATTERS for visual stability across captures.
constexpr const char* kTalismanNames[kTalCount] = {
    "SStars", "SLaw",   "SLife",  "SSky",
    "SChaos", "SDeath", "SSoul",  "SSun",
    "SOcean", "SMoon",  "SEarth", "SWard",
};

// =====================================================================
// Loaded assets + cached RT.
// =====================================================================
TMulti*  g_spellpaneDat = nullptr;
PTBitmap g_chrome       = nullptr;   // 188x174 spellconstr
PTBitmap g_invokeU      = nullptr;   // 142x36  SpellInvU
PTBitmap g_backU        = nullptr;   //  22x20  SpellBackU
PTBitmap g_arwUU        = nullptr;   //  20x26  SpellArwUU
PTBitmap g_arwDU        = nullptr;   //  20x26  SpellArwDU
PTBitmap g_talU         = nullptr;   //  18x18  SpellTalU
PTBitmap g_talIcons[kTalCount] = { nullptr };

const SFontAtlas* g_font = nullptr;  // for composed-word display + spell-name text
TSurface* g_pane = nullptr;          // composed RT (188x174 — pane-sized)

// =====================================================================
// #10b/c/d — Spell composition buffer.
//
// The user clicks a talisman icon → it is appended to g_composedWord[].
// The "back" button removes the last talisman.
// The "invoke" button looks up the word in the harness spell table and
// shows the result (spec §10d — the retail spell-lookup path via the
// "Code" stat is not yet extracted per spec §14-6; the harness uses a
// static word→spell table matching spell.def recipes).
//
// SPELLSIZE = 11 (snapshot src/spellpane.h:30; spec §14-3 UNCONFIRMED).
// =====================================================================
constexpr int32_t kSpellSize = 11;   // max talismans in a composed word (snapshot)

// Each entry is a talisman index (0-11 into kTalismanNames) or -1 for empty.
int32_t g_composedWord[kSpellSize] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
int32_t g_composedLen = 0;

// Spell lookup result — name of the matched spell, or "" if none.
char    g_matchedSpell[64] = {};
bool    g_dirty = true;

// =====================================================================
// Harness spell table — word → spell name mapping.
// Subset of spell.def TALISMANS recipes for the 5 harness spells.
// Each entry: a sequence of talisman indices (into kTalismanNames[]),
// terminated by -1, plus the spell name.
// =====================================================================
struct SHarnessSpell
{
    const char* name;
    int32_t talismans[8];  // indices into kTalismanNames; -1 terminates
};

// kTalismanNames order: SStars(0), SLaw(1), SLife(2), SSky(3), SChaos(4),
// SDeath(5), SSoul(6), SSun(7), SOcean(8), SMoon(9), SEarth(10), SWard(11)
// Note: order must match kTalismanNames[] defined above.
static const SHarnessSpell kHarnessSpells[] = {
    { "Heal",             { 2,   7,  -1, -1, -1, -1, -1, -1 } },  // Life, Sun
    { "Advanced Healing", { 2,   7,   6, -1, -1, -1, -1, -1 } },  // Life, Sun, Soul
    { "Iron Skin",        { 10, 11,   1, -1, -1, -1, -1, -1 } },  // Earth, Ward, Law
    { "Fire Flash",       {  7,  4,  -1, -1, -1, -1, -1, -1 } },  // Sun, Chaos
    { "Ice Bolt",         {  8,  9,  -1, -1, -1, -1, -1, -1 } },  // Ocean, Moon
    { nullptr, { -1 } },
};

// Check if the current g_composedWord matches a harness spell.
static const char* LookupComposedWord()
{
    if (g_composedLen == 0) return "";
    for (int32_t s = 0; kHarnessSpells[s].name; ++s)
    {
        const SHarnessSpell& sp = kHarnessSpells[s];
        // Count recipe length.
        int32_t rlen = 0;
        while (rlen < 8 && sp.talismans[rlen] >= 0) ++rlen;
        if (rlen != g_composedLen) continue;
        bool match = true;
        for (int32_t t = 0; t < rlen; ++t)
        {
            if (g_composedWord[t] != sp.talismans[t]) { match = false; break; }
        }
        if (match) return sp.name;
    }
    return "";
}

// Append a talisman to the composition buffer (#10b).
static void ComposerAddTalisman(int32_t talIdx)
{
    if (g_composedLen >= kSpellSize) return;
    g_composedWord[g_composedLen++] = talIdx;
    std::snprintf(g_matchedSpell, sizeof(g_matchedSpell), "%s", LookupComposedWord());
    g_dirty = true;
    log_info("[ui-spellcreate] add talisman[%d]='%s' word-len=%d match='%s'",
             talIdx, kTalismanNames[talIdx], g_composedLen, g_matchedSpell);
}

// Remove last talisman from buffer (#10c).
static void ComposerBackspace()
{
    if (g_composedLen <= 0) return;
    g_composedWord[--g_composedLen] = -1;
    std::snprintf(g_matchedSpell, sizeof(g_matchedSpell), "%s", LookupComposedWord());
    g_dirty = true;
    log_info("[ui-spellcreate] backspace word-len=%d", g_composedLen);
}

// Invoke the composed word (#10d) — harness shows matched spell name.
static void ComposerInvoke()
{
    const char* sp = LookupComposedWord();
    if (sp && sp[0])
    {
        log_info("[ui-spellcreate] INVOKE matched='%s' — spell created", sp);
        // In production this calls the script/spell engine via the TalismanClass
        // "Code" lookup path. In the harness, we just note the match and clear.
        // The resulting spell would appear in the spellbook (uispellbooktest) and
        // on QuickSpell. Full wire-up requires: spell engine integration +
        // SHudState::quickspellBindings (Agent A) + TSpellIconSlot drop (#6b).
    }
    else
    {
        log_info("[ui-spellcreate] INVOKE no match for composed word (len=%d)",
                 g_composedLen);
    }
    // Clear after invoke.
    for (int32_t i = 0; i < kSpellSize; ++i) g_composedWord[i] = -1;
    g_composedLen = 0;
    g_matchedSpell[0] = 0;
    g_dirty = true;
}

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
// HUD drawable — composes the pane RT then DrawSurface's it once at the
// bottom-right anchored screen origin. Production pane sits at screen
// (452, 306) on a Classic 640x480 screen; pane is 188x174, so the right
// edge sits at 640 and the bottom at 480 — i.e. flush bottom-right. The
// test mode keeps the same anchor intent for the live display size.
// =====================================================================
class TSpellCreateHud : public THudDrawable
{
public:
    void Draw() override
    {
        if (!g_pane) return;
        // Spec §3: pane is right-anchored at the lower MULTIPANE slot.
        // For the harness we bottom-right anchor with a small inset so the
        // chrome reads in the corner where the player will see it in-engine.
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

        // Only recompose when state changed (#10b/c/d dirty flag).
        if (!g_dirty) return;
        g_dirty = false;

        const int32_t tw = g_pane->Width();
        const int32_t th = g_pane->Height();

        // Transparent clear so the chrome's alpha-keyed edge (none here —
        // spellconstr is opaque BM_15BIT) composites cleanly; the test
        // backdrop shows through any pane region the chrome doesn't paint.
        g_pane->StartPass(0.0f, 0.0f, 0.0f, 0.0f);

        // --- (1) Chrome at pane-local (0,0) — spec §2 idx 0 ---------
        // spellconstr is the full pane background (188x174, opaque
        // BM_15BIT). The retail slot-84 DrawBackground stamps this first
        // (the body is unextracted — GAP-1 — but every sibling pane's
        // DrawBackground stamps its chrome at pane (0,0) as step 1).
        Renderer->DrawBitmapToTarget(g_chrome, 0, 0, tw, th);

        // --- (2) Talisman icon grid (UNCONFIRMED — GAP-1) -----------
        // 12 S* icons in a 4-col x 3-row placeholder grid. Drawn after
        // chrome so the alpha-keyed (BM_16BIT|BM_ALPHA, kc=0) icons
        // composite over the parchment background.
        for (int32_t i = 0; i < kTalCount; ++i)
        {
            PTBitmap icon = g_talIcons[i];
            if (!icon) continue;
            const int32_t col = i % kTalGridCols;
            const int32_t row = i / kTalGridCols;
            const int32_t x = kTalGridX0 + col * kTalPitchX;
            const int32_t y = kTalGridY0 + row * kTalPitchY;
            // DrawBitmapToTarget composites via the bitmap's own alpha
            // buffer (BM_ALPHA flag); the icons should appear keyed.
            Renderer->DrawBitmapToTarget(icon, x, y, tw, th);
        }

        // --- (3) Buttons (resting Up states) — spec §3 confirmed ----
        // Spec §3 button order is fixed by Initialize (cls_0x5a5978.cpp:
        // 136-190). We draw the Up sprite for each so a single capture
        // shows the idle pane the player sees on first open.

        // (3a) Invoke / spell-name long bar — spec §3 row 1
        if (g_invokeU)
            Renderer->DrawBitmapToTarget(g_invokeU, kInvokeX, kInvokeY, tw, th);

        // (3b) "back" backspace — spec §3 row 2
        if (g_backU)
            Renderer->DrawBitmapToTarget(g_backU, kBackX, kBackY, tw, th);

        // (3c) "up" arrow — spec §3 row 3
        if (g_arwUU)
            Renderer->DrawBitmapToTarget(g_arwUU, kUpArwX, kUpArwY, tw, th);

        // (3d) "down" arrow — spec §3 row 4
        if (g_arwDU)
            Renderer->DrawBitmapToTarget(g_arwDU, kDnArwX, kDnArwY, tw, th);

        // (3e) "min" / SpellTal toggle — spec §3 row 5
        if (g_talU)
            Renderer->DrawBitmapToTarget(g_talU, kMinX, kMinY, tw, th);

        // --- (4) Composed-word display (#10b "spell pouch area") ------
        // Show the currently-composed talisman sequence as glyph icons
        // in a horizontal strip in the middle area (pouch region).
        // Each composed talisman is shown at 16×16 (halved from 32×32
        // to fit up to 11 in the 150px-wide area).
        // GAP-3: exact pouch cell positions / size are UNCONFIRMED (spec
        // §14-3); we use a harness approximation below the talisman grid.
        if (g_composedLen > 0)
        {
            const int32_t cellW = 14;   // halved icon cell size
            const int32_t gap   = 2;
            for (int32_t i = 0; i < g_composedLen && i < kSpellSize; ++i)
            {
                const int32_t idx = g_composedWord[i];
                if (idx < 0 || idx >= kTalCount) continue;
                PTBitmap icon = g_talIcons[idx];
                if (!icon) continue;
                const int32_t cx = kPouchX + i * (cellW + gap);
                // Stretch 32×32 icon into 14×14 cell for the pouch display.
                Renderer->DrawBitmapSubrectStretchedToTarget(
                    icon, cx, kPouchY, cellW, cellW,
                    0, 0, icon->width, icon->height,
                    tw, th);
            }
        }

        // --- (5) Matched spell name on the Invoke bar (#10d preview) --
        // Show the spell name if the composed word matches a known spell.
        // Renders over the SpellInvU bar bitmap (below it so it's readable).
        if (g_font && g_matchedSpell[0])
        {
            const int32_t lineH = int32_t(TextLineHeight(g_font) + 0.5f);
            // Center text vertically in the invoke bar (h=36).
            const int32_t ty = kInvokeY + (kInvokeH - lineH) / 2;
            DrawTextShadowedToTarget(
                g_font, g_matchedSpell,
                kInvokeX + 4, ty, kInvokeW - 8, lineH,
                ETextAlign::Center,
                kMatchR, kMatchG, kMatchB, tw, th);
        }

        g_pane->EndPass();
    }

private:
    void EnsurePane()
    {
        if (g_pane) return;
        // Spec §3: pane is fixed 188x174. The whole compose (chrome +
        // icons + buttons) lands in this RT; HUD-pass DrawSurface's it
        // once to the screen at the bottom-right lower-sidebar slot.
        g_pane = new TSurface(kPaneW, kPaneH, SG_PIXELFORMAT_RGBA8);
    }
};

TSpellCreateHud g_hud;

}  // namespace

// =====================================================================
// Entry points (testmodes.cpp links these — keep the signatures).
// =====================================================================
bool InitializeUISpellCreateMode()
{
    log_info("[ui-spellcreate] === TSpellPane (clean-room from spec; "
             "forensics-skeleton — see GAP-1..GAP-5 in source) ===");

    // Spec §2: spellpane.dat is a 25-entry TMulti — owns chrome + 5
    // button base/down pairs + 12 S* talisman icons.
    g_spellpaneDat = TMulti::LoadMulti((char*)kArchive);
    if (!g_spellpaneDat)
    {
        log_error("[ui-spellcreate] LoadMulti('%s') FAILED", kArchive);
        return true;   // keep the test mode alive so the harness still runs
    }

    // Pane-frame chrome + button base sprites.
    g_chrome  = LookupByName(g_spellpaneDat, kChromeName);
    g_invokeU = LookupByName(g_spellpaneDat, kInvokeUName);
    g_backU   = LookupByName(g_spellpaneDat, kBackUName);
    g_arwUU   = LookupByName(g_spellpaneDat, kArwUUName);
    g_arwDU   = LookupByName(g_spellpaneDat, kArwDUName);
    g_talU    = LookupByName(g_spellpaneDat, kTalUName);

    log_info("[ui-spellcreate] chrome=%s Invoke=%s Back=%s Up=%s Dn=%s Tal=%s",
             g_chrome  ? "OK" : "MISS",
             g_invokeU ? "OK" : "MISS",
             g_backU   ? "OK" : "MISS",
             g_arwUU   ? "OK" : "MISS",
             g_arwDU   ? "OK" : "MISS",
             g_talU    ? "OK" : "MISS");
    if (g_chrome)
        log_info("[ui-spellcreate] chrome %dx%d (expect 188x174)",
                 g_chrome->width, g_chrome->height);

    // 12 talisman icons (spec §2 idx 13..24). Each is 32x32
    // BM_16BIT|BM_ALPHA — the alpha buffer drives the composite.
    int32_t got_icons = 0;
    for (int32_t i = 0; i < kTalCount; ++i)
    {
        g_talIcons[i] = LookupByName(g_spellpaneDat, kTalismanNames[i]);
        if (g_talIcons[i]) ++got_icons;
    }
    log_info("[ui-spellcreate] talismans loaded: %d / %d", got_icons, kTalCount);
    if (got_icons < kTalCount)
    {
        for (int32_t i = 0; i < kTalCount; ++i)
            log_info("[ui-spellcreate]   tal[%d] '%s' = %s",
                     i, kTalismanNames[i], g_talIcons[i] ? "OK" : "MISS");
    }

    // Font — for composed-word + spell-match display (GAP-2 approximation).
    g_font = BuildTTFAtlas(kFontPath, kFontPx);
    log_info("[ui-spellcreate] font %s @%dpx = %s",
             kFontPath, kFontPx, g_font ? "OK" : "MISS");

    // Reset the cached RT (recreated on first Refresh).
    delete g_pane;
    g_pane = nullptr;
    g_composedLen = 0;
    for (int32_t i = 0; i < kSpellSize; ++i) g_composedWord[i] = -1;
    g_matchedSpell[0] = 0;
    g_dirty = true;

    Renderer->AddHud(&g_hud, 0.0f);
    return true;
}

void RenderUISpellCreateMode()
{
    g_hud.Refresh();

    // Muted slate backdrop matching the other ui*test modes so the
    // parchment chrome reads in isolation (no playfield behind the HUD
    // in test mode).
    Display.BackBuffer()->StartPass(0.18f, 0.20f, 0.26f, 1.0f);
    Display.BackBuffer()->EndPass();
}

void CloseUISpellCreateMode()
{
    Renderer->RemoveHud(&g_hud);
    delete g_pane;
    g_pane         = nullptr;
    g_chrome       = nullptr;
    g_invokeU      = nullptr;
    g_backU        = nullptr;
    g_arwUU        = nullptr;
    g_arwDU        = nullptr;
    g_talU         = nullptr;
    g_font         = nullptr;
    for (int32_t i = 0; i < kTalCount; ++i) g_talIcons[i] = nullptr;
    g_spellpaneDat = nullptr;
    g_composedLen  = 0;
    for (int32_t i = 0; i < kSpellSize; ++i) g_composedWord[i] = -1;
    g_matchedSpell[0] = 0;
    g_dirty = true;
}

// =====================================================================
// #10b/c/d — Mouse handler for talisman click + backspace + invoke.
// =====================================================================
void HandleMouseClickUISpellCreateMode(int32_t button, int32_t x, int32_t y)
{
    if (button != MB_LEFTDOWN) return;

    // Convert screen coords to pane-local.
    // Pane is right-bottom anchored (spec §3 ctor: x=452, y=306 classic).
    const int32_t dw    = Display.Width()  > 0 ? Display.Width()  : 640;
    const int32_t dh    = Display.Height() > 0 ? Display.Height() : 480;
    const int32_t paneX = dw - kPaneW;
    const int32_t paneY = dh - kPaneH;
    const int32_t lx    = x - paneX;
    const int32_t ly    = y - paneY;

    // Hit-test talisman grid (#10b): 4-col x 3-row placeholder grid at
    // (kTalGridX0, kTalGridY0) pitch (kTalPitchX, kTalPitchY).
    // Each icon cell is kTalIconW×kTalIconH.
    if (lx >= kTalGridX0 && ly >= kTalGridY0)
    {
        const int32_t col = (lx - kTalGridX0) / kTalPitchX;
        const int32_t row = (ly - kTalGridY0) / kTalPitchY;
        // Verify it's inside the icon area (not in the gap).
        const int32_t localX = (lx - kTalGridX0) % kTalPitchX;
        const int32_t localY = (ly - kTalGridY0) % kTalPitchY;
        if (col >= 0 && col < kTalGridCols &&
            row >= 0 && row < kTalGridRows &&
            localX < kTalIconW && localY < kTalIconH)
        {
            const int32_t idx = row * kTalGridCols + col;
            if (idx < kTalCount)
            {
                ComposerAddTalisman(idx);
                return;
            }
        }
    }

    // Hit-test "back" button (#10c).
    if (lx >= kBackX && lx < kBackX + kBackW &&
        ly >= kBackY && ly < kBackY + kBackH)
    {
        ComposerBackspace();
        return;
    }

    // Hit-test "invoke" button (#10d).
    if (lx >= kInvokeX && lx < kInvokeX + kInvokeW &&
        ly >= kInvokeY && ly < kInvokeY + kInvokeH)
    {
        ComposerInvoke();
        return;
    }
}
