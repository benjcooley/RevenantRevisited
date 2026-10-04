// revsmk — clean-room Smacker (SMK2) decoder.  zlib license, see LICENSE.
//
// Header parse, palette, video-block decode and audio decode. See
// docs/cinematics/SMK_FORMAT.md for the byte-level format reference.
#include "revsmk.h"

#include <algorithm>
#include <cstdio>
#include <cstring>

#include "bitreader.h"
#include "huffman.h"

namespace revsmk {

namespace {

// Run lengths selected by bits 2..7 of a Type code.
constexpr int kSizeTable[64] = {
    1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
    17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
    33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
    49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 128, 256, 512, 1024, 2048,
};

// 6-bit palette component -> 8-bit. Equivalent to (v<<2)|(v>>4).
constexpr uint8_t kPal6to8[64] = {
    0x00, 0x04, 0x08, 0x0C, 0x10, 0x14, 0x18, 0x1C, 0x20, 0x24, 0x28, 0x2C, 0x30, 0x34, 0x38, 0x3C,
    0x41, 0x45, 0x49, 0x4D, 0x51, 0x55, 0x59, 0x5D, 0x61, 0x65, 0x69, 0x6D, 0x71, 0x75, 0x79, 0x7D,
    0x82, 0x86, 0x8A, 0x8E, 0x92, 0x96, 0x9A, 0x9E, 0xA2, 0xA6, 0xAA, 0xAE, 0xB2, 0xB6, 0xBA, 0xBE,
    0xC3, 0xC7, 0xCB, 0xCF, 0xD3, 0xD7, 0xDB, 0xDF, 0xE3, 0xE7, 0xEB, 0xEF, 0xF3, 0xF7, 0xFB, 0xFF,
};

constexpr int kHeaderSize = 104;

uint32_t rd32(const uint8_t* p) {
    return uint32_t(p[0]) | (uint32_t(p[1]) << 8) | (uint32_t(p[2]) << 16) | (uint32_t(p[3]) << 24);
}

} // namespace

// ---------------------------------------------------------------------------
// Impl
// ---------------------------------------------------------------------------

struct Decoder::Impl {
    std::vector<uint8_t> bytes;        // whole file

    int bw = 0, bh = 0;                // block grid dimensions
    bool isSMK4 = false;

    std::vector<size_t> frameOff;      // payload offset per stored frame
    std::vector<size_t> frameLen;      // payload length per stored frame
    std::vector<uint8_t> frameType;    // per-frame chunk flags

    BigTree mmap, mclr, full, type;
    size_t treesStart = 0;             // offset of the trees blob
    uint32_t treesSize = 0;
    uint32_t mmapSize = 0, mclrSize = 0, fullSize = 0, typeSize = 0;

    AudioInfo audioInfo[7];
    std::vector<uint8_t> audioOut[7];  // decoded PCM for the current frame

    // Build (or rebuild, resetting the MRU caches) the four picture trees.
    bool BuildTrees() {
        BitReader tr(bytes.data() + treesStart, treesSize);
        return mmap.Build(tr, mmapSize) && mclr.Build(tr, mclrSize) &&
               full.Build(tr, fullSize) && type.Build(tr, typeSize);
    }

    void DecodePalette(const uint8_t* data, size_t len, uint8_t pal[256 * 3]);
    void DecodeVideo(const uint8_t* data, size_t len, int width, int height,
                     std::vector<uint8_t>& image);
    void DecodeAudio(int track, const uint8_t* data, size_t len, uint32_t unpackedSize);
};

// ---------------------------------------------------------------------------
// Palette
// ---------------------------------------------------------------------------

void Decoder::Impl::DecodePalette(const uint8_t* data, size_t len, uint8_t pal[256 * 3]) {
    uint8_t oldPal[256 * 3];
    std::memcpy(oldPal, pal, sizeof(oldPal));

    size_t p = 0;
    int out = 0; // palette entry index being written
    while (out < 256 && p < len) {
        const uint8_t op = data[p++];
        if (op & 0x80) {
            // Keep (count) entries from the previous palette at the same index.
            int count = (op & 0x7F) + 1;
            for (int k = 0; k < count && out < 256; ++k, ++out)
                std::memcpy(&pal[out * 3], &oldPal[out * 3], 3);
        } else if (op & 0x40) {
            // Copy (count) entries from the previous palette starting at src.
            int count = (op & 0x3F) + 1;
            if (p >= len) break;
            int src = data[p++];
            for (int k = 0; k < count && out < 256; ++k, ++out) {
                if (src + k < 256) std::memcpy(&pal[out * 3], &oldPal[(src + k) * 3], 3);
            }
        } else {
            // Set one entry from 6-bit R,G,B.
            if (p + 1 >= len) break;
            const uint8_t r = op & 0x3F;
            const uint8_t g = data[p++] & 0x3F;
            const uint8_t b = data[p++] & 0x3F;
            pal[out * 3 + 0] = kPal6to8[r];
            pal[out * 3 + 1] = kPal6to8[g];
            pal[out * 3 + 2] = kPal6to8[b];
            ++out;
        }
    }
}

// ---------------------------------------------------------------------------
// Video
// ---------------------------------------------------------------------------

void Decoder::Impl::DecodeVideo(const uint8_t* data, size_t len, int width, int height,
                                std::vector<uint8_t>& image) {
    // Smacker resets the four trees' MRU caches at the start of every frame.
    type.ResetCache();
    mmap.ResetCache();
    mclr.ResetCache();
    full.ResetCache();

    BitReader br(data, len);
    const int blocks = bw * bh;
    int blk = 0;

    while (blk < blocks && !br.Overrun()) {
        const int t = type.GetCode(br);
        const int kind = t & 3;
        const int run = kSizeTable[(t >> 2) & 0x3F];

        if (kind == 2) { // VOID — keep previous frame's pixels
            blk += run;
            continue;
        }

        for (int r = 0; r < run && blk < blocks; ++r, ++blk) {
            const int bx = (blk % bw) * 4;
            const int by = (blk / bw) * 4;

            switch (kind) {
            case 0: { // MONO — two colors, 4x4 bit mask
                const int clr = mclr.GetCode(br);
                int map = mmap.GetCode(br);
                const uint8_t hi = uint8_t(clr >> 8);
                const uint8_t lo = uint8_t(clr & 0xFF);
                for (int yy = 0; yy < 4; ++yy) {
                    const int py = by + yy;
                    for (int xx = 0; xx < 4; ++xx) {
                        const uint8_t v = (map & 1) ? hi : lo;
                        map >>= 1;
                        const int px = bx + xx;
                        if (px < width && py < height) image[py * width + px] = v;
                    }
                }
                break;
            }
            case 1: { // FULL — 16 explicit pixels, two codes per row
                for (int yy = 0; yy < 4; ++yy) {
                    const int py = by + yy;
                    int pix = full.GetCode(br);
                    const uint8_t c2 = uint8_t(pix & 0xFF), c3 = uint8_t(pix >> 8);
                    pix = full.GetCode(br);
                    const uint8_t c0 = uint8_t(pix & 0xFF), c1 = uint8_t(pix >> 8);
                    const uint8_t cols[4] = {c0, c1, c2, c3};
                    for (int xx = 0; xx < 4; ++xx) {
                        const int px = bx + xx;
                        if (px < width && py < height) image[py * width + px] = cols[xx];
                    }
                }
                break;
            }
            case 3: { // SOLID — single color from the Type code's high byte
                const uint8_t v = uint8_t(t >> 8);
                for (int yy = 0; yy < 4; ++yy) {
                    const int py = by + yy;
                    for (int xx = 0; xx < 4; ++xx) {
                        const int px = bx + xx;
                        if (px < width && py < height) image[py * width + px] = v;
                    }
                }
                break;
            }
            default: break;
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Audio (DPCM + per-byte-lane Huffman). Secondary to video; the byte-lane and
// channel ordering below is the verify-and-adjust spot if audio comes out wrong.
// ---------------------------------------------------------------------------

void Decoder::Impl::DecodeAudio(int track, const uint8_t* data, size_t len, uint32_t unpackedSize) {
    // Huffman-DPCM audio. Stream layout: DataPresent, IsStereo, Is16Bits, then
    // one 8-bit tree per sample byte, then the base (starting) bytes, then a
    // Huffman-packed stream of per-byte deltas. See docs/cinematics/SMK_FORMAT.md.
    std::vector<uint8_t>& out = audioOut[track];
    out.clear();
    if (len == 0) return;

    BitReader br(data, len);
    if (br.ReadBit() == 0) return; // DataPresent == 0: no audio this chunk

    const bool stereo = br.ReadBit() != 0;
    const bool is16 = br.ReadBit() != 0;
    const int chans = stereo ? 2 : 1;
    const int nTrees = chans * (is16 ? 2 : 1); // 1, 2, 2 or 4

    // One tree per sample byte, stored in data-byte order: for 16-bit stereo
    // that is L-low, L-high, R-low, R-high.
    SmallTree tree[4];
    for (int i = 0; i < nTrees; ++i)
        if (!tree[i].Build(br)) return;

    out.reserve(unpackedSize);

    if (is16) {
        // Bases: high byte first, right channel before left.
        int pred[2] = {0, 0};
        for (int c = chans - 1; c >= 0; --c) {
            const int hi = br.ReadBits(8);
            const int lo = br.ReadBits(8);
            pred[c] = int16_t((hi << 8) | lo);
        }
        // First emitted sample is the base; output is left then right.
        for (int c = 0; c < chans; ++c) {
            out.push_back(uint8_t(pred[c] & 0xFF));
            out.push_back(uint8_t((pred[c] >> 8) & 0xFF));
        }
        // Deltas: low byte then high, left channel then right.
        while (out.size() < unpackedSize && !br.Overrun()) {
            for (int c = 0; c < chans; ++c) {
                const int lo = tree[c * 2 + 0].Decode(br);
                const int hi = tree[c * 2 + 1].Decode(br);
                pred[c] = int16_t(pred[c] + int16_t(lo | (hi << 8)));
                out.push_back(uint8_t(pred[c] & 0xFF));
                out.push_back(uint8_t((pred[c] >> 8) & 0xFF));
            }
        }
    } else {
        // 8-bit unsigned PCM; bases right channel before left.
        int pred[2] = {0, 0};
        for (int c = chans - 1; c >= 0; --c) pred[c] = br.ReadBits(8);
        for (int c = 0; c < chans; ++c) out.push_back(uint8_t(pred[c]));
        while (out.size() < unpackedSize && !br.Overrun()) {
            for (int c = 0; c < chans; ++c) {
                pred[c] = uint8_t(pred[c] + tree[c].Decode(br));
                out.push_back(uint8_t(pred[c]));
            }
        }
    }
    if (out.size() > unpackedSize) out.resize(unpackedSize);
}

// ---------------------------------------------------------------------------
// Decoder
// ---------------------------------------------------------------------------

Decoder::Decoder() : impl_(std::make_unique<Impl>()) {}
Decoder::~Decoder() = default;

std::unique_ptr<Decoder> Decoder::OpenFile(const std::string& path, std::string* err) {
    FILE* f = std::fopen(path.c_str(), "rb");
    if (!f) { if (err) *err = "cannot open file: " + path; return nullptr; }
    std::fseek(f, 0, SEEK_END);
    long n = std::ftell(f);
    std::fseek(f, 0, SEEK_SET);
    if (n <= 0) { std::fclose(f); if (err) *err = "empty file"; return nullptr; }
    std::vector<uint8_t> bytes(static_cast<size_t>(n));
    size_t got = std::fread(bytes.data(), 1, bytes.size(), f);
    std::fclose(f);
    if (got != bytes.size()) { if (err) *err = "short read"; return nullptr; }
    return OpenMemory(std::move(bytes), err);
}

std::unique_ptr<Decoder> Decoder::OpenMemory(std::vector<uint8_t> bytes, std::string* err) {
    auto setErr = [&](const char* m) { if (err) *err = m; };
    if (bytes.size() < kHeaderSize) { setErr("file smaller than header"); return nullptr; }

    const uint8_t* h = bytes.data();
    const bool smk2 = std::memcmp(h, "SMK2", 4) == 0;
    const bool smk4 = std::memcmp(h, "SMK4", 4) == 0;
    if (!smk2 && !smk4) { setErr("not a Smacker file (bad signature)"); return nullptr; }
    if (smk4) { setErr("SMK4 not supported (Revenant assets are SMK2)"); return nullptr; }

    auto dec = std::unique_ptr<Decoder>(new Decoder());
    Impl& im = *dec->impl_;
    im.bytes = std::move(bytes);
    const uint8_t* p = im.bytes.data();

    Info& info = dec->info_;
    info.width = int(rd32(p + 0x04));
    info.height = int(rd32(p + 0x08));
    const int frames = int(rd32(p + 0x0C));
    const int32_t frameRate = int32_t(rd32(p + 0x10));
    const uint32_t flags = rd32(p + 0x14);
    const uint32_t treesSize = rd32(p + 0x34);
    const uint32_t mmapSize = rd32(p + 0x38);
    const uint32_t mclrSize = rd32(p + 0x3C);
    const uint32_t fullSize = rd32(p + 0x40);
    const uint32_t typeSize = rd32(p + 0x44);

    if (info.width <= 0 || info.height <= 0 || frames <= 0) { setErr("bad dimensions/frame count"); return nullptr; }
    if (info.width > 8192 || info.height > 8192) { setErr("implausible dimensions"); return nullptr; }

    info.frameCount = frames;
    info.ringFrame = (flags & 1) != 0;
    info.yInterlaced = (flags & 2) != 0;
    info.yDoubled = (flags & 4) != 0;
    if (frameRate > 0) info.fps = 1000.0 / frameRate;
    else if (frameRate < 0) info.fps = 100000.0 / (-frameRate);
    else info.fps = 10.0;

    for (int t = 0; t < 7; ++t) {
        const uint32_t rate = rd32(p + 0x48 + t * 4);
        AudioInfo& a = info.audio[t];
        a.present = (rate & 0x40000000u) != 0;
        a.compressed = (rate & 0x80000000u) != 0;
        a.bytesPerSample = (rate & 0x20000000u) ? 2 : 1;
        a.channels = (rate & 0x10000000u) ? 2 : 1;
        a.sampleRate = int(rate & 0x00FFFFFFu);
        a.maxUnpackedBytes = rd32(p + 0x18 + t * 4);
        im.audioInfo[t] = a;
    }

    const int storedFrames = frames + (info.ringFrame ? 1 : 0);

    // Frame size + type arrays, then the trees blob, then frame payloads.
    size_t off = kHeaderSize;
    if (im.bytes.size() < off + size_t(storedFrames) * 5 + treesSize) {
        setErr("file truncated (header arrays/trees)"); return nullptr;
    }

    std::vector<uint32_t> frameSize(storedFrames);
    for (int i = 0; i < storedFrames; ++i) { frameSize[i] = rd32(p + off) & ~3u; off += 4; }
    im.frameType.resize(storedFrames);
    for (int i = 0; i < storedFrames; ++i) { im.frameType[i] = p[off]; off += 1; }

    const size_t treesStart = off;
    off += treesSize;
    const size_t frameDataStart = off;

    // Self-check: declared frame sizes must account for exactly the rest of the file.
    size_t sumSizes = 0;
    for (uint32_t s : frameSize) sumSizes += s;
    if (frameDataStart + sumSizes != im.bytes.size()) {
        // Not fatal for playback of leading frames, but signals a parse error.
        std::fprintf(stderr,
            "revsmk: frame-size sum mismatch (data starts %zu, sum %zu, file %zu)\n",
            frameDataStart, sumSizes, im.bytes.size());
    }

    im.frameOff.resize(storedFrames);
    im.frameLen.resize(storedFrames);
    size_t fp = frameDataStart;
    for (int i = 0; i < storedFrames; ++i) {
        im.frameOff[i] = fp;
        im.frameLen[i] = frameSize[i];
        fp += frameSize[i];
        if (fp > im.bytes.size()) im.frameLen[i] = im.bytes.size() - im.frameOff[i];
    }

    // Build the four picture trees from one continuous bit stream over the blob.
    im.treesStart = treesStart;
    im.treesSize = treesSize;
    im.mmapSize = mmapSize;
    im.mclrSize = mclrSize;
    im.fullSize = fullSize;
    im.typeSize = typeSize;
    if (!im.BuildTrees()) { setErr("failed to build Huffman trees"); return nullptr; }

    im.bw = (info.width + 3) / 4;
    im.bh = (info.height + 3) / 4;

    dec->image_.assign(size_t(info.width) * info.height, 0);
    std::memset(dec->palette_, 0, sizeof(dec->palette_));
    dec->curFrame_ = -1;
    return dec;
}

void Decoder::Rewind() {
    // The trees' MRU caches carry running state, so rebuild them to reset. Cheap
    // relative to decoding, and cinematics rarely rewind.
    impl_->BuildTrees();
    std::fill(image_.begin(), image_.end(), uint8_t(0));
    std::memset(palette_, 0, sizeof(palette_));
    curFrame_ = -1;
}

bool Decoder::DecodeNextFrame() {
    if (curFrame_ + 1 >= info_.frameCount) return false;
    ++curFrame_;

    Impl& im = *impl_;
    const uint8_t* base = im.bytes.data() + im.frameOff[curFrame_];
    size_t remaining = im.frameLen[curFrame_];
    const uint8_t flags = im.frameType[curFrame_];

    for (auto& a : im.audioOut) a.clear();

    // 1) Palette chunk.
    if (flags & 1) {
        if (remaining >= 1) {
            const size_t chunkBytes = size_t(base[0]) * 4;
            const size_t avail = chunkBytes <= remaining ? chunkBytes : remaining;
            if (avail >= 1) im.DecodePalette(base + 1, avail - 1, palette_);
            base += avail;
            remaining -= avail;
        }
    }

    // 2) Audio chunks for present tracks, in order 0..6.
    for (int t = 0; t < 7; ++t) {
        if (!(flags & (1u << (t + 1)))) continue;
        if (remaining < 4) break;
        const uint32_t chunkSize = rd32(base);
        const size_t avail = chunkSize <= remaining ? chunkSize : remaining;
        if (avail >= 8) {
            const uint32_t unpacked = rd32(base + 4);
            im.DecodeAudio(t, base + 8, avail - 8, unpacked);
        }
        base += avail;
        remaining -= avail;
    }

    // 3) Video bitstream is whatever remains.
    im.DecodeVideo(base, remaining, info_.width, info_.height, image_);
    return true;
}

void Decoder::BlitRGBA(uint8_t* dst, int dstStrideBytes) const {
    const int w = info_.width;
    const int h = info_.height;
    const bool dbl = info_.yInterlaced || info_.yDoubled;
    for (int y = 0; y < h; ++y) {
        const uint8_t* srcRow = &image_[size_t(y) * w];
        const int outRows = dbl ? 2 : 1;
        for (int rep = 0; rep < outRows; ++rep) {
            uint8_t* d = dst + size_t(y * outRows + rep) * dstStrideBytes;
            for (int x = 0; x < w; ++x) {
                const uint8_t idx = srcRow[x];
                d[x * 4 + 0] = palette_[idx * 3 + 0];
                d[x * 4 + 1] = palette_[idx * 3 + 1];
                d[x * 4 + 2] = palette_[idx * 3 + 2];
                d[x * 4 + 3] = 255;
            }
        }
    }
}

const std::vector<uint8_t>& Decoder::AudioData(int track) const {
    static const std::vector<uint8_t> empty;
    if (track < 0 || track >= 7) return empty;
    return impl_->audioOut[track];
}

} // namespace revsmk
