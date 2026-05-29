// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           weaponswipe.h - S09 TWeaponSwipe (sword slash trail)        *
// *************************************************************************
//
// The actual sword/weapon slash visual. Distinct from S01 TStripEffect
// (lightning bolt): both ride the SR pipeline but TWeaponSwipe is owned
// by a character's animator and traces a per-frame ring buffer of
// (hilt, tip) world-space anchor pairs sampled from the equipped
// weapon's matrix. The strip then Catmull-Rom-splines through the ring
// to produce a smooth ribbon.
//
// Phase B port (this file): a bespoke per-weapon state machine that
// owns the (hilt, tip) ring and emits SStripSegment[] via
// Renderer->SubmitFxStrip. The pre-release D3D body lives at
// src/weapontrail.cpp under `#if 0` (preserved verbatim) and provided
// every retail constant + the Catmull-Rom math shape; see Phase A
// forensics for the full mapping (docs/vfx/INVENTORY.md S09 row).
//
// **Bespoke per AGENT_GUIDE §3.2.1** — per-vertex custom geometry
// (weapon-extents-driven width) + character-attached behaviour
// disqualify this from the data-driven engine path. The SR pipeline
// (engine) owns the actual strip submission; the *math* (ring fill +
// spline + colour curve) is bespoke.
//
// Wiring into live gameplay (TCharAnimator::Animate / ::Render) is
// out of scope for this row (see S09 §6.6 / S09b carve-out). This
// header is consumed by the --test=vfx harness today; the live
// integration is a separate pass that calls the same `Tick` /
// `Submit` entry points from TCharAnimator with the live weapon
// matrix.
//
// *************************************************************************

#pragma once

#include <cstdint>
#include <vector>

#include "renderer.h"     // EFxDebugMode + SStripSegment for the submit path

class TWeaponSwipe
{
  public:
    TWeaponSwipe()  = default;
    ~TWeaponSwipe() = default;

    // No copy: owns vector buffers + per-character state.
    TWeaponSwipe(const TWeaponSwipe&)            = delete;
    TWeaponSwipe& operator=(const TWeaponSwipe&) = delete;

    // Set the per-character swipe colour. Mirrors retail
    // `SCharData::swipecolor` (RGB 0..1) — the trail is tinted with the
    // normalized colour and the hilt end gets a +0.4 brightness boost.
    // Pre-release default of (0,0,0) skips initialization entirely;
    // here we treat a zero colour as "don't draw" via `colour_zero_`.
    void SetColour(float r, float g, float b);

    // Re-initialize against a new weapon. Drops the ring buffer.
    // Callers should invoke this once at boot and whenever the
    // equipped weapon changes (matches retail's primehand string
    // compare in TWeaponSwipe::Animate / Close path).
    void ResetForNewWeapon();

    // Per-frame submit driver for the --test=vfx harness. Inputs:
    //   * world_hilt / world_tip — world-space anchor pair sampled from
    //     the live weapon matrix this render frame. The caller resolves
    //     these from a CharacterRig SVfxAttachment (see vfxtest.cpp).
    //   * attack_visible — true when the owning character is in an
    //     ACTION_ATTACK frame (`IsAttack()` in retail). The ring keeps
    //     advancing even when false (so the trail starts on the very
    //     first attack tick with real history); only Submit gates on
    //     this flag.
    //   * dbg — debug-mode forward through to SStripDrawItem.
    //
    // The function:
    //   1. Sim-tick-gates ring advance to 24 Hz (matches retail).
    //   2. CycleStrip() + insert new (hilt, tip) at head[0].
    //   3. When attack_visible and we have >=2 ring pairs, builds the
    //      Catmull-Rom-splined SStripSegment[] and submits.
    //
    // Safe to call before ResetForNewWeapon — does nothing.
    void TickAndSubmit(const float world_hilt[3],
                       const float world_tip[3],
                       bool        attack_visible,
                       EFxDebugMode dbg);

    [[nodiscard]] bool Initialized() const { return initialized_; }

  private:
    // Retail ring-buffer shape — Phase A forensics:
    //   maxsegs   = length * smooth = 5 * 8 = 40 retail triangle count
    //   maxpoints = (maxsegs / smooth) + 3 = 8 ring entries
    //   smooth    = 8 Catmull-Rom subdivisions per anchor pair
    static constexpr int32_t kLength     = 5;
    static constexpr int32_t kSmooth     = 8;
    static constexpr int32_t kMaxPoints  = (kLength * kSmooth / kSmooth) + 3;

    // Sim-tick gate. Matches S01 — retail animator ran at 24 Hz, we
    // accumulate render-frame dt and only cycle the ring on the tick
    // boundary so the trail history is independent of display rate.
    static constexpr int32_t kSimTickMs  = 1000 / 24;

    // Per-vertex alpha + brightness curve constants (retail
    // `GenerateStrip`): the trail starts semi-transparent (max 0.3) at
    // the head and fades to zero across the strip; hilt-side colour is
    // brightened by +0.4 per channel before tapering to the normalized
    // base. We replicate the *shape* of the curve at segment endpoints
    // since SR pipeline only takes per-endpoint (not per-vertex) data.
    static constexpr float   kAlphaHead       = 0.3f;
    static constexpr float   kHiltBrightness  = 0.4f;

    struct SPair
    {
        float hilt[3] = {0.0f, 0.0f, 0.0f};
        float tip [3] = {0.0f, 0.0f, 0.0f};
    };

    // Catmull-Rom evaluator (mirrors src/math3d.h Spline for plain
    // float[3]; kept private so we don't pull hmm_vec3 into the
    // header).
    static void CatmullRom(float out[3], float t,
                           const float p0[3], const float p1[3],
                           const float p2[3], const float p3[3]);

    std::vector<SPair>          ring_;          // size <= kMaxPoints
    std::vector<SStripSegment>  seg_scratch_;   // reused submit buffer

    float   colour_[3]   = {1.0f, 1.0f, 1.0f};  // normalized swipe colour
    bool    colour_zero_ = false;               // pre-release "no swipe" path
    bool    initialized_ = false;
    bool    first_submit_logged_ = false;

    int32_t tick_accum_  = 0;                   // ms remainder for sim-tick gate
};
