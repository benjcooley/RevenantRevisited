// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   executable.cpp - the program's path and image on Windows            *
// *************************************************************************

#if defined(_WIN32)

#include "engine/platform/executable.h"

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <windows.h>

#include <vector>

namespace rev_platform
{

std::filesystem::path ExecutableDir()
{
    // GetModuleFileNameW truncates silently; grow until the path fits.
    std::vector<wchar_t> raw(MAX_PATH);
    for (;;)
    {
        const DWORD n = GetModuleFileNameW(nullptr, raw.data(), DWORD(raw.size()));
        if (n == 0)
            return {};
        if (n < raw.size())
            return std::filesystem::path(raw.data()).parent_path();
        raw.resize(raw.size() * 2);
    }
}

uintptr_t ImageOffset(const void* address)
{
    HMODULE module = nullptr;
    const uintptr_t at = reinterpret_cast<uintptr_t>(address);
    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           static_cast<LPCWSTR>(address), &module))
        return at - reinterpret_cast<uintptr_t>(module);
    return at;
}

} // namespace rev_platform

#endif
