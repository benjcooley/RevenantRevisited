// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                    automap.h  - Automapper File                       * 
// *************************************************************************

#pragma once

#include "revenant.h"
#include "screen.h"
#include "automapdata.h"

// Define the area around the player that gets revealed
#define AUTOMAP_VIEW_WIDTH  8
#define AUTOMAP_VIEW_HEIGHT 8

// Define the maximum number of pixels the map can move while trying to
// reach the Target scroll position
#define MAX_AUTOMAP_SCROLL_SPEED 16

//============================================================================
// Class : TAutoMap.
//----------------------------------------------------------------------------
//  Desc : The automapper pane is a small version of the automatic mapping
//         system which keeps track of where the player has been and displays
//         a portion of this map on the mini-pane.
//============================================================================

_CLASSDEF(TAutoMap)
class TAutoMap : public TPane
{
    // Member Functions

    public:

    TAutoMap() : TPane(MULTIPANEX, MULTIPANEY, MULTIPANEWIDTH, MULTIPANEHEIGHT) {}
    ~TAutoMap() {}

    virtual bool Initialize();
    virtual void Close();

    virtual void DrawBackground();

    bool LoadMapGraphics(int32_t MapNum);
    void FreeMapGraphics();

    void RecordTravels();

    int32_t GetMapNumber(char *MapName);

    bool GetMaskColorActive(int32_t MapNum, int32_t Color);
    bool GetMaskColorActive(char *MapName, int32_t Color);

    void SetMaskColorActive(int32_t MapNum, int32_t Color, uint8_t Active);
    void SetMaskColorActive(char *MapName, int32_t Color, uint8_t Active);

    virtual void MouseClick(int32_t button, int32_t x, int32_t y);
    virtual void MouseMove(int32_t button, int32_t x, int32_t y);

    size_t WriteAutoMapData(FILE *fp);
    size_t ReadAutoMapData(FILE *fp);


    protected:

    void DrawPlayerMarker(int32_t PlayerX, int32_t PlayerY);
    void RestorePlayerMarker();

    void DrawAutoMap();

    inline uint16_t MergePixel(uint16_t *Background, uint16_t *Overlay, int32_t Mul, int32_t Div);
    inline uint16_t MergePixelToBlack(uint16_t *Overlay, int32_t Mul, int32_t Div);

    void CheckMaps();

    void GetPlayerOnAutoMap(int32_t &PlayerX, int32_t &PlayerY);


    // Member Variables

    protected:

    PTAutoMapList MapList;             // Pointer to an array holding
                                       // information about all the automaps

    SRect MapArea;                     // Where on the full map the current Automap
                                       // is showing (in World Coords.)

    int32_t ScrollX, ScrollY;          // Current scroll position of the map
    int32_t OldScrollX, OldScrollY;    // Scroll position of previous frame

    bool ScrollToTarget;               // Flag that we're supposed to scroll
                                       // the map to the TargetScroll
    int32_t TargetScrollX, TargetScrollY;  // Target position to Scroll the map to
    int32_t MapScrollDx, MapScrollDy;      // How many pixels the map moves when
                                       // trying to reach the TargetScroll

    int32_t OldPlayerX, OldPlayerY;    // Previous player position

    uint16_t SavePlayerPixel;          // Saves the pixel underneath where the player is
    int32_t MarkerX, MarkerY;          // Where the SavePlayerPixel is

    bool DraggingMap;                  // Flag for if the user is moving the
                                       // map with the mouse

    int32_t DragMouseX, DragMouseY;    // Used to keep track of how far the mouse
                                       // has moved since starting to drag the map

    int32_t DragScrollX, DragScrollY;  // Used to keep track of the original scroll
                                       // position when the map begins to scroll

    int32_t CurrentMap;                // Which map the automap is currently showing

    PTAutoMapGraphics MapGrfx;         // Pointer to the class holding all
                                       // the graphics needed for an automap

    PTBitmap DisplayMap;               // Version of the map shown to the user

    bool HasMap;                       // Flag for if the player has a map for
                                       // this map in his inventory or not

    uint8_t *ActiveBuf;                // Pointer to a buffer that holds the
                                       // active information for all the maps

    uint8_t *Active;                   // Pointer to the array of colors used
                                       // to reveal the current automap
};

