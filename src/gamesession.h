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
    [[nodiscard]] float Progress() const;
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

  private:
    enum class EState : uint8_t { Idle, Loading, Ready, Failed };

    struct SStep
    {
        const char* name;
        bool (TGameSession::*run)();
    };
    static const SStep kLoadSteps[];

    // Load steps, in order.
    bool LoadAreas();
    bool LoadGameState();
    bool EnterWorld();

    void PlaceAtDevStart(const TGameMap& map) const;
    bool SaveNow(const std::string& slot);

    SSessionStart start;
    int32_t       nextStep = 0;
    EState        state    = EState::Idle;
    std::string   lastSlot;        // last slot saved or loaded

    std::string   pendingLoad;     // requested slot, empty = none
    std::string   pendingSave;
};
