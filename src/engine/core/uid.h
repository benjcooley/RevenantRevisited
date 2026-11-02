// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   uid.h - the persistent 64-bit identity of nodes and assets          *
// *************************************************************************
//
// A serialized link is a uid, never a runtime soft-ref id
// (docs/OBJECT_MODEL_PLAN.md phase 4, "Identity for links"). A node gets
// one at construction (random) unless its record or document says
// otherwise; a retail-loaded object's is derived from where it sits
// (level, sector, slot) so it is the same on every load; a prefab's
// authored uids are remapped per stamp. Assets use the asset cache's
// AssetUid, the same 64 bits.
//
// Random uids come from a generator of their own, never the game's
// random stream, so a spawn's uid can never move a replay.
//
// *************************************************************************

#pragma once

#include <cstdint>

struct TUid
{
    uint64_t value = 0;

    constexpr TUid() = default;
    constexpr explicit TUid(uint64_t v) : value(v) {}

    [[nodiscard]] constexpr bool IsValid() const { return value != 0; }
    [[nodiscard]] constexpr explicit operator bool() const { return IsValid(); }
    [[nodiscard]] constexpr bool operator==(const TUid& o) const { return value == o.value; }
    [[nodiscard]] constexpr bool operator!=(const TUid& o) const { return value != o.value; }

    // A fresh uid from the uid generator (not the game's random stream)
    [[nodiscard]] static TUid Random();
    // The same uid for the same inputs, every run: a mix of up to four
    // integers (a retail object's level, sector x, sector y and slot)
    [[nodiscard]] static TUid Derived(uint64_t a, uint64_t b = 0, uint64_t c = 0, uint64_t d = 0);
};

// splitmix64's output function: the mixer both Random and Derived use.
[[nodiscard]] constexpr uint64_t UidMix(uint64_t z)
{
    z += 0x9E3779B97F4A7C15ull;
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
    return z ^ (z >> 31);
}
