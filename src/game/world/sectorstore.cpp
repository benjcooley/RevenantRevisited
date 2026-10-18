// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   sectorstore.cpp - the sectors the game has changed                  *
// *************************************************************************

#include "game/world/sectorstore.h"

#include "engine/core/logging.h"
#include "engine/core/revdefs.h"
#include "game/app/revenant.h"
#include "game/app/revutils.h"

#include <algorithm>
#include <cstdio>
#include <map>
#include <system_error>
#include <tuple>

namespace fs = std::filesystem;

namespace SectorStore
{
namespace
{

using TKey = std::tuple<int32_t, int32_t, int32_t>;     // level, x, y

// The working set: each written sector's image.
std::map<TKey, std::vector<uint8_t>> g_workingSet;

bool KeyOf(const char* filename, TKey& key)
{
    int32_t level = 0, x = 0, y = 0;
    if (!filename || !SaveFile::ParseSectorFileName(filename, level, x, y))
        return false;
    key = {level, x, y};
    return true;
}

std::string NameOf(const TKey& key)
{
    const auto [level, x, y] = key;
    return SaveFile::SectorFileName(level, x, y);
}

bool ReadAll(FILE* fp, std::vector<uint8_t>& bytes)
{
    bytes.clear();
    if (!fp)
        return false;
    uint8_t chunk[16384];
    size_t got = 0;
    while ((got = std::fread(chunk, 1, sizeof(chunk), fp)) > 0)
        bytes.insert(bytes.end(), chunk, chunk + got);
    const bool ok = !std::ferror(fp);
    std::fclose(fp);
    return ok;
}

// The base map through the resource layer: SavePath's loose map/ (editor
// publishes), the Revisited overlay, the install, then the module's packed
// Map/.
FILE* OpenBase(const char* filename)
{
    char path[MAXPATHLEN];
    std::snprintf(path, sizeof(path), "%s%s\\%s", BaseMapPath, BASEMAPDIR, filename);
    return rev_fopen(path, "rb");
}

// The sector `delta` rebuilds from the base map; false when it can't be.
bool Rebuild(const SaveFile::SSectorDelta& delta, std::vector<uint8_t>& image)
{
    const std::string name = SaveFile::SectorFileName(delta.level, delta.x, delta.y);
    std::vector<uint8_t> base;
    const bool hasBase = ReadBase(name.c_str(), base);
    SaveFile::SApplyResult result;
    if (!SaveFile::ApplySectorDelta(delta, hasBase ? &base : nullptr, image, result))
    {
        log_warn("[sectorstore] %s: the saved changes can't be applied (%s); it is the module's", name.c_str(),
                 result.failure);
        return false;
    }
    if (result.missed > 0)
        log_warn("[sectorstore] %s: %d of the saved changes are to objects the module's sector no longer "
                 "has; those objects are the module's", name.c_str(), result.missed);
    return true;
}

bool IsSectorFile(const fs::directory_entry& entry)
{
    std::error_code ec;
    return entry.is_regular_file(ec) && stricmp(entry.path().extension().string().c_str(), ".DAT") == 0;
}

bool CreateDir(const fs::path& dir)
{
    std::error_code ec;
    fs::create_directories(dir, ec);
    if (ec)
        log_error("[sectorstore] can't create %s: %s", dir.string().c_str(), ec.message().c_str());
    return !ec;
}

bool WriteSectorFile(const fs::path& file, const std::vector<uint8_t>& image)
{
    FILE* fp = std::fopen(file.string().c_str(), "wb");
    const bool ok = fp && std::fwrite(image.data(), 1, image.size(), fp) == image.size();
    if (fp && std::fclose(fp) == 0 && ok)
        return true;
    log_error("[sectorstore] writing %s failed", file.string().c_str());
    return false;
}

// Writes `images` ({name, bytes}) into `dir`, replacing its sector files.
int32_t ReplaceSectorFiles(const std::vector<std::pair<std::string, std::vector<uint8_t>>>& images, const fs::path& dir)
{
    std::error_code ec;
    for (fs::directory_iterator it(dir, ec), end; !ec && it != end; it.increment(ec))
    {
        std::error_code removeError;
        if (IsSectorFile(*it))
            fs::remove(it->path(), removeError);
    }
    if (!CreateDir(dir))
        return 0;
    int32_t written = 0;
    for (const auto& [name, image] : images)
        written += WriteSectorFile(dir / name, image) ? 1 : 0;
    return written;
}

}  // namespace

bool Read(const char* filename, std::vector<uint8_t>& bytes)
{
    TKey key;
    if (KeyOf(filename, key))
        if (const auto it = g_workingSet.find(key); it != g_workingSet.end())
        {
            bytes = it->second;
            return true;
        }
    return ReadBase(filename, bytes);
}

bool Exists(const char* filename)
{
    TKey key;
    if (KeyOf(filename, key) && g_workingSet.count(key))
        return true;
    FILE* fp = OpenBase(filename);
    if (fp)
        std::fclose(fp);
    return fp != nullptr;
}

void Write(const char* filename, const uint8_t* data, size_t size)
{
    TKey key;
    if (!KeyOf(filename, key))
    {
        log_error("[sectorstore] '%s' isn't a sector file name; not kept", filename ? filename : "");
        return;
    }
    std::vector<uint8_t> image(data, data + size);
    std::vector<uint8_t> base;
    if (ReadBase(filename, base) && base == image)
        g_workingSet.erase(key);
    else
        g_workingSet[key] = std::move(image);
}

void Clear()
{
    log_info("[sectorstore] cleared the working set (%zu sectors)", g_workingSet.size());
    g_workingSet.clear();
}

std::vector<std::pair<int32_t, int32_t>> SectorsOn(int32_t level)
{
    std::vector<std::pair<int32_t, int32_t>> sectors;
    for (const auto& [key, image] : g_workingSet)
        if (std::get<0>(key) == level)
            sectors.emplace_back(std::get<1>(key), std::get<2>(key));
    return sectors;
}

fs::path BaseMapDir()
{
    // `<BaseMapPath>map` resolved as retail resolved it: makepath() roots a
    // relative path at SavePath.
    char relative[MAXPATHLEN];
    std::snprintf(relative, sizeof(relative), "%s%s", BaseMapPath, BASEMAPDIR);
    char resolved[MAXPATHLEN];
    return fs::path(makepath(relative, resolved, sizeof(resolved)));
}

bool ReadBase(const char* filename, std::vector<uint8_t>& bytes)
{
    return ReadAll(OpenBase(filename), bytes);
}

std::vector<SaveFile::SSectorDelta> Deltas()
{
    std::vector<SaveFile::SSectorDelta> deltas;
    deltas.reserve(g_workingSet.size());
    std::vector<uint8_t> base;
    for (const auto& [key, image] : g_workingSet)
    {
        const auto [level, x, y] = key;
        const bool hasBase = ReadBase(NameOf(key).c_str(), base);
        SaveFile::SSectorDelta delta = SaveFile::MakeSectorDelta(level, x, y, hasBase ? &base : nullptr, image);
        if (delta.whole || !delta.changes.empty())          // none: the base's objects, perhaps a later version
            deltas.push_back(std::move(delta));
    }
    return deltas;                              // the map's order is the sort order
}

void SetDeltas(const std::vector<SaveFile::SSectorDelta>& deltas)
{
    g_workingSet.clear();
    std::vector<uint8_t> image;
    for (const SaveFile::SSectorDelta& delta : deltas)
        if (Rebuild(delta, image))
            Write(SaveFile::SectorFileName(delta.level, delta.x, delta.y).c_str(), image.data(), image.size());
    log_info("[sectorstore] the working set holds %zu saved sectors", g_workingSet.size());
}

int32_t ImportFrom(const fs::path& dir)
{
    g_workingSet.clear();
    int32_t read = 0;
    std::error_code ec;
    std::vector<uint8_t> image;
    for (fs::directory_iterator it(dir, ec), end; !ec && it != end; it.increment(ec))
    {
        const std::string name = it->path().filename().string();
        TKey key;
        if (!IsSectorFile(*it) || !KeyOf(name.c_str(), key))
            continue;
        if (!ReadAll(std::fopen(it->path().string().c_str(), "rb"), image))
        {
            log_warn("[sectorstore] can't read %s", it->path().string().c_str());
            continue;
        }
        Write(name.c_str(), image.data(), image.size());
        ++read;
    }
    log_info("[sectorstore] read %d sectors from %s", read, dir.string().c_str());
    return read;
}

int32_t ExportTo(const fs::path& dir)
{
    std::vector<std::pair<std::string, std::vector<uint8_t>>> images;
    for (const auto& [key, image] : g_workingSet)
        images.emplace_back(NameOf(key), image);
    const int32_t written = ReplaceSectorFiles(images, dir);
    log_info("[sectorstore] wrote %d sectors to %s", written, dir.string().c_str());
    return written;
}

int32_t ExportDeltasTo(const std::vector<SaveFile::SSectorDelta>& deltas, const fs::path& dir)
{
    std::vector<std::pair<std::string, std::vector<uint8_t>>> images;
    std::vector<uint8_t> image;
    for (const SaveFile::SSectorDelta& delta : deltas)
        if (Rebuild(delta, image))
            images.emplace_back(SaveFile::SectorFileName(delta.level, delta.x, delta.y), image);
    const int32_t written = ReplaceSectorFiles(images, dir);
    log_info("[sectorstore] wrote %d of %zu saved sectors to %s", written, deltas.size(), dir.string().c_str());
    return written;
}

int32_t Publish()
{
    const fs::path dir = BaseMapDir();
    if (!CreateDir(dir))
        return 0;
    int32_t written = 0;
    for (auto it = g_workingSet.begin(); it != g_workingSet.end();)
    {
        if (WriteSectorFile(dir / NameOf(it->first), it->second))
        {
            it = g_workingSet.erase(it);
            ++written;
        }
        else
            ++it;                               // kept: the edit isn't lost
    }
    log_info("[sectorstore] published %d sectors to %s (%zu not written)", written, dir.string().c_str(),
             g_workingSet.size());
    return written;
}

}  // namespace SectorStore
