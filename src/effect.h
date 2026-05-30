// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      effect.h - TEffect module                        *
// *************************************************************************

#pragma once

#include "revenant.h"

#include <memory>
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
#include "time.h"   // TTime::LegacyFrameCount() for flipbook 24 Hz gate

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
#include "renderer.h"   // EFxDebugMode, SBillboardDrawItem, TRenderer

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
//
// Submission path: each frame, MapRenderer's per-instance Submit walk
// calls Submit(*Renderer, *oi). The component fills out an
// SBillboardDrawItem (world-space, world-unit size, normalized uv_rect)
// and hands it to Renderer->SubmitFxBillboard; the renderer's fx_pass
// drains the queue after RunLightingPass (see docs/vfx/PHASE1_SPINE.md).
//
// Diagnostic ladder is per-instance EFxDebugMode; the shader branches per
// PHASE1_SPINE.md §6 so the same pipeline serves Normal / SolidColor /
// FullTexture / CurrentFrame.
class TFlipbookBillboardComponent : public TObjectComponent
{
  public:
    [[nodiscard]] const char* ComponentName() const override { return "flipbook_billboard"; }

    void Configure(TTextureHandle texture, int32_t texture_width, int32_t texture_height,
                   int32_t columns, int32_t rows, int32_t frames,
                   float width, float height,
                   bool additive, bool replaces_default)
    {
        texture_handle = texture;
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
    void SetDebugMode(EFxDebugMode mode) { debug_mode = mode; }
    bool SetFrameExpression(const char* expr, std::string* error = nullptr)
        { return frame_expr.Compile(expr, error); }
    bool SetUvRectExpression(const char* expr, std::string* error = nullptr)
        { return uv_rect_expr.Compile(expr, error); }

    void OnAttach() override { RegisterUpdate(&TObjectComponent::Update); }
    void OnDetach() override { UnregisterUpdate(&TObjectComponent::Update); }

    [[nodiscard]] bool ReplacesDefaultVisual() const { return replaces_default_visual; }
    [[nodiscard]] TTextureHandle Texture() const { return texture_handle; }
    [[nodiscard]] float Width() const { return size_w; }
    [[nodiscard]] float Height() const { return size_h; }
    [[nodiscard]] bool AdditiveBlend() const { return additive_blend; }
    [[nodiscard]] EFxDebugMode DebugMode() const { return debug_mode; }

    // Fill an SBillboardDrawItem from current component state and submit
    // it to the renderer's FX queue. Called from the per-instance Submit
    // walk in maprenderer.cpp.
    void Submit(TRenderer& renderer, const TObjectInstance& inst) const;

  protected:
    void OnUpdate() override
    {
        // RunUpdateList runs every render frame (60-120 Hz). The
        // flipbook animation is authored for the legacy 24 Hz sim
        // rate -- gate increments to actual legacy-frame transitions
        // so it ticks 24x/sec on any display rate. Without this gate
        // a 60 Hz display ran the 18-frame cycle 2.5x too fast.
        const int64_t now = TTime::LegacyFrameCount();
        if (now == last_legacy_seen_)
            return;
        last_legacy_seen_ = now;
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

    TTextureHandle texture_handle = kInvalidTexture;
    int32_t tex_w = 1, tex_h = 1;
    int32_t cols = 1, rows = 1, frame_count = 1;
    int32_t legacy_frame = 0;
    // Last legacy frame index we observed in OnUpdate -- gates frame
    // advancement to 24 Hz regardless of the render frame rate.
    int64_t last_legacy_seen_ = -1;
    TParticleExpression frame_expr;
    TParticleExpression uv_rect_expr;
    float size_w = 1.0f, size_h = 1.0f;
    bool additive_blend = true;
    bool replaces_default_visual = true;
    EFxDebugMode debug_mode = EFxDebugMode::Normal;
};

struct SParticleBucketEffectDef
{
    std::string name;
    int32_t texture_slot = 0;
    int32_t atlas_cols = 1;
    int32_t atlas_rows = 1;
    int32_t atlas_frames = 1;
    // Random start frame from the atlas grid on spawn (and on chain-spawn).
    // When true and atlas_frames > 1, DrawFrame is auto-set to a uniform
    // [0, atlas_frames) integer per particle, using the per-particle seed
    // for determinism. Saves `frame = floor(rand01() * N)` boilerplate in
    // spawn_expr for "N drop variants" / "N spark variants" cases.
    // spawn_expr (if any) can still override.
    bool random_start_frame = false;
    float width = 1.0f;
    float height = 1.0f;
    float scale = 1.0f;
    bool additive = true;
    bool flip_v = false;
    bool chroma_key = false;
    float chroma_key_rgb[3] = {1.0f, 0.0f, 0.0f};
    std::string frame_expr;
    std::string uv_rect_expr;

    // ---- VM-extension fields (2026-05-17) -----------------------------
    // Imagery override for buckets that ship a procedural texture or
    // reach into a non-owner imagery (B01 / M05 ports load a separate
    // I3D via TObjectImagery::LoadImagery rather than using the owner's
    // primary imagery). Empty = use the owner-imagery texture_slot path.
    std::string imagery_path;

    // Blend/light/depth/orientation knobs (canonical strings parsed into
    // the per-bucket EParticle* enums). When unset, BuildRuntimeBucketDesc
    // falls back to the legacy `additive` bool + Unlit + TestNoWrite +
    // ScreenAligned defaults.
    std::string blend_mode;          // "alpha" | "additive" | "additive_straight" | "premul_alpha"
    std::string light_mode;          // "unlit" | "lit_flat"
    std::string depth_mode;          // "test_no_write" | "test_write" | "none"
    std::string orientation;         // "screen_aligned" | "world_xy"

    // Per-particle dynamics (statement-form expressions). Compiled once
    // at first-use; cached on the runtime bucket record.
    std::string spawn_expr;          // ran once at particle add time
    std::string tick_expr;           // ran each sim tick per live particle
    std::string kill_expr;           // boolean; true -> respawn (re-run spawn_expr, age=0)

    // Spawn cadence:
    //   spawn_burst > 0 → emit N particles once at attach (one-shot)
    //   spawn_count > 0 → maintain a steady-state population of N particles
    //                     (runtime tops up when count drops below target)
    // At most one should be non-zero; if both are zero the bucket emits
    // nothing on its own and is driven by the legacy emitter/output path.
    int32_t spawn_burst = 0;
    int32_t spawn_count = 0;

    // Integration cadence:
    //   tick_hz > 0  → integrate tick_expr at this fixed rate (sim-tick
    //                  gated, matches retail's 24 Hz authoring cadence)
    //   tick_hz == 0 → integrate every render frame (no gating)
    int32_t tick_hz = 0;

    // Default particle life in seconds (set into EParticleVar::Life at
    // spawn time). <= 0 means "no auto-kill" (drops live until kill_expr
    // fires).
    float default_life = -1.0f;

    // ---- Chain emission (reflection-plane / state-transition mechanism) ----
    // When a particle in this bucket dies (kill_expr fires, or life
    // expires, or default_life is 0 with no kill_expr running), spawn one
    // particle into `chain_bucket` instead of (or in addition to) reaping
    // the source. The chain spawn runs the target bucket's spawn_expr
    // with `emit_pos` and `emit_vel` set to the dying particle's pos /
    // vel — so the chain particle inherits the impact site naturally.
    //
    // Generic mechanism — blood uses it for FLY droplet -> SPLAT decal
    // transition; future "bullet impact -> spark burst", "spell missile ->
    // explosion smoke", etc. wire through the same field. Empty = no
    // chain emission (default).
    std::string chain_bucket;
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
    // Per-frame integration tick. Called from TParticleEffectComponent::
    // DrawPulse via the render-frame walk so each runtime's bucket
    // expressions (spawn / tick / kill) advance once per frame using the
    // shared TTime::DeltaTime.
    void IntegrateEffect(const SParticleEffectDef* effect_def, TObjectInstance* owner, float dt_seconds);
    void ExpireInactiveEffects();
    void Clear();

  private:
    // Per-bucket compiled-expression cache + sim-tick accumulator.
    // Created lazily when the runtime first encounters a bucket_def with
    // a non-empty spawn_expr / tick_expr / kill_expr.
    struct SBucketRuntime
    {
        TParticleBucket* bucket = nullptr;
        const SParticleBucketEffectDef* bucket_def = nullptr;
        TParticleExpression spawn_expr;     // statement form
        TParticleExpression tick_expr;      // statement form
        TParticleExpression kill_expr;      // single expression (boolean)
        bool spawn_compiled = false;
        bool tick_compiled = false;
        bool kill_compiled = false;
        double sim_accum_ms = 0.0;
        int32_t spawned_burst = 0;          // tracks one-shot burst so we don't re-emit
    };

    struct SRuntime
    {
        const SParticleEffectDef* effect_def = nullptr;
        TSafeRef<> owner;
        float owner_particle_id = -1.0f;
        uint32_t last_draw_pulse_pass = 0;
        std::vector<TParticleBucket*> buckets;
        std::vector<SBucketRuntime> bucket_runtimes;
    };

    SRuntime* FindRuntime(const SParticleEffectDef* effect_def, int32_t owner_map_index);
    void StartRuntime(SRuntime& runtime, TObjectInstance* owner);
    void StopRuntime(size_t runtime_index);
    void IntegrateBucket(SRuntime& runtime, SBucketRuntime& brt, TObjectInstance* owner,
                         float dt_seconds, float owner_particle_id);
    void EmitSpawnTopup(SBucketRuntime& brt, TObjectInstance* owner, float owner_particle_id);
    // Emit one chain particle into `chain_brt` using `(pos, vel)` as the
    // chain spawn's emit_pos / emit_vel. Returns the new particle index
    // (-1 if the chain bucket is full or the chain runtime is missing).
    int32_t EmitChainParticle(SBucketRuntime& chain_brt, TObjectInstance* owner,
                              float owner_particle_id,
                              const float dying_pos[3],
                              const float dying_vel[3]);
    // Find the sibling bucket runtime within `runtime` by bucket name
    // (matches SParticleBucketEffectDef::chain_bucket).
    SBucketRuntime* FindBucketRuntime(SRuntime& runtime, const std::string& bucket_name);

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

    void DrawPulse();

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

// Number of scatter quads in the ambient-fire patch. Pre-release matches
// `NUMFIRES` further down (the legacy TFireAnimator declaration).
// Forensics §6: legacy TFireAnimator::Animate/Render loops over `c < 10`
// even though `NUMFIRES == 15` — pre-release WIP quirk. The port keeps
// the full 15-quad scatter as the modern default; INVENTORY F03 gap 7(a)
// records the legacy 10-loop quirk for traceability.
inline constexpr int32_t kFireScatterQuads = 15;

// One scatter quad's transient state. Pre-release stored these as parallel
// arrays `p[NUMFIRES]` / `f[NUMFIRES]` on TFireAnimator; we collapse onto
// the effect class (matches the H03 / M05 / L02 animator-state collapse).
struct SFireScatterQuad
{
    float   ox = 0.0f;      // x offset from patch origin, in world units
    float   oy = 0.0f;      // y offset from patch origin, in world units
    int32_t frame = 0;      // per-quad atlas frame counter (-22..30 cycle)
};

class TFireEffect : public TEffect
{
  public:
    TFireEffect(TObjectImagery* newim) : TEffect(newim) {}
    TFireEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}

    virtual void Pulse();

    // Spawn a standalone TFireEffect for the --test=vfx harness (F03
    // rig). Per INVENTORY F03 forensics §1: no live retail caller exists
    // (no spell.def variant invokes the bare "fire" builder, no
    // ATTACHEFFECT "fire") — this harness path IS the canonical Phase-B
    // exercise route. Loads the real `Misc\Fire.I3D` asset (F03b — the
    // 2026-05-16 procedural-gradient stand-in was replaced per
    // AGENT_GUIDE §4.2.1; helper trace lives in the F03b inventory
    // sub-section). Caller owns the returned pointer.
    [[nodiscard]] static TFireEffect* SpawnForTest(const S3DPoint& origin);

    // Per-frame tick + submit for the harness; mirrors F01 / H03 / M05 /
    // L02. Drives per-quad atlas-frame cycling (sim-tick-gated at 24 Hz
    // to match other Fire-family cadence) and submits
    // `kFireScatterQuads` ground-projected (WorldXY) textured billboards
    // via SubmitFxBillboard each frame, picking the per-quad texture
    // frame from the real I3D's `framehtexs[]` table. FB pipeline only
    // — pre-release renders quads directly, no particle bucket
    // (forensics §6 / F03b §1).
    void TickAndSubmitForTest(EFxDebugMode debug_mode);

    // True until the harness `delete`s the effect. Pre-release Pulse
    // self-killed via `spell->GetFire()` damage gate; the standalone rig
    // is permanently alive (no spell context) so this just mirrors the
    // alive_ flag for parity with F01 / H03 / M05 / L02. The harness's
    // SpellGround re-fire cadence rotates fresh instances in.
    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    // Pre-release TFireAnimator owned `p[NUMFIRES]` + `f[NUMFIRES]`;
    // collapsed onto the effect class (same pattern as H03 / M05 / L02).
    // S3DMat `mat[NUMFIRES]` from pre-release is dropped — material slots
    // were zeroed on init and never re-touched (legacy effect.cpp:909-925),
    // which has no analog in the modern FB-billboard pipeline (per-item
    // additive blend is set via key.blend, not per-material).
    SFireScatterQuad quads_[kFireScatterQuads] {};
    bool             alive_ = true;
    double           sim_accum_ms_ = 0.0;
    int32_t          rng_seed_ = 0;     // per-instance RNG seed; reseeded each lifetime
    // Real-asset state (F03b): the Misc\Fire.I3D imagery and the resolved
    // texture-slot-0 frame array. The imagery is owned by the effect
    // (FreeImagery in dtor) — same lifecycle pattern as F01 / H04. The
    // frame texture handle array is a cached snapshot of `framehtexs[0..N)`
    // captured at SpawnForTest to avoid a virtual GetTexture per quad per
    // frame in the submit hot path.
    TObjectImagery*           imagery_ = nullptr;
    std::vector<TTextureHandle> frame_textures_ {};
};

// ****************
// * TSparkEffect *
// ****************
//
// X22 generic spark burst ("sparks" / TSparkAnimator). Faithful direct
// port of the pre-release particle loop, NOT a re-derivation through the
// TParticleBucket / effects.def abstraction (which is how a prior attempt
// drifted into mixed colors / wrong blend). The original is the simplest
// effect in the game:
//
//   spawn  = TCharacter::EffectBurst "sparks" branch (character.cpp:2262-2317)
//            builds an SParticleParams and calls anim->InitParticles(&pr).
//   update = TParticle3DAnimator::Animate  (effect_old.cpp:4790-4942).
//   render = TParticle3DAnimator::Render   (effect_old.cpp:4944-4986).
//
// The animator was a fully generic particle system parameterized entirely
// by SParticleParams; for the spark use only the non-seeking ballistic
// path runs (seektargets=false). The per-particle arrays
// (pos/vel/life/start/obj) from TParticle3DAnimator are collapsed onto
// this effect class (same animator-state-collapse convention as F03 fire
// / H03 ripple / M05 mist). One-shot burst: random(15,25) particles seeded
// at once, the object self-destructs (killobj) once all particles expire.
//
// Single color per burst: objflags = 1 << (ObjId() & 3) selects exactly
// ONE of the 4 photon sub-objects (photon / photon01 / photon02 / photon03)
// of Misc\Sparks.I3D for the WHOLE burst — every particle draws that one
// sprite. The 4 are NOT mixed.
//
// Retail-confirmed param divergences from the snapshot (verified IEEE bit
// patterns at recon/classes/cls_0x5a7b98.cpp:4640-4664): gravity 0.25
// (snapshot 0.2), trails 2 (snapshot 1 — each spark draws as a 2-step
// motion streak), bounce true (snapshot false — sparks bounce off the
// floor). See forensics SPARKS_TSparkAnimator.md §2.1.
//
// Blend = Alpha (SRC_ALPHA / INV_SRC_ALPHA) per the combat-spark Render body
// (SetBlendState) and forensics §7. (An "additive" hypothesis came from
// comparing against the GREEN Fountain/Sparkle effect — a different effect;
// combat sparks are Alpha.) Snapshot-only since the retail
// TParticle3DAnimator::Render TU was not decompiled; vet against real
// combat-spark footage. Unlit, TestNoWrite, ScreenAligned.

// Max particles a single burst can hold (caller seeds random(15,25)). The
// pre-release allocated `new[params.particles]`; the modern port uses a
// fixed-cap inline array (no heap churn per burst).
inline constexpr int32_t kSparkMaxParticles = 32;

// One spark particle's transient state. Pre-release stored these as the
// parallel arrays p[]/v[]/l[]/s[]/o[] on TParticle3DAnimator; collapsed
// here onto a per-particle struct (life/start are integer ticks in the
// original — kept as float real-tick counters for framerate-independent
// integration, see TickAndSubmitForTest).
struct SSparkParticle
{
    hmm_vec3 pos   = {0.0f, 0.0f, 0.0f};   // object-local position (wu)
    hmm_vec3 vel   = {0.0f, 0.0f, 0.0f};   // velocity (wu / sim-tick)
    float    life  = 0.0f;                 // remaining lifetime, in sim-ticks
    float    start = 0.0f;                 // start delay, in sim-ticks
};

class TSparkEffect : public TEffect
{
  public:
    TSparkEffect(TObjectImagery* newim) : TEffect(newim) {}
    TSparkEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}

    // Spawn a standalone single-burst TSparkEffect for the --test=vfx
    // harness. Loads the real Misc\Sparks.I3D imagery (NO procedural
    // stand-in — the authored photon sprite IS the visual identity),
    // picks one photon variant for the whole burst, and seeds
    // random(15,25) particles via the ported InitParticles loop. Returns
    // nullptr if the imagery can't be loaded. Caller owns the pointer.
    [[nodiscard]] static TSparkEffect* SpawnForTest(const S3DPoint& origin);

    // Per-frame tick + submit for the harness. Ports
    // TParticle3DAnimator::Animate (ballistic integrate + gravity +
    // bounce + death) and ::Render (one billboard per live particle, plus
    // trails-1 ghost copies stepped along velocity) directly, converted to
    // framerate-independent integration (per-tick rates -> per-second via a
    // 24 Hz sim-tick accumulator). FB pipeline (one additive/alpha textured
    // billboard per draw via SubmitFxBillboard).
    void TickAndSubmitForTest(EFxDebugMode debug_mode);

    // True until the last particle expires (mirrors the original's
    // killobj=true self-destruct). The harness uses this to know when a
    // burst has fully played out before re-triggering.
    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    SSparkParticle particles_[kSparkMaxParticles] {};
    int32_t        num_particles_ = 0;
    float          gravity_       = 0.25f;  // wu / sim-tick^2 (retail)
    int32_t        trails_        = 2;      // render sub-steps per particle (retail)
    bool           bounce_        = true;   // retail
    TTextureHandle texture_       = kInvalidTexture; // chosen photon variant
    // UV sub-rect (x,y,w,h normalized) of the shared Sparks.I3D texture
    // for the chosen photon variant. The 4 sub-objects (photon/01/02/03)
    // partition ONE atlas texture into 4 differently-tinted photon cells;
    // drawing the full [0,0,1,1] rect would show all 4 colors at once
    // (the mixed-color bug). Computed at spawn from the variant's authored
    // vertex UVs so every particle in the burst draws its single cell.
    float          uv_rect_[4]    = {0.0f, 0.0f, 1.0f, 1.0f};
    float          quad_size_wu_  = 24.0f;  // billboard size (from sprite cell)
    bool           alive_         = true;
    double         sim_accum_ms_  = 0.0;
};

class TFlameEffect : public TEffect
{
  public:
    TFlameEffect(TObjectImagery* newim) : TEffect(newim) { InitializeVisualComponent(newim); }
    TFlameEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) { InitializeVisualComponent(newim); }

    static void AttachVisualComponent(TObjectInstance* inst, TObjectImagery* imagery);

    // Spawn a standalone TFlameEffect for the --test=vfx harness. Loads the
    // canonical TorchFlame imagery (`Magic\flame.i3d`), constructs a
    // sector-less instance pinned to world `origin`, and attaches the
    // flipbook + particle components. Returns nullptr if the imagery can't
    // be loaded (asset missing / not yet ready). The caller owns the
    // returned pointer and must `delete` it to release the imagery refcount
    // and the attached components.
    [[nodiscard]] static TFlameEffect* SpawnForTest(const S3DPoint& origin);

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
      float halostep = 0.0f;
      int32_t totframes = 0;
  public:
    THaloEffect(TObjectImagery* newim) : TEffect(newim) { }
    THaloEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) { }
    virtual ~THaloEffect() {}

    virtual void Initialize();
    virtual void Pulse();

    virtual void InitParams(int32_t totalframes, float step) { totframes = totalframes; halostep = step; }
    virtual int32_t GetTotalFrames() { return totframes; }
    virtual float GetHaloStep() { return halostep; }

    // Spawn a standalone THaloEffect for the --test=vfx harness. No
    // imagery lookup — the halo's radial-gradient texture is built
    // procedurally via `Renderer->RegisterTextureAsset` (no
    // `Magic\halo.i3d` ships in data/ — see INVENTORY L02 gap 7.2).
    // Constructs a sector-less instance pinned to world `origin`,
    // stamps a fresh map index, seeds InitParams(totframes, halostep)
    // for the triangle-wave envelope. Returns nullptr if the renderer
    // isn't ready. Caller owns the returned pointer and `delete`s it.
    [[nodiscard]] static THaloEffect* SpawnForTest(const S3DPoint& origin);

    // Per-frame tick + submit for the harness; mirrors F01 / H03 / M05.
    // Drives the pre-release triangle-wave scale envelope (grow for
    // totframes/2 ticks then shrink linearly back to zero), submits one
    // screen-aligned additive billboard via SubmitFxBillboard (FB
    // pipeline) AND re-adds one dynamic point light via AddPointLight
    // (LS pipeline coupling — INVENTORY L02 §4). Self-killed when
    // `frameon > totframes`.
    void TickAndSubmitForTest(EFxDebugMode debug_mode);

    // True until the halo's `frameon > totframes` condition fires
    // (matching pre-release THaloAnimator::Animate's KillThisEffect
    // gate). The harness uses this to early-out after death.
    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    // Per-instance animator state. Pre-release split this across
    // THaloEffect (halostep / totframes) + THaloAnimator (haloscale /
    // frameon). For the standalone test-harness port we collapse the
    // animator state onto the effect class (matches H03 collapse).
    float   haloscale_   = 0.0f;     // current scale; triangle-wave envelope
    int32_t frameon_     = 0;        // monotonic frame counter (sim-tick-gated)
    bool    alive_       = true;
    double  sim_accum_ms_ = 0.0;     // sim-tick gate accumulator
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

// ***********************
// * TTeleporterEffect *
// ***********************
//
// M09 — Misthaven recall + Player Teleport + 12+ monster-summoning
// variants (16 callers in spell.def). Pre-release source lives at
// src/effect_old.cpp:5444-5880 (TTeleporterEffect + TTeleporterAnimator,
// both intact). Recon: not extracted (only Teleporter string XREFs at
// 0x004e6a20 / 0x004e6a40 are visible). Forensics: docs/vfx/M09_FORENSICS.md.
//
// Port shape (M09b, real mesh draw): loads the variant's I3D imagery
// (Magic\teleportation.I3D or Magic\gvortex.I3D), pulls sub-object 1
// (the cylinder geometry) via ExtractSubMesh, registers it as a single
// MeshHandle, then stamps it 5x per frame via SubmitHelperMesh in the
// additive-blended helper pass — pre-release's
// `for (z=0..4) RenderObject(obj)` loop with rot.z = rotation + z*0.5
// (radians) and per-flare scale envelope. Visual primitive; gameflow
// wires the actual character SetPos payload separately via GetPhase()
// polling of the OUT→MOVE transition. See M09_FORENSICS.md §3, §7.6,
// and the M09b section for the I3D draw path.
//
// Lifecycle: INIT (1 tick) → OUT (50) → MOVE (1, payload) → IN (≤49)
// at 24 Hz; total ~4.17 s.

_CLASSDEF(TTeleporterEffect)

// Pre-release state-machine phase identifiers (effect_old.cpp:5448-5451).
// kept as an enum class so call sites read cleanly; the int32_t values
// are stable across versions in case any serialization path lands.
enum class ETeleporterPhase : int32_t
{
    Init = 1,    // resolve spell variant (1 tick)
    Out  = 2,    // grow rotating column over 50 sim ticks (caster fade-out)
    Move = 3,    // single-tick payload (the actual SetPos happens here in-game)
    In   = 4,    // shrink rotating column over ≤49 sim ticks at destination
};

class TTeleporterEffect : public TEffect
{
  public:
    TTeleporterEffect(TObjectImagery* newim) : TEffect(newim) { }
    TTeleporterEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) { }
    // M09b: destructor releases the per-instance entry in the mesh-
    // binding side table (defined in effect.cpp). Not =default so the
    // header doesn't drag in the imagery API.
    ~TTeleporterEffect() override;

    void OffScreen() override { KillThisEffect(); }

    virtual void Initialize();
    virtual void Pulse();

    // Spawn a standalone TTeleporterEffect for the --test=vfx harness.
    // Loads `imagery_path` (default: gvortex.I3D, the Misthaven recall
    // asset — the user's headline ask) and pulls the cylinder mesh out
    // for later per-frame stamping. Constructs a sector-less instance
    // pinned to world `origin`, stamps a fresh map index, seeds the
    // per-flare position table from the pre-release Initialize body.
    // Returns nullptr if the renderer isn't ready or the imagery can't
    // be loaded. Caller owns the returned pointer and `delete`s it.
    [[nodiscard]] static TTeleporterEffect* SpawnForTest(
        const S3DPoint& origin,
        const char* imagery_path = "Magic\\gvortex.I3D");

    // Per-frame sim-tick advance: drains the 24 Hz sim accumulator into
    // pre-release Animate kinematics (rotation, iteration morph, p[i]
    // squeeze) and ticks Pulse() for the phase state machine. Called
    // first each frame by the harness, before the tile pass opens.
    void TickForTest();

    // Per-frame mesh submission. Stamps the loaded I3D cylinder asset
    // 5x with per-flare rot.z + scale (the pre-release
    // `for(z=0..4) RenderObject(obj)` loop), each through
    // SubmitHelperMesh (additive, double-sided). MUST be called after
    // Renderer->BeginTilePass — that call clears the
    // transparent_world_queue which holds pending SubmitHelperMesh
    // entries. The vfxtest harness routes this through SEffect::
    // submit_world; see vfxtest.cpp Render() Step 3.
    void SubmitWorldForTest(EFxDebugMode debug_mode);

    // Convenience wrapper: TickForTest + SubmitWorldForTest in sequence.
    // Useful for callers that don't care about the tile-pass split (e.g.
    // headless harness probes). NOT used by the vfxtest browser path
    // because the tile-pass timing matters there.
    void TickAndSubmitForTest(EFxDebugMode debug_mode)
    {
        TickForTest();
        SubmitWorldForTest(debug_mode);
    }

    // Phase enum for gameflow's payload coupling. Gameflow polls this
    // and fires its caster->SetPos(destination) on the OUT→MOVE
    // transition (single tick), then SetFade(0,-10) on the caster as
    // we enter IN. See M09_FORENSICS.md §7.6.
    [[nodiscard]] ETeleporterPhase GetPhase() const { return phase_; }

    // True until life >= 100 sim ticks (pre-release animator self-kill
    // gate at effect_old.cpp:5818). The harness checks this for early-out.
    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    // --- Pre-release effect-side state (effect_old.cpp:5459-5463) ---
    ETeleporterPhase phase_       = ETeleporterPhase::Init;
    int32_t          life_        = 0;
    int32_t          spell_level_ = 1;       // 0=VortexM/Misthaven recall, 1=Teleport, 2=Teleport2
    bool             alive_       = true;

    // --- Pre-release animator-side state (effect_old.cpp:5478-5482) -
    // Collapsed onto the effect class for the harness port (matches
    // the H03 / L02 / F03 collapse). The 5 per-flare positions p[i]
    // morph over the OUT phase per Animate's iteration logic; we
    // store them as the live values (initialized in SpawnForTest).
    float   flare_p_x_[5]    = {0,0,0,0,0};
    float   flare_p_z_[5]    = {0,0,0,0,0};
    int32_t iterations_      = 0;    // OUT-phase morph counter (caps at 50)
    int32_t ticks_           = 0;    // render-envelope clock (`% 100` per pre-release)
    float   rotation_rad_    = 0.0f; // continuous spin, +0.1 rad / sim tick

    // sim-tick gate accumulator (same pattern as F01 / H03 / M05 / L02 / F03).
    double  sim_accum_ms_    = 0.0;
};

// *********************
// * Teleporter Animator *
// *********************
//
// Legacy declaration kept to satisfy the existing in-game animator-
// registry comments (`REGISTER_3DANIMATOR("Teleporter", TTeleporterAnimator)`
// at effect.cpp:29). The pre-release body lives at
// src/effect_old.cpp:5474-5880 and is the source-of-truth for behaviour;
// in the modern port the animator state is collapsed onto
// TTeleporterEffect (M09 forensics §1, §7.4). This stub exists so the
// vtable links if any caller still references the class name; calling
// Animate / Render on it is a no-op until the real I3D mesh port (M09b)
// lands. Per-instance ports should drive the effect via
// TTeleporterEffect::TickAndSubmitForTest, not this stub.

_CLASSDEF(TTeleporterAnimator)

class TTeleporterAnimator : public T3DAnimator
{
  public:
    TTeleporterAnimator(TObjectInstance* oi) : T3DAnimator(oi) { }
    ~TTeleporterAnimator() override { Close(); }

    void Initialize() override {}                  // M09b: port pre-release body
    void Animate(bool /*draw*/) override {}        // M09b: port pre-release body
    bool Render() override { return true; }        // M09b: port pre-release body
};

// *****************
// * TRippleEffect *
// *****************

_CLASSDEF(TRippleEffect)

class TRippleEffect : public TEffect
{
  private:
     int32_t len = 0;
  public:
    TRippleEffect(TObjectImagery* newim) : TEffect(newim) { }
    TRippleEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) { }
    virtual ~TRippleEffect() {}

    virtual void Initialize();
    virtual void Pulse();

    virtual int32_t GetLength() { return len; }
    virtual void SetLength(int32_t length) { len = length; }

    // Spawn a standalone TRippleEffect for the --test=vfx harness. No
    // imagery lookup — the ripple's atlas is built procedurally via
    // `Renderer->RegisterTextureAsset` (see effect.cpp). Constructs a
    // sector-less instance pinned to world `origin`, stamps a fresh
    // map index, and seeds the per-instance animator state. Returns
    // nullptr if the renderer isn't ready. The caller owns the
    // returned pointer and `delete`s it when done.
    [[nodiscard]] static TRippleEffect* SpawnForTest(const S3DPoint& origin);

    // Per-frame tick + submit for the harness; mirrors B01 / S01. Grows
    // the ring scale, cycles the 4x4 atlas frame, fades alpha during the
    // dissipation phase, and submits one screen-aligned billboard via
    // SubmitFxBillboard. Self-killed when the lifetime elapses (caller
    // detects via `IsAlive()` and respawns on next retrigger tick).
    void TickAndSubmitForTest(EFxDebugMode debug_mode);

    // True until the ripple's `frameon > length && ripframe == 15`
    // condition fires (matching pre-release TRippleAnimator::Animate's
    // OF_KILL gate). The harness uses this to early-out after death.
    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    // Per-instance animator state. Pre-release split this across
    // TRippleEffect (just `len`) + TRippleAnimator (frameon, ripframe,
    // scale). For the standalone test-harness port we collapse the two
    // into the effect class; there's no caller-side rig that needs the
    // animator/effect separation, and the H03 row's pre-release source
    // bodies live cleanly as one unit.
    int32_t frameon_  = 0;        // monotonic frame counter (sim-tick-gated)
    int32_t ripframe_ = 0;        // 0..15 cycle index into rippleframeof[]
    float   scale_    = 0.5f;     // ring scale; grows by 1/16 per sim tick
    bool    alive_    = true;
    double  sim_accum_ms_ = 0.0;  // sim-tick gate accumulator
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
    // Pre-release per-instance params (sector-script-configured via the
    // `setdrip` command — src/command.cpp:1544). Defaults mirror the
    // TDripAnimator ctor at effect_old.cpp:11058 (rippelsize=64,
    // height=128, period=48). Field initializers per project rule.
    int32_t ripplesize = 64;
    int32_t height     = 128;
    int32_t period     = 48;

    // --- Phase 2 (H04) PE-pipeline scaffold -----------------------------
    // Single-particle bucket borrowed from the global TParticleManager.
    // Pre-release tracked one in-flight drop per emitter (single pos/vel,
    // not an array). The PE-bucket equivalent is one particle per drip
    // instance with respawn-in-place semantics (similar in shape to M05's
    // continuous emitter but only ever 1 particle alive at a time).
    //
    // The bucket itself outlives this effect; per-instance drops are
    // disambiguated by `owner_particle_id_` and killed off in the
    // destructor via TParticleBucket::KillParticlesByOwner.
    TParticleBucket* bucket_            = nullptr;
    float            owner_particle_id_ = -1.0f;
    double           sim_accum_ms_      = 0.0;   // 24 Hz sim-tick gate (forensics §7.4)

    // Cyclic emitter state. `dead_` corresponds to pre-release
    // TDripAnimator::dead; `time_` to its `time` frame counter (used in
    // the dead-branch respawn gate `time > period && !random(0,
    // period/2)`). When alive, the bucket particle is visible and
    // integrates pos/vel; when dead, the particle is parked offscreen
    // and time_ counts up toward the next respawn coin flip.
    bool             dead_              = true;
    int32_t          time_              = 0;
    // Ripples spawned on landing — the drip→ripple chain (forensics §3).
    // Owned by this effect, ticked + pruned each frame in
    // TickAndSubmitForTest. Reuses H03's standalone SpawnForTest.
    std::vector<std::unique_ptr<TRippleEffect>> spawned_ripples_;

  public:
    TDripEffect(TObjectImagery* newim) : TEffect(newim) {  }
    TDripEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) { }
    ~TDripEffect() override;

    virtual void Initialize();
    void Pulse() override;

    virtual void SetParams(int32_t ri, int32_t he, int32_t pe) { ripplesize = ri; height = he; period = pe; };
    virtual void GetParams(int32_t* ri, int32_t* he, int32_t* pe) { *ri = ripplesize; *he = height; *pe = period; };

    virtual void Load(RTInputStream is, int32_t version, int32_t objversion);
        // Loads object data from the sector
    virtual void Save(RTOutputStream os);
        // Saves object data to the sector

    // Spawn a standalone TDripEffect for the --test=vfx harness. Loads
    // `Magic\drip.i3d` (the canonical drip sprite at
    // legacy/Imagery/Magic/drip.i3d), allocates / reuses a global PE
    // bucket keyed off the drip texture, seeds one particle parked in
    // the dead state, and stamps the instance with a fresh map index.
    // Returns nullptr if the imagery can't be loaded. The caller owns
    // the returned pointer and must `delete` it to release the imagery
    // refcount, evict its particle, and drop any in-flight spawned
    // ripples. The harness-rig path also reduces the retail period (48)
    // to a screencap-friendly default (~24) so the drop is visible
    // within a 4-sec capture; in-game placement keeps the retail default.
    //
    // PE-pipeline scope: validates the single-particle-emitter shape
    // (B01 = burst, M05 = continuous 50-drop, H04 = single recurring),
    // and (first time in the harness) the chained "effect spawns
    // another effect" pattern via TRippleEffect::SpawnForTest on landing.
    [[nodiscard]] static TDripEffect* SpawnForTest(const S3DPoint& origin);

    // Drive the owned bucket + chained ripples forward by one sim tick
    // (Euler integrate + gravity + landing → spawn ripple + dead-state
    // respawn coin flip), then submit the drop bucket + each live
    // spawned ripple to the FX queue. Idempotent if the effect has no
    // bucket yet.
    void TickAndSubmitForTest(EFxDebugMode debug_mode);
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
//
// B01 generic blood spray ("Blood" / TBloodAnimator / TBloodSystem).
// FAITHFUL DIRECT PORT of the pre-release particle loop — same model the
// SPARKS port follows. The original is a 30-particle directional-spray
// ballistic system embedded inside TBloodAnimator, and its three bodies:
//
//   spawn  = TCharacter::EffectBurst "blood" branch (character.cpp:2225-2261)
//            + TCharacter::ResolveAttack decap squirts (character.cpp:231-256);
//            Init is TBloodSystem::Init (effectcomp.cpp:1182-1307).
//   update = TBloodSystem::Animate (effectcomp.cpp:1309-1370) — 3-stage
//            FLY → SPLAT → SHRINK state machine, 24 Hz sim-tick cadence.
//   render = TBloodSystem::Render (effectcomp.cpp:1415-1530) — each used
//            droplet drawn TWICE (Alpha base pass + ONE/ONE additive overlay
//            pass) with one of the 8 box sub-objects of Misc\Blood.I3D
//            per pass-set.
//
// The per-particle arrays from TBloodSystem are collapsed onto this effect
// class (same animator-state-collapse convention as F03 / H03 / M05 / X22).
// One-shot burst: Init seeds num + trail-fill droplets at once; the object
// self-destructs when every droplet expires.
//
// Retail-confirmed param divergences from the snapshot (forensics §2.1):
// for attack/impale path use hspread=32 / vspread=5 (NOT snapshot's 80/20);
// the retail ResolveAttack immediates for the live impale squirt are
// (height=40, hangle=face-128, vangle=32, hspread=32, vspread=5, num=1) per
// recon/discovered/cls_0x5a7b98_TCharacter_ResolveAttack_4c1bb0.cpp:190.
//
// Blend = TWO passes per droplet:
//   pass 0 = Alpha       (SetBlendState: SRC_ALPHA / INV_SRC_ALPHA) — the
//            solid dark-red droplet body, sub-objects {box05..box08}.
//   pass 1 = AdditiveStraight (ONE/ONE) with MODULATE texture stage — the
//            self-luminous wet sheen, sub-objects {box01..box04}.
// Scene-LIT (LitFlat) so blood darkens in shadow per forensics §7 / §10.
// TestNoWrite depth. FLY droplets are tipped ground-flat (WorldXY-ish)
// + 45° in-plane spin; SPLAT/SHRINK keep authored facing. Forensics §7.

// MAX_BLOODS particles per burst — matches retail TBloodSystem (effectcomp.cpp:374).
// Kept as a reference constant for the legacy bespoke loop preserved under
// `#if 0` in effect.cpp; the engine-particle rework uses the bucket
// `spawn_burst` value declared in effects.def instead.
inline constexpr int32_t kBloodMaxParticles = 30;

_CLASSDEF(TBloodEffect)

// B01 TBloodEffect — engine-particle rework per the resettled forensics.
// The 1998 TBloodSystem-style per-particle array + 3-stage state machine
// (FLY -> SPLAT -> SHRINK) is now modelled in effects.def as two engine
// particle buckets (`blood_fly` + `blood_splat`) with a reflection-plane
// chain wiring FLY -> SPLAT on ground impact. The bespoke C++ port is
// preserved under `#if 0` in effect.cpp for reference.
class TBloodEffect : public TEffect
{
  private:
    // Retail SetParams payload (spray descriptor) — kept for the eventual
    // in-game caller. The harness path (SpawnForTest + effects.def) reads
    // its spray params from the bucket's spawn_expr; in-game wiring will
    // need to forward these via a per-effect override pass (B01a).
    int32_t height = 0;
    int32_t hangle = 0;
    int32_t vangle = 0;
    int32_t hspread = 0;
    int32_t vspread = 0;
    int32_t num = 0;

    // Bucket cursors -- populated at SpawnForTest. Both reference engine-
    // owned global buckets that outlive this effect (the engine manages
    // particle storage; we just hold the owner_id used to scope reaping
    // when the effect is destroyed).
    TParticleBucket* fly_bucket_     = nullptr;
    TParticleBucket* splat_bucket_   = nullptr;
    float            owner_particle_id_ = -1.0f;

  public:
    TBloodEffect(TObjectImagery* newim) : TEffect(newim) {  }
    TBloodEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) { }
    ~TBloodEffect() override;

    void OffScreen() override { KillThisEffect(); }

    virtual void Initialize();
    virtual void Pulse();

    virtual void SetParams(int32_t he, int32_t ha, int32_t va, int32_t hs, int32_t vs, int32_t nu) { height = he; hangle = ha; vangle = va; hspread = hs, vspread = vs; num = nu; }
    virtual void GetParams(int32_t *he, int32_t *ha, int32_t *va, int32_t *hs, int32_t *vs, int32_t *nu) { *he = height; *ha = hangle; *va = vangle; *hs = hspread; *vs = vspread; *nu = num; }

    // Spawn a standalone single-burst TBloodEffect for the --test=vfx
    // harness. Loads the real Misc\Blood.I3D imagery and attaches a
    // TParticleEffectComponent configured with the "Blood" effects.def
    // entry, then pulses it once so the engine seeds the 10-droplet FLY
    // burst. Returns nullptr if the imagery can't be loaded. Caller owns
    // the returned pointer.
    [[nodiscard]] static TBloodEffect* SpawnForTest(const S3DPoint& origin);

    // Per-frame tick + submit for the harness. Drives the engine's
    // per-frame integration via TParticleEffectComponent::DrawPulse,
    // then submits both buckets (FLY droplets + SPLAT decals) to the
    // FX queue.
    void TickAndSubmitForTest(EFxDebugMode debug_mode);

    // True until both FLY and SPLAT buckets have drained of this effect's
    // particles. The harness uses this to know when a burst has fully
    // played out before re-triggering for clean single-instance
    // verification.
    [[nodiscard]] bool IsAlive() const;
};

// *************************************************************************
// * TBloodEffect_Bespoke — A/B reference (faithful direct C++ port)        *
// *************************************************************************
//
// Pre-engine-rework reference implementation of B01 blood: the 1998
// TBloodSystem-style 30-particle per-class state machine, three-stage
// FLY -> SPLAT -> SHRINK lifecycle, two-pass per-droplet draw (Alpha base
// + AdditiveStraight overlay) using the 8 box sub-objects of
// Misc\Blood.I3D. Lives alongside the engine port as a side-by-side A/B
// verification baseline driven by --test=vfx --vfx=TBloodEffect_BESPOKE
// (see tools/vfx/snap_ab.py for the filmstrip capture). The math /
// constants / sub-object resolution are the snapshot port preserved per
// project preserve-old-code rule.

// One blood droplet's transient state. Pre-release stored these as
// SBloodParticle (effectcomp.h:357) inside TBloodSystem; we collapse
// onto a per-particle struct on the bespoke effect class. stage is one
// of kBloodStageFly / kBloodStageSplat / kBloodStageShrink.
struct SBloodParticleEx
{
    hmm_vec3 pos   = {0.0f, 0.0f, 0.0f};   // object-local position (wu)
    hmm_vec3 vel   = {0.0f, 0.0f, 0.0f};   // velocity (wu / sim-tick)
    float    scl   = 0.0f;                 // current scale multiplier
    int32_t  size  = 0;                    // 0=small, 1=med, 2=big (collapses to small)
    int32_t  stage = 0;                    // kBloodStageFly / Splat / Shrink
    int32_t  delay = 0;                    // ticks until this particle starts integrating
    int32_t  count = 0;                    // sub-stage tick counter (SPLAT hold etc.)
    bool     used  = false;                // active slot?
};

// One sub-object's resolved draw data — captured at SpawnForTest from the
// authored UVs and htextures[] slot for that sub-object. The 8 boxes map:
//   index 0..3 = box01..box04 = pass 1 (AdditiveStraight overlay)
//   index 4..7 = box05..box08 = pass 0 (Alpha base)
// Within each set: [0]=small, [1]=med, [2]=big, [3]=splat (DEAD).
struct SBloodSubObject
{
    TTextureHandle texture    = kInvalidTexture;
    float          uv_rect[4] = {0.0f, 0.0f, 1.0f, 1.0f};   // x,y,w,h normalized
    float          size_wu    = 16.0f;                       // billboard size in wu
};

_CLASSDEF(TBloodEffect_Bespoke)

class TBloodEffect_Bespoke : public TEffect
{
  public:
    TBloodEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TBloodEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TBloodEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    // Spawn a standalone single-burst bespoke blood reference for the
    // --test=vfx harness. Loads Misc\Blood.I3D, resolves the 8 box
    // sub-objects, then runs TBloodSystem::Init (effectcomp.cpp:1182-1307)
    // verbatim against an internal 30-particle array. Returns nullptr if
    // the imagery can't be loaded. Caller owns the returned pointer.
    [[nodiscard]] static TBloodEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);

    // Per-frame tick + submit. Ports TBloodSystem::Animate
    // (effectcomp.cpp:1309-1370) + ::Render (effectcomp.cpp:1415-1483)
    // directly, integrated via a 24 Hz sim-tick accumulator for
    // framerate-independent motion. Two billboard draws per live droplet
    // (Alpha base box05..box08 then AdditiveStraight overlay box01..box04).
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);

    // True until the last droplet finishes its FLY → SPLAT → SHRINK
    // lifecycle. The harness uses this to know when a burst has fully
    // played out before re-triggering for clean single-instance
    // verification.
    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    SBloodParticleEx particles_[kBloodMaxParticles] {};
    SBloodSubObject  subobjs_[8] {};
    float            height_local_  = 0.0f;     // emit height (wu, from height param)
    bool             alive_         = true;
    double           sim_accum_ms_  = 0.0;      // 24 Hz sim-tick gate
};

// *************************************************************************
// * TFlameEffect_Bespoke — F01 A/B reference (faithful direct C++ port)    *
// *************************************************************************
//
// Line-by-line port of TFlameAnimator::Initialize / Animate / Render
// (src/effect_old.cpp:4486-4565). Single ScreenAligned billboard quad of
// `Magic\flame.i3d`'s sole `box01` sub-object; per-frame UV cell pick out
// of a 4-col x 2-row atlas using `n = frame*11/24; col = n%4; row = n/4`.
// `frame` cycles 0..17. Blend = Alpha (DECAL) per snapshot SetBlendState
// — F01 forensics §7 BLEND SANITY-CHECK flags this as "snapshot-only,
// retail-unconfirmed"; we preserve the snapshot literally per
// translation rule 3.
//
// Lives alongside the existing TFlameEffect (which uses the
// TFlipbookBillboardComponent engine path) as an A/B reference for the
// --test=vfx --vfx=TFlameEffect_BESPOKE harness entry.

_CLASSDEF(TFlameEffect_Bespoke)

class TFlameEffect_Bespoke : public TEffect
{
  public:
    TFlameEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TFlameEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TFlameEffect_Bespoke() override = default;

    // Spawn a standalone single-quad bespoke flame for --test=vfx. Loads
    // Magic\flame.i3d, resolves the single texture handle, snapshots the
    // canonical 128x160 surface size, and seeds `frame = 0`. Caller owns
    // the returned pointer.
    [[nodiscard]] static TFlameEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin,
                                                                    const char* asset_override = nullptr);

    // Per-frame tick + submit. Ports TFlameAnimator::Animate (snapshot
    // ungated, framerate-dependent) via a 24Hz sim-tick accumulator for
    // framerate-independent cadence per project memory feedback. Submits
    // ONE SubmitFxBillboard with the per-frame 0.25x0.5 UV sub-rect.
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);

    // Always alive (snapshot has no kill condition) — present for parity
    // with the bespoke family signature.
    [[nodiscard]] bool IsAlive() const { return true; }

  private:
    int32_t        frame_         = 0;        // snapshot's int32_t frame
    TTextureHandle texture_       = kInvalidTexture;
    float          quad_size_wu_  = 32.0f;    // matches sister flame port (kFireQuadSizeWu)
    double         sim_accum_ms_  = 0.0;      // 24 Hz cadence gate
};

// *************************************************************************
// * TFireEffect_Bespoke — F03 A/B reference (faithful direct C++ port)     *
// *************************************************************************
//
// Line-by-line port of the snapshot TFireAnimator (legacy
// walkcode/effect.cpp:886-991): NUMFIRES=15 scatter quads, per-quad random
// XY offset in +/-50 wu patch, per-quad startup frame in (-22..-2), each
// tick `f[c]++` and respawn at `f[c] >= 30`; per quad selects
// `framehtexs[f]` from Misc\Fire.I3D's frame array. Blend = Alpha (DECAL
// default; snapshot SetBlendState). Orientation = WorldXY per
// rot.x=-π/2; preserving the snapshot's per-quad rot.z=-π/4 (45° in-plane
// spin) via SParticleDrawItem's rotation_rad — the F03c gap noted in the
// batch brief.
//
// Lives alongside the existing TFireEffect as an A/B reference for the
// --test=vfx --vfx=TFireEffect_BESPOKE harness entry.

_CLASSDEF(TFireEffect_Bespoke)

class TFireEffect_Bespoke : public TEffect
{
  public:
    TFireEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TFireEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TFireEffect_Bespoke() override = default;

    [[nodiscard]] static TFireEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);

    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);

    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    SFireScatterQuad            quads_[kFireScatterQuads] {};
    bool                        alive_         = true;
    double                      sim_accum_ms_  = 0.0;
    TObjectImagery*             imagery_       = nullptr;
    std::vector<TTextureHandle> frame_textures_ {};
};

// *************************************************************************
// * TFireSwarmEffect_Bespoke — F05 A/B reference (faithful direct port)    *
// *************************************************************************
//
// Line-by-line port of the snapshot TFireSwarmAnimator
// (src/effect_old.cpp:10483-10549). Forensics doc:
// docs/vfx/forensics/F05_TFireSwarmEffect.md — a spinning, expanding,
// flattening single I3D cylinder mesh `tube01` (sub-object index 1) of
// `Magic\FireSwarm.i3d`. Per-tick state machine (3 floats + 1 int):
//   cylth   += 0.5  rad/tick (yaw about world Z, wrapped at 2π)
//   cylhscl += 0.4  (XY radius scale)
//   cylvscl -= 0.4  (Z height scale, from init 30.0)
// Kill when frameon > 75. Blend = Alpha (snapshot SetBlendState — F05
// forensics §7 BLEND SANITY-CHECK flags as suspect, but ARGB4444 alpha
// channel implies the snapshot literal is intentional; preserved
// verbatim per translation rule 3).
//
// First-pass NOTE: The original draws a 64-vertex I3D cylinder with a
// per-frame Scale·RotZ matrix. The Sokol FB pipeline does not yet have a
// per-effect mesh-submission path with arbitrary blend overrides, so this
// first-pass renders the cylinder as a single WorldUpAligned-equivalent
// ScreenAligned billboard using the `tube01` sub-object's authored
// texture (the 64×128 ARGB4444 flame skin). The billboard's size_wu
// tracks cylhscl × cylvscl so the visual still expands radially and
// flattens vertically as the original mesh would. Drift documented in
// the return — full mesh path is a follow-up.

_CLASSDEF(TFireSwarmEffect_Bespoke)

class TFireSwarmEffect_Bespoke : public TEffect
{
  public:
    TFireSwarmEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TFireSwarmEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TFireSwarmEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    // Spawn a single FireSwarm burst at `origin` for the --test=vfx
    // harness. Loads Magic\FireSwarm.i3d, resolves the tube01 sub-object's
    // texture slot, seeds the 3-float state from the snapshot Initialize
    // body (effect_old.cpp:10483-10490). Returns nullptr on asset failure.
    [[nodiscard]] static TFireSwarmEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin,
                                                                        const char* asset_override = nullptr);

    // Per-frame tick + submit. Ports TFireSwarmAnimator::Animate
    // (effect_old.cpp:10499-10515) verbatim through a 24 Hz sim-tick
    // accumulator (framerate-independent per project memory), then submits
    // one Alpha billboard scaled by (cylhscl, cylvscl) per
    // TFireSwarmAnimator::Render (effect_old.cpp:10524-10549).
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);

    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    // Snapshot animator fields (effect.h:1996-2034 + effect_old.cpp Initialize).
    int32_t        frameon_       = 0;
    float          cylhscl_       = 0.4f;       // FIRESWARM_CYLHSCLSTEP
    float          cylvscl_      = 30.0f;       // FIRESWARM_CYLVSCLINIT
    float          cylth_         = 0.0f;       // rad
    // Resolved asset state.
    TTextureHandle texture_       = kInvalidTexture;
    float          base_size_wu_  = 16.0f;      // per-axis world-unit base
    bool           alive_         = true;
    double         sim_accum_ms_  = 0.0;
};

// *************************************************************************
// * TBurnEffect_Bespoke — M04 A/B reference (faithful direct port)         *
// *************************************************************************
//
// Line-by-line port of the snapshot TBurnAnimator
// (src/effect_old.cpp:3292-3473). Forensics doc:
// docs/vfx/forensics/M04_TBurnEffect.md — composite particle status
// overlay attached to a burning character. Two TParticleSystem instances
// (fire + smoke), each capacity BURN_COUNT=70, both seeded from
// Magic\burnbabyburn.I3D's two sub-objects (smoke01 / smoke). Per tick:
//   1. Promote any fire particle with life == life_span -> smoke
//      (life_span = (old*2)/3, scl *= 1.25).
//   2. Animate each system (integrate pos += vel, vel *= acc).
//   3. Spawn `to_add` new fire particles at random character bones
//      with random scale [0.15,0.4], pure-upward vel [3.0,7.5], life
//      [5,15], rot=(-90,0,-45).
//   4. Decay each live particle scl *= BURN_DEC (=0.97).
// `to_add` ramps 0->8 over ticks 0-50, then 8->0 over ticks 50-58.
// Kill when frame>=BURN_FRAME (=50) and to_add==0 and all particles done.
// Blend = AdditiveStraight (snapshot SetAddBlendState, fire-family).
//
// First-pass NOTE: the harness has no live TCharacter rig wired for the
// random-bone emit, so the bone-emit path is simplified to spawn from
// the effect's origin + ±BURN_SPREAD jitter on each axis. The per-tick
// physics (velocity, life, decay, fire→smoke promotion) is preserved
// verbatim. CharacterRig integration is a follow-up — for now the
// harness entry uses preview_style=Static so the effect spawns at a
// fixed origin and runs its full ramp-up / ramp-down / drain cycle.

constexpr int32_t kBurnBespokeCount = 70;     // BURN_COUNT (effect.h:1526)

// One particle slot — collapsed from SParticleSystemInfo
// (src/effectcomp.h:357-372) into a compact bespoke record. `system`
// disambiguates fire vs smoke (the snapshot stores them in separate
// TParticleSystem arrays; we use a single flat array tagged with the
// owning system so the fire→smoke promotion is just a tag flip).
struct SBurnBespokeParticle
{
    hmm_vec3 pos        = {0.0f, 0.0f, 0.0f};
    hmm_vec3 vel        = {0.0f, 0.0f, 0.0f};
    hmm_vec3 scl        = {0.0f, 0.0f, 0.0f};
    hmm_vec3 acc        = {1.0f, 1.0f, 1.0f};
    int32_t  life       = 0;
    int32_t  life_span  = 0;
    int32_t  system     = 0;       // 0 = fire (smoke01), 1 = smoke (smoke)
    bool     used       = false;
};

_CLASSDEF(TBurnEffect_Bespoke)

class TBurnEffect_Bespoke : public TEffect
{
  public:
    TBurnEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TBurnEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TBurnEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    // Spawn a single BURN overlay at `origin` for the --test=vfx
    // harness. Loads Magic\burnbabyburn.I3D, resolves the texture slots
    // for the two sub-objects (smoke01 -> fire system, smoke -> smoke
    // system per forensics §4), seeds the empty particle array and
    // ramp envelope from the snapshot Initialize body (effect_old.cpp
    // :3292-3313). Returns nullptr on asset failure.
    [[nodiscard]] static TBurnEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);

    // Per-frame tick + submit. Ports TBurnAnimator::Animate verbatim
    // through a 24 Hz sim-tick accumulator, then submits one
    // AdditiveStraight billboard per live particle (texture switches by
    // particle.system) per the SetAddBlendState bracket
    // (effect_old.cpp:3449-3460).
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);

    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    SBurnBespokeParticle particles_[kBurnBespokeCount * 2] {};   // fire + smoke arenas (flat)
    int32_t              frame_         = 0;     // snapshot's animator-side frame counter
    int32_t              to_add_        = 0;     // per-tick spawn cap (ramps 0..8..0)
    int32_t              size_          = 1;     // ca->NumObjects() — 1 for harness (no rig)
    TTextureHandle       tex_fire_      = kInvalidTexture;       // smoke01 sub-object
    TTextureHandle       tex_smoke_     = kInvalidTexture;       // smoke sub-object
    float                base_size_wu_  = 48.0f;
    bool                 alive_         = true;
    double               sim_accum_ms_  = 0.0;
};

// *************************************************************************
// * Strip/Ribbon family bespoke first-pass A/B references                 *
// *                                                                       *
// * Four faithful direct ports of the pre-release snapshot animator       *
// * bodies for Wave 2 batch wave-bespoke-06-strip-ribbon:                 *
// *                                                                       *
// *   S04 TLightningAnimator_Bespoke  — composite lightning bolt (strip   *
// *                                     of jittered anchors).             *
// *   S05 TShockAnimator_Bespoke      — expanding shockwave ring          *
// *                                     (animator-only, EMBEDDED in I21   *
// *                                     in retail; here a self-contained  *
// *                                     test effect).                     *
// *   S07 TStreamerEffect_Bespoke     — trailing streamer/ribbon (the     *
// *                                     hemispherical 4-stream spiral).   *
// *   X11 TRibbonAnimator_Bespoke     — slow-grow ribbon disc + sparks    *
// *                                     (revive halo).                    *
// *                                                                       *
// * Each lives alongside any future engine port as an A/B baseline. The   *
// * math / constants / per-tick step are translated line-for-line from    *
// * the snapshot bodies (effect_old.cpp / effectcomp.cpp); only the       *
// * render API is adapted (RenderObject -> SubmitFxBillboard /            *
// * SubmitFxStrip). Pipelines: S04 = SR strip + FB glow; S05/S07/X11 =    *
// * FB billboard composites (no I3D asset bound yet — first-pass uses    *
// * a procedural white glow texture). See src/effect.cpp.                *
// *************************************************************************

// --- S04 TLightningAnimator_Bespoke ---------------------------------------
// Faithful port of the pre-release TLightningAnimator strip body (the
// jittered bolt; snapshot in src/stripeffect.cpp `#if 0` :822-915 +
// :534-662). Drives a TStripAnimator-shaped anchor ring expanding along
// the caster facing direction, with per-tick smoothing-jitter on every
// SMOOTH_SIZE-th anchor + cubic-shaped interpolation in between,
// SetAddBlendState (-> AdditiveStraight) for the strip render, plus a
// procedural-glow halo placeholder (no Lightning Spark.I3D asset bound
// here yet — would land alongside a real imagery load in a follow-up).
//
// One-shot lifecycle: LAUNCH(1 tick) -> FLY(STRIP_FLY_DURATION ticks of
// grow-and-fly) -> EXPLODE(STRIP_EXPLODE_TICKS ticks of tail-shrink) ->
// dead. Harness Combat cadence re-fires.
_CLASSDEF(TLightningAnimator_Bespoke)
class TLightningAnimator_Bespoke : public TEffect
{
  public:
    TLightningAnimator_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TLightningAnimator_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TLightningAnimator_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TLightningAnimator_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);
    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    // Lifecycle phases (mirror retail STRIP_LAUNCH / STRIP_FLY / STRIP_EXPLODE).
    enum { kLaunch = 0, kFly = 1, kExplode = 2 };

    // One strip anchor along the bolt — world-space position + per-tick
    // jitter offset (px, pz in retail's local frame; here applied perp to
    // the bolt's XY forward axis).
    struct SBoltAnchor
    {
        float pos[3]    = {0.0f, 0.0f, 0.0f};
        float jitter[2] = {0.0f, 0.0f};
    };

    SBoltAnchor anchors_[100] {};    // STRIP_MAX_POINTS = 100 in retail
    int32_t     numpoints_      = 0;
    int32_t     maxpoints_      = 0;
    int32_t     state_          = kLaunch;
    int32_t     duration_       = 20;       // STRIP_FLY_DURATION
    float       forward_dir_[3] = {1.0f, 0.0f, 0.0f};
    float       glow_scale_     = 3.4f;     // retail initial
    float       rotdegree_      = 0.0f;     // glow rot accumulator (degrees, mod 360)
    float       morrotdegree_   = 0.0f;     // counter-rot accumulator
    float       u_scroll_       = 0.0f;     // ScrollTexture(-0.1)/tick accumulator
    bool        alive_          = true;
    double      sim_accum_ms_   = 0.0;      // 24 Hz sim-tick gate
};

// --- S05 TShockAnimator_Bespoke -------------------------------------------
// Faithful port of TShockAnimator (effectcomp.cpp:609-770) — the expanding
// shockwave ring used as an embedded member by I21 TIceBoltAnimator. The
// retail member animates a TStripAnimator-style ring object; here we
// stand it up as a self-contained test effect spawning a "ring" of
// billboards procedurally arrayed in a circle, with the same scale
// state machine (grow → optional shrink-via-flag → done), same alpha
// fade ramp (when SHOCKWAVE_FLAG_FADE), and same per-ring colour
// (ARGB packed in the snapshot; here flattened to one ring of N
// vertices). Default flags = SHRINK | FADE (the I21 ring usage).
//
// Blend = Alpha (snapshot SetBlendState = SRC_ALPHA / INV_SRC_ALPHA).
_CLASSDEF(TShockAnimator_Bespoke)
class TShockAnimator_Bespoke : public TEffect
{
  public:
    TShockAnimator_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TShockAnimator_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TShockAnimator_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TShockAnimator_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);
    [[nodiscard]] bool IsAlive() const { return !done_; }

  private:
    enum { kGrow = 0, kShrink = 1 };
    enum {
        kFlagShrink      = 1 << 0,   // SHOCKWAVE_FLAG_SHRINK
        kFlagFade        = 1 << 1,   // SHOCKWAVE_FLAG_FADE
        kFlagStartShrink = 1 << 2,   // SHOCKWAVE_FLAG_START_SHRINK
    };

    // SShockParam mirror, retail names preserved (effectcomp.h:206-216).
    int32_t   flags_         = kFlagShrink | kFlagFade;
    hmm_vec3  pos_           = {0.0f, 0.0f, 0.0f};
    hmm_vec3  rot_           = {0.0f, 0.0f, 0.0f};
    hmm_vec3  scale_         = {0.05f, 0.05f, 0.05f};
    hmm_vec3  scale_factor_  = {1.10f, 1.10f, 1.10f};
    hmm_vec3  shrink_factor_ = {0.92f, 0.92f, 0.92f};
    hmm_vec3  max_size_      = {2.0f, 2.0f, 2.0f};
    hmm_vec3  min_size_      = {0.05f, 0.05f, 0.05f};
    hmm_vec3  init_scale_    = {0.05f, 0.05f, 0.05f};

    // Single ring of N vertices for the SR-equivalent. The retail
    // SetRingColor packs ARGB8 per ring; we store one ring (RGBA float).
    static constexpr int32_t kRingVertices = 24;
    float     ring_color_[4] = {1.0f, 1.0f, 1.0f, 1.0f};

    bool      done_          = false;
    int32_t   grow_          = kGrow;       // SHOCKWAVE_GROW (== 0)
    double    sim_accum_ms_  = 0.0;
};

// --- S07 TStreamerEffect_Bespoke ------------------------------------------
// Faithful port of TStreamerAnimator (effect_old.cpp:10258-10434) — the
// hemispherical 4-stream spiral. Each tick spawns ~STREAMER_SKIP*(4-j)
// new particles per stream (4 streams total); the angles th/h advance by
// dth/dh, and a sphere-mapped parametric position is plotted along a
// scaled radius. Particle scale ramps down via dscl. Lifetime is gated
// by STREAMER_DURATION = 100 ticks (per-stream h going past PI also
// short-circuits via frameon=DURATION+1).
//
// Blend = AdditiveStraight (snapshot SetAddBlendState in Render).
_CLASSDEF(TStreamerEffect_Bespoke)
class TStreamerEffect_Bespoke : public TEffect
{
  public:
    TStreamerEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TStreamerEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TStreamerEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TStreamerEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);
    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    // Retail constants (preserved names; values from effect_old.cpp:10252-10256).
    static constexpr int32_t kStreamerMaxParticles = 50;   // STREAMER_MAXPARTICLES
    static constexpr int32_t kStreamerMaxStreams   = 4;    // STREAMER_MAXSTREAMS (from effect.h:1960)
    static constexpr int32_t kStreamerDuration     = 100;  // STREAMER_DURATION
    static constexpr int32_t kStreamerModifier     = 40;   // STREAMER_MODIFIER
    static constexpr int32_t kStreamerModifierV    = 50;   // STREAMER_MODIFIERV
    static constexpr int32_t kStreamerSkip         = 1;    // STREAMER_SKIP

    // SStreamerParticle mirror (effect.h:1953-1958).
    struct SStreamerParticleEx
    {
        hmm_vec3 pos   = {0.0f, 0.0f, 0.0f};
        float    scl   = 0.0f;
        int32_t  count = 0;
    };

    SStreamerParticleEx stream_[kStreamerMaxStreams][kStreamerMaxParticles] {};
    float    dscl_[kStreamerMaxStreams] = {0.0f, 0.0f, 0.0f, 0.0f};
    float    scl_init_[kStreamerMaxStreams] = {0.0f, 0.0f, 0.0f, 0.0f};
    float    th_[kStreamerMaxStreams] = {0.0f, 0.0f, 0.0f, 0.0f};
    float    dth_[kStreamerMaxStreams] = {0.0f, 0.0f, 0.0f, 0.0f};
    float    h_[kStreamerMaxStreams] = {0.0f, 0.0f, 0.0f, 0.0f};
    float    dh_[kStreamerMaxStreams] = {0.0f, 0.0f, 0.0f, 0.0f};
    int32_t  frameon_   = 0;
    bool     alive_     = true;
    double   sim_accum_ms_ = 0.0;

    // Helper: pre-tick "InitStreamer(x)" snapshot — adds one particle to
    // stream x at the next parametric (th, h) point, scaled by modif and
    // stream index.
    void InitStreamerSnap(int32_t x);
    void AddStreamerSnap(int32_t num, const hmm_vec3& pos, float scl);
};

// --- X11 TRibbonAnimator_Bespoke ------------------------------------------
// Faithful port of TRibbonAnimator (effect_old.cpp:4069-4321) — the
// revive-spell ground halo: a slow-growing central spark (scaled by
// ribscale) + NUM_RIBBONS=3 outer ribbons rotating around it, plus
// NUM_RIBBON_SPARKS=30 small sparks orbiting + lifting upward. Pre-
// release used REVIVE_STARTTICKS for the grow window; we use a literal
// here (no TReviveEffect available in harness — pass through the GROW
// branch with ribscale incrementing every tick).
//
// Blend = Alpha (snapshot SetBlendState in Render).
_CLASSDEF(TRibbonAnimator_Bespoke)
class TRibbonAnimator_Bespoke : public TEffect
{
  public:
    TRibbonAnimator_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TRibbonAnimator_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TRibbonAnimator_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TRibbonAnimator_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin,
                                                                      const char* asset_override = nullptr);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);
    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    // Snapshot constants (effect.h:1001-1006). NUM_RIBBONS=3, NUM_RIBBON_SPARKS=30
    // are macros — we re-use them where in scope.
    static constexpr int32_t kRibbonGrowFrames = 24;  // REVIVE_STARTTICKS placeholder
    static constexpr int32_t kRibbonLifeTicks  = 80;  // harness one-shot duration

    hmm_vec3 p_[NUM_RIBBON_SPARKS] {};
    hmm_vec3 v_[NUM_RIBBON_SPARKS] {};
    float    scale_spark_[NUM_RIBBON_SPARKS] = {0.0f};
    int32_t  framenum_[NUM_RIBBON_SPARKS] = {0};
    hmm_vec3 ribpos_     = {0.0f, 0.0f, 0.0f};
    float    rotation_[NUM_RIBBONS] = {0.0f, 2.0f, 4.0f};
    int32_t  ribbontimer_ = 0;
    float    ribscale_    = 0.05f;   // RIBBON_MINSCALE
    float    centertilt_     = 0.0f;
    float    centertilt_dx_  = 0.001f;
    bool     alive_       = true;
    double   sim_accum_ms_ = 0.0;
};

// *************************************************************************
// * TMissileEffect_Bespoke — S08 base infrastructure (faithful direct port)*
// *************************************************************************
//
// The TMissileEffect base class is a pure 3-state LAUNCH -> FLY -> EXPLODE
// state machine: no rendering of its own (each leaf supplies its own
// animator). The same machine is shared by:
//   F07  TFireBallEffect   (Magic\NewFireBall.I3D)
//   M07  TPhotonEffect      (Magic\Photon.I3D)
//   X19  TYFireBallEffect   (Magic\YellowFireBall.I3D)
//   F09  TFireColumnEffect  (vestigial)
//   F11  TFlameDiscEffect   (vestigial)
//
// Snapshot source of truth: src/missileeffect.cpp:31-141 (the entire
// `#if 0` body), TMissileEffect::Initialize / SetSpeed / Pulse / OffScreen.
// Retail recon: cls_0x5a50e8 (base) — referenced by cls_0x5b4290 (FireBall)
// which inherits from it. See F07 forensics doc §6.1 for the per-tick
// LAUNCH/FLY/EXPLODE pseudocode.
//
// This bespoke base is the foundation that M07 (batch 5) and X19 (later
// wave) inherit from. F07 is already shipped with the state machine
// folded onto its leaf (TFireBallEffect in src/effect.h:3060+); leaving
// F07 alone per the skip list.
//
// Harness rendering: TMissileEffect has no visual on its own. To prove
// the state-machine ticks LAUNCH -> FLY -> EXPLODE, the bespoke port
// submits ONE marker billboard at the missile world-pos each frame,
// colored by state (LAUNCH=yellow, FLY=orange, EXPLODE=red). The marker
// uses the renderer's white texture so no I3D imagery is required.

// MISSILE_LAUNCH / FLY / EXPLODE — verbatim mirror of missileeffect.h:20-24.
inline constexpr int32_t kMissileLaunch  = 0;
inline constexpr int32_t kMissileFly     = 1;
inline constexpr int32_t kMissileExplode = 2;

// missileeffect.h:26-27 — snapshot defaults shared by every missile leaf.
inline constexpr int32_t kMissileSpeedDefault = 16;
inline constexpr int32_t kMissileRangeDefault = 2;

// 24 Hz sim cadence (family-consistent with B01/F07): the per-tick
// integration runs once per accumulated sim tick so motion is
// framerate-independent.
inline constexpr int32_t kMissileSimTickMs = 1000 / 24;

// Harness-only LAUNCH-hold so the visible transition is observable in
// --test=vfx (the in-game caller leaves this at 0). The animator drives
// SetStatus(true) on the very first tick in retail, so without this hold
// the LAUNCH frame is invisible. Same trick TFireBallEffect uses.
inline constexpr int32_t kMissileHarnessLaunchHoldTicks = 12;
inline constexpr float   kMissileHarnessSpeedScale      = 0.4f;

// Marker billboard size in world units (harness visualization only).
// Picked to read clearly against the test scene (~64 wu wall sprites).
inline constexpr float   kMissileMarkerSizeWu          = 24.0f;

_CLASSDEF(TMissileEffect_Bespoke)

class TMissileEffect_Bespoke : public TEffect
{
  public:
    TMissileEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) { Initialize(); }
    TMissileEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) { Initialize(); }
    ~TMissileEffect_Bespoke() override = default;

    // missileeffect.cpp:137-141 — MissileEffects stay on-screen until the
    // effect is finished (only OffScreen when EXPLODE done AND status).
    // For the harness we keep it simple: pure base path lets the engine
    // do its thing.
    void OffScreen() override
    {
        if ((state_ == kMissileExplode) && status_)
            TObjectInstance::OffScreen();
    }

    // Faithful port of missileeffect.cpp:31-40.
    virtual void Initialize();

    // Faithful port of missileeffect.cpp:52-135. Drives the 3-state
    // LAUNCH -> FLY -> EXPLODE machine. Called from TickAndSubmitForTest's
    // sim-tick gate (and in-game directly from the engine's Pulse walk).
    void Pulse() override;

    // missileeffect.cpp:42-50.
    int32_t GetSpeed() const { return speed_ / ROLLOVER; }
    void    SetSpeed(int32_t newspeed) { speed_ = newspeed * ROLLOVER; }
    int32_t GetMissileSpeed() const { return speed_; }
    void    SetStatus(bool newstatus) { status_ = newstatus; }
    int32_t GetMissileState() const { return state_; }

    // Spawn a standalone TMissileEffect_Bespoke for the --test=vfx
    // harness. No imagery required (the marker billboard uses the
    // renderer's white texture). Returns nullptr if the renderer isn't
    // initialized.
    [[nodiscard]] static TMissileEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);

    // Per-frame tick + submit. Ports TMissileEffect::Pulse via a 24 Hz
    // sim-tick accumulator, then submits ONE marker billboard at the
    // missile world-pos colored by state.
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);

    // True until the EXPLODE phase ages out (~24 ticks after EXPLODE
    // entry). Harness uses this to know when to re-fire.
    [[nodiscard]] bool IsAlive() const { return alive_; }

  protected:
    // --- TMissileEffect base-class state (missileeffect.h:33-37) -------
    int32_t state_    = kMissileLaunch;  // MISSILE_LAUNCH / FLY / EXPLODE
    int32_t range_    = 32768;           // ticks left until self-explode
    int32_t speed_    = 0;               // wu/tick * ROLLOVER
    bool    status_   = false;           // animator -> base "launch now" handshake
    int32_t aim_angle_ = 0;              // 0..255 byte-angle (snapshot `angle`)

    // Per-tick velocity in world wu/tick (set on LAUNCH->FLY transition).
    // The snapshot uses S3DPoint vel on TObjectInstance and integrates it
    // via Move(); for the harness we integrate here so the marker moves
    // without engine collision involvement.
    hmm_vec3 vel_     = {0.0f, 0.0f, 0.0f};

    // Harness-only LAUNCH-hold counter (see
    // kMissileHarnessLaunchHoldTicks). In-game this stays at 0.
    int32_t launch_hold_ticks_remaining_ = kMissileHarnessLaunchHoldTicks;

    // Post-explode lifetime so the harness shows the EXPLODE frame
    // before re-firing.
    int32_t explode_ticks_remaining_ = 24;

    // Lifecycle
    bool   alive_         = true;
    double sim_accum_ms_  = 0.0;   // 24 Hz sim-tick gate
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

// ******************
// * TFizzleEffect *
// ******************
//
// X21 spell-failure puff ("Fizzle" / TFizzleAnimator / TParticleSystem).
// FAITHFUL DIRECT PORT of the pre-release per-tick particle loop — same
// model SPARKS / BLOOD follow. The original is three small `TParticleSystem`
// instances (blue/red/purple) owned by `TFizzleAnimator`, each binding one
// of the 3 sub-objects (box01/02/03) of `Magic\Fizzle.I3D` (a 32x32
// ARGB4444 dust sprite); each tick the animator spawns ~1.5 particles for
// the first 15 ticks, runs a grow→shrink scale state machine + in-plane
// spin, and self-destructs when emission is done AND every particle's
// scale cycle has finished. See forensics docs/vfx/forensics/X21_TFizzleEffect.md.
//
// Three bodies (snapshot, retail animator body not extracted — §2.1):
//
//   spawn  = TPlayer::InvokeQuickSpell -> CastByName("Fizzle")
//            (player.cpp:547-574; spell.def:253-263)
//   update = TFizzleAnimator::Animate (effect_old.cpp:12361-12487) — owns
//            the cadence, scale state machine (life_span = 100 -> 200 -> 0
//            phase tag), spin, flicker re-roll, self-kill.
//   render = TFizzleAnimator::Render (effect_old.cpp:12489-12502) +
//            TParticleSystem::Render (effectcomp.cpp:1070-1109) — Alpha
//            blend, ground-tipped WorldXY, per-particle Z-axis spin,
//            +1.5x flicker scale boost.
//
// The per-particle arrays from the 3 TParticleSystems are collapsed onto
// this effect class (same convention as SSparkParticle / SBloodParticleEx).
// One-shot fixed-duration burst, emission window = 15 sim-ticks.
//
// Retail fidelity: registration + trigger + asset visual payload are
// retail-confirmed (forensics §2.1(2)(3)(4)); the per-tick DUST_* constants
// and the animator body are snapshot-only (§2.1(1)). The shipped fizzle.i3d
// has identical decoded texture pixels and identical 3-subobj geometry.
//
// Caveats (forensics §13.2 / §13.3):
//   - life_span is OVERLOADED as a phase tag (100=GROW, 200=SHRINK, 0=DEAD),
//     NOT a tick-countdown — the real lifetime is the grow+shrink scale
//     cycle ((temp.z/0.05) + (temp.z/0.02) ticks per particle).
//   - The original spawn has a dead `rot.z = random(0,359)` immediately
//     overwritten to 0; do NOT port the dead line.
//
// Blend = Alpha (SRC_ALPHA / INV_SRC_ALPHA), Unlit, TestNoWrite,
// orientation = WorldXY (lies flat on ground, RotateX(-pi/2) tip).

// Per-system cap = DUST_COUNT (effect_old.cpp:12293). Emission caps at
// ~1.5/tick × 15 ticks ≈ 22-23 total split across the 3 systems, so DUST_COUNT
// is never reached in practice — but the pre-release allocated this many
// slots per system, so we hold the same capacity per system for fidelity.
inline constexpr int32_t kFizzleParticlesPerSystem = 30;

// Three systems (blue/red/purple) × DUST_COUNT each. Particles join a
// random system at spawn (random(1,3)). We collapse onto a single
// fixed-cap inline array — no heap churn per burst.
inline constexpr int32_t kFizzleNumSystems     = 3;
inline constexpr int32_t kFizzleMaxParticles   =
    kFizzleParticlesPerSystem * kFizzleNumSystems;

// One fizzle particle's transient state. Pre-release stored these as
// SParticleSystemInfo (effectcomp.h:308-325) inside the per-system arrays;
// we collapse onto a per-particle struct on the effect class. system_idx
// records which of the 3 systems (= which sub-object texture) it draws from.
// The 24Hz cadence (DUST_*) integrates via the same sim-tick accumulator
// the sparks/blood ports use — converted to per-second wall-clock via
// TickAndSubmitForTest's accumulator, so motion stays framerate-independent.
struct SFizzleParticle
{
    hmm_vec3 pos       = {0.0f, 0.0f, 0.0f};   // object-local position (wu)
    hmm_vec3 vel       = {0.0f, 0.0f, 0.0f};   // velocity (wu / sim-tick)
    hmm_vec3 scl       = {0.0f, 0.0f, 0.0f};   // current scale (grow/shrink)
    float    rot_z_deg = 0.0f;                 // in-plane spin angle (degrees)
    float    spin_rate = 0.0f;                 // deg / sim-tick (temp.y)
    float    max_scl   = 0.0f;                 // peak scale 0.05..0.25 (temp.z)
    int32_t  phase     = 0;                    // 100 GROW, 200 SHRINK, 0 DEAD
    int32_t  system    = 0;                    // 0=blue, 1=red, 2=purple
    bool     used      = false;                // active slot?
    bool     flicker   = false;                // re-rolled each tick (×1.5 scale)
};

// One sub-object's resolved draw data — captured at SpawnForTest from the
// authored UVs of the box's verts and the htextures[] slot the box uses.
// The 3 systems map to box01/box02/box03 (forensics §4). Mirrors the
// SBloodSubObject pattern (effect.h:2573-2578).
struct SFizzleSubObject
{
    TTextureHandle texture    = kInvalidTexture;
    float          uv_rect[4] = {0.0f, 0.0f, 1.0f, 1.0f};  // x,y,w,h normalized
    float          size_wu    = 16.0f;                      // billboard size in wu
};

_CLASSDEF(TFizzleEffect)

class TFizzleEffect : public TEffect
{
  public:
    TFizzleEffect(TObjectImagery* newim) : TEffect(newim) {}
    TFizzleEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TFizzleEffect() override = default;

    // Spawn a standalone single-burst TFizzleEffect for the --test=vfx
    // harness. Loads the real Magic\Fizzle.I3D imagery (NO procedural
    // stand-in — the 3 colored dust sprites ARE the visual identity per
    // forensics §10), resolves each of the 3 box sub-objects (box01/blue,
    // box02/purple, box03/red — note the index↔label crossing from
    // effect_old.cpp:12354-12356) to (texture, UV sub-rect, size). Resets
    // the per-burst counters; particles are seeded by the per-tick
    // emission loop inside TickAndSubmitForTest (not at spawn — the
    // original emits over the first 15 ticks, not all at once). Returns
    // nullptr if the imagery can't be loaded. Caller owns the pointer.
    [[nodiscard]] static TFizzleEffect* SpawnForTest(const S3DPoint& origin);

    // Per-frame tick + submit for the harness. Ports
    // TFizzleAnimator::Animate (effect_old.cpp:12361-12487) +
    // TParticleSystem::Animate/Render (effectcomp.cpp:1041-1109) directly,
    // converted to framerate-independent integration via the 24 Hz sim-tick
    // accumulator (same pattern as sparks/blood). FB pipeline — but uses
    // SubmitFxParticle (not SubmitFxBillboard) so each particle's
    // in-plane spin (rot.z) is honored by the per-particle rotation_rad
    // attribute on the WorldXY-oriented quad.
    void TickAndSubmitForTest(EFxDebugMode debug_mode);

    // True until the last particle's scale cycle has finished (mirrors
    // TFizzleAnimator self-destruct via KillThisEffect when frame_count
    // >= 15 AND every used particle is done — effect_old.cpp:12482-12485).
    // The harness uses this to gate non-overlapping re-fires.
    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    // Per-particle state (faithful collapse of the 3 TParticleSystem
    // arrays onto the class — same pattern as SSparkParticle in sparks,
    // SBloodParticleEx in blood).
    SFizzleParticle particles_[kFizzleMaxParticles] {};
    // Resolved sub-object textures + UV sub-rects. Indexed by particle's
    // `system` field (0=blue/box01, 1=red/box03, 2=purple/box02).
    SFizzleSubObject subobjs_[kFizzleNumSystems] {};

    // Emission cadence — directly mirrors `add`/`frame_count` in
    // TFizzleAnimator (effect_old.cpp:12313-12314).
    float   emit_add_       = 0.0f;
    int32_t frame_count_    = 0;
    // 24Hz sim-tick accumulator (ms) — same as sparks/blood.
    double  sim_accum_ms_   = 0.0;
    bool    alive_          = true;};

// ********************
// * TFireBallEffect *
// ********************
//
// F07 fireball — TMissileEffect : TEffect base + TFireBallEffect leaf.
// FAITHFUL DIRECT PORT of the pre-release `TMissileEffect::Pulse`
// (src/missileeffect.cpp:52-135) + `TFireBallAnimator::Animate` /
// `::Render` body (src/missileeffect.cpp:549-1085). Doc:
// docs/vfx/forensics/F07_TFireBallEffect.md.
//
// Visual shape: 3-state machine LAUNCH→FLY→EXPLODE.
//   LAUNCH (~25 ticks): ball grows above caster, animator hands off
//                       to TMissileEffect::Pulse with SetStatus(true).
//   FLY:                ball travels along aim, grows to MAX_SIZE,
//                       sheds a 10-slot mesh-trail ring buffer + 30
//                       photon-spark particles per tick.
//   EXPLODE:            single-tick blast (`firsttime`) spawns 10
//                       burst quads + 4-ring shockwave + 40-spark
//                       burst; subsequent ticks shrink the burst and
//                       fade the ring; effect self-destructs when all
//                       sub-systems are quiet.
//
// Asset: real `Magic\NewFireBall.I3D` (forensics §4). 3 sub-objects:
//   box01      (idx 0) — fire-ball quad, reused for ball/glow/trail/burst
//                        via 4×4 atlas UV cell selection (`SetAnimFrame`).
//   box02      (idx 1) — spark quad handed to the spark trail.
//   cylinder01 (idx 2) — shockwave ring geometry, drawn as helper-mesh.
//
// Pipeline split (forensics §7):
//   - box01 (ball/glow/trail/burst) + box02 (sparks) → FB-pipeline
//     billboards (`SubmitFxBillboard`) with per-instance UV sub-rect for
//     atlas cell selection. Engine-native ScreenAligned approximates the
//     original's −30°/+60°-tilt+facing-spin orientation (§13.6).
//   - cylinder01 (ring) → helper-mesh (`SubmitHelperMesh`, additive),
//     same primitive M09b TTeleporterEffect uses.
//
// Per-instance per-frame state (animator class collapse, same convention
// as SParkS/Blood/Teleporter): the `FireBallData` head + 10-slot trail +
// 10-slot burst + spark-trail array all live on this class.

// 24 Hz sim cadence — the per-tick rates (FIREBALL_GROW_RATE 0.05/tick,
// trail shrink 0.85/slot/tick, ring scale 1.085/tick, burst shrink 0.90/
// tick, spark gravity 0.37/tick²) are integrated once per accumulated
// sim tick (family-consistent with F03/H03/M05/X22/B01).
inline constexpr int32_t kFireBallSimTickMs = 1000 / 24;

// Forensics §3 constants — kept side-by-side with the doc's spelling.
inline constexpr float   kFireBallSpeed         = 8.0f;            // pos-units/tick (pre-ROLLOVER), §3 FIREBALL_SPEED
inline constexpr float   kFireBallSpawnLiftZ    = 50.0f;           // ball spawned +50 wu above caster
inline constexpr float   kFireBallGrowRate      = 0.05f;           // FLY/EXPLODE scale step / tick
inline constexpr float   kFireBallMaxSize       = 0.60f;           // scale cap
inline constexpr float   kFireBallTrailScale    = 0.85f;           // per-slot trail shrink
inline constexpr int32_t kFireBallTrailSize     = 10;              // ring-buffer length
inline constexpr int32_t kFireBallMaxFrame      = 16;              // 4x4 atlas frame count
inline constexpr int32_t kFireBallGlowFrame     = 3;               // reserved glow cell
inline constexpr int32_t kFireBallMaxSpark      = 40;              // spark trail cap
inline constexpr int32_t kFireBallNormSpark     = 30;              // FLY spark target
inline constexpr int32_t kFireBallMaxBurst      = 10;              // impact burst quad count
inline constexpr float   kFireBallSparkScale    = 0.15f;           // initial spark scale
inline constexpr float   kFireBallSparkScaleDec = 0.90f;           // spark scale decay / tick
inline constexpr float   kFireBallSparkGravity  = 0.37f;           // wu / tick²
inline constexpr int32_t kFireBallSparkMinLife  = 15;              // spark life range (ticks)
inline constexpr int32_t kFireBallSparkMaxLife  = 20;
inline constexpr float   kFireBallSparkFlicker  = 1.75f;           // flicker scale boost
inline constexpr int32_t kFireBallBlastRadius   = 150;             // wu (BlastCharactersInRange)
inline constexpr float   kFireBallRingScale     = 25.0f;           // ring init scale (all axes)
inline constexpr float   kFireBallRingFactor    = 1.085f;          // ring grow factor / tick
inline constexpr float   kFireBallRingMaxSize   = 120.0f;          // ring scale cap
inline constexpr int32_t kFireBallRingRings     = 4;               // shockwave ring count
inline constexpr int32_t kFireBallRingVerts     = 24;              // ring vertex count
inline constexpr int32_t kFireBallFlyRangeTicks = 60;              // (240*MISSILE_RANGE)/FIREBALL_SPEED
                                                                    // = (240*2)/8 = 60 ticks of flight
// Harness adaptation only: the original animator transitions LAUNCH→FLY
// on the very first tick after SetStatus(true) (~1-tick LAUNCH phase),
// so the visible "grow above caster" moment is just one frame and the
// state-machine semantics aren't observable in --test=vfx. We extend
// LAUNCH to ~16 ticks (~0.67 s) so the user can see the LAUNCH→FLY edge.
// This does NOT change the in-game cadence — gameflow's caller skips
// the harness gate and uses the original 1-tick transition. The
// kFireBallSpeedScale below halves the FLY velocity for the harness so
// the ball stays in the camera view envelope (~240 wu radius) instead
// of zipping off-screen in ~1.2 s.
inline constexpr int32_t kFireBallHarnessLaunchHoldTicks = 16;
inline constexpr float   kFireBallHarnessSpeedScale      = 0.4f;
// Per-burst billboard size baseline. The original's `obj->scl` multiplies
// the box01 quad's authored geometry; in the FB-pipeline world-space the
// `size_wu` field is the on-screen ratio. 192 wu × 0.6 max scale = 115 wu
// at peak, which reads as a "moderate-size fireball" against the dungeon's
// ~64 wu wall sprites — tunable per §3 snapshot-only.
inline constexpr float   kFireBallBaseQuadWu    = 192.0f;
// Spark quad world-unit size — small enough that the 30-40 sparks
// visibly separate against the moving ball, large enough to read as a
// flicker. The original draws box02 at the I3D's authored scale; this is
// the engine equivalent.
inline constexpr float   kFireBallSparkQuadWu   = 6.0f;
// Spell-carried point-light parameters (forensics §9, spell.def LIGHT
// directive). Color (255,130,0) is RETAIL-CONFIRMED; intensity / radius
// are tuned to match the warm flicker on the scene without saturating.
inline constexpr float   kFireBallLightR        = 1.0f;       // 255/255
inline constexpr float   kFireBallLightG        = 0.51f;      // 130/255
inline constexpr float   kFireBallLightB        = 0.0f;
inline constexpr float   kFireBallLightRadiusWu = 320.0f;     // MULT 20 family default
inline constexpr float   kFireBallLightInt      = 1.5f;       // baseline; multiplied by glow flicker

// One slot in the head trail / burst ring buffer. Mirrors `FireBallData`
// (missileeffect.h:179-187). `rotation` is the per-instance in-plane
// spin (degrees) the snapshot accumulates on the ball head (+2/tick) and
// PROPAGATES into trail copies + burst quads via `trail[0] = fireball`
// / `burst[i].rotation = 0`. The per-trail-slot stale-snapshot of
// rotation is what gives the trail its tumbling/streak look — every
// card freezes at whatever rotation the ball had when that slot got
// recorded. Drawn through the FB-particle pipeline (SubmitFxParticle)
// with WorldXY orientation + per-instance rotation_rad so the same
// world-space matrix tilt the original used (rotZ(spin) * rotX(-30°) *
// rotY(+60°) * rotZ(facing)) lands as a ground-tipped tumble trail,
// not a camera-facing swirl.
struct SFireBallData
{
    hmm_vec3 pos      = {0.0f, 0.0f, 0.0f};   // world-space (trail) or local (head)
    float    scale    = 0.0f;                  // draw scale
    float    glow     = 1.0f;                  // flicker multiplier (×1.0..1.75)
    float    frame    = 0.0f;                  // atlas cell index
    float    rotation = 0.0f;                  // in-plane spin (degrees, +2/tick on head)
    bool     used     = false;                 // burst slot active?
};

// One spark in the photon-spark trail (collapses TSubParticleAnimator's
// per-particle arrays onto the effect class — same convention as
// SSparkParticle / SBloodParticleEx).
struct SFireBallSpark
{
    hmm_vec3 pos   = {0.0f, 0.0f, 0.0f};   // world-space
    hmm_vec3 vel   = {0.0f, 0.0f, 0.0f};   // wu / tick
    float    scale = 0.0f;                  // current scale (×0.90/tick decay)
    int32_t  life  = 0;                     // remaining ticks
    bool     used  = false;
};

_CLASSDEF(TFireBallEffect)

// Asset binding state captured at SpawnForTest. Held as a private member
// rather than a side-table because the F07 port is single-file (no
// imagery-API leakage in the header is preserved by keeping the type
// internal; the file that defines TFireBallEffect::SpawnForTest owns
// the binding semantics).
struct SFireBallAssetBind
{
    TTextureHandle box01_tex     = 0;     // ball / glow / trail / burst sprite atlas
    TTextureHandle box02_tex     = 0;     // spark quad sprite
    // The cylinder01 ring is drawn via SubmitHelperMesh — its MeshHandle
    // is kept here so the effect doesn't need to re-extract per draw.
    MeshHandle     ring_mesh     = 0;
    float          ring_mat[16]  = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
    // Authoring scale of the box01 quad (atlas-cell size in normalized
    // texture coords). The original uses a 4×4 grid (0.25 × 0.25).
    float          atlas_cell_w  = 0.25f;
    float          atlas_cell_h  = 0.25f;
};

class TFireBallEffect : public TEffect
{
  public:
    TFireBallEffect(TObjectImagery* newim) : TEffect(newim) {}
    TFireBallEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TFireBallEffect() override = default;

    void OffScreen() override { /* MissileEffect-family stays simulating until EXPLODE done */ }

    // No `override` — TEffect doesn't declare Initialize() as virtual
    // (the family pattern is to define Initialize() on each leaf as a
    // plain virtual). Pulse IS virtual on TEffect, so override there.
    virtual void Initialize();
    void Pulse() override;

    // Spawn a standalone TFireBallEffect for the --test=vfx harness.
    // Loads `Magic\NewFireBall.I3D`, resolves the 3 sub-object textures +
    // the cylinder01 ring mesh, picks a random horizontal aim direction
    // (the harness has no caster/target geometry — gameflow's in-game
    // caller will supply the actual aim via TSpell::Timer).
    // Returns nullptr if the imagery can't be loaded.
    [[nodiscard]] static TFireBallEffect* SpawnForTest(const S3DPoint& origin);

    // Per-frame tick + submit. Ports the 3-state Pulse + the animator's
    // Animate/Render bodies directly to the FB pipeline (box01/box02
    // billboards) + helper-mesh additive pass (cylinder01 ring). Drives
    // the spell light via Renderer->AddPointLight each tick while alive.
    //
    // The mesh draws (cylinder01 ring) must be submitted AFTER the
    // tile-pass opens (same convention as TTeleporterEffect::
    // SubmitWorldForTest). The harness wires `submit` for the
    // billboard+tick path and `submit_world` for the ring.
    void TickAndSubmit(EFxDebugMode debug_mode);
    void SubmitWorldRing(EFxDebugMode debug_mode);

    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    // --- TMissileEffect base-class state (forensics §6.1) -----------
    // The original carries these on TMissileEffect; we fold them onto the
    // leaf for the single-file port. The semantics are unchanged: 3-state
    // machine, byte-angle aim, integer-tick range countdown.
    int32_t state_      = 0;            // MISSILE_LAUNCH=0 / FLY=1 / EXPLODE=2
    int32_t range_      = 32768;        // ticks left until self-explode
    bool    status_     = false;        // animator → base "launch now" handshake
    int32_t aim_angle_  = 0;            // 0..255 byte-angle (horizontal facing)
    // Harness-only LAUNCH-hold counter (see kFireBallHarnessLaunchHoldTicks).
    // In-game callers leave this at 0 — gameflow's port will set
    // launch_hold_=0 so the transition matches retail's 1-tick LAUNCH.
    int32_t launch_hold_ticks_remaining_ = kFireBallHarnessLaunchHoldTicks;

    // Per-tick velocity in world wu/tick (set on LAUNCH→FLY transition).
    hmm_vec3 vel_       = {0.0f, 0.0f, 0.0f};

    // --- TFireBallAnimator state (forensics §6.2) -------------------
    SFireBallData  fireball_;                                     // ball head
    SFireBallData  trail_[kFireBallTrailSize];                    // 10-slot mesh-trail ring buffer
    SFireBallData  burst_[kFireBallMaxBurst];                     // 10-slot impact burst
    SFireBallSpark sparks_[kFireBallMaxSpark];                    // 40-slot photon-spark trail
    int32_t        frame_count_  = kFireBallMaxFrame;             // atlas total frames
    int32_t        glow_frame_   = kFireBallGlowFrame;            // reserved glow cell
    int32_t        old_state_    = 0;                              // state-edge tracker
    int32_t        firsttime_    = 0;                              // edge for explode-once
    int32_t        explode_      = 0;                              // 0 pre / 1 first-tick / -1 after

    // --- Ring kinematics (forensics §6.3) ---------------------------
    bool           ring_active_  = false;
    float          ring_scale_   = kFireBallRingScale;
    bool           ring_done_    = true;
    hmm_vec3       ring_pos_     = {0.0f, 0.0f, 0.0f};

    // --- Asset binding (resolved at SpawnForTest) --------------------
    SFireBallAssetBind asset_;

    // --- Lifecycle -----------------------------------------------------
    bool           alive_        = true;
    double         sim_accum_ms_ = 0.0;

    // Internal helpers (translate the body line-by-line; private so they
    // can mutate state without polluting the public API).
    void StepMissilePulse();      // TMissileEffect::Pulse (forensics §6.1)
    void StepAnimate();           // TFireBallAnimator::Animate (forensics §6.2)
    void SubmitBillboards(EFxDebugMode debug_mode) const;
    [[nodiscard]] bool IsTrailDraining() const;
    [[nodiscard]] bool AnyBurstAlive() const;
    [[nodiscard]] int32_t LiveSparkCount() const;
    void FillAtlasUv(int32_t frame_idx, float out_uv[4]) const;
};

// *****************
// * TMistEffect *
// *****************

_CLASSDEF(TMistEffect)

class TMistEffect : public TEffect
{
  private:
    // --- Phase 2 (M05) PE-pipeline scaffold -----------------------------
    // Bucket borrowed from the global TParticleManager — same model as
    // B01 TBloodEffect. The bucket itself outlives this effect; per-
    // instance drops are disambiguated by `owner_particle_id_` and
    // killed off in the destructor via TParticleBucket::KillParticlesByOwner.
    //
    // Unlike B01 (one-shot 10-droplet burst), Mist is a **continuous
    // ambient emitter**: the 50 drops are seeded once at SpawnForTest
    // and then respawn-in-place on death. The bucket sees no churn at
    // the count level — same 50 particles tick forever, just with
    // recycled state. See INVENTORY M05 forensics §3 for the cadence.
    TParticleBucket* bucket_           = nullptr;
    float            owner_particle_id_ = -1.0f;
    double           sim_accum_ms_     = 0.0;   // 24 Hz sim-tick gate (forensics §7.4)

  public:
    TMistEffect(TObjectImagery* newim) : TEffect(newim) {  }
    TMistEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) { }
    ~TMistEffect() override;

    void OffScreen() override { KillThisEffect(); }

    virtual void Initialize();
    void Pulse() override;

    // Spawn a standalone TMistEffect for the --test=vfx harness. Loads
    // `Magic\mist.i3d` (the canonical mist sprite at
    // legacy/Imagery/Magic/mist.i3d), allocates / reuses a global PE
    // bucket keyed off the mist texture, seeds 50 drops with retail-
    // faithful pos + upward velocity envelope, and stamps the instance
    // with a fresh map index so its particles can be tracked by owner.
    // Returns nullptr if the imagery can't be loaded. The caller owns
    // the returned pointer and must `delete` it to release the imagery
    // refcount and evict its particles.
    //
    // PE-pipeline scope: validates the bucket/submit path end-to-end
    // through the real effect class lineage with a long-lived
    // continuous-emitter pattern (B01 = burst, M05 = continuous).
    [[nodiscard]] static TMistEffect* SpawnForTest(const S3DPoint& origin);

    // Drive the owned bucket forward by one sim tick (Euler integrate
    // + gravity + respawn-in-place on landing), then submit to the FX
    // queue. Idempotent if the effect has no bucket yet.
    void TickAndSubmitForTest(EFxDebugMode debug_mode);
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

// *************************************************************************
// * TWaterFallEffect_Bespoke — H02 first-pass faithful port (snapshot)     *
// *************************************************************************
//
// Direct C++ port of TWaterFallAnimator::Initialize / UpdateStuff /
// Render (src/effect_old.cpp:11701-11860). Same constants, same per-tick
// math, same render-pass order. Only render API call changed: instead of
// RenderObject(D3D MATRIX) we submit one screen-aligned billboard per
// drop via TRenderer::SubmitFxBillboard. The snapshot Render() writes
// SRCBLEND/DESTBLEND=ONE/ONE (additive) with the SetAddBlendState comment
// preserved — we emit AdditiveStraight verbatim.
//
// First-pass bespoke (compiles + boots + renders something) per
// wave-bespoke-07-water orchestrator brief. No video A/B validation yet.

_CLASSDEF(TWaterFallEffect_Bespoke)

class TWaterFallEffect_Bespoke : public TEffect
{
  public:
    TWaterFallEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TWaterFallEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TWaterFallEffect_Bespoke() override;

    void OffScreen() override { KillThisEffect(); }

    // Spawn a standalone TWaterFallEffect_Bespoke for the --test=vfx
    // harness. Loads Misc\Water.I3D, resolves a billboard texture, seeds
    // WATERFALL_MAXDROPS drops and warms the simulator the same way
    // TWaterFallAnimator::Initialize does. Returns nullptr if the
    // imagery can't be loaded; caller owns the returned pointer.
    [[nodiscard]] static TWaterFallEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin,
                                                                        const char* asset_override = nullptr);

    // Per-frame tick + submit. Ports UpdateStuff() (per sim-tick) and
    // Render() (per frame) verbatim. Animator was framerate-locked in
    // the snapshot — gated here via a 24 Hz sim-tick accumulator for
    // framerate-independent motion. Emits one AdditiveStraight billboard
    // per live drop.
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);

    [[nodiscard]] bool IsAlive() const { return true; }   // persistent

  private:
    void InitParticle(int32_t i);
    void UpdateStuff();

    SWaterParticle* drops_     = nullptr;
    int32_t         numdrops_  = 0;
    TTextureHandle  texture_   = kInvalidTexture;
    float           uv_rect_[4] = {0.0f, 0.0f, 1.0f, 1.0f};
    float           size_wu_   = 16.0f;
    double          sim_accum_ms_ = 0.0;
};

// *************************************************************************
// * TWaterEffect_Bespoke — H01 first-pass faithful port (snapshot)         *
// *************************************************************************
//
// Direct C++ port of TWaterAnimator::Initialize / UpdateStuff / Render
// (src/effect_old.cpp:11911-12062). Ambient horizontal-stream water spray.
// Particles travel along +X at WATER_SPEED * ((i%2)+1) until they cross
// WATER_LENGTH/2 then respawn. Same constants, same per-tick math, same
// render order. Snapshot Render() writes SRCBLEND/DESTBLEND=ONE/ONE
// (additive) — emit AdditiveStraight verbatim.

_CLASSDEF(TWaterEffect_Bespoke)

class TWaterEffect_Bespoke : public TEffect
{
  public:
    TWaterEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TWaterEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TWaterEffect_Bespoke() override;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TWaterEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin,
                                                                    const char* asset_override = nullptr);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);
    [[nodiscard]] bool IsAlive() const { return true; }   // persistent

  private:
    void InitParticle(int32_t i);
    void UpdateStuff();

    SWaterParticle* drops_     = nullptr;
    int32_t         numdrops_  = 0;
    TTextureHandle  texture_   = kInvalidTexture;
    float           uv_rect_[4] = {0.0f, 0.0f, 1.0f, 1.0f};
    float           size_wu_   = 16.0f;
    double          sim_accum_ms_ = 0.0;
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

// *************************************************************************
// * Ice family bespoke ports (wave-bespoke-03-ice)                         *
// *************************************************************************
//
// Three sister effects in the player's frost-spell chain:
//   I20 TIceEffect      — shard-burst impact effect (NO ported body —
//                         needs forensics doc first; STUB returns nullptr)
//   I21 TIceBoltEffect  — stationary caster->target freeze beam, ~5 s
//                         lifetime, composite Magic\icebolt.I3D draw +
//                         50 frost + 50 snow billboards. Posts I22.
//   I22 TIcedEffect     — 10 s frozen-victim overlay, then 30-chunk
//                         shatter. Posted by I21.
//
// All three follow the TBloodEffect_Bespoke template: T<Class>_Bespoke
// subclass of TEffect, SpawnForTest_BESPOKE static factory, and
// TickAndSubmitForTest_BESPOKE per-frame driver. Forensics:
//   docs/vfx/forensics/I21_TIceBoltEffect.md
//   docs/vfx/forensics/I22_TIcedEffect.md
// Snapshot bodies: src/effect_old.cpp:8015-8688 (I21), :8749-8953 (I22).

// --- I20 TIceEffect bespoke stub --------------------------------------
// I20 has only header declarations (effect.h:607-615). No Animate/Render
// body exists in src/effect_old.cpp or src/effectcomp.cpp — the snapshot
// shipped just TIceEffect / TIceAnimator decls + a constructor-only
// `flags |= OF_MOVING; pos += ICEDIST` ctor (no Pulse body, no animator
// body). The brief flags this as "needs forensics doc first — DO NOT
// port code blind". SpawnForTest returns nullptr + log_warn; harness
// renders nothing for this id. Re-enable once a forensics doc exists.
_CLASSDEF(TIceEffect_Bespoke)

class TIceEffect_Bespoke : public TEffect
{
  public:
    TIceEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TIceEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TIceEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    // BLOCKED: returns nullptr — no snapshot body and no forensics doc.
    [[nodiscard]] static TIceEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);
    [[nodiscard]] bool IsAlive() const { return false; }
};

// --- I21 TIceBoltEffect bespoke ---------------------------------------
// FAITHFUL DIRECT PORT of TIceBoltAnimator::Initialize/Animate/Render
// (src/effect_old.cpp:8015-8688). Per forensics §3 constants and §6
// state machine. Stationary freeze beam (NOT a missile — base is
// TEffect; the TMissileEffect::Initialize/Pulse calls in the snapshot
// are commented out). Composite draw: 4 nested core cylinders
// (cyl04/01/02/03), counter-rotating spirals (cyl06), sliding rings
// (cyl05), 2 end glow spheres (box01), 50 frost particles (box01), 50
// snow particles (box01). Blend = Alpha, Lit-mode = Unlit (asset color
// carries blue), Depth = TestNoWrite. spell.def LIGHT(100,100,255) is
// retail-only — pre-release parser discards it; no light added here.
inline constexpr int32_t kIceBoltMaxFrost = 50;   // MAX_FROST_PARTICLES (effect.h:1651)
inline constexpr int32_t kIceBoltMaxSnow  = 50;   // MAX_SNOW_PARTICLES  (effect.h:1652)

// 24 Hz sim-tick gate (family pattern: blood/spark/fireball/iced).
inline constexpr int32_t kIceBoltSimTickMs = 1000 / 24;

// One frost particle (snapshot: p[],v[],s[],t[] member arrays in the
// animator). Re-flattened onto a per-particle struct on the effect class
// — mirrors SBloodParticleEx convention.
struct SIceBoltFrostParticle
{
    hmm_vec3 pos = {0.0f, 0.0f, 0.0f};   // p[i] (wu, object-local)
    hmm_vec3 vel = {0.0f, 0.0f, 0.0f};   // v[i] (wu/tick)
    float    s   = 0.0f;                  // size (FROST_INIT_SIZE → 0)
    float    t   = 0.0f;                  // spawn delay (ticks)
};

// One snow particle (snapshot: h[],th[],r[],rs[],sz[] member arrays).
struct SIceBoltSnowParticle
{
    float h  = 0.0f;   // height (z, ±50 wu)
    float th = 0.0f;   // orbit angle (rad)
    float r  = 0.0f;   // orbit radius (wu)
    float rs = 0.0f;   // angular velocity (rad/tick)
    float sz = 0.0f;   // particle scale
};

_CLASSDEF(TIceBoltEffect_Bespoke)

class TIceBoltEffect_Bespoke : public TEffect
{
  public:
    TIceBoltEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TIceBoltEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TIceBoltEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TIceBoltEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);
    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    // Per-particle arrays (50+50 = 100 quads max), forensics §3.
    SIceBoltFrostParticle frost_[kIceBoltMaxFrost] {};
    SIceBoltSnowParticle  snow_[kIceBoltMaxSnow] {};

    // Beam-body transform animation envelopes (snapshot member fields:
    // length, spherescale[2], cylscale, ringout, spiralang, spiralscale, cy).
    // Same names as snapshot for line-by-line fidelity.
    float length_       = 0.0f;          // beam length (wu); harness-fixed
    float spherescale_[2] = {0.0f, 0.0f};// glow-sphere scales (snapshot spherescale[0/1])
    float cylscale_     = 0.0f;          // core-cylinder x/y scale (snapshot cylscale)
    float ringout_      = 0.0f;          // ring slide offset (snapshot ringout)
    float spiralang_    = 0.0f;          // spiral rotation angle (rad)
    float spiralscale_  = 0.0f;          // spiral xy scale
    float cy_           = 0.0f;          // snow cluster axial sweep
    int32_t angle_      = 0;             // byte facing (0..255) toward target
    int32_t frameon_    = 0;             // simulation tick counter
    int32_t subspell_   = 3;             // ::SetSubSpell(3) force (snapshot :8049)

    // Resolved texture handles for the 7 sub-objects of icebolt.I3D.
    // §4 mapping: [0..3] = cylinder04/01/02/03 (core beam),
    //             [4]    = box01 (glow sphere / frost / snow billboard),
    //             [5]    = cylinder05 (rings),
    //             [6]    = cylinder06 (spirals).
    TTextureHandle subobj_tex_[7] {kInvalidTexture, kInvalidTexture, kInvalidTexture,
                                   kInvalidTexture, kInvalidTexture, kInvalidTexture,
                                   kInvalidTexture};
    float          subobj_uv_[7][4] {};

    bool   alive_         = true;
    double sim_accum_ms_  = 0.0;
};

// --- I22 TIcedEffect bespoke -------------------------------------------
// FAITHFUL DIRECT PORT of TIcedAnimator::Initialize/Animate/Render
// (src/effect_old.cpp:8749-8953). 3-phase state machine:
//   Phase A — HOLD: frameon < ICED_DURATION (240 ticks ≈ 10s @ 24Hz).
//     Reveal face01..face43 sub-objects 4/frame over paralyzed victim.
//   Phase B — SHATTER: frameon == ICED_DURATION. Seed MAX_ICED_CHUNKS=30
//     cube chunks in 4 quadrants × 5 rows.
//   Phase C — TUMBLE: integrate p+=v, v.z-=ICED_CHUNK_GRAVITY(0.25),
//     spin t+=w, bounce at p.z<=16 (v.z*=-0.5, spin x2), die after
//     random(2,3) bounces. KillThisEffect when all chunks dead.
//
// Blend = Alpha, Lit-mode = Unlit (asset color), Depth = TestNoWrite.
// Posted by I21 (and retail-only TStormAnimator) — harness drives this
// effect standalone (no victim coupling: `icedchar` stays nullptr,
// SetParalize/IsIced/IsDead branches are guarded out for sector-less
// preview, lifecycle runs purely on frameon).
inline constexpr int32_t kIcedSimTickMs = 1000 / 24;
inline constexpr int32_t kIcedDuration  = 24 * 10;   // ICED_DURATION (effect_old.cpp:8720)
inline constexpr float   kIcedChunkGravity = 0.25f;  // ICED_CHUNK_GRAVITY (effect_old.cpp:8721)
inline constexpr int32_t kIcedRevealCap = 50;        // min(frameon*4, 50) (effect_old.cpp:8909)

// One shatter chunk (snapshot: p[], v[], t[], w[], s[], l[] member
// arrays of size MAX_ICED_CHUNKS). Re-flattened per family convention.
struct SIcedChunk
{
    hmm_vec3 pos = {0.0f, 0.0f, 0.0f};   // p[i]
    hmm_vec3 vel = {0.0f, 0.0f, 0.0f};   // v[i]
    hmm_vec3 rot = {0.0f, 0.0f, 0.0f};   // t[i] (angular position)
    hmm_vec3 ang = {0.0f, 0.0f, 0.0f};   // w[i] (angular velocity)
    hmm_vec3 scl = {0.0f, 0.0f, 0.0f};   // s[i]
    int32_t  life = 0;                    // l[i] (bounces left)
};

_CLASSDEF(TIcedEffect_Bespoke)

class TIcedEffect_Bespoke : public TEffect
{
  public:
    TIcedEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TIcedEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TIcedEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TIcedEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin,
                                                                  const char* asset_override = nullptr);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);
    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    SIcedChunk chunks_[MAX_ICED_CHUNKS] {};
    float   angle_         = 0.0f;       // random(0,359)*TORADIAN, Z-axis spin for reveal facets
    int32_t frameon_       = 0;
    int32_t donebouncing_  = 0;

    // box01 (chunk mesh) texture handle + UVs.
    TTextureHandle facet_tex_ = kInvalidTexture;
    float          facet_uv_[4] {0.0f, 0.0f, 1.0f, 1.0f};
    float          facet_size_wu_ = 24.0f;

    bool   alive_        = true;
    double sim_accum_ms_ = 0.0;
};

// *************************************************************************
// * Magic family — wave-bespoke-04 first-pass bespoke ports                *
// * (M01 TAuraEffect, M03 THealEffect, M05 TMistEffect, X09 TShieldEffect) *
// *************************************************************************
//
// Faithful direct ports of the snapshot animator bodies for the magic
// family. Each effect ships as T<Name>Effect_Bespoke + SpawnForTest_BESPOKE
// + TickAndSubmitForTest_BESPOKE + a harness entry "T<Name>_BESPOKE".
// Pattern mirrors TBloodEffect_Bespoke at src/effect.cpp:1690-2065.
//
// Snapshot sources (per-batch translation rules):
//   M01: TAuraEffect/TAuraAnimator   src/effect_old.cpp:3482-3688
//   M03: THealEffect/THealAnimator   src/effect_old.cpp:464-849
//   M05: TMistEffect/TMistAnimator   src/effect_old.cpp:11342-11515
//   X09: TShieldAnimator             src/effect_old.cpp:4336-4392
//
// Translation rules applied (per task brief):
//   1. Per-tick math + variable names + constants from the snapshot.
//   2. Blend mode preserved AS WRITTEN: `SetAddBlendState` -> AdditiveStraight,
//      `SetBlendState` -> Alpha.
//   3. Two-pass shape preserved if present (no dead-code drop).
//   4. Animation uses the 24Hz sim-tick accumulator pattern (B01/H03/M05).
//   5. Render-path API adapted to Sokol (SubmitFxBillboard); mesh-based
//      RenderObject calls in the snapshot draw as billboards positioned at
//      the original sub-object pos/rot/scl — first-pass visual.

_CLASSDEF(TAuraEffect_Bespoke)

// M01: character aura particle system. Snapshot:
// src/effect_old.cpp:3535-3667 (TAuraAnimator). Emits particles uniformly
// distributed across the character's animator sub-objects with random
// upward velocity; particles shrink (scl *= 0.97) until the AURA_FRAME
// window closes. SetAddBlendState -> AdditiveStraight.
class TAuraEffect_Bespoke : public TEffect
{
  public:
    TAuraEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TAuraEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TAuraEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TAuraEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);

    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    // Snapshot constants — AURA_COUNT=100; AURA_PART_MIN/MAX=50;
    // AURA_ADD=2; AURA_SPREAD=2; AURA_MIN_SCL=25, AURA_MAX_SCL=60;
    // AURA_MIN_Z=5, AURA_MAX_Z=30; AURA_MIN_LIFE=15, AURA_MAX_LIFE=30;
    // AURA_DEC=0.97; AURA_SEC=30; AURA_FRAME=AURA_SEC*24=720.
    static constexpr int32_t kAuraCount    = 100;
    static constexpr int32_t kAuraAdd      = 2;
    static constexpr int32_t kAuraSpread   = 2;
    static constexpr int32_t kAuraMinScl   = 25;
    static constexpr int32_t kAuraMaxScl   = 60;
    static constexpr int32_t kAuraMinZ     = 5;
    static constexpr int32_t kAuraMaxZ     = 30;
    static constexpr int32_t kAuraMinLife  = 15;
    static constexpr int32_t kAuraMaxLife  = 30;
    static constexpr float   kAuraDec      = 0.97f;
    static constexpr int32_t kAuraFrameCap = 30 * 24;   // AURA_SEC * 24Hz
    static constexpr int32_t kAuraSimTickMs = 1000 / 24;

    struct SAuraParticle
    {
        bool     used = false;
        hmm_vec3 pos  = {0.0f, 0.0f, 0.0f};
        hmm_vec3 vel  = {0.0f, 0.0f, 0.0f};
        hmm_vec3 scl  = {0.0f, 0.0f, 0.0f};
        int32_t  life_span = 0;
    };

    SAuraParticle  particles_[kAuraCount] {};
    int32_t        frame_         = 0;
    int32_t        to_add_        = 0;
    double         sim_accum_ms_  = 0.0;
    bool           alive_         = true;
    TTextureHandle texture_       = kInvalidTexture;
    float          uv_rect_[4]    = {0.0f, 0.0f, 1.0f, 1.0f};
};

_CLASSDEF(THealEffect_Bespoke)

// M03: healing spell visual. Snapshot:
// src/effect_old.cpp:628-833 (THealAnimator). 60 bubbles rising from
// player's feet over HEAL_DURATION=40 ticks + a cylindrical glow at the
// feet that grows then shrinks. SetBlendState -> Alpha.
class THealEffect_Bespoke : public TEffect
{
  public:
    THealEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    THealEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~THealEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static THealEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);

    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    static constexpr int32_t kHealBubbles    = 60;
    static constexpr float   kHealScaleStep  = 0.15f;
    static constexpr int32_t kHealingRadius  = 20;
    static constexpr int32_t kHealDuration   = 40;
    static constexpr int32_t kHealSimTickMs  = 1000 / 24;

    hmm_vec3 p_[kHealBubbles]        {};
    float    scale_[kHealBubbles]    {};
    float    rise_[kHealBubbles]     {};
    int32_t  framenum_[kHealBubbles] {};
    int32_t  activebubbles_ = 0;
    float    rotation_     = 0.0f;
    int32_t  heal_num_     = 0;     // bubble sub-object index (0/2/4/6)
    int32_t  glow_num_     = 1;     // glow sub-object index (1/3/5/7)
    double   sim_accum_ms_ = 0.0;
    bool     alive_        = true;
    TTextureHandle bubble_tex_  = kInvalidTexture;
    TTextureHandle glow_tex_    = kInvalidTexture;
    float          bubble_uv_[4] = {0.0f, 0.0f, 1.0f, 1.0f};
    float          glow_uv_[4]   = {0.0f, 0.0f, 1.0f, 1.0f};
};

_CLASSDEF(TMistEffect_Bespoke)

// M05: continuous ambient mist emitter. Snapshot:
// src/effect_old.cpp:11376-11502 (TMistAnimator). 50 drops with retail
// pos/vel envelope; ascends, falls under gravity, respawns-in-place on
// landing (steady-state flux). SetAddBlendState -> AdditiveStraight.
// BESPOKE pivot per BESPOKE_WAVE_ROADMAP (engine TMistEffect is the legacy
// path; this bespoke class is canonical for first-pass).
class TMistEffect_Bespoke : public TEffect
{
  public:
    TMistEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TMistEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TMistEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TMistEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);

    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    static constexpr int32_t kMistMaxDrops    = 50;
    static constexpr int32_t kMistLength      = 64;
    static constexpr int32_t kMistWidth       = 20;
    static constexpr int32_t kMistHeight      = 4;
    static constexpr float   kMistScale       = 0.7f;
    static constexpr float   kMistGravity     = 0.37f;     // RIPPLE_GRAVITY
    static constexpr float   kMistSpawnZ      = 5.0f;      // snapshot literal
    static constexpr int32_t kMistSimTickMs   = 1000 / 24;
    static constexpr float   kMistBaseSizeWu  = 96.0f;     // billboard size, snapshot rendered scale 0.7 against unknown quad

    struct SMistDrop
    {
        hmm_vec3 pos  = {0.0f, 0.0f, 0.0f};
        hmm_vec3 vel  = {0.0f, 0.0f, 0.0f};
        bool     dead = false;
    };

    SMistDrop      drops_[kMistMaxDrops] {};
    double         sim_accum_ms_  = 0.0;
    bool           alive_         = true;
    TTextureHandle texture_       = kInvalidTexture;
    float          uv_rect_[4]    = {0.0f, 0.0f, 1.0f, 1.0f};
};

_CLASSDEF(TShieldEffect_Bespoke)

// X09: shield bubble. Snapshot:
// src/effect_old.cpp:4336-4392 (TShieldAnimator). Animator-only in
// pre-release (bridges via TEffect on caster object). Renders a single
// I3D sphere mesh at pos.z=40 above the character with SHIELD_SCALE=2.0
// and a fixed -π/3 X-rotation + -π/4 Z-rotation. SetBlendState -> Alpha.
// First-pass: a single Alpha billboard at the same pos/scale; mesh
// rendering is a future upgrade.
class TShieldEffect_Bespoke : public TEffect
{
  public:
    TShieldEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TShieldEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TShieldEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TShieldEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);

    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    static constexpr float   kShieldScale     = 2.0f;     // SHIELD_SCALE
    static constexpr float   kShieldLiftZ     = 40.0f;    // snapshot pos.z = 40
    static constexpr float   kShieldSizeWu    = 96.0f;
    static constexpr int32_t kShieldSimTickMs = 1000 / 24;

    hmm_vec3       pos_           = {0.0f, 0.0f, 40.0f};   // snapshot init
    int32_t        framenum_      = 0;
    double         sim_accum_ms_  = 0.0;
    bool           alive_         = true;
    TTextureHandle texture_       = kInvalidTexture;
    float          uv_rect_[4]    = {0.0f, 0.0f, 1.0f, 1.0f};
};

// *************************************************************************
// * wave-bespoke-05: sparkle/glow family — flare / sym-glow / photon /     *
// * pixie. Faithful direct ports of the snapshot animator bodies for the   *
// * --test=vfx harness, mirroring the TBloodEffect_Bespoke pattern at      *
// * effect.cpp:1690-2065. Each class loads its real .I3D imagery (no       *
// * procedural stand-in per feedback_no_standins) and runs the snapshot's  *
// * per-tick state machine LINE BY LINE against an internal state array.  *
// * Pipeline + blend preserved verbatim — SetBlendState in the snapshot    *
// * means Alpha here; SetAddBlendState means AdditiveStraight; no          *
// * reinterpretation. Each effect couples a point light (LS) where the     *
// * family cross-check says it should. Framerate-independent via a 24Hz    *
// * sim-tick accumulator (per feedback_framerate_independent_anim).        *
// *************************************************************************

// ----- X03 TFlareAnimator -------------------------------------------------
//
// "Flare" — 10 small ground-flat textured quads bouncing on the floor.
// REGISTER_3DANIMATOR("Flare", TFlareAnimator) (effect_old.cpp:515).
// Animate (effect_old.cpp:557-582): each of 10 sparks integrates straight
// gravity-only physics; when a spark hits z=0 it bounces (vel.z *= -0.5)
// and once damped (|vel.z|<0.4) respawns with a fresh launch velocity.
// Render (effect_old.cpp:584-611): sub-object 0 drawn 10 times, each at
// rot=(-pi/2, 0, -pi/4) (ground-flat) and scl=4. SetBlendState (= Alpha,
// not additive — per feedback_retail_first + family cross-check with the
// existing FlareSpawn placeholder at vfxtest.cpp:2257 which DOES use
// Additive, but the SNAPSHOT writes SetBlendState which translates to
// Alpha; we preserve the snapshot verbatim per the no-reinterpretation
// rule).
//
// LS coupling: family note + "Sparkle/glow family" + L01 lightsource
// note in roadmap. The snapshot animator doesn't call any light API
// directly (that lives on the parent TEffect via spell.def coupling),
// so we re-add one point light per frame in TickAndSubmitForTest at the
// effect origin, warm-yellow, modest radius. Verified by L02 / X17
// pattern.
//
// Asset: Misc\IrisFlare.I3D (the only "Flare-like" imagery registered in
// Class.Def with a TFlareAnimator-class animator — also used by
// "Teleporter" / "IrisFlare").

inline constexpr int32_t kFlareBespokeNumSparks = 10;   // hard-coded loop limit (effect_old.cpp:562)
inline constexpr int32_t kFlareBespokeSimTickMs = 1000 / 24;
inline constexpr float   kFlareBespokeScale     = 4.0f;
inline constexpr float   kFlareBespokeBaseSizeWu = 12.0f;
inline constexpr float   kFlareBespokeLightRadiusWu  = 240.0f;
inline constexpr float   kFlareBespokeLightIntensity = 1.0f;

struct SFlareBespokeSpark
{
    hmm_vec3 p = {0.0f, 0.0f, 0.0f};
    hmm_vec3 v = {0.0f, 0.0f, 0.0f};
};

_CLASSDEF(TFlareEffect_Bespoke)

class TFlareEffect_Bespoke : public TEffect
{
  public:
    TFlareEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TFlareEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TFlareEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    // Standalone spawn for the --test=vfx harness. Loads Misc\IrisFlare.I3D,
    // resolves sub-object 0 (the single flare quad), seeds the 10-spark
    // array. Returns nullptr if the imagery can't be loaded.
    [[nodiscard]] static TFlareEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin,
                                                                    const char* asset_override = nullptr);

    // Per-frame tick + submit. Ports TFlareAnimator::Animate +
    // TFlareAnimator::Render directly. 10 alpha-blended billboards drawn
    // per frame + 1 point light re-added at effect origin.
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);

    // Always alive for the harness — TFlareAnimator self-respawns each
    // spark on bounce-decay, so the effect never naturally ends.
    [[nodiscard]] bool IsAlive() const { return true; }

  private:
    SFlareBespokeSpark sparks_[kFlareBespokeNumSparks] {};
    TTextureHandle     texture_      = kInvalidTexture;
    float              uv_rect_[4]   = {0.0f, 0.0f, 1.0f, 1.0f};
    float              size_wu_      = kFlareBespokeBaseSizeWu;
    double             sim_accum_ms_ = 0.0;
};

// ----- X10 TSymGlowAnimator -----------------------------------------------
//
// "SymGlow" — single glowing-symbol billboard whose texture pulses (V
// scroll), the Z-scale of the quad breathes between 2.0 and 5.0, and the
// authored uv.tv is creeping up slowly. REGISTER_3DANIMATOR("SymGlow",
// TSymGlowAnimator) (effect_old.cpp:4584). Animator-only (no Effect
// class); attached to whatever object is named "SymGlow" in a sector
// (likely magical altars / runes).
//
// SetupObjects (effect_old.cpp:4588-4606): walk sub-objects, copy verts,
// shift authored uv.tv by -0.01 across all verts. Animate
// (effect_old.cpp:4615-4633): 40-tick toggle on dz sign; integrate zscale
// in [2,5]; u (the per-tick tv scroll step) re-randomized to (2..8)/100.
// Render (effect_old.cpp:4642-4665): SetBlendState (Alpha), draw the one
// sub-object with scl=(1.4, 1.4, zscale) and obj->verts[*].tu += u per
// vertex per draw. The scl.z 2..5 stretch on a 2D billboard is the
// "breathing" effect; vertex.tu accumulation drives the V scroll.
//
// LS coupling: roadmap says "Likely couples a soft point light" — we
// re-add a warm point light per frame for the LS-pipeline coupling
// invariant, sized to ~symbol radius.
//
// Asset: Misc\SymGlow.I3D (Class.Def-registered).

inline constexpr int32_t kSymGlowBespokeSimTickMs    = 1000 / 24;
inline constexpr float   kSymGlowBespokeScaleXY      = 1.4f;
inline constexpr float   kSymGlowBespokeZMin         = 2.0f;
inline constexpr float   kSymGlowBespokeZMax         = 5.0f;
inline constexpr float   kSymGlowBespokeDzInit       = 0.1f;
inline constexpr int32_t kSymGlowBespokeFlipTicks    = 40;
inline constexpr int32_t kSymGlowBespokeUScrollMin   = 2;       // /100
inline constexpr int32_t kSymGlowBespokeUScrollMax   = 8;       // /100
inline constexpr float   kSymGlowBespokeBaseSizeWu   = 32.0f;
inline constexpr float   kSymGlowBespokeLightRadiusWu  = 200.0f;
inline constexpr float   kSymGlowBespokeLightIntensity = 0.8f;

_CLASSDEF(TSymGlowEffect_Bespoke)

class TSymGlowEffect_Bespoke : public TEffect
{
  public:
    TSymGlowEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TSymGlowEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TSymGlowEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TSymGlowEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);
    [[nodiscard]] bool IsAlive() const { return true; }

  private:
    TTextureHandle texture_      = kInvalidTexture;
    float          uv_rect_[4]   = {0.0f, 0.0f, 1.0f, 1.0f};
    float          size_wu_      = kSymGlowBespokeBaseSizeWu;
    float          u_offset_     = 0.0f;       // accumulated V scroll (we shift along V like the
                                               // snapshot's obj->verts[].tu += u — but in src this
                                               // is named "u" though it advances the V axis through
                                               // the per-vertex tu field — see §gotcha below).
    float          zscale_       = 2.0f;
    float          dz_           = kSymGlowBespokeDzInit;
    int32_t        timer_        = 0;
    double         sim_accum_ms_ = 0.0;
};

// ----- M07 TPhotonEffect (Bespoke) -----------------------------------------
//
// "Photon" — 32-slot energy-bolt missile (forensics docs/vfx/forensics/
// M07_TPhotonEffect.md). REGISTER_3DANIMATOR("Photon", TPhotonAnimator)
// (missileeffect.cpp:176). Three-state LAUNCH / FLY / EXPLODE machine; the
// in-game TMissileEffect base drives transit + state transitions, but the
// harness drives the visual choreography only (no live char/collision).
//
// We collapse the choreography onto a freestanding TEffect subclass and
// cycle LAUNCH -> FLY -> EXPLODE on a fixed visual cadence so the harness
// can replay every state without the missile-base machinery. The
// per-state per-spark kinematics are line-for-line from
// missileeffect.cpp:187-399 (Initialize/Animate); the render is from
// :408-441.
//
// SetBlendState in the snapshot's Render is Alpha. We submit one alpha
// billboard per spark per frame and re-add one point light at the
// effect origin per frame (LS coupling — bright blue-white glow during
// the bolt, brightest at EXPLODE).
//
// Asset: Magic\Photon.I3D (Class.Def-registered + byte-identical in
// data/imagery.rvi per forensics §2.1(1)).

inline constexpr int32_t kPhotonBespokeNumSparks       = 16;     // NUM_PHOTON_SPARKS
inline constexpr int32_t kPhotonBespokeSlots           = 32;     // NUM_PHOTON_SPARKS * 2
inline constexpr int32_t kPhotonBespokeLaunchRadius    = 20;     // PHOTON_LAUNCH_RADIUS
inline constexpr int32_t kPhotonBespokeLaunchDuration  = 25;     // PHOTON_LAUNCH_DURATION
inline constexpr float   kPhotonBespokeScaleStep       = 0.25f;  // PHOTON_LAUNCH_SCALE_STEP
inline constexpr int32_t kPhotonBespokeSimTickMs       = 1000 / 24;
inline constexpr float   kPhotonBespokeBaseSizeWu      = 16.0f;
inline constexpr float   kPhotonBespokeLightRadiusWu   = 280.0f;
inline constexpr float   kPhotonBespokeLightIntensity  = 1.4f;
// Harness-only state-cycle cadence (tick durations for LAUNCH/FLY/EXPLODE).
// In-game these are driven by TMissileEffect range/Pulse; the harness
// cycles them on a fixed timer so all three visuals replay.
inline constexpr int32_t kPhotonBespokeFlyTicks        = 40;
inline constexpr int32_t kPhotonBespokeExplodeTicks    = 30;
inline constexpr int32_t kPhotonBespokeIdleTicks       = 12;     // gap between EXPLODE and next LAUNCH

_CLASSDEF(TPhotonEffect_Bespoke)

class TPhotonEffect_Bespoke : public TEffect
{
  public:
    TPhotonEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TPhotonEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TPhotonEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TPhotonEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);
    [[nodiscard]] bool IsAlive() const { return true; }   // cycles forever in the harness

  private:
    void ResetLaunch_();
    void EnterFly_();
    void EnterExplode_();

    hmm_vec3       p_[kPhotonBespokeSlots] {};
    hmm_vec3       v_[kPhotonBespokeSlots] {};
    float          scale_[kPhotonBespokeSlots] {};
    int32_t        framenum_[kPhotonBespokeSlots] {};
    int32_t        activesparks_ = 0;
    int32_t        numtexframes_ = 1;
    int32_t        state_        = 0;     // 0=LAUNCH 1=FLY 2=EXPLODE 3=IDLE (harness gap)
    int32_t        oldstate_     = -1;    // force first-time block on entry
    int32_t        state_ticks_  = 0;
    TTextureHandle texture_      = kInvalidTexture;
    float          uv_rect_[4]   = {0.0f, 0.0f, 1.0f, 1.0f};
    float          size_wu_      = kPhotonBespokeBaseSizeWu;
    double         sim_accum_ms_ = 0.0;
};

// ----- M08 TPixieEffect (Bespoke) ------------------------------------------
//
// "Pixie" — small swarm of 25 twinkling particles (fairy dust) attached to
// the effect origin. DEFINE_BUILDER("Pixie", TPixieEffect) (effect_old.cpp:
// 12082), REGISTER_3DANIMATOR("Pixie", TPixieAnimator) (:12099). The
// animator (effect_old.cpp:12109-12277) seeds 25 per-particle slots with
// random pos in [-32, 32]^3 and random vel/25; each tick the particles
// drift, the velocity gets a centring restorative force (PIX_ACC=0.4),
// scale jitters in [0.06, 0.08], time flips on a 1/49 chance, and the
// pixies flee from any nearby character. SetBlendState (Alpha) renders
// the sub-object selected by `pix[i].time` (0 or 1 — two variants of the
// glow). Per-particle: scale uniform, rotZ = -face_byteangle * TORADIAN
// (in-game), translate to (pos.x*charnear/100, pos.y*charnear/100,
// PIX_HEIGHT + pos.z).
//
// Harness simplification: we strip the character-flee logic (FindObjects
// InRange is a live-map query) and use the static origin as the rest
// point. The character-near scale (`charnear`) stays at 100 (= 1.0x).
// The face byte-angle defaults to 0.
//
// LS coupling: roadmap calls for a "soft point light coupling"; we
// re-add a small green-blue point light per frame, modulated by the
// swarm's collective scale, to drive the LS pipeline.
//
// Asset: Misc\Pixies.I3D (Class.Def-registered).

inline constexpr int32_t kPixieBespokeNumParts        = 25;     // PIX_NUMPARTS
inline constexpr float   kPixieBespokeAcc             = 0.4f;   // PIX_ACC
inline constexpr int32_t kPixieBespokeHeight          = 32;     // PIX_HEIGHT
inline constexpr float   kPixieBespokeMinScale        = 0.06f;  // PIX_MINSCALE
inline constexpr float   kPixieBespokeMaxScale        = 0.08f;  // PIX_MAXSCALE
inline constexpr int32_t kPixieBespokeSimTickMs       = 1000 / 24;
inline constexpr float   kPixieBespokeBaseSizeWu      = 256.0f; // sub-object scaled by ~0.07 -> 18 wu visible
inline constexpr float   kPixieBespokeLightRadiusWu   = 220.0f;
inline constexpr float   kPixieBespokeLightIntensity  = 0.7f;

// One pixie particle's transient state. Pre-release stored these as
// SWaterParticle (effectcomp.h) inside the animator's heap array; we
// inline a per-particle struct to match the SBloodParticleEx pattern.
struct SPixieParticle
{
    hmm_vec3 scale = {kPixieBespokeMinScale, kPixieBespokeMinScale, kPixieBespokeMinScale};
    hmm_vec3 pos   = {0.0f, 0.0f, 0.0f};
    hmm_vec3 vel   = {0.0f, 0.0f, 0.0f};
    int32_t  time  = 0;     // 0 or 1 — selects sub-object texture (two glow variants)
};

_CLASSDEF(TPixieEffect_Bespoke)

class TPixieEffect_Bespoke : public TEffect
{
  public:
    TPixieEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TPixieEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TPixieEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TPixieEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);
    [[nodiscard]] bool IsAlive() const { return true; }   // ambient swarm — never dies

  private:
    SPixieParticle pix_[kPixieBespokeNumParts] {};
    int32_t        charnear_     = 100;          // 100 = 1.0x scale (no nearby char)
    TTextureHandle textures_[2]  = {kInvalidTexture, kInvalidTexture};
    float          uv_rect_[2][4] = {{0,0,1,1}, {0,0,1,1}};
    float          size_wu_      = kPixieBespokeBaseSizeWu;
    double         sim_accum_ms_ = 0.0;
};

// ----- X04/X05/X06/X07/X08 TFountainAnimator (Bespoke) --------------------
//
// "Fountain" sparkle column — 10 colored sparkle billboards rising from a
// 40x40 wu ground disc, shrinking linearly to nothing, then respawning at
// the floor on death. Continuous loop with no kill-on-done; scripted
// `.delete` terminates in-game. Despite the class name "Fountain", the
// live in-game usage is the Sabu witch vanish/appear puff (REDFONT) and
// the chest-unlock sparkle (CYANFONT) in the Ahkuilon module — see the
// FOUNTAIN_TFountainAnimator forensics doc.
//
// One bespoke port covers the abstract base + all 4 color leaves
// (TCyanFountainAnimator / TRedFountainAnimator / TGreenFountainAnimator /
// TBlueFountainAnimator). The only per-variant code in the snapshot is
// `SetColorObject() { colorobj = 0|1|2|3; }` (effect.h:949,964,979,994);
// we expose colorobj_ as a SpawnForTest_BESPOKE parameter and resolve
// the sub-object 0..3 at spawn.
//
// Asset: Misc\Sparkle.I3D (4 sub-objects 'photon'/'photon01'/'02'/'03',
// each with its own baked vertex DIFFUSE tint giving cyan/red/green/blue).
// Class.Def-registered 4 times as CyanFont/RedFont/GreenFont/BlueFont
// (legacy/Class.Def:2023-2026).
//
// Blend: snapshot uses SetBlendState (= Alpha; MODULATE + SRC_ALPHA /
// INV_SRC_ALPHA). Forensics flags this as SUSPECT (bright-on-dark
// sprite, the textbook "code says Alpha but reads as glow" case) but
// the rule is to preserve blend AS WRITTEN — Alpha it is.
//
// LS coupling: none in the snapshot (no AddPointLight call); we add a
// faint per-variant tinted point light to drive the LS pipeline path
// the same way Pixie does — soft, low intensity, scaled with active
// bubble count.

inline constexpr int32_t kFountainBespokeNumBubbles  = 10;     // NUM_FOUNTAIN_BUBBLES
inline constexpr float   kFountainBespokeScaleStep   = 0.15f;  // FOUNTAIN_SCALE_STEP
inline constexpr int32_t kFountainBespokeRadius      = 20;     // FOUNTAIN_RADIUS
inline constexpr float   kFountainBespokeInitScale   = 2.0f;   // scale[n] at spawn
inline constexpr int32_t kFountainBespokeSimTickMs   = 1000 / 24;
inline constexpr float   kFountainBespokeBaseSizeWu  = 64.0f;  // photon sub-object ~unit-scaled at 2.0
inline constexpr float   kFountainBespokeLightRadiusWu  = 180.0f;
inline constexpr float   kFountainBespokeLightIntensity = 0.5f;
inline constexpr int32_t kFountainBespokeNumSubObjs  = 4;      // photon/photon01/02/03

_CLASSDEF(TFountainAnimator_Bespoke)

class TFountainAnimator_Bespoke : public TEffect
{
  public:
    TFountainAnimator_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TFountainAnimator_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TFountainAnimator_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    // colorobj selects the photon sub-object (0=Cyan / 1=Red / 2=Green / 3=Blue)
    // and the matching tinted point light color.
    [[nodiscard]] static TFountainAnimator_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin,
                                                                         int32_t colorobj,
                                                                         const char* asset_override = nullptr);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);
    [[nodiscard]] bool IsAlive() const { return true; }   // ambient loop — never dies

  private:
    // Per-bubble state. Mirrors TFountainAnimator's p/scale/rise/framenum
    // arrays (effect.h:911-914).
    hmm_vec3 p_[kFountainBespokeNumBubbles] {};
    float    scale_[kFountainBespokeNumBubbles] {};
    float    rise_[kFountainBespokeNumBubbles]  {};
    int32_t  framenum_[kFountainBespokeNumBubbles] {};
    int32_t  colorobj_ = 0;     // 0..3 -> photon/photon01/photon02/photon03

    // Per-sub-object texture + uv (resolved at spawn). We pre-resolve
    // all 4 even though only colorobj_ is drawn — keeps the SpawnForTest
    // path uniform and lets a future use case re-tint without reload.
    TTextureHandle textures_[kFountainBespokeNumSubObjs]  = {kInvalidTexture, kInvalidTexture, kInvalidTexture, kInvalidTexture};
    float          uv_rects_[kFountainBespokeNumSubObjs][4] = {{0,0,1,1},{0,0,1,1},{0,0,1,1},{0,0,1,1}};
    float          size_wu_      = kFountainBespokeBaseSizeWu;
    double         sim_accum_ms_ = 0.0;
};

// =========================================================================
// W2D batch — Misc A — character/ambient particle emitters (X01, B04)
// =========================================================================
// Localized-at-end additions for clean wave-3 merge. Faithful direct ports
// of the snapshot animator bodies in src/effect_old.cpp.

// ----- X01 TFlyEffect (Bespoke) ------------------------------------------
// FAITHFUL DIRECT PORT of TFlyAnimator (snapshot src/effect_old.cpp:
// 6514-6603). Ambient corpse-decay fly swarm. Per-particle random wander
// inside a fixed cube (FLY_RANGE_X/Y/Z = 30 wu), one fly is "switched on"
// per tick at random until all 20 are active. Velocity is per-tick
// random(-2,2) integer steps in each axis; if a fly oversteps the cube
// edge it's pushed back by 2*vel (a hard reflect-bounce). Render is
// SetBlendState() (Alpha) with cull=NONE — billboards already no-cull.
// Asset: Misc\Flies.I3D — small dark-grey/black pixel-sprite quads.

inline constexpr int32_t kFlyBespokeCount        = FLY_COUNT;     // 20
inline constexpr int32_t kFlyBespokeRangeX       = FLY_RANGE_X;   // 30
inline constexpr int32_t kFlyBespokeRangeY       = FLY_RANGE_Y;   // 30
inline constexpr int32_t kFlyBespokeRangeZ       = FLY_RANGE_Z;   // 30
inline constexpr float   kFlyBespokeScale        = 0.3f;          // Init: scl.xyz = .3
inline constexpr int32_t kFlyBespokeSimTickMs    = 1000 / 24;
inline constexpr float   kFlyBespokeBaseSizeWu   = 10.0f;         // size.x = size.y = 10

struct SFlyParticleBespoke
{
    hmm_vec3 pos  = {0.0f, 0.0f, 0.0f};
    hmm_vec3 scl  = {kFlyBespokeScale, kFlyBespokeScale, kFlyBespokeScale};
    hmm_vec3 rot  = {0.0f, 0.0f, 0.0f};
    bool     used = false;   // staged activation (1 per tick chance)
};

_CLASSDEF(TFlyEffect_Bespoke)

class TFlyEffect_Bespoke : public TEffect
{
  public:
    TFlyEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TFlyEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TFlyEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TFlyEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);
    [[nodiscard]] bool IsAlive() const { return true; }   // ambient swarm — never dies

  private:
    SFlyParticleBespoke flies_[kFlyBespokeCount] {};
    TTextureHandle      texture_      = kInvalidTexture;
    float               uv_rect_[4]   = {0.0f, 0.0f, 1.0f, 1.0f};
    float               size_wu_      = kFlyBespokeBaseSizeWu;
    double              sim_accum_ms_ = 0.0;
};

// ----- B04 TPulpEffect (Bespoke) — STUBBED -------------------------------
// FAITHFUL DIRECT PORT BLOCKED. TPulpEffect::Set(vel, ch, ...) requires a
// live PTCharacter whose TCharAnimator yields per-bone sub-object meshes
// (head/limb chunks) ejected as falling rigid bodies + blood spray. The
// harness rig path exists but there is no SubmitFx* API for animated
// character sub-object meshes (only billboards/particles); a faithful
// port would need either the IM body-part submission path or coupling
// against the live rig from inside the bespoke effect — both out of
// W2D scope. STUB per brief: SpawnForTest returns nullptr + log_warn.
// Re-enable once either (a) char-mesh submission lands or (b) we accept
// the blood-only subset and reuse the B01 path (brief notes: "May share
// particles with B01 — keep B01 path canonical"). Status: BLOCKED.

_CLASSDEF(TPulpEffect_Bespoke)

class TPulpEffect_Bespoke : public TEffect
{
  public:
    TPulpEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TPulpEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TPulpEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TPulpEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);
    [[nodiscard]] bool IsAlive() const { return true; }
};

// *************************************************************************
// * Wave 3 batch W2C: Weather C — character-cast storm + wind strip.       *
// *                                                                       *
// *   W07 TStormAnimator_Bespoke  — particle storm (rain-of-bolts) driven *
// *                                 by SStormParams (effectcomp.cpp:38).  *
// *                                 First-pass mirrors the TStormAnimator *
// *                                 inner helper as a standalone effect — *
// *                                 the M09-style "wrapper" (e.g.         *
// *                                 TMeteorStormAnimator) becomes a fixed *
// *                                 params struct chosen at spawn time.   *
// *                                                                       *
// *   S02 TWindStripAnimator_Bespoke — 3-strip wind-blown ambient streak  *
// *                                    (stripeffect.cpp:1063-1417). Each  *
// *                                    strip walks a Lissajous-like curve *
// *                                    around the caster, spawning drop    *
// *                                    sparks behind it + a fade-in/out   *
// *                                    halo. Snapshot SetBlendState =     *
// *                                    Alpha for strips, AddBlendState    *
// *                                    for halo.                          *
// *                                                                       *
// * Translation rules: identical math, identical per-tick constants,      *
// * identical render-pass order. Only the render API adapts. Both port    *
// * reuses the StripFamilyGlowTexture / StripFamilySparkTexture procedural*
// * stand-ins already proven by S04 TLightningAnimator_Bespoke.           *
// *************************************************************************

// --- W07 TStormAnimator_Bespoke -------------------------------------------
// Faithful direct port of the inner TStormAnimator helper (effectcomp.cpp:
// 38-380). The helper is a particle bucket of falling animated billboards;
// each instance is born above the caster, falls under gravity, switches to
// an "impact" animation when it hits the walk-height, then dies after the
// impact frames play out. Snapshot Render blend = SetAddBlendState ->
// AdditiveStraight (cf. TMeteorStormAnimator::Render, effect_old.cpp:6485).
//
// For the harness we instantiate with ice-bolt-style params (a smaller,
// blue-tinted storm — the "stormbolt.i3d" intent). The MeteorStormAnimator
// wrapper in effect_old.cpp:6367-6445 is our reference for how outer
// animators populate SStormParams.
//
// Asset binding is deferred (no stormbolt.i3d loader yet); first-pass uses
// the StripFamilySparkTexture procedural stand-in tinted blue for the
// "particle" phase and white for the "impact" phase.
inline constexpr int32_t kStormBespokeMaxInstance = 20;   // STORM_DEFAULT_MAX_INSTANCE
inline constexpr int32_t kStormBespokeSimTickMs   = 1000 / 24;
inline constexpr int32_t kStormBespokeDurationTicks = 24 * 5;  // ~5 s of spawning
inline constexpr int32_t kStormBespokeRampSize     = 8;   // pulse_count target

// One storm particle (snapshot SStormInstance — effectcomp.h:56).
struct SStormBespokeInstance
{
    bool     used               = false;
    hmm_vec3 pos                = {0.0f, 0.0f, 0.0f};
    bool     is_particle        = true;
    bool     explosion_sounded  = false;
    hmm_vec3 velocity           = {0.0f, 0.0f, 0.0f};
    hmm_vec3 part_scl           = {0.0f, 0.0f, 0.0f};
    hmm_vec3 expl_scl           = {0.0f, 0.0f, 0.0f};
    float    gravity            = 0.0f;
    float    frame              = 0.0f;
    float    impact_frame_inc   = 0.0f;
    float    particle_frame_inc = 0.0f;
};

// The bespoke holds its own SStormParams (snapshot effectcomp.h:21). All
// fields copied verbatim — kept here as a local struct so the bespoke is
// self-contained without dragging in effectcomp.h.
struct SStormBespokeParams
{
    int32_t  particles          = 0;
    int32_t  tex_u              = 64;
    int32_t  tex_v              = 64;
    int32_t  particle_u         = 8;
    int32_t  particle_v         = 2;
    int32_t  particle_begin     = 0;
    int32_t  particle_end       = 7;
    int32_t  impact_u           = 4;
    int32_t  impact_v           = 4;
    int32_t  impact_begin       = 8;
    int32_t  impact_end         = 15;
    float    gravity            = 0.37f;
    hmm_vec3 velocity           = {-10.0f, 0.0f, -15.0f};
    hmm_vec3 pos                = {0.0f, 0.0f, 0.0f};
    hmm_vec3 pos_spread         = {100.0f, 100.0f, 0.0f};
    float    impact_frame_inc   = 0.7f;
    float    particle_frame_inc = 0.5f;
    hmm_vec3 particle_scale     = {0.5f, 1.5f, 1.0f};
    hmm_vec3 impact_scale       = {1.0f, 1.0f, 1.0f};
    hmm_vec3 rot                = {0.0f, 0.0f, 0.0f};
};

_CLASSDEF(TStormAnimator_Bespoke)
class TStormAnimator_Bespoke : public TEffect
{
  public:
    TStormAnimator_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TStormAnimator_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TStormAnimator_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TStormAnimator_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);
    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    // Snapshot animator helpers (effectcomp.cpp:53/80/116).
    int32_t GetCount_() const;
    void    Create_();
    void    Animate_();

    SStormBespokeParams   params_ {};
    SStormBespokeInstance storm_instance_[kStormBespokeMaxInstance] {};

    // Outer-animator state (mirrors TMeteorStormAnimator: ticks/tracker
    // drive the particle-count ramp).
    int32_t ticks_   = 0;
    int32_t tracker_ = 0;
    bool    alive_   = true;
    double  sim_accum_ms_ = 0.0;
};

// --- S02 TWindStripAnimator_Bespoke ---------------------------------------
// Faithful direct port of TWindStripAnimator::Initialize / SetupObjects /
// Animate / Render (src/stripeffect.cpp:1063-1417). Three parallel strip
// curves walk Lissajous-like paths around the caster; behind each strip
// point we spawn a "drop spark" with gravity. A central halo grows during
// the first half of the fadeout window and shrinks after. Snapshot has the
// caster following the spell-invoker's facing — for the harness we hold
// the center at the spawn origin and let centerang wobble.
//
// Blend: SetBlendState (Alpha) for strips + drops, SetAddBlendState
// (AdditiveStraight) for the halo (preserved as written in :1333).
//
// Asset: Magic\windstrip.i3d (no loader yet); first-pass uses the
// StripFamilyGlowTexture for the strips and the StripFamilySparkTexture
// for drops + halo.

inline constexpr int32_t kWindStripBespokeMaxStrips      = 3;
inline constexpr int32_t kWindStripBespokePoints         = 20;   // WINDSTRIP_POINTS
inline constexpr int32_t kWindStripBespokeWidth          = 15;   // WINDSTRIP_WIDTH
inline constexpr int32_t kWindStripBespokeWidth2         = 15;   // WINDSTRIP_WIDTH2
inline constexpr int32_t kWindStripBespokeDuration       = 250;  // WINDSTRIP_DURATION
inline constexpr int32_t kWindStripBespokeFadeout        = 100;  // WINDSTRIP_FADEOUT
inline constexpr int32_t kWindStripBespokeModifier       = 5;    // WINDSTRIP_MODIFIER
inline constexpr int32_t kWindStripBespokeModifierV      = 15;   // WINDSTRIP_MODIFIERV
inline constexpr int32_t kWindStripBespokeRadius         = 2;    // WINDSTRIP_RADIUS
inline constexpr int32_t kWindStripBespokeNumSparks      = 20 * 20;
inline constexpr float   kWindStripBespokeSparkScale     = 0.4f;
inline constexpr float   kWindStripBespokeSparkDScale    = 0.005f;
inline constexpr float   kWindStripBespokeSparkGrav      = 0.2f;
inline constexpr float   kWindStripBespokeSparkFall      = 0.5f;
inline constexpr float   kWindStripBespokeHaloStep       = 0.04f;
inline constexpr int32_t kWindStripBespokeSimTickMs      = 1000 / 24;

// One Lissajous point along a strip (head-prepended ring buffer).
struct SWindStripBespokePoint
{
    hmm_vec3 pos = {0.0f, 0.0f, 0.0f};
    bool     used = false;
};

// One drop spark.
struct SWindStripBespokeDrop
{
    hmm_vec3 pos = {0.0f, 0.0f, 0.0f};
    hmm_vec3 vel = {0.0f, 0.0f, 0.0f};
    float    scl = 0.0f;
    bool     used = false;
};

_CLASSDEF(TWindStripAnimator_Bespoke)
class TWindStripAnimator_Bespoke : public TEffect
{
  public:
    TWindStripAnimator_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TWindStripAnimator_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TWindStripAnimator_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TWindStripAnimator_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);
    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    void InitDrops_();
    void AddDrop_(const hmm_vec3& pos);

    // Per-strip Lissajous parameters (member arrays in the snapshot —
    // mystrip[j], r[j], th[j], h[j], dr[j], dth[j], dh[j], ac[j], dac[j]).
    float r_[kWindStripBespokeMaxStrips]   {};
    float th_[kWindStripBespokeMaxStrips]  {};
    float h_[kWindStripBespokeMaxStrips]   {};
    float dr_[kWindStripBespokeMaxStrips]  {};
    float dth_[kWindStripBespokeMaxStrips] {};
    float dh_[kWindStripBespokeMaxStrips]  {};
    float ac_[kWindStripBespokeMaxStrips]  {};
    float dac_[kWindStripBespokeMaxStrips] {};

    // Per-strip point ring buffer (head = newest). Snapshot uses
    // TStripAnimator's AddPoint / DelStartPoint; we replicate the FIFO.
    SWindStripBespokePoint strip_points_[kWindStripBespokeMaxStrips]
                                        [kWindStripBespokePoints] {};
    int32_t strip_count_[kWindStripBespokeMaxStrips] = {0, 0, 0};

    // Center pos + offset accumulators (snapshot center, realpos).
    hmm_vec3 center_   = {0.0f, 0.0f, 0.0f};
    hmm_vec3 realpos_  = {0.0f, 0.0f, 0.0f};
    int32_t  centerang_ = 0;
    float    haloscale_ = 0.0f;
    int32_t  frameon_   = 0;

    // Spark bucket — fixed-size heap array in the snapshot.
    SWindStripBespokeDrop drops_[kWindStripBespokeNumSparks] {};

    bool   alive_        = true;
    double sim_accum_ms_ = 0.0;
};

// =========================================================================
// * Wave-2B Weather B — large composite spawners (W01/W02/W03)             *
// *                                                                       *
// * These three effects (Meteor Storm / Tornado / Vortex) are large       *
// * composite animators that lean on infrastructure not yet available     *
// * in the bespoke port pipeline:                                         *
// *                                                                      *
// *   - W01 TMeteorStormAnimator delegates to TStormAnimator              *
// *     (effectcomp.{h,cpp}) for spawn/animate/render. The whole          *
// *     TStormAnimator body in effectcomp.cpp is gated `#if 0` (Phase-3   *
// *     sokol pipeline TODO). Without that subsystem ported there is     *
// *     no faithful path here.                                           *
// *                                                                      *
// *   - W02 TTornadoAnimator renders three multi-sub-object I3D mesh     *
// *     families (flames / tornado particles / ice chunks+splashes) via  *
// *     T3DAnimator::GetObject(N)+RenderObject(), with full per-instance *
// *     D3DMATRIX matrix stacks. The bespoke pipeline today only         *
// *     submits billboards/strips/lights — no mesh-RenderObject path.    *
// *                                                                      *
// *   - W03 TVortexAnimator does the same multi-sub-object mesh dance    *
// *     (sub-objects 0/1/2 + 36-vert glow + particle system) plus heavy  *
// *     per-vertex alpha writeback into LVERTEX.color (D3DRGBA). Same    *
// *     blocker.                                                         *
// *                                                                      *
// * Per batch protocol: stub SpawnForTest to return nullptr + log_warn,  *
// * mark blocked. The classes exist so the harness has something to      *
// * register against and a future port can flesh out the bodies         *
// * without touching the registration side.                              *
// =========================================================================

_CLASSDEF(TMeteorStormEffect_Bespoke)

class TMeteorStormEffect_Bespoke : public TEffect
{
  public:
    TMeteorStormEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TMeteorStormEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TMeteorStormEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TMeteorStormEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);
    [[nodiscard]] bool IsAlive() const { return true; }
};

_CLASSDEF(TTornadoEffect_Bespoke)

class TTornadoEffect_Bespoke : public TEffect
{
  public:
    TTornadoEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TTornadoEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TTornadoEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TTornadoEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);
    [[nodiscard]] bool IsAlive() const { return true; }
};

_CLASSDEF(TVortexEffect_Bespoke)

class TVortexEffect_Bespoke : public TEffect
{
  public:
    TVortexEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TVortexEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TVortexEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TVortexEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);
    [[nodiscard]] bool IsAlive() const { return true; }
};

// *************************************************************************
// * wave-bespoke-W2A: weather / ground-scatter family — fog / sandswirl /  *
// * quicksand. Faithful direct ports of the snapshot animator bodies for   *
// * the --test=vfx harness, mirroring the TBloodEffect_Bespoke pattern.    *
// * Each class loads its real .I3D imagery (no procedural stand-in per     *
// * feedback_no_standins) and runs the snapshot's per-tick state machine   *
// * LINE BY LINE against an internal state array. Pipeline + blend         *
// * preserved verbatim — SetBlendState in the snapshot means Alpha here;   *
// * SetAddBlendState means AdditiveStraight; no constants invented.        *
// *************************************************************************

_CLASSDEF(TFogEffect_Bespoke)

// W06: ambient ground fog overlay. Snapshot:
// src/effect_old.cpp:6621-6750 (TFogAnimator). 36-vertex flat grid (6x6)
// over a scl=7.475 quad, with per-vertex random color/alpha brightness
// drift on 20 "anchored" border verts (frozen at c=.7/a=.125) and the
// other 16 inner verts drifting through random color_velocity /
// alpha_velocity / position-delta walks. SetBlendState -> Alpha (verbatim,
// preserved AS WRITTEN — no AdditiveStraight reinterpretation).
//
// First-pass bespoke: collapse the 36-vertex animated grid down to a
// single ground-aligned (WorldXY) Alpha billboard whose color/alpha is
// the mean of the inner verts. Preserves the snapshot's brightness/alpha
// random walk shape (same min/max/velocity bounds) so the visual breathes
// at the same cadence; the per-vertex gradient is a follow-up upgrade
// once the engine has a triangulated-quad-with-vertex-color render path.
class TFogEffect_Bespoke : public TEffect
{
  public:
    TFogEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TFogEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TFogEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TFogEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);

    [[nodiscard]] bool IsAlive() const { return true; }   // ambient overlay — never dies

  private:
    // Snapshot constants (effect_old.cpp:6621-6750).
    static constexpr int32_t kFogVertex      = 36;   // FOG_VERTEX
    static constexpr float   kFogScale       = 7.475f;
    static constexpr int32_t kFogSimTickMs   = 1000 / 24;
    static constexpr float   kFogBaseSizeWu  = 96.0f * kFogScale;  // grid quad ~ 96 wu base * 7.475

    struct SFogVert
    {
        float c   = 0.7f;
        float a   = 0.125f;
        float cv  = 0.0f;     // color_velocity
        float av  = 0.0f;     // alpha_velocity
        hmm_vec3 dpos     = {0.0f, 0.0f, 0.0f};
        hmm_vec3 velocity = {0.0f, 0.0f, 0.0f};
        bool  anchored    = false;   // border vert (frozen) vs inner vert (drifting)
    };

    SFogVert       verts_[kFogVertex] {};
    double         sim_accum_ms_  = 0.0;
    TTextureHandle texture_       = kInvalidTexture;
    float          uv_rect_[4]    = {0.0f, 0.0f, 1.0f, 1.0f};
};

_CLASSDEF(TSandswirlEffect_Bespoke)

// W04: sandswirl spell-cast burst. Snapshot:
// src/effect_old.cpp:9476-9683 (TSandswirlAnimator). Spawns a child
// "sand" particle effect (TParticle3DAnimator) on Initialize with 50-100
// particles, sets seek-targets to (0,0,100) above the cast point. After
// SANDSWIRL_DURATION (75) the snapshot scans nearby characters and
// retargets the particles at them, also spawning child Quicksand effects.
// At frame SANDSWIRL_DURATION*4 (=300) the effect kills itself. Render()
// is fully commented-out in the snapshot — no own draws; visuals are
// entirely the child sand PE.
//
// First-pass bespoke: own particle pool (50 sand particles) running the
// snapshot's seek-target curve (z=100 ascending point), so the effect
// reads visually as the swirling sand updraft. Blend = AdditiveStraight
// by sister-family precedent (sand, dust, swirl PEs all SetAddBlendState
// in the snapshot family). Retarget-to-character logic is stubbed: in
// the harness there are no nearby characters, so on frame == SANDSWIRL_
// DURATION we keep the original (0,0,100) target. KillThisEffect at
// frame SANDSWIRL_DURATION*4 preserved.
class TSandswirlEffect_Bespoke : public TEffect
{
  public:
    TSandswirlEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TSandswirlEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TSandswirlEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TSandswirlEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);

    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    // Snapshot constants.
    static constexpr float   kSandswirlDuration = 75.0f;   // SANDSWIRL_DURATION
    static constexpr int32_t kSandMaxParticles  = 100;     // upper bound of random(50,100)
    static constexpr float   kSandSpawnZ        = 70.0f;   // pr.pos.z
    static constexpr float   kSandSpreadXY      = 50.0f;   // pr.pspread.x/y
    static constexpr float   kSandSpreadZ       = 50.0f;   // pr.pspread.z
    static constexpr float   kSandSeekTargetZ   = 100.0f;  // pr.targetpos[0].z
    static constexpr float   kSandSeekSpeedBase = 5.0f;    // pr.seekspeed
    static constexpr float   kSandTurnAng       = 0.5f;    // pr.turnang
    static constexpr int32_t kSandMinLife       = 100;     // pr.minlife
    static constexpr int32_t kSandMaxLife       = 150;     // pr.maxlife
    static constexpr int32_t kSandSimTickMs     = 1000 / 24;
    static constexpr float   kSandBaseSizeWu    = 32.0f;   // single sand mote

    struct SSandParticle
    {
        hmm_vec3 pos    = {0.0f, 0.0f, 0.0f};
        hmm_vec3 vel    = {0.0f, 0.0f, 0.0f};
        int32_t  life   = 0;
        bool     alive  = false;
        int32_t  start  = 0;     // delay until particle activates
    };

    SSandParticle  parts_[kSandMaxParticles] {};
    int32_t        numparticles_  = 0;
    int32_t        frameon_       = 0;
    int32_t        angle_         = 0;     // snapshot: inst->GetAngle()
    hmm_vec3       seektarget_    = {0.0f, 0.0f, kSandSeekTargetZ};
    bool           alive_         = true;
    TTextureHandle texture_       = kInvalidTexture;
    float          uv_rect_[4]    = {0.0f, 0.0f, 1.0f, 1.0f};
    double         sim_accum_ms_  = 0.0;
};

_CLASSDEF(TQuicksandEffect_Bespoke)

// W05: quicksand ground trap. Snapshot:
// src/effect_old.cpp:9137-9474 (TQuicksandAnimator at lines 9137-9442 +
// REGISTER + Pulse at TQuicksandEffect:9004-9122). A duplicate body
// exists at 12520-12836 inside /***** Old Quicksand: Redone by Pepper *****/
// comment block — DEAD (commented-out); the canonical body is at line
// 9194.
//
// The snapshot uses three sub-objects on a 3-mesh I3D:
//   GetObject(0) — spinning sand decal (4-frame UV-flip texture), scaled
//                  by `scalesize` (animated), rotated by `ang` (+=0.06/tick).
//   GetObject(1) — twin cylinders, scaled by (cylscale, cylheight), one
//                  rotated by `cylrot`, the other by M_PI - cylrot.
//   GetObject(2) — dust mesh (34 verts) U-scrolling at -0.06/tick, rendered
//                  in 3 concentric scales: (scalesize), (scalesize/1.25),
//                  (scalesize/1.5), (scalesize/1.75). zscalefactor ramps
//                  down after QUICKSAND_DURATION*2.
//
// SetBlendState -> Alpha (verbatim, preserved AS WRITTEN). Render does
// SetBlendState a second time mid-body which is a no-op (alpha->alpha).
//
// First-pass bespoke: ONE ground-aligned (WorldXY) Alpha billboard whose
// scale follows the animated `scalesize` curve and orientation follows
// `ang`. Cylinder mesh + 4-stack dust passes are deferred to a follow-up
// (engine doesn't have axis-aligned-cylinder rendering today; future
// upgrade once mesh submission path exposes scale-with-rotation).
// scalesize / stage / count animation curve preserved verbatim from
// snapshot Animate (line 9194-9236).
class TQuicksandEffect_Bespoke : public TEffect
{
  public:
    TQuicksandEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TQuicksandEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TQuicksandEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TQuicksandEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);

    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    // Snapshot constants (effect_old.cpp:8993-8997 — canonical body).
    static constexpr int32_t kQuicksandDuration  = 40;     // QUICKSAND_DURATION
    static constexpr float   kQuicksandSwitch    = 10.0f;  // QUICKSAND_SWITCH
    static constexpr float   kQuicksandCylRot    = 0.5f;   // QUICKSAND_CYLROT
    static constexpr float   kQuicksandSpinStart = 10.0f;  // QUICKSAND_SPIN_START
    static constexpr int32_t kQuicksandSimTickMs = 1000 / 24;
    static constexpr float   kQuicksandBaseSizeWu = 128.0f;  // ground decal footprint

    // Internal state — direct mapping of TQuicksandAnimator privates.
    float    scalesize_    = 0.0f;
    float    cylscale_     = 0.0f;
    float    cylheight_    = 1.0f;
    float    cylrot_       = 0.0f;
    int32_t  cylcount_     = 0;
    int32_t  stage_        = 0;
    int32_t  count_        = 0;
    float    ang_          = 0.0f;
    float    zscalefactor_ = 1.0f;
    int32_t  frameon_      = 0;
    bool     alive_        = true;
    double   sim_accum_ms_ = 0.0;

    TTextureHandle texture_    = kInvalidTexture;
    float          uv_rect_[4] = {0.0f, 0.0f, 1.0f, 1.0f};
};

// =========================================================================
// Wave-3 W3-F: Buff overlays (character-attached state effects)
// -------------------------------------------------------------------------
// Five retail-only effects with NO snapshot source body and effectively no
// per-effect Ghidra class identification:
//
//   - Might         magic\might.i3d        s_Might_*    : 1 ambiguous XREF
//   - Stoneskin     magic\Stone.i3d        s_Stone_*    : 2 ambiguous XREFs
//                                                         (could be terrain)
//   - Invisible     Magic\Invisible.I3D    s_invisible_*: 11 XREFs but
//                                                         spread across
//                                                         combat/AI invis
//                                                         flag plus the
//                                                         effect
//   - charm         magic\Charm.i3d        none in _data.txt — asset only
//   - speed         magic\Speed.i3d        no clear XREF — asset only
//
// The original Revenant buff family likely shared ONE TBuffEffect or
// TAuraEffect animator with color/asset overrides per buff (the asset-only
// registrations support a data-driven dispatch). The closest base in the
// snapshot is TAuraAnimator (Magic\aura.i3d, AURA_COUNT=100 particles); see
// TAuraEffect_Bespoke at src/effect.cpp:8572 for the faithful port.
//
// Without per-effect Ghidra evidence and without snapshot bodies, each of
// the 5 effects below ships as a MINIMAL PLACEHOLDER (status=stubbed):
//
//   - SpawnForTest loads the verbatim asset path via TryLoadMagicTexture.
//   - TickAndSubmit draws ONE ScreenAligned Alpha billboard at the effect
//     origin with a per-effect color tint sampled from the buff's visual
//     family (gold for Might, grey for Stoneskin, cyan-shimmer for
//     Invisible, pink for charm, blue-streak for speed).
//   - A simple `lifetime_ms_` timer self-terminates after ~6s so the
//     harness can re-trigger via the SpellGround cadence.
//
// Each effect gets a distinct C++ type so the bespoke dispatch / future
// per-effect tuning has a hook; collapsing them into a single
// TBuffEffect_Bespoke type would lose the harness A/B identity. When the
// user has video footage, per-effect tuning can land here without touching
// the shared infra.
//
// Once the shared TBuffEffect / TAuraEffect base is identified in Ghidra
// (and a real animator is ported), these classes should become thin
// adapters around it. For now: bespoke placeholders so each row has a
// boot-able harness entry.

_CLASSDEF(TBuffEffect_Bespoke__Might)

class TBuffEffect_Bespoke__Might : public TEffect
{
  public:
    TBuffEffect_Bespoke__Might(TObjectImagery* newim) : TEffect(newim) {}
    TBuffEffect_Bespoke__Might(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TBuffEffect_Bespoke__Might() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TBuffEffect_Bespoke__Might* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);
    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    bool   alive_         = true;
    double age_ms_        = 0.0;
    double sim_accum_ms_  = 0.0;
    TTextureHandle texture_    = kInvalidTexture;
    float          uv_rect_[4] = {0.0f, 0.0f, 1.0f, 1.0f};
};

_CLASSDEF(TBuffEffect_Bespoke__Stoneskin)

class TBuffEffect_Bespoke__Stoneskin : public TEffect
{
  public:
    TBuffEffect_Bespoke__Stoneskin(TObjectImagery* newim) : TEffect(newim) {}
    TBuffEffect_Bespoke__Stoneskin(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TBuffEffect_Bespoke__Stoneskin() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TBuffEffect_Bespoke__Stoneskin* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);
    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    bool   alive_         = true;
    double age_ms_        = 0.0;
    double sim_accum_ms_  = 0.0;
    TTextureHandle texture_    = kInvalidTexture;
    float          uv_rect_[4] = {0.0f, 0.0f, 1.0f, 1.0f};
};

_CLASSDEF(TInvisibleEffect_Bespoke)

class TInvisibleEffect_Bespoke : public TEffect
{
  public:
    TInvisibleEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TInvisibleEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TInvisibleEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TInvisibleEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);
    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    bool   alive_         = true;
    double age_ms_        = 0.0;
    double sim_accum_ms_  = 0.0;
    TTextureHandle texture_    = kInvalidTexture;
    float          uv_rect_[4] = {0.0f, 0.0f, 1.0f, 1.0f};
};

_CLASSDEF(TBuffEffect_Bespoke__charm)

class TBuffEffect_Bespoke__charm : public TEffect
{
  public:
    TBuffEffect_Bespoke__charm(TObjectImagery* newim) : TEffect(newim) {}
    TBuffEffect_Bespoke__charm(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TBuffEffect_Bespoke__charm() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TBuffEffect_Bespoke__charm* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);
    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    bool   alive_         = true;
    double age_ms_        = 0.0;
    double sim_accum_ms_  = 0.0;
    TTextureHandle texture_    = kInvalidTexture;
    float          uv_rect_[4] = {0.0f, 0.0f, 1.0f, 1.0f};
};

_CLASSDEF(TBuffEffect_Bespoke__speed)

class TBuffEffect_Bespoke__speed : public TEffect
{
  public:
    TBuffEffect_Bespoke__speed(TObjectImagery* newim) : TEffect(newim) {}
    TBuffEffect_Bespoke__speed(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TBuffEffect_Bespoke__speed() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TBuffEffect_Bespoke__speed* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);
    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    bool   alive_         = true;
    double age_ms_        = 0.0;
    double sim_accum_ms_  = 0.0;
    TTextureHandle texture_    = kInvalidTexture;
    float          uv_rect_[4] = {0.0f, 0.0f, 1.0f, 1.0f};
};

// =========================================================================
// * wave-3 W3-E: Spell-only effects (asset-only, no dedicated class).      *
// *                                                                       *
// * Five retail-shipped spell visuals (cataclysm, funnel, maelstrom,      *
// * magicshield, Nakrnoth) whose Class.Def registrations point at named   *
// * I3D assets in the retail data tree (Magic\Cataclysm.I3D, etc.) but   *
// * whose game-side classes are SPELL-INIT STUBS — the Ghidra decomp for *
// * each is a single-statement ctor that flips one default member        *
// * (cls_0x4fe9e0 / cls_0x4fd890 / cls_0x502420 etc.). All of the actual *
// * animator logic lives in a shared TStormAnimator / TMeteorStormAnimator *
// * / TShieldEffect parent, which is either already covered (TShieldEffect, *
// * X09 + W3-D wave) or BLOCKED on a sokol-pipeline TODO (W01            *
// * TMeteorStormEffect_Bespoke is `#if 0`'d).                            *
// *                                                                       *
// * The I3D assets themselves are not present in the local                *
// * RevenantRepo/Imagery/Magic tree — they ship only with the retail     *
// * data drop. The five bespoke classes therefore ship as MINIMAL        *
// * PLACEHOLDERS per batch protocol: SpawnForTest_BESPOKE attempts to    *
// * load the named .I3D candidate set, and TickAndSubmit draws a single  *
// * ScreenAligned Alpha billboard with the asset's first sub-object      *
// * texture if it resolved (otherwise no submission, log_warn). Status   *
// * is `stubbed` — the harness has a row to A/B against retail game      *
// * video; the body can be fleshed out once the user provides reference  *
// * video + I3D assets show up in the local data tree.                   *
// *                                                                       *
// * Per batch brief / feedback_no_standins: we do NOT draw procedural    *
// * stand-ins for these named real effects. If the asset doesn't         *
// * resolve, we draw nothing and log_warn.                                *
// =========================================================================

_CLASSDEF(TCataclysmEffect_Bespoke)

// W3-E #1: "cataclysm" — spell registered via Class.Def to Magic\Cataclysm.I3D.
// Ghidra evidence: s_Cataclysm_005e13a0 XREFs at 004fe7c0 / 004fe7e0 are
// inside cls_0x4fe9e0 (spell-init region) whose ctor body just clears a
// single member (mbr_0x190 = 1) — no animator logic. Parent of W3-D's
// YCataclysm. Likely a meteor-storm variant whose visual lives in the
// shared TMeteorStormAnimator (W01, BLOCKED on TStormAnimator #if 0).
class TCataclysmEffect_Bespoke : public TEffect
{
  public:
    TCataclysmEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TCataclysmEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TCataclysmEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TCataclysmEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);

    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    static constexpr float   kCataclysmBaseSizeWu = 96.0f;   // ground burst footprint
    static constexpr int32_t kCataclysmLifeMs     = 4000;    // 4s default lifetime

    bool           alive_       = true;
    double         age_ms_      = 0.0;
    TTextureHandle texture_     = kInvalidTexture;
    float          uv_rect_[4]  = {0.0f, 0.0f, 1.0f, 1.0f};
};

_CLASSDEF(TFunnelEffect_Bespoke)

// W3-E #2: "funnel" — Magic\Funnel.I3D. Ghidra evidence at 004fd3b0 /
// 004fd3d0 is the spell-init region; no dedicated animator class. Likely
// a tornado-funnel variant whose visual lives in TTornadoEffect (W02,
// BLOCKED on multi-sub-object I3D mesh path). Asset-only placeholder.
class TFunnelEffect_Bespoke : public TEffect
{
  public:
    TFunnelEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TFunnelEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TFunnelEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TFunnelEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);

    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    static constexpr float   kFunnelBaseSizeWu = 128.0f;   // funnel column footprint
    static constexpr int32_t kFunnelLifeMs     = 5000;     // 5s default lifetime

    bool           alive_       = true;
    double         age_ms_      = 0.0;
    TTextureHandle texture_     = kInvalidTexture;
    float          uv_rect_[4]  = {0.0f, 0.0f, 1.0f, 1.0f};
};

_CLASSDEF(TMaelstromEffect_Bespoke)

// W3-E #3: "maelstrom" — magic\maelstrom.i3d. NO XREF found in
// recon/ghidra/_data.txt — pure Class.Def registration with no game-side
// class body. Parent of W3-D's ymaelstrom. Likely a swirling water/energy
// vortex whose visual lives in TVortexEffect (W03, BLOCKED).
class TMaelstromEffect_Bespoke : public TEffect
{
  public:
    TMaelstromEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TMaelstromEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TMaelstromEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TMaelstromEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);

    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    static constexpr float   kMaelstromBaseSizeWu = 144.0f;   // swirl footprint
    static constexpr int32_t kMaelstromLifeMs     = 5000;     // 5s default lifetime

    bool           alive_       = true;
    double         age_ms_      = 0.0;
    TTextureHandle texture_     = kInvalidTexture;
    float          uv_rect_[4]  = {0.0f, 0.0f, 1.0f, 1.0f};
};

_CLASSDEF(TMagicShieldEffect_Bespoke)

// W3-E #4: "magicshield" — Magic\Mshield.I3D. Ghidra has 4 string XREFs
// from cls_0x502420 (spell-init region) for "MagicShield" / "MagicShield2" /
// "MagicShield3" — 3-tier variant set, all sharing the same init class. The
// init ctor (meth_0x502420) sets mbr_0x184 = 0 / mbr_0x188 = 0xf and
// calls a base init via vtable[0x158] — no animator body. The actual visual
// is TShieldAnimator (already ported as TShieldEffect_Bespoke, X09) with a
// different I3D asset. First-pass: try Mshield.I3D, fall through to ALPHA
// billboard with a pale-blue tint like TShieldEffect, but ScreenAligned.
class TMagicShieldEffect_Bespoke : public TEffect
{
  public:
    TMagicShieldEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TMagicShieldEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TMagicShieldEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TMagicShieldEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);

    [[nodiscard]] bool IsAlive() const { return true; }   // shield = persistent buff

  private:
    // Mirror TShieldEffect_Bespoke constants/layout — magicshield is the
    // same animator with a different I3D.
    static constexpr float   kMagicShieldBaseSizeWu = 64.0f;
    static constexpr float   kMagicShieldScale      = 2.0f;   // SHIELD_SCALE
    static constexpr float   kMagicShieldLiftZ      = 40.0f;  // pos.z=40

    int32_t        framenum_   = 0;
    TTextureHandle texture_    = kInvalidTexture;
    float          uv_rect_[4] = {0.0f, 0.0f, 1.0f, 1.0f};
};

_CLASSDEF(TNakrnothEffect_Bespoke)

// W3-E #5: "Nakrnoth" — magic\Nakrnoth.I3D. NO XREF in
// recon/ghidra/_data.txt — boss-specific (Nakrnoth = dragon boss) asset-
// only registration. Probably a one-off scripted spell visual whose
// owning class hasn't been found yet. First-pass placeholder: try to
// load the I3D and draw the first sub-object as an Alpha billboard.
class TNakrnothEffect_Bespoke : public TEffect
{
  public:
    TNakrnothEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TNakrnothEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TNakrnothEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TNakrnothEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);

    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    static constexpr float   kNakrnothBaseSizeWu = 96.0f;    // boss-spell footprint
    static constexpr int32_t kNakrnothLifeMs     = 4000;     // 4s default lifetime

    bool           alive_       = true;
    double         age_ms_      = 0.0;
    TTextureHandle texture_     = kInvalidTexture;
    float          uv_rect_[4]  = {0.0f, 0.0f, 1.0f, 1.0f};
};

// =========================================================================
// * Wave-3 W3-D Y-prefix boss-variant bespoke stubs                        *
// *                                                                       *
// * Five retail-only Y-prefix effects (Yhagoro / boss-variant assets).    *
// * No snapshot source body. Ghidra evidence is THIN for four of the      *
// * five — only TYFireBallEffect has a dedicated cls_0x5b4814 candidate   *
// * (with the same vtable shape as cls_0x5b4290 = base FireBall, i.e.     *
// * "same code, different asset"). The other four (YFireWind, YCataclysm,*
// * YManadrain, ymaelstrom) only show string XREFs in spell-handler       *
// * routines — no dedicated class layout to port.                         *
// *                                                                       *
// * Per the batch protocol, each ships as a MINIMAL placeholder: load     *
// * the I3D asset (path verbatim from /tmp/retail_effect_inventory.tsv),  *
// * draw the first sub-object as a ScreenAligned Alpha billboard, with a *
// * default lifetime. Status = stubbed. Bodies are intentionally tiny —   *
// * forensics-without-decomp is fabrication; the placeholders give the    *
// * harness a row to A/B against game video once the user captures one.   *
// *                                                                       *
// * Each class follows the lightweight stub pattern: nullptr-imagery      *
// * ctor + TryLoadMagicTexture-style asset load, single-billboard tick.   *
// *************************************************************************

// W3-D YFireBall — Yhagoro fireball variant.
// Ghidra: cls_0x5b4814, MEDIUM confidence (recon/mappings/
// TYFireBallEffect_cls_0x5b4814_candidate.yaml). Same vtable shape and
// self-allocated size (0x4cc bytes) as cls_0x5b4290 (base FireBall) — Y
// variant is functionally identical with overridden asset. Asset
// Magic\YFireBall.I3D (yellow/large fireball).
//
// Stub renders a single ScreenAligned Alpha billboard sized like a small
// fireball. Behavioral parity with the base TFireBallEffect (which is
// already fully ported at effect.cpp:6694+) is left for a follow-up that
// either subclasses TFireBallEffect or accepts an asset_override on its
// SpawnForTest — that wiring is non-trivial because TFireBallEffect's
// Init bakes Magic\NewFireBall.I3D into its sub-object resolution.
_CLASSDEF(TFireBallEffect_Bespoke__YFireBall)
class TFireBallEffect_Bespoke__YFireBall : public TEffect
{
  public:
    TFireBallEffect_Bespoke__YFireBall(TObjectImagery* newim) : TEffect(newim) {}
    TFireBallEffect_Bespoke__YFireBall(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TFireBallEffect_Bespoke__YFireBall() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TFireBallEffect_Bespoke__YFireBall* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);
    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    static constexpr int32_t kStubLifetimeMs   = 2500;
    static constexpr float   kStubBaseSizeWu   = 24.0f;
    static constexpr int32_t kStubSimTickMs    = 1000 / 24;

    int32_t  age_ms_       = 0;
    bool     alive_        = true;
    double   sim_accum_ms_ = 0.0;
    TTextureHandle texture_    = kInvalidTexture;
    float          uv_rect_[4] = {0.0f, 0.0f, 1.0f, 1.0f};
};

// W3-D YFireWind — boss-variant firewind. No dedicated class candidate; only
// string XREFs in spell-handler area. Asset Magic\YFirewind.I3D.
// Minimal placeholder — single ScreenAligned Alpha billboard.
_CLASSDEF(TFireWindEffect_Bespoke__YFireWind)
class TFireWindEffect_Bespoke__YFireWind : public TEffect
{
  public:
    TFireWindEffect_Bespoke__YFireWind(TObjectImagery* newim) : TEffect(newim) {}
    TFireWindEffect_Bespoke__YFireWind(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TFireWindEffect_Bespoke__YFireWind() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TFireWindEffect_Bespoke__YFireWind* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);
    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    static constexpr int32_t kStubLifetimeMs   = 3000;
    static constexpr float   kStubBaseSizeWu   = 32.0f;
    static constexpr int32_t kStubSimTickMs    = 1000 / 24;

    int32_t  age_ms_       = 0;
    bool     alive_        = true;
    double   sim_accum_ms_ = 0.0;
    TTextureHandle texture_    = kInvalidTexture;
    float          uv_rect_[4] = {0.0f, 0.0f, 1.0f, 1.0f};
};

// W3-D YCataclysm — boss-variant cataclysm. Ghidra XREF for s_YCataclysm_
// at 0x005e1520 lives in virt_meth_0x507530 (spell handler), no dedicated
// class. Base Cataclysm is similarly spell-only (no class). Asset
// Magic\YCataclysm.I3D. Minimal placeholder.
_CLASSDEF(TCataclysmEffect_Bespoke__YCataclysm)
class TCataclysmEffect_Bespoke__YCataclysm : public TEffect
{
  public:
    TCataclysmEffect_Bespoke__YCataclysm(TObjectImagery* newim) : TEffect(newim) {}
    TCataclysmEffect_Bespoke__YCataclysm(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TCataclysmEffect_Bespoke__YCataclysm() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TCataclysmEffect_Bespoke__YCataclysm* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);
    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    static constexpr int32_t kStubLifetimeMs   = 4000;
    static constexpr float   kStubBaseSizeWu   = 64.0f;
    static constexpr int32_t kStubSimTickMs    = 1000 / 24;

    int32_t  age_ms_       = 0;
    bool     alive_        = true;
    double   sim_accum_ms_ = 0.0;
    TTextureHandle texture_    = kInvalidTexture;
    float          uv_rect_[4] = {0.0f, 0.0f, 1.0f, 1.0f};
};

// W3-D YManadrain — boss-variant mana-drain tether/beam. Ghidra XREF for
// s_YManaDrain_005e1550 lives in virt_meth_0x5082c0 (spell handler); base
// ManaDrain has XREFs in cls handler 0x4fbf50 but no clean class body
// candidate identified. Asset Magic\YManadrain.I3D. Minimal placeholder.
_CLASSDEF(TManadrainEffect_Bespoke__YManadrain)
class TManadrainEffect_Bespoke__YManadrain : public TEffect
{
  public:
    TManadrainEffect_Bespoke__YManadrain(TObjectImagery* newim) : TEffect(newim) {}
    TManadrainEffect_Bespoke__YManadrain(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TManadrainEffect_Bespoke__YManadrain() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TManadrainEffect_Bespoke__YManadrain* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);
    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    static constexpr int32_t kStubLifetimeMs   = 3000;
    static constexpr float   kStubBaseSizeWu   = 20.0f;
    static constexpr int32_t kStubSimTickMs    = 1000 / 24;

    int32_t  age_ms_       = 0;
    bool     alive_        = true;
    double   sim_accum_ms_ = 0.0;
    TTextureHandle texture_    = kInvalidTexture;
    float          uv_rect_[4] = {0.0f, 0.0f, 1.0f, 1.0f};
};

// W3-D ymaelstrom — boss-variant maelstrom vortex. Pure asset-only (no
// s_maelstrom_/s_ymaelstrom_ XREF in _data.txt — registered via class.def).
// Asset magic\ymaelstrom.i3d (lowercase, preserved verbatim from
// /tmp/retail_effect_inventory.tsv). Minimal placeholder.
_CLASSDEF(TMaelstromEffect_Bespoke__ymaelstrom)
class TMaelstromEffect_Bespoke__ymaelstrom : public TEffect
{
  public:
    TMaelstromEffect_Bespoke__ymaelstrom(TObjectImagery* newim) : TEffect(newim) {}
    TMaelstromEffect_Bespoke__ymaelstrom(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TMaelstromEffect_Bespoke__ymaelstrom() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TMaelstromEffect_Bespoke__ymaelstrom* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);
    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    static constexpr int32_t kStubLifetimeMs   = 5000;
    static constexpr float   kStubBaseSizeWu   = 96.0f;
    static constexpr int32_t kStubSimTickMs    = 1000 / 24;

    int32_t  age_ms_       = 0;
    bool     alive_        = true;
    double   sim_accum_ms_ = 0.0;
    TTextureHandle texture_    = kInvalidTexture;
    float          uv_rect_[4] = {0.0f, 0.0f, 1.0f, 1.0f};
};

// *************************************************************************
// * Wave-3 W3-A Dragon/Fire bespokes (Blast, FireFlash, FireWind,         *
// * FireCone, Faultfire) — retail-only effects.                            *
// *                                                                       *
// * Status summary (see effect.cpp bodies for per-class detail):           *
// *   TBlastEffect_Bespoke      — STUBBED (no animator, no Ghidra body)    *
// *   TFireFlashEffect_Bespoke  — STUBBED (snapshot animator depends on    *
// *                                PTSpell/PTCharacter rig not in test     *
// *                                harness; Ghidra cls_0x5a9194 is a       *
// *                                merged composite, untrustworthy)        *
// *   TFireWindEffect_Bespoke   — STUBBED (same dependency profile;        *
// *                                supports YFireWind variant via asset    *
// *                                override)                               *
// *   TFireConeEffect_Bespoke   — STUBBED (TParticleSystem fire+smoke+     *
// *                                burst tri-system; not yet harness-      *
// *                                ready; supports dragonfire variant)     *
// *   TFaultFireEffect_Bespoke  — PORTED (small self-contained UV-scroll   *
// *                                + scale-cosine pulse animator; faithful *
// *                                first-pass billboard approximation)     *
// *                                                                       *
// * All five register a SpawnForTest_BESPOKE that loads the cited .I3D     *
// * asset, and a TickAndSubmitForTest_BESPOKE that draws ONE Alpha         *
// * ScreenAligned (FaultFire: WorldXY) billboard using the asset's first   *
// * mapped texture. Awaiting user video A/B for kinematic tuning.          *
// *************************************************************************

_CLASSDEF(TBlastEffect_Bespoke)

class TBlastEffect_Bespoke : public TEffect
{
  public:
    TBlastEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TBlastEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TBlastEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TBlastEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void                                       TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);
    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    static constexpr float   kBlastBaseSizeWu = 48.0f;
    static constexpr int32_t kBlastLifetimeMs = 1500;

    TTextureHandle texture_      = kInvalidTexture;
    float          age_ms_       = 0.0f;
    bool           alive_        = true;
};

_CLASSDEF(TFireFlashEffect_Bespoke)

class TFireFlashEffect_Bespoke : public TEffect
{
  public:
    TFireFlashEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TFireFlashEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TFireFlashEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TFireFlashEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void                                           TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);
    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    static constexpr float   kFireFlashBaseSizeWu = 32.0f;
    static constexpr int32_t kFireFlashLifetimeMs = 1600;

    TTextureHandle texture_      = kInvalidTexture;
    float          age_ms_       = 0.0f;
    bool           alive_        = true;
};

_CLASSDEF(TFireWindEffect_Bespoke)

class TFireWindEffect_Bespoke : public TEffect
{
  public:
    TFireWindEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TFireWindEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TFireWindEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    // asset_override supports the YFireWind sibling variant (Magic\YFirewind.I3D)
    // — Y-prefix in the snapshot is the "yellow" tint variant; same animator.
    [[nodiscard]] static TFireWindEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin,
                                                                       const char*     asset_override = nullptr);
    void                                          TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);
    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    static constexpr float   kFireWindBaseSizeWu = 40.0f;
    static constexpr int32_t kFireWindLifetimeMs = 2000;

    TTextureHandle texture_      = kInvalidTexture;
    float          age_ms_       = 0.0f;
    bool           alive_        = true;
};

_CLASSDEF(TFireConeEffect_Bespoke)

class TFireConeEffect_Bespoke : public TEffect
{
  public:
    TFireConeEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TFireConeEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TFireConeEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    // asset_override supports dragonfire (same FireCone.I3D under a different
    // spell name) and any future cone-shape sibling.
    [[nodiscard]] static TFireConeEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin,
                                                                       const char*     asset_override = nullptr);
    void                                          TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);
    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    static constexpr float   kFireConeBaseSizeWu = 56.0f;
    static constexpr int32_t kFireConeLifetimeMs = 1800;

    TTextureHandle texture_      = kInvalidTexture;
    float          age_ms_       = 0.0f;
    bool           alive_        = true;
};

_CLASSDEF(TFaultFireEffect_Bespoke)

// Faithful first-pass port of TFaultFireAnimator
// (src/effect_old.cpp:11145-11225). Self-contained: no PTSpell/PTCharacter
// dependency, no particle-system manager. Animator state is:
//   th   += FF_STEP (=0.1)  every tick, wrapped at 2π
//   per-tick: du = random(2..8) / 100, accumulate tu on every vertex
//   render: 2 passes, each with scale = 0.125 * (cos(th + i*π/2) + 7)
//           (range ≈ 0.75..1.0); apply tv = base_tv * scale per vert.
//
// First-pass drift: full mesh vertex submission (UV scroll, per-vert
// tv-scale, two-pass overdraw) requires a per-effect mesh path we don't
// yet expose. We collapse to ONE WorldXY billboard whose size pulses on the
// same cos(th)+7 envelope, with a continuous UV-X scroll on the harness's
// uv_rect.x to approximate the per-tick tu accumulation. Faithful version
// pending mesh-submission support.
class TFaultFireEffect_Bespoke : public TEffect
{
  public:
    TFaultFireEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TFaultFireEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TFaultFireEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TFaultFireEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void                                           TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);
    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    // Snapshot constants (effect_old.cpp:11143 + body).
    static constexpr float   kFFStep         = 0.1f;       // FF_STEP
    static constexpr int32_t kFFSimTickMs    = 1000 / 24;  // 24Hz sim-tick gate
    static constexpr float   kFFBaseSizeWu   = 48.0f;      // billboard footprint
    static constexpr int32_t kFFLifetimeMs   = 4000;       // harness display window
                                                            // (snapshot is persistent;
                                                            // we self-kill so it cycles)

    float          th_           = 0.0f;     // animator's th
    float          tu_scroll_    = 0.0f;     // accumulated tu (mod 1)
    float          age_ms_       = 0.0f;
    double         sim_accum_ms_ = 0.0;
    TTextureHandle texture_      = kInvalidTexture;
    bool           alive_        = true;
};

// =========================================================================
// * Wave-3 W3-B — Arrow/projectile family (5 retail-only effects)        *
// *   Forensics: thin Ghidra evidence (cls_0x5b0cfc has decompiled       *
// *   ctor/dtor/vtbl helpers but no Animate/Render body; the other 4    *
// *   effects have no dedicated class decomp). Each effect ships as a   *
// *   minimal SpawnForTest + Tick body that loads its named I3D and     *
// *   draws sub-object 0 as a ScreenAligned Alpha billboard with a      *
// *   sensible default lifetime. Awaiting user video A/B for kinematic  *
// *   tuning. Pattern mirrors TBloodEffect_Bespoke template.            *
// =========================================================================

_CLASSDEF(TArrowEffect_Bespoke)
// W3-B-1 — arroweffect / queenarrow. Both retail strings resolve to the
// same Ghidra class cls_0x5b0cfc (288 bytes, 5 vtbl methods, registrar
// fn 0x503740 attaches to caster's 'rhand' bone). Asset is
// misc\Arroweffects.I3D — shared between the two variants. The
// queenarrow variant (TArrowEffect_Bespoke__queenarrow) is the boss-fight
// (Queen Mahara) arrow, same asset with a red tint. Constructor at
// 0x503740 sets mbr_0x114 = 100.0f (likely initial speed/range) and zeros
// mbr_0x118/0x11c.
class TArrowEffect_Bespoke : public TEffect
{
  public:
    TArrowEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TArrowEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TArrowEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TArrowEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);

    [[nodiscard]] bool IsAlive() const { return alive_; }

    // Variant tint (queenarrow override sets this red).
    float tint_rgba_[4] = {1.0f, 1.0f, 1.0f, 1.0f};

  private:
    // Default 24Hz sim cadence + ~1.0s lifetime placeholder. Real lifetime
    // (snapshot Pulse-driven) unknown without a richer decomp.
    static constexpr int32_t kSimTickMs       = 1000 / 24;
    static constexpr int32_t kLifeTicks       = 24;     // ~1.0s placeholder
    static constexpr float   kBaseSizeWu      = 48.0f;  // billboard footprint

    int32_t  ticks_        = 0;
    bool     alive_        = true;
    double   sim_accum_ms_ = 0.0;

    TTextureHandle texture_    = kInvalidTexture;
    float          uv_rect_[4] = {0.0f, 0.0f, 1.0f, 1.0f};
};

_CLASSDEF(TSparksEffect_Bespoke)
// W3-B-3 — Sparks. Generic impact-spark effect (asset Misc\Sparks.I3D).
// Six string XREFs in the global string table; spawned from TCharacter
// PostEvent at combat-hit frame. No dedicated class decomp — treated as
// a brief ground-spark burst. Could collapse onto TStreamerEffect_Bespoke
// in a future pass; first cut keeps it as its own _Bespoke for separable
// harness rows.
class TSparksEffect_Bespoke : public TEffect
{
  public:
    TSparksEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TSparksEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TSparksEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TSparksEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);

    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    static constexpr int32_t kSimTickMs  = 1000 / 24;
    static constexpr int32_t kLifeTicks  = 16;     // ~0.66s sparks burst
    static constexpr float   kBaseSizeWu = 32.0f;

    int32_t  ticks_        = 0;
    bool     alive_        = true;
    double   sim_accum_ms_ = 0.0;

    TTextureHandle texture_    = kInvalidTexture;
    float          uv_rect_[4] = {0.0f, 0.0f, 1.0f, 1.0f};
};

_CLASSDEF(TCombatFlashEffect_Bespoke)
// W3-B-4 — combatflash. Triggered from TCharacter combat code at hit-
// frame (cls_0x5a7b98 method 0x4c8500 builds a NewObjectByName request
// with type=0x19 at character pos + (+0x1e, +0x50, +0x1e)). Asset is
// misc\Impact.i3d — shared hit-flash imagery. Brief bright additive
// flash; lifetime <0.5s.
class TCombatFlashEffect_Bespoke : public TEffect
{
  public:
    TCombatFlashEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TCombatFlashEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TCombatFlashEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TCombatFlashEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);

    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    static constexpr int32_t kSimTickMs  = 1000 / 24;
    static constexpr int32_t kLifeTicks  = 10;     // ~0.42s flash
    static constexpr float   kBaseSizeWu = 56.0f;

    int32_t  ticks_        = 0;
    bool     alive_        = true;
    double   sim_accum_ms_ = 0.0;

    TTextureHandle texture_    = kInvalidTexture;
    float          uv_rect_[4] = {0.0f, 0.0f, 1.0f, 1.0f};
};

_CLASSDEF(TStrikeEffect_Bespoke)
// W3-B-5 — StrikeEffect. Asset is Misc\Dummy.i3d (placeholder/no-imagery
// marker). Likely audio-only or script-trigger effect with no visible
// body. Stubbed: SpawnForTest still allocates the effect (so it shows
// up in the harness cycle) but Tick draws nothing.
class TStrikeEffect_Bespoke : public TEffect
{
  public:
    TStrikeEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TStrikeEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TStrikeEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TStrikeEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);

    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    static constexpr int32_t kSimTickMs = 1000 / 24;
    static constexpr int32_t kLifeTicks = 12;     // ~0.5s marker lifetime

    int32_t ticks_        = 0;
    bool    alive_        = true;
    double  sim_accum_ms_ = 0.0;
};

// *************************************************************************
// * Wave 3 batch W3-C: Cave / environment ambient                         *
// *                                                                       *
// *   sgeyser/fgeyser TGeyserEffect_Bespoke — Steam / Fire periodic geyser*
// *                                           spout. Retail Ghidra evid:  *
// *                                           cls_0x5b79ac (348-byte      *
// *                                           class) is a TParticle3D-    *
// *                                           Animator subclass; the      *
// *                                           registrar at FUN_00526780   *
// *                                           differentiates 'CavFGeyser' *
// *                                           (fire) and 'CavSGeyser'     *
// *                                           (steam) by name and selects *
// *                                           'fgeyser'/'sgeyser' sound   *
// *                                           cue (Sound/effects/         *
// *                                           {f,s}geyser.wav are on      *
// *                                           disk). The 100-iter loop on *
// *                                           meth_0x526040 (a TEffect    *
// *                                           sub-object spawn) implies   *
// *                                           ~100 particles per emission *
// *                                           burst. Snapshot has no body *
// *                                           — synthesis: periodic       *
// *                                           upward spurt (gravity-      *
// *                                           ballistic particles)        *
// *                                           tinted by variant.          *
// *                                                                       *
// *   cfire TFlameAnimator_Bespoke__cfire — campfire ambient flame. No   *
// *                                          Ghidra match, no asset on    *
// *                                          disk in legacy/data. Likely  *
// *                                          a TFlameAnimator variant     *
// *                                          with a different I3D, but    *
// *                                          asset absent. STUB: spawn    *
// *                                          returns nullptr with         *
// *                                          diagnostic; harness row      *
// *                                          present for A/B with future  *
// *                                          captured video.              *
// *                                                                       *
// *   MistFog TFogEffect_Bespoke__MistFog — ambient mist (gravity smoke   *
// *                                         puffs). Faithful direct port  *
// *                                         of TMistFogAnimator           *
// *                                         (legacy/effect.cpp:11311-     *
// *                                         11456): NUMMISTFOG=25 smoke   *
// *                                         puffs, gravity, lifetime 200, *
// *                                         scale grows then shrinks,     *
// *                                         resets when scale dies.       *
// *                                         SetAddBlendState ->            *
// *                                         AdditiveStraight (preserved   *
// *                                         literally). Per task spec we  *
// *                                         add a dedicated class rather  *
// *                                         than re-using TFogEffect      *
// *                                         since the snapshot bodies     *
// *                                         differ materially.            *
// *                                                                       *
// *   RockStorm TRockStormEffect_Bespoke — falling/orbiting rocks spell.  *
// *                                        Faithful direct port of        *
// *                                        TRockStormAnimator             *
// *                                        (legacy/effect3.cpp:96-413).   *
// *                                        4-7 rocks orbit caster at      *
// *                                        radius, descending to land     *
// *                                        height, contract to             *
// *                                        min_radius, damage stage,      *
// *                                        then bounce out. Snapshot     *
// *                                        SetBlendState (rock = Alpha,  *
// *                                        glow = AdditiveStraight)       *
// *                                        preserved. First-pass collapses*
// *                                        per-rock rendering to a single *
// *                                        composite point: each rock is  *
// *                                        a screen-aligned billboard at  *
// *                                        the computed orbital pos with  *
// *                                        scl_fac scale.                 *
// * ************************************************************************

_CLASSDEF(TGeyserEffect_Bespoke)

// W3-C sgeyser/fgeyser. Retail-only effect (no snapshot body).
// Synthesized from Ghidra cls_0x5b79ac forensics:
//   - 348-byte class, derived from TEffect (cls_0x5a47f0 base via
//     cls_0x5a7e38 dispatcher path; vtable shows TScreen sub-objects).
//   - meth_0x526040 looped 100x in virt_meth_0x526860 (registrar) →
//     ~100 particle sub-objects per burst.
//   - Registrar branches on the registered name "CavFGeyser" vs default
//     ("CavSGeyser") → picks fgeyser/sgeyser sound cue (sound assets are
//     on disk under Sound/effects/). Visual variant: fire (warm) vs steam
//     (cool) tint over the same particle behavior.
//
// Synthesis: a periodic eruption every kGeyserPeriodTicks. During an
// eruption (kGeyserEruptionTicks) we hold up to kGeyserParticles ballistic
// particles spawned with upward velocity + lateral spread. Gravity pulls
// them back down; particles fade by alpha over their life. Variant
// (fire/steam) sets the color ramp. No I3D asset on disk → renders as
// procedural billboards keyed off a generic smoke/flame fallback texture.
//
// This is a first-pass synthesis; constants tuned to "looks like a
// geyser." Awaiting reference video for A/B kinematic tuning.
class TGeyserEffect_Bespoke : public TEffect
{
  public:
    enum class EVariant : uint8_t
    {
        Steam = 0,   // sgeyser (cool: white/blue)
        Fire  = 1,   // fgeyser (warm: orange/red)
    };

    TGeyserEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TGeyserEffect_Bespoke(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) {}
    ~TGeyserEffect_Bespoke() override = default;

    void OffScreen() override { /* ambient — do not kill */ }

    [[nodiscard]] static TGeyserEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin,
                                                                    EVariant variant);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);

    [[nodiscard]] bool IsAlive() const { return true; }   // ambient

  private:
    // Synthesis constants. Particle count taken from Ghidra (100 iter).
    static constexpr int32_t kGeyserParticles      = 100;
    static constexpr int32_t kGeyserSimTickMs      = 1000 / 24;
    static constexpr int32_t kGeyserPeriodTicks    = 24 * 5;   // 5s period
    static constexpr int32_t kGeyserEruptionTicks  = 24 * 2;   // 2s eruption
    static constexpr float   kGeyserParticleLife   = 60.0f;    // ticks
    static constexpr float   kGeyserGravity        = -0.30f;   // wu/tick^2
    static constexpr float   kGeyserUpVel          = 4.5f;     // wu/tick
    static constexpr float   kGeyserSideSpread     = 0.4f;     // wu/tick
    static constexpr float   kGeyserBaseSizeWu     = 28.0f;
    static constexpr float   kGeyserSpawnSpreadXY  = 6.0f;     // base radius

    struct SGeyserParticle
    {
        bool     used = false;
        hmm_vec3 pos  = {0.0f, 0.0f, 0.0f};
        hmm_vec3 vel  = {0.0f, 0.0f, 0.0f};
        float    life = 0.0f;
        float    maxlife = 0.0f;
        float    scl  = 1.0f;
    };

    SGeyserParticle particles_[kGeyserParticles] {};
    EVariant       variant_       = EVariant::Steam;
    int32_t        cycle_tick_    = 0;    // 0..kGeyserPeriodTicks
    int32_t        spawn_cursor_  = 0;
    double         sim_accum_ms_  = 0.0;
    TTextureHandle texture_       = kInvalidTexture;
    float          uv_rect_[4]    = {0.0f, 0.0f, 1.0f, 1.0f};
};

_CLASSDEF(TFlameAnimator_Bespoke__cfire)

// W3-C cfire. STUB. No Ghidra body, no snapshot body, no asset on
// disk (Class.Def does not register "cfire"; ATTACHEFFECT lines in
// char.def are commented out). SpawnForTest returns nullptr with a
// diagnostic; harness row present so the id appears in --vfx-list for
// future A/B once real video / asset surface.
class TFlameAnimator_Bespoke__cfire : public TEffect
{
  public:
    TFlameAnimator_Bespoke__cfire(TObjectImagery* newim) : TEffect(newim) {}
    TFlameAnimator_Bespoke__cfire(SObjectDef* def, TObjectImagery* newim)
        : TEffect(def, newim) {}
    ~TFlameAnimator_Bespoke__cfire() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TFlameAnimator_Bespoke__cfire* SpawnForTest_BESPOKE(
        const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);

    [[nodiscard]] bool IsAlive() const { return false; }   // stub never alive
};

_CLASSDEF(TFogEffect_Bespoke__MistFog)

// W3-C MistFog. Faithful direct port of TMistFogAnimator
// (legacy/effect.cpp:11311-11456). 25-puff gravity-falling smoke field
// using SSmoke per-puff state (x,y,z,rot,vx,vy,vz,life,size). Animate
// integrates ballistic motion, grows size linearly, after life>200 starts
// shrinking and ResetBall when size<=0.01.
//
// Render: SetAddBlendState -> AdditiveStraight (preserved literally). The
// snapshot uses GetObject(0) of mistfog.i3d and re-positions/re-scales it
// per puff. First-pass collapses each puff to a ScreenAligned additive
// billboard at the puff pos with size = puff.size. The .i3d asset exists
// at Misc/mistfog.I3D and Magic/mistfog.I3D, we pull texture slot 0.
class TFogEffect_Bespoke__MistFog : public TEffect
{
  public:
    TFogEffect_Bespoke__MistFog(TObjectImagery* newim) : TEffect(newim) {}
    TFogEffect_Bespoke__MistFog(SObjectDef* def, TObjectImagery* newim)
        : TEffect(def, newim) {}
    ~TFogEffect_Bespoke__MistFog() override = default;

    void OffScreen() override { /* ambient — do not kill */ }

    [[nodiscard]] static TFogEffect_Bespoke__MistFog* SpawnForTest_BESPOKE(
        const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);

    [[nodiscard]] bool IsAlive() const { return true; }

  private:
    static constexpr int32_t kMistFogNumPuffs   = 25;   // NUMMISTFOG
    static constexpr int32_t kMistFogSimTickMs  = 1000 / 24;
    static constexpr float   kMistFogSmokeGrav  = -0.012f;  // SMOKE_GRAV reasonable default
    static constexpr float   kMistFogBaseSizeWu = 28.0f;

    struct SMistPuff
    {
        float    x = 0.0f, y = 0.0f, z = 0.0f;
        float    rot = 0.0f;
        float    vx = 0.0f, vy = 0.0f, vz = 0.0f;
        int32_t  life = 0;
        float    size = 0.0f;
    };

    SMistPuff      smoke_[kMistFogNumPuffs] {};
    float          centerx_       = 0.0f;
    float          centery_       = 0.0f;
    int32_t        ticks_         = 0;
    double         sim_accum_ms_  = 0.0;
    TTextureHandle texture_       = kInvalidTexture;
    float          uv_rect_[4]    = {0.0f, 0.0f, 1.0f, 1.0f};

    void ResetBall_(int32_t b);
};

_CLASSDEF(TRockStormEffect_Bespoke)

// W3-C RockStorm. Faithful direct port of TRockStormAnimator
// (legacy/effect3.cpp:96-413). The animator runs a 5-stage state machine:
//   START   → level-2 only: ramp scl_fac[max] in
//   STAGE1  → level 1: rocks orbit, descending, then contract radius
//             level 2: big rock descends
//   DAMAGE  → flip lin_vel to 4.0 (bounce out)
//   STAGE2  → rocks rise, expanding radius, fading
//   DONE    → kill effect
//
// First-pass bespoke targets level 1 only (no spell-target lookup
// available in harness; level 2 needs a real target). Renders each rock
// as a screen-aligned billboard with scl_fac scale, plus an additive
// "glow" billboard during STAGE1.
//
// Blend per snapshot Render: rock = SetBlendState (Alpha),
// glow = SetAddBlendState (AdditiveStraight). Preserved literally.
//
// Asset Magic\Rocks.I3D is referenced in Class.Def but not present on
// disk in legacy/Imagery/Magic. Bespoke tries to RegisterImagery/Load
// and falls back to a procedural untextured render if missing.
class TRockStormEffect_Bespoke : public TEffect
{
  public:
    TRockStormEffect_Bespoke(TObjectImagery* newim) : TEffect(newim) {}
    TRockStormEffect_Bespoke(SObjectDef* def, TObjectImagery* newim)
        : TEffect(def, newim) {}
    ~TRockStormEffect_Bespoke() override = default;

    void OffScreen() override { KillThisEffect(); }

    [[nodiscard]] static TRockStormEffect_Bespoke* SpawnForTest_BESPOKE(
        const S3DPoint& origin);
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode debug_mode);

    [[nodiscard]] bool IsAlive() const { return alive_; }

  private:
    // Snapshot constants (legacy/effect3.cpp:37-45).
    static constexpr int32_t kRockStormMinRocks   = 4;   // ROCKSTORM_MINROCKS
    static constexpr int32_t kRockStormVarRocks   = 3;   // ROCKSTORM_VARROCKS
    static constexpr int32_t kRockStormMaxRocks   = 7;   // ROCKSTORM_MAXROCKS
    static constexpr int32_t kRockStormSimTickMs  = 1000 / 24;
    static constexpr float   kRockStormBaseSizeWu = 22.0f;
    static constexpr float   kRockStormGlowSizeWu = 36.0f;

    enum EStage
    {
        kStageStart   = 0,
        kStageStage1  = 1,
        kStageDamage  = 2,
        kStageStage2  = 3,
        kStageDone    = 4,
    };

    // Mirror TRockStormAnimator privates (legacy/effect3.cpp:99-112).
    int32_t  level_       = 1;
    int32_t  stage_       = kStageStart;
    int32_t  num_rocks_   = 0;
    S3DPoint target_pos_  = {0, 0, 0};
    float    min_radius_  = 8.0f;
    float    radius_[kRockStormMaxRocks] {};
    float    ang_vel_[kRockStormMaxRocks] {};
    float    ang_[kRockStormMaxRocks] {};
    float    dest_height_[kRockStormMaxRocks + 1] {};
    float    lin_vel_[kRockStormMaxRocks + 1] {};
    float    height_[kRockStormMaxRocks + 1] {};
    float    scl_fac_[kRockStormMaxRocks + 1] {};
    int32_t  frameon_    = 0;
    bool     alive_      = true;
    double   sim_accum_ms_ = 0.0;
    TTextureHandle rock_tex_  = kInvalidTexture;
    TTextureHandle glow_tex_  = kInvalidTexture;
    float          rock_uv_[4] = {0.0f, 0.0f, 1.0f, 1.0f};
    float          glow_uv_[4] = {0.0f, 0.0f, 1.0f, 1.0f};
};
