// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  headless_window.cpp - CLI parsing + cross-platform stub             *
// *************************************************************************

#include "headless_window.h"

#include <cstring>

namespace HeadlessWindow {

namespace {
bool g_active = false;

const char* StripPrefix(const char* s)
{
    while (*s == '-' || *s == '/') ++s;
    return s;
}
} // namespace

bool ParseArgs(int argc, char** argv)
{
    for (int i = 1; i < argc; ++i)
    {
        if (!argv[i]) continue;
        const char* p = StripPrefix(argv[i]);
        if (std::strcmp(p, "headless") == 0)
            g_active = true;
        else if (std::strcmp(p, "no-headless") == 0)
            g_active = false;
    }
    return g_active;
}

bool IsActive() { return g_active; }

#if !defined(__APPLE__)
// macOS impl lives in platform/macosx/headless_window.mm. Stub for other
// platforms so the build links — they'd need their own platform-specific
// hide path (X11 unmap on Linux, ShowWindow(SW_HIDE) on Win32, etc.).
void HideAllWindows() {}
#endif

} // namespace HeadlessWindow
