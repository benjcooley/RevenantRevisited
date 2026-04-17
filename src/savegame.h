// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 savegame.h - SaveGame header file                     *
// *************************************************************************

#pragma once

#include "revenant.h"

#include "object.h"
#include "graphics.h"

_CLASSDEF(TSaveGame)
class TSaveGame
{
  public:
    TSaveGame() { saved = nullptr; }

    bool WriteGame(char *name = nullptr);
    bool WriteGame(int32_t);
        // Write savegame to disk
    bool ReadGame(char *name = nullptr);
    bool ReadGame(int32_t);
        // Read savegame from disk
    bool IsLoading() const {return loading;}
        // Checks to see if the game is loading

  protected:
    TObjectInstance* saved;                 // Saved object (Locke)
    int32_t gametime;                       // Game time when saved
    int32_t version;                        // Version of savegame
    int32_t pane;                           // Which pane was up
    bool loading;
};
