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
#include "revdefs.h"

#include <memory>

namespace {

constexpr int32_t kBitmapAtlasMaxItems = 4096;
constexpr int32_t kBitmapAtlasMaxFrames = 16384;
constexpr int32_t kBitmapAtlasWidth = 4096;
constexpr int32_t kBitmapAtlasMaxHeight = 4096;
constexpr int32_t kBitmapAtlasPad = 2;

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

void DestroyBitmapAtlas(SBitmapAtlas* atlas)
{
    if (!atlas) return;
    if (atlas->image.id)
        sg_destroy_image(atlas->image);
    atlas->image = {};
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

    sg_image_desc d = {};
    d.width = atlas->width;
    d.height = atlas->height;
    d.pixel_format = SG_PIXELFORMAT_RGBA8;
    d.min_filter = SG_FILTER_NEAREST;
    d.mag_filter = SG_FILTER_NEAREST;
    d.wrap_u = SG_WRAP_CLAMP_TO_EDGE;
    d.wrap_v = SG_WRAP_CLAMP_TO_EDGE;
    d.data.subimage[0][0].ptr = rgba.get();
    d.data.subimage[0][0].size = size_t(atlas->width) * atlas->height * 4;
    d.label = "ui.atlas";
    atlas->image = sg_make_image(&d);

    if (sg_query_image_state(atlas->image) != SG_RESOURCESTATE_VALID)
    {
        log_error("[ui] atlas upload failed");
        DestroyBitmapAtlas(atlas);
        return false;
    }

    log_info("[ui] atlas %dx%d packed with %zu items / %zu frames",
             atlas->width, atlas->height, atlas->items.size(), atlas->frames.size());
    return true;
}
