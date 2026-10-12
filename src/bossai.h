// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *        bossai.h - the four bosses' own state (AI_PerMonster)          *
// *************************************************************************
//
// What each boss keeps between ticks: retail's AI_PerMonster (0x004c9b70)
// allocates it at +0x284 the first time it meets the boss (forensics:
// docs/gameplay/forensics/BOSS_AI.md). Field order and units are
// retail's; the retail offset of each field is beside it. The behaviour
// is TCharacter's (src/bossai.cpp).

#pragma once

#include "complexobj.h"

#include <array>
#include <cstdint>
#include <variant>

// Baez, the Ogrok champion (0x58 bytes): his stun pauses and the fake
// death that ends the ring fight (BOSS_AI.md §2).
struct SBaezState
{
    static constexpr int32_t kWatchers = 11;      // the crowd (retail table 0x005df54c)

    int32_t stunned = 0;               // +0x00 in a stun pause
    int32_t stunticks = 0;             // +0x04 ticks left of it
    int32_t stunlevel = 0;             // +0x08 the health band that last stunned him (80, 60, 40, 20)
    int32_t stage = 0;                 // +0x0c 0 fighting, 1 the walk to the table, 2 the throw, 3 dead
    S3DPoint baezspot{};               // +0x10 where he walks to: the table + (0, 0x15e)
    S3DPoint playerspot{};             // +0x1c where the player is walked second: the table + (0, 0xa0)
    int32_t table = 0;                 // +0x28 the table's map index
    int32_t frame = 0;                 // +0x2c ticks into the throw
    int32_t throwlength = 0;           // +0x30 frames of his "combat to cdead"
    int32_t watchersfound = 0;         // +0x34 the crowd looked up
    int32_t playerhealth = 0;          // +0x38 the player's health last tick
    int32_t health = 0;                // +0x3c his own last tick
    int32_t falling = 0;               // +0x40 the fall into "fakedead" still to play
    S3DPoint firstspot{};              // +0x44 where the player is walked first: the table + (0x20, 0x1b0, 0x58)
    int32_t playerarrived = 0;         // +0x50 the player reached firstspot
    int32_t waitticks = 0;             // +0x54 how long the walking rests, counted down

    // Retail keeps the crowd in a global array of raw pointers (0x0066caac),
    // never cleared; here it is Baez's, held safely.
    std::array<TSafeRef<TComplexObject>, kWatchers> watchers{};
};

// Solifuge, the cave spider (0x34 bytes): the stun, the sky walk and the
// brood it calls (BOSS_AI.md §3).
struct SSolifugeState
{
    int32_t skytimer = 0;              // +0x00 ticks to the next sky walk
    int32_t skyphase = 0;              // +0x04 0 on the ground, 1 going up, 2 in the air
    int32_t skyticks = 0;              // +0x08 ticks left in the air
    int32_t band = 0;                  // +0x0c health band: 0 above 66%, 1 above 33%, 2 the rest
    int32_t stunticks = 0;             // +0x10 ticks left stunned
    int32_t stunned = 0;               // +0x14 stunned on the ground
    int32_t tinted = 0;                // +0x18 his materials are blued (paralysed)
    std::array<int32_t, 3> brood{};    // +0x1c map indices of the spiders he called
    int32_t landing = 0;               // +0x28 the last 16 ticks of the sky walk
    int32_t pissedoff = 0;             // +0x2c a band was lost: the roar to play
    int32_t slam = 0;                  // +0x30 just landed: the next stance slams the ground
};

// Jhaga, the priest (0x5c bytes): his teleports round four waypoints
// (BOSS_AI.md §4).
struct SJhagaState
{
    static constexpr int32_t kPoints = 5;

    int32_t mode = 0;                  // +0x00 0 not set up, 1 resting, 2 teleporting
    // +0x04 / +0x18 / +0x2c (retail keeps x, y and z in three arrays): the
    // waypoint below the centre in x and y, below / above, above / below,
    // above in both, then the centre.
    std::array<S3DPoint, kPoints> points{};
    int32_t resttimer = 0;             // +0x40 ticks to the next teleport cycle
    int32_t draw = 0;                  // +0x44 random(0, 2) at each cycle, zeroed at once (unused)
    int32_t hop = 0;                   // +0x48 which jump of the cycle
    S3DPoint dest{};                   // +0x4c where this jump lands
    int32_t frame = 0;                 // +0x58 ticks into this jump
};

// Yhagoro, the last boss (0xc bytes): walking or fighting, by his health
// (BOSS_AI.md §5).
struct SYhagoroState
{
    int32_t mode = 0;                  // +0x00 0 walking, 1 fighting, -1 going back to walking
    int32_t towalk = 0;                // +0x04 the health band says walk
    int32_t tocombat = 0;              // +0x08 the health band says fight
};

// A character's boss state: none, or its boss's (retail +0x284).
using TBossState = std::variant<std::monostate, SBaezState, SSolifugeState, SJhagaState, SYhagoroState>;

// The state of boss S; a kind set without its state (retail would read a
// null +0x284) starts from a fresh one, as the first tick gives.
template <typename S>
S& BossStateOf(TBossState& state)
{
    if (S* s = std::get_if<S>(&state))
        return *s;
    return state.emplace<S>();
}
