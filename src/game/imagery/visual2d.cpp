// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   visual2d.cpp - the 2D (animation imagery) visual of a node          *
// *************************************************************************

#include "game/imagery/visual2d.h"

#include "game/imagery/animimage.h"
#include "game/object/animation.h"
#include "game/object/object.h"

TBitmap* T2DVisualComponent::Still(int32_t state) const
{
    if (state < 0 || state >= imagery_->NumStates())
        return nullptr;
    return imagery_->GetStillImage(state);
}

TBitmap* T2DVisualComponent::AnimationFrame(int32_t state, int32_t frame) const
{
    if (state < 0 || state >= imagery_->NumStates() || frame < 0)
        return nullptr;
    TAnimation* anim = imagery_->GetAnimation(state);
    return anim ? anim->GetFrame(frame) : nullptr;
}

T2DVisualComponent* T2DVisualComponent::Of(const TObjectInstance* oi)
{
    TVisualComponent* visual = oi ? oi->Visual() : nullptr;
    return visual && visual->Kind() == EVisualKind::Sprite ? static_cast<T2DVisualComponent*>(visual) : nullptr;
}
