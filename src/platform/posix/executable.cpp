// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   executable.cpp - the program's path and image, macOS and Linux      *
// *************************************************************************

#if defined(__APPLE__) || defined(__linux__)

#include "../executable.h"

#include <climits>
#include <cstdint>
#include <dlfcn.h>             // dladdr

#if defined(__APPLE__)
#  include <mach-o/dyld.h>     // _NSGetExecutablePath
#else
#  include <unistd.h>          // readlink
#endif

namespace rev_platform
{

std::filesystem::path ExecutableDir()
{
    namespace fs = std::filesystem;
    char raw[PATH_MAX];
#if defined(__APPLE__)
    uint32_t size = sizeof(raw);
    if (_NSGetExecutablePath(raw, &size) != 0)
        return {};
    // The path may run through symlinks or "..": resolve it.
    std::error_code ec;
    const fs::path path = fs::weakly_canonical(fs::path(raw), ec);
    return (ec ? fs::path(raw) : path).parent_path();
#else
    const ssize_t n = readlink("/proc/self/exe", raw, sizeof(raw) - 1);
    if (n <= 0)
        return {};
    raw[n] = 0;
    return fs::path(raw).parent_path();
#endif
}

uintptr_t ImageOffset(const void* address)
{
    Dl_info info;
    const uintptr_t at = reinterpret_cast<uintptr_t>(address);
    if (dladdr(address, &info) && info.dli_fbase)
        return at - reinterpret_cast<uintptr_t>(info.dli_fbase);
    return at;
}

} // namespace rev_platform

#endif
