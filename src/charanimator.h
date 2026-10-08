// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   charanimator.h - TCharAnimator module               *
// *************************************************************************

#pragma once

#include "3dimage.h"

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

  // Drawn transparency (retail +0x588). The scene manager calls
  // UpdateDrawState once per drawn frame and draws the character at
  // DrawAlpha(); the fade itself (TCharacter::Transparency) is simulation.
    void UpdateDrawState(double dt_seconds) override;
      // Hidden while OF_INVISIBLE (outside the editor); otherwise the drawn
      // alpha moves toward Transparency()/100
    [[nodiscard]] float DrawAlpha() const override;
      // The drawn alpha, or 0 while hidden or too faint to draw

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
};

_CLASSDEF(TPlayerAnimator)
class TPlayerAnimator : public TCharAnimator
{
  public:
    TPlayerAnimator(TObjectInstance* oi) : TCharAnimator(oi) {}
    ~TPlayerAnimator() override = default;
};
