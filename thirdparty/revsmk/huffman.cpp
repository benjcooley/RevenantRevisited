// revsmk — clean-room Smacker (SMK2) decoder.  zlib license, see LICENSE.
#include "huffman.h"

namespace revsmk {

// Cap recursion so a hostile/truncated stream can't blow the C++ stack. A real
// byte Huffman tree is at most 256 leaves deep; big trees are similarly shallow.
static constexpr int kMaxTreeDepth = 512;

// ---------------------------------------------------------------------------
// SmallTree
// ---------------------------------------------------------------------------

int SmallTree::BuildNode(BitReader& br, size_t nodeBudget, int depth, bool& ok) {
    if (depth > kMaxTreeDepth || nodes_.size() >= nodeBudget) { ok = false; return -1; }

    if (br.ReadBit() == 0) {
        // Leaf: next 8 bits (LSB-first) reconstruct the byte value.
        const int idx = static_cast<int>(nodes_.size());
        Node n;
        n.leaf = true;
        n.value = static_cast<uint8_t>(br.ReadBits(8));
        nodes_.push_back(n);
        return idx;
    }

    // Internal node. Reserve our slot before recursing so child indices are
    // assigned in stream order.
    const int idx = static_cast<int>(nodes_.size());
    nodes_.push_back(Node{});
    const int l = BuildNode(br, nodeBudget, depth + 1, ok);
    const int r = BuildNode(br, nodeBudget, depth + 1, ok);
    nodes_[idx].left = l;
    nodes_[idx].right = r;
    return idx;
}

bool SmallTree::Build(BitReader& br, size_t nodeBudget) {
    nodes_.clear();
    nodes_.reserve(64);

    if (br.ReadBit() == 0) {
        // Absent tree: a single constant-0 leaf, consuming no body bits.
        Node n;
        n.leaf = true;
        n.value = 0;
        nodes_.push_back(n);
        root_ = 0;
        return true;
    }

    bool ok = true;
    root_ = BuildNode(br, nodeBudget, 0, ok);
    br.ReadBit(); // terminator
    return ok && root_ >= 0 && !br.Overrun();
}

int SmallTree::Decode(BitReader& br) const {
    int n = root_;
    if (n < 0) return 0;
    while (!nodes_[n].leaf) {
        n = br.ReadBit() ? nodes_[n].right : nodes_[n].left;
        if (n < 0) return 0; // malformed; bail safely
    }
    return nodes_[n].value;
}

// ---------------------------------------------------------------------------
// BigTree
// ---------------------------------------------------------------------------

int BigTree::BuildNode(BitReader& br, const SmallTree& lo, const SmallTree& hi,
                       const uint32_t esc[3], size_t nodeBudget, int depth, bool& ok) {
    if (depth > kMaxTreeDepth || nodes_.size() >= nodeBudget) { ok = false; return -1; }

    if (br.ReadBit() == 0) {
        // Leaf: 16-bit value = low byte | (high byte << 8), each via its tree.
        const int idx = static_cast<int>(nodes_.size());
        Node n;
        n.leaf = true;
        const int low = lo.Decode(br);
        const int high = hi.Decode(br);
        const int v = low | (high << 8);
        n.value = v;
        // The first three matching escape values mark the MRU cache slots; such
        // a leaf starts at 0 (empty cache) and is later filled by GetCode().
        if (static_cast<uint32_t>(v) == esc[0]) { n.value = 0; slot_[0] = idx; }
        else if (static_cast<uint32_t>(v) == esc[1]) { n.value = 0; slot_[1] = idx; }
        else if (static_cast<uint32_t>(v) == esc[2]) { n.value = 0; slot_[2] = idx; }
        nodes_.push_back(n);
        return idx;
    }

    const int idx = static_cast<int>(nodes_.size());
    nodes_.push_back(Node{});
    const int l = BuildNode(br, lo, hi, esc, nodeBudget, depth + 1, ok);
    const int r = BuildNode(br, lo, hi, esc, nodeBudget, depth + 1, ok);
    nodes_[idx].left = l;
    nodes_[idx].right = r;
    return idx;
}

bool BigTree::Build(BitReader& br, size_t nodeBudget) {
    nodes_.clear();
    // The header *Size field is a byte budget; node count is bounded well under
    // it, but keep a generous cap with headroom for the build itself.
    const size_t budget = nodeBudget ? nodeBudget * 2 + 16 : (1u << 21);
    nodes_.reserve(1024);

    auto makeEmpty = [&]() {
        // Outer Tag was 0: a constant-0 tree with three (unused) cache slots.
        for (int& s : slot_) {
            s = static_cast<int>(nodes_.size());
            Node n; n.leaf = true; n.value = 0;
            nodes_.push_back(n);
        }
        root_ = slot_[0];
    };

    if (br.ReadBit() == 0) { makeEmpty(); return true; }

    SmallTree lo, hi;
    if (!lo.Build(br) || !hi.Build(br)) return false;

    uint32_t esc[3];
    esc[0] = br.ReadBits(16);
    esc[1] = br.ReadBits(16);
    esc[2] = br.ReadBits(16);

    bool ok = true;
    root_ = BuildNode(br, lo, hi, esc, budget, 0, ok);
    br.ReadBit(); // trailing terminator bit

    // Any escape value that never appeared in the tree still needs a cache cell.
    for (int& s : slot_) {
        if (s < 0) {
            s = static_cast<int>(nodes_.size());
            Node n; n.leaf = true; n.value = 0;
            nodes_.push_back(n);
        }
    }
    return ok && root_ >= 0;
}

void BigTree::ResetCache() {
    for (int s : slot_)
        if (s >= 0) nodes_[s].value = 0;
}

int BigTree::GetCode(BitReader& br) {
    int n = root_;
    if (n < 0) return 0;
    while (!nodes_[n].leaf) {
        n = br.ReadBit() ? nodes_[n].right : nodes_[n].left;
        if (n < 0) return 0;
    }
    const int v = nodes_[n].value;
    // Promote v to the front of the 3-value MRU cache.
    if (v != nodes_[slot_[0]].value) {
        nodes_[slot_[2]].value = nodes_[slot_[1]].value;
        nodes_[slot_[1]].value = nodes_[slot_[0]].value;
        nodes_[slot_[0]].value = v;
    }
    return v;
}

} // namespace revsmk
