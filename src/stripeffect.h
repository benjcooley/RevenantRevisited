// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 TStripEffect.h - TStripEffect module                  *
// *************************************************************************

#pragma once

#include "revenant.h"

#include "object.h"
#include "effect.h"
#include "effectcomp.h"
#include "3dimage.h"
#include "mappane.h"
#include "missileeffect.h"

/*
	//* I'm not sure how to do this since the points exist in the strip, but the rectangle
	//* needs to be in the animator, and I don't know how to get or translate the visible
	//* rectangle for any point and add it individually to the Extents list

	// WorldToScreen( RS3DPoint pos, int32_t &x, int32_t &y);
	// WorldToScreen( RS3DPoint pos, RS3DPoint spos);

	AddUpdateRects(SRect drawextents); // Called after outside function calls RenderObject()
	    // For obj returned by GetStrip()..
		// Adds an update rect size of segboundrect for each point drawn
		// Should check size of extents, and size of each bound rect added together, and
		// add smaller amount.

	SetBoundingRect(&SRect r);  // Any rects based on segboundrect completely outside this
	  // rect won't be added to in the Generate function
*/

// ******************
// * TStripEffect   *
// ******************

enum {
	STRIP_LAUNCH = 0,
	STRIP_FLY,
	STRIP_EXPLODE
};

#define STRIP_SPEED	16
#define STRIP_RANGE	2

_CLASSDEF(TStripEffect)

class TStripEffect : public TEffect
{
	public:
	int32_t duration;
	int32_t	targetrange;
	int32_t my_state;
	PTCharacter chr;

	TStripEffect(PTObjectImagery newim) : TEffect(newim) { Initialize(); }
	TStripEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { Initialize(); }

	virtual void Initialize();

	virtual void Pulse();

	virtual void OffScreen();	// StripEffects stay on-screen until the effect is finished
};

// **********************
// * Lightning Animator *
// **********************

// Consists of a lightning strip, a glow above the caster,
// and a shower of sparks

#define SMOOTH_SIZE		4
#define ADD_FACTOR		2
#define TARGET_POS		1	// make this 0 to test lightning effect, 1 to actually use it
#define LIGHTNING_SCALE_N	2
#define LIGHTNING_SCALE_D	1
#define MAX_MAXPOINTS		100

_CLASSDEF(TLightningAnimator)
class TLightningAnimator : public T3DAnimator
{
protected:
	TSubParticleAnimator spark;	// da sparks!
	TSubParticleAnimator impact_spark;	// da sparks!
	float glow_scale;
	int32_t total_offset;
	hmm_vec3 end_p;
	bool damage_flag;
	bool firsttime;
public:
	PTStripAnimator	mystrip;
	int32_t	numpoints;		// number of points on strip
	int32_t	maxpoints;		// maxpoints
	//hmm_vec3 next_strips[SMOOTH_SIZE];	// the strips to add

	int32_t showangle;		// Angle of strip
	int32_t rotdegree;		// Rotation of first disc
	int32_t morrotdegree;	// Rotation of last disc
	bool maxsize;		// Have all the points been added?  (Used when deleting points)
	bool down;

    TLightningAnimator(PTObjectInstance oi) : T3DAnimator(oi) { mystrip = nullptr; numpoints = 0; }
	  // Constructor (initialization handled by Initialize)
    virtual ~TLightningAnimator() { Close(); }
	  // Call close function

    virtual void SetupObjects();
	  // Called to create S3DAnimObj's and add to object array
	virtual void Initialize();
      // Initializes the materials
	virtual void Animate(bool draw);
	  // Called to upsdate frame state
	virtual bool Render();
	  // Called to render a frame
	virtual void RefreshZBuffer();
	  // cause the zbuffer to refresh itself
};

// ********************
// * TWindStripEffect *
// ********************

_CLASSDEF(TWindStripEffect)

class TWindStripEffect : public TEffect
{
  private:
	PTCharacter targets[10];
	int32_t end_of_targets;
	int32_t count;
  public:
	TWindStripEffect(PTObjectImagery newim) : TEffect(newim) { Initialize();}
    TWindStripEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { Initialize();}
	virtual ~TWindStripEffect() {}

	virtual void Initialize();
	virtual void Pulse();
};

// **********************
// * WindStrip Animator *
// **********************

// Consists of a white/blue strip w/ a fadeout-alpha gradient

#define WINDSTRIP_MAX_STRIPS	20

_STRUCTDEF(SDropSparkSystem)
struct SDropSparkSystem
{
	hmm_vec3 pos, vel;
	float scl;
	
	bool used;
};

_CLASSDEF(TWindStripAnimator)
class TWindStripAnimator : public T3DAnimator
{
protected:
	int32_t total_offset;
	hmm_vec3 front, center;
	float r[WINDSTRIP_MAX_STRIPS], th[WINDSTRIP_MAX_STRIPS], h[WINDSTRIP_MAX_STRIPS],
		ac[WINDSTRIP_MAX_STRIPS], dr[WINDSTRIP_MAX_STRIPS], dth[WINDSTRIP_MAX_STRIPS],
		dh[WINDSTRIP_MAX_STRIPS], dac[WINDSTRIP_MAX_STRIPS], haloscale;
	int32_t frameon, centerang;
	PSDropSparkSystem drops;

public:
	PTStripAnimator	mystrip[WINDSTRIP_MAX_STRIPS];
	
	int32_t subspell;
	hmm_vec3 realpos;

	TWindStripAnimator(PTObjectInstance oi) : T3DAnimator(oi) { for (int32_t i = 0; i < WINDSTRIP_MAX_STRIPS; i++) mystrip[i] = nullptr; }
	  // Constructor (initialization handled by Initialize)
    virtual ~TWindStripAnimator() { Close(); }
	  // Call close function

    virtual void SetupObjects();
	  // Called to create S3DAnimObj's and add to object array
	virtual void Initialize();
      // Initializes the materials
	virtual void Animate(bool draw);
	  // Called to upsdate frame state
	virtual bool Render();
	  // Called to render a frame
	virtual void RefreshZBuffer();
	  // cause the zbuffer to refresh itself
	virtual void InitDrops();
	virtual void AddDrop(hmm_vec3 pos);
};

