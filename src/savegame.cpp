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
#include "script.h"
#include "sector.h"
#include "statusbar.h"

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

    FILE *fp = fopen(name, "rb");
    if (fp == nullptr)
    {
        loading = false;
        return false;
    }
    size_t bufsize = _filelength(fileno(fp));
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
    PlayerManager.AddPlayer((PTPlayer)saved);

  // Tell system this is our main player
    PlayerManager.SetMainPlayer((PTPlayer)saved);

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
