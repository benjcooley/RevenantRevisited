// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  videocap.h - Video Capture Class                     *
// *************************************************************************

#pragma once

#if 0

#include "revenant.h"
#include "graphics.h"
#include "bitmap.h"
#include "bmsurface.h"

_CLASSDEF(TVideoCapture)
class TVideoCapture
{
  public:
    bool Initialize(int32_t bufmegs = 64, int32_t fps = FRAMERATE);
      // Initializes the capture system (allocates buffer of frame * framerate * secs.. 
      // this buffer can be HUGE, so make sure 'secs' value is reasonable)
    void Close();
      // Closes the capture object and deletes all buffers
    void Start();
      // Start a capture session
    void Stop();
      // Stop capturing video
    void SaveFrame();
      // Called by system to save the current video frame
    void Flush();
      // Flushes the current video buffers
    bool IsCapturing() { return saving; }
      // Returns true if the capture system is setup to capture
      
  private:
    int32_t framespersecond, seconds;
    bool saving;
    uint8_t *buffer;
    TBitmapSurface **bmsurface;
    int32_t totframes;
    int32_t frame;
    int32_t bufframe;
    int32_t bufsize;
    int32_t framesize;
    int32_t frameaccum;
    int32_t frameadd;
};

#endif
