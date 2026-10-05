// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *                   sound.h - Music and sound module                    *
// *************************************************************************
//
// 1998 TSound / TSoundPlayer public API sitting on top of the modern
// miniaudio facade (see audio_backend.h). The DirectSound pointer types
// that used to live in this header are gone; an opaque audio::Source*
// replaces LPDIRECTSOUNDBUFFER for per-sound voices, and TSoundPlayer
// now owns its boot state through an explicit Initialize()/Close() pair.
//
// The sound list is retail's (TSoundPlayer, cls_0x41c7d0; see
// docs/gameflow/forensics/DIALOG.md §3.4): every .wav and .mp3 in the
// resource sound directories, registered at Initialize, and in the current
// module's, registered when the module mounts; sorted by name without case
// and searched by bsearch. A sound's id is its index in that list, so ids
// change when a module's sounds come or go. A dialog voice is the sound
// named by its dialog tag (I1LOC00 -> Sound/english/i1loc00.mp3).
//
// Registration, lookup and SampleLengthMs work whether or not audio output
// is live (--headless silences it); Mount/Play/Stop need live output.

#pragma once

#include "revenant.h"

#include "wavedata.h"

#include <array>
#include <memory>
#include <string>
#include <vector>

namespace audio { struct Source; }

// Sound effects classes
_CLASSDEF(TSound)
class TSound
{
  public:
    TSound();
    ~TSound();

    static PTSound Load(WAVEFORMATEX *format, uint32_t size, uint8_t *data, bool looping);
        // Build a sound from in-memory PCM
    static PTSound Load(const char *path);
        // Read a sound file (.wav, .mp3) through the resource layer and decode it

    PTSound Duplicate();
        // Make a duplicate of this sound

    void Play(int32_t volume = 0, int32_t freq = 0, S3DPoint* lpos = nullptr, S3DPoint* spos = nullptr);
        // Without lpos the sound is not positioned: listener moves don't pan
        // or attenuate it
    void Stop();

    void SetListenerPos(S3DPoint* lpos = nullptr);
        // set the listener's position for direction-based audio
    void SetSoundPos(S3DPoint* spos = nullptr);
        // set the sound's position for direction-based audio

    void GetListenerPos(S3DPoint* lpos) { *lpos = listener_pos; }
    void GetSoundPos(S3DPoint* spos) { *spos = sound_pos; }
    int32_t GetSoundVolume() { return sound_volume; }

    uint32_t GetStatus();
    bool IsPlaying();
    bool IsLooping();
    void SetLooping(bool state) { looping = state; }

    PTSound Next() { return next; }
    void SetNext(PTSound n) { next = n; }
        // Linked list utils

    int32_t GetSize() { return size; }
      // Gets size of wave buffer data
    int32_t GetChannels() { return format.nChannels; }
      // Gets number of channels for buffer
    int32_t GetSamples() { return size / format.nBlockAlign; }
      // Gets number of samples for buffer data
    int32_t GetSamplesPerSec() { return format.nSamplesPerSec; }
      // Gets number of samples for buffer data
    int32_t GetLength() { return GetSamples() * 100 / GetSamplesPerSec(); }
      // Returns length of sound in 100ths of a second

  // 3D Sound parameters!
//  void SetPosition(int32_t x, int32_t y, int32_t z);
//    // Set sound maker position
//  void SetVelocity(int32_t x, int32_t y, int32_t z);
//    // Set sound maker velocity
//  void SetOrientation(float x, float y, float z);
//    // Set sound maker orientation (unit vector in float format)
//  void CommitSettings();
//    // Commits the previously set settings

  protected:
    S3DPoint listener_pos;
    S3DPoint sound_pos;
    int32_t sound_volume;
        // save the volume of this sound so that we can get the correct value later on...
    bool positional = false;    // last Play() had a listener position

    uint32_t size;
    WAVEFORMATEX format;
    audio::Source* source;  // opaque miniaudio voice (was LPDIRECTSOUNDBUFFER in 1998)
    bool looping;

    PTSound next;           // next in list
};

// Sound ref flags
constexpr uint32_t SOUND_DYING = 1 << 0;   // The sound should be deallocated once it stops playing

// The directories sounds are registered from. Retail keeps one path buffer
// per directory in TSoundPlayer and each sound-list entry points at its
// buffer (+0x08); the module's two are how UnloadModuleSounds finds the
// module's entries.
enum class ESoundDir : uint8_t
{
    ResourceEffects,    // <ResourcePath>sound\effects\              (+0x0c8)
    ResourceLanguage,   // <ResourcePath>sound\<Language>\           (+0x1cc)
    ImageryEffects,     // <ImageryPath>sound\effects\               (+0x2d0)
    ImageryLanguage,    // <ImageryPath>sound\<Language>\            (+0x3d4)
    ModuleEffects,      // <ModulesPath><module>\sound\effects\      (+0x4d8)
    ModuleLanguage,     // <ModulesPath><module>\sound\<Language>\   (+0x5dc)
    Count
};

// One registered sound: retail's 0x1c-byte sound-list entry, made by the
// directory scan (0x0049ad20).
struct SSoundRef
{
    std::string name;                           // file name up to its first '.' (+0x00)
    std::string file;                           // file name as found (retail keeps its extension, +0x04)
    ESoundDir dir = ESoundDir::ResourceEffects; // directory it was found in (+0x08)
    int32_t usecount = 0;                       // Mount/Unmount references
    uint32_t flags = 0;                         // SOUND_DYING
    int32_t lengthms = -1;                      // SampleLengthMs cache; -1 = not measured yet
    std::unique_ptr<TSound> sound;              // decoded voice(s) while mounted
};

class TSoundPlayer
{
  public:
    friend class TSound;

    TSoundPlayer() = default;
    // Trivial dtor: explicit Close() runs from ShutdownGlobals before the
    // global destructs. Walking soundlist from a global dtor risks
    // cross-TU teardown ordering bugs.
    ~TSoundPlayer() = default;

    bool Initialize();      // registers the resource sounds, then brings up audio output
    void Close();           // idempotent — safe to call twice

    bool Functioning() const;
        // Audio output is live (Mount/Play/Stop do something)

    void Pause();
    void Unpause();
        // Start and stop all sound effects (ie, game pausing/unpausing)

    void SetVolume(int32_t volume = 0);
        // An argument of 0 is the normal playing level

  // Module sounds (TModuleManager::SetCurModule)
    bool LoadModuleSounds(const char *module);
        // Register the module's sound\effects\ and sound\<Language>\ (0x0049b220)
    void UnloadModuleSounds();
        // Drop the current module's sounds

  // Finds sound id's by name
    int32_t FindSound(const char *soundname, int32_t nr = -1) const;
        // Find a given sound: name (plus nr, if given) without case, -1 if none

  // Simple sound garbage-collector functions to make playing sounds a bit handier
  // These functions work with the sound id returned from FindSound()
    bool Mount(int32_t id);
        // Inform sound system to prepare this sound for later use
    bool Unmount(int32_t id);
        // Inform sound system that you are done with this sound
    bool Play(int32_t id, int32_t volume = 0, int32_t freq = 0, S3DPoint* spos = nullptr);
        // Play a mounted sound at volume (hundredths of a dB: 0 = full,
        // -10000 = silent). Positioned at spos relative to the listener;
        // without spos, not positioned (dialog voices, ambience)
    bool Stop(int32_t id);
        // Stop a playing, mounted sound
    int32_t SampleLengthMs(int32_t id);
        // Length of the sound in milliseconds, decoded from its file (cached);
        // 0 if it can't be decoded. Needs no audio output.

  // Sound name functions (has to search sound list every time, but easier to use)
    bool Mount(const char *n, int32_t nr = -1) { return Mount(FindSound(n, nr)); }
        // Mount by sound name and number
    bool Unmount(const char *n, int32_t nr = -1) { return Unmount(FindSound(n, nr)); }
        // Unmount by sound name and number
    bool Play(const char *n, int32_t nr = -1, int32_t volume = -1, int32_t freq = -1)
      { return Play(FindSound(n, nr), volume, freq); }
        // Play a mounted sound  by name and number
    bool Stop(const char *n, int32_t nr = -1) { return Stop(FindSound(n, nr)); }
        // Stop a sound by name and number

  // 3D Sound functions
    void SetListenerPos(int32_t x, int32_t y, int32_t z);
//    // Sets the current listeners position

//  void SetOrientation(float facex, float facey, float facez,
//      float topx, float topy, float topz);
//    // Set the current listener's orientation (f-front vector, t-top of head vector)
//  void SetVelocity(int32_t x, int32_t y, int32_t z);
//    // Set the current listener's orientation
//  void CommitSettings();
//    // Commits all settings that were just set

  // Returns actual sound object for sound
  // NOTE: Sound must be MOUNTED or this will return nullptr!
    PTSound GetSound(int32_t id);

  // Read-only access to the registry. Used by the --test=audio panel
  // and any future settings UI that wants to enumerate effects by name.
    int32_t NumItems() const { return static_cast<int32_t>(soundlist.size()); }
    const SSoundRef* GetRef(int32_t id) const
        { return (id >= 0 && id < NumItems()) ? soundlist[id].get() : nullptr; }

  private:
    SSoundRef* Ref(int32_t id)
        { return (id >= 0 && id < NumItems()) ? soundlist[id].get() : nullptr; }
    std::string& Dir(ESoundDir dir) { return sounddirs[static_cast<size_t>(dir)]; }
    std::string SoundPath(const SSoundRef& ref) const
        { return sounddirs[static_cast<size_t>(ref.dir)] + ref.file; }

    void LoadResourceSounds();
        // Registers the resource (and imagery) sound directories
    int32_t RegisterSounds(ESoundDir dir);
        // Adds every .wav, then every .mp3, in one sound directory
    void SortSoundList();
        // Orders the list by name, ignoring case
    void UpdateDying();
        // Loop through ref list and kill off any dying sounds

    bool initialized = false;   // Initialize() has run (resource sounds registered)

    std::vector<std::unique_ptr<SSoundRef>> soundlist;  // registered sounds, sorted by name
    std::array<std::string, static_cast<size_t>(ESoundDir::Count)> sounddirs;

    S3DPoint listener_pos{};
};

// Easy access function for one-time sounds
inline bool PLAY(const char *x)
{
    int32_t id = SoundPlayer.FindSound(x);
    if (id < 0)
        return false;
    if (!SoundPlayer.Mount(id))
        return false;
    if (!SoundPlayer.Play(id))
        return false;
    if (!SoundPlayer.Unmount(id))
        return false;
    return true;
}

inline bool PLAYN(const char *x, int32_t n)
{
    int32_t id = SoundPlayer.FindSound(x, n);
    if (id < 0)
        return false;
    if (!SoundPlayer.Mount(id))
        return false;
    if (!SoundPlayer.Play(id))
        return false;
    if (!SoundPlayer.Unmount(id))
        return false;
    return true;
}
