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

#include <cstdio>
#include <iterator>
#include <utility>

const TGameSession::SStep TGameSession::kLoadSteps[] = {
    { "areas", &TGameSession::LoadAreas },
    { "exits", &TGameSession::LoadExits },
    { "game",  &TGameSession::LoadGameState },
    { "world", &TGameSession::EnterWorld },
};

// ***********
// * Loading *
// ***********

void TGameSession::Start(const SSessionStart& request)
{
    start    = request;
    nextStep = 0;
    state    = EState::Loading;

    if (start.kind == SSessionStart::EKind::LoadSlot)
        log_info("[session] start: load '%s'", start.slot.c_str());
    else
        log_info("[session] start: new game");
}

bool TGameSession::Step()
{
    if (state != EState::Loading)
        return false;

    const SStep& step = kLoadSteps[nextStep];
    if (!(this->*step.run)())
    {
        log_error("[session] load step '%s' failed", step.name);
        state = EState::Failed;
        return false;
    }

    if (++nextStep == (int32_t)std::size(kLoadSteps))
    {
        state = EState::Ready;
        return false;
    }
    return true;
}

float TGameSession::Progress() const
{
    switch (state)
    {
    case EState::Ready:   return 1.0f;
    case EState::Loading: return (float)nextStep / (float)std::size(kLoadSteps);
    default:              return 0.0f;
    }
}

// REVSYNC: TAreaMgr::Initialize in TPlayScreen::Initialize @ 0x0047a660 —
// the active module's area.def.
bool TGameSession::LoadAreas()
{
    return AreaManager.Initialize();
}

// REVSYNC: TExit::Initialize @ 0x0050c880, which retail ran from the map
// pane's initialize (0x0044d5c0) inside TPlayScreen::Initialize. A missing or
// malformed exit.def leaves fewer exits; the game still starts.
bool TGameSession::LoadExits()
{
    TExit::Initialize();
    return true;
}

// REVSYNC: TPlayScreen::Initialize @ 0x0047a660, start modes 0 and 1. A slot
// that can't be loaded falls back to a new game, as retail (which also
// showed GAMENOTFOUND on the text bar).
bool TGameSession::LoadGameState()
{
    if (start.kind == SSessionStart::EKind::LoadSlot)
    {
        ::SaveGame.RefreshSlots();
        if (::SaveGame.Load(start.slot.c_str()))
        {
            lastSlot = start.slot;
            return true;
        }
        log_warn("[session] save '%s' can't be loaded; starting a new game", start.slot.c_str());
    }
    return ::SaveGame.LoadNewGame();
}

// REVSYNC: the end of TPlayScreen::Initialize @ 0x0047a660 loads the sectors
// around the main player (0x004997d0). The port loads the main player's whole
// level and puts each player into the sector it stands in.
bool TGameSession::EnterWorld()
{
    if (!Player)
        return false;

    const int32_t level = start.devLevel >= 0 ? start.devLevel : Player->GetLevel();
    TGameMap* map = MapManager.SetCurrentLevel(level);
    if (!map)
    {
        log_error("[session] level %d has no sectors", level);
        return false;
    }
    if (start.devLevel >= 0)
        PlaceAtDevStart(*map);

    PlacePlayers(*map);

    const S3DPoint pos = Player->Pos();
    log_info("[session] entered level %d; player at (%d,%d,%d) in sector %d_%d",
             level, pos.x, pos.y, pos.z, pos.x >> SECTORWSHIFT, pos.y >> SECTORHSHIFT);
    return true;
}

// REVSYNC: 0x00459b80 -- every player on the map's level who isn't in a
// sector goes into the one under it. (Retail also took players that had
// left the game out of the map; single player has none.)
void TGameSession::PlacePlayers(TGameMap& map) const
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
            log_warn("[session] no sector under player %d at (%d,%d,%d) on level %d",
                     i, pos.x, pos.y, pos.z, map.Level());
            continue;
        }
        sector->AddObject(player);
    }
}

void TGameSession::EnterLevel()
{
    if (state != EState::Ready)
        return;

    const int32_t level = MapPane.GetMapLevel();
    TGameMap* map = MapManager.CurrentMap();
    if (!map || map->Level() != level)
    {
        // Retail loaded synchronously with LOADMAPMSG on the text bar
        // (EXITS.md §3.2). The port shows it for a frame, then loads.
        if (!MapManager.GetCached(level) && !loadAnnounced)
        {
            TextBar.Print("%s", DialogList.GetLine("LOADMAPMSG"));
            loadAnnounced = true;
            return;
        }

        map = MapManager.SetCurrentLevel(level);
        if (loadAnnounced)
        {
            TextBar.Clear();
            loadAnnounced = false;
        }
        if (!map)
        {
            log_error("[session] level %d has no sectors", level);
            return;
        }
        log_info("[session] entered level %d", level);
    }

    PlacePlayers(*map);
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
    // REVSYNC: QuickSave @ 0x0047e850 follows writing the thumbnail
    // (0x0047dd08).
    ::SaveGame.CaptureThumbnail();
    ::SaveGame.RefreshSlots();
    char name[128];
    for (int32_t n = 1; n < 1000; n++)
    {
        std::snprintf(name, sizeof(name), "Quick Save %d", n);
        if (::SaveGame.FindSlot(name) < 0)
            break;
    }
    RequestSave(name);
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
// a pending load, then a pending save. Retail announced each on the text bar.
void TGameSession::ProcessRequests()
{
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
            Start(load);
            while (Step())
                ;
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
