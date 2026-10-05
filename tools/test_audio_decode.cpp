// *************************************************************************
// *                         Cinematix Revenant                            *
// *                      Revenant Revisited 2026                          *
// *       test_audio_decode.cpp - sound file decoding and lengths         *
// *************************************************************************
//
// Pins audio::DecodeToPCM16 / audio::DecodedLengthMs, which give
// TSoundPlayer::SampleLengthMs (retail 0x0049c640) a voice's length with or
// without audio output. WAV and MP3 inputs are built here, so the expected
// lengths are known exactly; with REVENANT_DATA_PATH set, the opening's
// first voice is measured from the shipped Ahkuilon.rvm too. No playback
// device is opened. Run via build/test_audio_decode after build.
// *************************************************************************

#include "../src/audio_backend.h"
#include "../src/wavedata.h"

#include <miniz.h>

#include <gtest/gtest.h>

#include <cstdlib>
#include <string>
#include <vector>

namespace {

void PutU16(std::vector<uint8_t>& b, uint16_t v)
{
    b.push_back(static_cast<uint8_t>(v));
    b.push_back(static_cast<uint8_t>(v >> 8));
}

void PutU32(std::vector<uint8_t>& b, uint32_t v)
{
    PutU16(b, static_cast<uint16_t>(v));
    PutU16(b, static_cast<uint16_t>(v >> 16));
}

void PutTag(std::vector<uint8_t>& b, const char* tag)
{
    b.insert(b.end(), tag, tag + 4);
}

// A PCM RIFF/WAVE file of `frames` frames of a ramp.
std::vector<uint8_t> MakeWav(uint32_t rate, uint16_t channels, uint16_t bits, uint32_t frames)
{
    const uint16_t block = static_cast<uint16_t>(channels * bits / 8);
    const uint32_t data_bytes = frames * block;
    std::vector<uint8_t> b;
    PutTag(b, "RIFF");
    PutU32(b, 36 + data_bytes);
    PutTag(b, "WAVE");
    PutTag(b, "fmt ");
    PutU32(b, 16);
    PutU16(b, 1);  // PCM
    PutU16(b, channels);
    PutU32(b, rate);
    PutU32(b, rate * block);
    PutU16(b, block);
    PutU16(b, bits);
    PutTag(b, "data");
    PutU32(b, data_bytes);
    for (uint32_t i = 0; i < data_bytes; ++i)
        b.push_back(static_cast<uint8_t>(i * 7));
    return b;
}

// `count` MPEG-1 Layer III frames, mono, 44.1 kHz, 128 kbit/s, no padding:
// a valid header and all-zero side info and main data, which decodes to
// 1152 frames of silence each.
std::vector<uint8_t> MakeSilentMp3(int count)
{
    constexpr size_t kFrameBytes = 144 * 128000 / 44100;   // 417
    std::vector<uint8_t> b;
    for (int f = 0; f < count; ++f) {
        const size_t at = b.size();
        b.resize(at + kFrameBytes, 0);
        b[at + 0] = 0xFF;   // sync
        b[at + 1] = 0xFB;   // sync, MPEG-1, Layer III, no CRC
        b[at + 2] = 0x90;   // 128 kbit/s, 44.1 kHz, no padding
        b[at + 3] = 0xC0;   // mono
    }
    return b;
}

// A file from a shipped pack, or empty.
std::vector<uint8_t> ReadPackEntry(const std::string& pack, const char* entry)
{
    std::vector<uint8_t> out;
    mz_zip_archive zip{};
    if (!mz_zip_reader_init_file(&zip, pack.c_str(), 0))
        return out;
    const int index = mz_zip_reader_locate_file(&zip, entry, nullptr, 0);
    mz_zip_archive_file_stat st;
    if (index >= 0 && mz_zip_reader_file_stat(&zip, static_cast<mz_uint>(index), &st)) {
        out.resize(static_cast<size_t>(st.m_uncomp_size));
        if (!mz_zip_reader_extract_to_mem(&zip, static_cast<mz_uint>(index), out.data(), out.size(), 0))
            out.clear();
    }
    mz_zip_reader_end(&zip);
    return out;
}

} // namespace

TEST(DecodedLength, WavIsFramesOverRate)
{
    const auto wav = MakeWav(22050, 1, 16, 11025);
    EXPECT_EQ(audio::DecodedLengthMs(wav.data(), wav.size()), 500u);
}

TEST(DecodedLength, TruncatesToWholeMilliseconds)
{
    // 1000 frames at 22050 Hz = 45.35 ms.
    const auto wav = MakeWav(22050, 2, 8, 1000);
    EXPECT_EQ(audio::DecodedLengthMs(wav.data(), wav.size()), 45u);
}

TEST(DecodedLength, Mp3CountsEveryFrame)
{
    // 20 frames * 1152 / 44100 Hz = 522.4 ms.
    const auto mp3 = MakeSilentMp3(20);
    EXPECT_EQ(audio::DecodedLengthMs(mp3.data(), mp3.size()), 522u);
}

TEST(DecodedLength, GarbageIsZero)
{
    const std::vector<uint8_t> junk(1000, 0x5A);
    EXPECT_EQ(audio::DecodedLengthMs(junk.data(), junk.size()), 0u);
    EXPECT_EQ(audio::DecodedLengthMs(nullptr, 0), 0u);
}

TEST(DecodeToPCM16, WavKeepsRateChannelsAndFrames)
{
    const auto wav = MakeWav(22050, 2, 8, 1000);
    WAVEFORMATEX fmt{};
    std::vector<uint8_t> pcm;
    ASSERT_TRUE(audio::DecodeToPCM16(wav.data(), wav.size(), &fmt, pcm));
    EXPECT_EQ(fmt.wFormatTag, 1);
    EXPECT_EQ(fmt.nChannels, 2);
    EXPECT_EQ(fmt.nSamplesPerSec, 22050u);
    EXPECT_EQ(fmt.wBitsPerSample, 16);
    EXPECT_EQ(fmt.nBlockAlign, 4);
    EXPECT_EQ(fmt.nAvgBytesPerSec, 22050u * 4);
    EXPECT_EQ(pcm.size(), 1000u * 4);
}

TEST(DecodeToPCM16, Mp3MatchesItsLength)
{
    const auto mp3 = MakeSilentMp3(20);
    WAVEFORMATEX fmt{};
    std::vector<uint8_t> pcm;
    ASSERT_TRUE(audio::DecodeToPCM16(mp3.data(), mp3.size(), &fmt, pcm));
    EXPECT_EQ(fmt.nChannels, 1);
    EXPECT_EQ(fmt.nSamplesPerSec, 44100u);
    EXPECT_EQ(pcm.size(), 20u * 1152 * 2);
}

TEST(DecodeToPCM16, GarbageFails)
{
    const std::vector<uint8_t> junk(1000, 0x5A);
    WAVEFORMATEX fmt{};
    std::vector<uint8_t> pcm;
    EXPECT_FALSE(audio::DecodeToPCM16(junk.data(), junk.size(), &fmt, pcm));
    EXPECT_TRUE(pcm.empty());
}

// The opening's first two lines (keep.s SardokR): Locke's I1LOC00 is 47
// frames, Sardok's I1SAR00 128 (44.1 kHz stereo, 1152 samples a frame).
TEST(ShippedVoices, OpeningLines)
{
    const char* data = std::getenv("REVENANT_DATA_PATH");
    if (!data)
        GTEST_SKIP() << "REVENANT_DATA_PATH not set";
    const std::string pack = std::string(data) + "/Modules/Ahkuilon.rvm";

    const auto loc = ReadPackEntry(pack, "Sound/english/i1loc00.mp3");
    const auto sar = ReadPackEntry(pack, "Sound/english/i1sar00.mp3");
    if (loc.empty() || sar.empty())
        GTEST_SKIP() << "no Ahkuilon.rvm voices under " << data;
    EXPECT_EQ(audio::DecodedLengthMs(loc.data(), loc.size()), 1227u);
    EXPECT_EQ(audio::DecodedLengthMs(sar.data(), sar.size()), 3343u);
}
