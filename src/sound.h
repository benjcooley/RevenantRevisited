// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   sound.h - Music and sound module                    *
// *************************************************************************

#pragma once

#include "revenant.h"

#include "wavedata.h"

// Make it so we don't have to include directsound
#ifndef __DSOUND_INCLUDED__
struct IDirectSoundBuffer;
typedef struct IDirectSoundBuffer *LPDIRECTSOUNDBUFFER;
struct IDirectSound;
typedef struct IDirectSound *LPDIRECTSOUND;
struct IDirectSound3DBuffer;
typedef struct IDirectSound3DBuffer *LPDIRECTSOUND3DBUFFER;
struct IDirectSound3DListener;
typedef struct IDirectSound3DListener *LPDIRECTSOUND3DLISTENER;
#endif

// CD functions
void CDOpen();
void CDClose();
void CDPlayTrack(int32_t track);
uint32_t CDTrackLength(int32_t track);
void CDPlayRandomTrack();
void CDStop();
bool CDPlaying();
void CDSetVolume(uint16_t volume);

// Load a wave as a game sound object
PTWaveData LoadWave(char *filename, 
    int32_t volume = 0, int32_t loopstart = 0x7FFFFFFF, int32_t loopend = 0x7FFFFFFF);

// Wave data structure
_CLASSDEF(TWaveData)

// Sound effects classes
_CLASSDEF(TSound)
class TSound
{
  public:
    TSound();
    ~TSound();

    static PTSound Load(WAVEFORMATEX *format, uint32_t size, uint8_t *data, bool looping);
    static PTSound Load(char *name, int32_t dirresid);
    static PTSound Load(int32_t resid);

    PTSound Duplicate();
        // Make a duplicate of this sound

    void Play(int32_t volume = 0, int32_t freq = 0, S3DPoint* lpos = nullptr, S3DPoint* spos = nullptr);
    void Stop();

    void SetListenerPos(S3DPoint* lpos = nullptr);
        // set the listener's position for direction-based audio
    void SetSoundPos(S3DPoint* spos = nullptr);
        // set the sound's position for direction-based audio

    void GetListenerPos(S3DPoint* lpos) { memcpy(lpos, &listener_pos, sizeof(S3DPoint)); }
    void GetSoundPos(S3DPoint* spos) { memcpy(spos, &sound_pos, sizeof(S3DPoint)); }
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
    
    uint32_t size; 
    WAVEFORMATEX format;
    LPDIRECTSOUNDBUFFER SoundBuffer;
//  LPDIRECTSOUND3DBUFFER SoundBuffer3D;
    bool looping;

    PTSound next;           // next in list
};

// Sound ref flags
#define SOUND_DYING     (1 << 0)        // The sound should be deallocated once it stops playing

#define DIRRESID_EFFECTDIR -1
#define DIRRESID_DIALOGDIR -2

_STRUCTDEF(SSoundRef)
struct SSoundRef
{
    char *name;             // name of sound
    char *dir;              // directory where sound is stored
    int32_t resid;              // Resource id (negative value is directory id for WAV files)
    int32_t usecount;           // how many in use
    uint32_t flags;            // sound flags
    PTSound sound;          // sound data, may be more than one
};
typedef TPointerArray<SSoundRef, 32, 32> TSoundArray;

_CLASSDEF(TSoundPlayer)
class TSoundPlayer
{
  public:
    friend class TSound;

    TSoundPlayer() { DirectSound = nullptr; PrimaryBuffer = nullptr; soundlist.Clear(); }
    ~TSoundPlayer() { Close(); }

    bool Initialize();
    void Close();

    bool Functioning() { return (DirectSound && PrimaryBuffer); }

    void Pause();
    void Unpause();
        // Start and stop all sound effects (ie, game pausing/unpausing)

    void SetVolume(int32_t volume = 0);
        // An argument of 0 is the normal playing level

  // Finds sound id's by name
    int32_t FindSound(char *soundname, int32_t nr = -1);
        // Find a given sound
    int32_t NewSound(char *soundname, int32_t nr = -1);
        // Creates a new sound

  // Simple sound garbage-collector functions to make playing sounds a bit handier
  // These functions work with the sound id returned from FindSound()
    bool Mount(int32_t id);
        // Inform sound system to prepare this sound for later use
    bool Unmount(int32_t id);
        // Inform sound system that you are done with this sound
    bool Play(int32_t id, int32_t volume = 0, int32_t freq = 0, S3DPoint* spos = nullptr);
        // Play a mounted sound
    bool Stop(int32_t id);
        // Stop a playing, mounted sound

  // Sound name functions (has to search sound list every time, but easier to use)
    bool Mount(char *n, int32_t nr = -1) { return Mount(FindSound(n, nr)); }
        // Mount by sound name and number
    bool Unmount(char *n, int32_t nr = -1) { return Unmount(FindSound(n, nr)); }
        // Unmount by sound name and number
    bool Play(char *n, int32_t nr = -1, int32_t volume = -1, int32_t freq = -1)
      { return Play(FindSound(n, nr), volume, freq); }
        // Play a mounted sound  by name and number
    bool Stop(char *n, int32_t nr = -1) { return Stop(FindSound(n, nr)); }
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

    LPDIRECTSOUND DirectSound;

  private:
    bool SearchSoundDir(char *soundpath, char *subdir, int32_t dirresid);
        // Searches the given sound dir in the given sound path for all WAV files
        // and adds them to the sound list
    bool ReadSoundList();
        // Reads in sound list from SOUND.DEF file (for backwards compatibility)
    void DestroySoundList();
        // Destroys sound list
    void UpdateDying();
        // Loop through ref list and kill off any dying sounds

    LPDIRECTSOUNDBUFFER PrimaryBuffer;
//  LPDIRECTSOUND3DLISTENER Listener;

    TSoundArray soundlist;          // Array of active sounds

    S3DPoint listener_pos;
};

// Easy access function for one-time sounds
inline bool PLAY(char *x)
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

inline bool PLAYN(char *x, int32_t n)
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

#endif