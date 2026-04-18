// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   charanimator.h - TCharAnimator module               *
// *************************************************************************

#pragma once

#include "3dimage.h"

_CLASSDEF(TWeaponSwipe)
_CLASSDEF(TCharAnimator)

// weapon shtuffs

_STRUCTDEF(SWeaponSwipeParams)
struct SWeaponSwipeParams
{
    S3DLVertex* weaponverts;    // vertices of weapon, used for extents
    int32_t numverts;               // number of vertices in weaponverts
    TCharAnimator* charanim;    // animator of character this swipe is for
    float r, g, b;              // color (to be normalized) of swipe
    int32_t maxsegs;                // number of segments/length of swipe
    int32_t smooth;                 // number of triangles per frame such that maxsegs % smooth == 0
    const char* primehand;          // using this weapon...(to test for change of weapon)
};

class TWeaponSwipe
{
  private:
    S3DAnimObj theobj;
    S3DAnimObj* obj;
    int32_t maxpoints, maxverts;        // Maximum number of points we can have, max verts
    hmm_vec3 *points[2];   // The points themselves points[0] = hilt, points[1] = tip or vv
    hmm_vec3 vweapbeg, vweapend; // vertex of hilt, tip of weapon
    hmm_mat4* weaponmat; // translation table for weapon
    bool initialized;   // is this real
    // duplicate los parameteros
    S3DLVertex* weaponverts;    // vertices of weapon, used for extents
    int32_t numverts;               // number of vertices in weaponverts
    TCharAnimator* charanim;    // animator of character this swipe is for
    float r, g, b;              // color (to be normalized) of swipe
    int32_t maxsegs;                // number of segments/length of swipe
    int32_t smooth;                 // number of triangles per frame such that maxsegs % smooth == 0
    const char* primehand;          // using this weapon...(to test for change of weapon)

  public:
    TWeaponSwipe() { initialized = false; maxpoints = 0; primehand = nullptr; }
    virtual ~TWeaponSwipe() { }

    void Init(SWeaponSwipeParams* p);
    void GenerateStrip();
    void Animate();
    void CycleStrip();
    void GetWeaponExtents();
    void NormalizeColors();
    void ChangeColor(float r, float g, float b);
    void Render();
    void Close();
    hmm_mat4* GetCharsWeaponMatrix();
    bool GetInitialized() const { return initialized; }
    TCharAnimator* GetCharAnim() const { return charanim; }
};




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
    TWeaponSwipe* GetWeaponSwipe() { return &weaponswipe; }
      // get the weaponswipe

    // WeaponSwipe handling stuff
    void SetupWeaponSwipe();
      // Called from SetupObjects() to init the weaponswipe!
    S3DLVertex* GetWeaponVertices(int32_t len);
      // Called from SetupWeaponSwipe to get the correct weapon vertices
    int32_t GetWeaponNumVerts();
      // Called from SetupWeaponSwipe to get the correct number of weapon vertices
    T3DImagery* GetWeaponImagery(int32_t objnum);
      // called from GetWeaponVertices and GetWeaponNumVerts
    int32_t GetWeaponNum();
      // called from GetWeaponVertices and GetWeaponNumVerts

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

    TWeaponSwipe weaponswipe;               // swipe structure!
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
