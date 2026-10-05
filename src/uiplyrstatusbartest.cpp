// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uiplyrstatusbartest.cpp - --test=ui-plyrstatusbar                     *
// *************************************************************************
//
// Clean-room reconstruction of TPlyrStatusBar (the player + current-target
// stat-bar HUD chip). Built ONLY from docs/ui/forensics/TPlyrStatusBar_SPEC.md
// (with NOMENCLATURE / UI_METHOD_MAP §12 / RECONSTRUCTION_PROTOCOL). Every
// coordinate, asset, color, alignment, shadow, bar-fill and animation cites its
// spec section inline.
//
// Architecture (spec §3 direct-renderer contract): the WHOLE pane (player chip
// + target chip: chrome + ring + portrait + icons + bars + text) is composed
// into ONE offscreen TSurface render target via the `…ToTarget` primitive
// family, then DrawSurface'd once in the HUD pass — exactly like
// uisidetabstest.cpp. No swapchain/glyph path is mixed in (no scale drift).
//
// Primitives: only the canonical shared toolbox (UI_METHOD_MAP §12) —
//   Renderer->DrawBitmap{,Subrect}{,Tinted,Shadowed}ToTarget
//   font.h DrawTextShadowedToTarget (3-pass black shadow + colored top)
// No hand-rolled shadow passes or glyph walks.
//
// *** Frame composition (spec §3 Frame table — the load-bearing fix) ***
//   screen_x = chip_origin_in_screen + chrome_origin_in_chip + x_in_chrome
//   Player:  chip_origin = 0,           chrome_origin = 0     → screen_x = x_in_chrome
//   Target:  chip_origin = pane_w-0xc1, chrome_origin = 0x41  → screen_x = pane_w-0x80 + x_in_chrome
// So target BackPanel → screen pane_w-0x80; Ring center → pane_w-0x1e;
// icons → pane_w-0x46. The prior attempt dropped the 0x41 chrome origin and
// placed the right card 0x41 too far left.
//
// Data (slot23 FUN_0054af20): the left card is the main player (`Player`,
// retail DAT_00667fcc), the right card the character the player fights
// (TCharacter::Fighting). The pane keeps no game data of its own; the
// --test=ui-plyrstatusbar host supplies a demo player (uidemoplayer.h).
//
// *************************************************************************

#include "uiplyrstatusbartest.h"

#include "bitmap.h"
#include "bitmapatlas.h"
#include "dialog.h"
#include "display.h"
#include "font.h"
#include "hudstate.h"
#include "logging.h"
#include "multi.h"
#include "player.h"
#include "renderer.h"
#include "surface.h"
#include "time.h"

#include <cstdint>
#include <cstdio>
#include <cstring>

namespace {

// =====================================================================
// Geometry constants — all from TPlyrStatusBar_SPEC.md, cited inline.
// =====================================================================

// --- pane (spec §3 "Pane on screen") ---------------------------------
constexpr int32_t kPaneH = 0x70;   // 112 — pane height (§1, §3)
constexpr int32_t kChipW = 0xc1;   // 193 — chip width, wider than chrome (§3 chip model)

// Retail insets each chip ~6px from the screen edge it hugs (not flush): the
// player chip starts +6 from the left, the target chip ends -6 from the right.
// The whole chip (chrome/ring/icons/bars/text) shifts by this single amount.
constexpr int32_t kChipInset = 6;

// --- chrome-surface origin in chip-local (spec §3 Frame table) -------
// Player chrome composites at chip-local 0; target chrome at chip-local 0x41.
constexpr int32_t kChromeOriginPlayer = 0x00;        // §3 frame table
constexpr int32_t kChromeOriginTarget = 0x41;        // §3 frame table (0xc1 - 0x80)

// --- ring (chrome-surface-local, spec §4 / §6a) ----------------------
// Ring is centered on an anchor; helper offsets by half w/h. Ring 44x44.
constexpr int32_t kRingW = 44;
constexpr int32_t kRingH = 44;
constexpr int32_t kRingAnchorYCenter       = 0x1f;   // 31 (§4 / §6a)
constexpr int32_t kRingAnchorXCenterPlayer = 0x1a;   // 26 chrome-local (§4 / §6a)
constexpr int32_t kRingAnchorXCenterTarget = 0x62;   // 98 chrome-local (§4 / §6a)

// --- icons 24x24, chrome-surface-local x (spec §4 / §6a) -------------
constexpr int32_t kIconXPlayer  = 0x2b;   // 43 chrome-local (§4)
constexpr int32_t kIconXTarget  = 0x3a;   // 58 chrome-local (§4)
constexpr int32_t kIconYHealth  = 0x03;   // 3  (§4)
constexpr int32_t kIconYMana    = 0x11;   // 17 (§4)
constexpr int32_t kIconYFatigue = 0x20;   // 32 (§4)

// =====================================================================
// Per-bar geometry — spec §6 "Per-bar call-site literals".
// dstX (player) and targetOff (target = pane_w - off) are RAW retail screen
// destinations. brightY/dimY are the source band rows in the Bars atlas (§2).
// fullW = the 100% fill width; tailX = total drawn extent (cap+interior+tail);
// the pointed tip past fullW is the dim tail (§6b table: tailW = tailX-fullW+capW).
// =====================================================================
struct SBarGeom
{
    int32_t dstXPlayer;   // §6: player dstX (screen, = chip-local)
    int32_t dstY;         // §6: dstY (same both sides)
    int32_t targetOff;    // §6: target dstX = pane_w - targetOff
    int32_t srcX;         // §6: 2 for every bar
    int32_t brightY;      // §6: bright band srcY
    int32_t dimY;         // §6: dim band srcY
    int32_t h;            // §6: source/dest height
    int32_t fullW;        // §6: 100% fill width
    int32_t tailX;        // §6: total drawn extent (incl. pointed tail)
};

// Order: HP, MP, FT (spec §4 / §6 row order).
constexpr SBarGeom kBars[3] = {
    // HP: dstX 0x44, dstY 0x0f, target off 0xc1, srcX 2, brightY 1,   dimY 0x31, h 0x11, fullW 0x77, tailX 0x7d (§6)
    { 0x44, 0x0f, 0xc1, 2, 0x01, 0x31, 0x11, 0x77, 0x7d },
    // MP: dstX 0x44, dstY 0x1f, target off 0x91, srcX 2, brightY 0x13, dimY 0x42, h 0x0c, fullW 0x45, tailX 0x4d (§6)
    { 0x44, 0x1f, 0x91, 2, 0x13, 0x42, 0x0c, 0x45, 0x4d },
    // FT: dstX 0x44, dstY 0x2c, target off 0x79, srcX 2, brightY 0x22, dimY 0x51, h 0x0c, fullW 0x2d, tailX 0x35 (§6)
    { 0x44, 0x2c, 0x79, 2, 0x22, 0x51, 0x0c, 0x2d, 0x35 },
};
enum { BAR_HP = 0, BAR_MP = 1, BAR_FT = 2 };

// Chrome→chip normalization for the bars (spec §11 #2 / §6 trap). The retail
// bar dst is raw screen ONLY because the chrome/effect buffer was first drawn
// via the FUN_00438d80(…,4,4) two-stage path. We compose the chrome at
// chip-local (0,0), so undo the +4,+4 (player) / mirror (target).
constexpr int32_t kBarBiasX = 4;   // §11 #2 / §6
constexpr int32_t kBarBiasY = 4;   // §11 #2 / §6

// The target (right) chip's bars sit 4px right of the pure geometric mirror of
// the player bars — measured against retail. The player/target chrome art is
// not a perfect mirror, so the shadow-bias mirror over-corrects by this much.
constexpr int32_t kTargetBarXFix = 4;

// Bar/text drop-shadow (spec §7). Player casts down-RIGHT (+4,+4); target
// mirrors to down-LEFT (-4,+4). The primitive offsets the shadow from the
// content dst (NOMENCLATURE §4 / project-retail-shadow-semantics).
constexpr int32_t kShadowDX    = 4;       // §7
constexpr int32_t kShadowDY    = 4;       // §7
constexpr float   kShadowAlpha = 0.55f;

// --- text cells (spec §4 / §8) ---------------------------------------
// Value cells 0x32x0x0e (50x14); name/level 0x40x0x40 (64x64). Player x is
// pane-local; target x is pane_w - offset.
constexpr int32_t kValueCellW     = 0x32;   // 50 (§8)
constexpr int32_t kValueCellH     = 0x0e;   // 14 (§8)
constexpr int32_t kValueXPlayer   = 0x47;   // 71 (§8, player LEFT-aligned)
constexpr int32_t kValueOffTarget = 0x80;   // target cell x = pane_w-0x80 (§8, RIGHT-aligned)
constexpr int32_t kValueYHealth   = 0x07;   // 7  (§8)
constexpr int32_t kValueYMana     = 0x17;   // 23 (§8)
constexpr int32_t kValueYFatigue  = 0x24;   // 36 (§8)

constexpr int32_t kNameCellW     = 0x40;    // 64 (§8)
constexpr int32_t kNameCellH     = 0x40;    // 64 (§8)
// Name/level is centered under the portrait ring at runtime (see DrawSide,
// ringScreenCX) rather than at the §8 cell literals (0 / pane_w-0x44), which
// sit ~6px off the ring center.
constexpr int32_t kNameCellY     = 0x36;    // 54 (§8)

// Font: Arimo-Regular ("Small") — the Arial-metric-compatible font, spec §8 px ~12.
constexpr const char* kFontPath = "thirdparty/fonts/Arimo-Regular.ttf";
constexpr int32_t     kFontPx   = 12;       // §8 ("~12")

// --- animation (spec §9) ---------------------------------------------
constexpr int32_t kFadeMax   = 6;             // §9 ramp range 0..6
constexpr double  kSimTickMs = 1000.0 / 24.0; // 24Hz sim gate (protocol rule 6, §9)

// Name/level format (FUN_0054ae10): players get the STATBARFMT dialog line,
// default "%s\nLevel %d"; any other character shows its name alone.
constexpr const char* kNameLevelTag     = "STATBARFMT";
constexpr const char* kNameLevelDefault = "%s\nLevel %d";

// =====================================================================
// Loaded assets.
// =====================================================================
TMulti*  g_statusbarDat = nullptr;
TMulti*  g_portraitsDat = nullptr;
PTBitmap g_backPanel   = nullptr;
PTBitmap g_bars        = nullptr;
PTBitmap g_healthIcon  = nullptr;
PTBitmap g_manaIcon    = nullptr;
PTBitmap g_fatigueIcon = nullptr;
PTBitmap g_ring        = nullptr;
PTBitmap g_lockeFace   = nullptr;   // 30x30 shipped player portrait (portraits.dat)

const SFontAtlas* g_font = nullptr;

TSurface* g_pane  = nullptr;
int32_t   g_paneW = 0;             // live pane width (this+0xc, §3) = display width

// Fade ramps (spec §9). Player targets kFadeMax while a player exists;
// target ramps toward kFadeMax while the player fights someone, else 0.
int32_t g_playerFade = 0;          // this[0xd4] (§9)
int32_t g_targetFade = 0;          // this[0xdc] (§9)
double  g_lastTickMs = 0.0;

// The last character the player fought (this+0xb8, a map index in retail):
// the target card keeps drawing it while it fades out (slot23 :623-628).
TSafeRef<TCharacter> g_lastTarget;

// The player's current target (FUN_0054af20 :64-71): the root action's
// object when the root action is combat (3) or bow (0x19).
TCharacter* CurrentTarget()
{
    return Player ? Player->Fighting() : nullptr;
}

// Portrait: the character's baked .i3d head icon (the +0x130 inventory
// image PlayerSide stamps), probed across states because the icon lives on
// one state only. Characters that bake none fall back to the shipped
// LockeFace, as the harness always has (retail draws no face then).
PTBitmap PortraitOf(TObjectInstance* ch)
{
    TObjectImagery* img = ch ? ch->GetImagery() : nullptr;
    if (img)
        for (int32_t s = 0; s < img->NumStates(); ++s)
            if (PTBitmap bm = img->GetInvImage(s))
                return bm;
    return g_lockeFace;
}

// FUN_0054ae10: "<name>\nLevel <n>" for players, the bare name otherwise.
// Returns the two lines through line1/line2.
void FormatNameLevel(TCharacter* ch, char (&line1)[64], char (&line2)[64])
{
    line1[0] = line2[0] = '\0';
    const char* name = ch->GetName() ? ch->GetName() : "";
    if (ch->ObjClass() != OBJCLASS_PLAYER)
    {
        std::snprintf(line1, sizeof(line1), "%s", name);
        return;
    }
    const char* fmt = DialogList.GetLine(const_cast<char*>(kNameLevelTag));
    char text[128];
    std::snprintf(text, sizeof(text), fmt ? fmt : kNameLevelDefault,
                  name, static_cast<int>(static_cast<TPlayer*>(ch)->Level()));
    const char* nl = std::strchr(text, '\n');
    const size_t n1 = nl ? size_t(nl - text) : std::strlen(text);
    std::snprintf(line1, sizeof(line1), "%.*s", int(n1), text);
    if (nl)
        std::snprintf(line2, sizeof(line2), "%s", nl + 1);
}

// fade → bar/chrome alpha (spec §9: alpha = ctr*255/6).
float FadeAlpha(int32_t fade)
{
    if (fade <= 0)        return 0.0f;
    if (fade >= kFadeMax) return 1.0f;
    return float(fade) / float(kFadeMax);
}

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
// Bar fill — spec §6b (FUN_0054a5d0, the 4-section kernel).
//
// The drawn bar spans `tailX` px (dstX .. dstX+tailX); the pointed tip past
// `fullW` is the dim tail. fillW = value*fullW/max splits bright (filled) from
// dim (empty). dir = +1 (player) fills left→right; dir = -1 (target) drains
// from the inner edge (bright on the RIGHT, toward the chrome).
//
// Bright reads from the Bars band at (srcX, brightY); dim from (srcX, dimY).
// Source x tracks the destination offset within the bar so the band gradient
// stays continuous. All slices blit through the shared tinted / shadowed
// to-target primitives (no hand-rolled passes).
// =====================================================================
void DrawBar(const SBarGeom& g, int32_t dstX, int32_t dstY,
             int32_t value, int32_t maxv, int32_t dir,
             float fadeAlpha, int32_t shadowDX, int32_t tw, int32_t th)
{
    if (!g_bars) return;

    if (maxv < 1)        maxv  = 1;            // clamp (§6b :40-48)
    if (value < 0)       value = 0;
    if (value > maxv)    value = maxv;

    const int32_t totalW = g.tailX;            // full drawn extent incl. tail
    const int32_t h      = g.h;
    const int32_t srcX   = g.srcX;

    int32_t fillW = (int32_t)((int64_t)value * g.fullW / maxv);  // §6b :55
    if (fillW < 0)        fillW = 0;
    if (fillW > totalW)   fillW = totalW;
    const int32_t emptyW = totalW - fillW;

    // One slice: read [srcX+srcOff .. +sliceW) from band `srcY`, draw at dstX+dx.
    // The bright (filled) slice casts the §7 drop shadow when fully faded in;
    // the dim (empty track) slice draws flat (no shadow halo).
    auto blit = [&](int32_t dx, int32_t srcOff, int32_t sliceW,
                    int32_t srcY, bool bright)
    {
        if (sliceW <= 0) return;
        if (bright && fadeAlpha >= 0.999f)
        {
            Renderer->DrawBitmapSubrectShadowedToTarget(
                g_bars, dstX + dx, dstY, srcX + srcOff, srcY, sliceW, h,
                tw, th, shadowDX, kShadowDY, kShadowAlpha);
        }
        else
        {
            Renderer->DrawBitmapSubrectTintedToTarget(
                g_bars, dstX + dx, dstY, srcX + srcOff, srcY, sliceW, h,
                tw, th, 1.0f, 1.0f, 1.0f, fadeAlpha);
        }
    };

    if (dir >= 0)
    {
        // player: bright = leftmost fillW, dim tail = remainder
        blit(0,     0,     fillW,  g.brightY, true);
        blit(fillW, fillW, emptyW, g.dimY,    false);
    }
    else
    {
        // target: dim (empty) = leftmost emptyW, bright (filled) = right fillW
        blit(0,      0,      emptyW, g.dimY,    false);
        blit(emptyW, emptyW, fillW,  g.brightY, true);
    }
}

// =====================================================================
// Draw one chip (player or target) into the pane RT.
// Mirrors slot 20 (chrome compose, §6a) + slot 23 (bars + text, §5).
// =====================================================================
void DrawSide(TCharacter* ch, bool isRight, int32_t fade,
              int32_t tw, int32_t th)
{
    const float fadeAlpha = FadeAlpha(fade);
    if (fadeAlpha <= 0.0f) return;

    // ---------- chrome (spec §6a / §3 Frame table, slot 20 order) ----------
    // screen_x = chip_origin + chrome_origin + x_in_chrome (§3).
    //   player: chrome_origin 0,    chip_origin 0           → x_in_chrome
    //   target: chrome_origin 0x41, chip_origin pane_w-0xc1 → pane_w-0x80 + x_in_chrome
    // Single per-side inset shift: player chip moves right, target chip moves
    // left, so each clears the screen edge by kChipInset (§ chip inset note).
    const int32_t sideShift = isRight ? -kChipInset : kChipInset;
    const int32_t chromeBase = (isRight ? (g_paneW - kChipW + kChromeOriginTarget)
                                        : (0 + kChromeOriginPlayer)) + sideShift;
    const int32_t ringCX = isRight ? kRingAnchorXCenterTarget : kRingAnchorXCenterPlayer;
    const int32_t iconX  = chromeBase + (isRight ? kIconXTarget : kIconXPlayer);
    // Screen-space center of the portrait ring — the name/level centers under
    // THIS, not under a generic name cell (the cell center sits ~6px off the
    // ring; forensics captured the cell literal, not the centering intent).
    const int32_t ringScreenCX = chromeBase + ringCX;

    // BackPanel chrome at chrome-local (0,0), opaque (§4 / §6a).
    if (g_backPanel)
        Renderer->DrawBitmapToTarget(g_backPanel, chromeBase, 0, tw, th);

    // Portrait FIRST (under the ring), centered on the ring anchor.
    if (PTBitmap face = PortraitOf(ch))
    {
        const int32_t fx = chromeBase + ringCX - face->width / 2;
        const int32_t fy = kRingAnchorYCenter - face->height / 2;
        Renderer->DrawBitmapToTarget(face, fx, fy, tw, th);
    }

    // Ring OVER the portrait (§4 / §6a): a semi-transparent glass disc + brass
    // border — the glass tints the face and the brass frames its edge, so it
    // must composite on top of the portrait, not under it.
    const int32_t ringTLy = kRingAnchorYCenter - kRingH / 2;     // 9
    if (g_ring)
    {
        const int32_t ringTLx = chromeBase + ringCX - kRingW / 2;
        Renderer->DrawBitmapToTarget(g_ring, ringTLx, ringTLy, tw, th);
    }

    // Three stat icons (§4 / §6a) — alpha sprites.
    if (g_fatigueIcon) Renderer->DrawBitmapToTarget(g_fatigueIcon, iconX, kIconYFatigue, tw, th);
    if (g_manaIcon)    Renderer->DrawBitmapToTarget(g_manaIcon,    iconX, kIconYMana,    tw, th);
    if (g_healthIcon)  Renderer->DrawBitmapToTarget(g_healthIcon,  iconX, kIconYHealth,  tw, th);

    // ---------- bars (spec §6b, slot 23 step 7/8) ----------
    // dir: player +1, target -1. Shadow offset: player +dx (down-right),
    // target -dx (down-left mirror) per §7.
    const int32_t dir      = isRight ? -1 : +1;
    const int32_t shadowDX = isRight ? -kShadowDX : kShadowDX;

    // Value getters +0x1c0/+0x1d0/+0x1c8, maxima +0x1d8/+0x1e8/+0x1e0 (§6).
    const int32_t values[3] = { ch->Health(), ch->Mana(), ch->Fatigue() };
    struct { int bar; int v; int mx; } rows[3] = {
        { BAR_HP, values[0], ch->MaxHealth()  },
        { BAR_MP, values[1], ch->MaxMana()    },
        { BAR_FT, values[2], ch->MaxFatigue() },
    };
    for (const auto& r : rows)
    {
        const SBarGeom& g = kBars[r.bar];
        // dst: player raw screen - chrome bias (§11 #2); target = pane_w-off,
        // de-biased on the mirrored side (§7).
        const int32_t dstY = g.dstY - kBarBiasY;
        const int32_t dstX = (isRight ? (g_paneW - g.targetOff) + kBarBiasX + kTargetBarXFix
                                      : (g.dstXPlayer - kBarBiasX)) + sideShift;
        DrawBar(g, dstX, dstY, r.v, r.mx, dir, fadeAlpha, shadowDX, tw, th);
    }

    // ---------- text (spec §8, slot 23 steps 2/3/5/6) ----------
    // White, 3-pass black shadow (font flag 0x400), GDI-style TOP-aligned in
    // the cell (DT_TOP, no vcenter). Player values LEFT (0x401); target values
    // RIGHT (0x404); name/level CENTER (0x402, HORIZONTAL only). No per-element
    // vertical nudge — font.cpp kGdiTopLeading handles the baseline once.
    if (g_font)
    {
        const ETextAlign valAlign = isRight ? ETextAlign::Right : ETextAlign::Left;
        const int32_t    valX     = (isRight ? (g_paneW - kValueOffTarget) : kValueXPlayer) + sideShift;

        char buf[32];
        const int32_t valYs[3] = { kValueYHealth, kValueYMana, kValueYFatigue };
        for (int i = 0; i < 3; ++i)
        {
            std::snprintf(buf, sizeof(buf), "%d", values[i]);         // "%d" (§8)
            DrawTextShadowedToTarget(g_font, buf, valX, valYs[i],
                                     kValueCellW, kValueCellH, valAlign,
                                     1.0f, 1.0f, 1.0f, tw, th);        // white (§8)
        }

        // name + level — single "%s\nLevel %d" block, CENTER, in the 0x40x0x40
        // cell (§8). DrawText…ToTarget is single-line, so emit line 1 at the
        // cell TOP and line 2 one lineHeight below (DT_TOP — the lower part of
        // the 64px cell stays EMPTY; NOT vertically centered, error #7).
        // Center the name/level cell on the portrait ring (not the forensics
        // cell literal): nameX = ringScreenCX - cellW/2 (fixes the ~6px off).
        const int32_t nameX = ringScreenCX - kNameCellW / 2;
        const float   lh    = TextLineHeight(g_font);
        char line1[64];
        char line2[64];
        FormatNameLevel(ch, line1, line2);

        DrawTextShadowedToTarget(g_font, line1, nameX, kNameCellY,
                                 kNameCellW, kValueCellH, ETextAlign::Center,
                                 1.0f, 1.0f, 1.0f, tw, th);
        if (line2[0])
            DrawTextShadowedToTarget(g_font, line2, nameX,
                                     kNameCellY + (int32_t)(lh + 0.5f),
                                     kNameCellW, kValueCellH, ETextAlign::Center,
                                     1.0f, 1.0f, 1.0f, tw, th);
    }
}

// =====================================================================
// HUD drawable — composes the whole pane RT then DrawSurface's it once
// (spec §3 direct-renderer contract), like uisidetabstest.cpp.
// =====================================================================
class TPlyrStatusHud : public THudDrawable
{
public:
    void Draw() override
    {
        // Both chips sit kChipInset px in from the screen edges they hug — the
        // horizontal inset is applied per-side inside the pane (sideShift); the
        // top gap is this uniform downward offset of the whole pane.
        if (g_pane) Renderer->DrawSurface(g_pane, 0, kChipInset);
    }

    void Refresh()
    {
        if (!g_backPanel) return;
        EnsurePane();
        if (!g_pane) return;

        TCharacter* target = CurrentTarget();
        if (target)
            g_lastTarget = target;
        AdvanceRamps(target != nullptr);

        const int32_t tw = g_pane->Width();
        const int32_t th = g_pane->Height();

        // Transparent clear so the chip art alone shows over the playfield
        // (the pane is a HUD overlay; spec §3).
        g_pane->StartPass(0.0f, 0.0f, 0.0f, 0.0f);

        // Nothing at all without a player (slot23 :61-63).
        if (Player)
        {
            // Player chip: origin screen 0 (§3).
            DrawSide(Player, /*isRight*/ false, g_playerFade, tw, th);

            // Target chip: origin screen pane_w-0xc1 (§3), while its fade > 0;
            // a target the player let go keeps drawing as it fades (§5/§9).
            TCharacter* shown = target ? target : g_lastTarget.Get();
            if (shown && g_targetFade > 0)
                DrawSide(shown, /*isRight*/ true, g_targetFade, tw, th);
        }

        g_pane->EndPass();
    }

private:
    void EnsurePane()
    {
        constexpr int32_t kSidebarW = 0xbc;
        const int32_t dw = Display.Width();
        const SHudState& hs = GetHudState();
        const int32_t sidebarAdj = (hs.sidebarState == HUD_SIDEBAR_OPEN) ? kSidebarW : 0;
        const int32_t want = ((dw > 0) ? dw : (2 * kChipW)) - sidebarAdj;
        if (g_pane && g_paneW == want) return;
        delete g_pane;
        g_paneW = want;
        // Compose the WHOLE pane (both chips) into one RT (§3): full width x 0x70.
        g_pane = new TSurface(g_paneW > 0 ? g_paneW : 1, kPaneH, SG_PIXELFORMAT_RGBA8);
    }

    // Fade ramps, gated to the 24Hz tick (slot 19 FUN_00549da0, spec §9):
    // the player card ramps to 6 while a player exists, the target card
    // toward 6 while the player fights someone and back to 0 after.
    static void AdvanceRamps(bool targetPresent)
    {
        const double nowMs = TTime::Time() * 1000.0;
        if (g_lastTickMs == 0.0) g_lastTickMs = nowMs;
        int32_t guard = 0;
        while (nowMs - g_lastTickMs >= kSimTickMs && guard < 64)
        {
            g_lastTickMs += kSimTickMs;
            ++guard;
            if (!Player)
            {
                g_playerFade = g_targetFade = 0;
                continue;
            }
            if (g_playerFade < kFadeMax) ++g_playerFade;
            const int32_t tgt = targetPresent ? kFadeMax : 0;
            if (g_targetFade < tgt)      ++g_targetFade;
            else if (tgt < g_targetFade) --g_targetFade;
        }
    }
};

TPlyrStatusHud g_hud;

}  // namespace

// =====================================================================
// Entry points (testmodes.cpp links these — keep the signatures).
// =====================================================================
bool InitializeUIPlyrStatusBarMode()
{
    log_info("[ui-plyrstatusbar] === TPlyrStatusBar (clean-room from spec) ===");

    // Real assets (spec §2). Classic / no-texture build: statusbarnotex.dat.
    g_statusbarDat = TMulti::LoadMulti((char*)"statusbarnotex.dat");
    g_portraitsDat = TMulti::LoadMulti((char*)"portraits.dat");
    RegisterUIBitmapAtlasArchive(g_statusbarDat);
    RegisterUIBitmapAtlasArchive(g_portraitsDat);

    if (g_statusbarDat)
    {
        g_backPanel   = LookupByName(g_statusbarDat, "BackPanel");
        g_bars        = LookupByName(g_statusbarDat, "Bars");
        g_healthIcon  = LookupByName(g_statusbarDat, "HealthIcon");
        g_manaIcon    = LookupByName(g_statusbarDat, "ManaIcon");
        g_fatigueIcon = LookupByName(g_statusbarDat, "FatigueIcon");
        g_ring        = LookupByName(g_statusbarDat, "Ring");
    }
    if (g_portraitsDat)
        g_lockeFace = LookupByName(g_portraitsDat, "LockeFace");

    log_info("[ui-plyrstatusbar] assets: BackPanel=%s Bars=%s "
             "Health=%s Mana=%s Fatigue=%s Ring=%s LockeFace=%s",
             g_backPanel ? "OK" : "MISS", g_bars ? "OK" : "MISS",
             g_healthIcon ? "OK" : "MISS", g_manaIcon ? "OK" : "MISS",
             g_fatigueIcon ? "OK" : "MISS", g_ring ? "OK" : "MISS",
             g_lockeFace ? "OK" : "MISS");

    // Font: LiberationSans "Small" (spec §8).
    g_font = BuildTTFAtlas(kFontPath, kFontPx);
    log_info("[ui-plyrstatusbar] font %s @%dpx = %s",
             kFontPath, kFontPx, g_font ? "OK" : "MISS");

    delete g_pane;
    g_pane       = nullptr;
    g_paneW      = 0;
    g_playerFade = 0;
    g_targetFade = 0;
    g_lastTickMs = 0.0;
    g_lastTarget.Clear();

    Renderer->AddHud(&g_hud, 0.0f);
    return true;
}

void RenderUIPlyrStatusBarMode()
{
    RenderUIPlyrStatusBarModeEmbedded();

    // Backdrop so the chip's transparent areas + the round portrait/ring edges
    // read in isolation (no playfield behind the HUD in test mode).
    Display.BackBuffer()->StartPass(0.32f, 0.36f, 0.28f, 1.0f);
    Display.BackBuffer()->EndPass();
}

void RenderUIPlyrStatusBarModeEmbedded()
{
    g_hud.Refresh();
}

void CloseUIPlyrStatusBarMode()
{
    Renderer->RemoveHud(&g_hud);
    delete g_pane;
    g_pane         = nullptr;
    g_paneW        = 0;
    g_backPanel    = g_bars = g_healthIcon = g_manaIcon = nullptr;
    g_fatigueIcon  = g_ring = g_lockeFace = nullptr;
    g_lastTarget.Clear();
    g_statusbarDat = nullptr;
    g_portraitsDat = nullptr;
    g_font         = nullptr;
    g_lastTickMs   = 0.0;
}
