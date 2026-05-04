// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           gamemap.cpp - per-level sector container                   *
// *************************************************************************

#include "gamemap.h"

#include "logging.h"
#include "mappane.h"
#include "object.h"
#include "revutils.h"
#include "sector.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <string>
#include <unordered_set>

namespace
{
// Disk scan: walk every per-sector .DAT file under the resolved data
// roots and return the (sx, sy) coordinates that exist for `level`.
// Lifted from maprenderer.cpp's FindLevelSectorCoords -- this is the
// same multi-path scan (curmap / basemap / data-root subdirs) plus the
// 64x64 brute-force rev_fopen fallback that catches files visible only
// through the VFS.
struct SCoord { int32_t sx, sy; };

inline std::filesystem::path NormalizeFsPath(const char* path)
{
    std::string s = (path && path[0]) ? path : ".";
    for (char& ch : s)
        if (ch == '\\') ch = '/';
    if (s.empty()) s = ".";
    return std::filesystem::path(s);
}

std::filesystem::path ResolveSectorDataRoot()
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

void GatherCoordsFromDir(const std::filesystem::path& dir, int32_t level,
                         std::vector<SCoord>& out)
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
        int32_t file_lvl = 0, sx = 0, sy = 0;
        char ext[16] = {};
        if (std::sscanf(name.c_str(), "%d_%d_%d.%15s", &file_lvl, &sx, &sy, ext) != 4)
            continue;
        if (file_lvl != level || stricmp(ext, "DAT") != 0)
            continue;
        out.push_back({ sx, sy });
    }
}

std::vector<SCoord> ScanLevelCoords(int32_t level)
{
    namespace fs = std::filesystem;
    std::vector<SCoord> coords;
    std::vector<fs::path> scan_dirs;
    scan_dirs.push_back(NormalizeFsPath(CurMapPath)  / CURMAPDIR);
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
        GatherCoordsFromDir(dir, level, coords);

    // VFS / rvr-pack fallback: try rev_fopen for every (sx,sy) up to
    // 64x64. Picks up sectors visible through the resource archives
    // even when there's no on-disk .DAT file in the scan dirs.
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

    // De-dup (sx,sy) pairs across the scan dirs.
    std::sort(coords.begin(), coords.end(), [](const SCoord& a, const SCoord& b) {
        if (a.sy != b.sy) return a.sy < b.sy;
        return a.sx < b.sx;
    });
    coords.erase(std::unique(coords.begin(), coords.end(),
                             [](const SCoord& a, const SCoord& b) {
                                 return a.sx == b.sx && a.sy == b.sy;
                             }),
                 coords.end());
    return coords;
}
}  // namespace

bool TGameMap::Load(int32_t lvl)
{
    if (lvl < 0)
        return false;
    if (level == lvl && !sectors.empty())
        return true;          // already loaded at this level

    if (IsLoaded())
        Unload();             // swap to a different level

    level = lvl;

    const std::vector<SCoord> coords = ScanLevelCoords(level);
    sectors.reserve(coords.size());

    int32_t loaded_objs = 0;
    for (const SCoord& c : coords)
    {
        TSector* sec = TSector::LoadSector(level, c.sx, c.sy, false);
        if (!sec)
        {
            log_warn("[gamemap] level %d sector %d_%d: LoadSector failed",
                     level, c.sx, c.sy);
            continue;
        }
        sectors.push_back(sec);
        loaded_objs += sec->NumItems();
    }

    // TODO(phase 2b): per-tile walkmap stamping should live here so
    // it owns the same lifecycle as the sectors. Currently the
    // MapPane.TransferWalkmap path looks up sectors via the renderer's
    // sectorsKept (added as a fallback when MapPane.sectors[][] went
    // empty), and at this point in the boot order the renderer's
    // mirror hasn't been populated yet. Renderer keeps the stamping
    // call after its sectorsKept mirror runs; we move it here once
    // TGameMap exposes a direct sector lookup that doesn't need
    // MapPane / the renderer fallback at all.

    log_info("[gamemap] level %d loaded: %zu sectors, %d objects",
             level, sectors.size(), loaded_objs);

    listeners.Notify(EGameMapEvent::Loaded, this);
    return true;
}

void TGameMap::Unload()
{
    if (!IsLoaded()) return;

    // Fire Unloaded BEFORE freeing sectors so subscribers can drop
    // references against still-valid pointers.
    listeners.Notify(EGameMapEvent::Unloaded, this);

    for (TSector* sec : sectors)
        if (sec) TSector::CloseSector(sec);

    sectors.clear();
    level = -1;
}

TSector* TGameMap::FindSector(int32_t sx, int32_t sy) const
{
    for (TSector* sec : sectors)
        if (sec && sec->SectorX() == sx && sec->SectorY() == sy)
            return sec;
    return nullptr;
}
