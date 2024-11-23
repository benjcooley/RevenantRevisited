// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  animimage.h - TAnimImagery object                    *
// *************************************************************************

#pragma once

#include "revenant.h"
#include "animimagebody.h"
#include "bitmap.h"
#include "animation.h"
#include "resource.h"
#include "object.h"

// ****************
// * TAnimImagery *
// ****************

// This object displays an anim image resource to the screen.
// It draws lit or unlit data for various states, and also
// creates a 2D sprite animator for animated objects.

_CLASSDEF(TAnimImagery)
class TAnimImagery : public TObjectImagery
{
  public:
    TAnimImagery(int32_t imageryid) : TObjectImagery(imageryid) {}
    virtual ~TAnimImagery() {}

    PSAnimImageryState GetAnimState(int32_t state)
      { return  &(((PSAnimImageryBody)GetBody())->states[state]); }
        // Returns pointer to anim imagery state

  // Caching functions
    virtual void CacheImagery();
      // Causes imagery to load itself and get ready for drawing
      // (Called by the SectorCache system to cause imagery to be loaded and decompressed)
      // AnimImagery uses this to call the TBitmap::CacheChunks() function  

    virtual void DrawUnlit(PTObjectInstance oi, PTSurface surface);
        // Draws unlit imagery to background
    virtual void DrawLit(PTObjectInstance oi, PTSurface surface);
        // Draws lit imagery to background
    virtual bool GetZ(PTObjectInstance oi, PTSurface surface);
        // Get first unclipped zbuffer point by simulating drawing to the surface
    virtual void DrawSelected(PTObjectInstance oi, PTSurface surface);
        // Causes image to draw selection (hilighting) around itself
    virtual bool AlwaysOnTop(PTObjectInstance oi);
        // Lit imagery is always on top because it has no zbuffer
    virtual PTBitmap GetStillImage(int32_t state, int32_t num = 0)
      { return (PTBitmap)GetAnimState(state)->still.ptr(); }
      // Returns the still image for the item
    virtual PTBitmap GetInvImage(int32_t state, int32_t num = 0)
      { return (PTBitmap)GetAnimState(state)->invitem.ptr(); }
      // Returns the inventory image for the item
    virtual PTAnimation GetInvAnimation(int32_t state)
      { return (PTAnimation)GetAnimState(state)->invanim.ptr(); }
        // Get inventory animation for state
    virtual PTAnimation GetAnimation(int32_t state) { return (PTAnimation)GetAnimState(state)->anim.ptr(); }
        // Get animation for state
    virtual uint32_t GetImageFlags(int32_t state) { return GetAnimState(state)->flags; }
        // Gets imagery flags for state
    virtual int32_t GetAniLength(int32_t state)
      { if (GetAnimation(state)) return GetAnimation(state)->NumFrames(); return 0; }
      // Returns length (in frames) of animation for this state
    virtual bool SaveBitmap(char *path, int32_t state = 0, bool zbuffer = true);
        // Saves a bitmap of the current object given the current state

    virtual PTObjectAnimator NewObjectAnimator(PTObjectInstance oi);
        // Creates an animtor for the given object
    virtual bool NeedsAnimator(PTObjectInstance oi);
      // Returns whether or not an animator is necessary
};

class TAnimAnimator : public TObjectAnimator
{
  public:
    TAnimAnimator(PTObjectInstance oi);
      // Constructor. Sets objectimagery. and object instance to nullptr.
    virtual void Animate(bool draw);
      // Animates object
    virtual ~TAnimAnimator();
      // Destructor.
};

DEFINE_IMAGERYBUILDER(OBJIMAGE_ANIMATION, TAnimImagery);


