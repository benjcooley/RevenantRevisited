// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   SaveGame.cpp - savegame objects                     *
// *************************************************************************

#include "savegame.h"

#include <cerrno>

#include "automap.h"
#include "dls.h"
#include "gamemap.h"
#include "inventory.h"
#include "logging.h"
#include "mapmanager.h"
#include "mappane.h"
#include "multictrl.h"
#include "player.h"
#include "playscreen.h"
#include "revdefs.h"
#include "revutils.h"
#include "script.h"
#include "sector.h"
#include "statusbar.h"
#include "hudstate.h"

#define NOTHING     0
#define CONTENTS    1
#define NEXT        2

#define DATA_SLOTS      32      // number of extra data slots in the savefile

bool TSaveGame::WriteGame(char *name)
{
    if (!name)
        name = "game01.sav";

    if (!Player) {
        log_warn("[savegame] WriteGame('%s'): no Player to save", name);
        return false;
    }

    if (saved != Player)
        saved = Player;

    pane = MultiCtrl.GetActivePane();
    gametime = PlayScreen.GameTime();
    version = MAP_VERSION;

    TOutputStream os(32768, 16384);

    // Use rev_fopen so the write lands in SavePath (the per-user
    // writable dir) rather than cwd. The matching ReadGame path
    // already uses rev_fopen and would fail to find a cwd-written
    // file on a typical install.
    FILE *fp = rev_fopen(name, "wb");
    if (!fp) {
        log_error("[savegame] WriteGame('%s'): rev_fopen failed (errno=%d)", name, errno);
        return false;
    }

    bool retval = true;

    fseek(fp, 0, 0);

    // Write out the Auto Map data first
    if (AutoMap.WriteAutoMapData(fp) == -1)
        retval = false;
    else
    {
        // Then some other random info...
        // Note there are DATA_SLOTS integers here for adding more extra info
        // to the save file without ruining any existing ones
        int32_t data[DATA_SLOTS];
        memset(data, 0, sizeof(int32_t) * DATA_SLOTS);
        data[0] = gametime;
        data[1] = pane;
        data[7] = version;

        // HudState slots [2..11] + presence flag slot [12]:
        //   Serialize the 8 SHudState int32_t fields so the player's
        //   sidebar/panel selection persists across save/load.
        //   data[12] = 0xABCD is the presence flag: if slot 12 != 0xABCD
        //   (old save without HudState), ReadGame falls back to defaults.
        {
            const SHudState& hs = GetHudState();
            data[12] = 0xABCD;   // HudState presence sentinel
            data[2]  = hs.topSlot;
            data[3]  = hs.bottomSlot;
            data[4]  = hs.sidebarState;
            data[5]  = hs.bottomBarOpen;
            data[6]  = hs.textBarVisible;
            data[8]  = hs.statsBarVisible;
            data[9]  = hs.inventoryPage;
            data[10] = hs.inventoryContainer;
            data[11] = hs.spellbookScroll;
        }

        if (fwrite(data, sizeof(int32_t), DATA_SLOTS, fp) < DATA_SLOTS)
            retval = false;
        else
        {
            // Finally Player himself (including all inventory and equipment)
            TObjectInstance::SaveObject(saved, os);
            if (fwrite(os.Buffer(), os.DataSize(), 1, fp) < 1)
                retval = false;
        }
    }

    fclose(fp);

    // Pre-save snapshot of the player's key retail-faithful state. Pairs
    // with the matching log in ReadGame so we can spot any field that
    // doesn't round-trip through TObjectInstance::Save/LoadObject. Logs
    // Pos() (which reads through transform_), plus sector liveness and
    // inventory item count so post-load mismatches (dangling inventory
    // slot, post-load sector-attach missing) jump out of revenant.log
    // without an interactive repro. (The legacy `pos` mirror is private
    // — Pos() is the canonical read.)
    {
        const S3DPoint p = saved->Pos();
        log_info("[savegame] WriteGame('%s'): %s gametime=%d ver=%d "
                 "player level=%d pos=(%d,%d,%d) flags=0x%x mapindex=%d "
                 "sector=%s inv=%d",
                 name, retval ? "ok" : "FAIL", gametime, version,
                 saved->GetLevel(), p.x, p.y, p.z,
                 saved->Flags(), saved->GetMapIndex(),
                 saved->GetSector() ? "live" : "null",
                 saved->RealNumInventoryItems());
    }
    return retval;
}

bool TSaveGame::WriteGame(int32_t gamenum)
{
    char name[12];

    if (gamenum < 0)
        gamenum = 0;

    sprintf(name, "game%02d.sav", gamenum);

    return WriteGame(name);
}

bool TSaveGame::ReadGame(char *name)
{
    loading = true;

    if (!name)
        name = "game01.sav";

  // Modern map reset: drop the entire TGameMap cache (which TMapPane
  // shares sector pointers with — calling the legacy
  // MapPane.ClearCurMap directly causes a use-after-free in
  // TGameMap::FindSector). MapManager.Shutdown walks each loaded map
  // through TGameMap::Unload → TSector::CloseSector exactly once and
  // fires Unloaded so the renderer drops its currentMap pointer too.
  // We re-Init and SetCurrentLevel after the player loads so the
  // engine has a live map again.
  //
  // The retail engine's MapPane.ClearCurMap-then-LoadCurMap pair is
  // a different shape from ours (per-slot CurMap copy; tracked
  // elsewhere as a follow-up). What's retail-faithful here is the
  // player object's serialization, not the map orchestration around
  // it — see [feedback-retail-first] in /memory.
    MapManager.Shutdown();
    MapManager.Init();

  // Reload initial game states
    ScriptManager.ReloadStates();

  // Clear the current player list
    PlayerManager.Clear();
    
  // Now attempt to load the game
    bool retval = true;

    FILE *fp = rev_fopen(name, "rb");
    if (fp == nullptr)
    {
        log_warn("[savegame] ReadGame('%s'): file not found", name);
        loading = false;
        return false;
    }
    fseek(fp, 0, SEEK_END);
    size_t bufsize = (size_t)ftell(fp);
    fseek(fp, 0, SEEK_SET);
    uint8_t *buf = nullptr;

    if (bufsize > 0)
    {
        // Keep track of how big the Auto Map data is
        size_t size;

        size = AutoMap.ReadAutoMapData(fp);
        if (size == -1)
            retval = false;
        else
        {
            // Read in the spare data
            bufsize -= DATA_SLOTS * sizeof(int32_t);
            int32_t data[DATA_SLOTS];

            if (fread(data, sizeof(int32_t), DATA_SLOTS, fp) < DATA_SLOTS)
                retval = false;
            else
            {
                // Transfer the data to the appropriate values
                gametime = data[0];
                pane = data[1];
                version = data[7];

                // Restore HudState from save slots [2..11].
                // Presence sentinel: data[12] == 0xABCD means this save has
                // valid HudState fields (written by a version that includes
                // this change). Old saves leave data[12] == 0 → use defaults.
                if (data[12] == 0xABCD)
                {
                    SHudState& hs = GetHudState();
                    hs.topSlot            = data[2];
                    hs.bottomSlot         = data[3];
                    hs.sidebarState       = data[4];
                    hs.bottomBarOpen      = data[5];
                    hs.textBarVisible     = data[6];
                    hs.statsBarVisible    = data[8];
                    hs.inventoryPage      = data[9];
                    hs.inventoryContainer = data[10];
                    hs.spellbookScroll    = data[11];
                }
                // else: old save without HudState — leave GetHudState() at
                // its zero-init defaults (topSlot=EQUIP, bottomSlot=INV,
                // sidebarState=OPEN) which is the sensible startup state.

                // Now adjust bufsize so we can read in the streamed object list
                bufsize -= size;

                buf = (uint8_t *)malloc(bufsize);

                if (fread(buf, bufsize, 1, fp) < 1)
                    retval = false;
            }
        }
    }
    else
    {
        buf = (uint8_t *)malloc(sizeof(int32_t));
        bufsize = 4;
        *((int32_t *)buf) = 0;
    }

    fclose(fp);

    if (version < 3)        // first version supporting recursive object writes
        FatalError("game01.sav is an old version, get rid of it please");

    if (retval)
    {
        TInputStream is(buf, bufsize);
        saved = TObjectInstance::LoadObject(is, version);
    }

    if (buf)
        free(buf);

    if (!saved)
    {
        loading = false;
        return false;
    }

  // Set visible pane
    MultiCtrl.ActivatePane(pane);

  // Adds the player into the player manager
    PlayerManager.AddPlayer((TPlayer*)saved);

  // Tell system this is our main player
    PlayerManager.SetMainPlayer((TPlayer*)saved);

  // Re-anchor the modern map registry to the player's level so
  // UpdateActiveWindow / FindSector have a live TGameMap. The renderer
  // rebind happens at the caller layer (TPlayScreen::Update after
  // ReadGame returns) since the renderer is a PlayScreen owned member.
    MapManager.SetCurrentLevel(saved->GetLevel());

  // Post-load sector attach. The loaded player object carries its world
  // pos.x / pos.y / level faithfully (TObjectInstance::Load round-trips
  // them), but TObjectInstance::Load does NOT attach the object to a
  // sector — it never has, because in retail sector membership was
  // re-established by the surrounding LoadGame orchestration (which
  // recreated curmap from the slot, then re-loaded sectors). Our modern
  // map system reloads sectors on SetCurrentLevel, but doesn't know to
  // wire the freshly-loaded OF_NONMAP player into the sector that owns
  // its world pos. Without this attach, oi->GetSector() stays nullptr,
  // and code paths that go through sector → walkmap → floor-Z (and the
  // dispatch chain that uses GetSector() for OF_NONMAP follow-the-player
  // logic) fall back to defaults — Locke renders below the walkmap
  // surface. Documented in docs/gameflow/T5_FORENSIC.md §4 (H3).
  //
  // Per the user's architectural rule, this is map-side orchestration
  // but uses our modern engine API (TGameMap::FindSector + the existing
  // TSector::AddObject), not the retail FUN_00460d60 chain. The player
  // object's serialization itself stayed retail-faithful — what we're
  // wiring up is the modern-engine response to a retail-faithful loaded
  // player.
    {
        const S3DPoint p = saved->Pos();
        const int32_t sx = p.x >> SECTORWSHIFT;
        const int32_t sy = p.y >> SECTORHSHIFT;
        TGameMap* gm = MapManager.CurrentMap();
        TSector* sec = gm ? gm->FindSector(sx, sy) : nullptr;
        if (sec)
        {
            // Player is OF_NONMAP — TSector::~TSector skips delete on
            // OF_NONMAP objects, so the player's lifetime stays owned by
            // PlayerManager. Safe to AddObject. AddObject also calls
            // oi->ForceSector(this) so GetSector() goes live.
            sec->AddObject(saved);
            log_info("[savegame] ReadGame: attached player to sector "
                     "%d_%d_%d at world (%d,%d,%d)",
                     saved->GetLevel(), sx, sy, p.x, p.y, p.z);
        }
        else
        {
            log_warn("[savegame] ReadGame: no sector found for player "
                     "at level=%d sector=(%d,%d) world=(%d,%d,%d); "
                     "Locke will likely render below the floor",
                     saved->GetLevel(), sx, sy, p.x, p.y, p.z);
        }
    }

  // Restore game time
    PlayScreen.SetGameTime(gametime);

  // Redraw whole window
    PlayScreen.Redraw();

    loading = false;
    // Post-load snapshot — pair with the pre-save log in WriteGame to
    // diagnose any drift in retail-faithful player serialization.
    // `sector=null` post-load = the sector attach step didn't run
    // (Locke-in-ground symptom, see docs/gameflow/T5_FORENSIC.md).
    {
        const S3DPoint p = saved->Pos();
        log_info("[savegame] ReadGame('%s'): %s gametime=%d ver=%d "
                 "player level=%d pos=(%d,%d,%d) flags=0x%x mapindex=%d "
                 "sector=%s inv=%d",
                 name, retval ? "ok" : "FAIL", gametime, version,
                 saved->GetLevel(), p.x, p.y, p.z,
                 saved->Flags(), saved->GetMapIndex(),
                 saved->GetSector() ? "live" : "null",
                 saved->RealNumInventoryItems());
    }
    return retval;
}

bool TSaveGame::ReadGame(int32_t gamenum)
{
    char name[12];
    
    if (gamenum < 0)
        gamenum = 0;

    sprintf(name, "game%02d.sav", gamenum);

    return ReadGame(name);
}
