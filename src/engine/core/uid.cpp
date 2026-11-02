// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   uid.cpp - the persistent 64-bit identity of nodes and assets        *
// *************************************************************************

#include "engine/core/uid.h"

#include <chrono>

namespace {

// splitmix64, seeded once from the clock. Never the game's random stream.
uint64_t& UidState()
{
    static uint64_t state = uint64_t(std::chrono::steady_clock::now().time_since_epoch().count()) ^
                            uint64_t(std::chrono::system_clock::now().time_since_epoch().count());
    return state;
}

} // namespace

TUid TUid::Random()
{
    uint64_t& s = UidState();
    uint64_t v = 0;
    while (v == 0)
    {
        s += 0x9E3779B97F4A7C15ull;
        v = UidMix(s);
    }
    return TUid(v);
}

TUid TUid::Derived(uint64_t a, uint64_t b, uint64_t c, uint64_t d)
{
    uint64_t v = UidMix(a);
    v = UidMix(v ^ UidMix(b + 1));
    v = UidMix(v ^ UidMix(c + 2));
    v = UidMix(v ^ UidMix(d + 3));
    return TUid(v ? v : 1);
}
