// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *        platform_compat.cpp - portability shims with a body            *
// *************************************************************************

#include "engine/core/platform_compat.h"

#include <cassert>
#include <cctype>
#include <cstring>

#if !defined(_WIN32)

int32_t stricmp(const char* s1, const char* s2)
{
  assert(s1 != NULL);
  assert(s2 != NULL);

#ifdef HAVE_STRCASECMP
    return strcasecmp(s1, s2)
#else
    while (tolower((unsigned char) *s1) == tolower((unsigned char) *s2)) 
    {
        if (*s1 == '\0')
        {
            return 0;
        }
        s1++;
        s2++;
    }

    return (int) tolower((unsigned char) *s1) -
        (int) tolower((unsigned char) *s2);
#endif
}

int32_t strnicmp(const char* s1, const char* s2, size_t n)
{
    assert(s1 != nullptr);
    assert(s2 != nullptr);
    while (n-- > 0)
    {
        int c1 = tolower((unsigned char) *s1++);
        int c2 = tolower((unsigned char) *s2++);
        if (c1 != c2)
            return c1 - c2;
        if (c1 == 0)
            return 0;
    }
    return 0;
}

#endif // !_WIN32

void BEGIN_CRITICAL() {}
void END_CRITICAL() {}
