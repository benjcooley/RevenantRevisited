// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   retailadler32.h - Adler-32 as retail computes it                    *
// *************************************************************************
//
// The sector state hash's checksum (0x0056ff60 init / 0x0056ff80 update;
// SAVE_GAME.md §11.6): the bytes are signed chars, and the sums are reduced
// mod 65521 only after each 5552-byte chunk, with 32-bit wraparound in
// between.
#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>

class TRetailAdler32
{
  public:
    void Update(const void* data, size_t count)
    {
        const int8_t* bytes = static_cast<const int8_t*>(data);
        uint32_t a = state & 0xffff;
        uint32_t b = state >> 16;
        while (count > 0)
        {
            const size_t chunk = std::min<size_t>(count, kChunk);
            for (size_t i = 0; i < chunk; i++)
            {
                a += (uint32_t)(int32_t)bytes[i];
                b += a;
            }
            bytes += chunk;
            count -= chunk;
            a %= kModulus;
            b %= kModulus;
        }
        state = (b << 16) | a;
    }

    [[nodiscard]] uint32_t Value() const { return state; }

  private:
    static constexpr size_t   kChunk   = 5552;
    static constexpr uint32_t kModulus = 65521;
    uint32_t state = 1;
};
