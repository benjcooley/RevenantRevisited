// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *           revisited.cpp - Runtime toggles for Revisited features      *
// *************************************************************************
//
// See revisited.h and revisited/README.md.
//
// *************************************************************************

#include "revisited.h"

#include "revutils.h"

namespace Revisited {

bool IsEnabled(const char* key)
{
    if (!key || !*key)
        return false;

    // Per-call section stamping matches the convention in src/ctrlmap.cpp
    // (INISetSection("Controls") before each batch of INIGet* calls).
    // The INI accessors are backed by the CSimpleIniA singleton in
    // revutils.cpp; this just routes the lookup to the right section.
    INISetSection("Revisited");

    // INIGetBool with nullptr yes/no falls back to the permissive token
    // set "yes on true 1" / "no off false 0" -- friendliest for a
    // hand-edited [Revisited] section.
    return INIGetBool(key, false, nullptr, nullptr);
}

}  // namespace Revisited
