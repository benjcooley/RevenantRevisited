// *************************************************************************
// *                         Cinematix Revenant                            *
// *                                                                        *
// *   ttfatlas.cpp — TrueType font atlas builder using stb_truetype        *
// *                                                                        *
// *   Produces an SFontAtlas from a TTF file path so WINFONT entries in    *
// *   FONT.DEF (currently falling back to nearest bitmap face) can render  *
// *   through the same per-glyph Composite path as BMFONT atoms.           *
// *************************************************************************

#include "font.h"

#include "logging.h"
#include "renderer.h"

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include "stb_truetype.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <unordered_map>

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

uint8_t* ReadWholeFile(const char* path, size_t* out_size)
{
    FILE* f = fopen(path, "rb");
    if (!f) return nullptr;
    fseek(f, 0, SEEK_END);
    const long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return nullptr; }

    uint8_t* buf = (uint8_t*)std::malloc((size_t)sz);
    if (!buf) { fclose(f); return nullptr; }
    if (fread(buf, 1, (size_t)sz, f) != (size_t)sz)
    {
        std::free(buf);
        fclose(f);
        return nullptr;
    }
    fclose(f);
    if (out_size) *out_size = (size_t)sz;
    return buf;
}

} // namespace

const SFontAtlas* BuildTTFAtlas(const char* path, int pixel_height)
{
    if (!path || pixel_height <= 0) return nullptr;

    const std::string key = MakeKey(path, pixel_height);
    if (auto it = g_ttfAtlases.find(key); it != g_ttfAtlases.end())
        return it->second;

    size_t file_size = 0;
    uint8_t* ttf = ReadWholeFile(path, &file_size);
    if (!ttf)
    {
        log_error("[ttf] could not read '%s'", path);
        return nullptr;
    }

    // Atlas dims: 512 wide, height scaled by font size so tall fonts get
    // more room to pack. stbtt_PackBegin wants a single-channel scratch.
    constexpr int kFirst = 32;
    constexpr int kCount = 96;
    // At 2x oversampling each glyph takes ~2x the raw pixel area, so bump
    // dims aggressively once we're past ~32px — Times @48px didn't fit in
    // 512² and stbtt_PackFontRange returned 0 for every glyph.
    const int atlas_w = pixel_height >= 32 ? 1024 : 512;
    const int atlas_h = (pixel_height < 16 ? 128
                        : pixel_height < 32 ? 256
                        : pixel_height < 48 ? 512
                        : 1024);

    std::unique_ptr<uint8_t[]> coverage(new uint8_t[atlas_w * atlas_h]);
    std::memset(coverage.get(), 0, size_t(atlas_w) * atlas_h);

    std::unique_ptr<stbtt_packedchar[]> chardata(new stbtt_packedchar[kCount]);

    stbtt_pack_context spc;
    if (!stbtt_PackBegin(&spc, coverage.get(), atlas_w, atlas_h, 0, 1, nullptr))
    {
        log_error("[ttf] stbtt_PackBegin failed for '%s'", path);
        std::free(ttf);
        return nullptr;
    }
    stbtt_PackSetOversampling(&spc, 2, 2);

    if (!stbtt_PackFontRange(&spc, ttf, 0, (float)pixel_height,
                              kFirst, kCount, chardata.get()))
    {
        log_error("[ttf] stbtt_PackFontRange failed for '%s' @%dpx",
            path, pixel_height);
        stbtt_PackEnd(&spc);
        std::free(ttf);
        return nullptr;
    }
    stbtt_PackEnd(&spc);

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

    // Translate stb's packed quads into our SFontAtlas rect table. We only
    // need the pixel rect inside the atlas — the draw code computes dst_xy
    // from pen + glyph metrics on the fly.
    auto atlas = std::make_unique<SFontAtlas>();
    atlas->firstchar = (int16_t)kFirst;
    atlas->numchars  = (int16_t)kCount;
    atlas->width     = atlas_w;
    atlas->height    = atlas_h;
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
    for (int i = 0; i < kCount; i++)
    {
        const stbtt_packedchar& pc = chardata[i];
        auto& r = atlas->rects[i];
        r.x = (uint16_t)pc.x0;
        r.y = (uint16_t)pc.y0;
        r.w = (uint16_t)(pc.x1 - pc.x0);
        r.h = (uint16_t)(pc.y1 - pc.y0);
        r.xoff     = roundf(pc.xoff);
        r.yoff     = roundf(pc.yoff);
        r.xoff2    = roundf(pc.xoff2);
        r.yoff2    = roundf(pc.yoff2);
        r.xadvance = roundf(pc.xadvance);
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

    std::free(ttf);

    if (atlas->texture == kInvalidTexture)
    {
        log_error("[ttf] texture upload failed for '%s'", path);
        return nullptr;
    }

    log_info("[ttf] atlas built: %dx%d for '%s' @%dpx", atlas_w, atlas_h,
        path, pixel_height);

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
