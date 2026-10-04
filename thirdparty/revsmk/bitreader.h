// revsmk — clean-room Smacker (SMK2) decoder.  zlib license, see LICENSE.
//
// LSB-first bit reader over a byte buffer. Smacker reads bits starting from the
// least-significant bit of each byte and advances to the next byte after bit 7,
// so a multi-bit field has its first bit in the lowest output position. See
// docs/cinematics/SMK_FORMAT.md.
#pragma once

#include <cstddef>
#include <cstdint>

namespace revsmk {

class BitReader {
public:
    BitReader() = default;
    BitReader(const uint8_t* data, size_t size) : data_(data), bitCount_(size * 8) {}

    // Read one bit (0/1). Past end-of-stream reads return 0 and set overrun.
    int ReadBit() {
        if (pos_ >= bitCount_) { overrun_ = true; return 0; }
        const int bit = (data_[pos_ >> 3] >> (pos_ & 7)) & 1;
        ++pos_;
        return bit;
    }

    // Read `n` bits LSB-first (n in 0..32). Bit 0 of the result is read first.
    uint32_t ReadBits(int n) {
        uint32_t v = 0;
        for (int i = 0; i < n; ++i)
            v |= static_cast<uint32_t>(ReadBit()) << i;
        return v;
    }

    bool Overrun() const { return overrun_; }
    size_t BitPos() const { return pos_; }
    bool AtEnd() const { return pos_ >= bitCount_; }

private:
    const uint8_t* data_ = nullptr;
    size_t bitCount_ = 0;
    size_t pos_ = 0;
    bool overrun_ = false;
};

} // namespace revsmk
