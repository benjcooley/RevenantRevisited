// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 TStripEffect.h - TStripEffect module                  *
// *                                                                       *
// *  Phase 2.3 SR-pipeline port (2026):                                   *
// *  TStripEffect is the **lightning bolt** spell effect (registered as  *
// *  "lightstrip" / "LightStrip" — see data/Resources/spell.def Lightning *
// *  variants). The retail visual is animated by TLightningAnimator      *
// *  (S04 row, recon at recon/classes/cls_0x5b0a28.cpp) — a strip with  *
// *  per-vertex jitter + a rotating glow + sparks. This port covers the  *
// *  strip half only (SR pipeline) as the Phase 2.3 validator; glow +    *
// *  sparks land in S01a once the PE/IM pipeline sub-effects are ready.  *
// *  The full pre-release D3D body for both TStripEffect and             *
// *  TLightningAnimator + TWindStripEffect/TWindStripAnimator is         *
// *  preserved verbatim in stripeffect.cpp under `#if 0` — do not        *
// *  delete until those follow-ups land.                                 *
// *************************************************************************

#pragma once

#include "effect.h"
#include "object.h"
#include "renderer.h"     // EFxDebugMode + SStripSegment forward used in inline storage

#include <cstdint>
#include <vector>

// ******************
// * TStripEffect   *
// ******************

// State machine matches retail pre-release (src/stripeffect.cpp `#if 0`):
//   LAUNCH   = 1-tick raycast probe to find target distance / wall hit.
//   FLY      = strip is visible at full length for `duration` ticks.
//   EXPLODE  = strip shrinks from the tail; effect destroys when empty.
enum {
    STRIP_LAUNCH  = 0,
    STRIP_FLY,
    STRIP_EXPLODE,
};

// Retail constants (Phase A forensics: see docs/vfx/INVENTORY.md S01).
// These mirror the pre-release `#define`s; if Ghidra extraction surfaces
// retail divergence later, update at touch site.
#define STRIP_SPEED          16    // wu/segment advance during LAUNCH raycast
#define STRIP_RANGE          2     // overshoot bias on target range
#define STRIP_MAX_POINTS     100   // hard cap on segment count
#define STRIP_SEG_LEN        10    // local-Y stride between adjacent anchors
#define STRIP_JITTER_MAG     13    // per-vertex (-13, 13) xy jitter
#define STRIP_SMOOTH_SIZE    4     // cubic-interpolate between every 4th anchor
#define STRIP_WIDTH_HILT     32.0f // wu, base width at caster end (LIGHTNING_SCALE_N*16 / LIGHTNING_SCALE_D)
#define STRIP_WIDTH_TIP      20.0f // wu, base width at far end
#define STRIP_FLY_DURATION   20    // sim ticks of full visibility
#define STRIP_EXPLODE_TICKS  5     // sim ticks of shrink-from-tail

class TStripEffect : public TEffect
{
  public:
    int32_t duration    = STRIP_FLY_DURATION;
    int32_t targetrange = -1;     // -1 = unbounded (no wall/char hit found)
    int32_t my_state    = STRIP_LAUNCH;

    // Retail had a `PTCharacter chr` for damage routing; the SR-pipeline
    // port doesn't resolve damage (gameplay path lands later with the
    // spell wiring). Kept absent to keep the port minimal + obviously-
    // not-game-wired.

    TStripEffect(TObjectImagery* newim)
        : TEffect(newim)
    {
        flags |= OF_IMMOBILE | OF_PULSE;
    }
    TStripEffect(SObjectDef* def, TObjectImagery* newim)
        : TEffect(def, newim)
    {
        flags |= OF_IMMOBILE | OF_PULSE;
    }

    ~TStripEffect() override = default;

    void Initialize();
    void Pulse() override;

    // StripEffects stay on-screen until the effect is finished — pre-release
    // override (here = noop signature; the real OffScreen guards lifecycle).
    void OffScreen() override
    {
        if (my_state == STRIP_EXPLODE && commanddone)
            TObjectInstance::OffScreen();
    }

    // ------------------------------------------------------------------
    // --test=vfx SR-pipeline harness hooks (mirrors F01 TFlameEffect /
    // B01 TBloodEffect). Spawn a sector-less standalone instance pinned
    // to `origin`, populate the segment ring with retail-shaped anchors
    // launched along caster.facing (= 0, world +X). The caller owns
    // the returned pointer; delete to release imagery refcount + bucket.
    // Returns nullptr only on truly fatal failures (none currently — we
    // sidestep imagery loading by using the renderer's white texture, so
    // this is currently infallible; kept Maybe<T>-shaped for parity).
    // ------------------------------------------------------------------
    [[nodiscard]] static TStripEffect* SpawnForTest(const S3DPoint& origin);

    // Drive the segment ring forward by one frame (advance growth +
    // re-jitter at sim-tick boundary + tail-shrink during explode +
    // alpha-fade along length), then submit to the SR-pipeline FX queue.
    // No-op if no segments allocated yet.
    void TickAndSubmitForTest(EFxDebugMode debug_mode);

  private:
    // Each anchor is a world-space point along the bolt; the strip
    // submission interprets consecutive anchors as one SStripSegment.
    struct SAnchor
    {
        float pos[3]   = {0.0f, 0.0f, 0.0f};   // world-space
        float jitter[2] = {0.0f, 0.0f};        // xy jitter applied on sim tick
    };

    std::vector<SAnchor>      anchors_;        // ring of points along the bolt
    std::vector<SStripSegment> seg_scratch_;   // reused submit buffer
    float                     forward_dir_[3] = {1.0f, 0.0f, 0.0f};  // caster.facing as unit vec
    float                     glow_scale_   = 3.4f;                  // matches retail initial
    float                     u_scroll_     = 0.0f;                  // texture U scroll accumulator
    int32_t                   max_points_   = 40;                    // dynamic cap (retail walks raycast)
    int32_t                   tick_accum_   = 0;                     // ms remainder for sim-tick gate
    int32_t                   sim_ticks_    = 0;                     // wall-clock sim-tick count (24 Hz)
};

// ********************
// * TWindStripEffect *
// ********************
//
// Pre-release-only at present. Full body preserved in stripeffect.cpp under
// `#if 0` — port lives in S02 row (separate slice).
// (Header forward kept slim to avoid pulling in pre-release types until S02
// claims this work.)


#if 0   // S02/S03 follow-up — pre-release WindStrip header preserved

#define WINDSTRIP_MAX_STRIPS    20

_STRUCTDEF(SDropSparkSystem)
struct SDropSparkSystem
{
    hmm_vec3 pos, vel;
    float scl;
    bool used;
};

_CLASSDEF(TWindStripEffect)
class TWindStripEffect : public TEffect
{
  private:
    PTCharacter targets[10];
    int32_t end_of_targets;
    int32_t count;
  public:
    TWindStripEffect(TObjectImagery* newim) : TEffect(newim) { Initialize(); }
    TWindStripEffect(SObjectDef* def, TObjectImagery* newim) : TEffect(def, newim) { Initialize(); }
    virtual ~TWindStripEffect() {}
    virtual void Initialize();
    virtual void Pulse();
};

#endif // S02/S03 follow-up
