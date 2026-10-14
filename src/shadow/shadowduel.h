// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *        shadowduel.h - Shadow Smoke's fights, without the animation      *
// *************************************************************************
//
// A fight between Locke and the monsters engaging him, resolved on a
// timeline of game ticks rather than played (docs/shadow/ARCHITECTURE.md
// §6). What is real is called, not copied:
//
// - the attack records of char.def, and which of them a character may use
//   (the parts of IsValidAttack that don't read animation state);
// - the dice of every blow (TCharacter::RollAttack), the guard and glance
//   arithmetic (TCharacter::WeighBlow) and the blow's landing with its
//   experience (TCharacter::LandBlow): ResolveHit's own pieces;
// - the monsters' habits: their attack timer (ATTACKFREQ), their pick of an
//   attack (attackpcnt, as RandomAttack and FindPcntAttack draw it), their
//   guard (BLOCKFREQ, BLOCKMIN..BLOCKMAX), their retreat (RETREATAT).
//
// What is modelled is where the two stand and when each acts: a monster
// closes at its own speed to its attack range and fights from there; Locke
// fights at that range with what is valid there.
//
// A live fight resolves in one game tick: each event goes through the
// game's functions in order, and the model reads the game back between
// them, so a kill's level-up refills Locke when the game says. Its duration
// then passes as real ticks (the caller's), while Locke loots. A trial
// fight -- sizing one up -- runs the same timeline on copies of the
// numbers, with the game's random stream lent to Shadow's own so the game's
// is untouched.

#pragma once

#include "../saferef.h"

#include <cstdint>
#include <random>
#include <string>
#include <vector>

class TCharacter;
class TPlayer;
class TShadowLedger;
class TShadowLog;
struct SCharAttackData;

// A player's habits (docs/gameplay/PLAYER_BOT.md §3, GRIND.md §9).
struct SShadowTactics
{
    int32_t reaction    = 5;    // ticks before he reads a monster's wind-up
    int32_t blockpct    = 35;   // how often he guards a blow he sees coming
    int32_t floorpct    = 35;   // drinks below this share of his health
};

struct SShadowFightReport
{
    bool    won     = false;    // every foe dead
    bool    died    = false;    // Locke dead
    bool    escaped = false;    // a foe got away (retreated, faster than him)
    bool    stalled = false;    // nobody won in the time allowed
    int32_t ticks   = 0;        // how long it took
    int32_t blows   = 0;        // Locke's
    int32_t hits    = 0;        // of them, those that weren't glances
    int32_t taken   = 0;        // blows that reached him
    int32_t dealt   = 0;        // damage he dealt
    int32_t suffered = 0;       // and took
    int32_t blocks  = 0;        // blows he guarded
    int32_t potions = 0;        // drunk in the fight
    std::vector<std::string> killed;
};

class TShadowDuel
{
  public:
    enum class EMode : uint8_t { Live, Trial };

    TShadowDuel(EMode mode, const SShadowTactics& tactics, uint32_t seed)
        : mode(mode), tactics(tactics), random(seed)
    {
    }

    // Fights `foes` with Locke. Live: through the game at run tick `tick`,
    // with the ledger told what the fight may bring, events and the report
    // to `log`. Trial: on copies, nothing changes (ledger and log unused).
    SShadowFightReport Fight(TPlayer& locke, const std::vector<TCharacter*>& foes, int64_t tick,
                             TShadowLedger* ledger, TShadowLog* log);

  private:
    struct SFighter
    {
        TCharacter* who        = nullptr;
        bool        locke      = false;
        int32_t     health     = 0;     // the game's (live) or the copy (trial)
        double      fatigue    = 0.0;
        int32_t     reach      = 0;     // the range it fights from
        int32_t     ready      = 0;     // tick it is in range
        int32_t     busy       = 0;     // tick it may start something new
        int32_t     guard      = -1;    // guarding until this tick
        int32_t     nextattack = 0;     // monster: tick its attack timer runs out
        bool        retreating = false;
        // The blow in the air.
        const SCharAttackData* swing = nullptr;
        SFighter*   at         = nullptr;
        int32_t     impact     = -1;
        int32_t     damage     = 0;     // as thrown at the choice (tiered)
        int32_t     tohit      = 0;
        int32_t     roll       = 0;
        bool        read       = false; // the other side has decided about this blow
        // Chains: the last attack and when it started; the same-button
        // rule: the buttons of the last two.
        const SCharAttackData* last = nullptr;
        int32_t     lasttick   = -1000;
        int32_t     button1    = -1;
        int32_t     button2    = -1;
    };

    // `dmgpcnt`: the damage percentage the attack choice threw (a player's
    // is his StrengthMod, which RollAttack takes itself).
    void Start(SFighter& f, SFighter& target, const SCharAttackData& attack, int32_t now, int32_t dmgpcnt);
    void Land(SFighter& f, int32_t now, SShadowFightReport& report, TShadowLog* log);
    void MonsterTurn(SFighter& m, SFighter& locke, int32_t now);
    void LockeTurn(SFighter& locke, std::vector<SFighter>& foes, int32_t now, int64_t tick,
                   SShadowFightReport& report, TShadowLedger* ledger, TShadowLog* log);
    [[nodiscard]] bool Usable(SFighter& f, const SCharAttackData& a, int32_t dist, int32_t now) const;
    [[nodiscard]] int32_t Health(const SFighter& f) const;
    [[nodiscard]] int32_t Draw(int32_t low, int32_t high);   // Shadow's own stream, inclusive

    EMode          mode;
    SShadowTactics tactics;
    std::mt19937   random;
};
