// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                bitmapatlas.cpp - Bitmap atlas helpers                 *
// *************************************************************************

#include "engine/render/bitmapatlas.h"

#include "engine/assets/animimagebody.h"
#include "engine/assets/bitmap.h"
#include "engine/assets/bitmapdecode.h"
#include "engine/assets/imageres.h"
#include "engine/core/logging.h"
#include "engine/assets/multi.h"
#include "engine/render/renderer.h"
#include "engine/core/revdefs.h"

#include <algorithm>
#include <cstring>
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace {

constexpr int32_t kUIAtlasSmallW = 2048;
constexpr int32_t kUIAtlasSmallH = 2048;
constexpr int32_t kUIAtlasPad = 2;
constexpr int32_t kUIAtlasSmallLimit = 128;
constexpr int32_t kUIAtlasStandaloneDim = 512;
constexpr int32_t kUIAtlasStandaloneAspect = 4;
constexpr uint64_t kFnvOffset64 = 1469598103934665603ull;
constexpr uint64_t kFnvPrime64 = 1099511628211ull;

struct SUIBitmapAtlasCandidate
{
    PTBitmap bm = nullptr;
    EBitmapDecode decode = EBitmapDecode::Pixels;
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

    const uint32_t knownFormatFlags = BM_8BIT | BM_15BIT | BM_16BIT | BM_ARGB4444;
    if (!(bm->flags & knownFormatFlags)) return false;

    const uint64_t pixels = uint64_t(bm->width) * uint64_t(bm->height);
    if (pixels == 0 || pixels > uint64_t(kUIAtlasMaxDim) * uint64_t(kUIAtlasMaxDim))
        return false;

    if (bm->flags & BM_8BIT)
        return bm->datasize >= pixels && bm->palette.ptr() != nullptr;
    if (bm->flags & (BM_15BIT | BM_16BIT | BM_ARGB4444))
        return bm->datasize >= pixels * 2;
    return false;
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
    if (!DecodeBitmapToRGBA(bm, rgba.data(), bm->width * 4, 0, 0, c.decode))
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

inline void ClearBitmapAtlasMetadata(SBitmapAtlas* atlas)
{
    if (!atlas) return;
    atlas->width = 0;
    atlas->height = 0;
    atlas->items.clear();
    atlas->frames.clear();
}

}  // namespace

uint64_t UIBitmapAtlasKey(PTBitmap bm, EBitmapDecode decode)
{
    uint64_t h = kFnvOffset64;
    HashU64(h, uintptr_t(bm));
    HashU64(h, uint64_t(decode));
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

void RegisterUIBitmapAtlasBitmap(PTBitmap bm, EBitmapDecode decode)
{
    if (!g_uiCollecting || !IsUIBitmapAtlasDecodable(bm))
        return;

    const uint64_t key = UIBitmapAtlasKey(bm, decode);
    if (!g_uiCandidateKeys.insert(key).second)
        return;

    SUIBitmapAtlasCandidate c;
    c.bm = bm;
    c.decode = decode;
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
        if (!DecodeBitmapToRGBA(bm, page.rgba.data(), pitch, px, py, c.decode))
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

bool LookupUIBitmapAtlasSlice(PTBitmap bm, EBitmapDecode decode, SBitmapAtlasSlice* out)
{
    if (!out || !g_uiReady || !bm) return false;
    auto it = g_uiSlices.find(UIBitmapAtlasKey(bm, decode));
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

// For the game's imagery feeder (game/ui/imageryatlas.cpp).
bool UIBitmapAtlasCollecting() { return g_uiCollecting; }
int32_t UIBitmapAtlasCandidateCount() { return int32_t(g_uiCandidates.size()); }
bool UIBitmapAtlasDecodable(PTBitmap bm) { return IsUIBitmapAtlasDecodable(bm); }
