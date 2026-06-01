// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   SaveGame.cpp - savegame objects                     *
// *************************************************************************

#include "savegame.h"

#include "automap.h"
#include "dls.h"
#include "inventory.h"
#include "mappane.h"
#include "multictrl.h"
#include "player.h"
#include "playscreen.h"
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

    if (!Player)
        return false;

    if (saved != Player)
        saved = Player;

    pane = MultiCtrl.GetActivePane();
    gametime = PlayScreen.GameTime();
    version = MAP_VERSION;

    TOutputStream os(32768, 16384);

    FILE *fp;
    fp = fopen(name, "wb");
    if (!fp)
        return false;

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

  // Kill the currently loaded map (return to original map)
    MapPane.ClearCurMap();

  // Reload initial game states
    ScriptManager.ReloadStates();

  // Clear the current player list
    PlayerManager.Clear();
    
  // Now attempt to load the game
    bool retval = true;

    FILE *fp = rev_fopen(name, "rb");
    if (fp == nullptr)
    {
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

  // Restore game time
    PlayScreen.SetGameTime(gametime);

  // Redraw whole window
    PlayScreen.Redraw();

    loading = false;
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
