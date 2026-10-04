// smkdump — standalone verification tool for the revsmk Smacker decoder.
//
// Prints stream info and dumps frames to PNG (via miniz) and audio to WAV so the
// decoder can be eyeballed against the real Revenant .SMK assets without any
// engine dependency.
//
//   smkdump <file.smk> [--out DIR] [--frames N] [--every K] [--no-wav]
//
//   --out DIR    where to write PNG/WAV (default ".")
//   --frames N   stop after decoding N frames (default: all)
//   --every K    write a PNG every K decoded frames (default: 30; 0 = none)
//   --no-wav     skip audio WAV output
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include "miniz.h"
#include "revsmk.h"

static bool WritePNG(const std::string& path, const uint8_t* rgba, int w, int h) {
    size_t len = 0;
    void* png = tdefl_write_image_to_png_file_in_memory_ex(rgba, w, h, 4, &len, 6, MZ_FALSE);
    if (!png) return false;
    FILE* f = std::fopen(path.c_str(), "wb");
    bool ok = f && std::fwrite(png, 1, len, f) == len;
    if (f) std::fclose(f);
    mz_free(png);
    return ok;
}

static void WriteLE(std::vector<uint8_t>& v, uint32_t x, int n) {
    for (int i = 0; i < n; ++i) v.push_back(uint8_t((x >> (8 * i)) & 0xFF));
}

static bool WriteWAV(const std::string& path, const std::vector<uint8_t>& pcm,
                     int channels, int bytesPerSample, int sampleRate) {
    if (pcm.empty()) return false;
    std::vector<uint8_t> h;
    const uint32_t dataLen = uint32_t(pcm.size());
    const uint32_t byteRate = uint32_t(sampleRate) * channels * bytesPerSample;
    h.insert(h.end(), {'R', 'I', 'F', 'F'});
    WriteLE(h, 36 + dataLen, 4);
    h.insert(h.end(), {'W', 'A', 'V', 'E', 'f', 'm', 't', ' '});
    WriteLE(h, 16, 4);                       // fmt chunk size
    WriteLE(h, 1, 2);                        // PCM
    WriteLE(h, uint32_t(channels), 2);
    WriteLE(h, uint32_t(sampleRate), 4);
    WriteLE(h, byteRate, 4);
    WriteLE(h, uint32_t(channels * bytesPerSample), 2); // block align
    WriteLE(h, uint32_t(bytesPerSample * 8), 2);        // bits per sample
    h.insert(h.end(), {'d', 'a', 't', 'a'});
    WriteLE(h, dataLen, 4);
    FILE* f = std::fopen(path.c_str(), "wb");
    if (!f) return false;
    bool ok = std::fwrite(h.data(), 1, h.size(), f) == h.size() &&
              std::fwrite(pcm.data(), 1, pcm.size(), f) == pcm.size();
    std::fclose(f);
    return ok;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::fprintf(stderr, "usage: %s <file.smk> [--out DIR] [--frames N] [--every K] [--no-wav]\n", argv[0]);
        return 2;
    }
    std::string path = argv[1];
    std::string outDir = ".";
    int maxFrames = -1;
    int every = 30;
    bool wav = true;
    for (int i = 2; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--out" && i + 1 < argc) outDir = argv[++i];
        else if (a == "--frames" && i + 1 < argc) maxFrames = std::atoi(argv[++i]);
        else if (a == "--every" && i + 1 < argc) every = std::atoi(argv[++i]);
        else if (a == "--no-wav") wav = false;
        else { std::fprintf(stderr, "unknown arg: %s\n", a.c_str()); return 2; }
    }

    std::string err;
    auto dec = revsmk::Decoder::OpenFile(path, &err);
    if (!dec) { std::fprintf(stderr, "open failed: %s\n", err.c_str()); return 1; }

    const revsmk::Info& info = dec->GetInfo();
    std::printf("SMK2 %dx%d (display %d), %d frames @ %.2f fps%s%s%s\n",
                info.width, info.height, info.displayHeight(), info.frameCount, info.fps,
                info.ringFrame ? " [ring]" : "", info.yInterlaced ? " [interlace]" : "",
                info.yDoubled ? " [ydouble]" : "");
    for (int t = 0; t < 7; ++t) {
        const revsmk::AudioInfo& a = info.audio[t];
        if (a.present)
            std::printf("  audio[%d]: %d ch, %d-bit, %d Hz%s\n", t, a.channels,
                        a.bytesPerSample * 8, a.sampleRate, a.compressed ? ", compressed" : "");
    }

    const int dispH = info.displayHeight();
    std::vector<uint8_t> rgba(size_t(info.width) * dispH * 4);
    std::vector<uint8_t> audio0;
    const revsmk::AudioInfo& a0 = info.audio[0];

    int decoded = 0, written = 0;
    while (dec->DecodeNextFrame()) {
        const int f = dec->CurrentFrame();
        if (every > 0 && (f % every == 0)) {
            dec->BlitRGBA(rgba.data(), info.width * 4);
            char name[512];
            std::snprintf(name, sizeof(name), "%s/frame_%05d.png", outDir.c_str(), f);
            if (WritePNG(name, rgba.data(), info.width, dispH)) { std::printf("wrote %s\n", name); ++written; }
        }
        if (wav && a0.present) {
            const std::vector<uint8_t>& pcm = dec->AudioData(0);
            audio0.insert(audio0.end(), pcm.begin(), pcm.end());
        }
        ++decoded;
        if (maxFrames > 0 && decoded >= maxFrames) break;
    }
    std::printf("decoded %d frames, wrote %d PNGs\n", decoded, written);

    if (wav && a0.present && !audio0.empty()) {
        std::string wpath = outDir + "/audio0.wav";
        if (WriteWAV(wpath, audio0, a0.channels, a0.bytesPerSample, a0.sampleRate))
            std::printf("wrote %s (%zu bytes PCM)\n", wpath.c_str(), audio0.size());
    }
    return 0;
}
