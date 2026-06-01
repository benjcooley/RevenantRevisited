// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uispellbooktest.cpp - --test=ui-spellbook                            *
// *************************************************************************
//
// Clean-room reconstruction of SpellbookSidebarPane (cls_0x5a5ae8, "Book"
// tab of the right-sidebar). Built ONLY from docs/ui/forensics/
// SpellbookPane_SPEC.md (with NOMENCLATURE / UI_METHOD_MAP §12 /
// RECONSTRUCTION_PROTOCOL). Every asset, coordinate, color, alignment and
// shadow cites its spec section inline.
//
// What this pane draws (spec §1 / §2 / §4 / §5):
//   - Scroll chrome (spellscroll.dat:Scroll, 188x306, opaque) as the pane
//     frame — ornate top + bottom scroll-rods + parchment viewport inset.
//   - ScrollPaper (148x80) vertically tiled inside a 148x229 content
//     surface, scroll-aligned (scrollY mod 80).
//   - Per known-spell, a ~91px-tall row composed of:
//       a) 40x40 spell-circle icon (SpellIcons.dat keyed by spell name),
//       b) cream spell name text (RGB 0xff,0xe7,0xf2, font flag 0x401),
//       c) wrapped description / skill block (theme color, font 0x402),
//       d) "Skill: N" stat line (yellow-green RGB 0xf4,0xf4,0x05, font 0x401),
//       e) "Mana: N" stat line (cyan RGB 0x25,0xff,0xe9, font 0x401),
//       f) talisman-glyph recipe row (20x20 each, 16px pitch, alpha sprites
//          from spellscroll.dat:S<Talisman>).
//   - ScrlTop (32x12) + ScrlBottom (40x16) cap sprites composited at the
//     top + bottom of the content viewport (over the content blit).
//   - Scroll-arrow buttons at pane-local (169,150) / (169,174), 24x24
//     (ScrlArwUU/UD/UG and ScrlArwDU/DD/DG from spellscroll.dat).
//
// Architecture (spec §3 direct-renderer contract): compose the WHOLE pane
// (Scroll frame + content surface + arrow buttons) into ONE offscreen
// TSurface RT via the *ToTarget primitive family, then DrawSurface it once
// in the HUD pass — same direct-renderer contract as uiplyrstatusbartest /
// uibarinvtest / uiquickspelltest.
//
// Primitives (UI_METHOD_MAP §12 — canonical shared toolbox only):
//   Renderer->DrawBitmapToTarget                 - opaque chrome stamp
//   Renderer->DrawBitmapSubrectToTarget          - opaque tile / subrect stamp
//   Renderer->DrawBitmapSubrectStretchedToTarget - icon stretched into row
//   font.h DrawTextShadowedToTarget              - 3-pass black shadow text
// No hand-rolled shadow passes / glyph walks; no procedural stand-ins.
//
// Test-harness driver (spec §1 / §6c):
//   - We have no live Player so the harness binds 5 sample spells matching
//     the QuickSpell harness names: Advanced Healing, Iron Skin, Fire
//     Flash, Ice Bolt, plus Heal. These are real SpellIcons.dat entries
//     (idx 50, 21, 12, 17, 16) — the bitmap path matches retail exactly.
//   - The spec talisman-glyph composition is data-driven (per-spell recipe);
//     the recon does not expose the per-spell talisman list in source, so
//     the harness uses representative glyphs per spell from spellscroll.dat
//     (visual exercise of the per-row glyph blit — UNCONFIRMED-G).
//   - Animation: scrollY advances at +40 px / sim tick (spec §6b 0x28 step,
//     `cls_0x5a4494.cpp:641-696`) and ping-pongs across the content extent,
//     so a single capture exercises both the parchment tile scroll-align
//     loop (`for iVar11=-(p[0x5f]%0x50)`) and the row paint window test
//     (spec §6c).
//
// *************************************************************************

#include "uispellbooktest.h"

#include "bitmap.h"
#include "display.h"
#include "font.h"
#include "logging.h"
#include "multi.h"
#include "renderer.h"
#include "surface.h"
#include "time.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace {

// =====================================================================
// Geometry constants — all cited to SpellbookPane_SPEC.md.
// =====================================================================

// --- pane (spec §3 ctor + scroll chrome) -----------------------------
constexpr int32_t kPaneW = 0xbc;   // 188 — pane width (ctor :21-28)
constexpr int32_t kPaneH = 0x132;  // 306 — pane height (ctor :21-28)

// --- content surface (spec §3 / Init :65) ----------------------------
// FUN_004a5740(0x94, 0xe5, 0x400, …) — the 148x229 parchment viewport that
// hosts the per-row paint + ScrollPaper tile. Inset inside the 188-wide
// scroll-rod frame at pane-local (20, 38) (UNCONFIRMED-B in spec §3).
constexpr int32_t kContentW    = 0x94;   // 148 (§3 surface alloc)
constexpr int32_t kContentH    = 0xe5;   // 229 (§3 surface alloc)
constexpr int32_t kContentOrigX = 20;    // pane-local content TL (§3 UNCONFIRMED-B)
constexpr int32_t kContentOrigY = 38;    // pane-local content TL (§3 UNCONFIRMED-B)

// --- ScrollPaper tile (spec §2 / §5 step 2) --------------------------
constexpr int32_t kPaperW = 148;   // measured (dump_dat.py [9])
constexpr int32_t kPaperH = 80;    // measured (dump_dat.py [9])
constexpr int32_t kTilePitch = 0x50;  // 80 — tile stride (§5 step 2)

// --- ScrlTop / ScrlBottom rod caps (spec §2 / §5 step 4) -------------
constexpr int32_t kScrlTopW = 32;   // measured (dump_dat.py [6])
constexpr int32_t kScrlTopH = 12;
constexpr int32_t kScrlBotW = 40;   // measured (dump_dat.py [7])
constexpr int32_t kScrlBotH = 16;

// --- scroll-arrow buttons (spec §4 / Init :128, :147) ----------------
// Pane-local (169, 150) and (169, 174); 24x24 each. The Init call is
// FUN_0042c2d0(…, 0xa9, 0x96, 0x18, 0x18, …) for the up button and
// (…, 0xa9, 0xae, …) for the down button.
constexpr int32_t kArrowX     = 0xa9;   // 169 pane-local
constexpr int32_t kArrowUpY   = 0x96;   // 150 pane-local
constexpr int32_t kArrowDownY = 0xae;   // 174 pane-local
constexpr int32_t kArrowW     = 0x18;   // 24
constexpr int32_t kArrowH     = 0x18;   // 24

// --- per-spell row layout — content-surface-local (spec §4 / §6a) ----
// Row pitch: 0x5b=91 (fixed block) + lines*font_lineHeight. For the
// harness all single-line spell names → row pitch ~= 91 (we add the
// font line height since the spec rebuild does `lines*lineH + 0x5b`).
constexpr int32_t kRowPitchBase = 0x5b;   // 91 — fixed per-row block (§6a)
constexpr int32_t kIconX        = 12;     // ~12 + 0xc depth-bumped (§4)
constexpr int32_t kIconW        = 40;     // SpellIcons 40x40 (§2)
constexpr int32_t kIconH        = 40;
constexpr int32_t kNameX        = 10;     // 0xa (§4)
constexpr int32_t kNameYRel     = 0x2c;   // 44 — name baseline within row (§4)
constexpr int32_t kNameW        = 0x84;   // 132 (§4)
constexpr int32_t kDescX        = 0x36;   // 54 (§4)
constexpr int32_t kDescYRel     = 0;      // line 1 at row top (§4)
constexpr int32_t kDescLine2YRel = 0x10;  // 16 — line 2 (§4)
constexpr int32_t kDescW        = 0x55;   // 85 (§4)
constexpr int32_t kDescH        = 0x56;   // 86 (§4)
constexpr int32_t kGlyphXStart  = 0x0c;   // 12 — first talisman glyph x (§4)
constexpr int32_t kGlyphYRel    = 0x3d;   // 61 — glyph row within row (§4)
constexpr int32_t kGlyphPitch   = 0x10;   // 16 — per-glyph step (§4)
constexpr int32_t kGlyphW       = 20;     // spellscroll glyphs 20x20 (§2)
constexpr int32_t kGlyphH       = 20;
constexpr int32_t kSkillX       = 10;     // 0xa, same row as Mana (§4)
constexpr int32_t kManaX        = 0x5a;   // 90 (§4)
// Spec §4 literal says +0x2c (44 — same baseline as name); recon line 297/305
// `FUN_004be2b0(10/0x5a, pcVar12 + (0x2c - iVar10), 0x84, …)` confirms the
// row-relative y is 0x2c. But the reference image shows Skill/Mana BELOW the
// talisman-glyph row (which sits at +0x3d = 61) — so the literal `0x2c`
// placement collides with the name and over-prints the description. Render
// Skill/Mana below the glyph row at +82 (glyph row end + small gap) so the
// harness reads correctly; **spec gap: §4 Skill/Mana y likely needs another
// row offset added** (UNCONFIRMED).
constexpr int32_t kStatYRel     = 0x52;   // 82 — below glyph row (spec gap §4)
constexpr int32_t kStatW        = 0x84;   // 132 (§4)

// --- text colors (spec §8 PackRGB decode) ----------------------------
// PackRGB call(dst,B,G,R) → final RGB. From spec §8:
//   name cream  FUN_00429950(0xf2,0xe7,0xff) → RGB(0xff,0xe7,0xf2)
//   skill green FUN_00429950(5,    0xf4,0xf4) → RGB(0xf4,0xf4,5)
//   mana cyan   FUN_00429950(0xe9, 0xff,0x25) → RGB(0x25,0xff,0xe9)
constexpr float kNameR = 0xff / 255.0f, kNameG = 0xe7 / 255.0f, kNameB = 0xf2 / 255.0f;
constexpr float kSkillR = 0xf4 / 255.0f, kSkillG = 0xf4 / 255.0f, kSkillB = 0x05 / 255.0f;
constexpr float kManaR = 0x25 / 255.0f, kManaG = 0xff / 255.0f, kManaB = 0xe9 / 255.0f;
// Description: theme default (DAT_00667c2c, spec §8). A light parchment-
// reading color reads cleanly on the tan paper; near-black brown matches
// the visual intent of an ink-on-parchment description block.
constexpr float kDescColR = 0x40 / 255.0f, kDescColG = 0x28 / 255.0f, kDescColB = 0x10 / 255.0f;

// --- font (spec §8) --------------------------------------------------
// All cells: font_id 0x401 / 0x402 — both go through FUN_004be2b0 with bit
// 0x400 set (3-pass black shadow). Arimo-Regular @ ~11 px matches the
// Arial-metric retail font for HUD-scale text.
constexpr const char* kFontPath = "thirdparty/fonts/Arimo-Regular.ttf";
constexpr int32_t     kFontPx   = 11;

// --- scroll animation (spec §6b — step 0x28 = 40 px/tick) ------------
constexpr int32_t kScrollStep   = 0x28;       // 40 px / sim tick (§6b)
constexpr double  kSimTickMs    = 1000.0 / 24.0;  // 24Hz sim gate

// =====================================================================
// Asset roster — spec §2.
// =====================================================================
constexpr const char* kScrollDat     = "spellscroll.dat";
constexpr const char* kSpellIconsDat = "SpellIcons.dat";

// Ordered talisman name table — spec §2 footnote `PTR_DAT_005e4fb4` holds
// the 12 S* names in retail order: SSun, SLife, SOcean, SLaw, SSoul,
// SStars, SDeath, SChaos, SSky, SEarth, SWard, SMoon. Kept here so the
// harness can drive recipe glyphs by talisman name (spec §4 glyph row).
constexpr const char* kTalismanNames[12] = {
    "SSun", "SLife", "SOcean", "SLaw", "SSoul", "SStars",
    "SDeath", "SChaos", "SSky", "SEarth", "SWard", "SMoon",
};

TMulti*  g_scrollDat     = nullptr;
TMulti*  g_spellIconsDat = nullptr;

PTBitmap g_scrollFrame  = nullptr;   // 188x306 Scroll (chrome)
PTBitmap g_scrollPaper  = nullptr;   // 148x80 ScrollPaper (tile)
PTBitmap g_scrlTop      = nullptr;   // 32x12 ScrlTop cap
PTBitmap g_scrlBottom   = nullptr;   // 40x16 ScrlBottom cap
PTBitmap g_arrowUpUp    = nullptr;   // ScrlArwUU 24x24
PTBitmap g_arrowDownUp  = nullptr;   // ScrlArwDU 24x24

// Talisman glyphs keyed by name (S*); resolved at init from g_scrollDat.
PTBitmap g_talismanGlyphs[12] = { nullptr };

const SFontAtlas* g_font = nullptr;

TSurface* g_pane = nullptr;

// =====================================================================
// #8 iOS-style velocity scroll state.
//
// Physics:
//   - Drag: while mouse-down, track position changes per-tick to compute
//     velocity (pixels/sec). g_scrollY follows the drag position directly.
//   - Release: velocity decays with exponential friction (50%/tick at 24Hz
//     → very fast. Use 85%/tick = 0.85 decay factor for pleasant feel).
//   - Rubber-band: when scroll exceeds [0, maxScroll], the boundary
//     resistance increases (extra δy / 3 past edge). On release, spring
//     back with a simple lerp toward the clamped position (10% per tick).
//   - Snap: when |velocity| < 1.0 px/s AND within bounds, zero velocity.
//
// Auto-drive: if no drag is active, the harness applies a slow auto-scroll
// so filmstrip captures exercise mid-scroll layout (replaces old ping-pong).
// Auto-drive is disabled once any mouse drag has been detected.
// =====================================================================

int32_t g_scrollY    = 0;       // current scroll position (px, may be out-of-range during rubber-band)
double  g_velocityY  = 0.0;     // px / second
double  g_lastTickMs = 0.0;     // last 24Hz tick timestamp (ms)

// Drag state.
bool    g_dragging    = false;   // true while mouse is held
int32_t g_dragStartY  = 0;      // cursor y at mouse-down
int32_t g_dragScrollStart = 0;  // g_scrollY at mouse-down
double  g_dragLastMs  = 0.0;    // time of last drag-move sample
int32_t g_dragLastCurY = 0;     // cursor y at last move sample (for velocity)
bool    g_autoScrollActive = true;  // auto-drive active until first real drag
int32_t g_autoScrollDir = +1;       // auto-drive direction

// iOS rubber-band physics constants (spec #8).
constexpr double kFriction       = 0.85;   // velocity decay per 24Hz tick
constexpr double kSnapThreshold  = 1.0;    // px/s — velocity snap-to-zero
constexpr double kSpringRate     = 0.12;   // fraction per tick for edge spring-back
constexpr double kRubberBandDiv  = 3.0;    // resistance divisor past edge

// Pane screen coordinates for hit-testing (spec §3).
// pane is anchored at the right edge; the content surface is at
// pane-local (20, 38) with size 148×229.
constexpr int32_t kPaneScreenX = 452;      // sidebar pane origin x (UNCONFIRMED-A)
constexpr int32_t kPaneScreenY = 24;       // approximate test-mode placement y

// =====================================================================
// Per-spell harness binding — names from QuickSpell + Heal (spec §1
// "Test mode populates 4-5 representative spells"). Skill/mana values
// are representative; the talisman recipe (sequence of S* names) is the
// retail per-spell recipe — driven from a small static table here since
// the recon snapshot does not expose the per-spell talisman list in C++
// source (spec §14-G UNCONFIRMED).
// =====================================================================
struct SSpell
{
    const char* name;            // SpellIcons.dat entry name
    const char* desc;            // wrapped description (theme color)
    int32_t     skill;
    int32_t     mana;
    int32_t     talismans[6];    // indices into kTalismanNames[]; -1 terminator
    PTBitmap    icon;            // cached at init from g_spellIconsDat
};

SSpell g_spells[5] = {
    {
        "Advanced Healing",
        "Restores significant\nhealth to caster.",
        12, 35,
        { 1 /*SLife*/, 0 /*SSun*/, 4 /*SSoul*/, -1, -1, -1 },
        nullptr,
    },
    {
        "Iron Skin",
        "Hardens flesh to\nresist physical blows.",
        8, 20,
        { 9 /*SEarth*/, 10 /*SWard*/, 3 /*SLaw*/, -1, -1, -1 },
        nullptr,
    },
    {
        "Fire Flash",
        "A burst of fire\nstuns nearby foes.",
        6, 18,
        { 0 /*SSun*/, 7 /*SChaos*/, -1, -1, -1, -1 },
        nullptr,
    },
    {
        "Ice Bolt",
        "Hurls a freezing\nshard at the target.",
        5, 14,
        { 2 /*SOcean*/, 11 /*SMoon*/, -1, -1, -1, -1 },
        nullptr,
    },
    {
        "Heal",
        "Mends small wounds\non the caster.",
        2, 6,
        { 1 /*SLife*/, 0 /*SSun*/, -1, -1, -1, -1 },
        nullptr,
    },
};
constexpr int32_t kSpellCount = sizeof(g_spells) / sizeof(g_spells[0]);

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

// Per-row height = lines * lineH + 0x5b (spec §6a). For the harness the
// desc is 2 lines, name is 1 line; we use the spec rebuild formula with
// the wrapped-name lines count (1) → row pitch = lineH + 91.
int32_t RowPitch()
{
    const int32_t lineH = g_font ? int32_t(TextLineHeight(g_font) + 0.5f) : 12;
    return lineH + kRowPitchBase;   // lines=1 for our short names (§6a)
}

int32_t ContentTotalHeight()
{
    return RowPitch() * kSpellCount;  // sum of per-row heights (§6a)
}

// Max scroll = max(0, contentHeight - 229) (spec §6b clamp).
int32_t MaxScroll()
{
    const int32_t total = ContentTotalHeight();
    return total > kContentH ? (total - kContentH) : 0;
}

// =====================================================================
// HUD drawable — composes the whole pane RT then DrawSurface's it once
// (spec §3 direct-renderer contract).
// =====================================================================
class TSpellbookHud : public THudDrawable
{
public:
    void Draw() override
    {
        if (!g_pane) return;
        // Place the pane on the right side of the screen, top-anchored
        // with a small inset (the retail pane is owned by TSidePane's
        // upper region — exact screen origin is UNCONFIRMED-A in spec
        // §14; we hug the right edge to read as the upper sidebar).
        const int32_t dw = Display.Width();
        const int32_t x  = (dw > 0 ? dw : kPaneW) - kPaneW - 8;
        Renderer->DrawSurface(g_pane, x < 0 ? 0 : x, 24);
    }

    void Refresh()
    {
        if (!g_scrollFrame) return;
        EnsurePane();
        if (!g_pane) return;

        AdvanceScroll();

        const int32_t tw = g_pane->Width();
        const int32_t th = g_pane->Height();

        // Transparent clear so the pane composites over the test backdrop
        // (the chrome itself is opaque, so the keyed-edge of the round
        // scroll-rod art will show the backdrop where it punches through).
        g_pane->StartPass(0.0f, 0.0f, 0.0f, 0.0f);

        // --- (1) Scroll-frame chrome at pane-local (0,0) — spec §5 step 4
        //     (the pane DrawBackground composes [0x64]/Scroll which is the
        //     188x306 ornate frame surrounding the parchment viewport).
        Renderer->DrawBitmapToTarget(g_scrollFrame, 0, 0, tw, th);

        // --- (2) ScrollPaper vertical tile loop inside the content surface
        //     (spec §5 step 2): for iVar11 = -(scrollY mod 80); iVar11 < 320;
        //     iVar11 += 80. We compose tiles directly into the pane RT at
        //     the content origin — same visual result as composing into a
        //     separate content surface then DrawSurface'ing it (UI_METHOD_MAP
        //     §3 — single-RT compose is equivalent for opaque tiles).
        const int32_t tileOffY = -(g_scrollY % kTilePitch);
        for (int32_t y = tileOffY; y < (kContentH + kTilePitch); y += kTilePitch)
        {
            // Clip the tile to the content viewport: src y starts at 0 of the
            // tile, dst y at kContentOrigY + y; we use SubrectToTarget so the
            // tile can extend past the bottom without overflowing into the
            // bottom scroll-rod chrome.
            const int32_t dstY  = kContentOrigY + y;
            const int32_t srcY  = 0;
            int32_t       sh    = kPaperH;
            int32_t       drawY = dstY;
            int32_t       srcOffY = srcY;
            if (drawY < kContentOrigY)
            {
                const int32_t clip = kContentOrigY - drawY;
                srcOffY += clip;
                sh      -= clip;
                drawY    = kContentOrigY;
            }
            const int32_t bottomLimit = kContentOrigY + kContentH;
            if (drawY + sh > bottomLimit)
                sh = bottomLimit - drawY;
            if (sh <= 0) continue;
            Renderer->DrawBitmapSubrectToTarget(
                g_scrollPaper,
                kContentOrigX, drawY,
                0, srcOffY,
                kPaperW, sh,
                tw, th);
        }

        // --- (3) Per-spell row loop (spec §5 step 3 / §6c paint-window test).
        //     row_top accumulates down the column; iVar10 = scrollY - running
        //     translates row-local y to content-surface-local y. Intersect
        //     each row's [rowTop, rowTop+rowH] with the visible window
        //     [scrollY, scrollY+229]; skip non-intersecting rows.
        const int32_t rowH = RowPitch();
        for (int32_t i = 0; i < kSpellCount; ++i)
        {
            const int32_t rowTop = i * rowH;
            const int32_t rowBot = rowTop + rowH;
            if (rowBot <= g_scrollY)           continue;          // above window
            if (rowTop >= g_scrollY + kContentH) break;            // below window

            // iVar10 = (running) - scrollY in spec §6c. The content y of an
            // element at row-local rl is: rowTop + rl - scrollY (content-
            // surface-local), then + (kContentOrigX, kContentOrigY) → pane.
            const int32_t rowYInContent = rowTop - g_scrollY;

            DrawSpellRow(g_spells[i], rowYInContent, tw, th);
        }

        // --- (4) Top + bottom scroll-rod caps (spec §5 step 4).
        //     ScrlTop at content TL; ScrlBottom at content BL. They are
        //     composited OVER the content (mask the parchment edge where it
        //     meets the rods).
        if (g_scrlTop)
        {
            const int32_t topX = kContentOrigX + (kContentW - kScrlTopW) / 2;
            Renderer->DrawBitmapToTarget(
                g_scrlTop, topX, kContentOrigY - kScrlTopH / 2, tw, th);
        }
        if (g_scrlBottom)
        {
            const int32_t botX = kContentOrigX + (kContentW - kScrlBotW) / 2;
            Renderer->DrawBitmapToTarget(
                g_scrlBottom, botX,
                kContentOrigY + kContentH - kScrlBotH / 2, tw, th);
        }

        // --- (5) Scroll-arrow buttons (spec §4 / Init :128, :147). The
        //     retail buttons cycle between U/D/G art based on TButton state;
        //     this test mode shows the resting Up state (ScrlArwUU / DU).
        if (g_arrowUpUp)
            Renderer->DrawBitmapToTarget(g_arrowUpUp, kArrowX, kArrowUpY, tw, th);
        if (g_arrowDownUp)
            Renderer->DrawBitmapToTarget(g_arrowDownUp, kArrowX, kArrowDownY, tw, th);

        g_pane->EndPass();
    }

private:
    // Compose one spell row's visible elements (spec §4 / §5 step 3 inner
    // body). All coords are content-surface-local; compose to pane via
    // (kContentOrigX, kContentOrigY).
    static void DrawSpellRow(const SSpell& sp, int32_t rowYInContent,
                             int32_t tw, int32_t th)
    {
        const int32_t paneX0 = kContentOrigX;
        const int32_t paneY0 = kContentOrigY + rowYInContent;

        // (a) Spell circle icon — spec §4 / Draw FUN_004bd680(iStack_2d0,
        //     uStack_2b4, icon, 0x100/0x2000). 40x40 at row-local
        //     (~12 + depth, 0). Stretched-blit so undersized harness icons
        //     still render correctly into the 40x40 cell (most icons are
        //     exactly 40x40 — the stretch is identity in that case).
        if (sp.icon)
        {
            Renderer->DrawBitmapSubrectStretchedToTarget(
                sp.icon,
                paneX0 + kIconX, paneY0,
                kIconW, kIconH,
                0, 0, sp.icon->width, sp.icon->height,
                tw, th);
        }

        // (b) Spell name text — spec §8 row 1. cell (10, +44, 132, lineH),
        //     font 0x401, cream RGB(0xff,0xe7,0xf2), left-aligned, 3-pass
        //     black shadow (font flag 0x400 → DrawTextShadowedToTarget).
        if (g_font)
        {
            const int32_t lineH = int32_t(TextLineHeight(g_font) + 0.5f);
            DrawTextShadowedToTarget(
                g_font, sp.name,
                paneX0 + kNameX, paneY0 + kNameYRel,
                kNameW, lineH,
                ETextAlign::Left,
                kNameR, kNameG, kNameB,
                tw, th);

            // (c) Description / skill block — spec §8 row 2/3. cell (54, 0,
            //     85, 86), font 0x402, theme color, left-aligned, shadowed.
            //     Spec splits into two lines at +0 and +16; we emit one
            //     DrawTextShadowedToTarget per line for the explicit "\n"
            //     break in the harness desc, then let any further wrapping
            //     be the cell-clip (DrawTextShadowedToTarget is single-line).
            const char* desc = sp.desc;
            const char* nl   = std::strchr(desc, '\n');
            char line1[64], line2[64];
            if (nl)
            {
                const size_t n = (size_t)(nl - desc);
                const size_t cap1 = sizeof(line1) - 1;
                const size_t cp = n < cap1 ? n : cap1;
                std::memcpy(line1, desc, cp);
                line1[cp] = 0;
                std::snprintf(line2, sizeof(line2), "%s", nl + 1);
            }
            else
            {
                std::snprintf(line1, sizeof(line1), "%s", desc);
                line2[0] = 0;
            }
            DrawTextShadowedToTarget(
                g_font, line1,
                paneX0 + kDescX, paneY0 + kDescYRel,
                kDescW, lineH,
                ETextAlign::Left,
                kDescColR, kDescColG, kDescColB,
                tw, th);
            if (line2[0])
                DrawTextShadowedToTarget(
                    g_font, line2,
                    paneX0 + kDescX, paneY0 + kDescLine2YRel,
                    kDescW, lineH,
                    ETextAlign::Left,
                    kDescColR, kDescColG, kDescColB,
                    tw, th);

            // (d) "Skill: N" stat line — spec §8 row 4. cell (10, +44, 132),
            //     font 0x401, yellow-green RGB(0xf4,0xf4,0x05), shadowed.
            //     Format = "%s: %d" with label "Skill" (UNCONFIRMED-D label
            //     text — retail resolves SPANESKILLS via meth_0x49d800).
            char buf[32];
            std::snprintf(buf, sizeof(buf), "Skill: %d", sp.skill);
            DrawTextShadowedToTarget(
                g_font, buf,
                paneX0 + kSkillX, paneY0 + kStatYRel,
                kStatW, lineH,
                ETextAlign::Left,
                kSkillR, kSkillG, kSkillB,
                tw, th);

            // (e) "Mana: N" stat line — spec §8 row 5. cell (90, +44, 132),
            //     font 0x401, cyan RGB(0x25,0xff,0xe9), shadowed.
            std::snprintf(buf, sizeof(buf), "Mana: %d", sp.mana);
            DrawTextShadowedToTarget(
                g_font, buf,
                paneX0 + kManaX, paneY0 + kStatYRel,
                kStatW, lineH,
                ETextAlign::Left,
                kManaR, kManaG, kManaB,
                tw, th);
        }

        // (f) Talisman recipe glyph row — spec §4 / Draw glyph loop. start
        //     x = 12, step 16 per glyph; y = row top + 61. Each glyph is
        //     20x20 from spellscroll.dat:S<Talisman>, alpha (drawmode
        //     0x2000) — the bitmap's own alpha carries the composite.
        int32_t gx = kGlyphXStart;
        for (int32_t t = 0; t < 6 && sp.talismans[t] >= 0; ++t)
        {
            const int32_t idx = sp.talismans[t];
            if (idx < 0 || idx >= 12) continue;
            PTBitmap glyph = g_talismanGlyphs[idx];
            if (!glyph) { gx += kGlyphPitch; continue; }
            Renderer->DrawBitmapToTarget(
                glyph,
                paneX0 + gx, paneY0 + kGlyphYRel,
                tw, th);
            gx += kGlyphPitch;
        }
    }

    void EnsurePane()
    {
        if (g_pane) return;
        // Spec §3: pane is fixed 188x306. The whole compose (chrome +
        // content + arrows) lands in this RT; HUD-pass DrawSurface's it
        // once to the screen at the upper-sidebar slot.
        g_pane = new TSurface(kPaneW, kPaneH, SG_PIXELFORMAT_RGBA8);
    }

    // #8 iOS-style velocity scroll driver.
    // Called every Refresh() frame; applies momentum decay, rubber-band
    // spring-back, and (when no drag is active) a slow auto-scroll so the
    // harness filmstrip exercises mid-scroll layout.
    static void AdvanceScroll()
    {
        const double nowMs  = TTime::Time() * 1000.0;
        if (g_lastTickMs == 0.0) g_lastTickMs = nowMs;

        // Run 24Hz sim ticks to keep physics framerate-independent.
        const int32_t maxS  = MaxScroll();
        int32_t guard = 0;
        while (nowMs - g_lastTickMs >= kSimTickMs && guard < 64)
        {
            g_lastTickMs += kSimTickMs;
            ++guard;

            if (g_dragging)
            {
                // During drag: scroll follows cursor directly (done in
                // HandleMouseMove); velocity is computed from delta.
                continue;
            }

            // Auto-scroll: slow constant drift when no user drag has
            // happened yet. Disabled once any real drag fires.
            if (g_autoScrollActive)
            {
                if (maxS <= 0) { g_scrollY = 0; continue; }
                // 5 px / tick auto-speed — shows content scrolling gently.
                g_scrollY += g_autoScrollDir * 5;
                if (g_scrollY >= maxS) { g_scrollY = maxS; g_autoScrollDir = -1; }
                if (g_scrollY <= 0)    { g_scrollY = 0;    g_autoScrollDir = +1; }
                continue;
            }

            // Rubber-band spring-back: if out of bounds, spring toward edge.
            const bool outLow  = (g_scrollY < 0);
            const bool outHigh = (maxS > 0) && (g_scrollY > maxS);
            if (outLow || outHigh)
            {
                const int32_t target = outLow ? 0 : maxS;
                const double  delta  = (target - (double)g_scrollY) * kSpringRate;
                g_scrollY  = int32_t(g_scrollY + delta);
                g_velocityY = 0.0;  // zero velocity during spring-back
                continue;
            }

            // Normal momentum decay.
            if (std::abs(g_velocityY) < kSnapThreshold)
            {
                g_velocityY = 0.0;
                continue;
            }
            const double dtSec = kSimTickMs / 1000.0;
            g_scrollY   = int32_t(g_scrollY + g_velocityY * dtSec);
            g_velocityY *= kFriction;

            // Clamp and absorb velocity at edges.
            if (maxS <= 0)
            {
                g_scrollY  = 0;
                g_velocityY = 0.0;
            }
            else if (g_scrollY < 0)
            {
                g_scrollY   = 0;
                g_velocityY = 0.0;
            }
            else if (g_scrollY > maxS)
            {
                g_scrollY   = maxS;
                g_velocityY = 0.0;
            }
        }
    }
};

TSpellbookHud g_hud;

}  // namespace

// =====================================================================
// Entry points (testmodes.cpp links these — keep the signatures).
// =====================================================================
bool InitializeUISpellbookMode()
{
    log_info("[ui-spellbook] === SpellbookSidebarPane (clean-room from spec) ===");

    // Spec §2: spellscroll.dat owns Scroll + ScrollPaper + ScrlTop/Bottom
    // + ScrlArwU{U,D,G} / ScrlArwD{U,D,G} + the 12 S* talisman glyphs.
    g_scrollDat = TMulti::LoadMulti((char*)kScrollDat);
    if (g_scrollDat)
    {
        g_scrollFrame  = LookupByName(g_scrollDat, "Scroll");
        g_scrollPaper  = LookupByName(g_scrollDat, "ScrollPaper");
        g_scrlTop      = LookupByName(g_scrollDat, "ScrlTop");
        g_scrlBottom   = LookupByName(g_scrollDat, "ScrlBottom");
        g_arrowUpUp    = LookupByName(g_scrollDat, "ScrlArwUU");
        g_arrowDownUp  = LookupByName(g_scrollDat, "ScrlArwDU");
        for (int32_t i = 0; i < 12; ++i)
            g_talismanGlyphs[i] = LookupByName(g_scrollDat, kTalismanNames[i]);
    }

    log_info("[ui-spellbook] chrome: Scroll=%s Paper=%s Top=%s Bottom=%s "
             "ArrUp=%s ArrDn=%s",
             g_scrollFrame  ? "OK" : "MISS",
             g_scrollPaper  ? "OK" : "MISS",
             g_scrlTop      ? "OK" : "MISS",
             g_scrlBottom   ? "OK" : "MISS",
             g_arrowUpUp    ? "OK" : "MISS",
             g_arrowDownUp  ? "OK" : "MISS");
    if (g_scrollFrame)
        log_info("[ui-spellbook] Scroll %dx%d (expect 188x306)",
                 g_scrollFrame->width, g_scrollFrame->height);

    // Spec §2: SpellIcons.dat owns the 40x40 spell-circle icons keyed by
    // spell name. Same archive uiquickspelltest already uses.
    g_spellIconsDat = TMulti::LoadMulti((char*)kSpellIconsDat);
    if (g_spellIconsDat)
    {
        for (int32_t i = 0; i < kSpellCount; ++i)
            g_spells[i].icon = LookupByName(g_spellIconsDat, g_spells[i].name);
    }
    for (int32_t i = 0; i < kSpellCount; ++i)
        log_info("[ui-spellbook] spell[%d] '%s' icon=%s",
                 i, g_spells[i].name, g_spells[i].icon ? "OK" : "MISS");

    // Font: small Arimo for the spell-name + stat lines (spec §8 — retail
    // fonts 0x401/0x402, Arial-metric compatible).
    g_font = BuildTTFAtlas(kFontPath, kFontPx);
    log_info("[ui-spellbook] font %s @%dpx = %s",
             kFontPath, kFontPx, g_font ? "OK" : "MISS");

    delete g_pane;
    g_pane            = nullptr;
    g_scrollY         = 0;
    g_velocityY       = 0.0;
    g_lastTickMs      = 0.0;
    g_dragging        = false;
    g_autoScrollActive = true;
    g_autoScrollDir   = +1;

    Renderer->AddHud(&g_hud, 0.0f);
    return true;
}

void RenderUISpellbookMode()
{
    g_hud.Refresh();

    // Muted slate backdrop matching the other ui*test modes so the
    // parchment scroll's chrome reads in isolation (no playfield behind
    // the HUD in test mode).
    Display.BackBuffer()->StartPass(0.18f, 0.20f, 0.26f, 1.0f);
    Display.BackBuffer()->EndPass();
}

void CloseUISpellbookMode()
{
    Renderer->RemoveHud(&g_hud);
    delete g_pane;
    g_pane         = nullptr;
    g_scrollFrame  = g_scrollPaper = g_scrlTop = g_scrlBottom = nullptr;
    g_arrowUpUp    = g_arrowDownUp = nullptr;
    for (int32_t i = 0; i < 12; ++i) g_talismanGlyphs[i] = nullptr;
    for (int32_t i = 0; i < kSpellCount; ++i) g_spells[i].icon = nullptr;
    g_scrollDat     = nullptr;
    g_spellIconsDat = nullptr;
    g_font          = nullptr;
    g_scrollY         = 0;
    g_velocityY       = 0.0;
    g_lastTickMs      = 0.0;
    g_dragging        = false;
    g_autoScrollActive = true;
    g_autoScrollDir   = +1;
}

// =====================================================================
// #8 iOS-style velocity scroll — mouse handlers.
// =====================================================================


// Hit-test helper: returns true if (x, y) is within the spellbook
// content area on screen. The pane is right-anchored at screen x =
// (display_w - kPaneW); the content surface is at pane-local (20, 38)
// with size 148×229. kPaneScreenX/Y are harness approximations (UNCONFIRMED-A).
static bool SpellbookHitTest(int32_t x, int32_t y)
{
    const int32_t dw   = Display.Width()  > 0 ? Display.Width()  : 640;
    const int32_t paneX = dw - kPaneW;
    const int32_t paneY = kPaneScreenY;
    // Content area on screen:
    const int32_t cx0 = paneX + kContentOrigX;
    const int32_t cy0 = paneY + kContentOrigY;
    const int32_t cx1 = cx0 + kContentW;
    const int32_t cy1 = cy0 + kContentH;
    return (x >= cx0 && x < cx1 && y >= cy0 && y < cy1);
}

void HandleMouseMoveUISpellbookMode(int32_t button, int32_t x, int32_t y)
{
    if (!(button & MB_LEFTDOWN)) { g_dragging = false; return; }
    if (!g_dragging) return;

    const double nowMs = TTime::Time() * 1000.0;
    // Compute velocity from last move sample.
    const double dtMs = nowMs - g_dragLastMs;
    if (dtMs > 0.0)
    {
        const int32_t dy = y - g_dragLastCurY;
        // velocity in px/s (negative dy = scroll down = positive scrollY)
        g_velocityY = -(dy / (dtMs / 1000.0));
    }
    g_dragLastMs   = nowMs;
    g_dragLastCurY = y;

    // Direct-follow: scroll position tracks cursor.
    const int32_t dy = y - g_dragStartY;
    const int32_t maxS = MaxScroll();

    int32_t newScroll = g_dragScrollStart - dy;

    // Rubber-band past edges: δy / kRubberBandDiv resistance.
    if (newScroll < 0)
        newScroll = int32_t(newScroll / kRubberBandDiv);
    else if (maxS > 0 && newScroll > maxS)
        newScroll = maxS + int32_t((newScroll - maxS) / kRubberBandDiv);

    g_scrollY = newScroll;
}

void HandleMouseClickUISpellbookMode(int32_t button, int32_t x, int32_t y)
{
    // Coordinate note: screen coords passed in from testmodes dispatcher.
    // We convert to pane-local inside the hit-test helper.

    if (button == MB_LEFTDOWN)
    {
        if (!SpellbookHitTest(x, y)) return;
        // Disable auto-scroll once user first drags.
        g_autoScrollActive = false;
        g_dragging         = true;
        g_dragStartY       = y;
        g_dragScrollStart  = g_scrollY;
        g_dragLastMs       = TTime::Time() * 1000.0;
        g_dragLastCurY     = y;
        g_velocityY        = 0.0;
    }
    else if (button == MB_LEFTUP)
    {
        g_dragging = false;
        // Velocity is already set from the last move sample; momentum
        // decay + rubber-band spring-back runs in AdvanceScroll().
    }

    // Scroll arrows (spec §10): up-arrow at pane-local (169,150),
    // down-arrow at pane-local (169,174). Both are 24×24 hit-rects.
    // The actual click handling for the retail arrows uses TButton
    // dispatch; in the test harness we do a simple hit-rect check.
    if (button == MB_LEFTDOWN)
    {
        const int32_t dw   = Display.Width()  > 0 ? Display.Width()  : 640;
        const int32_t paneX = dw - kPaneW;
        const int32_t paneY = kPaneScreenY;

        const int32_t lx = x - paneX;
        const int32_t ly = y - paneY;

        // Up arrow: pane-local (169, 150) 24×24
        if (lx >= kArrowX && lx < kArrowX + kArrowW &&
            ly >= kArrowUpY && ly < kArrowUpY + kArrowH)
        {
            {
                const int32_t v = g_scrollY - kScrollStep;
                g_scrollY = v > 0 ? v : 0;
            }
            g_velocityY = 0.0;
        }
        // Down arrow: pane-local (169, 174) 24×24
        else if (lx >= kArrowX && lx < kArrowX + kArrowW &&
                 ly >= kArrowDownY && ly < kArrowDownY + kArrowH)
        {
            const int32_t ms = MaxScroll();
            const int32_t v  = g_scrollY + kScrollStep;
            g_scrollY = v < ms ? v : ms;
            g_velocityY = 0.0;
        }
    }
}
