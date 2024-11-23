// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *              animation.h - Animation Definition File                  *
// *************************************************************************

#pragma once

#include "revenant.h"
#include "animdata.h"

class TAnimation : public TAnimationData
{
  public:

    TAnimation();
    static PTAnimation Load(int32_t resource);
      // Loads animation resource
    int32_t  GetDecBufSize(int32_t frame) {return frames[frame].decbufsize;}
    bool Decompress(PSDrawBlock db, int32_t frame, bool Transparent);
      // Decompresses frame to surface
    void Put(PTSurface surface, void *decbuf, int32_t frame, PSDrawParam dp);
      // Draws a frame to a surface
    void Stretch(PTSurface surface, PTSurface decbuf, int32_t x, int32_t y, int32_t size, int32_t frame);
      // Stretches a frame of animation to a surface
    PTBitmap GetFrame(int32_t frame);
      // Returns bitmap for the given frame
    int32_t NumFrames();
      // Returns number of frames in the animation
    ~TAnimation();

  private:
    void Move(int32_t &x, int32_t &y, int32_t frame, int32_t orientation = 0);
      // Returns new X/Y position based on frame dx/dy.
    void SizeMove(int32_t &x, int32_t &y, int32_t size, int32_t frame, int32_t orientation = 0);
      // Returns new X/Y position based on frame dx/dy. Scales movement based on size.
};

