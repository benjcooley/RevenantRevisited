// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  wavedata.h - Wave (sound) objects                    *
// *************************************************************************

#pragma once

#include "revenant.h"

/* general extended waveform format structure
   Use this for all NON PCM formats
   (information common to all formats)
*/
#ifndef _WAVEFORMATEX_
#define _WAVEFORMATEX_
typedef struct tWAVEFORMATEX
{
    uint16_t    wFormatTag;        /* format type */
    uint16_t    nChannels;         /* number of channels (i.e. mono, stereo...) */
    uint32_t    nSamplesPerSec;    /* sample rate */
    uint32_t    nAvgBytesPerSec;   /* for buffer estimation */
    uint16_t    nBlockAlign;       /* block size of data */
    uint16_t    wBitsPerSample;    /* Number of bits per sample of mono data */
    uint16_t    cbSize;            /* The count in bytes of the size of
                                    extra information (after cbSize) */

} WAVEFORMATEX;
typedef WAVEFORMATEX      *PWAVEFORMATEX;
typedef WAVEFORMATEX      *NPWAVEFORMATEX;
typedef WAVEFORMATEX      *LPWAVEFORMATEX;
#endif /* _WAVEFORMATEX_ */

// *************
// * TWaveData *
// *************

_CLASSDEF(TWaveData)
class TWaveData
{
  public:
    WAVEFORMATEX format;            // Format of wave; sent directly to DirectSound
    uint32_t size;                  // Number of bytes of data
    int32_t volume;                 // Volume adjustment
    int32_t loopstart;                 // Begin loop location
    int32_t loopend;                   // End loop location
    uint8_t data[1];                // Actual sound data
};

