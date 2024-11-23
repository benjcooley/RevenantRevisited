// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     area.h - Game Area Manager                        *
// *************************************************************************

#pragma once

#include "revenant.h"

#include "graphics.h"
#include "parse.h"

typedef TSizableArray<SRect, 4, 4> TRectArray;

#define MAXPLAYLISTSIZE 32

// If these flags are set, the area will perform the following functions
#define AREA_PLAYERIN           (1<<1)  // Player is currently in this area (Pulse() and Animate())
#define AREA_SETAMBIENT         (1<<2)  // This area should set the ambient light value
#define AREA_DONIGHT            (1<<3)  // This area should do day/night cycles
#define AREA_LOADSCRIPTS        (1<<4)  // This area should load a script file
#define AREA_PLAYCDMUSIC        (1<<5)  // This area should play cd music tracks
#define AREA_PLAYAMBIENT        (1<<6)  // This area should play ambient sound effects

_CLASSDEF(TArea)
class TArea
{
  private:
    char name[MAXNAMELEN];              // Name of area
    int32_t level;                      // Level of area
    int32_t flags;                      // Area flags
    TRectArray rects;                   // Array of rects which define the area
    char scriptfile[FILENAMELEN];       // Name of script file to load when in area 
    int32_t amblight, nightamblight;    // Ambient lighting value
    SColor ambcolor, nightambcolor;     // Ambient color
    int32_t lastdaylight;               // Last daylight value

  // Ambient sound stuff
    void InitAmbientSounds();
      // Initializes ambient sounds for area
    void CloseAmbientSounds();
      // Deinitializes ambient sounds for area
    void PlayAmbientSounds();
      // Plays ambient sound effects when music not playing

  // CD audio music playing
    int32_t cdplaynum;                      // Current track to play
    int32_t cdplaylistsize;                 // Size of current play list
    int32_t cdplaylist[MAXPLAYLISTSIZE];    // Play list
    bool cdplayisrandom;                // Play random tracks from list?
    int32_t cdplaypause;                    // Seconds to pause between tracks
    int32_t cdplaystart;
    int32_t cdplaywait;
    int32_t cdplaylength;

  // CD music functions
    void InitCDMusic();
      // Initializes data needed to play CD tracks
    void CloseCDMusic();
      // Frees data needed to play CD tracks
    void PlayCDMusic();
      // Plays CD music tracks

  public:
    TArea();
      // Initializes and loads an area
    ~TArea();
      // Gets rid of area

    bool Load(char *aname, TToken &t);
      // Loads the area from the "AREA.DEF" file

    int32_t GetFlags() { return flags; }
      // Returns area flags
        
    bool In(S3DPoint &pos, int32_t lev);
      // Returns true if player's position is in this area

    void GetCurrentAmbient(int32_t &ambient, SColor &color);
      // Returns the current ambient values based on the time of day

    void Pulse();
      // Called by the game screen Pulse() function to update area stuff
    void Animate(bool draw);
      // Called by the game screen Animate() function to update area stuff

    void Enter();
      // Called when the player enters the area
    void Exit();
      // Called when the player exits the area
};
typedef TPointerArray<TArea, 16, 16> TAreaArray;

_CLASSDEF(TAreaManager)
class TAreaManager
{
  private:
    bool initialized;
    TAreaArray areas;

  public:
    bool Initialize();
      // Initialize the area manager
    void Close();
      // Closes the area manager
    int32_t NumAreas() { return areas.NumItems(); }
      // Returns number of areas
    PTArea GetArea(int32_t index) { return areas[index]; }
      // Returns the control entry for the given index
    PTArea InArea(S3DPoint &pos, int32_t lev);
      // Returns the area that the point is in, or nullptr if not in any area
    bool Load();
      // Loads all areas from the "AREA.DEF" file
    void Pulse();
      // Called by the game screen Pulse() function to update area stuff
    void Animate(bool draw);
      // Called by the game screen Animate() function to update area stuff
};

