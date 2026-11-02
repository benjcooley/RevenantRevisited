// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   visual3d.cpp - the 3D (I3D imagery) visual of a node                *
// *************************************************************************

#include "game/imagery/visual3d.h"

#include "game/imagery/3dimage.h"
#include "game/object/object.h"

T3DAnimator* T3DVisualComponent::Animator() const
{
    TObjectInstance* owner = OwnerInstance();
    return owner ? dynamic_cast<T3DAnimator*>(owner->GetAnimator()) : nullptr;
}

void T3DVisualComponent::EnsureAnimatorComponents()
{
    if (TObjectInstance* owner = OwnerInstance())
        imagery_->AttachAnimatorComponents(owner);
}

T3DVisualComponent* T3DVisualComponent::Of(const TObjectInstance* oi)
{
    TVisualComponent* visual = oi ? oi->Visual() : nullptr;
    return visual && visual->Kind() == EVisualKind::Mesh ? static_cast<T3DVisualComponent*>(visual) : nullptr;
}
