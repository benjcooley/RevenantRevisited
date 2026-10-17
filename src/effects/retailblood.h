// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *     retailblood.h - retail combat blood (TBloodAnimator 0x005acd18)   *
// *************************************************************************
//
// The blood a blow knocks out of a character: TCharacter::EffectBurst
// ("blood", 0x004c85d0) and the death squirts in TCharacter::Pulse spawn a
// "Blood" effect and hand it six spray parameters (TBloodEffect::SetParams,
// 0x004f9490). Its animator, TBloodAnimator, owns up to 30 droplets
// (bloodspray.h): it sprays them at Initialize (0x004f18d0), flies, splats
// and shrinks them once a tick in Pulse (0x004f1c60), and draws each one
// twice in Render (0x004f1de0) -- a dark alpha mask, then the red colour
// added on top. Forensics: docs/gameplay/forensics/COMBAT_EFFECTS.md §2.
//
// The snapshot's TBloodSystem (effectcomp.cpp) and the earlier gallery
// ports in effect.cpp / effects/blood.cpp are the 1998 design; this is the
// shipped one, and the one the game spawns.
//
// *************************************************************************

#pragma once

#include <array>
#include <cstdint>

#include "../3dimage.h"
#include "bloodspray.h"

class TRenderer;
class TObjectInstance;
enum class EFxDebugMode : uint8_t;

// Registered for the "Blood" effect type through a builder that also
// attaches the visual component (retailblood.cpp): the Blood.I3D boxes are
// never drawn as static meshes; Submit draws the droplets.
class TBloodAnimator final : public T3DAnimator
{
  public:
    explicit TBloodAnimator(TObjectInstance* oi) : T3DAnimator(oi) {}

    void Initialize() override;
    void Pulse() override;

    // The two-pass droplet draw (Render @ 0x004f1de0), eased between the
    // last two ticks. Called from the visual component in the map's FX walk.
    void Submit(TRenderer& renderer, EFxDebugMode debug_mode) const;

    [[nodiscard]] const retail_blood::TSpray& Spray() const { return spray; }

  private:
    struct SQuad
    {
        bool    valid = false;
        float   pos[4][3] = {};   // authored corners, ordered for SQuadDrawItem
        float   uv[4][2] = {};
        int32_t texture = -1;     // texture index the box draws with
    };

    // Retail +0x534..+0x550: the mask boxes (objects 4-7) and colour boxes
    // (objects 0-3), indexed by droplet size.
    std::array<SQuad, 4> mask{};
    std::array<SQuad, 4> color{};
    retail_blood::TSpray spray;
    // Render-only: the droplets as they were before the last tick, so
    // Submit can ease the motion between ticks.
    std::array<retail_blood::SDrop, retail_blood::kMaxDrops> previous{};
    int32_t spray_height = 0;
    bool initialized = false;

    void LoadQuad(int32_t object, SQuad& quad) const;
};
