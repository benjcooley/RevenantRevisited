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
//   (IsValidAttack's tests that don't read animation state, in its order);
// - the dice of every blow (TCharacter::RollAttack), the guard and glance
//   arithmetic (TCharacter::WeighBlow) and the blow's landing with its
//   experience (TCharacter::LandBlow): ResolveHit's own pieces;
// - the monsters' habits: their attack timer (ATTACKFREQ), their pick of an
//   attack or a move (attackpcnt, as RandomAttack and FindPcntAttack draw
//   it), their guard (BLOCKFREQ, BLOCKMIN..BLOCKMAX), their retreat
//   (RETREATAT, RETREATFOR);
// - every action's length and root motion, read from the character's
//   imagery: a blow, a monster's side step or jump, a flinch.
//
// What is modelled is where they stand and when each acts. Each fighter is
// a point on open ground with a facing (no walls, no collision). A monster
// closes to its MAXATTACKRANGE and from there does what its AI does; Locke
// fights as a player does (SShadowTactics: the player bot's habits, docs/
// gameplay/PLAYER_BOT.md): he holds off just outside its reach, comes in to
// strike when it is open, backs off, and steps out of, guards or takes
// the blows he sees coming. A blow strikes when its target is inside its
// reach and arc at the impact frame, as ResolveHit asks
// (docs/shadow/CALIBRATION.md measures all this against real fights).
//
// A live fight resolves in one game tick: each event goes through the
// game's functions in order, and the model reads the game back between
// them, so a kill's level-up refills Locke when the game says. Its duration
// then passes as real ticks (the caller's), while Locke loots. A trial
// fight -- sizing one up -- runs the same timeline on copies of the
// numbers, with the game's random stream lent to Shadow's own so the game's
// is untouched.

#pragma once

#include "engine/core/revtypes.h"

#include <cstdint>
#include <map>
#include <optional>
#include <random>
#include <string>
#include <utility>
#include <vector>

class TCharacter;
class TObjectInstance;
class TPlayer;
class TShadowLedger;
class TShadowLog;
struct SCharAttackData;

// A point on the ground, or a step across it.
struct SShadowPoint
{
    double x = 0.0, y = 0.0;
};

// An animation as the duel needs it: the frames it plays, and each frame's
// root motion, forward and aside of the facing it plays with.
struct SShadowAnim
{
    int32_t                   frames = 1;
    std::vector<SShadowPoint> steps;
};

// A player's habits (docs/gameplay/PLAYER_BOT.md §3, GRIND.md §9).
struct SShadowTactics
{
    int32_t reaction    = 5;    // ticks before he reads a monster's wind-up
    int32_t blockpct    = 35;   // how often he guards a blow he can't step out of
    int32_t dodgepct    = 15;   // ... or rolls aside from it instead
    int32_t aggression  = 60;   // how often a moment to come in to strike is taken
    int32_t window      = 12;   // ticks between those moments
    int32_t margin      = 24;   // how far outside a monster's reach he holds off
    int32_t floorpct    = 35;   // drinks below this share of his health
};

struct SShadowFightReport
{
    bool    won     = false;    // every foe dead
    bool    died    = false;    // Locke dead
    bool    escaped = false;    // a foe got away: out of sight in its retreat
    bool    stalled = false;    // nobody won in the time allowed
    int32_t ticks   = 0;        // how long it took
    int32_t first   = -1;       // when the first blow struck, either way (-1: none)
    int32_t swings  = 0;        // Locke's attacks, struck or not
    int32_t blows   = 0;        // of them, those that struck (a glance strikes)
    int32_t hits    = 0;        // of them, those that weren't glances
    int32_t foeswings = 0;      // the foes' attacks at him
    int32_t foeblows = 0;       // of them, those that struck
    int32_t foehits = 0;        // of them, those that weren't glances
    int32_t foemoves = 0;       // the foes' moves: side steps, jumps, feints
    int32_t taken   = 0;        // blows that reached him (a guarded glance doesn't)
    int32_t dealt   = 0;        // damage he dealt
    int32_t suffered = 0;       // and took
    int32_t blocks  = 0;        // blows he guarded
    int32_t evades  = 0;        // blows he stepped out of
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

    // Writes the fight's timeline into `sink` as it runs: every action begun,
    // every blow's fall, and where both stand each tick (calibration work).
    void TraceInto(std::string* sink) { trace = sink; }
    // A trial of a fight still to come: Locke as he will stand then, at `at`
    // (where a walk meets them), not where he is.
    void StandAt(const S3DPoint& at) { standat = at; }

  private:
    // What a fighter is doing. A blow and a move are both ATTACK actions to
    // the game, which leaves the one making them open to a counter (+25).
    // Idle: a monster that hasn't noticed him yet.
    enum class EAct : uint8_t { Idle, Stand, Blow, Move, Guard, Flinch, Drink, Dead };
    // Locke's state of mind (the bot's, PLAYER_BOT.md §2).
    enum class EPlan : uint8_t { Standoff, Approach, BackOff };

    struct SFighter
    {
        TCharacter* who        = nullptr;
        bool        locke      = false;
        int32_t     health     = 0;     // the game's (live) or the copy (trial)
        double      fatigue    = 0.0;
        double      radius     = 0.0;
        double      walk       = 0.0;   // units a tick, stepping in the combat stance
        SShadowPoint pos;
        double      face       = 0.0;   // 256 to a turn, as the game's angles (0 along +x)
        SFighter*   target     = nullptr;

        // The action under way, and the root motion carrying it.
        EAct               act      = EAct::Stand;
        int32_t            until    = 0;    // the tick it ends
        const SShadowAnim* anim     = nullptr;
        int32_t            animat   = 0;    // the tick it began
        const char*        doing    = "";   // its animation's name (a response reads it)
        int32_t            impactflags = 0; // a flinch's: stunned, knocked down

        // The blow in the air: its record, its dice, its impact tick.
        const SCharAttackData* swing = nullptr;
        int32_t     impact     = -1;
        int32_t     damage     = 0;     // as thrown at the choice (tiered)
        int32_t     tohit      = 0;
        int32_t     roll       = 0;
        int32_t     impactnum  = -1;
        bool        read       = false; // Locke has stepped out of this blow (counted once)

        // Monster: its AI's timer, its retreat.
        int32_t     nextattack = -1;
        int32_t     retreat    = -1;    // retreating until this tick

        // Chains: the last attack and when it started; the same-button rule:
        // the buttons of the last two.
        const SCharAttackData* last = nullptr;
        int32_t     lasttick   = -1000;
        int32_t     button1    = -1;
        int32_t     button2    = -1;

        // Locke's plan.
        EPlan       plan       = EPlan::Standoff;
        int32_t     button     = 0;     // the button he is coming in with
        int32_t     weaveside  = 1;
        int32_t     weaveuntil = 0;
        bool        moved      = false; // stepping this tick (a CA_MOVING attack needs it)
        bool        stepped    = false; // ... and last tick (a roll needs the stance)
        double      key        = -1.0;  // the direction key he holds (its angle), -1 none
        double      aim        = 0.0;   // the facing that key turns him to ...
        double      turnrate   = 8.0;   // ... this much a tick
    };

    // A search's result: the attack, its dice and its impact.
    struct SPick
    {
        const SCharAttackData* attack = nullptr;
        int32_t damage = -1, tohit = -1, roll = -1;
        int32_t impactnum = -1;
    };

    // The timeline, a tick at a time.
    void Advance(SFighter& f, int32_t now);
    void Land(SFighter& f, int32_t now, SShadowFightReport& report, TShadowLog* log);
    void MonsterTurn(SFighter& m, SFighter& locke, int32_t now, SShadowFightReport& report);
    // Locke's tick: his choices, and the direction key he holds after them.
    void LockeTurn(SFighter& locke, std::vector<SFighter>& foes, int32_t now, int64_t tick,
                   SShadowFightReport& report, TShadowLedger* ledger, TShadowLog* log);
    void LockeChoices(SFighter& locke, std::vector<SFighter>& foes, int32_t now, int64_t tick,
                      SShadowFightReport& report, TShadowLedger* ledger, TShadowLog* log);

    // The searches: a player's button (FindButtonAttack's three passes) and
    // a monster's percentage (FindPcntAttack's random picks), each a walk of
    // IsValidAttack over the candidates, throwing the dice at the first that
    // gets that far and tiering them again at each one after.
    [[nodiscard]] SPick ButtonSearch(SFighter& f, SFighter& target, int32_t button, int32_t now);
    [[nodiscard]] SPick PcntSearch(SFighter& f, SFighter& target, int32_t now);
    // IsValidAttack's tests before the dice; `pcnt` 0 for a player's button.
    [[nodiscard]] bool Usable(const SFighter& f, const SFighter& target, const SCharAttackData& a, int32_t pcnt,
                              int32_t now) const;
    // ... and after them: a death blow that doesn't kill, an impact the
    // target can't play. The impact it would play (-1: none), or kRefused.
    static constexpr int32_t kRefused = -2;
    [[nodiscard]] int32_t AfterDice(const SFighter& target, const SCharAttackData& a, int32_t damage) const;

    void StartAction(SFighter& f, EAct act, const SShadowAnim* anim, int32_t now, int32_t length);
    void StartBlow(SFighter& f, SFighter& target, const SPick& pick, int32_t now, SShadowFightReport& report);
    // OnAttacked and IsValidAttack's forced guard: a monster raises its guard.
    void MonsterGuard(SFighter& m, const SFighter& attacker, int32_t now, bool forced);
    void Flinch(SFighter& f, const SCharAttackData& attack, int32_t impactnum, int32_t now);
    void Walk(SFighter& f, double angle, double speed);

    [[nodiscard]] bool Open(const SFighter& target, const SFighter& attacker) const;
    [[nodiscard]] bool Strikes(const SFighter& f, const SFighter& target, const SCharAttackData& attack) const;
    [[nodiscard]] double Edge(const SFighter& a, const SFighter& b) const;
    [[nodiscard]] int32_t Health(const SFighter& f) const;
    [[nodiscard]] int32_t Draw(int32_t low, int32_t high);   // Shadow's own stream, inclusive
    [[nodiscard]] bool Chance(int32_t pct) { return Draw(1, 100) <= pct; }
    void Note(int32_t now, const SFighter& f, const std::string& what);

    // `name`'s animation for `who`, read from its imagery once a fight;
    // nullptr when it has none.
    [[nodiscard]] const SShadowAnim* Anim(TCharacter& who, const std::string& name);

    EMode          mode = EMode::Trial;
    SShadowTactics tactics;
    std::mt19937   random;
    std::map<std::pair<const void*, std::string>, SShadowAnim> anims;
    std::map<const TObjectInstance*, int32_t> drunk;  // trial: the belt's potions the copy has drunk
    int32_t        phase = 0;                   // the bot's own clock against the fight's
    std::string*   trace = nullptr;
    std::optional<S3DPoint> standat;
};
