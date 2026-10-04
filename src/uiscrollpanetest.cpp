// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uiscrollpanetest.cpp - --test=ui-scrollpane                          *
// *************************************************************************
//
// Clean-room reconstruction of TScrollPane (cls_0x5b5750) — the in-game
// parchment scroll reader. Built ONLY from
// docs/ui/forensics/ScrollPane_SPEC.md. Every coordinate, asset, color,
// shadow, and algorithm cites the spec section inline.
//
// What this panel draws (spec §1 / §4 / §5):
//   - 570×374 `scroll` parchment chrome (scroll.dat) centred on the
//     640×480 logical screen at chip origin (35, 26) — spec §3.
//     drawmode = DM_TRANSPARENT | DM_BACKGROUND (chroma-keyed, opaque).
//   - Body text rendered in cell screen (100, 94, 430, 230):
//       startline=line, wrap at 430px, clipped to numLinesPerPage lines.
//       3-pass SE black shadow + sepia/brown text via DrawTextShadowedToTarget.
//       (UNCONFIRMED-E: retail uses "Scroll" BMFONT; we approximate with
//        Arimo-Regular 12px sepia #4a2810.)
//   - Optional 40×40 spell-icon bitmapped at scroll-local (16, 68) for spell
//     scrolls (the reference image shows a circular icon in the upper-left
//     of the text area — spec §14 UNCONFIRMED-F discusses placement; we put
//     it beside the text cell, offset left by icon width + gap).
//   - Three button sprites drawn in their registered positions (resting Up
//     state; Down state activated by harness during simulated input):
//       down  button: screen (487, 356) 28×31 — downup/downdown
//       up    button: screen (519, 349) 28×31 — upup/updown
//       exit  button: screen (553, 347) 28×31 — exitup/exitdown
//
// Architecture (spec §3 direct-renderer contract):
//   The retail panel draws directly to the display surface (no scratch
//   buffer). The port still uses the compose-to-target contract for
//   correctness: compose the whole pane into a 640×480 RT, then DrawSurface
//   it once in the HUD pass. This is equivalent because the pane IS the
//   full 640×480 viewport. (RECONSTRUCTION_PROTOCOL §3 "compose chip to
//   fixed RT, blit in HUD pass".)
//
// Primitives (UI_METHOD_MAP §12):
//   Renderer->DrawBitmapToTarget         — chrome stamp + button sprites
//   font.h DrawTextShadowedToTarget      — 3-pass black shadow + sepia text
//   font.h TextLineHeight                — numLinesPerPage calculation
// No hand-rolled shadow passes or glyph walks.
//
// Test-harness driver (spec §10):
//   - Two synthetic content bindings exercising item-scroll and spell-scroll
//     variants. The harness auto-pages every kAutoPageMs milliseconds so a
//     filmstrip capture covers multiple page positions.
//   - UIScrollPane::SetContent / Scroll / GetScrollLine are accessible from
//     input scripts for targeted filmstrip captures.
//
// Spec gaps noted during build:
//   UNCONFIRMED-E — "Scroll" font identity; approximated with Arimo 12px.
//   UNCONFIRMED-F — sidebar vs centred placement; using centred 640×480.
//   UNCONFIRMED-D — body text shadow flag; reproduced with shadow per §7.
//
// *************************************************************************

#include "uiscrollpanetest.h"

#include "bitmap.h"
#include "display.h"
#include "font.h"
#include "logging.h"
#include "multi.h"
#include "renderer.h"
#include "surface.h"
#include "time.h"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace {

// =====================================================================
// Geometry constants — all cited to ScrollPane_SPEC.md.
// =====================================================================

// --- pane rect (spec §3 ctor) ----------------------------------------
// TScrollPane ctor sets (0,0,640,480) — the pane IS the full Classic
// screen (spec §3 §ctor). Both the port RT and the logical screen share
// this rect.
constexpr int32_t kScreenW = 640;
constexpr int32_t kScreenH = 480;

// --- chip origin (spec §3 / DrawBackground :46-47) -------------------
// x = (640 − 570) / 2 = 35 (literal 0x23, DrawBackground :46)
// y = (480 − 374) / 4 = 26 (literal 0x1a, DrawBackground :47) — NOTE
//   the spec §3 confirms the quirky /4 (not /2): the parchment hangs
//   near the top, matching the visual. "NOT /2 — the /4 is the literal."
constexpr int32_t kScrollW    = 570;   // scroll bitmap width  (spec §2 dump)
constexpr int32_t kScrollH    = 374;   // scroll bitmap height (spec §2 dump)
constexpr int32_t kChipX      = (kScreenW - kScrollW) / 2;   // = 35
constexpr int32_t kChipY      = (kScreenH - kScrollH) / 4;   // = 26

// --- body text cell (spec §4) ----------------------------------------
// screen (100, 94, 430, 230) — scroll-local (65, 68) + chip origin (35, 26)
// = screen (100, 94). Width = SCROLLWIDTH = 0x1ae = 430. Height = 0xe6 = 230.
// spec §4 cite: DrawBackground :68 `dstX = iVar6 + 0x41; dstY = iVar7 + 0x44`
constexpr int32_t kTextX    = 100;   // screen x (35 + 65)
constexpr int32_t kTextY    = 94;    // screen y (26 + 68)
constexpr int32_t kTextW    = 430;   // SCROLLWIDTH 0x1ae (Initialize :21)
constexpr int32_t kTextH    = 230;   // body-cell height 0xe6 (Initialize :24)

// --- buttons (spec §4) -----------------------------------------------
// All three buttons use (chip_x + scroll-local_x, chip_y + scroll-local_y).
// Registered hw (0x1c, 0x1f) = (28, 31) for all three.
//   down:  scroll-local (452, 330) → screen (487, 356)
//   up:    scroll-local (484, 323) → screen (519, 349)
//   exit:  scroll-local (518, 321) → screen (553, 347)
// spec §4 cite: Initialize :61, :73, :85
constexpr int32_t kBtnW = 28;
constexpr int32_t kBtnH = 31;

constexpr int32_t kDownBtnX = kChipX + 0x1c4;  // 35 + 452 = 487
constexpr int32_t kDownBtnY = kChipY + 0x14a;  // 26 + 330 = 356
constexpr int32_t kUpBtnX   = kChipX + 0x1e4;  // 35 + 484 = 519
constexpr int32_t kUpBtnY   = kChipY + 0x143;  // 26 + 323 = 349
constexpr int32_t kExitBtnX = kChipX + 0x206;  // 35 + 518 = 553
constexpr int32_t kExitBtnY = kChipY + 0x141;  // 26 + 321 = 347

// --- icon placement for spell-scroll variant (reference image) -------
// The reference image `spell_scroll-might_description.png` shows a 40×40
// circular icon in the upper-left body area. We place it at the body text
// cell top-left, then offset the text right by (icon_w + gap).
// UNCONFIRMED: exact offset is not in the spec; derived from reference image.
constexpr int32_t kIconW     = 40;
constexpr int32_t kIconH     = 40;
constexpr int32_t kIconX     = kTextX - kIconW - 4;  // left of text cell
constexpr int32_t kIconY     = kTextY;

// --- text colors (spec §7 / §8 UNCONFIRMED-E) -----------------------
// Retail uses the FONT.DEF "Scroll" font colour (unknown). Reference image
// shows dark brown ink on parchment → sepia ~#4a2810. Reproduced here as
// the closest visual match pending FONT.DEF read (UNCONFIRMED-E §14).
constexpr float kTextR = 0x4a / 255.0f;
constexpr float kTextG = 0x28 / 255.0f;
constexpr float kTextB = 0x10 / 255.0f;

// --- font (spec §8 UNCONFIRMED-E) ------------------------------------
// Retail: "Scroll" BMFONT from font.def:57 `scrlfont`. Port approximation:
// Arimo-Regular @ 12px (Arial-metric compatible, matching HUD sizing).
// UNCONFIRMED-E: need font.def Scroll entry to confirm exact size + face.
constexpr const char* kFontPath = "thirdparty/fonts/Arimo-Regular.ttf";
constexpr int32_t     kFontPx   = 12;

// --- auto-page timing ------------------------------------------------
// Harness auto-advances a few seconds between pages so a filmstrip
// capture exercises paging without manual input.
constexpr double kAutoPageMs = 3000.0;

// =====================================================================
// Asset roster — spec §2.
// =====================================================================
constexpr const char* kScrollDat     = "scroll.dat";
constexpr const char* kSpellIconsDat = "SpellIcons.dat";

// =====================================================================
// Module state.
// =====================================================================
TMulti*  g_scrollDat     = nullptr;
TMulti*  g_spellIconsDat = nullptr;

PTBitmap g_scrollBm    = nullptr;   // 570×374 parchment chrome
PTBitmap g_downUp      = nullptr;   // 28×23 down-button released
PTBitmap g_downDown    = nullptr;   // 28×23 down-button pressed
PTBitmap g_upUp        = nullptr;   // 28×23 up-button released
PTBitmap g_upDown      = nullptr;   // 28×23 up-button pressed
PTBitmap g_exitUp      = nullptr;   // 28×31 exit-button released
PTBitmap g_exitDown    = nullptr;   // 28×31 exit-button pressed

const SFontAtlas* g_font = nullptr;
TSurface*         g_pane = nullptr;   // 640×480 full-screen RT

// =====================================================================
// Scroll content binding.
// The harness can switch between two prebuilt content descriptors so
// both the item-scroll and spell-scroll variants are exercised.
// =====================================================================
struct SScrollContent
{
    const char* title;      // appears first in body (name/type/value block)
    const char* body;       // multi-line description text
    bool        isSpell;    // if true, show a 40×40 icon at kIconX,kIconY
    const char* iconName;   // SpellIcons.dat entry name (spell-scroll only)
    PTBitmap    icon;       // resolved at init
};

// Two synthetic bindings matching the reference images:
//   item_scroll-lettuce_description.png  — "Lettuce" item scroll
//   spell_scroll-might_description.png   — "Might" spell scroll
SScrollContent g_contents[2] = {
    {
        "Name: Lettuce\nType: Food\nValue: 120",
        "Description:\n"
        "  A head of lettuce. Restores a\n"
        "  bit of health and a little mana.",
        false, nullptr, nullptr
    },
    {
        "Might",
        "Calling the avatar phantom for a\n"
        "short period of time, the might\n"
        "spell can be extremely powerful if\n"
        "activated properly. Usage of the\n"
        "might spell can be essential in\n"
        "order to utilise the spell effectively.\n"
        "\nSkill: 3     Mana: 71",
        true, "Might", nullptr
    },
};
constexpr int32_t kContentCount = 2;
int32_t g_contentIdx = 0;   // which binding is displayed

// Paging state — spec §9 / struct field layout §3.
int32_t g_line        = 0;   // current top-line (mbr_0x188)
int32_t g_numlines    = 0;   // total wrapped lines (mbr_0x18c)
int32_t g_numLinesPerPage = 0;  // cached = bodyH / lineHeight

bool   g_isDirty      = true;
double g_lastAutoPage = 0.0;

// Button press state (for the test harness; shows Down sprite briefly).
bool g_downPressed = false;
bool g_upPressed   = false;
bool g_exitPressed = false;

// =====================================================================
// Asset lookup helper — same as other ui*test panes.
// =====================================================================
static PTBitmap LookupByName(TMulti* m, const char* name)
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
// Count how many lines the body text wraps to at cellW.
// Simple word-wrap counter that matches the spec's line-count contract
// (spec §6 Initialize — FUN_004acb80 is the retail wrap kernel; here we
// use a character-advance estimate good enough for the harness).
// =====================================================================
static int32_t CountWrappedLines(const char* text, int32_t cellW)
{
    if (!g_font || !text || cellW <= 0) return 1;
    int32_t lines = 1;
    float   x     = 0.0f;
    for (const char* p = text; *p; ++p)
    {
        if (*p == '\n') { ++lines; x = 0.0f; continue; }
        // Approximate advance with average glyph width ~ fontPx * 0.55
        float adv = float(kFontPx) * 0.55f;
        x += adv;
        if (x > float(cellW)) { ++lines; x = adv; }
    }
    return lines;
}

// Recompute numlines from current content + font.
static void RecomputeNumlines()
{
    if (g_contentIdx < 0 || g_contentIdx >= kContentCount) return;
    const auto& c = g_contents[g_contentIdx];
    // Count lines in both title and body combined.
    int32_t tLines = CountWrappedLines(c.title, kTextW);
    int32_t bLines = CountWrappedLines(c.body,  kTextW);
    g_numlines = tLines + 1 + bLines;  // +1 blank line between title and body

    if (g_font)
    {
        const int32_t lineH = int32_t(TextLineHeight(g_font) + 0.5f);
        g_numLinesPerPage = lineH > 0 ? (kTextH / lineH) : 11;
    }
    else
    {
        g_numLinesPerPage = 11;
    }

    g_line  = std::clamp(g_line, 0, g_numlines);
    g_isDirty = true;
}

// =====================================================================
// HUD drawable — composes the whole panel into the 640×480 RT, then
// DrawSurface's it once in the HUD pass.
// =====================================================================
class TScrollPaneHud : public THudDrawable
{
public:
    void Draw() override
    {
        if (!g_pane) return;
        Renderer->DrawSurface(g_pane, 0, 0);
    }

    void Refresh()
    {
        if (!g_scrollBm) return;
        EnsurePane();
        if (!g_pane) return;

        AutoPage();

        if (!g_isDirty) return;
        g_isDirty = false;

        const int32_t tw = g_pane->Width();   // 640
        const int32_t th = g_pane->Height();  // 480

        // Transparent clear — the parchment chrome itself is opaque, but
        // any pixels outside the chrome's magenta-keyed boundary show
        // through to the test backdrop (spec §7 §11).
        g_pane->StartPass(0.0f, 0.0f, 0.0f, 0.0f);

        // --- (1) Parchment chrome — spec §5 step 2 / §4 ----------
        // FUN_004bd680(display, 35, 26, scroll_handle, 0x110)
        // drawmode 0x110 = DM_TRANSPARENT | DM_BACKGROUND
        // Port: DrawBitmapToTarget handles the magenta key via the bitmap
        // decoder's stored kc field; DM_BACKGROUND is moot in the RT path.
        Renderer->DrawBitmapToTarget(g_scrollBm, kChipX, kChipY, tw, th);

        // --- (2) Body text — spec §5 step 3 / §6 DrawBackground ----
        // FUN_004be2b0_CompositeBuffer(display, 100, 94, 430, 230, text, line, …)
        // Port: DrawTextShadowedToTarget with multi-line wrap simulation.
        // We draw title block then body block, skipping `g_line` leading
        // lines to implement the startLine paging offset (spec §8 / §6).
        DrawBodyText(tw, th);

        // --- (3) Spell-scroll icon (spec §1 reference image context) ---
        // The reference image shows a 40×40 circular icon in the upper-left
        // of the body area when the scroll contains a spell. This is NOT
        // explicitly called out in the spec's §4 static layout (which only
        // lists the text cell + buttons), but the reference image confirms
        // it. UNCONFIRMED source for exact position — placed left of text
        // cell with a 4px gap.
        DrawSpellIcon(tw, th);

        // --- (4) Buttons — spec §5 steps 4-5 / §4 -------------------
        // Button visibility: down hidden when line + numLinesPerPage >= numlines
        //                    up   hidden when line < 1
        //                    exit always shown.
        // Use Down sprite when harness marks a button pressed.
        DrawButtons(tw, th);

        g_pane->EndPass();
    }

private:
    // Multi-line text draw with startLine offset — approximate port of the
    // retail WriteText(text, x, y, numLines, font, nullptr, DM_USEDEFAULT,
    // cellW, line) path (spec §8). We concatenate title + blank + body,
    // skip the first `g_line` lines, draw up to `numLinesPerPage` lines
    // each at cellY + lineIndex * lineH.
    static void DrawBodyText(int32_t tw, int32_t th)
    {
        if (!g_font) return;
        if (g_contentIdx < 0 || g_contentIdx >= kContentCount) return;
        const auto& c = g_contents[g_contentIdx];

        const int32_t lineH = int32_t(TextLineHeight(g_font) + 0.5f);
        if (lineH <= 0) return;

        // Build a flat array of line strings from title + blank + body
        // (simple \n-split; the retail wrap kernel does word-wrap — this
        // harness uses pre-wrapped newlines as a close enough approximation
        // given UNCONFIRMED-E on the exact font metrics).
        static const char* lines[64];
        int32_t lineCount = 0;

        auto pushLines = [&](const char* text) {
            const char* start = text;
            for (const char* p = text; *p && lineCount < 63; ++p)
            {
                if (*p == '\n')
                {
                    static char buf[64][128];
                    const size_t n = (size_t)(p - start);
                    const size_t cap = sizeof(buf[0]) - 1;
                    const size_t cp  = n < cap ? n : cap;
                    std::memcpy(buf[lineCount], start, cp);
                    buf[lineCount][cp] = 0;
                    lines[lineCount] = buf[lineCount];
                    ++lineCount;
                    start = p + 1;
                }
            }
            if (*start && lineCount < 63)
            {
                static char buf[64][128];
                const size_t n = std::strlen(start);
                const size_t cap = sizeof(buf[0]) - 1;
                const size_t cp  = n < cap ? n : cap;
                std::memcpy(buf[lineCount], start, cp);
                buf[lineCount][cp] = 0;
                lines[lineCount] = buf[lineCount];
                ++lineCount;
            }
        };

        pushLines(c.title);
        // blank separator line
        if (lineCount < 63) { lines[lineCount++] = ""; }
        pushLines(c.body);

        // Draw from startLine = g_line, up to numLinesPerPage lines.
        const int32_t maxVisible = g_numLinesPerPage > 0 ? g_numLinesPerPage : 11;
        int32_t drawn = 0;
        for (int32_t i = g_line; i < lineCount && drawn < maxVisible; ++i, ++drawn)
        {
            const int32_t y = kTextY + drawn * lineH;
            if (y + lineH > kTextY + kTextH) break;
            DrawTextShadowedToTarget(
                g_font, lines[i],
                kTextX, y,
                kTextW, lineH,
                ETextAlign::Left,
                kTextR, kTextG, kTextB,
                tw, th);
        }
    }

    static void DrawSpellIcon(int32_t tw, int32_t th)
    {
        if (g_contentIdx < 0 || g_contentIdx >= kContentCount) return;
        const auto& c = g_contents[g_contentIdx];
        if (!c.isSpell || !c.icon) return;
        // Place icon to the left of the text cell. kIconX is pre-computed
        // as kTextX - kIconW - 4 (spec §1 reference-image context; UNCONFIRMED).
        Renderer->DrawBitmapToTarget(c.icon, kIconX, kIconY, tw, th);
    }

    // Draw button sprites at their retail-specified screen positions (spec §4).
    // Visibility gating per spec §5 step 4 (down/up button hide/show logic).
    static void DrawButtons(int32_t tw, int32_t th)
    {
        // down-button: HIDDEN when line + numLinesPerPage - 1 >= numlines
        // (spec §5 step 4: `if (line + NUMLINES - 1 >= numlines) HIDE`)
        const bool downVisible =
            (g_numlines > 0) &&
            (g_line + g_numLinesPerPage - 1 < g_numlines);
        if (downVisible)
        {
            PTBitmap bm = g_downPressed ? g_downDown : g_downUp;
            if (bm)
                Renderer->DrawBitmapToTarget(bm, kDownBtnX, kDownBtnY, tw, th);
        }

        // up-button: HIDDEN when line < 1 (spec §5 step 4)
        const bool upVisible = (g_line >= 1);
        if (upVisible)
        {
            PTBitmap bm = g_upPressed ? g_upDown : g_upUp;
            if (bm)
                Renderer->DrawBitmapToTarget(bm, kUpBtnX, kUpBtnY, tw, th);
        }

        // exit-button: always visible (spec §5 step 4)
        {
            PTBitmap bm = g_exitPressed ? g_exitDown : g_exitUp;
            if (bm)
                Renderer->DrawBitmapToTarget(bm, kExitBtnX, kExitBtnY, tw, th);
        }
    }

    void EnsurePane()
    {
        if (g_pane) return;
        // spec §3: pane = full 640×480 (ctor sets w=640, h=480).
        g_pane = new TSurface(kScreenW, kScreenH, SG_PIXELFORMAT_RGBA8);
    }

    // Auto-page driver: advance to the next content variant every
    // kAutoPageMs. Cycles item-scroll ↔ spell-scroll and resets to line=0
    // so each variant is seen from the top on first capture.
    static void AutoPage()
    {
        const double nowMs = TTime::Time() * 1000.0;
        if (g_lastAutoPage == 0.0) g_lastAutoPage = nowMs;
        if (nowMs - g_lastAutoPage >= kAutoPageMs)
        {
            g_lastAutoPage = nowMs;
            g_contentIdx   = (g_contentIdx + 1) % kContentCount;
            g_line         = 0;
            RecomputeNumlines();
            g_isDirty      = true;
        }
    }
};

TScrollPaneHud g_hud;

}  // anonymous namespace

// =====================================================================
// UIScrollPane API (harness / input-script hooks — declared in header).
// =====================================================================
namespace UIScrollPane {

void SetContent(const char* /*title*/, const char* /*body*/,
                bool /*isSpell*/, const char* /*iconName*/)
{
    // Future: used by in-game TScroll::Use binding.
    // For the test harness, content is pre-built in g_contents[].
    g_isDirty = true;
}

void Scroll(int32_t lines)
{
    const int32_t old = g_line;
    g_line = std::clamp(g_line + lines, 0, g_numlines);
    if (g_line != old) g_isDirty = true;
}

int32_t GetScrollLine() { return g_line; }

} // namespace UIScrollPane

// =====================================================================
// Entry points (testmodes.cpp links these — keep the signatures).
// =====================================================================
bool InitializeUIScrollPaneMode()
{
    log_info("[ui-scrollpane] === TScrollPane (clean-room from ScrollPane_SPEC.md) ===");

    // Spec §2: scroll.dat — parchment chrome + 6 button sprites.
    g_scrollDat = TMulti::LoadMulti((char*)kScrollDat);
    if (g_scrollDat)
    {
        g_scrollBm  = LookupByName(g_scrollDat, "scroll");
        g_downUp    = LookupByName(g_scrollDat, "downup");
        g_downDown  = LookupByName(g_scrollDat, "downdown");
        g_upUp      = LookupByName(g_scrollDat, "upup");
        g_upDown    = LookupByName(g_scrollDat, "updown");
        g_exitUp    = LookupByName(g_scrollDat, "exitup");
        g_exitDown  = LookupByName(g_scrollDat, "exitdown");
    }
    else
    {
        log_error("[ui-scrollpane] LoadMulti('%s') FAILED — chrome will be blank",
                  kScrollDat);
    }

    log_info("[ui-scrollpane] scroll=%s downUp=%s downDn=%s upUp=%s upDn=%s "
             "exitUp=%s exitDn=%s",
             g_scrollBm ? "OK" : "MISS",
             g_downUp   ? "OK" : "MISS",
             g_downDown ? "OK" : "MISS",
             g_upUp     ? "OK" : "MISS",
             g_upDown   ? "OK" : "MISS",
             g_exitUp   ? "OK" : "MISS",
             g_exitDown ? "OK" : "MISS");
    if (g_scrollBm)
        log_info("[ui-scrollpane] scroll chrome %dx%d (expect 570x374)",
                 g_scrollBm->width, g_scrollBm->height);

    // Spec §2 / §14 UNCONFIRMED-C: SpellIcons.dat for the spell-scroll
    // variant's 40×40 icon (reference image shows one). Same archive as
    // uiquickspelltest / uispellbooktest.
    g_spellIconsDat = TMulti::LoadMulti((char*)kSpellIconsDat);
    if (g_spellIconsDat)
    {
        for (int32_t i = 0; i < kContentCount; ++i)
        {
            if (g_contents[i].isSpell && g_contents[i].iconName)
                g_contents[i].icon = LookupByName(
                    g_spellIconsDat, g_contents[i].iconName);
        }
    }
    for (int32_t i = 0; i < kContentCount; ++i)
    {
        if (g_contents[i].isSpell)
            log_info("[ui-scrollpane] spell icon '%s' = %s",
                     g_contents[i].iconName ? g_contents[i].iconName : "(null)",
                     g_contents[i].icon ? "OK" : "MISS");
    }

    // Font — spec §8 UNCONFIRMED-E (retail "Scroll" BMFONT → Arimo approx).
    g_font = BuildTTFAtlas(kFontPath, kFontPx);
    log_info("[ui-scrollpane] font %s @%dpx = %s",
             kFontPath, kFontPx, g_font ? "OK" : "MISS");

    delete g_pane;
    g_pane         = nullptr;
    g_contentIdx   = 0;
    g_line         = 0;
    g_isDirty      = true;
    g_lastAutoPage = 0.0;
    g_downPressed  = false;
    g_upPressed    = false;
    g_exitPressed  = false;

    RecomputeNumlines();

    Renderer->AddHud(&g_hud, 0.0f);
    return true;
}

void RenderUIScrollPaneMode()
{
    g_hud.Refresh();

    // Dark slate backdrop so the parchment chrome reads clearly and the
    // magenta-keyed edge pixels show the backdrop (spec §11 no pink-halo).
    Display.BackBuffer()->StartPass(0.18f, 0.20f, 0.26f, 1.0f);
    Display.BackBuffer()->EndPass();
}

void CloseUIScrollPaneMode()
{
    Renderer->RemoveHud(&g_hud);
    delete g_pane;
    g_pane         = nullptr;
    g_scrollBm     = nullptr;
    g_downUp       = nullptr;
    g_downDown     = nullptr;
    g_upUp         = nullptr;
    g_upDown       = nullptr;
    g_exitUp       = nullptr;
    g_exitDown     = nullptr;
    g_font         = nullptr;
    for (int32_t i = 0; i < kContentCount; ++i)
        g_contents[i].icon = nullptr;
    g_scrollDat     = nullptr;
    g_spellIconsDat = nullptr;
    g_line          = 0;
    g_numlines      = 0;
    g_numLinesPerPage = 0;
    g_isDirty       = true;
    g_lastAutoPage  = 0.0;
}
