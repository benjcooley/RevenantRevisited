// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *                  sound.cpp - Music and sound module                   *
// *************************************************************************
//
// 1998 TSound / TSoundPlayer rebuilt on the miniaudio facade in
// audio_backend.h. The DirectSound + MCI + winmm bodies that used to live
// here are now in attic/src/sound_directsound.cpp as a reference; this
// file is short on purpose.
//
// What survives from 1998:
//   - The TSoundPlayer ref-counted sound list (FindSound / Mount /
//     Unmount / Play / Stop by id), which is the API every game-side
//     caller (PLAY, character.cpp, effect_old.cpp, etc.) talks to.
//   - The CalcPan / CalcDirectionalVol mix law — flat stereo pan +
//     distance attenuation in DirectSound dB units. Audio backend
//     converts to linear under the hood.
//
// What changed:
//   - No CDOpen/CDClose/CDPlayTrack shim. Music goes through
//     audio::MusicPlayFile directly (callers updated, e.g. area.cpp).
//   - LPDIRECTSOUNDBUFFER replaced by audio::Source* in TSound::source.
//   - TSoundPlayer needs an explicit Initialize() now (called from
//     revmain boot) — the 1998 code paired init with the device-open
//     side effect of the first directsound call.
//
// *************************************************************************

#include "sound.h"

#include "audio_backend.h"
#include "file.h"
#include "logging.h"
#include "mainwnd.h"
#include "object.h"
#include "parse.h"
#include "resource.h"
#include "revutils.h"
#include "wavedata.h"

#include <cmath>
#include <cstdio>
#include <cstring>
#include <filesystem>

// Distance thresholds (world units) for the flat 2D mix law. < MINDIST is
// full-volume; > MAXDIST is silent; in between we linearly interpolate in
// the DirectSound dB attenuation range.
#define MINDIST 256
#define MAXDIST 1024

// SoundSystemOn — kept as a global gate so callers that pre-date Init can
// disable audio without crashing. Set by revmain config.
extern bool SoundSystemOn;

namespace {

// Read a little-endian 32-bit value out of a buffer.
inline uint32_t rd_u32_le(const uint8_t* p) {
    return static_cast<uint32_t>(p[0])       |
           static_cast<uint32_t>(p[1]) <<  8 |
           static_cast<uint32_t>(p[2]) << 16 |
           static_cast<uint32_t>(p[3]) << 24;
}
inline uint16_t rd_u16_le(const uint8_t* p) {
    return static_cast<uint16_t>(p[0]) |
           static_cast<uint16_t>(p[1]) << 8;
}

}  // namespace

// LoadWave — small portable PCM RIFF/WAVE reader. Replaces the 1998
// winmm mmio + ACM-decompress path. Only uncompressed PCM (fmtTag == 1)
// is accepted; the shipped data set is all PCM.
//
// Allocates one TWaveData via operator new[]'d byte buffer (the trailing
// flex array `uint8_t data[1]` carries the PCM samples). Caller frees
// with `delete wave`.
PTWaveData LoadWave(char* filename, int32_t volume, int32_t loopstart, int32_t loopend)
{
    if (!filename) return nullptr;

    // rev_fopen walks SavePath → overlay → RunPath → data root → mounted
    // archives (keyed by basename). That last step is what lets
    // "sound/effects/aura.wav" find aura.wav inside the resources.rvr
    // zip without us mounting an explicit per-subsystem virtual fs.
    FILE* fp = rev_fopen(filename, "rb");
    if (!fp) return nullptr;

    std::fseek(fp, 0, SEEK_END);
    const long flen_l = std::ftell(fp);
    std::fseek(fp, 0, SEEK_SET);
    if (flen_l < 44 /* RIFF header + minimal fmt/data */) {
        std::fclose(fp);
        return nullptr;
    }

    const size_t file_bytes = static_cast<size_t>(flen_l);
    uint8_t* file_buf = new uint8_t[file_bytes];
    const size_t got = std::fread(file_buf, 1, file_bytes, fp);
    std::fclose(fp);
    if (got != file_bytes) {
        delete[] file_buf;
        return nullptr;
    }

    if (std::memcmp(file_buf,     "RIFF", 4) != 0 ||
        std::memcmp(file_buf + 8, "WAVE", 4) != 0) {
        log_warn("audio: %s is not a RIFF/WAVE file", filename);
        delete[] file_buf;
        return nullptr;
    }

    // Walk subchunks until we find both 'fmt ' and 'data'.
    WAVEFORMATEX fmt{};
    const uint8_t* pcm_data = nullptr;
    uint32_t       pcm_len  = 0;
    bool           have_fmt = false;

    size_t cursor = 12;  // skip RIFF/size/WAVE
    while (cursor + 8 <= file_bytes) {
        const char* ckid = reinterpret_cast<const char*>(file_buf + cursor);
        const uint32_t cksz = rd_u32_le(file_buf + cursor + 4);
        const size_t body = cursor + 8;
        if (body + cksz > file_bytes) break;

        if (std::memcmp(ckid, "fmt ", 4) == 0 && cksz >= 16) {
            fmt.wFormatTag      = rd_u16_le(file_buf + body + 0);
            fmt.nChannels       = rd_u16_le(file_buf + body + 2);
            fmt.nSamplesPerSec  = rd_u32_le(file_buf + body + 4);
            fmt.nAvgBytesPerSec = rd_u32_le(file_buf + body + 8);
            fmt.nBlockAlign     = rd_u16_le(file_buf + body + 12);
            fmt.wBitsPerSample  = rd_u16_le(file_buf + body + 14);
            fmt.cbSize          = 0;
            have_fmt = true;
        } else if (std::memcmp(ckid, "data", 4) == 0) {
            pcm_data = file_buf + body;
            pcm_len  = cksz;
        }
        // Chunks are 2-byte aligned per the RIFF spec.
        cursor = body + cksz + (cksz & 1);
        if (have_fmt && pcm_data) break;
    }

    if (!have_fmt || !pcm_data || pcm_len == 0) {
        log_warn("audio: %s missing fmt/data chunk", filename);
        delete[] file_buf;
        return nullptr;
    }
    if (fmt.wFormatTag != 1 /* WAVE_FORMAT_PCM */) {
        log_warn("audio: %s is non-PCM (tag=%u) — only PCM is supported",
                 filename, fmt.wFormatTag);
        delete[] file_buf;
        return nullptr;
    }

    // Allocate a TWaveData big enough for the PCM payload. TWaveData
    // ends in `uint8_t data[1]` — extend the alloc by (pcm_len - 1).
    // ::operator new (non-array) so callers can `delete wave;` and have
    // it match.
    const size_t alloc_bytes = sizeof(TWaveData) + pcm_len - 1;
    auto* wave = static_cast<TWaveData*>(::operator new(alloc_bytes));
    wave->format    = fmt;
    wave->size      = pcm_len;
    wave->volume    = volume;
    wave->loopstart = loopstart;
    wave->loopend   = loopend;
    std::memcpy(wave->data, pcm_data, pcm_len);

    delete[] file_buf;
    return wave;
}

// *********************
// * TSound (one voice) *
// *********************

TSound::TSound()
{
    source = nullptr;
    looping = false;
    next = nullptr;
    sound_volume = 0;
    memset(&listener_pos, 0, sizeof(listener_pos));
    memset(&sound_pos,    0, sizeof(sound_pos));
    memset(&format,       0, sizeof(format));
    size = 0;
}

TSound::~TSound()
{
    if (source) {
        audio::DestroySource(source);
        source = nullptr;
    }
    if (next)
        delete next;
}

bool TSound::IsPlaying()
{
    return source && audio::IsPlaying(source);
}

bool TSound::IsLooping()
{
    if (source) return audio::IsLooping(source);
    return looping;
}

// Load from in-memory PCM (the only Load that actually has data to feed
// the backend; the file/resource Loads below funnel into this one).
PTSound TSound::Load(WAVEFORMATEX* format_in, uint32_t size_in, uint8_t* data_in, bool looping_in)
{
    if (!format_in || !data_in || size_in == 0)
        return nullptr;
    if (!SoundPlayer.Functioning())
        return nullptr;

    PTSound sound = new TSound;

    memcpy(&sound->format, format_in, sizeof(WAVEFORMATEX));
    sound->size    = size_in;
    sound->looping = looping_in;

    sound->source = audio::CreateSourceFromPCM(format_in, data_in, size_in, looping_in);
    if (!sound->source) {
        delete sound;
        return nullptr;
    }
    return sound;
}

// Load by name from the active sound directory (effects/<lang>). The
// file-system path is intentionally Posix-style; the 1998 backslashes are
// gone with the Win32 build.
PTSound TSound::Load(char* name, int32_t dirresid)
{
    if (!name || !SoundPlayer.Functioning())
        return nullptr;

    // rev_fopen handles all the path resolution: SavePath → overlay →
    // RunPath → data root → mounted ZIPs (keyed by basename). The
    // "sound/<sub>/<name>.wav" prefix matters for the on-disk fallback
    // chain but is collapsed to just "<name>.wav" when the lookup ends
    // up in resources.rvr.
    char filename[MAXPATHLEN];
    strncpyz(filename, "sound/", MAXPATHLEN);
    if (dirresid == DIRRESID_EFFECTDIR)
        strncatz(filename, "effects", MAXPATHLEN);
    else if (dirresid == DIRRESID_DIALOGDIR)
        strncatz(filename, Language.CStr(), MAXPATHLEN);
    strncatz(filename, "/",  MAXPATHLEN);
    strncatz(filename, name, MAXPATHLEN);
    strncatz(filename, ".wav", MAXPATHLEN);

    PTWaveData wave = ::LoadWave(filename);
    if (!wave) {
        log_warn("audio: wav not found %s", filename);
        return nullptr;
    }

    const bool wave_loops = (wave->loopend - wave->loopstart) > 0;
    PTSound sound = Load(&wave->format, wave->size, wave->data, wave_loops);
    delete wave;
    return sound;
}

// Load from a resource pack (the old WAVE.### resource ID path). We keep
// the entry point so call sites don't churn, but the resource pack itself
// is bypassed in the modern port — log a TODO and return nullptr until a
// caller actually needs it.
PTSound TSound::Load(int32_t resid)
{
    if (!SoundPlayer.Functioning())
        return nullptr;
    log_warn("audio: TSound::Load(resid=%d) not yet wired — resource-pack WAV path is unused", resid);
    return nullptr;
}

PTSound TSound::Duplicate()
{
    if (!source || !SoundPlayer.Functioning())
        return nullptr;

    PTSound sound = new TSound;
    memcpy(&sound->format, &format, sizeof(WAVEFORMATEX));
    sound->size    = size;
    sound->looping = looping;
    sound->source  = audio::DuplicateSource(source);
    if (!sound->source) {
        delete sound;
        return nullptr;
    }
    return sound;
}

// CalcPan / CalcDirectionalVol — the 1998 flat 2D mix law in DirectSound
// units. Kept verbatim modulo unit literals so the audible behavior of
// every shipped sound effect stays identical.

// lpos is the listener's position; spos is the sound's position.
int32_t CalcPan(S3DPoint* lpos, S3DPoint* spos)
{
    int32_t pan = 0;
    if (lpos && spos) {
        const int32_t distance = ::Distance(*lpos, *spos);
        if (distance < MAXDIST) {
            // 1998: cos(angle) * DSBPAN_RIGHT(=+10000) / 16. The /16 is
            // intentional — kept full panning rare so off-screen sounds
            // still feel anchored to the world rather than the headphones.
            const double dx = static_cast<double>(spos->x - lpos->x);
            const double dy = static_cast<double>(spos->y - lpos->y);
            double a = atan2(dy, dx) + (M_PI / 2.0);
            pan = static_cast<int32_t>(cos(a) * 10000.0 / 16.0);
        }
    }
    return pan;
}

int32_t CalcDirectionalVol(int32_t orig_vol, S3DPoint* lpos, S3DPoint* spos)
{
    int32_t vol = orig_vol;
    if (lpos && spos) {
        const int32_t distance = ::Distance(*lpos, *spos);
        if (distance > MAXDIST)
            vol = -10000;  // DSBVOLUME_MIN — full attenuation
        else if (distance > MINDIST)
            vol = ((orig_vol - (-10000)) * (MAXDIST - distance) / (MAXDIST - MINDIST)) + (-10000);
    }
    return vol;
}

void TSound::SetListenerPos(S3DPoint* lpos)
{
    if (!lpos) return;
    memcpy(&listener_pos, lpos, sizeof(S3DPoint));
    if (source) {
        const int32_t v = CalcDirectionalVol(sound_volume, &listener_pos, &sound_pos);
        const int32_t p = CalcPan(&listener_pos, &sound_pos);
        audio::SetSourceVolumePan(source, v, p);
    }
}

void TSound::SetSoundPos(S3DPoint* spos)
{
    if (!spos) return;
    memcpy(&sound_pos, spos, sizeof(S3DPoint));
    if (source) {
        const int32_t v = CalcDirectionalVol(sound_volume, &listener_pos, &sound_pos);
        const int32_t p = v ? CalcPan(&listener_pos, &sound_pos) : 0;
        audio::SetSourceVolumePan(source, v, p);
    }
}

// lpos is the listener's position; spos is the sound's position.
void TSound::Play(int32_t volume, int32_t freq, S3DPoint* lpos, S3DPoint* spos)
{
    if (lpos) {
        memcpy(&listener_pos, lpos, sizeof(S3DPoint));
        if (spos)
            memcpy(&sound_pos, spos, sizeof(S3DPoint));
        else
            memcpy(&sound_pos, lpos, sizeof(S3DPoint));
        volume = CalcDirectionalVol(volume, lpos, spos ? spos : lpos);
    }

    sound_volume = volume;

    if (!SoundPlayer.Functioning() || source == nullptr)
        return;

    const int32_t pan = (lpos && volume) ? CalcPan(lpos, spos ? spos : lpos) : 0;
    audio::PlaySource(source, volume, freq, pan);
}

void TSound::Stop()
{
    if (!SoundPlayer.Functioning() || source == nullptr) return;
    audio::StopSource(source);
}

uint32_t TSound::GetStatus()
{
    // 1998 callers only ever asked "is this thing playing?" / "is it
    // looping?"; the bitfield itself was DirectSound-specific. Expose
    // just the two booleans via the dedicated IsPlaying/IsLooping methods
    // instead, and return 0 here.
    return 0;
}

// **********************************
// * TSoundPlayer (registry + mixer) *
// **********************************

bool TSoundPlayer::Initialize()
{
    if (initialized) return true;

    if (!SoundSystemOn) {
        log_info("audio: SoundSystemOn=false — leaving audio offline");
        return false;
    }

    if (!audio::Init()) {
        log_warn("audio: backend init failed — TSoundPlayer offline");
        return false;
    }

    // ReadSoundList() walks ClassDefPath/sound.def (if present) plus the
    // effects/ and language/ folders under ResourcePath. Either path can
    // legitimately be empty — we still come up.
    ReadSoundList();
    initialized = true;
    log_info("audio: TSoundPlayer ready, %d entries", soundlist.NumItems());
    return true;
}

void TSoundPlayer::Close()
{
    DestroySoundList();
    if (initialized) {
        audio::Shutdown();
        initialized = false;
    }
}

bool TSoundPlayer::Functioning() const
{
    return initialized && audio::Functioning();
}

void TSoundPlayer::Pause()
{
    if (Functioning()) audio::PauseAll();
}

void TSoundPlayer::Unpause()
{
    if (Functioning()) audio::UnpauseAll();
}

void TSoundPlayer::SetVolume(int32_t volume)
{
    // 1998 called PrimaryBuffer->SetVolume(hundredths-of-a-dB). Forward
    // to the audio backend's linear master volume.
    if (!Functioning()) return;
    const float linear = (volume <= -10000) ? 0.0f
                       : (volume >=      0) ? 1.0f
                       : std::pow(10.0f, static_cast<float>(volume) / 2000.0f);
    audio::SetMasterVolume(linear);
}

// ---- registry lookup ----------------------------------------------------

int32_t TSoundPlayer::FindSound(char* soundname, int32_t nr)
{
    char buf[80];
    if (nr >= 0) {
        sprintf(buf, "%s%d", soundname, nr);
        return FindSound(buf, -1);
    }
    strcpy(buf, soundname);

    for (int32_t c = 0; c < soundlist.NumItems(); c++) {
        PSSoundRef ref = soundlist[c];
        if (ref && !stricmp(ref->name, buf))
            return c;
    }
    return -1;
}

int32_t TSoundPlayer::NewSound(char* soundname, int32_t nr)
{
    char buf[80];
    if (nr >= 0) {
        sprintf(buf, "%s%d", soundname, nr);
        return FindSound(buf, -1);
    }
    strcpy(buf, soundname);

    PSSoundRef ref = new SSoundRef;
    ref->name     = strdup(buf);
    ref->dir      = nullptr;
    ref->resid    = -1;
    ref->usecount = 0;
    ref->flags    = 0;
    ref->sound    = nullptr;

    int32_t id = soundlist.Add(ref);
    if (id < 0) {
        free(ref->name);
        delete ref;
        return -1;
    }
    return id;
}

// ---- gc for SOUND_DYING ---------------------------------------------------

void TSoundPlayer::UpdateDying()
{
    for (int32_t c = 0; c < soundlist.NumItems(); c++) {
        PSSoundRef ref = soundlist[c];
        if (!ref) continue;

        // Walk duplicate voices and prune the finished ones.
        if (ref->sound && ref->sound->Next()) {
            PTSound prev = ref->sound;
            PTSound next = prev->Next();
            for (PTSound snd; (snd = next); ) {
                next = snd->Next();
                if (snd->IsPlaying()) {
                    prev = snd;
                } else {
                    prev->SetNext(snd->Next());
                    snd->SetNext(nullptr);
                    delete snd;
                }
            }
        }

        // Retire the main sound if the SOUND_DYING flag is set and it's
        // done playing.
        if (ref->flags & SOUND_DYING) {
            if (ref->sound && (!ref->sound->IsPlaying() || ref->sound->IsLooping())) {
                delete ref->sound;
                ref->sound = nullptr;
                ref->flags &= ~SOUND_DYING;
            }
        }
    }
}

// ---- mount / unmount / play / stop --------------------------------------

bool TSoundPlayer::Mount(int32_t id)
{
    if (!Functioning() || id < 0 || id >= soundlist.NumItems())
        return false;

    PSSoundRef ref = soundlist[id];
    if (!ref) return false;

    if (ref->usecount < 1) {
        if (ref->flags & SOUND_DYING) {
            ref->flags &= ~SOUND_DYING;
        } else {
            if (ref->resid >= 0)
                ref->sound = TSound::Load(ref->resid);
            else
                ref->sound = TSound::Load(ref->name, ref->resid);
        }
        ref->usecount = ref->sound ? 1 : 0;
    } else {
        ref->usecount++;
    }

    UpdateDying();
    return ref->sound != nullptr;
}

bool TSoundPlayer::Unmount(int32_t id)
{
    if (!Functioning() || id < 0 || id >= soundlist.NumItems())
        return false;

    PSSoundRef ref = soundlist[id];
    if (!ref) return false;

    ref->usecount--;
    if (ref->usecount < 1) {
        if (ref->sound) {
            if (ref->sound->IsPlaying() && !ref->sound->IsLooping()) {
                ref->flags |= SOUND_DYING;
            } else {
                delete ref->sound;
                ref->sound = nullptr;
            }
        }
        ref->usecount = 0;
    }

    UpdateDying();
    return true;
}

bool TSoundPlayer::Play(int32_t id, int32_t volume, int32_t freq, S3DPoint* spos)
{
    if (!Functioning() || id < 0 || id >= soundlist.NumItems())
        return false;

    PSSoundRef ref = soundlist[id];
    if (!ref || !ref->sound) return false;

    if (!ref->sound->IsPlaying()) {
        ref->sound->Play(volume, freq, &listener_pos, spos);
    } else {
        // Overlap an existing playing instance: duplicate the source so
        // the new voice can run in parallel without re-triggering the old.
        PTSound newsound = ref->sound->Duplicate();
        if (!newsound) return false;
        newsound->SetNext(ref->sound->Next());
        ref->sound->SetNext(newsound);
        newsound->Play(volume, freq, &listener_pos, spos);
    }

    UpdateDying();
    return true;
}

bool TSoundPlayer::Stop(int32_t id)
{
    if (!Functioning() || id < 0 || id >= soundlist.NumItems())
        return false;

    PSSoundRef ref = soundlist[id];
    if (!ref) return false;
    if (ref->sound) ref->sound->Stop();
    return true;
}

PTSound TSoundPlayer::GetSound(int32_t id)
{
    if (!Functioning() || id < 0 || id >= soundlist.NumItems())
        return nullptr;
    PSSoundRef ref = soundlist[id];
    return ref ? ref->sound : nullptr;
}

void TSoundPlayer::SetListenerPos(int32_t x, int32_t y, int32_t z)
{
    listener_pos.x = x;
    listener_pos.y = y;
    listener_pos.z = z;

    for (int32_t c = 0; c < soundlist.NumItems(); c++) {
        PSSoundRef ref = soundlist[c];
        if (ref && ref->sound)
            ref->sound->SetListenerPos(&listener_pos);
    }
}

// ---- sound-list population from disk ------------------------------------

bool TSoundPlayer::SearchSoundDir(const char* soundpath, const char* subdir, int32_t dirresid)
{
    if (!subdir) return false;

    // First-wins de-dup: a sound that's already in the registry (from a
    // prior pass or sound.def) doesn't get clobbered by an archive entry
    // with the same basename.
    auto add_if_new = [&](const char* basename) {
        if (FindSound(const_cast<char*>(basename), -1) >= 0) return;
        auto* ref     = new SSoundRef;
        ref->name     = strdup(basename);
        ref->dir      = nullptr;
        ref->resid    = dirresid;
        ref->usecount = 0;
        ref->flags    = 0;
        ref->sound    = nullptr;
        soundlist.Add(ref);
    };

    // 1) Loose WAVs on disk under <RunPath>/sound/<subdir>/*.wav. Modders
    // or partial extractions will land here; first pass for compatibility.
    if (soundpath) {
        std::filesystem::path dir = std::filesystem::path(soundpath) / subdir;
        std::error_code ec;
        if (std::filesystem::is_directory(dir, ec)) {
            for (auto& ent : std::filesystem::directory_iterator(dir, ec)) {
                if (ec) break;
                if (!ent.is_regular_file()) continue;
                auto ext = ent.path().extension().string();
                for (auto& c : ext) c = static_cast<char>(std::tolower(c));
                if (ext != ".wav") continue;

                add_if_new(ent.path().stem().string().c_str());
            }
        }
    }

    // 2) Archive-resident WAVs under "Sound/<subdir>/" inside the mounted
    // resource ZIPs (resources.rvr ships effects/ this way). Listing
    // returns lowercased basenames *with* extension; strip ".wav" for
    // the registry to match the disk path.
    std::string prefix = std::string("Sound/") + subdir + "/";
    std::vector<std::string> entries;
    VFSListByPrefix(prefix.c_str(), entries);
    for (auto& fname : entries) {
        // Strip extension. (We already filtered to .wav-shaped names by
        // path, but be defensive about other extensions slipping in.)
        std::string base = fname;
        auto dot = base.rfind('.');
        std::string ext = (dot == std::string::npos) ? "" : base.substr(dot);
        for (auto& c : ext) c = static_cast<char>(std::tolower(c));
        if (ext != ".wav") continue;
        base.resize(dot);
        add_if_new(base.c_str());
    }
    return true;
}

bool TSoundPlayer::ReadSoundList()
{
    soundlist.Clear();

    // sound.def is optional (it carried the legacy resource-pack ID map);
    // the directory scan below is what actually populates the registry
    // for the modern WAV-on-disk path.
    char defpath[MAXPATHLEN];
    snprintf(defpath, MAXPATHLEN, "%ssound.def", ClassDefPath);
    if (FILE* fp = TryOpen(defpath, "rb")) {
        TFileParseStream s(fp, defpath);
        TToken t(s);
        t.Get();

        char name[128];
        int32_t resid;
        while (t.Type() != TKN_EOF) {
            if (t.Type() == TKN_RETURN || t.Type() == TKN_WHITESPACE) {
                t.LineGet();
                continue;
            }
            if (!Parse(t, "%s %d", name, &resid)) {
                log_warn("audio: malformed sound.def near token '%s'", name);
                break;
            }
            auto* ref = new SSoundRef;
            ref->name     = strdup(name);
            ref->dir      = nullptr;
            ref->resid    = resid;
            ref->usecount = 0;
            ref->flags    = 0;
            ref->sound    = nullptr;
            soundlist.Add(ref);
        }
        fclose(fp);
    }

    char fname[MAXPATHLEN];
    strncpyz(fname, RunPath, MAXPATHLEN);
    strncatz(fname, "sound/", MAXPATHLEN);

    SearchSoundDir(fname, "effects",        DIRRESID_EFFECTDIR);
    SearchSoundDir(fname, Language.CStr(),  DIRRESID_DIALOGDIR);
    return true;
}

void TSoundPlayer::DestroySoundList()
{
    for (int32_t c = 0; c < soundlist.NumItems(); c++) {
        PSSoundRef ref = soundlist[c];
        if (!ref) continue;
        if (ref->sound) delete ref->sound;
        free(ref->name);
    }
    soundlist.DeleteAll();
}
