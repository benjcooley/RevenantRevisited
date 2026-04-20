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

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <memory>
#include <string>
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

// --test=sector: upload every OBJCLASS_TILE still bitmap in the loaded level
// and draw them iso-projected each frame. One sg_image per unique bitmap
// (cached by pointer), one SSectorTileInst per instance carrying its screen
// placement + anchor_z. Per-pixel depth in the tile fragment shader lets
// overlapping tiles interpenetrate naturally without painter sort.
struct SSectorTileTex {
    struct SDepthDump {
        uint16_t u16_min = 0xFFFF;
        uint16_t u16_max = 0;
        int32_t  i16_min =  32767;
        int32_t  i16_max = -32768;
        int32_t  sample_count = 0;
        int32_t  high_bit_count = 0;
        int32_t  bbox_x0 =  0x7FFFFFFF;
        int32_t  bbox_y0 =  0x7FFFFFFF;
        int32_t  bbox_x1 = -0x7FFFFFFF;
        int32_t  bbox_y1 = -0x7FFFFFFF;
    };
    void*    bm_key;    // TBitmap* — cache key
    sg_image color;
    sg_image depth;
    int32_t  w, h;
    float    z_local_min;
    float    z_local_max;
    SDepthDump z_dump;
    const char* debug_classname = nullptr;
    S3DPoint    debug_world_pos = {0,0,0};
    int32_t     debug_regx = 0;
    int32_t     debug_regy = 0;
    int32_t     debug_regz = 0;
};
// A tile is a world-positioned billboard: stored in world-space, transformed
// to camera space at draw time, then iso-projected. The renderer subtracts
// the camera's world offset from the tile's world pos, WorldToScreen produces
// screen x/y + iso-z in one go, and the centering offset (ox, oy) keeps the
// initial framing stable.
struct SSectorTileInst {
    int32_t  tex_idx;        // into g_sectorTileTex
    S3DPoint world_pos;      // tile anchor in world units
    int32_t  regx, regy;     // imagery registration offset (screen-pixel)
    int32_t  regz;           // imagery registration offset (screen-z/native)
    bool     has_authored_local_dz = false;
    float    authored_local_dz_min = 0.0f;
    float    authored_local_dz_max = 0.0f;
    int32_t  wwidth = 0, wlength = 0, wheight = 0;  // world bbox dims (wu)
    int32_t  wregx = 0, wregy = 0, wregz = 0;       // world reg (bbox origin)
    TSafeRef<> src;             // mapindex-backed live ref to the source oi
};
std::vector<SSectorTileTex>  g_sectorTileTex;
std::vector<SSectorTileInst> g_sectorTileInst;
std::vector<TSector*> g_sectorsKept;  // kept alive so instance pointers stay valid
// Camera-space depth in raw world units. 1 world unit = 1 cm (so 100 wu =
// 1 m); a 5-sector span is 5*1024 = 5120 wu ≈ 51 m. A typical iso camera
// range covers ~5 m to ~50 m of scene, i.e. z_near ~500, z_far ~5000.
//
// This replaces the old biased iso-z (WORLDZOFFSET-relative ~21000) with
// a clean camera-space forward distance. The sector test now carries a real
// world-space camera (`g_sectorCameraWorld`); `g_sectorWorldCenter` is only
// the initial camera spawn and fixed screen-origin anchor for the test view.
float    g_sectorSceneZMin  = 500.0f;
float    g_sectorSceneZMax  = 5000.0f;
int32_t  g_sectorCenterOx   = 0;        // screen-pixel centering offset
int32_t  g_sectorCenterOy   = 0;        // computed from the loaded bbox.
S3DPoint g_sectorWorldCenter = {0,0,0}; // bbox center of loaded tiles,
                                        // ground plane (z=0).

// Virtual camera's forward offset from the scene anchor, in world units
// (100 wu = 1 m). Retail's iso view is from the NE+above, so points with
// larger wx/wy/wz are CLOSER to the camera; we model that with a virtual
// camera sitting this many wu in front of the scene anchor along the iso
// view axis. Scene_z = forward distance from that camera (near = small,
// far = large), centering a 5-sector load in roughly [500..5000] wu.
constexpr float kIsoCos30     = 867.0f / 1000.0f;
constexpr float kCamForwardWU = 2750.0f;
// Fixed depth window for the game's uniform isometric camera. Once the tile
// BM_ZBUFFER is decoded back to signed local deltas, the visible playfield
// sits comfortably inside this range and 3D/tile interop can share one
// stable mapping instead of re-fitting per sector.
constexpr float kSectorDepthNearWU = -1024.0f;
constexpr float kSectorDepthFarWU  = 16384.0f;

// Camera-space forward depth, in world units, for a world-space delta
// (tile - scene_cam). Matches retail's iso-z convention: larger (wx+wy) or
// wz means closer to camera, so scene_z DECREASES in that direction. With
// kCamForwardWU added, scene_z is always positive for tiles inside the
// frustum, and "near = small, far = large" like a standard z buffer.
inline float CameraDepth(const S3DPoint& rel)
{
    return kCamForwardWU
         - (float(rel.x + rel.y) * kIsoCos30 + float(rel.z) * 0.5f);
}

S3DPoint g_sectorCameraWorld = {0,0,0}; // live camera center in world space
bool     g_sectorShowTileBboxes = false; // overlay every tile instance's dst rect
                                         // (toggle via Sector lighting UI)
bool     g_sectorShowGizmos     = true;  // in-world editor gizmos (light star
                                         // billboards today; add more here)

inline S3DPoint SectorCameraRel(const S3DPoint& world)
{
    return { world.x - g_sectorCameraWorld.x,
             world.y - g_sectorCameraWorld.y,
             world.z - g_sectorCameraWorld.z };
}

inline void SectorProjectWorld(const S3DPoint& world, S3DPoint& screen)
{
    const S3DPoint rel = SectorCameraRel(world);
    WorldToScreen(rel, screen.x, screen.y);
    screen.z = int32_t(CameraDepth(rel));
}

inline float SectorCameraSceneZ(const SSectorTileInst& inst)
{
    return CameraDepth(SectorCameraRel(inst.world_pos));
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

    const float dx[2] = {
        float(-wregx),
        float(wwidth - wregx)
    };
    const float dy[2] = {
        float(-wregy),
        float(wlength - wregy)
    };
    const float dz[2] = {
        float(-wregz),
        float(wheight - wregz)
    };

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

struct SSectorCoord {
    int32_t sx = 0;
    int32_t sy = 0;
};

static std::filesystem::path NormalizeFsPath(const char* path)
{
    std::string s = (path && path[0]) ? path : ".";
    for (char& ch : s)
        if (ch == '\\')
            ch = '/';
    if (s.empty())
        s = ".";
    return std::filesystem::path(s);
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
        if (cand.empty())
            continue;
        if (fs::exists(cand / "imagery.rvi", ec) ||
            fs::exists(cand / "Modules", ec) ||
            fs::is_directory(cand / "Curmap", ec) ||
            fs::is_directory(cand / "curmap", ec))
        {
            return fs::canonical(cand, ec);
        }
    }
    return {};
}

static void GatherLevelSectorCoordsFromDir(const std::filesystem::path& dir,
                                           int32_t level,
                                           std::vector<SSectorCoord>& out)
{
    namespace fs = std::filesystem;
    std::error_code ec;
    if (!fs::is_directory(dir, ec))
        return;

    for (const fs::directory_entry& entry :
         fs::directory_iterator(dir, fs::directory_options::skip_permission_denied, ec))
    {
        if (ec)
            break;
        if (!entry.is_regular_file(ec))
            continue;

        const std::string name = entry.path().filename().string();
        int32_t file_lvl = 0, sx = 0, sy = 0;
        char ext[16] = {};
        if (std::sscanf(name.c_str(), "%d_%d_%d.%15s", &file_lvl, &sx, &sy, ext) != 4)
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
        [](const fs::path& a, const fs::path& b) {
            return a.generic_string() < b.generic_string();
        });
    scan_dirs.erase(std::unique(scan_dirs.begin(), scan_dirs.end(),
        [](const fs::path& a, const fs::path& b) {
            return a.generic_string() == b.generic_string();
        }),
        scan_dirs.end());

    for (const fs::path& dir : scan_dirs)
        GatherLevelSectorCoordsFromDir(dir, level, coords);

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

inline void SectorCameraOriginScreen(int32_t& sx, int32_t& sy)
{
    WorldToScreen(g_sectorWorldCenter, sx, sy);
    sx += g_sectorCenterOx;
    sy += g_sectorCenterOy;
}

// Camera drag state for --test=sector. Left-drag moves the camera in world
// space, so projection, depth, and reconstruction all stay in lockstep.
bool    g_sectorDragging = false;
int32_t g_dragStartX = 0, g_dragStartY = 0;
S3DPoint g_dragCameraStartWorld = {0,0,0};

// Light drag state. Left-click on an OF_LIGHT object's tile icon picks and
// drags the instance itself in the world xy plane; shift+drag moves it in
// world z (positive = up). The light's world pos and the tile render pos
// both derive from oi->Pos() each frame, so there is no snapshot to sync.
int32_t  g_lightDragIdx      = -1;
int32_t  g_lightDragStartSX  = 0;
int32_t  g_lightDragStartSY  = 0;
S3DPoint g_lightDragStartOiPos;

// Sector point lights. Each entry is a TSafeRef to an OF_LIGHT instance
// in one of the kept sectors; position, radius, color and multiplier are
// read back from oi / oi->GetLightDef() each frame. If the instance is
// deleted or paged out, Get() returns nullptr and the helpers no-op —
// nothing here can dangle.
struct SSectorLight {
    TSafeRef<> ref;
    bool       enabled;
};
std::vector<SSectorLight> g_sectorLights;

inline S3DPoint SectorLightPos(const SSectorLight& L)
{
    TObjectInstance* oi = L.ref.Get();
    if (!oi) return { 0, 0, 0 };
    S3DPoint p = oi->Pos();
    if (PSLightDef ld = oi->GetLightDef()) {
        p.x += ld->pos.x; p.y += ld->pos.y; p.z += ld->pos.z;
    }
    return p;
}
inline float SectorLightRadius(const SSectorLight& L)
{
    TObjectInstance* oi = L.ref.Get();
    if (!oi) return 0.0f;
    PSLightDef ld = oi->GetLightDef();
    return ld ? float(ld->intensity) : 0.0f;
}
inline void SectorLightColor(const SSectorLight& L, float rgb[3])
{
    TObjectInstance* oi = L.ref.Get();
    PSLightDef ld = oi ? oi->GetLightDef() : nullptr;
    if (ld) {
        rgb[0] = ld->color.red   / 255.0f;
        rgb[1] = ld->color.green / 255.0f;
        rgb[2] = ld->color.blue  / 255.0f;
    } else {
        rgb[0] = rgb[1] = rgb[2] = 0.0f;
    }
}
inline float SectorLightIntensity(const SSectorLight& L)
{
    // Retail: GetLightBrightness returns BrightnessTable[d] * multiplier /
    // (multiplierscale/2), where multiplierscale = 20, i.e. divide by 10.
    // Default lightdef multiplier = 28 → 2.8× peak (colortable.cpp).
    TObjectInstance* oi = L.ref.Get();
    PSLightDef ld = oi ? oi->GetLightDef() : nullptr;
    if (!ld || ld->multiplier <= 0) return 0.0f;
    return float(ld->multiplier) / 10.0f;
}
inline const char* SectorLightClassName(const SSectorLight& L)
{
    TObjectInstance* oi = L.ref.Get();
    return oi ? oi->GetClassName() : nullptr;
}

// Legacy hello-world globals (used by earlier single-tile test path).
sg_image g_tileColorImg = {};
sg_image g_tileDepthImg = {};
int32_t  g_tileW        = 0;
int32_t  g_tileH        = 0;
constexpr int32_t kTileTexSize = 64;

// Expand an 8/15/16-bit TBitmap into RGBA8 and upload its depth source as an
// R32F texture of per-pixel bitmap-z deltas. Retail's software ZPut path adds
// the source BM_ZBUFFER sample to the draw's `zpos` in a 16-bit register, so
// authored negative deltas naturally arrive as wrapped u16 values. The
// deferred path works in signed camera-space depth instead of modulo-65536
// native z, so we decode each nontransparent source sample back to int16
// before uploading it.
//
// If a bitmap has no BM_ZBUFFER, retail still draws it through ZPut using
// DM_ZSTATIC: every covered pixel writes the anchor depth only. We mirror that
// by uploading a flat zero depth texture and letting alpha/keycolor define the
// covered footprint.
//
// Supports chunked+compressed bitmaps: CacheChunks() decompresses through the
// shared chunk cache and we assemble the flat w*h planes from 64x64 tiles.
bool UploadTileBitmap(PTBitmap bm,
                      sg_image* out_color, sg_image* out_depth,
                      int32_t* out_w, int32_t* out_h,
                      float* out_z_local_min = nullptr,
                      float* out_z_local_max = nullptr,
                      SSectorTileTex::SDepthDump* out_z_dump = nullptr)
{
    if (!bm || bm->width <= 0 || bm->height <= 0) {
        log_warn("[UploadTileBitmap] bad dims bm=%p w=%d h=%d",
            (void*)bm, bm ? bm->width : 0, bm ? bm->height : 0);
        return false;
    }
    const bool is_8bit = (bm->flags & BM_8BIT) != 0;
    const bool is_16bit = (bm->flags & (BM_15BIT | BM_16BIT)) != 0;
    const bool has_zbuffer = (bm->flags & BM_ZBUFFER) != 0;
    if (!is_8bit && !is_16bit) {
        log_warn("[UploadTileBitmap] unsupported format (flags=0x%x w=%d h=%d)",
            bm->flags, bm->width, bm->height);
        return false;
    }
    SPalette* pal = (SPalette*)bm->palette.ptr();
    if (is_8bit && !pal) {
        log_warn("[UploadTileBitmap] no palette (flags=0x%x w=%d h=%d)",
            bm->flags, bm->width, bm->height);
        return false;
    }

    const int32_t w = bm->width, h = bm->height;
    const size_t  npx = size_t(w) * size_t(h);
    const uint8_t key8 = (uint8_t)bm->keycolor;
    const uint16_t key16 = (uint16_t)bm->keycolor;

    std::unique_ptr<uint8_t[]>  idxplane;
    std::unique_ptr<uint16_t[]> rgbplane16;
    if (is_8bit) idxplane.reset(new uint8_t[npx]);
    else         rgbplane16.reset(new uint16_t[npx]);

    std::unique_ptr<uint16_t[]> zplane;
    if (has_zbuffer)
        zplane.reset(new uint16_t[npx]);

    if (bm->flags & BM_CHUNKED)
    {
        if (!bm->CacheChunks()) {
            log_warn("[UploadTileBitmap] CacheChunks failed (flags=0x%x w=%d h=%d)",
                bm->flags, bm->width, bm->height);
            return false;
        }
        SChunkHeader* hdr  = (SChunkHeader*)(void*)bm->data8;
        SChunkHeader* zhdr = has_zbuffer ? (SChunkHeader*)bm->zbuffer.ptr() : nullptr;
        if (!hdr || (has_zbuffer && !zhdr)) {
            log_warn("[UploadTileBitmap] chunked but hdr=%p zhdr=%p (flags=0x%x)",
                (void*)hdr, (void*)zhdr, bm->flags);
            return false;
        }
        const int32_t cw = hdr->width, ch = hdr->height;
        // Prefill with transparent keycolor. Blank blocks (OFFSET==0)
        // legitimately leave their region untouched.
        if (is_8bit)
            std::memset(idxplane.get(), key8, npx);
        else
            std::fill_n(rgbplane16.get(), npx, key16);
        if (has_zbuffer)
            for (size_t i = 0; i < npx; ++i) zplane[i] = 0x7F7F;
        for (int32_t by = 0; by < ch; ++by)
        for (int32_t bx = 0; bx < cw; ++bx)
        {
            void* cptr = hdr->block[by * cw + bx].ptr();
            void* zptr = zhdr->block[by * cw + bx].ptr();
            const uint8_t*  c8  = (is_8bit && cptr)
                ? (const uint8_t*)ChunkCache.AddChunk(cptr, 1) : nullptr;
            const uint16_t* c16 = (is_16bit && cptr)
                ? (const uint16_t*)ChunkCache.AddChunk16(cptr, 1) : nullptr;
            const uint16_t* z16 = (has_zbuffer && zptr)
                ? (const uint16_t*)ChunkCache.AddChunkZ(zptr, 2) : nullptr;
            const int32_t x0 = bx * CHUNKWIDTH, y0 = by * CHUNKHEIGHT;
            const int32_t cxmax = (w - x0 < CHUNKWIDTH)  ? (w - x0) : CHUNKWIDTH;
            const int32_t cymax = (h - y0 < CHUNKHEIGHT) ? (h - y0) : CHUNKHEIGHT;
            if (cxmax <= 0 || cymax <= 0) continue;
            for (int32_t y = 0; y < cymax; ++y)
            {
                if (c8)
                    std::memcpy(&idxplane[(y0 + y) * w + x0],
                                &c8[y * CHUNKWIDTH], cxmax);
                if (c16)
                    std::memcpy(&rgbplane16[(y0 + y) * w + x0],
                                &c16[y * CHUNKWIDTH], cxmax * sizeof(uint16_t));
                if (z16)
                    std::memcpy(&zplane[(y0 + y) * w + x0],
                                &z16[y * CHUNKWIDTH], cxmax * sizeof(uint16_t));
            }
        }
    }
    else
    {
        if (bm->flags & BM_COMPRESSED) {
            log_warn("[UploadTileBitmap] non-chunked BM_COMPRESSED (flags=0x%x w=%d h=%d)",
                bm->flags, bm->width, bm->height);
            return false;
        }
        uint16_t* zbuf = has_zbuffer ? (uint16_t*)bm->zbuffer.ptr() : nullptr;
        if (has_zbuffer && !zbuf) {
            log_warn("[UploadTileBitmap] non-chunked, no zbuffer (flags=0x%x w=%d h=%d)",
                bm->flags, bm->width, bm->height);
            return false;
        }
        if (is_8bit)
            std::memcpy(idxplane.get(), bm->data8, npx);
        else
            std::memcpy(rgbplane16.get(), bm->data16, npx * sizeof(uint16_t));
        if (has_zbuffer)
            std::memcpy(zplane.get(), zbuf, npx * sizeof(uint16_t));
    }

    std::unique_ptr<uint8_t[]> rgba(new uint8_t[npx * 4]);
    std::unique_ptr<float[]>   dflt(new float[npx]);
    float z_local_min = FLT_MAX;
    float z_local_max = -FLT_MAX;
    SSectorTileTex::SDepthDump z_dump = {};
    // Revenant's zbuffered tile renderer keys transparency off BM_ZBUFFER:
    // z == 0x7F7F means "no pixel". When a tile has no BM_ZBUFFER retail
    // falls back to DM_ZSTATIC, so visibility comes from keycolor instead.
    for (size_t i = 0; i < npx; i++)
    {
        const uint8_t idx8 = is_8bit ? idxplane[i] : 0;
        const uint16_t px16 = is_16bit ? rgbplane16[i] : 0;
        const uint16_t z = has_zbuffer ? zplane[i] : 0;
        const bool transparent = has_zbuffer
            ? (z == 0x7F7F)
            : (is_8bit ? (idx8 == key8) : (px16 == key16));
        if (transparent)
        {
            rgba[i*4+0]=rgba[i*4+1]=rgba[i*4+2]=rgba[i*4+3]=0;
        }
        else
        {
            if (is_8bit)
            {
                const uint32_t c = pal->rgbcolors[idx8];
                rgba[i*4+0] = (uint8_t)( c        & 0xFF);
                rgba[i*4+1] = (uint8_t)((c >> 8)  & 0xFF);
                rgba[i*4+2] = (uint8_t)((c >> 16) & 0xFF);
            }
            else
            {
                // Non-paletted retail bitmaps are RGB555 with bit 15 unused,
                // regardless of BM_15BIT vs BM_16BIT flag. Matches the font/UI
                // decoder in font.cpp's BlitBitmap16ToRGBA8.
                rgba[i*4+0] = (uint8_t)(((px16 >> 10) & 0x1F) << 3);
                rgba[i*4+1] = (uint8_t)(((px16 >> 5)  & 0x1F) << 3);
                rgba[i*4+2] = (uint8_t)(( px16        & 0x1F) << 3);
            }
            rgba[i*4+3] = 255;
        }
        // Retail adds BM_ZBUFFER samples to `dp.zpos` in 16-bit arithmetic.
        // Decode the stored u16 back to a signed local delta before moving
        // into our camera-space float depth model; otherwise wrapped negative
        // offsets look like giant far-plane values (e.g. 65520 instead of -16).
        if (!has_zbuffer || transparent)
        {
            dflt[i] = 0.0f;
        }
        else
        {
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
            const int32_t zi16 = int32_t(int16_t(z));
            dflt[i] = z_local;
            if (z_local < z_local_min) z_local_min = z_local;
            if (z_local > z_local_max) z_local_max = z_local;
            if (zi16 < z_dump.i16_min) z_dump.i16_min = zi16;
            if (zi16 > z_dump.i16_max) z_dump.i16_max = zi16;
        }
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

    const sg_resource_state cs = sg_query_image_state(*out_color);
    const sg_resource_state ds = sg_query_image_state(*out_depth);
    if (cs != SG_RESOURCESTATE_VALID || ds != SG_RESOURCESTATE_VALID) {
        log_error("[UploadTileBitmap] sg_make_image state color=%d depth=%d"
                  " (w=%d h=%d 8bit=%d 16bit=%d has_z=%d)",
            (int)cs, (int)ds, w, h, is_8bit?1:0, is_16bit?1:0, has_zbuffer?1:0);
        return false;
    }

    *out_w = w; *out_h = h;
    if (out_z_local_min) *out_z_local_min = z_local_min;
    if (out_z_local_max) *out_z_local_max = z_local_max;
    if (out_z_dump) *out_z_dump = z_dump;
    return true;
}

static void LogSectorTileDepthDump()
{
    struct SAabb3 {
        float minx = FLT_MAX, miny = FLT_MAX, minz = FLT_MAX;
        float maxx = -FLT_MAX, maxy = -FLT_MAX, maxz = -FLT_MAX;
    };
    auto compute_world_aabb = [](const SSectorTileTex& tex, float dz0, float dz1) {
        SAabb3 aabb;
        const float sx[2] = {
            float(tex.z_dump.bbox_x0 - tex.debug_regx),
            float(tex.z_dump.bbox_x1 - tex.debug_regx)
        };
        const float sy[2] = {
            float(tex.z_dump.bbox_y0 - tex.debug_regy),
            float(tex.z_dump.bbox_y1 - tex.debug_regy)
        };
        const float dd[2] = { dz0, dz1 };
        constexpr float kIsoWzDenom = 2.0f * kIsoCos30 * kIsoCos30 + 0.5f;
        for (int ix = 0; ix < 2; ++ix)
        for (int iy = 0; iy < 2; ++iy)
        for (int iz = 0; iz < 2; ++iz)
        {
            // The bitmap footprint is in screen space. BM_ZBUFFER contributes
            // a local camera-depth/native-z delta, not a literal world-z
            // displacement, so recover the local world delta by inverting the
            // iso projection from (dS, dT, dSceneZ).
            const float S = sx[ix];
            const float T = sy[iy];
            const float scene_dz = dd[iz];
            const float lz = (-scene_dz - 2.0f * T * kIsoCos30) / kIsoWzDenom;
            const float sum = 2.0f * (T + lz * kIsoCos30);
            const float lx = 0.5f * (sum + S);
            const float ly = 0.5f * (sum - S);
            const float wx = float(tex.debug_world_pos.x) + lx;
            const float wy = float(tex.debug_world_pos.y) + ly;
            const float wz = float(tex.debug_world_pos.z) + lz;
            if (wx < aabb.minx) aabb.minx = wx;
            if (wy < aabb.miny) aabb.miny = wy;
            if (wz < aabb.minz) aabb.minz = wz;
            if (wx > aabb.maxx) aabb.maxx = wx;
            if (wy > aabb.maxy) aabb.maxy = wy;
            if (wz > aabb.maxz) aabb.maxz = wz;
        }
        return aabb;
    };

    if (g_sectorTileTex.empty()) return;

    uint16_t agg_u16_min = 0xFFFF;
    uint16_t agg_u16_max = 0;
    int32_t  agg_i16_min =  32767;
    int32_t  agg_i16_max = -32768;
    int64_t  total_samples = 0;
    int64_t  total_high_bit = 0;
    int32_t  textures_with_high_bit = 0;
    std::vector<size_t> wrapped;
    std::vector<size_t> widest_signed;

    for (size_t i = 0; i < g_sectorTileTex.size(); ++i)
    {
        const auto& tex = g_sectorTileTex[i];
        if (tex.z_dump.sample_count <= 0)
            continue;
        widest_signed.push_back(i);
        total_samples += tex.z_dump.sample_count;
        total_high_bit += tex.z_dump.high_bit_count;
        if (tex.z_dump.u16_min < agg_u16_min) agg_u16_min = tex.z_dump.u16_min;
        if (tex.z_dump.u16_max > agg_u16_max) agg_u16_max = tex.z_dump.u16_max;
        if (tex.z_dump.i16_min < agg_i16_min) agg_i16_min = tex.z_dump.i16_min;
        if (tex.z_dump.i16_max > agg_i16_max) agg_i16_max = tex.z_dump.i16_max;
        if (tex.z_dump.high_bit_count > 0) {
            ++textures_with_high_bit;
            wrapped.push_back(i);
        }
    }

    if (total_samples == 0) {
        log_info("[sector] zfmt: no valid BM_ZBUFFER samples across %zu textures",
                 g_sectorTileTex.size());
        return;
    }

    std::sort(wrapped.begin(), wrapped.end(),
        [](size_t a, size_t b) {
            const auto& A = g_sectorTileTex[a];
            const auto& B = g_sectorTileTex[b];
            const double af = double(A.z_dump.high_bit_count) / double(A.z_dump.sample_count);
            const double bf = double(B.z_dump.high_bit_count) / double(B.z_dump.sample_count);
            if (af != bf) return af > bf;
            return A.z_dump.sample_count > B.z_dump.sample_count;
        });
    std::sort(widest_signed.begin(), widest_signed.end(),
        [](size_t a, size_t b) {
            const auto& A = g_sectorTileTex[a];
            const auto& B = g_sectorTileTex[b];
            const int32_t aspan = A.z_dump.i16_max - A.z_dump.i16_min;
            const int32_t bspan = B.z_dump.i16_max - B.z_dump.i16_min;
            if (aspan != bspan) return aspan > bspan;
            if (A.z_dump.i16_max != B.z_dump.i16_max) return A.z_dump.i16_max > B.z_dump.i16_max;
            return A.z_dump.sample_count > B.z_dump.sample_count;
        });

    log_info("[sector] zfmt: textures=%zu samples=%lld"
             " u16=[%u..%u] i16=[%d..%d] high-bit=%lld (%0.2f%%) textures_with_high=%d",
             g_sectorTileTex.size(),
             (long long)total_samples,
             unsigned(agg_u16_min), unsigned(agg_u16_max),
             agg_i16_min, agg_i16_max,
             (long long)total_high_bit,
             total_samples > 0 ? (100.0 * double(total_high_bit) / double(total_samples)) : 0.0,
             textures_with_high_bit);

    const size_t dump_n = std::min<size_t>(wrapped.size(), 8);
    for (size_t n = 0; n < dump_n; ++n)
    {
        const auto& tex = g_sectorTileTex[wrapped[n]];
        const double frac = 100.0 * double(tex.z_dump.high_bit_count) / double(tex.z_dump.sample_count);
        const float signed_dz0 = float(tex.z_dump.i16_min - tex.debug_regz);
        const float signed_dz1 = float(tex.z_dump.i16_max - tex.debug_regz);
        const float unsigned_dz0 = float(int32_t(tex.z_dump.u16_min) - tex.debug_regz);
        const float unsigned_dz1 = float(int32_t(tex.z_dump.u16_max) - tex.debug_regz);
        const SAabb3 signed_aabb = compute_world_aabb(tex, signed_dz0, signed_dz1);
        const SAabb3 unsigned_aabb = compute_world_aabb(tex, unsigned_dz0, unsigned_dz1);
        log_info("[sector] zfmt[%zu]: bm=%p %dx%d bbox=[%d,%d..%d,%d) class=%s pos=(%d,%d,%d)"
                 " reg=(%d,%d,%d) samples=%d high=%d (%0.2f%%)"
                 " u16=[%u..%u] i16=[%d..%d]",
                 wrapped[n], tex.bm_key, tex.w, tex.h,
                 tex.z_dump.bbox_x0, tex.z_dump.bbox_y0,
                 tex.z_dump.bbox_x1, tex.z_dump.bbox_y1,
                 tex.debug_classname ? tex.debug_classname : "?",
                 tex.debug_world_pos.x, tex.debug_world_pos.y, tex.debug_world_pos.z,
                 tex.debug_regx, tex.debug_regy, tex.debug_regz,
                 tex.z_dump.sample_count, tex.z_dump.high_bit_count, frac,
                 unsigned(tex.z_dump.u16_min), unsigned(tex.z_dump.u16_max),
                 tex.z_dump.i16_min, tex.z_dump.i16_max);
        log_info("[sector] zfmt[%zu]: local_dz int16=[%.0f..%.0f] uint16=[%.0f..%.0f]",
                 wrapped[n], signed_dz0, signed_dz1, unsigned_dz0, unsigned_dz1);
        log_info("[sector] zfmt[%zu]: world_bbox int16=[%.0f,%.0f,%.0f..%.0f,%.0f,%.0f]"
                 " uint16=[%.0f,%.0f,%.0f..%.0f,%.0f,%.0f]",
                 wrapped[n],
                 signed_aabb.minx, signed_aabb.miny, signed_aabb.minz,
                 signed_aabb.maxx, signed_aabb.maxy, signed_aabb.maxz,
                 unsigned_aabb.minx, unsigned_aabb.miny, unsigned_aabb.minz,
                 unsigned_aabb.maxx, unsigned_aabb.maxy, unsigned_aabb.maxz);
    }

    const size_t span_n = std::min<size_t>(widest_signed.size(), 5);
    for (size_t n = 0; n < span_n; ++n)
    {
        const auto& tex = g_sectorTileTex[widest_signed[n]];
        const float signed_dz0 = float(tex.z_dump.i16_min - tex.debug_regz);
        const float signed_dz1 = float(tex.z_dump.i16_max - tex.debug_regz);
        const SAabb3 signed_aabb = compute_world_aabb(tex, signed_dz0, signed_dz1);
        log_info("[sector] zspan[%zu]: bm=%p %dx%d class=%s pos=(%d,%d,%d) reg=(%d,%d,%d)"
                 " bbox=[%d,%d..%d,%d) i16=[%d..%d] local_dz=[%.0f..%.0f] span=%d",
                 widest_signed[n], tex.bm_key, tex.w, tex.h,
                 tex.debug_classname ? tex.debug_classname : "?",
                 tex.debug_world_pos.x, tex.debug_world_pos.y, tex.debug_world_pos.z,
                 tex.debug_regx, tex.debug_regy, tex.debug_regz,
                 tex.z_dump.bbox_x0, tex.z_dump.bbox_y0,
                 tex.z_dump.bbox_x1, tex.z_dump.bbox_y1,
                 tex.z_dump.i16_min, tex.z_dump.i16_max,
                 signed_dz0, signed_dz1,
                 tex.z_dump.i16_max - tex.z_dump.i16_min);
        log_info("[sector] zspan[%zu]: world_bbox int16=[%.0f,%.0f,%.0f..%.0f,%.0f,%.0f]",
                 widest_signed[n],
                 signed_aabb.minx, signed_aabb.miny, signed_aabb.minz,
                 signed_aabb.maxx, signed_aabb.maxy, signed_aabb.maxz);
    }
}

inline void Decode555(uint16_t px, uint8_t* rgba_out)
{
    rgba_out[0] = (uint8_t)(((px >> 10) & 0x1F) << 3);
    rgba_out[1] = (uint8_t)(((px >> 5)  & 0x1F) << 3);
    rgba_out[2] = (uint8_t)(( px        & 0x1F) << 3);
    rgba_out[3] = 255;
}

inline void Decode565(uint16_t px, uint8_t* rgba_out)
{
    rgba_out[0] = (uint8_t)(((px >> 11) & 0x1F) << 3);
    rgba_out[1] = (uint8_t)(((px >> 5)  & 0x3F) << 2);
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
                else                           { Decode555(px, row); }
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
        // Load the whole level that contains --sector=L_X_Y (default
        // 0_2_25, Misthaven). The editor doesn't reliably re-bin moved
        // objects, so drawing from a local sector neighborhood can miss
        // tiles that visually belong in the current view but are stored in
        // some other sector file entirely. The per-frame screen cull is now
        // cheap enough that loading the whole level is the simplest way to
        // eliminate "was never loaded" as an explanation for missing tiles.
        int32_t keep_lvl = 0, keep_sx = 2, keep_sy = 25;
        if (StartupSectorId[0])
        {
            int32_t l=0, x=0, y=0;
            if (std::sscanf(StartupSectorId, "%d_%d_%d", &l, &x, &y) == 3)
            {
                keep_lvl = l; keep_sx = x; keep_sy = y;
            }
            else
            {
                log_warn("[sector] bad --sector='%s', expected L_X_Y", StartupSectorId);
            }
        }

        // Reset LoadObject null-bucket counters so the sector scan only sees
        // drops from sector files we load right below.
        extern int32_t g_loadObjNullObjVerNeg, g_loadObjNullClassNeg,
                       g_loadObjNullBadClass,  g_loadObjNullBadType,
                       g_loadObjNullNewObjFail, g_loadObjNullCorruptDrop,
                       g_loadObjOk;
        g_loadObjNullObjVerNeg = g_loadObjNullClassNeg = g_loadObjNullBadClass =
        g_loadObjNullBadType = g_loadObjNullNewObjFail = g_loadObjNullCorruptDrop =
        g_loadObjOk = 0;

        struct SLoaded { int32_t lvl, sx, sy; TSector* sec; };
        std::vector<SLoaded> loaded;
        std::vector<SSectorCoord> load_coords = FindLevelSectorCoords(keep_lvl);
        if (load_coords.empty())
        {
            log_warn("[sector] no level-wide sector scan for level %d; falling back to 5x5 around %d_%d",
                keep_lvl, keep_sx, keep_sy);
            for (int32_t dy = -2; dy <= 2; dy++)
            for (int32_t dx = -2; dx <= 2; dx++)
                load_coords.push_back({ keep_sx + dx, keep_sy + dy });
        }
        else
        {
            int32_t sx_min = load_coords.front().sx;
            int32_t sx_max = load_coords.front().sx;
            int32_t sy_min = load_coords.front().sy;
            int32_t sy_max = load_coords.front().sy;
            for (const SSectorCoord& c : load_coords)
            {
                if (c.sx < sx_min) sx_min = c.sx;
                if (c.sx > sx_max) sx_max = c.sx;
                if (c.sy < sy_min) sy_min = c.sy;
                if (c.sy > sy_max) sy_max = c.sy;
            }
            log_info("[sector] level %d scan: %zu sectors x=[%d..%d] y=[%d..%d] focus=%d_%d",
                keep_lvl, load_coords.size(), sx_min, sx_max, sy_min, sy_max, keep_sx, keep_sy);
        }

        for (const SSectorCoord& coord : load_coords)
        {
            const int32_t sx = coord.sx;
            const int32_t sy = coord.sy;
            log_info("[sector] -> loading %d_%d_%d", keep_lvl, sx, sy);
            const int32_t pre_ok       = g_loadObjOk;
            const int32_t pre_objver   = g_loadObjNullObjVerNeg;
            const int32_t pre_classneg = g_loadObjNullClassNeg;
            const int32_t pre_badclass = g_loadObjNullBadClass;
            const int32_t pre_badtype  = g_loadObjNullBadType;
            const int32_t pre_newfail  = g_loadObjNullNewObjFail;
            const int32_t pre_corrupt  = g_loadObjNullCorruptDrop;
            TSector* sec = TSector::LoadSector(keep_lvl, sx, sy, false);
            if (!sec)
            {
                log_warn("[sector] %d_%d_%d: LoadSector failed", keep_lvl, sx, sy);
                continue;
            }
            const int32_t d_ok       = g_loadObjOk              - pre_ok;
            const int32_t d_objver   = g_loadObjNullObjVerNeg   - pre_objver;
            const int32_t d_classneg = g_loadObjNullClassNeg    - pre_classneg;
            const int32_t d_badclass = g_loadObjNullBadClass    - pre_badclass;
            const int32_t d_badtype  = g_loadObjNullBadType     - pre_badtype;
            const int32_t d_newfail  = g_loadObjNullNewObjFail  - pre_newfail;
            const int32_t d_corrupt  = g_loadObjNullCorruptDrop - pre_corrupt;
            log_info("[sector]   %d_%d_%d buckets: ok=%d objver_neg=%d class_neg=%d"
                     " bad_class=%d bad_type=%d new_fail=%d corrupt=%d",
                keep_lvl, sx, sy, d_ok, d_objver, d_classneg,
                d_badclass, d_badtype, d_newfail, d_corrupt);
            log_info("[sector] <- %d_%d_%d: %d objects (lights=%d anim=%d)",
                keep_lvl, sx, sy, sec->NumItems(),
                sec->NumObjSetItems(OBJSET_LIGHTS),
                sec->NumObjSetItems(OBJSET_ANIMATE));
            loaded.push_back({ keep_lvl, sx, sy, sec });
            g_sectorsKept.push_back(sec);
        }

        if (!loaded.empty())
        {
            // Pass 1: walk every tile in every loaded sector, dedup bitmaps
            // globally (the same bm can appear in multiple sectors). regx/regy
            // live on the imagery state — TAnimImagery::DrawUnlit subtracts
            // them from the iso-projected screen pos. Track iso-z range for
            // anchor_z normalisation.
            struct WorldInst {
                int32_t tex_idx;
                S3DPoint wpos;
                int32_t regx, regy, regz;
                bool    has_authored_local_dz;
                float   authored_local_dz_min;
                float   authored_local_dz_max;
                int32_t wwidth, wlength, wheight;
                int32_t wregx, wregy, wregz;
                TObjectInstance* oi;
            };
            std::vector<WorldInst> work;
            int32_t total_tiles = 0, no_img = 0, no_body = 0,
                    bad_state = 0, no_still = 0, upload_fail = 0,
                    non_2d = 0;
            int32_t tiles_drawflip = 0, tiles_invisible = 0,
                    tiles_seldraw = 0, tiles_nowalk = 0;
            int32_t numstates_hist[8] = {};
            int32_t state_hist[8] = {};
            // Diagnostic: of every non-TILE object in the loaded sectors, how
            // many would pass the same "has valid OBJIMAGE_ANIMATION still"
            // check we apply to tiles? Those are the 2D sprite props (floor
            // planks, scenery, shadows, etc.) retail also blits but that this
            // filter currently discards — making the map look incomplete.
            constexpr int32_t kNumObjClasses = OBJCLASS_EFFECT + 1;
            int32_t nontile_class_total[kNumObjClasses]      = {};
            int32_t nontile_class_with_anim[kNumObjClasses]  = {};
            int32_t nontile_class_with_still[kNumObjClasses] = {};
            int32_t scan_slot_total = 0, scan_slot_null = 0;
            int32_t scan_class_oor = 0; // class ID outside the known enum
            for (auto& L : loaded)
            {
                TSector* sec = L.sec;
                const int32_t sec_lvl = L.lvl, sec_sx = L.sx, sec_sy = L.sy;
                scan_slot_total += sec->NumItems();
                for (int32_t i = 0; i < sec->NumItems(); i++)
                {
                    TObjectInstance* oi = sec->GetInstance(i);
                    if (!oi) { scan_slot_null++; continue; }
                    if (oi->ObjClass() != OBJCLASS_TILE) {
                        if ((uint32_t)oi->ObjClass() >= (uint32_t)kNumObjClasses)
                            scan_class_oor++;
                        const int32_t cls = oi->ObjClass();
                        if ((uint32_t)cls < (uint32_t)kNumObjClasses) {
                            nontile_class_total[cls]++;
                            if (TObjectImagery* xi = oi->GetImagery()) {
                                SImageryHeader* xh = xi->GetHeader();
                                SImageryBody*   xb = xi->GetBody();
                                if (xh && xb && xh->imageryid == OBJIMAGE_ANIMATION) {
                                    nontile_class_with_anim[cls]++;
                                    const int32_t xs = oi->GetState();
                                    if (xs >= 0 && xs < xh->numstates) {
                                        auto* xab = (SAnimImageryBody*)xb;
                                        if (xab->states[xs].still)
                                            nontile_class_with_still[cls]++;
                                    }
                                }
                            }
                        }
                        continue;
                    }
                    total_tiles++;
                    {
                        const uint32_t fl = oi->GetFlags();
                        if (fl & OF_DRAWFLIP)  tiles_drawflip++;
                        if (fl & OF_INVISIBLE) tiles_invisible++;
                        if (fl & OF_SELDRAW)   tiles_seldraw++;
                        if (fl & OF_NOWALK)    tiles_nowalk++;
                    }
                    TObjectImagery* img = oi->GetImagery();
                    if (!img) { no_img++;
                        log_warn("[sector]   drop tile[%d] %s: no imagery",
                            i, oi->GetClassName() ? oi->GetClassName() : "?");
                        continue;
                    }
                    SImageryHeader* hdr = img->GetHeader();
                    SImageryBody*   body = img->GetBody();
                    if (!hdr || !body) { no_body++;
                        log_warn("[sector]   drop tile[%d] %s: hdr=%p body=%p",
                            i, oi->GetClassName() ? oi->GetClassName() : "?",
                            (void*)hdr, (void*)body);
                        continue;
                    }
                    // Only OBJIMAGE_ANIMATION tiles have SAnimImageryBody layout
                    // with per-state `still` bitmaps. I3D/multi tiles (Barrel,
                    // Crate, etc.) live on the unported 3D path — skip here
                    // rather than misinterpret their body bytes as still ptrs.
                    if (hdr->imageryid != OBJIMAGE_ANIMATION) {
                        non_2d++;
                        continue;
                    }
                    auto* ab = (SAnimImageryBody*)body;
                    const int32_t st = oi->GetState();
                    {
                        const int32_t ns = hdr->numstates;
                        const int32_t nb = ns < 0 ? 0 : (ns >= 8 ? 7 : ns);
                        numstates_hist[nb]++;
                        const int32_t sb = st < 0 ? 0 : (st >= 8 ? 7 : st);
                        state_hist[sb]++;
                    }
                    if (st < 0 || st >= hdr->numstates) { bad_state++;
                        log_warn("[sector]   drop tile[%d] %s: state=%d numstates=%d"
                                 " file=%s",
                            i, oi->GetClassName() ? oi->GetClassName() : "?",
                            st, hdr->numstates,
                            img->GetResFilename() ? img->GetResFilename() : "?");
                        continue;
                    }
                    PTBitmap bm = (TBitmap*)ab->states[st].still;
                    if (!bm) { no_still++;
                        log_warn("[sector]   drop tile[%d] %s: no still for state %d"
                                 " file=%s",
                            i, oi->GetClassName() ? oi->GetClassName() : "?", st,
                            img->GetResFilename() ? img->GetResFilename() : "?");
                        continue;
                    }

                    int32_t tex_idx = -1;
                    for (size_t t = 0; t < g_sectorTileTex.size(); t++)
                        if (g_sectorTileTex[t].bm_key == bm) { tex_idx = (int32_t)t; break; }
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
                                              &t.z_dump))
                        { upload_fail++;
                            S3DPoint p = oi->Pos();
                            log_warn("[sector]   drop tile[%d] %s pos=(%d,%d,%d)"
                                     " here_sec=%d_%d_%d UploadTileBitmap failed",
                                i, oi->GetClassName() ? oi->GetClassName() : "?",
                                p.x, p.y, p.z,
                                sec_lvl, p.x >> SECTORWSHIFT, p.y >> SECTORHSHIFT);
                            continue;
                        }
                        tex_idx = (int32_t)g_sectorTileTex.size();
                        g_sectorTileTex.push_back(t);
                    }
                    work.push_back({ tex_idx, oi->Pos(),
                                     img->GetRegX(st), img->GetRegY(st),
                                     img->GetRegZ(st),
                                     false, 0.0f, 0.0f,
                                     0, 0, 0, 0, 0, 0, oi });
                    WorldInst& winst = work.back();
                    int32_t wwidth = 0, wlength = 0, wheight = 0;
                    img->GetWorldBoundBox(st, wwidth, wlength, wheight);
                    winst.wwidth  = wwidth;
                    winst.wlength = wlength;
                    winst.wheight = wheight;
                    winst.wregx   = img->GetWorldRegX(st);
                    winst.wregy   = img->GetWorldRegY(st);
                    winst.wregz   = img->GetWorldRegZ(st);
                    ComputeAuthoredLocalDepthRange(wwidth, wlength, wheight,
                                                   winst.wregx, winst.wregy, winst.wregz,
                                                   winst.has_authored_local_dz,
                                                   winst.authored_local_dz_min,
                                                   winst.authored_local_dz_max);
                }
            }
            log_info("[sector] tile scan: total=%d kept=%zu non_2d=%d"
                     " drops: no_img=%d no_body=%d bad_state=%d"
                     " no_still=%d upload_fail=%d",
                total_tiles, work.size(), non_2d, no_img, no_body, bad_state,
                no_still, upload_fail);
            log_info("[sector] tile flags: drawflip=%d invisible=%d seldraw=%d nowalk=%d",
                tiles_drawflip, tiles_invisible, tiles_seldraw, tiles_nowalk);
            log_info("[sector] scan slots: total=%d null=%d class_oor=%d accounted=%d",
                scan_slot_total, scan_slot_null, scan_class_oor,
                scan_slot_total - scan_slot_null);
            log_info("[sector] LoadObject buckets: ok=%d objver_neg=%d class_neg=%d"
                     " bad_class=%d bad_type=%d new_obj_fail=%d corrupt_drop=%d",
                g_loadObjOk, g_loadObjNullObjVerNeg, g_loadObjNullClassNeg,
                g_loadObjNullBadClass, g_loadObjNullBadType,
                g_loadObjNullNewObjFail, g_loadObjNullCorruptDrop);
            {
                static const char* kClassNames[kNumObjClasses] = {
                    "ITEM","WEAPON","ARMOR","TALISMAN","FOOD","CONTAINER",
                    "LIGHTSOURCE","TOOL","MONEY","TILE","EXIT","PLAYER",
                    "CHARACTER","TRAP","SHADOW","HELPER","KEY","UNUSED1",
                    "UNUSED2","UNUSED3","UNUSED4","AMMO","SCROLL","RANGEDWEAPON",
                    "UNUSED5","EFFECT",
                };
                for (int32_t c = 0; c < kNumObjClasses; ++c) {
                    if (nontile_class_total[c] == 0) continue;
                    log_info("[sector] non-tile class %-12s total=%d anim=%d with_still=%d",
                        kClassNames[c], nontile_class_total[c],
                        nontile_class_with_anim[c], nontile_class_with_still[c]);
                }
            }
            log_info("[sector] numstates hist: 0=%d 1=%d 2=%d 3=%d 4=%d 5=%d 6=%d 7+=%d",
                numstates_hist[0], numstates_hist[1], numstates_hist[2], numstates_hist[3],
                numstates_hist[4], numstates_hist[5], numstates_hist[6], numstates_hist[7]);
            log_info("[sector] state hist: 0=%d 1=%d 2=%d 3=%d 4=%d 5=%d 6=%d 7+=%d",
                state_hist[0], state_hist[1], state_hist[2], state_hist[3],
                state_hist[4], state_hist[5], state_hist[6], state_hist[7]);
            LogSectorTileDepthDump();

            // Pass 2: project every tile so we can derive an initial camera
            // framing from the requested startup sector neighborhood while
            // still keeping the entire level resident. That preserves the old
            // local view instead of centering the whole map on screen.
            const int32_t focus_wx0 = (keep_sx - 1) * SECTORWIDTH;
            const int32_t focus_wx1 = (keep_sx + 2) * SECTORWIDTH;
            const int32_t focus_wy0 = (keep_sy - 1) * SECTORHEIGHT;
            const int32_t focus_wy1 = (keep_sy + 2) * SECTORHEIGHT;

            struct SViewBounds {
                int32_t sx_min = INT32_MAX, sx_max = INT32_MIN;
                int32_t sy_min = INT32_MAX, sy_max = INT32_MIN;
                int32_t wx_min = INT32_MAX, wx_max = INT32_MIN;
                int32_t wy_min = INT32_MAX, wy_max = INT32_MIN;
                int32_t count = 0;
            } focus_bounds, all_bounds;

            auto accumulate_bounds =
                [](SViewBounds& b, const S3DPoint& wpos, const S3DPoint& s,
                   int32_t regx, int32_t regy, int32_t tex_w, int32_t tex_h)
            {
                const int32_t x0 = s.x - regx;
                const int32_t y0 = s.y - regy;
                const int32_t x1 = x0 + tex_w;
                const int32_t y1 = y0 + tex_h;
                if (x0 < b.sx_min) b.sx_min = x0;
                if (y0 < b.sy_min) b.sy_min = y0;
                if (x1 > b.sx_max) b.sx_max = x1;
                if (y1 > b.sy_max) b.sy_max = y1;
                if (wpos.x < b.wx_min) b.wx_min = wpos.x;
                if (wpos.x > b.wx_max) b.wx_max = wpos.x;
                if (wpos.y < b.wy_min) b.wy_min = wpos.y;
                if (wpos.y > b.wy_max) b.wy_max = wpos.y;
                ++b.count;
            };

            for (auto& w : work)
            {
                S3DPoint s;
                WorldToScreen(w.wpos, s);
                const auto& tex = g_sectorTileTex[w.tex_idx];
                accumulate_bounds(all_bounds, w.wpos, s, w.regx, w.regy, tex.w, tex.h);
                if (w.wpos.x >= focus_wx0 && w.wpos.x < focus_wx1 &&
                    w.wpos.y >= focus_wy0 && w.wpos.y < focus_wy1)
                {
                    accumulate_bounds(focus_bounds, w.wpos, s, w.regx, w.regy, tex.w, tex.h);
                }
            }

            const SViewBounds& view_bounds = focus_bounds.count > 0 ? focus_bounds : all_bounds;

            const int32_t tw = Display ? Display->Width()  : 1024;
            const int32_t th = Display ? Display->Height() : 768;
            const int32_t bw = view_bounds.sx_max - view_bounds.sx_min;
            const int32_t bh = view_bounds.sy_max - view_bounds.sy_min;
            const int32_t ox = (tw - bw) / 2 - view_bounds.sx_min;
            const int32_t oy = (th - bh) / 2 - view_bounds.sy_min;
            g_sectorCenterOx  = ox;
            g_sectorCenterOy  = oy;

            // Scene anchor: focus bbox center at ground plane (z=0). This is
            // the initial camera position and fixed screen-origin anchor for
            // the test harness; the whole level is loaded, but startup view
            // stays around the requested sector instead of the full level.
            g_sectorWorldCenter.x = (view_bounds.wx_min + view_bounds.wx_max) / 2;
            g_sectorWorldCenter.y = (view_bounds.wy_min + view_bounds.wy_max) / 2;
            g_sectorWorldCenter.z = 0;
            g_sectorCameraWorld   = g_sectorWorldCenter;

            // Camera-space scene-z bounds (world units). Match retail's
            // software blit semantics: final per-fragment depth is the draw's
            // anchor z (`screenpos.z - regz`) plus the bitmap's local z
            // sample. Since CameraDepth is an affine transform of screen z,
            // the full fragment range is:
            //   CameraDepth(wpos - anchor) - regz + local_z
            // for each tile's authored local z extrema.
            float sz_min = FLT_MAX, sz_max = -FLT_MAX;
            for (const auto& w : work)
            {
                const auto& tex = g_sectorTileTex[w.tex_idx];
                const float camera_z = CameraDepth(w.wpos - g_sectorCameraWorld);
                const float z0 = w.has_authored_local_dz
                    ? (camera_z + w.authored_local_dz_min)
                    : (camera_z + (tex.z_local_min - float(w.regz)));
                const float z1 = w.has_authored_local_dz
                    ? (camera_z + w.authored_local_dz_max)
                    : (camera_z + (tex.z_local_max - float(w.regz)));
                if (z0 < sz_min) sz_min = z0;
                if (z1 > sz_max) sz_max = z1;
            }
            g_sectorSceneZMin = sz_min;
            g_sectorSceneZMax = sz_max;

            for (const auto& w : work)
            {
                SSectorTileInst inst = {};
                inst.tex_idx   = w.tex_idx;
                inst.world_pos = w.wpos;
                inst.regx      = w.regx;
                inst.regy      = w.regy;
                inst.regz      = w.regz;
                inst.has_authored_local_dz = w.has_authored_local_dz;
                inst.authored_local_dz_min = w.authored_local_dz_min;
                inst.authored_local_dz_max = w.authored_local_dz_max;
                inst.wwidth  = w.wwidth;
                inst.wlength = w.wlength;
                inst.wheight = w.wheight;
                inst.wregx   = w.wregx;
                inst.wregy   = w.wregy;
                inst.wregz   = w.wregz;
                inst.src     = w.oi;
                g_sectorTileInst.push_back(inst);
            }

            log_info("[sector] built tile list: %zu instances, %zu unique bitmaps",
                g_sectorTileInst.size(), g_sectorTileTex.size());
            log_info("[sector]   startup focus bbox=%dx%d (%d tiles) scene_z=[%.0f..%.0f] wu  anchor=(%d,%d,%d)",
                bw, bh, view_bounds.count, sz_min, sz_max,
                g_sectorWorldCenter.x, g_sectorWorldCenter.y, g_sectorWorldCenter.z);

            // Pass 3: bind every OF_LIGHT object in every loaded sector as a
            // live light reference. Position, radius, color and multiplier
            // are read from oi / oi->GetLightDef() each frame, so the lights
            // track whatever the scene does (drag, script, save-load) — no
            // snapshot to keep in sync.
            for (auto& Ls : loaded)
            {
                TSector* sec = Ls.sec;
                for (int32_t i = 0; i < sec->NumItems(); i++)
                {
                    TObjectInstance* oi = sec->GetInstance(i);
                    if (!oi || !oi->IsLight()) continue;
                    PSLightDef ld = oi->GetLightDef();
                    if (!ld || ld->intensity == 0) continue;
                    g_sectorLights.push_back({ TSafeRef<>(oi), true });
                }
            }
            {
                float rmin = FLT_MAX, rmax = 0.0f, rsum = 0.0f;
                float imin = FLT_MAX, imax = 0.0f;
                for (const auto& L : g_sectorLights) {
                    const float r = SectorLightRadius(L);
                    const float m = SectorLightIntensity(L);
                    rmin = fminf(rmin, r);
                    rmax = fmaxf(rmax, r);
                    rsum += r;
                    imin = fminf(imin, m);
                    imax = fmaxf(imax, m);
                }
                const float rmean = g_sectorLights.empty() ? 0.0f
                    : rsum / float(g_sectorLights.size());
                log_info("[sector] collected %zu point lights  radius=[%.0f..%.0f avg %.0f] intensity=[%.2f..%.2f]",
                    g_sectorLights.size(),
                    g_sectorLights.empty() ? 0.0f : rmin, rmax, rmean,
                    g_sectorLights.empty() ? 0.0f : imin, imax);
            }
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
    for (TSector* s : g_sectorsKept) if (s) TSector::CloseSector(s);
    g_sectorsKept.clear();
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
        static float dir[3]       = { 0.28f, -0.025f, 0.431f };
        static float intensity    = 1.4f;
        static float color[3]     = { 1.0f, 1.0f, 1.0f };
        static float ambient      = 0.15f;
        // Stopgap tuning (proper fix is world-space geometry in camera space,
        // since screen-space derivatives of a quantised per-pixel depthmap
        // can't give correct normals across tile seams):
        //   depth_mul   — stretches/compresses the raw bitmap-z samples before
        //                 they are combined with the draw anchor. This is the
        //                 calibration knob for matching billboard depth to the
        //                 3D/native z space.
        //   normal_mul  — scales the sampled z feeding dfdx/dfdy for normal
        //                 reconstruction. Independent of depth_mul because
        //                 raw samples are tiny (u16/65535) and need to be
        //                 cranked into the thousands to tilt a normal.
        //   z_near/z_far — the frustum volume in iso-z world units. A tile's
        //                 scene_z maps to NDC depth via (scene_z - z_near) /
        //                 (z_far - z_near), clamped to [0,1]. The game's
        //                 isometric camera is fixed, so default to one global
        //                 range instead of re-fitting per loaded sector.
        static float depth_mul     = 1.0f;
        static float normal_mul    = 15000.0f;
        static float normal_radius = 1.5f;
        // Bilateral reject threshold for normal reconstruction, in the same
        // native screen-z units as retail's bitmap zbuffer samples. The old
        // normalized 0..1 control mapped roughly to 0..65535, so 64 keeps us
        // near the previous ~0.001 default.
        static float edge_thr      = 64.0f;
        // Depth window in raw world units (100 wu = 1 m). Keep one fixed
        // game-wide default; the view/perspective never changes.
        static float z_near        = kSectorDepthNearWU;
        static float z_far         = kSectorDepthFarWU;
        static bool  z_range_logged = false;
        static bool  animate      = false;
        static int32_t tick       = 0;
        static int32_t view_mode    = 0;   // 0=lit 1=albedo 2=depth 3=normals
        static bool    lights_on    = true;
        static float   radius_mul   = 1.0f;
        static float   intensity_mul = 1.0f;

        const int32_t vw = Display->Width();
        const int32_t vh = Display->Height();
        int32_t cam_ox = 0, cam_oy = 0;
        SectorCameraOriginScreen(cam_ox, cam_oy);

        float scene_z_min_fit = FLT_MAX;
        float scene_z_max_fit = -FLT_MAX;
        int32_t fit_tiles = 0;
        for (const auto& inst : g_sectorTileInst)
        {
            const auto& tex = g_sectorTileTex[inst.tex_idx];
            S3DPoint sp;
            SectorProjectWorld(inst.world_pos, sp);
            const int32_t x0 = sp.x - inst.regx + cam_ox;
            const int32_t y0 = sp.y - inst.regy + cam_oy;
            const int32_t x1 = x0 + tex.w;
            const int32_t y1 = y0 + tex.h;
            if (x1 <= 0 || y1 <= 0 || x0 >= vw || y0 >= vh)
                continue;
            const float camera_z = SectorCameraSceneZ(inst);
            const float z0 = inst.has_authored_local_dz
                ? (camera_z + depth_mul * inst.authored_local_dz_min)
                : (camera_z + depth_mul * (tex.z_local_min - float(inst.regz)));
            const float z1 = inst.has_authored_local_dz
                ? (camera_z + depth_mul * inst.authored_local_dz_max)
                : (camera_z + depth_mul * (tex.z_local_max - float(inst.regz)));
            if (z0 < scene_z_min_fit) scene_z_min_fit = z0;
            if (z1 > scene_z_max_fit) scene_z_max_fit = z1;
            ++fit_tiles;
        }
        if (!(scene_z_min_fit < scene_z_max_fit))
        {
            for (const auto& inst : g_sectorTileInst)
            {
                const auto& tex = g_sectorTileTex[inst.tex_idx];
                const float camera_z = SectorCameraSceneZ(inst);
                const float z0 = inst.has_authored_local_dz
                    ? (camera_z + depth_mul * inst.authored_local_dz_min)
                    : (camera_z + depth_mul * (tex.z_local_min - float(inst.regz)));
                const float z1 = inst.has_authored_local_dz
                    ? (camera_z + depth_mul * inst.authored_local_dz_max)
                    : (camera_z + depth_mul * (tex.z_local_max - float(inst.regz)));
                if (z0 < scene_z_min_fit) scene_z_min_fit = z0;
                if (z1 > scene_z_max_fit) scene_z_max_fit = z1;
            }
        }
        if (!(scene_z_min_fit < scene_z_max_fit))
        {
            scene_z_min_fit = g_sectorSceneZMin;
            scene_z_max_fit = g_sectorSceneZMax;
        }

        if (animate)
        {
            ++tick;
            const float t = float(tick) * (6.2831853f / 180.0f);
            dir[0] = std::cos(t);
            dir[2] = std::sin(t);
        }

        if (!z_range_logged)
        {
            z_range_logged = true;
            log_info("[sector] fixed z window [%.0f..%.0f] wu (visible scene=[%.0f..%.0f])",
                     z_near, z_far, scene_z_min_fit, scene_z_max_fit);
        }

        ImGui::SetNextWindowSize(ImVec2(320, 0), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Sector lighting"))
        {
            // Frame-time / event-queue diagnostic. dt_ema smooths sapp_frame_duration
            // so spikes stay visible; qdepth is an ImGui-side proxy for "are mouse
            // events arriving behind the frames they belong to" — when this climbs
            // without you moving the mouse, events are queueing.
            {
                static float dt_ema = 0.0f;
                const float dt = float(TTime::DeltaTime());
                dt_ema = dt_ema == 0.0f ? dt : (dt_ema * 0.95f + dt * 0.05f);
                const ImGuiIO& io = ImGui::GetIO();
                ImGui::Text("frame: %.2f ms (ema %.2f ms)  tiles=%zu lights=%zu",
                            dt * 1000.0f, dt_ema * 1000.0f,
                            g_sectorTileInst.size(), g_sectorLights.size());
                ImGui::Text("mouse=(%.0f,%.0f) dragging=%d cam=(%d,%d,%d)",
                            io.MousePos.x, io.MousePos.y,
                            g_sectorDragging ? 1 : 0,
                            g_sectorCameraWorld.x,
                            g_sectorCameraWorld.y,
                            g_sectorCameraWorld.z);
                ImGui::Separator();
            }
            ImGui::TextUnformatted("view:");
            ImGui::SameLine(); if (ImGui::RadioButton("lit",     view_mode == 0)) view_mode = 0;
            ImGui::SameLine(); if (ImGui::RadioButton("albedo",  view_mode == 1)) view_mode = 1;
            ImGui::SameLine(); if (ImGui::RadioButton("depth",   view_mode == 2)) view_mode = 2;
            ImGui::SameLine(); if (ImGui::RadioButton("normals", view_mode == 3)) view_mode = 3;
            ImGui::SameLine(); if (ImGui::RadioButton("points",  view_mode == 4)) view_mode = 4;
            ImGui::Separator();
            ImGui::Checkbox("animate (rotate dir in XZ)", &animate);
            ImGui::SliderFloat3("dir (xyz)", dir, -1.0f, 1.0f);
            ImGui::SliderFloat("intensity",  &intensity,   0.0f, 4.0f);
            ImGui::ColorEdit3("color",       color);
            ImGui::SliderFloat("ambient",    &ambient,     0.0f, 10.0f);
            // depth_mul feeds the output depth; normal_mul feeds dfdx/dfdy for
            // normal reconstruction. They stay independent because matching the
            // billboard depth to native z-space is a separate concern from
            // tuning the reconstructed surface tilt.
            ImGui::SliderFloat("depth_mul",     &depth_mul,     0.0f, 128.0f);
            ImGui::SliderFloat("normal_mul",    &normal_mul,    0.01f, 65536.0f, "%.1f", ImGuiSliderFlags_Logarithmic);
            ImGui::SliderFloat("normal_radius", &normal_radius, 0.5f, 8.0f);
            ImGui::SliderFloat("edge_thr",      &edge_thr,      1.0f, 2048.0f, "%.1f", ImGuiSliderFlags_Logarithmic);
            // z_near/z_far are the fixed game-view frustum planes in raw
            // world units (100 wu = 1 m). Keep them tweakable for debug, but
            // default/reset always returns to the game-wide constants.
            {
                const float span    = scene_z_max_fit - scene_z_min_fit;
                const float margin  = span > 0.0f ? span : 2500.0f;
                const float slo     = fminf(kSectorDepthNearWU, scene_z_min_fit - margin);
                const float shi     = fmaxf(kSectorDepthFarWU, scene_z_max_fit + margin);
                if (ImGui::Button("reset z-range"))
                {
                    z_near = kSectorDepthNearWU;
                    z_far  = kSectorDepthFarWU;
                }
                ImGui::SameLine();
                ImGui::Text("clip span=%.0f wu  fit tiles=%d", z_far - z_near, fit_tiles);
                ImGui::SliderFloat("z_near", &z_near, slo, shi);
                ImGui::SliderFloat("z_far",  &z_far,  slo, shi);
                ImGui::Text("view z=[%.0f..%.0f] wu (~%.1f..%.1f m)",
                            scene_z_min_fit, scene_z_max_fit,
                            scene_z_min_fit / 100.0f,
                            scene_z_max_fit / 100.0f);
                if (scene_z_min_fit < z_near || scene_z_max_fit > z_far)
                    ImGui::TextColored(ImVec4(1.0f, 0.45f, 0.2f, 1.0f),
                                       "view depth is clipping");
            }

            ImGui::Separator();
            ImGui::Checkbox("show tile bboxes", &g_sectorShowTileBboxes);
            ImGui::Checkbox("show gizmos",      &g_sectorShowGizmos);
            ImGui::Separator();
            ImGui::Text("point lights (%zu from sector)", g_sectorLights.size());
            ImGui::Checkbox("lights enabled",    &lights_on);
            ImGui::SliderFloat("radius x",        &radius_mul,    0.1f, 8.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
            ImGui::SliderFloat("intensity x",     &intensity_mul, 0.0f, 100.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
            if (ImGui::TreeNode("per-light"))
            {
                for (size_t i = 0; i < g_sectorLights.size(); ++i)
                {
                    SSectorLight& L = g_sectorLights[i];
                    float rgb[3]; SectorLightColor(L, rgb);
                    const char* cn = SectorLightClassName(L);
                    ImGui::PushID(int(i));
                    ImGui::Checkbox("##on", &L.enabled);
                    ImGui::SameLine();
                    ImGui::Text("%zu %s  r=%.0f  (%.2f,%.2f,%.2f) int=%.2f",
                                i, cn ? cn : "?",
                                SectorLightRadius(L), rgb[0], rgb[1], rgb[2],
                                SectorLightIntensity(L));
                    ImGui::PopID();
                }
                ImGui::TreePop();
            }
        }
        ImGui::End();

        Display->SetLight(dir[0], dir[1], dir[2], intensity,
                          color[0], color[1], color[2], ambient);
        Display->SetNormalRadius(normal_radius);
        Display->SetEdgeThreshold(edge_thr);
        Display->SetTileViewMode(view_mode);
        // Camera pose. The test harness now keeps a real world-space camera
        // center at g_sectorCameraWorld, so projection, depth, picking, and
        // reconstruction all use the same camera state.
        //
        // Tile screen position: project(world_pos - g_sectorCameraWorld) and
        // add the fixed screen origin.
        // Tile depth: CameraDepth(world_pos - g_sectorCameraWorld) - regz,
        // mapped to [0,1] via (z - z_near) / (z_far - z_near).
        const float   zspan = z_far - z_near;

        // View-frustum light cull: project each enabled light to screen and
        // keep only those whose radius-inflated footprint intersects the
        // viewport. Shader caps at 16 per draw — if more pass the view
        // cull, keep the 16 closest (world distance) to the view center.
        // Cull runs once per frame; the kept list is shared by every tile.
        Display->ClearPointLights();
        if (lights_on)
        {
            // View center in world: back-project screen-center through the
            // camera's z=0 plane. Used as the tie-breaker when more than
            // 16 lights pass the visibility test.
            S3DPoint vc_rel;
            ScreenToWorld(vw / 2 - cam_ox,
                          vh / 2 - cam_oy,
                          vc_rel, 0);
            const S3DPoint vc_w = vc_rel + g_sectorCameraWorld;

            struct Pick { int32_t light_idx; float d2_to_view; };
            Pick picks[TDisplay::kMaxPointLights];
            int32_t pick_n = 0;
            for (int32_t i = 0; i < int32_t(g_sectorLights.size()); ++i)
            {
                const SSectorLight& L = g_sectorLights[i];
                if (!L.enabled) continue;
                const S3DPoint wp = SectorLightPos(L);
                // Project to final screen position (same math as the gizmo).
                S3DPoint sp;
                SectorProjectWorld(wp, sp);
                const int32_t sx = sp.x + cam_ox;
                const int32_t sy = sp.y + cam_oy;
                // Inflated viewport test: a sphere of radius R in world is
                // at most R screen-pixels across in iso (x is unsquashed;
                // y is 2x squashed). So a loose R-pixel margin on all sides
                // catches any light that could touch a visible fragment.
                const float r_px = SectorLightRadius(L) * radius_mul;
                if (sx + r_px < 0 || sx - r_px >= vw) continue;
                if (sy + r_px < 0 || sy - r_px >= vh) continue;
                const float dx = float(wp.x - vc_w.x);
                const float dy = float(wp.y - vc_w.y);
                const float dz = float(wp.z - vc_w.z);
                const float d2 = dx*dx + dy*dy + dz*dz;
                if (pick_n < TDisplay::kMaxPointLights) {
                    picks[pick_n++] = { i, d2 };
                } else {
                    int32_t worst = 0;
                    for (int32_t k = 1; k < pick_n; ++k)
                        if (picks[k].d2_to_view > picks[worst].d2_to_view) worst = k;
                    if (d2 < picks[worst].d2_to_view) picks[worst] = { i, d2 };
                }
            }
            for (int32_t k = 0; k < pick_n; ++k)
            {
                const SSectorLight& L = g_sectorLights[picks[k].light_idx];
                const S3DPoint wp = SectorLightPos(L);
                float rgb[3]; SectorLightColor(L, rgb);
                Display->AddPointLight(float(wp.x), float(wp.y), float(wp.z),
                                       SectorLightRadius(L) * radius_mul,
                                       rgb[0], rgb[1], rgb[2],
                                       SectorLightIntensity(L) * intensity_mul);
            }
        }

        // Deferred lighting reconstruction. The light pass reads only the
        // G-buffer (albedo + normal + depth) and reconstructs each fragment's
        // world position from the same world-space iso camera used by the
        // tile pass.
        {
            Display->SetReconstructionParams(float(cam_ox),
                                             float(cam_oy),
                                             z_near, z_far,
                                             float(g_sectorCameraWorld.x),
                                             float(g_sectorCameraWorld.y),
                                             kCamForwardWU, 0.0f);
        }

        // Retail software blit semantics still matter: BM_ZBUFFER is a local
        // per-pixel offset from the imagery anchor, not an absolute scene
        // plane. But the anchor itself comes from world space in camera
        // coordinates, not from the projected screenpos.z.
        const float depth_scale_norm  = std::fabs(zspan) > 1e-6f
            ? depth_mul / zspan : 0.0f;

        Display->BeginTilePass(0.12f, 0.16f, 0.10f, 1.0f);  // dark-green placeholder ground
        static bool draw_stats_logged = false;
        int32_t draw_submitted = 0, draw_invalid_img = 0, draw_offscreen = 0;
        // Coverage map at 32x32 pixel cells. A cell is "covered" if any on-screen
        // tile rect overlaps it. Lets us answer "is swiss cheese gaps between
        // tiles or holes within tiles?" without a visual capture.
        constexpr int32_t kCovCellPx = 32;
        const int32_t cov_cw = (vw + kCovCellPx - 1) / kCovCellPx;
        const int32_t cov_ch = (vh + kCovCellPx - 1) / kCovCellPx;
        std::vector<uint8_t> cov(size_t(cov_cw) * size_t(cov_ch), 0);
        for (const auto& inst : g_sectorTileInst)
        {
            if (!g_sectorShowGizmos) {
                TObjectInstance* oi = inst.src.Get();
                if (oi && oi->IsLight()) continue;
            }
            const auto& tex = g_sectorTileTex[inst.tex_idx];
            S3DPoint sp;
            SectorProjectWorld(inst.world_pos, sp);
            const float anchor_scene = SectorCameraSceneZ(inst) - depth_mul * float(inst.regz);
            const float anchor_scene_norm = std::fabs(zspan) > 1e-6f
                ? (anchor_scene - z_near) / zspan : 0.5f;
            const int32_t dx = sp.x - inst.regx + cam_ox;
            const int32_t dy = sp.y - inst.regy + cam_oy;
            if (!tex.color.id || !tex.depth.id) { draw_invalid_img++; continue; }
            const bool onscreen = !(dx + tex.w <= 0 || dy + tex.h <= 0 || dx >= vw || dy >= vh);
            if (!draw_stats_logged && onscreen)
            {
                log_info("[sector] onscreen tile: wpos=(%d,%d,%d) reg=(%d,%d,%d)"
                         " dst=(%d,%d %dx%d) class=%s",
                    inst.world_pos.x, inst.world_pos.y, inst.world_pos.z,
                    inst.regx, inst.regy, inst.regz,
                    dx, dy, tex.w, tex.h,
                    tex.debug_classname ? tex.debug_classname : "?");
            }
            if (!onscreen) draw_offscreen++;
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
            draw_submitted++;
            Display->DrawTile(tex.color, tex.depth,
                              dx, dy,
                              tex.w, tex.h,
                              anchor_scene_norm, depth_scale_norm, normal_mul,
                              float(inst.world_pos.x),
                              float(inst.world_pos.y),
                              float(inst.world_pos.z),
                              float(inst.regx), float(inst.regy),
                              1.0f);
        }
        if (!draw_stats_logged) {
            draw_stats_logged = true;
            int32_t cov_hit = 0;
            for (uint8_t b : cov) cov_hit += b;
            const int32_t cov_total = int32_t(cov.size());
            log_info("[sector] draw stats: total_inst=%zu submitted=%d invalid_img=%d"
                     " offscreen_submitted=%d vw=%d vh=%d cov_cells=%d/%d (%.1f%%)",
                g_sectorTileInst.size(), draw_submitted, draw_invalid_img, draw_offscreen,
                vw, vh, cov_hit, cov_total,
                cov_total > 0 ? 100.0 * cov_hit / cov_total : 0.0);
            // ASCII map of coverage: '#' covered, '.' gap. Shows the gap pattern.
            std::string line;
            line.reserve(size_t(cov_cw) + 1);
            for (int32_t cy = 0; cy < cov_ch; ++cy) {
                line.clear();
                for (int32_t cx = 0; cx < cov_cw; ++cx)
                    line.push_back(cov[size_t(cy) * size_t(cov_cw) + size_t(cx)] ? '#' : '.');
                log_info("[sector] cov %02d %s", cy, line.c_str());
            }
        }
        Display->EndTilePass();
        Display->RunLightingPass();

        // Tile bbox overlay — project each tile's world-space bounding box
        // (wwidth x wlength x wheight wu at world_pos-wreg) onto the screen
        // and wireframe it. Lets us see which tiles are loaded and where they
        // sit, independent of whether their bitmap actually covered the pixel.
        //
        // The sector test loads the whole level (thousands of tiles), so the
        // overlay is frustum-culled to the viewport — drawing every tile's
        // wireframe would dwarf the screen with off-screen geometry and push
        // well over the simgui vertex budget even after the 1M bump.
        if (g_sectorShowTileBboxes)
        {
            if (ImDrawList* dl = ImGui::GetForegroundDrawList())
            {
                const ImU32 edge_ground = IM_COL32(255, 80, 80, 255);
                const ImU32 edge_top    = IM_COL32(255, 220, 80, 255);
                const ImU32 edge_vert   = IM_COL32(120, 180, 255, 255);
                auto proj = [&](int32_t wx, int32_t wy, int32_t wz) {
                    S3DPoint w = { wx, wy, wz };
                    S3DPoint sp;
                    SectorProjectWorld(w, sp);
                    return ImVec2(float(sp.x + cam_ox), float(sp.y + cam_oy));
                };
                static int32_t bbox_debug_logged = 0;
                int32_t bbox_with_dims = 0, bbox_drawn = 0;
                // `GetWorldBoundBox` / `GetWorldRegX|Y|Z` return values in walk-grid
                // units (see helper.cpp: obj->pos.X = (wwidth/2 - wregx) * GRIDSIZE).
                // One grid cell = 16 world units, so an "8x8" tile is actually
                // 128x128 world units on the ground. Using a local constant avoids
                // pulling mappane.h just for GRIDSIZE/GRIDSHIFT.
                constexpr int32_t kGridWU = 16;
                for (const auto& inst : g_sectorTileInst)
                {
                    if (inst.wwidth <= 0 || inst.wlength <= 0) continue;
                    ++bbox_with_dims;
                    const int32_t x0 = inst.world_pos.x - inst.wregx * kGridWU;
                    const int32_t y0 = inst.world_pos.y - inst.wregy * kGridWU;
                    const int32_t z0 = inst.world_pos.z - inst.wregz * kGridWU;
                    const int32_t x1 = x0 + inst.wwidth  * kGridWU;
                    const int32_t y1 = y0 + inst.wlength * kGridWU;
                    const int32_t z1 = z0 + (inst.wheight > 0 ? inst.wheight * kGridWU : 0);
                    const ImVec2 c000 = proj(x0, y0, z0);
                    const ImVec2 c100 = proj(x1, y0, z0);
                    const ImVec2 c010 = proj(x0, y1, z0);
                    const ImVec2 c110 = proj(x1, y1, z0);
                    const ImVec2 c001 = proj(x0, y0, z1);
                    const ImVec2 c101 = proj(x1, y0, z1);
                    const ImVec2 c011 = proj(x0, y1, z1);
                    const ImVec2 c111 = proj(x1, y1, z1);
                    float bxmin = c000.x, bxmax = c000.x;
                    float bymin = c000.y, bymax = c000.y;
                    auto expand = [&](const ImVec2& v) {
                        if (v.x < bxmin) bxmin = v.x; if (v.x > bxmax) bxmax = v.x;
                        if (v.y < bymin) bymin = v.y; if (v.y > bymax) bymax = v.y;
                    };
                    expand(c100); expand(c010); expand(c110);
                    expand(c001); expand(c101); expand(c011); expand(c111);
                    if (bxmax < 0.0f || bymax < 0.0f
                        || bxmin > float(vw) || bymin > float(vh))
                        continue;
                    ++bbox_drawn;
                    if (bbox_debug_logged < 6) {
                        ++bbox_debug_logged;
                        log_info("[sector] bbox[%d] wpos=(%d,%d,%d) wreg=(%d,%d,%d)"
                                 " wsize=%dx%dx%d screen_bbox=[%.1f,%.1f..%.1f,%.1f]"
                                 " c000=(%.1f,%.1f) c110=(%.1f,%.1f) c111=(%.1f,%.1f)",
                            bbox_drawn, inst.world_pos.x, inst.world_pos.y, inst.world_pos.z,
                            inst.wregx, inst.wregy, inst.wregz,
                            inst.wwidth, inst.wlength, inst.wheight,
                            bxmin, bymin, bxmax, bymax,
                            c000.x, c000.y, c110.x, c110.y, c111.x, c111.y);
                    }
                    // Bottom rect
                    dl->AddLine(c000, c100, edge_ground, 1.0f);
                    dl->AddLine(c100, c110, edge_ground, 1.0f);
                    dl->AddLine(c110, c010, edge_ground, 1.0f);
                    dl->AddLine(c010, c000, edge_ground, 1.0f);
                    if (inst.wheight > 0) {
                        // Top rect
                        dl->AddLine(c001, c101, edge_top, 1.0f);
                        dl->AddLine(c101, c111, edge_top, 1.0f);
                        dl->AddLine(c111, c011, edge_top, 1.0f);
                        dl->AddLine(c011, c001, edge_top, 1.0f);
                        // Vertical edges
                        dl->AddLine(c000, c001, edge_vert, 1.0f);
                        dl->AddLine(c100, c101, edge_vert, 1.0f);
                        dl->AddLine(c010, c011, edge_vert, 1.0f);
                        dl->AddLine(c110, c111, edge_vert, 1.0f);
                    }
                }
                static bool bbox_stats_logged = false;
                if (!bbox_stats_logged) {
                    bbox_stats_logged = true;
                    int32_t no_dims = 0;
                    for (const auto& inst : g_sectorTileInst)
                        if (inst.wwidth <= 0 || inst.wlength <= 0) no_dims++;
                    log_info("[sector] bbox overlay: total=%zu with_dims=%d"
                             " no_dims=%d onscreen_drawn=%d",
                        g_sectorTileInst.size(), bbox_with_dims, no_dims,
                        bbox_drawn);
                }
            }
        }

        // Light drag feedback — the light's in-world billboard icon is its
        // own visual, so we only draw a crosshair on the currently-dragged
        // light. Pick/drag happens by clicking the icon directly. Right-drag
        // to move in world xy; shift + right-drag to move in world z.
        if (g_lightDragIdx >= 0 && g_lightDragIdx < int32_t(g_sectorLights.size()))
        {
            if (ImDrawList* dl = ImGui::GetForegroundDrawList())
            {
                const SSectorLight& L = g_sectorLights[g_lightDragIdx];
                if (L.enabled)
                {
                    S3DPoint sp;
                    SectorProjectWorld(SectorLightPos(L), sp);
                    const ImVec2 c(float(sp.x + cam_ox),
                                   float(sp.y + cam_oy));
                    const ImU32 outl = IM_COL32(0, 0, 0, 220);
                    dl->AddLine(ImVec2(c.x - 12, c.y), ImVec2(c.x + 12, c.y), outl, 1.5f);
                    dl->AddLine(ImVec2(c.x, c.y - 12), ImVec2(c.x, c.y + 12), outl, 1.5f);
                }
            }
        }
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

void TTestScreen::MouseClick(int32_t button, int32_t x, int32_t y)
{
    if (strcmp(StartupTestMode, "sector") != 0) return;
    if (button == MB_LEFTDOWN)
    {
        // Ctrl-click always pans. Otherwise try to pick a light within
        // threshold pixels; if nothing picks, fall through to pan so the
        // map stays draggable anywhere outside a gizmo.
        const bool ctrl_pan = CtrlDown;
        int32_t picked = -1;
        if (!ctrl_pan)
        {
            int32_t cam_ox = 0, cam_oy = 0;
            SectorCameraOriginScreen(cam_ox, cam_oy);
            float best_d2 = 24.0f * 24.0f;  // 24-pixel pick radius
            for (size_t i = 0; i < g_sectorLights.size(); ++i)
            {
                const SSectorLight& L = g_sectorLights[i];
                if (!L.enabled) continue;
                S3DPoint sp;
                SectorProjectWorld(SectorLightPos(L), sp);
                const float dx = float((sp.x + cam_ox) - x);
                const float dy = float((sp.y + cam_oy) - y);
                const float d2 = dx * dx + dy * dy;
                if (d2 < best_d2) { best_d2 = d2; picked = int32_t(i); }
            }
        }
        if (picked >= 0)
        {
            if (TObjectInstance* pi = g_sectorLights[picked].ref.Get())
            {
                g_lightDragIdx        = picked;
                g_lightDragStartSX    = x;
                g_lightDragStartSY    = y;
                // Drag the OF_LIGHT instance itself — the light pos and its
                // tile render pos both derive from oi->Pos(), so moving the
                // instance moves both together. Snapshot the starting oi
                // pos so the drag is relative to the click, not jumpy.
                g_lightDragStartOiPos = pi->Pos();
            }
            else
            {
                picked = -1;  // ref went stale between cull and pick — pan
                g_sectorDragging       = true;
                g_dragStartX           = x;
                g_dragStartY           = y;
                g_dragCameraStartWorld = g_sectorCameraWorld;
            }
        }
        else
        {
            g_sectorDragging       = true;
            g_dragStartX           = x;
            g_dragStartY           = y;
            g_dragCameraStartWorld = g_sectorCameraWorld;
        }
    }
    else if (button == MB_LEFTUP)
    {
        g_sectorDragging = false;
        g_lightDragIdx   = -1;
    }
}

void TTestScreen::MouseMove(int32_t button, int32_t x, int32_t y)
{
    (void)button;
    if (g_lightDragIdx >= 0 && g_lightDragIdx < int32_t(g_sectorLights.size()))
    {
        SSectorLight& L = g_sectorLights[g_lightDragIdx];
        TObjectInstance* oi = L.ref.Get();
        if (!oi) { g_lightDragIdx = -1; return; }
        S3DPoint newpos = g_lightDragStartOiPos;
        if (ShiftDown)
        {
            // Shift-drag: move instance in world z. Screen y is inverted so
            // dragging the mouse up raises the instance. 1 wu/px.
            const int32_t dy = y - g_lightDragStartSY;
            newpos.z = g_lightDragStartOiPos.z - dy;
        }
        else
        {
            // XY drag: back-project the current mouse to world space at the
            // instance's original z height relative to the live camera.
            int32_t cam_ox = 0, cam_oy = 0;
            SectorCameraOriginScreen(cam_ox, cam_oy);
            S3DPoint wp_rel;
            ScreenToWorld(x - cam_ox,
                          y - cam_oy,
                          wp_rel, int32_t(g_lightDragStartOiPos.z));
            newpos.x = wp_rel.x + g_sectorCameraWorld.x;
            newpos.y = wp_rel.y + g_sectorCameraWorld.y;
            newpos.z = g_lightDragStartOiPos.z;
        }
        // SetPos(override=true) bypasses move validation; retail uses it for
        // editor/debug moves. Then sync any cached tile-inst world_pos that
        // was snapshotted from oi->Pos() at load so the icon follows.
        oi->SetPos(newpos, -1, true);
        const S3DPoint now = oi->Pos();
        for (auto& ti : g_sectorTileInst) {
            if (ti.src.Get() == oi) ti.world_pos = now;
        }
        return;
    }

    if (!g_sectorDragging) return;
    S3DPoint drag_delta_w = {0, 0, 0};
    ScreenToWorld(x - g_dragStartX,
                  y - g_dragStartY,
                  drag_delta_w, 0);
    g_sectorCameraWorld.x = g_dragCameraStartWorld.x - drag_delta_w.x;
    g_sectorCameraWorld.y = g_dragCameraStartWorld.y - drag_delta_w.y;
    g_sectorCameraWorld.z = g_dragCameraStartWorld.z;
}
