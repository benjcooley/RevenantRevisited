// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   aicomponent.h - what a character's AI remembers between ticks       *
// *************************************************************************
//
// The state TCharacter::AI and the resolvers keep from one tick to the
// next: the attack timers and the requests OnAttacked raises, the target
// last seen and the waypoint being walked, the wander, the retreat, the
// item a Goto carries, the boss kind and its state, the characters seen
// lately, the auto-combat switch, and what the character last gave off
// for others to hear and see. Every character has one from construction,
// reached through Brain() (AI() is the routine that runs it). None of it
// is saved, the references included: retail's save left the AI to start
// over, and so does the port's; the properties are for the inspector.
//
// *************************************************************************

#pragma once

#include "engine/scene/component.h"
#include "engine/core/revtypes.h"
#include "game/object/character.h"      // the has-seen references resolve to characters
#include "game/ai/bossai.h"

#include <vector>

// Maximum number of characters a character can remember seeing
constexpr int32_t MAXHASSEEN = 8;

// One character seen lately (retail +0x1c0, MAXHASSEEN entries). The
// character is a weak reference: a deleted one reads as none.
struct SHasSeen
{
    REFL_IDS(1, 9)
    PROP(1, "The character seen")                   TSafeRef<TCharacter> chr;
    PROP(2, "Game ticks when last seen")            int32_t time = 0;
    PROP(3, "Keeps the player from auto-combat")    bool noautocombat = false;
};

class TAIComponent final : public TComponent
{
  public:
    DEF_COMPONENT("ai")
    [[nodiscard]] const char* ComponentName() const override { return "ai"; }

    TAIComponent() { hasseen.resize(MAXHASSEEN); }

  // The attack timers and requests
    PROP_NOSAVE(1, "Ticks until the AI may attack again")         int32_t  nextattack  = 1;   // retail +0x120: IsValidAttack refuses a monster's attack while non-zero
    PROP_NOSAVE(2, "Ticks until a MAGICATTACK may be tried")      int32_t  magictimer  = 1;   // retail +0x124: AI re-arms it from MAGICFREQ; IsValidAttack spends it
    PROP_NOSAVE(3, "AI requests, the kRequest bits")              uint32_t requestbits = 0;   // retail +0x128: set by OnAttacked
    PROP_NOSAVE(4, "Attacks left to make")                        int32_t  attackcount = 0;   // retail +0x12c: decremented by the AI per attack made, no reader found

  // The target and the waypoint: where the AI last saw or heard its target,
  // the centre of its waypoint search when it loses it; the object it
  // walks toward and ResolveCombat faces when no visible target overrides
  // it, the waypoint AI 0x004c8b60 heads for and the one Wander 0x004c9790
  // patrols to; how many more ticks it keeps facing that waypoint while
  // chasing, a new waypoint gives it 6.
    PROP_NOSAVE(5, "Where the target was last seen or heard")     S3DPoint target_last_position = {};  // retail +0x23c
    PROP_NOSAVE(6, "What the AI walks toward and faces")          TSafeRef<TObjectInstance> ai_lookat;              // retail +0x234
    PROP_NOSAVE(7, "Ticks left facing the waypoint")              int32_t  waypointticks = 0;          // retail +0x248

  // The wander (retail +0x238 / +0x24c / +0x250): the waypoint before the
  // current one, its pick passes over both and with none to pick it goes
  // back, retail drops it when its object goes, 0x004c38a6; the ticks it
  // still does nothing; the ticks a run has left.
    PROP_NOSAVE(8, "The waypoint before the current one")         TSafeRef<TObjectInstance> lastwaypoint;
    PROP_NOSAVE(9, "Ticks the wander still idles")                int32_t  wanderidle = 0;
    PROP_NOSAVE(10, "Ticks a wander's run has left")              int32_t  runticks   = 0;

  // The retreat (retail +0x254 / +0x258 / +0x25c; COMBAT_ATTACK_CHOICE.md
  // §3.10.6). The AI's tail, every tick it runs: retreating = latch =
  // (Health <= RETREATAT || latch) && frames != 0, then frames counts
  // down. Damage arms the frames with RETREATFOR when a hit leaves
  // 1 <= Health <= RETREATAT; StartRetreat (0x004d5fc0) with 96 and sets
  // both flags. A retreating character runs straight away from its target
  // and takes no new one; ResolveCombat doesn't face it; a blocked step
  // (MoveStep) clears all three.
    PROP_NOSAVE(11, "Running from the target")                    bool     retreating    = false;
    PROP_NOSAVE(12, "The retreat latch")                          bool     retreatlatch  = false;
    PROP_NOSAVE(13, "Frames of retreat left")                     int32_t  retreatframes = 0;

  // The item a Goto carries (retail +0x288): the map pane's walk to an item
  // out of reach, 0x004cedb0, picked up when that walk arrives (0x004c6155,
  // 0x004c7fc3). Kept until then, or until a Goto carries another.
    PROP_NOSAVE(14, "The item a Goto picks up on arrival")        TSafeRef<TObjectInstance> gotoitem;

  // The boss (retail +0x280 / +0x284): the kind AI_PerMonster 0x004c9b70
  // gives, once (TCharacter::kMonsterBaez, whom magic can't hurt, Solifuge,
  // Jhaga, Yhagoro, -1 any other), and the boss's own state (bossai.h).
    PROP_NOSAVE(15, "The boss kind; 0 not looked yet, -1 none")   int32_t  monsterkind = 0;
    TBossState bossstate;

  // The characters seen lately: MAXHASSEEN entries, the oldest replaced
    PROP_NOSAVE(16, "The characters seen lately")                 std::vector<SHasSeen> hasseen;

  // The auto-combat switch: mimics the global, but lets Locke flee in the
  // face of too many enemies.
    PROP_NOSAVE(17, "Begins combat on sight")                     bool     autocombat = true;

  // What the character last gave off for others' AI to sense: 1-100, how
  // visible and how loud the last move was; -1 before any.
    PROP_NOSAVE(18, "How visible the last move was")              int32_t  glimpse = -1;
    PROP_NOSAVE(19, "How loud the last move was")                 int32_t  noise   = -1;
};
