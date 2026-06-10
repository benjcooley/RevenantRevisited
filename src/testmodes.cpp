// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   testmodes.cpp - Test mode helpers                   *
// *************************************************************************

#include "testmodes.h"

#include <sokol_app.h>          // sapp_request_quit on script drain
#include <stb_image_write.h>   // for i3ddump test mode (impl lives in framesnap.cpp)

#include "3dimage.h"
#include "animimage.h"
#include "audio_backend.h"
#include "bitmapatlas.h"
#include "bitmapdecode.h"
#include "decompdata.h"
#include "display.h"
#include "font.h"
#include "fonttable.h"
#include "framesnap.h"
#include "hudstate.h"
#include "imagery.h"
#include "imageres.h"
#include "imgui.h"
#include "chunkcache.h"
#include "character.h"
#include "cursor.h"
#include "logging.h"
#include "maprenderer.h"
#include "meshextract.h"
#include "multi.h"
#include "render_metadata.h"
#include "renderer.h"
#include "revenant.h"
#include "sound.h"
#include "testconfig.h"
#include "time.h"
#include "tile.h"
#include "uidragstate.h"
#include "uianchortest.h"
#include "uibarinvtest.h"
#include "uibottombartest.h"
#include "uicliptest.h"
#include "uideathtest.h"
#include "uiequiptest.h"
#include "uihudtest.h"
#include "uidefscreentest.h"
#include "uiinventorytest.h"
#include "uilayouttest.h"
#include "uiloadscreentest.h"
#include "uimainmenutest.h"
#include "uimaptest.h"
#include "uinineslicetest.h"
#include "uiplyrstatusbartest.h"
#include "uiquickspelltest.h"
#include "uisidebartest.h"
#include "uisidetabstest.h"
#include "uiscrollpanetest.h"
#include "uispellbooktest.h"
#include "uispellcreatetest.h"
#include "uistatstest.h"
#include "uistyletest.h"
#include "uitextbartest.h"
#include "vfxtest.h"
// uihudmockuptest + uistatusbartest moved to attic/: the rects/labels they
// encoded were derived from the pre-release src/ defines (HEALTHBARX etc)
// and the AI-generated docs/HUD.md, neither of which match the shipped
// retail HUD layout. See docs/ui/BURNDOWN.md "Phase B retraction" note.

#include <cmath>
#include <cctype>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

extern TObjectClass TileClass;
extern TObjectClass CharacterClass;
extern TObjectClass PlayerClass;

extern char ResourcePath[];
extern char RunPath[];

namespace {

TMapRenderer g_mapRenderer;
SBitmapAtlas g_uiAtlas;

std::string SanitizeFilenameComponent(const char* name)
{
    std::string s = name ? name : "unnamed";
    for (char& ch : s)
    {
        const unsigned char u = static_cast<unsigned char>(ch);
        if (!(std::isalnum(u) || ch == '_' || ch == '-' || ch == '.'))
            ch = '_';
    }
    if (s.empty())
        s = "unnamed";
    return s;
}

void MatrixMul16(const float a[16], const float b[16], float out[16])
{
    for (int32_t r = 0; r < 4; ++r)
    {
        for (int32_t c = 0; c < 4; ++c)
        {
            float s = 0.0f;
            for (int32_t i = 0; i < 4; ++i)
                s += a[r * 4 + i] * b[i * 4 + c];
            out[r * 4 + c] = s;
        }
    }
}

// Shared minimal-PNG writer + bitmap decoder for the asset dump modes
// (--dumptiles, --dumpicons).
namespace fs = std::filesystem;

auto append_be32 = [](std::vector<uint8_t>& out, uint32_t v) {
    out.push_back(uint8_t((v >> 24) & 0xFF));
    out.push_back(uint8_t((v >> 16) & 0xFF));
    out.push_back(uint8_t((v >>  8) & 0xFF));
    out.push_back(uint8_t((v      ) & 0xFF));
};
auto crc32_bytes = [](const uint8_t* data, size_t len) -> uint32_t {
    static uint32_t table[256] = {};
    static bool init = false;
    if (!init) {
        for (uint32_t i = 0; i < 256; ++i) {
            uint32_t c = i;
            for (int k = 0; k < 8; ++k)
                c = (c & 1) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);
            table[i] = c;
        }
        init = true;
    }
    uint32_t c = 0xFFFFFFFFu;
    for (size_t i = 0; i < len; ++i)
        c = table[(c ^ data[i]) & 0xFFu] ^ (c >> 8);
    return c ^ 0xFFFFFFFFu;
};
auto adler32_bytes = [](const uint8_t* data, size_t len) -> uint32_t {
    uint32_t s1 = 1, s2 = 0;
    for (size_t i = 0; i < len; ++i) {
        s1 = (s1 + data[i]) % 65521u;
        s2 = (s2 + s1) % 65521u;
    }
    return (s2 << 16) | s1;
};
auto write_png_rgba = [](const fs::path& path, int32_t w, int32_t h, const std::vector<uint8_t>& rgba) -> bool {
    if (w <= 0 || h <= 0 || rgba.size() != size_t(w) * size_t(h) * 4) return false;
    std::vector<uint8_t> raw;
    raw.reserve(size_t(h) * (size_t(w) * 4 + 1));
    for (int32_t y = 0; y < h; ++y) {
        raw.push_back(0); // filter type 0
        const uint8_t* row = rgba.data() + size_t(y) * size_t(w) * 4;
        raw.insert(raw.end(), row, row + size_t(w) * 4);
    }

    std::vector<uint8_t> zlib;
    zlib.reserve(raw.size() + raw.size() / 65535 * 5 + 16);
    zlib.push_back(0x78);
    zlib.push_back(0x01);
    size_t off = 0;
    while (off < raw.size()) {
        const size_t chunk = (std::min)(size_t(65535), raw.size() - off);
        const bool final = (off + chunk) == raw.size();
        zlib.push_back(final ? 0x01 : 0x00);
        zlib.push_back(uint8_t(chunk & 0xFF));
        zlib.push_back(uint8_t((chunk >> 8) & 0xFF));
        const uint16_t nlen = uint16_t(~uint16_t(chunk));
        zlib.push_back(uint8_t(nlen & 0xFF));
        zlib.push_back(uint8_t((nlen >> 8) & 0xFF));
        zlib.insert(zlib.end(), raw.begin() + ptrdiff_t(off), raw.begin() + ptrdiff_t(off + chunk));
        off += chunk;
    }
    append_be32(zlib, adler32_bytes(raw.data(), raw.size()));

    std::vector<uint8_t> png;
    const uint8_t sig[8] = { 137,80,78,71,13,10,26,10 };
    png.insert(png.end(), sig, sig + 8);

    auto append_chunk = [&](const char type[4], const std::vector<uint8_t>& payload) {
        append_be32(png, uint32_t(payload.size()));
        const size_t type_off = png.size();
        png.push_back(uint8_t(type[0]));
        png.push_back(uint8_t(type[1]));
        png.push_back(uint8_t(type[2]));
        png.push_back(uint8_t(type[3]));
        png.insert(png.end(), payload.begin(), payload.end());
        const uint32_t crc = crc32_bytes(png.data() + type_off, 4 + payload.size());
        append_be32(png, crc);
    };

    std::vector<uint8_t> ihdr;
    ihdr.reserve(13);
    append_be32(ihdr, uint32_t(w));
    append_be32(ihdr, uint32_t(h));
    ihdr.push_back(8); // bit depth
    ihdr.push_back(6); // RGBA
    ihdr.push_back(0); // compression
    ihdr.push_back(0); // filter
    ihdr.push_back(0); // interlace
    append_chunk("IHDR", ihdr);
    append_chunk("IDAT", zlib);
    append_chunk("IEND", {});

    std::ofstream f(path, std::ios::binary);
    if (!f) return false;
    f.write((const char*)png.data(), std::streamsize(png.size()));
    return f.good();
};
auto decode_bitmap_rgba = [](PTBitmap bm, std::vector<uint8_t>& rgba_out,
                                  std::vector<float>* zraw_out = nullptr,
                                  std::vector<uint8_t>* alpha_out = nullptr,
                                  int32_t* valid_out = nullptr,
                                  float* zmin_out = nullptr,
                                  float* zmax_out = nullptr) -> bool {
    if (!bm || bm->width <= 0 || bm->height <= 0) return false;
    const bool is_8bit = (bm->flags & BM_8BIT) != 0;
    const bool is_16bit = (bm->flags & (BM_15BIT | BM_16BIT)) != 0;
    const bool has_zbuffer = (bm->flags & BM_ZBUFFER) != 0;
    if (!is_8bit && !is_16bit) return false;
    SPalette* pal = (SPalette*)bm->palette.ptr();
    if (is_8bit && !pal) return false;
    const int32_t w = bm->width, h = bm->height;
    const size_t npx = size_t(w) * size_t(h);
    const uint8_t key8 = (uint8_t)bm->keycolor;
    const uint16_t key16 = (uint16_t)bm->keycolor;
    std::unique_ptr<uint8_t[]> idxplane;
    std::unique_ptr<uint16_t[]> rgbplane16;
    if (is_8bit) idxplane.reset(new uint8_t[npx]); else rgbplane16.reset(new uint16_t[npx]);
    std::unique_ptr<uint16_t[]> zplane;
    if (has_zbuffer) zplane.reset(new uint16_t[npx]);

    if (bm->flags & BM_CHUNKED)
    {
        if (!bm->CacheChunks()) return false;
        SChunkHeader* hdr = (SChunkHeader*)(void*)bm->data8;
        SChunkHeader* zhdr = has_zbuffer ? (SChunkHeader*)bm->zbuffer.ptr() : nullptr;
        if (!hdr || (has_zbuffer && !zhdr)) return false;
        const int32_t cw = hdr->width, ch = hdr->height;
        if (is_8bit) std::memset(idxplane.get(), key8, npx);
        else std::fill_n(rgbplane16.get(), npx, key16);
        if (has_zbuffer) for (size_t i = 0; i < npx; ++i) zplane[i] = 0;
        for (int32_t by = 0; by < ch; ++by)
        for (int32_t bx = 0; bx < cw; ++bx)
        {
            void* cptr = hdr->block[by * cw + bx].ptr();
            void* zptr = zhdr ? zhdr->block[by * cw + bx].ptr() : nullptr;
            const uint8_t*  c8  = (is_8bit && cptr) ? (const uint8_t*)ChunkCache.AddChunk(cptr, 1) : nullptr;
            const uint16_t* c16 = (is_16bit && cptr) ? (const uint16_t*)ChunkCache.AddChunk16(cptr, 1) : nullptr;
            const uint16_t* z16 = (has_zbuffer && zptr) ? (const uint16_t*)ChunkCache.AddChunkZ(zptr, 2) : nullptr;
            const int32_t x0 = bx * CHUNKWIDTH, y0 = by * CHUNKHEIGHT;
            const int32_t cxmax = (w - x0 < CHUNKWIDTH) ? (w - x0) : CHUNKWIDTH;
            const int32_t cymax = (h - y0 < CHUNKHEIGHT) ? (h - y0) : CHUNKHEIGHT;
            if (cxmax <= 0 || cymax <= 0) continue;
            for (int32_t y = 0; y < cymax; ++y)
            {
                if (c8) std::memcpy(&idxplane[(y0 + y) * w + x0], &c8[y * CHUNKWIDTH], cxmax);
                if (c16) std::memcpy(&rgbplane16[(y0 + y) * w + x0], &c16[y * CHUNKWIDTH], cxmax * sizeof(uint16_t));
                if (z16) std::memcpy(&zplane[(y0 + y) * w + x0], &z16[y * CHUNKWIDTH], cxmax * sizeof(uint16_t));
            }
        }
    }
    else
    {
        if (is_8bit) std::memcpy(idxplane.get(), bm->data8, npx);
        else std::memcpy(rgbplane16.get(), bm->data16, npx * sizeof(uint16_t));
        if (has_zbuffer)
        {
            uint16_t* zbuf = (uint16_t*)bm->zbuffer.ptr();
            if (zbuf) std::memcpy(zplane.get(), zbuf, npx * sizeof(uint16_t));
        }
    }

    rgba_out.assign(npx * 4, 0);
    if (zraw_out) zraw_out->assign(npx, 0.0f);
    if (alpha_out) alpha_out->assign(npx, 0);
    int32_t valid = 0;
    float zmin = FLT_MAX;
    float zmax = -FLT_MAX;
    for (size_t i = 0; i < npx; ++i)
    {
        const uint8_t idx8 = is_8bit ? idxplane[i] : 0;
        const uint16_t px16 = is_16bit ? rgbplane16[i] : 0;
        const uint16_t z = has_zbuffer ? zplane[i] : 1;
        const bool transparent = has_zbuffer ? (z == 0 || z == 0x7F7F)
                                             : (is_8bit ? (idx8 == key8) : (px16 == key16));
        uint8_t* row = rgba_out.data() + i * 4;
        if (transparent)
            continue;
        ++valid;
        if (alpha_out) (*alpha_out)[i] = 255;
        const float zraw = has_zbuffer ? float(int16_t(z)) : 0.0f;
        if (zraw_out) (*zraw_out)[i] = zraw;
        zmin = (std::min)(zmin, zraw);
        zmax = (std::max)(zmax, zraw);
        if (is_8bit)
        {
            const uint32_t c = pal->rgbcolors[idx8];
            row[0] = (uint8_t)( c        & 0xFF);
            row[1] = (uint8_t)((c >> 8)  & 0xFF);
            row[2] = (uint8_t)((c >> 16) & 0xFF);
        }
        else
        {
            row[0] = (uint8_t)(((px16 >> 10) & 0x1F) << 3);
            row[1] = (uint8_t)(((px16 >> 5)  & 0x1F) << 3);
            row[2] = (uint8_t)(( px16        & 0x1F) << 3);
        }
        row[3] = 255;
    }
    if (valid == 0) { zmin = 0.0f; zmax = 0.0f; }
    if (valid_out) *valid_out = valid;
    if (zmin_out) *zmin_out = zmin;
    if (zmax_out) *zmax_out = zmax;
    return true;
};

bool DumpTilesToPath(const char* out_path_cstr)
{
    namespace fs = std::filesystem;
    const fs::path out_dir = (out_path_cstr && out_path_cstr[0])
        ? fs::path(out_path_cstr)
        : (fs::current_path() / "tile_dump_albedo");
    std::error_code ec;
    fs::create_directories(out_dir, ec);
    if (ec)
    {
        log_error("[tiledump] create_directories failed for '%s': %s",
                  out_dir.string().c_str(), ec.message().c_str());
        return false;
    }

    int dumped = 0;
    int failed = 0;
    std::ofstream report(out_dir / "tile_z_report.csv");
    if (report)
        report << "objtype,name,width,height,valid,zmin,zmax,expected_flat_zmin,expected_flat_zmax,mean_abs_flat_error,max_abs_flat_error,flat_fit_offset,mean_abs_fit_error,max_abs_fit_error\n";

    for (int32_t objtype = 0; objtype < TileClass.NumTypes(); ++objtype)
    {
        SObjectInfo* info = TileClass.GetObjType(objtype);
        if (!info || !info->name)
            continue;

        TObjectImagery* imagery = TObjectImagery::LoadImagery(info->imageryid);
        if (!imagery)
        {
            ++failed;
            log_warn("[tiledump] imagery load failed for tile[%d] '%s'", objtype, info->name);
            continue;
        }

        TAnimImagery* anim = dynamic_cast<TAnimImagery*>(imagery);
        if (!anim || anim->NumStates() <= 0)
        {
            ++failed;
            log_warn("[tiledump] imagery is not anim/still for tile[%d] '%s'", objtype, info->name);
            continue;
        }
        PTBitmap bm = anim->GetStillImage(0);
        if (!bm)
        {
            ++failed;
            log_warn("[tiledump] no still image for tile[%d] '%s'", objtype, info->name);
            continue;
        }
        std::vector<uint8_t> rgba;
        std::vector<float> zraw;
        std::vector<uint8_t> alpha;
        int32_t valid_count = 0;
        float zmin = 0.0f, zmax = 0.0f;
        if (!decode_bitmap_rgba(bm, rgba, &zraw, &alpha, &valid_count, &zmin, &zmax))
        {
            ++failed;
            log_warn("[tiledump] decode failed for tile[%d] '%s' flags=0x%x", objtype, info->name, bm->flags);
            continue;
        }

        char stem[256];
        const std::string safe_name = SanitizeFilenameComponent(info->name);
        std::snprintf(stem, sizeof(stem), "%04d_%s.png", objtype, safe_name.c_str());
        const fs::path out_path = out_dir / stem;
        if (!write_png_rgba(out_path, bm->width, bm->height, rgba))
        {
            ++failed;
            log_warn("[tiledump] png write failed for tile[%d] '%s'", objtype, info->name);
            continue;
        }

        {
            std::snprintf(stem, sizeof(stem), "%04d_%s.ztile", objtype, safe_name.c_str());
            std::ofstream zf(out_dir / stem, std::ios::binary);
            if (zf)
            {
                const uint32_t magic = 0x455A5452u; // RTZE little-endian marker
                const int32_t w = bm->width;
                const int32_t h = bm->height;
                zf.write((const char*)&magic, sizeof(magic));
                zf.write((const char*)&w, sizeof(w));
                zf.write((const char*)&h, sizeof(h));
                zf.write((const char*)rgba.data(), std::streamsize(rgba.size()));
                zf.write((const char*)zraw.data(), std::streamsize(zraw.size() * sizeof(float)));
            }
        }

        std::vector<uint8_t> alpha_rgba(size_t(bm->width) * size_t(bm->height) * 4, 0);
        std::vector<uint8_t> depth_rgba(size_t(bm->width) * size_t(bm->height) * 4, 0);
        const float span = (zmax > zmin) ? (zmax - zmin) : 1.0f;
        double abs_sum = 0.0;
        double abs_max = 0.0;
        double offset_sum = 0.0;
        for (int32_t y = 0; y < bm->height; ++y)
        for (int32_t x = 0; x < bm->width; ++x)
        {
            const size_t i = size_t(y) * size_t(bm->width) + size_t(x);
            uint8_t* ar = alpha_rgba.data() + i * 4;
            uint8_t* dr = depth_rgba.data() + i * 4;
            ar[3] = dr[3] = 255;
            if (!alpha.empty() && alpha[i])
            {
                ar[0] = ar[1] = ar[2] = 255;
                const float t = (zraw[i] - zmin) / span;
                dr[0] = uint8_t(255.0f * t);
                dr[1] = uint8_t(128.0f + 127.0f * t);
                dr[2] = uint8_t(255.0f * (1.0f - t));
                // Expected flat square depth relative to center registration.
                const float local_y = float(y) + 0.5f - float(bm->height) * 0.5f;
                const float expected = -2.0f * 0.867f * local_y;
                offset_sum += double(zraw[i] - expected);
                const double e = std::fabs(double(zraw[i] - expected));
                abs_sum += e;
                if (e > abs_max) abs_max = e;
            }
        }
        const double fit_offset = valid_count > 0 ? offset_sum / double(valid_count) : 0.0;
        double fit_abs_sum = 0.0;
        double fit_abs_max = 0.0;
        for (int32_t y = 0; y < bm->height; ++y)
        for (int32_t x = 0; x < bm->width; ++x)
        {
            const size_t i = size_t(y) * size_t(bm->width) + size_t(x);
            if (alpha.empty() || !alpha[i]) continue;
            const float local_y = float(y) + 0.5f - float(bm->height) * 0.5f;
            const float expected = -2.0f * 0.867f * local_y;
            const double e = std::fabs(double(zraw[i]) - (double(expected) + fit_offset));
            fit_abs_sum += e;
            if (e > fit_abs_max) fit_abs_max = e;
        }

        std::snprintf(stem, sizeof(stem), "%04d_%s_alpha.png", objtype, safe_name.c_str());
        write_png_rgba(out_dir / stem, bm->width, bm->height, alpha_rgba);
        std::snprintf(stem, sizeof(stem), "%04d_%s_depth.png", objtype, safe_name.c_str());
        write_png_rgba(out_dir / stem, bm->width, bm->height, depth_rgba);
        if (report)
        {
            const float expected_min = -2.0f * 0.867f * (float(bm->height) - 0.5f - float(bm->height) * 0.5f);
            const float expected_max = -2.0f * 0.867f * (0.5f - float(bm->height) * 0.5f);
            const double mean_abs = valid_count > 0 ? abs_sum / double(valid_count) : 0.0;
            const double mean_fit_abs = valid_count > 0 ? fit_abs_sum / double(valid_count) : 0.0;
            report << objtype << ",\"" << (info->name ? info->name : "?") << "\","
                   << bm->width << "," << bm->height << ","
                   << valid_count << "," << zmin << "," << zmax << ","
                   << expected_min << "," << expected_max << ","
                   << mean_abs << "," << abs_max << ","
                   << fit_offset << "," << mean_fit_abs << "," << fit_abs_max << "\n";
        }

        ++dumped;
    }

    log_info("[tiledump] dumped=%d failed=%d folder='%s'",
             dumped, failed, out_dir.string().c_str());
    return dumped > 0;
}

bool InitializeTileDumpMode()
{
    return DumpTilesToPath(StartupDumpTilesPath);
}

// Walk every object class/type, load its imagery, and export every baked
// inventory icon (GetInvImage per state) as a PNG. This captures all item
// icons and character portraits (portrait = state 0 of the body imagery);
// only I3D imagery bakes icons, types without them are silently skipped.
bool DumpIconsToPath(const char* out_path_cstr)
{
    const fs::path out_dir = (out_path_cstr && out_path_cstr[0])
        ? fs::path(out_path_cstr)
        : (fs::current_path() / "icons");
    std::error_code ec;
    fs::create_directories(out_dir, ec);
    if (ec)
    {
        log_error("[icondump] create_directories failed for '%s': %s",
                  out_dir.string().c_str(), ec.message().c_str());
        return false;
    }

    int dumped = 0;
    int failed = 0;
    for (int32_t classid = 0; classid < TObjectClass::NumClasses(); ++classid)
    {
        const TObjectClass* cls = TObjectClass::GetClass(classid);
        if (!cls)
            continue;
        const std::string safe_class = SanitizeFilenameComponent(cls->ClassName());

        for (int32_t objtype = 0; objtype < cls->NumTypes(); ++objtype)
        {
            SObjectInfo* info = cls->GetObjType(objtype);
            if (!info || !info->name)
                continue;

            TObjectImagery* imagery = TObjectImagery::LoadImagery(info->imageryid);
            if (!imagery)
                continue;

            const std::string safe_name = SanitizeFilenameComponent(info->name);
            const int32_t nstates = imagery->NumStates();
            for (int32_t state = 0; state < nstates; ++state)
            {
                TBitmap* bm = imagery->GetInvImage(state);
                if (!bm)
                    continue;

                std::vector<uint8_t> rgba;
                if (!decode_bitmap_rgba(bm, rgba))
                {
                    ++failed;
                    log_warn("[icondump] decode failed for %s '%s' state=%d flags=0x%x",
                             cls->ClassName(), info->name, state, bm->flags);
                    continue;
                }

                // Chroma-key the magenta background to transparent. The .i3d-
                // baked portraits (CHARACTER/PLAYER) come back with an opaque
                // magenta fill instead of the keyed/zbuffered transparency that
                // item icons use, so decode_bitmap_rgba leaves it solid. Inv
                // icons are chroma-keyed by definition and real icon art never
                // uses pure (248,0,248) magenta, so keying it here is safe.
                for (size_t p = 0; p + 3 < rgba.size(); p += 4)
                {
                    if (rgba[p] >= 224 && rgba[p + 1] <= 32 && rgba[p + 2] >= 224)
                        rgba[p] = rgba[p + 1] = rgba[p + 2] = rgba[p + 3] = 0;
                }

                char stem[512];
                if (state == 0)
                    std::snprintf(stem, sizeof(stem), "%s_%04d_%s.png",
                                  safe_class.c_str(), objtype, safe_name.c_str());
                else
                    std::snprintf(stem, sizeof(stem), "%s_%04d_%s_s%02d.png",
                                  safe_class.c_str(), objtype, safe_name.c_str(), state);
                if (!write_png_rgba(out_dir / stem, bm->width, bm->height, rgba))
                {
                    ++failed;
                    log_warn("[icondump] png write failed for %s '%s' state=%d",
                             cls->ClassName(), info->name, state);
                    continue;
                }
                ++dumped;
            }
        }
    }

    log_info("[icondump] dumped=%d failed=%d folder='%s'",
             dumped, failed, out_dir.string().c_str());
    return dumped > 0;
}

// =========================================================================
// * i3ddump — extract everything from an I3D file into a folder.          *
// *                                                                       *
// * Per asset:                                                            *
// *   manifest.txt            — sub-object list, materials, texture refs *
// *   texture_NN_frame_FF.png — every decoded RGBA frame, every slot      *
// *   subobj_NN_<name>.obj    — Wavefront OBJ per sub-object               *
// *                                                                       *
// * CLI: --dumpi3d=Magic\\comet.I3D [--dumpi3dout=DIR]                    *
// * If --dumpi3dout omitted, defaults to ./i3d_dump/<asset_basename>/.    *
// *                                                                       *
// * Texture data is captured by setting                                   *
// * T3DImagery::g_retain_decoded_rgba = true BEFORE LoadImagery — the    *
// * loader retains decoded bytes per frame in S3DTex::dump_rgba_frames    *
// * (see 3dimage.cpp).                                                    *
// =========================================================================

bool DumpI3DToPath(const char* asset_path_cstr, const char* out_dir_cstr)
{
    namespace fs = std::filesystem;
    if (!asset_path_cstr || !asset_path_cstr[0])
    {
        log_error("[i3ddump] asset path is empty");
        return false;
    }

    // Compute output folder. Default: ./i3d_dump/<basename-no-extension>/
    fs::path out_dir;
    if (out_dir_cstr && out_dir_cstr[0])
    {
        out_dir = fs::path(out_dir_cstr);
    }
    else
    {
        std::string base = asset_path_cstr;
        // Strip directory (Win or Unix separators).
        size_t slash = base.find_last_of("/\\");
        if (slash != std::string::npos)
            base = base.substr(slash + 1);
        // Strip extension.
        size_t dot = base.find_last_of('.');
        if (dot != std::string::npos)
            base = base.substr(0, dot);
        out_dir = fs::current_path() / "i3d_dump" / base;
    }
    std::error_code ec;
    fs::create_directories(out_dir, ec);
    if (ec)
    {
        log_error("[i3ddump] create_directories failed for '%s': %s",
                  out_dir.string().c_str(), ec.message().c_str());
        return false;
    }

    // Enable CPU-side RGBA retention BEFORE LoadImagery so the texture
    // loader keeps decoded bytes per frame.
    T3DImagery::g_retain_decoded_rgba = true;

    int32_t img_id = TObjectImagery::FindImagery(asset_path_cstr);
    if (img_id < 0)
    {
        // Asset isn't registered in class.def's imagery table (e.g. legacy
        // / test assets like Magic\fireball.i3d that were replaced by
        // newfireball.i3d before ship). Try RegisterImagery as a fallback —
        // this adds the path to the lookup table on-the-fly.
        std::string path_copy = asset_path_cstr;
        img_id = TObjectImagery::RegisterImagery(path_copy.data());
        if (img_id < 0)
        {
            log_error("[i3ddump] FindImagery + RegisterImagery both failed for '%s'",
                      asset_path_cstr);
            return false;
        }
        log_info("[i3ddump]   '%s' not in class.def; registered as id=%d",
                 asset_path_cstr, img_id);
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    if (!base)
    {
        log_error("[i3ddump] LoadImagery(id=%d '%s') failed",
                  img_id, asset_path_cstr);
        return false;
    }
    T3DImagery* img3d = dynamic_cast<T3DImagery*>(base);
    if (!img3d)
    {
        log_error("[i3ddump] '%s' is not a T3DImagery (got %s)",
                  asset_path_cstr, typeid(*base).name());
        return false;
    }

    auto sanitize = [](const char* s) -> std::string {
        std::string r;
        if (!s) return std::string("unnamed");
        for (; *s; ++s) {
            const char c = *s;
            r.push_back((c == '/' || c == '\\' || c == ':' || c == ' ' || c == '\t') ? '_' : c);
        }
        if (r.empty()) r = "unnamed";
        return r;
    };

    // ---- manifest.txt ----
    std::ofstream mf(out_dir / "manifest.txt");
    mf << "asset: " << asset_path_cstr << "\n";
    mf << "NumObjects: " << img3d->NumObjects() << "\n";
    mf << "NumTextures: " << img3d->NumTextures() << "\n";
    mf << "NumMaterials: " << img3d->NumMaterials() << "\n";
    mf << "\n";

    // ---- textures → PNG ----
    // Frame data is on T3DImagery::dump_textures (NOT S3DTex — vector
    // members in S3DTex get corrupted by TVirtualArray's memcpy copy
    // semantics).
    //
    // Textures need a vertical flip on write because the I3D format
    // stores them in D3D's top-down convention (row 0 = top), but PNG
    // viewers expect bottom-up display semantics for the in-game look.
    // (Try toggling this if dumps come out mirrored.)
    stbi_flip_vertically_on_write(1);
    int32_t pngs_written = 0;
    for (int32_t t = 0; t < img3d->NumTextures(); ++t)
    {
        S3DTex tex = {};
        img3d->GetTexture(t, &tex);
        mf << "texture[" << t << "]: "
           << tex.desc.width << "x" << tex.desc.height
           << " frames=" << tex.numframes << "\n";
        const int32_t w = int32_t(tex.desc.width);
        const int32_t h = int32_t(tex.desc.height);
        if (t >= int32_t(img3d->dump_textures.size())) continue;
        const auto& frames = img3d->dump_textures[size_t(t)];
        for (int32_t f = 0; f < int32_t(frames.size()); ++f)
        {
            const std::vector<uint8_t>& rgba = frames[size_t(f)];
            if (rgba.empty() || w <= 0 || h <= 0) continue;
            if (int32_t(rgba.size()) != w * h * 4)
            {
                log_warn("[i3ddump]   texture[%d] frame[%d] size mismatch "
                         "(%zu vs %d)",
                         t, f, rgba.size(), w * h * 4);
                continue;
            }
            char stem[256];
            std::snprintf(stem, sizeof(stem), "texture_%02d_frame_%02d.png", t, f);
            const fs::path png_path = out_dir / stem;
            if (stbi_write_png(png_path.string().c_str(), w, h, 4,
                               rgba.data(), w * 4) == 0)
            {
                log_warn("[i3ddump] stbi_write_png failed: %s",
                         png_path.string().c_str());
                continue;
            }
            mf << "  frame[" << f << "]: " << stem << "\n";
            ++pngs_written;
        }
    }
    mf << "\n";

    // ---- materials ----
    for (int32_t m = 0; m < img3d->NumMaterials(); ++m)
    {
        S3DMat mat = {};
        img3d->GetMaterial(m, &mat);
        mf << "material[" << m << "]: "
           << "diffuse=(" << mat.matdesc.diffuse.r
           << "," << mat.matdesc.diffuse.g
           << "," << mat.matdesc.diffuse.b
           << "," << mat.matdesc.diffuse.a << ") "
           << "emissive=(" << mat.matdesc.emissive.r
           << "," << mat.matdesc.emissive.g
           << "," << mat.matdesc.emissive.b << ") "
           << "texture=" << mat.texture << "\n";
    }
    mf << "\n";

    // ---- sub-objects → single combined Wavefront OBJ ----
    // One <asset_basename>.obj per I3D, with `o NAME` markers per
    // sub-object. Blender + most DCC tools can drag-drop this directly
    // and see each sub-object as a separately named selectable mesh.
    //
    // Indexing rule (Wavefront): v/vt indices are 1-based and GLOBAL to
    // the file. We accumulate a `vert_base` offset as we walk
    // sub-objects so each block's `f` lines reference its own verts.
    std::string asset_stem = asset_path_cstr;
    {
        size_t slash = asset_stem.find_last_of("/\\");
        if (slash != std::string::npos) asset_stem = asset_stem.substr(slash + 1);
        size_t dot = asset_stem.find_last_of('.');
        if (dot != std::string::npos) asset_stem = asset_stem.substr(0, dot);
    }
    const fs::path combined_obj_path = out_dir / (asset_stem + ".obj");
    std::ofstream combined_obj(combined_obj_path);
    combined_obj << "# Revenant I3D dump\n";
    combined_obj << "# asset: " << asset_path_cstr << "\n";
    combined_obj << "# sub-objects: " << img3d->NumObjects() << "\n\n";

    int32_t objs_written = 0;
    int32_t global_vert_base = 0;
    for (int32_t o = 0; o < img3d->NumObjects(); ++o)
    {
        const char* name = img3d->GetObjectName(o);
        const std::string safe = sanitize(name);

        // Try every texture slot (matches d3d::RegisterSubMesh pattern).
        // Stop at the first slot that yields verts/indices. This handles
        // both textured sub-objects (UVs in the texture's slot) and
        // solid-fill ones (UVs in slot 0).
        std::vector<SMeshVertex> verts;
        std::vector<uint16_t>    indices;
        const int32_t num_tex = img3d->NumTextures();
        int32_t picked_slot = -1;
        for (int32_t slot = 0; slot < num_tex + 1; ++slot)
        {
            verts.clear();
            indices.clear();
            if (!ExtractSubMeshTextureSlot(img3d, o, slot, verts, indices))
                continue;
            if (verts.empty() || indices.empty()) continue;
            picked_slot = slot;
            break;
        }

        mf << "object[" << o << "]: name='" << (name ? name : "")
           << "' verts=" << verts.size()
           << " idxs=" << indices.size()
           << " texslot=" << picked_slot;

        // Bbox for manifest.
        if (!verts.empty())
        {
            float mn[3] = { verts[0].pos[0], verts[0].pos[1], verts[0].pos[2] };
            float mx[3] = { mn[0], mn[1], mn[2] };
            for (const auto& v : verts) {
                for (int32_t k = 0; k < 3; ++k) {
                    mn[k] = (std::min)(mn[k], v.pos[k]);
                    mx[k] = (std::max)(mx[k], v.pos[k]);
                }
            }
            mf << " bbox=(" << mn[0] << "," << mn[1] << "," << mn[2]
               << ")..(" << mx[0] << "," << mx[1] << "," << mx[2] << ")";
        }
        mf << "\n";

        if (verts.empty() || indices.empty())
            continue;

        // Append this sub-object as a named block in the combined OBJ.
        combined_obj << "# sub-object[" << o << "] name='" << (name ? name : "")
                     << "' texslot=" << picked_slot
                     << " verts=" << verts.size()
                     << " indices=" << indices.size() << "\n";
        combined_obj << "o " << safe << "\n";
        for (const auto& v : verts)
            combined_obj << "v " << v.pos[0] << " " << v.pos[1] << " "
                         << v.pos[2] << "\n";
        for (const auto& v : verts)
            combined_obj << "vt " << v.uv[0] << " " << v.uv[1] << "\n";
        for (size_t i = 0; i + 2 < indices.size(); i += 3) {
            const int32_t a = int32_t(indices[i + 0]) + 1 + global_vert_base;
            const int32_t b = int32_t(indices[i + 1]) + 1 + global_vert_base;
            const int32_t c = int32_t(indices[i + 2]) + 1 + global_vert_base;
            combined_obj << "f " << a << "/" << a
                         << " "  << b << "/" << b
                         << " "  << c << "/" << c << "\n";
        }
        combined_obj << "\n";
        global_vert_base += int32_t(verts.size());
        ++objs_written;
    }

    log_info("[i3ddump] dumped %d textures, %d sub-objects from '%s' -> %s",
             pngs_written, objs_written,
             asset_path_cstr, out_dir.string().c_str());
    return (pngs_written > 0) || (objs_written > 0);
}

bool InitializeI3DDumpMode()
{
    // Batch mode: --dumpi3d=@LIST_FILE — process one asset path per
    // non-empty/non-comment line. Output for each goes into
    // <DUMPI3DOUT>/<asset_basename>/. Bootstraps the engine once instead
    // of N times for catalog-wide dumps.
    if (StartupDumpI3DPath[0] == '@')
    {
        namespace fs = std::filesystem;
        const char* list_path = StartupDumpI3DPath + 1;
        std::ifstream list(list_path);
        if (!list)
        {
            log_error("[i3ddump] cannot open list file '%s'", list_path);
            return false;
        }
        const fs::path base_out = StartupDumpI3DOutPath[0]
            ? fs::path(StartupDumpI3DOutPath)
            : (fs::current_path() / "i3d_dump_all");
        std::error_code ec;
        fs::create_directories(base_out, ec);
        int32_t ok = 0, fail = 0;
        std::string line;
        while (std::getline(list, line))
        {
            // Trim whitespace.
            while (!line.empty() && (line.back() == '\r' || line.back() == '\n'
                                  || line.back() == ' '  || line.back() == '\t'))
                line.pop_back();
            size_t s = 0;
            while (s < line.size() && (line[s] == ' ' || line[s] == '\t')) ++s;
            if (s) line = line.substr(s);
            if (line.empty() || line[0] == '#') continue;
            // Per-asset output dir: <base_out>/<stem-of-path-without-extension>
            std::string base = line;
            size_t slash = base.find_last_of("/\\");
            if (slash != std::string::npos) base = base.substr(slash + 1);
            size_t dot = base.find_last_of('.');
            if (dot != std::string::npos) base = base.substr(0, dot);
            const fs::path sub = base_out / base;
            log_info("[i3ddump] [%d] %s -> %s",
                     ok + fail + 1, line.c_str(), sub.string().c_str());
            if (DumpI3DToPath(line.c_str(), sub.string().c_str()))
                ++ok;
            else
                ++fail;
        }
        log_info("[i3ddump] batch done: ok=%d fail=%d base='%s'",
                 ok, fail, base_out.string().c_str());
        return ok > 0;
    }
    return DumpI3DToPath(StartupDumpI3DPath, StartupDumpI3DOutPath);
}

struct SCharPreviewState
{
    TTextureHandle fallback_albedo = kInvalidTexture;
    struct Sub {
        MeshHandle handle = 0;
        int32_t objnum = -1;
        int32_t texslot = -1;
    };
    std::vector<Sub> subs;
    TObjectInstance* inst = nullptr;
    T3DImagery* img = nullptr;
    std::vector<std::pair<int32_t, int32_t>> roster; // {objclass,objtype}
    int32_t roster_idx = 0;
    float bbox_min[3] = { 0, 0, 0 };
    float bbox_max[3] = { 0, 0, 0 };
    float scale = 1.0f;
    float spin = 0.0f;
    bool paused = false;
    int64_t last_legacy_tick = -1;
};
SCharPreviewState g_charPreview;

SAnimPose SampleCharPreviewPose()
{
    if (!g_charPreview.img || !g_charPreview.inst)
        return {};
    return SampleI3DAnimPose(g_charPreview.img,
                             g_charPreview.inst->GetState(),
                             g_charPreview.inst->GetFrame(),
                             g_charPreview.inst->GetPrevState(),
                             g_charPreview.inst->GetPrevFrame());
}

void CloseCharPreviewMode()
{
    if (g_charPreview.inst)
    {
        g_charPreview.inst->OffScreen();
        delete g_charPreview.inst;
        g_charPreview.inst = nullptr;
    }
    g_charPreview.fallback_albedo = kInvalidTexture;
    g_charPreview.subs.clear();
    g_charPreview.img = nullptr;
    g_charPreview.spin = 0.0f;
    g_charPreview.last_legacy_tick = -1;
}

bool RebuildCharPreviewForRosterIndex(int32_t roster_idx)
{
    if (!Renderer || roster_idx < 0 || roster_idx >= int32_t(g_charPreview.roster.size()))
        return false;

    if (g_charPreview.inst)
    {
        g_charPreview.inst->OffScreen();
        delete g_charPreview.inst;
        g_charPreview.inst = nullptr;
    }
    g_charPreview.subs.clear();
    g_charPreview.img = nullptr;
    g_charPreview.spin = 0.0f;
    g_charPreview.last_legacy_tick = -1;
    g_charPreview.roster_idx = roster_idx;

    const auto [objclass, objtype] = g_charPreview.roster[roster_idx];
    TObjectClass* cl = TObjectClass::GetClass(objclass);
    if (!cl) return false;

    SObjectDef def = {};
    def.objclass = short(objclass);
    def.objtype = short(objtype);
    def.state = 0;
    def.level = 0;
    def.pos = {0, 0, 0};
    def.vel = {0, 0, 0};
    def.accum = {0, 0, 0};
    def.rotatex = 0;
    def.rotatey = 0;
    def.rotatez = 32;
    def.group = 0;

    TObjectInstance* inst = cl->NewObject(&def);
    if (!inst)
        return false;
    if (inst->IsCharacter())
    {
        if (auto* chr = dynamic_cast<TCharacter*>(inst))
        {
            const char* root = chr->DefaultRootState();
            if (root && *root)
                chr->SetState(root);
        }
    }
    inst->OnScreen();
    g_charPreview.inst = inst;

    g_charPreview.img = dynamic_cast<T3DImagery*>(inst->GetImagery());
    if (!g_charPreview.img)
        return false;

    if (g_charPreview.fallback_albedo == kInvalidTexture)
        g_charPreview.fallback_albedo = Renderer->WhiteTextureHandle();

    const int32_t state = inst->GetState();
    const int32_t frame = inst->GetFrame();
    const int32_t prevstate = inst->GetPrevState();
    const int32_t prevframe = inst->GetPrevFrame();
    const SAnimPose pose = SampleCharPreviewPose();
    bool bbox_init = false;
    const int32_t texslots = g_charPreview.img->NumTextures() + 1;
    for (int32_t objnum = 0; objnum < g_charPreview.img->NumObjects(); ++objnum)
    {
        if (g_charPreview.img->IsHidden(objnum, state))
            continue;

        bool obj_kept = false;
        for (int32_t texslot = 0; texslot < texslots; ++texslot)
        {
            std::vector<SMeshVertex> verts;
            std::vector<uint16_t> indices;
            if (!ExtractSubMeshTextureSlot(g_charPreview.img, objnum, texslot, verts, indices))
                continue;

            TTextureHandle albedo = g_charPreview.fallback_albedo;
            if (texslot > 0)
            {
                S3DTex tex = {};
                g_charPreview.img->GetTexture(texslot - 1, &tex);
                if (tex.htexture != kInvalidTexture)
                    albedo = tex.htexture;
            }
            MeshHandle h = Renderer->RegisterMesh(verts.data(), int32_t(verts.size()),
                                                  indices.data(), int32_t(indices.size()),
                                                  albedo);
            if (!h) return false;
            g_charPreview.subs.push_back({h, objnum, texslot});
            obj_kept = true;

            float world[16];
            BuildAnimPoseObjectMatrix(g_charPreview.img, pose, state, objnum, world);
            for (const auto& v : verts)
            {
                const float x = v.pos[0], y = v.pos[1], z = v.pos[2];
                const float wx = world[0] * x + world[1] * y + world[2]  * z + world[3];
                const float wy = world[4] * x + world[5] * y + world[6]  * z + world[7];
                const float wz = world[8] * x + world[9] * y + world[10] * z + world[11];
                if (!bbox_init) {
                    g_charPreview.bbox_min[0] = g_charPreview.bbox_max[0] = wx;
                    g_charPreview.bbox_min[1] = g_charPreview.bbox_max[1] = wy;
                    g_charPreview.bbox_min[2] = g_charPreview.bbox_max[2] = wz;
                    bbox_init = true;
                } else {
                    g_charPreview.bbox_min[0] = std::fmin(g_charPreview.bbox_min[0], wx);
                    g_charPreview.bbox_min[1] = std::fmin(g_charPreview.bbox_min[1], wy);
                    g_charPreview.bbox_min[2] = std::fmin(g_charPreview.bbox_min[2], wz);
                    g_charPreview.bbox_max[0] = std::fmax(g_charPreview.bbox_max[0], wx);
                    g_charPreview.bbox_max[1] = std::fmax(g_charPreview.bbox_max[1], wy);
                    g_charPreview.bbox_max[2] = std::fmax(g_charPreview.bbox_max[2], wz);
                }
            }
        }
        if (!obj_kept)
        {
            std::vector<SMeshVertex> verts;
            std::vector<uint16_t> indices;
            if (!ExtractSubMesh(g_charPreview.img, objnum, verts, indices))
                continue;
            TTextureHandle albedo = g_charPreview.fallback_albedo;
            if (g_charPreview.img->NumTextures() > 0)
            {
                S3DTex tex = {};
                g_charPreview.img->GetTexture(0, &tex);
                if (tex.htexture != kInvalidTexture)
                    albedo = tex.htexture;
            }
            MeshHandle h = Renderer->RegisterMesh(verts.data(), int32_t(verts.size()),
                                                  indices.data(), int32_t(indices.size()),
                                                  albedo);
            if (!h) return false;
            g_charPreview.subs.push_back({h, objnum, -1});

            float world[16];
            BuildAnimPoseObjectMatrix(g_charPreview.img, pose, state, objnum, world);
            for (const auto& v : verts)
            {
                const float x = v.pos[0], y = v.pos[1], z = v.pos[2];
                const float wx = world[0] * x + world[1] * y + world[2]  * z + world[3];
                const float wy = world[4] * x + world[5] * y + world[6]  * z + world[7];
                const float wz = world[8] * x + world[9] * y + world[10] * z + world[11];
                if (!bbox_init) {
                    g_charPreview.bbox_min[0] = g_charPreview.bbox_max[0] = wx;
                    g_charPreview.bbox_min[1] = g_charPreview.bbox_max[1] = wy;
                    g_charPreview.bbox_min[2] = g_charPreview.bbox_max[2] = wz;
                    bbox_init = true;
                } else {
                    g_charPreview.bbox_min[0] = std::fmin(g_charPreview.bbox_min[0], wx);
                    g_charPreview.bbox_min[1] = std::fmin(g_charPreview.bbox_min[1], wy);
                    g_charPreview.bbox_min[2] = std::fmin(g_charPreview.bbox_min[2], wz);
                    g_charPreview.bbox_max[0] = std::fmax(g_charPreview.bbox_max[0], wx);
                    g_charPreview.bbox_max[1] = std::fmax(g_charPreview.bbox_max[1], wy);
                    g_charPreview.bbox_max[2] = std::fmax(g_charPreview.bbox_max[2], wz);
                }
            }
        }
    }

    if (g_charPreview.subs.empty() || !bbox_init)
        return false;

    const float bbox_w = std::fmax(
        std::fmax(g_charPreview.bbox_max[0] - g_charPreview.bbox_min[0],
                  g_charPreview.bbox_max[1] - g_charPreview.bbox_min[1]),
                  g_charPreview.bbox_max[2] - g_charPreview.bbox_min[2]);
    const float autofit = (bbox_w > 1e-3f) ? (280.0f / bbox_w) : 1.0f;
    g_charPreview.scale = (StartupAssetScale > 0.0f) ? StartupAssetScale : autofit;
    log_info("[char3d] loaded %s:%s state=%d frame=%d prev=(%d,%d) subs=%zu scale=%.2f",
             cl->ClassName(), inst->GetTypeName(), state, frame, prevstate, prevframe,
             g_charPreview.subs.size(), g_charPreview.scale);
    return true;
}

bool InitializeCharPreviewMode()
{
    CloseCharPreviewMode();
    g_charPreview.roster.clear();
    for (int32_t i = 0; i < CharacterClass.NumTypes(); ++i)
        if (CharacterClass.GetObjType(i))
            g_charPreview.roster.push_back({OBJCLASS_CHARACTER, i});
    for (int32_t i = 0; i < PlayerClass.NumTypes(); ++i)
        if (PlayerClass.GetObjType(i))
            g_charPreview.roster.push_back({OBJCLASS_PLAYER, i});
    if (g_charPreview.roster.empty())
        return false;

    int32_t pick = 0;
    if (StartupAssetPath[0])
    {
        const int32_t c = CharacterClass.FindObjType(StartupAssetPath);
        const int32_t p = PlayerClass.FindObjType(StartupAssetPath);
        if (c >= 0)
            for (int32_t i = 0; i < int32_t(g_charPreview.roster.size()); ++i)
                if (g_charPreview.roster[i].first == OBJCLASS_CHARACTER && g_charPreview.roster[i].second == c)
                    pick = i;
        if (p >= 0)
            for (int32_t i = 0; i < int32_t(g_charPreview.roster.size()); ++i)
                if (g_charPreview.roster[i].first == OBJCLASS_PLAYER && g_charPreview.roster[i].second == p)
                    pick = i;
    }
    return RebuildCharPreviewForRosterIndex(pick);
}

void RenderCharPreviewMode()
{
    if (!Renderer || !Display.IsActive() || !Display.BackBuffer()) return;
    if (!g_charPreview.inst || g_charPreview.subs.empty() || !g_charPreview.img) return;

    const int64_t legacy_tick = TTime::LegacyFrameCount();
    if (!g_charPreview.paused && legacy_tick != g_charPreview.last_legacy_tick)
    {
        g_charPreview.last_legacy_tick = legacy_tick;
        g_charPreview.inst->NextFrame();
        if (g_charPreview.inst->NeedsAnimator() && !g_charPreview.inst->HasAnimator())
            g_charPreview.inst->OnScreen();
        g_charPreview.inst->Animate(false);
        g_charPreview.spin += 0.035f;
    }

    const int32_t state = g_charPreview.inst->GetState();
    const int32_t vw = Display.Width();
    const int32_t vh = Display.Height();
    const int32_t cam_ox = vw / 2;
    const int32_t cam_oy = vh / 2;

    Renderer->SetLight(0.6f, -0.6f, 0.4f, 1.0f, 1.0f, 1.0f, 1.0f, 0.25f);
    Renderer->SetAmbientColor(0.55f, 0.55f, 0.55f);
    Renderer->SetAmbientOcclusion(false, 12.0f, 1.0f, 0.15f, 96.0f);
    Renderer->SetNormalLightingHardness(1.0f);
    Renderer->SetLightingMode(1);
    Renderer->SetTileViewMode(0);
    Renderer->SetSunShadow(false, 24.0f, 3.0f, 32);

    const float s = g_charPreview.scale;
    const float bbox_w = std::fmax(
        std::fmax(g_charPreview.bbox_max[0] - g_charPreview.bbox_min[0],
                  g_charPreview.bbox_max[1] - g_charPreview.bbox_min[1]),
                  g_charPreview.bbox_max[2] - g_charPreview.bbox_min[2]);
    constexpr float kCam = 2750.0f;
    const float half_z = std::fmax(256.0f, bbox_w * s);
    const float znear = kCam - half_z - 128.0f;
    const float zfar  = kCam + half_z + 128.0f;
    Renderer->SetReconstructionParams(float(cam_ox), float(cam_oy), znear, zfar, 0.0f, 0.0f, kCam, 0.0f);
    Renderer->ClearPointLights();
    Renderer->BeginTilePass(0.08f, 0.08f, 0.12f, 1.0f);

    const float cx = 0.5f * (g_charPreview.bbox_min[0] + g_charPreview.bbox_max[0]);
    const float cy = 0.5f * (g_charPreview.bbox_min[1] + g_charPreview.bbox_max[1]);
    const float cz = 0.5f * (g_charPreview.bbox_min[2] + g_charPreview.bbox_max[2]);
    const float c = std::cos(g_charPreview.spin);
    const float si = std::sin(g_charPreview.spin);
    const float rot[16] = {
         c, -si, 0.0f, 0.0f,
         si,  c, 0.0f, 0.0f,
       0.0f, 0.0f, 1.0f, 0.0f,
       0.0f, 0.0f, 0.0f, 1.0f
    };
    const SAnimPose pose = SampleCharPreviewPose();
    for (const auto& sub : g_charPreview.subs)
    {
        SMeshSubmit m = {};
        m.mesh = sub.handle;
        float w[16];
        BuildAnimPoseObjectMatrix(g_charPreview.img, pose, state, sub.objnum, w);
        w[0] *= s; w[1] *= s; w[2] *= s; w[3] *= s;
        w[4] *= s; w[5] *= s; w[6] *= s; w[7] *= s;
        w[8] *= s; w[9] *= s; w[10] *= s; w[11] *= s;
        w[3] -= cx * s;
        w[7] -= cy * s;
        w[11] -= cz * s;
        float wr[16];
        MatrixMul16(rot, w, wr);
        std::memcpy(m.world, wr, sizeof(wr));
        m.tint[0] = m.tint[1] = m.tint[2] = m.tint[3] = 1.0f;
        Renderer->SubmitMesh(m);
    }
    Renderer->EndTilePass();
    Renderer->RunLightingPass();
}

// --- Mesh test state ----------------------------------------------------
struct SMeshTestState
{
    MeshHandle     cube   = 0;
    TTextureHandle albedo = kInvalidTexture;
    float          spin   = 0.0f;
    int32_t        frames = 0;
};
SMeshTestState g_meshTest;

bool InitializeMeshMode()
{
    if (!Renderer) { log_error("[mesh] Renderer is null"); return false; }

    // 1x1 white albedo so the cube reads the tint directly.
    g_meshTest.albedo = Renderer->WhiteTextureHandle();

    // Cube, half-extent h, face-independent normals.
    constexpr float h = 128.0f;
    const SMeshVertex verts[24] = {
        // +X face (normal +X)
        { {  h, -h, -h }, { 1, 0, 0 }, { 0, 0 } },
        { {  h,  h, -h }, { 1, 0, 0 }, { 1, 0 } },
        { {  h,  h,  h }, { 1, 0, 0 }, { 1, 1 } },
        { {  h, -h,  h }, { 1, 0, 0 }, { 0, 1 } },
        // -X face (normal -X)
        { { -h,  h, -h }, { -1, 0, 0 }, { 0, 0 } },
        { { -h, -h, -h }, { -1, 0, 0 }, { 1, 0 } },
        { { -h, -h,  h }, { -1, 0, 0 }, { 1, 1 } },
        { { -h,  h,  h }, { -1, 0, 0 }, { 0, 1 } },
        // +Y face (normal +Y)
        { {  h,  h, -h }, { 0, 1, 0 }, { 0, 0 } },
        { { -h,  h, -h }, { 0, 1, 0 }, { 1, 0 } },
        { { -h,  h,  h }, { 0, 1, 0 }, { 1, 1 } },
        { {  h,  h,  h }, { 0, 1, 0 }, { 0, 1 } },
        // -Y face (normal -Y)
        { { -h, -h, -h }, { 0, -1, 0 }, { 0, 0 } },
        { {  h, -h, -h }, { 0, -1, 0 }, { 1, 0 } },
        { {  h, -h,  h }, { 0, -1, 0 }, { 1, 1 } },
        { { -h, -h,  h }, { 0, -1, 0 }, { 0, 1 } },
        // +Z face (normal +Z, top)
        { { -h, -h,  h }, { 0, 0, 1 }, { 0, 0 } },
        { {  h, -h,  h }, { 0, 0, 1 }, { 1, 0 } },
        { {  h,  h,  h }, { 0, 0, 1 }, { 1, 1 } },
        { { -h,  h,  h }, { 0, 0, 1 }, { 0, 1 } },
        // -Z face (normal -Z, bottom)
        { { -h,  h, -h }, { 0, 0, -1 }, { 0, 0 } },
        { {  h,  h, -h }, { 0, 0, -1 }, { 1, 0 } },
        { {  h, -h, -h }, { 0, 0, -1 }, { 1, 1 } },
        { { -h, -h, -h }, { 0, 0, -1 }, { 0, 1 } },
    };
    const uint16_t indices[36] = {
        0,  1,  2,   0,  2,  3,   // +X
        4,  5,  6,   4,  6,  7,   // -X
        8,  9, 10,   8, 10, 11,   // +Y
       12, 13, 14,  12, 14, 15,   // -Y
       16, 17, 18,  16, 18, 19,   // +Z
       20, 21, 22,  20, 22, 23,   // -Z
    };
    g_meshTest.cube = Renderer->RegisterMesh(verts, 24, indices, 36, g_meshTest.albedo);
    if (!g_meshTest.cube) { log_error("[mesh] RegisterMesh failed"); return false; }
    log_info("[mesh] registered cube mesh=%u", g_meshTest.cube);
    return true;
}

void RenderMeshMode()
{
    if (!Renderer || !Display.IsActive() || !Display.BackBuffer()) return;
    if (!g_meshTest.cube) return;

    g_meshTest.spin  += 0.015f;
    g_meshTest.frames++;

    const int32_t vw = Display.Width();
    const int32_t vh = Display.Height();
    const int32_t cam_ox = vw / 2;
    const int32_t cam_oy = vh / 2;

    // Reasonable defaults for a single-cube preview.
    Renderer->SetLight(0.6f, -0.6f, 0.4f, 1.0f, 1.0f, 1.0f, 1.0f, 0.25f);
    Renderer->SetAmbientColor(0.55f, 0.55f, 0.55f);
    Renderer->SetAmbientOcclusion(false, 12.0f, 1.0f, 0.15f, 96.0f);
    Renderer->SetNormalLightingHardness(1.0f);
    Renderer->SetLightingMode(1);
    Renderer->SetSunShadow(false, 24.0f, 3.0f, 32);
    Renderer->SetShadowWorldDir(0.6f, -0.6f, 0.4f);
    Renderer->SetShadowVariance(0.0f, 0.0f, 1.0f);

    // Cube sits at world (0,0,0). Pick a z window centered on the same
    // kcam_forward the map test uses so the cube lands mid-range.
    constexpr float kCam    = 2750.0f;
    constexpr float zHalf   =  512.0f;
    const float znear = kCam - zHalf;
    const float zfar  = kCam + zHalf;
    Renderer->SetReconstructionParams(float(cam_ox), float(cam_oy),
                                      znear, zfar, 0.0f, 0.0f, kCam, 0.0f);
    Renderer->ClearPointLights();

    Renderer->BeginTilePass(0.08f, 0.08f, 0.12f, 1.0f);

    // Rotation around Z (iso "up") by spin.
    const float c  = std::cos(g_meshTest.spin);
    const float si = std::sin(g_meshTest.spin);
    SMeshSubmit m = {};
    m.mesh    = g_meshTest.cube;
    // Row 0: [c, -s, 0, 0]
    m.world[0] =  c;  m.world[1] = -si; m.world[2] = 0; m.world[3] = 0;
    // Row 1: [s, c, 0, 0]
    m.world[4] = si;  m.world[5] =  c;  m.world[6] = 0; m.world[7] = 0;
    // Row 2: [0, 0, 1, 0]
    m.world[8] = 0;   m.world[9] =  0;  m.world[10] = 1; m.world[11] = 0;
    // Row 3: [0, 0, 0, 1]
    m.world[12] = 0;  m.world[13] = 0;  m.world[14] = 0; m.world[15] = 1;
    m.tint[0] = 0.9f; m.tint[1] = 0.3f; m.tint[2] = 0.2f; m.tint[3] = 1.0f;
    Renderer->SubmitMesh(m);

    Renderer->EndTilePass();
    Renderer->RunLightingPass();

    if (g_meshTest.frames == 1)
        log_info("[mesh] first-frame submit: cube=%u tint=(%.2f,%.2f,%.2f)",
                 m.mesh, m.tint[0], m.tint[1], m.tint[2]);
}

void CloseMeshMode()
{
    g_meshTest.albedo = kInvalidTexture;
    g_meshTest.cube = 0;
}

// --- I3D animation test state -------------------------------------------
// Loads one T3DImagery asset and samples one selected state directly as a
// pure loop. No TObjectInstance, player state machine, or prevstate transition
// blend is involved; this screen is the baseline for validating authored loop
// keys and frame-to-frame interpolation.
struct SI3DStaticTestState
{
    TTextureHandle fallback_albedo = kInvalidTexture;
    RendererImagePairHandle bg_black = 0;
    RendererImagePairHandle bg_brown = 0;
    RendererImagePairHandle bg_green = 0;
    RendererImagePairHandle bg_checker = 0;
    struct Sub {
        MeshHandle handle;
        int32_t    objnum = -1;
        int32_t    texslot = -1;
        int32_t    texture_idx = -1;
        float      world[16];  // state-0/frame-0 local hierarchy matrix
    };
    struct CharacterChoice {
        int32_t objclass = -1;
        int32_t objtype = -1;
        int32_t imageryid = -1;
        std::string label;
    };
    std::vector<Sub> subs;
    std::vector<std::string> roster;
    std::vector<CharacterChoice> character_roster;
    int32_t    roster_idx = 0;
    int32_t    character_idx = -1;
    std::string active_asset;
    int32_t    active_imagery_id = -1;
    int32_t    bg_mode = 0;
    T3DImagery* img = nullptr;
    bool       transparent_preview = false;
    SRenderMetadata render_meta;
    const SRenderPolicy* active_policy = nullptr;
    float      bbox_min[3] = { 0, 0, 0 };
    float      bbox_max[3] = { 0, 0, 0 };
    float      scale  = 1.0f;
    float      spin = 0.0f;
    double     anim_time = 0.0;
    float      anim_speed = 1.0f;
    int32_t    anim_state = 0;
    int32_t    anim_frame = 0;
    int32_t    anim_prev_frame = 0;
    int32_t    frames = 0;
    bool       anim_paused = false;
    bool       logged = false;
};
SI3DStaticTestState g_i3dTest;

const char* kI3DBGNames[] = {
    "checkerboard",
    "black",
    "light brown",
    "green",
};

void MixI3DPreviewKey(uint64_t& hash, uint64_t value)
{
    hash ^= value;
    hash *= 1099511628211ull;
}

uint64_t I3DPreviewMeshKey(int32_t imageryid, int32_t objnum, int32_t texslot)
{
    uint64_t hash = 1469598103934665603ull;
    MixI3DPreviewKey(hash, 0x493344544553544Dull); // "I3DTESTM"
    MixI3DPreviewKey(hash, uint64_t(uint32_t(imageryid)));
    MixI3DPreviewKey(hash, uint64_t(uint32_t(objnum)));
    MixI3DPreviewKey(hash, uint64_t(uint32_t(texslot + 1)));
    return hash ? hash : 1ull;
}

void ClearI3DStaticMeshes()
{
    if (Renderer)
    {
        for (const auto& sub : g_i3dTest.subs)
            Renderer->ReleaseMeshAssetRef(sub.handle);
    }
    g_i3dTest.subs.clear();
}

void ResetI3DStaticPlayback(int32_t state = 0)
{
    g_i3dTest.anim_time = 0.0;
    g_i3dTest.anim_state = state;
    g_i3dTest.anim_frame = 0;
    g_i3dTest.anim_prev_frame = 0;
}

int32_t ClampI3DStaticState(int32_t state)
{
    if (!g_i3dTest.img || g_i3dTest.img->NumStates() <= 0)
        return 0;
    if (state < 0)
        return 0;
    if (state >= g_i3dTest.img->NumStates())
        return g_i3dTest.img->NumStates() - 1;
    return state;
}

int32_t CurrentI3DStaticAnimLength()
{
    if (!g_i3dTest.img || g_i3dTest.img->NumStates() <= 0)
        return 1;
    g_i3dTest.anim_state = ClampI3DStaticState(g_i3dTest.anim_state);
    return (std::max)(1, g_i3dTest.img->GetAniLength(g_i3dTest.anim_state));
}

void UpdateI3DStaticFrameFromTime()
{
    const int32_t len = CurrentI3DStaticAnimLength();
    const double wrapped = std::fmod(g_i3dTest.anim_time, double(len));
    const double positive = wrapped < 0.0 ? wrapped + double(len) : wrapped;
    g_i3dTest.anim_time = positive;
    g_i3dTest.anim_frame = int32_t(positive) % len;
    g_i3dTest.anim_prev_frame = (g_i3dTest.anim_frame + len - 1) % len;
}

void StepI3DStaticFrames(int32_t frames)
{
    g_i3dTest.anim_paused = true;
    g_i3dTest.anim_time += double(frames);
    UpdateI3DStaticFrameFromTime();
}

void AddI3DCharacterChoices(TObjectClass& cl, int32_t objclass)
{
    for (int32_t objtype = 0; objtype < cl.NumTypes(); ++objtype)
    {
        SObjectInfo* info = cl.GetObjType(objtype);
        if (!info)
            continue;

        SImageryEntry* entry = TObjectImagery::GetImageryEntry(info->imageryid);
        if (!entry)
            continue;
        if (!entry->header ||
            (entry->header->imageryid != OBJIMAGE_MESH3D &&
             entry->header->imageryid != OBJIMAGE_MESH3DHELPER))
            continue;

        SI3DStaticTestState::CharacterChoice choice;
        choice.objclass = objclass;
        choice.objtype = objtype;
        choice.imageryid = info->imageryid;
        choice.label = cl.ClassName() ? cl.ClassName() : "Object";
        choice.label += " / ";
        choice.label += info->name ? info->name : "?";
        choice.label += "  ";
        choice.label += entry->filename;
        g_i3dTest.character_roster.push_back(choice);
    }
}

void BuildI3DCharacterChoices()
{
    g_i3dTest.character_roster.clear();
    AddI3DCharacterChoices(CharacterClass, OBJCLASS_CHARACTER);
    AddI3DCharacterChoices(PlayerClass, OBJCLASS_PLAYER);
    g_i3dTest.character_idx = g_i3dTest.character_roster.empty() ? -1 : 0;
}

int32_t FindI3DCharacterChoiceByImagery(int32_t imageryid)
{
    for (int32_t i = 0; i < int32_t(g_i3dTest.character_roster.size()); ++i)
        if (g_i3dTest.character_roster[i].imageryid == imageryid)
            return i;
    return -1;
}

const char* I3DFlagListText(int32_t flags)
{
    static char buf[160];
    buf[0] = 0;
    auto append = [&](const char* text) {
        if (buf[0])
            strncat(buf, " ", sizeof(buf) - strlen(buf) - 1);
        strncat(buf, text, sizeof(buf) - strlen(buf) - 1);
    };
    if (flags & AF_LOOPING) append("loop");
    if (flags & AF_PINGPONG) append("pingpong");
    if (flags & AF_REVERSE) append("reverse");
    if (flags & AF_NOINTERPOLATION) append("no-transition-interp");
    if (flags & AF_MOVE) append("move");
    if (flags & AF_ROOT) append("root");
    if (flags & AF_ROOT2ROOT) append("root2root");
    if (!buf[0])
        strcpy(buf, "none");
    return buf;
}

bool LoadI3DStaticAsset(const char* path);
bool LoadI3DStaticImagery(int32_t imageryid, const char* label);

RendererImagePairHandle MakeSolidImagePair(uint64_t key, uint32_t rgba)
{
    const float zero = 0.0f;
    return Renderer->RegisterImagePairAsset(key,
                                            &rgba, sizeof(rgba),
                                            &zero, sizeof(zero),
                                            1, 1,
                                            sizeof(rgba) + sizeof(zero));
}

RendererImagePairHandle MakeCheckerImagePair(uint64_t key)
{
    static constexpr int32_t kW = 64;
    static constexpr int32_t kH = 64;
    uint32_t pixels[kW * kH] = {};
    for (int32_t y = 0; y < kH; ++y)
    {
        for (int32_t x = 0; x < kW; ++x)
        {
            const bool dark = (((x / 8) + (y / 8)) & 1) != 0;
            const uint8_t c = dark ? 0x66 : 0xB8;
            pixels[y * kW + x] = 0xFF000000u | (uint32_t(c) << 16) | (uint32_t(c) << 8) | uint32_t(c);
        }
    }
    float depth[kW * kH] = {};
    return Renderer->RegisterImagePairAsset(key,
                                            pixels, sizeof(pixels),
                                            depth, sizeof(depth),
                                            kW, kH,
                                            sizeof(pixels) + sizeof(depth));
}

bool EnsureI3DStaticBackgroundAssets()
{
    if (g_i3dTest.bg_black && g_i3dTest.bg_brown &&
        g_i3dTest.bg_green && g_i3dTest.bg_checker)
        return true;

    if (!g_i3dTest.bg_black)   g_i3dTest.bg_black   = MakeSolidImagePair(0x4933444247424C4Bull, 0xFF000000u);
    if (!g_i3dTest.bg_brown)   g_i3dTest.bg_brown   = MakeSolidImagePair(0x493344424742524Eull, 0xFF6E7B9Au);
    if (!g_i3dTest.bg_green)   g_i3dTest.bg_green   = MakeSolidImagePair(0x493344424747524Eull, 0xFF4E7A4Au);
    if (!g_i3dTest.bg_checker) g_i3dTest.bg_checker = MakeCheckerImagePair(0x493344424743484Bull);

    return g_i3dTest.bg_black && g_i3dTest.bg_brown &&
           g_i3dTest.bg_green && g_i3dTest.bg_checker;
}

RendererImagePairHandle CurrentI3DBackgroundImagePair()
{
    switch (g_i3dTest.bg_mode)
    {
    case 1: return g_i3dTest.bg_black;
    case 2: return g_i3dTest.bg_brown;
    case 3: return g_i3dTest.bg_green;
    default: return g_i3dTest.bg_checker;
    }
}

const char* RenderMetaBlendName(ERenderMetaBlend blend)
{
    switch (blend)
    {
    case ERenderMetaBlend::Opaque: return "opaque";
    case ERenderMetaBlend::Alpha: return "alpha";
    case ERenderMetaBlend::Additive: return "additive";
    }
    return "?";
}

const char* RenderMetaDrawableName(ERenderMetaDrawable drawable)
{
    switch (drawable)
    {
    case ERenderMetaDrawable::Mesh: return "mesh";
    case ERenderMetaDrawable::WaterParticles: return "water_particles";
    case ERenderMetaDrawable::WaterfallParticles: return "waterfall_particles";
    }
    return "?";
}

const char* RenderMetaLightingName(ERenderMetaLighting lighting)
{
    switch (lighting)
    {
    case ERenderMetaLighting::Lit: return "lit";
    case ERenderMetaLighting::Fullbright: return "fullbright";
    case ERenderMetaLighting::VertexLit: return "vertex_lit";
    case ERenderMetaLighting::UnlitShadow: return "unlit_shadow";
    }
    return "?";
}

const char* RenderMetaTextureAnimName(ERenderMetaTextureAnim anim)
{
    switch (anim)
    {
    case ERenderMetaTextureAnim::None: return "none";
    case ERenderMetaTextureAnim::FrameByInstance: return "frame_by_instance";
    }
    return "?";
}

const char* WaterPreviewTypeName()
{
    static const char* kNames[] = {
        "Water",
        "StillWater",
        "FlowWater",
        "BendWater1",
        "BendWater2",
        "SewerWater",
        "Wave",
        "WaveS",
        "WaveM",
        "WaterFlft",
        "WaterFrt",
        "WaterClft",
        "WaterCrt",
        "RiverFall",
        "Box",
        "Axis",
    };
    if (g_i3dTest.roster_idx >= 0 && g_i3dTest.roster_idx < int32_t(sizeof(kNames) / sizeof(kNames[0])))
        return kNames[g_i3dTest.roster_idx];
    return nullptr;
}

void LogWaterPreviewPolicy()
{
    const char* type_name = WaterPreviewTypeName();
    g_i3dTest.active_policy = type_name ? g_i3dTest.render_meta.FindEffect(type_name) : nullptr;
    if (!g_i3dTest.active_policy && type_name)
    {
        if (const SRenderObjectPolicy* helper = g_i3dTest.render_meta.FindHelper(type_name))
            g_i3dTest.active_policy = helper;
    }
    if (g_i3dTest.active_policy)
    {
        const SRenderPolicy& p = *g_i3dTest.active_policy;
        log_info("[water3d] showing type='%s' asset='%s' expected drawable=%s blend=%s lighting=%s ztest=%d zwrite=%d texanim=%d",
                 type_name ? type_name : "?",
                 g_i3dTest.active_asset.c_str(),
                 RenderMetaDrawableName(p.drawable),
                 RenderMetaBlendName(p.blend),
                 RenderMetaLightingName(p.lighting),
                 p.ztest ? 1 : 0,
                 p.zwrite ? 1 : 0,
                 int(p.texture_anim));
    }
    else
    {
        log_info("[water3d] showing type='%s' asset='%s' expected policy=<none>",
                 type_name ? type_name : "?",
                 g_i3dTest.active_asset.c_str());
    }
}

void LogI3DAnimationSummary(T3DImagery* img, const char* label)
{
    if (!img)
        return;
    log_info("[water3d] anim summary '%s': states=%d textures=%d",
             label ? label : "?", img->NumStates(), img->NumTextures());
    for (int32_t st = 0; st < img->NumStates() && st < 6; ++st)
    {
        PSImageryStateHeader state = img->GetState(st);
        log_info("[water3d]   state[%d] name='%s' frames=%d flags=0x%x",
                 st,
                 state ? state->animname : "?",
                 img->GetAniLength(st),
                 img->GetAniFlags(st));
    }
    for (int32_t texnum = 0; texnum < img->NumTextures() && texnum < 6; ++texnum)
    {
        S3DTex tex = {};
        img->GetTexture(texnum, &tex);
        log_info("[water3d]   tex[%d] frames=%d current=%d handle=%u",
                 texnum, tex.numframes, tex.framenum, tex.htexture);
    }
    const int32_t st = 0;
    const int32_t len = img->NumStates() > 0 ? img->GetAniLength(st) : 0;
    for (int32_t objnum = 0; objnum < img->NumObjects() && objnum < 4; ++objnum)
    {
        int32_t frames[3] = {0, len > 1 ? len / 2 : 0, len > 1 ? len - 1 : 0};
        for (int32_t i = 0; i < 3; ++i)
        {
            hmm_vec3 pos = {}, rot = {}, scl = {1,1,1};
            const bool ok = len > 0 && img->GetUninterpolatedAniKey(objnum, st, frames[i], pos, rot, scl);
            log_info("[water3d]   obj[%d] frame=%d key_ok=%d pos=(%.3f,%.3f,%.3f) rot=(%.3f,%.3f,%.3f) scl=(%.3f,%.3f,%.3f)",
                     objnum, frames[i], ok ? 1 : 0,
                     pos.X, pos.Y, pos.Z,
                     rot.X, rot.Y, rot.Z,
                     scl.X, scl.Y, scl.Z);
        }
    }
}

void DrawI3DPreviewPanel()
{
    ImGui::SetNextWindowPos(ImVec2(12.0f, 12.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(0.86f);
    ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize |
                             ImGuiWindowFlags_NoSavedSettings |
                             ImGuiWindowFlags_NoFocusOnAppearing;
    if (!ImGui::Begin("I3D Animation Test", nullptr, flags))
    {
        ImGui::End();
        return;
    }

    const bool water_mode = g_i3dTest.transparent_preview && !g_i3dTest.roster.empty();
    if (water_mode)
    {
        const char* preview = (g_i3dTest.roster_idx >= 0 &&
                               g_i3dTest.roster_idx < int32_t(g_i3dTest.roster.size()))
            ? g_i3dTest.roster[g_i3dTest.roster_idx].c_str()
            : "?";
        ImGui::SetNextItemWidth(360.0f);
        if (ImGui::BeginCombo("Asset", preview))
        {
            for (int32_t i = 0; i < int32_t(g_i3dTest.roster.size()); ++i)
            {
                const bool selected = (i == g_i3dTest.roster_idx);
                if (ImGui::Selectable(g_i3dTest.roster[i].c_str(), selected))
                {
                    g_i3dTest.roster_idx = i;
                    LoadI3DStaticAsset(g_i3dTest.roster[i].c_str());
                }
                if (selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }
    else if (!g_i3dTest.character_roster.empty())
    {
        const char* preview = (g_i3dTest.character_idx >= 0 &&
                               g_i3dTest.character_idx < int32_t(g_i3dTest.character_roster.size()))
            ? g_i3dTest.character_roster[g_i3dTest.character_idx].label.c_str()
            : g_i3dTest.active_asset.c_str();
        ImGui::SetNextItemWidth(420.0f);
        if (ImGui::BeginCombo("Character", preview))
        {
            for (int32_t i = 0; i < int32_t(g_i3dTest.character_roster.size()); ++i)
            {
                const auto& choice = g_i3dTest.character_roster[i];
                const bool selected = (i == g_i3dTest.character_idx);
                if (ImGui::Selectable(choice.label.c_str(), selected))
                {
                    g_i3dTest.character_idx = i;
                    LoadI3DStaticImagery(choice.imageryid, choice.label.c_str());
                }
                if (selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }

    ImGui::SetNextItemWidth(180.0f);
    if (ImGui::BeginCombo("Background", kI3DBGNames[g_i3dTest.bg_mode]))
    {
        for (int32_t i = 0; i < 4; ++i)
        {
            const bool selected = (i == g_i3dTest.bg_mode);
            if (ImGui::Selectable(kI3DBGNames[i], selected))
                g_i3dTest.bg_mode = i;
            if (selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::Separator();
    ImGui::TextWrapped("Source: %s", g_i3dTest.active_asset.empty() ? "?" : g_i3dTest.active_asset.c_str());
    ImGui::Text("Sampling: direct I3D loop, prevstate disabled");

    if (g_i3dTest.img && g_i3dTest.img->NumStates() > 0)
    {
        g_i3dTest.anim_state = ClampI3DStaticState(g_i3dTest.anim_state);

        char preview[160];
        const char* state_name = g_i3dTest.img->GetAniName(g_i3dTest.anim_state);
        std::snprintf(preview, sizeof(preview), "%02d  %s",
                      g_i3dTest.anim_state, state_name ? state_name : "?");

        ImGui::SetNextItemWidth(320.0f);
        if (ImGui::BeginCombo("State", preview))
        {
            for (int32_t st = 0; st < g_i3dTest.img->NumStates(); ++st)
            {
                char label[160];
                const char* name = g_i3dTest.img->GetAniName(st);
                std::snprintf(label, sizeof(label), "%02d  %s", st, name ? name : "?");
                const bool selected = (st == g_i3dTest.anim_state);
                if (ImGui::Selectable(label, selected))
                    ResetI3DStaticPlayback(st);
                if (selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        const int32_t len = CurrentI3DStaticAnimLength();
        const int32_t next_frame = (g_i3dTest.anim_frame + 1) % len;
        const float frac = float(g_i3dTest.anim_time - std::floor(g_i3dTest.anim_time));
        const int32_t state_flags = g_i3dTest.img->GetAniFlags(g_i3dTest.anim_state);
        ImGui::Text("Frame: %d -> %d   frac %.3f   len %d",
                    g_i3dTest.anim_frame, next_frame, frac, len);
        ImGui::Text("Flags: 0x%04x  %s", state_flags, I3DFlagListText(state_flags));

        ImGui::Checkbox("Pause", &g_i3dTest.anim_paused);
        ImGui::SameLine();
        if (ImGui::Button("- frame"))
            StepI3DStaticFrames(-1);
        ImGui::SameLine();
        if (ImGui::Button("+ frame"))
            StepI3DStaticFrames(1);
        ImGui::SameLine();
        if (ImGui::Button("Reset"))
            ResetI3DStaticPlayback(g_i3dTest.anim_state);

        ImGui::SetNextItemWidth(220.0f);
        ImGui::SliderFloat("Speed", &g_i3dTest.anim_speed, 0.0f, 2.0f, "%.2fx");

        float time_scale = float(TTime::TimeScale());
        ImGui::SetNextItemWidth(220.0f);
        if (ImGui::SliderFloat("Game time scale", &time_scale, 0.0f, 4.0f, "%.3fx"))
            TTime::SetTimeScale(time_scale);
        ImGui::TextUnformatted("Presets:");
        ImGui::SameLine();
        if (ImGui::RadioButton("1x##i3d_timescale", TTime::TimeScale() == 1.0))
            TTime::SetTimeScale(1.0);
        ImGui::SameLine();
        if (ImGui::RadioButton("1/2##i3d_timescale", TTime::TimeScale() == 0.5))
            TTime::SetTimeScale(0.5);
        ImGui::SameLine();
        if (ImGui::RadioButton("1/4##i3d_timescale", TTime::TimeScale() == 0.25))
            TTime::SetTimeScale(0.25);
        ImGui::SameLine();
        if (ImGui::RadioButton("1/8##i3d_timescale", TTime::TimeScale() == 0.125))
            TTime::SetTimeScale(0.125);
        ImGui::SameLine();
        if (ImGui::RadioButton("2x##i3d_timescale", TTime::TimeScale() == 2.0))
            TTime::SetTimeScale(2.0);
        ImGui::SameLine();
        if (ImGui::RadioButton("4x##i3d_timescale", TTime::TimeScale() == 4.0))
            TTime::SetTimeScale(4.0);
    }
    else
    {
        ImGui::Text("No animation states");
    }

    if (water_mode)
    {
        const char* type_name = WaterPreviewTypeName();
        ImGui::Separator();
        ImGui::Text("Water type: %s", type_name ? type_name : "?");
        if (g_i3dTest.active_policy)
        {
            const SRenderPolicy& p = *g_i3dTest.active_policy;
            ImGui::Text("Drawable: %s", RenderMetaDrawableName(p.drawable));
            ImGui::Text("Blend: %s   Lighting: %s",
                        RenderMetaBlendName(p.blend),
                        RenderMetaLightingName(p.lighting));
            ImGui::Text("ztest/zwrite: %d / %d", p.ztest ? 1 : 0, p.zwrite ? 1 : 0);
        }
    }

    ImGui::Separator();
    ImGui::Text("Objects: %zu   textures: %d",
                g_i3dTest.subs.size(),
                g_i3dTest.img ? g_i3dTest.img->NumTextures() : 0);

    ImGui::End();
}

bool LoadI3DStaticAsset(const char* path)
{
    log_info("[i3d3d] resolving '%s'", path ? path : "");
    const int32_t id = TObjectImagery::FindImagery(path);
    log_info("[i3d3d]   FindImagery returned id=%d", id);
    if (id < 0)
    {
        log_error("[i3d3d] FindImagery failed for '%s'", path ? path : "");
        return false;
    }
    return LoadI3DStaticImagery(id, path);
}

bool LoadI3DStaticImagery(int32_t id, const char* label)
{
    if (!Renderer) { log_error("[i3d3d] Renderer is null"); return false; }

    ClearI3DStaticMeshes();
    g_i3dTest.logged = false;
    g_i3dTest.spin = 0.0f;
    g_i3dTest.frames = 0;
    ResetI3DStaticPlayback(0);
    g_i3dTest.img = nullptr;
    g_i3dTest.active_imagery_id = -1;
    g_i3dTest.active_policy = nullptr;

    if (g_i3dTest.fallback_albedo == kInvalidTexture)
        g_i3dTest.fallback_albedo = Renderer->WhiteTextureHandle();
    if (!EnsureI3DStaticBackgroundAssets())
    {
        log_error("[i3d3d] failed to initialize preview backgrounds");
        return false;
    }

    g_i3dTest.active_asset = label ? label : "";
    log_info("[i3d3d]   LoadImagery id=%d label='%s'...",
             id, g_i3dTest.active_asset.c_str());
    TObjectImagery* base = TObjectImagery::LoadImagery(id);
    log_info("[i3d3d]   LoadImagery -> %p", (void*)base);
    if (!base)
    {
        log_error("[i3d3d] LoadImagery returned null for id=%d '%s'",
                  id, g_i3dTest.active_asset.c_str());
        return false;
    }
    T3DImagery* img = dynamic_cast<T3DImagery*>(base);
    if (!img) { log_error("[i3d3d] imagery is not a T3DImagery (id=%d)", id); return false; }
    g_i3dTest.img = img;
    g_i3dTest.active_imagery_id = id;

    const int32_t choice = FindI3DCharacterChoiceByImagery(id);
    if (choice >= 0)
        g_i3dTest.character_idx = choice;

    log_info("[i3d3d] loaded '%s' (NumVerts=%d NumFaces=%d NumObjects=%d)",
             g_i3dTest.active_asset.c_str(),
             img->NumVerts(), img->NumFaces(), img->NumObjects());
    if (!g_i3dTest.roster.empty())
        LogI3DAnimationSummary(img, g_i3dTest.active_asset.c_str());

    bool bbox_init = false;
    int32_t total_tris = 0;
    const int32_t texslots = img->NumTextures() + 1;
    for (int32_t objnum = 0; objnum < img->NumObjects(); ++objnum) {
        if (img->IsHidden(objnum, 0))
            continue;
        bool logged_obj = false;
        for (int32_t texslot = 0; texslot < texslots; ++texslot) {
            std::vector<SMeshVertex> verts;
            std::vector<uint16_t>    indices;
            if (!ExtractSubMeshTextureSlot(img, objnum, texslot, verts, indices))
                continue;

            TTextureHandle albedo = g_i3dTest.fallback_albedo;
            int32_t texture_idx = -1;
            if (texslot > 0) {
                S3DTex tex = {};
                img->GetTexture(texslot - 1, &tex);
                if (tex.htexture != kInvalidTexture)
                    albedo = tex.htexture;
                texture_idx = texslot - 1;
            }
            if (texture_idx < 0)
            {
                S3DObj obj = {};
                img->GetObject(objnum, &obj);
                if (obj.material >= 0 && obj.material < img->NumMaterials())
                {
                    S3DMat mat = {};
                    img->GetMaterial(obj.material, &mat);
                    if (mat.texture >= 0 && mat.texture < img->NumTextures())
                    {
                        S3DTex tex = {};
                        img->GetTexture(mat.texture, &tex);
                        if (tex.htexture != kInvalidTexture)
                        {
                            albedo = tex.htexture;
                            texture_idx = mat.texture;
                        }
                    }
                }
            }

            const uint64_t mesh_key =
                I3DPreviewMeshKey(g_i3dTest.active_imagery_id, objnum, texslot);
            MeshHandle h = Renderer->RegisterMeshAsset(mesh_key,
                                                       verts.data(), int32_t(verts.size()),
                                                       indices.data(), int32_t(indices.size()),
                                                       albedo);
            if (!h) {
                log_error("[i3d3d] RegisterMesh failed for obj %d texslot %d", objnum, texslot);
                return false;
            }
            Renderer->AddMeshAssetRef(h);

            SI3DStaticTestState::Sub s = {};
            s.handle = h;
            s.objnum = objnum;
            s.texslot = texslot;
            s.texture_idx = texture_idx;
            BuildStaticObjectMatrix(img, objnum, 0, 0, s.world);
            g_i3dTest.subs.push_back(s);
            total_tris += int32_t(indices.size() / 3);

            if (!logged_obj)
            {
                logged_obj = true;
                float cmin[3] = { 0, 0, 0 };
                float cmax[3] = { 0, 0, 0 };
                if (!verts.empty())
                {
                    cmin[0] = cmax[0] = verts[0].pos[0];
                    cmin[1] = cmax[1] = verts[0].pos[1];
                    cmin[2] = cmax[2] = verts[0].pos[2];
                    for (const auto& v : verts)
                    {
                        cmin[0] = std::fmin(cmin[0], v.pos[0]);
                        cmin[1] = std::fmin(cmin[1], v.pos[1]);
                        cmin[2] = std::fmin(cmin[2], v.pos[2]);
                        cmax[0] = std::fmax(cmax[0], v.pos[0]);
                        cmax[1] = std::fmax(cmax[1], v.pos[1]);
                        cmax[2] = std::fmax(cmax[2], v.pos[2]);
                    }
                }
                hmm_vec3 key_pos = {}, key_rot = {}, key_scl = { 1.0f, 1.0f, 1.0f };
                img->GetUninterpolatedAniKey(objnum, 0, 0, key_pos, key_rot, key_scl);
                const float center_x = 0.5f * (cmin[0] + cmax[0]);
                const float center_y = 0.5f * (cmin[1] + cmax[1]);
                const float center_z = 0.5f * (cmin[2] + cmax[2]);
                log_info(
                    "[i3d3d]   obj[%d] '%s' parent=%d key_pos=(%.3f,%.3f,%.3f) "
                    "key_rot=(%.3f,%.3f,%.3f) key_scl=(%.3f,%.3f,%.3f) "
                    "local_center=(%.3f,%.3f,%.3f) world_t=(%.3f,%.3f,%.3f)",
                    objnum, img->GetObjectName(objnum), img->GetObjectParent(objnum, 0),
                    key_pos.X, key_pos.Y, key_pos.Z,
                    key_rot.X, key_rot.Y, key_rot.Z,
                    key_scl.X, key_scl.Y, key_scl.Z,
                    center_x, center_y, center_z,
                    s.world[3], s.world[7], s.world[11]);
            }

            for (const auto& v : verts) {
                const float x = v.pos[0];
                const float y = v.pos[1];
                const float z = v.pos[2];
                const float wx = s.world[0] * x + s.world[1] * y + s.world[2]  * z + s.world[3];
                const float wy = s.world[4] * x + s.world[5] * y + s.world[6]  * z + s.world[7];
                const float wz = s.world[8] * x + s.world[9] * y + s.world[10] * z + s.world[11];
                if (!bbox_init) {
                    g_i3dTest.bbox_min[0] = g_i3dTest.bbox_max[0] = wx;
                    g_i3dTest.bbox_min[1] = g_i3dTest.bbox_max[1] = wy;
                    g_i3dTest.bbox_min[2] = g_i3dTest.bbox_max[2] = wz;
                    bbox_init = true;
                } else {
                    g_i3dTest.bbox_min[0] = std::fmin(g_i3dTest.bbox_min[0], wx);
                    g_i3dTest.bbox_min[1] = std::fmin(g_i3dTest.bbox_min[1], wy);
                    g_i3dTest.bbox_min[2] = std::fmin(g_i3dTest.bbox_min[2], wz);
                    g_i3dTest.bbox_max[0] = std::fmax(g_i3dTest.bbox_max[0], wx);
                    g_i3dTest.bbox_max[1] = std::fmax(g_i3dTest.bbox_max[1], wy);
                    g_i3dTest.bbox_max[2] = std::fmax(g_i3dTest.bbox_max[2], wz);
                }
            }
        }
    }

    if (g_i3dTest.subs.empty()) {
        log_error("[i3d3d] no renderable sub-meshes extracted");
        return false;
    }

    log_info("[i3d3d]   object-meshes: subs=%zu tris=%d",
             g_i3dTest.subs.size(), total_tris);
    // Auto-fit scale: target a ~200-world-unit-wide render regardless of
    // the asset's native units. Override via --scale=f.
    const float bbox_w = std::fmax(
        std::fmax(g_i3dTest.bbox_max[0] - g_i3dTest.bbox_min[0],
                  g_i3dTest.bbox_max[1] - g_i3dTest.bbox_min[1]),
                  g_i3dTest.bbox_max[2] - g_i3dTest.bbox_min[2]);
    // Fit the full authored bbox into the preview. Treat oversized manual
    // scales as hints, not absolute truth, so the model remains viewable.
    const float autofit = (bbox_w > 1e-3f) ? (300.0f / bbox_w) : 1.0f;
    if (StartupAssetScale > 0.0f)
        g_i3dTest.scale = std::fmin(StartupAssetScale, autofit);
    else
        g_i3dTest.scale = autofit;

    log_info("[i3d3d] world bbox min=(%.1f,%.1f,%.1f) max=(%.1f,%.1f,%.1f) scale=%.2f",
             g_i3dTest.bbox_min[0], g_i3dTest.bbox_min[1], g_i3dTest.bbox_min[2],
             g_i3dTest.bbox_max[0], g_i3dTest.bbox_max[1], g_i3dTest.bbox_max[2],
             g_i3dTest.scale);
    if (!g_i3dTest.roster.empty())
        LogWaterPreviewPolicy();
    return true;
}

// Re-use --asset=... arg as the asset path selector. If unset, default.
bool InitializeI3DStaticMode()
{
    g_i3dTest.roster.clear();
    g_i3dTest.roster_idx = 0;
    g_i3dTest.transparent_preview = false;
    BuildI3DCharacterChoices();

    if (StartupAssetPath[0])
        return LoadI3DStaticAsset(StartupAssetPath);

    if (!g_i3dTest.character_roster.empty())
    {
        g_i3dTest.character_idx = 0;
        const auto& choice = g_i3dTest.character_roster[g_i3dTest.character_idx];
        return LoadI3DStaticImagery(choice.imageryid, choice.label.c_str());
    }

    return LoadI3DStaticAsset("Misc\\Blood.I3D");
}

bool InitializeWaterPreviewMode()
{
    std::string meta_error;
    const std::filesystem::path meta_path =
        std::filesystem::current_path() / ".." / "data" / "Resources" / "render_metadata.def";
    if (!LoadRenderMetadataFile(meta_path.string().c_str(), g_i3dTest.render_meta, &meta_error))
        log_warn("[water3d] failed to load render metadata '%s': %s",
                 meta_path.string().c_str(), meta_error.c_str());

    g_i3dTest.roster = {
        "Misc\\Water.I3D",
        "Misc\\StillWater.I3D",
        "Misc\\FlowWater.I3D",
        "Misc\\BendWater1.I3D",
        "Misc\\BendWater2.I3D",
        "Misc\\SewerW.I3D",
        "Misc\\Wave.I3D",
        "Misc\\WaveS.I3D",
        "Misc\\WaveM.I3D",
        "Misc\\WFall.I3D",
        "Misc\\WFall2.I3D",
        "Misc\\WCap.I3D",
        "Misc\\WCap2.I3D",
        "Misc\\RiverFall.I3D",
        "Misc\\Box.I3D",
        "Misc\\Axis.I3D",
    };
    g_i3dTest.roster_idx = 0;
    g_i3dTest.character_idx = -1;
    g_i3dTest.transparent_preview = true;
    return LoadI3DStaticAsset(g_i3dTest.roster[g_i3dTest.roster_idx].c_str());
}

void RenderI3DStaticMode()
{
    if (!Renderer || !Display.IsActive() || !Display.BackBuffer()) return;
    if (!EnsureI3DStaticBackgroundAssets()) return;

    g_i3dTest.spin += 0.01f;
    g_i3dTest.frames++;

    const int32_t vw = Display.Width();
    const int32_t vh = Display.Height();
    const int32_t cam_ox = vw / 2;
    const int32_t cam_oy = vh / 2;

    Renderer->SetLight(0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    Renderer->SetAmbientColor(1.0f, 1.0f, 1.0f);
    Renderer->SetAmbientOcclusion(false, 12.0f, 1.0f, 0.15f, 96.0f);
    Renderer->SetNormalLightingHardness(1.0f);
    Renderer->SetLightingMode(1);
    Renderer->SetTileViewMode(0);  // lit preview
    Renderer->SetSunShadow(false, 24.0f, 3.0f, 32);

    // Work in scaled-asset world units: bbox * scale, centered at origin.
    const float s = g_i3dTest.scale;
    const float bbox_w = std::fmax(
        std::fmax(g_i3dTest.bbox_max[0] - g_i3dTest.bbox_min[0],
                  g_i3dTest.bbox_max[1] - g_i3dTest.bbox_min[1]),
                  g_i3dTest.bbox_max[2] - g_i3dTest.bbox_min[2]);
    constexpr float kCam = 2750.0f;
    const float half_z = std::fmax(256.0f, bbox_w * s);
    const float znear = kCam - half_z - 128.0f;
    const float zfar  = kCam + half_z + 128.0f;
    Renderer->SetReconstructionParams(float(cam_ox), float(cam_oy),
                                      znear, zfar, 0.0f, 0.0f, kCam, 0.0f);
    Renderer->ClearPointLights();

    if (g_i3dTest.img)
    {
        g_i3dTest.anim_state = ClampI3DStaticState(g_i3dTest.anim_state);
        if (!g_i3dTest.anim_paused)
            g_i3dTest.anim_time += TTime::DeltaTime() * 24.0 * double(g_i3dTest.anim_speed);
        UpdateI3DStaticFrameFromTime();
    }

    // Apply uniform scale to the I3D mesh (sub 0 is the control triangle
    // which is pre-sized in world units; don't scale that).
    Renderer->BeginTilePass(0.08f, 0.08f, 0.12f, 1.0f);
    const float cx = 0.5f * (g_i3dTest.bbox_min[0] + g_i3dTest.bbox_max[0]);
    const float cy = 0.5f * (g_i3dTest.bbox_min[1] + g_i3dTest.bbox_max[1]);
    const float cz = 0.5f * (g_i3dTest.bbox_min[2] + g_i3dTest.bbox_max[2]);
    {
        STileSubmit bg = {};
        bg.image_pair = CurrentI3DBackgroundImagePair();
        bg.dst_x = 0;
        bg.dst_y = 0;
        bg.dst_w = vw;
        bg.dst_h = vh;
        bg.anchor_z = 0.995f;
        bg.depth_mul = 0.0f;
        bg.normal_mul = 0.0f;
        bg.root_wx = 0.0f;
        bg.root_wy = 0.0f;
        bg.root_wz = 0.0f;
        bg.anchor_px_x = 0.0f;
        bg.anchor_px_y = 0.0f;
        bg.zraw_to_wu = 0.0f;
        Renderer->SubmitTile(bg);
    }

    SAnimPose anim_pose;
    bool have_anim_pose = false;
    if (g_i3dTest.img)
    {
        const int32_t len = CurrentI3DStaticAnimLength();
        const float frac = float(g_i3dTest.anim_time - std::floor(g_i3dTest.anim_time));
        const int32_t next_frame = (g_i3dTest.anim_frame + 1) % len;
        anim_pose = SampleI3DAnimPoseInterpolated(g_i3dTest.img,
            g_i3dTest.anim_state, g_i3dTest.anim_frame, next_frame, frac);
        have_anim_pose = true;
    }

    for (const auto& sub : g_i3dTest.subs) {
        if (g_i3dTest.img && sub.texture_idx >= 0)
        {
            g_i3dTest.img->SetTextureFrame(sub.texture_idx, g_i3dTest.anim_frame);
            S3DTex tex = {};
            g_i3dTest.img->GetTexture(sub.texture_idx, &tex);
            if (tex.htexture != kInvalidTexture)
                Renderer->SetMeshAlbedo(sub.handle, tex.htexture);
        }
        float w[16];
        if (g_i3dTest.img && have_anim_pose)
            BuildAnimPoseObjectMatrix(g_i3dTest.img, anim_pose, g_i3dTest.anim_state, sub.objnum, w);
        else
            std::memcpy(w, sub.world, sizeof(w));
        // Uniform scale in object/world space: scale both basis vectors and
        // translations because the hierarchy stores both in asset units.
        w[0] *= s; w[1] *= s; w[2] *= s; w[3] *= s;
        w[4] *= s; w[5] *= s; w[6] *= s; w[7] *= s;
        w[8] *= s; w[9] *= s; w[10] *= s; w[11] *= s;

        // Recenter the authored asset-space bbox around the preview origin so
        // characters authored away from (0,0,0) still frame on screen.
        w[3]  -= cx * s;
        w[7]  -= cy * s;
        w[11] -= cz * s;
        if (g_i3dTest.transparent_preview)
        {
            SHelperMeshSubmit m = {};
            m.mesh = sub.handle;
            m.additive_blend = g_i3dTest.active_policy &&
                               g_i3dTest.active_policy->blend == ERenderMetaBlend::Additive;
            std::memcpy(m.world, w, sizeof(w));
            m.diffuse[0] = m.diffuse[1] = m.diffuse[2] = m.diffuse[3] = 1.0f;
            m.ambient[0] = m.ambient[1] = m.ambient[2] = m.ambient[3] = 1.0f;
            m.specular[0] = m.specular[1] = m.specular[2] = m.specular[3] = 0.0f;
            m.emissive[0] = m.emissive[1] = m.emissive[2] = 0.0f;
            m.emissive[3] = 1.0f;
            m.power = 1.0f;
            m.sort_depth = kCam - 0.867f * (w[3] + w[7]) - 0.5f * w[11];
            Renderer->SubmitHelperMesh(m);
        }
        else
        {
            SMeshSubmit m = {};
            m.mesh = sub.handle;
            std::memcpy(m.world, w, sizeof(w));
            m.tint[0] = 1.0f; m.tint[1] = 1.0f; m.tint[2] = 1.0f; m.tint[3] = 1.0f;
            Renderer->SubmitMesh(m);
        }
    }
    Renderer->EndTilePass();
    Renderer->RunLightingPass();

    if (!g_i3dTest.logged) {
        g_i3dTest.logged = true;
        log_info("[i3d3d] first-frame: %zu subs, znear=%.1f zfar=%.1f",
                 g_i3dTest.subs.size(), znear, zfar);
    }

    DrawI3DPreviewPanel();
}

void CloseI3DStaticMode()
{
    g_i3dTest.img = nullptr;
    // Drop preview refs. Renderer-owned assets remain resident until the
    // renderer/backend chooses to clear zero-ref assets.
    ClearI3DStaticMeshes();
    g_i3dTest.roster.clear();
    g_i3dTest.character_roster.clear();
    g_i3dTest.roster_idx = 0;
    g_i3dTest.character_idx = -1;
    g_i3dTest.active_asset.clear();
    g_i3dTest.active_imagery_id = -1;
    g_i3dTest.bg_mode = 0;
    g_i3dTest.transparent_preview = false;
    g_i3dTest.logged = false;
}

bool InitializeTTFMode()
{
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

bool InitializeTextMode()
{
    if (!SystemFont)
    {
        log_error("[text] SystemFont is null");
        return true;
    }
    const SFontAtlas* atlas = BuildFontAtlas(SystemFont);
    if (!atlas || atlas->texture == kInvalidTexture)
        log_error("[text] SystemFont atlas build failed");
    return true;
}

bool InitializeIconMode()
{
    if (!SystemFont)
    {
        log_error("[icon] SystemFont is null");
        return true;
    }
    const SFontAtlas* atlas = BuildFontAtlas(SystemFont);
    if (!atlas || atlas->texture == kInvalidTexture)
    {
        log_error("[icon] SystemFont atlas build failed");
        return true;
    }
    log_info("[icon] atlas %dx%d for %d glyphs",
        atlas->width, atlas->height, (int)atlas->numchars);
    return true;
}

bool InitializeI3DMode()
{
    const char* probes[] = {
        "Misc\\Blood.I3D", "Misc\\Sparks.I3D", nullptr
    };
    for (int i = 0; probes[i]; i++)
    {
        const int32_t id = TObjectImagery::FindImagery(probes[i]);
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

bool InitializeFontMode()
{
    log_info("[font] FontTable=%p SystemFont=%p DialogFont=%p SmallFont=%p GameFont=%p",
        (void*)FontTable, (void*)SystemFont, (void*)DialogFont,
        (void*)SmallFont, (void*)GameFont);

    const char* probes[] = {
        "System", "Scroll", "Book", "Dialog", "Small", "Med", "Gold", nullptr
    };
    for (int i = 0; probes[i]; i++)
        LogFontInfo(probes[i]);

    LogFontGlyphAsciiArt("System", 'A');
    if (FontTable)
    {
        for (const char* fn : { "System", "Gold" })
            LogFontGlyphHexDump(fn, 'A');
    }
    return true;
}

void RenderUiMode()
{
    const int32_t tw = Display.Width();
    const int32_t th = Display.Height();
    int32_t cols = 1;
    const int32_t item_count = int32_t(g_uiAtlas.items.size());
    while (cols * cols < item_count) cols++;
    const int32_t rows = cols;

    constexpr int32_t pad = 4;
    int32_t cell_w = (tw - pad) / cols;
    if (cell_w < 16) cell_w = 16;
    const int32_t cell_h = cell_w;
    const int32_t grid_w = cols * cell_w;
    const int32_t total_h = rows * cell_h;
    const int32_t ox = (tw - grid_w) / 2;

    constexpr double kAnimHz = 8.0;
    constexpr double kScrollPxS = 40.0;
    const int32_t tick = (int32_t)(TTime::Time() * kAnimHz);
    const int32_t scroll = total_h > 0
        ? ((int32_t)(TTime::Time() * kScrollPxS) % total_h + total_h) % total_h
        : 0;

    Display.BackBuffer()->StartPass(0.0f, 0.0f, 0.0f, 1.0f);
    for (int i = 0; i < item_count; i++)
    {
        const auto& it = g_uiAtlas.items[i];
        const int32_t fi = it.firstFrame + (it.numFrames > 1 ? (tick % it.numFrames) : 0);
        const auto& fr = g_uiAtlas.frames[fi];

        const int32_t col = i % cols;
        const int32_t row = i / cols;
        const int32_t cx = ox + col * cell_w + cell_w / 2;
        const int32_t dx = cx - fr.w / 2;

        for (int k = 0; k < 2; k++)
        {
            const int32_t cy = row * cell_h + cell_h / 2 - scroll + k * total_h;
            const int32_t dy = cy - fr.h / 2;
            if (dy + fr.h <= 0 || dy >= th) continue;
            Renderer->Composite(
                g_uiAtlas.texture,
                dx, dy, fr.w, fr.h,
                tw, th,
                fr.ax, fr.ay, fr.w, fr.h,
                g_uiAtlas.width, g_uiAtlas.height);
        }
    }
    Display.BackBuffer()->EndPass();
}

void RenderIconMode()
{
    const SFontAtlas* atlas = SystemFont ? FindFontAtlas(SystemFont) : nullptr;
    if (!atlas || atlas->texture == kInvalidTexture)
        return;

    const int32_t tw = Display.Width();
    const int32_t th = Display.Height();
    const int32_t dx = (tw - atlas->width) / 2;
    const int32_t dy = (th - atlas->height) / 2;

    Display.BackBuffer()->StartPass(0.0f, 0.0f, 0.0f, 1.0f);
    Renderer->Composite(atlas->texture, dx, dy, atlas->width, atlas->height, tw, th);
    Display.BackBuffer()->EndPass();
}

void RenderTTFMode()
{
    const int32_t tw = Display.Width();
    const int32_t th = Display.Height();
    const char* msg = "Revenant 0123 !?";
    struct STtfLine { const char* path; int size; };
    const STtfLine lines[] = {
        { "/System/Library/Fonts/Supplemental/Arial.ttf",           28 },
        { "/System/Library/Fonts/Supplemental/Times New Roman.ttf", 48 },
    };

    Display.BackBuffer()->StartPass(0.0f, 0.0f, 0.0f, 1.0f);

    int32_t baseline = 20 + lines[0].size;
    for (const auto& L : lines)
    {
        const SFontAtlas* atlas = BuildTTFAtlas(L.path, L.size);
        if (!atlas || atlas->texture == kInvalidTexture) continue;

        // One shared glyph walk (font.cpp); this mode just centers the line.
        const float pen_x = (tw - TextWidth(atlas, msg)) * 0.5f;
        DrawTextAtBaseline(atlas, msg, pen_x, (float)baseline,
                           1.0f, 1.0f, 1.0f, tw, th);
        baseline += L.size + 16;
    }

    Display.BackBuffer()->EndPass();
}

void RenderTextMode()
{
    if (!FontTable)
        return;

    const int32_t tw = Display.Width();
    const int32_t th = Display.Height();
    const char* msg = "Revenant 0123 !?.";

    struct SLine { TFont* font; const SFontAtlas* atlas; };
    SLine lines[2] = {
        { SystemFont, SystemFont ? BuildFontAtlas(SystemFont) : nullptr },
        { FontTable->Bitmap("Gold"), FontTable->Bitmap("Gold") ? BuildFontAtlas(FontTable->Bitmap("Gold")) : nullptr },
    };

    int32_t total_h = 0;
    for (const auto& L : lines)
        if (L.font) total_h += (int32_t)((TFontData*)L.font)->height + 4;

    Display.BackBuffer()->StartPass(0.0f, 0.0f, 0.0f, 1.0f);

    int32_t line_top = (th - total_h) / 2;
    for (const auto& L : lines)
    {
        if (!L.font || !L.atlas || L.atlas->texture == kInvalidTexture) continue;

        // Bitmap atlases now carry the same metrics as TTF (font.cpp), so this
        // mode funnels through the one shared glyph walk too. pen_y is the line
        // baseline the bitmap StartHeight metric references (line top + height).
        const int32_t line_h = (int32_t)((TFontData*)L.font)->height;
        const int32_t pen_x  = (tw - (int32_t)TextWidth(L.atlas, msg)) / 2;
        const int32_t pen_y  = line_top + line_h;
        DrawTextAtBaseline(L.atlas, msg, (float)pen_x, (float)pen_y,
                           1.0f, 1.0f, 1.0f, tw, th);

        line_top += line_h + 4;
    }

    Display.BackBuffer()->EndPass();
}

void RenderBlankMode()
{
    const double t = TTime::Time();
    const float r = 0.5f + 0.5f * float(std::sin(t * 1.0));
    const float g = 0.5f + 0.5f * float(std::sin(t * 1.3 + 2.0));
    const float b = 0.5f + 0.5f * float(std::sin(t * 1.7 + 4.0));
    Display.BackBuffer()->StartPass(r, g, b, 1.0f);
    Display.BackBuffer()->EndPass();
}

// ====================================================== audio test mode
//
// `--test=audio` — exercise the miniaudio backend in isolation:
//   * lists registered SFX (loaded by SoundPlayer.ReadSoundList) with a
//     Play button per entry
//   * cycles through TrackNN.ogg files under data/MUSIC/ for music
//   * exposes master/sfx/music volume sliders
//
// Useful pre-game-loop ear-test for the backend before sounds are
// triggered organically from inside `--test=sector`.

struct AudioTestState {
    std::vector<std::string> music_tracks;     // absolute paths under data/MUSIC/
    int                      music_idx = 0;
    bool                     music_loop = false;
    float                    master_vol = 1.0f;
    float                    sfx_vol    = 1.0f;
    float                    music_vol  = 0.7f;
    std::string              status;
};
static AudioTestState g_audioTest;

bool InitializeAudioMode()
{
    g_audioTest = {};

    namespace fs = std::filesystem;
    fs::path music_dir = fs::path(RunPath) / "MUSIC";
    std::error_code ec;
    if (fs::is_directory(music_dir, ec)) {
        for (auto& ent : fs::directory_iterator(music_dir, ec)) {
            if (ec) break;
            if (!ent.is_regular_file()) continue;
            auto ext = ent.path().extension().string();
            for (auto& c : ext) c = static_cast<char>(std::tolower(c));
            if (ext == ".ogg" || ext == ".wav")
                g_audioTest.music_tracks.push_back(ent.path().string());
        }
        std::sort(g_audioTest.music_tracks.begin(), g_audioTest.music_tracks.end());
    }
    if (g_audioTest.music_tracks.empty())
        log_warn("[audio] no .ogg/.wav files found under %s", music_dir.string().c_str());

    audio::SetMasterVolume(g_audioTest.master_vol);
    audio::SetSfxVolume   (g_audioTest.sfx_vol);
    audio::SetMusicVolume (g_audioTest.music_vol);

    // Auto-fire the simplest possible playback path: data/open.wav via
    // ma_engine_play_sound. This isolates the engine + device wiring from
    // the streaming / vorbis decoder path used by music.
    {
        std::string wav = (fs::path(RunPath) / "open.wav").string();
        if (fs::exists(wav, ec)) {
            audio::PlayOneShot(wav.c_str());
            log_info("[audio] auto-fired one-shot %s (master_vol=%.2f)",
                     wav.c_str(), audio::GetMasterVolume());
        } else {
            log_warn("[audio] no test wav at %s", wav.c_str());
        }
    }

    // Auto-start the first music track on entry so the user gets an
    // immediate audible signal without having to click Play.
    if (!g_audioTest.music_tracks.empty()) {
        audio::MusicPlayFile(g_audioTest.music_tracks[0].c_str(), /*loop*/ false);
        log_info("[audio] auto-started music %s", g_audioTest.music_tracks[0].c_str());
    }
    return true;
}

void CloseAudioMode()
{
    audio::MusicStop();
}

void RenderAudioMode()
{
    Display.BackBuffer()->StartPass(0.08f, 0.08f, 0.10f, 1.0f);

    ImGui::SetNextWindowSize(ImVec2(620, 500), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Audio Test", nullptr, ImGuiWindowFlags_NoCollapse))
    {
        ImGui::Text("Backend: %s", audio::Functioning() ? "miniaudio (functioning)" : "OFFLINE");
        ImGui::Text("Engine master volume: %.2f", audio::GetMasterVolume());
        ImGui::Separator();

        // Simplest possible playback test — bypasses Source / group code.
        if (ImGui::Button("Fire one-shot open.wav")) {
            std::string wav = (std::filesystem::path(RunPath) / "open.wav").string();
            audio::PlayOneShot(wav.c_str());
        }
        ImGui::Separator();

        // ---- mixer ----
        if (ImGui::SliderFloat("Master##vol", &g_audioTest.master_vol, 0.0f, 1.0f))
            audio::SetMasterVolume(g_audioTest.master_vol);
        if (ImGui::SliderFloat("SFX##vol",    &g_audioTest.sfx_vol,    0.0f, 1.0f))
            audio::SetSfxVolume(g_audioTest.sfx_vol);
        if (ImGui::SliderFloat("Music##vol",  &g_audioTest.music_vol,  0.0f, 1.0f))
            audio::SetMusicVolume(g_audioTest.music_vol);

        // ---- music ----
        ImGui::Separator();
        ImGui::Text("Music: %s",
                    g_audioTest.music_tracks.empty() ? "<no tracks under data/MUSIC>" :
                    audio::MusicPlaying()             ? g_audioTest.music_tracks[g_audioTest.music_idx].c_str()
                                                      : "(stopped)");
        ImGui::Checkbox("Loop", &g_audioTest.music_loop);
        ImGui::SameLine();
        if (ImGui::Button("Prev") && !g_audioTest.music_tracks.empty()) {
            g_audioTest.music_idx = (g_audioTest.music_idx - 1 + int(g_audioTest.music_tracks.size())) %
                                    int(g_audioTest.music_tracks.size());
            audio::MusicPlayFile(g_audioTest.music_tracks[g_audioTest.music_idx].c_str(), g_audioTest.music_loop);
        }
        ImGui::SameLine();
        if (ImGui::Button("Play") && !g_audioTest.music_tracks.empty()) {
            audio::MusicPlayFile(g_audioTest.music_tracks[g_audioTest.music_idx].c_str(), g_audioTest.music_loop);
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop")) audio::MusicStop();
        ImGui::SameLine();
        if (ImGui::Button("Next") && !g_audioTest.music_tracks.empty()) {
            g_audioTest.music_idx = (g_audioTest.music_idx + 1) % int(g_audioTest.music_tracks.size());
            audio::MusicPlayFile(g_audioTest.music_tracks[g_audioTest.music_idx].c_str(), g_audioTest.music_loop);
        }

        // ---- sfx registry ----
        ImGui::Separator();
        ImGui::Text("SFX registry (%d entries)", SoundPlayer.NumItems());
        if (ImGui::BeginChild("sfxlist", ImVec2(0, 0), true)) {
            for (int32_t i = 0; i < SoundPlayer.NumItems(); ++i) {
                PSSoundRef ref = SoundPlayer.GetRef(i);
                if (!ref || !ref->name) continue;
                ImGui::PushID(i);
                if (ImGui::Button("Play")) {
                    if (SoundPlayer.Mount(i)) {
                        SoundPlayer.Play(i);
                        SoundPlayer.Unmount(i);
                    } else {
                        log_warn("[audio] mount failed for sound[%d] '%s'", i, ref->name);
                    }
                }
                ImGui::SameLine();
                ImGui::Text("%s", ref->name);
                ImGui::PopID();
            }
        }
        ImGui::EndChild();
    }
    ImGui::End();

    Display.BackBuffer()->EndPass();
}

// =====================================================================
// Scripted input simulator (--input-script="...", alias --mouse-script).
//
// Replays a synthetic sequence of mouse + keyboard events into the active test
// mode's input dispatch, so UI behavior can be driven and verified headlessly
// (no real input). The script is a timeline: `pause` advances a running clock;
// `moveto`/`left_down`/`key_down`/etc. emit an event at the current clock +
// cursor position. Each frame, InputSimTick() fires every event whose timestamp
// has elapsed (measured against TTime), calling the same HandleMouseMove /
// HandleMouseClick / HandleKeyPress path real input uses. Coordinates are in
// Classic 640x480 content pixels (the UI test modes letterbox internally).
// =====================================================================
enum { MS_MOVE = 0, MS_CLICK, MS_KEY, MS_LOG, MS_SNAP };

struct SInputEvent
{
    double      at_ms = 0.0;   // fire time, ms from script start
    int32_t     kind  = MS_MOVE;
    int32_t     button = 0;    // MB_* for MS_CLICK; VK code for MS_KEY
    int32_t     x = 0, y = 0;  // for MS_KEY: x = 1 (down) / 0 (up)
    std::string text;          // for MS_LOG
};

// Map an input-script key token to a legacy VK_* code (the space the engine's
// KeyPress dispatch uses — see SappKeyToVK in revmain). Accepts a single char
// (letters/digits = uppercase ASCII = VK), a named special key, or a numeric
// VK code. Returns 0 (unknown) on miss.
int32_t InputSimKeyCode(const std::string& tokIn)
{
    if (tokIn.empty()) return 0;
    std::string t = tokIn;
    for (char& c : t) c = (char)std::tolower((unsigned char)c);

    if (t.size() == 1)
    {
        const char c = t[0];
        if (c >= 'a' && c <= 'z') return (int32_t)(c - 'a' + 'A');   // VK letter
        if (c >= '0' && c <= '9') return (int32_t)c;                 // VK digit
    }
    if (t == "esc" || t == "escape")        return VK_ESCAPE;
    if (t == "enter" || t == "return")      return VK_RETURN;
    if (t == "tab")                         return VK_TAB;
    if (t == "space")                       return VK_SPACE;
    if (t == "up")                          return VK_UP;
    if (t == "down")                        return VK_DOWN;
    if (t == "left")                        return VK_LEFT;
    if (t == "right")                       return VK_RIGHT;
    if (t == "backspace" || t == "back")    return VK_BACK;
    if (t == "del" || t == "delete")        return VK_DELETE;
    if (t == "home")                        return VK_HOME;
    if (t == "end")                         return VK_END;
    if (t == "pgup" || t == "pageup")       return VK_PRIOR;
    if (t == "pgdn" || t == "pagedown")     return VK_NEXT;
    if (t.size() >= 2 && t[0] == 'f' && std::isdigit((unsigned char)t[1]))
    {
        const int32_t n = (int32_t)std::strtol(t.c_str() + 1, nullptr, 10);
        if (n >= 1 && n <= 12) return VK_F1 + (n - 1);
    }
    // Numeric VK code fallback.
    char* endp = nullptr;
    const long v = std::strtol(t.c_str(), &endp, 0);
    if (endp && *endp == '\0' && v > 0) return (int32_t)v;
    return 0;
}

std::vector<SInputEvent> g_inputSimEvents;
size_t  g_inputSimNext   = 0;
double  g_inputSimStartMs = 0.0;   // 0 until the first tick stamps it
bool    g_inputSimActive = false;
bool    g_inputSimLoop   = false;
// Latched once we've fired sapp_request_quit() on script drain (see
// auto-exit block at the end of InputSimTick) so we don't spam the quit
// request every frame between request + actual app shutdown.
bool    g_inputSimQuitRequested = false;

// Split a string on a delimiter into trimmed, non-empty tokens.
std::vector<std::string> SplitTokens(const std::string& s, char delim)
{
    std::vector<std::string> out;
    size_t i = 0;
    while (i <= s.size())
    {
        size_t j = s.find(delim, i);
        if (j == std::string::npos) j = s.size();
        size_t a = i, b = j;
        while (a < b && std::isspace((unsigned char)s[a])) ++a;
        while (b > a && std::isspace((unsigned char)s[b - 1])) --b;
        if (b > a) out.emplace_back(s.substr(a, b - a));
        i = j + 1;
    }
    return out;
}

void InputSimStart(const char* script)
{
    g_inputSimEvents.clear();
    g_inputSimNext    = 0;
    g_inputSimStartMs = 0.0;
    g_inputSimActive  = false;
    g_inputSimLoop    = false;
    mousebutton       = 0;   // start with no buttons held (drag-state baseline)
    if (!script || !script[0]) return;

    // Default glide duration for `moveto` — the cursor animates from its
    // current spot to the target over this many ms (so the move is visible and
    // passes hover over intervening widgets), unless an explicit duration arg
    // (incl. 0 for an instant jump) is given.
    constexpr int32_t kDefaultMoveMs = 500;

    // Default hold for the convenience press commands (mouse_click / key_press):
    // down, hold this long, then up. Overridable with a trailing ms arg.
    constexpr int32_t kDefaultPressMs = 200;

    double  t_ms = 0.0;            // running timeline cursor
    int32_t cx = WIDTH / 2;        // running synthetic cursor position
    int32_t cy = HEIGHT / 2;       // (seeded to screen center, like the modes)

    for (const std::string& cmd : SplitTokens(script, ';'))
    {
        std::vector<std::string> tok = SplitTokens(cmd, ' ');
        if (tok.empty()) continue;
        std::string op = tok[0];
        for (char& c : op) c = (char)std::tolower((unsigned char)c);

        auto argi = [&](size_t idx) -> int32_t
        { return idx < tok.size() ? (int32_t)std::strtol(tok[idx].c_str(), nullptr, 10) : 0; };

        if (op == "moveto" || op == "move")
        {
            const int32_t tx = argi(1), ty = argi(2);
            const int32_t dur = (tok.size() > 3) ? argi(3) : kDefaultMoveMs;
            if (dur <= 0)
            {
                // Instant jump.
                g_inputSimEvents.push_back({ t_ms, MS_MOVE, 0, tx, ty, "" });
            }
            else
            {
                // Glide: ~60 Hz intermediate samples, interpolated start->target.
                int32_t steps = dur / 16;
                if (steps < 1)   steps = 1;
                if (steps > 240) steps = 240;
                for (int32_t s = 1; s <= steps; ++s)
                {
                    const int32_t ix = cx + (tx - cx) * s / steps;
                    const int32_t iy = cy + (ty - cy) * s / steps;
                    const double  at = t_ms + (double)dur * s / steps;
                    g_inputSimEvents.push_back({ at, MS_MOVE, 0, ix, iy, "" });
                }
                t_ms += dur;   // the glide consumes timeline; later cmds follow it
            }
            cx = tx; cy = ty;
        }
        else if (op == "pause" || op == "wait" || op == "delay")
        {
            t_ms += (double)argi(1);
        }
        // Discrete button down/up only (NO combined "click") so a press can be
        // held across intervening moves — i.e. drags: left_down; moveto …; left_up.
        else if (op == "left_down" || op == "leftdown" || op == "ldown" || op == "down")
            g_inputSimEvents.push_back({ t_ms, MS_CLICK, MB_LEFTDOWN, cx, cy, "" });
        else if (op == "left_up" || op == "leftup" || op == "lup" || op == "up")
            g_inputSimEvents.push_back({ t_ms, MS_CLICK, MB_LEFTUP, cx, cy, "" });
        else if (op == "right_down" || op == "rightdown")
            g_inputSimEvents.push_back({ t_ms, MS_CLICK, MB_RIGHTDOWN, cx, cy, "" });
        else if (op == "right_up" || op == "rightup")
            g_inputSimEvents.push_back({ t_ms, MS_CLICK, MB_RIGHTUP, cx, cy, "" });
        else if (op == "middle_down" || op == "middledown")
            g_inputSimEvents.push_back({ t_ms, MS_CLICK, MB_MIDDLEDOWN, cx, cy, "" });
        else if (op == "middle_up" || op == "middleup")
            g_inputSimEvents.push_back({ t_ms, MS_CLICK, MB_MIDDLEUP, cx, cy, "" });
        else if (op == "mouse_click" || op == "click" || op == "left_click")
        {
            // Convenience left click: down, hold (default 200ms or arg), up.
            const int32_t hold = (tok.size() > 1) ? argi(1) : kDefaultPressMs;
            g_inputSimEvents.push_back({ t_ms, MS_CLICK, MB_LEFTDOWN, cx, cy, "" });
            t_ms += hold;
            g_inputSimEvents.push_back({ t_ms, MS_CLICK, MB_LEFTUP, cx, cy, "" });
        }
        else if (op == "key_down" || op == "keydown" || op == "key_up" || op == "keyup")
        {
            const int32_t vk = (tok.size() > 1) ? InputSimKeyCode(tok[1]) : 0;
            const int32_t down = (op == "key_down" || op == "keydown") ? 1 : 0;
            if (vk != 0)
                g_inputSimEvents.push_back({ t_ms, MS_KEY, vk, down, 0, "" });
            else
                log_warn("[input-sim] %s: unknown key '%s'", op.c_str(),
                         tok.size() > 1 ? tok[1].c_str() : "");
        }
        else if (op == "key_press" || op == "keypress" || op == "key_click")
        {
            // Convenience key press: key_down, hold (default 200ms or arg), key_up.
            const int32_t vk = (tok.size() > 1) ? InputSimKeyCode(tok[1]) : 0;
            const int32_t hold = (tok.size() > 2) ? argi(2) : kDefaultPressMs;
            if (vk != 0)
            {
                g_inputSimEvents.push_back({ t_ms, MS_KEY, vk, 1, 0, "" });
                t_ms += hold;
                g_inputSimEvents.push_back({ t_ms, MS_KEY, vk, 0, 0, "" });
            }
            else
                log_warn("[input-sim] key_press: unknown key '%s'",
                         tok.size() > 1 ? tok[1].c_str() : "");
        }
        else if (op == "loop")
            g_inputSimLoop = true;
        else if (op == "take_snapshot" || op == "snapshot" || op == "snap")
        {
            // Optional label after the command name:
            //   take_snapshot                     (no label)
            //   take_snapshot click upper book    (free-form label, joined)
            size_t sp = cmd.find(' ');
            std::string label = (sp == std::string::npos) ? "" : cmd.substr(sp + 1);
            g_inputSimEvents.push_back({ t_ms, MS_SNAP, 0, 0, 0, label });
        }
        else if (op == "log")
        {
            size_t sp = cmd.find(' ');
            std::string msg = (sp == std::string::npos) ? "" : cmd.substr(sp + 1);
            g_inputSimEvents.push_back({ t_ms, MS_LOG, 0, 0, 0, msg });
        }
        else
            log_warn("[input-sim] unknown command '%s'", cmd.c_str());
    }

    g_inputSimActive = !g_inputSimEvents.empty();
    if (g_inputSimActive)
        log_info("[input-sim] %zu events parsed (loop=%d)",
                 g_inputSimEvents.size(), g_inputSimLoop ? 1 : 0);
}

}  // namespace

namespace TestModes {

// Defined here (in the TestModes namespace) so it can call the dispatch
// functions directly. Fires all synthetic input events whose timestamp has
// elapsed since the script started.
static void InputSimTick(const char* mode)
{
    if (!g_inputSimActive) return;
    const double now_ms = TTime::Time() * 1000.0;
    if (g_inputSimStartMs == 0.0) g_inputSimStartMs = now_ms;
    const double elapsed = now_ms - g_inputSimStartMs;

    while (g_inputSimNext < g_inputSimEvents.size() &&
           g_inputSimEvents[g_inputSimNext].at_ms <= elapsed)
    {
        const SInputEvent& e = g_inputSimEvents[g_inputSimNext++];
        switch (e.kind)
        {
        case MS_MOVE:
            // Drive the shared cursor position so the in-frame game cursor
            // (TCursorHud, when a mode disables the hardware cursor) follows
            // the script. Same globals the real mouse handler writes. Pass the
            // held-button mask so moves between down/up read as a drag.
            cursorx = e.x; cursory = e.y;
            HandleMouseMove(mode, mousebutton, e.x, e.y);
            break;
        case MS_CLICK:
            cursorx = e.x; cursory = e.y;
            // Mirror the real handler's mousebutton bookkeeping so a held
            // press persists across subsequent moves (drag support).
            switch (e.button)
            {
            case MB_LEFTDOWN:   mousebutton |= MB_LEFTDOWN;    break;
            case MB_LEFTUP:     mousebutton &= ~MB_LEFTDOWN;   break;
            case MB_RIGHTDOWN:  mousebutton |= MB_RIGHTDOWN;   break;
            case MB_RIGHTUP:    mousebutton &= ~MB_RIGHTDOWN;  break;
            case MB_MIDDLEDOWN: mousebutton |= MB_MIDDLEDOWN;  break;
            case MB_MIDDLEUP:   mousebutton &= ~MB_MIDDLEDOWN; break;
            default: break;
            }
            HandleMouseClick(mode, e.button, e.x, e.y);
            break;
        case MS_KEY:
            // e.button = VK code, e.x = 1 (down) / 0 (up). Same path real keys
            // take; real keyboard is NOT gated, so synthetic + real coexist.
            HandleKeyPress(mode, e.button, e.x != 0);
            break;
        case MS_SNAP:
            // Manual filmstrip capture (--filmstrip=N,0). Captures the LAST
            // fully-rendered frame, so put a short `pause` before take_snapshot
            // to let the just-driven state reach the backbuffer. No-op unless a
            // manual-mode filmstrip is active. Optional label (e.text) is
            // baked into the per-frame PNG filename + drawn on the cell.
            if (FrameSnap::TriggerSnapshot(e.text.empty() ? nullptr : e.text.c_str()))
                log_info("[input-sim] take_snapshot%s%s",
                         e.text.empty() ? "" : " label=",
                         e.text.c_str());
            break;
        case MS_LOG:
            log_info("[input-sim] %s", e.text.c_str());
            break;
        }
    }

    if (g_inputSimNext >= g_inputSimEvents.size() && g_inputSimLoop)
    {
        g_inputSimNext    = 0;
        g_inputSimStartMs = now_ms;
    }

    // Auto-exit when a one-shot --input-script drains. Agents have been
    // accidentally leaving Revenant test processes running indefinitely
    // because the test modes never exit on their own (manual filmstrip
    // mode resets for the next batch, looping cycles forever). When the
    // user provided an explicit one-shot script, the test is by
    // definition non-interactive — quit as soon as the script's last
    // event fires. Single-shot guarded by g_inputSimQuitRequested so we
    // don't spam sapp_request_quit() across frames.
    if (!g_inputSimLoop &&
        g_inputSimNext >= g_inputSimEvents.size() &&
        !g_inputSimQuitRequested)
    {
        g_inputSimQuitRequested = true;
        log_info("[input-sim] script drained -- requesting quit");
        sapp_request_quit();
    }
}

bool InputScriptActive()
{
    // Own the mouse only while the script still has work to do: events pending,
    // or looping forever. Once a one-shot script drains, hand control back.
    return g_inputSimActive &&
           (g_inputSimLoop || g_inputSimNext < g_inputSimEvents.size());
}

bool DumpTilesToFolder(const char* path)
{
    return DumpTilesToPath(path);
}

bool DumpIconsToFolder(const char* path)
{
    return DumpIconsToPath(path);
}

bool Initialize(const char* mode)
{
    // Arm the scripted input simulator (no-op if --input-script was not given).
    InputSimStart(StartupInputScript);

    if (strcmp(mode, "blank") == 0 || strcmp(mode, "ticker") == 0)
        return true;
    if (strcmp(mode, "sector") == 0)
        return g_mapRenderer.InitializeFromStartupArgs();
    if (strcmp(mode, "mesh") == 0)
        return InitializeMeshMode();
    if (strcmp(mode, "char3d") == 0)
        return InitializeCharPreviewMode();
    if (strcmp(mode, "tiledump") == 0)
        return InitializeTileDumpMode();
    if (strcmp(mode, "i3ddump") == 0)
        return InitializeI3DDumpMode();
    if (strcmp(mode, "i3d3d") == 0)
        return InitializeI3DStaticMode();
    if (strcmp(mode, "water3d") == 0)
        return InitializeWaterPreviewMode();
    if (strcmp(mode, "ttf") == 0)
        return InitializeTTFMode();
    if (strcmp(mode, "text") == 0)
        return InitializeTextMode();
    if (strcmp(mode, "icon") == 0)
        return InitializeIconMode();
    if (strcmp(mode, "ui") == 0)
    {
        BuildImageryBitmapAtlas(&g_uiAtlas);
        return true;
    }
    if (strcmp(mode, "i3d") == 0)
        return InitializeI3DMode();
    if (strcmp(mode, "font") == 0)
        return InitializeFontMode();
    if (strcmp(mode, "ui-layout") == 0)
        return InitializeUILayoutMode();
    if (strcmp(mode, "ui-anchors") == 0)
        return InitializeUIAnchorMode();
    if (strcmp(mode, "ui-nineslice") == 0)
        return InitializeUINineSliceMode();
    if (strcmp(mode, "ui-style") == 0)
        return InitializeUIStyleMode();
    if (strcmp(mode, "ui-clip") == 0)
        return InitializeUIClipMode();
    if (strcmp(mode, "ui-textbar") == 0)
        return InitializeUITextBarMode();
    if (strcmp(mode, "ui-plyrstatusbar") == 0)
        return InitializeUIPlyrStatusBarMode();
    if (strcmp(mode, "ui-sidetabs") == 0)
        return InitializeUISideTabsMode();
    if (strcmp(mode, "ui-sidebar") == 0)
    {
        SetUISidebarSyntheticStateEnabled(true);
        return InitializeUISidebarMode();
    }
    if (strcmp(mode, "ui-quickspell") == 0)
    {
        SetUIQuickSpellSyntheticStateEnabled(true);
        return InitializeUIQuickSpellMode();
    }
    if (strcmp(mode, "ui-bottombar") == 0)
        return InitializeUIBottomBarMode();
    if (strcmp(mode, "ui-barinv") == 0)
        return InitializeUIBarInvMode();
    if (strcmp(mode, "ui-map") == 0)
        return InitializeUIMapMode();
    if (strcmp(mode, "ui-scrollpane") == 0)
        return InitializeUIScrollPaneMode();
    if (strcmp(mode, "ui-spellbook") == 0)
        return InitializeUISpellbookMode();
    if (strcmp(mode, "ui-spellcreate") == 0)
        return InitializeUISpellCreateMode();
    if (strcmp(mode, "ui-stats") == 0)
        return InitializeUIStatsMode();
    if (strcmp(mode, "ui-equip") == 0)
        return InitializeUIEquipMode();
    if (strcmp(mode, "ui-inventory") == 0)
        return InitializeUIInventoryMode();
    if (strcmp(mode, "ui-hud") == 0)
    {
        SetUISidebarSyntheticStateEnabled(true);
        SetUIQuickSpellSyntheticStateEnabled(true);
        SetUIHudCursorOverlayEnabled(true);
        return InitializeUIHudMode();
    }
    if (strcmp(mode, "ui-loadscreen") == 0)
        return InitializeUILoadScreenMode();
    if (strcmp(mode, "ui-mainmenu") == 0)
        return InitializeUIMainMenuMode();
    if (strcmp(mode, "ui-death") == 0)
        return InitializeUIDeathMode();
    if (IsUIDefScreenMode(mode))
        return InitializeUIDefScreenMode(mode);
    if (strcmp(mode, "audio") == 0)
        return InitializeAudioMode();
    if (strcmp(mode, "vfx") == 0)
        return VfxTest::Initialize();

    log_error("[test] unknown mode '%s' — falling back to blank", mode);
    return true;
}

void Close(const char* mode)
{
    if (strcmp(mode, "sector") == 0)
        g_mapRenderer.Shutdown();
    if (strcmp(mode, "mesh") == 0)
        CloseMeshMode();
    if (strcmp(mode, "char3d") == 0)
        CloseCharPreviewMode();
    if (strcmp(mode, "i3d3d") == 0)
        CloseI3DStaticMode();
    if (strcmp(mode, "water3d") == 0)
        CloseI3DStaticMode();
    if (strcmp(mode, "ui-layout") == 0)
        CloseUILayoutMode();
    if (strcmp(mode, "ui-anchors") == 0)
        CloseUIAnchorMode();
    if (strcmp(mode, "ui-nineslice") == 0)
        CloseUINineSliceMode();
    if (strcmp(mode, "ui-style") == 0)
        CloseUIStyleMode();
    if (strcmp(mode, "ui-clip") == 0)
        CloseUIClipMode();
    if (strcmp(mode, "ui-textbar") == 0)
        CloseUITextBarMode();
    if (strcmp(mode, "ui-plyrstatusbar") == 0)
        CloseUIPlyrStatusBarMode();
    if (strcmp(mode, "ui-sidetabs") == 0)
        CloseUISideTabsMode();
    if (strcmp(mode, "ui-sidebar") == 0)
        CloseUISidebarMode();
    if (strcmp(mode, "ui-quickspell") == 0)
        CloseUIQuickSpellMode();
    if (strcmp(mode, "ui-bottombar") == 0)
        CloseUIBottomBarMode();
    if (strcmp(mode, "ui-barinv") == 0)
        CloseUIBarInvMode();
    if (strcmp(mode, "ui-map") == 0)
        CloseUIMapMode();
    if (strcmp(mode, "ui-scrollpane") == 0)
        CloseUIScrollPaneMode();
    if (strcmp(mode, "ui-spellbook") == 0)
        CloseUISpellbookMode();
    if (strcmp(mode, "ui-spellcreate") == 0)
        CloseUISpellCreateMode();
    if (strcmp(mode, "ui-stats") == 0)
        CloseUIStatsMode();
    if (strcmp(mode, "ui-equip") == 0)
        CloseUIEquipMode();
    if (strcmp(mode, "ui-inventory") == 0)
        CloseUIInventoryMode();
    if (strcmp(mode, "ui-hud") == 0)
        CloseUIHudMode();
    if (strcmp(mode, "ui-loadscreen") == 0)
        CloseUILoadScreenMode();
    if (strcmp(mode, "ui-mainmenu") == 0)
        CloseUIMainMenuMode();
    if (strcmp(mode, "ui-death") == 0)
        CloseUIDeathMode();
    if (IsUIDefScreenMode(mode))
        CloseUIDefScreenMode();
    if (strcmp(mode, "audio") == 0)
        CloseAudioMode();
    if (strcmp(mode, "vfx") == 0)
        VfxTest::Close();
    DestroyBitmapAtlas(&g_uiAtlas);
}

void Render(const char* mode)
{
    if (!Display.IsActive() || !Display.BackBuffer())
        return;

    // Advance the scripted input simulator before painting so any hover/down
    // state change is reflected in this frame (no-op without --input-script).
    InputSimTick(mode);

    if (strcmp(mode, "sector") == 0)
        return g_mapRenderer.RenderFrame();
    if (strcmp(mode, "mesh") == 0)
        return RenderMeshMode();
    if (strcmp(mode, "char3d") == 0)
        return RenderCharPreviewMode();
    if (strcmp(mode, "i3d3d") == 0)
        return RenderI3DStaticMode();
    if (strcmp(mode, "water3d") == 0)
        return RenderI3DStaticMode();
    if (strcmp(mode, "ui") == 0 && g_uiAtlas.texture != kInvalidTexture && !g_uiAtlas.items.empty())
        return RenderUiMode();
    if (strcmp(mode, "icon") == 0)
        return RenderIconMode();
    if (strcmp(mode, "ttf") == 0)
        return RenderTTFMode();
    if (strcmp(mode, "text") == 0)
        return RenderTextMode();
    if (strcmp(mode, "ui-layout") == 0)
        return RenderUILayoutMode();
    if (strcmp(mode, "ui-anchors") == 0)
        return RenderUIAnchorMode();
    if (strcmp(mode, "ui-nineslice") == 0)
        return RenderUINineSliceMode();
    if (strcmp(mode, "ui-style") == 0)
        return RenderUIStyleMode();
    if (strcmp(mode, "ui-clip") == 0)
        return RenderUIClipMode();
    if (strcmp(mode, "ui-textbar") == 0)
        return RenderUITextBarMode();
    if (strcmp(mode, "ui-plyrstatusbar") == 0)
        return RenderUIPlyrStatusBarMode();
    if (strcmp(mode, "ui-sidetabs") == 0)
        return RenderUISideTabsMode();
    if (strcmp(mode, "ui-sidebar") == 0)
        return RenderUISidebarMode();
    if (strcmp(mode, "ui-quickspell") == 0)
        return RenderUIQuickSpellMode();
    if (strcmp(mode, "ui-bottombar") == 0)
        return RenderUIBottomBarMode();
    if (strcmp(mode, "ui-barinv") == 0)
        return RenderUIBarInvMode();
    if (strcmp(mode, "ui-map") == 0)
        return RenderUIMapMode();
    if (strcmp(mode, "ui-scrollpane") == 0)
        return RenderUIScrollPaneMode();
    if (strcmp(mode, "ui-spellbook") == 0)
        return RenderUISpellbookMode();
    if (strcmp(mode, "ui-spellcreate") == 0)
        return RenderUISpellCreateMode();
    if (strcmp(mode, "ui-stats") == 0)
        return RenderUIStatsMode();
    if (strcmp(mode, "ui-equip") == 0)
        return RenderUIEquipMode();
    if (strcmp(mode, "ui-inventory") == 0)
        return RenderUIInventoryMode();
    if (strcmp(mode, "ui-hud") == 0)
        return RenderUIHudMode();
    if (strcmp(mode, "ui-loadscreen") == 0)
        return RenderUILoadScreenMode();
    if (strcmp(mode, "ui-mainmenu") == 0)
        return RenderUIMainMenuMode();
    if (strcmp(mode, "ui-death") == 0)
        return RenderUIDeathMode();
    if (IsUIDefScreenMode(mode))
        return RenderUIDefScreenMode();
    if (strcmp(mode, "audio") == 0)
        return RenderAudioMode();
    if (strcmp(mode, "vfx") == 0)
        return VfxTest::Render();
    return RenderBlankMode();
}

void HandleMouseClick(const char* mode, int32_t button, int32_t x, int32_t y)
{
    if (strcmp(mode, "ui-mainmenu") == 0)
        return HandleMouseClickUIMainMenuMode(button, x, y);
    if (strcmp(mode, "ui-death") == 0)
        return HandleMouseClickUIDeathMode(button, x, y);
    if (IsUIDefScreenMode(mode))
        return HandleMouseClickUIDefScreenMode(button, x, y);
    if (strcmp(mode, "ui-hud") == 0)
    {
        const SHudState& s = GetHudState();
        if (HandleMouseClickUISidebarModeConsumed(button, x, y))
            return;
        if (s.bottomBarOpen ||
            (UIDragState::IsActive() &&
             UIDragState::Get().source == EDragSource::SpellPane))
        {
            HandleMouseClickUIQuickSpellMode(button, x, y);
        }
        if (s.sidebarState == HUD_SIDEBAR_OPEN && s.topSlot == HUD_TOP_BOOK)
            HandleMouseClickUISpellbookMode(button, x, y);
        if (s.sidebarState == HUD_SIDEBAR_OPEN && s.topSlot == HUD_TOP_EQUIP)
            HandleMouseClickUIEquipMode(button, x, y);
        return;
    }
    if (strcmp(mode, "ui-sidebar") == 0)
        return HandleMouseClickUISidebarMode(button, x, y);
    if (strcmp(mode, "ui-quickspell") == 0)
        return HandleMouseClickUIQuickSpellMode(button, x, y);
    if (strcmp(mode, "ui-spellbook") == 0)
        return HandleMouseClickUISpellbookMode(button, x, y);
    if (strcmp(mode, "ui-spellcreate") == 0)
        return HandleMouseClickUISpellCreateMode(button, x, y);
    if (strcmp(mode, "ui-equip") == 0)
        return HandleMouseClickUIEquipMode(button, x, y);
    if (strcmp(mode, "ui-scrollpane") == 0)
        return;  // TODO: add scroll-paging mouse handler if needed
    (void)x; (void)y;
    if (strcmp(mode, "char3d") == 0)
    {
        if (!g_charPreview.inst || g_charPreview.roster.empty())
            return;
        if (button == MB_LEFTDOWN)
        {
            int32_t st = g_charPreview.inst->GetState() + 1;
            if (st >= g_charPreview.inst->NumStates())
                st = 0;
            g_charPreview.inst->SetState(st);
            g_charPreview.last_legacy_tick = -1;
            log_info("[char3d] state -> %d ('%s')",
                     st,
                     g_charPreview.inst->GetImagery()->GetState(st)
                         ? g_charPreview.inst->GetImagery()->GetState(st)->animname
                         : "?");
            return;
        }
        if (button == MB_RIGHTDOWN)
        {
            const int32_t next = (g_charPreview.roster_idx + 1) % int32_t(g_charPreview.roster.size());
            RebuildCharPreviewForRosterIndex(next);
            return;
        }
        if (button == MB_MIDDLEDOWN)
        {
            g_charPreview.paused = !g_charPreview.paused;
            log_info("[char3d] paused=%d", g_charPreview.paused ? 1 : 0);
            return;
        }
        return;
    }
    if (strcmp(mode, "water3d") == 0)
    {
        if (button == MB_LEFTDOWN && !g_i3dTest.roster.empty())
        {
            g_i3dTest.roster_idx = (g_i3dTest.roster_idx + 1) % int32_t(g_i3dTest.roster.size());
            const char* asset = g_i3dTest.roster[g_i3dTest.roster_idx].c_str();
            log_info("[water3d] asset -> %s", asset);
            LoadI3DStaticAsset(asset);
        }
        else if (button == MB_RIGHTDOWN)
        {
            g_i3dTest.bg_mode = (g_i3dTest.bg_mode + 1) % 4;
            log_info("[water3d] background -> %s", kI3DBGNames[g_i3dTest.bg_mode]);
        }
        return;
    }
    if (strcmp(mode, "sector") != 0) return;
    g_mapRenderer.HandleMouseClick(button, x, y);
}

void HandleMouseMove(const char* mode, int32_t button, int32_t x, int32_t y)
{
    if (strcmp(mode, "ui-mainmenu") == 0)
        return HandleMouseMoveUIMainMenuMode(x, y);
    if (strcmp(mode, "ui-death") == 0)
        return HandleMouseMoveUIDeathMode(x, y);
    if (IsUIDefScreenMode(mode))
        return HandleMouseMoveUIDefScreenMode(button, x, y);
    // #8 iOS-style velocity drag for the spellbook scroll
    if (strcmp(mode, "ui-spellbook") == 0)
        return HandleMouseMoveUISpellbookMode(button, x, y);
    if (strcmp(mode, "ui-hud") == 0)
    {
        const SHudState& s = GetHudState();
        if (HandleMouseMoveUISidebarModeConsumed(button, x, y))
            return;
        if (UIDragState::IsActive() &&
            UIDragState::Get().source == EDragSource::SpellPane)
        {
            HandleMouseMoveUIQuickSpellMode(button, x, y);
            return;
        }
        if (s.sidebarState == HUD_SIDEBAR_OPEN && s.topSlot == HUD_TOP_BOOK)
            HandleMouseMoveUISpellbookMode(button, x, y);
        if (s.sidebarState == HUD_SIDEBAR_OPEN && s.topSlot == HUD_TOP_EQUIP)
            HandleMouseMoveUIEquipMode(button, x, y);
        return;
    }
    if (strcmp(mode, "ui-quickspell") == 0)
        return HandleMouseMoveUIQuickSpellMode(button, x, y);
    if (strcmp(mode, "ui-equip") == 0)
        return HandleMouseMoveUIEquipMode(button, x, y);
    if (strcmp(mode, "sector") != 0) return;
    g_mapRenderer.HandleMouseMove(button, x, y);
}

void HandleKeyPress(const char* mode, int32_t key, bool down)
{
    if (strcmp(mode, "vfx") == 0)
    {
        VfxTest::HandleKeyPress(key, down);
        return;
    }
    if (IsUIDefScreenMode(mode))
    {
        HandleKeyPressUIDefScreenMode(key, down);
        return;
    }
    // HUD test modes that compose the sidebar / bottom-bar / six-button
    // strip receive keyboard control: V toggles sidebar, B toggles
    // bottom-bar, 1-6 select panels (per uisidebartest.h docstring).
    if (strcmp(mode, "ui-sidebar") == 0 || strcmp(mode, "ui-hud") == 0)
    {
        HandleKeyPressUISidebarMode(key, down);
        return;
    }
    if (strcmp(mode, "sector") != 0) return;
    g_mapRenderer.HandleKeyPress(key, down);
}

}  // namespace TestModes
