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
#include "effect.h"
#include "renderer.h"
#include "debugui.h"
#include "imgui.h"
#include "imagery.h"
#include "imageres.h"
#include "gamemap.h"
#include "logging.h"
#include "mapmanager.h"
#include "mappane.h"
#include "math3d.h"
#include "meshextract.h"
#include "object.h"
#include "runtimemode.h"
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

inline float CameraDepth(const S3DPoint& rel, float cam_forward)
{
    return cam_forward
         - (float(rel.x + rel.y) * kIsoCos30 + float(rel.z) * 0.5f);
}

inline void ProjectCameraRelToScreen(const SMapRenderContext& ctx,
                                     const S3DPoint& rel,
                                     float& sx, float& sy)
{
    int32_t iso_x = 0, iso_y = 0;
    WorldToScreen(rel, iso_x, iso_y);
    sx = float(iso_x);
    sy = float(iso_y);
    if (!ctx.perspective_camera)
        return;

    const float z = (std::max)(CameraDepth(rel, ctx.cam_forward), 1.0f);
    const float focal_zoom = (std::max)(ctx.cam_forward * ctx.camera_zoom, 1.0f);
    sx *= focal_zoom / z;
    sy *= focal_zoom / z;
}

inline void ProjectWorldPointToScreen(const SMapRenderContext& ctx,
                                      const S3DPoint& world,
                                      float& sx, float& sy,
                                      float* scene_z_out = nullptr)
{
    // Match the mesh vertex shader convention: X/Y are camera-relative,
    // but Z remains absolute world height. Subtracting camera.z here makes
    // CPU overlays drift against GPU-projected 3D meshes in perspective mode.
    const S3DPoint rel = {
        world.x - ctx.sectorCameraWorld.x,
        world.y - ctx.sectorCameraWorld.y,
        world.z
    };
    ProjectCameraRelToScreen(ctx, rel, sx, sy);
    const float scene_z = CameraDepth(rel, ctx.cam_forward);
    if (scene_z_out)
        *scene_z_out = scene_z;
}

inline int32_t FloorDiv(int32_t v, int32_t d)
{
    return (v >= 0) ? (v / d) : -(((-v) + d - 1) / d);
}

inline int64_t SectorBinKey(int32_t sx, int32_t sy)
{
    return (int64_t(sx) << 32) ^ uint32_t(sy);
}

void SubmitParticleBillboards(const SMapRenderContext& ctx, SMapRenderStats& stats)
{
    TParticleManager& particles = ParticleManager();
    for (int32_t bucket_index = 0; bucket_index < particles.GlobalBucketCount(); ++bucket_index)
    {
        const TParticleBucket* bucket = particles.GlobalBucket(bucket_index);
        if (!bucket || !bucket->Active())
            continue;

        const SParticleBucketDesc& desc = bucket->Desc();
        const bool debug_solid = desc.debug_solid && ctx.debug_green_img.id;
        const sg_image image = debug_solid ? ctx.debug_green_img : desc.image;
        if (!image.id)
            continue;

        for (int32_t particle_index = 0; particle_index < bucket->Count(); ++particle_index)
        {
            const float* draw_pos = bucket->VarPtr(particle_index, EParticleVar::DrawPos);
            if (!draw_pos)
                continue;

            const S3DPoint world_pos = {
                int32_t(std::lround(draw_pos[0])),
                int32_t(std::lround(draw_pos[1])),
                int32_t(std::lround(draw_pos[2])),
            };
            float sx = 0.0f, sy = 0.0f;
            float scene_z = 0.0f;
            ProjectWorldPointToScreen(ctx, world_pos, sx, sy, &scene_z);

            float overlay_scale = 1.0f;
            if (ctx.perspective_camera)
            {
                const float z = (std::max)(scene_z, 1.0f);
                const float focal_zoom = (std::max)(ctx.cam_forward * ctx.camera_zoom, 1.0f);
                overlay_scale = focal_zoom / z;
            }

            float width = desc.default_width;
            float height = desc.default_height;
            if (const float* draw_scl = bucket->VarPtr(particle_index, EParticleVar::DrawScl))
            {
                width = draw_scl[0];
                height = draw_scl[1];
            }

            const int32_t w = (std::max)(1, int32_t(std::lround(width * overlay_scale)));
            const int32_t h = (std::max)(1, int32_t(std::lround(height * overlay_scale)));
            const int32_t dst_x = int32_t(std::lround(sx)) + ctx.cam_ox - w / 2;
            const int32_t dst_y = int32_t(std::lround(sy)) + ctx.cam_oy - h / 2;
            if (dst_x > ctx.vw || dst_y > ctx.vh || dst_x + w < 0 || dst_y + h < 0)
            {
                ++stats.draw_offscreen;
                continue;
            }

            float uv[4] = {0.0f, 0.0f, 1.0f, 1.0f};
            if (const float* draw_uv = bucket->VarPtr(particle_index, EParticleVar::DrawUvRect))
            {
                uv[0] = draw_uv[0];
                uv[1] = draw_uv[1];
                uv[2] = draw_uv[2];
                uv[3] = draw_uv[3];
            }

            SOverlaySubmit sub = {};
            sub.color_img = image;
            sub.dst_x = dst_x;
            sub.dst_y = dst_y;
            sub.dst_w = w;
            sub.dst_h = h;
            sub.src_x = debug_solid ? 0 : int32_t(uv[0] * float(desc.texture_width));
            sub.src_y = debug_solid ? 0 : int32_t((uv[1] + (desc.flip_v ? uv[3] : 0.0f)) * float(desc.texture_height));
            sub.src_w = debug_solid ? 1 : (std::max)(1, int32_t(uv[2] * float(desc.texture_width)));
            sub.src_h = debug_solid ? 1 : (desc.flip_v ? -1 : 1) * (std::max)(1, int32_t(uv[3] * float(desc.texture_height)));
            sub.src_tex_w = debug_solid ? 1 : desc.texture_width;
            sub.src_tex_h = debug_solid ? 1 : desc.texture_height;
            sub.additive_blend = !debug_solid && desc.blend == EParticleBlendMode::Additive;
            sub.chroma_key = !debug_solid && desc.chroma_key;
            sub.chroma_key_rgb[0] = desc.chroma_key_rgb[0];
            sub.chroma_key_rgb[1] = desc.chroma_key_rgb[1];
            sub.chroma_key_rgb[2] = desc.chroma_key_rgb[2];
            Renderer->SubmitOverlay(sub);
            ++stats.draw_submitted;
        }
    }
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

static bool TileCameraVolumeVisible(const SMapRenderContext& ctx,
                                    const S3DPoint& rel,
                                    int32_t regx, int32_t regy,
                                    int32_t w, int32_t h,
                                    float zraw_min, float zraw_max)
{
    int32_t sx_i = 0, sy_i = 0;
    WorldToScreen(rel, sx_i, sy_i);
    const float anchor_x = float(sx_i);
    const float anchor_y = float(sy_i);

    const float scale = ctx.perspective_camera ? (ctx.tile_scale > 0.0f ? ctx.tile_scale : 1.0f) : 1.0f;
    const float x0 = anchor_x - float(regx) * scale;
    const float x1 = anchor_x + (float(w) - float(regx)) * scale;
    const float y0 = anchor_y - float(regy) * scale;
    const float y1 = anchor_y + (float(h) - float(regy)) * scale;

    if (!ctx.perspective_camera)
    {
        const float vx0 = -float(ctx.cam_ox);
        const float vy0 = -float(ctx.cam_oy);
        const float vx1 = float(ctx.vw - ctx.cam_ox);
        const float vy1 = float(ctx.vh - ctx.cam_oy);
        return x1 >= vx0 && x0 <= vx1 && y1 >= vy0 && y0 <= vy1;
    }

    float z0 = CameraDepth(rel, ctx.cam_forward) + zraw_min * ctx.depth_mul;
    float z1 = CameraDepth(rel, ctx.cam_forward) + zraw_max * ctx.depth_mul;
    if (z1 < z0) std::swap(z0, z1);
    z0 = (std::max)(z0, 1.0f);
    z1 = (std::max)(z1, z0 + 1.0f);

    const float focal_zoom = (std::max)(ctx.cam_forward * (ctx.camera_zoom > 0.0f ? ctx.camera_zoom : 1.0f), 1.0f);
    const float left   = -float(ctx.cam_ox);
    const float right  =  float(ctx.vw - ctx.cam_ox);
    const float top    = -float(ctx.cam_oy);
    const float bottom =  float(ctx.vh - ctx.cam_oy);

    bool any_left = false, any_right = false, any_top = false, any_bottom = false;
    for (float z : { z0, z1 })
    for (float x : { x0, x1 })
    for (float y : { y0, y1 })
    {
        const float px = x * focal_zoom / z;
        const float py = y * focal_zoom / z;
        any_left   |= px >= left;
        any_right  |= px <= right;
        any_top    |= py >= top;
        any_bottom |= py <= bottom;
    }
    return any_left && any_right && any_top && any_bottom;
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

    // 3D-mesh object positions are in the common world space (pos.z
    // pre-scaled by WORLD3D_Z_SCALE at sector load), so translate by
    // raw pos.z -- no FIX_Z_VALUE / per-frame Z scaling. The 1.5 mesh
    // local scale that compensates for the dropped post-transform is
    // applied to the local matrix before this root multiplies it.
    hmm_vec3 pos = {
        (float)oi->Pos().x,
        (float)oi->Pos().y,
        (float)oi->Pos().z
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
        const bool transparent = has_zbuffer ? (z == 0 || z == 0x7F7F)
                                             : (is_8bit ? (idx8 == key8) : (px16 == key16));
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
    dd.min_filter = SG_FILTER_LINEAR;
    dd.mag_filter = SG_FILTER_LINEAR;
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

static int32_t CacheSectorBitmap(std::vector<SSectorTileTex>& cache,
                                 PTBitmap bm,
                                 TObjectInstance* oi,
                                 TObjectImagery* img,
                                 int32_t state)
{
    if (!bm) return -1;
    for (size_t t = 0; t < cache.size(); ++t)
        if (cache[t].bm_key == bm) return int32_t(t);

    SSectorTileTex tex = {};
    tex.bm_key = bm;
    tex.debug_classname = oi ? oi->GetClassName() : nullptr;
    tex.debug_typename = oi ? oi->GetTypeName() : nullptr;
    tex.debug_world_pos = oi ? oi->Pos() : S3DPoint{0,0,0};
    tex.debug_regx = img ? img->GetRegX(state) : 0;
    tex.debug_regy = img ? img->GetRegY(state) : 0;
    tex.debug_regz = img ? img->GetRegZ(state) : 0;
    uint32_t bm_flags = 0;
    if (!UploadTileBitmap(bm, &tex.color, &tex.depth, &tex.w, &tex.h,
                          &tex.z_local_min, &tex.z_local_max,
                          &tex.z_dump, &tex.has_alpha, &bm_flags,
                          &tex.cpu_depth_local, &tex.cpu_opaque))
        return -1;
    tex.pixel_count = tex.w * tex.h;
    for (uint8_t opaque : tex.cpu_opaque)
        if (opaque) ++tex.opaque_count;
    tex.bm_flags = bm_flags;
    const int32_t idx = int32_t(cache.size());
    cache.push_back(std::move(tex));
    return idx;
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
    // Camera-relative XY only. Z stays in absolute world space so the
    // object's projected screen Y depends solely on its own world Z,
    // not on where the camera happens to be.
    S3DPoint rel = { world_pos.x - ctx.sectorCameraWorld.x,
                     world_pos.y - ctx.sectorCameraWorld.y,
                     world_pos.z };
    if (kind == ESectorDrawableKind::Mesh)
    {
        const S3DPoint mesh_world  = MapRendererMeshWorld(world_pos,             ctx.mesh_scale_x, ctx.mesh_scale_y, ctx.mesh_scale_z);
        const S3DPoint mesh_camera = MapRendererMeshWorld(ctx.sectorCameraWorld, ctx.mesh_scale_x, ctx.mesh_scale_y, ctx.mesh_scale_z);
        rel = { mesh_world.x - mesh_camera.x, mesh_world.y - mesh_camera.y, mesh_world.z };
    }
    const float camera_z = CameraDepth(rel, ctx.cam_forward);
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
        z0 = has_authored_local_dz ? (camera_z + ctx.depth_mul * authored_local_dz_min)
                                   : (camera_z + ctx.depth_mul * (tex.z_local_min - float(regz)));
        z1 = has_authored_local_dz ? (camera_z + ctx.depth_mul * authored_local_dz_max)
                                   : (camera_z + ctx.depth_mul * (tex.z_local_max - float(regz)));
        if (!TileCameraVolumeVisible(ctx, rel, regx, regy, tex.w, tex.h,
                                     has_authored_local_dz ? authored_local_dz_min : (tex.z_local_min - float(regz)),
                                     has_authored_local_dz ? authored_local_dz_max : (tex.z_local_max - float(regz))))
            return;
        ++fit_tiles;
    }
    scene_z_min_fit = (std::min)(scene_z_min_fit, z0);
    scene_z_max_fit = (std::max)(scene_z_max_fit, z1);
}

void SSectorDrawableInst::Submit(const SMapRenderContext& ctx, SMapRenderStats& stats, uint32_t obj_id) const
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
        const S3DPoint rel = {world_pos.x - ctx.sectorCameraWorld.x, world_pos.y - ctx.sectorCameraWorld.y, world_pos.z};
        WorldToScreen(rel, sp.x, sp.y);
        const float anchor_scene = CameraDepth(rel, ctx.cam_forward) - ctx.depth_mul * float(regz);
        const float anchor_scene_norm = std::fabs(ctx.zspan) > 1e-6f ? (anchor_scene - ctx.z_near) / ctx.zspan : 0.5f;
        const int32_t dx = sp.x - regx + ctx.cam_ox;
        const int32_t dy = sp.y - regy + ctx.cam_oy;
        if (!tex.color.id || !tex.depth.id) { ++stats.draw_invalid_img; return; }
        const bool onscreen = TileCameraVolumeVisible(
            ctx, rel, regx, regy, tex.w, tex.h,
            has_authored_local_dz ? authored_local_dz_min : (tex.z_local_min - float(regz)),
            has_authored_local_dz ? authored_local_dz_max : (tex.z_local_max - float(regz)));
        if (!onscreen) {
            ++stats.draw_offscreen;
            return;       // <-- BUG FIX: was falling through and submitting
        }
        if (ctx.cov)
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
        if (oi->IsLight() || oi->ObjClass() == OBJCLASS_EFFECT)
        {
            float light_sx = float(sp.x);
            float light_sy = float(sp.y);
            ProjectCameraRelToScreen(ctx, rel, light_sx, light_sy);
            float overlay_scale = 1.0f;
            if (ctx.perspective_camera)
            {
                const float z = (std::max)(CameraDepth(rel, ctx.cam_forward), 1.0f);
                const float focal_zoom = (std::max)(ctx.cam_forward * ctx.camera_zoom, 1.0f);
                overlay_scale = focal_zoom / z;
            }
            SOverlaySubmit sub = {};
            sub.color_img = tex.color;
            sub.dst_x = int32_t(std::lround(light_sx - float(regx) * overlay_scale)) + ctx.cam_ox;
            sub.dst_y = int32_t(std::lround(light_sy - float(regy) * overlay_scale)) + ctx.cam_oy;
            sub.dst_w = (std::max)(1, int32_t(std::lround(float(tex.w) * overlay_scale)));
            sub.dst_h = (std::max)(1, int32_t(std::lround(float(tex.h) * overlay_scale)));
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
        sub.zraw_min    = tex.z_local_min;
        sub.zraw_max    = tex.z_local_max;
        sub.sort_depth  = anchor_scene;
        sub.obj_id      = obj_id;
        if (tex.has_alpha)
            Renderer->SubmitTransparentTile(sub);
        else
            Renderer->SubmitTile(sub);
        return;
    }

    if (kind == ESectorDrawableKind::Billboard)
    {
        if (oi->GetComponent<TParticleEffectComponent>())
            return;

        auto* flipbook = oi->GetComponent<TFlipbookBillboardComponent>();
        if (!flipbook && !ctx.debug_green_img.id) return;
        const bool debug_solid = flipbook && flipbook->DebugSolid() && ctx.debug_green_img.id;
        const sg_image image = debug_solid ? ctx.debug_green_img : flipbook->Image();
        if (!image.id) return;
        static bool logged_billboard_submit = false;
        if (!logged_billboard_submit)
        {
            logged_billboard_submit = true;
            log_info("[component-render] submit billboard inst=%p class='%s' type='%s' debug=%d image=%u green=%u",
                     (void*)oi, oi->GetClassName(), oi->GetTypeName(),
                     debug_solid ? 1 : 0, image.id, ctx.debug_green_img.id);
        }
        const S3DPoint billboard_world = MapRendererMeshWorld(world_pos, ctx.mesh_scale_x, ctx.mesh_scale_y, ctx.mesh_scale_z);
        const S3DPoint billboard_camera = MapRendererMeshWorld(ctx.sectorCameraWorld, ctx.mesh_scale_x, ctx.mesh_scale_y, ctx.mesh_scale_z);
        const S3DPoint rel = billboard_world - billboard_camera;
        float sx = 0.0f, sy = 0.0f;
        ProjectCameraRelToScreen(ctx, rel, sx, sy);
        float overlay_scale = 1.0f;
        if (ctx.perspective_camera)
        {
            const float z = (std::max)(CameraDepth(rel, ctx.cam_forward), 1.0f);
            const float focal_zoom = (std::max)(ctx.cam_forward * ctx.camera_zoom, 1.0f);
            overlay_scale = focal_zoom / z;
        }
        const float billboard_width = flipbook ? flipbook->Width() : 25.0f;
        const float billboard_height = flipbook ? flipbook->Height() : 62.5f;
        const int32_t w = (std::max)(1, int32_t(std::lround(billboard_width * overlay_scale)));
        const int32_t h = (std::max)(1, int32_t(std::lround(billboard_height * overlay_scale)));
        SOverlaySubmit sub = {};
        sub.color_img = image;
        sub.dst_x = int32_t(std::lround(sx)) + ctx.cam_ox - w / 2;
        sub.dst_y = int32_t(std::lround(sy)) + ctx.cam_oy - h / 2;
        sub.dst_w = w;
        sub.dst_h = h;
        sub.src_x = debug_solid ? 0 : flipbook->SourceX();
        sub.src_y = debug_solid ? 0 : flipbook->SourceY();
        sub.src_w = debug_solid ? 1 : flipbook->SourceWidth();
        sub.src_h = debug_solid ? 1 : flipbook->SourceHeight();
        sub.src_tex_w = debug_solid ? 1 : flipbook->TextureWidth();
        sub.src_tex_h = debug_solid ? 1 : flipbook->TextureHeight();
        sub.additive_blend = debug_solid ? false : flipbook->AdditiveBlend();
        Renderer->SubmitOverlay(sub);
        ++stats.draw_submitted;
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

        // Cheap viewport cull: project the mesh anchor to screen pixels
        // and skip if it's well outside the viewport. Margin of ~256 px
        // covers tall/wide meshes whose bounds extend past the anchor.
        {
            const S3DPoint mesh_world  = MapRendererMeshWorld(world_pos,             ctx.mesh_scale_x, ctx.mesh_scale_y, ctx.mesh_scale_z);
            const S3DPoint mesh_camera = MapRendererMeshWorld(ctx.sectorCameraWorld, ctx.mesh_scale_x, ctx.mesh_scale_y, ctx.mesh_scale_z);
            const S3DPoint rel = { mesh_world.x - mesh_camera.x, mesh_world.y - mesh_camera.y, mesh_world.z };
            S3DPoint sp; WorldToScreen(rel, sp.x, sp.y);
            const int32_t px = sp.x + ctx.cam_ox;
            const int32_t py = sp.y + ctx.cam_oy;
            constexpr int32_t kMargin = 256;
            if (px + kMargin <= 0 || py + kMargin <= 0 ||
                px - kMargin >= ctx.vw || py - kMargin >= ctx.vh)
            {
                ++stats.mesh_skipped;
                return;
            }
        }
        const SSectorMeshAsset& asset = (*ctx.mesh_assets)[asset_idx];
        if (state < 0 || state >= meshimg->NumStates() || meshimg->IsHidden(asset.objnum, state))
        {
            ++stats.mesh_skipped;
            return;
        }
        // C4: read the world-space mesh matrix straight off the bone's
        // TTransform, populated each frame by T3DAnimator::Animate ->
        // UpdateBoneTransforms -> RefreshHierarchy. The bone's
        // transform.Matrix() chains through (parent bones ->) inst's
        // transform_ which carries object world pos / rot + the
        // (1, 1, WORLD3D_Z_SCALE) Z stretch, so no separate root
        // compose or Z-scale post-multiply is needed.
        //
        // Fallback paths kept for cases that don't have a live
        // animator-with-bones to read from:
        //   * force_mesh_preview_pose (editor preview): bones aren't
        //     animator-driven; use BuildStaticObjectMatrix as before.
        //   * No animator on the instance: fall back to the legacy
        //     pose-sample compose for safety.
        bool pose_key_ok = false;
        float world_renderer[16];
        bool used_bone_xform = false;
        if (!ctx.force_mesh_preview_pose)
        {
            T3DAnimator* d3 = dynamic_cast<T3DAnimator*>(oi->GetAnimator());
            if (d3)
            {
                S3DAnimObj* bone = d3->GetObject(asset.objnum);
                if (bone)
                {
                    pose_key_ok = true;
                    const hmm_mat4& bone_world = bone->transform.Matrix();
                    TransposeSourceToRenderer(bone_world, world_renderer);
                    used_bone_xform = true;
                }
            }
        }
        if (!used_bone_xform)
        {
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
            // Mesh-local Z stretch -- only on the legacy compose path;
            // the bone-transform path already has it baked in via
            // inst.transform_'s SetLocalScl.
            {
                float local_scale[16];
                float scaled_local[16];
                MatrixScale16(1.0f, 1.0f, WORLD3D_Z_SCALE, local_scale);
                MatrixMul16(local_scale, local_renderer, scaled_local);
                std::memcpy(local_renderer, scaled_local, sizeof(local_renderer));
            }
            hmm_mat4 root_source = {};
            BuildRootMatrixSource(oi, &root_source);
            float root_renderer[16];
            TransposeSourceToRenderer(root_source, root_renderer);
            MatrixMul16(root_renderer, local_renderer, world_renderer);
        }
        if (ctx.mesh_scale_x != 1.0f || ctx.mesh_scale_y != 1.0f || ctx.mesh_scale_z != 1.0f)
        {
            float world_scale[16];
            float scaled_world[16];
            MatrixScale16(ctx.mesh_scale_x, ctx.mesh_scale_y, ctx.mesh_scale_z, world_scale);
            MatrixMul16(world_scale, world_renderer, scaled_world);
            std::memcpy(world_renderer, scaled_world, sizeof(world_renderer));
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
            m.sort_depth = CameraDepth({ mesh_world.x - mesh_camera.x, mesh_world.y - mesh_camera.y, mesh_world.z }, ctx.cam_forward);
            Renderer->SubmitHelperMesh(m);
        }
        else
        {
            SMeshSubmit m = {};
            m.mesh = asset.handle;
            std::memcpy(m.world, world_renderer, sizeof(m.world));
            m.tint[0] = m.tint[1] = m.tint[2] = m.tint[3] = 1.0f;
            m.obj_id = obj_id;
            Renderer->SubmitMesh(m);
        }
        ++stats.mesh_submitted;
    }
}

TMapRenderer::TMapRenderer() : impl(std::make_unique<Impl>()) {}
TMapRenderer::~TMapRenderer() = default;

float TMapRenderer::Impl::sectorCameraForward(int32_t viewport_h) const
{
    if (!sectorPerspectiveCamera || viewport_h <= 0)
        return kMapRendererCamForwardWU;
    float fov = sectorPerspectiveFovDeg;
    if (fov < 2.0f) fov = 2.0f;
    if (fov > 20.0f) fov = 20.0f;
    constexpr float kDegToRad = 3.14159265358979323846f / 180.0f;
    const float half_fov = fov * 0.5f * kDegToRad;
    const float t = std::tan(half_fov);
    return t > 1.0e-5f ? (float(viewport_h) * 0.5f) / t : kMapRendererCamForwardWU;
}

void TMapRenderer::Impl::sectorProjectWorldForViewport(const S3DPoint& world,
                                                       int32_t viewport_h,
                                                       S3DPoint& screen) const
{
    // Keep CPU-projected world markers in the same coordinate convention as
    // the GPU mesh shader: camera-relative X/Y, absolute world Z.
    const S3DPoint rel = { world.x - sectorCameraWorld.x,
                           world.y - sectorCameraWorld.y,
                           world.z };
    WorldToScreen(rel, screen.x, screen.y);
    const float scene_z = sectorCameraDepth(rel, viewport_h);
    screen.z = int32_t(scene_z);
    if (!sectorPerspectiveCamera)
        return;

    const float focal_zoom = (std::max)(sectorCameraForward(viewport_h) * sectorCameraZoom, 1.0f);
    const float z = (std::max)(scene_z, 1.0f);
    screen.x = int32_t(std::lround(float(screen.x) * focal_zoom / z));
    screen.y = int32_t(std::lround(float(screen.y) * focal_zoom / z));
}

void TMapRenderer::GetLoadedSectors(std::vector<TSector*>& out) const
{
    out.clear();
    if (!impl) return;
    TGameMap* m = impl->currentMap.Get();
    if (!m) return;
    const std::vector<TSector*>& src = m->Sectors();
    out.reserve(src.size());
    for (TSector* s : src)
        if (s) out.push_back(s);
}

int32_t TMapRenderer::CameraLevel() const
{
    return impl ? impl->cameraLevel : 0;
}

S3DPoint TMapRenderer::CameraWorld() const
{
    return impl ? impl->sectorCameraWorld : S3DPoint{0, 0, 0};
}

TSector* TMapRenderer::FindLoadedSector(int32_t level, int32_t sector_x, int32_t sector_y) const
{
    if (!impl) return nullptr;
    TGameMap* m = impl->currentMap.Get();
    if (!m || m->Level() != level) return nullptr;
    return m->FindSector(sector_x, sector_y);
}

void TMapRenderer::SyncContentsCache()
{
    if (!impl) return;
    Impl& s = *impl;

    // Sum of ContentVer() across loaded sectors. Cheap to compute, and
    // monotonic per-sector — any Add/Remove/Set in any sector advances
    // the sum, even if mutations cancel out object-count-wise. (See
    // memory/feedback_versions_over_flags.md on why versions over
    // dirty flags: a skipped read can never miss a click.)
    TGameMap* m = s.currentMap.Get();
    if (!m) return;
    const std::vector<TSector*>& kept = m->Sectors();

    int64_t cur_sum = 0;
    for (TSector* sec : kept)
        if (sec) cur_sum += sec->ContentVer();
    if (cur_sum == s.lastSyncedSectorVerSum) return;
    s.lastSyncedSectorVerSum = cur_sum;

    // V1: rebuild tile drawables only. Mesh/light entries stay stale
    // until full sector reload (a separate inch). Tile path covers
    // the editor's current paste/duplicate test cases.
    //
    // Strategy: drop existing tile entries from sectorDrawInst and
    // rebuild from the current map's sectors. Reuse the cached sectorTileTex (keyed
    // by bm pointer) — instances whose imagery isn't already cached
    // are skipped, with a one-shot warning log.
    std::vector<SSectorDrawableInst> rebuilt;
    rebuilt.reserve(s.sectorDrawInst.size());

    // Keep non-tile entries verbatim (mesh, etc.). They go stale on
    // mutation but at least don't disappear after a paste.
    int32_t kept_non_tile = 0;
    for (const auto& d : s.sectorDrawInst) {
        if (d.kind != ESectorDrawableKind::Tile) {
            rebuilt.push_back(d);
            ++kept_non_tile;
        }
    }

    int32_t tile_skipped_no_cache = 0;
    int32_t tile_rebuilt = 0;
    for (TSector* sec : kept) {
        if (!sec) continue;
        for (int32_t i = 0; i < sec->NumItems(); ++i) {
            TObjectInstance* oi = sec->GetInstance(i);
            if (!oi) continue;
            if (oi->ObjClass() != OBJCLASS_TILE) continue;

            TObjectImagery* img = oi->GetImagery();
            if (!img) continue;
            SImageryHeader* hdr = img->GetHeader();
            SImageryBody*   body = img->GetBody();
            if (!hdr || !body) continue;
            if (hdr->imageryid != OBJIMAGE_ANIMATION) continue;
            const int32_t st = oi->GetState();
            if (st < 0 || st >= hdr->numstates) continue;
            auto* ab = (SAnimImageryBody*)body;
            PTBitmap bm = (TBitmap*)ab->states[st].still;
            if (!bm) continue;

            // Look up cached texture. Skip if not present (caller
            // would need a full reload to register a new bitmap).
            int32_t tex_idx = -1;
            for (size_t t = 0; t < s.sectorTileTex.size(); ++t)
                if (s.sectorTileTex[t].bm_key == bm) { tex_idx = (int32_t)t; break; }
            if (tex_idx < 0) { ++tile_skipped_no_cache; continue; }

            SSectorDrawableInst winst = {};
            winst.kind = ESectorDrawableKind::Tile;
            winst.asset_idx = tex_idx;
            winst.world_pos = oi->Pos();
            winst.regx = img->GetRegX(st);
            winst.regy = img->GetRegY(st);
            winst.regz = img->GetRegZ(st);
            winst.src = TSafeRef<>(oi);
            winst.debug_sector_level = sec->SectorLevel();
            winst.debug_sector_x = sec->SectorX();
            winst.debug_sector_y = sec->SectorY();
            winst.debug_sector_slot = i;
            int32_t wwidth = 0, wlength = 0, wheight = 0;
            img->GetWorldBoundBox(st, wwidth, wlength, wheight);
            winst.wwidth = wwidth; winst.wlength = wlength; winst.wheight = wheight;
            winst.wregx = img->GetWorldRegX(st);
            winst.wregy = img->GetWorldRegY(st);
            winst.wregz = img->GetWorldRegZ(st);
            ComputeAuthoredLocalDepthRange(wwidth, wlength, wheight,
                                           winst.wregx, winst.wregy, winst.wregz,
                                           winst.has_authored_local_dz,
                                           winst.authored_local_dz_min,
                                           winst.authored_local_dz_max);
            rebuilt.push_back(winst);
            ++tile_rebuilt;
        }
    }

    s.sectorDrawInst = std::move(rebuilt);
    s.rebuildBins();

    log_info("[mr-sync] tile rebuild: kept_mesh=%d tile=%d skipped(no cache)=%d  ver_sum=%lld",
             kept_non_tile, tile_rebuilt, tile_skipped_no_cache, (long long)cur_sum);
}

void TMapRenderer::SetCameraWorld(int32_t level, int32_t world_x, int32_t world_y, int32_t world_z)
{
    if (!impl) return;
    Impl& s = *impl;
    s.cameraLevel        = level;
    s.sectorCameraWorld  = S3DPoint{ world_x, world_y, world_z };
}

void TMapRenderer::SetSunShadowEnabled(bool enable) { if (impl) impl->sun_shadow = enable; }
bool TMapRenderer::SunShadowEnabled() const         { return impl ? impl->sun_shadow : false; }

TMapRenderer::SDrawCounts TMapRenderer::GetLastDrawCounts() const
{
    SDrawCounts out;
    if (!impl) return out;
    out.total_drawables  = impl->last_draw_counts.total_drawables;
    out.tiles_submitted  = impl->last_draw_counts.tiles_submitted;
    out.meshes_submitted = impl->last_draw_counts.meshes_submitted;
    out.offscreen_culled = impl->last_draw_counts.offscreen_culled;
    return out;
}

TObjectInstance* TMapRenderer::InstanceFromDrawId(uint32_t obj_id) const
{
    if (!impl) return nullptr;
    const uint32_t raw = obj_id & kObjIdMask;
    if (raw == 0) return nullptr;
    const uint32_t idx = raw - 1u;
    if (idx >= impl->sectorDrawInst.size()) return nullptr;
    return impl->sectorDrawInst[idx].src.Get();
}

uint32_t TMapRenderer::DrawIdForInstance(TObjectInstance* oi) const
{
    if (!impl || !oi) return 0;
    const auto& v = impl->sectorDrawInst;
    for (size_t i = 0; i < v.size(); ++i)
        if (v[i].src.Get() == oi) return uint32_t(i) + 1u;
    return 0;
}

void TMapRenderer::SetSelectedMapIndices(const std::vector<int32_t>& indices)
{
    if (!impl) return;
    impl->selectedMapIndices.clear();
    impl->selectedMapIndices.reserve(indices.size());
    for (int32_t i : indices) if (i >= 0) impl->selectedMapIndices.insert(i);
}

bool TMapRenderer::IsMapIndexSelected(int32_t mapindex) const
{
    if (!impl || mapindex < 0) return false;
    return impl->selectedMapIndices.count(mapindex) != 0;
}

// ---------------------------------------------------------------------------
// Iso view + projection matrices that reproduce WorldToScreen exactly.
//
// Revenant's iso projection (object.cpp WorldToScreen):
//     screen.x =  wx -  wy
//     screen.y = (wx + wy)/2 - wz * 0.867       (image-space Y is +DOWN)
//
// In standard 3D terms this is an orthographic camera with:
//     world right  -> screen +X        (camera_right_world = (1,-1,0)/sqrt(2))
//     world up_iso -> screen -Y (up)   (camera_up_world    = (-0.5,-0.5,0.866)/1.118)
//     look (cam->scene) = -cross(right, up_iso)
//
// We solve the camera-position offset so HMM_LookAt produces those basis
// vectors when handed an up-hint of world +Z.
// ---------------------------------------------------------------------------
void TMapRenderer::GetViewProj(float view_out[16], float proj_out[16],
                               int32_t vp_w, int32_t vp_h) const
{
    if (!impl || vp_w <= 0 || vp_h <= 0) return;
    const Impl& s = *impl;

    // Camera offset direction such that HMM_LookAt with world-Z up gives
    // exactly the iso right / up basis. Magnitudes derived in the comment
    // above; D is arbitrary for an ortho camera, just pick something
    // bigger than the scene depth.
    // For HMM_LookAt(eye, target, up_z) to give the basis that matches
    // Revenant's iso (right_world=(1,-1,0)/√2, up_world=(-0.5,-0.5,0.866)/1.118),
    // forward must be (+0.5477, +0.5477, +0.6324). So eye is target -
    // forward*D = target + (-0.5477, -0.5477, -0.6324)*D. Earlier the
    // Z sign was flipped, which silently inverted the screen-Y axis --
    // overlays drew with world+Z going down instead of up.
    constexpr float kOffX  = -0.5477f;
    constexpr float kOffY  = -0.5477f;
    constexpr float kOffZ  = -0.6324f;
    constexpr float D      = 100000.0f;

    // GPU view target uses camera X/Y but a constant Z. The camera follows
    // Locke's full position (including Z) so other systems (lighting, fog,
    // etc.) can read his height; but the LookAt target's Z must not move
    // with him -- otherwise the entire scene scrolls vertically and the
    // user sees objects "rise" with the player on stairs.
    const hmm_vec3 target = HMM_Vec3(float(s.sectorCameraWorld.x),
                                     float(s.sectorCameraWorld.y),
                                     0.0f);
    const hmm_vec3 eye    = HMM_Vec3(target.X + kOffX * D,
                                     target.Y + kOffY * D,
                                     target.Z + kOffZ * D);
    const hmm_vec3 up_h   = HMM_Vec3(0.0f, 0.0f, 1.0f);

    const hmm_mat4 view = HMM_LookAt(eye, target, up_h);

    // After the orthonormal view, screen X = view_x * sqrt(2),
    //                              screen Y = view_y * 1/0.8944 (=1.1180).
    // To get NDC ∈ [-1,1] across the full pixel viewport:
    //   ortho.left/right = ±(vp_w/2) / sqrt(2)
    //   ortho.bot/top    = ±(vp_h/2) * 0.8944
    constexpr float kInvSqrt2  = 0.7071068f;
    constexpr float kIsoUpNorm = 0.8944272f;  // 1 / 1.118
    // Mirror the renderer's `zoom` so editor overlays (bbox / axis /
    // ImGuizmo) project to the same pixels as the rendered scene. The
    // shader path multiplies S/T by zoom; in matrix form that's a 1/zoom
    // shrink of the ortho half-extents.
    const float zoom = (s.sectorCameraZoom > 1e-4f) ? s.sectorCameraZoom : 1.0f;
    const float half_w = (float(vp_w) * 0.5f * kInvSqrt2) / zoom;
    const float half_h = (float(vp_h) * 0.5f * kIsoUpNorm) / zoom;
    const hmm_mat4 proj = HMM_Orthographic(-half_w, half_w,
                                           -half_h, half_h,
                                           -D * 2.0f, D * 2.0f);
    // TODO: when sectorPerspectiveCamera is on, build a true perspective
    // matrix so ImGuizmo lands on the same pixels. Ortho path covers the
    // common case for now.

    std::memcpy(view_out, &view, sizeof(view));
    std::memcpy(proj_out, &proj, sizeof(proj));
}

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

bool TMapRenderer::InitializeFromStartupArgs(std::function<void(int32_t, int32_t, int32_t)> post_load_hook)
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
    s.cameraLevel = keep_lvl;

    // Load + own the level via MapManager (game-side ownership, see
    // gamemap.h / mapmanager.h). The renderer just borrows.
    TGameMap* gmap = MapManager.SetCurrentLevel(keep_lvl);
    if (!gmap || gmap->Sectors().empty())
    {
        if (use_level_origin)
            log_error("[sector] level %d: MapManager returned no sectors; aborting startup", keep_lvl);
        return !use_level_origin;
    }
    s.currentMap = gmap;        // SafeRef captures (id, gen)

    // Build the local `loaded` list the rest of this function uses for
    // anchor / draw_work iteration. Sectors live on the map; we don't
    // own them.
    for (TSector* sec : gmap->Sectors())
    {
        if (!sec) continue;
        loaded_obj_total   += sec->NumItems();
        loaded_light_total += sec->NumObjSetItems(OBJSET_LIGHTS);
        loaded_anim_total  += sec->NumObjSetItems(OBJSET_ANIMATE);
        loaded.push_back({ sec->SectorLevel(), sec->SectorX(), sec->SectorY(), sec });
    }
    if (loaded.empty())
    {
        if (use_level_origin)
            log_error("[sector] level %d: MapManager loaded zero sectors; aborting startup", keep_lvl);
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
    for (size_t i = 0; i < census_samples.size(); ++i)
    {
        const auto& sample = census_samples[i];
    }

    // (Walkmap stamping happens in TGameMap::Load now.)

    // The map is loaded and the SafeRef points at it. Run the optional
    // post-load hook so callers can inject objects (player spawn,
    // scripted prelude, etc.) before the drawable scan below sees the
    // sector contents.
    if (post_load_hook)
        post_load_hook(keep_lvl, keep_sx, keep_sy);

    // Bind the renderer to the now-loaded TGameMap, passing the
    // parsed --sector anchor (or defaulting to level-origin if
    // --level was given alone). SetMap subscribes to the map's
    // events and triggers RebuildForCurrentMap, which (re)builds
    // the draw / camera / light caches against the live sectors.
    SetMap(gmap, use_level_origin, keep_sx, keep_sy);

    DebugUI::RegisterContributor(this);
    return true;
}

void TMapRenderer::SetMap(TGameMap* m, bool use_level_origin,
                          int32_t anchor_sx, int32_t anchor_sy)
{
    Impl& s = *impl;

    // Unsubscribe from the old map's events before flipping the ref.
    if (TGameMap* old = s.currentMap.Get(); old && s.mapListenerId)
    {
        old->RemoveListener(s.mapListenerId);
        s.mapListenerId = 0;
    }

    s.currentMap = m;

    if (m)
    {
        // Update camera-level + anchor state for the new map.
        s.cameraLevel           = m->Level();
        s.useInitialLevelOrigin = use_level_origin;
        s.initialAnchorSx       = anchor_sx;
        s.initialAnchorSy       = anchor_sy;

        // Subscribe -- Loaded / Updated trigger a full rebuild;
        // Unloaded clears the ref before the underlying sectors are
        // freed so subsequent draws don't dereference a dead map.
        s.mapListenerId = m->AddListener(
            [this](EGameMapEvent ev, TGameMap* /*map*/) {
                if (ev == EGameMapEvent::Unloaded)
                {
                    impl->currentMap.Clear();
                    impl->mapListenerId = 0;
                }
                RebuildForCurrentMap();
            });
    }

    RebuildForCurrentMap();
}

void TMapRenderer::RebuildForCurrentMap()
{
    Impl& s = *impl;

    // Clear all per-map caches first. Level-swap path destroys GPU
    // textures from the prior level; first-time build is a no-op
    // since the vectors are empty.
    for (auto& t : s.sectorTileTex)
    {
        if (t.color.id) sg_destroy_image(t.color);
        if (t.depth.id) sg_destroy_image(t.depth);
    }
    s.sectorTileTex.clear();
    s.sectorMeshAsset.clear();
    s.sectorDrawInst.clear();
    s.sectorDrawBins.clear();
    s.sectorLights.clear();
    s.lastSyncedSectorVerSum = -1;

    TGameMap* map = s.currentMap.Get();
    if (!map) return;

    // Pull anchor + level state from Impl (set by
    // InitializeFromStartupArgs at boot, reused on level swaps).
    const int32_t keep_lvl = s.cameraLevel;
    const int32_t keep_sx  = s.initialAnchorSx;
    const int32_t keep_sy  = s.initialAnchorSy;
    const bool    use_level_origin = s.useInitialLevelOrigin;

    // Build local `loaded` from the current map. The rest of this
    // function (lifted from the legacy InitializeFromStartupArgs body)
    // expects this vector for camera-anchor / draw_work iteration.
    struct SLoaded { int32_t lvl, sx, sy; TSector* sec; };
    std::vector<SLoaded> loaded;
    for (TSector* sec : map->Sectors())
    {
        if (!sec) continue;
        loaded.push_back({ sec->SectorLevel(), sec->SectorX(), sec->SectorY(), sec });
    }
    if (loaded.empty()) return;

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
                    ++char_no_drawable_logged;
                }
                continue;
            }
            const int32_t st = oi->GetState();
            if (T3DImagery* meshimg = dynamic_cast<T3DImagery*>(img))
                meshimg->AttachAnimatorComponents(oi);

            TFlipbookBillboardComponent* flipbook = oi->GetComponent<TFlipbookBillboardComponent>();
            if (flipbook)
            {
                static bool logged_billboard_build = false;
                if (!logged_billboard_build)
                {
                    logged_billboard_build = true;
                    log_info("[component-render] build billboard inst=%p class='%s' type='%s' debug=%d image=%u src=(%d,%d %dx%d) tex=%dx%d",
                             (void*)oi, oi->GetClassName(), oi->GetTypeName(),
                             flipbook->DebugSolid() ? 1 : 0,
                             flipbook->Image().id,
                             flipbook->SourceX(), flipbook->SourceY(),
                             flipbook->SourceWidth(), flipbook->SourceHeight(),
                             flipbook->TextureWidth(), flipbook->TextureHeight());
                }
                SSectorDrawableInst billboard = {};
                billboard.kind = ESectorDrawableKind::Billboard;
                billboard.world_pos = oi->Pos();
                billboard.state = st;
                billboard.frame = oi->GetFrame();
                billboard.src = TSafeRef<>(oi);
                billboard.debug_sector_level = L.lvl;
                billboard.debug_sector_x = L.sx;
                billboard.debug_sector_y = L.sy;
                billboard.debug_sector_slot = i;
                draw_work.push_back(billboard);
                if (flipbook->ReplacesDefaultVisual())
                    continue;
            }

            if (hdr->imageryid == OBJIMAGE_MESH3D)
            {
                ++total_mesh_objs;
                T3DImagery* meshimg = dynamic_cast<T3DImagery*>(img);
                if (!meshimg) continue;
                if (st < 0 || st >= hdr->numstates) {
                    ++bad_state;
                    if (oi->IsCharacter() && char_no_drawable_logged < 64)
                    {
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
                    for (int32_t objdiag = 0; objdiag < meshimg->NumObjects() && objdiag < 4; ++objdiag)
                    {
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
                            std::vector<SMeshVertex> diag_verts;
                            std::vector<uint16_t> diag_indices;
                            const bool direct_ok = ExtractSubMesh(meshimg, 0, diag_verts, diag_indices);
                            diag_verts.clear();
                            diag_indices.clear();
                            const bool slot0_ok = ExtractSubMeshTextureSlot(meshimg, 0, 0, diag_verts, diag_indices);
                        }
                    }
                    ++char_no_drawable_logged;
                }
                continue;
            }

            if (oi->IsCharacter() && char_no_drawable_logged < 64)
            {
                ++char_no_drawable_logged;
            }

            if (oi->ObjClass() == OBJCLASS_EFFECT && hdr->imageryid == OBJIMAGE_ANIMATION)
            {
                TAnimImagery* anim_img = dynamic_cast<TAnimImagery*>(img);
                PTAnimation anim = (anim_img && st >= 0 && st < anim_img->NumStates()) ? anim_img->GetAnimation(st) : nullptr;
                PTBitmap bm = anim ? anim->GetFrame(oi->GetFrame()) : nullptr;
                if (!bm) { ++no_still; continue; }

                const int32_t tex_idx = CacheSectorBitmap(s.sectorTileTex, bm, oi, img, st);
                if (tex_idx < 0) { ++upload_fail; continue; }

                SSectorDrawableInst einst = {};
                einst.kind = ESectorDrawableKind::Tile;
                einst.asset_idx = tex_idx;
                einst.world_pos = oi->Pos();
                // Legacy TAnimAnimator draws at WorldToScreen(pos) + animreg
                // with DM_USEREG, which subtracts the frame bitmap reg point.
                einst.regx = bm->regx - img->GetAnimRegX(st);
                einst.regy = bm->regy - img->GetAnimRegY(st);
                einst.regz = img->GetAnimRegZ(st);
                einst.src = TSafeRef<>(oi);
                einst.debug_sector_level = L.lvl;
                einst.debug_sector_x = L.sx;
                einst.debug_sector_y = L.sy;
                einst.debug_sector_slot = i;
                draw_work.push_back(einst);
                continue;
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
    const float init_camera_forward = s.sectorCameraForward(th);
    for (const auto& w : draw_work)
    {
        // Z stays absolute world (don't subtract camera.z) so depth ordering
        // doesn't shift when the camera moves vertically.
        const float camera_z = (w.kind == ESectorDrawableKind::Mesh)
            ? CameraDepth(s.sectorCameraRelMesh(w.world_pos), init_camera_forward)
            : CameraDepth(S3DPoint{ w.world_pos.x - s.sectorCameraWorld.x,
                                    w.world_pos.y - s.sectorCameraWorld.y,
                                    w.world_pos.z },
                          init_camera_forward);
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
    s.dlightTexIdx = -1;
    if (s.debugGreenImage.id) { sg_destroy_image(s.debugGreenImage); s.debugGreenImage = {}; }
    s.sectorDrawInst.clear();
    s.sectorDrawBins.clear();
    s.sectorMeshAsset.clear();
    // Sectors are owned by TGameMap (via TMapManager); the renderer
    // just borrowed via the SafeRef. Drop the ref without freeing.
    s.currentMap.Clear();
    s.sectorLights.clear();
    s.lightDragIdx = -1;
    s.sectorDragging = false;
    DebugUI::UnregisterContributor(this);
}

void TMapRenderer::RenderFrame()
{
    Impl& s = *impl;
    if (!Display || !Display->BackBuffer())
        return;
    // Reconcile per-instance drawable cache against current sector
    // contents BEFORE the empty-cache early-return; otherwise an
    // initial empty cache (or a paste from empty) never kicks the
    // rebuild even when sectors have content.
    SyncContentsCache();
    if (s.sectorDrawInst.empty())
        return;

    const int64_t legacy_tick = TTime::LegacyFrameCount();
    TGameMap* current_map = s.currentMap.Get();
    if (current_map && legacy_tick != s.lastLegacyAnimTick)
    {
        const bool first_tick = (s.lastLegacyAnimTick < 0);
        s.lastLegacyAnimTick = legacy_tick;
        const std::vector<TSector*>& kept = current_map->Sectors();

        for (TSector* sec : kept)
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
        for (TSector* sec : kept)
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
        if (oi->ObjClass() == OBJCLASS_EFFECT)
        {
            TObjectImagery* img = oi->GetImagery();
            TAnimImagery* anim_img = dynamic_cast<TAnimImagery*>(img);
            const int32_t st = oi->GetState();
            PTAnimation anim = (anim_img && st >= 0 && st < anim_img->NumStates()) ? anim_img->GetAnimation(st) : nullptr;
            PTBitmap bm = anim ? anim->GetFrame(oi->GetFrame()) : nullptr;
            const int32_t tex_idx = CacheSectorBitmap(s.sectorTileTex, bm, oi, img, st);
            if (tex_idx >= 0)
            {
                inst.asset_idx = tex_idx;
                if (img && bm)
                {
                    inst.regx = bm->regx - img->GetAnimRegX(st);
                    inst.regy = bm->regy - img->GetAnimRegY(st);
                    inst.regz = img->GetAnimRegZ(st);
                }
            }
        }
        if (inst.kind == ESectorDrawableKind::Tile && oi->ObjClass() != OBJCLASS_EFFECT)
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
    const float camera_forward = s.sectorCameraForward(vh);
    int32_t cam_ox = 0, cam_oy = 0;
    s.sectorCameraOriginScreen(cam_ox, cam_oy);

    float scene_z_min_fit = FLT_MAX, scene_z_max_fit = -FLT_MAX;
    int32_t fit_tiles = 0;
    for (const auto& inst : s.sectorDrawInst)
    {
        const float camera_z = (inst.kind == ESectorDrawableKind::Mesh)
            ? s.sectorCameraSceneZMesh(inst, vh)
            : s.sectorCameraSceneZ(inst, vh);
        float z0 = camera_z - 512.0f;
        float z1 = camera_z + 512.0f;
        if (inst.kind == ESectorDrawableKind::Tile)
        {
            const auto& tex = s.sectorTileTex[inst.asset_idx];
            S3DPoint sp; s.sectorProjectWorldForViewport(inst.world_pos, vh, sp);
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
    Renderer->SetPerspectiveDebugMode(s.sectorPerspectiveCamera ? s.sectorPerspectiveDebugMode : 0);
    Renderer->SetPerspectiveRaycastParams(s.sectorPerspectiveSteps, s.sectorPerspectiveRefine);
    Renderer->SetReconstructionParams(float(cam_ox), float(cam_oy), s.z_near, s.z_far,
                                      float(s.sectorCameraWorld.x), float(s.sectorCameraWorld.y),
                                      camera_forward, s.sectorPerspectiveCamera ? 1.0f : 0.0f,
                                      s.sectorCameraZoom,
                                      s.sectorPerspectiveZOffset,
                                      s.sectorPerspectiveZScale,
                                      s.sectorPerspectiveTileScale);

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
            S3DPoint sp; s.sectorProjectWorldForViewport(wp, vh, sp);
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
    const float cull_zoom = s.sectorCameraZoom > 0.01f ? s.sectorCameraZoom : 1.0f;
    float cull_scale = 1.0f / cull_zoom;
    if (s.sectorPerspectiveCamera)
    {
        const float max_depth = (std::max)(camera_forward, s.debugSceneZMaxFit + 1024.0f);
        const float focal_zoom = (std::max)(camera_forward * cull_zoom, 1.0f);
        cull_scale = (std::max)(cull_scale, max_depth / focal_zoom);
        cull_scale *= (std::max)(s.sectorPerspectiveTileScale, 1.0f);
    }
    auto unzoomScreen = [cull_scale](int32_t v) -> int32_t {
        return int32_t(std::floor(float(v) * cull_scale));
    };
    ScreenToWorld(unzoomScreen(-cam_ox - TRenderer::kGBufPad),
                  unzoomScreen(-cam_oy - TRenderer::kGBufPad), c0, 0);
    ScreenToWorld(unzoomScreen(vw - cam_ox + TRenderer::kGBufPad),
                  unzoomScreen(-cam_oy - TRenderer::kGBufPad), c1, 0);
    ScreenToWorld(unzoomScreen(-cam_ox - TRenderer::kGBufPad),
                  unzoomScreen(vh - cam_oy + TRenderer::kGBufPad), c2, 0);
    ScreenToWorld(unzoomScreen(vw - cam_ox + TRenderer::kGBufPad),
                  unzoomScreen(vh - cam_oy + TRenderer::kGBufPad), c3, 0);
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
    if (!s.debugGreenImage.id)
    {
        static const uint32_t kGreen = 0xFF00FF00u;
        sg_image_desc desc = {};
        desc.width = 1;
        desc.height = 1;
        desc.pixel_format = SG_PIXELFORMAT_RGBA8;
        desc.data.subimage[0][0] = { &kGreen, sizeof(kGreen) };
        desc.label = "maprenderer.debug.green";
        s.debugGreenImage = sg_make_image(&desc);
    }
    SMapRenderContext drawctx = {};
    drawctx.tile_assets = &s.sectorTileTex;
    drawctx.mesh_assets = &s.sectorMeshAsset;
    drawctx.debug_green_img = s.debugGreenImage;
    drawctx.sectorCameraWorld = s.sectorCameraWorld;
    drawctx.cam_ox = cam_ox;
    drawctx.cam_oy = cam_oy;
    drawctx.vw = vw;
    drawctx.vh = vh;
    drawctx.cam_forward = camera_forward;
    drawctx.camera_zoom = s.sectorCameraZoom;
    drawctx.tile_scale = s.sectorPerspectiveTileScale;
    drawctx.z_near = s.z_near;
    drawctx.z_far = s.z_far;
    drawctx.zspan = zspan;
    drawctx.depth_mul = s.depth_mul;
    drawctx.perspective_camera = s.sectorPerspectiveCamera;
    // Editor-only decorations (light / helper gizmo bitmaps drawn from
    // the per-instance Submit path) are forced off in game mode regardless
    // of the underlying toggle so the player never sees editor visuals.
    drawctx.show_gizmos = s.sectorShowGizmos && EditorOverlaysEnabled();
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
    ParticleManager().BeginDrawPulsePass();
    for (int32_t sy = min_sy; sy <= max_sy; ++sy)
    for (int32_t sx = min_sx; sx <= max_sx; ++sx)
    {
        auto it = s.sectorDrawBins.find(SectorBinKey(sx, sy));
        if (it == s.sectorDrawBins.end()) continue;
        for (int32_t idx : it->second)
        {
            // obj_id = drawable index + 1 (0 reserved for "no object").
            // OR editor-state flag bits into obj_id for any selected
            // drawable. We compare by map index (the universal id all
            // TSafeRef<T> share) so the test handles any selectable
            // type without committing to TObjectInstance here.
            uint32_t obj_id = uint32_t(idx) + 1u;
            // Selection highlight is an editor-only visual; suppress in
            // game mode even if the editor's selection set is non-empty.
            if (EditorOverlaysEnabled())
            {
                const int32_t mi = s.sectorDrawInst[idx].src.MapIndex();
                if (mi >= 0 && s.selectedMapIndices.count(mi))
                    obj_id |= kObjFlagSelected;
            }
            if (TObjectInstance* oi = s.sectorDrawInst[idx].src.Get())
                if (TParticleEffectComponent* particle_effect = oi->GetComponent<TParticleEffectComponent>())
                    particle_effect->DrawPulse();
            s.sectorDrawInst[idx].Submit(drawctx, stats, obj_id);
        }
    }
    SubmitParticleBillboards(drawctx, stats);
    // Mirror this frame's counts into the impl so the editor status bar
    // can read them without re-running the render.
    s.last_draw_counts.total_drawables  = int32_t(s.sectorDrawInst.size());
    s.last_draw_counts.tiles_submitted  = stats.draw_submitted;
    s.last_draw_counts.meshes_submitted = stats.mesh_submitted;
    s.last_draw_counts.offscreen_culled = stats.draw_offscreen;

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
                S3DPoint sp; s.sectorProjectWorldForViewport(s.sectorLightPos(L), Display ? Display->Height() : 0, sp);
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

void TMapRenderer::HandleKeyPress(int32_t key, bool down)
{
    if (!down || !impl) return;
    Impl& s = *impl;
    const int32_t step = ShiftDown ? 512 : 128;
    S3DPoint delta = {0, 0, 0};
    switch (key)
    {
        case VK_LEFT:
        case 'A': delta.x -= step; delta.y += step; break;
        case VK_RIGHT:
        case 'D': delta.x += step; delta.y -= step; break;
        case VK_UP:
        case 'W': delta.x -= step; delta.y -= step; break;
        case VK_DOWN:
        case 'S': delta.x += step; delta.y += step; break;
        default: return;
    }
    s.sectorCameraWorld.x += delta.x;
    s.sectorCameraWorld.y += delta.y;
    s.sectorDragging = false;
    s.lightDragIdx = -1;
}
