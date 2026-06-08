// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uistatstest.cpp - --test=ui-stats                                    *
// *************************************************************************
//
// Clean-room reconstruction of TStatPane (right-sidebar Stats sheet).
// Built ONLY from docs/ui/forensics/CharacterStatsPane_SPEC.md
// (with FORENSICS_PROTOCOL / NOMENCLATURE / UI_METHOD_MAP / RECONSTRUCTION_PROTOCOL).
//
// What this pane draws (spec §1 / §5 / §8):
//   - "Stats" parchment chrome (statspane.dat idx 0, 188x306 RGB555 opaque)
//     blitted at pane-local (0,0) as the whole backdrop (spec §2, §5 step 2).
//   - The CLASS "Player" FORMAT "Page1" block of statpane.def (lines 6-127):
//     POS 28 62, LINEHEIGHT 12, then a stack of NEXTLINE-separated rows
//     printing labels in cream + colored values (aqua/teal/red/blue/green/
//     yellow). The 6 trainable-stat rows use TAB to push to columns
//     x=40 (right-align), x=42, x=84 (right-align), x=98 (spec §8).
//   - Each TEXT/FIELD goes through the §6b 3-pass black shadow + colored
//     base pattern, which is exactly what the canonical
//     font.h DrawTextShadowedToTarget implements (UI_METHOD_MAP §5).
//
// Architecture (spec §3 direct-renderer contract): compose the WHOLE pane
// (chrome + every text line) into ONE offscreen TSurface render target via
// the *ToTarget primitive family, then DrawSurface it once in the HUD pass.
// This mirrors uiplyrstatusbartest / uisidetabstest / uibottombartest —
// the canonical compose-to-target contract.
//
// Primitives (UI_METHOD_MAP §12 — canonical shared toolbox only):
//   Renderer->DrawBitmapToTarget        — opaque chrome stamp
//   font.h DrawTextShadowedToTarget     — text with 3-pass black shadow
// No hand-rolled glyph walks or shadow passes; no procedural stand-ins.
//
// Scope note (spec §0 / §14):
//   The FIELD value getter (FUN_00547240 — UNCONFIRMED-1) and the full DEF
//   interpreter (FUN_005475e0) are out of scope for this test mode; the
//   forensics agent flagged the value-resolver as an open question. We
//   render synthetic but plausible values (matching the reference player
//   "Locke" style) directly through the same TEXT/FIELD coordinate model
//   the interpreter would use — POS 28 62 + LINEHEIGHT 12 + TAB-from-line-
//   start. So the chrome, layout, colors, shadow, and right-alignment are
//   all spec-accurate; only the field-name→value lookup is stubbed pending
//   the DEF engine port. The intent is: a stranger comparing this capture
//   to a retail Stats-tab screenshot can verify every coord and color.
//
// *************************************************************************

#include "uistatstest.h"

#include "bitmap.h"
#include "bitmapatlas.h"
#include "display.h"
#include "font.h"
#include "logging.h"
#include "multi.h"
#include "renderer.h"
#include "surface.h"

#include <cstdint>
#include <cstdio>
#include <cstring>

namespace {

// =====================================================================
// Geometry constants — all cited to CharacterStatsPane_SPEC.md.
// =====================================================================

// --- pane (spec §3 / ctor :6-14) -------------------------------------
// 0xbc x 0x132 = 188 x 306. Pane-local origin (0,0). On-screen (x,y) is
// owned by TSidePane (UNCONFIRMED-3) — for the harness we right-anchor.
constexpr int32_t kPaneW = 0xbc;          // 188 (§3 newwidth)
constexpr int32_t kPaneH = 0x132;         // 306 (§3 newheight)

// --- cursor defaults (spec §6a interpreter init :106-113) ------------
// POS 28 62 from statpane.def:29 — drives lineStartX, cursorX, cursorY.
// LINEHEIGHT 12 from statpane.def:8 — every NEXTLINE drops cursorY by 12.
constexpr int32_t kStartX     = 28;        // §8 / def :29 (POS 28 62)
constexpr int32_t kStartY     = 62;        // §8 / def :29 (POS 28 62)
constexpr int32_t kLineHeight = 12;        // §6a / def :8 (LINEHEIGHT 12)

// --- pane on-screen placement ---------------------------------------
// Retail TSidePane places upper content at the right-sidebar top slot:
// x = display_w - 188, y = 0. The pane still authors everything pane-local
// into its own render surface.
constexpr int32_t kPaneY = 0;

// --- DEF COLOR palette (spec §8 / def :20-27) ------------------------
// Cream default (255,240,215, def :20); named overrides for value tokens.
struct SRgb { float r, g, b; };
constexpr SRgb kColCream  = { 255.0f/255.0f, 240.0f/255.0f, 215.0f/255.0f };  // def :20 default
constexpr SRgb kColAqua   = { 113.0f/255.0f, 187.0f/255.0f, 255.0f/255.0f };  // def :21
constexpr SRgb kColTeal   = {  95.0f/255.0f, 228.0f/255.0f, 193.0f/255.0f };  // def :22
constexpr SRgb kColRed    = { 255.0f/255.0f, 150.0f/255.0f, 150.0f/255.0f };  // def :24 (bumped — §11)
constexpr SRgb kColBlue   = { 110.0f/255.0f, 160.0f/255.0f, 255.0f/255.0f };  // def :25
constexpr SRgb kColGreen  = {   6.0f/255.0f, 221.0f/255.0f,   0.0f/255.0f };  // def :26
constexpr SRgb kColYellow = { 255.0f/255.0f, 247.0f/255.0f,  73.0f/255.0f };  // def :27

// --- font (spec §2 UNCONFIRMED-2 + UI_METHOD_MAP §5/§12) -------------
// The retail sidebar font is a shared global (DAT_0065abc4) selected into
// the HDC; UNCONFIRMED-2 — almost certainly the gold parchment face but
// the literal value was not read. Use the canonical small Arimo atlas
// (Arial-metric-compatible) at ~12 px so the cells laid out by POS 28 62
// + LINEHEIGHT 12 stack correctly; this matches every other HUD text path
// in the port (project-ui-text-rendering).
constexpr const char* kFontPath = "thirdparty/fonts/Arimo-Regular.ttf";
constexpr int32_t     kFontPx   = 11;       // §8 12px GDI; Arimo @11 reads ~12 px tall

// --- assets (spec §2) ------------------------------------------------
constexpr const char* kArchive    = "statspane.dat";   // §2 asset roster
constexpr const char* kChromeName = "Stats";           // §2 idx 0 (188x306)

TMulti*           g_statspaneDat = nullptr;
PTBitmap          g_chrome       = nullptr;
const SFontAtlas* g_font         = nullptr;

TSurface*         g_pane         = nullptr;
bool              g_hudVisible   = true;

// =====================================================================
// Synthetic player state — drives the per-FIELD values until the DEF
// engine + FUN_00547240 value-resolver port (spec §14 UNCONFIRMED-1).
// Values are plausible mid-game numbers matching the reference player.
// =====================================================================
struct SPlayerSheet
{
    const char* name;
    const char* klass;
    int32_t     level, exp, nextexp;
    int32_t     health, maxhealth;
    int32_t     mana,   maxmana;
    int32_t     trainstrn, strn, strn_dmg, strn_hit;
    int32_t     traincons, cons, cons_hlth, cons_ftg;
    int32_t     trainagil, agil, agil_atk;
    int32_t     trainrflx, rflx, rflx_def;
    int32_t     trainmind, mind, mind_mana, mind_spl;
    int32_t     trainluck, luck, luck_rolls;
    int32_t     armor, damage;
};

// Reference player matches the Locke-style sample player used by
// uiplyrstatusbartest. The trainXxx counters seed at 0 (no points to spend).
SPlayerSheet g_player = {
    "Locke", "Knight",
    26, 12500, 14000,
    1833, 1930,
    2174, 2650,
    0, 18, 4, 2,
    0, 16, 8, 6,
    0, 14, 3,
    0, 12, 2,
    0, 10, 1, 1,
    0,  8, 4,
    24, 12,
};

// =====================================================================
// Cursor state used by the per-line emitters. Mirrors the §6a interpreter
// model: lineStartX is the snap-back x set by POS (initially 28); cursorX
// is the running pen; cursorY advances by kLineHeight per NEXTLINE. TAB n
// sets cursorX = lineStartX + n (RELATIVE to line start — spec §6a).
// =====================================================================
struct SCursor
{
    int32_t lineStartX = kStartX;
    int32_t cursorX    = kStartX;
    int32_t cursorY    = kStartY;
};

void POS(SCursor& c, int32_t x, int32_t y)          // spec §6a POS
{
    c.cursorX = x;
    c.cursorY = y;
    c.lineStartX = x;
}
void NEXTLINE(SCursor& c)                            // spec §6a NEXTLINE
{
    c.cursorY += kLineHeight;
    c.cursorX  = c.lineStartX;
}
void TAB(SCursor& c, int32_t n)                      // spec §6a TAB n (relative)
{
    c.cursorX = c.lineStartX + n;
}

// =====================================================================
// One "TEXT" / "FIELD" emit — spec §6b WriteText pattern. Draws the run
// at (cursorX, cursorY) with the §6b 3-pass black shadow + colored base
// (the canonical font.h primitive does both passes in one call), then
// advances cursorX by the drawn text width. Alignment defaults to left
// (the cell is set wide enough to fit the run and the cell origin is the
// pen; left-aligned + cell origin == TextOut anchor).
//
// For `right`-aligned FIELDs (the trainXxx + stat-value columns; spec
// §8 / def :62-66), pass `rightAt` = the pen x; the cell is left-anchored
// at (pen - cellW) with Right alignment so the digits END at pen — exactly
// the SetTextAlign(TA_RIGHT) GDI semantics (spec §6a :316-319).
//
// `cellH` is set to kLineHeight + a small pad so the baseline lands where
// GDI's DT_TOP / TextOutA top-aligns it; the per-cell vertical baseline
// is handled once inside font.cpp (NOMENCLATURE §2 / project-ui-text-rendering).
// =====================================================================
constexpr int32_t kCellH    = kLineHeight + 2;       // baseline pad (font.cpp owns the metric)
constexpr int32_t kCellWMax = kPaneW;                // generous cell — left-anchor at pen

int32_t TextRunW(const char* s)
{
    return g_font ? (int32_t)(TextWidth(g_font, s) + 0.5f) : 0;
}

void EmitText(SCursor& c, const char* text, SRgb col, int32_t tw, int32_t th)
{
    if (!g_font || !text || !*text) return;
    const int32_t w = TextRunW(text);
    DrawTextShadowedToTarget(g_font, text,
                             c.cursorX, c.cursorY, kCellWMax, kCellH,
                             ETextAlign::Left,
                             col.r, col.g, col.b, tw, th);
    c.cursorX += w;
}

// Right-aligned FIELD — the cell is sized to fit the run, and anchored so
// its right edge sits at `rightAt`. The pen does NOT advance after a
// right-aligned FIELD (TA_NOUPDATECP, spec §6a :316-319 — the next TAB sets
// the pen explicitly). Caller is responsible for issuing the next TAB.
void EmitRight(const char* text, int32_t rightAt, int32_t y, SRgb col,
               int32_t tw, int32_t th)
{
    if (!g_font || !text || !*text) return;
    const int32_t w     = TextRunW(text);
    const int32_t cellW = w + 2;        // tight cell so Right-align lands at rightAt
    const int32_t cellX = rightAt - cellW;
    DrawTextShadowedToTarget(g_font, text,
                             cellX, y, cellW, kCellH,
                             ETextAlign::Right,
                             col.r, col.g, col.b, tw, th);
}

// =====================================================================
// Per-stat row emitter (spec §8 — per-stat row x-stops).
//   TAB 12 → x=40   trainXxx field (right-aligned)
//   TAB 14 → x=42   "Strn"/"Cons"/etc label (cream, left)
//   TAB 56 → x=84   stat value (right-aligned, color = cream)
//   TAB 70 → x=98   mods column (cream, "Dmg<v> Hit<v>" / "Hlth<v> Ftg<v>"...)
//
// The "right" variant means the digits end at the pen, not start there
// (spec §8 visual-anchor check). lineStartX is 28 (from POS 28 62) so the
// per-stat x-stops are lineStartX + TAB: 40, 42, 84, 98.
// =====================================================================
void EmitStatRow(SCursor& c,
                 const char* statLabel,
                 int32_t trainPts,
                 int32_t statVal,
                 const char* modLabel1, int32_t modVal1,
                 const char* modLabel2, int32_t modVal2,   // pass null for unused
                 int32_t tw, int32_t th)
{
    char buf[32];

    // TAB 12 → x=40, right-aligned trainXxx FIELD.
    // Color: GREEN when trainable points > 0, cream otherwise (spec §10
    // grey/active state — `0x3c3c3c` grey for unspent=0). We use cream for
    // the inactive state since the harness has no spendable-points source;
    // green when synthetic trainPts > 0 to demonstrate the active branch.
    TAB(c, 12);
    std::snprintf(buf, sizeof(buf), "%d", trainPts);
    EmitRight(buf, c.cursorX, c.cursorY,
              trainPts > 0 ? kColGreen : kColCream, tw, th);

    // TAB 14 → x=42, stat label (cream).
    TAB(c, 14);
    EmitText(c, statLabel, kColCream, tw, th);

    // TAB 56 → x=84, stat value (right-aligned, cream).
    TAB(c, 56);
    std::snprintf(buf, sizeof(buf), "%d", statVal);
    EmitRight(buf, c.cursorX, c.cursorY, kColCream, tw, th);

    // TAB 70 → x=98, mods column. Each mod is "<label><value>"; the FIELD
    // with inline-label form (def :68 `FIELD "Dmg" strnmod1`) prints the
    // quoted label immediately before the resolved value in the same color
    // (spec §8 "FIELD with inline label" note).
    TAB(c, 70);
    if (modLabel1)
    {
        std::snprintf(buf, sizeof(buf), "%s%d", modLabel1, modVal1);
        EmitText(c, buf, kColCream, tw, th);
    }
    if (modLabel2)
    {
        std::snprintf(buf, sizeof(buf), "%s%d", modLabel2, modVal2);
        EmitText(c, buf, kColCream, tw, th);
    }
}

// =====================================================================
// HUD drawable — composes the chrome + every Page1 text line into one RT
// (spec §3 direct-renderer contract), DrawSurface'd once at the right-
// anchored pane origin.
// =====================================================================
class TStatPaneHud : public THudDrawable
{
public:
    void Draw() override
    {
        if (!g_hudVisible || !g_pane) return;
        Renderer->DrawSurface(g_pane, m_paneX, m_paneY);
    }

    void Refresh()
    {
        if (!g_hudVisible) return;
        if (!g_chrome) return;
        EnsurePane();
        if (!g_pane) return;

        const int32_t tw = g_pane->Width();
        const int32_t th = g_pane->Height();

        // Transparent clear — the chrome is opaque RGB555 (kc=0, no alpha
        // — spec §2 chrome blit is opaque); the parchment fills the whole
        // 188x306 pane so transparency is irrelevant under the chrome but
        // matters around any compositing margin.
        g_pane->StartPass(0.0f, 0.0f, 0.0f, 0.0f);

        // --- spec §5 step 2: chrome blit -------------------------------
        //   FUN_004bd680("Stats", 0, 0, target, drawmode)
        //   = opaque sprite stamp of the full 188x306 "Stats" at (0,0).
        // Port primitive: DrawBitmapToTarget (opaque, dst full size).
        Renderer->DrawBitmapToTarget(g_chrome, 0, 0, tw, th);

        // --- spec §5 step 7: walk the DEF Player/Page1 block ----------
        // Static cursor initialized by POS 28 62 (def :29). Each emit
        // advances the pen by the drawn text width; NEXTLINE drops the
        // cursor by LINEHEIGHT (12) and snaps x back to lineStartX (28).
        // (BUTTON tokens at def :11-18 are widgets, drawn by TButtonPane
        // base — not text — and out of scope for this static reconstruction.)
        SCursor c;
        POS(c, kStartX, kStartY);                       // POS 28 62

        // line y=62: "Name: " + FIELD aqua name        (def :30-31)
        EmitText(c, "Name: ", kColCream, tw, th);
        EmitText(c, g_player.name, kColAqua, tw, th);

        // line y=74: "Class: " + FIELD aqua class      (def :33-35)
        NEXTLINE(c);
        EmitText(c, "Class: ", kColCream, tw, th);
        EmitText(c, g_player.klass, kColAqua, tw, th);

        // line y=86: "Lvl: <teal> Exp: <teal> Nxt: <teal>"  (def :37-43)
        NEXTLINE(c);
        EmitText(c, "Lvl: ", kColCream, tw, th);
        EmitNumber(c, g_player.level,   kColTeal, tw, th);
        EmitText(c, " Exp: ", kColCream, tw, th);
        EmitNumber(c, g_player.exp,     kColTeal, tw, th);
        EmitText(c, " Nxt: ", kColCream, tw, th);
        EmitNumber(c, g_player.nextexp, kColTeal, tw, th);

        // line y=98: "Hlth: <red>/<red> Mana: <blue>/<blue>"   (def :45-53)
        NEXTLINE(c);
        EmitText(c, "Hlth: ", kColCream, tw, th);
        EmitNumber(c, g_player.health,    kColRed,  tw, th);
        EmitText(c, "/", kColRed, tw, th);                       // def :48 (red "/")
        EmitNumber(c, g_player.maxhealth, kColRed,  tw, th);
        EmitText(c, " Mana: ", kColCream, tw, th);
        EmitNumber(c, g_player.mana,      kColBlue, tw, th);
        EmitText(c, "/", kColBlue, tw, th);                      // def :52 (blue "/")
        EmitNumber(c, g_player.maxmana,   kColBlue, tw, th);

        // line y=110: TAB 40 → x=68; "Stats"(green)"/Skills"(cream)  (def :55-58)
        NEXTLINE(c);
        TAB(c, 40);
        EmitText(c, "Stats",  kColGreen, tw, th);
        EmitText(c, "/Skills", kColCream, tw, th);

        // -------- 6 trainable-stat rows (def :60-121) -----------------
        // Each row: TAB 12 trainXxx (right) · TAB 14 label · TAB 56 value (right) ·
        //           TAB 70 mods("LabelN"). lineStartX is still 28 so the x-stops
        //           are absolute 40 / 42 / 84 / 98 (spec §8 per-stat row x-stops).
        NEXTLINE(c);                                                // y=120 Strn
        EmitStatRow(c, "Strn", g_player.trainstrn, g_player.strn,
                    "Dmg", g_player.strn_dmg, " Hit", g_player.strn_hit, tw, th);

        NEXTLINE(c);                                                // y=132 Cons
        EmitStatRow(c, "Cons", g_player.traincons, g_player.cons,
                    "Hlth", g_player.cons_hlth, " Ftg", g_player.cons_ftg, tw, th);

        NEXTLINE(c);                                                // y=144 Agil
        EmitStatRow(c, "Agil", g_player.trainagil, g_player.agil,
                    "Atk",  g_player.agil_atk, nullptr, 0, tw, th);

        NEXTLINE(c);                                                // y=156 Rflx
        EmitStatRow(c, "Rflx", g_player.trainrflx, g_player.rflx,
                    "Def",  g_player.rflx_def, nullptr, 0, tw, th);

        NEXTLINE(c);                                                // y=168 Mind
        EmitStatRow(c, "Mind", g_player.trainmind, g_player.mind,
                    "Mana ", g_player.mind_mana, " Spl ", g_player.mind_spl, tw, th);

        NEXTLINE(c);                                                // y=180 Luck
        EmitStatRow(c, "Luck", g_player.trainluck, g_player.luck,
                    "Rolls ", g_player.luck_rolls, nullptr, 0, tw, th);

        // line y=192: "Armor: <yellow> Dmg: <yellow>"   (def :123-127)
        NEXTLINE(c);
        EmitText(c, "Armor: ", kColCream, tw, th);
        EmitNumber(c, g_player.armor,  kColYellow, tw, th);
        EmitText(c, " Dmg: ",   kColCream, tw, th);
        EmitNumber(c, g_player.damage, kColYellow, tw, th);

        g_pane->EndPass();
    }

private:
    // %d emit — synthetic FIELD value stand-in (the real FUN_00547240
    // resolver is UNCONFIRMED-1, spec §14). The numeric format is "%d" per
    // every FIELD in the Page1 block (def :40-127, all integer fields).
    static void EmitNumber(SCursor& c, int32_t v, SRgb col,
                           int32_t tw, int32_t th)
    {
        char buf[16];
        std::snprintf(buf, sizeof(buf), "%d", v);
        EmitText(c, buf, col, tw, th);
    }

    void EnsurePane()
    {
        if (g_pane) { PlacePane(); return; }
        // Compose the whole 188x306 pane into one RT (spec §3 direct-
        // renderer contract). RGBA8 for the alpha clear + colored text.
        g_pane = new TSurface(kPaneW, kPaneH, SG_PIXELFORMAT_RGBA8);
        PlacePane();
    }

    void PlacePane()
    {
        const int32_t dw = Display.Width();
        m_paneX = (dw > 0 ? dw : kPaneW) - kPaneW;
        if (m_paneX < 0) m_paneX = 0;
        m_paneY = kPaneY;
    }

    int32_t m_paneX = 0;
    int32_t m_paneY = 0;
};

TStatPaneHud g_hud;

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

}  // namespace

// =====================================================================
// Entry points (testmodes.cpp links these — keep the signatures).
// =====================================================================
bool InitializeUIStatsMode()
{
    log_info("[ui-stats] === TStatPane (clean-room from spec) ===");

    // Spec §2: real retail statspane.dat — "Stats" chrome at idx 0 (188x306).
    g_statspaneDat = TMulti::LoadMulti((char*)kArchive);
    RegisterUIBitmapAtlasArchive(g_statspaneDat);
    if (g_statspaneDat)
        g_chrome = LookupByName(g_statspaneDat, kChromeName);

    log_info("[ui-stats] assets: Stats chrome=%s", g_chrome ? "OK" : "MISS");
    if (g_chrome)
        log_info("[ui-stats] Stats chrome %dx%d (expect 188x306)",
                 g_chrome->width, g_chrome->height);

    g_font = BuildTTFAtlas(kFontPath, kFontPx);
    log_info("[ui-stats] font %s @%dpx = %s",
             kFontPath, kFontPx, g_font ? "OK" : "MISS");

    delete g_pane;
    g_pane = nullptr;
    g_hudVisible = true;

    Renderer->AddHud(&g_hud, 0.0f);
    return true;
}

void RenderUIStatsMode()
{
    RenderUIStatsModeEmbedded();

    // Backdrop so the parchment chrome reads in isolation (no playfield
    // behind the right-anchored sidebar in test mode). Use the same muted
    // slate as the other sidebar test modes so eyeballing relative tints
    // is consistent across captures.
    Display.BackBuffer()->StartPass(0.18f, 0.20f, 0.26f, 1.0f);
    Display.BackBuffer()->EndPass();
}

void RenderUIStatsModeEmbedded()
{
    if (!g_hudVisible) return;
    g_hud.Refresh();
}

void SetUIStatsModeVisible(bool visible)
{
    g_hudVisible = visible;
}

void CloseUIStatsMode()
{
    Renderer->RemoveHud(&g_hud);
    delete g_pane;
    g_pane         = nullptr;
    g_chrome       = nullptr;
    g_statspaneDat = nullptr;
    g_font         = nullptr;
    g_hudVisible   = true;
}
