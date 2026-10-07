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
// What is retail's (TSoundPlayer, cls_0x41c7d0; DIALOG.md §3.4):
//   - The sound list: every .wav and .mp3 in the resource directories
//     sound\effects\ and sound\<Language>\ (and under ImageryPath) at Initialize
//     (0x0049afd0), and in the module's sound\effects\ and
//     sound\<Language>\ when the module mounts (0x0049b220); sorted by name
//     without case and searched by bsearch (0x0049c430).
//
// What changed:
//   - No CDOpen/CDClose/CDPlayTrack shim. Music goes through
//     audio::MusicPlayFile directly (callers updated, e.g. area.cpp).
//   - LPDIRECTSOUNDBUFFER replaced by audio::Source* in TSound::source.
//   - TSoundPlayer needs an explicit Initialize() now (called from
//     revmain boot) — the 1998 code paired init with the device-open
//     side effect of the first directsound call.
//   - Sound files are decoded to PCM when mounted (retail kept the file
//     bytes and let Miles decode while playing). The registry and
//     SampleLengthMs don't need audio output, so --headless runs see the
//     same sounds and lengths as a run with sound.
//
// *************************************************************************

#include "sound.h"

#include "audio_backend.h"
#include "logging.h"
#include "object.h"
#include "revutils.h"
#include "wavedata.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>

// Distance thresholds (world units) for the flat 2D mix law. < MINDIST is
// full-volume; > MAXDIST is silent; in between we linearly interpolate in
// the DirectSound dB attenuation range.
#define MINDIST 256
#define MAXDIST 1024

// SoundSystemOn — kept as a global gate so callers that pre-date Init can
// disable audio without crashing. Set by revmain config.
extern bool SoundSystemOn;

// *********************
// * TSound (one voice) *
// *********************

TSound::TSound()
{
    source = nullptr;
    looping = false;
    next = nullptr;
    sound_volume = 0;
    listener_pos = S3DPoint(0, 0, 0);
    sound_pos    = S3DPoint(0, 0, 0);
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

// A sound PauseSamples stopped is still in progress (it resumes where it
// was), so the dying-sound collector keeps it.
bool TSound::IsPlaying()
{
    return paused || (source && audio::IsPlaying(source));
}

bool TSound::IsLooping()
{
    if (source) return audio::IsLooping(source);
    return looping;
}

// Load from in-memory PCM (the only Load that actually has data to feed
// the backend; the file Load below funnels into this one).
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

// Reads a sound file through the resource layer, pack first as retail's
// load (0x0049b650) opens it, and decodes it to PCM: .wav and .mp3 alike.
PTSound TSound::Load(const char* path)
{
    if (!path || !SoundPlayer.Functioning())
        return nullptr;

    std::vector<uint8_t> bytes;
    if (!rev_read_file(path, bytes)) {
        log_warn("audio: can't read %s", path);
        return nullptr;
    }
    WAVEFORMATEX fmt{};
    std::vector<uint8_t> pcm;
    if (!audio::DecodeToPCM16(bytes.data(), bytes.size(), &fmt, pcm)) {
        log_warn("audio: can't decode %s", path);
        return nullptr;
    }
    if (pcm.empty()) {
        // A file without samples (blank.wav, the silent pick in the grunt
        // lists) is a sound that plays nothing, as Miles played it.
        PTSound sound = new TSound;
        memcpy(&sound->format, &fmt, sizeof(WAVEFORMATEX));
        return sound;
    }
    return Load(&fmt, static_cast<uint32_t>(pcm.size()), pcm.data(), false);
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
    listener_pos = *lpos;
    if (source && positional) {
        const int32_t v = CalcDirectionalVol(sound_volume, &listener_pos, &sound_pos);
        const int32_t p = CalcPan(&listener_pos, &sound_pos);
        audio::SetSourceVolumePan(source, v, p);
    }
}

void TSound::SetSoundPos(S3DPoint* spos)
{
    if (!spos) return;
    sound_pos = *spos;
    if (source && positional) {
        const int32_t v = CalcDirectionalVol(sound_volume, &listener_pos, &sound_pos);
        const int32_t p = v ? CalcPan(&listener_pos, &sound_pos) : 0;
        audio::SetSourceVolumePan(source, v, p);
    }
}

// lpos is the listener's position; spos is the sound's position. Without
// lpos the sound plays flat and stays that way.
void TSound::Play(int32_t volume, int32_t freq, S3DPoint* lpos, S3DPoint* spos)
{
    paused = false;
    positional = lpos != nullptr;
    if (lpos) {
        listener_pos = *lpos;
        if (spos)
            sound_pos = *spos;
        else
            sound_pos = *lpos;
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
    paused = false;
    if (!SoundPlayer.Functioning() || source == nullptr) return;
    audio::StopSource(source);
}

void TSound::Pause()
{
    if (paused || !source || !audio::IsPlaying(source))
        return;
    audio::StopSource(source);
    paused = true;
}

void TSound::Resume()
{
    if (!paused)
        return;
    paused = false;
    if (SoundPlayer.Functioning() && source)
        audio::ResumeSource(source);
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

// REVSYNC: 0x0049a830 — sound init: opens output, then registers the
// resource sounds (0x0049afd0) and sorts the list.
// REVSYNC-DIVERGENCE: retail has no sound list when output is off or fails
// to open (0x00668114 set: nothing registered, every voice falls back to
// text pacing). The port registers regardless, so lookups and
// SampleLengthMs answer the same with output silenced (--headless).
bool TSoundPlayer::Initialize()
{
    if (initialized) return true;
    initialized = true;

    LoadResourceSounds();

    if (!SoundSystemOn)
        log_info("audio: SoundSystemOn=false — output offline, sounds registered");
    else if (!audio::Init())
        log_info("audio: output offline (silenced or no device) — sounds registered");
    return true;
}

void TSoundPlayer::Close()
{
    // Sounds release their voices while the engine is still up.
    soundlist.clear();
    for (std::string& dir : sounddirs)
        dir.clear();
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

// Retail walked its 16 2D and 16 3D sample slots; the port's voices are the
// mounted sounds and their duplicates.
void TSoundPlayer::PauseSamples()
{
    if (!Functioning())
        return;
    for (const std::unique_ptr<SSoundRef>& ref : soundlist)
        for (TSound* sound = ref->sound.get(); sound; sound = sound->Next())
            sound->Pause();
}

void TSoundPlayer::ResumeSamples()
{
    if (!Functioning())
        return;
    for (const std::unique_ptr<SSoundRef>& ref : soundlist)
        for (TSound* sound = ref->sound.get(); sound; sound = sound->Next())
            sound->Resume();
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

// ---- the player's levels ---------------------------------------------------

void ApplyMusicVolume(int32_t level)
{
    const int32_t clamped = (std::clamp)(level, 0, kMusicLevelMax);
    audio::SetMusicVolume(static_cast<float>(clamped) / static_cast<float>(kMusicLevelMax));
    log_info("[sound] music level %d -> gain %.3f", clamped,
             static_cast<double>(clamped) / kMusicLevelMax);
}

void ApplyEffectsVolume(int32_t level)
{
    const int32_t clamped = (std::clamp)(level, 0, kEffectsLevelMax);
    audio::SetSfxVolume(static_cast<float>(clamped) / static_cast<float>(kEffectsLevelMax));
    log_info("[sound] effects level %d -> gain %.3f", clamped,
             static_cast<double>(clamped) / kEffectsLevelMax);
}

// ---- sound list -----------------------------------------------------------

// REVSYNC: 0x0049ad20 — every "<dir>*.wav", then every "<dir>*.mp3",
// through the packs (findfirst 0x004a19d0); the name is the file name up to
// its first '.'. No duplicate check, as retail. (Retail also marks the .mp3
// entries 2D-only, +0x18 bit 1; the port's positioning is the 1998 pan law
// for every sound.)
int32_t TSoundPlayer::RegisterSounds(ESoundDir dir)
{
    const std::string& path = Dir(dir);
    int32_t added = 0;
    for (const char* ext : {".wav", ".mp3"}) {
        std::vector<std::string> files;
        rev_find_files(path.c_str(), ext, files);
        for (std::string& file : files) {
            auto ref  = std::make_unique<SSoundRef>();
            ref->name = file.substr(0, file.find('.'));
            ref->file = std::move(file);
            ref->dir  = dir;
            soundlist.push_back(std::move(ref));
            ++added;
        }
    }
    return added;
}

// REVSYNC: qsort (0x0058c9ff) with 0x0049ab00 — names compared by _stricmp
// (0x0059a530) — after each registration.
// REVSYNC-DIVERGENCE: a stable sort, so of two sounds with one name the
// first registered is found; retail's qsort + bsearch find either. The
// shipped data has no such pair.
void TSoundPlayer::SortSoundList()
{
    std::stable_sort(soundlist.begin(), soundlist.end(),
                     [](const std::unique_ptr<SSoundRef>& a, const std::unique_ptr<SSoundRef>& b) {
                         return stricmp(a->name.c_str(), b->name.c_str()) < 0;
                     });
}

// REVSYNC: 0x0049afd0 — <ResourcePath>sound\effects\ and
// <ResourcePath>sound\<Language>\, then the same two under ImageryPath when
// they name other directories. Retail lower-cases the paths (_strlwr); the
// port's lookups ignore case.
void TSoundPlayer::LoadResourceSounds()
{
    const std::string language = Language.CStr();
    Dir(ESoundDir::ResourceEffects)  = std::string(ResourcePath) + "sound\\effects\\";
    Dir(ESoundDir::ResourceLanguage) = std::string(ResourcePath) + "sound\\" + language + "\\";
    Dir(ESoundDir::ImageryEffects)   = std::string(ImageryPath) + "sound\\effects\\";
    Dir(ESoundDir::ImageryLanguage)  = std::string(ImageryPath) + "sound\\" + language + "\\";

    const int32_t effects = RegisterSounds(ESoundDir::ResourceEffects);
    const int32_t voices  = RegisterSounds(ESoundDir::ResourceLanguage);
    int32_t imagery = 0;
    if (stricmp(Dir(ESoundDir::ImageryEffects).c_str(), Dir(ESoundDir::ResourceEffects).c_str()) != 0)
        imagery += RegisterSounds(ESoundDir::ImageryEffects);
    if (stricmp(Dir(ESoundDir::ImageryLanguage).c_str(), Dir(ESoundDir::ResourceLanguage).c_str()) != 0)
        imagery += RegisterSounds(ESoundDir::ImageryLanguage);
    SortSoundList();

    log_info("audio: resource sounds: %d in %s, %d in %s, %d under %s; %d registered",
             effects, Dir(ESoundDir::ResourceEffects).c_str(),
             voices, Dir(ESoundDir::ResourceLanguage).c_str(),
             imagery, ImageryPath, NumItems());
}

// REVSYNC: 0x0049b220 — called by SetCurModule (0x004609f0) right after the
// module's dialog list: <ModulesPath><module>\sound\effects\ and
// <ModulesPath><module>\sound\<Language>\, then the sort.
// REVSYNC-DIVERGENCE: retail skips this when output is off (0x00668114); see
// Initialize.
bool TSoundPlayer::LoadModuleSounds(const char* module)
{
    if (!module || !module[0]) return false;

    const std::string base = std::string(ModulesPath) + module;
    Dir(ESoundDir::ModuleEffects)  = base + "\\sound\\effects\\";
    Dir(ESoundDir::ModuleLanguage) = base + "\\sound\\" + Language.CStr() + "\\";

    const int32_t effects = RegisterSounds(ESoundDir::ModuleEffects);
    const int32_t voices  = RegisterSounds(ESoundDir::ModuleLanguage);
    SortSoundList();

    log_info("audio: module sounds: %d in %s, %d in %s; %d registered",
             effects, Dir(ESoundDir::ModuleEffects).c_str(),
             voices, Dir(ESoundDir::ModuleLanguage).c_str(), NumItems());
    return true;
}

// REVSYNC: 0x0049b400 — called by SetCurModule before another module mounts
// (and by the module close, 0x00460c10): drops every entry found in the
// module's directories; the rest keep their order. Retail leaves an entry
// that is still playing out of the list without freeing it; the port frees
// it, which stops it.
void TSoundPlayer::UnloadModuleSounds()
{
    if (Dir(ESoundDir::ModuleEffects).empty()) return;

    soundlist.erase(std::remove_if(soundlist.begin(), soundlist.end(),
                                   [](const std::unique_ptr<SSoundRef>& ref) {
                                       return ref->dir == ESoundDir::ModuleEffects ||
                                              ref->dir == ESoundDir::ModuleLanguage;
                                   }),
                    soundlist.end());
    Dir(ESoundDir::ModuleEffects).clear();
    Dir(ESoundDir::ModuleLanguage).clear();
}

// REVSYNC: 0x0049c430 — bsearch over the sorted list, names compared
// without case.
int32_t TSoundPlayer::FindSound(const char* soundname, int32_t nr) const
{
    if (!soundname) return -1;

    char buf[80];
    if (nr >= 0) {
        snprintf(buf, sizeof(buf), "%s%d", soundname, nr);
        soundname = buf;
    }

    const auto it = std::lower_bound(soundlist.begin(), soundlist.end(), soundname,
                                     [](const std::unique_ptr<SSoundRef>& ref, const char* name) {
                                         return stricmp(ref->name.c_str(), name) < 0;
                                     });
    if (it == soundlist.end() || stricmp((*it)->name.c_str(), soundname) != 0)
        return -1;
    return static_cast<int32_t>(it - soundlist.begin());
}

// REVSYNC: 0x0049c640 — the sound's length in milliseconds; TCharacter::Say
// (0x004d0610) sizes the say action from it.
// REVSYNC-DIVERGENCE: retail asks Miles for the total of the 2D sample
// playing the sound (AIL_sample_ms_position), so it answers 0 unless the
// sound is playing. The port decodes the file once and caches the length:
// the answer doesn't depend on audio output (--headless) or on playback.
int32_t TSoundPlayer::SampleLengthMs(int32_t id)
{
    SSoundRef* ref = Ref(id);
    if (!ref) return 0;

    if (ref->lengthms < 0) {
        const std::string path = SoundPath(*ref);
        std::vector<uint8_t> bytes;
        std::optional<uint32_t> ms;
        if (rev_read_file(path.c_str(), bytes))
            ms = audio::DecodedLengthMs(bytes.data(), bytes.size());
        ref->lengthms = ms ? static_cast<int32_t>(*ms) : 0;
        if (!ms)
            log_warn("audio: can't measure %s", path.c_str());
    }
    return ref->lengthms;
}

// ---- gc for SOUND_DYING ---------------------------------------------------

void TSoundPlayer::UpdateDying()
{
    for (const std::unique_ptr<SSoundRef>& ref : soundlist) {
        // Walk duplicate voices and prune the finished ones.
        if (ref->sound && ref->sound->Next()) {
            PTSound prev = ref->sound.get();
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
                ref->sound.reset();
                ref->flags &= ~SOUND_DYING;
            }
        }
    }
}

// ---- mount / unmount / play / stop --------------------------------------

// The port's load (retail 0x0049b650): decodes the sound's file.
bool TSoundPlayer::Mount(int32_t id)
{
    if (!Functioning()) return false;
    SSoundRef* ref = Ref(id);
    if (!ref) return false;

    if (ref->usecount < 1) {
        if (ref->flags & SOUND_DYING)
            ref->flags &= ~SOUND_DYING;
        else
            ref->sound.reset(TSound::Load(SoundPath(*ref).c_str()));
        ref->usecount = ref->sound ? 1 : 0;
    } else {
        ref->usecount++;
    }

    UpdateDying();
    return ref->sound != nullptr;
}

bool TSoundPlayer::Unmount(int32_t id)
{
    if (!Functioning()) return false;
    SSoundRef* ref = Ref(id);
    if (!ref) return false;

    ref->usecount--;
    if (ref->usecount < 1) {
        if (ref->sound) {
            if (ref->sound->IsPlaying() && !ref->sound->IsLooping())
                ref->flags |= SOUND_DYING;
            else
                ref->sound.reset();
        }
        ref->usecount = 0;
    }

    UpdateDying();
    return true;
}

// REVSYNC: 0x0049b990 — without a position (pos NULL) the sound plays at the
// listener as a 2D sample and isn't moved by later listener updates; that
// is how TCharacter::Say (0x004d0610) and the death screen (0x005339b0)
// play voices. Their volume 0x7f is retail's full scale, relative to the
// SFX volume: the port's 0 (the sfx group carries the SFX volume).
bool TSoundPlayer::Play(int32_t id, int32_t volume, int32_t freq, S3DPoint* spos)
{
    if (!Functioning()) return false;
    SSoundRef* ref = Ref(id);
    if (!ref || !ref->sound) return false;

    S3DPoint* lpos = spos ? &listener_pos : nullptr;
    if (!ref->sound->IsPlaying()) {
        ref->sound->Play(volume, freq, lpos, spos);
    } else {
        // Overlap an existing playing instance: duplicate the source so
        // the new voice can run in parallel without re-triggering the old.
        PTSound newsound = ref->sound->Duplicate();
        if (!newsound) return false;
        newsound->SetNext(ref->sound->Next());
        ref->sound->SetNext(newsound);
        newsound->Play(volume, freq, lpos, spos);
    }

    UpdateDying();
    return true;
}

bool TSoundPlayer::Stop(int32_t id)
{
    if (!Functioning()) return false;
    SSoundRef* ref = Ref(id);
    if (!ref) return false;
    if (ref->sound) ref->sound->Stop();
    return true;
}

PTSound TSoundPlayer::GetSound(int32_t id)
{
    if (!Functioning()) return nullptr;
    SSoundRef* ref = Ref(id);
    return ref ? ref->sound.get() : nullptr;
}

void TSoundPlayer::SetListenerPos(int32_t x, int32_t y, int32_t z)
{
    listener_pos.x = x;
    listener_pos.y = y;
    listener_pos.z = z;

    for (const std::unique_ptr<SSoundRef>& ref : soundlist) {
        if (ref->sound)
            ref->sound->SetListenerPos(&listener_pos);
    }
}
