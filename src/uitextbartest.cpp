// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *    uitextbartest.cpp - --test=ui-textbar: TTextBar data API + draw    *
// *************************************************************************
//
// Two responsibilities:
//   (1) Drive the TTextBar data API (Print, SetHealthDisplay, Clear, dirty)
//       and log the observable state, so the data contract is verified
//       (unchanged from the original placeholder).
//   (2) Render the retail-faithful textbar overlay: a stack of up to 9
//       visible message lines anchored at the bottom of the play viewport,
//       each in gold ("Small" font color, FUN_0054cd40:91-94 / SPEC §7),
//       with a 3-pass black shadow (FUN_004be2b0 / SPEC §13) and a per-
//       line TTL fade over the last 24 ticks (alpha = ttl*255/24, SPEC §8
//       slot-7 fade formula `FUN_0054c600:31-60`).
//
// The pre-release TTextBar holds a single `char text[80]`; the retail
// multi-line record table + TTL field never made it into the snapshot's
// data model. We keep a local ring of 9 (SPEC §3 `maxLines = DAT_005e5800`)
// lines in the visualizer so the BEHAVIOR -- stacked feed + per-line fade
// -- is verifiable now, ahead of evolving TTextBar's record table to
// retail shape.
//
// *************************************************************************

#include "uitextbartest.h"

#include "display.h"
#include "font.h"
#include "logging.h"
#include "renderer.h"
#include "revdefs.h"
#include "surface.h"
#include "textbar.h"
#include "time.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <memory>
#include <string>

namespace {

// ---- font + color (TTextBar_SPEC §2 / §7) -------------------------------
// "Small" = Arial 12px (FONT.DEF `WINFONT "Small" FONT "Arial" 12`); the
// modern port uses Arimo-Regular as the Arial-metric substitute per
// `project_ui_text_rendering.md`. The default feed-line color is the
// retail override gold (255, 200, 0) -- per FUN_0054cd40:91-94 it is
// applied at the DrawTextA call site, NOT inherited from FONT.DEF's
// default white. See SPEC §7 color table; per-record color overrides
// (e.g. color 8 = hostile yellow) would land via a future record table.
constexpr const char* kFontPath  = "thirdparty/fonts/Arimo-Regular.ttf";
constexpr int32_t     kFontPx    = 12;
constexpr float       kGoldR     = 1.0f;
constexpr float       kGoldG     = 200.0f / 255.0f;
constexpr float       kGoldB     = 0.0f;

// ---- multi-line ring (SPEC §3) ------------------------------------------
// maxLines = DAT_005e5800 = 9 visible (FUN_0054bf70:51-57). Each record
// carries text + TTL in ticks (24Hz). Fade window is 24 ticks (~1s at
// 24Hz, per FUN_0054c600:31-60): `alpha = (ttl<24) ? ttl*255/24 : 255`.
// Initial TTL is held for several seconds before fade begins; retail's
// `0x78 = 120` ticks ~ 5s feels right for play feedback.
constexpr int32_t kMaxLines     = 9;     // DAT_005e5800
constexpr int32_t kFadeTicks    = 24;    // SPEC §8 fade window
constexpr int32_t kInitialTtl   = 480;   // ~20s opaque before fade (demo)
constexpr double  kTickMs       = 1000.0 / 24.0;

struct SLine {
    std::string text;
    int32_t     ttl = 0;   // ticks remaining; <=0 == drop
};

std::array<SLine, kMaxLines> g_lines;
int32_t g_lineCount    = 0;
double  g_lastTickMs   = 0.0;

const SFontAtlas* g_font = nullptr;
std::unique_ptr<TTextBar> g_textbar;

// Composed render target: holds the 9-line stack as paint into a
// transparent TSurface that the HUD pass then DrawSurface's once. This is
// the direct-renderer contract `DrawTextShadowedToTarget` requires --
// it can only paint into an *active TSurface render pass*, never into
// the swapchain pass that runs Draw() itself.
TSurface* g_pane     = nullptr;
int32_t   g_paneOffY = 0;     // pane top = textbar bottom - paneH

// Append a message to the ring; if full, the oldest (front) drops to
// make room. Newest sits at the BOTTOM of the visible stack.
void PushLine(const char* text)
{
    if (!text || !*text) return;
    if (g_lineCount == kMaxLines)
    {
        for (int32_t i = 0; i + 1 < kMaxLines; ++i)
            g_lines[i] = std::move(g_lines[i + 1]);
        --g_lineCount;
    }
    g_lines[g_lineCount].text = text;
    g_lines[g_lineCount].ttl  = kInitialTtl;
    ++g_lineCount;
}

// Decrement every line's TTL by the elapsed-tick count, drop expired.
// Called once per frame at the top of Draw so we don't double-tick when
// both standalone (--test=ui-textbar) and composed (--test=ui-hud)
// drivers hit us.
void TickFade()
{
    const double now = TTime::Time() * 1000.0;
    if (g_lastTickMs <= 0.0) g_lastTickMs = now;
    int32_t guard = 0;
    while (now - g_lastTickMs >= kTickMs && guard < 240)
    {
        g_lastTickMs += kTickMs;
        ++guard;
        for (int32_t i = 0; i < g_lineCount; ++i)
            if (g_lines[i].ttl > 0) --g_lines[i].ttl;
    }
    // Compact: drop any leading expired records (oldest fades first).
    int32_t drop = 0;
    while (drop < g_lineCount && g_lines[drop].ttl <= 0) ++drop;
    if (drop > 0)
    {
        for (int32_t i = 0; i + drop < g_lineCount; ++i)
            g_lines[i] = std::move(g_lines[i + drop]);
        for (int32_t i = g_lineCount - drop; i < g_lineCount; ++i)
            g_lines[i] = SLine{};
        g_lineCount -= drop;
    }
}

// SPEC §8 fade: alpha = ttl*255/24 in the last 24 ticks; full alpha
// before that. Returned as a 0..1 multiplier on the gold base color.
float FadeAlpha(int32_t ttl)
{
    if (ttl <= 0)          return 0.0f;
    if (ttl >= kFadeTicks) return 1.0f;
    return float(ttl) / float(kFadeTicks);
}

// Ensure a transparent RT sized to the textbar's width and (maxLines * lineH)
// tall so the full 9-line stack composes in one pass. Built lazily on first
// Refresh once the font atlas is up. The pane is bottom-anchored at the
// textbar rect's bottom edge.
void EnsurePane()
{
    if (g_pane || !g_font) return;
    const int32_t paneW = g_textbar ? g_textbar->GetWidth() : TEXTBARWIDTH;
    const int32_t lineH = (std::max)(1, int32_t(TextLineHeight(g_font) + 0.5f));
    const int32_t paneH = (std::max)(lineH, kMaxLines * lineH);
    g_pane = new TSurface(paneW, paneH, SG_PIXELFORMAT_RGBA8);
    g_paneOffY = paneH;   // text-bar bottom edge sits at pane bottom edge
}

// HUD drawable -- DrawSurface's the composed pane RT in one call. The
// per-line text/shadow is painted into the RT by Refresh (called from
// RenderUITextBarModeEmbedded, before the HUD pass runs). The pane is
// bottom-anchored to (TEXTBARY + TEXTBARHEIGHT) so lines stack UPWARD
// matching the retail SPEC §3 (`pane.top = pane.bottom - maxLines * lineH`).
class TTextBarHud : public THudDrawable
{
public:
    void Draw() override
    {
        if (!g_pane || !g_textbar) return;
        const int32_t x = g_textbar->GetPosX();
        const int32_t y = (g_textbar->GetPosY() + g_textbar->GetHeight())
                          - g_paneOffY;
        Renderer->DrawSurface(g_pane, x, y);
    }

    // Repaint the RT every frame: tick the per-line TTLs, then re-emit
    // the visible lines bottom-up with the SPEC §8 alpha fade applied
    // per-line. Called from RenderUITextBarModeEmbedded so both
    // --test=ui-textbar and --test=ui-hud composite refresh on the same
    // schedule.
    void Refresh()
    {
        if (!g_textbar || !g_font) return;
        EnsurePane();
        if (!g_pane) return;

        TickFade();

        const int32_t paneW = g_pane->Width();
        const int32_t paneH = g_pane->Height();
        const int32_t lineH = (std::max)(1, int32_t(TextLineHeight(g_font) + 0.5f));

        // Transparent clear -- HUD overlay; the playfield bleeds through
        // everywhere we DON'T paint a glyph (SPEC §3 direct-renderer
        // contract).
        g_pane->StartPass(0.0f, 0.0f, 0.0f, 0.0f);

        // Newest message sits at the bottom row; older above. Skip if
        // the ring is empty (nothing to paint -- the RT just stays
        // transparent and DrawSurface costs only a quad).
        for (int32_t i = g_lineCount - 1; i >= 0; --i)
        {
            const int32_t rowFromBottom = (g_lineCount - 1) - i;
            const int32_t cellY = paneH - (rowFromBottom + 1) * lineH;
            const float   a     = FadeAlpha(g_lines[i].ttl);
            // DrawTextShadowedToTarget paints a black 3-pass shadow first
            // then the colored top (SPEC §13 / FUN_004be2b0). The colored
            // layer is multiplied by (r,g,b), so pre-multiply gold by
            // the fade scalar to get the alpha fall-off in lockstep.
            DrawTextShadowedToTarget(g_font, g_lines[i].text.c_str(),
                                     /*cellX*/ 4, cellY, paneW - 8, lineH,
                                     ETextAlign::Left,
                                     kGoldR * a, kGoldG * a, kGoldB * a,
                                     paneW, paneH);
        }

        g_pane->EndPass();
    }
};
TTextBarHud g_viz;

// Reach in via a friend-free shim: the protected members of TTextBar are
// not exposed publicly. For verification we infer state from the public
// observable side -- IsDirty, rect, and "has health display" via the
// behavior of ClearHealthDisplay (which clears dirty only if name was
// non-empty). Logs the observable surface; the protected text / name
// fields stay opaque.
void LogObservable(const char* label)
{
    if (!g_textbar) { log_info("[ui-textbar] %s (null pane)", label); return; }
    log_info("[ui-textbar] %-30s rect=(x=%d y=%d w=%d h=%d) dirty=%s open=%s",
             label,
             g_textbar->GetPosX(), g_textbar->GetPosY(),
             g_textbar->GetWidth(), g_textbar->GetHeight(),
             g_textbar->IsDirty() ? "YES" : "no",
             g_textbar->IsOpen()  ? "YES" : "no");
}

// Seed a representative feed so the standalone mode visibly exercises the
// 9-line stack + per-line fade. Skipped when running embedded in ui-hud
// (the live game will Print real messages).
void SeedSampleFeed()
{
    static const char* kSampleLines[] = {
        "You feel the power of Soul rising",
        "Got the Hammer of Wounding",
        "The Skeleton hits you for 8 damage",
        "You slay the Skeleton",
        "The door is now open",
        "You hear a low chant from the east",
        "Picked up: Lesser Healing Potion",
    };
    for (const char* s : kSampleLines)
        PushLine(s);
    // Stagger TTLs so the visible stack shows a gradient -- older lines
    // start closer to the fade window so the verify filmstrip captures
    // mid-fade naturally.
    for (int32_t i = 0; i < g_lineCount; ++i)
    {
        const int32_t age = (g_lineCount - 1 - i) * 6;  // older = less ttl
        g_lines[i].ttl = (std::max)(2, kInitialTtl - age);
    }
}

}  // namespace

bool InitializeUITextBarMode()
{
    log_info("[ui-textbar] === TTextBar data-API contract + render ===");

    g_font = BuildTTFAtlas(kFontPath, kFontPx);
    log_info("[ui-textbar] font %s @%dpx = %s",
             kFontPath, kFontPx, g_font ? "OK" : "MISS");

    g_textbar = std::make_unique<TTextBar>();
    LogObservable("after construct");

    g_textbar->Initialize();
    LogObservable("after Initialize()");

    char msg[] = "hello bar";
    g_textbar->Print(msg);
    PushLine("hello bar");
    LogObservable("after Print(\"hello bar\")");

    char fmt[] = "level %d of %s";
    char arg2[] = "the keep";
    g_textbar->Print(fmt, 3, arg2);
    PushLine("level 3 of the keep");
    LogObservable("after Print(\"level %d of %s\", 3, \"the keep\")");

    SeedSampleFeed();

    // Health-display data-API exercise (unchanged from the placeholder).
    g_textbar->SetHealthDisplay("Skeleton", 75);
    LogObservable("after SetHealthDisplay(\"Skeleton\", 75)");
    g_textbar->SetDirty(false);
    g_textbar->SetHealthDisplay("Skeleton", 50);
    LogObservable("after SetHealthDisplay(\"Skeleton\", 50) (same name)");
    g_textbar->SetDirty(false);
    g_textbar->SetHealthDisplay("Orc", 90);
    LogObservable("after SetHealthDisplay(\"Orc\", 90) (new name)");
    g_textbar->ClearHealthDisplay();
    LogObservable("after ClearHealthDisplay()");
    g_textbar->SetDirty(false);
    g_textbar->ClearHealthDisplay();
    LogObservable("after ClearHealthDisplay() (already clear)");

    g_textbar->Clear();
    LogObservable("after Clear()");

    log_info("[ui-textbar] visualizer: %d sample lines seeded, gold "
             "(255,200,0) + 3-pass black shadow, fades over last %d ticks "
             "(SPEC §8 / FUN_0054c600)", g_lineCount, kFadeTicks);

    Renderer->AddHud(&g_viz, 0.0f);
    log_info("[ui-textbar] visualizer registered with renderer HUD pipeline");

    return true;
}

void RenderUITextBarMode()
{
    RenderUITextBarModeEmbedded();

    const double t = TTime::Time();
    const float r = 0.10f + 0.04f * float(std::sin(t * 0.6));
    const float g = 0.12f + 0.04f * float(std::sin(t * 0.8 + 1.0));
    const float b = 0.16f + 0.04f * float(std::sin(t * 1.0 + 2.0));
    Display.BackBuffer()->StartPass(r, g, b, 1.0f);
    Display.BackBuffer()->EndPass();
}

void RenderUITextBarModeEmbedded()
{
    // Per-frame RT compose. Must run before the HUD pass (which calls
    // TTextBarHud::Draw → DrawSurface) so the texture has fresh fade
    // state. Same shape as RenderUIPlyrStatusBarModeEmbedded.
    g_viz.Refresh();
}

void CloseUITextBarMode()
{
    Renderer->RemoveHud(&g_viz);
    if (g_textbar)
        g_textbar->Close();
    g_textbar.reset();
    g_font = nullptr;
    g_lineCount = 0;
    g_lastTickMs = 0.0;
    g_paneOffY = 0;
    if (g_pane) { delete g_pane; g_pane = nullptr; }
    for (auto& l : g_lines) l = SLine{};
}
