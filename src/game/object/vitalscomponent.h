// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   vitalscomponent.h - a character's recovery, poison and teleport     *
// *************************************************************************
//
// What TCharacter::Pulse gathers between pulses: the game time each vital
// last recovered and what has accumulated towards its next point, the
// poison clock, and the teleport point a spell set. Every character has
// one from construction, reached through Vitals(). The clocks and the
// teleport point are the character's record (SCharacterState); the
// accumulators are between pulses and are not saved, as in retail.
//
// *************************************************************************

#pragma once

#include "engine/scene/component.h"
#include "engine/core/revtypes.h"

class TVitalsComponent final : public TComponent
{
  public:
    DEF_COMPONENT("vitals")
    [[nodiscard]] const char* ComponentName() const override { return "vitals"; }

  // Recovery (TCharacter::PulseRecovery): the Clock() time each vital last
  // recovered, and what has gathered towards its next point, in 1/10000
  // points.
    PROP(1, "Game time health last recovered")          int32_t lasthealthrecov  = 0;   // retail +0x138
    PROP(2, "Game time fatigue last recovered")         int32_t lastfatiguerecov = 0;   // retail +0x13c
    PROP(3, "Game time mana last recovered")            int32_t lastmanarecov    = 0;   // retail +0x140
    PROP_NOSAVE(4, "Last running charge; -1 not running") int32_t lastrunfatigue = 0;   // retail +0x144
    PROP_NOSAVE(5, "Towards the next health point")     int32_t healthaccum      = 0;   // retail +0x148
    PROP_NOSAVE(6, "Towards the next fatigue point")    int32_t fatigueaccum     = 0;   // retail +0x14c
    PROP_NOSAVE(7, "Towards the next mana point")       int32_t manaaccum        = 0;   // retail +0x150
    PROP_NOSAVE(8, "Fatigue the running costs")         int32_t runaccum         = 0;   // retail +0x154
    PROP_NOSAVE(9, "Towards the next poison point")     int32_t poisonaccum      = 0;   // retail +0x158
    PROP(10, "Last poison charge; -1 not poisoned")     int32_t lastpoisondamage = 0;   // retail +0x15c
  // The teleport point a spell set
    PROP(11, "Teleport point; -1 none")                 S3DPoint teleport_position = {-1, -1, -1};
    PROP(12, "Teleport level; -1 none")                 int32_t  teleport_level    = -1;
};
