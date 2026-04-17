// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  sound.cpp - Music and sound module                   *
// *************************************************************************
//
// Phase 2 port note: every body here is a stub. The original Win32 / MCI /
// DirectSound implementation is preserved verbatim inside `#if 0` blocks
// so it survives as a reference for the Phase 3 audio backend (miniaudio
// or sokol_audio). When that lands, the `#if 0` content migrates to
// `attic/src/sound_directsound.cpp` and this file gains real bodies.
//
// Keep the public signatures in sound.h unchanged — callers across the
// codebase (mappane.cpp, revmain.cpp, etc.) must keep compiling.

#include "sound.h"

#include "file.h"
#include "mainwnd.h"
#include "object.h"
#include "parse.h"
#include "resource.h"
#include "wavedata.h"

#include <math.h>
#include <stdio.h>

// *****************************
// * CD Sound System Functions *
// *****************************

#define SOUND_KILLCLICKS    (true)

#define DISTFACTOR (1.0f / (float)UNITSPERMETER)

static bool isopen;

#if 0 // TODO(port): real audio backend — Phase 3 (CD audio → ogg from data/Music/)
static int32_t cdstartticks;
static int32_t cdtracklen;
static int32_t cdtrackpos;
#endif

void CDOpen()
{
#if 0 // TODO(port): real audio backend — Phase 3 (MCI cdaudio)
    char err[120];
    uint32_t res = mciSendString("open cdaudio shareable", nullptr, 0, 0L);
    mciGetErrorString(res, err, 120);
#endif

    isopen = true;
}

void CDClose()
{
#if 0 // TODO(port): real audio backend — Phase 3 (MCI cdaudio)
    char err[120];
    uint32_t res = mciSendString("close cdaudio", nullptr, 0, 0L);
    mciGetErrorString(res, err, 120);
#endif

    isopen = false;
}

void CDPlayTrack(int32_t /*track*/)
{
#if 0 // TODO(port): real audio backend — Phase 3 (MCI cdaudio)
    char err[120];
    if (!SoundSystemOn)
        return;

    if (!isopen)
        CDOpen();

    char ret[40];
    char buf[40];

    uint32_t res = mciSendString("stop cdaudio", nullptr, 0, 0L);
    mciGetErrorString(res, err, 120);

    res = mciSendString("set cdaudio time format ms", nullptr, 0, 0L);
    mciGetErrorString(res, err, 120);

    wsprintf(buf, "status cdaudio position track %d", track);
    res = mciSendString(buf, ret, 39, 0L);
    mciGetErrorString(res, err, 120);

    cdtrackpos = atol(ret);

    wsprintf(buf, "status cdaudio length track %d", track);
    res = mciSendString(buf, ret, 39, 0L);
    mciGetErrorString(res, err, 120);

    cdtracklen = atol(ret);

    wsprintf(buf, "play cdaudio from %ld to %ld", cdtrackpos, cdtrackpos + cdtracklen - 1);

    cdstartticks = GetTickCount();

    res = mciSendString(buf, nullptr, 0, 0L);
    mciGetErrorString(res, err, 120);
#endif
}

uint32_t CDTrackLength(int32_t /*track*/)
{
#if 0 // TODO(port): real audio backend — Phase 3 (MCI cdaudio)
    if (!SoundSystemOn)
        return 0;

    if (!isopen)
        CDOpen();

    char ret[40];
    char buf[40];

    mciSendString("set cdaudio time format ms", nullptr, 0, 0L);

    wsprintf(buf, "status cdaudio length track %d", track);
    mciSendString(buf, ret, 39, 0L);

    return atol(ret);
#endif
    return 0;
}

void CDPlayRandomTrack()
{
#if 0 // TODO(port): real audio backend — Phase 3 (MCI cdaudio)
    if (!SoundSystemOn)
        return;

    if (!isopen)
        CDOpen();

    char ret[40];
    int32_t first = 1;
    int32_t last;

    mciSendString("status cdaudio number of tracks", ret, 39, 0L);
    last = atoi(ret);

    mciSendString("status cdaudio type track 1", ret, 39, 0L);
    if (!stricmp(ret, "audio"))
        first = 1;
    else
        first = 2;

  // Check if this is our CD
/*  char buf[40];
    mciSendString("set cdaudio time format ms", nullptr, 0, 0L);
    wsprintf(buf, "status cdaudio length track %d", 2);
    mciSendString(buf, ret, 39, 0L);
    int32_t tracklen = atol(ret);
    if (tracklen > 1000L && tracklen < 1000L)
        first = 2;
*/

    static int32_t played[20];
    int32_t numplayed = min(20, last - first);
    int32_t play;

    bool wasplayed;
    do
    {
        wasplayed = false;
        play = random(first, last);
        for (int32_t c = 0; c < numplayed; c++)
        {
            if (play == played[c])
            {
                wasplayed = true;
                break;
            }
        }
    } while (wasplayed);

    CDPlayTrack(play);

    for (int32_t c = 0; c < numplayed - 1; c++)
        played[c] = played[c + 1];
    played[numplayed - 1] = play;
#endif
}

void CDStop()
{
#if 0 // TODO(port): real audio backend — Phase 3 (MCI cdaudio)
    if (!isopen)
        CDOpen();

    if (!SoundSystemOn)
        return;

    mciSendString("stop cdaudio", nullptr, 0, 0L);
#endif
}

bool CDPlaying()
{
#if 0 // TODO(port): real audio backend — Phase 3 (MCI cdaudio)
    if (!isopen)
        CDOpen();

    if (cdtracklen == 0)
        return false;

    if (GetTickCount() - cdstartticks < (uint32_t)cdtracklen)
        return true;
#endif

    return false;
}

void CDSetVolume(uint16_t /*volume*/)
{
#if 0 // TODO(port): real audio backend — Phase 3 (winmm aux device)
    if (!isopen)
        CDOpen();

    int32_t devs = auxGetNumDevs();

    AUXCAPS caps;

    for (int32_t c = 0; c < devs; c++)
    {
        auxGetDevCaps(c, &caps, sizeof(AUXCAPS));
        if (caps.wTechnology == AUXCAPS_CDAUDIO)
        {
            auxSetVolume(c, MAKELONG(volume,volume));
            return;
        }
    }
#endif
}

// *****************
// * Sound Effects *
// *****************

// Loads a wave data object and returns a pointer to it.
// NOTE: sound.h declares a different LoadWave() (returns PTWaveData). The
// free function below is the 1998 internal helper that was only called
// from TSound::Load. It uses Win32 mmio / LPBYTE and is kept under `#if 0`
// in its entirety — no stub body, because it has no external callers.
#if 0 // TODO(port): real audio backend — Phase 3 (winmm mmio WAVE loader)
bool LoadWave(char *filename, LPWAVEFORMATEX &format, uint32_t &size, LPBYTE &data)
{
    HMMIO hmmio = mmioOpen(filename, nullptr, MMIO_READ | MMIO_ALLOCBUF);
    if (!hmmio)
        return false;

    MMCKINFO mmckchunkinfo;
    mmckchunkinfo.fccType = mmioFOURCC('W', 'A', 'V', 'E');
    if (mmioDescend(hmmio, (LPMMCKINFO)&mmckchunkinfo, nullptr, MMIO_FINDRIFF))
    {
        mmioClose(hmmio, 0);
        return false;
    }

    MMCKINFO subchunk;
    subchunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
    if (mmioDescend(hmmio, &subchunk, &mmckchunkinfo, MMIO_FINDCHUNK))
    {
        mmioClose(hmmio, 0);
        return false;
    }

  //read the fmt chunk
    uint32_t dwfmtsize = subchunk.cksize;
    format = (LPWAVEFORMATEX)new uint8_t[max(dwfmtsize, sizeof(WAVEFORMATEX))];
    memset(format, 0, dwfmtsize);
    if (mmioRead(hmmio, (HPSTR)format, dwfmtsize) != (int32_t)dwfmtsize)
    {
        mmioClose(hmmio, 0);
        return false;
    }

  // search data chunk
    subchunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
    if (mmioDescend(hmmio, &subchunk, &mmckchunkinfo,
                MMIO_FINDCHUNK))
    {
        delete format;
        mmioClose(hmmio, 0);
        return false;
    }

    size = subchunk.cksize;
    if (size == 0)
    {
        delete format;
        mmioClose(hmmio, 0);
        return false;
    }

    data = new uint8_t[size];

    if (mmioRead(hmmio, (char *)data, size) != (int32_t)size)
    {
        delete data;
        delete format;
        mmioClose(hmmio, 0);
        return false;
    }

    mmioClose(hmmio, 0);

    return true;
}
#endif

TSound::TSound()
{
    SoundBuffer = nullptr;
    looping = false;
    next = nullptr;
    sound_volume = 0;
    memset(&listener_pos, 0, sizeof(listener_pos));
    memset(&sound_pos, 0, sizeof(sound_pos));
    memset(&format, 0, sizeof(format));
    size = 0;
}

TSound::~TSound()
{
#if 0 // TODO(port): real audio backend — Phase 3 (DirectSound buffer release)
    if (SoundBuffer)
    {
        SoundBuffer->Release();
        SoundBuffer = nullptr;
    }
#endif
    if (next)
        delete next;
}

bool TSound::IsPlaying()
{
#if 0 // TODO(port): real audio backend — Phase 3 (DSBSTATUS_PLAYING bit)
    return (GetStatus() & DSBSTATUS_PLAYING);
#endif
    return false;
}

bool TSound::IsLooping()
{
#if 0 // TODO(port): real audio backend — Phase 3 (DSBSTATUS_LOOPING bit)
    return (GetStatus() & DSBSTATUS_LOOPING);
#endif
    return looping;
}

// Load from wave file
PTSound TSound::Load(char * /*name*/, int32_t /*dirresid*/)
{
#if 0 // TODO(port): real audio backend — Phase 3 (file-based WAV load)
    char filename[MAXPATHLEN];

    makepath(ResourcePath, filename, MAXPATHLEN - 1);
    strcat(filename, "sound\\");
    if (dirresid == DIRRESID_EFFECTDIR)
        strcat(filename, "effects");
    else if (dirresid == DIRRESID_DIALOGDIR)
        strcat(filename, Language);
    strcat(filename, "\\");
    strcat(filename, name);
    strcat(filename, ".wav");

    LPWAVEFORMATEX format;
    uint32_t size;
    LPBYTE data;

    if (!LoadWave(filename, format, size, data))
        return nullptr;

    PTSound sound = Load(format, size, data, false);

    delete format;
    delete data;

    return sound;
#endif
    return nullptr;
}

// Load from resource file
PTSound TSound::Load(int32_t /*resid*/)
{
#if 0 // TODO(port): real audio backend — Phase 3 (resource-pack WAV load)
    char filename[MAXPATHLEN];
    strcpy(filename, "\\sound\\old\\wave");

    PTWaveData wave = (PTWaveData)LoadResource(filename, resid);
    if (!wave)
        return nullptr;

    bool looping = (wave->loopend - wave->loopstart) > 0;

    PTSound sound = Load(&(wave->format), wave->size, wave->data, looping);

    delete wave;

    return sound;
#endif
    return nullptr;
}

// Load from a resource buffer
PTSound TSound::Load(WAVEFORMATEX * /*format*/, uint32_t /*size*/, uint8_t * /*data*/, bool /*looping*/)
{
#if 0 // TODO(port): real audio backend — Phase 3 (DirectSound buffer + ACM decode)
    MMRESULT mmres;
    HACMSTREAM acmstream;
    bool acmdecomp;

    if (!format || !data)
        return nullptr;

    if (!SoundPlayer.Functioning())
        return nullptr;

    PTSound sound = new TSound;

  // Copy wave format
    memcpy(&sound->format, format, sizeof(WAVEFORMATEX));
    if (sound->format.wFormatTag != WAVE_FORMAT_PCM)
    {
        sound->format.wFormatTag = WAVE_FORMAT_PCM;
//      sound->format.nChannels = 1;
        sound->format.wBitsPerSample = 16;
        sound->format.nBlockAlign = sound->format.wBitsPerSample * sound->format.nChannels / 8;
        sound->format.nAvgBytesPerSec = sound->format.nSamplesPerSec * sound->format.nBlockAlign;
        sound->format.cbSize = 0;

        mmres = acmStreamOpen(&acmstream, nullptr, format, &(sound->format),
            nullptr, nullptr, nullptr, ACM_STREAMOPENF_NONREALTIME);
        if (mmres != 0)
        {
            delete sound;
            return nullptr;
        }

        acmStreamSize(acmstream, size, (uint32_t *)&(sound->size), ACM_STREAMSIZEF_SOURCE);

        acmdecomp = true;
    }
    else
    {
        sound->size = size;
        acmdecomp = false;
    }

    DSBUFFERDESC desc;
    memset(&desc, 0, sizeof(DSBUFFERDESC));
    desc.dwSize = sizeof(DSBUFFERDESC);
//  desc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_CTRLVOLUME | DSBCAPS_STATIC;
    desc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_STATIC;
    desc.dwBufferBytes = sound->size;
    desc.lpwfxFormat = &sound->format;

    HRESULT res = SoundPlayer.DirectSound->CreateSoundBuffer(&desc, &(sound->SoundBuffer), nullptr);
    if (res != DS_OK)
    {
        delete sound;
        return nullptr;
    }

  // Get 3D buffer
//  res = sound->SoundBuffer->QueryInterface(IID_IDirectSound3DBuffer, (LPVOID *)&(sound->SoundBuffer3D));
//  if (res != DS_OK)
//  {
//      delete sound;
//      return nullptr;
//  }

    LPVOID buffer;
    res = sound->SoundBuffer->Lock(0, sound->size, &buffer, &sound->size, nullptr, nullptr, 0);
    if (res != DS_OK)
    {
        sound->SoundBuffer->Release();
        delete sound;
        return nullptr;
    }

    if (acmdecomp)
    {
        ACMSTREAMHEADER acmheader;

        memset(&acmheader, 0, sizeof(ACMSTREAMHEADER));
        acmheader.cbStruct = sizeof(ACMSTREAMHEADER);
        acmheader.pbSrc = data;
        acmheader.cbSrcLength = size;
        acmheader.pbDst = (LPBYTE)buffer;
        acmheader.cbDstLength = sound->size;

        mmres = acmStreamPrepareHeader(acmstream, &acmheader, 0);
        if (mmres == 0)
            mmres = acmStreamConvert(acmstream, &acmheader, ACM_STREAMCONVERTF_BLOCKALIGN);

        if (mmres != 0)
        {
            sound->SoundBuffer->Unlock(&buffer, sound->size, nullptr, 0);
            sound->SoundBuffer->Release();
            delete sound;
            return nullptr;
        }

        acmStreamClose(acmstream, 0);
    }
    else
        memcpy(buffer, data, size);


#if SOUND_KILLCLICKS

#define SOUND_RAMPLEVELS    (32)
    if (!looping)
    {
        // Kill clicks 16 bit mono signed
        if (sound->format.nChannels == 1 &&
            sound->format.wBitsPerSample == 16 &&
            sound->size > 64)
        {
            short *s = (short *)buffer;
            short *e = (short *)((uint8_t *)buffer + sound->size - 2);
            for (int32_t c = 0; c < SOUND_RAMPLEVELS; c++, s++, e--)
            {
                *s = (short)(*s * (float)(c / SOUND_RAMPLEVELS));
                *e = (short)(*e * (float)(c / SOUND_RAMPLEVELS));
            }
        }

        // Kill clicks 8 bit mono unsigned
        if (sound->format.nChannels == 1 &&
            sound->format.wBitsPerSample == 8 &&
            sound->size > 32)
        {
            uint8_t *s = (uint8_t *)buffer;
            uint8_t *e = (uint8_t *)((uint8_t *)buffer + sound->size - 1);
            for (int32_t c = 0; c < SOUND_RAMPLEVELS; c++, s++, e--)
            {
                *s = (uint8_t)(*s * (float)(c / SOUND_RAMPLEVELS));
                *e = (uint8_t)(*e * (float)(c / SOUND_RAMPLEVELS));
            }
        }
    }
#endif

    sound->SoundBuffer->Unlock(&buffer, sound->size, nullptr, 0);

  // Set 3D info
//  sound->SoundBuffer3D->SetMode(DS3DMODE_DISABLE, DS3D_IMMEDIATE);
//  sound->SoundBuffer3D->SetConeAngles(DS3D_MAXCONEANGLE, DS3D_MAXCONEANGLE, DS3D_IMMEDIATE);
//  sound->SoundBuffer3D->SetConeOrientation(0.0f, 0.0f, 0.0f, DS3D_IMMEDIATE);
//  sound->SoundBuffer3D->SetConeOutsideVolume(DSBVOLUME_MAX, DS3D_IMMEDIATE);
//  sound->SoundBuffer3D->SetMinDistance(256.0f, DS3D_IMMEDIATE);
//  sound->SoundBuffer3D->SetMaxDistance(256.0f + 768.0f, DS3D_IMMEDIATE);

    sound->looping = looping;
    sound->next = nullptr;

    return sound;
#endif
    return nullptr;
}

PTSound TSound::Duplicate()
{
#if 0 // TODO(port): real audio backend — Phase 3 (DirectSound DuplicateSoundBuffer)
    if (!SoundPlayer.Functioning())
        return nullptr;

    PTSound sound = new TSound;

  // Copy format and size
    memcpy(&sound->format, &format, sizeof(WAVEFORMATEX));
    sound->size = size;

  // Copy buffer
    HRESULT res = SoundPlayer.DirectSound->DuplicateSoundBuffer(SoundBuffer, &sound->SoundBuffer);
    if (res != DS_OK)
    {
        delete sound;
        return nullptr;
    }

  // Great!
    return sound;
#endif
    return nullptr;
}


#define MINDIST 256
#define MAXDIST 1024


// lpos is the listeners position
// spos is the sound's position
int32_t CalcPan(S3DPoint* lpos, S3DPoint* spos)
{
    int32_t pan = 0;
    S3DPoint tmp_pos;

    if (lpos && spos)
    {
        int32_t distance = ::Distance(*lpos, *spos);

        if (distance < MAXDIST)
        {
            // get the difference in positions...
            memcpy(&tmp_pos, spos, sizeof(S3DPoint));
            tmp_pos.x -= lpos->x;
            tmp_pos.y -= lpos->y;
            // Get panning based on angle from position
            double a = atan2(tmp_pos.y, tmp_pos.x);
            a += (M_PI / 2.0);
#if 0 // TODO(port): real audio backend — Phase 3 (DSBPAN_RIGHT constant)
            pan = (int32_t)(cos(a) * (double)DSBPAN_RIGHT / 16);
#else
            // DSBPAN_RIGHT was +10000 in DirectSound; keep the math shape so
            // the Phase 3 backend can swap in its own pan range unit.
            pan = (int32_t)(cos(a) * 10000.0 / 16.0);
#endif
        }
    }

    return pan;
}

// lpos is the listeners position
// spos is the sound's position
int32_t CalcDirectionalVol(int32_t orig_vol, S3DPoint* lpos, S3DPoint* spos)
{
    int32_t vol = orig_vol;

    if (lpos && spos)
    {
        // Get volume based on distance
        int32_t distance = ::Distance(*lpos, *spos);

        if (distance > MAXDIST)
            vol = 0;
        else if (distance > MINDIST)
#if 0 // TODO(port): real audio backend — Phase 3 (DSBVOLUME_MIN constant)
            vol = ((orig_vol - DSBVOLUME_MIN) * (MAXDIST - distance) / (MAXDIST - MINDIST)) + DSBVOLUME_MIN;
#else
            // DSBVOLUME_MIN was -10000 (hundredths of a dB) in DirectSound.
            vol = ((orig_vol - (-10000)) * (MAXDIST - distance) / (MAXDIST - MINDIST)) + (-10000);
#endif
    }

    return vol;
}


void TSound::SetListenerPos(S3DPoint* lpos)
{
    if (lpos)
    {
        // get the listener's position
        memcpy(&listener_pos, lpos, sizeof(S3DPoint));

#if 0 // TODO(port): real audio backend — Phase 3 (SoundBuffer SetVolume/SetPan)
        int32_t volume = CalcDirectionalVol(sound_volume, &listener_pos, &sound_pos);
        int32_t pan = CalcPan(&listener_pos, &sound_pos);

        SoundBuffer->SetVolume(volume);
        SoundBuffer->SetPan(pan);
#endif
    }
}

void TSound::SetSoundPos(S3DPoint* spos)
{
    if (spos)
    {
        // get the sound's position
        memcpy(&sound_pos, spos, sizeof(S3DPoint));

#if 0 // TODO(port): real audio backend — Phase 3 (SoundBuffer SetVolume/SetPan)
        int32_t volume = CalcDirectionalVol(sound_volume, &listener_pos, &sound_pos);
        int32_t pan = 0;
        if (volume)
            pan = CalcPan(&listener_pos, &sound_pos);

        SoundBuffer->SetVolume(volume);
        SoundBuffer->SetPan(pan);
#endif
    }
}

// lpos is the listeners position
// spos is the sound's position
void TSound::Play(int32_t volume, int32_t /*freq*/, S3DPoint* lpos, S3DPoint* spos)
{
    if (lpos)
    {
        // get the listener's position
        memcpy(&listener_pos, lpos, sizeof(S3DPoint));

        if (spos)   // if we are given a sound position, then use it...
            memcpy(&sound_pos, spos, sizeof(S3DPoint));
        else        // otherwise, center it on the listener's position for right now...
            memcpy(&sound_pos, lpos, sizeof(S3DPoint));

        // Get volume based on delta point
        volume = CalcDirectionalVol(volume, lpos, spos);
    }

    sound_volume = volume;
        // store this value away for possible use later on

#if 0 // TODO(port): real audio backend — Phase 3 (DirectSound Play/SetVolume/SetFrequency/SetPan)
    int32_t pan = 0;

    if (!SoundPlayer.Functioning() || SoundBuffer == nullptr)
        return;

    if (lpos)
    {
        if (volume)
            pan = CalcPan(lpos, spos);
    }

    SoundBuffer->SetVolume(volume);
    SoundBuffer->SetFrequency(freq);
    SoundBuffer->SetPan(pan);

//  SoundBuffer->SetCurrentPosition(0);
    HRESULT error = SoundBuffer->Play(0, 0, looping ? DSBPLAY_LOOPING : 0);
    if (error != DS_OK)
    {
        if (error == DSERR_BUFFERLOST)
            error = 0;
        else if (error == DSERR_INVALIDCALL)
            error = 0;
        else if (error == DSERR_INVALIDPARAM)
            error = 0;
        else if (error == DSERR_PRIOLEVELNEEDED)
            error = 0;
    }
#endif
}

void TSound::Stop()
{
#if 0 // TODO(port): real audio backend — Phase 3 (DirectSound buffer stop)
    if (!SoundPlayer.Functioning() || SoundBuffer == nullptr)
        return;

    SoundBuffer->Stop();
#endif
}

uint32_t TSound::GetStatus()
{
#if 0 // TODO(port): real audio backend — Phase 3 (DirectSound buffer GetStatus)
    if (!SoundPlayer.Functioning() || SoundBuffer == nullptr)
        return 0;

    uint32_t status;
    SoundBuffer->GetStatus(&status);

    return status;
#endif
    return 0;
}

//void TSound::SetPosition(int32_t x, int32_t y, int32_t z)
//{
//  SoundBuffer3D->SetMode(DS3DMODE_HEADRELATIVE, DS3D_IMMEDIATE);
//  SoundBuffer3D->SetPosition((float)x * DISTFACTOR, (float)y * DISTFACTOR, (float)z * DISTFACTOR, DS3D_IMMEDIATE);
//}

//void TSound::SetVelocity(int32_t x, int32_t y, int32_t z)
//{
//  SoundBuffer3D->SetMode(DS3DMODE_HEADRELATIVE, DS3D_IMMEDIATE);
//  SoundBuffer3D->SetPosition((float)x * DISTFACTOR, (float)y * DISTFACTOR, (float)z * DISTFACTOR, DS3D_IMMEDIATE);
//}

//void TSound::SetOrientation(float x, float y, float z)
//{
//  SoundBuffer3D->SetMode(DS3DMODE_HEADRELATIVE, DS3D_IMMEDIATE);
//  SoundBuffer3D->SetConeOrientation(x, y, z, DS3D_IMMEDIATE);
//}

// ***********************
// * Sound Effect Player *
// ***********************

bool TSoundPlayer::Initialize()
{
    // Phase 2 stub: no audio device. Keep returning true so the game
    // startup path does not treat audio init as fatal — this matches
    // the original behavior (it always returned true, even on failure).

#if 0 // TODO(port): real audio backend — Phase 3 (DirectSound device + primary buffer)
    HRESULT res = DirectSoundCreate(nullptr, &DirectSound, nullptr);
    if (res != DS_OK)
        return true;

    res = DirectSound->SetCooperativeLevel(MainWindow.Hwnd(), DSSCL_PRIORITY);
    if (res != DS_OK)
    {
        DirectSound = nullptr;
        return true;
    }

    DSBUFFERDESC desc;
    memset(&desc, 0, sizeof(DSBUFFERDESC));
    desc.dwSize = sizeof(DSBUFFERDESC);
    desc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;
    desc.dwBufferBytes = 0;
    desc.lpwfxFormat = nullptr;

    res = DirectSound->CreateSoundBuffer(&desc, &PrimaryBuffer, nullptr);
    if (res != DS_OK)
    {
        DirectSound->Release();
        DirectSound = nullptr;
        return true;
    }

  // Set primary buffer format     WAVEFORMATEX wfx;
    WAVEFORMATEX wfx;
    memset(&wfx, 0, sizeof(WAVEFORMATEX));
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 2;
    wfx.nSamplesPerSec = 44100;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = wfx.wBitsPerSample / 8 * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    res = PrimaryBuffer->SetFormat(&wfx);
    if (res != DS_OK)
    {
        DirectSound->Release();
        DirectSound = nullptr;
        return true;
    }

    // Get listener interface
//  res = PrimaryBuffer->QueryInterface(IID_IDirectSound3DListener, (LPVOID *)&Listener);
//  if (res != DS_OK)
//  {
//      DirectSound->Release();
//      DirectSound = nullptr;
//      return true;
//  }

  // Set the listener format
//  Listener->SetRolloffFactor(1.0f, DS3D_IMMEDIATE);
//  Listener->SetDopplerFactor(0.0f, DS3D_IMMEDIATE);
//  Listener->SetVelocity(0.0f, 0.0f, 0.0f, DS3D_IMMEDIATE);
//  Listener->SetOrientation(-0.7f, -0.7f, 0.0f, 0.0f, 0.0f, 0.99f, DS3D_IMMEDIATE);

  // Start primary buffer
    PrimaryBuffer->Play(0, 0, DSBPLAY_LOOPING);

  // Reads in sound list
    ReadSoundList();
#endif

    DirectSound = nullptr;
    PrimaryBuffer = nullptr;
    return true;
}

void TSoundPlayer::Close()
{
    DestroySoundList();

#if 0 // TODO(port): real audio backend — Phase 3 (DirectSound buffer / device release)
    if (PrimaryBuffer)
    {
        PrimaryBuffer->Release();
        PrimaryBuffer = nullptr;
    }

    if (DirectSound)
    {
        DirectSound->Release();
        DirectSound = nullptr;
    }
#endif

    PrimaryBuffer = nullptr;
    DirectSound = nullptr;
}

void TSoundPlayer::Pause()
{
#if 0 // TODO(port): real audio backend — Phase 3 (PrimaryBuffer->Stop)
    if (Functioning())
        PrimaryBuffer->Stop();
#endif
}

void TSoundPlayer::Unpause()
{
#if 0 // TODO(port): real audio backend — Phase 3 (PrimaryBuffer->Play looping)
    if (Functioning())
        PrimaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
#endif
}

void TSoundPlayer::SetVolume(int32_t /*volume*/)
{
#if 0 // TODO(port): real audio backend — Phase 3 (PrimaryBuffer->SetVolume)
    if (Functioning())
        PrimaryBuffer->SetVolume(volume);
#endif
}

// The sound system high-level (easy-interface) functions

int32_t TSoundPlayer::FindSound(char *soundname, int32_t nr)
{
    char buf[80];
    if (nr >= 0)
    {
        sprintf(buf, "%s%d", soundname, nr);
        return FindSound(buf, -1);
    }
    else
        strcpy(buf, soundname);

    for (int32_t c = 0; c < soundlist.NumItems(); c++)
    {
        PSSoundRef ref = soundlist[c];
        if (!ref)
            continue;

        if (!stricmp(ref->name, buf))
            return c;
    }

    return -1;
}

int32_t TSoundPlayer::NewSound(char *soundname, int32_t nr)
{
    char buf[80];
    if (nr >= 0)
    {
        sprintf(buf, "%s%d", soundname, nr);
        return FindSound(buf, -1);
    }
    else
        strcpy(buf, soundname);

    PSSoundRef ref = new SSoundRef;
    ref->name = strdup(buf);
    ref->resid = -1;
    ref->usecount = 0;
    ref->sound = nullptr;

    int32_t id = soundlist.Add(ref);
    if (id < 0)
    {
        free(ref->name);
        delete ref;
        return -1;
    }

    return id;
}

void TSoundPlayer::UpdateDying()
{
    for (int32_t c = 0; c < soundlist.NumItems(); c++)
    {
        PSSoundRef ref = soundlist[c];
        if (!ref)
            continue;

      // Clear any duplicated sounds for this ref if they've finished playing
        if (ref->sound && ref->sound->Next())
        {
            PTSound prev, snd, next;

            prev = ref->sound;
            next = prev->Next();

            while ((snd = next))
            {
                next = snd->Next();

                if (snd->IsPlaying())
                    prev = snd;
                else
                {
                    prev->SetNext(snd->Next());
                    snd->SetNext(nullptr);
                    delete snd;
                }
            }
        }

      // Clear the main sound if its SOUND_DYING flag is set
        if (ref->flags & SOUND_DYING)
        {
            if (ref->sound && (!ref->sound->IsPlaying() || ref->sound->IsLooping()))
            {
                delete ref->sound;
                ref->sound = nullptr;
                ref->flags &= ~SOUND_DYING;
            }
        }
    }
}

bool TSoundPlayer::Mount(int32_t id)
{
    // Phase 2 stub: no functioning audio device means every Mount is a no-op.
    if (!Functioning() || id < 0 || id >= soundlist.NumItems())
        return false;

#if 0 // TODO(port): real audio backend — Phase 3 (load + refcount mounted sound)
    PSSoundRef ref = soundlist[id];
    if (!ref)
        return false;

    if (ref->usecount < 1)
    {
        if (ref->flags & SOUND_DYING)
            ref->flags &= ~SOUND_DYING;
        else
        {
            if (ref->resid >= 0)
                ref->sound = TSound::Load(ref->resid);            // Resid is WAVE.### id num
            else
                ref->sound = TSound::Load(ref->name, ref->resid); // Resid is directory id num
        }

        if (ref->sound)
            ref->usecount = 1;
        else
            ref->usecount = 0;
    }
    else
        ref->usecount++;

    UpdateDying();

    return true;
#endif
    return false;
}

bool TSoundPlayer::Unmount(int32_t id)
{
    if (!Functioning() || id < 0 || id >= soundlist.NumItems())
        return false;

#if 0 // TODO(port): real audio backend — Phase 3 (refcount + retire mounted sound)
    PSSoundRef ref = soundlist[id];
    if (!ref)
        return false;

    ref->usecount--;

    if (ref->usecount < 1)
    {
        if (ref->sound)
        {
            if (ref->sound->IsPlaying() && !ref->sound->IsLooping())
                ref->flags |= SOUND_DYING;
            else
            {
                delete ref->sound;
                ref->sound = nullptr;
            }
        }

        ref->usecount = 0;
    }

    UpdateDying();

    return true;
#endif
    return false;
}

bool TSoundPlayer::Play(int32_t id, int32_t /*volume*/, int32_t /*freq*/, S3DPoint* /*spos*/)
{
    if (!Functioning() || id < 0 || id >= soundlist.NumItems())
        return false;

#if 0 // TODO(port): real audio backend — Phase 3 (play or duplicate+play a mounted sound)
    PSSoundRef ref = soundlist[id];
    if (!ref)
        return false;

    if (ref && ref->sound)
    {
        if (!ref->sound->IsPlaying())
            ref->sound->Play(volume, freq, &listener_pos, spos);
        else
        {
            // create a duplicate of the buffer to play seperately
            PTSound newsound = ref->sound->Duplicate();
            if (!newsound)
                return false;

            newsound->SetNext(ref->sound->Next());
            ref->sound->SetNext(newsound);
            newsound->Play(volume);
        }
    }

    UpdateDying();

    return true;
#endif
    return false;
}

bool TSoundPlayer::Stop(int32_t id)
{
    if (!Functioning() || id < 0 || id >= soundlist.NumItems())
        return false;

#if 0 // TODO(port): real audio backend — Phase 3 (stop a mounted sound)
    PSSoundRef ref = soundlist[id];
    if (!ref)
        return false;

    if (ref && ref->sound)
        ref->sound->Stop();

    return true;
#endif
    return false;
}

// Returns actual sound object for sound (sound must be MOUNTED or this will return nullptr)
PTSound TSoundPlayer::GetSound(int32_t id)
{
    if (!Functioning() || id < 0 || id >= soundlist.NumItems())
        return nullptr;

    PSSoundRef ref = soundlist[id];
    if (!ref)
        return nullptr;

    return ref->sound;
}




void TSoundPlayer::SetListenerPos(int32_t x, int32_t y, int32_t z)
{
    PSSoundRef ref;

    listener_pos.x = x; listener_pos.y = y; listener_pos.z = z;

    // iterate through all the mounted sounds and set the new listener's position
    for (int32_t c = 0; c < soundlist.NumItems(); c++)
    {
        // get the next sound, but if it's null, then continue on...
        if (!(ref = soundlist[c]))
            continue;

        // now set the listener's position for this sound...
        if (ref->sound)
            ref->sound->SetListenerPos(&listener_pos);
    }
}

//void TSoundPlayer::SetVelocity(int32_t x, int32_t y, int32_t z)
//{
//  Listener->SetPosition((float)x * DISTFACTOR, (float)y * DISTFACTOR, (float)z * DISTFACTOR, DS3D_IMMEDIATE);
//}

//void TSoundPlayer::SetOrientation(float facex, float facey, float facez,
//  float topx, float topy, float topz)
//{
//  Listener->SetOrientation(facex, facey, facez, topx, topy, topz, DS3D_IMMEDIATE);
//}

//void TSoundPlayer::CommitSettings()
//{
//  Listener->CommitDeferredSettings();
//}

bool TSoundPlayer::SearchSoundDir(char * /*soundpath*/, char * /*subdir*/, int32_t /*dirresid*/)
{
#if 0 // TODO(port): real audio backend — Phase 3 (_findfirst WAV enumeration)
    char fname[MAXPATHLEN];

    PSSoundRef ref;
    char name[128];

  // ****** Search for sounds in current sound directory

    strcpy(fname, soundpath);
    strcat(fname, subdir);
    strcat(fname, "\\*.wav");

    struct _finddata_t data;

    int32_t found, handle;
    found = handle = _findfirst(fname, &data);
    while (found != -1)
    {
        strcpy(name, data.name);
        char *p = strchr(name, '.');
        if (p)
            *p = nullptr;

        ref = new SSoundRef;
        ref->name = strdup(name);
        ref->resid = dirresid;          // A negative number defined by DIRRESID_xxx macros
        ref->usecount = 0;
        ref->sound = nullptr;

        soundlist.Add(ref);

        found = _findnext(handle, &data);
    }

    return true;
#endif
    return true;
}

bool TSoundPlayer::ReadSoundList()
{
    soundlist.Clear();

#if 0 // TODO(port): real audio backend — Phase 3 (parse sound.def + enumerate WAVs)
    char fname[MAXPATHLEN];
    sprintf(fname, "%ssound.def", ClassDefPath);

    FILE *fp = TryOpen(fname, "rb");
    if (fp == nullptr)
        return false;

    TFileParseStream s(fp, fname);
    TToken t(s);

    t.Get();

    PSSoundRef ref;
    char name[128];
    int32_t resid;

    do
    {
        if (t.Type() == TKN_RETURN || t.Type() == TKN_WHITESPACE)
            t.LineGet();

        if (t.Type() == TKN_EOF)
            break;

        if (!Parse(t, "%s %d", name, &resid))
            return false;

        ref = new SSoundRef;
        ref->name = strdup(name);
        ref->resid = resid;
        ref->usecount = 0;
        ref->sound = nullptr;

        soundlist.Add(ref);

    } while (t.Type() != TKN_EOF);

    fclose(fp);

  // ****** Search for sounds in current sound directory

    makepath(ResourcePath, fname, MAXPATHLEN - 1);
    strcat(fname, "sound\\");

    if (!SearchSoundDir(fname, "effects", DIRRESID_EFFECTDIR))
        return false;

    if (!SearchSoundDir(fname, Language, DIRRESID_DIALOGDIR))
        return false;
#endif

    return true;
}

void TSoundPlayer::DestroySoundList()
{
    for (int32_t c = 0; c < soundlist.NumItems(); c++)
    {
        PSSoundRef ref = soundlist[c];
        if (!ref)
            continue;

        if (ref->sound)
            delete ref->sound;

        free(ref->name);
    }

    soundlist.DeleteAll();
}
