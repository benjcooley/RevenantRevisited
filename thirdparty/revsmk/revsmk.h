// revsmk — clean-room Smacker (SMK2) decoder.  zlib license, see LICENSE.
//
// Self-contained decoder for the Smacker SMK2 video format used by Revenant's
// FMV / credits assets. Depends only on the C++17 standard library. See
// docs/cinematics/SMK_FORMAT.md for the format reference and clean-room
// provenance.
#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace revsmk {

struct AudioInfo {
    bool present = false;
    bool compressed = false;
    int channels = 0;       // 1 = mono, 2 = stereo
    int bytesPerSample = 0; // 1 = 8-bit, 2 = 16-bit
    int sampleRate = 0;     // Hz
    uint32_t maxUnpackedBytes = 0;
};

struct Info {
    int width = 0;
    int height = 0;       // coded height; display height doubles under a scale flag
    int frameCount = 0;   // logical frames to play (excludes the ring frame)
    double fps = 0.0;
    bool ringFrame = false;
    bool yInterlaced = false;
    bool yDoubled = false;
    AudioInfo audio[7];

    // Height after applying the Y scale flags — what the picture should occupy.
    int displayHeight() const { return (yInterlaced || yDoubled) ? height * 2 : height; }
};

class Decoder {
public:
    ~Decoder();
    Decoder(const Decoder&) = delete;
    Decoder& operator=(const Decoder&) = delete;

    // Open from a file path or an in-memory copy. On failure returns nullptr and,
    // if `err` is non-null, fills it with the reason.
    static std::unique_ptr<Decoder> OpenFile(const std::string& path, std::string* err = nullptr);
    static std::unique_ptr<Decoder> OpenMemory(std::vector<uint8_t> bytes, std::string* err = nullptr);

    const Info& GetInfo() const { return info_; }

    // Decode the next frame in sequence. Frames are delta-coded, so they must be
    // decoded in order; use Rewind() to start over. Returns false at end.
    bool DecodeNextFrame();
    void Rewind();
    int CurrentFrame() const { return curFrame_; } // -1 before the first decode
    bool AtEnd() const { return curFrame_ + 1 >= info_.frameCount; }

    // Most recently decoded frame.
    const uint8_t* IndexedImage() const { return image_.data(); } // width*height indices
    const uint8_t* PaletteRGB() const { return palette_; }        // 256*3 bytes

    // Expand the current frame to RGBA8 into `dst`, applying the Y scale flags.
    // `dst` must hold displayHeight() rows of `dstStrideBytes`.
    void BlitRGBA(uint8_t* dst, int dstStrideBytes) const;

    // Decoded PCM for the current frame on the given track (0..6). Interleaved,
    // native byte order, matching AudioInfo. Empty when the track has no data.
    const std::vector<uint8_t>& AudioData(int track) const;

private:
    Decoder();
    struct Impl;
    std::unique_ptr<Impl> impl_;

    Info info_;
    std::vector<uint8_t> image_;     // width*height palette indices (current frame)
    uint8_t palette_[256 * 3] = {};
    int curFrame_ = -1;
};

} // namespace revsmk
