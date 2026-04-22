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

    sg_image_desc d = {};
    d.width        = atlas->width;
    d.height       = atlas->height;
    d.pixel_format = SG_PIXELFORMAT_RGBA8;
    d.min_filter   = SG_FILTER_NEAREST;
    d.mag_filter   = SG_FILTER_NEAREST;
    d.wrap_u       = SG_WRAP_CLAMP_TO_EDGE;
    d.wrap_v       = SG_WRAP_CLAMP_TO_EDGE;
    d.data.subimage[0][0].ptr  = rgba;
    d.data.subimage[0][0].size = bytes;
    d.label = "font.atlas";
    atlas->image = sg_make_image(&d);

    std::free(rgba);

    if (!atlas->image.id)
    {
        log_error("[font] BuildFontAtlas: sg_make_image failed");
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
        if (atlas->image.id)
            sg_destroy_image(atlas->image);
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
