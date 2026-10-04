// revsmk — clean-room Smacker (SMK2) decoder.  zlib license, see LICENSE.
//
// Smacker's two Huffman tree kinds. See docs/cinematics/SMK_FORMAT.md for the
// bitstream layout and the 3-value MRU cache that the big tree carries.
#pragma once

#include <cstdint>
#include <vector>

#include "bitreader.h"

namespace revsmk {

// 8-bit "small" tree. The packed form is: a presence Tag bit, then a recursive
// body (1 = node, 0 = leaf followed by an 8-bit value), then a terminator bit.
// An absent tree (Tag == 0) decodes to a constant 0 reading no further bits.
// Used to encode the low and high bytes of the big trees.
class SmallTree {
public:
    bool Build(BitReader& br, size_t nodeBudget = 1u << 16);
    int Decode(BitReader& br) const; // 0..255

private:
    struct Node {
        int left = -1;   // child indices; -1 on a leaf
        int right = -1;
        uint8_t value = 0;
        bool leaf = false;
    };
    std::vector<Node> nodes_;
    int root_ = -1;

    int BuildNode(BitReader& br, size_t nodeBudget, int depth, bool& ok);
};

// 16-bit "big" tree (MMap / MClr / Full / Type). Packed form: an outer Tag bit,
// then a low-byte SmallTree, a high-byte SmallTree, three 16-bit escape values,
// the recursive main body (each leaf is low|high<<8), then a terminator bit.
// The three escape values mark three leaves that form a most-recently-used
// cache; GetCode() promotes each decoded value to the front.
class BigTree {
public:
    bool Build(BitReader& br, size_t nodeBudget); // nodeBudget from the *Size header field
    int GetCode(BitReader& br);                   // 0..65535, mutates the MRU cache
    void ResetCache();                            // zero the 3 MRU cache slots

    bool Valid() const { return root_ >= 0; }

private:
    struct Node {
        int left = -1;
        int right = -1;
        int value = 0;
        bool leaf = false;
    };
    std::vector<Node> nodes_;
    int root_ = -1;
    int slot_[3] = {-1, -1, -1}; // node indices of the three MRU cache leaves

    int BuildNode(BitReader& br, const SmallTree& lo, const SmallTree& hi,
                  const uint32_t esc[3], size_t nodeBudget, int depth, bool& ok);
};

} // namespace revsmk
