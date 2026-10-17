// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   particlecomponent.h - the engine particle effect as a node          *
// *                         component (docs/vfx/PREFAB_SYSTEM.md §4.3)   *
// *************************************************************************
//
// One component = one particle effect (its buckets and emitters) on a
// node. The definition either lives inline, read from a prefab:
//
//     particles
//     {
//         effect Mist { bucket mist_drops { ... } }
//     }
//
// or is a shared record the owning effect class points at (the
// effects.def consumers). The runtime (TParticleEffectManager) keys its
// state by the owning node and anchors emission at the node's world
// position, so the same component serves a map object and a prefab node.
//
// *************************************************************************

#pragma once

#include "objectcomponent.h"
#include "particledef.h"
#include "refl.h"
#include "renderer.h"      // EFxDebugMode

class TParticleEffectManager;
extern TParticleEffectManager* ParticleEffectManager;

class TParticleEffectComponent : public TObjectComponent
{
  public:
    DEF_COMPONENT("particles")
    REFL_FRIEND(TParticleEffectComponent)

    [[nodiscard]] const char* ComponentName() const override { return "particles"; }

    // Point at a shared definition (owned elsewhere, outlives this component).
    void Configure(const SParticleEffectDef* def) { shared_def_ = def; }
    // The definition in force: the shared one if configured, else the inline block.
    [[nodiscard]] const SParticleEffectDef* Def() const { return shared_def_ ? shared_def_ : &effect_; }

    void OnAttach() override;
    void OnDetach() override;
    // Per render frame, from whoever draws the owning node: pulses the
    // runtime (starting it on first call) and integrates the buckets.
    void DrawPulse();
    // Standalone drivers only (harness): submit this runtime's buckets.
    // In the map the renderer drains every bucket once per frame.
    void Submit(EFxDebugMode debug_mode);

  protected:
    void OnUpdate() override;

  private:
    void PostLoad();

    PROP(1) SParticleEffectDef effect_;                 // inline definition (prefabs)
    const SParticleEffectDef* shared_def_ = nullptr;
};
