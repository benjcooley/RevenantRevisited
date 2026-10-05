// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   sectorstore.cpp - where sector files live on disk                   *
// *************************************************************************

#include "sectorstore.h"

#include "logging.h"
#include "revdefs.h"
#include "revenant.h"
#include "revutils.h"

#include <system_error>

namespace fs = std::filesystem;

namespace SectorStore
{
namespace
{

// `<root><subdir>` resolved as retail resolved it: makepath() roots a
// relative path at SavePath. Retail TSector file save (0x00498a40) writes
// CurMapPath + "curmap\" + filename.
fs::path ResolveDir(const char* root, const char* subdir)
{
    char relative[MAXPATHLEN];
    std::snprintf(relative, sizeof(relative), "%s%s", root, subdir);
    char resolved[MAXPATHLEN];
    return fs::path(makepath(relative, resolved, sizeof(resolved)));
}

bool IsSectorFile(const fs::directory_entry& entry)
{
    std::error_code ec;
    return entry.is_regular_file(ec) &&
           stricmp(entry.path().extension().string().c_str(), ".DAT") == 0;
}

int32_t CopySectorFiles(const fs::path& from, const fs::path& to)
{
    std::error_code ec;
    if (!fs::is_directory(from, ec))
        return 0;
    fs::create_directories(to, ec);
    if (ec)
    {
        log_error("[sectorstore] can't create %s: %s", to.string().c_str(), ec.message().c_str());
        return 0;
    }

    int32_t copied = 0;
    for (fs::directory_iterator it(from, ec), end; !ec && it != end; it.increment(ec))
    {
        if (!IsSectorFile(*it))
            continue;
        std::error_code copyError;
        fs::copy_file(it->path(), to / it->path().filename(),
                      fs::copy_options::overwrite_existing, copyError);
        if (copyError)
            log_warn("[sectorstore] copy %s failed: %s",
                     it->path().string().c_str(), copyError.message().c_str());
        else
            ++copied;
    }
    return copied;
}

}  // namespace

fs::path WorkingSetDir()
{
    return ResolveDir(CurMapPath, CURMAPDIR);
}

fs::path BaseMapDir()
{
    return ResolveDir(BaseMapPath, BASEMAPDIR);
}

FILE* OpenForRead(const char* filename)
{
    if (FILE* fp = std::fopen((WorkingSetDir() / filename).string().c_str(), "rb"))
        return fp;

    // The base map goes through the resource layer: SavePath's loose map/
    // (editor publishes), the Revisited overlay, the install, then the
    // module's packed Map/.
    char path[MAXPATHLEN];
    std::snprintf(path, sizeof(path), "%s%s\\%s", BaseMapPath, BASEMAPDIR, filename);
    return rev_fopen(path, "rb");
}

FILE* OpenForWrite(const char* filename)
{
    const fs::path dir = WorkingSetDir();
    std::error_code ec;
    fs::create_directories(dir, ec);
    if (ec)
    {
        log_error("[sectorstore] can't create %s: %s", dir.string().c_str(), ec.message().c_str());
        return nullptr;
    }
    return std::fopen((dir / filename).string().c_str(), "wb");
}

void Clear()
{
    const fs::path dir = WorkingSetDir();
    int32_t removed = 0;
    std::error_code ec;
    for (fs::directory_iterator it(dir, ec), end; !ec && it != end; it.increment(ec))
    {
        std::error_code removeError;
        if (IsSectorFile(*it) && fs::remove(it->path(), removeError))
            ++removed;
    }
    log_info("[sectorstore] cleared working set %s (%d sectors)", dir.string().c_str(), removed);
}

int32_t ImportFrom(const fs::path& dir)
{
    const int32_t copied = CopySectorFiles(dir, WorkingSetDir());
    log_info("[sectorstore] imported %d sectors from %s", copied, dir.string().c_str());
    return copied;
}

int32_t ExportTo(const fs::path& dir)
{
    const int32_t copied = CopySectorFiles(WorkingSetDir(), dir);
    log_info("[sectorstore] exported %d sectors to %s", copied, dir.string().c_str());
    return copied;
}

}  // namespace SectorStore
