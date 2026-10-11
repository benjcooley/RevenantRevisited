// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   charanimator.h - TCharAnimator module               *
// *************************************************************************

#pragma once

#include "3dimage.h"

#include <array>
#include <vector>

_CLASSDEF(TCharAnimator)

// TWeaponSwipe was the legacy strip-mesh trail renderer that drew the
// weapon arc during attacks. The drawable system will own that effect
// when it is reintroduced; for now the swipe is fully retired.

// *****************
// * TCharAnimator *
// *****************

class TCharAnimator : public T3DAnimator
{
  public:
    TCharAnimator(TObjectInstance* oi);
    ~TCharAnimator() override;

    void Animate(bool draw) override;
    bool Render() override;
    void Pulse() override;
      // The tick: T3DAnimator's, then the rising numbers'

  // Rising damage numbers (retail +0x104: eight slots of 0x14 bytes -- the
  // number's bitmap, ticks left, x, y, rise). Damage shows one; the tick
  // lifts and ages it; the play screen's world overlay draws it
  // (TRisingNumberLayer), fading over its last second.
    struct SRisingNumber
    {
        int32_t  amount = 0;    // as drawn ("-12")
        uint32_t rgb = 0;       // 0xRRGGBB
        int32_t  x = 0;         // map-screen point (WorldToScreen's space):
        int32_t  y = 0;         //   the drawn image's top centre
        int32_t  ticks = 0;     // left to show; 0: the slot is free
        int32_t  rise = 0;      // above y, in 1/256 px
        uint32_t serial = 0;    // which number this is (the overlay's image key)
    };
    static constexpr int32_t kRisingNumberSlots = 8;
    static constexpr int32_t kRisingNumberTicks = 36;       // 1.5 s
    static constexpr int32_t kRisingNumberRise = 0x190;     // a tick, in 1/256 px (37.5 px/s)
    static constexpr int32_t kRisingNumberFadeTicks = 24;   // the last second fades
    void ShowNumber(int32_t amount, int32_t x, int32_t y, uint32_t rgb) override;
      // REVSYNC: 0x004da3b0 -- into the first free slot of 1..7 (slot 0 is
      // never used); with none free, not shown
    [[nodiscard]] const std::array<SRisingNumber, kRisingNumberSlots>& RisingNumbers() const { return numbers; }

  // Drawn transparency (retail +0x588). The scene manager calls
  // UpdateDrawState once per drawn frame and draws the character at
  // DrawAlpha(); the fade itself (TCharacter::Transparency) is simulation.
    void UpdateDrawState(double dt_seconds) override;
      // Hidden while OF_INVISIBLE (outside the editor); otherwise the drawn
      // alpha moves toward Transparency()/100
    [[nodiscard]] float DrawAlpha() const override;
      // The drawn alpha, or 0 while hidden or too faint to draw

  // What a player's equipment draws this frame (retail +0x594, rebuilt for
  // each object it draws). Retail draws an equipped item's mesh object with
  // the matrix of the character object it names; UpdateDrawState lists the
  // pairs and hides the character objects they replace, the scene manager
  // submits them. A character that isn't the player draws only its own
  // objects, less any named sword or weapon.
    struct SEquipAttachment
    {
        TSafeRef<TObjectInstance> item;     // the equipped item; its imagery is the mesh
        int32_t object = -1;                // the item's object drawn
        int32_t bone = -1;                  // this character's object whose matrix it takes
    };
    [[nodiscard]] const std::vector<SEquipAttachment>& EquipAttachments() const { return equipment; }

  protected:

  // Poison functions
    void InitPoisonColor();
      // Initializes poison material
    void ClosePoisonColor();
      // Clears poison material
    void SetPoisonColor();
      // Sets poison color for character

  // Vision Indicator (to show the player in sneak mode how close he is to making himself known)
    void RenderVisionIndicator();

  // Equipment
    void UpdateEquipment();
      // Hides this frame's replaced objects and lists what the equipment draws
    void RenderEquipment();
      // The legacy draw of EquipAttachments() through T3DImagery::RenderObject

  // Transparency functions
    [[nodiscard]] bool IsHidden() const;
      // OF_INVISIBLE outside the editor: not drawn, and the drawn alpha holds
    void InitTransparency();
      // Seeds the drawn alpha from the character's Transparency()
    void UpdateTransparency(double dt_seconds);
      // Moves the drawn alpha toward Transparency()/100 at the retail rate
    void SetMaterialTransparency(T3DImagery* img);
      // Sets the transparency for a given imagery (all materials) based on current
      // transparency level
    void ResetMaterialTransparency(T3DImagery* img);
      // Resets all imagery transparency back to 1.0

  // Utility Imagery stuff (shadow, combat flashes, bloody chunks, etc.)
    void InitUtilityImagery();
    void CloseUtilityImagery();
    void RenderShadow();
    void RenderCombatFlashes();
    void RenderBloodyChunks();

  // cool function (like T3DAnimator::NewObject except uses imagery passed to it!
    S3DAnimObj* GetNewImObject(T3DImagery* imagery, int32_t objnum, int32_t flags = 0);
    void GetImFaces(T3DImagery* imagery, S3DAnimObj* obj);
    void GetImVerts(T3DImagery* imagery, S3DAnimObj* obj, ERender3DVertex verttype);

    T3DImagery* utilityimagery = nullptr;

    float* origmatred = nullptr;            // saved material values
    float* origmatgreen = nullptr;
    int32_t oldpoison = 0;                  // update only when needed
    float transparency = 1.0f;              // drawn alpha 0..1 (retail +0x588)
    bool reseed_transparency = false;       // hidden since the last drawn frame
    float visindicator_shiftval = 0.0f;

    std::array<SRisingNumber, kRisingNumberSlots> numbers{};
    std::vector<SEquipAttachment> equipment;    // this frame's (UpdateEquipment)
};

_CLASSDEF(TPlayerAnimator)
class TPlayerAnimator : public TCharAnimator
{
  public:
    TPlayerAnimator(TObjectInstance* oi) : TCharAnimator(oi) {}
    ~TPlayerAnimator() override = default;
};
