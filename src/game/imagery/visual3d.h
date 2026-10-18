// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   visual3d.h - the 3D (I3D imagery) visual of a node                  *
// *************************************************************************
//
// What a 3D object shows: the I3D's submeshes, posed by the authored
// animator. The imagery is the shared asset the map renderer registers
// meshes from; the animator is the owner's T3DAnimator (the retail
// animator classes keep the playhead until it moves into the animator
// component). TObjectImagery::NewVisualComponent makes one for every
// object with 3D imagery.
//
// *************************************************************************

#pragma once

#include "engine/render/visualcomponent.h"

_CLASSDEF(T3DImagery)
_CLASSDEF(T3DAnimator)

class T3DVisualComponent final : public TVisualComponent
{
  public:
    explicit T3DVisualComponent(T3DImagery& imagery) : imagery_(&imagery) {}

    [[nodiscard]] const char* ComponentName() const override { return "visual3d"; }
    [[nodiscard]] EVisualKind Kind() const override { return EVisualKind::Mesh; }

    [[nodiscard]] T3DImagery& Imagery() const { return *imagery_; }

    // The owner's authored animator, or nullptr while it has none (or a
    // 2D one, which never poses a mesh)
    [[nodiscard]] T3DAnimator* Animator() const;
    // Attaches the retail animator's components to the owner once; the
    // renderer calls it when it first draws the object
    void EnsureAnimatorComponents();
    // The owning object's 3D visual, or nullptr for any other picture
    [[nodiscard]] static T3DVisualComponent* Of(const TObjectInstance* oi);

  private:
    T3DImagery* imagery_ = nullptr;   // the asset, cache-owned
};
