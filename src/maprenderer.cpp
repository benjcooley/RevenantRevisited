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
#include "player.h"
#include "runtimemode.h"
#include "revenant.h"
#include "revdefs.h"
#include "revutils.h"
#include "sector.h"
#include "surface.h"
#include "testconfig.h"
#include "time.h"

#include <algorithm>
#include <chrono>
#include <cfloat>
#include <cmath>
#include <cstdio>
#include <cstdint>
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

struct SMapContentSignature {
    int64_t version_sum = 0;
    int32_t object_count = 0;
    uint64_t object_hash = 1469598103934665603ull;
};

struct SMapCameraViewport {
    float scale = 1.0f;
    float offset_x = 0.0f;
    float offset_y = 0.0f;
};

using SMapFrameClock = std::chrono::steady_clock;

static float FrameProfileMs(SMapFrameClock::time_point start,
                            SMapFrameClock::time_point end)
{
    return std::chrono::duration<float, std::milli>(end - start).count();
}

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

static SMapCameraViewport ComputeMapCameraViewport(int32_t viewport_w, int32_t viewport_h)
{
    // The gameplay camera is authored against the classic 640x480 view.
    // Higher render resolutions cover the framebuffer with that virtual view
    // instead of expanding world coverage and changing the ortho camera scale.
    // Non-4:3 modes crop one axis; they must not reveal extra world.
    SMapCameraViewport v = {};
    if (viewport_w <= 0 || viewport_h <= 0)
        return v;

    const float sx = float(viewport_w) / float(WIDTH);
    const float sy = float(viewport_h) / float(HEIGHT);
    v.scale = (std::max)(0.0001f, (std::max)(sx, sy));
    v.offset_x = (float(viewport_w) - float(WIDTH) * v.scale) * 0.5f;
    v.offset_y = (float(viewport_h) - float(HEIGHT) * v.scale) * 0.5f;
    return v;
}

static int32_t ScalePixel(float scale, float value)
{
    return (std::max)(1, int32_t(std::lround(value * scale)));
}

static void MixMapSignature(uint64_t& hash, uint64_t value)
{
    hash ^= value;
    hash *= 1099511628211ull;
}

static void AccumulateObjectSignature(uint64_t& hash, uint64_t object_hash)
{
    const uint64_t rotated = (object_hash << 31) | (object_hash >> 33);
    hash += object_hash * 0x9E3779B185EBCA87ull;
    hash ^= rotated;
}

static uint64_t RendererSourcePointerKey(uint32_t domain, const void* ptr)
{
    uint64_t hash = 1469598103934665603ull;
    MixMapSignature(hash, domain);
    MixMapSignature(hash, uint64_t(uintptr_t(ptr) >> 4));
    return hash ? hash : 1ull;
}

static uint64_t RendererI3DMeshKey(AssetUid asset_id, int32_t objnum, int32_t texslot)
{
    uint64_t hash = asset_id ? asset_id : 1469598103934665603ull;
    MixMapSignature(hash, 0x4933444Du); // 'I3DM'
    MixMapSignature(hash, uint64_t(uint32_t(objnum)));
    MixMapSignature(hash, uint64_t(uint32_t(texslot + 1)));
    return hash ? hash : 1ull;
}

static int64_t ComputeMapContentVersionSum(const TGameMap* map)
{
    int64_t sum = 0;
    if (!map)
        return sum;
    for (TSector* sec : map->Sectors())
        if (sec)
            sum += sec->ContentVer();
    return sum;
}

static void AddRendererImagePairSourceRef(TObjectImagery* img,
                                          uint64_t key,
                                          RendererImagePairHandle handle)
{
    if (!Renderer || !img || handle == 0 ||
        img->ReferencedResourceHandle(EAssetReferencedResourceKind::RendererImagePair, key) != 0)
        return;

    Renderer->AddImagePairAssetRef(handle);
    img->AddReferencedResource(
        EAssetReferencedResourceKind::RendererImagePair,
        key,
        handle,
        "i2d.tile.image_pair",
        [handle]() {
            if (Renderer)
                Renderer->ReleaseImagePairAssetRef(handle);
        });
}

static void AddRendererMeshSourceRef(TObjectImagery* img, uint64_t key, MeshHandle handle)
{
    if (!Renderer || !img || handle == 0 ||
        img->ReferencedResourceHandle(EAssetReferencedResourceKind::RendererMesh, key) != 0)
        return;

    Renderer->AddMeshAssetRef(handle);
    img->AddReferencedResource(
        EAssetReferencedResourceKind::RendererMesh,
        key,
        handle,
        "i3d.mesh",
        [handle]() {
            if (Renderer)
                Renderer->ReleaseMeshAssetRef(handle);
        });
}

static SMapContentSignature ComputeMapContentSignature(const TGameMap* map)
{
    SMapContentSignature sig = {};
    if (!map)
        return sig;

    for (TSector* sec : map->Sectors())
    {
        if (!sec)
            continue;

        sig.version_sum += sec->ContentVer();
        for (int32_t i = 0; i < sec->NumItems(); ++i)
        {
            TObjectInstance* oi = sec->GetInstance(i);
            if (!oi)
                continue;

            ++sig.object_count;
            // Object identity, not location. Sector transfers advance
            // ContentVer but keep this signature stable, which lets the
            // renderer update bins without rebuilding resident assets.
            uint64_t object_hash = 1469598103934665603ull;
            MixMapSignature(object_hash, uint64_t(uintptr_t(oi) >> 4));
            MixMapSignature(object_hash, uint32_t(oi->GetMapIndex()));
            MixMapSignature(object_hash, uint32_t(oi->ObjClass()));
            MixMapSignature(object_hash, uint32_t(oi->ObjType()));
            AccumulateObjectSignature(sig.object_hash, object_hash);
        }
    }
    return sig;
}

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
    const float zoom = (std::max)(ctx.camera_zoom, 0.0001f);
    sx = float(iso_x) * zoom;
    sy = float(iso_y) * zoom;
    if (!ctx.perspective_camera)
        return;

    const float z = (std::max)(CameraDepth(rel, ctx.cam_forward), 1.0f);
    const float focal_zoom = (std::max)(ctx.cam_forward * zoom, 1.0f);
    sx = float(iso_x) * focal_zoom / z;
    sy = float(iso_y) * focal_zoom / z;
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

struct SSectorCullRange {
    int32_t min_sx = 0;
    int32_t max_sx = -1;
    int32_t min_sy = 0;
    int32_t max_sy = -1;
};

template <typename TImpl>
static void StoreDrawRecordRange(TImpl& s, const SSectorCullRange& r)
{
    s.drawRangeMinSx = r.min_sx;
    s.drawRangeMaxSx = r.max_sx;
    s.drawRangeMinSy = r.min_sy;
    s.drawRangeMaxSy = r.max_sy;
}

template <typename TImpl>
static SSectorCullRange ComputeRenderSectorRange(const TImpl& s,
                                                 int32_t vw, int32_t vh,
                                                 int32_t cam_ox, int32_t cam_oy,
                                                 float camera_forward,
                                                 float camera_zoom)
{
    S3DPoint c0, c1, c2, c3;
    const float cull_zoom = camera_zoom > 0.01f ? camera_zoom : 1.0f;
    const float cull_scale = 1.0f / cull_zoom;
    (void)camera_forward;
    // Perspective projection can make far-away map tiles collapse toward the
    // screen center. That is a fine projection effect, but it is not license
    // to submit the whole map. The renderer's candidate window is the bounded
    // gameplay camera rectangle, with submit-time tests handling the exact
    // tile volume inside that rectangle.
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

    SSectorCullRange r;
    r.min_sx = FloorDiv(min_wx, SECTORWIDTH);
    r.max_sx = FloorDiv(max_wx, SECTORWIDTH);
    r.min_sy = FloorDiv(min_wy, SECTORHEIGHT);
    r.max_sy = FloorDiv(max_wy, SECTORHEIGHT);
    return r;
}

// SubmitParticleBillboards: deleted in the Phase 1 VFX spine rewrite.
// Bucket submission now goes through Renderer->SubmitFxParticleBucket,
// which is drained inside the dedicated fx_pass after RunLightingPass
// (see docs/vfx/PHASE1_SPINE.md §5).

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
                                    float zraw_min, float zraw_max,
                                    float pad_px = float(TRenderer::kGBufPad))
{
    float anchor_x = 0.0f, anchor_y = 0.0f;
    if (ctx.perspective_camera)
    {
        int32_t sx_i = 0, sy_i = 0;
        WorldToScreen(rel, sx_i, sy_i);
        anchor_x = float(sx_i);
        anchor_y = float(sy_i);
    }
    else
    {
        ProjectCameraRelToScreen(ctx, rel, anchor_x, anchor_y);
    }

    const float scale = ctx.perspective_camera
        ? (ctx.tile_scale > 0.0f ? ctx.tile_scale : 1.0f)
        : (std::max)(ctx.camera_zoom, 0.0001f);
    const float x0 = anchor_x - float(regx) * scale;
    const float x1 = anchor_x + (float(w) - float(regx)) * scale;
    const float y0 = anchor_y - float(regy) * scale;
    const float y1 = anchor_y + (float(h) - float(regy)) * scale;
    if (!ctx.perspective_camera)
    {
        // Callers choose either the visible presentation rect (pad=0) or the
        // padded G-buffer rect (pad=kGBufPad) depending on whether the draw
        // contributes only color or also feeds screen-space occlusion.
        const float vx0 = -float(ctx.cam_ox) - pad_px;
        const float vy0 = -float(ctx.cam_oy) - pad_px;
        const float vx1 = float(ctx.vw - ctx.cam_ox) + pad_px;
        const float vy1 = float(ctx.vh - ctx.cam_oy) + pad_px;
        return x1 >= vx0 && x0 <= vx1 && y1 >= vy0 && y0 <= vy1;
    }

    float z0 = CameraDepth(rel, ctx.cam_forward) + zraw_min * ctx.depth_mul;
    float z1 = CameraDepth(rel, ctx.cam_forward) + zraw_max * ctx.depth_mul;
    if (z1 < z0) std::swap(z0, z1);
    z0 = (std::max)(z0, 1.0f);
    z1 = (std::max)(z1, z0 + 1.0f);

    const float focal_zoom = (std::max)(ctx.cam_forward * (ctx.camera_zoom > 0.0f ? ctx.camera_zoom : 1.0f), 1.0f);
    const float left   = -float(ctx.cam_ox) - pad_px;
    const float right  =  float(ctx.vw - ctx.cam_ox) + pad_px;
    const float top    = -float(ctx.cam_oy) - pad_px;
    const float bottom =  float(ctx.vh - ctx.cam_oy) + pad_px;

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
                             RendererImagePairHandle* out_image_pair,
                             uint64_t* out_key,
                             int32_t* out_w, int32_t* out_h,
                             float* out_z_local_min = nullptr,
                             float* out_z_local_max = nullptr,
                             SSectorTileTex::SDepthDump* out_z_dump = nullptr,
                             bool* out_has_alpha = nullptr,
                             uint32_t* out_bm_flags = nullptr,
                             std::vector<float>* out_cpu_depth_local = nullptr,
                             std::vector<uint8_t>* out_cpu_opaque = nullptr)
{
    if (!Renderer || !out_image_pair || !bm || bm->width <= 0 || bm->height <= 0) return false;
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
    const uint64_t key = RendererSourcePointerKey(1u, bm);
    const uint64_t gpu_bytes = uint64_t(npx) * (4u + uint64_t(sizeof(float)));
    *out_image_pair = Renderer->RegisterImagePairAsset(key,
                                                       rgba.get(), npx * 4,
                                                       dflt.get(), npx * sizeof(float),
                                                       w, h, gpu_bytes);
    if (!*out_image_pair)
        return false;
    if (out_key) *out_key = key;
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
    return true;
}

static int32_t CacheSectorBitmap(std::vector<SSectorTileTex>& cache,
                                 std::unordered_map<PTBitmap, int32_t>& by_bitmap,
                                 PTBitmap bm,
                                 TObjectInstance* oi,
                                 TObjectImagery* img,
                                 int32_t state)
{
    if (!bm) return -1;
    auto found = by_bitmap.find(bm);
    if (found != by_bitmap.end())
    {
        const int32_t idx = found->second;
        if ((uint32_t)idx < (uint32_t)cache.size() && cache[size_t(idx)].bm_key == bm)
            return idx;
    }

    SSectorTileTex tex = {};
    tex.bm_key = bm;
    tex.debug_classname = oi ? oi->GetClassName() : nullptr;
    tex.debug_typename = oi ? oi->GetTypeName() : nullptr;
    tex.debug_world_pos = oi ? oi->Pos() : S3DPoint{0,0,0};
    tex.debug_regx = img ? img->GetRegX(state) : 0;
    tex.debug_regy = img ? img->GetRegY(state) : 0;
    tex.debug_regz = img ? img->GetRegZ(state) : 0;
    uint32_t bm_flags = 0;
    uint64_t renderer_key = 0;
    if (!UploadTileBitmap(bm, &tex.image_pair, &renderer_key, &tex.w, &tex.h,
                          &tex.z_local_min, &tex.z_local_max,
                          &tex.z_dump, &tex.has_alpha, &bm_flags,
                          &tex.cpu_depth_local, &tex.cpu_opaque))
        return -1;
    AddRendererImagePairSourceRef(img, renderer_key, tex.image_pair);
    tex.pixel_count = tex.w * tex.h;
    for (uint8_t opaque : tex.cpu_opaque)
        if (opaque) ++tex.opaque_count;
    tex.bm_flags = bm_flags;
    const int32_t idx = int32_t(cache.size());
    cache.push_back(std::move(tex));
    by_bitmap[bm] = idx;
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
        const SRendererImagePairInfo* image_pair = Renderer ? Renderer->ImagePairInfo(tex.image_pair) : nullptr;
        const S3DPoint rel = {world_pos.x - ctx.sectorCameraWorld.x, world_pos.y - ctx.sectorCameraWorld.y, world_pos.z};
        int32_t anchor_cam_x = 0, anchor_cam_y = 0;
        WorldToScreen(rel, anchor_cam_x, anchor_cam_y);
        float spx = 0.0f, spy = 0.0f;
        ProjectCameraRelToScreen(ctx, rel, spx, spy);
        const float sprite_scale = (std::max)(ctx.camera_zoom, 0.0001f);
        const float anchor_scene = CameraDepth(rel, ctx.cam_forward) - ctx.depth_mul * float(regz);
        const float anchor_scene_norm = std::fabs(ctx.zspan) > 1e-6f ? (anchor_scene - ctx.z_near) / ctx.zspan : 0.5f;
        const int32_t dx = int32_t(std::lround(spx - float(regx) * sprite_scale)) + ctx.cam_ox;
        const int32_t dy = int32_t(std::lround(spy - float(regy) * sprite_scale)) + ctx.cam_oy;
        const int32_t dst_w = ScalePixel(sprite_scale, float(tex.w));
        const int32_t dst_h = ScalePixel(sprite_scale, float(tex.h));
        if (!image_pair) { ++stats.draw_invalid_img; return; }
        const bool visually_visible = TileCameraVolumeVisible(
            ctx, rel, regx, regy, tex.w, tex.h,
            has_authored_local_dz ? authored_local_dz_min : (tex.z_local_min - float(regz)),
            has_authored_local_dz ? authored_local_dz_max : (tex.z_local_max - float(regz)),
            0.0f);
        const bool gbuffer_visible = TileCameraVolumeVisible(
            ctx, rel, regx, regy, tex.w, tex.h,
            has_authored_local_dz ? authored_local_dz_min : (tex.z_local_min - float(regz)),
            has_authored_local_dz ? authored_local_dz_max : (tex.z_local_max - float(regz)));
        const bool visual_only = tex.has_alpha || oi->IsLight() || oi->ObjClass() == OBJCLASS_EFFECT;
        if ((visual_only && !visually_visible) || (!visual_only && !gbuffer_visible)) {
            ++stats.draw_offscreen;
            return;       // <-- BUG FIX: was falling through and submitting
        }
        if (ctx.cov && visually_visible)
        {
            int32_t cx0 = dx / ctx.cov_cell_px; if (cx0 < 0) cx0 = 0;
            int32_t cy0 = dy / ctx.cov_cell_px; if (cy0 < 0) cy0 = 0;
            int32_t cx1 = (dx + dst_w + ctx.cov_cell_px - 1) / ctx.cov_cell_px;
            int32_t cy1 = (dy + dst_h + ctx.cov_cell_px - 1) / ctx.cov_cell_px;
            if (cx1 > ctx.cov_cw) cx1 = ctx.cov_cw;
            if (cy1 > ctx.cov_ch) cy1 = ctx.cov_ch;
            for (int32_t cy = cy0; cy < cy1; ++cy)
                for (int32_t cx = cx0; cx < cx1; ++cx)
                    (*ctx.cov)[size_t(cy) * size_t(ctx.cov_cw) + size_t(cx)] = 1;
        }
        ++stats.draw_submitted;
        if (visually_visible) ++stats.draw_visible_submitted;
        else ++stats.draw_gbuffer_border_submitted;
        if (oi->IsLight() || oi->ObjClass() == OBJCLASS_EFFECT)
        {
            float light_sx = spx;
            float light_sy = spy;
            float overlay_scale = (std::max)(ctx.camera_zoom, 0.0001f);
            if (ctx.perspective_camera)
            {
                const float z = (std::max)(CameraDepth(rel, ctx.cam_forward), 1.0f);
                const float focal_zoom = (std::max)(ctx.cam_forward * ctx.camera_zoom, 1.0f);
                overlay_scale = focal_zoom / z;
            }
            SOverlaySubmit sub = {};
            sub.image_pair = tex.image_pair;
            sub.dst_x = int32_t(std::lround(light_sx - float(regx) * overlay_scale)) + ctx.cam_ox;
            sub.dst_y = int32_t(std::lround(light_sy - float(regy) * overlay_scale)) + ctx.cam_oy;
            sub.dst_w = (std::max)(1, int32_t(std::lround(float(tex.w) * overlay_scale)));
            sub.dst_h = (std::max)(1, int32_t(std::lround(float(tex.h) * overlay_scale)));
            Renderer->SubmitOverlay(sub);
            return;
        }
        STileSubmit sub = {};
        sub.image_pair  = tex.image_pair;
        sub.dst_x       = dx;
        sub.dst_y       = dy;
        sub.dst_w       = dst_w;
        sub.dst_h       = dst_h;
        sub.src_w       = tex.w;
        sub.src_h       = tex.h;
        sub.anchor_z    = anchor_scene_norm;
        sub.depth_mul   = std::fabs(ctx.zspan) > 1e-6f ? ctx.depth_mul / ctx.zspan : 0.0f;
        sub.normal_mul  = 1.0f;
        sub.root_wx     = float(world_pos.x);
        sub.root_wy     = float(world_pos.y);
        sub.root_wz     = float(world_pos.z);
        sub.anchor_px_x = float(regx);
        sub.anchor_px_y = float(regy);
        if (tex.z_dump.sample_count > 0)
        {
            sub.coverage_px_x0 = float(tex.z_dump.bbox_x0);
            sub.coverage_px_y0 = float(tex.z_dump.bbox_y0);
            sub.coverage_px_x1 = float(tex.z_dump.bbox_x1);
            sub.coverage_px_y1 = float(tex.z_dump.bbox_y1);
        }
        else
        {
            sub.coverage_px_x0 = 0.0f;
            sub.coverage_px_y0 = 0.0f;
            sub.coverage_px_x1 = float(tex.w);
            sub.coverage_px_y1 = float(tex.h);
        }
        sub.anchor_cam_x = float(anchor_cam_x);
        sub.anchor_cam_y = float(anchor_cam_y);
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
        if (!flipbook || !Renderer)
            return;
        if (flipbook->Texture() == kInvalidTexture)
            return;

        static bool logged_billboard_submit = false;
        if (!logged_billboard_submit)
        {
            logged_billboard_submit = true;
            log_info("[component-render] submit billboard via fx inst=%p class='%s' type='%s' debug=%d texture=%u",
                     (void*)oi, oi->GetClassName(), oi->GetTypeName(),
                     int(flipbook->DebugMode()), flipbook->Texture());
        }
        flipbook->Submit(*Renderer, *oi);
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

        // Cheap padded-G-buffer cull: project the mesh anchor to screen
        // pixels and skip only once it is well outside the drawable border.
        // The object-size margin covers tall/wide meshes whose bounds extend
        // past the anchor; kGBufPad preserves offscreen shadow occluders.
        {
            const S3DPoint mesh_world  = MapRendererMeshWorld(world_pos,             ctx.mesh_scale_x, ctx.mesh_scale_y, ctx.mesh_scale_z);
            const S3DPoint mesh_camera = MapRendererMeshWorld(ctx.sectorCameraWorld, ctx.mesh_scale_x, ctx.mesh_scale_y, ctx.mesh_scale_z);
            const S3DPoint rel = { mesh_world.x - mesh_camera.x, mesh_world.y - mesh_camera.y, mesh_world.z };
            float spx = 0.0f, spy = 0.0f;
            ProjectCameraRelToScreen(ctx, rel, spx, spy);
            const int32_t px = int32_t(std::lround(spx)) + ctx.cam_ox;
            const int32_t py = int32_t(std::lround(spy)) + ctx.cam_oy;
            const int32_t kMargin = ScalePixel(ctx.camera_zoom, 256.0f);
            const int32_t pad = TRenderer::kGBufPad;
            if (px + kMargin <= -pad || py + kMargin <= -pad ||
                px - kMargin >= ctx.vw + pad || py - kMargin >= ctx.vh + pad)
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
        // Runtime meshes read world matrices from live animator bones. The
        // frame loop creates/updates needed animators before submit, so render
        // submission must not sample legacy I3D pose data or allocate fallback
        // poses here. Editor preview is the one explicit exception: it asks for
        // a static matrix path through force_mesh_preview_pose.
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
                    const hmm_mat4& bone_world = bone->transform.Matrix();
                    TransposeSourceToRenderer(bone_world, world_renderer);
                    used_bone_xform = true;
                }
            }
        }
        if (!used_bone_xform)
        {
            if (!ctx.force_mesh_preview_pose)
            {
                ++stats.mesh_skipped;
                return;
            }

            float local_renderer[16];
            BuildStaticObjectMatrix(meshimg, asset.objnum, 0, 0, local_renderer);
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
TMapRenderer::~TMapRenderer()
{
  // Defensive: unregister our map listener even if Shutdown() wasn't
  // called (e.g. abrupt teardown path that destroys TPlayScreen via
  // its unique_ptr without going through Close()). Idempotent — safe
  // to run after a normal Shutdown().
    if (impl)
    {
        if (TGameMap* m = impl->currentMap.Get(); m && impl->mapListenerId)
        {
            m->RemoveListener(impl->mapListenerId);
            impl->mapListenerId = 0;
        }
    }
}

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
    // The map camera is calibrated in logical 640x480 pixels, then scaled to
    // the physical render target. Keep the focal length in that logical space;
    // the viewport scale is applied separately as camera_zoom.
    return t > 1.0e-5f ? (float(viewport_h) * 0.5f) / t
                       : kMapRendererCamForwardWU;
}

void TMapRenderer::Impl::sectorProjectWorldForViewport(const S3DPoint& world,
                                                       int32_t viewport_h,
                                                       S3DPoint& screen) const
{
    (void)viewport_h;
    // Keep CPU-projected world markers in the same coordinate convention as
    // the GPU mesh shader: camera-relative X/Y, absolute world Z.
    // This helper returns logical 640x480 camera coordinates. Physical render
    // targets apply ComputeMapCameraViewport scale/offset at the call site.
    const S3DPoint rel = { world.x - sectorCameraWorld.x,
                           world.y - sectorCameraWorld.y,
                           world.z };
    WorldToScreen(rel, screen.x, screen.y);
    const float scene_z = sectorCameraDepth(rel, HEIGHT);
    screen.z = int32_t(scene_z);
    if (!sectorPerspectiveCamera)
        return;

    const float focal_zoom = (std::max)(sectorCameraForward(HEIGHT) * sectorCameraZoom, 1.0f);
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

    TGameMap* m = s.currentMap.Get();
    if (!m) return;

    const int64_t version_sum = ComputeMapContentVersionSum(m);
    if (version_sum == s.lastSyncedSectorVerSum)
    {
        return;
    }

    // Camera motion is not content. A cache rebuild is only justified when
    // loaded sector contents change; ordinary movement through the map should
    // only update the per-frame candidate list and submit-time culling.
    RebuildForCurrentMap();

    log_info("[mr-sync] content rebuild: records=%zu range=(%d..%d,%d..%d) lights=%zu/%d ver_sum=%lld objects=%d",
             s.sectorDrawInst.size(),
             s.drawRangeMinSx, s.drawRangeMaxSx, s.drawRangeMinSy, s.drawRangeMaxSy,
             s.sectorLights.size(), s.residentPointLightCount,
             (long long)s.lastSyncedSectorVerSum, s.lastSyncedObjectSetCount);
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
    out.draw_candidates  = impl->last_draw_counts.draw_candidates;
    out.resident_lights  = impl->last_draw_counts.resident_lights;
    out.active_lights    = impl->last_draw_counts.active_lights;
    out.point_lights_considered = impl->last_draw_counts.point_lights_considered;
    out.point_lights_submitted = impl->last_draw_counts.point_lights_submitted;
    out.tiles_submitted  = impl->last_draw_counts.tiles_submitted;
    out.tiles_visible_submitted = impl->last_draw_counts.tiles_visible_submitted;
    out.tiles_gbuffer_border_submitted = impl->last_draw_counts.tiles_gbuffer_border_submitted;
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

    // Bind the renderer to the now-loaded TGameMap. SetMap subscribes to map
    // events and triggers RebuildForCurrentMap, which builds non-owning draw
    // and light records over the level-resident asset cache.
    SetMap(gmap, use_level_origin, keep_sx, keep_sy);

    DebugUI::RegisterContributor(this);
    return true;
}

void TMapRenderer::SetMap(TGameMap* m, bool use_level_origin,
                          int32_t anchor_sx, int32_t anchor_sy)
{
    Impl& s = *impl;
    TGameMap* old = s.currentMap.Get();

    // Unsubscribe from the old map's events before flipping the ref.
    if (old && s.mapListenerId)
    {
        old->RemoveListener(s.mapListenerId);
        s.mapListenerId = 0;
    }
    if (old != m)
    {
        s.clearDrawRecords();
        // Resident assets are cumulative for the session. Switching maps only
        // invalidates draw/light records; assets remain pinned in the renderer
        // cache even if their ref count drops to zero.
    }

    s.currentMap = m;
    s.drawRecordLevel = -1;

    if (m)
    {
        // Update camera-level + anchor state for the new map.
        s.cameraLevel           = m->Level();
        s.useInitialLevelOrigin = use_level_origin;
        s.initialAnchorSx       = anchor_sx;
        s.initialAnchorSy       = anchor_sy;

        // Subscribe to map lifetime/content events. Loaded implies a fresh
        // level scan; Updated can often be reconciled without touching GPU
        // assets. Unloaded drops non-owning records, but resident assets are
        // retained until explicit renderer/game shutdown.
        s.mapListenerId = m->AddListener(
            [this](EGameMapEvent ev, TGameMap* /*map*/) {
                if (ev == EGameMapEvent::Unloaded)
                {
                    impl->currentMap.Clear();
                    impl->mapListenerId = 0;
                    impl->clearDrawRecords();
                    return;
                }
                if (ev == EGameMapEvent::Updated)
                    SyncContentsCache();
                else
                    RebuildForCurrentMap();
            });
    }

    RebuildForCurrentMap();
}

template <typename TImpl>
static void CountExistingAssetRefForObject(TImpl& s, TObjectInstance* oi)
{
    if (!oi)
        return;

    TObjectImagery* img = oi->GetImagery();
    if (!img)
        return;
    SImageryHeader* hdr = img->GetHeader();
    SImageryBody* body = img->GetBody();
    if (!hdr || !body)
        return;

    const int32_t st = oi->GetState();
    if (hdr->imageryid == OBJIMAGE_MESH3D)
    {
        T3DImagery* meshimg = dynamic_cast<T3DImagery*>(img);
        if (!meshimg || st < 0 || st >= hdr->numstates)
            return;

        for (SSectorMeshAsset& asset : s.sectorMeshAsset)
        {
            if (asset.imagery_key != img)
                continue;
            if (asset.objnum < 0 || asset.objnum >= meshimg->NumObjects())
                continue;
            if (meshimg->IsHidden(asset.objnum, st))
                continue;

            ++asset.ref_count;
        }
        return;
    }

    PTBitmap bm = nullptr;
    if (oi->ObjClass() == OBJCLASS_EFFECT && hdr->imageryid == OBJIMAGE_ANIMATION)
    {
        TAnimImagery* anim_img = dynamic_cast<TAnimImagery*>(img);
        PTAnimation anim = (anim_img && st >= 0 && st < anim_img->NumStates()) ? anim_img->GetAnimation(st) : nullptr;
        bm = anim ? anim->GetFrame(oi->GetFrame()) : nullptr;
    }
    else if (oi->ObjClass() == OBJCLASS_TILE && hdr->imageryid == OBJIMAGE_ANIMATION)
    {
        if (st < 0 || st >= hdr->numstates)
            return;
        auto* ab = (SAnimImageryBody*)body;
        bm = (TBitmap*)ab->states[st].still;
    }

    if (!bm)
        return;

    for (SSectorTileTex& tex : s.sectorTileTex)
    {
        if (tex.bm_key != bm)
            continue;

        ++tex.ref_count;
        return;
    }
}

template <typename TImpl>
static void RecomputeLoadedMapAssetRefs(TImpl& s)
{
    // Draw records are non-owning and map switches are not unloads. Reference
    // counts are therefore recomputed from all maps currently cached by
    // TMapManager, not from whatever the current camera happens to draw.
    s.resetAssetRefCounts();
    MapManager.ForEachLoadedMap(
        [&s](TGameMap* map) {
            if (!map)
                return;
            for (TSector* sec : map->Sectors())
            {
                if (!sec)
                    continue;
                for (int32_t i = 0; i < sec->NumItems(); ++i)
                    CountExistingAssetRefForObject(s, sec->GetInstance(i));
            }
        });
}

void TMapRenderer::RebuildForCurrentMap()
{
    Impl& s = *impl;

    // Rebuild non-owning draw/light records only. This scan may register new
    // renderer assets, but draw records do not own them. A final residency pass
    // below recomputes asset refs from all loaded maps; zero-ref assets are
    // retained until explicit shutdown or a future eviction policy.
    s.clearDrawRecords();
    s.resetAssetRefCounts();

    TGameMap* map = s.currentMap.Get();
    if (!map) return;
    const bool preserve_camera = (s.drawRecordLevel == map->Level());
    const int32_t saved_camera_level = s.cameraLevel;
    const S3DPoint saved_camera_world = s.sectorCameraWorld;

    // Pull anchor + level state from Impl (set by
    // InitializeFromStartupArgs at boot, reused on level swaps).
    const int32_t keep_sx  = s.initialAnchorSx;
    const int32_t keep_sy  = s.initialAnchorSy;
    const bool    use_level_origin = s.useInitialLevelOrigin;

    // Build draw records from every resident sector. Visibility/camera culling
    // happens at submit time; draw records are not a GPU residency mechanism.
    struct SLoaded { int32_t lvl, sx, sy; TSector* sec; };
    std::vector<SLoaded> loaded;
    loaded.reserve(map->Sectors().size());
    bool have_bounds = false;
    int32_t min_sx = 0, max_sx = 0, min_sy = 0, max_sy = 0;
    for (TSector* sec : map->Sectors())
    {
        if (!sec)
            continue;
        loaded.push_back({ sec->SectorLevel(), sec->SectorX(), sec->SectorY(), sec });
        if (!have_bounds)
        {
            min_sx = max_sx = sec->SectorX();
            min_sy = max_sy = sec->SectorY();
            have_bounds = true;
        }
        else
        {
            min_sx = (std::min)(min_sx, sec->SectorX());
            max_sx = (std::max)(max_sx, sec->SectorX());
            min_sy = (std::min)(min_sy, sec->SectorY());
            max_sy = (std::max)(max_sy, sec->SectorY());
        }
    }
    if (loaded.empty())
        return;

    int32_t focus_sx = keep_sx;
    int32_t focus_sy = keep_sy;
    S3DPoint cull_camera_world = s.sectorCameraWorld;
    bool focus_from_player = false;
    if (Player && Player->GetLevel() == map->Level())
    {
        S3DPoint p;
        Player->GetPos(p);
        focus_sx = p.x >> SECTORWSHIFT;
        focus_sy = p.y >> SECTORHSHIFT;
        cull_camera_world = p;
        focus_from_player = true;
    }
    else if (use_level_origin && have_bounds)
    {
        focus_sx = (min_sx + max_sx) / 2;
        focus_sy = (min_sy + max_sy) / 2;
    }
    if (!focus_from_player && !preserve_camera)
    {
        cull_camera_world = {
            focus_sx * SECTORWIDTH + SECTORWIDTH / 2,
            focus_sy * SECTORHEIGHT + SECTORHEIGHT / 2,
            0
        };
    }
    s.sectorCameraWorld = cull_camera_world;
    s.drawRecordLevel = map->Level();

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
            {
                meshimg->AttachAnimatorComponents(oi);
                if (oi->NeedsAnimator() && !oi->HasAnimator())
                    oi->OnScreen();
                if (!oi->IsAnimated() && oi->HasAnimator())
                    oi->Animate(false);
            }

            TFlipbookBillboardComponent* flipbook = oi->GetComponent<TFlipbookBillboardComponent>();
            if (flipbook)
            {
                static bool logged_billboard_build = false;
                if (!logged_billboard_build)
                {
                    logged_billboard_build = true;
                    log_info("[component-render] build billboard inst=%p class='%s' type='%s' debug=%d texture=%u size=%.1fx%.1f",
                             (void*)oi, oi->GetClassName(), oi->GetTypeName(),
                             int(flipbook->DebugMode()),
                             flipbook->Texture(),
                             flipbook->Width(), flipbook->Height());
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

                            TTextureHandle albedo = kInvalidTexture;
                            if (texslot > 0)
                                albedo = meshimg->GetTextureHandle(texslot - 1);
                            if (albedo == kInvalidTexture && oi->ObjClass() == OBJCLASS_HELPER)
                            {
                                S3DObj o = {};
                                meshimg->GetObject(objnum, &o);
                                if (o.material >= 0 && o.material < meshimg->NumMaterials())
                                {
                                    S3DMat mat = {};
                                    meshimg->GetMaterial(o.material, &mat);
                                    if (mat.texture >= 0 && mat.texture < meshimg->NumTextures())
                                        albedo = meshimg->GetTextureHandle(mat.texture);
                                }
                            }
                            if (albedo == kInvalidTexture)
                                albedo = Renderer ? Renderer->WhiteTextureHandle() : kInvalidTexture;

                            const uint64_t mesh_key = RendererI3DMeshKey(meshimg->AssetId(), objnum, texslot);
                            MeshHandle h = Renderer->RegisterMeshAsset(
                                mesh_key,
                                verts.data(), int32_t(verts.size()),
                                indices.data(), int32_t(indices.size()),
                                albedo);
                            if (!h) { ++mesh_upload_fail; continue; }
                            AddRendererMeshSourceRef(meshimg, mesh_key, h);

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
                                TTextureHandle albedo = kInvalidTexture;
                                if (meshimg->NumTextures() > 0)
                                    albedo = meshimg->GetTextureHandle(0);
                                if (albedo == kInvalidTexture && oi->ObjClass() == OBJCLASS_HELPER)
                                {
                                    S3DObj o = {};
                                    meshimg->GetObject(objnum, &o);
                                    if (o.material >= 0 && o.material < meshimg->NumMaterials())
                                    {
                                        S3DMat mat = {};
                                        meshimg->GetMaterial(o.material, &mat);
                                        if (mat.texture >= 0 && mat.texture < meshimg->NumTextures())
                                            albedo = meshimg->GetTextureHandle(mat.texture);
                                    }
                                }
                                if (albedo == kInvalidTexture)
                                    albedo = Renderer ? Renderer->WhiteTextureHandle() : kInvalidTexture;
                                const uint64_t mesh_key = RendererI3DMeshKey(meshimg->AssetId(), objnum, -1);
                                MeshHandle h = Renderer->RegisterMeshAsset(
                                    mesh_key,
                                    verts.data(), int32_t(verts.size()),
                                    indices.data(), int32_t(indices.size()),
                                    albedo);
                                if (h)
                                {
                                    AddRendererMeshSourceRef(meshimg, mesh_key, h);
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

                const int32_t tex_idx = CacheSectorBitmap(s.sectorTileTex, s.sectorTileTexByBitmap, bm, oi, img, st);
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
                uint64_t renderer_key = 0;
                if (!UploadTileBitmap(bm, &t.image_pair, &renderer_key, &t.w, &t.h,
                                      &t.z_local_min, &t.z_local_max,
                                      &t.z_dump, &t.has_alpha, &bm_flags,
                                      &t.cpu_depth_local, &t.cpu_opaque))
                { ++upload_fail; continue; }
                AddRendererImagePairSourceRef(img, renderer_key, t.image_pair);
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
    const int32_t focus_wx0 = (focus_sx - 1) * SECTORWIDTH;
    const int32_t focus_wx1 = (focus_sx + 2) * SECTORWIDTH;
    const int32_t focus_wy0 = (focus_sy - 1) * SECTORHEIGHT;
    const int32_t focus_wy1 = (focus_sy + 2) * SECTORHEIGHT;
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
    const int32_t bw = view_bounds.sx_max - view_bounds.sx_min;
    const int32_t bh = view_bounds.sy_max - view_bounds.sy_min;
    if (use_level_origin) {
        s.sectorCenterOx = WIDTH / 2;
        s.sectorCenterOy = HEIGHT / 2;
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
            log_warn("[sector] focus sector %d_%d has no tile anchors; centering exact sector coordinates", focus_sx, focus_sy);
        s.sectorWorldCenter = {
            focus_sx * SECTORWIDTH + SECTORWIDTH / 2,
            focus_sy * SECTORHEIGHT + SECTORHEIGHT / 2,
            0
        };
        s.sectorCameraWorld = s.sectorWorldCenter;
        S3DPoint sector_center_screen;
        WorldToScreen(s.sectorWorldCenter, sector_center_screen);
        s.sectorCenterOx = WIDTH / 2 - sector_center_screen.x;
        s.sectorCenterOy = HEIGHT / 2 - sector_center_screen.y;
    }
    if (preserve_camera)
    {
        s.cameraLevel = saved_camera_level;
        s.sectorCameraWorld = saved_camera_world;
    }

    StoreDrawRecordRange(s, SSectorCullRange{ min_sx, max_sx, min_sy, max_sy });

    float sz_min = FLT_MAX, sz_max = -FLT_MAX;
    const float init_camera_forward = s.sectorCameraForward(HEIGHT);
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
    if (!(sz_min < sz_max))
    {
        sz_min = kSectorDepthNearWU;
        sz_max = kSectorDepthFarWU;
    }
    s.sectorSceneZMin = sz_min; s.sectorSceneZMax = sz_max;
    s.sectorDrawInst = std::move(draw_work);
    RecomputeLoadedMapAssetRefs(s);
    int32_t resident_lights = 0;
    for (auto& Ls : loaded)
    {
        TSector* sec = Ls.sec;
        for (int32_t i = 0; i < sec->NumItems(); ++i)
        {
            TObjectInstance* oi = sec->GetInstance(i);
            if (!oi || !oi->IsLight()) continue;
            PSLightDef ld = oi->GetLightDef();
            if (!ld || ld->intensity == 0) continue;
            ++resident_lights;
            s.sectorLights.push_back({ TSafeRef<>(oi), true });
        }
    }
    s.residentPointLightCount = resident_lights;
    s.rebuildBins();
    const SMapContentSignature sig = ComputeMapContentSignature(map);
    s.lastSyncedSectorVerSum = sig.version_sum;
    s.lastSyncedObjectSetCount = sig.object_count;
    s.lastSyncedObjectSetHash = sig.object_hash;
}

void TMapRenderer::Shutdown()
{
    Impl& s = *impl;

  // Unregister our map listener BEFORE clearing currentMap so a later
  // ~TGameMap()→Unload()→Notify() can't fire the lambda after our
  // impl is dead. Static destruction order at process exit can drop
  // TMapRenderer before TMapManager, and the lambda captures `this`;
  // ASan caught this as a heap-use-after-free at exit (load 8 through
  // a freed unique_ptr<Impl>).
    if (TGameMap* m = s.currentMap.Get(); m && s.mapListenerId)
    {
        m->RemoveListener(s.mapListenerId);
        s.mapListenerId = 0;
    }

    s.clearResidentAssetMetadataForShutdown();
    s.debugGreenTexture = kInvalidTexture;
    s.clearDrawRecords();
    // Sectors are owned by TGameMap (via TMapManager); the renderer
    // just borrowed via the SafeRef. Drop the ref without freeing.
    s.currentMap.Clear();
    s.lightDragIdx = -1;
    s.sectorDragging = false;
    DebugUI::UnregisterContributor(this);
}

void TMapRenderer::RenderFrame()
{
    Impl& s = *impl;
    if (!Display.IsActive() || !Display.BackBuffer())
        return;

    SMapFrameTimings timings = {};
    const auto frame_begin = SMapFrameClock::now();
    auto phase_begin = frame_begin;
    auto mark_phase = [&](float& slot) {
        const auto now = SMapFrameClock::now();
        slot = FrameProfileMs(phase_begin, now);
        phase_begin = now;
    };
    auto finish_timings = [&]() {
        const auto now = SMapFrameClock::now();
        timings.total_ms = FrameProfileMs(frame_begin, now);
        s.last_frame_timings = timings;
    };

    // Reconcile draw intent against map contents. Camera movement affects only
    // submit-time culling, never draw-list or GPU-asset residency.
    SyncContentsCache();
    mark_phase(timings.sync_ms);

    TGameMap* current_map = s.currentMap.Get();
    if (s.sectorDrawInst.empty())
    {
        finish_timings();
        return;
    }

    if (current_map)
    {
        for (TMapIterator i(nullptr, CHECK_NOINVENT, OBJSET_ANIMATE); i; i++)
        {
            TObjectInstance* oi = i.Item();
            if (!oi || !oi->IsAnimated()) continue;
            if (oi->NeedsAnimator() && !oi->HasAnimator())
                oi->OnScreen();
            // Authoritative 24 Hz frame advancement lives in the game/map
            // tick path (TMapPane::Pulse in the legacy pane path, or
            // TGameModeImpl::Tick in the modern game path). The renderer only
            // refreshes/samples poses for the current draw. Calling NextFrame()
            // here double-advances live gameplay animation and skips authored
            // keys at loop boundaries.
            oi->Animate(false);
        }
    }
    mark_phase(timings.animate_ms);

    auto refresh_draw_inst = [&](SSectorDrawableInst& inst) {
        TObjectInstance* oi = inst.src.Get();
        if (!oi) return;
        const int32_t old_state = inst.state;
        const int32_t old_frame = inst.frame;
        inst.world_pos = oi->Pos();
        inst.state = oi->GetState();
        inst.frame = oi->GetFrame();
        if (oi->ObjClass() == OBJCLASS_EFFECT)
        {
            if (inst.asset_idx >= 0 && inst.state == old_state && inst.frame == old_frame)
                return;
            TObjectImagery* img = oi->GetImagery();
            TAnimImagery* anim_img = dynamic_cast<TAnimImagery*>(img);
            const int32_t st = oi->GetState();
            PTAnimation anim = (anim_img && st >= 0 && st < anim_img->NumStates()) ? anim_img->GetAnimation(st) : nullptr;
            PTBitmap bm = anim ? anim->GetFrame(oi->GetFrame()) : nullptr;
            const int32_t tex_idx = CacheSectorBitmap(s.sectorTileTex, s.sectorTileTexByBitmap, bm, oi, img, st);
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
    };
    const int32_t vw = Display.Width();
    const int32_t vh = Display.Height();
    const SMapCameraViewport camera_view = ComputeMapCameraViewport(vw, vh);
    const float effective_camera_zoom = s.sectorCameraZoom * camera_view.scale;
    const float camera_forward = s.sectorCameraForward(HEIGHT);
    int32_t cam_ox_logical = 0, cam_oy_logical = 0;
    s.sectorCameraOriginScreen(cam_ox_logical, cam_oy_logical);
    const int32_t cam_ox = int32_t(std::lround(camera_view.offset_x + float(cam_ox_logical) * camera_view.scale));
    const int32_t cam_oy = int32_t(std::lround(camera_view.offset_y + float(cam_oy_logical) * camera_view.scale));

    SMapRenderContext projectctx = {};
    projectctx.sectorCameraWorld = s.sectorCameraWorld;
    projectctx.cam_ox = cam_ox;
    projectctx.cam_oy = cam_oy;
    projectctx.vw = vw;
    projectctx.vh = vh;
    projectctx.cam_forward = camera_forward;
    projectctx.camera_zoom = effective_camera_zoom;
    projectctx.tile_scale = s.sectorPerspectiveTileScale;
    projectctx.z_near = s.z_near;
    projectctx.z_far = s.z_far;
    projectctx.zspan = s.z_far - s.z_near;
    projectctx.depth_mul = s.depth_mul;
    projectctx.perspective_camera = s.sectorPerspectiveCamera;
    const SSectorCullRange render_range =
        ComputeRenderSectorRange(s, vw, vh, cam_ox, cam_oy, camera_forward,
                                 effective_camera_zoom);
    s.frameDrawIndices.clear();
    for (int32_t sy = render_range.min_sy; sy <= render_range.max_sy; ++sy)
    for (int32_t sx = render_range.min_sx; sx <= render_range.max_sx; ++sx)
    {
        auto it = s.sectorDrawBins.find(SectorBinKey(sx, sy));
        if (it == s.sectorDrawBins.end())
            continue;
        s.frameDrawIndices.insert(s.frameDrawIndices.end(), it->second.begin(), it->second.end());
    }
    s.frameLightIndices.clear();
    constexpr int32_t kLightSectorPad = 2;
    for (int32_t sy = render_range.min_sy - kLightSectorPad; sy <= render_range.max_sy + kLightSectorPad; ++sy)
    for (int32_t sx = render_range.min_sx - kLightSectorPad; sx <= render_range.max_sx + kLightSectorPad; ++sx)
    {
        auto it = s.sectorLightBins.find(SectorBinKey(sx, sy));
        if (it == s.sectorLightBins.end())
            continue;
        s.frameLightIndices.insert(s.frameLightIndices.end(), it->second.begin(), it->second.end());
    }
    mark_phase(timings.setup_ms);

    // Only the records in the current padded render range need per-frame
    // instance refresh. The resident draw list can be much larger than the
    // submit window, and walking the whole map here was pure CPU drag.
    for (int32_t idx : s.frameDrawIndices)
        if (idx >= 0 && idx < int32_t(s.sectorDrawInst.size()))
            refresh_draw_inst(s.sectorDrawInst[idx]);
    mark_phase(timings.refresh_ms);

    float scene_z_min_fit = FLT_MAX, scene_z_max_fit = -FLT_MAX;
    int32_t fit_tiles = 0;
    for (int32_t idx : s.frameDrawIndices)
    {
        if (idx < 0 || idx >= int32_t(s.sectorDrawInst.size()))
            continue;
        const auto& inst = s.sectorDrawInst[idx];
        const float camera_z = (inst.kind == ESectorDrawableKind::Mesh)
            ? s.sectorCameraSceneZMesh(inst, HEIGHT)
            : s.sectorCameraSceneZ(inst, HEIGHT);
        float z0 = camera_z - 512.0f;
        float z1 = camera_z + 512.0f;
        if (inst.kind == ESectorDrawableKind::Tile)
        {
            const auto& tex = s.sectorTileTex[inst.asset_idx];
            float spx = 0.0f, spy = 0.0f;
            ProjectWorldPointToScreen(projectctx, inst.world_pos, spx, spy);
            const int32_t x0 = int32_t(std::lround(spx - float(inst.regx) * effective_camera_zoom)) + cam_ox;
            const int32_t y0 = int32_t(std::lround(spy - float(inst.regy) * effective_camera_zoom)) + cam_oy;
            const int32_t x1 = x0 + ScalePixel(effective_camera_zoom, float(tex.w));
            const int32_t y1 = y0 + ScalePixel(effective_camera_zoom, float(tex.h));
            // Fit depth against the same padded rectangle the renderer fills
            // for occlusion. Presented albedo is still cropped to the visible
            // display rect, but offscreen border occluders need correct z.
            const int32_t pad = TRenderer::kGBufPad;
            if (x1 <= -pad || y1 <= -pad || x0 >= vw + pad || y0 >= vh + pad)
                continue;
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
    mark_phase(timings.depth_fit_ms);

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
        // The puck describes incoming light in screen space; the renderer
        // uses the opposite vector, from the receiver back toward the sun.
        const float to_sun_screen_x = s.puck_u;
        const float to_sun_screen_y = s.puck_v;
        const float to_sun_center = std::sqrt(fmaxf(0.0f, 1.0f - r2));
        const float fx = kIsoCos30, fy = kIsoCos30, fz = 1.0f;
        const float fn = std::sqrt(fx*fx + fy*fy + fz*fz);
        const float FwX = fx / fn, FwY = fy / fn, FwZ = fz / fn;
        const float rx = 1.0f, ry = -1.0f, rz = 0.0f;
        const float rn = std::sqrt(rx*rx + ry*ry + rz*rz);
        const float RwX = rx / rn, RwY = ry / rn, RwZ = rz / rn;
        const float UwX = -FwY * RwZ + FwZ * RwY;
        const float UwY = -FwZ * RwX + FwX * RwZ;
        const float UwZ = -FwX * RwY + FwY * RwX;
        s.light_dir[0] = to_sun_screen_x * RwX + to_sun_screen_y * UwX + to_sun_center * FwX;
        s.light_dir[1] = to_sun_screen_x * RwY + to_sun_screen_y * UwY + to_sun_center * FwY;
        s.light_dir[2] = to_sun_screen_x * RwZ + to_sun_screen_y * UwZ + to_sun_center * FwZ;
        // Shadow ray invariant: this vector points from a receiver toward
        // the sun, exactly like the lighting vector. Cast direction emerges
        // from marching receivers toward the light and finding vertical
        // occluders; do not store an opposite "shadow direction" here.
        s.dir[0] = s.light_dir[0];
        s.dir[1] = s.light_dir[1];
        s.dir[2] = s.light_dir[2];
    }
    // Global debug UI owns the ImGui shell; it will call DrawDebugTab() on
    // this renderer as a contributor.

    Renderer->SetLight(s.light_dir[0], s.light_dir[1], s.light_dir[2], s.intensity, s.color[0], s.color[1], s.color[2], s.ambient);
    Renderer->SetAmbientColor(s.ambient_color[0], s.ambient_color[1], s.ambient_color[2]);
    Renderer->SetAmbientOcclusion(s.ao_enable, s.ao_radius_px, s.ao_strength, s.ao_bias, s.ao_max_dist);
    Renderer->SetShadowWorldDir(s.light_dir[0], s.light_dir[1], s.light_dir[2]);
    Renderer->SetSunShadowRaycast(s.sun_shadow_samples, s.sun_shadow_depth_cutoff,
                                  s.sun_shadow_bias, s.sdir_wz_mul);
    Renderer->SetNormalLightingHardness(s.normal_hardness);
    Renderer->SetNormalRadius(s.normal_radius);
    Renderer->SetEdgeThreshold(s.edge_thr);
    Renderer->SetTileViewMode(s.view_mode);
    Renderer->SetLightingMode(s.lighting_mode);
    Renderer->SetSunShadow(s.sun_shadow, s.sun_shadow_step, s.sun_shadow_soft, s.sun_shadow_max);
    Renderer->SetPerspectiveDebugMode(s.sectorPerspectiveCamera ? s.sectorPerspectiveDebugMode : 0);
    Renderer->SetPerspectiveProjectionMode(s.sectorPerspectiveCamera ? s.sectorPerspectiveProjectionMode : 0);
    Renderer->SetPerspectiveProxyRasterScale(s.sectorPerspectiveCamera ? s.sectorPerspectiveProxyRasterScale : 1.0f);
    Renderer->SetPerspectiveRaycastParams(s.sectorPerspectiveSteps, s.sectorPerspectiveRefine);
    Renderer->SetReconstructionParams(float(cam_ox), float(cam_oy), s.z_near, s.z_far,
                                      float(s.sectorCameraWorld.x), float(s.sectorCameraWorld.y),
                                      camera_forward, s.sectorPerspectiveCamera ? 1.0f : 0.0f,
                                      effective_camera_zoom,
                                      s.sectorPerspectiveZOffset,
                                      s.sectorPerspectiveZScale,
                                      s.sectorPerspectiveTileScale);
    mark_phase(timings.render_state_ms);

    SMapRenderStats stats = {};

    Renderer->ClearPointLights();
    if (s.lights_on)
    {
        S3DPoint vc_rel;
        ScreenToWorld(int32_t(std::lround(float(vw / 2 - cam_ox) / (std::max)(effective_camera_zoom, 0.0001f))),
                      int32_t(std::lround(float(vh / 2 - cam_oy) / (std::max)(effective_camera_zoom, 0.0001f))),
                      vc_rel, 0);
        const S3DPoint vc_w = vc_rel + s.sectorCameraWorld;
        struct Pick { int32_t light_idx; float d2_to_view; };
        Pick picks[TRenderer::kMaxPointLights];
        int32_t pick_n = 0;
        for (int32_t light_idx : s.frameLightIndices)
        {
            if (light_idx < 0 || light_idx >= int32_t(s.sectorLights.size()))
                continue;
            ++stats.point_lights_considered;
            const SSectorLight& L = s.sectorLights[light_idx];
            if (!L.enabled) continue;
            const S3DPoint wp = s.sectorLightPos(L);
            float spx = 0.0f, spy = 0.0f;
            ProjectWorldPointToScreen(projectctx, wp, spx, spy);
            const int32_t sx = int32_t(std::lround(spx)) + cam_ox;
            const int32_t sy = int32_t(std::lround(spy)) + cam_oy;
            const float r_px = s.sectorLightRadius(L) * s.radius_mul * effective_camera_zoom;
            if (sx + r_px < 0 || sx - r_px >= vw) continue;
            if (sy + r_px < 0 || sy - r_px >= vh) continue;
            const float dx = float(wp.x - vc_w.x), dy = float(wp.y - vc_w.y), dz = float(wp.z - vc_w.z);
            const float d2 = dx*dx + dy*dy + dz*dz;
            if (pick_n < TRenderer::kMaxPointLights) picks[pick_n++] = { light_idx, d2 };
            else {
                int32_t worst = 0;
                for (int32_t k = 1; k < pick_n; ++k)
                    if (picks[k].d2_to_view > picks[worst].d2_to_view) worst = k;
                if (d2 < picks[worst].d2_to_view) picks[worst] = { light_idx, d2 };
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
        stats.point_lights_submitted = pick_n;
    }
    mark_phase(timings.point_lights_ms);

    const float zspan = s.z_far - s.z_near;
    Renderer->BeginTilePass(0.12f, 0.16f, 0.10f, 1.0f);
    mark_phase(timings.begin_pass_ms);

    constexpr int32_t kCovCellPx = 32;
    const int32_t cov_cw = (vw + kCovCellPx - 1) / kCovCellPx;
    const int32_t cov_ch = (vh + kCovCellPx - 1) / kCovCellPx;
    const size_t cov_count = size_t(cov_cw) * size_t(cov_ch);
    if (s.coverageScratch.size() != cov_count)
        s.coverageScratch.resize(cov_count);
    std::fill(s.coverageScratch.begin(), s.coverageScratch.end(), uint8_t(0));
    if (Renderer && s.debugGreenTexture == kInvalidTexture)
        s.debugGreenTexture = Renderer->SolidColorTexture(0x4D4150475245454Eull,
                                                          0xFF00FF00u,
                                                          "maprenderer.debug.green");
    SMapRenderContext drawctx = {};
    drawctx.tile_assets = &s.sectorTileTex;
    drawctx.mesh_assets = &s.sectorMeshAsset;
    drawctx.debug_green_texture = s.debugGreenTexture;
    drawctx.sectorCameraWorld = s.sectorCameraWorld;
    drawctx.cam_ox = cam_ox;
    drawctx.cam_oy = cam_oy;
    drawctx.vw = vw;
    drawctx.vh = vh;
    drawctx.cam_forward = camera_forward;
    drawctx.camera_zoom = effective_camera_zoom;
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
    drawctx.cov = &s.coverageScratch;
    ParticleManager().BeginDrawPulsePass();
    for (int32_t idx : s.frameDrawIndices)
    {
        if (idx < 0 || idx >= int32_t(s.sectorDrawInst.size()))
            continue;
        ++stats.draw_candidates;
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
    // Effect-component buckets are drained through the FX submission API
    // (see docs/vfx/PHASE1_SPINE.md). The renderer's DrainFxQueue runs
    // inside fx_pass, immediately after RunLightingPass below.
    {
        TParticleManager& particles = ParticleManager();
        for (int32_t bi = 0; bi < particles.GlobalBucketCount(); ++bi)
        {
            const TParticleBucket* bucket = particles.GlobalBucket(bi);
            if (!bucket || !bucket->Active())
                continue;
            Renderer->SubmitFxParticleBucket(*bucket);
            ++stats.draw_submitted;
        }
    }
    mark_phase(timings.submit_ms);
    // Mirror this frame's counts into the impl so the editor status bar
    // can read them without re-running the render.
    s.last_draw_counts.total_drawables  = int32_t(s.sectorDrawInst.size());
    s.last_draw_counts.draw_candidates  = stats.draw_candidates;
    s.last_draw_counts.resident_lights  = s.residentPointLightCount;
    s.last_draw_counts.active_lights    = int32_t(s.frameLightIndices.size());
    s.last_draw_counts.point_lights_considered = stats.point_lights_considered;
    s.last_draw_counts.point_lights_submitted = stats.point_lights_submitted;
    s.last_draw_counts.tiles_submitted  = stats.draw_submitted;
    s.last_draw_counts.tiles_visible_submitted = stats.draw_visible_submitted;
    s.last_draw_counts.tiles_gbuffer_border_submitted = stats.draw_gbuffer_border_submitted;
    s.last_draw_counts.meshes_submitted = stats.mesh_submitted;
    s.last_draw_counts.offscreen_culled = stats.draw_offscreen;

    Renderer->EndTilePass();
    mark_phase(timings.end_tile_pass_ms);
    Renderer->RunLightingPass();
    mark_phase(timings.lighting_pass_ms);
    finish_timings();

    static int64_t last_spike_frame = -1000000;
    const float spike_ms = timings.total_ms;
    if (spike_ms >= 34.0f && (last_spike_frame < 0 || last_spike_frame + 30 <= TTime::FrameCount()))
    {
        last_spike_frame = TTime::FrameCount();
        const SRendererTilePassStats tile_stats =
            Renderer ? Renderer->GetLastTilePassStats() : SRendererTilePassStats{};
        log_info("[mr-frame] %.2f ms sync=%.2f anim=%.2f refresh=%.2f setup=%.2f zfit=%.2f state=%.2f lights=%.2f begin=%.2f submit=%.2f endtile=%.2f lighting=%.2f records=%zu candidates=%d lights=%d/%d tiles=%d meshes=%d tilepx=%llu/%llu rects=%u/%u proxy=%u hull=%u/%u",
                 timings.total_ms,
                 timings.sync_ms,
                 timings.animate_ms,
                 timings.refresh_ms,
                 timings.setup_ms,
                 timings.depth_fit_ms,
                 timings.render_state_ms,
                 timings.point_lights_ms,
                 timings.begin_pass_ms,
                 timings.submit_ms,
                 timings.end_tile_pass_ms,
                 timings.lighting_pass_ms,
                 s.sectorDrawInst.size(),
                 s.last_draw_counts.draw_candidates,
                 s.last_draw_counts.active_lights,
                 s.last_draw_counts.resident_lights,
                 s.last_draw_counts.tiles_submitted,
                 s.last_draw_counts.meshes_submitted,
                 (unsigned long long)tile_stats.tile_clipped_pixels,
                 (unsigned long long)tile_stats.tile_projected_pixels,
                 tile_stats.tile_draws,
                 tile_stats.tile_rects_culled,
                 tile_stats.tile_proxy_draws,
                 tile_stats.tile_tight_proxy_draws,
                 tile_stats.tile_tight_proxy_points);
    }
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
            const int32_t vw = Display.IsActive() ? Display.Width() : WIDTH;
            const int32_t vh = Display.IsActive() ? Display.Height() : HEIGHT;
            const SMapCameraViewport camera_view = ComputeMapCameraViewport(vw, vh);
            int32_t cam_ox_logical = 0, cam_oy_logical = 0;
            s.sectorCameraOriginScreen(cam_ox_logical, cam_oy_logical);
            const int32_t cam_ox = int32_t(std::lround(camera_view.offset_x + float(cam_ox_logical) * camera_view.scale));
            const int32_t cam_oy = int32_t(std::lround(camera_view.offset_y + float(cam_oy_logical) * camera_view.scale));
            SMapRenderContext pickctx = {};
            pickctx.sectorCameraWorld = s.sectorCameraWorld;
            pickctx.cam_ox = cam_ox;
            pickctx.cam_oy = cam_oy;
            pickctx.vw = vw;
            pickctx.vh = vh;
            pickctx.cam_forward = s.sectorCameraForward(HEIGHT);
            pickctx.camera_zoom = s.sectorCameraZoom * camera_view.scale;
            pickctx.tile_scale = s.sectorPerspectiveTileScale;
            pickctx.perspective_camera = s.sectorPerspectiveCamera;
            float best_d2 = 24.0f * 24.0f;
            for (size_t i = 0; i < s.sectorLights.size(); ++i)
            {
                const SSectorLight& L = s.sectorLights[i];
                if (!L.enabled) continue;
                float spx = 0.0f, spy = 0.0f;
                ProjectWorldPointToScreen(pickctx, s.sectorLightPos(L), spx, spy);
                const float dx = float((int32_t(std::lround(spx)) + cam_ox) - x);
                const float dy = float((int32_t(std::lround(spy)) + cam_oy) - y);
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
            const int32_t vw = Display.IsActive() ? Display.Width() : WIDTH;
            const int32_t vh = Display.IsActive() ? Display.Height() : HEIGHT;
            const SMapCameraViewport camera_view = ComputeMapCameraViewport(vw, vh);
            const float effective_camera_zoom = (std::max)(s.sectorCameraZoom * camera_view.scale, 0.0001f);
            const int32_t dy = y - s.lightDragStartSY;
            newpos.z = s.lightDragStartOiPos.z - int32_t(std::lround(float(dy) / effective_camera_zoom));
        }
        else
        {
            const int32_t vw = Display.IsActive() ? Display.Width() : WIDTH;
            const int32_t vh = Display.IsActive() ? Display.Height() : HEIGHT;
            const SMapCameraViewport camera_view = ComputeMapCameraViewport(vw, vh);
            const float effective_camera_zoom = (std::max)(s.sectorCameraZoom * camera_view.scale, 0.0001f);
            int32_t cam_ox_logical = 0, cam_oy_logical = 0;
            s.sectorCameraOriginScreen(cam_ox_logical, cam_oy_logical);
            const int32_t cam_ox = int32_t(std::lround(camera_view.offset_x + float(cam_ox_logical) * camera_view.scale));
            const int32_t cam_oy = int32_t(std::lround(camera_view.offset_y + float(cam_oy_logical) * camera_view.scale));
            S3DPoint wp_rel;
            ScreenToWorld(int32_t(std::lround(float(x - cam_ox) / effective_camera_zoom)),
                          int32_t(std::lround(float(y - cam_oy) / effective_camera_zoom)),
                          wp_rel,
                          int32_t(s.lightDragStartOiPos.z));
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
    const int32_t vw = Display.IsActive() ? Display.Width() : WIDTH;
    const int32_t vh = Display.IsActive() ? Display.Height() : HEIGHT;
    const SMapCameraViewport camera_view = ComputeMapCameraViewport(vw, vh);
    const float effective_camera_zoom = (std::max)(s.sectorCameraZoom * camera_view.scale, 0.0001f);
    S3DPoint drag_delta_w = {0,0,0};
    ScreenToWorld(int32_t(std::lround(float(x - s.dragStartX) / effective_camera_zoom)),
                  int32_t(std::lround(float(y - s.dragStartY) / effective_camera_zoom)),
                  drag_delta_w,
                  0);
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
