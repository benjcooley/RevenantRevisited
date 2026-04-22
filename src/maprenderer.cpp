// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 maprenderer.cpp - Map renderer object                 *
// *************************************************************************

#include "maprenderer.h"
#include "maprenderer_internal.h"

#include "animimage.h"
#include "animimagebody.h"
#include "bitmap.h"
#include "bitmapdata.h"
#include "chunkcache.h"
#include "decompdata.h"
#include "display.h"
#include "debugui.h"
#include "imgui.h"
#include "imagery.h"
#include "imageres.h"
#include "logging.h"
#include "object.h"
#include "revenant.h"
#include "revdefs.h"
#include "revutils.h"
#include "sector.h"
#include "surface.h"
#include "testconfig.h"
#include "time.h"

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

struct SSectorCoord {
    int32_t sx = 0;
    int32_t sy = 0;
};

struct SSectorSurfaceProbe {
    bool  hit = false;
    FVec3 world = { 0.0f, 0.0f, 0.0f };
    FVec3 normal = { 0.0f, 0.0f, 1.0f };
    float scene_z = 0.0f;
};

constexpr float kIsoCos30     = 867.0f / 1000.0f;
constexpr float kCamForwardWU = 2750.0f;
constexpr float kSectorDepthNearWU = -1024.0f;
constexpr float kSectorDepthFarWU  = 16384.0f;

inline std::filesystem::path NormalizeFsPath(const char* path)
{
    std::string s = (path && path[0]) ? path : ".";
    for (char& ch : s)
        if (ch == '\\')
            ch = '/';
    if (s.empty())
        s = ".";
    return std::filesystem::path(s);
}

inline float CameraDepth(const S3DPoint& rel)
{
    return kCamForwardWU
         - (float(rel.x + rel.y) * kIsoCos30 + float(rel.z) * 0.5f);
}

inline int32_t FloorDiv(int32_t v, int32_t d)
{
    return (v >= 0) ? (v / d) : -(((-v) + d - 1) / d);
}

inline int64_t SectorBinKey(int32_t sx, int32_t sy)
{
    return (int64_t(sx) << 32) ^ uint32_t(sy);
}

inline FVec3 MakeFVec3(float x, float y, float z) { return { x, y, z }; }
inline FVec3 Add(const FVec3& a, const FVec3& b) { return { a.x + b.x, a.y + b.y, a.z + b.z }; }
inline FVec3 Scale(const FVec3& v, float s) { return { v.x * s, v.y * s, v.z * s }; }
inline float Dot(const FVec3& a, const FVec3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline FVec3 Cross(const FVec3& a, const FVec3& b) {
    return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
}
inline FVec3 Normalize(const FVec3& v)
{
    const float len2 = Dot(v, v);
    if (len2 <= 1e-12f) return { 0.0f, 0.0f, 0.0f };
    return Scale(v, 1.0f / std::sqrt(len2));
}

static std::filesystem::path ResolveSectorDataRoot()
{
    namespace fs = std::filesystem;
    if (const char* env = std::getenv("REVENANT_DATA_PATH"))
    {
        std::error_code ec;
        fs::path p(env);
        if (fs::exists(p, ec))
            return p;
    }
    std::error_code ec;
    const fs::path cwd = fs::current_path(ec);
    for (const fs::path& cand : { cwd, cwd / "data", cwd / ".." / "data" })
    {
        if (cand.empty()) continue;
        if (fs::exists(cand / "imagery.rvi", ec) ||
            fs::exists(cand / "Modules", ec) ||
            fs::is_directory(cand / "Curmap", ec) ||
            fs::is_directory(cand / "curmap", ec))
            return fs::canonical(cand, ec);
    }
    return {};
}

static void GatherLevelSectorCoordsFromDir(const std::filesystem::path& dir,
                                           int32_t level,
                                           std::vector<SSectorCoord>& out)
{
    namespace fs = std::filesystem;
    std::error_code ec;
    if (!fs::is_directory(dir, ec)) return;
    for (const fs::directory_entry& entry :
         fs::directory_iterator(dir, fs::directory_options::skip_permission_denied, ec))
    {
        if (ec) break;
        if (!entry.is_regular_file(ec)) continue;
        const std::string name = entry.path().filename().string();
        std::string tail = name;
        const size_t sep = tail.find_last_of("\\/");
        if (sep != std::string::npos && sep + 1 < tail.size()) tail = tail.substr(sep + 1);
        int32_t file_lvl = 0, sx = 0, sy = 0;
        char ext[16] = {};
        if (std::sscanf(tail.c_str(), "%d_%d_%d.%15s", &file_lvl, &sx, &sy, ext) != 4)
            continue;
        if (file_lvl != level || stricmp(ext, "DAT") != 0)
            continue;
        out.push_back({ sx, sy });
    }
}

static std::vector<SSectorCoord> FindLevelSectorCoords(int32_t level)
{
    namespace fs = std::filesystem;
    std::vector<SSectorCoord> coords;
    std::vector<fs::path> scan_dirs;
    scan_dirs.push_back(NormalizeFsPath(CurMapPath) / CURMAPDIR);
    scan_dirs.push_back(NormalizeFsPath(BaseMapPath) / BASEMAPDIR);
    const fs::path data_root = ResolveSectorDataRoot();
    if (!data_root.empty())
    {
        scan_dirs.push_back(data_root / "Curmap");
        scan_dirs.push_back(data_root / "curmap");
        scan_dirs.push_back(data_root / "Map");
        scan_dirs.push_back(data_root / "map");
    }
    std::sort(scan_dirs.begin(), scan_dirs.end(),
        [](const fs::path& a, const fs::path& b) { return a.generic_string() < b.generic_string(); });
    scan_dirs.erase(std::unique(scan_dirs.begin(), scan_dirs.end(),
        [](const fs::path& a, const fs::path& b) { return a.generic_string() == b.generic_string(); }),
        scan_dirs.end());
    for (const fs::path& dir : scan_dirs)
        GatherLevelSectorCoordsFromDir(dir, level, coords);
    {
        char relpath[MAXPATHLEN] = {};
        for (int32_t sy = 0; sy < 64; ++sy)
        for (int32_t sx = 0; sx < 64; ++sx)
        {
            std::snprintf(relpath, sizeof(relpath), CURMAPDIR "\\%d_%d_%d.DAT", level, sx, sy);
            FILE* fp = rev_fopen(relpath, "rb");
            if (!fp)
            {
                std::snprintf(relpath, sizeof(relpath), BASEMAPDIR "\\%d_%d_%d.DAT", level, sx, sy);
                fp = rev_fopen(relpath, "rb");
            }
            if (fp)
            {
                fclose(fp);
                coords.push_back({ sx, sy });
            }
        }
    }
    std::sort(coords.begin(), coords.end(),
        [](const SSectorCoord& a, const SSectorCoord& b) {
            if (a.sy != b.sy) return a.sy < b.sy;
            return a.sx < b.sx;
        });
    coords.erase(std::unique(coords.begin(), coords.end(),
        [](const SSectorCoord& a, const SSectorCoord& b) {
            return a.sx == b.sx && a.sy == b.sy;
        }),
        coords.end());
    return coords;
}

static void ComputeAuthoredLocalDepthRange(int32_t wwidth, int32_t wlength, int32_t wheight,
                                           int32_t wregx, int32_t wregy, int32_t wregz,
                                           bool& has_range, float& out_min, float& out_max)
{
    has_range = false;
    out_min = 0.0f;
    out_max = 0.0f;
    if (wwidth <= 0 || wlength <= 0 || wheight < 0)
        return;
    const float dx[2] = { float(-wregx), float(wwidth - wregx) };
    const float dy[2] = { float(-wregy), float(wlength - wregy) };
    const float dz[2] = { float(-wregz), float(wheight - wregz) };
    float zmin = FLT_MAX;
    float zmax = -FLT_MAX;
    for (int ix = 0; ix < 2; ++ix)
    for (int iy = 0; iy < 2; ++iy)
    for (int iz = 0; iz < 2; ++iz)
    {
        const float local_dz = -(kIsoCos30 * (dx[ix] + dy[iy]) + 0.5f * dz[iz]);
        if (local_dz < zmin) zmin = local_dz;
        if (local_dz > zmax) zmax = local_dz;
    }
    if (zmin <= zmax) {
        has_range = true;
        out_min = zmin;
        out_max = zmax;
    }
}

static bool UploadTileBitmap(PTBitmap bm,
                             sg_image* out_color, sg_image* out_depth,
                             int32_t* out_w, int32_t* out_h,
                             float* out_z_local_min = nullptr,
                             float* out_z_local_max = nullptr,
                             SSectorTileTex::SDepthDump* out_z_dump = nullptr,
                             std::vector<float>* out_cpu_depth_local = nullptr,
                             std::vector<uint8_t>* out_cpu_opaque = nullptr)
{
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
        if (has_zbuffer) for (size_t i = 0; i < npx; ++i) zplane[i] = 0x7F7F;
        for (int32_t by = 0; by < ch; ++by)
        for (int32_t bx = 0; bx < cw; ++bx)
        {
            void* cptr = hdr->block[by * cw + bx].ptr();
            void* zptr = zhdr->block[by * cw + bx].ptr();
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
        uint16_t* zbuf = has_zbuffer ? (uint16_t*)bm->zbuffer.ptr() : nullptr;
        if (is_8bit) std::memcpy(idxplane.get(), bm->data8, npx);
        else std::memcpy(rgbplane16.get(), bm->data16, npx * sizeof(uint16_t));
        if (has_zbuffer && zbuf) std::memcpy(zplane.get(), zbuf, npx * sizeof(uint16_t));
    }
    std::unique_ptr<uint8_t[]> rgba(new uint8_t[npx * 4]);
    std::unique_ptr<float[]> dflt(new float[npx]);
    float z_local_min = FLT_MAX, z_local_max = -FLT_MAX;
    SSectorTileTex::SDepthDump z_dump = {};
    for (size_t i = 0; i < npx; ++i)
    {
        const uint8_t idx8 = is_8bit ? idxplane[i] : 0;
        const uint16_t px16 = is_16bit ? rgbplane16[i] : 0;
        const uint16_t z = has_zbuffer ? zplane[i] : 0;
        const bool transparent = has_zbuffer ? (z == 0x7F7F) : (is_8bit ? (idx8 == key8) : (px16 == key16));
        if (transparent) {
            rgba[i*4+0]=rgba[i*4+1]=rgba[i*4+2]=rgba[i*4+3]=0;
            dflt[i] = 0.0f;
            continue;
        }
        if (is_8bit)
        {
            const uint32_t c = pal->rgbcolors[idx8];
            rgba[i*4+0] = (uint8_t)( c        & 0xFF);
            rgba[i*4+1] = (uint8_t)((c >> 8)  & 0xFF);
            rgba[i*4+2] = (uint8_t)((c >> 16) & 0xFF);
        }
        else
        {
            rgba[i*4+0] = (uint8_t)(((px16 >> 10) & 0x1F) << 3);
            rgba[i*4+1] = (uint8_t)(((px16 >> 5)  & 0x1F) << 3);
            rgba[i*4+2] = (uint8_t)(( px16        & 0x1F) << 3);
        }
        rgba[i*4+3] = 255;
        if (!has_zbuffer) { dflt[i] = 0.0f; continue; }
        ++z_dump.sample_count;
        if (z & 0x8000) ++z_dump.high_bit_count;
        if (z < z_dump.u16_min) z_dump.u16_min = z;
        if (z > z_dump.u16_max) z_dump.u16_max = z;
        const int32_t px = int32_t(i % size_t(w));
        const int32_t py = int32_t(i / size_t(w));
        if (px < z_dump.bbox_x0) z_dump.bbox_x0 = px;
        if (py < z_dump.bbox_y0) z_dump.bbox_y0 = py;
        if (px + 1 > z_dump.bbox_x1) z_dump.bbox_x1 = px + 1;
        if (py + 1 > z_dump.bbox_y1) z_dump.bbox_y1 = py + 1;
        const float z_local = float(int16_t(z));
        dflt[i] = z_local;
        if (z_local < z_local_min) z_local_min = z_local;
        if (z_local > z_local_max) z_local_max = z_local;
    }
    if (z_local_min == FLT_MAX) z_local_min = 0.0f;
    if (z_local_max == -FLT_MAX) z_local_max = 0.0f;
    if (z_dump.sample_count == 0) {
        z_dump.u16_min = z_dump.u16_max = 0;
        z_dump.i16_min = z_dump.i16_max = 0;
        z_dump.bbox_x0 = z_dump.bbox_y0 = 0;
        z_dump.bbox_x1 = z_dump.bbox_y1 = 0;
    }
    sg_image_desc cd = {};
    cd.width = w; cd.height = h;
    cd.pixel_format = SG_PIXELFORMAT_RGBA8;
    cd.min_filter = SG_FILTER_LINEAR;
    cd.mag_filter = SG_FILTER_LINEAR;
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
    if (out_z_local_min) *out_z_local_min = z_local_min;
    if (out_z_local_max) *out_z_local_max = z_local_max;
    if (out_z_dump) *out_z_dump = z_dump;
    if (out_cpu_depth_local) out_cpu_depth_local->assign(dflt.get(), dflt.get() + npx);
    if (out_cpu_opaque)
    {
        out_cpu_opaque->resize(npx);
        for (size_t i = 0; i < npx; ++i)
            (*out_cpu_opaque)[i] = rgba[i * 4 + 3] ? 1 : 0;
    }
    return sg_query_image_state(*out_color) == SG_RESOURCESTATE_VALID &&
           sg_query_image_state(*out_depth) == SG_RESOURCESTATE_VALID;
}

static void DrawArrow(ImDrawList* dl, const ImVec2& a, const ImVec2& b,
                      ImU32 color, float thickness)
{
    dl->AddLine(a, b, color, thickness);
    const ImVec2 d = ImVec2(b.x - a.x, b.y - a.y);
    const float len2 = d.x*d.x + d.y*d.y;
    if (len2 < 1.0f) return;
    const float inv_len = 1.0f / std::sqrt(len2);
    const ImVec2 dir(d.x * inv_len, d.y * inv_len);
    const ImVec2 perp(-dir.y, dir.x);
    const float head = 8.0f, wing = 4.0f;
    const ImVec2 h0(b.x - dir.x * head + perp.x * wing, b.y - dir.y * head + perp.y * wing);
    const ImVec2 h1(b.x - dir.x * head - perp.x * wing, b.y - dir.y * head - perp.y * wing);
    dl->AddLine(b, h0, color, thickness);
    dl->AddLine(b, h1, color, thickness);
}

} // namespace

TMapRenderer::TMapRenderer() : impl(std::make_unique<Impl>()) {}
TMapRenderer::~TMapRenderer() = default;

const char* TMapRenderer::GetDebugTabName() const
{
    return "Map";
}

bool TMapRenderer::InitializeFromStartupArgs()
{
    Impl& s = *impl;
    int32_t keep_lvl = 0, keep_sx = 2, keep_sy = 25;
    bool use_level_origin = false;
    if (StartupLevelId[0])
    {
        int32_t l = 0;
        if (std::sscanf(StartupLevelId, "%d", &l) == 1) {
            keep_lvl = l; keep_sx = 0; keep_sy = 0; use_level_origin = true;
        } else log_warn("[sector] bad --level='%s', expected L", StartupLevelId);
    }
    if (StartupSectorId[0])
    {
        int32_t l=0,x=0,y=0;
        if (std::sscanf(StartupSectorId, "%d_%d_%d", &l, &x, &y) == 3) {
            keep_lvl = l; keep_sx = x; keep_sy = y; use_level_origin = false;
        } else log_warn("[sector] bad --sector='%s', expected L_X_Y", StartupSectorId);
    }

    extern int32_t g_loadObjNullObjVerNeg, g_loadObjNullClassNeg,
                   g_loadObjNullBadClass,  g_loadObjNullBadType,
                   g_loadObjNullNewObjFail, g_loadObjNullCorruptDrop,
                   g_loadObjOk;
    g_loadObjNullObjVerNeg = g_loadObjNullClassNeg = g_loadObjNullBadClass =
    g_loadObjNullBadType = g_loadObjNullNewObjFail = g_loadObjNullCorruptDrop =
    g_loadObjOk = 0;

    struct SLoaded { int32_t lvl, sx, sy; TSector* sec; };
    std::vector<SLoaded> loaded;
    int32_t loaded_obj_total = 0;
    int32_t loaded_light_total = 0;
    int32_t loaded_anim_total = 0;
    std::vector<SSectorCoord> load_coords = FindLevelSectorCoords(keep_lvl);
    if (load_coords.empty())
    {
        if (use_level_origin)
        {
            log_error("[sector] level %d: no sectors found; aborting startup", keep_lvl);
            return false;
        }
        log_warn("[sector] no level-wide sector scan for level %d; falling back to 5x5 around %d_%d",
            keep_lvl, keep_sx, keep_sy);
        for (int32_t dy = -2; dy <= 2; ++dy)
        for (int32_t dx = -2; dx <= 2; ++dx)
            load_coords.push_back({ keep_sx + dx, keep_sy + dy });
    }
    else
    {
        int32_t sx_min = load_coords.front().sx, sx_max = load_coords.front().sx;
        int32_t sy_min = load_coords.front().sy, sy_max = load_coords.front().sy;
        for (const auto& c : load_coords) {
            sx_min = (std::min)(sx_min, c.sx); sx_max = (std::max)(sx_max, c.sx);
            sy_min = (std::min)(sy_min, c.sy); sy_max = (std::max)(sy_max, c.sy);
        }
        log_info("[sector] level %d scan: %zu sectors x=[%d..%d] y=[%d..%d] focus=%d_%d",
            keep_lvl, load_coords.size(), sx_min, sx_max, sy_min, sy_max, keep_sx, keep_sy);
    }

    for (const auto& coord : load_coords)
    {
        const int32_t sx = coord.sx, sy = coord.sy;
        log_info("[sector] -> loading %d_%d_%d", keep_lvl, sx, sy);
        TSector* sec = TSector::LoadSector(keep_lvl, sx, sy, false);
        if (!sec) {
            log_warn("[sector] %d_%d_%d: LoadSector failed", keep_lvl, sx, sy);
            continue;
        }
        log_info("[sector] <- %d_%d_%d: %d objects (lights=%d anim=%d)",
            keep_lvl, sx, sy, sec->NumItems(),
            sec->NumObjSetItems(OBJSET_LIGHTS),
            sec->NumObjSetItems(OBJSET_ANIMATE));
        loaded_obj_total += sec->NumItems();
        loaded_light_total += sec->NumObjSetItems(OBJSET_LIGHTS);
        loaded_anim_total += sec->NumObjSetItems(OBJSET_ANIMATE);
        loaded.push_back({ keep_lvl, sx, sy, sec });
        s.sectorsKept.push_back(sec);
    }
    if (loaded.empty())
    {
        if (use_level_origin)
            log_error("[sector] level %d: found sector names but loaded none; aborting startup", keep_lvl);
        return !use_level_origin;
    }

    int32_t loaded_sx_min = loaded.front().sx, loaded_sx_max = loaded.front().sx;
    int32_t loaded_sy_min = loaded.front().sy, loaded_sy_max = loaded.front().sy;
    for (const auto& L : loaded)
    {
        loaded_sx_min = (std::min)(loaded_sx_min, L.sx);
        loaded_sx_max = (std::max)(loaded_sx_max, L.sx);
        loaded_sy_min = (std::min)(loaded_sy_min, L.sy);
        loaded_sy_max = (std::max)(loaded_sy_max, L.sy);
    }
    log_info("[sector] load summary: sectors=%zu/%zu objects=%d lights=%d anim=%d x=[%d..%d] y=[%d..%d]",
        loaded.size(), load_coords.size(),
        loaded_obj_total, loaded_light_total, loaded_anim_total,
        loaded_sx_min, loaded_sx_max, loaded_sy_min, loaded_sy_max);

    struct WorldInst {
        int32_t tex_idx;
        S3DPoint wpos;
        int32_t regx, regy, regz;
        bool has_authored_local_dz;
        float authored_local_dz_min, authored_local_dz_max;
        int32_t wwidth, wlength, wheight;
        int32_t wregx, wregy, wregz;
        TSafeRef<> oi;
    };
    std::vector<WorldInst> work;
    int32_t total_tiles = 0, non_2d = 0, bad_state = 0, no_still = 0, upload_fail = 0, no_img = 0, no_body = 0;

    for (auto& L : loaded)
    {
        TSector* sec = L.sec;
        for (int32_t i = 0; i < sec->NumItems(); ++i)
        {
            TObjectInstance* oi = sec->GetInstance(i);
            if (!oi) continue;
            if (oi->ObjClass() != OBJCLASS_TILE) continue;
            ++total_tiles;
            TObjectImagery* img = oi->GetImagery();
            if (!img) { ++no_img; continue; }
            SImageryHeader* hdr = img->GetHeader();
            SImageryBody* body = img->GetBody();
            if (!hdr || !body) { ++no_body; continue; }
            if (hdr->imageryid != OBJIMAGE_ANIMATION) { ++non_2d; continue; }
            auto* ab = (SAnimImageryBody*)body;
            const int32_t st = oi->GetState();
            if (st < 0 || st >= hdr->numstates) { ++bad_state; continue; }
            PTBitmap bm = (TBitmap*)ab->states[st].still;
            if (!bm) { ++no_still; continue; }
            int32_t tex_idx = -1;
            for (size_t t = 0; t < s.sectorTileTex.size(); ++t)
                if (s.sectorTileTex[t].bm_key == bm) { tex_idx = (int32_t)t; break; }
            if (tex_idx < 0)
            {
                SSectorTileTex t = {};
                t.bm_key = bm;
                t.debug_classname = oi->GetClassName();
                t.debug_world_pos = oi->Pos();
                t.debug_regx = img->GetRegX(st);
                t.debug_regy = img->GetRegY(st);
                t.debug_regz = img->GetRegZ(st);
                if (!UploadTileBitmap(bm, &t.color, &t.depth, &t.w, &t.h,
                                      &t.z_local_min, &t.z_local_max,
                                      &t.z_dump, &t.cpu_depth_local, &t.cpu_opaque))
                { ++upload_fail; continue; }
                tex_idx = (int32_t)s.sectorTileTex.size();
                s.sectorTileTex.push_back(std::move(t));
            }
            work.push_back({ tex_idx, oi->Pos(),
                             img->GetRegX(st), img->GetRegY(st), img->GetRegZ(st),
                             false, 0.0f, 0.0f, 0,0,0,0,0,0, TSafeRef<>(oi) });
            WorldInst& winst = work.back();
            int32_t wwidth = 0, wlength = 0, wheight = 0;
            img->GetWorldBoundBox(st, wwidth, wlength, wheight);
            winst.wwidth = wwidth; winst.wlength = wlength; winst.wheight = wheight;
            winst.wregx = img->GetWorldRegX(st); winst.wregy = img->GetWorldRegY(st); winst.wregz = img->GetWorldRegZ(st);
            ComputeAuthoredLocalDepthRange(wwidth, wlength, wheight,
                                           winst.wregx, winst.wregy, winst.wregz,
                                           winst.has_authored_local_dz,
                                           winst.authored_local_dz_min,
                                           winst.authored_local_dz_max);
        }
    }
    log_info("[sector] tile scan: total=%d kept=%zu non_2d=%d drops: no_img=%d no_body=%d bad_state=%d no_still=%d upload_fail=%d",
        total_tiles, work.size(), non_2d, no_img, no_body, bad_state, no_still, upload_fail);

    const int32_t focus_wx0 = (keep_sx - 1) * SECTORWIDTH;
    const int32_t focus_wx1 = (keep_sx + 2) * SECTORWIDTH;
    const int32_t focus_wy0 = (keep_sy - 1) * SECTORHEIGHT;
    const int32_t focus_wy1 = (keep_sy + 2) * SECTORHEIGHT;
    struct SViewBounds { int32_t sx_min=INT32_MAX,sx_max=INT32_MIN,sy_min=INT32_MAX,sy_max=INT32_MIN,wx_min=INT32_MAX,wx_max=INT32_MIN,wy_min=INT32_MAX,wy_max=INT32_MIN,count=0; } focus_bounds, all_bounds;
    struct SOccupiedSectorStat {
        int32_t sx = 0, sy = 0;
        int32_t count = 0;
        int32_t wx_min = INT32_MAX, wx_max = INT32_MIN;
        int32_t wy_min = INT32_MAX, wy_max = INT32_MIN;
    };
    auto accumulate_bounds = [](SViewBounds& b, const S3DPoint& wpos, const S3DPoint& sp, int32_t regx, int32_t regy, int32_t tw, int32_t th)
    {
        const int32_t x0 = sp.x - regx, y0 = sp.y - regy;
        const int32_t x1 = x0 + tw, y1 = y0 + th;
        b.sx_min = (std::min)(b.sx_min, x0); b.sy_min = (std::min)(b.sy_min, y0);
        b.sx_max = (std::max)(b.sx_max, x1); b.sy_max = (std::max)(b.sy_max, y1);
        b.wx_min = (std::min)(b.wx_min, wpos.x); b.wx_max = (std::max)(b.wx_max, wpos.x);
        b.wy_min = (std::min)(b.wy_min, wpos.y); b.wy_max = (std::max)(b.wy_max, wpos.y);
        ++b.count;
    };
    std::unordered_map<int64_t, SOccupiedSectorStat> occupied_sector_stats;
    for (auto& w : work)
    {
        S3DPoint sp; WorldToScreen(w.wpos, sp);
        const auto& tex = s.sectorTileTex[w.tex_idx];
        accumulate_bounds(all_bounds, w.wpos, sp, w.regx, w.regy, tex.w, tex.h);
        const int32_t occ_sx = FloorDiv(w.wpos.x, SECTORWIDTH);
        const int32_t occ_sy = FloorDiv(w.wpos.y, SECTORHEIGHT);
        SOccupiedSectorStat& stat = occupied_sector_stats[SectorBinKey(occ_sx, occ_sy)];
        stat.sx = occ_sx;
        stat.sy = occ_sy;
        stat.count++;
        stat.wx_min = (std::min)(stat.wx_min, w.wpos.x);
        stat.wx_max = (std::max)(stat.wx_max, w.wpos.x);
        stat.wy_min = (std::min)(stat.wy_min, w.wpos.y);
        stat.wy_max = (std::max)(stat.wy_max, w.wpos.y);
        if (w.wpos.x >= focus_wx0 && w.wpos.x < focus_wx1 && w.wpos.y >= focus_wy0 && w.wpos.y < focus_wy1)
            accumulate_bounds(focus_bounds, w.wpos, sp, w.regx, w.regy, tex.w, tex.h);
    }
    const SViewBounds& view_bounds = (use_level_origin || focus_bounds.count <= 0) ? all_bounds : focus_bounds;
    const int32_t tw = Display ? Display->Width() : 1024;
    const int32_t th = Display ? Display->Height() : 768;
    const int32_t bw = view_bounds.sx_max - view_bounds.sx_min;
    const int32_t bh = view_bounds.sy_max - view_bounds.sy_min;
    if (use_level_origin) {
        s.sectorCenterOx = tw / 2;
        s.sectorCenterOy = th / 2;
        s.sectorWorldCenter = {0,0,0};
        const int32_t level_cx = (all_bounds.wx_min + all_bounds.wx_max) / 2;
        const int32_t level_cy = (all_bounds.wy_min + all_bounds.wy_max) / 2;
        const SOccupiedSectorStat* startup_sector = nullptr;
        int64_t best_dist2 = INT64_MAX;
        for (const auto& entry : occupied_sector_stats)
        {
            const SOccupiedSectorStat& stat = entry.second;
            const int32_t stat_cx = (stat.wx_min + stat.wx_max) / 2;
            const int32_t stat_cy = (stat.wy_min + stat.wy_max) / 2;
            const int64_t dx = int64_t(stat_cx) - int64_t(level_cx);
            const int64_t dy = int64_t(stat_cy) - int64_t(level_cy);
            const int64_t dist2 = dx * dx + dy * dy;
            if (!startup_sector ||
                dist2 < best_dist2 ||
                (dist2 == best_dist2 && stat.count > startup_sector->count))
            {
                startup_sector = &stat;
                best_dist2 = dist2;
            }
        }
        if (startup_sector)
        {
            s.sectorCameraWorld.x = (startup_sector->wx_min + startup_sector->wx_max) / 2;
            s.sectorCameraWorld.y = (startup_sector->wy_min + startup_sector->wy_max) / 2;
            log_info("[sector] startup occupied sector: %d_%d (%d tiles)",
                startup_sector->sx, startup_sector->sy, startup_sector->count);
        }
        else
        {
            s.sectorCameraWorld.x = level_cx;
            s.sectorCameraWorld.y = level_cy;
        }
        s.sectorCameraWorld.z = 0;
    } else {
        if (focus_bounds.count <= 0)
            log_warn("[sector] focus sector %d_%d has no tile anchors; centering exact sector coordinates", keep_sx, keep_sy);
        s.sectorWorldCenter = {
            keep_sx * SECTORWIDTH + SECTORWIDTH / 2,
            keep_sy * SECTORHEIGHT + SECTORHEIGHT / 2,
            0
        };
        s.sectorCameraWorld = s.sectorWorldCenter;
        S3DPoint sector_center_screen;
        WorldToScreen(s.sectorWorldCenter, sector_center_screen);
        s.sectorCenterOx = tw / 2 - sector_center_screen.x;
        s.sectorCenterOy = th / 2 - sector_center_screen.y;
    }

    float sz_min = FLT_MAX, sz_max = -FLT_MAX;
    for (const auto& w : work)
    {
        const auto& tex = s.sectorTileTex[w.tex_idx];
        const float camera_z = CameraDepth(w.wpos - s.sectorCameraWorld);
        const float z0 = w.has_authored_local_dz ? (camera_z + w.authored_local_dz_min)
                                                 : (camera_z + (tex.z_local_min - float(w.regz)));
        const float z1 = w.has_authored_local_dz ? (camera_z + w.authored_local_dz_max)
                                                 : (camera_z + (tex.z_local_max - float(w.regz)));
        sz_min = (std::min)(sz_min, z0); sz_max = (std::max)(sz_max, z1);
    }
    s.sectorSceneZMin = sz_min; s.sectorSceneZMax = sz_max;
    for (const auto& w : work)
    {
        SSectorTileInst inst = {};
        inst.tex_idx = w.tex_idx; inst.world_pos = w.wpos; inst.regx = w.regx; inst.regy = w.regy; inst.regz = w.regz;
        inst.has_authored_local_dz = w.has_authored_local_dz;
        inst.authored_local_dz_min = w.authored_local_dz_min;
        inst.authored_local_dz_max = w.authored_local_dz_max;
        inst.wwidth = w.wwidth; inst.wlength = w.wlength; inst.wheight = w.wheight;
        inst.wregx = w.wregx; inst.wregy = w.wregy; inst.wregz = w.wregz;
        inst.src = w.oi;
        s.sectorTileInst.push_back(inst);
    }
    s.rebuildBins();
    for (auto& Ls : loaded)
    {
        TSector* sec = Ls.sec;
        for (int32_t i = 0; i < sec->NumItems(); ++i)
        {
            TObjectInstance* oi = sec->GetInstance(i);
            if (!oi || !oi->IsLight()) continue;
            PSLightDef ld = oi->GetLightDef();
            if (!ld || ld->intensity == 0) continue;
            s.sectorLights.push_back({ TSafeRef<>(oi), true });
        }
    }
    log_info("[sector] built tile list: %zu instances, %zu unique bitmaps",
        s.sectorTileInst.size(), s.sectorTileTex.size());
    log_info("[sector] renderable summary: tile_instances=%zu unique_bitmaps=%zu point_lights=%zu",
        s.sectorTileInst.size(), s.sectorTileTex.size(), s.sectorLights.size());
    log_info("[sector]   startup focus bbox=%dx%d (%d tiles) scene_z=[%.0f..%.0f] wu  anchor=(%d,%d,%d)",
        bw, bh, view_bounds.count, sz_min, sz_max,
        s.sectorWorldCenter.x, s.sectorWorldCenter.y, s.sectorWorldCenter.z);
    DebugUI::RegisterContributor(this);
    return true;
}

void TMapRenderer::Shutdown()
{
    Impl& s = *impl;
    for (auto& t : s.sectorTileTex)
    {
        if (t.color.id) sg_destroy_image(t.color);
        if (t.depth.id) sg_destroy_image(t.depth);
    }
    s.sectorTileTex.clear();
    s.sectorTileInst.clear();
    s.sectorTileBins.clear();
    for (TSector* sec : s.sectorsKept) if (sec) TSector::CloseSector(sec);
    s.sectorsKept.clear();
    s.sectorLights.clear();
    s.lightDragIdx = -1;
    s.sectorDragging = false;
    DebugUI::UnregisterContributor(this);
}

void TMapRenderer::RenderFrame()
{
    Impl& s = *impl;
    if (!Display || !Display->BackBuffer() || s.sectorTileInst.empty())
        return;
    const int32_t vw = Display->Width();
    const int32_t vh = Display->Height();
    int32_t cam_ox = 0, cam_oy = 0;
    s.sectorCameraOriginScreen(cam_ox, cam_oy);

    float scene_z_min_fit = FLT_MAX, scene_z_max_fit = -FLT_MAX;
    int32_t fit_tiles = 0;
    for (const auto& inst : s.sectorTileInst)
    {
        const auto& tex = s.sectorTileTex[inst.tex_idx];
        S3DPoint sp; s.sectorProjectWorld(inst.world_pos, sp);
        const int32_t x0 = sp.x - inst.regx + cam_ox, y0 = sp.y - inst.regy + cam_oy;
        const int32_t x1 = x0 + tex.w, y1 = y0 + tex.h;
        if (x1 <= 0 || y1 <= 0 || x0 >= vw || y0 >= vh) continue;
        const float camera_z = s.sectorCameraSceneZ(inst);
        const float z0 = inst.has_authored_local_dz ? (camera_z + s.depth_mul * inst.authored_local_dz_min)
                                                    : (camera_z + s.depth_mul * (tex.z_local_min - float(inst.regz)));
        const float z1 = inst.has_authored_local_dz ? (camera_z + s.depth_mul * inst.authored_local_dz_max)
                                                    : (camera_z + s.depth_mul * (tex.z_local_max - float(inst.regz)));
        scene_z_min_fit = (std::min)(scene_z_min_fit, z0);
        scene_z_max_fit = (std::max)(scene_z_max_fit, z1);
        ++fit_tiles;
    }
    if (!(scene_z_min_fit < scene_z_max_fit)) {
        scene_z_min_fit = s.sectorSceneZMin;
        scene_z_max_fit = s.sectorSceneZMax;
    }
    s.debugSceneZMinFit = scene_z_min_fit;
    s.debugSceneZMaxFit = scene_z_max_fit;
    s.debugFitTiles = fit_tiles;

    if (s.animate)
    {
        ++s.tick;
        const float t = float(s.tick) * (6.2831853f / 180.0f);
        s.puck_u = 0.8f * std::cos(t);
        s.puck_v = 0.5f + 0.4f * std::sin(t);
    }
    {
        const float r2 = s.puck_u * s.puck_u + s.puck_v * s.puck_v;
        if (r2 > 1.0f) {
            const float k = 1.0f / std::sqrt(r2);
            s.puck_u *= k; s.puck_v *= k;
        }
    }
    {
        const float r2 = s.puck_u * s.puck_u + s.puck_v * s.puck_v;
        const float cam_x_shadow = -s.puck_u;
        const float cam_y_shadow = -s.puck_v;
        const float cam_x_light = -cam_x_shadow;
        const float cam_y_light = -cam_y_shadow;
        const float cam_z = std::sqrt(fmaxf(0.0f, 1.0f - r2));
        const float fx = kIsoCos30, fy = kIsoCos30, fz = 1.0f;
        const float fn = std::sqrt(fx*fx + fy*fy + fz*fz);
        const float FwX = fx / fn, FwY = fy / fn, FwZ = fz / fn;
        const float rx = 1.0f, ry = -1.0f, rz = 0.0f;
        const float rn = std::sqrt(rx*rx + ry*ry + rz*rz);
        const float RwX = rx / rn, RwY = ry / rn, RwZ = rz / rn;
        const float UwX = -FwY * RwZ + FwZ * RwY;
        const float UwY = -FwZ * RwX + FwX * RwZ;
        const float UwZ = -FwX * RwY + FwY * RwX;
        s.dir[0] = cam_x_shadow * RwX + cam_y_shadow * UwX + cam_z * FwX;
        s.dir[1] = cam_x_shadow * RwY + cam_y_shadow * UwY + cam_z * FwY;
        s.dir[2] = cam_x_shadow * RwZ + cam_y_shadow * UwZ + cam_z * FwZ;
        s.light_dir[0] = cam_x_light * RwX + cam_y_light * UwX + cam_z * FwX;
        s.light_dir[1] = cam_x_light * RwY + cam_y_light * UwY + cam_z * FwY;
        s.light_dir[2] = cam_x_light * RwZ + cam_y_light * UwZ + cam_z * FwZ;
    }
    if (!s.z_range_logged)
    {
        s.z_range_logged = true;
        log_info("[sector] fixed z window [%.0f..%.0f] wu (visible scene=[%.0f..%.0f])",
            s.z_near, s.z_far, scene_z_min_fit, scene_z_max_fit);
    }

    // Global debug UI owns the ImGui shell; it will call DrawDebugTab() on
    // this renderer as a contributor.

    Display->SetLight(s.light_dir[0], s.light_dir[1], s.light_dir[2], s.intensity, s.color[0], s.color[1], s.color[2], s.ambient);
    Display->SetAmbientColor(s.ambient_color[0], s.ambient_color[1], s.ambient_color[2]);
    Display->SetAmbientOcclusion(s.ao_enable, s.ao_radius_px, s.ao_strength, s.ao_bias, s.ao_max_dist);
    Display->SetShadowWorldDir(s.dir[0], s.dir[1], s.dir[2]);
    Display->SetShadowVariance(s.sdir_off_x, s.sdir_off_y, s.sdir_wz_mul);
    Display->SetNormalLightingHardness(s.normal_hardness);
    Display->SetNormalRadius(s.normal_radius);
    Display->SetEdgeThreshold(s.edge_thr);
    Display->SetTileViewMode(s.view_mode);
    Display->SetLightingMode(s.lighting_mode);
    Display->SetSunShadow(s.sun_shadow, s.sun_shadow_step, s.sun_shadow_soft, s.sun_shadow_max);
    Display->SetReconstructionParams(float(cam_ox), float(cam_oy), s.z_near, s.z_far,
                                     float(s.sectorCameraWorld.x), float(s.sectorCameraWorld.y),
                                     kCamForwardWU, 0.0f);

    Display->ClearPointLights();
    if (s.lights_on)
    {
        S3DPoint vc_rel;
        ScreenToWorld(vw / 2 - cam_ox, vh / 2 - cam_oy, vc_rel, 0);
        const S3DPoint vc_w = vc_rel + s.sectorCameraWorld;
        struct Pick { int32_t light_idx; float d2_to_view; };
        Pick picks[TDisplay::kMaxPointLights];
        int32_t pick_n = 0;
        for (int32_t i = 0; i < int32_t(s.sectorLights.size()); ++i)
        {
            const SSectorLight& L = s.sectorLights[i];
            if (!L.enabled) continue;
            const S3DPoint wp = s.sectorLightPos(L);
            S3DPoint sp; s.sectorProjectWorld(wp, sp);
            const int32_t sx = sp.x + cam_ox;
            const int32_t sy = sp.y + cam_oy;
            const float r_px = s.sectorLightRadius(L) * s.radius_mul;
            if (sx + r_px < 0 || sx - r_px >= vw) continue;
            if (sy + r_px < 0 || sy - r_px >= vh) continue;
            const float dx = float(wp.x - vc_w.x), dy = float(wp.y - vc_w.y), dz = float(wp.z - vc_w.z);
            const float d2 = dx*dx + dy*dy + dz*dz;
            if (pick_n < TDisplay::kMaxPointLights) picks[pick_n++] = { i, d2 };
            else {
                int32_t worst = 0;
                for (int32_t k = 1; k < pick_n; ++k)
                    if (picks[k].d2_to_view > picks[worst].d2_to_view) worst = k;
                if (d2 < picks[worst].d2_to_view) picks[worst] = { i, d2 };
            }
        }
        for (int32_t k = 0; k < pick_n; ++k)
        {
            const SSectorLight& L = s.sectorLights[picks[k].light_idx];
            const S3DPoint wp = s.sectorLightPos(L);
            float rgb[3]; s.sectorLightColor(L, rgb);
            Display->AddPointLight(float(wp.x), float(wp.y), float(wp.z),
                                   s.sectorLightRadius(L) * s.radius_mul,
                                   rgb[0], rgb[1], rgb[2],
                                   s.sectorLightIntensity(L) * s.intensity_mul);
        }
    }

    const float zspan = s.z_far - s.z_near;
    Display->BeginTilePass(0.12f, 0.16f, 0.10f, 1.0f);
    static bool draw_stats_logged = false;
    int32_t draw_submitted = 0, draw_invalid_img = 0, draw_offscreen = 0;

    S3DPoint c0, c1, c2, c3;
    ScreenToWorld(-cam_ox - TDisplay::kGBufPad, -cam_oy - TDisplay::kGBufPad, c0, 0);
    ScreenToWorld(vw - cam_ox + TDisplay::kGBufPad, -cam_oy - TDisplay::kGBufPad, c1, 0);
    ScreenToWorld(-cam_ox - TDisplay::kGBufPad, vh - cam_oy + TDisplay::kGBufPad, c2, 0);
    ScreenToWorld(vw - cam_ox + TDisplay::kGBufPad, vh - cam_oy + TDisplay::kGBufPad, c3, 0);
    const int32_t min_wx = (std::min)((std::min)(c0.x, c1.x), (std::min)(c2.x, c3.x)) + s.sectorCameraWorld.x - SECTORWIDTH;
    const int32_t max_wx = (std::max)((std::max)(c0.x, c1.x), (std::max)(c2.x, c3.x)) + s.sectorCameraWorld.x + SECTORWIDTH;
    const int32_t min_wy = (std::min)((std::min)(c0.y, c1.y), (std::min)(c2.y, c3.y)) + s.sectorCameraWorld.y - SECTORHEIGHT;
    const int32_t max_wy = (std::max)((std::max)(c0.y, c1.y), (std::max)(c2.y, c3.y)) + s.sectorCameraWorld.y + SECTORHEIGHT;
    const int32_t min_sx = FloorDiv(min_wx, SECTORWIDTH);
    const int32_t max_sx = FloorDiv(max_wx, SECTORWIDTH);
    const int32_t min_sy = FloorDiv(min_wy, SECTORHEIGHT);
    const int32_t max_sy = FloorDiv(max_wy, SECTORHEIGHT);

    constexpr int32_t kCovCellPx = 32;
    const int32_t cov_cw = (vw + kCovCellPx - 1) / kCovCellPx;
    const int32_t cov_ch = (vh + kCovCellPx - 1) / kCovCellPx;
    std::vector<uint8_t> cov(size_t(cov_cw) * size_t(cov_ch), 0);

    for (int32_t sy = min_sy; sy <= max_sy; ++sy)
    for (int32_t sx = min_sx; sx <= max_sx; ++sx)
    {
        auto it = s.sectorTileBins.find(SectorBinKey(sx, sy));
        if (it == s.sectorTileBins.end()) continue;
        for (int32_t idx : it->second)
        {
            const auto& inst = s.sectorTileInst[idx];
            if (!s.sectorShowGizmos) {
                TObjectInstance* oi = inst.src.Get();
                if (oi && oi->IsLight()) continue;
            }
            const auto& tex = s.sectorTileTex[inst.tex_idx];
            S3DPoint sp; s.sectorProjectWorld(inst.world_pos, sp);
            const float anchor_scene = s.sectorCameraSceneZ(inst) - s.depth_mul * float(inst.regz);
            const float anchor_scene_norm = std::fabs(zspan) > 1e-6f ? (anchor_scene - s.z_near) / zspan : 0.5f;
            const int32_t dx = sp.x - inst.regx + cam_ox;
            const int32_t dy = sp.y - inst.regy + cam_oy;
            if (!tex.color.id || !tex.depth.id) { ++draw_invalid_img; continue; }
            const bool onscreen = !(dx + tex.w <= 0 || dy + tex.h <= 0 || dx >= vw || dy >= vh);
            if (!onscreen) ++draw_offscreen;
            else {
                int32_t cx0 = dx / kCovCellPx; if (cx0 < 0) cx0 = 0;
                int32_t cy0 = dy / kCovCellPx; if (cy0 < 0) cy0 = 0;
                int32_t cx1 = (dx + tex.w + kCovCellPx - 1) / kCovCellPx;
                int32_t cy1 = (dy + tex.h + kCovCellPx - 1) / kCovCellPx;
                if (cx1 > cov_cw) cx1 = cov_cw;
                if (cy1 > cov_ch) cy1 = cov_ch;
                for (int32_t cy = cy0; cy < cy1; ++cy)
                    for (int32_t cx = cx0; cx < cx1; ++cx)
                        cov[size_t(cy) * size_t(cov_cw) + size_t(cx)] = 1;
            }
            ++draw_submitted;
            Display->DrawTile(tex.color, tex.depth, dx, dy, tex.w, tex.h,
                              anchor_scene_norm, std::fabs(zspan) > 1e-6f ? s.depth_mul / zspan : 0.0f, 1.0f,
                              float(inst.world_pos.x), float(inst.world_pos.y), float(inst.world_pos.z),
                              float(inst.regx), float(inst.regy), s.depth_mul);
        }
    }
    if (!draw_stats_logged) {
        draw_stats_logged = true;
        int32_t cov_hit = 0;
        for (uint8_t b : cov) cov_hit += b;
        const int32_t cov_total = int32_t(cov.size());
        log_info("[sector] draw stats: total_inst=%zu submitted=%d invalid_img=%d offscreen_submitted=%d vw=%d vh=%d cov_cells=%d/%d (%.1f%%)",
            s.sectorTileInst.size(), draw_submitted, draw_invalid_img, draw_offscreen, vw, vh, cov_hit, cov_total,
            cov_total > 0 ? 100.0 * cov_hit / cov_total : 0.0);
    }
    Display->EndTilePass();
    Display->RunLightingPass();
}

void TMapRenderer::HandleMouseClick(int32_t button, int32_t x, int32_t y)
{
    Impl& s = *impl;
    if (button == MB_LEFTDOWN)
    {
        const bool ctrl_pan = CtrlDown;
        int32_t picked = -1;
        if (!ctrl_pan)
        {
            int32_t cam_ox = 0, cam_oy = 0;
            s.sectorCameraOriginScreen(cam_ox, cam_oy);
            float best_d2 = 24.0f * 24.0f;
            for (size_t i = 0; i < s.sectorLights.size(); ++i)
            {
                const SSectorLight& L = s.sectorLights[i];
                if (!L.enabled) continue;
                S3DPoint sp; s.sectorProjectWorld(s.sectorLightPos(L), sp);
                const float dx = float((sp.x + cam_ox) - x);
                const float dy = float((sp.y + cam_oy) - y);
                const float d2 = dx*dx + dy*dy;
                if (d2 < best_d2) { best_d2 = d2; picked = int32_t(i); }
            }
        }
        if (picked >= 0)
        {
            if (TObjectInstance* pi = s.sectorLights[picked].ref.Get())
            {
                s.lightDragIdx = picked;
                s.lightDragStartSX = x;
                s.lightDragStartSY = y;
                s.lightDragStartOiPos = pi->Pos();
            }
            else
            {
                s.sectorDragging = true;
                s.dragStartX = x; s.dragStartY = y; s.dragCameraStartWorld = s.sectorCameraWorld;
            }
        }
        else
        {
            s.sectorDragging = true;
            s.dragStartX = x; s.dragStartY = y; s.dragCameraStartWorld = s.sectorCameraWorld;
        }
    }
    else if (button == MB_LEFTUP)
    {
        s.sectorDragging = false;
        s.lightDragIdx = -1;
    }
}

void TMapRenderer::HandleMouseMove(int32_t button, int32_t x, int32_t y)
{
    (void)button;
    Impl& s = *impl;
    if (s.lightDragIdx >= 0 && s.lightDragIdx < int32_t(s.sectorLights.size()))
    {
        SSectorLight& L = s.sectorLights[s.lightDragIdx];
        TObjectInstance* oi = L.ref.Get();
        if (!oi) { s.lightDragIdx = -1; return; }
        S3DPoint newpos = s.lightDragStartOiPos;
        if (ShiftDown)
        {
            const int32_t dy = y - s.lightDragStartSY;
            newpos.z = s.lightDragStartOiPos.z - dy;
        }
        else
        {
            int32_t cam_ox = 0, cam_oy = 0;
            s.sectorCameraOriginScreen(cam_ox, cam_oy);
            S3DPoint wp_rel;
            ScreenToWorld(x - cam_ox, y - cam_oy, wp_rel, int32_t(s.lightDragStartOiPos.z));
            newpos.x = wp_rel.x + s.sectorCameraWorld.x;
            newpos.y = wp_rel.y + s.sectorCameraWorld.y;
            newpos.z = s.lightDragStartOiPos.z;
        }
        oi->SetPos(newpos, -1, true);
        const S3DPoint now = oi->Pos();
        for (auto& ti : s.sectorTileInst)
            if (ti.src.Get() == oi) ti.world_pos = now;
        s.rebuildBins();
        return;
    }
    if (!s.sectorDragging) return;
    S3DPoint drag_delta_w = {0,0,0};
    ScreenToWorld(x - s.dragStartX, y - s.dragStartY, drag_delta_w, 0);
    s.sectorCameraWorld.x = s.dragCameraStartWorld.x - drag_delta_w.x;
    s.sectorCameraWorld.y = s.dragCameraStartWorld.y - drag_delta_w.y;
    s.sectorCameraWorld.z = s.dragCameraStartWorld.z;
}
