// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *               effectcomp.h - Effect component module                  *
// *************************************************************************

#pragma once

#include "revenant.h"

#include "3dimage.h"
#include "object.h"
#include "sound.h"

// *********************************************************
// * Storm Animator - Use for Meteor, Ice, Fire, Whatever! *
// *********************************************************

#define STORM_DEFAULT_MAX_INSTANCE      20

_STRUCTDEF(SStormParams)
struct SStormParams
{
    int32_t particles;          // change the number of particles allowed
    int32_t tex_u;              // the texture u size, for refreshing
    int32_t tex_v;              // the texture v size, for refreshing

    int32_t particle_u;         // the particle frame's u size
    int32_t particle_v;         // the particle frame's v size
    int32_t particle_begin;     // the particle's beginning frame
    int32_t particle_end;       // the particle's ending frame

    int32_t impact_u;           // the impact frame's u size
    int32_t impact_v;           // the impact frame's v size
    int32_t impact_begin;       // the impact's beginning frame
    int32_t impact_end;         // the impact's ending frame

    float gravity;          // the force due to weight
    hmm_vec3 velocity;     // vectored velocity

    hmm_vec3 pos;          // the creation position base
    hmm_vec3 pos_spread;   // spread them around, random value

    float impact_frame_inc; // how much to increment the impact frame
    float particle_frame_inc;// how much to increment the particle frame

    hmm_vec3 particle_scale;// how big to scale it
    hmm_vec3 impact_scale; // how big to scale it
    hmm_vec3 rot;          // value for rotation
};


#define METEOR_DAMAGE_MAX 15
#define METEOR_DAMAGE_MIN 5

_STRUCTDEF(SStormInstance)
struct SStormInstance
{
    bool used;              // is this thing in use
    hmm_vec3 pos;          // the position of the instance
    bool is_particle;       // is this thing a particle or an impact
    bool explosion_sounded;  // has the explosion sound been played
    hmm_vec3 velocity;     // the velocity
    hmm_vec3 part_scl;     // scaling for individual meteor particles
    hmm_vec3 expl_scl;     // scaling for individual meteor explosions
    float gravity;          // gravity
    float frame;            // the frame count
    float impact_frame_inc; // how much to increment the impact frame
    float particle_frame_inc;// how much to increment the particle frame
};

_CLASSDEF(TStormAnimator)
class TStormAnimator
{
  protected:
    T3DAnimator* animator;          // pointer to the animator
    S3DAnimObj* obj;                // pointer to a 3D object
    SStormParams params;            // the storm parameters
    SStormInstance* storm_instance; // the instance stuff
    int32_t size;                       // the size of the instance array

  protected:
    virtual void Create();          // create new particles if necessary
    virtual int32_t GetCount();         // create a counter
  public:
    // my constructor
    TStormAnimator(const int32_t& instance_size = STORM_DEFAULT_MAX_INSTANCE)
    { storm_instance = new SStormInstance[instance_size]; size = instance_size; }
    // my destructor
    virtual ~TStormAnimator()
    { delete [] storm_instance; }

    virtual void Init(T3DAnimator* anim, S3DAnimObj* o);    // init, must be called
    virtual void Set(SStormParams* nparams);                // set the parameters
    virtual void Get(SStormParams* nparams);                // get the parameters
    virtual void Animate();                                 // animate the storm effect
    virtual void Render();                                  // render the storm effect
    virtual void RefreshZBuffer();                          // refresh the storm effect
    virtual bool IsDone()                                   // check to see if effect is done
    { if (!GetCount()) return true; else return false; }
};

// **********************************
// * The SubParticle Animator Class *
// **********************************

#define SUBPARTICLE_MAX_PARTICLE    20

_STRUCTDEF(SSubParticleParams)
struct SSubParticleParams
{
    int32_t particles;          // the amount of particles on the screen
    int32_t chance;             // the chance of a new particle being created % wise

    hmm_vec3 pos;          // the base pos value
    hmm_vec3 pos_spread;   // the random portion of a generated pos

    hmm_vec3 scale;        // the base scale value
    hmm_vec3 scale_dec;    // how much scale is decremented each time
    hmm_vec3 scale_spread; // the random portion of a generated scale value
    // the initial scale value = (scale) + (random(0, scale_spread))

    hmm_vec3 velocity;     // the base velocity value
    hmm_vec3 velocity_dir; // -val for only - signs, 0 for both signs, +val for only + signs
    hmm_vec3 velocity_spread;// the random portion of a generated velocity value
    // the initial velocity = (velocity) + (random(0, velocity_spread))

    float gravity;          // how the particles are affected by gravity
    
    int32_t min_life;           // minimum life span in frames
    int32_t max_life;           // maximum life span in frames
    // life = random(min_life, max_life)

    bool flicker;           // can it flicker
    float flicker_size;     // how much does it flicker
};

_STRUCTDEF(SParticle)
struct SParticle
{
    bool used;              // is this particle used
    hmm_vec3 pos;          // position of particle
    hmm_vec3 velocity;     // velocity of particle
    hmm_vec3 scale;        // scale of particle
    hmm_vec3 scale_dec;    // the decrementor!
    float gravity;          // how gravity affects the particle
    int32_t life;               // the lifespan in frames of the particle
    bool flicker;           // can it flicker
    bool flicker_status;    // is it flickering?
    float flicker_size;     // how much does it flicker 
};

_CLASSDEF(TSubParticleAnimator)

class TSubParticleAnimator
{
  private:
    SSubParticleParams params;  // the parameters to use when creating a new particle
    SParticle *particle;        // the list of particles
    int32_t max_particles;          // the total number of particles
    T3DAnimator* animator;      // animator
    S3DAnimObj* obj;            // 3d object
  protected:
  public:
    // constructor
    TSubParticleAnimator(int32_t max_subparticles = SUBPARTICLE_MAX_PARTICLE)
        {  max_particles = max_subparticles; particle = new SParticle[max_particles]; }
    // destructor
    virtual ~TSubParticleAnimator() { delete [] particle;}

    // set the parameters
    virtual void Set(SSubParticleParams* nparams);
    virtual void Get(SSubParticleParams* nparams);

    // animate the existing particles
    virtual void Animate();

    // render the particles
    virtual void Render();
    // refresh their zbuffers
    virtual void RefreshZBuffer(int32_t tex_u, int32_t tex_v);

    // find the total number of active particles
    int32_t GetCount();

    // create a particle
    void Create();

    // init the particle parameters
    virtual void Init(T3DAnimator* anim, S3DAnimObj* o);
};

// ************************
// * A Shockwave Animator *
// ************************

#define SHOCKWAVE_GROW      1
#define SHOCKWAVE_SHRINK    2

#define SHOCKWAVE_FLAG_SHRINK       (1 << 0)
#define SHOCKWAVE_FLAG_FADE         (1 << 1)
#define SHOCKWAVE_FLAG_START_SHRINK (1 << 2)


_STRUCTDEF(SShockParam)
struct SShockParam
{
    int32_t flags;              // the flags for the shockwave
    hmm_vec3 pos;          // the center of the shockwave
    hmm_vec3 rot;          // how is it rotated
    hmm_vec3 scale;        // what is the scaling
    hmm_vec3 scale_factor; // how much is it scaled each time
    hmm_vec3 shrink_factor;// how fast will it shrink
    hmm_vec3 max_size;     // how big can it get
    hmm_vec3 min_size;     // how small can it get
};

_CLASSDEF(TShockAnimator)

class TShockAnimator
{
  private:
    T3DAnimator* animator;          // 3d animator
    S3DAnimObj* obj;                // the object 
    SShockParam params;             // describe the shockwave
    int32_t ring_count;                 // the number of rings
    int32_t vertex_count;               // the number of vertices
    D3DCOLOR *ring;                 // the rings
    hmm_vec3 init_scale;           // the original size of the scale
    bool done;                      // is it done animating
    int32_t grow;                       // is it growing
  public:
    TShockAnimator()                // constructor
    { done = true; }
    virtual ~TShockAnimator()       // destructor
    { delete [] ring; }

    virtual void Set(SShockParam* nparams); // set the parameters
    virtual void Init(T3DAnimator* anim, S3DAnimObj* o, int32_t rings, int32_t vertices);// init the animator
    virtual void Animate();                 // animate the shockwave
    virtual void RefreshZBuffer(int32_t tex_u, int32_t tex_v);// refresh the zbuffer
    virtual void Render();                  // render the shockwave
    bool IsDone()                           // check to see if the shockwave is done
    { return done; }
    void SetRingColor(int32_t ring_num, D3DCOLOR color)
    { ring[ring_num] = color; }
};

_CLASSDEF(TStripAnimator)
class TStripAnimator
{
    public:
    S3DAnimObj* obj;
    int32_t pbeg, pend;     // The beginning and end points
    int32_t startsize;      // How thick is the strip at the origin
    int32_t endsize;        // How thick is the strip at the end
    int32_t maxpoints;      // Maximum number of points we can have
    int32_t curpoints;      // Current number of points we have
    hmm_vec3 *points;  // The points themselves
    float ufrequency;    // Texture U Incement added for each point
    float uoff;          // Texture U Offset for strip
    float ubeg;          // Texture U beginning range
    float uend;          // Texture U ending range

    float myx[8];
    float myz[8];

    TStripAnimator( S3DAnimObj* myobj, int32_t maxp);
        // Build strip with maximum number of points

    S3DAnimObj* GetStripObj() { return obj; };
        // Returns pointer to obj.  Why?  I'm not sure

    void GenerateStrip( int32_t angle);
        // takes current point list and sets verts and faces in obj

    void AddPoint( hmm_vec3 *p);
        // Adds point to head of point array, moves array 1, and

    void DelEndPoint();
        // Deletes point from the end of the point array

    void DelStartPoint();
        // Deletes point from the start of the point array

    void AdvanceStrip();
        // Shifts the points once away from the origin

    void ReverseStrip();
        // Shifts the points once toward the origin

    void SetTextureRange( float startu, float endu);
        // Which texture map to use

    void ScrollTexture( float deltau);
        // Scroll Texture base by deltau

    void SetWidth( int32_t start, int32_t end);
        // Which texture map to use

    void AddUpdateRects( SRenderRect* drawextents);
        // Called after outside function calls RenderObject()

    hmm_vec3& operator [] (int32_t i)
        { return points[ (i + pbeg) >= maxpoints ? (i + pbeg - maxpoints) : (i + pbeg)]; }
        // Fancy way of saying points[i]
};

_STRUCTDEF(SParticleSystemInfo)
struct SParticleSystemInfo
{
    hmm_vec3 pos;
    hmm_vec3 scl;
    hmm_vec3 rot;
    hmm_vec3 acc;
    hmm_vec3 vel;

    hmm_vec3 temp;

    bool flicker;

    int32_t life;
    int32_t life_span;

    bool used;
};

_CLASSDEF(TParticleSystem)

class TParticleSystem
{
  private:
    SParticleSystemInfo* particle;      // information about the particles
    T3DAnimator* animator;              // pointer to the animator used
    S3DAnimObj* object;                 // pointer to the object
    S3DPoint size;                      // the size for zbuffer refresh
    int32_t count;                          // the number of particles in the system
    float facing;                       // the facing of the thing
    bool check_move;                    // is this thing supposed to move
  public:
    // constructor
    TParticleSystem(int32_t maxsize)    { particle = new SParticleSystemInfo[maxsize]; for(int32_t i = 0; i < maxsize; ++i) particle[i].used = false; count = maxsize; }
    // destructor
    virtual ~TParticleSystem()      { delete [] particle; }

    // our basic functions
    virtual void Init(T3DAnimator* a, S3DAnimObj* o, S3DPoint s, bool move = false, float facing_angle = 0.0f);
    virtual void Animate();
    virtual void Render(bool flicker = false, bool abs_pos = false);
    virtual void RefreshZBuffer();
    virtual void Add(SParticleSystemInfo* p);

    // an accessor or two for good measure
    SParticleSystemInfo* Get(int32_t i) { if (i >= count || i < 0) return nullptr; else return &particle[i]; }
};

_STRUCTDEF(SBloodParticle)
struct SBloodParticle
{
    hmm_vec3 vel, pos;
    float scl;
    bool used;
    uint8_t size, stage, count, delay;
};

_STRUCTDEF(SBloodSystemParams)
struct SBloodSystemParams
{
    T3DAnimator* a;
    S3DAnimObj* s, m, b, sp, s2, m2, b2, sp2;
    int32_t maxsize, num, height, hangle, vangle, hspread, vspread;
    S3DPoint zbuf, effectpos;
};

#define MAX_BLOODS      30

_CLASSDEF(TBloodSystem)
class TBloodSystem
{
  private:
    SBloodParticle* blood;              // blood particles
    T3DAnimator* animator;              // animator pointer
    S3DAnimObj* sml, med, big, spl, sml2, med2, big2, spl2;// objects (small, med, or large--would you like fries?)
    S3DPoint size, eff;
    bool done;
    float height, bifscale;//facing, height;
    int32_t bifon;

  public:
    TBloodSystem() { blood = new SBloodParticle[MAX_BLOODS]; for (int32_t i = 0; i < MAX_BLOODS; i++) blood[i].used = 0; };
    virtual ~TBloodSystem()     { delete [] blood; }

    // our basic functions
                                // size = 0, 1, or 2
    virtual void Init(SBloodSystemParams me);
    virtual void Animate();
    virtual void Render();
    virtual void RefreshZBuffer();

    virtual void DoLighting(float x, float y, float z, S3DAnimObj* object);

    virtual bool GetDone() { return done; }
};
