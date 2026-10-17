// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   memfile.cpp - a FILE over memory on Windows                         *
// *************************************************************************
//
// The Windows C runtime has no FILE over memory or over callbacks. The
// nearest thing is a temporary file opened short-lived ("T": the cache
// keeps it rather than writing it out) and deleted on close ("D").

#if defined(_WIN32)

#include "../memfile.h"

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <windows.h>

namespace rev_platform
{

FILE* OpenMemoryFile(std::vector<uint8_t> bytes)
{
    wchar_t dir[MAX_PATH + 1];
    wchar_t path[MAX_PATH + 1];
    const DWORD n = GetTempPathW(MAX_PATH + 1, dir);
    if (n == 0 || n > MAX_PATH || GetTempFileNameW(dir, L"rev", 0, path) == 0)
        return nullptr;

    FILE* fp = _wfopen(path, L"w+bTD");
    if (!fp)
    {
        DeleteFileW(path);
        return nullptr;
    }
    if (std::fwrite(bytes.data(), 1, bytes.size(), fp) != bytes.size() ||
        std::fseek(fp, 0, SEEK_SET) != 0)
    {
        std::fclose(fp);
        return nullptr;
    }
    return fp;
}

} // namespace rev_platform

#endif
