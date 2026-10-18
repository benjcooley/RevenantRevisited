// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   visual2d.h - the 2D (animation imagery) visual of a node            *
// *************************************************************************
//
// What a 2D object shows: the state's still, drawn in the unlit or the lit
// pass by the state's ANIIM flags, and the state's animation as a separate
// layer at the animation registration point (TAnimAnimator draws it after
// the lights in retail). The imagery is the shared asset; the state and
// frame are the owner's until the playhead moves into the animator
// component. TObjectImagery::NewVisualComponent makes one for every object
// with 2D imagery; the map renderer reads its bitmaps and registration
// through here instead of the imagery body.
//
// *************************************************************************

#pragma once

#include "engine/render/visualcomponent.h"

_CLASSDEF(TAnimImagery)
_CLASSDEF(TBitmap)

class T2DVisualComponent final : public TVisualComponent
{
  public:
    explicit T2DVisualComponent(TAnimImagery& imagery) : imagery_(&imagery) {}

    [[nodiscard]] const char* ComponentName() const override { return "visual2d"; }
    [[nodiscard]] EVisualKind Kind() const override { return EVisualKind::Sprite; }

    [[nodiscard]] TAnimImagery& Imagery() const { return *imagery_; }

    // The still a state shows, or nullptr out of range
    [[nodiscard]] TBitmap* Still(int32_t state) const;
    // The frame of a state's animation layer, or nullptr when the state has
    // none or the frame is out of range
    [[nodiscard]] TBitmap* AnimationFrame(int32_t state, int32_t frame) const;
    // The owning object's 2D visual, or nullptr for any other picture
    [[nodiscard]] static T2DVisualComponent* Of(const TObjectInstance* oi);

    // The Classic software path: the imagery's retail draw and pick code,
    // run for the owner (TAnimImagery::DrawUnlit / DrawLit / DrawSelected /
    // AlwaysOnTop / GetZ)
    void DrawUnlit(TSurface* surface) override;
    void DrawLit(TSurface* surface) override;
    void DrawSelected(TSurface* surface) override;
    [[nodiscard]] bool AlwaysOnTop() const override;
    [[nodiscard]] bool GetZ(const TNode* frontmost) const override;

  private:
    TAnimImagery* imagery_ = nullptr;   // the asset, cache-owned
};
