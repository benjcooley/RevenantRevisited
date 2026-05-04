// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           gamemap.cpp - per-level sector container                   *
// *************************************************************************

#include "gamemap.h"

#include "logging.h"
#include "mappane.h"
#include "object.h"
#include "sector.h"

#include <cstdio>
#include <cstring>
#include <filesystem>
#include <string>
#include <unordered_set>

namespace
{
// Disk scan: walk every per-sector .DAT file under the resolved data
// roots and return the (sx, sy) coordinates that exist for `level`.
// Equivalent to the version that lived in maprenderer.cpp; will move
// fully into TGameMap once the renderer stops doing its own load.
struct SCoord { int32_t sx, sy; };

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

    // The renderer's older scan also looked under data-root subdirs and
    // tried rev_fopen for every (sx,sy) up to 64x64. Phase 1 keeps this
    // lighter -- the curmap dir scan covers everything Misthaven ships
    // with. Tighten if a level shows missing sectors.
    auto scan_dirs = std::vector<fs::path>{
        fs::path(CurMapPath)  / CURMAPDIR,
        fs::path(BaseMapPath) / BASEMAPDIR,
    };
    for (const fs::path& dir : scan_dirs)
        GatherCoordsFromDir(dir, level, coords);

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

    // Stamp tile walkmaps onto each sector's per-cell walkmap. Under
    // the legacy MapPane.AddObject path this happened automatically per
    // tile; the new renderer-owns-sectors path skipped it. Doing it
    // here, while the map owns the sector list, is the right home --
    // walkmap is sector data, not renderer data.
    int32_t walkmap_tiles_stamped = 0;
    for (TSector* sec : sectors)
    {
        if (!sec) continue;
        const int32_t n = sec->NumItems();
        for (int32_t i = 0; i < n; ++i)
        {
            TObjectInstance* oi = sec->GetInstance(i);
            if (!oi || oi->ObjClass() != OBJCLASS_TILE) continue;
            if (oi->Flags() & OF_NOWALK) continue;
            MapPane.TransferWalkmap(oi);
            ++walkmap_tiles_stamped;
        }
    }

    log_info("[gamemap] level %d loaded: %zu sectors, %d objects, "
             "%d tile walkmaps stamped",
             level, sectors.size(), loaded_objs, walkmap_tiles_stamped);

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
