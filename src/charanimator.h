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
    void InitTransparency();
      // Sets initial transparency level for character
    void UpdateTransparency();
      // Updates the character's transparency each frame
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

    T3DImagery* utilityimagery;

    float *origmatred, *origmatgreen;       // saved material values
    int32_t oldpoison;                          // update only when needed
    float transparency;                     // Current transparency level for character
    float visindicator_shiftval;
};

_CLASSDEF(TPlayerAnimator)
class TPlayerAnimator : public TCharAnimator
{
  public:
    TPlayerAnimator(TObjectInstance* oi) : TCharAnimator(oi) {}
    ~TPlayerAnimator() override = default;
};
