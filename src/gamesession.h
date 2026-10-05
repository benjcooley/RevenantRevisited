// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   gamesession.h - TGameSession, the lifetime of one game              *
// *************************************************************************
//
// Everything that lives exactly as long as one play-through — the areas, the
// players, the game states and the sector working set — from New Game or a
// load until the player leaves the game. Retail kept this inside TPlayScreen
// (Initialize .. Close); the session owns it so the screens only present it.
// Owned by TGameFlow. Design: docs/gameflow/ARCHITECTURE.md §3; retail
// behavior: docs/gameflow/forensics/SAVE_GAME.md, GAME_FLOW.md §2.3.
#pragma once

#include <cstdint>
#include <string>

class TGameMap;

// How a game starts: retail TPlayScreen start modes 0 (new game) and 1 (load
// a save slot, falling back to a new game when the slot can't be loaded).
struct SSessionStart
{
    enum class EKind : uint8_t { NewGame, LoadSlot };

    EKind       kind = EKind::NewGame;
    std::string slot;              // LoadSlot: the save slot's name

    // Developer override (--level / --sector): once loaded, move the main
    // player to the middle of this sector. A negative sector means the
    // level's first sector; a negative level means no override.
    int32_t devLevel   = -1;
    int32_t devSectorX = -1;
    int32_t devSectorY = -1;
};

class TGameSession
{
  public:
    // Begin a game, replacing any game in progress. Loading runs as a
    // sequence of steps: call Step() until it returns false.
    void Start(const SSessionStart& start);
    bool Step();
    // How far the load is, per mille: where retail's loading bar stood after
    // the matching part of TPlayScreen::Initialize (0x0047a660).
    [[nodiscard]] int32_t Progress() const;
    [[nodiscard]] bool  Ready() const  { return state == EState::Ready; }
    [[nodiscard]] bool  Failed() const { return state == EState::Failed; }

    // Leave the game: unload the world, the players and the areas.
    void End();

    // Requests made during play, carried out at the start of the next
    // simulation tick (retail TPlayScreen +0x5e4..+0x5f0).
    void RequestLoad(const std::string& slot);
    void RequestSave(const std::string& slot);
    // REVSYNC: 0x0047e850 — save to the first unused "Quick Save N" slot.
    void RequestQuickSave();
    // Developer convenience, not retail: reload the slot most recently saved
    // or loaded this session.
    void RequestReloadLastSlot();
    void ProcessRequests();

    // REVSYNC: the level half of the map pane's sector update (0x00459220,
    // 0x00459b80). When the camera has moved to another level -- a player
    // went through an exit or `pos` -- make that level current, and put
    // every player who left the map back into the sector under it. A level
    // not yet loaded loads a slice per call under the text bar's loading
    // line. Runs each tick after the simulation. True when the camera's
    // level is the current one.
    bool EnterLevel();
    // A level is part loaded: the world holds until EnterLevel finishes it,
    // as retail's synchronous load stalled the game.
    [[nodiscard]] bool LevelLoading() const { return levelLoading; }

  private:
    enum class EState : uint8_t { Idle, Loading, Ready, Failed };

    // A load step finishes, needs another tick (it reports how far it is in
    // stepFraction), or fails the load.
    enum class EStep : uint8_t { Done, Again, Failed };

    struct SStep
    {
        const char* name;
        EStep (TGameSession::*run)();
        int32_t     progress;          // per mille once the step is done
    };
    static const SStep kLoadSteps[];

    // Load steps, in order.
    EStep LoadAreas();
    EStep LoadExits();
    EStep LoadGameState();
    EStep EnterWorld();

    void PlaceAtDevStart(const TGameMap& map) const;
    void PlacePlayers(TGameMap& map, bool entering) const;
    bool SaveNow(const std::string& slot);

    SSessionStart start;
    int32_t       nextStep = 0;
    float         stepFraction = 0.0f; // how far the running step is (EStep::Again)
    EState        state    = EState::Idle;
    std::string   lastSlot;        // last slot saved or loaded

    std::string   pendingLoad;     // requested slot, empty = none
    std::string   pendingSave;
    bool          levelLoading = false;    // EnterLevel is part way through a level
};
