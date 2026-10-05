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
// from a retail bitmap TFont (keycolor pixels become alpha=0 so straight-alpha
// blend composites correctly) and BuildTTFAtlas from a TrueType face. The
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

const SFontAtlas* BuildFontAtlas(TFont* font);
  // Returns the cached atlas for `font`, building it on first call.
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
const SFontAtlas* BuildTTFAtlas(const char* path, int pixel_height);
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
// Use to top-align a string inside a cell: baseline = cellY + ascent.
float TextAscent(const SFontAtlas* atlas);
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
// horizontally aligned per `align`, baseline top-aligned in the cell
// (baseline = cellY + ascent). Color is straight RGB in [0,1]; the atlas
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

// Word-wraps `text` to `wrapWidth` pixels the way retail's GDI DT_WORDBREAK
// did (FUN_004acb80): breaks at spaces and '\n', drops the spaces at a break,
// keeps a word wider than the line whole. Appends the lines to `lines` and
// returns how many it added (at least one).
int32_t WrapTextLines(const SFontAtlas* atlas, const char* text, float wrapWidth,
                      std::vector<std::string>& lines,
                      ETextEncoding encoding = kGameTextEncoding);
