// *************************************************************************
// *                         Cinematix Revenant                            *
// *                                                                        *
// *   testscreen.cpp — bring-up harness for the retail sync port           *
// *************************************************************************

#include "testscreen.h"

#include "animimage.h"
#include "animimagebody.h"
#include "animation.h"
#include "animdata.h"
#include "bitmap.h"
#include "bitmapdata.h"
#include "chunkcache.h"
#include "decompdata.h"
#include "revdefs.h"
#include "display.h"
#include "font.h"
#include "fonttable.h"
#include "imagery.h"
#include "imageres.h"
#include "logging.h"
#include "object.h"
#include "revenant.h"
#include "sector.h"
#include "surface.h"
#include "time.h"

#include <sokol_gfx.h>

#include "imgui.h"

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <vector>

TTestScreen TestScreen;
char        StartupTestMode[32] = "";
char        StartupSectorId[32] = "";

// Icon test state: we composite SystemFont's prebuilt GPU atlas onto the
// backbuffer every frame. The atlas lives in the font subsystem's parallel
// cache (keyed by TFont*); we just look it up.
namespace {
sg_image g_iconImg     = {};
int32_t  g_iconWidth   = 0;
int32_t  g_iconHeight  = 0;

// --test=ui single atlas: shelf-packs every .I2D/.TN state[0] bitmap — and
// each animation frame, as a standalone rect — into one RGBA8 sg_image.
// 4096² is the portable cap guaranteed across GL ES 3.0 / Metal / Vulkan /
// D3D11. Items index into a parallel g_uiFrames table; static items carry
// numFrames=1, animated items (potions, etc.) store every frame back-to-back
// on the same shelf so sampler locality stays good.
struct SUiFrame {
    int32_t ax, ay, w, h;              // atlas rect
    int16_t dx, dy;                    // per-frame draw offset (SAnimationFrame)
    int16_t regx, regy;                // registration offset (anchor)
};
struct SUiItem {
    int32_t firstFrame;
    int32_t numFrames;
    int32_t maxW, maxH;                // largest frame in this item — cell size
    char    label[64];
};
constexpr int     kMaxUiItems   = 4096;
constexpr int     kMaxUiFrames  = 16384;
constexpr int32_t kUiAtlasW     = 4096;
constexpr int32_t kUiAtlasMaxH  = 4096;
SUiItem  g_uiItems[kMaxUiItems]  = {};
SUiFrame g_uiFrames[kMaxUiFrames] = {};
int32_t  g_uiItemCount  = 0;
int32_t  g_uiFrameCount = 0;
sg_image g_uiAtlas      = {};
int32_t  g_uiAtlasW     = 0;
int32_t  g_uiAtlasH     = 0;

// --test=sector: upload every OBJCLASS_TILE still bitmap in a loaded sector
// and draw them iso-projected each frame. One sg_image per unique bitmap
// (cached by pointer), one SSectorTileInst per instance carrying its screen
// placement + anchor_z. Per-pixel depth in the tile fragment shader lets
// overlapping tiles interpenetrate naturally without painter sort.
struct SSectorTileTex {
    void*    bm_key;    // TBitmap* — cache key
    sg_image color;
    sg_image depth;
    int32_t  w, h;
};
struct SSectorTileInst {
    int32_t  tex_idx;   // into g_sectorTileTex
    int32_t  dst_x;     // top-left pixel of the bitmap in viewport
    int32_t  dst_y;
    float    anchor_z;
};
std::vector<SSectorTileTex>  g_sectorTileTex;
std::vector<SSectorTileInst> g_sectorTileInst;
TSector* g_sectorKept = nullptr;  // kept alive so instance pointers stay valid

// Screen-space sector point lights. Collected once at sector load: each
// OF_LIGHT object is iso-projected and recentered via the same (ox,oy) used
// for tiles. Intensity + color carried through verbatim from lightdef; the
// ImGui panel multiplies by global scales for interactive tuning.
struct SSectorLight {
    const char* classname; // for the debug panel listing
    float x, y;            // screen pixels
    float z;               // "above the screen" in pixels (use world-z mapped into pixels)
    float radius;          // retail lightdef.intensity (doubles as radius)
    float col[3];          // 0..1 rgb
    float intensity;       // derived from multiplier (28 = baseline in retail)
    bool  enabled;
};
std::vector<SSectorLight> g_sectorLights;

// Legacy hello-world globals (used by earlier single-tile test path).
sg_image g_tileColorImg = {};
sg_image g_tileDepthImg = {};
int32_t  g_tileW        = 0;
int32_t  g_tileH        = 0;
constexpr int32_t kTileTexSize = 64;

// Palette-expand an 8-bit TBitmap into RGBA8 and upload its BM_ZBUFFER u16
// depthmap as R32F (normalized [0,1] by /65535). Supports chunked+compressed
// bitmaps: CacheChunks() decompresses through the shared chunk cache and we
// assemble the flat w*h planes from 64x64 tiles.
bool UploadTileBitmap(PTBitmap bm,
                      sg_image* out_color, sg_image* out_depth,
                      int32_t* out_w, int32_t* out_h)
{
    if (!bm || bm->width <= 0 || bm->height <= 0) return false;
    if (!(bm->flags & BM_8BIT))    return false;
    if (!(bm->flags & BM_ZBUFFER)) return false;
    SPalette* pal = (SPalette*)bm->palette.ptr();
    if (!pal) return false;

    const int32_t w = bm->width, h = bm->height;
    const size_t  npx = size_t(w) * size_t(h);
    const uint8_t key = (uint8_t)bm->keycolor;

    std::unique_ptr<uint8_t[]>  idxplane(new uint8_t[npx]);
    std::unique_ptr<uint16_t[]> zplane  (new uint16_t[npx]);

    if (bm->flags & BM_CHUNKED)
    {
        if (!bm->CacheChunks()) return false;
        SChunkHeader* hdr  = (SChunkHeader*)(void*)bm->data8;
        SChunkHeader* zhdr = (SChunkHeader*)bm->zbuffer.ptr();
        if (!hdr || !zhdr) return false;
        const int32_t cw = hdr->width, ch = hdr->height;
        // Prefill with "transparent" (keycolor) and z=max. Blank blocks
        // (OFFSET==0) legitimately leave their region untouched.
        std::memset(idxplane.get(), key, npx);
        for (size_t i = 0; i < npx; ++i) zplane[i] = 0x7F7F;
        for (int32_t by = 0; by < ch; ++by)
        for (int32_t bx = 0; bx < cw; ++bx)
        {
            void* cptr = hdr->block[by * cw + bx].ptr();
            void* zptr = zhdr->block[by * cw + bx].ptr();
            const uint8_t*  c8  = cptr ? (const uint8_t*)ChunkCache.AddChunk(cptr, 1)
                                       : nullptr;
            const uint16_t* z16 = zptr ? (const uint16_t*)ChunkCache.AddChunkZ(zptr, 2)
                                       : nullptr;
            const int32_t x0 = bx * CHUNKWIDTH, y0 = by * CHUNKHEIGHT;
            const int32_t cxmax = (w - x0 < CHUNKWIDTH)  ? (w - x0) : CHUNKWIDTH;
            const int32_t cymax = (h - y0 < CHUNKHEIGHT) ? (h - y0) : CHUNKHEIGHT;
            if (cxmax <= 0 || cymax <= 0) continue;
            for (int32_t y = 0; y < cymax; ++y)
            {
                if (c8)
                    std::memcpy(&idxplane[(y0 + y) * w + x0],
                                &c8[y * CHUNKWIDTH], cxmax);
                if (z16)
                    std::memcpy(&zplane[(y0 + y) * w + x0],
                                &z16[y * CHUNKWIDTH], cxmax * sizeof(uint16_t));
            }
        }
    }
    else
    {
        if (bm->flags & BM_COMPRESSED) return false;
        uint16_t* zbuf = (uint16_t*)bm->zbuffer.ptr();
        if (!zbuf) return false;
        std::memcpy(idxplane.get(), bm->data8, npx);
        std::memcpy(zplane.get(),   zbuf,      npx * sizeof(uint16_t));
    }

    std::unique_ptr<uint8_t[]> rgba(new uint8_t[npx * 4]);
    std::unique_ptr<float[]>   dflt(new float[npx]);
    // Revenant's tile renderer treats zbuffer == 0x7F7F as transparent
    // (see graphics.cpp ZFindChunk / PutChunk8). Color-keycolor alone is
    // not sufficient — e.g. diamond tile corners have valid color but
    // z=0x7F7F so the engine skips them.
    for (size_t i = 0; i < npx; i++)
    {
        const uint8_t  idx = idxplane[i];
        const uint16_t z   = zplane[i];
        if (idx == key || z == 0x7F7F)
        {
            rgba[i*4+0]=rgba[i*4+1]=rgba[i*4+2]=rgba[i*4+3]=0;
        }
        else
        {
            const uint32_t c = pal->rgbcolors[idx];
            rgba[i*4+0] = (uint8_t)( c        & 0xFF);
            rgba[i*4+1] = (uint8_t)((c >> 8)  & 0xFF);
            rgba[i*4+2] = (uint8_t)((c >> 16) & 0xFF);
            rgba[i*4+3] = 255;
        }
        dflt[i] = float(z) / 65535.0f;
    }

    sg_image_desc cd = {};
    cd.width = w; cd.height = h;
    cd.pixel_format = SG_PIXELFORMAT_RGBA8;
    cd.min_filter = SG_FILTER_NEAREST;
    cd.mag_filter = SG_FILTER_NEAREST;
    cd.wrap_u = cd.wrap_v = SG_WRAP_CLAMP_TO_EDGE;
    cd.data.subimage[0][0].ptr  = rgba.get();
    cd.data.subimage[0][0].size = npx * 4;
    cd.label = "tile.color.bm";
    *out_color = sg_make_image(&cd);

    sg_image_desc dd = {};
    dd.width = w; dd.height = h;
    dd.pixel_format = SG_PIXELFORMAT_R32F;
    dd.min_filter = SG_FILTER_NEAREST;
    dd.mag_filter = SG_FILTER_NEAREST;
    dd.wrap_u = dd.wrap_v = SG_WRAP_CLAMP_TO_EDGE;
    dd.data.subimage[0][0].ptr  = dflt.get();
    dd.data.subimage[0][0].size = npx * sizeof(float);
    dd.label = "tile.depth.bm";
    *out_depth = sg_make_image(&dd);

    *out_w = w; *out_h = h;
    return true;
}

inline void Decode555(uint16_t px, uint8_t* rgba_out)
{
    rgba_out[0] = (uint8_t)(((px >> 10) & 0x1F) << 3);
    rgba_out[1] = (uint8_t)(((px >> 5)  & 0x1F) << 3);
    rgba_out[2] = (uint8_t)(( px        & 0x1F) << 3);
    rgba_out[3] = 255;
}

// Decode a TBitmap into RGBA8 rows inside a larger destination buffer.
// dst_pitch is in bytes. (ox, oy) is the top-left of the glyph in dst.
// Keycolor pixels become alpha=0. Returns false for unsupported formats.
bool DecodeBitmapToRGBA(PTBitmap bm, uint8_t* dst, int32_t dst_pitch,
                        int32_t ox, int32_t oy)
{
    if (!bm || bm->width <= 0 || bm->height <= 0) return false;
    if (bm->flags & BM_COMPRESSED) return false;  // TODO decompressor
    const int32_t w = bm->width, h = bm->height;

    if (bm->flags & BM_8BIT)
    {
        SPalette* pal = (SPalette*)bm->palette.ptr();
        if (!pal) return false;
        const uint8_t key = (uint8_t)bm->keycolor;
        const uint8_t* src = bm->data8;
        for (int32_t y = 0; y < h; y++)
        {
            uint8_t* row = dst + (oy + y) * dst_pitch + ox * 4;
            for (int32_t x = 0; x < w; x++)
            {
                const uint8_t idx = src[y * w + x];
                if (idx == key) { row[0]=row[1]=row[2]=row[3]=0; }
                else
                {
                    // rgbcolors is Windows COLORREF: 0x00BBGGRR.
                    const uint32_t c = pal->rgbcolors[idx];
                    row[0] = (uint8_t)( c        & 0xFF);
                    row[1] = (uint8_t)((c >> 8)  & 0xFF);
                    row[2] = (uint8_t)((c >> 16) & 0xFF);
                    row[3] = 255;
                }
                row += 4;
            }
        }
        return true;
    }
    if (bm->flags & (BM_15BIT | BM_16BIT))
    {
        const uint16_t key = (uint16_t)bm->keycolor;
        const uint16_t* src = bm->data16;
        for (int32_t y = 0; y < h; y++)
        {
            uint8_t* row = dst + (oy + y) * dst_pitch + ox * 4;
            for (int32_t x = 0; x < w; x++)
            {
                const uint16_t px = src[y * w + x];
                if (px == key) { row[0]=row[1]=row[2]=row[3]=0; }
                else           { Decode555(px, row); }
                row += 4;
            }
        }
        return true;
    }
    return false;
}
}  // namespace

static void DumpFont(const char* name)
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

    // Sanity-check a couple of glyphs: 'A' and '0' should load as bitmaps
    // with a plausible width/height. If the retail .dat layout diverges
    // from pre-release, the TOffset resolve will produce garbage dims.
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

static void AsciiRenderGlyph(const char* fontname, unsigned char ch)
{
    if (!FontTable) return;
    TFont* f = FontTable->Bitmap(fontname);
    if (!f) return;
    PTBitmap bm = f->GetChar(ch);
    if (!bm) return;
    int w = bm->width, h = bm->height;
    if (w <= 0 || h <= 0 || w > 64 || h > 64)
    {
        log_warn("[font] skip ascii-render '%c' from '%s': implausible %dx%d", ch, fontname, w, h);
        return;
    }
    uint16_t key = (uint16_t)bm->keycolor;
    log_info("[font] ascii-render '%c' from '%s' (%dx%d, 16-bit, key=0x%04x):", ch, fontname, w, h, key);
    for (int y = 0; y < h; y++)
    {
        char row[72];
        int pos = 0;
        for (int x = 0; x < w && pos < 70; x++)
        {
            uint16_t px = bm->data16[y * w + x];
            row[pos++] = (px == key) ? '.' : '#';
        }
        row[pos] = 0;
        log_info("[font]   |%s|", row);
    }
}

bool TTestScreen::Initialize()
{
    const char *mode = StartupTestMode[0] ? StartupTestMode : "blank";
    log_info("[test] initialize mode='%s'", mode);
    lastLoggedLegacyFrame = -1;

    if (strcmp(mode, "blank") == 0 || strcmp(mode, "ticker") == 0)
        return true;

    if (strcmp(mode, "ttf") == 0)
    {
        // Bring up stb_truetype: rasterize a TrueType font and cache the
        // atlas. Rendering happens in Animate; Initialize just warms the
        // cache so we catch load failures before the first frame.
        const char* paths[] = {
            "/System/Library/Fonts/Supplemental/Arial.ttf",
            "/System/Library/Fonts/Supplemental/Times New Roman.ttf",
            "/System/Library/Fonts/Supplemental/Georgia.ttf",
        };
        const int sizes[] = { 28, 48 };
        for (const char* p : paths)
            for (int s : sizes)
                if (!BuildTTFAtlas(p, s))
                    log_warn("[ttf] skipping unavailable '%s'", p);
        return true;
    }

    if (strcmp(mode, "text") == 0)
    {
        // Build SystemFont's atlas so Animate can draw per-glyph sub-rects.
        // No per-frame state to cache here — Animate reads the atlas directly
        // out of the parallel cache each frame.
        if (!SystemFont)
        {
            log_error("[text] SystemFont is null");
            return true;
        }
        const SFontAtlas* atlas = BuildFontAtlas(SystemFont);
        if (!atlas || !atlas->image.id)
            log_error("[text] SystemFont atlas build failed");
        return true;
    }

    if (strcmp(mode, "icon") == 0)
    {
        // Composite SystemFont's GPU atlas as the test "icon". The atlas is
        // built eagerly in TFontTable::LoadAtom and keyed by TFont*, so we
        // just look it up here.
        if (!SystemFont)
        {
            log_error("[icon] SystemFont is null");
            return true;
        }
        const SFontAtlas* atlas = BuildFontAtlas(SystemFont);
        if (!atlas || !atlas->image.id)
        {
            log_error("[icon] SystemFont atlas build failed");
            return true;
        }
        log_info("[icon] atlas %dx%d for %d glyphs",
            atlas->width, atlas->height, (int)atlas->numchars);
        g_iconImg    = atlas->image;
        g_iconWidth  = atlas->width;
        g_iconHeight = atlas->height;
        return true;
    }

    if (strcmp(mode, "ui") == 0)
    {
        // Walk the imagery registry, pick .I2D/.TN entries, expand invanim
        // animations into individual frames, shelf-pack everything into one
        // atlas. Static items resolve to 1 frame (invitem or still fallback).
        const int32_t n = TObjectImagery::NumEntries();
        log_info("[ui] scanning %d registered imagery entries", n);

        constexpr int32_t kPad = 2;

        // Pass 1: collect frames. `bitmaps` runs parallel to g_uiFrames so
        // pass 2 can blit without re-resolving offsets.
        PTBitmap bitmaps[kMaxUiFrames] = {};
        int32_t cursor_x = 0, shelf_y = 0, shelf_h = 0;
        bool atlas_full = false;

        auto tryPushFrame = [&](PTBitmap bm, int16_t dx, int16_t dy,
                                int16_t regx, int16_t regy) -> bool
        {
            if (atlas_full) return false;
            if (!bm || bm->width <= 0 || bm->height <= 0) return false;
            if (bm->flags & BM_COMPRESSED) return false;
            if (!(bm->flags & (BM_8BIT | BM_15BIT | BM_16BIT))) return false;
            if (bm->width > kUiAtlasW || bm->height > kUiAtlasMaxH) return false;
            if (g_uiFrameCount >= kMaxUiFrames) return false;

            if (cursor_x + bm->width + kPad > kUiAtlasW)
            {
                shelf_y += shelf_h + kPad;
                cursor_x = 0;
                shelf_h  = 0;
            }
            if (shelf_y + bm->height > kUiAtlasMaxH)
            {
                atlas_full = true;
                return false;
            }

            auto& fr = g_uiFrames[g_uiFrameCount];
            fr.ax = cursor_x;
            fr.ay = shelf_y;
            fr.w  = bm->width;
            fr.h  = bm->height;
            fr.dx = dx;   fr.dy = dy;
            fr.regx = regx; fr.regy = regy;
            bitmaps[g_uiFrameCount] = bm;
            g_uiFrameCount++;

            cursor_x += bm->width + kPad;
            if (bm->height > shelf_h) shelf_h = bm->height;
            return true;
        };

        for (int32_t id = 0; id < n && g_uiItemCount < kMaxUiItems && !atlas_full; id++)
        {
            if (!TObjectImagery::IsUsed(id)) continue;
            SImageryEntry* e = TObjectImagery::GetImageryEntry(id);
            if (!e || !e->filename[0]) continue;

            const size_t flen = strlen(e->filename);
            const bool is2d =
                (flen > 4 && (!stricmp(e->filename + flen - 4, ".I2D") ||
                              !stricmp(e->filename + flen - 3, ".TN")));
            if (!is2d) continue;

            TObjectImagery* img = TObjectImagery::LoadImagery(id);
            if (!img) continue;
            SImageryBody* body = img->GetBody();
            if (!body) continue;

            auto* ab = (SAnimImageryBody*)body;
            auto& st = ab->states[0];

            const int32_t firstFrame = g_uiFrameCount;
            int32_t maxW = 0, maxH = 0;

            // Prefer inventory animation, then inventory still, then world still.
            auto* invAnim = (TAnimation*)st.invanim;
            if (invAnim && invAnim->numframes > 0)
            {
                for (int f = 0; f < invAnim->numframes; f++)
                {
                    const SAnimationFrame& af = invAnim->frames[f];
                    PTBitmap bm = (TBitmap*)af.bitmap;
                    if (!tryPushFrame(bm,
                            (int16_t)af.dx, (int16_t)af.dy,
                            (int16_t)af.regx, (int16_t)af.regy))
                        break;
                    if (bm->width  > maxW) maxW = bm->width;
                    if (bm->height > maxH) maxH = bm->height;
                }
            }
            if (g_uiFrameCount == firstFrame)
            {
                PTBitmap bm = (TBitmap*)st.invitem;
                if (!bm) bm = (TBitmap*)st.still;
                if (tryPushFrame(bm, 0, 0, 0, 0))
                {
                    maxW = bm->width;
                    maxH = bm->height;
                }
            }
            if (g_uiFrameCount == firstFrame) continue;  // nothing packed

            auto& slot = g_uiItems[g_uiItemCount];
            slot.firstFrame = firstFrame;
            slot.numFrames  = g_uiFrameCount - firstFrame;
            slot.maxW       = maxW;
            slot.maxH       = maxH;
            strncpyz(slot.label, e->filename, sizeof(slot.label));
            g_uiItemCount++;
        }

        if (atlas_full)
            log_warn("[ui] atlas full at %d items / %d frames; dropping rest",
                g_uiItemCount, g_uiFrameCount);
        if (g_uiItemCount == 0)
        {
            log_error("[ui] no eligible 2D imagery in registry");
            return true;
        }

        // Round atlas height up to a POT for GPU friendliness (still ≤ 4096).
        const int32_t pack_h = shelf_y + shelf_h;
        int32_t atlas_h = 1;
        while (atlas_h < pack_h) atlas_h <<= 1;
        if (atlas_h > kUiAtlasMaxH) atlas_h = kUiAtlasMaxH;
        g_uiAtlasW = kUiAtlasW;
        g_uiAtlasH = atlas_h;

        // Pass 2: alloc RGBA buffer, blit every frame, upload as sg_image.
        std::unique_ptr<uint8_t[]> rgba(new uint8_t[size_t(g_uiAtlasW) * g_uiAtlasH * 4]);
        std::memset(rgba.get(), 0, size_t(g_uiAtlasW) * g_uiAtlasH * 4);
        const int32_t pitch = g_uiAtlasW * 4;
        for (int32_t i = 0; i < g_uiFrameCount; i++)
        {
            if (!DecodeBitmapToRGBA(bitmaps[i], rgba.get(), pitch,
                                    g_uiFrames[i].ax, g_uiFrames[i].ay))
            {
                log_warn("[ui] decode failed for frame %d (flags=0x%x)",
                    i, bitmaps[i] ? bitmaps[i]->flags : 0);
            }
        }

        sg_image_desc d = {};
        d.width = g_uiAtlasW;
        d.height = g_uiAtlasH;
        d.pixel_format = SG_PIXELFORMAT_RGBA8;
        d.min_filter = SG_FILTER_NEAREST;
        d.mag_filter = SG_FILTER_NEAREST;
        d.wrap_u = SG_WRAP_CLAMP_TO_EDGE;
        d.wrap_v = SG_WRAP_CLAMP_TO_EDGE;
        d.data.subimage[0][0].ptr  = rgba.get();
        d.data.subimage[0][0].size = size_t(g_uiAtlasW) * g_uiAtlasH * 4;
        d.label = "ui.atlas";
        g_uiAtlas = sg_make_image(&d);

        log_info("[ui] atlas %dx%d packed with %d items / %d frames",
            g_uiAtlasW, g_uiAtlasH, g_uiItemCount, g_uiFrameCount);
        return true;
    }

    if (strcmp(mode, "sector") == 0)
    {
        // Phase A sweep: try a handful of candidate sectors and log counts +
        // the classnames of the first few objects, so we can see which parse
        // cleanly before we pick one to render. Garbage / repeated ? names
        // signal stream desync (usually from missing class registrations).
        //
        // --sector=L_X_Y on the command line overrides which sector is kept
        // for rendering. Without it, we keep the hard-coded default below.
        struct SCand { int32_t lvl, sx, sy; };
        std::vector<SCand> cands = {
            { 0,  0, 27 },   // original pick
            { 0,  2, 25 },   // biggest .DAT — probably Misthaven town
            { 0,  1, 29 },   // mid-size, level 0
            { 6,  3,  5 },   // level 6 dungeon
            { 2,  8, 10 },   // level 2
            { 1,  6,  9 },   // level 1
        };
        int32_t keep_lvl = 0, keep_sx = 2, keep_sy = 25;
        if (StartupSectorId[0])
        {
            int32_t l=0, x=0, y=0;
            if (std::sscanf(StartupSectorId, "%d_%d_%d", &l, &x, &y) == 3)
            {
                keep_lvl = l; keep_sx = x; keep_sy = y;
                // Make sure our target is tried and logged too.
                bool present = false;
                for (const auto& c : cands)
                    if (c.lvl == l && c.sx == x && c.sy == y) { present = true; break; }
                if (!present) cands.insert(cands.begin(), { l, x, y });
            }
            else
            {
                log_warn("[sector] bad --sector='%s', expected L_X_Y", StartupSectorId);
            }
        }

        for (const auto& c : cands)
        {
            log_info("[sector] -> loading %d_%d_%d", c.lvl, c.sx, c.sy);
            TSector* sec = TSector::LoadSector(c.lvl, c.sx, c.sy, false);
            if (!sec)
            {
                log_warn("[sector] %d_%d_%d: LoadSector failed", c.lvl, c.sx, c.sy);
                continue;
            }
            log_info("[sector] <- loaded  %d_%d_%d", c.lvl, c.sx, c.sy);
            const int32_t total = sec->NumItems();
            log_info("[sector] %d_%d_%d: %d objects "
                     "(moving=%d char=%d lights=%d pulse=%d anim=%d notify=%d)",
                c.lvl, c.sx, c.sy, total,
                sec->NumObjSetItems(OBJSET_MOVING),
                sec->NumObjSetItems(OBJSET_CHARACTER),
                sec->NumObjSetItems(OBJSET_LIGHTS),
                sec->NumObjSetItems(OBJSET_PULSE),
                sec->NumObjSetItems(OBJSET_ANIMATE),
                sec->NumObjSetItems(OBJSET_NOTIFY));
            const int32_t dumpN = total < 6 ? total : 6;
            for (int32_t i = 0; i < dumpN; i++)
            {
                TObjectInstance* oi = sec->GetInstance(i);
                if (!oi) { log_info("[sector]   [%d] <null>", i); continue; }
                const S3DPoint& p = oi->Pos();
                log_info("[sector]   [%d] class=%s pos=(%d,%d,%d)",
                    i, oi->GetClassName(), p.x, p.y, p.z);
            }
            // Keep 0_0_27 alive: walk every TILE instance, upload each unique
            // bitmap, and iso-project its world position into a screen-space
            // draw list. Rendered each frame from Animate().
            if (!g_sectorKept && c.lvl == keep_lvl && c.sx == keep_sx && c.sy == keep_sy)
            {
                g_sectorKept = sec;

                // Pass 1: collect unique bitmaps → textures, record per-instance
                // world position + screen-space registration (reg lives on the
                // imagery state, not the bitmap — see TAnimImagery::DrawUnlit
                // which subtracts GetRegX(state)/GetRegY(state) from the iso-
                // projected screen pos). Track iso-z range for anchor_z norm.
                struct WorldInst { int32_t tex_idx; S3DPoint wpos; int32_t regx, regy; };
                std::vector<WorldInst> work;
                work.reserve(sec->NumItems());
                for (int32_t i = 0; i < sec->NumItems(); i++)
                {
                    TObjectInstance* oi = sec->GetInstance(i);
                    if (!oi || oi->ObjClass() != OBJCLASS_TILE) continue;
                    TObjectImagery* img = oi->GetImagery();
                    SImageryHeader* hdr = img ? img->GetHeader() : nullptr;
                    auto* ab = (SAnimImageryBody*)(img ? img->GetBody() : nullptr);
                    const int32_t st = oi->GetState();
                    PTBitmap bm = (ab && hdr && st < hdr->numstates)
                        ? (TBitmap*)ab->states[st].still : nullptr;
                    if (!bm) continue;

                    int32_t tex_idx = -1;
                    for (size_t t = 0; t < g_sectorTileTex.size(); t++)
                        if (g_sectorTileTex[t].bm_key == bm) { tex_idx = (int32_t)t; break; }
                    if (tex_idx < 0)
                    {
                        SSectorTileTex t = {};
                        t.bm_key = bm;
                        if (!UploadTileBitmap(bm, &t.color, &t.depth, &t.w, &t.h))
                            continue;
                        tex_idx = (int32_t)g_sectorTileTex.size();
                        g_sectorTileTex.push_back(t);
                    }
                    work.push_back({ tex_idx, oi->Pos(),
                                     img->GetRegX(st), img->GetRegY(st) });
                }

                // Pass 2: iso-project, find screen bounding box so we can
                // center the sector in the viewport, then emit instances.
                // anchor_z is normalized from the iso screen-z so tiles are
                // ordered front-to-back and z_scale leaves room for per-
                // pixel depth delta to poke through neighbors.
                int32_t sx_min = INT32_MAX, sx_max = INT32_MIN;
                int32_t sy_min = INT32_MAX, sy_max = INT32_MIN;
                int32_t sz_min = INT32_MAX, sz_max = INT32_MIN;
                std::vector<S3DPoint> spos;
                spos.reserve(work.size());
                for (auto& w : work)
                {
                    S3DPoint s;
                    WorldToScreen(w.wpos, s);
                    spos.push_back(s);
                    const auto& tex = g_sectorTileTex[w.tex_idx];
                    const int32_t x0 = s.x - w.regx;
                    const int32_t y0 = s.y - w.regy;
                    const int32_t x1 = x0 + tex.w;
                    const int32_t y1 = y0 + tex.h;
                    if (x0 < sx_min) sx_min = x0;
                    if (y0 < sy_min) sy_min = y0;
                    if (x1 > sx_max) sx_max = x1;
                    if (y1 > sy_max) sy_max = y1;
                    if ((int32_t)s.z < sz_min) sz_min = (int32_t)s.z;
                    if ((int32_t)s.z > sz_max) sz_max = (int32_t)s.z;
                }

                const int32_t tw = Display ? Display->Width()  : 1024;
                const int32_t th = Display ? Display->Height() : 768;
                const int32_t bw = sx_max - sx_min;
                const int32_t bh = sy_max - sy_min;
                const int32_t ox = (tw - bw) / 2 - sx_min;
                const int32_t oy = (th - bh) / 2 - sy_min;
                const float   zrange = float(sz_max - sz_min);

                for (size_t i = 0; i < work.size(); i++)
                {
                    const auto& w = work[i];
                    const auto& s = spos[i];
                    SSectorTileInst inst = {};
                    inst.tex_idx = w.tex_idx;
                    inst.dst_x   = s.x - w.regx + ox;
                    inst.dst_y   = s.y - w.regy + oy;
                    // Map iso screen-z to [0.1, 0.9] so per-pixel deltas
                    // (scaled by 0.05) don't clip against the depth clamp.
                    const float zn = zrange > 1.0f
                        ? float(int32_t(s.z) - sz_min) / zrange
                        : 0.5f;
                    inst.anchor_z = 0.10f + 0.80f * zn;
                    g_sectorTileInst.push_back(inst);
                }

                log_info("[sector] built tile list: %zu instances, %zu unique bitmaps",
                    g_sectorTileInst.size(), g_sectorTileTex.size());
                log_info("[sector]   screen bbox=%dx%d z=[%d..%d]",
                    bw, bh, sz_min, sz_max);

                // Pass 3: collect every OF_LIGHT object in the sector. Retail
                // stores position as a world-space offset (lightdef.pos) on top
                // of the owning object's Pos(); intensity is a byte that also
                // served as a screen-pixel radius. WorldToScreen + the same
                // (ox, oy) as tiles keeps lights pinned to what you see.
                for (int32_t i = 0; i < sec->NumItems(); i++)
                {
                    TObjectInstance* oi = sec->GetInstance(i);
                    if (!oi || !oi->IsLight()) continue;
                    PSLightDef ld = oi->GetLightDef();
                    if (!ld || ld->intensity == 0) continue;
                    S3DPoint wp = oi->Pos();
                    wp += ld->pos;
                    S3DPoint sp;
                    WorldToScreen(wp, sp);
                    SSectorLight L = {};
                    L.classname = oi->GetClassName();
                    L.x = float(sp.x + ox);
                    L.y = float(sp.y + oy);
                    L.z = 60.0f;                // reasonable default "height above screen"
                    L.radius = float(ld->intensity);
                    L.col[0] = ld->color.red   / 255.0f;
                    L.col[1] = ld->color.green / 255.0f;
                    L.col[2] = ld->color.blue  / 255.0f;
                    // multiplier in retail defaults ~28. Map to a 0..~2 range.
                    L.intensity = ld->multiplier > 0 ? float(ld->multiplier) / 28.0f : 1.0f;
                    L.enabled = true;
                    g_sectorLights.push_back(L);
                }
                log_info("[sector] collected %zu point lights", g_sectorLights.size());
                continue;  // skip CloseSector — keep this one alive
            }

            TSector::CloseSector(sec);
        }
        return true;
    }

    if (strcmp(mode, "i3d") == 0)
    {
        // By this point AppInit has mounted imagery.rvi and Loading Classes has
        // run, so the imagery EntryArray is populated for every class that
        // registered. We just prod the loader with a well-known name and log
        // what lands. If the pre-release .i3d parse diverges from retail, the
        // header fields will come out garbage.
        const char* probes[] = {
            "Misc\\Blood.I3D", "Misc\\Sparks.I3D", nullptr
        };
        for (int i = 0; probes[i]; i++)
        {
            int32_t id = TObjectImagery::FindImagery(probes[i]);
            log_info("[i3d] FindImagery('%s') -> id=%d", probes[i], id);
            if (id < 0)
                continue;
            TObjectImagery* img = TObjectImagery::LoadImagery(id);
            if (!img)
            {
                log_error("[i3d]   LoadImagery returned null for '%s'", probes[i]);
                continue;
            }
            SImageryHeader* hdr = img->GetHeader();
            if (!hdr)
            {
                log_error("[i3d]   header null for '%s'", probes[i]);
                continue;
            }
            log_info("[i3d]   loaded type=%d numstates=%d hdrsize=%d ressize=%u",
                img->ImageryId(), (int)hdr->numstates, img->GetHeaderSize(),
                img->GetResSize());
            for (int s = 0; s < hdr->numstates && s < 4; s++)
            {
                PSImageryStateHeader st = img->GetState(s);
                if (!st) break;
                log_info("[i3d]   state[%d] name='%s' w=%d h=%d frames=%d flags=0x%x",
                    s, st->animname, st->width, st->height, st->frames, st->flags);
            }
        }
        return true;
    }

    if (strcmp(mode, "font") == 0)
    {
        // InitSystem already called FontTable->Initialize() and populated
        // the globals; we just inspect what landed. Any format mismatch
        // with the retail .rvr will show up as zero/garbage metrics below.
        log_info("[font] FontTable=%p SystemFont=%p DialogFont=%p SmallFont=%p GameFont=%p",
            (void*)FontTable, (void*)SystemFont, (void*)DialogFont,
            (void*)SmallFont, (void*)GameFont);

        const char* probes[] = {
            "System", "Scroll", "Book", "Dialog", "Small", "Med", "Gold", nullptr
        };
        for (int i = 0; probes[i]; i++)
            DumpFont(probes[i]);

        AsciiRenderGlyph("System", 'A');

        // Dump raw 16-bit data for 'A' in System vs Gold so we can see if
        // Gold stores pre-colored pixels or a blank coverage mask (in which
        // case the renderer has to tint by FONT.DEF COLOR).
        if (FontTable)
        {
            for (const char* fn : { "System", "Gold" })
            {
                TFont* f = FontTable->Bitmap(fn);
                if (!f) continue;
                PTBitmap bm = f->GetChar('A');
                if (!bm || bm->width <= 0 || bm->height <= 0) continue;
                const uint16_t* s = bm->data16;
                // Dump several rows so we can see actual pixel values, not
                // just the sparse top of 'A'.
                for (int r = 0; r < bm->height && r < 8; r++)
                {
                    char row[256]; int p = 0;
                    const int n = bm->width > 14 ? 14 : bm->width;
                    for (int i = 0; i < n; i++)
                        p += snprintf(row + p, sizeof(row) - p, "%04x ",
                            s[r * bm->width + i]);
                    log_info("[font/diag] '%s' 'A' row%d: %s", fn, r, row);
                }
            }
        }
        return true;
    }

    log_error("[test] unknown mode '%s' — falling back to blank", mode);
    return true;
}

void TTestScreen::Close()
{
    if (g_iconImg.id)
    {
        sg_destroy_image(g_iconImg);
        g_iconImg = {};
    }
    if (g_uiAtlas.id) { sg_destroy_image(g_uiAtlas); g_uiAtlas = {}; }
    if (g_tileColorImg.id) { sg_destroy_image(g_tileColorImg); g_tileColorImg = {}; }
    if (g_tileDepthImg.id) { sg_destroy_image(g_tileDepthImg); g_tileDepthImg = {}; }
    for (auto& t : g_sectorTileTex)
    {
        if (t.color.id) sg_destroy_image(t.color);
        if (t.depth.id) sg_destroy_image(t.depth);
    }
    g_sectorTileTex.clear();
    g_sectorTileInst.clear();
    if (g_sectorKept) { TSector::CloseSector(g_sectorKept); g_sectorKept = nullptr; }
    g_uiItemCount  = 0;
    g_uiFrameCount = 0;
    log_info("[test] close (real frames=%lld, legacy frames=%lld, time=%.3fs)",
        static_cast<long long>(TTime::FrameCount()),
        static_cast<long long>(TTime::LegacyFrameCount()),
        TTime::Time());
}

void TTestScreen::Pulse()
{
    // ticker mode: sample TTime once per legacy second so we can eyeball the
    // counters without flooding the log.
    if (strcmp(StartupTestMode, "ticker") == 0)
    {
        int64_t now = TTime::LegacyFrameCount();
        if (now / TTime::LegacyFramerate != lastLoggedLegacyFrame / TTime::LegacyFramerate)
        {
            log_info("[ticker] t=%.3fs dt=%.4fs real=%lld legacy=%lld",
                TTime::Time(), TTime::DeltaTime(),
                static_cast<long long>(TTime::FrameCount()),
                static_cast<long long>(now));
            lastLoggedLegacyFrame = now;
        }
    }
}

void TTestScreen::DrawBackground() {}

void TTestScreen::Animate(bool)
{
    if (!Display || !Display->BackBuffer())
        return;

    // Full sector render: iso-projected tile list built in Initialize.
    // One tile pass per frame — depth-clear plus LEQUAL lets per-pixel
    // depth (anchor_z + sampled_delta * z_scale) from each fragment order
    // all tiles correctly without any painter sort on the CPU side.
    if (strcmp(StartupTestMode, "sector") == 0 && !g_sectorTileInst.empty())
    {
        // Persistent tuning state. Defaults match the values we dialed in
        // by hand; the panel lets us sweep without recompiling. `animate`
        // rotates the light in the XZ plane so the scene visibly breathes
        // when the checkbox is on.
        static float dir[3]       = { 1.0f, -0.3f, 0.0f };
        static float intensity    = 1.4f;
        static float color[3]     = { 1.0f, 1.0f, 1.0f };
        static float ambient      = 0.15f;
        static float deriv_scale  = 8192.0f;
        static float z_scale      = 8.0f;
        static bool  animate      = true;
        static int32_t tick       = 0;
        static int32_t view_mode    = 0;   // 0=lit 1=albedo 2=depth 3=normals
        static bool    lights_on    = true;
        static float   radius_mul   = 1.0f;
        static float   intensity_mul = 1.0f;
        static float   z_above      = 60.0f;

        if (animate)
        {
            ++tick;
            const float t = float(tick) * (6.2831853f / 180.0f);
            dir[0] = std::cos(t);
            dir[2] = std::sin(t);
        }

        ImGui::SetNextWindowSize(ImVec2(320, 0), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Sector lighting"))
        {
            ImGui::TextUnformatted("view:");
            ImGui::SameLine(); if (ImGui::RadioButton("lit",     view_mode == 0)) view_mode = 0;
            ImGui::SameLine(); if (ImGui::RadioButton("albedo",  view_mode == 1)) view_mode = 1;
            ImGui::SameLine(); if (ImGui::RadioButton("depth",   view_mode == 2)) view_mode = 2;
            ImGui::SameLine(); if (ImGui::RadioButton("normals", view_mode == 3)) view_mode = 3;
            ImGui::Separator();
            ImGui::Checkbox("animate (rotate dir in XZ)", &animate);
            ImGui::SliderFloat3("dir (xyz)", dir, -1.0f, 1.0f);
            ImGui::SliderFloat("intensity",  &intensity,   0.0f, 4.0f);
            ImGui::ColorEdit3("color",       color);
            ImGui::SliderFloat("ambient",    &ambient,     0.0f, 10.0f);
            ImGui::SliderFloat("deriv_scale", &deriv_scale, 1.0f, 32768.0f, "%.0f", ImGuiSliderFlags_Logarithmic);
            ImGui::SliderFloat("z_scale",    &z_scale,     0.0f, 100.0f, "%.4f", ImGuiSliderFlags_Logarithmic);

            ImGui::Separator();
            ImGui::Text("point lights (%zu from sector)", g_sectorLights.size());
            ImGui::Checkbox("lights enabled",    &lights_on);
            ImGui::SliderFloat("radius x",        &radius_mul,    0.1f, 8.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
            ImGui::SliderFloat("intensity x",     &intensity_mul, 0.0f, 100.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
            ImGui::SliderFloat("z above (all)",   &z_above,      -500.0f, 500.0f);
            if (ImGui::TreeNode("per-light"))
            {
                for (size_t i = 0; i < g_sectorLights.size(); ++i)
                {
                    SSectorLight& L = g_sectorLights[i];
                    ImGui::PushID(int(i));
                    ImGui::Checkbox("##on", &L.enabled);
                    ImGui::SameLine();
                    ImGui::Text("%zu %s  r=%.0f  (%.2f,%.2f,%.2f) int=%.2f",
                                i, L.classname ? L.classname : "?",
                                L.radius, L.col[0], L.col[1], L.col[2], L.intensity);
                    ImGui::PopID();
                }
                ImGui::TreePop();
            }
        }
        ImGui::End();

        Display->SetLight(dir[0], dir[1], dir[2], intensity,
                          color[0], color[1], color[2], ambient,
                          deriv_scale);
        Display->SetTileViewMode(view_mode);
        Display->ClearPointLights();
        if (lights_on)
        {
            for (const SSectorLight& L : g_sectorLights)
            {
                if (!L.enabled) continue;
                Display->AddPointLight(L.x, L.y, z_above,
                                       L.radius * radius_mul,
                                       L.col[0], L.col[1], L.col[2],
                                       L.intensity * intensity_mul);
            }
        }

        Display->BeginTilePass(0.12f, 0.16f, 0.10f, 1.0f);  // dark-green placeholder ground
        for (const auto& inst : g_sectorTileInst)
        {
            const auto& tex = g_sectorTileTex[inst.tex_idx];
            Display->DrawTile(tex.color, tex.depth,
                              inst.dst_x, inst.dst_y, tex.w, tex.h,
                              inst.anchor_z, z_scale);
        }
        Display->EndTilePass();
        return;
    }

    // UI mode: one Composite per item into the shared atlas. Cells are sized
    // to the largest frame across all items so each cell is uniform; animated
    // items cycle frames in place at a fixed rate (no per-anim timing data
    // on TAnimationData — the retail engine drove this off global tick rate).
    if (strcmp(StartupTestMode, "ui") == 0 && g_uiAtlas.id && g_uiItemCount > 0)
    {
        const int32_t tw = Display->Width();
        const int32_t th = Display->Height();

        // Square-ish NxN grid so scrolling stays a compact block. ceil(sqrt)
        // gives the smallest N with N*N >= itemCount; any trailing empty
        // cells in the last row just render nothing.
        int32_t cols = 1;
        while (cols * cols < g_uiItemCount) cols++;
        const int32_t rows = cols;

        // Fit cell size to the viewport rather than the largest item — the
        // registry has a few outsized thumbnails that would blow the grid
        // out of the window otherwise. Oversized icons just overlap their
        // neighbors; most entries are small enough to sit in a 72px cell.
        constexpr int32_t pad = 4;
        int32_t cell_w = (tw - pad) / cols; if (cell_w < 16) cell_w = 16;
        int32_t cell_h = cell_w;
        const int32_t grid_w = cols * cell_w;
        const int32_t total_h = rows * cell_h;
        const int32_t ox = (tw - grid_w) / 2;

        constexpr double kAnimHz    = 8.0;
        constexpr double kScrollPxS = 40.0;
        const int32_t tick   = (int32_t)(TTime::Time() * kAnimHz);
        // Wrap scroll to [0, total_h). Two copies of the grid (k=0,1) tile
        // the viewport seamlessly no matter how the grid compares to th.
        const int32_t scroll = total_h > 0
            ? ((int32_t)(TTime::Time() * kScrollPxS) % total_h + total_h) % total_h
            : 0;

        Display->BackBuffer()->StartPass(0.0f, 0.0f, 0.0f, 1.0f);
        for (int i = 0; i < g_uiItemCount; i++)
        {
            const auto& it = g_uiItems[i];
            const int32_t fi = it.firstFrame +
                (it.numFrames > 1 ? (tick % it.numFrames) : 0);
            const auto& fr = g_uiFrames[fi];

            const int32_t col = i % cols;
            const int32_t row = i / cols;
            const int32_t cx  = ox + col * cell_w + cell_w / 2;
            const int32_t dx  = cx - fr.w / 2;

            for (int k = 0; k < 2; k++)
            {
                const int32_t cy = row * cell_h + cell_h / 2 - scroll + k * total_h;
                const int32_t dy = cy - fr.h / 2;
                if (dy + fr.h <= 0 || dy >= th) continue;
                Display->Composite(
                    g_uiAtlas,
                    dx, dy, fr.w, fr.h,
                    tw, th,
                    fr.ax, fr.ay, fr.w, fr.h,
                    g_uiAtlasW, g_uiAtlasH);
            }
        }
        Display->BackBuffer()->EndPass();
        return;
    }

    // Icon mode: clear backbuffer to black, draw the baked icon centered.
    if (strcmp(StartupTestMode, "icon") == 0 && g_iconImg.id)
    {
        const int32_t tw = Display->Width();
        const int32_t th = Display->Height();
        const int32_t dx = (tw - g_iconWidth)  / 2;
        const int32_t dy = (th - g_iconHeight) / 2;

        Display->BackBuffer()->StartPass(0.0f, 0.0f, 0.0f, 1.0f);
        Display->Composite(g_iconImg, dx, dy, g_iconWidth, g_iconHeight, tw, th);
        Display->BackBuffer()->EndPass();
        return;
    }

    // TTF mode: draw one line per rasterized TrueType atlas. Per-glyph
    // placement comes from stb's packed metrics cached on the atlas:
    //   dst_x = round(pen_x + xoff)  dst_y = round(pen_y + yoff)
    //   pen_x += xadvance
    // pen_y is the baseline, so yoff is negative for ascenders.
    if (strcmp(StartupTestMode, "ttf") == 0)
    {
        const int32_t tw = Display->Width();
        const int32_t th = Display->Height();
        const char* msg = "Revenant 0123 !?";
        struct STtfLine { const char* path; int size; };
        const STtfLine lines[] = {
            { "/System/Library/Fonts/Supplemental/Arial.ttf",           28 },
            { "/System/Library/Fonts/Supplemental/Times New Roman.ttf", 48 },
        };

        Display->BackBuffer()->StartPass(0.0f, 0.0f, 0.0f, 1.0f);

        int32_t baseline = 20 + lines[0].size;
        for (const auto& L : lines)
        {
            const SFontAtlas* atlas = BuildTTFAtlas(L.path, L.size);
            if (!atlas || !atlas->image.id) continue;

            float line_w = 0.0f;
            for (const char* p = msg; *p; ++p)
                line_w += atlas->Rect((unsigned char)*p).xadvance;

            float pen_x = (float)(tw - (int32_t)line_w) * 0.5f;
            const float pen_y = (float)baseline;

            for (const char* p = msg; *p; ++p)
            {
                const auto& r = atlas->Rect((unsigned char)*p);
                if (r.w > 0 && r.h > 0)
                {
                    // dst quad is logical-pixel (xoff2-xoff × yoff2-yoff);
                    // src rect r.w × r.h is 2× that because the atlas is
                    // rasterized with 2× oversampling.
                    const int32_t gx = (int32_t)(pen_x + r.xoff + 0.5f);
                    const int32_t gy = (int32_t)(pen_y + r.yoff + 0.5f);
                    const int32_t gw = (int32_t)(r.xoff2 - r.xoff + 0.5f);
                    const int32_t gh = (int32_t)(r.yoff2 - r.yoff + 0.5f);
                    Display->Composite(
                        atlas->image,
                        gx, gy, gw, gh,
                        tw, th,
                        r.x, r.y, r.w, r.h,
                        atlas->width, atlas->height);
                }
                pen_x += r.xadvance;
            }
            baseline += L.size + 16;
        }

        Display->BackBuffer()->EndPass();
        return;
    }

    // Text mode: render one line per font via per-glyph atlas composites.
    // Exercises the uv_rect sub-rect path end-to-end and verifies that
    // startheight properly aligns ascenders/descenders on a shared baseline.
    if (strcmp(StartupTestMode, "text") == 0 && FontTable)
    {
        const int32_t tw = Display->Width();
        const int32_t th = Display->Height();
        const char* msg = "Revenant 0123 !?.";

        struct SLine { TFont* font; const SFontAtlas* atlas; };
        SLine lines[2] = {
            { SystemFont,              SystemFont              ? BuildFontAtlas(SystemFont)              : nullptr },
            { FontTable->Bitmap("Gold"), FontTable->Bitmap("Gold") ? BuildFontAtlas(FontTable->Bitmap("Gold")) : nullptr },
        };

        // Vertical layout: stack line_h rows centered with a small gap.
        int32_t total_h = 0;
        for (const auto& L : lines)
            if (L.font) total_h += (int32_t)((TFontData*)L.font)->height + 4;

        Display->BackBuffer()->StartPass(0.0f, 0.0f, 0.0f, 1.0f);

        int32_t line_top = (th - total_h) / 2;
        for (const auto& L : lines)
        {
            if (!L.font || !L.atlas || !L.atlas->image.id) continue;

            const int32_t line_h = (int32_t)((TFontData*)L.font)->height;

            // Advance per glyph = DrawRight - DrawLeft (matches retail
            // graphics.cpp TextDraw). Pen y is the baseline; glyph top =
            // pen_y - StartHeight(ch). We treat pen_y = line_top + line_h
            // so the tallest glyphs approximately fill the line box.
            int32_t line_w = 0;
            for (const char* p = msg; *p; ++p)
            {
                const unsigned char ch = (unsigned char)*p;
                line_w += (int32_t)L.font->DrawRight(ch) - (int32_t)L.font->DrawLeft(ch);
            }

            int32_t pen_x = (tw - line_w) / 2;
            const int32_t pen_y = line_top + line_h;
            for (const char* p = msg; *p; ++p)
            {
                const unsigned char ch = (unsigned char)*p;
                const auto& r = L.atlas->Rect(ch);
                if (r.w > 0 && r.h > 0)
                {
                    const int32_t gx = pen_x - (int32_t)L.font->DrawLeft(ch);
                    const int32_t gy = pen_y - (int32_t)L.font->StartHeight(ch);
                    Display->Composite(
                        L.atlas->image,
                        gx, gy, r.w, r.h,
                        tw, th,
                        r.x, r.y, r.w, r.h,
                        L.atlas->width, L.atlas->height);
                }
                pen_x += (int32_t)L.font->DrawRight(ch) - (int32_t)L.font->DrawLeft(ch);
            }

            line_top += line_h + 4;
        }

        Display->BackBuffer()->EndPass();
        return;
    }

    // Default (blank/ticker/unknown): cycling clear color — the original
    // bring-up test that proves StartPass/EndPass/FlipPage are alive.
    const double t = TTime::Time();
    const float r = 0.5f + 0.5f * float(std::sin(t * 1.0));
    const float g = 0.5f + 0.5f * float(std::sin(t * 1.3 + 2.0));
    const float b = 0.5f + 0.5f * float(std::sin(t * 1.7 + 4.0));

    Display->BackBuffer()->StartPass(r, g, b, 1.0f);
    Display->BackBuffer()->EndPass();
}

void TTestScreen::KeyPress(int32_t key, bool down)
{
    if (down && key == VK_ESCAPE)
        SetDone();
}
