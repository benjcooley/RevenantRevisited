// *************************************************************************
// *                         Revenant Revisited                            *
// *        equipanimator.h - The animator for equipment on the map        *
// *************************************************************************

#pragma once

#include "game/imagery/3dimage.h"

// REVSYNC: the "Equip" 3D animator (builder 0x005e8720, vtable 0x005a3784),
// which T3DImagery gives weapons, armor, light sources and arrows on the
// map (docs/gameflow/forensics/DEATH_LOOT.md §5-6). Such things are modelled
// for the hand; on the ground each part's own transform is replaced by the
// resting pose -- rotated (π/2, π/2, π/2) at the object's origin, armor
// (0, 0, -π/4) at 1.5 times its size -- or, while the object is tossed
// (TObjectAnimator::Toss), by the toss's pose.
class T3DEquipAnimator final : public T3DAnimator
{
  public:
    using T3DAnimator::T3DAnimator;

    // REVSYNC: 0x00410de0 jumps to the base TObjectAnimator::Pulse (the
    // landing pulse and the toss); none of the default 3D animator's.
    void Pulse() override { TObjectAnimator::Pulse(); }
    void Animate(bool draw) override;

  private:
    void PoseParts();
};
