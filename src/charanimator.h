// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   charanimator.h - TCharAnimator module               *
// *************************************************************************

#pragma once

#include "3dimage.h"
#include "weaponswipe.h"

#include <array>

_CLASSDEF(TCharAnimator)

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
    [[nodiscard]] bool DrawsCombatFlash() const override;
      // While the combat flash ticks run (retail Render 0x004d7be5)
    void SubmitFx(TRenderer& renderer) const override;
      // The weapon swipe, drawn after the character (Render 0x004d7c6f)
    static void SubmitSwipeStrip(TRenderer& renderer, const TWeaponSwipe::SStrip& strip,
                                 const hmm_mat4& model_to_world);
      // A swipe strip's triangles, alpha-blended and depth-tested

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

  // Equipment rendering functions
    void ProcessEquipment(int32_t task);
      // Iterates through equipment list for player and hides char parts or renders equip
    void HideCharParts();
      // Hides the character parts that will be replaced by the equipment
      // Calls ProcessEquipment()
    void RenderEquipment();
      // Renders the equipment parts (
      // Calls ProcessEquipment()

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

  // Weapon swipe (retail TWeaponSwipe at +0x1a8; weaponswipe.h)
    void SetupWeaponSwipe();
      // 0x004d8f10: starts the swipe of a character with a SWIPECOLOR
    void UpdateWeaponSwipe();
      // Starts it while it isn't running (Animate 0x004d7a20); updates it on
      // the first drawn frame after each tick (Render 0x004d7c47)
    TWeaponSwipe::SFrame WeaponSwipeFrame(bool next, TWeaponSwipe::SMatrix& weapon);
      // The owner as the swipe sees it at this tick's key, or the next
      // one's; `weapon` holds the sampled weapon matrix
    int32_t SwipeObjectNum();
      // This character's "weapon", "sword" or "ogrokaxe" object, or -1
    int32_t WeaponObjectNum();
      // 0x004d8bd0: the object of a player's weapon in hand that sits on
      // this body, or -1
    void WeaponExtents(TWeaponSwipe::SVec3& low, TWeaponSwipe::SVec3& high);
      // 0x004d8e60: the weapon object's vertex bounds

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

    TWeaponSwipe swipe;
    TWeaponSwipe::SRing swipe_next{};   // where the next tick's update takes the history
    uint32_t swipe_ticks = 0;           // ticks pulsed
    uint32_t swipe_ticks_seen = 0;      // ticks the swipe has caught up with
};

_CLASSDEF(TPlayerAnimator)
class TPlayerAnimator : public TCharAnimator
{
  public:
    TPlayerAnimator(TObjectInstance* oi) : TCharAnimator(oi) {}
    ~TPlayerAnimator() override = default;
};
