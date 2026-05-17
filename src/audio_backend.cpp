// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *         audio_backend.cpp - miniaudio facade implementation           *
// *************************************************************************
//
// This is the one TU that builds the miniaudio + stb_vorbis bodies.
// Anyone else just sees the audio:: facade in audio_backend.h.
//
// *************************************************************************

#include "audio_backend.h"

#include "logging.h"
#include "wavedata.h"   // for WAVEFORMATEX

#include <atomic>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <mutex>

// ---- stb_vorbis (header-only, then impl below the miniaudio impl) -------
// miniaudio detects vorbis through STB_VORBIS_INCLUDE_STB_VORBIS_H which
// the stb_vorbis.c header sets when included with STB_VORBIS_HEADER_ONLY.
#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"

// ---- miniaudio implementation -------------------------------------------
// Tighten: we don't need flac/mp3 (no game data in those formats), we
// only ship the engine API (not the low-level device API directly), and
// we skip the WAV writer (we never write audio).
#define MA_NO_FLAC
#define MA_NO_MP3
#define MA_NO_ENCODING
#define MA_NO_GENERATION
// Default null backend off so missing-device errors are not silent.

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

// ---- stb_vorbis full bodies (single instantiation) ----------------------
#undef STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"

// ============================================================ static state

namespace {

constexpr float kDsDbMin   = -10000.0f;   // DirectSound DSBVOLUME_MIN
constexpr float kDsPanFull = 10000.0f;    // DirectSound DSBPAN_RIGHT (or |LEFT|)

// One global engine + a sfx group and a music group sitting under it.
// Master volume rides on the engine itself.
struct State {
    bool             initialized = false;
    bool             init_ok     = false;
    ma_engine        engine{};
    ma_sound_group   sfx_group{};
    ma_sound_group   music_group{};

    // Per-source ma_audio_buffer needs to outlive the ma_sound that drives
    // it, and the PCM bytes need to outlive both. Sources may be Duplicated
    // (multiple Sources share the same PCM); ref-counted through SharedPCM.
    std::mutex       sources_mtx;          // guards source bookkeeping

    // The active music track (one at a time).
    bool             music_loaded = false;
    ma_sound         music{};
};

State& state() { static State s; return s; }

// Reference-counted PCM bytes shared between Duplicate()'d sources.
struct SharedPCM {
    std::atomic<int> refcount{1};
    ma_format        format    = ma_format_unknown;
    ma_uint32        channels  = 0;
    ma_uint32        sample_rate = 0;
    ma_uint64        frames    = 0;        // total frames (not bytes)
    uint8_t*         bytes     = nullptr;  // owned; freed when refcount hits 0
    size_t           bytes_len = 0;
};

float DsDbToLinear(int32_t volume_ds) {
    // DirectSound dB units are hundredths of a dB attenuation: 0 = full,
    // -10000 = silence. linear = 10^(db/2000) since db is hundredths.
    if (volume_ds <= static_cast<int32_t>(kDsDbMin)) return 0.0f;
    if (volume_ds >= 0)                              return 1.0f;
    return std::pow(10.0f, static_cast<float>(volume_ds) / 2000.0f);
}

float DsPanToFloat(int32_t pan_ds) {
    // -10000..+10000 → -1..+1
    if (pan_ds < -kDsPanFull) return -1.0f;
    if (pan_ds > +kDsPanFull) return +1.0f;
    return static_cast<float>(pan_ds) / kDsPanFull;
}

ma_format WaveFormatToMa(const tWAVEFORMATEX* w) {
    // Only uncompressed PCM is supported. The 1998 path went through ACM
    // for non-PCM (compressed mu-law, ADPCM, etc.); we don't carry that
    // forward — the shipped data is PCM.
    if (!w || w->wFormatTag != 1 /* WAVE_FORMAT_PCM */) return ma_format_unknown;
    if (w->wBitsPerSample == 8)  return ma_format_u8;   // unsigned 8-bit
    if (w->wBitsPerSample == 16) return ma_format_s16;  // signed 16-bit
    if (w->wBitsPerSample == 24) return ma_format_s24;
    if (w->wBitsPerSample == 32) return ma_format_f32;  // rare; assume float
    return ma_format_unknown;
}

}  // namespace

// =========================================================== Source object

namespace audio {

// One Source = (shared PCM) + (one ma_audio_buffer view over it) +
// (one ma_sound voice bound to the buffer). Voice can be played, stopped,
// queried. Duplicate() bumps the SharedPCM refcount and builds a new
// buffer+voice over the same bytes.
struct Source {
    SharedPCM*       pcm = nullptr;
    ma_audio_buffer  buffer{};
    bool             buffer_inited = false;
    ma_sound         sound{};
    bool             sound_inited  = false;
    bool             looping       = false;
};

}  // namespace audio

// ============================================================ helpers

namespace {

void ReleasePCM(SharedPCM* pcm) {
    if (!pcm) return;
    if (pcm->refcount.fetch_sub(1) == 1) {
        std::free(pcm->bytes);
        delete pcm;
    }
}

// Build a fresh ma_audio_buffer + ma_sound on `src`. Assumes pcm is set.
// Returns true on success.
bool BindSoundToBuffer(audio::Source* src) {
    if (!src || !src->pcm) return false;

    ma_audio_buffer_config bcfg = ma_audio_buffer_config_init(
        src->pcm->format,
        src->pcm->channels,
        src->pcm->frames,
        src->pcm->bytes,
        /*allocationCallbacks*/ nullptr);
    if (ma_audio_buffer_init(&bcfg, &src->buffer) != MA_SUCCESS) {
        log_error("audio: ma_audio_buffer_init failed");
        return false;
    }
    src->buffer_inited = true;

    if (ma_sound_init_from_data_source(&state().engine,
                                       &src->buffer,
                                       /*flags*/ 0,
                                       &state().sfx_group,
                                       &src->sound) != MA_SUCCESS) {
        log_error("audio: ma_sound_init_from_data_source failed");
        ma_audio_buffer_uninit(&src->buffer);
        src->buffer_inited = false;
        return false;
    }
    src->sound_inited = true;

    // We drive distance/pan from the 1998 math, so disable miniaudio's
    // spatializer. (The polish pass can re-enable + drop the manual
    // CalcPan path for Revisited mode.)
    ma_sound_set_spatialization_enabled(&src->sound, MA_FALSE);
    ma_sound_set_looping(&src->sound, src->looping ? MA_TRUE : MA_FALSE);
    return true;
}

void TearDownSound(audio::Source* src) {
    if (!src) return;
    if (src->sound_inited)  { ma_sound_uninit(&src->sound);          src->sound_inited  = false; }
    if (src->buffer_inited) { ma_audio_buffer_uninit(&src->buffer);  src->buffer_inited = false; }
}

}  // namespace

// ============================================================ lifecycle

bool audio::Init() {
    State& s = state();
    if (s.initialized) return s.init_ok;
    s.initialized = true;

    ma_engine_config cfg = ma_engine_config_init();
    // Default channel count / sample rate (whatever the device offers).
    if (ma_engine_init(&cfg, &s.engine) != MA_SUCCESS) {
        log_error("audio: ma_engine_init failed — audio disabled");
        s.init_ok = false;
        return false;
    }

    if (ma_sound_group_init(&s.engine, 0, nullptr, &s.sfx_group)   != MA_SUCCESS ||
        ma_sound_group_init(&s.engine, 0, nullptr, &s.music_group) != MA_SUCCESS) {
        log_error("audio: ma_sound_group_init failed — audio disabled");
        ma_engine_uninit(&s.engine);
        s.init_ok = false;
        return false;
    }

    ma_engine_listener_set_position(&s.engine, 0, 0.0f, 0.0f, 0.0f);

    // Force the playback device to a started state. ma_engine_init normally
    // auto-starts the device, but make it explicit so we never end up with
    // a configured-but-stopped device silently producing nothing.
    ma_device* dev = ma_engine_get_device(&s.engine);
    if (dev && ma_device_get_state(dev) != ma_device_state_started) {
        const ma_result sr = ma_device_start(dev);
        log_info("audio: device start (was state=%d) -> %d",
                 static_cast<int>(ma_device_get_state(dev)),
                 static_cast<int>(sr));
    }

    const char* dev_name = (dev && dev->playback.name[0]) ? dev->playback.name : "<unnamed>";
    const int   dev_state = dev ? static_cast<int>(ma_device_get_state(dev)) : -1;
    log_info("audio: miniaudio %u.%u.%u up — %uHz, %u ch, device='%s', state=%d, master_vol=%.2f",
             MA_VERSION_MAJOR, MA_VERSION_MINOR, MA_VERSION_REVISION,
             ma_engine_get_sample_rate(&s.engine),
             ma_engine_get_channels(&s.engine),
             dev_name,
             dev_state,
             ma_engine_get_volume(&s.engine));
    s.init_ok = true;
    return true;
}

void audio::Shutdown() {
    State& s = state();
    if (!s.initialized) return;
    if (s.init_ok) {
        if (s.music_loaded) { ma_sound_uninit(&s.music); s.music_loaded = false; }
        ma_sound_group_uninit(&s.music_group);
        ma_sound_group_uninit(&s.sfx_group);
        ma_engine_uninit(&s.engine);
    }
    s.initialized = false;
    s.init_ok     = false;
}

bool audio::Functioning() { return state().init_ok; }

// ============================================================ mixer

void audio::SetMasterVolume(float v) {
    State& s = state();
    if (!s.init_ok) return;
    ma_engine_set_volume(&s.engine, v);
}

void audio::SetSfxVolume(float v) {
    State& s = state();
    if (!s.init_ok) return;
    ma_sound_group_set_volume(&s.sfx_group, v);
}

void audio::SetMusicVolume(float v) {
    State& s = state();
    if (!s.init_ok) return;
    ma_sound_group_set_volume(&s.music_group, v);
}

void audio::PauseAll() {
    State& s = state();
    if (!s.init_ok) return;
    ma_engine_stop(&s.engine);
}

void audio::UnpauseAll() {
    State& s = state();
    if (!s.init_ok) return;
    ma_engine_start(&s.engine);
}

// ============================================================ sources

audio::Source* audio::CreateSourceFromPCM(const tWAVEFORMATEX* format,
                                          const uint8_t* data,
                                          uint32_t bytes,
                                          bool looping) {
    State& s = state();
    if (!s.init_ok || !format || !data || !bytes) return nullptr;

    ma_format ma_fmt = WaveFormatToMa(format);
    if (ma_fmt == ma_format_unknown || format->nChannels == 0 ||
        format->nSamplesPerSec == 0 || format->nBlockAlign == 0) {
        log_warn("audio: unsupported PCM format tag=%u ch=%u bps=%u",
                 format->wFormatTag, format->nChannels, format->wBitsPerSample);
        return nullptr;
    }

    auto* pcm = new SharedPCM();
    pcm->format      = ma_fmt;
    pcm->channels    = format->nChannels;
    pcm->sample_rate = format->nSamplesPerSec;
    pcm->frames      = bytes / format->nBlockAlign;
    pcm->bytes_len   = bytes;
    pcm->bytes       = static_cast<uint8_t*>(std::malloc(bytes));
    if (!pcm->bytes) { delete pcm; return nullptr; }
    std::memcpy(pcm->bytes, data, bytes);

    auto* src = new Source();
    src->pcm     = pcm;
    src->looping = looping;
    if (!BindSoundToBuffer(src)) {
        ReleasePCM(pcm);
        delete src;
        return nullptr;
    }
    return src;
}

audio::Source* audio::DuplicateSource(Source* in) {
    State& s = state();
    if (!s.init_ok || !in || !in->pcm) return nullptr;
    in->pcm->refcount.fetch_add(1);

    auto* src = new Source();
    src->pcm     = in->pcm;
    src->looping = in->looping;
    if (!BindSoundToBuffer(src)) {
        ReleasePCM(in->pcm);
        delete src;
        return nullptr;
    }
    return src;
}

void audio::DestroySource(Source* src) {
    if (!src) return;
    TearDownSound(src);
    ReleasePCM(src->pcm);
    delete src;
}

bool audio::PlaySource(Source* src, int32_t volume_ds, int32_t freq_hz, int32_t pan_ds) {
    if (!src || !src->sound_inited) return false;

    ma_sound_set_volume(&src->sound, DsDbToLinear(volume_ds));
    ma_sound_set_pan   (&src->sound, DsPanToFloat(pan_ds));

    if (freq_hz > 0 && src->pcm && src->pcm->sample_rate > 0) {
        const float ratio = static_cast<float>(freq_hz) /
                            static_cast<float>(src->pcm->sample_rate);
        ma_sound_set_pitch(&src->sound, ratio);
    } else {
        ma_sound_set_pitch(&src->sound, 1.0f);
    }

    ma_sound_seek_to_pcm_frame(&src->sound, 0);
    return ma_sound_start(&src->sound) == MA_SUCCESS;
}

void audio::StopSource(Source* src) {
    if (!src || !src->sound_inited) return;
    ma_sound_stop(&src->sound);
}

bool audio::IsPlaying(const Source* src) {
    if (!src || !src->sound_inited) return false;
    // ma_sound_is_playing wants a non-const pointer; the operation itself
    // is conceptually const so the cast is fine.
    return ma_sound_is_playing(const_cast<ma_sound*>(&src->sound)) == MA_TRUE;
}

bool audio::IsLooping(const Source* src) {
    if (!src) return false;
    return src->looping;
}

void audio::SetLooping(Source* src, bool looping) {
    if (!src) return;
    src->looping = looping;
    if (src->sound_inited) {
        ma_sound_set_looping(&src->sound, looping ? MA_TRUE : MA_FALSE);
    }
}

void audio::SetSourceVolumePan(Source* src, int32_t volume_ds, int32_t pan_ds) {
    if (!src || !src->sound_inited) return;
    ma_sound_set_volume(&src->sound, DsDbToLinear(volume_ds));
    ma_sound_set_pan   (&src->sound, DsPanToFloat(pan_ds));
}

// ============================================================ music

bool audio::MusicPlayFile(const char* path, bool looping) {
    State& s = state();
    if (!s.init_ok || !path) return false;

    if (s.music_loaded) { ma_sound_uninit(&s.music); s.music_loaded = false; }

    ma_uint32 flags = MA_SOUND_FLAG_STREAM | MA_SOUND_FLAG_NO_SPATIALIZATION;
    if (ma_sound_init_from_file(&s.engine, path, flags,
                                &s.music_group, nullptr,
                                &s.music) != MA_SUCCESS) {
        log_warn("audio: music open failed: %s", path);
        return false;
    }
    s.music_loaded = true;
    ma_sound_set_looping(&s.music, looping ? MA_TRUE : MA_FALSE);
    if (ma_sound_start(&s.music) != MA_SUCCESS) {
        ma_sound_uninit(&s.music);
        s.music_loaded = false;
        log_warn("audio: music start failed: %s", path);
        return false;
    }
    log_info("audio: music playing %s (loop=%d)", path, looping ? 1 : 0);
    return true;
}

void audio::MusicStop() {
    State& s = state();
    if (!s.init_ok || !s.music_loaded) return;
    ma_sound_stop(&s.music);
    ma_sound_uninit(&s.music);
    s.music_loaded = false;
}

bool audio::MusicPlaying() {
    State& s = state();
    if (!s.init_ok || !s.music_loaded) return false;
    return ma_sound_is_playing(&s.music) == MA_TRUE;
}

void audio::MusicSetVolume(float v) {
    State& s = state();
    if (!s.init_ok || !s.music_loaded) return;
    ma_sound_set_volume(&s.music, v);
}

bool audio::PlayOneShot(const char* path) {
    State& s = state();
    if (!s.init_ok || !path) return false;
    const ma_result r = ma_engine_play_sound(&s.engine, path, nullptr);
    log_info("audio: PlayOneShot('%s') -> %d", path, static_cast<int>(r));
    return r == MA_SUCCESS;
}

float audio::GetMasterVolume() {
    State& s = state();
    if (!s.init_ok) return 0.0f;
    return ma_engine_get_volume(&s.engine);
}
