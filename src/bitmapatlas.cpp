// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                bitmapatlas.cpp - Bitmap atlas helpers                 *
// *************************************************************************

#include "bitmapatlas.h"

#include "animation.h"
#include "animimagebody.h"
#include "bitmap.h"
#include "bitmapdecode.h"
#include "imagery.h"
#include "imageres.h"
#include "logging.h"
#include "multi.h"
#include "renderer.h"
#include "revdefs.h"

#include <algorithm>
#include <cstring>
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace {

constexpr int32_t kBitmapAtlasMaxItems = 4096;
constexpr int32_t kBitmapAtlasMaxFrames = 16384;
constexpr int32_t kBitmapAtlasWidth = 4096;
constexpr int32_t kBitmapAtlasMaxHeight = 4096;
constexpr int32_t kBitmapAtlasPad = 2;
constexpr int32_t kUIAtlasSmallW = 2048;
constexpr int32_t kUIAtlasSmallH = 2048;
constexpr int32_t kUIAtlasPad = 2;
constexpr int32_t kUIAtlasSmallLimit = 128;
constexpr int32_t kUIAtlasStandaloneDim = 512;
constexpr int32_t kUIAtlasStandaloneAspect = 4;
constexpr int32_t kUIAtlasMaxDim = 4096;
constexpr int32_t kUIAtlasMaxAnimationFrames = 512;
constexpr uint64_t kFnvOffset64 = 1469598103934665603ull;
constexpr uint64_t kFnvPrime64 = 1099511628211ull;

struct SUIBitmapAtlasCandidate
{
    PTBitmap bm = nullptr;
    bool prefer_alias = false;
    uint64_t key = 0;
};

struct SUIBitmapAtlasPage
{
    TTextureHandle texture = kInvalidTexture;
    int32_t width = 0;
    int32_t height = 0;
    int32_t cursor_x = 0;
    int32_t shelf_y = 0;
    int32_t shelf_h = 0;
    std::vector<uint8_t> rgba;
};

std::vector<SUIBitmapAtlasCandidate> g_uiCandidates;
std::unordered_set<uint64_t> g_uiCandidateKeys;
std::vector<SUIBitmapAtlasPage> g_uiPages;
std::unordered_map<uint64_t, SBitmapAtlasSlice> g_uiSlices;
bool g_uiCollecting = false;
bool g_uiReady = false;

inline void HashU64(uint64_t& h, uint64_t v)
{
    h ^= v;
    h *= kFnvPrime64;
}

inline bool IsSmallSprite(PTBitmap bm)
{
    return bm && bm->width <= kUIAtlasSmallLimit && bm->height <= kUIAtlasSmallLimit;
}

inline bool ShouldStandaloneUIBitmap(PTBitmap bm)
{
    if (!bm) return false;
    const int32_t longest = (std::max)(bm->width, bm->height);
    const int32_t shortest = (std::max)(1, (std::min)(bm->width, bm->height));
    return longest > kUIAtlasStandaloneDim ||
           (longest > kUIAtlasSmallLimit && longest / shortest >= kUIAtlasStandaloneAspect);
}

inline bool IsUIBitmapAtlasDecodable(PTBitmap bm)
{
    if (!bm || bm->width <= 0 || bm->height <= 0) return false;
    if (bm->flags & BM_COMPRESSED) return false;
    if (bm->width > kUIAtlasMaxDim || bm->height > kUIAtlasMaxDim) return false;

    const uint32_t knownFormatFlags = BM_8BIT | BM_15BIT | BM_16BIT | 0x10000;
    if (!(bm->flags & knownFormatFlags)) return false;

    const uint64_t pixels = uint64_t(bm->width) * uint64_t(bm->height);
    if (pixels == 0 || pixels > uint64_t(kUIAtlasMaxDim) * uint64_t(kUIAtlasMaxDim))
        return false;

    if (bm->flags & BM_8BIT)
        return bm->datasize >= pixels && bm->palette.ptr() != nullptr;
    if (bm->flags & (BM_15BIT | BM_16BIT | 0x10000))
        return bm->datasize >= pixels * 2;
    return false;
}

inline bool IsUIBitmapAtlasAnimation(PTAnimation anim)
{
    if (!anim || anim->numframes <= 0 || anim->numframes > kUIAtlasMaxAnimationFrames)
        return false;
    if (anim->maxwidth <= 0 || anim->maxheight <= 0 ||
        anim->maxwidth > kUIAtlasMaxDim || anim->maxheight > kUIAtlasMaxDim)
    {
        return false;
    }

    for (int32_t i = 0; i < anim->numframes; ++i)
        if (IsUIBitmapAtlasDecodable(anim->GetFrame(i)))
            return true;
    return false;
}

void RegisterUIBitmapAtlasAnimation(PTAnimation anim)
{
    if (!IsUIBitmapAtlasAnimation(anim)) return;
    for (int32_t i = 0; i < anim->numframes; ++i)
        RegisterUIBitmapAtlasBitmap(anim->GetFrame(i));
}

bool TryPackOnPage(SUIBitmapAtlasPage& page, PTBitmap bm, int32_t& out_x, int32_t& out_y)
{
    if (!bm) return false;
    if (bm->width + kUIAtlasPad > page.width || bm->height + kUIAtlasPad > page.height)
        return false;

    if (page.cursor_x + bm->width + kUIAtlasPad > page.width)
    {
        page.shelf_y += page.shelf_h + kUIAtlasPad;
        page.cursor_x = 0;
        page.shelf_h = 0;
    }

    if (page.shelf_y + bm->height > page.height)
        return false;

    out_x = page.cursor_x;
    out_y = page.shelf_y;
    page.cursor_x += bm->width + kUIAtlasPad;
    if (bm->height > page.shelf_h)
        page.shelf_h = bm->height;
    return true;
}

int32_t CreateUIBitmapAtlasPage()
{
    SUIBitmapAtlasPage page;
    page.width = kUIAtlasSmallW;
    page.height = kUIAtlasSmallH;
    page.rgba.assign(size_t(page.width) * size_t(page.height) * 4, 0);
    g_uiPages.push_back(std::move(page));
    return int32_t(g_uiPages.size()) - 1;
}

bool BuildStandaloneUIBitmapAtlasSlice(const SUIBitmapAtlasCandidate& c)
{
    PTBitmap bm = c.bm;
    if (!IsUIBitmapAtlasDecodable(bm)) return false;

    std::vector<uint8_t> rgba(size_t(bm->width) * size_t(bm->height) * 4, 0);
    if (!DecodeBitmapToRGBA(bm, rgba.data(), bm->width * 4, 0, 0, c.prefer_alias))
        return false;

    const TTextureHandle tex = Renderer->RegisterTextureAsset(
        0, rgba.data(), rgba.size(),
        bm->width, bm->height,
        ERendererTextureFormat::RGBA8,
        uint64_t(rgba.size()),
        ERendererTextureFilter::Nearest);
    if (tex == kInvalidTexture)
        return false;
    Renderer->AddTextureAssetRef(tex);

    SUIBitmapAtlasPage page;
    page.texture = tex;
    page.width = bm->width;
    page.height = bm->height;
    const int32_t pageIdx = int32_t(g_uiPages.size());
    g_uiPages.push_back(std::move(page));

    SBitmapAtlasSlice slice;
    slice.texture = tex;
    slice.src_x = 0;
    slice.src_y = 0;
    slice.width = bm->width;
    slice.height = bm->height;
    slice.tex_width = bm->width;
    slice.tex_height = bm->height;
    slice.page_index = pageIdx;
    g_uiSlices[c.key] = slice;
    return true;
}

inline bool IsImageryBitmapPreviewable(PTBitmap bm)
{
    if (!bm || bm->width <= 0 || bm->height <= 0) return false;
    if (bm->flags & BM_COMPRESSED) return false;
    if (!(bm->flags & (BM_8BIT | BM_15BIT | BM_16BIT))) return false;
    if (bm->width > kBitmapAtlasWidth || bm->height > kBitmapAtlasMaxHeight) return false;
    return true;
}

inline bool Is2DImageryFilename(const char* filename)
{
    if (!filename || !filename[0]) return false;
    const size_t flen = strlen(filename);
    return (flen > 4 && (!stricmp(filename + flen - 4, ".I2D") ||
                         !stricmp(filename + flen - 3, ".TN")));
}

inline void ClearBitmapAtlasMetadata(SBitmapAtlas* atlas)
{
    if (!atlas) return;
    atlas->width = 0;
    atlas->height = 0;
    atlas->items.clear();
    atlas->frames.clear();
}

}  // namespace

uint64_t UIBitmapAtlasKey(PTBitmap bm, bool prefer_alias)
{
    uint64_t h = kFnvOffset64;
    HashU64(h, uintptr_t(bm));
    HashU64(h, prefer_alias ? 1u : 0u);
    if (!bm) return h;

    HashU64(h, uint32_t(bm->width));
    HashU64(h, uint32_t(bm->height));
    HashU64(h, uint32_t(bm->regx));
    HashU64(h, uint32_t(bm->regy));
    HashU64(h, uint32_t(bm->flags));
    HashU64(h, uint32_t(bm->drawmode));
    HashU64(h, uint32_t(bm->keycolor));
    HashU64(h, uint32_t(bm->datasize));
    HashU64(h, uint32_t(bm->aliassize));
    HashU64(h, uint32_t(bm->alphasize));
    HashU64(h, uint32_t(bm->palettesize));
    HashU64(h, uintptr_t(bm->data8));
    HashU64(h, uintptr_t(bm->alias.ptr()));
    HashU64(h, uintptr_t(bm->alpha.ptr()));
    HashU64(h, uintptr_t(bm->palette.ptr()));
    return h;
}

void ClearUIBitmapAtlas()
{
    if (Renderer)
    {
        for (SUIBitmapAtlasPage& page : g_uiPages)
            if (page.texture != kInvalidTexture)
                Renderer->ReleaseTextureAssetRef(page.texture);
    }
    g_uiPages.clear();
    g_uiSlices.clear();
    g_uiCandidates.clear();
    g_uiCandidateKeys.clear();
    g_uiCollecting = false;
    g_uiReady = false;
}

void BeginUIBitmapAtlasBuild()
{
    ClearUIBitmapAtlas();
    g_uiCollecting = true;
}

void RegisterUIBitmapAtlasBitmap(PTBitmap bm, bool prefer_alias)
{
    if (!g_uiCollecting || !IsUIBitmapAtlasDecodable(bm))
        return;

    const uint64_t key = UIBitmapAtlasKey(bm, prefer_alias);
    if (!g_uiCandidateKeys.insert(key).second)
        return;

    SUIBitmapAtlasCandidate c;
    c.bm = bm;
    c.prefer_alias = prefer_alias;
    c.key = key;
    g_uiCandidates.push_back(c);
}

void RegisterUIBitmapAtlasArchive(TMulti* multi)
{
    if (!g_uiCollecting || !multi) return;
    for (int32_t i = 0; i < multi->numoffsets; ++i)
    {
        void* obj = multi->Object(i);
        if (!obj) continue;

        // TMulti does not expose per-entry resource type metadata. HUD DAT
        // archives used by these panes are bitmap collections, so archive
        // registration only admits entries that validate as bitmaps. Animated
        // inventory/world icon frames are registered by the explicit imagery
        // pass below.
        if (IsUIBitmapAtlasDecodable((PTBitmap)obj))
            RegisterUIBitmapAtlasBitmap((PTBitmap)obj);
    }
}

int32_t RegisterUIBitmapAtlasInventoryImagery()
{
    if (!g_uiCollecting) return 0;

    int32_t registered = 0;
    const int32_t n = TObjectImagery::NumEntries();
    for (int32_t id = 0; id < n; ++id)
    {
        if (!TObjectImagery::IsUsed(id)) continue;
        TObjectImagery* img = TObjectImagery::LoadImagery(id);
        if (!img || img->NumStates() <= 0) continue;

        for (int32_t state = 0; state < img->NumStates(); ++state)
        {
            const size_t before = g_uiCandidates.size();
            RegisterUIBitmapAtlasBitmap(img->GetInvImage(state));
            if (TAnimation* anim = img->GetInvAnimation(state))
                RegisterUIBitmapAtlasAnimation(anim);
            registered += int32_t(g_uiCandidates.size() - before);
        }
    }
    log_info("[ui-atlas] registered %d inventory icon bitmap(s) from imagery", registered);
    return registered;
}

bool BuildUIBitmapAtlas()
{
    if (!Renderer) return false;
    g_uiReady = false;
    g_uiSlices.clear();
    g_uiPages.clear();

    std::stable_sort(
        g_uiCandidates.begin(), g_uiCandidates.end(),
        [](const SUIBitmapAtlasCandidate& a, const SUIBitmapAtlasCandidate& b)
        {
            const bool as = IsSmallSprite(a.bm) && !ShouldStandaloneUIBitmap(a.bm);
            const bool bs = IsSmallSprite(b.bm) && !ShouldStandaloneUIBitmap(b.bm);
            if (as != bs) return as > bs;
            const int32_t aa = a.bm ? a.bm->width * a.bm->height : 0;
            const int32_t ba = b.bm ? b.bm->width * b.bm->height : 0;
            return aa > ba;
        });

    int32_t packed = 0;
    int32_t skipped = 0;
    for (const SUIBitmapAtlasCandidate& c : g_uiCandidates)
    {
        PTBitmap bm = c.bm;
        if (!IsUIBitmapAtlasDecodable(bm))
        {
            ++skipped;
            continue;
        }

        if (ShouldStandaloneUIBitmap(bm))
        {
            if (BuildStandaloneUIBitmapAtlasSlice(c))
                ++packed;
            else
                ++skipped;
            continue;
        }

        int32_t pageIdx = -1;
        int32_t px = 0;
        int32_t py = 0;
        for (int32_t i = 0; i < int32_t(g_uiPages.size()); ++i)
        {
            if (g_uiPages[i].texture != kInvalidTexture) continue;
            if (TryPackOnPage(g_uiPages[i], bm, px, py))
            {
                pageIdx = i;
                break;
            }
        }
        if (pageIdx < 0)
        {
            pageIdx = CreateUIBitmapAtlasPage();
            if (!TryPackOnPage(g_uiPages[pageIdx], bm, px, py))
            {
                ++skipped;
                continue;
            }
        }

        SUIBitmapAtlasPage& page = g_uiPages[pageIdx];
        const int32_t pitch = page.width * 4;
        if (!DecodeBitmapToRGBA(bm, page.rgba.data(), pitch, px, py, c.prefer_alias))
        {
            ++skipped;
            continue;
        }

        SBitmapAtlasSlice slice;
        slice.src_x = px;
        slice.src_y = py;
        slice.width = bm->width;
        slice.height = bm->height;
        slice.tex_width = page.width;
        slice.tex_height = page.height;
        slice.page_index = pageIdx;
        g_uiSlices[c.key] = slice;
        ++packed;
    }

    for (int32_t pageIdx = 0; pageIdx < int32_t(g_uiPages.size()); ++pageIdx)
    {
        SUIBitmapAtlasPage& page = g_uiPages[pageIdx];
        if (page.texture != kInvalidTexture || page.rgba.empty())
            continue;

        const int32_t usedH = (std::max)(1, page.shelf_y + page.shelf_h);
        std::vector<uint8_t> upload(size_t(page.width) * size_t(usedH) * 4, 0);
        for (int32_t y = 0; y < usedH; ++y)
        {
            memcpy(upload.data() + size_t(y) * page.width * 4,
                   page.rgba.data() + size_t(y) * page.width * 4,
                   size_t(page.width) * 4);
        }
        page.height = usedH;
        page.rgba.clear();
        page.rgba.shrink_to_fit();

        page.texture = Renderer->RegisterTextureAsset(
            0, upload.data(), upload.size(),
            page.width, page.height,
            ERendererTextureFormat::RGBA8,
            uint64_t(upload.size()),
            ERendererTextureFilter::Nearest);
        if (page.texture != kInvalidTexture)
            Renderer->AddTextureAssetRef(page.texture);
    }

    for (auto& it : g_uiSlices)
    {
        SBitmapAtlasSlice& slice = it.second;
        if (slice.page_index >= 0 && slice.page_index < int32_t(g_uiPages.size()))
        {
            const SUIBitmapAtlasPage& page = g_uiPages[slice.page_index];
            slice.texture = page.texture;
            slice.tex_width = page.width;
            slice.tex_height = page.height;
        }
    }

    g_uiReady = packed > 0;
    g_uiCollecting = false;
    log_info("[ui-atlas] built %d bitmap(s), %d skipped, %zu page(s)",
             packed, skipped, g_uiPages.size());
    return g_uiReady;
}

bool LookupUIBitmapAtlasSlice(PTBitmap bm, bool prefer_alias, SBitmapAtlasSlice* out)
{
    if (!out || !g_uiReady || !bm) return false;
    auto it = g_uiSlices.find(UIBitmapAtlasKey(bm, prefer_alias));
    if (it == g_uiSlices.end()) return false;
    if (it->second.texture == kInvalidTexture) return false;
    *out = it->second;
    return true;
}

void DestroyBitmapAtlas(SBitmapAtlas* atlas)
{
    if (!atlas) return;
    if (Renderer && atlas->texture != kInvalidTexture)
        Renderer->ReleaseTextureAssetRef(atlas->texture);
    atlas->texture = kInvalidTexture;
    ClearBitmapAtlasMetadata(atlas);
}

bool BuildImageryBitmapAtlas(SBitmapAtlas* atlas)
{
    if (!atlas) return false;
    DestroyBitmapAtlas(atlas);

    const int32_t n = TObjectImagery::NumEntries();
    log_info("[ui] scanning %d registered imagery entries", n);

    std::vector<PTBitmap> bitmaps;
    bitmaps.reserve(kBitmapAtlasMaxFrames);

    int32_t cursor_x = 0;
    int32_t shelf_y = 0;
    int32_t shelf_h = 0;
    bool atlas_full = false;

    auto tryPushFrame = [&](PTBitmap bm, int16_t dx, int16_t dy,
                            int16_t regx, int16_t regy) -> bool
    {
        if (atlas_full) return false;
        if (!IsImageryBitmapPreviewable(bm)) return false;
        if (atlas->frames.size() >= size_t(kBitmapAtlasMaxFrames)) return false;

        if (cursor_x + bm->width + kBitmapAtlasPad > kBitmapAtlasWidth)
        {
            shelf_y += shelf_h + kBitmapAtlasPad;
            cursor_x = 0;
            shelf_h = 0;
        }
        if (shelf_y + bm->height > kBitmapAtlasMaxHeight)
        {
            atlas_full = true;
            return false;
        }

        SBitmapAtlasFrame frame;
        frame.ax = cursor_x;
        frame.ay = shelf_y;
        frame.w = bm->width;
        frame.h = bm->height;
        frame.dx = dx;
        frame.dy = dy;
        frame.regx = regx;
        frame.regy = regy;
        atlas->frames.push_back(frame);
        bitmaps.push_back(bm);

        cursor_x += bm->width + kBitmapAtlasPad;
        if (bm->height > shelf_h) shelf_h = bm->height;
        return true;
    };

    for (int32_t id = 0; id < n && atlas->items.size() < size_t(kBitmapAtlasMaxItems) && !atlas_full; id++)
    {
        if (!TObjectImagery::IsUsed(id)) continue;
        SImageryEntry* e = TObjectImagery::GetImageryEntry(id);
        if (!e || !Is2DImageryFilename(e->filename)) continue;

        TObjectImagery* img = TObjectImagery::LoadImagery(id);
        if (!img) continue;
        SImageryBody* body = img->GetBody();
        if (!body) continue;

        auto* ab = (SAnimImageryBody*)body;
        auto& st = ab->states[0];

        const int32_t firstFrame = int32_t(atlas->frames.size());
        int32_t maxW = 0;
        int32_t maxH = 0;

        if (auto* invAnim = (TAnimation*)st.invanim)
        {
            if (invAnim->numframes > 0)
            {
                for (int f = 0; f < invAnim->numframes; f++)
                {
                    const SAnimationFrame& af = invAnim->frames[f];
                    PTBitmap bm = (TBitmap*)af.bitmap;
                    if (!tryPushFrame(bm, (int16_t)af.dx, (int16_t)af.dy,
                                      (int16_t)af.regx, (int16_t)af.regy))
                    {
                        break;
                    }
                    if (bm->width > maxW) maxW = bm->width;
                    if (bm->height > maxH) maxH = bm->height;
                }
            }
        }

        if (int32_t(atlas->frames.size()) == firstFrame)
        {
            PTBitmap bm = (TBitmap*)st.invitem;
            if (!bm) bm = (TBitmap*)st.still;
            if (tryPushFrame(bm, 0, 0, 0, 0))
            {
                maxW = bm->width;
                maxH = bm->height;
            }
        }

        if (int32_t(atlas->frames.size()) == firstFrame)
            continue;

        SBitmapAtlasItem item;
        item.firstFrame = firstFrame;
        item.numFrames = int32_t(atlas->frames.size()) - firstFrame;
        item.maxW = maxW;
        item.maxH = maxH;
        item.label = e->filename;
        atlas->items.push_back(std::move(item));
    }

    if (atlas_full)
    {
        log_warn("[ui] atlas full at %zu items / %zu frames; dropping rest",
                 atlas->items.size(), atlas->frames.size());
    }
    if (atlas->items.empty())
    {
        log_error("[ui] no eligible 2D imagery in registry");
        return false;
    }

    const int32_t pack_h = shelf_y + shelf_h;
    int32_t atlas_h = 1;
    while (atlas_h < pack_h) atlas_h <<= 1;
    if (atlas_h > kBitmapAtlasMaxHeight) atlas_h = kBitmapAtlasMaxHeight;
    atlas->width = kBitmapAtlasWidth;
    atlas->height = atlas_h;

    std::unique_ptr<uint8_t[]> rgba(new uint8_t[size_t(atlas->width) * atlas->height * 4]);
    memset(rgba.get(), 0, size_t(atlas->width) * atlas->height * 4);
    const int32_t pitch = atlas->width * 4;
    for (size_t i = 0; i < atlas->frames.size(); i++)
    {
        const SBitmapAtlasFrame& frame = atlas->frames[i];
        if (!DecodeBitmapToRGBA(bitmaps[i], rgba.get(), pitch, frame.ax, frame.ay))
        {
            log_warn("[ui] decode failed for frame %zu (flags=0x%x)",
                     i, bitmaps[i] ? bitmaps[i]->flags : 0);
        }
    }

    const size_t bytes = size_t(atlas->width) * atlas->height * 4;
    if (Renderer)
    {
        atlas->texture = Renderer->RegisterTextureAsset(0,
                                                        rgba.get(),
                                                        bytes,
                                                        atlas->width,
                                                        atlas->height,
                                                        ERendererTextureFormat::RGBA8,
                                                        bytes,
                                                        ERendererTextureFilter::Nearest);
        if (atlas->texture != kInvalidTexture)
            Renderer->AddTextureAssetRef(atlas->texture);
    }

    if (atlas->texture == kInvalidTexture)
    {
        log_error("[ui] atlas upload failed");
        DestroyBitmapAtlas(atlas);
        return false;
    }

    log_info("[ui] atlas %dx%d packed with %zu items / %zu frames",
             atlas->width, atlas->height, atlas->items.size(), atlas->frames.size());
    return true;
}
