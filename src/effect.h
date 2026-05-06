// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      effect.h - TEffect module                        *
// *************************************************************************

#pragma once

#include "revenant.h"

#include <string>
#include <utility>
#include <vector>

#include "3dimage.h"
#include "charanimator.h"
#include "effect.h"
#include "effectcomp.h"
#include "object.h"
#include "sound.h"
#include "spell.h"

bool SaveBlendState();
bool RestoreBlendState();
bool SetBlendState();
bool SetAddBlendState();
void DamageCharactersInRange(TObjectInstance* invoker, S3DPoint& pos, int32_t range, int32_t minamount, int32_t maxamount, int32_t type);
void BlastCharactersInRange(TObjectInstance* invoker, S3DPoint& pos, int32_t range, int32_t minamount, int32_t maxamount, int32_t type, int32_t interior_range = 0);
void PulpCharactersInRange(TObjectInstance* invoker, S3DPoint& pos, int32_t range);
void PulpGuy(PTCharacter ch, S3DPoint pos, S3DPoint blast);
void RestoreZ(int32_t x, int32_t y, int32_t width, int32_t height);

_CLASSDEF(TEffect)
_CLASSDEF(TSpellBlock)

#include "particlefx.h"

// ***********
// * TEffect *
// ***********

// The effect class - individual effects from spells
class TEffect : public TObjectInstance
{
  public:
    TEffect(TObjectImagery* newim) : TObjectInstance(newim) { flags |= OF_IMMOBILE | OF_PULSE; spell = nullptr; subspell = 0; angle = 0; }
    TEffect(SObjectDef* def, TObjectImagery* newim) : TObjectInstance(def, newim) { flags |= OF_IMMOBILE | OF_PULSE; spell = nullptr; subspell = 0; angle = 0; }

    virtual void Pulse();

    void SetSpell(PTSpell sp) { spell = sp; }    
    PTSpell GetSpell() { return spell; }

    void SetSubSpell(int32_t sp) { subspell = sp; }    
    int32_t GetSubSpell() { return subspell; }

    void KillThisEffect();  // Call when effect is finished

    int32_t GetAngle();         // Returns aiming angle

  protected:
    PTSpell spell;      // Spellblock which spawned this effect
    int32_t subspell;           // TEMP TEMP TEMP subspell (psuedo-talisman)
    int32_t angle;              // The direction we are cast
};

// Runtime visual component for effects rendered as animated billboard
// flipbooks. It is not serialized; effect builders attach/configure it from
// existing object/imagery definitions.
class TFlipbookBillboardComponent : public TObjectComponent
{
  public:
    [[nodiscard]] const char* ComponentName() const override { return "flipbook_billboard"; }

    void Configure(sg_image texture, int32_t texture_width, int32_t texture_height,
                   int32_t columns, int32_t rows, int32_t frames,
                   float width, float height,
                   bool additive, bool replaces_default)
    {
        image = texture;
        tex_w = texture_width > 0 ? texture_width : 1;
        tex_h = texture_height > 0 ? texture_height : 1;
        cols = columns > 0 ? columns : 1;
        this->rows = rows > 0 ? rows : 1;
        frame_count = frames > 0 ? frames : cols * this->rows;
        size_w = width;
        size_h = height;
        additive_blend = additive;
        replaces_default_visual = replaces_default;
    }
    void SetDebugSolid(bool enable) { debug_solid = enable; }
    bool SetFrameExpression(const char* expr, std::string* error = nullptr)
        { return frame_expr.Compile(expr, error); }
    bool SetUvRectExpression(const char* expr, std::string* error = nullptr)
        { return uv_rect_expr.Compile(expr, error); }

    void OnAttach() override { RegisterUpdate(&TObjectComponent::Update); }
    void OnDetach() override { UnregisterUpdate(&TObjectComponent::Update); }

    [[nodiscard]] bool ReplacesDefaultVisual() const { return replaces_default_visual; }
    [[nodiscard]] sg_image Image() const { return image; }
    [[nodiscard]] int32_t TextureWidth() const { return tex_w; }
    [[nodiscard]] int32_t TextureHeight() const { return tex_h; }
    [[nodiscard]] int32_t SourceX() const { float r[4]; UvRect(r); return int32_t(r[0] * float(tex_w)); }
    [[nodiscard]] int32_t SourceY() const { float r[4]; UvRect(r); return int32_t(r[1] * float(tex_h)); }
    [[nodiscard]] int32_t SourceWidth() const { float r[4]; UvRect(r); const int32_t w = int32_t(r[2] * float(tex_w)); return w > 0 ? w : 1; }
    [[nodiscard]] int32_t SourceHeight() const { float r[4]; UvRect(r); const int32_t h = int32_t(r[3] * float(tex_h)); return h > 0 ? h : 1; }
    [[nodiscard]] float Width() const { return size_w; }
    [[nodiscard]] float Height() const { return size_h; }
    [[nodiscard]] bool AdditiveBlend() const { return additive_blend; }
    [[nodiscard]] bool DebugSolid() const { return debug_solid; }

  protected:
    void OnUpdate() override
    {
        ++legacy_frame;
        if (legacy_frame >= 18)
            legacy_frame = 0;
    }

  private:
    [[nodiscard]] int32_t FrameCell() const
    {
        // Original TFlameAnimator timing: local 18-frame counter mapped into
        // an 8-cell 4x2 texture atlas.
        if (frame_expr.IsValid())
        {
            SParticleEvalContext ctx = {};
            ctx.time_frame = float(legacy_frame);
            const int32_t frame = int32_t(frame_expr.Eval(ctx));
            return frame_count > 0 ? ((frame % frame_count) + frame_count) % frame_count : 0;
        }
        return frame_count > 0 ? ((legacy_frame * 11 / 24) % frame_count) : 0;
    }

    void UvRect(float out[4]) const
    {
        if (uv_rect_expr.IsValid() && uv_rect_expr.ResultLanes() >= 4)
        {
            SParticleEvalContext ctx = {};
            ctx.time_frame = float(legacy_frame);
            uv_rect_expr.Eval(ctx, out, 4);
            return;
        }

        const int32_t frame = FrameCell();
        out[0] = float(frame % cols) / float(cols);
        out[1] = float(frame / cols) / float(rows);
        out[2] = 1.0f / float(cols);
        out[3] = 1.0f / float(rows);
    }

    sg_image image = {};
    int32_t tex_w = 1, tex_h = 1;
    int32_t cols = 1, rows = 1, frame_count = 1;
    int32_t legacy_frame = 0;
    TParticleExpression frame_expr;
    TParticleExpression uv_rect_expr;
    float size_w = 1.0f, size_h = 1.0f;
    bool additive_blend = true;
    bool replaces_default_visual = true;
    bool debug_solid = false;
};

struct SParticleBucketEffectDef
{
    std::string name;
    int32_t texture_slot = 0;
    int32_t atlas_cols = 1;
    int32_t atlas_rows = 1;
    int32_t atlas_frames = 1;
    float width = 1.0f;
    float height = 1.0f;
    float scale = 1.0f;
    bool additive = true;
    bool flip_v = false;
    bool chroma_key = false;
    float chroma_key_rgb[3] = {1.0f, 0.0f, 0.0f};
    std::string frame_expr;
    std::string uv_rect_expr;
};

struct SParticleEmitterOutputDef
{
    std::string bucket;
    float life = -1.0f;
};

struct SParticleEmitterEffectDef
{
    std::string name;
    float local_pos[3] = {};
    std::vector<SParticleEmitterOutputDef> outputs;
};

struct SParticleEffectDef
{
    std::string name;
    bool debug_solid = false;
    uint32_t inactive_ttl_pulses = 1;
    std::vector<SParticleBucketEffectDef> buckets;
    std::vector<SParticleEmitterEffectDef> emitters;
};

class TParticleEffectManager
{
  public:
    void PulseEffect(const SParticleEffectDef* effect_def, TObjectInstance* owner);
    void StopEffect(const SParticleEffectDef* effect_def, TObjectInstance* owner);
    void ExpireInactiveEffects();
    void Clear();

  private:
    struct SRuntime
    {
        const SParticleEffectDef* effect_def = nullptr;
        TSafeRef<> owner;
        float owner_particle_id = -1.0f;
        uint32_t last_draw_pulse_pass = 0;
        std::vector<TParticleBucket*> buckets;
    };

    SRuntime* FindRuntime(const SParticleEffectDef* effect_def, int32_t owner_map_index);
    void StartRuntime(SRuntime& runtime, TObjectInstance* owner);
    void StopRuntime(size_t runtime_index);

    std::vector<SRuntime> runtimes;
    uint32_t last_expire_pass = 0;
};

extern TParticleEffectManager* ParticleEffectManager;

class TParticleEffectComponent : public TObjectComponent
{
  public:
    [[nodiscard]] const char* ComponentName() const override { return "particle_effect"; }

    void Configure(const SParticleEffectDef* def) { effect_def = def; }

    void OnAttach() override
    {
        RegisterUpdate(&TObjectComponent::Update);
    }

    void OnDetach() override
    {
        if (ParticleEffectManager)
            ParticleEffectManager->StopEffect(effect_def, Owner());
        UnregisterUpdate(&TObjectComponent::Update);
    }

    void DrawPulse()
    {
        if (!ParticleEffectManager || !effect_def || !Owner())
            return;
        ParticleEffectManager->PulseEffect(effect_def, Owner());
    }

  protected:
    void OnUpdate() override
    {
        if (ParticleEffectManager)
            ParticleEffectManager->ExpireInactiveEffects();
    }

  private:
    const SParticleEffectDef* effect_def = nullptr;
};

// ***************
// * TFireEffect *
// ***************

_CLASSDEF(TFireEffect)

class TFireEffect : public TEffect
{
  public:
    TFireEffect(TObjectImagery* newim) : TEffect(newim) {}
    TFireEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}

    virtual void Pulse();
};

class TFlameEffect : public TEffect
{
  public:
    TFlameEffect(TObjectImagery* newim) : TEffect(newim) { InitializeVisualComponent(newim); }
    TFlameEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) { InitializeVisualComponent(newim); }

    static void AttachVisualComponent(TObjectInstance* inst, TObjectImagery* imagery);

  private:
    void InitializeVisualComponent(TObjectImagery* imagery);
};

// **************
// * TIceEffect *
// **************

#define ICEDIST     256
#define ICERATE     16

#define ICE_FLY     0
#define ICE_IMPACT  1

_CLASSDEF(TIceEffect)

class TIceEffect : public TEffect
{
  public:
    TIceEffect(TObjectImagery* newim) : TEffect(newim) { flags |= OF_MOVING; }
    TIceEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim)
        { pos.y += ICEDIST; pos.x -= ICEDIST; pos.z += ICEDIST; flags |= OF_MOVING; }

    virtual void Pulse();
};

#define LIGHTNING_SOUND     "lightning"

//*// ********************
//*// * TLightningEffect *
//*// ********************
//*
//*_CLASSDEF(TLightningEffect)
//*
//*class TLightningEffect : public TEffect
//*{
//*  public:
//* TLightningEffect(TObjectImagery* newim) : TEffect(newim)
//*     { flags |= OF_MOVING; firsttime = true; SoundPlayer.Mount(LIGHTNING_SOUND); }
//* TLightningEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim)
//*     { flags |= OF_MOVING; firsttime = true; SoundPlayer.Mount(LIGHTNING_SOUND); }
//* virtual ~TLightningEffect() { SoundPlayer.Unmount(LIGHTNING_SOUND); }
//*
//* virtual void Pulse();
//*
//*  protected:     
//* bool firsttime;         // flag for the first time through
//*};

// ***************
// * THealEffect *
// ***************

_CLASSDEF(THealEffect)

class THealEffect : public TEffect
{
  private:
    bool first_time;
  public:
    int32_t level;

    THealEffect(TObjectImagery* newim) : TEffect(newim) { Initialize(); }
    THealEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) { Initialize(); }

    virtual void Initialize();
    virtual void Pulse();
};

//*// *********************
//*// * TLightning2Effect *
//*// *********************
//*
//*_CLASSDEF(TLightning2Effect)
//*
//*class TLightning2Effect : public TEffect
//*{
//*  public:
//*    int32_t angle;
//*
//*  public:
//*    TLightning2Effect(TObjectImagery* newim) : TEffect(newim) { }
//*    TLightning2Effect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) { }
//*
//* virtual void Pulse();
//*};

// *****************
// * Test Animator *
// *****************

#define NUMPARTICLES 15

_CLASSDEF(TTest3DAnimator)

class TTest3DAnimator : public T3DAnimator
{
  private:
    hmm_vec3 v[NUMPARTICLES];
    hmm_vec3 p[NUMPARTICLES];
    S3DMat mat[NUMPARTICLES];

  public:
    TTest3DAnimator(TObjectInstance* oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TTest3DAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes velocity vectors and positions
//  virtual void SetupObjects();
      // Called to create S3DAnimObj's and add to object array
    virtual void Animate(bool draw);
      // Called to update frame state
    virtual bool Render();
      // Called to render a frame
};

// ******************
// * Flare Animator *
// ******************

#define NUMFLARES 15

_CLASSDEF(TFlareAnimator)
class TFlareAnimator : public T3DAnimator
{
  private:
    hmm_vec3 v[NUMFLARES];
    hmm_vec3 p[NUMFLARES];
    S3DMat mat[NUMFLARES];

  public:
    TFlareAnimator(TObjectInstance* oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TFlareAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes velocity vectors and positions
//  virtual void SetupObjects();
      // Called to create S3DAnimObj's and add to object array
    virtual void Animate(bool draw);
      // Called to update frame state
    virtual bool Render();
      // Called to render a frame
};

// ***************************
// * Pulsating Ball Animator *
// ***************************

#define BALL_MAX_SCALE      10.0f
#define BALL_SCALE_FACTOR    0.1f

_CLASSDEF(TBallAnimator)
class TBallAnimator : public T3DAnimator
{
  private:
    hmm_vec3 p;            // where is it at?
    float scale_factor;  // current scaling measure
    float scale;         // the scale used

  public:
    TBallAnimator(TObjectInstance* oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TBallAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes velocity vectors and positions
//  virtual void SetupObjects();
      // Called to create S3DAnimObj's and add to object array
    virtual void Animate(bool draw);
      // Called to update frame state
    virtual bool Render();
      // Called to render a frame
};


// *****************
// * Fire Animator *
// *****************

#define NUMFIRES 15

_CLASSDEF(TFireAnimator)
class TFireAnimator : public T3DAnimator
{
  private:
    hmm_vec3 p[NUMFIRES];
    int32_t f[NUMFIRES];
    S3DMat mat[NUMFIRES];

  public:
    TFireAnimator(TObjectInstance* oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TFireAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes velocity vectors and positions
//  virtual void SetupObjects();
      // Called to create S3DAnimObj's and add to object array
    virtual void Animate(bool draw);
      // Called to update frame state
    virtual bool Render();
      // Called to render a frame
};

// ****************
// * Ice Animator *
// ****************

#define NUM_ICE_CRYSTALS 10
#define ICE_SCALE_STEP .25

_CLASSDEF(TIceAnimator)
class TIceAnimator : public T3DAnimator
{
  private:
    hmm_vec3 p[NUM_ICE_CRYSTALS];
    float scale[NUM_ICE_CRYSTALS];    // Current scale value of each ice crystal
    int32_t framenum[NUM_ICE_CRYSTALS];      // Frame counters (used for delays & timing)

  public:
    TIceAnimator(TObjectInstance* oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TIceAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes velocity vectors and positions
    virtual void Animate(bool draw);
      // Called to update frame state
    virtual bool Render();
      // Called to render a frame
};

// *****************
// * Heal Animator *
// *****************

#define NUM_HEAL_BUBBLES  60
#define HEAL_SCALE_STEP   .15
#define HEALING_RADIUS    20
#define HEAL_DURATION     40

_CLASSDEF(THealAnimator)

class THealAnimator : public T3DAnimator
{
  private:
    hmm_vec3 p[NUM_HEAL_BUBBLES];       // Position of each healing bubble
    float scale[NUM_HEAL_BUBBLES];    // Current scale value of each healing bubble
    float rise[NUM_HEAL_BUBBLES];     // Rate at which each bubble rises
    int32_t framenum[NUM_HEAL_BUBBLES];      // Frame counters (used for delays & timing)
    int32_t activebubbles;                   // How many bubbles are currently floating up
    float rotation;
    int32_t heal_num;
    int32_t glow_num;

  public:
    THealAnimator(TObjectInstance* oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~THealAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes velocity vectors and positions
    virtual void Animate(bool draw);
      // Called to update frame state
    virtual bool Render();
      // Called to render a frame
    virtual void RefreshZBuffer();
      // make everything cool
};

//*// **********************
//*// * Lightning Animator *
//*// **********************
//*
//*#define LIGHTNING_DURATION      40
//*#define NUM_LIGHTNING_SEGMENTS  20
//*
//*_CLASSDEF(TLightningAnimator)
//*
//*class TLightningAnimator : public T3DAnimator
//*{
//*  private:
//*    hmm_vec3 p[NUM_LIGHTNING_SEGMENTS];   // Position of each lightning segment
//*    int32_t framenum;                          // Keeps track of time
//*
//*  public:
//*    TLightningAnimator(TObjectInstance* oi) : T3DAnimator(oi) {}
//*   // Constructor (initialization handled by Initialize)
//*    virtual ~TLightningAnimator() { Close(); }
//*   // Call close function
//*
//* virtual void Initialize();
//*   // Initializes velocity vectors and positions
//* virtual void Animate(bool draw);
//*   // Called to update frame state
//* virtual bool Render();
//*   // Called to render a frame
//*};

// *****************************
// * Fountain Sparkle Animator *
// *****************************

#define NUM_FOUNTAIN_BUBBLES  10
#define FOUNTAIN_SCALE_STEP   .15
#define FOUNTAIN_RADIUS       20

_CLASSDEF(TFountainAnimator)

class TFountainAnimator : public T3DAnimator
{
  private:
    hmm_vec3 p[NUM_FOUNTAIN_BUBBLES];       // Position of each fountain bubble
    float scale[NUM_FOUNTAIN_BUBBLES];    // Current scale value of each fountain bubble
    float rise[NUM_FOUNTAIN_BUBBLES];     // Rate at which each bubble rises
    int32_t framenum[NUM_FOUNTAIN_BUBBLES];      // Frame counters (used for delays & timing)

  protected:
    int32_t colorobj;                            // Which object to use for the desired texture color

  public:
    TFountainAnimator(TObjectInstance* oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TFountainAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes velocity vectors and positions
    virtual void Animate(bool draw);
      // Called to update frame state
    virtual bool Render();
      // Called to render a frame
    virtual void SetColorObject() = 0;
      // Pure virtual function used to set colorobj
    virtual void RefreshZBuffer();
      // Called to update Z buffer information
};

// **************************
// * Cyan Fountain Animator *
// **************************

_CLASSDEF(TCyanFountainAnimator)

class TCyanFountainAnimator : public TFountainAnimator
{
  public:
    TCyanFountainAnimator(TObjectInstance* oi) : TFountainAnimator(oi) {}
      // Constructor (initialization handled by Initialize)

    virtual void SetColorObject() { colorobj = 0; }
};

// *************************
// * Red Fountain Animator *
// *************************

_CLASSDEF(TRedFountainAnimator)

class TRedFountainAnimator : public TFountainAnimator
{
  public:
    TRedFountainAnimator(TObjectInstance* oi) : TFountainAnimator(oi) {}
      // Constructor (initialization handled by Initialize)

    virtual void SetColorObject() { colorobj = 1; }
};

// ***************************
// * Green Fountain Animator *
// ***************************

_CLASSDEF(TGreenFountainAnimator)

class TGreenFountainAnimator : public TFountainAnimator
{
  public:
    TGreenFountainAnimator(TObjectInstance* oi) : TFountainAnimator(oi) {}
      // Constructor (initialization handled by Initialize)

    virtual void SetColorObject() { colorobj = 2; }
};

// **************************
// * Blue Fountain Animator *
// **************************

_CLASSDEF(TBlueFountainAnimator)

class TBlueFountainAnimator : public TFountainAnimator
{
  public:
    TBlueFountainAnimator(TObjectInstance* oi) : TFountainAnimator(oi) {}
      // Constructor (initialization handled by Initialize)

    virtual void SetColorObject() { colorobj = 3; }
};

// *******************
// * Ribbon Animator *
// *******************

#define NUM_RIBBONS              3
#define RIBBON_DURATION          50
#define NUM_RIBBON_SPARKS        30
#define RIBBON_RADIUS            35
#define RIBBON_SPARK_DURATION    24
#define RIBBON_SPARK_SCALE_STEP  0.25

_CLASSDEF(TRibbonAnimator)

class TRibbonAnimator : public T3DAnimator
{
  private:
    hmm_vec3 p[NUM_RIBBON_SPARKS];       // Position of each ribbon spark
    hmm_vec3 v[NUM_RIBBON_SPARKS];       // Movement vector for each ribbon spark
    float scale[NUM_RIBBON_SPARKS];    // Current scale value of each ribbon spark
    int32_t framenum[NUM_RIBBON_SPARKS];      // Frame counters (used for delays & timing)
    hmm_vec3 ribpos;                     // Position of the ribbons
    float rotation[NUM_RIBBONS];       // Current rotation values for each ribbon
    int32_t ribbontimer;                      // Timer for the entire ribbon effect
    int32_t numtexframes;                     // How many frames the texture has
    float ribscale;                    // Scale value for the ribbons
    float centertilt;
    float centertilt_dx;

  public:
    TRibbonAnimator(TObjectInstance* oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TRibbonAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes velocity vectors and positions
    virtual void Animate(bool draw);
      // Called to update frame state
    virtual bool Render();
      // Called to render a frame
};

// *******************
// * Shield Animator *
// *******************

#define SHIELD_SCALE  2.0

_CLASSDEF(TShieldAnimator)

class TShieldAnimator : public T3DAnimator
{
  private:
    hmm_vec3 pos;     // Position of the sphere
    int32_t framenum;      // Frame counter

  public:
    TShieldAnimator(TObjectInstance* oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TShieldAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes velocity vectors and positions
    virtual void Animate(bool draw);
      // Called to update frame state
    virtual bool Render();
      // Called to render a frame
};

// ******************
// * Flame Animator *
// ******************

_CLASSDEF(TFlameAnimator)

class TFlameAnimator : public T3DAnimator
{
  private:
    int32_t frame;      // Frame counter
  
  public:
    TFlameAnimator(TObjectInstance* oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TFlameAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes velocity vectors and positions
    virtual void Animate(bool draw);
      // Called to update frame state
    virtual bool Render();
      // Called to render a frame
    virtual void RefreshZBuffer();
};

// ********************
// * SymGlow Animator *
// ********************

_CLASSDEF(TSymGlowAnimator)

class TSymGlowAnimator : public T3DAnimator
{
  private:
    float u;      // Current u offset in the texture
    float zscale;
    float dz;
    int32_t timer;

  public:
    TSymGlowAnimator(TObjectInstance* oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TSymGlowAnimator() { Close(); }
      // Call close function

    virtual void SetupObjects();
      // Called to create S3DAnimObj's and add to object array
    virtual void Animate(bool draw);
    virtual bool Render();
      // Called to render a frame
    virtual void RefreshZBuffer();
};

// *********************
// * Particle Animator *
// *********************

_STRUCTDEF(SParticleParams)
struct SParticleParams
{
    int32_t particles;                  // duh..
    hmm_vec3 pos, pspread, dir, spread; // direction and spread of particles
    float gravity;               // gravity value added each tick to vector
    int32_t trails;                     // number of trailing particles
    int32_t minstart, maxstart;         // minimum start, maximum start delay for particle
    int32_t minlife, maxlife;           // number of trail items, minlife, maxlife
    bool bounce;                    // bounce
    bool killobj;                   // true if this should kill the object it's made for when done
    uint32_t objflags;                 // 1 bit set for each object that should randomly be included

    bool seektargets;               // seektargets?
    bool seekz;                     // seek on the z value (or false to not change z value)
    int32_t numtargets;                 // number of targets to seek (up to 5)
    hmm_vec3 targetpos[5];         // positions of targets to seek (if numtargets > 0)
    float turnang;               // the turn angle per frame (in radians) for seeking targets
    float seekspeed;                 // speed (vel mag) of seeking particle
    float autorange;             // "radius" where turning stops, just interpolating
    float hitrange;              // "radius" where particle has "hit" target, stop
};

_CLASSDEF(TParticle3DAnimator)
class TParticle3DAnimator : public T3DAnimator
{
  private:
    SParticleParams params;         // Particle parameters
    hmm_vec3 *v;                   // current vectors
    hmm_vec3 *p;                   // current position
    int32_t *l, *s, *o;                 // Current ticks left for life and start, and cur object
    int32_t *ti;                        // ti = target index (0 to numtargets - 1)

  public:
    TParticle3DAnimator(TObjectInstance* oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TParticle3DAnimator() { Close(); }
      // Call close function

    virtual void InitParticles(PSParticleParams nparams);
      // Sets up particle animator
    virtual void ResetTargetInfo(SParticleParams nparams);
      // Change particle seekpeed or targets, etc.

    virtual void Initialize();
      // Initializes velocity vectors and positions
    virtual void Close();
      // Deletes all objects, etc.
//  virtual void SetupObjects();
      // Called to create S3DAnimObj's and add to object array
    virtual void Animate(bool draw);
      // Called to update frame state
    virtual bool Render();
      // Called to render a frame
    virtual void RefreshZBuffer();

};

// *****************************
// * SmokeEffect Animator     *
// *****************************
#define NUMSMOKEBALLS 50
#define SMOKE_GRAV -0.01f

_STRUCTDEF(SSmoke);
struct SSmoke
{
    float x,y,z,size, vx, vy, vz,vsize;
    float rot; 
    int32_t life;
};


// ****************
// * Meteor Storm *
// ****************

_CLASSDEF(TMeteorStormEffect)
class TMeteorStormEffect : public TEffect
{
  public:
    TMeteorStormEffect(TObjectImagery* newim) : TEffect(newim) { Initialize(); }
    TMeteorStormEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) { Initialize(); }
    virtual ~TMeteorStormEffect() {}

    virtual void Initialize();          // init

    virtual void Pulse();               // da heartbeat of america...
};

#define METEOR_STORM_SIZE       20
#define METEOR_STORM_TICKS      100

_CLASSDEF(TMeteorStormAnimator)
class TMeteorStormAnimator : public T3DAnimator
{
  private:
    TStormAnimator meteor_storm;        // the meteor storm
    int32_t ticks;                          // tick, tock --, tick, tock --> see also Mouse, and Clock
    int32_t tracker;                        // counts meteors in flight
  public:
    // construction...
    TMeteorStormAnimator(TObjectInstance* oi) : T3DAnimator(oi) {}
    // destruction!
    virtual ~TMeteorStormAnimator() { Close(); }

    virtual void Initialize();          // init stuff
    virtual void Animate(bool draw);    // do the animation
    virtual bool Render();              // render it to the screen
    virtual void RefreshZBuffer();      // refresh the z buffer
};

// VORTEX is a teleport spell of sorts
/*
_CLASSDEF(TVortexEffect)
class TVortexEffect : public TEffect
{
  public:
    TVortexEffect(TObjectImagery* newim) : TEffect(newim) { Initialize(); }
    TVortexEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) { Initialize(); }
    virtual ~TVortexEffect() {}

    virtual void Initialize();          // init

    virtual void Pulse();               // da heartbeat of america...
};

#define VORTEX_LIFE_SPAN        30

#define VORTEX_STATE_GROW       1
#define VORTEX_STATE_SHRINK     2
#define VORTEX_STATE_CONSTANT   3

#define VORTEX_ALPHA_FACTOR     .03f
#define VORTEX_ALPHA_MAX        1.0f

#define VORTEX_RING             9
#define VORTEX_COUNT            15
#define VORTEX_GLOW_SIDE        16
#define VORTEX_BASE             30
#define VORTEX_VERTEX           (VORTEX_RING * VORTEX_COUNT + VORTEX_BASE)
#define VORTEX_GLOW_COUNT       (VORTEX_RING * 4)

#define VORTEX_SCALE_X_MAX      .6f
#define VORTEX_SCALE_Y_MAX      .6f
#define VORTEX_SCALE_Z_MAX      1.0f
#define VORTEX_ROT_MAX          12.0f

#define VORTEX_PARTICLE_COUNT   150
#define VORTEX_RADIUS           37
#define VORTEX_HEIGHT           150
#define VORTEX_SCALE_POINT      25 

struct vortex_particle_info
{
    float rot;
    float rot_inc;
    float height_inc;
    float face_rot;
    float face_rot_inc;
    float scale;
    float radius;
    bool flicker;
};

_CLASSDEF(TVortexAnimator) 
class TVortexAnimator : public T3DAnimator 
{
  private:
    hmm_vec3 pos;          // position of vortex
    hmm_vec3 scale;        // scaling of vortex
    hmm_vec3 rot;          // rotation of vortex
    float alpha_blend;   // blending of alpha
    hmm_vec3 rotation;     // rotation the thing
    int32_t count;              // the counter
    int32_t vortex_state;       // the state of the vortex
    hmm_vec3 scale_factor; // the amount to scale
    float rot_factor;    // the rotation factor
    float rot_speed;     // the rotation speed
    float vertex[VORTEX_VERTEX];// vertex alpha value
    float glow[VORTEX_GLOW_COUNT];// vertex alpha value
    int32_t frame;              // the frame of the turn
    TParticleSystem particle_system;// the particle system
    vortex_particle_info particle_info[VORTEX_PARTICLE_COUNT];// particle stuff
    int32_t phase;
    float fade;
    int32_t level;
  public:
    // constructor
    TVortexAnimator(TObjectInstance* oi) : T3DAnimator(oi), particle_system(VORTEX_PARTICLE_COUNT) {}
    //TVortexAnimator(TObjectInstance* oi);
    // destructor
    virtual ~TVortexAnimator() { Close(); }

    virtual void Initialize();
    virtual void Animate(bool draw);
    virtual bool Render();
    virtual void RefreshZBuffer();
};
*/
// ******************
// * Handling Flies *
// ******************

_CLASSDEF(TFlyEffect)

class TFlyEffect : public TEffect
{
  public:
    TFlyEffect(TObjectImagery* newim) : TEffect(newim) { Initialize(); }
    TFlyEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) { Initialize(); }
    virtual ~TFlyEffect() {}

    virtual void Initialize();          // init

    virtual void Pulse();               // da heartbeat of america...
};

#define FLY_COUNT       20
#define FLY_RANGE_X     30
#define FLY_RANGE_Y     30
#define FLY_RANGE_Z     30

_CLASSDEF(TFlyAnimator)

class TFlyAnimator : public T3DAnimator
{
  protected:
    TParticleSystem flies;  // the flies    
  public:
    // constructor
    TFlyAnimator(TObjectInstance* oi) : T3DAnimator(oi), flies(FLY_COUNT) {}
    // destructor
    virtual ~TFlyAnimator() { Close(); }

    // init
    virtual void Initialize();
    // animate
    virtual void Animate(bool draw);
    // render
    virtual bool Render();
    // refresh the z buffer
    virtual void RefreshZBuffer();
};

_CLASSDEF(TFogAnimator)

struct _color_data
{
    float c, a;
};

#define FOG_VERTEX      36
#define FOG_VERTEX_X    6
#define FOG_VERTEX_Y    6

class TFogAnimator : public T3DAnimator
{
  private:
    _color_data color[FOG_VERTEX];  // the color of each vertex
    hmm_vec3 pos[FOG_VERTEX];      // the position of each vertex
    hmm_vec3 dpos[FOG_VERTEX];     // the change in position of each vertex
    hmm_vec3 velocity[FOG_VERTEX]; // the velocity of the fog
    float color_velocity[FOG_VERTEX];// the velocity of the color changing
    float alpha_velocity[FOG_VERTEX];// the velocity of the color changing
  public:
    TFogAnimator(TObjectInstance* oi) : T3DAnimator(oi) {}
    virtual ~TFogAnimator() { Close(); }

    virtual void Initialize();
    virtual void Animate(bool draw);
    virtual bool Render();
    virtual void RefreshZBuffer();
};

// *** PULP GUYS ***
// like destroy them by blowing up their parts

_STRUCTDEF(SBloodDrop)
struct SBloodDrop
{
    hmm_vec3 pos;
    hmm_vec3 scl;
    hmm_vec3 vel;
    bool used;
};

_STRUCTDEF(SBloodSplat)
struct SBloodSplat
{
    hmm_vec3 pos;
    hmm_vec3 scl;
    hmm_vec3 rot;
    int32_t frame;
    bool used;
};

_STRUCTDEF(SBodyPartPulp)
struct SBodyPartPulp
{
    hmm_vec3 vel;
    hmm_vec3 rot_vel;
    hmm_vec3 pos;
    hmm_vec3 rot;
    hmm_vec3 scl;
    bool done;
    int32_t obj_num;
    int32_t object;
};

#define MAX_BODY_PART       5
#define MAX_BLOOD_DROP      (MAX_BODY_PART)// * 30)
#define MAX_BLOOD_SPLAT     (MAX_BODY_PART)// * 3)
#define PULP_GRAVITY        .8f
#define PULP_SLOW_RATE      .45f
#define PULP_SIDE_SLOW      .65f
#define PULP_MISC_SLOW_RATE .75f
#define PULP_ANG_SLOW_RATE  .6f
#define PULP_CUTOFF         .1f

_CLASSDEF(TPulpEffect)
class TPulpEffect : public TEffect
{
  private:
    // pulping info
    PTCharacter character;
    PSBloodDrop blood;
    PSBloodSplat blood_splat;
    PSBodyPartPulp body_part;

    // size info
    int32_t blood_count;
    int32_t blood_splat_count;
    int32_t body_part_count;

    bool valid;                     // has this be inited
    int32_t index;                      // a unique id for the character
  public:
    // functions
    TPulpEffect(TObjectImagery* newim) : TEffect(newim) { Initialize(); }
    TPulpEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) { Initialize(); }
    virtual ~TPulpEffect() { if (blood_splat) delete [] blood_splat; if (body_part) delete [] body_part; if (blood) delete [] blood; }

    virtual void Initialize();          // init

    virtual void Pulse();               // da heartbeat of america...

    // specific functions
    PTCharacter GetCharacter()          { return character; }
        // get a pointer to the character
    PSBloodDrop GetBlood(int32_t i)         { if (i < 0 || i >= blood_count) return nullptr; else return &blood[i]; }
        // get a pointer to the blood
    PSBloodSplat GetBloodSplat(int32_t i)   { if (i < 0 || i >= blood_splat_count) return nullptr; else return &blood_splat[i]; }
        // get a pointer to the blood splat
    PSBodyPartPulp GetBodyPart(int32_t i)   { if (i < 0 || i >= body_part_count) return nullptr; else return &body_part[i]; }
        // get a pointer to the body parts
    bool IsDone()                       { if (!valid) return true; else return false; }
        // get the status
    void Set(S3DPoint vel, PTCharacter ch, int32_t num_body_part = MAX_BODY_PART, int32_t num_blood = MAX_BLOOD_DROP, int32_t num_splat = MAX_BLOOD_SPLAT);

    virtual void Notify(int32_t notify, void *ptr);
        // notify that our character has been deleted
};

_CLASSDEF(TPulpAnimator)
class TPulpAnimator : public T3DAnimator
{
  public:
    TPulpAnimator(TObjectInstance* oi) : T3DAnimator(oi) {}
    virtual ~TPulpAnimator() { Close(); }

        // virtual functions
    virtual void Initialize();
    virtual void Animate(bool draw);
    virtual bool Render();
    virtual void RefreshZBuffer();
};

// *********************
// * TBurnEffect *
// *********************

_CLASSDEF(TBurnEffect)

class TBurnEffect : public TEffect
{
private:    
    PTCharacter character;
    int32_t frame;
public:
    TBurnEffect(TObjectImagery* newim) : TEffect(newim) { Initialize(); }
    TBurnEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) { Initialize(); }

    virtual void Initialize();
    virtual void Pulse();
    PTCharacter GetCharacter()  { return character; }
    void Set(PTCharacter ch);
    virtual void Notify(int32_t notify, void *ptr);
    void ResetFrameCount();
};

#define BURN_COUNT      70

// ***********************
// * Burn Animator *
// ***********************

_CLASSDEF(TBurnAnimator)
class TBurnAnimator : public T3DAnimator
{
  private:
    TParticleSystem fire;
    TParticleSystem smoke;
    int32_t size;
    int32_t frame;
    int32_t to_add;
  public:
    TBurnAnimator(TObjectInstance* oi) : T3DAnimator(oi), fire(BURN_COUNT), smoke(BURN_COUNT) {}
    virtual ~TBurnAnimator() { Close(); }

    virtual void Initialize();
    virtual void Animate(bool draw);
    virtual bool Render();
    virtual void RefreshZBuffer();
    virtual void ResetFrameCount() { frame = 0; }
};

// *********************
// * TAuraEffect *
// *********************

_CLASSDEF(TAuraEffect)

class TAuraEffect : public TEffect
{
private:    
    PTCharacter character;
    bool first_time;
public:
    TAuraEffect(TObjectImagery* newim) : TEffect(newim) { Initialize(); }
    TAuraEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) { Initialize(); }

    virtual void Initialize();
    virtual void Pulse();
    PTCharacter GetCharacter()  { if (spell) return (PTCharacter)spell->GetInvoker(); else return nullptr;}
};

#define AURA_COUNT      100

// ***********************
// * Aura Animator *
// ***********************

_CLASSDEF(TAuraAnimator)
class TAuraAnimator : public T3DAnimator
{
  private:
    TParticleSystem fire;
    int32_t size;
    int32_t frame;
    int32_t to_add;
  public:
    TAuraAnimator(TObjectInstance* oi) : T3DAnimator(oi), fire(AURA_COUNT) {}
    virtual ~TAuraAnimator() { Close(); }

    virtual void Initialize();
    virtual void Animate(bool draw);
    virtual bool Render();
    virtual void RefreshZBuffer();
};

// ***********************
// * TIceBoltEffect *
// ***********************

_CLASSDEF(TIceBoltEffect)

class TIceBoltEffect : public TEffect
{
  private:
  public:
    TIceBoltEffect(TObjectImagery* newim) : TEffect(newim) { }
    TIceBoltEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) { }
    virtual ~TIceBoltEffect() {}

    virtual void Initialize();
    virtual void Pulse();
};

// **************************
// * Electric Bolt Animator *
// **************************

//#define NUM_EBOLT_SPARKS  25

_CLASSDEF(TIceBoltAnimator)

/*class TIceBoltAnimator : public T3DAnimator
{
  private:
    int32_t angle;                          // moving at this angle
    hmm_vec3 p[NUM_EBOLT_SPARKS];      // Position of each spark
    hmm_vec3 v[NUM_EBOLT_SPARKS];      // Direction of each spark
    int32_t frame[NUM_EBOLT_SPARKS];        // delay before spark appears
    float size[NUM_EBOLT_SPARKS];       // size of spark
    int32_t initexplode;                    // start exploding!
    int32_t frameon;                        // frame on... (for timing)
    TShockAnimator ring;                // da ring!

  public:
    TIceBoltAnimator(TObjectInstance* oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TIceBoltAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes velocity vectors and positions
    virtual void Animate(bool draw);
      // Called to update frame state
    virtual bool Render();
      // Called to render a frame
    virtual void RefreshZBuffer();
      // make everything cool
    void AddSparks(S3DPoint pos, int32_t angle);
};*/

#define MAX_FROST_PARTICLES     50
#define MAX_SNOW_PARTICLES      50
class TIceBoltAnimator : public T3DAnimator
{
  private:
    int32_t frameon;                        // frame on... (for timing)
    int32_t angle;
    float length,
        spherescale[2],
        cylscale,
        ringout,
        spiralang,
        spiralscale,
        cy;//, sws, rx, ry, rz;
    hmm_vec3 p[MAX_FROST_PARTICLES], v[MAX_FROST_PARTICLES];
    float s[MAX_FROST_PARTICLES], t[MAX_FROST_PARTICLES];
    //PTParticle3DAnimator particleanim;

    float h[MAX_SNOW_PARTICLES], th[MAX_SNOW_PARTICLES], r[MAX_SNOW_PARTICLES],
        rs[MAX_SNOW_PARTICLES], sz[MAX_SNOW_PARTICLES];
    
  public:
    TIceBoltAnimator(TObjectInstance* oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TIceBoltAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes velocity vectors and positions
    virtual void Animate(bool draw);
      // Called to update frame state
    virtual bool Render();
      // Called to render a frame
    virtual void RefreshZBuffer();    
};

// ***************
// * TIcedEffect *
// ***************

_CLASSDEF(TIcedEffect)

class TIcedEffect : public TEffect
{
  private:
  public:
    TIcedEffect(TObjectImagery* newim) : TEffect(newim) { }
    TIcedEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) { }
    virtual ~TIcedEffect() {}

    virtual void Initialize();
    virtual void Pulse();
};

// *****************
// * Iced Animator *
// *****************

_CLASSDEF(TIcedAnimator)

#define MAX_ICED_CHUNKS     30
class TIcedAnimator : public T3DAnimator
{
  private:
    int32_t frameon;                        // frame on... (for timing)
    float angle;
    int32_t donebouncing;
    hmm_vec3 p[MAX_ICED_CHUNKS], v[MAX_ICED_CHUNKS], t[MAX_ICED_CHUNKS], w[MAX_ICED_CHUNKS];
    hmm_vec3 s[MAX_ICED_CHUNKS];
    int32_t l[MAX_ICED_CHUNKS];
    TObjectInstance* icedchar;
    
  public:
    TIcedAnimator(TObjectInstance* oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TIcedAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes
    virtual void Animate(bool draw);
      // Called to update frame state
    virtual bool Render();
      // Called to render a frame
    virtual void RefreshZBuffer();

    void InitIced(TObjectInstance* iceme);
      // Tell who to ice!
};

// ********************
// * TQuicksandEffect *
// ********************

_CLASSDEF(TQuicksandEffect)

class TQuicksandEffect : public TEffect
{
  private:
    bool first_time;
  public:
    TQuicksandEffect(TObjectImagery* newim) : TEffect(newim) { Initialize(); }
    TQuicksandEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) { Initialize(); }
    virtual ~TQuicksandEffect() {}

    virtual void Initialize();
    virtual void Pulse();
};

// **********************
// * Quicksand Animator *
// **********************

_CLASSDEF(TQuicksandAnimator)

class TQuicksandAnimator : public T3DAnimator
{
  private:
    float scalesize, cylscale, cylheight, cylrot, cylcount;
    int32_t stage, count;
    TObjectInstance* quicksandchar;
    float ang;
    float zscalefactor;
  protected:
    void SetAnimFrame(int32_t frame_num, PS3DAnimObj obj);
      // Called to change texture frame
    
  public:
    int32_t frameon;                        // frame on... (for timing)
    int32_t z_level;
    float target_rotation;
    int32_t level;
    PTCharacter target[10];
    int32_t num_targets;
    S3DPoint target_position[10];

    TQuicksandAnimator(TObjectInstance* oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TQuicksandAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes
    virtual void Animate(bool draw);
      // Called to update frame state
    virtual bool Render();
      // Called to render a frame
    virtual void RefreshZBuffer();

    void InitQuicksand(TObjectInstance* quicksandme, int32_t delay);
      // Tell who to quicksand!
};

// ********************
// * TSandswirlEffect *
// ********************

_CLASSDEF(TSandswirlEffect)

class TSandswirlEffect : public TEffect
{
  private:
  public:
    TSandswirlEffect(TObjectImagery* newim) : TEffect(newim) { }
    TSandswirlEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) { }
    virtual ~TSandswirlEffect() {}

    virtual void Initialize();
    virtual void Pulse();
};

// **********************
// * Sandswirl Animator *
// **********************

_CLASSDEF(TSandswirlAnimator)

class TSandswirlAnimator : public T3DAnimator
{
  private:
    int32_t frameon;                        // frame on... (for timing)
    int32_t angle;
    PTParticle3DAnimator particleanim;
    
  public:
    TSandswirlAnimator(TObjectInstance* oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TSandswirlAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes
    virtual void Animate(bool draw);
      // Called to update frame state
    virtual bool Render();
      // Called to render a frame
    virtual void RefreshZBuffer();

//void SetParticles(TObjectInstance* inst) { particles = inst; }
//TObjectInstance* GetParticles(void) { return particles; }
};

// ********************
// * TTornadoEffect *
// ********************

_CLASSDEF(TTornadoEffect)

class TTornadoEffect : public TEffect
{
  private:
  public:
    TTornadoEffect(TObjectImagery* newim) : TEffect(newim) { }
    TTornadoEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) { }
    virtual ~TTornadoEffect() {}

    virtual void Initialize();
    virtual void Pulse();
};

// **********************
// * Tornado Animator *
// **********************

_CLASSDEF(TTornadoAnimator)

_STRUCTDEF(STornadoParticle)
struct STornadoParticle
{
    hmm_vec3 pos;
    float scl, dscl, th, dth, h, dh, r, dr;
    int32_t count;
};

_STRUCTDEF(SIceChunkParticle)
struct SIceChunkParticle
{
    hmm_vec3 pos, vel, w, th, scl;
    int32_t time, stage;
};

_STRUCTDEF(SFlameParticle)
struct SFlameParticle
{
    hmm_vec3 pos, vel;
    float scl, dscl;
};

class TTornadoAnimator : public T3DAnimator
{
  private:
    int32_t flameon;                        // frame on... (for timing)
    PSTornadoParticle torn;
    PSIceChunkParticle ice;
    PSFlameParticle flame;
        
  public:
    int32_t frameon;
    TTornadoAnimator(TObjectInstance* oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TTornadoAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes
    virtual void Animate(bool draw);
      // Called to update frame state
    virtual bool Render();
      // Called to render a frame
    virtual void RefreshZBuffer();

    virtual void InitTornadoParticle(int32_t i);
    virtual void InitIceParticle(int32_t i);
    virtual void InitFlameParticle(float _x, float _y, float _z, float _vx, float _vy, float _vz, int32_t sc);
};

// ********************
// * TStreamerEffect *
// ********************

_CLASSDEF(TStreamerEffect)

class TStreamerEffect : public TEffect
{
  private:
    bool first_time;
  public:
    TStreamerEffect(TObjectImagery* newim) : TEffect(newim) { Initialize();}
    TStreamerEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) { Initialize();}
    virtual ~TStreamerEffect() {}

    virtual void Initialize();
    virtual void Pulse();
};

// **********************
// * Streamer Animator *
// **********************

_CLASSDEF(TStreamerAnimator)

_STRUCTDEF(SStreamerParticle)
struct SStreamerParticle
{
    hmm_vec3 pos;
    float scl;
    int32_t count;
};

#define STREAMER_MAXSTREAMS     4

class TStreamerAnimator : public T3DAnimator
{
  private:
    int32_t frameon;                        // frame on... (for timing)
    float dscl[STREAMER_MAXSTREAMS], scl[STREAMER_MAXSTREAMS], th[STREAMER_MAXSTREAMS],
        dth[STREAMER_MAXSTREAMS], h[STREAMER_MAXSTREAMS], dh[STREAMER_MAXSTREAMS];
    PSStreamerParticle stream[STREAMER_MAXSTREAMS];
        
  public:
    TStreamerAnimator(TObjectInstance* oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TStreamerAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes
    virtual void Animate(bool draw);
      // Called to update frame state
    virtual bool Render();
      // Called to render a frame
    virtual void RefreshZBuffer();

    virtual void AddStreamer(int32_t num, hmm_vec3 pos, float scl);
      // Add/Update a particle to a trail
    virtual void InitStreamer(int32_t x);
      // Move a trail/Initialize pos/scl
};

// ********************
// * TFireSwarmEffect *
// ********************

_CLASSDEF(TFireSwarmEffect)

class TFireSwarmEffect : public TEffect
{
  private:
  public:
    TFireSwarmEffect(TObjectImagery* newim) : TEffect(newim) { }
    TFireSwarmEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) { }
    virtual ~TFireSwarmEffect() {}

    virtual void Initialize();
    virtual void Pulse();
};

// **********************
// * FireSwarm Animator *
// **********************

_CLASSDEF(TFireSwarmAnimator)

class TFireSwarmAnimator : public T3DAnimator
{
  private:
    int32_t frameon;                        // frame on... (for timing)
    float cylhscl, cylvscl, cylth;
            
  public:
    TFireSwarmAnimator(TObjectInstance* oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TFireSwarmAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes
    virtual void Animate(bool draw);
      // Called to update frame state
    virtual bool Render();
      // Called to render a frame
    virtual void RefreshZBuffer();

};

// ***************
// * THaloEffect *
// ***************

_CLASSDEF(THaloEffect)

class THaloEffect : public TEffect
{
  private:
      float halostep;
      int32_t totframes;
  public:
    THaloEffect(TObjectImagery* newim) : TEffect(newim) { }
    THaloEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) { }
    virtual ~THaloEffect() {}

    virtual void Initialize();
    virtual void Pulse();

    virtual void InitParams(int32_t totalframes, float step) { totframes = totalframes; halostep = step; }
    virtual int32_t GetTotalFrames() { return totframes; }
    virtual float GetHaloStep() { return halostep; }
};

// *****************
// * Halo Animator *
// *****************

_CLASSDEF(THaloAnimator)

class THaloAnimator : public T3DAnimator
{
  private:
    float haloscale;
    int32_t frameon;
            
  public:
    THaloAnimator(TObjectInstance* oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~THaloAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes
    virtual void Animate(bool draw);
      // Called to update frame state
    virtual bool Render();
      // Called to render a frame
    virtual void RefreshZBuffer();
};

// *****************
// * TRippleEffect *
// *****************

_CLASSDEF(TRippleEffect)

class TRippleEffect : public TEffect
{
  private:
     int32_t len;
  public:
    TRippleEffect(TObjectImagery* newim) : TEffect(newim) { }
    TRippleEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) { }
    virtual ~TRippleEffect() {}

    virtual void Initialize();
    virtual void Pulse();

    virtual int32_t GetLength() { return len; }
    virtual void SetLength(int32_t length) { len = length; }
};

// *******************
// * Ripple Animator *
// *******************

_STRUCTDEF(SDropParticle)
struct SDropParticle
{
    hmm_vec3 pos;
    hmm_vec3 vel;
    bool dead;
};

_CLASSDEF(TRippleAnimator)

class TRippleAnimator : public T3DAnimator
{
  private:
    int32_t length, ripframe, frameon, numdrops;                // frame on... (for timing)
    float scale;
    bool hassplashed;

    PSDropParticle drops;

  protected:
    void SetAnimFrame(int32_t frame_num, PS3DAnimObj obj);
      // Called to change texture frame
    
  public:
    TRippleAnimator(TObjectInstance* oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TRippleAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes
    virtual void Animate(bool draw);
      // Called to update frame state
    virtual bool Render();
      // Called to render a frame
    virtual void RefreshZBuffer();

    virtual void AddNewRipple(int32_t x, int32_t y, int32_t z, int32_t len);
};

// *****************
// * TDripEffect *
// *****************

_CLASSDEF(TDripEffect)

class TDripEffect : public TEffect
{
  private:
    int32_t ripplesize, height, period;
  public:
    TDripEffect(TObjectImagery* newim) : TEffect(newim) {  }
    TDripEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) { }
    virtual ~TDripEffect() {}

    virtual void Initialize();
    virtual void Pulse();

    virtual void SetParams(int32_t ri, int32_t he, int32_t pe) { ripplesize = ri; height = he; period = pe; };
    virtual void GetParams(int32_t* ri, int32_t* he, int32_t* pe) { *ri = ripplesize; *he = height; *pe = period; };

    virtual void Load(RTInputStream is, int32_t version, int32_t objversion);
        // Loads object data from the sector
    virtual void Save(RTOutputStream os);
        // Saves object data to the sector
};

// *******************
// * Drip Animator *
// *******************

_CLASSDEF(TDripAnimator)

class TDripAnimator : public T3DAnimator
{
  private:
    hmm_vec3 pos;
    hmm_vec3 vel;
    bool dead;
    int32_t time, ripplesize, height, period;

  protected:
    
  public:
    TDripAnimator(TObjectInstance* oi) : T3DAnimator(oi) { ripplesize = 64; height = 128; period = 48;}
      // Constructor (initialization handled by Initialize)
    virtual ~TDripAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes
    virtual void Animate(bool draw);
      // Called to update frame state
    virtual bool Render();
      // Called to render a frame
    virtual void RefreshZBuffer();

    virtual void AddNewRipple(int32_t x, int32_t y, int32_t z, int32_t len);
};

// ********************
// * TFaultFireEffect *
// ********************

_CLASSDEF(TFaultFireEffect)

class TFaultFireEffect : public TEffect
{
  private:
  public:
    TFaultFireEffect(TObjectImagery* newim) : TEffect(newim) { }
    TFaultFireEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) { }
    virtual ~TFaultFireEffect() {}

    virtual void Initialize();
    virtual void Pulse();
};

// **********************
// * FaultFire Animator *
// **********************

#define FF_GROUPS   10

_CLASSDEF(TFaultFireAnimator)

//static bool ff_init = false;
class TFaultFireAnimator : public T3DAnimator
{
  private:
    float th, tvs[4];
    //static float per[FF_GROUPS], phase[FF_GROUPS];
  public:
    TFaultFireAnimator(TObjectInstance* oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TFaultFireAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes
    virtual void Animate(bool draw);
      // Called to update frame state
    virtual bool Render();
      // Called to render a frame
    virtual void RefreshZBuffer();

    virtual void SetupObjects();
};

// *****************
// * TBloodEffect *
// *****************

_CLASSDEF(TBloodEffect)

class TBloodEffect : public TEffect
{
  private:
    int32_t height, hangle, vangle, hspread, vspread, num;
  public:
    TBloodEffect(TObjectImagery* newim) : TEffect(newim) {  }
    TBloodEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) { }
    virtual ~TBloodEffect() {}

    virtual void OffScreen() { KillThisEffect(); }

    virtual void Initialize();
    virtual void Pulse();

    virtual void SetParams(int32_t he, int32_t ha, int32_t va, int32_t hs, int32_t vs, int32_t nu) { height = he; hangle = ha; vangle = va; hspread = hs, vspread = vs; num = nu; }
    virtual void GetParams(int32_t *he, int32_t *ha, int32_t *va, int32_t *hs, int32_t *vs, int32_t *nu) { *he = height; *ha = hangle; *va = vangle; *hs = hspread; *vs = vspread; *nu = num; }
};

// *******************
// * Blood Animator *
// *******************

_CLASSDEF(TBloodAnimator)

class TBloodAnimator : public T3DAnimator
{
  private:
    TBloodSystem bloods;

  protected:
    
  public:
    TBloodAnimator(TObjectInstance* oi) : T3DAnimator(oi) { }
      // Constructor (initialization handled by Initialize)
    virtual ~TBloodAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes
    virtual void Animate(bool draw);
      // Called to update frame state
    virtual bool Render();
      // Called to render a frame
    virtual void RefreshZBuffer();
};

// *****************
// * TMistEffect *
// *****************

_CLASSDEF(TMistEffect)

class TMistEffect : public TEffect
{
  private:
    
  public:
    TMistEffect(TObjectImagery* newim) : TEffect(newim) {  }
    TMistEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) { }
    virtual ~TMistEffect() {}

    virtual void Initialize();
    virtual void Pulse();
};

// *******************
// * Mist Animator *
// *******************

_CLASSDEF(TMistAnimator)

class TMistAnimator : public T3DAnimator
{
  private:
    PSDropParticle drops;
    int32_t numdrops;   

  protected:
    
  public:
    TMistAnimator(TObjectInstance* oi) : T3DAnimator(oi) { }
      // Constructor (initialization handled by Initialize)
    virtual ~TMistAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes
    virtual void Animate(bool draw);
      // Called to update frame state
    virtual bool Render();
      // Called to render a frame
    virtual void RefreshZBuffer();

    virtual void AddNewRipple(int32_t x, int32_t y, int32_t z, int32_t len);
};

// *****************
// * TWaterFallEffect *
// *****************

_CLASSDEF(TWaterFallEffect)

class TWaterFallEffect : public TEffect
{
  private:
    
  public:
    TWaterFallEffect(TObjectImagery* newim) : TEffect(newim) {  }
    TWaterFallEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) { }
    virtual ~TWaterFallEffect() {}

    virtual void Initialize();
    virtual void Pulse();
};

// *******************
// * WaterFall Animator *
// *******************

_STRUCTDEF(SWaterParticle)
struct SWaterParticle
{
    hmm_vec3 pos;
    hmm_vec3 vel;
    hmm_vec3 scale;
    int32_t time;
};

_CLASSDEF(TWaterFallAnimator)
class TWaterFallAnimator : public T3DAnimator
{
  private:
    PSWaterParticle drops;
    int32_t numdrops;
    S3DPoint eff;

  protected:
    
  public:
    TWaterFallAnimator(TObjectInstance* oi) : T3DAnimator(oi) { }
      // Constructor (initialization handled by Initialize)
    virtual ~TWaterFallAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes
    virtual void Animate(bool draw);
      // Called to update frame state
    virtual bool Render();
      // Called to render a frame
    virtual void RefreshZBuffer();

    virtual void InitParticle(int32_t i);
    virtual void UpdateStuff();
    virtual void DoLighting(float x, float y, float z, PS3DAnimObj object);
};

// *****************
// * TWaterEffect *
// *****************

_CLASSDEF(TWaterEffect)

class TWaterEffect : public TEffect
{
  private:
    
  public:
    TWaterEffect(TObjectImagery* newim) : TEffect(newim) {  }
    TWaterEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) { }
    virtual ~TWaterEffect() {}

    virtual void Initialize();
    virtual void Pulse();
};

// *******************
// * Water Animator *
// *******************

_CLASSDEF(TWaterAnimator)
class TWaterAnimator : public T3DAnimator
{
  private:
    PSWaterParticle drops;
    int32_t numdrops;
    S3DPoint eff;

  protected:
    
  public:
    TWaterAnimator(TObjectInstance* oi) : T3DAnimator(oi) { }
      // Constructor (initialization handled by Initialize)
    virtual ~TWaterAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes
    virtual void Animate(bool draw);
      // Called to update frame state
    virtual bool Render();
      // Called to render a frame
    virtual void RefreshZBuffer();

    virtual void InitParticle(int32_t i);
    virtual void UpdateStuff();
    virtual void DoLighting(float x, float y, float z, PS3DAnimObj object);
};

// *****************
// * TPixieEffect *
// *****************

_CLASSDEF(TPixieEffect)

class TPixieEffect : public TEffect
{
  private:
    
  public:
    TPixieEffect(TObjectImagery* newim) : TEffect(newim) {  }
    TPixieEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) { }
    virtual ~TPixieEffect() {}

    virtual void Initialize();
    virtual void Pulse();
};

// *******************
// * Pixie Animator *
// *******************

_CLASSDEF(TPixieAnimator)
class TPixieAnimator : public T3DAnimator
{
  private:
    PSWaterParticle pix;
    int32_t charnear;
    S3DPoint origpos;

  protected:
    
  public:
    TPixieAnimator(TObjectInstance* oi) : T3DAnimator(oi) { }
      // Constructor (initialization handled by Initialize)
    virtual ~TPixieAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes
    virtual void Animate(bool draw);
      // Called to update frame state
    virtual bool Render();
      // Called to render a frame
    virtual void RefreshZBuffer();

};

// *** Ambient Sound Effect...

#define SOUNDNAME_LEN       (32)

_CLASSDEF(TAmbSoundEffect)

class TAmbSoundEffect : public TEffect
{
  protected:
    char sound_name[SOUNDNAME_LEN]; // the name of the sound effect
    int32_t id;                         // the sound's id
    int32_t sample_length;              // the number of frames (FRAMES, PEOPLE, ***FRAMES***) in the sound sample
    int32_t ticks;                      // our current tick count...

  public:
    TAmbSoundEffect(TObjectImagery* newim) : TEffect(newim) { Initialize(); }
    TAmbSoundEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) { Initialize(); }
    virtual ~TAmbSoundEffect() { if (id > 0) SoundPlayer.Unmount(id); }

    virtual void Load(RTInputStream is, int32_t version, int32_t objversion);
        // Loads object data from the sector
    virtual void Save(RTOutputStream os);
        // Saves object data to the sector
    virtual void Initialize();
    virtual void Pulse();

    virtual void GetParams(char *name, int32_t *len) { strcpy(name, sound_name); *len = sample_length; }
        // gets the current settings for this speaker object
    virtual void SetParams(char *name, int32_t len);
        // sets the sound name and length of that sample... used by the editor
};


// *** Ambient Sound Animator...

_CLASSDEF(TAmbSoundAnimator)
class TAmbSoundAnimator : public T3DAnimator
{
  public:
    TAmbSoundAnimator(TObjectInstance* oi) : T3DAnimator(oi) { }
    void Initialize();
    void Animate(bool);
    bool Render();
    void RefreshZBuffer();
};

