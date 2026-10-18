// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   visualcomponent.h - what a node shows                               *
// *************************************************************************
//
// A node's picture is a component, as its light is. The map renderer walks
// a node's visual components and never asks what kind of imagery an object
// has: the 2D sprite and the I3D mesh born of an object's imagery derive
// from this (game side, T2DVisualComponent / T3DVisualComponent), as does
// a billboard an effect attaches next to, or instead of, that default.
//
// The renderer draws sprites and meshes itself from its resident asset
// records (pass management and ordering stay in the renderer); a
// self-drawing visual fills the renderer's queues from Submit.
//
// *************************************************************************

#pragma once

#include "engine/scene/component.h"

#include <cstdint>

_CLASSDEF(TRenderer)

enum class EVisualKind : uint8_t
{
    Sprite,     // a 2D imagery state: still plus animation layer
    Mesh,       // an I3D imagery: submeshes posed by the animator
    Billboard,  // a camera-facing textured quad the component submits itself
};

class TVisualComponent : public TComponent
{
  public:
    [[nodiscard]] virtual EVisualKind Kind() const = 0;

    // True when this component is the node's whole picture: the renderer
    // leaves the imagery-born visual undrawn while this one is attached.
    [[nodiscard]] virtual bool ReplacesDefaultVisual() const { return false; }

    // Self-drawing visuals fill the renderer's queues here; sprites and
    // meshes are drawn by the map renderer from its records and leave it.
    virtual void Submit(TRenderer& renderer) const { (void)renderer; }
};

// The visual on a node that replaces the imagery-born one, or nullptr.
[[nodiscard]] TVisualComponent* ReplacingVisual(const TNode& node);
