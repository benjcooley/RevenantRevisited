// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *           audio_backend.h - miniaudio facade for TSound/TSoundPlayer  *
// *************************************************************************
//
// Backend for the 1998 TSound / TSoundPlayer API. Wraps miniaudio so the
// rest of the engine talks to an `audio::Source*` instead of an
// LPDIRECTSOUNDBUFFER.
//
// Design:
//   - One global `ma_engine` owned by this TU. Brought up by audio::Init()
//     at boot, torn down by audio::Shutdown() at exit.
//   - Three sound groups under the engine: master, sfx, music. The Classic
//     mixer maps DirectSound dB / pan units onto these.
//   - A `Source` owns the decoded PCM (via ma_audio_buffer) plus the
//     currently-playing voice (a single ma_sound bound to the sfx group).
//     Duplicate() creates an independent Source that shares the PCM bytes
//     by reference-counting the underlying buffer so we can play the same
//     SFX overlapped without re-decoding.
//   - Spatialization is OFF by default. We feed pan/volume from the 1998
//     CalcPan / CalcDirectionalVol path. The G4 polish pass can flip the
//     engine spatializer on for Revisited mode.
//   - Music is a separate one-shot streaming Source on the music group,
//     loaded from .ogg (stb_vorbis built into miniaudio).
//
// Unit conventions match the 1998 API so callers don't need to know about
// the backend:
//   - volume_ds: hundredths of a dB attenuation. 0 = full, -10000 = silence.
//   - pan_ds:    -10000 = full-left, 0 = center, +10000 = full-right.
//   - freq_hz:   0 = use source's native sample rate; nonzero overrides.
//
// *************************************************************************

#pragma once

#include <cstdint>

struct tWAVEFORMATEX;   // sound.h's WAVEFORMATEX

namespace audio {

// ---- Lifecycle ----------------------------------------------------------

// Silence audio at the OS device level. When true, Init() will NOT create
// a miniaudio engine / playback device — Functioning() stays false, all
// Play* / SetMusic / SetSfx calls become no-ops. Used by --headless so
// agent-driven test runs don't pump music + SFX onto the user's speakers.
// Must be called BEFORE Init(). Idempotent.
void SetSilenced(bool silenced);
bool IsSilenced();

// Bring up the audio engine. Idempotent — second call is a no-op and
// returns the previous result. Safe to call from boot before any
// TSound/TSoundPlayer use. If SetSilenced(true) was called first, this
// returns false immediately (no device, no thread, no output).
bool Init();

// Tear it down. Idempotent. Stops any in-flight sounds; the engine's
// playback thread drains before this returns.
void Shutdown();

// True if Init() succeeded and Shutdown() has not been called since.
bool Functioning();

// ---- Mixer --------------------------------------------------------------

// Master/sfx/music volume in linear 0..1. The factory default is 1.0
// (passthrough). Apply call after a Settings change.
void SetMasterVolume(float v);
void SetSfxVolume(float v);
void SetMusicVolume(float v);

// Mute / unmute the whole output without changing per-group volumes.
// Used by TSoundPlayer::Pause / Unpause to halt all playback during a
// game pause.
void PauseAll();
void UnpauseAll();

// ---- Source (one decoded SFX, possibly with one playing voice) ----------

struct Source;  // opaque

// Build a Source from in-memory PCM. The 1998 path passes a
// WAVEFORMATEX + raw bytes (uncompressed PCM after ACM decode); we copy
// the bytes into the Source so the caller can free its buffer.
//
// Returns nullptr if the format is unsupported (non-PCM, or channels !=
// 1/2, or bits != 8/16).
Source* CreateSourceFromPCM(const tWAVEFORMATEX* format,
                            const uint8_t* data,
                            uint32_t bytes,
                            bool looping);

// Independent copy of `src`. The underlying PCM is shared (ref-counted)
// so duplication is cheap. Used by TSoundPlayer::Play when a still-
// playing SFX is triggered again.
Source* DuplicateSource(Source* src);

void DestroySource(Source* src);

// Play / Stop / Status.
//
// PlaySource resets the voice to position 0 and starts it.
// freq_hz == 0 means "use the source's native sample rate."
// volume_ds and pan_ds use 1998 DirectSound units (see header preamble).
bool PlaySource(Source* src, int32_t volume_ds, int32_t freq_hz, int32_t pan_ds);
void StopSource(Source* src);

bool IsPlaying(const Source* src);
bool IsLooping(const Source* src);
void SetLooping(Source* src, bool looping);

// Live volume/pan update on a currently-playing voice. No-op if the
// voice isn't started yet.
void SetSourceVolumePan(Source* src, int32_t volume_ds, int32_t pan_ds);

// ---- Music --------------------------------------------------------------

// Start the music track. Replaces any currently-playing music. Path can
// be a .ogg or .wav. Looping plays to the end and seeks back to 0.
// Returns false if the file cannot be opened or decoded.
bool MusicPlayFile(const char* path, bool looping);

// Stop music (drains the buffer; does not block).
void MusicStop();

bool MusicPlaying();

// 0..1 linear, scaled on top of SetMusicVolume's group setting.
void MusicSetVolume(float v);

// ---- Diagnostics --------------------------------------------------------

// Fire-and-forget one-shot. Plays the file once at the engine's master
// volume on the default device. Used by --test=audio to validate the
// playback path without going through Source/group bookkeeping.
bool PlayOneShot(const char* path);

// Report current engine master volume (linear 0..1). Returns 0 if the
// backend is not up.
float GetMasterVolume();

}  // namespace audio
