// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   gamesession.cpp - TGameSession, the lifetime of one game            *
// *************************************************************************

#include "gamesession.h"

#include "area.h"
#include "dialog.h"
#include "exit.h"
#include "gamemap.h"
#include "logging.h"
#include "mapmanager.h"
#include "mappane.h"
#include "player.h"
#include "revenant.h"
#include "savegame.h"
#include "sector.h"
#include "textbar.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <iterator>
#include <utility>
#include <vector>

// Progress is retail's loading bar after the same work in
// TPlayScreen::Initialize (0x0047a660; LoadingScreen_SPEC.md §1): the module
// mount, HUD archives, scripts and areas reach 165; the panes (the map pane
// reads exit.def) 215; the game load and effect imagery 240; the sectors
// around the player fill the rest.
const TGameSession::SStep TGameSession::kLoadSteps[] = {
    { "areas", &TGameSession::LoadAreas,       0,  165 },
    { "exits", &TGameSession::LoadExits,     165,  215 },
    { "game",  &TGameSession::LoadGameState, 215,  240 },
    { "world", &TGameSession::EnterWorld,    240, 1000 },
};

// REVSYNC: the load dialog's in-game load (0x00539590): LoadGame, the bar at
// 80, then the sectors around the player with the bar at progress x 800 /
// 1000 (callback 0x00539990), which starts under the 80 already drawn
// (INGAME_MENU.md §5.1). The areas and exits stay: retail's LoadGame leaves
// every area (in the game step's reset) but doesn't read area.def or
// exit.def again.
const TGameSession::SStep TGameSession::kGameLoadSteps[] = {
    { "game",  &TGameSession::LoadGameState, 0,  80 },
    { "world", &TGameSession::EnterWorld,    0, 800 },
};

namespace {

// About a frame's worth of sector loading: a staged load gives the frame loop
// back after this long, so the loading bars move while a level comes in.
constexpr auto kLoadSlice = std::chrono::milliseconds(30);

// Loads sectors of `level` for one slice; the map, complete once
// !Loading(). nullptr if the level can't be loaded (a negative level).
TGameMap* LoadSlice(int32_t level)
{
    const auto until = std::chrono::steady_clock::now() + kLoadSlice;
    TGameMap* map = nullptr;
    do
        map = MapManager.LoadStaged(level, 1);
    while (map && map->Loading() && std::chrono::steady_clock::now() < until);
    return map;
}

}  // namespace

// ***********
// * Loading *
// ***********

void TGameSession::Start(const SSessionStart& request)
{
    Begin(request, kLoadSteps, (int32_t)std::size(kLoadSteps));
}

void TGameSession::Begin(const SSessionStart& request, const SStep* plan, int32_t planSteps)
{
    start    = request;
    steps    = plan;
    numSteps = planSteps;
    nextStep = 0;
    stepFraction = 0.0f;
    state    = EState::Loading;
    levelLoading = false;

    if (start.kind == SSessionStart::EKind::LoadSlot)
        log_info("[session] start: load '%s'", start.slot.c_str());
    else
        log_info("[session] start: new game");
}

bool TGameSession::Step()
{
    if (state != EState::Loading)
        return false;

    const SStep& step = steps[nextStep];
    switch ((this->*step.run)())
    {
      case EStep::Failed:
        log_error("[session] load step '%s' failed", step.name);
        state = EState::Failed;
        return false;
      case EStep::Again:
        return true;
      case EStep::Done:
        break;
    }

    stepFraction = 0.0f;
    if (++nextStep == numSteps)
    {
        state = EState::Ready;
        return false;
    }
    return true;
}

// Retail's bars only grew: a step that starts under the last one's end
// reports that end until it passes it.
int32_t TGameSession::Progress() const
{
    switch (state)
    {
    case EState::Ready:   return steps[numSteps - 1].to;
    case EState::Loading:
    {
        const int32_t done = nextStep > 0 ? steps[nextStep - 1].to : 0;
        const SStep&  step = steps[nextStep];
        const int32_t now  = step.from + static_cast<int32_t>((step.to - step.from) * stepFraction);
        return (std::max)(done, now);
    }
    default:              return 0;
    }
}

// REVSYNC: TAreaMgr::Initialize in TPlayScreen::Initialize @ 0x0047a660 —
// the active module's area.def.
TGameSession::EStep TGameSession::LoadAreas()
{
    return AreaManager.Initialize() ? EStep::Done : EStep::Failed;
}

// REVSYNC: TExit::Initialize @ 0x0050c880, which retail ran from the map
// pane's initialize (0x0044d5c0) inside TPlayScreen::Initialize. A missing or
// malformed exit.def leaves fewer exits; the game still starts.
TGameSession::EStep TGameSession::LoadExits()
{
    TExit::Initialize();
    return EStep::Done;
}

// REVSYNC: TPlayScreen::Initialize @ 0x0047a660, start modes 0 and 1. A slot
// that can't be loaded falls back to a new game, as retail (which also
// showed GAMENOTFOUND on the text bar).
TGameSession::EStep TGameSession::LoadGameState()
{
    if (start.kind == SSessionStart::EKind::LoadSlot)
    {
        ::SaveGame.RefreshSlots();
        if (::SaveGame.Load(start.slot.c_str()))
        {
            lastSlot = start.slot;
            return EStep::Done;
        }
        log_warn("[session] save '%s' can't be loaded; starting a new game", start.slot.c_str());
    }
    return ::SaveGame.LoadNewGame() ? EStep::Done : EStep::Failed;
}

// REVSYNC: the end of TPlayScreen::Initialize @ 0x0047a660 loads the sectors
// around the main player (0x004997d0, its progress callback 0x0047b260 filling
// the loading bar). The port loads the main player's whole level, about a
// frame's worth of sectors per tick, and then puts each player into the
// sector it stands in.
TGameSession::EStep TGameSession::EnterWorld()
{
    if (!Player)
        return EStep::Failed;

    const int32_t level = start.devLevel >= 0 ? start.devLevel : Player->GetLevel();
    TGameMap* map = LoadSlice(level);
    if (!map)
    {
        log_error("[session] level %d can't be loaded", level);
        return EStep::Failed;
    }
    if (map->Loading())
    {
        stepFraction = map->LoadFraction();
        return EStep::Again;
    }
    MapManager.SetCurrentMap(map);
    if (start.devLevel >= 0)
        PlaceAtDevStart(*map);

    PlacePlayers(*map, /*entering=*/true);

    // The camera starts on the main player and follows it (retail set this
    // up as TPlayScreen::Initialize made the player the main one).
    S3DPoint pos = Player->Pos();
    MapPane.CenterOnObj(Player);
    MapPane.SetMapPos(pos);
    MapPane.SetMapLevel(Player->GetLevel());

    log_info("[session] entered level %d; player at (%d,%d,%d) in sector %d_%d",
             level, pos.x, pos.y, pos.z, pos.x >> SECTORWSHIFT, pos.y >> SECTORHSHIFT);
    return EStep::Done;
}

// REVSYNC: 0x00459b80 -- every player on the map's level who isn't in a
// sector goes into the one under it. (Retail also took players that had
// left the game out of the map; single player has none.) Retail ran it on
// each sector update, so a player left where the level has no sector was
// retried as the window moved; the port retries each tick and reports it
// only on the tick the level is entered.
void TGameSession::PlacePlayers(TGameMap& map, bool entering) const
{
    for (int32_t i = 0; i < PlayerManager.NumPlayers(); i++)
    {
        TPlayer* player = PlayerManager.GetPlayer(i);
        if (!player || player->GetSector() || player->GetLevel() != map.Level())
            continue;

        const S3DPoint pos = player->Pos();
        TSector* sector = map.FindSector(pos.x >> SECTORWSHIFT, pos.y >> SECTORHSHIFT);
        if (!sector)
        {
            if (entering)
                log_warn("[session] no sector under player %d at (%d,%d,%d) on level %d",
                         i, pos.x, pos.y, pos.z, map.Level());
            continue;
        }
        sector->AddObject(player);
    }
}

// REVSYNC: the release half of the sector update (0x00459490): retail kept
// the sectors around the camera and every active player loaded and released
// the rest to the working set (0x00498460 -> curmap). The port loads whole
// levels, so after a level change it keeps the camera's level and those with
// a player on them and releases the others -- written to the working set
// (TGameMap::Unload), and their imagery leaves the GPU with them. Keeping
// every visited level filled the renderer's image pool: level 0 alone holds
// ~3,700 images of the 4,096.
void TGameSession::ReleaseUnusedLevels(int32_t current) const
{
    std::vector<int32_t> unused;
    for (int32_t i = 0; i < MapManager.NumCached(); i++)
    {
        const TGameMap* map = MapManager.Cached(i);
        if (!map || map->Level() == current)
            continue;
        bool occupied = false;
        for (int32_t p = 0; p < PlayerManager.NumPlayers() && !occupied; p++)
        {
            const TPlayer* player = PlayerManager.GetPlayer(p);
            occupied = player && player->GetLevel() == map->Level();
        }
        if (!occupied)
            unused.push_back(map->Level());
    }
    for (const int32_t level : unused)
    {
        log_info("[session] level %d released", level);
        MapManager.Evict(level);
    }
}

// REVSYNC: the map loader 0x004597b0 and its progress callback 0x00459a00.
// Retail loaded a new level synchronously, drawing the text bar's loading
// line straight to the display between sectors: LOADMAPMSG with the strip
// under it filling to progress x 180 / 1000, then ClearHealthDisplay, which
// leaves the message in the feed. The port can't present mid-tick, so it
// loads a slice per frame while the PlayScreen holds the world -- what the
// player saw either way: nothing moves while the strip fills (EXITS.md §3.2).
bool TGameSession::EnterLevel()
{
    if (state != EState::Ready)
        return true;

    const int32_t level = MapPane.GetMapLevel();
    TGameMap* map = MapManager.CurrentMap();
    bool entering = false;
    if (!map || map->Level() != level)
    {
        map = MapManager.GetCached(level);
        if (!map || map->Loading())
        {
            if (!levelLoading)
            {
                levelLoading = true;
                TextBar.SetHealthDisplay(DialogList.GetLine("LOADMAPMSG"));
            }
            map = LoadSlice(level);
            if (map && map->Loading())
            {
                const int32_t bar = int32_t(map->LoadFraction() * 180.0f);
                TextBar.SetLevels(bar, bar);
                return false;
            }
            levelLoading = false;
            TextBar.ClearHealthDisplay();
        }

        if (!map)
        {
            log_error("[session] level %d can't be loaded", level);
            return false;
        }
        MapManager.SetCurrentMap(map);
        log_info("[session] entered level %d", level);
        ReleaseUnusedLevels(level);
        entering = true;
    }

    PlacePlayers(*map, entering);
    return true;
}

// The player isn't in a sector yet, so the move is a plain position write.
void TGameSession::PlaceAtDevStart(const TGameMap& map) const
{
    TSector* sector = nullptr;
    if (start.devSectorX >= 0)
        sector = map.FindSector(start.devSectorX, start.devSectorY);
    else if (!map.Sectors().empty())
        sector = map.Sectors().front();
    if (!sector)
    {
        log_warn("[session] dev start: no sector %d_%d_%d", map.Level(),
                 start.devSectorX, start.devSectorY);
        return;
    }

    S3DPoint pos;
    pos.x = (sector->SectorX() << SECTORWSHIFT) + SECTORWIDTH / 2;
    pos.y = (sector->SectorY() << SECTORHSHIFT) + SECTORHEIGHT / 2;
    pos.z = sector->ReturnWalkmap((pos.x & (SECTORWIDTH - 1)) >> WALKMAPSHIFT,
                                  (pos.y & (SECTORHEIGHT - 1)) >> WALKMAPSHIFT);
    Player->SetPos(pos, map.Level(), /*override=*/true);
}

// **********
// * Ending *
// **********

// REVSYNC: the world half of TPlayScreen::Close @ 0x0047b290. Nothing
// continues from the working set once a game ends (a new game clears it, a
// load replaces it), so the loaded sectors are dropped rather than written.
void TGameSession::End()
{
    if (state == EState::Idle)
        return;

    AreaManager.ExitAll();
    TExit::Close();                 // retail: the map pane's close (0x0044d9c0)
    MapManager.ClearCurMap();
    PlayerManager.Clear();
    AreaManager.Close();

    pendingLoad.clear();
    pendingSave.clear();
    levelLoading = false;
    state = EState::Idle;
    log_info("[session] ended");
}

// ************
// * Requests *
// ************

void TGameSession::RequestLoad(const std::string& slot)
{
    pendingLoad = slot;
}

void TGameSession::RequestSave(const std::string& slot)
{
    pendingSave = slot;
}

// Retail reads the name format from the string table ("quicksavefmt"); the
// port has no string table yet and uses retail's built-in default.
void TGameSession::RequestQuickSave()
{
    ::SaveGame.RefreshSlots();
    char name[128];
    for (int32_t n = 1; n < 1000; n++)
    {
        std::snprintf(name, sizeof(name), "Quick Save %d", n);
        if (::SaveGame.FindSlot(name) < 0)
            break;
    }

    // REVSYNC: QuickSave @ 0x0047e850 follows writing the thumbnail
    // (0x0047dd08). The capture is read back after the next frame, so the
    // save waits for it; requested at once, it could store the previous
    // picture.
    ::SaveGame.CaptureThumbnail({}, [this, slot = std::string(name)] { RequestSave(slot); });
}

void TGameSession::RequestReloadLastSlot()
{
    if (lastSlot.empty())
    {
        log_warn("[session] nothing saved or loaded yet this session");
        return;
    }
    RequestLoad(lastSlot);
}

// REVSYNC: request handling in the PlayScreen frame (0x0047bfab..0x0047c0b8):
// a pending load, then a pending save. Retail announced each on the text bar
// and loaded synchronously; here a load starts and runs a step a tick, the
// PlayScreen holding the world (TPlayScreen::Update), so a pending save
// waits for the next tick that isn't loading.
void TGameSession::ProcessRequests()
{
    if (state == EState::Loading)
        return;

    if (!pendingLoad.empty())
    {
        const std::string slot = std::exchange(pendingLoad, {});
        ::SaveGame.RefreshSlots();
        if (::SaveGame.FindSlot(slot.c_str()) < 0)
        {
            log_warn("[session] no save slot named '%s'", slot.c_str());
        }
        else
        {
            SSessionStart load;
            load.kind = SSessionStart::EKind::LoadSlot;
            load.slot = slot;
            Begin(load, kGameLoadSteps, (int32_t)std::size(kGameLoadSteps));
            return;
        }
    }

    if (!pendingSave.empty())
        SaveNow(std::exchange(pendingSave, {}));
}

bool TGameSession::SaveNow(const std::string& slot)
{
    if (!::SaveGame.Save(slot.c_str()))
        return false;
    lastSlot = slot;
    return true;
}
