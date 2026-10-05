// *************************************************************************
// *                         Cinematix Revenant                            *
// *                                                                        *
// *   ttfatlas.cpp — TrueType font atlas builder using stb_truetype        *
// *                                                                        *
// *   Produces an SFontAtlas from a TTF file path so WINFONT entries in    *
// *   FONT.DEF render through the same per-glyph Composite path as BMFONT  *
// *   atoms (docs/ui/TEXT_RENDERING.md).                                   *
// *************************************************************************

#include "font.h"

#include "logging.h"
#include "renderer.h"
#include "revutils.h"   // rev_engine_asset
#include "textencoding.h"

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include "stb_truetype.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

// stb_truetype cache: keyed by "path|pixel_height" so multiple sizes of the
// same TTF don't re-rasterize. SFontAtlas lifetime is tied to shutdown via
// DestroyAllTTFAtlases (called from the same place as the bitmap variant).
std::unordered_map<std::string, SFontAtlas*> g_ttfAtlases;

std::string MakeKey(const char* path, int pixel_height)
{
    char buf[512];
    snprintf(buf, sizeof(buf), "%s|%d", path ? path : "", pixel_height);
    return std::string(buf);
}

// The file's bytes; empty when it can't be read.
std::vector<uint8_t> ReadWholeFile(const char* path)
{
    std::vector<uint8_t> bytes;
    FILE* f = fopen(path, "rb");
    if (!f) return bytes;
    fseek(f, 0, SEEK_END);
    const long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (sz > 0)
    {
        bytes.resize(size_t(sz));
        if (fread(bytes.data(), 1, bytes.size(), f) != bytes.size())
            bytes.clear();
    }
    fclose(f);
    return bytes;
}

// The glyphs are rasterized 2x2 oversampled with a pixel of padding.
constexpr int kOversample = 2;
constexpr int kPadding    = 1;
constexpr int kMaxAtlasDim = 4096;

// stb packs the face's .notdef (glyph 0) for a code point the face doesn't
// map. U+FFFF is a noncharacter, which no face maps, so asking for it packs
// .notdef: the atlas's fallback glyph.
constexpr int kNotdefCodepoint = 0xFFFF;

// Every printable character the CP1252 decode yields (textencoding.h):
// U+0020..U+007E, CP1252's specials in 0x80..0x9F and U+00A0..U+00FF. DEL and
// the five bytes CP1252 leaves undefined decode to controls and draw the
// fallback.
std::vector<int> Cp1252Repertoire()
{
    std::vector<int> codepoints;
    for (int byte = 0x20; byte <= 0xFF; ++byte)
    {
        const char32_t cp = Cp1252ToUnicode(uint8_t(byte));
        if (cp == 0x7F || (cp >= 0x80 && cp <= 0x9F))
            continue;
        codepoints.push_back(int(cp));
    }
    return codepoints;
}

// Pixels the glyphs of `codepoints` take at `scale`: each one's oversampled
// box plus padding, as stbtt_PackFontRangesGatherRects sizes it.
size_t PackedArea(const stbtt_fontinfo& info, const std::vector<int>& codepoints, float scale)
{
    size_t area = 0;
    for (const int cp : codepoints)
    {
        int x0 = 0, y0 = 0, x1 = 0, y1 = 0;
        stbtt_GetGlyphBitmapBoxSubpixel(&info, stbtt_FindGlyphIndex(&info, cp),
                                        scale * kOversample, scale * kOversample, 0.0f, 0.0f,
                                        &x0, &y0, &x1, &y1);
        area += size_t(x1 - x0 + kPadding + kOversample - 1)
              * size_t(y1 - y0 + kPadding + kOversample - 1);
    }
    return area;
}

} // namespace

std::string TTFFilePath(const char* file)
{
    // The directory is resolved once: TDefPane::FontFor asks for its face on
    // every text draw.
    static const std::string fonts_dir = rev_engine_asset("fonts");
    if (fonts_dir.empty() || !file || !*file)
        return {};
    return fonts_dir + "/" + file;
}

const SFontAtlas* BuildTTFAtlas(const char* path, int pixel_height)
{
    if (!path || pixel_height <= 0) return nullptr;

    const std::string key = MakeKey(path, pixel_height);
    if (auto it = g_ttfAtlases.find(key); it != g_ttfAtlases.end())
        return it->second;

    // A face that fails once fails every time: remember the failure, so a
    // draw that asks for it each frame doesn't retry and log again.
    auto fail = [&key]() -> const SFontAtlas* {
        g_ttfAtlases.emplace(key, nullptr);
        return nullptr;
    };

    const std::vector<uint8_t> ttf = ReadWholeFile(path);
    stbtt_fontinfo info;
    if (ttf.empty() || !stbtt_InitFont(&info, ttf.data(), stbtt_GetFontOffsetForIndex(ttf.data(), 0)))
    {
        log_error("[ttf] could not read '%s'", path);
        return fail();
    }

    // Glyph 0, the fallback, is the face's .notdef; then every character
    // the CP1252 decode yields that the face maps. One the face lacks draws
    // the fallback.
    std::vector<int> codepoints{ kNotdefCodepoint };
    if (stbtt_FindGlyphIndex(&info, kNotdefCodepoint) != 0)
        log_warn("[ttf] '%s' maps U+FFFF: its glyph, not .notdef, is the fallback", path);
    for (const int cp : Cp1252Repertoire())
    {
        if (stbtt_FindGlyphIndex(&info, cp) != 0)
            codepoints.push_back(cp);
        else
            log_warn("[ttf] '%s' has no glyph for U+%04X: it draws .notdef", path, cp);
    }

    // Atlas dims: 512 wide (wider for a big face), and the least power-of-two
    // height that holds 1.6 times the glyphs' packed area. stb's built-in row
    // packer fills 60-70% of an atlas; at 1.6 Arimo and Tinos pack first time
    // at every size from 8 to 64 px. Should stb still not fit every glyph,
    // grow and pack again.
    const float scale = stbtt_ScaleForPixelHeight(&info, float(pixel_height));
    const size_t needed = PackedArea(info, codepoints, scale) * 8 / 5;
    int atlas_w = 512;
    while (size_t(atlas_w) * atlas_w < needed && atlas_w < kMaxAtlasDim)
        atlas_w *= 2;
    int atlas_h = 64;
    while (size_t(atlas_w) * atlas_h < needed && atlas_h < kMaxAtlasDim)
        atlas_h *= 2;

    // stbtt_PackBegin wants a single-channel scratch.
    std::vector<uint8_t> coverage;
    std::vector<stbtt_packedchar> chardata(codepoints.size());
    for (;;)
    {
        coverage.assign(size_t(atlas_w) * atlas_h, 0);
        stbtt_pack_context spc;
        if (!stbtt_PackBegin(&spc, coverage.data(), atlas_w, atlas_h, 0, kPadding, nullptr))
        {
            log_error("[ttf] stbtt_PackBegin failed for '%s'", path);
            return fail();
        }
        stbtt_PackSetOversampling(&spc, kOversample, kOversample);
        stbtt_pack_range range = {};
        range.font_size = float(pixel_height);
        range.array_of_unicode_codepoints = codepoints.data();
        range.num_chars = int(codepoints.size());
        range.chardata_for_range = chardata.data();
        const bool packed = stbtt_PackFontRanges(&spc, ttf.data(), 0, &range, 1) != 0;
        stbtt_PackEnd(&spc);
        if (packed)
            break;
        if (atlas_w >= kMaxAtlasDim && atlas_h >= kMaxAtlasDim)
        {
            log_error("[ttf] '%s' @%dpx doesn't fit a %dx%d atlas", path, pixel_height,
                      kMaxAtlasDim, kMaxAtlasDim);
            return fail();
        }
        log_warn("[ttf] '%s' @%dpx didn't fit %dx%d; growing", path, pixel_height, atlas_w, atlas_h);
        if (atlas_h < atlas_w)
            atlas_h *= 2;
        else
            atlas_w *= 2;
    }

    // Promote the single-channel coverage atlas to straight-alpha RGBA8.
    // RGB stays white; alpha carries coverage. The composite pipeline uses
    // SRC_ALPHA blending, so putting coverage in RGB as well would square the
    // coverage and make tinted text too dim.
    const size_t rgba_bytes = size_t(atlas_w) * atlas_h * 4;
    std::unique_ptr<uint8_t[]> rgba(new uint8_t[rgba_bytes]);
    for (size_t i = 0; i < size_t(atlas_w) * atlas_h; i++)
    {
        const uint8_t a = coverage[i];
        rgba[i * 4 + 0] = 255;
        rgba[i * 4 + 1] = 255;
        rgba[i * 4 + 2] = 255;
        rgba[i * 4 + 3] = a;
    }

    // Translate stb's packed quads into our SFontAtlas glyph table. We only
    // need the pixel rect inside the atlas — the draw code computes dst_xy
    // from pen + glyph metrics on the fly.
    auto atlas = std::make_unique<SFontAtlas>();
    atlas->width     = atlas_w;
    atlas->height    = atlas_h;
    atlas->glyphs.reserve(codepoints.size());
    // Stash stb's per-glyph placement so the draw side doesn't need to
    // reach back into stb_truetype. xoff/yoff are the offsets from pen
    // (yoff is negative for ascenders — pen_y is the baseline), xadvance
    // is the pen advance after the glyph.
    //
    // Round every metric to the integer pixel grid. stb's metrics are
    // fractional, so snapping each glyph to whole pixels at draw time from a
    // fractional running pen makes inter-glyph gaps drift ±1px (uneven spacing)
    // and rounds glyph tops/bottoms onto different rows (lowercase bobbing off
    // the baseline). Quantizing the metrics here makes this small UI font
    // behave like the original bitmap fonts — even spacing, one flat baseline —
    // which is what the pixel-faithful UI needs. (The atlas bitmap stays 2x
    // oversampled for crisp downsampled coverage; only placement is gridded.)
    //
    // The line metrics (ascent, descent) come from the printable ASCII glyphs
    // alone, as before the atlas held more: an accented capital rises into
    // GDI's internal leading (kGdiTopLeading in font.cpp) as in retail rather
    // than moving every baseline down.
    for (size_t i = 0; i < codepoints.size(); i++)
    {
        const stbtt_packedchar& pc = chardata[i];
        SFontAtlasRect r;
        r.x = (uint16_t)pc.x0;
        r.y = (uint16_t)pc.y0;
        r.w = (uint16_t)(pc.x1 - pc.x0);
        r.h = (uint16_t)(pc.y1 - pc.y0);
        r.xoff     = roundf(pc.xoff);
        r.yoff     = roundf(pc.yoff);
        r.xoff2    = roundf(pc.xoff2);
        r.yoff2    = roundf(pc.yoff2);
        r.xadvance = roundf(pc.xadvance);

        const char32_t cp = char32_t(codepoints[i]);
        if (cp >= 0x20 && cp <= 0x7E && r.w > 0 && r.h > 0)
        {
            atlas->ascent  = (std::max)(atlas->ascent, -r.yoff);
            atlas->descent = (std::max)(atlas->descent, r.yoff2);
        }
        if (i != SFontAtlas::kFallbackGlyph)
            atlas->MapGlyph(cp, uint16_t(i));
        atlas->glyphs.push_back(r);
    }

    if (Renderer)
    {
        atlas->texture = Renderer->RegisterTextureAsset(0,
                                                        rgba.get(),
                                                        rgba_bytes,
                                                        atlas_w,
                                                        atlas_h,
                                                        ERendererTextureFormat::RGBA8,
                                                        rgba_bytes,
                                                        ERendererTextureFilter::Linear);
        if (atlas->texture != kInvalidTexture)
            Renderer->AddTextureAssetRef(atlas->texture);
    }

    if (atlas->texture == kInvalidTexture)
    {
        log_error("[ttf] texture upload failed for '%s'", path);
        return fail();
    }

    log_info("[ttf] atlas built: %dx%d, %d glyphs + .notdef, for '%s' @%dpx (ascent %.0f, descent %.0f)",
        atlas_w, atlas_h, int(codepoints.size()) - 1, path, pixel_height,
        atlas->ascent, atlas->descent);

    SFontAtlas* raw = atlas.release();
    g_ttfAtlases.emplace(key, raw);
    return raw;
}

void DestroyAllTTFAtlases()
{
    for (auto& [k, a] : g_ttfAtlases)
    {
        if (!a) continue;
        if (Renderer && a->texture != kInvalidTexture)
            Renderer->ReleaseTextureAssetRef(a->texture);
        delete a;
    }
    g_ttfAtlases.clear();
}
