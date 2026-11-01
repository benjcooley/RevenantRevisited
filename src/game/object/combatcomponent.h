// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   combatcomponent.h - what a character's fight keeps between ticks    *
// *************************************************************************
//
// The state the attack resolvers, the bow and the combat input keep from
// one tick to the next: the attack last started and whether it hit, the
// chain presses banked, the player's last attack button and how often it
// was pressed, the bow's shots queued and when it last shot, the combat
// flash, an impact's snap time, a cast's invoke delay, and the magic
// resistance a script set. Every character has one from construction,
// reached through Combat(). None of it is in the character's record:
// retail's save left the fight to start over, and so does the port's.
//
// *************************************************************************

#pragma once

#include "engine/scene/component.h"

#include <cstdint>

struct SCharAttackData;

class TCombatComponent final : public TComponent
{
  public:
    DEF_COMPONENT("combat")
    [[nodiscard]] const char* ComponentName() const override { return "combat"; }

  // The attack last started (ResolveAttack): the table entry, the
  // GameFrame it started on, whether it hit its main target, and the chain
  // presses banked (ButtonAttack, at most MAXCHAINHITS)
    SCharAttackData* lastattack = nullptr;                                                  // retail +0x160
    PROP_NOSAVE(1, "GameFrame the last attack started on")        int32_t lastattackticks = 0;   // retail +0x164
    PROP_NOSAVE(2, "The last attack hit its target")              int32_t lasthit         = 0;   // retail +0x168
    PROP_NOSAVE(3, "Chain presses banked")                        int32_t chainhits       = 0;   // retail +0x16c

  // The player's last attack button and how many times running it was
  // pressed (ButtonAttack 0x004d2480's same-button rule); SetFighting
  // resets them
    PROP_NOSAVE(4, "The last attack button; -1 none")             int32_t lastbutton   = -1;     // retail +0x28c
    PROP_NOSAVE(5, "Presses of that button running")              int32_t buttonrepeat = 0;      // retail +0x290

  // The bow: the GameFrame of the last shot, so it isn't shot too fast,
  // and the shots ShootBow has queued for ResolveBowShoot
    PROP_NOSAVE(6, "GameFrame of the last bow shot")              int32_t lastbowshot = 0;       // retail +0x22c
    PROP_NOSAVE(7, "Bow shots queued")                            int32_t bowshots    = 0;       // retail +0x230

  // The combat flash a blow sets to 5 and Pulse counts down; an impact's
  // snap time Damage stores, nothing reads it; the ticks an INVOKE waits
  // before its cast
    PROP_NOSAVE(8, "Ticks of combat flash left")                  int32_t combatflashticks = 0;  // retail +0x224
    PROP_NOSAVE(9, "An impact's snap time; -1 none")              int32_t snapticks        = -1; // retail +0x220
    PROP_NOSAVE(10, "Ticks until the invoke casts")               int32_t invokedelay      = 0;

  // The magic resistance a script set: 0 to 1, the share of magic damage
  // resisted
    PROP_NOSAVE(11, "Magic resistance, 0 to 1")                   float magic_resistance = 0.0f;
};
