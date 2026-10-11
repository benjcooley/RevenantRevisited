// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 font.h - EXILE Font Definition File                   *
// *************************************************************************

#pragma once

#include "revenant.h"

#include "bitmap.h"
#include "fontdata.h"
#include "render3d_types.h"
#include "resource.h"
#include "textencoding.h"

#include <array>
#include <string>
#include <utility>
#include <vector>

#define FONT_DRAWMODE   (DM_TRANSPARENT | DM_ALIAS | DM_BACKGROUND)

_CLASSDEF(TFont)

class TFont : public TFontData
{
  public:
    TFont() {}
    PTBitmap GetChar(unsigned char ch);
    int32_t FirstChar() const
        { return firstchar; }
    int32_t Numchars() const
        { return numchars; }
    int32_t StartHeight(unsigned char ch) const
        { return (int32_t) startheight[ch - firstchar]; }
    int32_t DrawRight(unsigned char ch) const
        { return (int32_t) drawright[ch - firstchar]; }
    int32_t DrawLeft(unsigned char ch) const
        { return (int32_t) drawleft[ch - firstchar]; }

    int32_t FindNumLinesInText(char *text, int32_t wrapwidth = -1, int32_t justify = JUSTIFY_LEFT);
        // Find the number of lines in the given text and wrapwidth - this function
        // resides in the font because it is highly variable depending on the overlap
        // and size of each character in the font.
};

// A font's glyphs baked into one RGBA8 texture, looked up by Unicode code
// point (docs/ui/TEXT_RENDERING.md). Two builders make one: BuildFontAtlas
// from a retail bitmap TFont (each glyph as retail draws it: keyed, with its
// anti-aliased edge at its coverage, in straight alpha) and BuildTTFAtlas
// from a TrueType face. The
// bitmap atlases live in a parallel cache keyed by TFont* rather than on the
// TFont itself, because TFont instances are raw resource-file memory
// (malloc'd to exact on-disk size by LoadResource) — appending member fields
// would corrupt the bitmap data that follows.
struct SFontAtlasRect
{
    uint16_t x = 0, y = 0, w = 0, h = 0;    // pixel rect inside the atlas image
    float xoff = 0, yoff = 0;     // top-left offset from pen baseline
    float xoff2 = 0, yoff2 = 0;   // bottom-right offset from pen baseline
    float xadvance = 0;           // pen advance after glyph
    // The float metrics are populated for both TTF (stb) and bitmap (DrawLeft/
    // StartHeight/DrawRight) atlases, so one glyph walk renders either kind.
};

// Retail's drop shadow under text (0x004be2b0, flag 0x400): black passes
// at (dx, dy), (dx + 1, dy) and (dx, dy + 1), then the colour; one pass at
// (dx, dy) when `single`.
struct STextShadow
{
    int8_t dx = 1, dy = 1;
    bool single = false;
};

// The shadow 0x004be2b0 gives a font of cell height `height` (0x004be716):
// (1, 1) up to 14 px, (2, 1) up to 25, a single pass at (2, 2) above.
[[nodiscard]] constexpr STextShadow RetailTextShadow(float height)
{
    if (height <= 14.0f)
        return { 1, 1, false };
    if (height <= 25.0f)
        return { 2, 1, false };
    return { 2, 2, true };
}

struct SFontAtlas
{
    // glyphs[kFallbackGlyph] draws every code point the atlas doesn't hold:
    // the face's .notdef box for TrueType, nothing (no advance) for a retail
    // bitmap font, which drew nothing for a byte outside it.
    static constexpr uint16_t kFallbackGlyph = 0;

    TTextureHandle texture = kInvalidTexture;
    int32_t        width = 0;
    int32_t        height = 0;
    float          ascent = 0.0f;       // line metrics: pixels above the baseline
    float          descent = 0.0f;      // and below it (TextAscent, TextLineHeight)
    // How far below a text cell's top the baseline sits, as retail hangs a
    // line in its cell (TextCellBaseline).
    float          cellBaseline = 0.0f;
    // The cell helpers draw the coloured text twice over, lifting thin
    // anti-aliased coverage to GDI's weight (the TrueType stand-ins for
    // retail's GDI fonts); a bitmap font's glyphs are blitted once.
    bool           doubleStrike = false;
    // How far apart wrapped lines sit: a GDI font's cell height (tmHeight,
    // the TrueType stand-in's pixel height); a bitmap font's line height.
    float          lineStep = 0.0f;
    // Retail's text shadow for the font (RetailTextShadow of lineStep).
    STextShadow    shadow;
    // What the atlas stands for in the A/B's text record (STextDraw), as the
    // emulator names retail's fonts: GDI's "Arial 12" ("<face> <height>[
    // bold]"), a bitmap font's FONT.DEF name.
    std::string    face;
    std::vector<SFontAtlasRect> glyphs;

    // The glyph that draws `cp`; the fallback for a code point the atlas
    // doesn't hold. Safe for every code point.
    [[nodiscard]] const SFontAtlasRect& Glyph(char32_t cp) const;

    // Builder side: `cp` draws glyphs[index].
    void MapGlyph(char32_t cp, uint16_t index);

  private:
    std::array<uint16_t, 0x100> latin{};                 // U+0000..U+00FF
    std::vector<std::pair<char32_t, uint16_t>> beyond;   // the rest, by code point
};

const SFontAtlas* BuildFontAtlas(TFont* font, const char* face = nullptr);
  // Returns the cached atlas for `font`, building it on first call; `face`
  // labels it if it has no label yet (SFontAtlas::face).
  // Null if the font is null or has no glyphs.
const SFontAtlas* FindFontAtlas(TFont* font);
  // Cache lookup only; returns null if never built.
void DestroyAllFontAtlases();
  // Releases every cached atlas image. Call during shutdown.

void LogFontInfo(const char* name);
  // Logs the named FONT.DEF entry plus a few representative glyph metrics.
void LogFontGlyphAsciiArt(const char* fontname, unsigned char ch);
  // Logs a coarse ASCII-art view of one glyph for bitmap-font debugging.
void LogFontGlyphHexDump(const char* fontname, unsigned char ch,
                        int max_rows = 8, int max_cols = 14);
  // Logs the raw 16-bit source pixels for the top-left rows of one glyph.

// stb_truetype-backed atlas for WINFONT entries. Rasterizes the printable
// Windows-1252 repertoire (ASCII, CP1252's 0x80..0x9F specials, Latin-1) at
// `pixel_height`, plus the face's .notdef as the fallback glyph. Uses a
// separate cache (keyed by path+size) from BuildFontAtlas's TFont* cache
// because TTF fonts have no TFont resource to key against.
const SFontAtlas* BuildTTFAtlas(const char* path, int pixel_height, const char* face = nullptr);
  // `face` labels the atlas if it has no label yet (SFontAtlas::face).

// The cell GDI gives a TrueType font made with CreateFontA(+height): the em
// size in pixels and the cell's ascent and descent above and below the
// baseline (tmAscent / tmDescent; tmHeight is their sum, at most the height
// asked for).
struct SGdiFontCell
{
    int32_t ppem = 0;
    int32_t ascent = 0;
    int32_t descent = 0;
};
// The stand-in for a retail GDI font: rasterized at the cell's em size, its
// lines `ascent + descent` apart with the baseline `ascent` under a cell's
// top, as DrawTextA lays them; each glyph's advance is the face's advance
// width at that size, rounded half to even (GDI's unhinted width -- the
// stand-ins share the Windows faces' advance widths).
const SFontAtlas* BuildTTFAtlas(const char* path, const SGdiFontCell& cell, const char* face);
void DestroyAllTTFAtlases();

// The path of a TrueType face the port ships, named by file
// ("Arimo-Regular.ttf"). The faces are engine assets in assets/fonts/, so
// this is rev_engine_asset("fonts/<file>"): it does not depend on the
// working directory. Every load of one of these faces resolves its file here.
// Empty when the engine assets can't be found (logged by rev_engine_asset).
[[nodiscard]] std::string TTFFilePath(const char* file);

// *************************************************************************
// * Canonical UI text drawing (the shared "text" / "text with shadow")   *
// *************************************************************************
//
// These are THE common text primitives every UI panel uses — do not
// hand-roll a glyph walk or a 3-pass shadow in a panel. They live here
// (UI/font layer) rather than on TRenderer because TRenderer is
// deliberately font-agnostic; they emit per-glyph blits through the
// renderer's to-target composite path, so they draw into the currently
// active TSurface render-target pass (the compose-to-target contract).
//
// Retail parity: DrawTextShadowedToTarget reproduces FUN_004be2b0's
// font-flag-0x400 behavior — 3 BLACK passes (base, +1x, +1y) then the
// colored pass at base (UI_METHOD_MAP §5). The pink halo seen in some
// retail captures is a chroma-key artifact, NOT reproduced here
// (project-retail-pink-halo-bug).
//
// Text is bytes in `encoding`, which defaults to the game data's
// Windows-1252 (textencoding.h); each character is decoded to a code point
// and drawn with SFontAtlas::Glyph. A UTF-8 source passes ETextEncoding::Utf8.

enum class ETextAlign { Left, Center, Right };

// Total advance width of `text` in `atlas` (pixels). For alignment.
float TextWidth(const SFontAtlas* atlas, const char* text,
                ETextEncoding encoding = kGameTextEncoding);
// Ascent of `atlas` (pixels above baseline), fixed when the atlas is built.
float TextAscent(const SFontAtlas* atlas);
// Where a line top-aligned in a cell puts its baseline, below the cell's top.
// A bitmap font hangs from its 'A' (retail's font table, 0x004ac990); a
// TrueType face stands in for a GDI font, which hangs from its ascent less
// GDI's internal leading. The cell helpers below draw at it.
float TextCellBaseline(const SFontAtlas* atlas);
// Line height of `atlas` (pixels) = ascent + descent. Use to stack the cells
// of a multi-line block: line N cellY = first cellY + N * lineHeight.
float TextLineHeight(const SFontAtlas* atlas);

// The single glyph walk shared by every text path: blit `text` from `atlas`
// starting at pen (penX, baselineY), tinted (r,g,b,1), into the active
// render-target pass. Each glyph sits at its true metric position; the dst
// extent is the glyph's logical size and the src is the (possibly oversampled)
// atlas rect, so it renders TTF and bitmap atlases identically. The cell/align
// helpers below and the test modes all funnel through this.
void DrawTextAtBaseline(const SFontAtlas* atlas, const char* text,
                        float penX, float baselineY,
                        float r, float g, float b,
                        int32_t target_w, int32_t target_h,
                        ETextEncoding encoding = kGameTextEncoding);

// Draw one line of `text` into the cell (cellX, cellY, cellW, cellH),
// horizontally aligned per `align`, top-aligned in the cell (baseline =
// cellY + TextCellBaseline). Color is straight RGB in [0,1]; the atlas
// coverage is tinted by it. Targets the active render-target pass.
// Multi-line: call once per line with the cell advanced by line height.
void DrawTextToTarget(const SFontAtlas* atlas, const char* text,
                      int32_t cellX, int32_t cellY, int32_t cellW, int32_t cellH,
                      ETextAlign align, float r, float g, float b,
                      int32_t target_w, int32_t target_h,
                      ETextEncoding encoding = kGameTextEncoding);
// Same, with the retail 3-pass black drop shadow under the colored text.
void DrawTextShadowedToTarget(const SFontAtlas* atlas, const char* text,
                              int32_t cellX, int32_t cellY, int32_t cellW, int32_t cellH,
                              ETextAlign align, float r, float g, float b,
                              int32_t target_w, int32_t target_h,
                              ETextEncoding encoding = kGameTextEncoding);
// The shadowed text with its pen at (penX, baselineY), for callers whose
// retail coordinates are a baseline rather than a cell top (the text bar's
// lines, TTextBar 0x0054cd40). Same passes as DrawTextShadowedToTarget.
void DrawTextShadowedAtBaseline(const SFontAtlas* atlas, const char* text,
                                float penX, float baselineY,
                                float r, float g, float b,
                                int32_t target_w, int32_t target_h,
                                ETextEncoding encoding = kGameTextEncoding);

// A wrapped block as retail's DrawTextA with DT_WORDBREAK | DT_EDITCONTROL
// lays it in its rect (x, y, w, h): `text` wrapped to w (WrapTextLines), a
// line every SFontAtlas::lineStep from the rect's top, each aligned in w,
// and only the lines that fit whole in h (the first always). Shadowed as
// DrawTextShadowedToTarget when `shadowed`. Returns the height drawn, as
// DrawTextA does.
int32_t DrawTextWrappedToTarget(const SFontAtlas* atlas, const char* text,
                                int32_t x, int32_t y, int32_t w, int32_t h,
                                ETextAlign align, float r, float g, float b, bool shadowed,
                                int32_t target_w, int32_t target_h,
                                ETextEncoding encoding = kGameTextEncoding);

// A text draw as the canonical helpers make it, for the retail A/B hosts:
// the A/B compares it with retail's own text compose (0x004be2b0, which
// the emulator records the same way) -- string, font, place and colour.
struct STextDraw
{
    std::string text;
    std::string face;                       // SFontAtlas::face
    int32_t x = 0, y = 0, w = 0, h = 0;     // the cell; the pen and 0 x 0 when `baseline`
    ETextAlign align = ETextAlign::Left;
    uint8_t r = 0, g = 0, b = 0;
    bool shadowed = false;
    bool baseline = false;                  // y is the baseline, not the cell's top
    bool wrapped = false;                   // DrawTextWrappedToTarget's rect
    std::vector<std::string> lines;         // `wrapped`: the lines drawn, in order
};
// While set, every canonical text draw is appended to `recorder` (an A/B
// host's; null stops it).
void SetTextDrawRecorder(std::vector<STextDraw>* recorder);

// Word-wraps `text` to `wrapWidth` pixels the way retail's GDI DT_WORDBREAK
// did (FUN_004acb80): breaks at spaces and '\n', drops the spaces at a break,
// keeps a word wider than the line whole. Appends the lines to `lines` and
// returns how many it added (at least one).
int32_t WrapTextLines(const SFontAtlas* atlas, const char* text, float wrapWidth,
                      std::vector<std::string>& lines,
                      ETextEncoding encoding = kGameTextEncoding);
