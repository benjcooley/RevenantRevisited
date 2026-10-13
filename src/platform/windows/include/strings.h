// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   strings.h - POSIX <strings.h> for Windows                           *
// *************************************************************************
//
// On the include path for Windows builds only (CMakeLists.txt), so code
// written to POSIX finds its case-insensitive compares under their POSIX
// names. The Windows C runtime has them as _stricmp / _strnicmp.

#pragma once

#include <cstddef>
#include <string.h>

inline int strcasecmp(const char* a, const char* b)
{
    return _stricmp(a, b);
}

inline int strncasecmp(const char* a, const char* b, size_t n)
{
    return _strnicmp(a, b, n);
}
