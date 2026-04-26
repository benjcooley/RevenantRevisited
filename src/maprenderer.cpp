// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 maprenderer.cpp - Map renderer object                 *
// *************************************************************************

#include "maprenderer.h"
#include "maprenderer_internal.h"

#include "animimage.h"
#include "animimagebody.h"
#include "3dimage.h"
#include "bitmap.h"
#include "bitmapdata.h"
#include "chunkcache.h"
#include "decompdata.h"
#include "display.h"
#include "renderer.h"
#include "debugui.h"
#include "imgui.h"
#include "imagery.h"
#include "imageres.h"
#include "logging.h"
#include "math3d.h"
#include "meshextract.h"
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

static bool IsTerrainGridTileName(const char* name)
{
    if (!name || !*name)
        return false;
    return strnicmp(name, "For", 3) == 0 ||
           strnicmp(name, "Beach", 5) == 0 ||
           strnicmp(name, "Sand", 4) == 0 ||
           strnicmp(name, "Gr", 2) == 0;
}

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

static void MatrixIdentity16(float out[16])
{
    for (int32_t i = 0; i < 16; ++i)
        out[i] = (i % 5 == 0) ? 1.0f : 0.0f;
}

static void MatrixMul16(const float a[16], const float b[16], float out[16])
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

static void MatrixScale16(float sx, float sy, float sz, float out[16])
{
    MatrixIdentity16(out);
    out[0] = sx;
    out[5] = sy;
    out[10] = sz;
}

static void BuildRootMatrixSource(const TObjectInstance* oi, hmm_mat4* out)
{
    MtxClear(out);
    if (!oi) return;

    const float kTurn = (float)(M_PI * 2.0 / 256.0);
    MtxRotateZ(out, oi->GetFace() * kTurn);
    hmm_mat4 temp = {};
    MtxClear(&temp); MtxRotateX(&temp, oi->GetRotateX() * kTurn); MtxMultiply(out, out, &temp);
    MtxClear(&temp); MtxRotateY(&temp, oi->GetRotateY() * kTurn); MtxMultiply(out, out, &temp);

    hmm_vec3 pos = {
        (float)oi->Pos().x,
        (float)oi->Pos().y,
        (float)FIX_Z_VALUE(oi->Pos().z)
    };
    MtxClear(&temp); MtxTranslate(&temp, &pos); MtxMultiply(out, out, &temp);
}

static void TransposeSourceToRenderer(const hmm_mat4& src, float out[16])
{
    for (int32_t r = 0; r < 4; ++r)
        for (int32_t c = 0; c < 4; ++c)
            out[r * 4 + c] = src.Elements[c][r];
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
                             bool* out_has_alpha = nullptr,
                             uint32_t* out_bm_flags = nullptr,
                             std::vector<float>* out_cpu_depth_local = nullptr,
                             std::vector<uint8_t>* out_cpu_opaque = nullptr)
{
    if (!bm || bm->width <= 0 || bm->height <= 0) return false;
    const bool is_8bit = (bm->flags & BM_8BIT) != 0;
    const bool is_16bit = (bm->flags & (BM_15BIT | BM_16BIT)) != 0;
    const bool has_zbuffer = (bm->flags & BM_ZBUFFER) != 0;
    const bool has_alpha = (bm->flags & BM_ALPHA) != 0;
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
    std::unique_ptr<uint8_t[]> alphaplane;
    if (has_alpha) alphaplane.reset(new uint8_t[npx]);
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
        if (has_alpha) std::fill_n(alphaplane.get(), npx, uint8_t(31));
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
        uint8_t* abuf = has_alpha ? (uint8_t*)bm->alpha.ptr() : nullptr;
        if (is_8bit) std::memcpy(idxplane.get(), bm->data8, npx);
        else std::memcpy(rgbplane16.get(), bm->data16, npx * sizeof(uint16_t));
        if (has_zbuffer && zbuf) std::memcpy(zplane.get(), zbuf, npx * sizeof(uint16_t));
        if (has_alpha && abuf) std::memcpy(alphaplane.get(), abuf, npx);
    }
    std::unique_ptr<uint8_t[]> rgba(new uint8_t[npx * 4]);
    std::unique_ptr<float[]> dflt(new float[npx]);
    float z_local_min = FLT_MAX, z_local_max = -FLT_MAX;
    SSectorTileTex::SDepthDump z_dump = {};
    bool any_alpha = false;
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
        uint8_t alpha_u8 = 255;
        if (has_alpha && alphaplane)
        {
            const uint8_t a31 = alphaplane[i] > 31 ? 31 : alphaplane[i];
            alpha_u8 = uint8_t((uint32_t(a31) * 255u) / 31u);
            any_alpha |= (a31 < 31);
        }
        rgba[i*4+3] = alpha_u8;
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
    if (out_has_alpha) *out_has_alpha = any_alpha;
    if (out_bm_flags) *out_bm_flags = bm->flags;
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

void SSectorDrawableInst::UpdateFromInstance()
{
    TObjectInstance* oi = src.Get();
    if (!oi) return;
    world_pos = oi->Pos();
    state = oi->GetState();
    frame = oi->GetFrame();
    if (kind == ESectorDrawableKind::Tile)
    {
        if (TObjectImagery* img = oi->GetImagery())
        {
            regx = img->GetRegX(state);
            regy = img->GetRegY(state);
            regz = img->GetRegZ(state);
        }
    }
}

void SSectorDrawableInst::AccumulateSceneZ(const SMapRenderContext& ctx, float& scene_z_min_fit,
                                           float& scene_z_max_fit, int32_t& fit_tiles) const
{
    S3DPoint rel = world_pos - ctx.sectorCameraWorld;
    if (kind == ESectorDrawableKind::Mesh)
    {
        const S3DPoint mesh_world = MapRendererMeshWorld(world_pos, ctx.mesh_scale_x, ctx.mesh_scale_y, ctx.mesh_scale_z);
        const S3DPoint mesh_camera = MapRendererMeshWorld(ctx.sectorCameraWorld, ctx.mesh_scale_x, ctx.mesh_scale_y, ctx.mesh_scale_z);
        rel = mesh_world - mesh_camera;
    }
    const float camera_z = CameraDepth(rel);
    float z0 = camera_z - 512.0f;
    float z1 = camera_z + 512.0f;
    if (kind == ESectorDrawableKind::Tile && ctx.tile_assets)
    {
        const auto& tex = (*ctx.tile_assets)[asset_idx];
        S3DPoint sp;
        WorldToScreen(rel, sp.x, sp.y);
        const int32_t x0 = sp.x - regx + ctx.cam_ox;
        const int32_t y0 = sp.y - regy + ctx.cam_oy;
        const int32_t x1 = x0 + tex.w;
        const int32_t y1 = y0 + tex.h;
        if (x1 <= 0 || y1 <= 0 || x0 >= ctx.vw || y0 >= ctx.vh)
            return;
        z0 = has_authored_local_dz ? (camera_z + ctx.depth_mul * authored_local_dz_min)
                                   : (camera_z + ctx.depth_mul * (tex.z_local_min - float(regz)));
        z1 = has_authored_local_dz ? (camera_z + ctx.depth_mul * authored_local_dz_max)
                                   : (camera_z + ctx.depth_mul * (tex.z_local_max - float(regz)));
        ++fit_tiles;
    }
    scene_z_min_fit = (std::min)(scene_z_min_fit, z0);
    scene_z_max_fit = (std::max)(scene_z_max_fit, z1);
}

void SSectorDrawableInst::Submit(const SMapRenderContext& ctx, SMapRenderStats& stats) const
{
    TObjectInstance* oi = src.Get();
    if (!oi)
    {
        if (kind == ESectorDrawableKind::Mesh) ++stats.mesh_skipped;
        return;
    }

    if (kind == ESectorDrawableKind::Tile)
    {
        if (!ctx.show_tiles || !ctx.tile_assets) return;
        if (!ctx.show_gizmos && (oi->IsLight() || oi->ObjClass() == OBJCLASS_HELPER)) return;
        const auto& tex = (*ctx.tile_assets)[asset_idx];
        S3DPoint sp;
        const S3DPoint rel = {world_pos.x - ctx.sectorCameraWorld.x, world_pos.y - ctx.sectorCameraWorld.y, world_pos.z - ctx.sectorCameraWorld.z};
        WorldToScreen(rel, sp.x, sp.y);
        const float anchor_scene = MapRendererCameraDepth(rel) - ctx.depth_mul * float(regz);
        const float anchor_scene_norm = std::fabs(ctx.zspan) > 1e-6f ? (anchor_scene - ctx.z_near) / ctx.zspan : 0.5f;
        const int32_t dx = sp.x - regx + ctx.cam_ox;
        const int32_t dy = sp.y - regy + ctx.cam_oy;
        if (!tex.color.id || !tex.depth.id) { ++stats.draw_invalid_img; return; }
        if (debug_sector_level == 0 && debug_sector_x == 7 && debug_sector_y == 22 &&
            (debug_sector_slot == 34 || debug_sector_slot == 36 ||
             debug_sector_slot == 37 || debug_sector_slot == 44 ||
             debug_sector_slot == 122))
        {
            static bool logged_slots[256] = {};
            const int32_t slot = debug_sector_slot;
            if ((uint32_t)slot < 256u && !logged_slots[slot])
            {
                logged_slots[slot] = true;
                const float depth_scale = std::fabs(ctx.zspan) > 1e-6f ? ctx.depth_mul / ctx.zspan : 0.0f;
                const float min_norm = anchor_scene_norm + tex.z_local_min * depth_scale;
                const float max_norm = anchor_scene_norm + tex.z_local_max * depth_scale;
                const float opaque_pct = tex.pixel_count > 0
                    ? (100.0f * float(tex.opaque_count) / float(tex.pixel_count))
                    : 0.0f;
                log_info("[sector] trace 7_22[%03d] '%s:%s' pos=(%d,%d,%d) local=(%d,%d) state=%d frame=%d asset=%d tex=%dx%d opaque=%d/%d(%.1f%%) reg=(%d,%d,%d) screen=(%d,%d)-(%d,%d) alpha=%d flags=0x%X zlocal=[%.0f..%.0f] znorm=[%.3f..%.3f] anchor=%.3f depth_scale=%.6f zraw_to_wu=%.3f samples=%d high=%d bbox=(%d,%d)-(%d,%d)",
                         slot,
                         oi->GetClassName() ? oi->GetClassName() : "?",
                         oi->GetTypeName() ? oi->GetTypeName() : "?",
                         world_pos.x, world_pos.y, world_pos.z,
                         world_pos.x - debug_sector_x * SECTORWIDTH,
                         world_pos.y - debug_sector_y * SECTORHEIGHT,
                         state, frame, asset_idx,
                         tex.w, tex.h, tex.opaque_count, tex.pixel_count, opaque_pct,
                         regx, regy, regz,
                         dx, dy, dx + tex.w, dy + tex.h,
                         tex.has_alpha ? 1 : 0, tex.bm_flags,
                         tex.z_local_min, tex.z_local_max,
                         min_norm, max_norm, anchor_scene_norm, depth_scale,
                         ctx.depth_mul,
                         tex.z_dump.sample_count, tex.z_dump.high_bit_count,
                         tex.z_dump.bbox_x0, tex.z_dump.bbox_y0,
                         tex.z_dump.bbox_x1, tex.z_dump.bbox_y1);
            }
        }
        const bool onscreen = !(dx + tex.w <= 0 || dy + tex.h <= 0 || dx >= ctx.vw || dy >= ctx.vh);
        if (!onscreen) ++stats.draw_offscreen;
        else if (ctx.cov)
        {
            int32_t cx0 = dx / ctx.cov_cell_px; if (cx0 < 0) cx0 = 0;
            int32_t cy0 = dy / ctx.cov_cell_px; if (cy0 < 0) cy0 = 0;
            int32_t cx1 = (dx + tex.w + ctx.cov_cell_px - 1) / ctx.cov_cell_px;
            int32_t cy1 = (dy + tex.h + ctx.cov_cell_px - 1) / ctx.cov_cell_px;
            if (cx1 > ctx.cov_cw) cx1 = ctx.cov_cw;
            if (cy1 > ctx.cov_ch) cy1 = ctx.cov_ch;
            for (int32_t cy = cy0; cy < cy1; ++cy)
                for (int32_t cx = cx0; cx < cx1; ++cx)
                    (*ctx.cov)[size_t(cy) * size_t(ctx.cov_cw) + size_t(cx)] = 1;
        }
        ++stats.draw_submitted;
        if (oi->IsLight())
        {
            SOverlaySubmit sub = {};
            sub.color_img = tex.color;
            sub.dst_x = dx;
            sub.dst_y = dy;
            sub.dst_w = tex.w;
            sub.dst_h = tex.h;
            Renderer->SubmitOverlay(sub);
            return;
        }
        STileSubmit sub = {};
        sub.color_img   = tex.color;
        sub.depth_img   = tex.depth;
        sub.dst_x       = dx;
        sub.dst_y       = dy;
        sub.dst_w       = tex.w;
        sub.dst_h       = tex.h;
        sub.anchor_z    = anchor_scene_norm;
        sub.depth_mul   = std::fabs(ctx.zspan) > 1e-6f ? ctx.depth_mul / ctx.zspan : 0.0f;
        sub.normal_mul  = 1.0f;
        sub.root_wx     = float(world_pos.x);
        sub.root_wy     = float(world_pos.y);
        sub.root_wz     = float(world_pos.z);
        sub.anchor_px_x = float(regx);
        sub.anchor_px_y = float(regy);
        sub.zraw_to_wu  = ctx.depth_mul;
        sub.sort_depth  = anchor_scene;
        if (tex.has_alpha)
            Renderer->SubmitTransparentTile(sub);
        else
            Renderer->SubmitTile(sub);
        return;
    }

    if (kind == ESectorDrawableKind::Mesh)
    {
        if (!ctx.show_meshes || !ctx.mesh_assets) return;
        if (!ctx.show_gizmos && (oi->IsLight() || oi->ObjClass() == OBJCLASS_HELPER)) return;
        if ((uint32_t)asset_idx >= (uint32_t)ctx.mesh_assets->size()) { ++stats.mesh_skipped; return; }
        TObjectImagery* img = oi->GetImagery();
        T3DImagery* meshimg = dynamic_cast<T3DImagery*>(img);
        if (!meshimg) { ++stats.mesh_skipped; return; }
        const SSectorMeshAsset& asset = (*ctx.mesh_assets)[asset_idx];
        if (state < 0 || state >= meshimg->NumStates() || meshimg->IsHidden(asset.objnum, state))
        {
            // Persistent (session-lifetime) diagnostic gate. SMapRenderStats
            // is rebuilt per frame; using its counter here means logging
            // every frame and choking the frame budget.
            static int s_char_mesh_skip_logged = 0;
            if (oi->IsCharacter() && s_char_mesh_skip_logged < 12)
            {
                log_info("[sector] char mesh skip %d '%s' type='%s' pos=(%d,%d,%d) state=%d frame=%d obj=%d tex=%d numstates=%d hidden=%d",
                    s_char_mesh_skip_logged,
                    oi->GetClassName(), oi->GetTypeName(),
                    world_pos.x, world_pos.y, world_pos.z,
                    state, frame, asset.objnum, asset.texslot, meshimg->NumStates(),
                    (state >= 0 && state < meshimg->NumStates()) ? (meshimg->IsHidden(asset.objnum, state) ? 1 : 0) : -1);
                ++s_char_mesh_skip_logged;
            }
            ++stats.mesh_skipped;
            return;
        }
        bool pose_key_ok = false;
        float local_renderer[16];
        if (!ctx.force_mesh_preview_pose)
        {
            const SAnimPose pose = SampleI3DAnimPose(meshimg, state, frame);
            pose_key_ok = pose.Has(AnimTrack(uint16_t(asset.objnum), EAnimChannel::PosX));
            BuildAnimPoseObjectMatrix(meshimg, pose, state, asset.objnum, local_renderer);
        }
        else
        {
            BuildStaticObjectMatrix(meshimg, asset.objnum, 0, 0, local_renderer);
        }
        hmm_mat4 root_source = {};
        BuildRootMatrixSource(oi, &root_source);
        float root_renderer[16];
        TransposeSourceToRenderer(root_source, root_renderer);
        float world_renderer[16];
        MatrixMul16(root_renderer, local_renderer, world_renderer);
        if (ctx.mesh_scale_x != 1.0f || ctx.mesh_scale_y != 1.0f || ctx.mesh_scale_z != 1.0f)
        {
            float world_scale[16];
            float scaled_world[16];
            MatrixScale16(ctx.mesh_scale_x, ctx.mesh_scale_y, ctx.mesh_scale_z, world_scale);
            MatrixMul16(world_scale, world_renderer, scaled_world);
            std::memcpy(world_renderer, scaled_world, sizeof(world_renderer));
        }

        // Persistent (session-lifetime) gate. See note on s_char_mesh_skip_logged.
        static int s_char_mesh_logged = 0;
        if (oi->IsCharacter() && s_char_mesh_logged < 16)
        {
            S3DPoint sp;
            const S3DPoint mesh_world = MapRendererMeshWorld(world_pos, ctx.mesh_scale_x, ctx.mesh_scale_y, ctx.mesh_scale_z);
            const S3DPoint mesh_camera = MapRendererMeshWorld(ctx.sectorCameraWorld, ctx.mesh_scale_x, ctx.mesh_scale_y, ctx.mesh_scale_z);
            const S3DPoint rel = mesh_world - mesh_camera;
            WorldToScreen(rel, sp.x, sp.y);
            sp.z = int32_t(MapRendererCameraDepth(rel));
            float anim_t0 = 0.0f, anim_t1 = 0.0f, anim_t2 = 0.0f;
            bool key_ok = false;
            hmm_vec3 key_pos = {}, key_rot = {}, key_scl = {};
            if (auto* animator = dynamic_cast<T3DAnimator*>(oi->GetAnimator()))
            {
                hmm_mat4 anim_local = {};
                if (animator->GetObjectMatrix(asset.objnum, &anim_local))
                {
                    float anim_renderer[16];
                    TransposeSourceToRenderer(anim_local, anim_renderer);
                    anim_t0 = anim_renderer[3];
                    anim_t1 = anim_renderer[7];
                    anim_t2 = anim_renderer[11];
                }
            }
            key_ok = meshimg->GetUninterpolatedAniKey(asset.objnum, state, frame, key_pos, key_rot, key_scl);
            log_info("[sector] char mesh %d '%s' type='%s' pos=(%d,%d,%d) zfix=%.2f screen=(%d,%d,%d) state=%d frame=%d obj=%d tex=%d pose=%s root_t=(%.2f,%.2f,%.2f) local_t=(%.2f,%.2f,%.2f) world_t=(%.2f,%.2f,%.2f)",
                s_char_mesh_logged,
                oi->GetClassName(), oi->GetTypeName(),
                world_pos.x, world_pos.y, world_pos.z,
                FIX_Z_VALUE(world_pos.z),
                sp.x + ctx.cam_ox, sp.y + ctx.cam_oy, sp.z,
                state, frame, asset.objnum, asset.texslot,
                ctx.force_mesh_preview_pose ? "forced_0_0" : (pose_key_ok ? "live" : "fallback_0_0"),
                root_renderer[3], root_renderer[7], root_renderer[11],
                local_renderer[3], local_renderer[7], local_renderer[11],
                world_renderer[3], world_renderer[7], world_renderer[11]);
            log_info("[sector] char mesh animator %d '%s' type='%s' obj=%d anim_local_t=(%.2f,%.2f,%.2f)",
                s_char_mesh_logged,
                oi->GetClassName(), oi->GetTypeName(),
                asset.objnum, anim_t0, anim_t1, anim_t2);
            log_info("[sector] char mesh key %d '%s' type='%s' obj=%d ok=%d len=%d pos=(%.2f,%.2f,%.2f) rot=(%.2f,%.2f,%.2f) scl=(%.2f,%.2f,%.2f)",
                s_char_mesh_logged,
                oi->GetClassName(), oi->GetTypeName(),
                asset.objnum, key_ok ? 1 : 0, meshimg->GetAniLength(state),
                key_pos.X, key_pos.Y, key_pos.Z,
                key_rot.X, key_rot.Y, key_rot.Z,
                key_scl.X, key_scl.Y, key_scl.Z);
            ++s_char_mesh_logged;
        }

        // Persistent (session-lifetime) gate. See note on s_char_mesh_skip_logged.
        static int s_mesh_project_logged = 0;
        if (s_mesh_project_logged < 12)
        {
            S3DPoint sp;
            const S3DPoint mesh_world = MapRendererMeshWorld(world_pos, ctx.mesh_scale_x, ctx.mesh_scale_y, ctx.mesh_scale_z);
            const S3DPoint mesh_camera = MapRendererMeshWorld(ctx.sectorCameraWorld, ctx.mesh_scale_x, ctx.mesh_scale_y, ctx.mesh_scale_z);
            const S3DPoint rel = mesh_world - mesh_camera;
            WorldToScreen(rel, sp.x, sp.y);
            sp.z = int32_t(MapRendererCameraDepth(rel));
            log_info("[sector] mesh sample %d '%s' pos=(%d,%d,%d) screen=(%d,%d,%d) state=%d frame=%d asset(obj=%d tex=%d)",
                     s_mesh_project_logged, oi->GetClassName(),
                     world_pos.x, world_pos.y, world_pos.z,
                     sp.x + ctx.cam_ox, sp.y + ctx.cam_oy, sp.z,
                     state, frame, asset.objnum, asset.texslot);
            ++s_mesh_project_logged;
        }

        if (oi->ObjClass() == OBJCLASS_HELPER)
        {
            const S3DPoint mesh_world = MapRendererMeshWorld(world_pos, ctx.mesh_scale_x, ctx.mesh_scale_y, ctx.mesh_scale_z);
            const S3DPoint mesh_camera = MapRendererMeshWorld(ctx.sectorCameraWorld, ctx.mesh_scale_x, ctx.mesh_scale_y, ctx.mesh_scale_z);
            SHelperMeshSubmit m = {};
            m.mesh = asset.handle;
            std::memcpy(m.world, world_renderer, sizeof(m.world));
            m.shadow_plane = asset.helper_shadow_plane;
            std::memcpy(m.diffuse, asset.diffuse, sizeof(m.diffuse));
            std::memcpy(m.ambient, asset.ambient, sizeof(m.ambient));
            std::memcpy(m.specular, asset.specular, sizeof(m.specular));
            std::memcpy(m.emissive, asset.emissive, sizeof(m.emissive));
            m.power = asset.power;
            m.sort_depth = MapRendererCameraDepth(mesh_world - mesh_camera);
            Renderer->SubmitHelperMesh(m);
        }
        else
        {
            SMeshSubmit m = {};
            m.mesh = asset.handle;
            std::memcpy(m.world, world_renderer, sizeof(m.world));
            m.tint[0] = m.tint[1] = m.tint[2] = m.tint[3] = 1.0f;
            Renderer->SubmitMesh(m);
        }
        ++stats.mesh_submitted;
    }
}

TMapRenderer::TMapRenderer() : impl(std::make_unique<Impl>()) {}
TMapRenderer::~TMapRenderer() = default;

void TMapRenderer::GetCameraStatus(int32_t& level, int32_t& sector_x, int32_t& sector_y,
                                   int32_t& world_x, int32_t& world_y, int32_t& world_z) const
{
    if (!impl) {
        level = sector_x = sector_y = -1;
        world_x = world_y = world_z = 0;
        return;
    }
    const Impl& s = *impl;
    level   = s.cameraLevel;
    world_x = s.sectorCameraWorld.x;
    world_y = s.sectorCameraWorld.y;
    world_z = s.sectorCameraWorld.z;
    sector_x = MapRendererFloorDiv(world_x, SECTORWIDTH);
    sector_y = MapRendererFloorDiv(world_y, SECTORHEIGHT);
}

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
                   g_loadObjNullNonMapDrop, g_loadObjOk;
    g_loadObjNullObjVerNeg = g_loadObjNullClassNeg = g_loadObjNullBadClass =
    g_loadObjNullBadType = g_loadObjNullNewObjFail = g_loadObjNullCorruptDrop =
    g_loadObjNullNonMapDrop = g_loadObjOk = 0;

    struct SLoaded { int32_t lvl, sx, sy; TSector* sec; };
    std::vector<SLoaded> loaded;
    int32_t loaded_obj_total = 0;
    int32_t loaded_light_total = 0;
    int32_t loaded_anim_total = 0;
    std::vector<SSectorCoord> load_coords = FindLevelSectorCoords(keep_lvl);
    s.cameraLevel = keep_lvl;
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
    log_info("[sector] LoadObject buckets: ok=%d placeholder_objver=%d placeholder_class=%d bad_class=%d bad_type=%d new_obj_fail=%d corrupt_drop=%d nonmap_drop=%d",
        g_loadObjOk, g_loadObjNullObjVerNeg, g_loadObjNullClassNeg,
        g_loadObjNullBadClass, g_loadObjNullBadType, g_loadObjNullNewObjFail,
        g_loadObjNullCorruptDrop, g_loadObjNullNonMapDrop);

    struct SCharacterCensusSample
    {
        int32_t lvl = 0, sx = 0, sy = 0;
        int32_t objclass = -1;
        int32_t state = 0, frame = 0;
        S3DPoint pos = {};
        const char* cls = nullptr;
        const char* type = nullptr;
        const char* name = nullptr;
        int32_t imageryid = -1;
    };
    int32_t live_players = 0, live_characters = 0, live_character_meshes = 0, live_character_anims = 0;
    std::vector<SCharacterCensusSample> census_samples;
    census_samples.reserve(16);
    for (const auto& L : loaded)
    {
        TSector* sec = L.sec;
        for (int32_t i = 0; i < sec->NumItems(); ++i)
        {
            TObjectInstance* oi = sec->GetInstance(i);
            if (!oi || !oi->IsCharacter())
                continue;

            if (oi->ObjClass() == OBJCLASS_PLAYER) ++live_players;
            if (oi->ObjClass() == OBJCLASS_CHARACTER) ++live_characters;

            int32_t imageryid = -1;
            if (TObjectImagery* img = oi->GetImagery())
            {
                if (SImageryHeader* hdr = img->GetHeader())
                {
                    imageryid = hdr->imageryid;
                    if (hdr->imageryid == OBJIMAGE_MESH3D) ++live_character_meshes;
                    if (hdr->imageryid == OBJIMAGE_ANIMATION) ++live_character_anims;
                }
            }

            if ((int32_t)census_samples.size() < 16)
            {
                SCharacterCensusSample sample = {};
                sample.lvl = L.lvl;
                sample.sx = L.sx;
                sample.sy = L.sy;
                sample.objclass = oi->ObjClass();
                sample.state = oi->GetState();
                sample.frame = oi->GetFrame();
                sample.pos = oi->Pos();
                sample.cls = oi->GetClassName();
                sample.type = oi->GetTypeName();
                sample.name = oi->GetName();
                sample.imageryid = imageryid;
                census_samples.push_back(sample);
            }
        }
    }
    log_info("[sector] character census: live_players=%d live_characters=%d mesh=%d anim=%d sample_count=%zu",
        live_players, live_characters, live_character_meshes, live_character_anims, census_samples.size());
    for (size_t i = 0; i < census_samples.size(); ++i)
    {
        const auto& sample = census_samples[i];
        log_info("[sector] character sample %zu: sector=%d_%d_%d class=%d '%s' type='%s' name='%s' pos=(%d,%d,%d) state=%d frame=%d imagery=%d",
            i, sample.lvl, sample.sx, sample.sy, sample.objclass,
            sample.cls ? sample.cls : "(null)",
            sample.type ? sample.type : "(null)",
            sample.name ? sample.name : "(null)",
            sample.pos.x, sample.pos.y, sample.pos.z,
            sample.state, sample.frame, sample.imageryid);
    }

    std::vector<SSectorDrawableInst> draw_work;
    int32_t total_tiles = 0, non_2d = 0, bad_state = 0, no_still = 0, upload_fail = 0, no_img = 0, no_body = 0;
    int32_t total_mesh_objs = 0, mesh_upload_fail = 0, mesh_hidden = 0, mesh_slots_kept = 0;
    int32_t char_no_drawable_logged = 0;
    for (auto& L : loaded)
    {
        TSector* sec = L.sec;
        for (int32_t i = 0; i < sec->NumItems(); ++i)
        {
            TObjectInstance* oi = sec->GetInstance(i);
            if (!oi) continue;
            TObjectImagery* img = oi->GetImagery();
            if (!img) {
                ++no_img;
                if (oi->IsCharacter() && char_no_drawable_logged < 64)
                {
                    log_info("[sector] char no drawable '%s' name='%s' sector=%d_%d_%d reason=no_imagery state=%d frame=%d",
                        oi->GetTypeName(), oi->GetName(), L.lvl, L.sx, L.sy, oi->GetState(), oi->GetFrame());
                    ++char_no_drawable_logged;
                }
                continue;
            }
            SImageryHeader* hdr = img->GetHeader();
            SImageryBody* body = img->GetBody();
            if (!hdr || !body) {
                ++no_body;
                if (oi->IsCharacter() && char_no_drawable_logged < 64)
                {
                    log_info("[sector] char no drawable '%s' name='%s' sector=%d_%d_%d reason=no_body hdr=%p body=%p state=%d frame=%d",
                        oi->GetTypeName(), oi->GetName(), L.lvl, L.sx, L.sy, (void*)hdr, (void*)body, oi->GetState(), oi->GetFrame());
                    ++char_no_drawable_logged;
                }
                continue;
            }
            const int32_t st = oi->GetState();

            if (hdr->imageryid == OBJIMAGE_MESH3D)
            {
                ++total_mesh_objs;
                T3DImagery* meshimg = dynamic_cast<T3DImagery*>(img);
                if (!meshimg) continue;
                if (st < 0 || st >= hdr->numstates) {
                    ++bad_state;
                    if (oi->IsCharacter() && char_no_drawable_logged < 64)
                    {
                        log_info("[sector] char no drawable '%s' name='%s' sector=%d_%d_%d reason=bad_state state=%d frame=%d numstates=%d",
                            oi->GetTypeName(), oi->GetName(), L.lvl, L.sx, L.sy, st, oi->GetFrame(), hdr->numstates);
                        ++char_no_drawable_logged;
                    }
                    continue;
                }

                const int32_t kept_before = mesh_slots_kept;
                int32_t hidden_for_state = 0;
                int32_t extract_fail = 0;
                int32_t empty_texslots = 0;
                const int32_t texslots = meshimg->NumTextures() + 1;
                for (int32_t objnum = 0; objnum < meshimg->NumObjects(); ++objnum)
                {
                    if (meshimg->IsHidden(objnum, st)) { ++mesh_hidden; ++hidden_for_state; continue; }
                    bool obj_kept = false;
                    for (int32_t texslot = 0; texslot < texslots; ++texslot)
                    {
                        int32_t asset_idx = -1;
                        for (int32_t a = 0; a < (int32_t)s.sectorMeshAsset.size(); ++a)
                        {
                            const SSectorMeshAsset& asset = s.sectorMeshAsset[a];
                            if (asset.imagery_key == img && asset.objnum == objnum && asset.texslot == texslot)
                            {
                                asset_idx = a;
                                break;
                            }
                        }
                        if (asset_idx < 0)
                        {
                            std::vector<SMeshVertex> verts;
                            std::vector<uint16_t> indices;
                            if (!ExtractSubMeshTextureSlot(meshimg, objnum, texslot, verts, indices))
                            {
                                ++extract_fail;
                                if (texslot > 0)
                                    ++empty_texslots;
                                continue;
                            }

                            sg_image albedo = {};
                            if (texslot > 0)
                            {
                                S3DTex tex = {};
                                meshimg->GetTexture(texslot - 1, &tex);
                                albedo = tex.surface;
                            }
                            if (!albedo.id && oi->ObjClass() == OBJCLASS_HELPER)
                            {
                                S3DObj o = {};
                                meshimg->GetObject(objnum, &o);
                                if (o.material >= 0 && o.material < meshimg->NumMaterials())
                                {
                                    S3DMat mat = {};
                                    meshimg->GetMaterial(o.material, &mat);
                                    if (mat.texture >= 0 && mat.texture < meshimg->NumTextures())
                                    {
                                        S3DTex tex = {};
                                        meshimg->GetTexture(mat.texture, &tex);
                                        albedo = tex.surface;
                                    }
                                }
                            }
                            if (!albedo.id)
                            {
                                sg_image_desc idesc = {};
                                static uint32_t white = 0xFFFFFFFFu;
                                idesc.width = 1; idesc.height = 1;
                                idesc.pixel_format = SG_PIXELFORMAT_RGBA8;
                                idesc.data.subimage[0][0] = { &white, sizeof(white) };
                                idesc.label = "maprenderer.mesh.fallback";
                                albedo = sg_make_image(&idesc);
                            }

                            MeshHandle h = Renderer->RegisterMesh(
                                verts.data(), int32_t(verts.size()),
                                indices.data(), int32_t(indices.size()),
                                albedo);
                            if (!h) { ++mesh_upload_fail; continue; }

                            SSectorMeshAsset asset = {};
                            asset.imagery_key = img;
                            asset.objnum = objnum;
                            asset.texslot = texslot;
                            asset.handle = h;
                            if (oi->ObjClass() == OBJCLASS_HELPER)
                            {
                                asset.helper_material = true;
                                S3DObj o = {};
                                meshimg->GetObject(objnum, &o);
                                asset.helper_shadow_plane = (std::strncmp(o.name, "rectangle", 9) == 0);
                                if (o.material >= 0 && o.material < meshimg->NumMaterials())
                                {
                                    S3DMat mat = {};
                                    meshimg->GetMaterial(o.material, &mat);
                                    asset.diffuse[0] = mat.matdesc.diffuse.r;
                                    asset.diffuse[1] = mat.matdesc.diffuse.g;
                                    asset.diffuse[2] = mat.matdesc.diffuse.b;
                                    asset.diffuse[3] = mat.matdesc.diffuse.a;
                                    asset.ambient[0] = mat.matdesc.ambient.r;
                                    asset.ambient[1] = mat.matdesc.ambient.g;
                                    asset.ambient[2] = mat.matdesc.ambient.b;
                                    asset.ambient[3] = mat.matdesc.ambient.a;
                                    asset.specular[0] = mat.matdesc.specular.r;
                                    asset.specular[1] = mat.matdesc.specular.g;
                                    asset.specular[2] = mat.matdesc.specular.b;
                                    asset.specular[3] = mat.matdesc.specular.a;
                                    asset.emissive[0] = mat.matdesc.emissive.r;
                                    asset.emissive[1] = mat.matdesc.emissive.g;
                                    asset.emissive[2] = mat.matdesc.emissive.b;
                                    asset.emissive[3] = mat.matdesc.emissive.a;
                                    asset.power = mat.matdesc.power;
                                    static int32_t s_helper_asset_log_count = 0;
                                    if (s_helper_asset_log_count < 24)
                                    {
                                        log_info("[sector] helper asset obj=%d name='%s' texslot=%d mat=%d mat_tex=%d htex=%u albedo_id=%u diff=(%.2f,%.2f,%.2f,%.2f)",
                                                 objnum, o.name, texslot, o.material, mat.texture,
                                                 (unsigned)mat.matdesc.hTexture, (unsigned)albedo.id,
                                                 asset.diffuse[0], asset.diffuse[1], asset.diffuse[2], asset.diffuse[3]);
                                        ++s_helper_asset_log_count;
                                    }
                                }
                            }
                            asset_idx = (int32_t)s.sectorMeshAsset.size();
                            s.sectorMeshAsset.push_back(asset);
                        }

                        if (asset_idx >= 0)
                        {
                            SSectorDrawableInst minst = {};
                            minst.kind = ESectorDrawableKind::Mesh;
                            minst.asset_idx = asset_idx;
                            minst.world_pos = oi->Pos();
                            minst.state = st;
                            minst.frame = oi->GetFrame();
                            minst.src = TSafeRef<>(oi);
                            draw_work.push_back(minst);
                            ++mesh_slots_kept;
                            obj_kept = true;
                        }
                    }
                    if (!obj_kept)
                    {
                        int32_t asset_idx = -1;
                        for (int32_t a = 0; a < (int32_t)s.sectorMeshAsset.size(); ++a)
                        {
                            const SSectorMeshAsset& asset = s.sectorMeshAsset[a];
                            if (asset.imagery_key == img && asset.objnum == objnum && asset.texslot == -1)
                            {
                                asset_idx = a;
                                break;
                            }
                        }
                        if (asset_idx < 0)
                        {
                            std::vector<SMeshVertex> verts;
                            std::vector<uint16_t> indices;
                            if (ExtractSubMesh(meshimg, objnum, verts, indices))
                            {
                                sg_image albedo = {};
                                if (meshimg->NumTextures() > 0)
                                {
                                    S3DTex tex = {};
                                    meshimg->GetTexture(0, &tex);
                                    albedo = tex.surface;
                                }
                                if (!albedo.id && oi->ObjClass() == OBJCLASS_HELPER)
                                {
                                    S3DObj o = {};
                                    meshimg->GetObject(objnum, &o);
                                    if (o.material >= 0 && o.material < meshimg->NumMaterials())
                                    {
                                        S3DMat mat = {};
                                        meshimg->GetMaterial(o.material, &mat);
                                        if (mat.texture >= 0 && mat.texture < meshimg->NumTextures())
                                        {
                                            S3DTex tex = {};
                                            meshimg->GetTexture(mat.texture, &tex);
                                            albedo = tex.surface;
                                        }
                                    }
                                }
                                if (!albedo.id)
                                {
                                    sg_image_desc idesc = {};
                                    static uint32_t white = 0xFFFFFFFFu;
                                    idesc.width = 1; idesc.height = 1;
                                    idesc.pixel_format = SG_PIXELFORMAT_RGBA8;
                                    idesc.data.subimage[0][0] = { &white, sizeof(white) };
                                    idesc.label = "maprenderer.mesh.whole_fallback";
                                    albedo = sg_make_image(&idesc);
                                }
                                MeshHandle h = Renderer->RegisterMesh(
                                    verts.data(), int32_t(verts.size()),
                                    indices.data(), int32_t(indices.size()),
                                    albedo);
                                if (h)
                                {
                                    SSectorMeshAsset asset = {};
                                    asset.imagery_key = img;
                                    asset.objnum = objnum;
                                    asset.texslot = -1;
                                    asset.handle = h;
                                    asset_idx = (int32_t)s.sectorMeshAsset.size();
                                    s.sectorMeshAsset.push_back(asset);
                                }
                                else
                                {
                                    ++mesh_upload_fail;
                                }
                            }
                        }
                        if (asset_idx >= 0)
                        {
                            SSectorDrawableInst minst = {};
                            minst.kind = ESectorDrawableKind::Mesh;
                            minst.asset_idx = asset_idx;
                            minst.world_pos = oi->Pos();
                            minst.state = st;
                            minst.frame = oi->GetFrame();
                            minst.src = TSafeRef<>(oi);
                            draw_work.push_back(minst);
                            ++mesh_slots_kept;
                        }
                    }
                }
                if (oi->IsCharacter() && mesh_slots_kept == kept_before && char_no_drawable_logged < 64)
                {
                    log_info("[sector] char no drawable '%s' name='%s' sector=%d_%d_%d reason=no_mesh_slots state=%d frame=%d numobjs=%d texslots=%d hidden_objs=%d extract_fail=%d empty_texslots=%d",
                        oi->GetTypeName(), oi->GetName(), L.lvl, L.sx, L.sy,
                        st, oi->GetFrame(), meshimg->NumObjects(), texslots, hidden_for_state, extract_fail, empty_texslots);
                    for (int32_t objdiag = 0; objdiag < meshimg->NumObjects() && objdiag < 4; ++objdiag)
                    {
                        log_info("[sector]   obj[%d] '%s' verts=%d faces=%d parent=%d hidden=%d",
                            objdiag,
                            meshimg->GetObjectName(objdiag),
                            meshimg->NumObjVerts(objdiag),
                            meshimg->NumObjFaces(objdiag),
                            meshimg->GetObjectParent(objdiag, st),
                            meshimg->IsHidden(objdiag, st) ? 1 : 0);
                    }
                    if (meshimg->NumObjects() > 0)
                    {
                        const int32_t nv0 = meshimg->NumObjVerts(0);
                        const int32_t nf0 = meshimg->NumObjFaces(0);
                        if (nv0 > 0 && nf0 > 0)
                        {
                            std::vector<S3DFace> faces0(nf0);
                            meshimg->GetObjFaces(0, faces0.data(), nullptr, nullptr);
                            int32_t invalid0 = 0;
                            for (const auto& f : faces0)
                            {
                                if ((int32_t)f.v1 >= nv0 || (int32_t)f.v2 >= nv0 || (int32_t)f.v3 >= nv0)
                                    ++invalid0;
                            }
                            const auto& f0 = faces0[0];
                            log_info("[sector]   obj[0] face sample nv=%d nf=%d invalid=%d first=(%u,%u,%u)",
                                nv0, nf0, invalid0, (unsigned)f0.v1, (unsigned)f0.v2, (unsigned)f0.v3);
                            std::vector<SMeshVertex> diag_verts;
                            std::vector<uint16_t> diag_indices;
                            const bool direct_ok = ExtractSubMesh(meshimg, 0, diag_verts, diag_indices);
                            diag_verts.clear();
                            diag_indices.clear();
                            const bool slot0_ok = ExtractSubMeshTextureSlot(meshimg, 0, 0, diag_verts, diag_indices);
                            log_info("[sector]   obj[0] extract sample numtex=%d direct_ok=%d slot0_ok=%d",
                                meshimg->NumTextures(), direct_ok ? 1 : 0, slot0_ok ? 1 : 0);
                        }
                    }
                    ++char_no_drawable_logged;
                }
                continue;
            }

            if (oi->IsCharacter() && char_no_drawable_logged < 64)
            {
                log_info("[sector] char no drawable '%s' name='%s' sector=%d_%d_%d reason=non_mesh_imagery imageryid=%d state=%d frame=%d",
                    oi->GetTypeName(), oi->GetName(), L.lvl, L.sx, L.sy, hdr->imageryid, st, oi->GetFrame());
                ++char_no_drawable_logged;
            }

            if (oi->ObjClass() != OBJCLASS_TILE) continue;
            ++total_tiles;
            if (hdr->imageryid != OBJIMAGE_ANIMATION) { ++non_2d; continue; }
            auto* ab = (SAnimImageryBody*)body;
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
                t.debug_typename = oi->GetTypeName();
                t.debug_world_pos = oi->Pos();
                t.debug_regx = img->GetRegX(st);
                t.debug_regy = img->GetRegY(st);
                t.debug_regz = img->GetRegZ(st);
                uint32_t bm_flags = 0;
                if (!UploadTileBitmap(bm, &t.color, &t.depth, &t.w, &t.h,
                                      &t.z_local_min, &t.z_local_max,
                                      &t.z_dump, &t.has_alpha, &bm_flags,
                                      &t.cpu_depth_local, &t.cpu_opaque))
                { ++upload_fail; continue; }
                t.pixel_count = t.w * t.h;
                for (uint8_t opaque : t.cpu_opaque)
                    if (opaque) ++t.opaque_count;
                static int32_t s_tile_alpha_log_count = 0;
                if (s_tile_alpha_log_count < 40 && (t.has_alpha || (bm_flags & BM_ALPHA)))
                {
                    log_info("[sector] alpha tile class='%s' type='%s' flags=0x%X has_alpha=%d size=%dx%d zbuf=%d",
                             t.debug_classname ? t.debug_classname : "?",
                             t.debug_typename ? t.debug_typename : "?",
                             bm_flags, t.has_alpha ? 1 : 0, t.w, t.h,
                             (bm_flags & BM_ZBUFFER) ? 1 : 0);
                    ++s_tile_alpha_log_count;
                }
                tex_idx = (int32_t)s.sectorTileTex.size();
                t.bm_flags = bm_flags;
                s.sectorTileTex.push_back(std::move(t));
            }
            SSectorDrawableInst winst = {};
            winst.kind = ESectorDrawableKind::Tile;
            winst.asset_idx = tex_idx;
            winst.world_pos = oi->Pos();
            winst.regx = img->GetRegX(st);
            winst.regy = img->GetRegY(st);
            winst.regz = img->GetRegZ(st);
            winst.src = TSafeRef<>(oi);
            winst.debug_sector_level = L.lvl;
            winst.debug_sector_x = L.sx;
            winst.debug_sector_y = L.sy;
            winst.debug_sector_slot = i;
            int32_t wwidth = 0, wlength = 0, wheight = 0;
            img->GetWorldBoundBox(st, wwidth, wlength, wheight);
            winst.wwidth = wwidth; winst.wlength = wlength; winst.wheight = wheight;
            winst.wregx = img->GetWorldRegX(st); winst.wregy = img->GetWorldRegY(st); winst.wregz = img->GetWorldRegZ(st);
            ComputeAuthoredLocalDepthRange(wwidth, wlength, wheight,
                                           winst.wregx, winst.wregy, winst.wregz,
                                           winst.has_authored_local_dz,
                                           winst.authored_local_dz_min,
                                           winst.authored_local_dz_max);
            draw_work.push_back(winst);
        }
    }
    log_info("[sector] tile scan: total=%d kept=%zu non_2d=%d drops: no_img=%d no_body=%d bad_state=%d no_still=%d upload_fail=%d",
        total_tiles, size_t(total_tiles - non_2d - no_still - upload_fail), non_2d, no_img, no_body, bad_state, no_still, upload_fail);
    log_info("[sector] mesh scan: total=%d kept=%zu hidden=%d upload_fail=%d cached_assets=%zu",
        total_mesh_objs, size_t(mesh_slots_kept), mesh_hidden, mesh_upload_fail, s.sectorMeshAsset.size());
    struct STerrainGridCell {
        bool present = false;
        const char* type = nullptr;
        int32_t slot = -1;
        int32_t z = 0;
    };
    struct STerrainGridSector {
        STerrainGridCell cells[8][8];
        int32_t terrain_count = 0;
    };
    std::unordered_map<int64_t, STerrainGridSector> terrain_grids;
    for (const auto& L : loaded)
    {
        TSector* sec = L.sec;
        if (!sec) continue;
        STerrainGridSector& grid = terrain_grids[SectorBinKey(L.sx, L.sy)];
        for (int32_t i = 0; i < sec->NumItems(); ++i)
        {
            TObjectInstance* oi = sec->GetInstance(i);
            if (!oi || oi->ObjClass() != OBJCLASS_TILE)
                continue;
            const char* type = oi->GetTypeName();
            if (!IsTerrainGridTileName(type))
                continue;
            const S3DPoint p = oi->Pos();
            const int32_t lx = p.x - L.sx * SECTORWIDTH;
            const int32_t ly = p.y - L.sy * SECTORHEIGHT;
            const int32_t gx = (lx - 64 + 64) / 128;
            const int32_t gy = (ly - 64 + 64) / 128;
            if ((uint32_t)gx >= 8u || (uint32_t)gy >= 8u)
                continue;
            STerrainGridCell& cell = grid.cells[gy][gx];
            if (!cell.present)
            {
                cell.present = true;
                cell.type = type;
                cell.slot = i;
                cell.z = p.z;
                ++grid.terrain_count;
            }
        }
    }
    int32_t terrain_hole_logs = 0;
    for (const auto& entry : terrain_grids)
    {
        const int32_t sx = int32_t(entry.first >> 32);
        const int32_t sy = int32_t(uint32_t(entry.first));
        const STerrainGridSector& grid = entry.second;
        if (grid.terrain_count < 12)
            continue;
        for (int32_t gy = 0; gy < 8; ++gy)
        for (int32_t gx = 0; gx < 8; ++gx)
        {
            if (grid.cells[gy][gx].present)
                continue;
            int32_t neighbors = 0;
            const char* sample_type = nullptr;
            for (int32_t dy = -1; dy <= 1; ++dy)
            for (int32_t dx = -1; dx <= 1; ++dx)
            {
                if (dx == 0 && dy == 0)
                    continue;
                const int32_t nx = gx + dx;
                const int32_t ny = gy + dy;
                if ((uint32_t)nx >= 8u || (uint32_t)ny >= 8u)
                    continue;
                if (grid.cells[ny][nx].present)
                {
                    ++neighbors;
                    if (!sample_type)
                        sample_type = grid.cells[ny][nx].type;
                }
            }
            if (neighbors < 5)
                continue;
            log_info("[sector] terrain grid hole sector=%d_%d cell=(%d,%d) local=(%d,%d) world=(%d,%d) neighbors=%d near='%s'",
                     sx, sy, gx, gy, gx * 128 + 64, gy * 128 + 64,
                     sx * SECTORWIDTH + gx * 128 + 64,
                     sy * SECTORHEIGHT + gy * 128 + 64,
                     neighbors, sample_type ? sample_type : "?");
            if (++terrain_hole_logs >= 128)
                goto terrain_hole_done;
        }
    }
terrain_hole_done:
    log_info("[sector] terrain grid hole scan: sectors=%zu logged=%d",
             terrain_grids.size(), terrain_hole_logs);
    int32_t debug_slot_mismatch_logs = 0;
    for (const auto& inst : draw_work)
    {
        if (inst.debug_sector_slot < 0)
            continue;
        TSector* sec = nullptr;
        for (const auto& L : loaded)
        {
            if (L.lvl == inst.debug_sector_level &&
                L.sx == inst.debug_sector_x &&
                L.sy == inst.debug_sector_y)
            {
                sec = L.sec;
                break;
            }
        }
        if (!sec || inst.debug_sector_slot >= sec->NumItems())
            continue;
        TObjectInstance* oi = sec->GetInstance(inst.debug_sector_slot);
        if (!oi)
            continue;
        const S3DPoint p = oi->Pos();
        if (p.x == inst.world_pos.x && p.y == inst.world_pos.y && p.z == inst.world_pos.z)
            continue;
        log_warn("[sector] draw slot mismatch %d_%d_%d[%03d] raw='%s:%s' raw=(%d,%d,%d) draw=(%d,%d,%d) kind=%d",
                 inst.debug_sector_level, inst.debug_sector_x, inst.debug_sector_y, inst.debug_sector_slot,
                 oi->GetClassName() ? oi->GetClassName() : "?",
                 oi->GetTypeName() ? oi->GetTypeName() : "?",
                 p.x, p.y, p.z,
                 inst.world_pos.x, inst.world_pos.y, inst.world_pos.z,
                 int(inst.kind));
        if (++debug_slot_mismatch_logs >= 64)
            break;
    }
    log_info("[sector] draw slot mismatch scan: logged=%d", debug_slot_mismatch_logs);
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
    for (auto& w : draw_work)
    {
        if (w.kind != ESectorDrawableKind::Tile)
            continue;
        S3DPoint sp; WorldToScreen(w.world_pos, sp);
        const auto& tex = s.sectorTileTex[w.asset_idx];
        accumulate_bounds(all_bounds, w.world_pos, sp, w.regx, w.regy, tex.w, tex.h);
        const int32_t occ_sx = FloorDiv(w.world_pos.x, SECTORWIDTH);
        const int32_t occ_sy = FloorDiv(w.world_pos.y, SECTORHEIGHT);
        SOccupiedSectorStat& stat = occupied_sector_stats[SectorBinKey(occ_sx, occ_sy)];
        stat.sx = occ_sx;
        stat.sy = occ_sy;
        stat.count++;
        stat.wx_min = (std::min)(stat.wx_min, w.world_pos.x);
        stat.wx_max = (std::max)(stat.wx_max, w.world_pos.x);
        stat.wy_min = (std::min)(stat.wy_min, w.world_pos.y);
        stat.wy_max = (std::max)(stat.wy_max, w.world_pos.y);
        if (w.world_pos.x >= focus_wx0 && w.world_pos.x < focus_wx1 &&
            w.world_pos.y >= focus_wy0 && w.world_pos.y < focus_wy1)
            accumulate_bounds(focus_bounds, w.world_pos, sp, w.regx, w.regy, tex.w, tex.h);
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
    for (const auto& w : draw_work)
    {
        const float camera_z = (w.kind == ESectorDrawableKind::Mesh)
            ? MapRendererCameraDepth(s.sectorCameraRelMesh(w.world_pos))
            : CameraDepth(w.world_pos - s.sectorCameraWorld);
        float z0 = camera_z - 512.0f;
        float z1 = camera_z + 512.0f;
        if (w.kind == ESectorDrawableKind::Tile)
        {
            const auto& tex = s.sectorTileTex[w.asset_idx];
            z0 = w.has_authored_local_dz ? (camera_z + w.authored_local_dz_min)
                                         : (camera_z + (tex.z_local_min - float(w.regz)));
            z1 = w.has_authored_local_dz ? (camera_z + w.authored_local_dz_max)
                                         : (camera_z + (tex.z_local_max - float(w.regz)));
        }
        sz_min = (std::min)(sz_min, z0); sz_max = (std::max)(sz_max, z1);
    }
    s.sectorSceneZMin = sz_min; s.sectorSceneZMax = sz_max;
    s.sectorDrawInst = std::move(draw_work);
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
    log_info("[sector] built drawable list: %zu instances, %zu unique bitmaps",
        s.sectorDrawInst.size(), s.sectorTileTex.size());
    log_info("[sector] renderable summary: draw_instances=%zu unique_bitmaps=%zu mesh_assets=%zu point_lights=%zu",
        s.sectorDrawInst.size(), s.sectorTileTex.size(), s.sectorMeshAsset.size(), s.sectorLights.size());
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
    s.sectorDrawInst.clear();
    s.sectorDrawBins.clear();
    s.sectorMeshAsset.clear();
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
    if (!Display || !Display->BackBuffer() || s.sectorDrawInst.empty())
        return;

    const int64_t legacy_tick = TTime::LegacyFrameCount();
    if (legacy_tick != s.lastLegacyAnimTick)
    {
        const bool first_tick = (s.lastLegacyAnimTick < 0);
        s.lastLegacyAnimTick = legacy_tick;

        for (TSector* sec : s.sectorsKept)
        {
            if (!sec) continue;
            for (int32_t i = 0; i < sec->NumItems(); ++i)
            {
                TObjectInstance* oi = sec->GetInstance(i);
                if (!oi) continue;
                if (!first_tick && oi->IsAnimated())
                    oi->NextFrame();
            }
        }
        for (TSector* sec : s.sectorsKept)
        {
            if (!sec) continue;
            for (int32_t i = 0; i < sec->NumItems(); ++i)
            {
                TObjectInstance* oi = sec->GetInstance(i);
                if (!oi || !oi->IsAnimated()) continue;
                if (oi->NeedsAnimator() && !oi->HasAnimator())
                    oi->OnScreen();
                oi->Animate(false);
            }
        }
    }

    // Baseline per-frame drawable refresh: sync instance-backed state after
    // the world simulation has advanced. Drawable-specific enrichments can be
    // layered on top of this later.
    for (auto& inst : s.sectorDrawInst)
    {
        TObjectInstance* oi = inst.src.Get();
        if (!oi) continue;
        inst.world_pos = oi->Pos();
        inst.state = oi->GetState();
        inst.frame = oi->GetFrame();
        if (inst.kind == ESectorDrawableKind::Tile)
        {
            if (TObjectImagery* img = oi->GetImagery())
            {
                const int32_t st = oi->GetState();
                inst.regx = img->GetRegX(st);
                inst.regy = img->GetRegY(st);
                inst.regz = img->GetRegZ(st);
            }
        }
    }

    const int32_t vw = Display->Width();
    const int32_t vh = Display->Height();
    int32_t cam_ox = 0, cam_oy = 0;
    s.sectorCameraOriginScreen(cam_ox, cam_oy);

    float scene_z_min_fit = FLT_MAX, scene_z_max_fit = -FLT_MAX;
    int32_t fit_tiles = 0;
    for (const auto& inst : s.sectorDrawInst)
    {
        const float camera_z = (inst.kind == ESectorDrawableKind::Mesh)
            ? s.sectorCameraSceneZMesh(inst)
            : s.sectorCameraSceneZ(inst);
        float z0 = camera_z - 512.0f;
        float z1 = camera_z + 512.0f;
        if (inst.kind == ESectorDrawableKind::Tile)
        {
            const auto& tex = s.sectorTileTex[inst.asset_idx];
            S3DPoint sp; s.sectorProjectWorld(inst.world_pos, sp);
            const int32_t x0 = sp.x - inst.regx + cam_ox, y0 = sp.y - inst.regy + cam_oy;
            const int32_t x1 = x0 + tex.w, y1 = y0 + tex.h;
            if (x1 <= 0 || y1 <= 0 || x0 >= vw || y0 >= vh) continue;
            z0 = inst.has_authored_local_dz ? (camera_z + s.depth_mul * inst.authored_local_dz_min)
                                            : (camera_z + s.depth_mul * (tex.z_local_min - float(inst.regz)));
            z1 = inst.has_authored_local_dz ? (camera_z + s.depth_mul * inst.authored_local_dz_max)
                                            : (camera_z + s.depth_mul * (tex.z_local_max - float(inst.regz)));
            ++fit_tiles;
        }
        scene_z_min_fit = (std::min)(scene_z_min_fit, z0);
        scene_z_max_fit = (std::max)(scene_z_max_fit, z1);
    }
    if (!(scene_z_min_fit < scene_z_max_fit)) {
        scene_z_min_fit = s.sectorSceneZMin;
        scene_z_max_fit = s.sectorSceneZMax;
    }
    s.debugSceneZMinFit = scene_z_min_fit;
    s.debugSceneZMaxFit = scene_z_max_fit;
    s.debugFitTiles = fit_tiles;
    if (fit_tiles > 0)
    {
        constexpr float kFitZMargin = 512.0f;
        s.z_near = (std::min)(s.z_near, scene_z_min_fit - kFitZMargin);
        s.z_far  = (std::max)(s.z_far,  scene_z_max_fit + kFitZMargin);
    }

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

    Renderer->SetLight(s.light_dir[0], s.light_dir[1], s.light_dir[2], s.intensity, s.color[0], s.color[1], s.color[2], s.ambient);
    Renderer->SetAmbientColor(s.ambient_color[0], s.ambient_color[1], s.ambient_color[2]);
    Renderer->SetAmbientOcclusion(s.ao_enable, s.ao_radius_px, s.ao_strength, s.ao_bias, s.ao_max_dist);
    Renderer->SetShadowWorldDir(s.dir[0], s.dir[1], s.dir[2]);
    Renderer->SetShadowVariance(s.sdir_off_x, s.sdir_off_y, s.sdir_wz_mul);
    Renderer->SetNormalLightingHardness(s.normal_hardness);
    Renderer->SetNormalRadius(s.normal_radius);
    Renderer->SetEdgeThreshold(s.edge_thr);
    Renderer->SetTileViewMode(s.view_mode);
    Renderer->SetLightingMode(s.lighting_mode);
    Renderer->SetSunShadow(s.sun_shadow, s.sun_shadow_step, s.sun_shadow_soft, s.sun_shadow_max);
    Renderer->SetReconstructionParams(float(cam_ox), float(cam_oy), s.z_near, s.z_far,
                                      float(s.sectorCameraWorld.x), float(s.sectorCameraWorld.y),
                                      kCamForwardWU, 0.0f);

    Renderer->ClearPointLights();
    if (s.lights_on)
    {
        S3DPoint vc_rel;
        ScreenToWorld(vw / 2 - cam_ox, vh / 2 - cam_oy, vc_rel, 0);
        const S3DPoint vc_w = vc_rel + s.sectorCameraWorld;
        struct Pick { int32_t light_idx; float d2_to_view; };
        Pick picks[TRenderer::kMaxPointLights];
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
            if (pick_n < TRenderer::kMaxPointLights) picks[pick_n++] = { i, d2 };
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
            Renderer->AddPointLight(float(wp.x), float(wp.y), float(wp.z),
                                    s.sectorLightRadius(L) * s.radius_mul,
                                    rgb[0], rgb[1], rgb[2],
                                    s.sectorLightIntensity(L) * s.intensity_mul);
        }
    }

    const float zspan = s.z_far - s.z_near;
    Renderer->BeginTilePass(0.12f, 0.16f, 0.10f, 1.0f);
    static bool draw_stats_logged = false;
    S3DPoint c0, c1, c2, c3;
    ScreenToWorld(-cam_ox - TRenderer::kGBufPad, -cam_oy - TRenderer::kGBufPad, c0, 0);
    ScreenToWorld(vw - cam_ox + TRenderer::kGBufPad, -cam_oy - TRenderer::kGBufPad, c1, 0);
    ScreenToWorld(-cam_ox - TRenderer::kGBufPad, vh - cam_oy + TRenderer::kGBufPad, c2, 0);
    ScreenToWorld(vw - cam_ox + TRenderer::kGBufPad, vh - cam_oy + TRenderer::kGBufPad, c3, 0);
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
    SMapRenderContext drawctx = {};
    drawctx.tile_assets = &s.sectorTileTex;
    drawctx.mesh_assets = &s.sectorMeshAsset;
    drawctx.sectorCameraWorld = s.sectorCameraWorld;
    drawctx.cam_ox = cam_ox;
    drawctx.cam_oy = cam_oy;
    drawctx.vw = vw;
    drawctx.vh = vh;
    drawctx.z_near = s.z_near;
    drawctx.z_far = s.z_far;
    drawctx.zspan = zspan;
    drawctx.depth_mul = s.depth_mul;
    drawctx.show_gizmos = s.sectorShowGizmos;
    drawctx.show_tiles = s.sectorShowTiles;
    drawctx.show_meshes = s.sectorShowMeshes;
    drawctx.force_mesh_preview_pose = s.sectorForceMeshPreviewPose;
    drawctx.mesh_scale_x = s.sectorMeshScaleX;
    drawctx.mesh_scale_y = s.sectorMeshScaleY;
    drawctx.mesh_scale_z = s.sectorMeshScaleZ;
    drawctx.cov_cell_px = kCovCellPx;
    drawctx.cov_cw = cov_cw;
    drawctx.cov_ch = cov_ch;
    drawctx.cov = &cov;
    SMapRenderStats stats = {};
    std::vector<uint8_t> bin_considered(s.sectorDrawInst.size(), 0);

    for (int32_t sy = min_sy; sy <= max_sy; ++sy)
    for (int32_t sx = min_sx; sx <= max_sx; ++sx)
    {
        auto it = s.sectorDrawBins.find(SectorBinKey(sx, sy));
        if (it == s.sectorDrawBins.end()) continue;
        for (int32_t idx : it->second)
        {
            if ((uint32_t)idx < (uint32_t)bin_considered.size())
                bin_considered[size_t(idx)] = 1;
            s.sectorDrawInst[idx].Submit(drawctx, stats);
        }
    }
    static bool bin_miss_logged = false;
    for (size_t idx = 0; idx < s.sectorDrawInst.size(); ++idx)
    {
        if (idx < bin_considered.size() && bin_considered[idx])
            continue;
        const auto& inst = s.sectorDrawInst[idx];
        if (inst.kind != ESectorDrawableKind::Tile)
            continue;
        if ((uint32_t)inst.asset_idx >= (uint32_t)s.sectorTileTex.size())
            continue;
        const auto& tex = s.sectorTileTex[inst.asset_idx];
        S3DPoint sp; s.sectorProjectWorld(inst.world_pos, sp);
        const int32_t x0 = sp.x - inst.regx + cam_ox;
        const int32_t y0 = sp.y - inst.regy + cam_oy;
        const int32_t x1 = x0 + tex.w;
        const int32_t y1 = y0 + tex.h;
        if (x1 <= 0 || y1 <= 0 || x0 >= vw || y0 >= vh)
            continue;
        ++stats.draw_bin_missed_visible;
        if (!bin_miss_logged && stats.draw_bin_missed_visible <= 24)
        {
            TObjectInstance* oi = inst.src.Get();
            log_warn("[sector] visible tile missed by bins idx=%zu class='%s' type='%s' sec=%d_%d pos=(%d,%d,%d) screen=(%d,%d)-(%d,%d)",
                     idx,
                     oi && oi->GetClassName() ? oi->GetClassName() : "?",
                     oi && oi->GetTypeName() ? oi->GetTypeName() : "?",
                     FloorDiv(inst.world_pos.x, SECTORWIDTH),
                     FloorDiv(inst.world_pos.y, SECTORHEIGHT),
                     inst.world_pos.x, inst.world_pos.y, inst.world_pos.z,
                     x0, y0, x1, y1);
        }
    }
    if (stats.draw_bin_missed_visible > 0)
        bin_miss_logged = true;
    if (!draw_stats_logged) {
        draw_stats_logged = true;
        int32_t cov_hit = 0;
        for (uint8_t b : cov) cov_hit += b;
        const int32_t cov_total = int32_t(cov.size());
        log_info("[sector] draw stats: drawables=%zu tile_submitted=%d invalid_img=%d offscreen_tiles=%d bin_missed_visible=%d mesh_submitted=%d mesh_skipped=%d vw=%d vh=%d cov_cells=%d/%d (%.1f%%)",
            s.sectorDrawInst.size(), stats.draw_submitted, stats.draw_invalid_img, stats.draw_offscreen,
            stats.draw_bin_missed_visible, stats.mesh_submitted, stats.mesh_skipped, vw, vh, cov_hit, cov_total,
            cov_total > 0 ? 100.0 * cov_hit / cov_total : 0.0);
    }
    Renderer->EndTilePass();
    Renderer->RunLightingPass();
}

void TMapRenderer::HandleMouseClick(int32_t button, int32_t x, int32_t y)
{
    Impl& s = *impl;
    if (button == MB_LEFTDOWN)
    {
        const bool ctrl_pan = CtrlDown;
        int32_t picked = -1;
        if (!ctrl_pan && s.sectorShowGizmos)
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
    if (!s.sectorShowGizmos && s.lightDragIdx >= 0)
    {
        s.lightDragIdx = -1;
        return;
    }
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
        for (auto& di : s.sectorDrawInst)
            if (di.src.Get() == oi) di.world_pos = now;
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
