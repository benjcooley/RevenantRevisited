// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 font.cpp - EXILE Font Objects File                    *
// *************************************************************************

#include "font.h"

#include "bitmap.h"
#include "fonttable.h"
#include "graphics.h"
#include "logging.h"
#include "renderer.h"

#include <cstdlib>
#include <cstring>
#include <memory>
#include <unordered_map>

PTBitmap TFont::GetChar(unsigned char ch)
{
    if (ch < firstchar || ch > (firstchar + numchars))
        return nullptr;

    PTBitmap character = (PTBitmap)chars[ch - firstchar].ptr();
    return character;
}

// ----- Font atlas parallel cache --------------------------------------------

namespace {

std::unordered_map<TFont*, SFontAtlas*> g_fontAtlases;

SFontAtlas* LookupAtlas(TFont* font)
{
    const auto it = g_fontAtlases.find(font);
    return it != g_fontAtlases.end() ? it->second : nullptr;
}

// Blit one glyph's pixels into an RGBA8 region of `dst`. Keycolor pixels go
// to alpha=0 so straight-alpha blend composites correctly. Non-paletted
// retail bitmaps are standard RGB555 with bit 15 unused:
//   R @ bits 14-10, G @ 9-5, B @ 4-0.
// (Note: retail's paletted convert uses a different layout — bit 5 unused —
// but that's only applied to 8-bit-source bitmaps, not true 16-bit ones.)
void BlitBitmap16ToRGBA8(uint8_t* dst, int32_t dst_pitch,
                         int32_t ox, int32_t oy, PTBitmap bm)
{
    const int32_t w = bm->width;
    const int32_t h = bm->height;
    const uint16_t key = (uint16_t)bm->keycolor;
    const uint16_t* src = bm->data16;
    for (int32_t y = 0; y < h; y++)
    {
        uint8_t* row = dst + (oy + y) * dst_pitch + ox * 4;
        for (int32_t x = 0; x < w; x++)
        {
            const uint16_t px = src[y * w + x];
            if (px == key)
            {
                row[0] = row[1] = row[2] = row[3] = 0;
            }
            else
            {
                row[0] = (uint8_t)(((px >> 10) & 0x1F) << 3);
                row[1] = (uint8_t)(((px >> 5)  & 0x1F) << 3);
                row[2] = (uint8_t)(( px        & 0x1F) << 3);
                row[3] = 255;
            }
            row += 4;
        }
    }
}

} // namespace

const SFontAtlas* BuildFontAtlas(TFont* font)
{
    if (!font) return nullptr;

    if (SFontAtlas* existing = LookupAtlas(font))
        return existing;

    const int32_t first = font->FirstChar();
    const int32_t count = font->Numchars();
    if (count <= 0)
        return nullptr;

    // Pass 1: shelf-pack into a 512-wide atlas to determine final height.
    constexpr int32_t kAtlasWidth   = 512;
    constexpr int32_t kGlyphPadding = 1;

    auto atlas = std::make_unique<SFontAtlas>();
    atlas->firstchar = (int16_t)first;
    atlas->numchars  = (int16_t)count;
    atlas->width     = kAtlasWidth;

    int32_t cursor_x = 0;
    int32_t shelf_y  = 0;
    int32_t shelf_h  = 0;

    for (int32_t i = 0; i < count; i++)
    {
        PTBitmap bm = font->GetChar((unsigned char)(first + i));
        auto& r = atlas->rects[i];
        if (!bm || bm->width <= 0 || bm->height <= 0)
        {
            r = { 0, 0, 0, 0 };
            continue;
        }
        const int32_t gw = bm->width;
        const int32_t gh = bm->height;

        if (cursor_x + gw > kAtlasWidth)
        {
            shelf_y += shelf_h + kGlyphPadding;
            cursor_x = 0;
            shelf_h  = 0;
        }

        r.x = (uint16_t)cursor_x;
        r.y = (uint16_t)shelf_y;
        r.w = (uint16_t)gw;
        r.h = (uint16_t)gh;

        // Carry the bitmap font's per-glyph metrics in the same stb-style
        // fields the TTF path uses, so one glyph walk serves both atlas kinds.
        // DrawLeft/DrawRight are horizontal bearings (advance = right-left);
        // StartHeight is the glyph top's rise above the line baseline that the
        // pen y references. Bitmap glyphs are 1:1 (no oversample) so the dst
        // extent (xoff2-xoff, yoff2-yoff) equals the source pixel extent.
        const unsigned char ch = (unsigned char)(first + i);
        r.xoff     = -(float)font->DrawLeft(ch);
        r.yoff     = -(float)font->StartHeight(ch);
        r.xoff2    = r.xoff + (float)gw;
        r.yoff2    = r.yoff + (float)gh;
        r.xadvance = (float)(font->DrawRight(ch) - font->DrawLeft(ch));

        cursor_x += gw + kGlyphPadding;
        if (gh > shelf_h) shelf_h = gh;
    }

    atlas->height = shelf_y + shelf_h;
    if (atlas->height <= 0)
    {
        log_warn("[font] BuildFontAtlas: font has no renderable glyphs");
        return nullptr;
    }

    // Pass 2: allocate RGBA8 buffer, blit each glyph into its packed rect.
    const size_t pitch = size_t(atlas->width) * 4;
    const size_t bytes = pitch * atlas->height;
    uint8_t* rgba = (uint8_t*)std::calloc(1, bytes);
    if (!rgba)
    {
        log_error("[font] BuildFontAtlas: calloc failed (%zu bytes)", bytes);
        return nullptr;
    }

    for (int32_t i = 0; i < count; i++)
    {
        const auto& r = atlas->rects[i];
        if (r.w == 0 || r.h == 0) continue;
        PTBitmap bm = font->GetChar((unsigned char)(first + i));
        if (!bm) continue;
        BlitBitmap16ToRGBA8(rgba, (int32_t)pitch, r.x, r.y, bm);
    }

    if (Renderer)
    {
        atlas->texture = Renderer->RegisterTextureAsset(0,
                                                        rgba,
                                                        bytes,
                                                        atlas->width,
                                                        atlas->height,
                                                        ERendererTextureFormat::RGBA8,
                                                        bytes,
                                                        ERendererTextureFilter::Nearest);
        if (atlas->texture != kInvalidTexture)
            Renderer->AddTextureAssetRef(atlas->texture);
    }

    std::free(rgba);

    if (atlas->texture == kInvalidTexture)
    {
        log_error("[font] BuildFontAtlas: texture upload failed");
        return nullptr;
    }

    log_info("[font] atlas built: %dx%d for %d glyphs (first=%d)",
        atlas->width, atlas->height, count, first);

    SFontAtlas* raw = atlas.release();
    g_fontAtlases.emplace(font, raw);
    return raw;
}

const SFontAtlas* FindFontAtlas(TFont* font)
{
    return LookupAtlas(font);
}

void DestroyAllFontAtlases()
{
    for (auto& [font, atlas] : g_fontAtlases)
    {
        if (!atlas) continue;
        if (Renderer && atlas->texture != kInvalidTexture)
            Renderer->ReleaseTextureAssetRef(atlas->texture);
        delete atlas;
    }
    g_fontAtlases.clear();
}

void LogFontInfo(const char* name)
{
    if (!FontTable)
    {
        log_error("[font] FontTable is null");
        return;
    }

    TGenericFont* g = FontTable->FindFont(name);
    if (!g)
    {
        log_warn("[font] '%s' not in font.def", name);
        return;
    }

    TFont* f = g->primary;
    if (!f)
    {
        log_warn("[font] '%s' has no primary bitmap atom (type=%d)", name, g->type);
        return;
    }

    log_info("[font] '%s' firstchar=%d numchars=%d height=%d",
        name, (int)f->FirstChar(), (int)f->Numchars(), (int)f->height);

    for (unsigned char probe : { (unsigned char)'A', (unsigned char)'0', (unsigned char)' ' })
    {
        PTBitmap bm = f->GetChar(probe);
        if (!bm)
        {
            log_warn("[font]   glyph '%c' (0x%02x): GetChar returned null", probe, probe);
            continue;
        }
        log_info("[font]   glyph '%c' (0x%02x): w=%d h=%d flags=0x%08x keycolor=0x%08x",
            probe, probe, bm->width, bm->height, bm->flags, bm->keycolor);
    }
}

void LogFontGlyphAsciiArt(const char* fontname, unsigned char ch)
{
    if (!FontTable) return;
    TFont* f = FontTable->Bitmap(fontname);
    if (!f) return;

    PTBitmap bm = f->GetChar(ch);
    if (!bm) return;

    const int w = bm->width;
    const int h = bm->height;
    if (w <= 0 || h <= 0 || w > 64 || h > 64)
    {
        log_warn("[font] skip ascii-render '%c' from '%s': implausible %dx%d", ch, fontname, w, h);
        return;
    }

    const uint16_t key = (uint16_t)bm->keycolor;
    log_info("[font] ascii-render '%c' from '%s' (%dx%d, 16-bit, key=0x%04x):", ch, fontname, w, h, key);
    for (int y = 0; y < h; y++)
    {
        char row[72];
        int pos = 0;
        for (int x = 0; x < w && pos < 70; x++)
        {
            const uint16_t px = bm->data16[y * w + x];
            row[pos++] = (px == key) ? '.' : '#';
        }
        row[pos] = 0;
        log_info("[font]   |%s|", row);
    }
}

void LogFontGlyphHexDump(const char* fontname, unsigned char ch,
                         int max_rows, int max_cols)
{
    if (!FontTable) return;
    TFont* f = FontTable->Bitmap(fontname);
    if (!f) return;

    PTBitmap bm = f->GetChar(ch);
    if (!bm || bm->width <= 0 || bm->height <= 0) return;

    const uint16_t* src = bm->data16;
    const int rows = bm->height < max_rows ? bm->height : max_rows;
    const int cols = bm->width < max_cols ? bm->width : max_cols;
    for (int r = 0; r < rows; r++)
    {
        char row[256];
        int p = 0;
        row[0] = 0;
        for (int i = 0; i < cols && p < int(sizeof(row)); i++)
            p += snprintf(row + p, sizeof(row) - p, "%04x ", src[r * bm->width + i]);
        log_info("[font/diag] '%s' '%c' row%d: %s", fontname, ch, r, row);
    }
}

int32_t TFont::FindNumLinesInText(char *text, int32_t wrapwidth, int32_t justify)
{
    SDrawBlock db;
    SDrawParam dp;
    STextParam tp;
    dp.func = TextDraw;
    dp.dx = dp.dy = 0;
    dp.data = (void *)&tp;

    tp.text = text;
    tp.numlines = 100000;
    tp.startline = 0;
    tp.wrapwidth = wrapwidth;
    tp.font = this;
    tp.justify = justify;
    tp.draw = false;
    tp.length = 0;

    TextDraw(&db, &dp);
    return tp.length;
}

// *************************************************************************
// * Canonical UI text drawing (see font.h)                               *
// *************************************************************************

float TextWidth(const SFontAtlas* atlas, const char* text)
{
    if (!atlas || !text) return 0.0f;
    float w = 0.0f;
    for (const char* p = text; *p; ++p)
        w += atlas->Rect((unsigned char)*p).xadvance;
    return w;
}

float TextAscent(const SFontAtlas* atlas)
{
    if (!atlas) return 0.0f;
    // Ascent = how far the tallest glyph rises above the baseline. stb's
    // yoff is the (negative) top offset from the pen baseline, so the
    // ascent is max(-yoff) over the renderable glyphs.
    float ascent = 0.0f;
    for (int i = 0; i < atlas->numchars; ++i)
    {
        const SFontAtlasRect& r = atlas->rects[i];
        if (r.w > 0 && r.h > 0 && -r.yoff > ascent)
            ascent = -r.yoff;
    }
    return ascent;
}

float TextLineHeight(const SFontAtlas* atlas)
{
    if (!atlas) return 0.0f;
    // Line height = ascent + descent. stb's yoff2 is the (positive) bottom
    // offset from the baseline, so descent = max(yoff2) over the glyphs.
    float descent = 0.0f;
    for (int i = 0; i < atlas->numchars; ++i)
    {
        const SFontAtlasRect& r = atlas->rects[i];
        if (r.w > 0 && r.h > 0 && r.yoff2 > descent)
            descent = r.yoff2;
    }
    return TextAscent(atlas) + descent;
}

namespace {

// Shared glyph walk: blit each glyph of `text` into the active render-target
// pass at the given pen baseline, tinted (r,g,b,1). The destination extent is
// the glyph's LOGICAL size (xoff2-xoff, yoff2-yoff); the source extent is the
// packed atlas rect (rc.w, rc.h), which is 2x larger because the TTF atlas is
// 2x2 oversampled (ttfatlas.cpp). The GPU sampler downsamples src->dst, giving
// crisp antialiased glyphs at the intended size. (Drawing dst == rc.w/rc.h
// instead renders every glyph 2x too big and overlapping.) Glyphs sit at their
// true metric position so caps/ascenders keep their height above the baseline;
// there is no per-glyph cell scissor (that truncated cap tops and made
// lowercase look top-justified). This matches the vetted RenderTTFMode walk in
// testmodes.cpp. Used by both the plain and shadowed public helpers.
void DrawGlyphRun(const SFontAtlas* atlas, const char* text,
                  float penX, float baselineY,
                  float r, float g, float b,
                  int32_t target_w, int32_t target_h)
{
    if (!Renderer) return;
    for (const char* p = text; *p; ++p)
    {
        const SFontAtlasRect& rc = atlas->Rect((unsigned char)*p);
        if (rc.w > 0 && rc.h > 0)
        {
            const int32_t gx = (int32_t)(penX + rc.xoff + 0.5f);
            const int32_t gy = (int32_t)(baselineY + rc.yoff + 0.5f);
            const int32_t gw = (int32_t)(rc.xoff2 - rc.xoff + 0.5f);
            const int32_t gh = (int32_t)(rc.yoff2 - rc.yoff + 0.5f);
            if (gw > 0 && gh > 0)
                Renderer->CompositeTinted(atlas->texture,
                                          gx, gy, gw, gh,           // dst = logical glyph size
                                          target_w, target_h,
                                          rc.x, rc.y, rc.w, rc.h,   // src = 2x oversampled
                                          atlas->width, atlas->height,
                                          r, g, b, 1.0f);
        }
        penX += rc.xadvance;
    }
}

// Pen-x for the given alignment of `text` within [cellX, cellX+cellW).
float AlignedPenX(const SFontAtlas* atlas, const char* text,
                  int32_t cellX, int32_t cellW, ETextAlign align)
{
    if (align == ETextAlign::Left) return (float)cellX;
    const float tw = TextWidth(atlas, text);
    if (align == ETextAlign::Center) return cellX + (cellW - tw) * 0.5f;
    return cellX + (cellW - tw);   // Right
}

} // namespace

void DrawTextAtBaseline(const SFontAtlas* atlas, const char* text,
                        float penX, float baselineY,
                        float r, float g, float b,
                        int32_t target_w, int32_t target_h)
{
    if (!atlas || atlas->texture == kInvalidTexture || !text || !*text) return;
    DrawGlyphRun(atlas, text, penX, baselineY, r, g, b, target_w, target_h);
}

// Retail drew UI text with GDI DrawTextA (DT_TOP). GDI's DT_TOP hangs glyphs
// from the cell top using the font's internal-leading, which sits the visible
// strokes ~2px higher than a naive `cellTop + ascent` baseline. This is a GDI
// rendering artifact the recon/spec can't see — we reproduce it once here so
// every panel's text matches retail without each spec carrying a font metric.
// (See NOMENCLATURE §2 "baseline".)
constexpr float kGdiTopLeading = 2.0f;

// The retail font drew each colored glyph twice at the same position to lift the
// thin antialiased coverage of small glyphs up to a solid read (two alpha-over
// passes: 0.5 coverage -> 0.75). We keep that double pass in the canonical
// helpers so every panel's text matches retail's weight.
static void DrawGlyphRunDoubled(const SFontAtlas* atlas, const char* text,
                                float penX, float baselineY,
                                float r, float g, float b,
                                int32_t target_w, int32_t target_h)
{
    DrawGlyphRun(atlas, text, penX, baselineY, r, g, b, target_w, target_h);
    DrawGlyphRun(atlas, text, penX, baselineY, r, g, b, target_w, target_h);
}

void DrawTextToTarget(const SFontAtlas* atlas, const char* text,
                      int32_t cellX, int32_t cellY, int32_t cellW, int32_t cellH,
                      ETextAlign align, float r, float g, float b,
                      int32_t target_w, int32_t target_h)
{
    (void)cellH;
    if (!atlas || atlas->texture == kInvalidTexture || !text || !*text) return;
    const float penX = AlignedPenX(atlas, text, cellX, cellW, align);
    const float baselineY = cellY + TextAscent(atlas) - kGdiTopLeading;
    DrawGlyphRunDoubled(atlas, text, penX, baselineY, r, g, b, target_w, target_h);
}

void DrawTextShadowedToTarget(const SFontAtlas* atlas, const char* text,
                              int32_t cellX, int32_t cellY, int32_t cellW, int32_t cellH,
                              ETextAlign align, float r, float g, float b,
                              int32_t target_w, int32_t target_h)
{
    (void)cellH;
    if (!atlas || atlas->texture == kInvalidTexture || !text || !*text) return;
    const float penX = AlignedPenX(atlas, text, cellX, cellW, align);
    const float baselineY = cellY + TextAscent(atlas) - kGdiTopLeading;
    // Retail FUN_004be2b0 font-flag-0x400: 3 black passes (base, +1x, +1y) then
    // the colored (doubled) pass at base. 1px shadow on the right + bottom.
    DrawGlyphRun(atlas, text, penX,        baselineY,        0, 0, 0, target_w, target_h);
    DrawGlyphRun(atlas, text, penX + 1.0f, baselineY,        0, 0, 0, target_w, target_h);
    DrawGlyphRun(atlas, text, penX,        baselineY + 1.0f, 0, 0, 0, target_w, target_h);
    DrawGlyphRunDoubled(atlas, text, penX,  baselineY,        r, g, b, target_w, target_h);
}
