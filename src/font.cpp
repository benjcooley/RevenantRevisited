// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 font.cpp - EXILE Font Objects File                    *
// *************************************************************************

#include "font.h"

#include "bitmap.h"
#include "bitmapdecode.h"
#include "fonttable.h"
#include "graphics.h"
#include "logging.h"
#include "renderer.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <unordered_map>

PTBitmap TFont::GetChar(unsigned char ch)
{
    if (ch < firstchar || ch >= firstchar + numchars)
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

// Orders SFontAtlas's code point table (sorted by code point) for lower_bound.
bool CodepointBefore(const std::pair<char32_t, uint16_t>& entry, char32_t cp)
{
    return entry.first < cp;
}

} // namespace

const SFontAtlasRect& SFontAtlas::Glyph(char32_t cp) const
{
    uint16_t index = kFallbackGlyph;
    if (cp < latin.size())
        index = latin[cp];
    else
    {
        const auto it = std::lower_bound(beyond.begin(), beyond.end(), cp, CodepointBefore);
        if (it != beyond.end() && it->first == cp)
            index = it->second;
    }
    if (index < glyphs.size())
        return glyphs[index];
    static const SFontAtlasRect kNoGlyph;
    return glyphs.empty() ? kNoGlyph : glyphs[kFallbackGlyph];
}

void SFontAtlas::MapGlyph(char32_t cp, uint16_t index)
{
    if (cp < latin.size())
    {
        latin[cp] = index;
        return;
    }
    const auto it = std::lower_bound(beyond.begin(), beyond.end(), cp, CodepointBefore);
    if (it != beyond.end() && it->first == cp)
        it->second = index;
    else
        beyond.insert(it, { cp, index });
}

const SFontAtlas* BuildFontAtlas(TFont* font, const char* face)
{
    if (!font) return nullptr;

    if (SFontAtlas* existing = LookupAtlas(font))
    {
        if (face && existing->face.empty())
            existing->face = face;
        return existing;
    }

    const int32_t first = font->FirstChar();
    const int32_t count = font->Numchars();
    if (count <= 0)
        return nullptr;

    // Pass 1: shelf-pack into a 512-wide atlas to determine final height.
    constexpr int32_t kAtlasWidth   = 512;
    constexpr int32_t kGlyphPadding = 1;

    auto atlas = std::make_unique<SFontAtlas>();
    atlas->width     = kAtlasWidth;
    // Glyph 0, the fallback, is empty with no advance: retail's TFont drew
    // nothing for a byte outside the font.
    atlas->glyphs.reserve(size_t(count) + 1);
    atlas->glyphs.emplace_back();

    int32_t cursor_x = 0;
    int32_t shelf_y  = 0;
    int32_t shelf_h  = 0;

    for (int32_t i = 0; i < count; i++)
    {
        const unsigned char ch = (unsigned char)(first + i);
        PTBitmap bm = font->GetChar(ch);
        if (!bm || bm->width <= 0 || bm->height <= 0)
            continue;       // draws the fallback: nothing, like retail
        const int32_t gw = bm->width;
        const int32_t gh = bm->height;

        if (cursor_x + gw > kAtlasWidth)
        {
            shelf_y += shelf_h + kGlyphPadding;
            cursor_x = 0;
            shelf_h  = 0;
        }

        SFontAtlasRect r;
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
        r.xoff     = -(float)font->DrawLeft(ch);
        r.yoff     = -(float)font->StartHeight(ch);
        r.xoff2    = r.xoff + (float)gw;
        r.yoff2    = r.yoff + (float)gh;
        r.xadvance = (float)(font->DrawRight(ch) - font->DrawLeft(ch));
        atlas->ascent  = (std::max)(atlas->ascent, -r.yoff);
        atlas->descent = (std::max)(atlas->descent, r.yoff2);

        // A bitmap font is indexed by byte, as retail drew it. Under the
        // byte's CP1252 code point, the game text's CP1252 decode brings
        // every byte back to its own glyph.
        atlas->MapGlyph(Cp1252ToUnicode(ch), (uint16_t)atlas->glyphs.size());
        atlas->glyphs.push_back(r);

        cursor_x += gw + kGlyphPadding;
        if (gh > shelf_h) shelf_h = gh;
    }

    // Retail's font table takes a bitmap font's ascent from its 'A'
    // (0x004ac990; GDI's tmAscent for a WINFONT), and text drawn in a cell
    // hangs from it.
    atlas->cellBaseline = ('A' >= first && 'A' < first + count) ? (float)font->StartHeight('A') : atlas->ascent;
    atlas->lineStep = atlas->ascent + atlas->descent;
    atlas->shadow = RetailTextShadow(atlas->lineStep);
    if (face)
        atlas->face = face;
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
        const unsigned char ch = (unsigned char)(first + i);
        const auto& r = atlas->Glyph(Cp1252ToUnicode(ch));
        if (r.w == 0 || r.h == 0) continue;
        PTBitmap bm = font->GetChar(ch);
        if (!bm) continue;
        // As retail draws a glyph: keyed, with its anti-aliased edge
        // (FONT.DEF's default DM_TRANSPARENT | DM_ALIAS).
        DecodeBitmapToRGBA(bm, rgba, (int32_t)pitch, r.x, r.y, EBitmapDecode::PixelsAndAlias);
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
        atlas->width, atlas->height, (int)atlas->glyphs.size() - 1, first);

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

namespace {

// The one walk over a string's glyphs: decode each character of `text` per
// `encoding` and hand `fn` the glyph that draws it (the atlas's fallback for
// one it doesn't hold). Measuring, wrapping and drawing all step through here.
template <typename Fn>
void ForEachGlyph(const SFontAtlas& atlas, const char* text, ETextEncoding encoding, Fn&& fn)
{
    for (const char* p = text; *p;)
        fn(atlas.Glyph(DecodeChar(p, encoding)));
}

} // namespace

float TextWidth(const SFontAtlas* atlas, const char* text, ETextEncoding encoding)
{
    if (!atlas || !text) return 0.0f;
    float w = 0.0f;
    ForEachGlyph(*atlas, text, encoding, [&w](const SFontAtlasRect& rc) { w += rc.xadvance; });
    return w;
}

float TextAscent(const SFontAtlas* atlas)
{
    // How far the glyphs rise above the baseline: max(-yoff) over the glyphs
    // the builder measures (all of a bitmap font, the ASCII of a TrueType one).
    return atlas ? atlas->ascent : 0.0f;
}

float TextCellBaseline(const SFontAtlas* atlas)
{
    return atlas ? atlas->cellBaseline : 0.0f;
}

float TextLineHeight(const SFontAtlas* atlas)
{
    // Ascent + descent; descent is max(yoff2), how far glyphs drop below it.
    return atlas ? atlas->ascent + atlas->descent : 0.0f;
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
void DrawGlyphRun(const SFontAtlas* atlas, const char* text, ETextEncoding encoding,
                  float penX, float baselineY,
                  float r, float g, float b,
                  int32_t target_w, int32_t target_h)
{
    if (!Renderer) return;
    ForEachGlyph(*atlas, text, encoding, [&](const SFontAtlasRect& rc) {
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
    });
}

// Pen-x for the given alignment of `text` within [cellX, cellX+cellW).
float AlignedPenX(const SFontAtlas* atlas, const char* text, ETextEncoding encoding,
                  int32_t cellX, int32_t cellW, ETextAlign align)
{
    if (align == ETextAlign::Left) return (float)cellX;
    const float tw = TextWidth(atlas, text, encoding);
    if (align == ETextAlign::Center) return cellX + (cellW - tw) * 0.5f;
    return cellX + (cellW - tw);   // Right
}

// The coloured pass of the cell helpers: once, as retail blits a bitmap
// font's glyphs; twice over for an atlas that asks (SFontAtlas::doubleStrike).
void DrawColouredRun(const SFontAtlas* atlas, const char* text, ETextEncoding encoding,
                     float penX, float baselineY,
                     float r, float g, float b,
                     int32_t target_w, int32_t target_h)
{
    DrawGlyphRun(atlas, text, encoding, penX, baselineY, r, g, b, target_w, target_h);
    if (atlas->doubleStrike)
        DrawGlyphRun(atlas, text, encoding, penX, baselineY, r, g, b, target_w, target_h);
}

// The shadowed text with its pen at (penX, baselineY).
void DrawShadowedRun(const SFontAtlas* atlas, const char* text, ETextEncoding encoding,
                     float penX, float baselineY, float r, float g, float b,
                     int32_t target_w, int32_t target_h)
{
    // Retail 0x004be2b0 with flag 0x400 draws the string black at (dx, dy),
    // (dx + 1, dy) and (dx, dy + 1) -- (1, 1) for a 12 px font -- then in its
    // colour at the pen (STextShadow; measured from retail's own DrawTextA
    // calls in the emulator, docs/ui/HUD_REBUILD.md P1b).
    const float dx = atlas->shadow.dx, dy = atlas->shadow.dy;
    DrawGlyphRun(atlas, text, encoding, penX + dx, baselineY + dy, 0, 0, 0, target_w, target_h);
    if (!atlas->shadow.single)
    {
        DrawGlyphRun(atlas, text, encoding, penX + dx + 1.0f, baselineY + dy, 0, 0, 0, target_w, target_h);
        DrawGlyphRun(atlas, text, encoding, penX + dx, baselineY + dy + 1.0f, 0, 0, 0, target_w, target_h);
    }
    DrawColouredRun(atlas, text, encoding, penX, baselineY, r, g, b, target_w, target_h);
}

std::vector<STextDraw>* g_textDrawRecorder = nullptr;

// One public draw, as the A/B compares it with retail's text calls.
void RecordTextDraw(const SFontAtlas* atlas, const char* text, int32_t x, int32_t y, int32_t w, int32_t h,
                    ETextAlign align, float r, float g, float b, bool shadowed, bool baseline, bool wrapped,
                    std::vector<std::string> lines = {})
{
    if (!g_textDrawRecorder)
        return;
    const auto byte = [](float c) { return static_cast<uint8_t>(std::clamp(c, 0.0f, 1.0f) * 255.0f + 0.5f); };
    g_textDrawRecorder->push_back({ text, atlas->face, x, y, w, h, align, byte(r), byte(g), byte(b), shadowed,
                                    baseline, wrapped, std::move(lines) });
}

} // namespace

void SetTextDrawRecorder(std::vector<STextDraw>* recorder)
{
    g_textDrawRecorder = recorder;
}

void DrawTextAtBaseline(const SFontAtlas* atlas, const char* text,
                        float penX, float baselineY,
                        float r, float g, float b,
                        int32_t target_w, int32_t target_h,
                        ETextEncoding encoding)
{
    if (!atlas || atlas->texture == kInvalidTexture || !text || !*text) return;
    RecordTextDraw(atlas, text, int32_t(penX), int32_t(baselineY), 0, 0, ETextAlign::Left, r, g, b, false, true,
                   false);
    DrawGlyphRun(atlas, text, encoding, penX, baselineY, r, g, b, target_w, target_h);
}

void DrawTextToTarget(const SFontAtlas* atlas, const char* text,
                      int32_t cellX, int32_t cellY, int32_t cellW, int32_t cellH,
                      ETextAlign align, float r, float g, float b,
                      int32_t target_w, int32_t target_h,
                      ETextEncoding encoding)
{
    if (!atlas || atlas->texture == kInvalidTexture || !text || !*text) return;
    RecordTextDraw(atlas, text, cellX, cellY, cellW, cellH, align, r, g, b, false, false, false);
    const float penX = AlignedPenX(atlas, text, encoding, cellX, cellW, align);
    const float baselineY = cellY + TextCellBaseline(atlas);
    DrawColouredRun(atlas, text, encoding, penX, baselineY, r, g, b, target_w, target_h);
}

void DrawTextShadowedToTarget(const SFontAtlas* atlas, const char* text,
                              int32_t cellX, int32_t cellY, int32_t cellW, int32_t cellH,
                              ETextAlign align, float r, float g, float b,
                              int32_t target_w, int32_t target_h,
                              ETextEncoding encoding)
{
    if (!atlas || atlas->texture == kInvalidTexture || !text || !*text) return;
    RecordTextDraw(atlas, text, cellX, cellY, cellW, cellH, align, r, g, b, true, false, false);
    const float penX = AlignedPenX(atlas, text, encoding, cellX, cellW, align);
    const float baselineY = cellY + TextCellBaseline(atlas);
    DrawShadowedRun(atlas, text, encoding, penX, baselineY, r, g, b, target_w, target_h);
}

void DrawTextShadowedAtBaseline(const SFontAtlas* atlas, const char* text,
                                float penX, float baselineY,
                                float r, float g, float b,
                                int32_t target_w, int32_t target_h,
                                ETextEncoding encoding)
{
    if (!atlas || atlas->texture == kInvalidTexture || !text || !*text) return;
    RecordTextDraw(atlas, text, int32_t(penX), int32_t(baselineY), 0, 0, ETextAlign::Left, r, g, b, true, true,
                   false);
    DrawShadowedRun(atlas, text, encoding, penX, baselineY, r, g, b, target_w, target_h);
}

// Retail's description box (DrawTextA, DT_WORDBREAK | DT_EDITCONTROL):
// word-wrapped to `w`, a line every lineStep, whole lines only.
int32_t DrawTextWrappedToTarget(const SFontAtlas* atlas, const char* text,
                                int32_t x, int32_t y, int32_t w, int32_t h,
                                ETextAlign align, float r, float g, float b, bool shadowed,
                                int32_t target_w, int32_t target_h, ETextEncoding encoding)
{
    if (!atlas || atlas->texture == kInvalidTexture || !text || !*text) return 0;
    std::vector<std::string> lines;
    WrapTextLines(atlas, text, float(w), lines, encoding);
    // DT_EDITCONTROL: the first line always, then whole lines only.
    const float step = atlas->lineStep > 0.0f ? atlas->lineStep : TextLineHeight(atlas);
    size_t shown = 1;
    while (shown < lines.size() && float(shown + 1) * step <= float(h))
        ++shown;
    lines.resize(shown);
    RecordTextDraw(atlas, text, x, y, w, h, align, r, g, b, shadowed, false, true, lines);
    for (size_t i = 0; i < lines.size(); ++i)
    {
        const float penX = AlignedPenX(atlas, lines[i].c_str(), encoding, x, w, align);
        const float baselineY = y + float(i) * step + TextCellBaseline(atlas);
        if (shadowed)
            DrawShadowedRun(atlas, lines[i].c_str(), encoding, penX, baselineY, r, g, b, target_w, target_h);
        else
            DrawColouredRun(atlas, lines[i].c_str(), encoding, penX, baselineY, r, g, b, target_w, target_h);
    }
    return static_cast<int32_t>(static_cast<float>(lines.size()) * step);
}

int32_t WrapTextLines(const SFontAtlas* atlas, const char* text, float wrapWidth,
                      std::vector<std::string>& lines, ETextEncoding encoding)
{
    const size_t first = lines.size();
    // Control characters other than '\n' measure as spaces (the atlas holds
    // printable glyphs only). The walk below works on bytes: ' ' and '\n'
    // never occur inside a multi-byte UTF-8 character, so it splits either
    // encoding cleanly.
    std::string src(text ? text : "");
    for (char& c : src)
        if ((unsigned char)c < ' ' && c != '\n')
            c = ' ';

    std::string line;
    float lineW = 0.0f;
    const char* p = src.c_str();
    for (;;)
    {
        const char* gapStart = p;
        while (*p == ' ')
            ++p;
        const std::string gap(gapStart, p);
        const char* wordStart = p;
        while (*p && *p != ' ' && *p != '\n')
            ++p;
        if (p > wordStart)
        {
            const std::string word(wordStart, p);
            const float wordW = TextWidth(atlas, word.c_str(), encoding);
            float gapW = line.empty() ? 0.0f : TextWidth(atlas, gap.c_str(), encoding);
            if (!line.empty() && lineW + gapW + wordW > wrapWidth)
            {
                lines.push_back(line);
                line.clear();
                lineW = gapW = 0.0f;
            }
            if (!line.empty())
                line += gap;
            line += word;
            lineW += gapW + wordW;
        }
        if (*p == '\n')
        {
            lines.push_back(line);
            line.clear();
            lineW = 0.0f;
            ++p;
            continue;
        }
        if (!*p)
            break;
    }
    if (!line.empty() || lines.size() == first)
        lines.push_back(line);
    return (int32_t)(lines.size() - first);
}
