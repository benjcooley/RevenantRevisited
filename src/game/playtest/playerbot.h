// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *   playerbot.h - Locke played by code, the way a person plays him      *
// *************************************************************************
//
// A Revisited feature (docs/gameplay/PLAYER_BOT.md): it stands off out of an
// enemy's reach, comes in to strike when the enemy is open and backs off,
// steps out of incoming blows and missiles (blocking or dodging some it
// can't), and prefers open ground, falling back to it from a tight spot.
// It plays by pressing the player's keys -- the control map's bindings,
// through the game mode's key entry -- so everything it does runs the
// player's own code.

#pragma once

#include "engine/core/revtypes.h"   // S3DPoint
#include "engine/core/saferef.h"

#include <cstdint>
#include <deque>
#include <vector>

class TCharacter;

class TPlayerBot
{
  public:
    struct SProfile
    {
        int32_t reaction = 5;           // ticks the senses lag behind the world (~0.2 s)
        float   aggression = 0.6f;      // chance a decision window comes in to strike
        float   block = 0.35f;          // chance to block a blow it can't step out of
        float   dodge = 0.15f;          // chance to dodge one instead
        int32_t margin = 24;            // standoff margin outside the enemy's reach
        int32_t openness = 4;           // clearance (walk cells) under which a spot is tight
        int32_t weaveMin = 18;          // a weave strafe's length, in ticks
        int32_t weaveMax = 40;
    };

    void Enable(bool on);
      // Starts or stops it; stopping lets go of every key it holds
    [[nodiscard]] bool Enabled() const { return enabled; }
    void SetProfile(const SProfile& p) { profile = p; }
    void Seed(uint32_t seed) { rng = seed ? seed : 1u; }
      // Its own random stream: it never draws on the game's
    void Tick();
      // Once a game tick, before the play screen reads the held controls

  private:
    // ---- What it sees (TBotSenses in the design) ----------------------
    struct SFoe
    {
        TSafeRef<TCharacter> who;
        S3DPoint pos;
        int32_t edge = 0;               // edge to edge from Locke
        int32_t bearing = 0;            // angle from Locke to it
        int32_t reach = 0;              // its blows' longest reach
        bool    swinging = false;       // in an ATTACK that will strike
        bool    aimed = false;          // ... with Locke inside its arc
        int32_t impact = -1;            // ticks to that blow's impact frame
        int32_t hitreach = 0;           // that blow's reach
    };
    struct SMissile
    {
        int32_t id = -1;                // map index
        S3DPoint pos;
        S3DPoint vel;                   // per tick, from its last positions
        int32_t closest = 0;            // its closest approach to Locke ...
        int32_t when = 0;               // ... in this many ticks
    };
    struct SView
    {
        int32_t tick = 0;
        std::vector<SFoe> foes;
        std::vector<SMissile> missiles;
    };
    void See();
    [[nodiscard]] const SView& Seen() const;
      // The view `reaction` ticks old (or the oldest kept)

    // ---- The ground (TBotSpace) ----------------------------------------
    static constexpr int32_t kWindow = 32;                  // cells each side of Locke
    static constexpr int32_t kSpan = 2 * kWindow + 1;
    struct SCell
    {
        int16_t height = 0;
        bool    stand = false;          // a character of Locke's radius can stand here
        int16_t clear = 0;              // cells to the nearest place it can't
        int16_t steps = -1;             // path length from Locke; -1 unreachable
        int16_t from = -1;              // the path's previous cell
    };
    void Survey();
    [[nodiscard]] SCell* Cell(int32_t gx, int32_t gy);
    [[nodiscard]] int32_t Clearance(const S3DPoint& p);
    [[nodiscard]] bool OpenSpot(S3DPoint& spot);
      // The best open ground in reach, away from the foes; false if none
      // beats where Locke stands
    [[nodiscard]] int32_t PathAngle(const S3DPoint& spot);
      // The way to walk toward spot along the surveyed path, or -1

    // ---- What it does (TBotBrain) ---------------------------------------
    enum class EState : uint8_t { Idle, Standoff, Approach, Strike, BackOff, Evade, Block, Missile, Fallback };
    void Decide();
    [[nodiscard]] int32_t Steer(int32_t angle, bool room = false);
      // `angle`, or the nearest of its neighbours Locke can step to (with
      // `room`, the one of them with the most room); -1 if none. A way
      // that just went nowhere is left out a while.
    void SetState(EState s, const char* why);

    // ---- How it does it (TBotInput) --------------------------------------
    void Hold(uint32_t cmdflags);
      // The held controls this tick (direction, block, run): keys pressed
      // and released to match
    void Tap(int32_t command);
      // A command's keys down this tick, up the next
    void Flush();
    void ReleaseAll();
    void Key(int32_t key, bool down);

    [[nodiscard]] uint32_t Random();
    [[nodiscard]] float Chance() { return float(Random() % 10000) / 10000.0f; }
    [[nodiscard]] int32_t Between(int32_t lo, int32_t hi) { return lo + int32_t(Random() % uint32_t(hi - lo + 1)); }

    SProfile profile;
    bool enabled = false;
    uint32_t rng = 1;
    int32_t tick = 0;

    std::deque<SView> views;
    std::vector<std::pair<int32_t, S3DPoint>> lastmissiles;    // map index, position
    S3DPoint lastpos;
    float speed = 4.0f;                 // Locke's measured speed, units a tick

    std::vector<SCell> cells = std::vector<SCell>(kSpan * kSpan);
    int32_t surveyx = 0, surveyy = 0;   // the window's centre cell
    int32_t surveyed = -1000;           // tick of the last survey

    EState state = EState::Idle;
    int32_t weaveside = 1;
    int32_t weaveuntil = 0;
    int32_t blockuntil = 0;
    int32_t calmsince = 0;              // last tick a foe was near
    int32_t attackbutton = 0;           // the attack it is coming in with (1..3)
    S3DPoint fallbackspot;              // the open ground it's falling back to ...
    int32_t fallbackuntil = 0;          // ... committed until then (or there)
    int32_t fallbackcooldown = 0;       // no new fall back before then
    int32_t heldangle = -1;             // the way it's holding
    int32_t stuckticks = 0;             // ... and how long that has gone nowhere
    int32_t blockedangle = -1;          // a way that went nowhere ...
    int32_t blockeduntil = 0;           // ... left out until then

    std::vector<int32_t> heldkeys;      // keys it has down
    uint32_t heldflags = 0;
    std::vector<int32_t> tapkeys;       // a tap's keys, let go next tick
};

extern TPlayerBot PlayerBot;
