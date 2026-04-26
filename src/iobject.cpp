// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *               iobject.cpp - Reflection table impl                     *
// *************************************************************************

#include "iobject.h"

#include <cctype>
#include <cstring>

namespace {

bool iequals(const char* a, const char* b)
{
    if (!a || !b) return a == b;
    while (*a && *b) {
        const int ca = std::tolower(static_cast<unsigned char>(*a));
        const int cb = std::tolower(static_cast<unsigned char>(*b));
        if (ca != cb) return false;
        ++a; ++b;
    }
    return *a == '\0' && *b == '\0';
}

} // namespace

TReflectionTable::TReflectionTable(const char* class_name, int32_t class_id,
                                   std::vector<SPropertyDesc> props)
    : name_(class_name), id_(class_id), props_(std::move(props))
{}

const SPropertyDesc* TReflectionTable::FindProperty(const char* name) const
{
    if (!name) return nullptr;
    for (const auto& p : props_)
        if (iequals(p.name, name))
            return &p;
    return nullptr;
}
