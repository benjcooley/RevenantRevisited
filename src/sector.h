// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      sector.h - TSector object                        *
// *************************************************************************

#pragma once

#include "revenant.h"

#include "object.h"

extern char sectorfilename[80];

// ***********************************************************
// * TSector - An independenly loadable area of the game map *
// ***********************************************************

// The TSector object represents a small portion of the game map which contains a list
// of objects on that portion of the map.  The game map is divided into x,y sectors and
// levels, and the name of the sector resource file indicates which level and/or
// x,y sector of the game the sector comes from.

_STRUCTDEF(SLabel)
struct SLabel
{
    bool used;
    bool isgroup;
    char label[NAMELEN];
    int32_t index;
};

#define MAXWALKMAPS         32
#define MAXLABELS           64
#define MAXSECTOROBJECTS    4096    // if you increase this number you MUST change MAKEINDEX in mappane.h to handle more bits!
#define MAXPRELOADRECTS     32

#define WALKMAPSIZE         ((SECTORWIDTH >> WALKMAPSHIFT) * (SECTORHEIGHT >> WALKMAPSHIFT))

typedef TPointerArray<TObjectInstance, MAXSECTOROBJECTS> TObjectArray;
typedef TVirtualArray<int32_t, 0, MAXSECTOROBJECTS> TObjSetArray;
typedef TObjectArray *PTObjectArray;
typedef TPointerIterator<TObjectInstance> TObjectIterator;

typedef TPointerArray<TSector, 64, 64> TSectorArray;

// ***************
// * Object Sets *
// ***************
//
// All objects within a sector are stored in the 'object' array.  This is usually fine,
// for most applications (like drawing tiles, etc.), but can bog the system down when you
// are constantly iterating through massive object arrays to find players, moving objects
// for intersection, etc.  
//
// To make iteration through smaller sets of objects easier, the sector system has a set
// of funcitons which deal with object sets.  Object sets are arrays of indexes into the
// main object array for objects which match the criteria of a given set.  To save time, 
// a function can iterate through the objects in the object set, instead of all the objects
// in a map.

// Currently there is only one object set.  You can add new object sets by increasing this
// number, defining the name below, and expanding the InObjectSet() function to recognize
// whether an object is a member of a set.  That's all there is to it.

#define NUMOBJSETS     7

// Object find/search sets (using a set for a search/find command greatly reduces)
// the time to find the objects)
#define OBJSET_ALL       0  // All objects
#define OBJSET_MOVING    1  // Moving objects
#define OBJSET_CHARACTER 2  // Player and character objects
#define OBJSET_LIGHTS    3  // Light objects
#define OBJSET_PULSE     4  // Pulsed objects
#define OBJSET_ANIMATE   5  // Animated objects
#define OBJSET_NOTIFY    6  // Notify objects

// If the following flags change, the object will be put into different OBJSET arrays...
#define OBJSETOBJFLAGS (OF_MOVING | OF_LIGHT | OF_PULSE | OF_ANIMATE)

_CLASSDEF(TSector)
class TSector final
{
  public:
    TSector(int32_t newlevel, int32_t newsectorx, int32_t newsectory);
    ~TSector();

    _NODEFAULTCONS(TSector)

    void Clear();

  // Creates and loads a sector (uses preloaded sector if it can find one)

    // Loads the sector.. keeps file open so sector is locked
    static TSector* LoadSector(int32_t newlevel, int32_t newsectorx, int32_t newsectory, bool preload = true);
    // Save and delete the sector (doesn't really delete it if sector is preload)
    static void CloseSector(TSector* sector);

    // Load and save the sector (straight load.. don't use preloaded sector list)

    bool Load(bool lock = false);
    void Save();

    // Sector

    int32_t SectorLevel() { return level; }
    int32_t SectorX() { return sectorx; }
    int32_t SectorY() { return sectory; }
    void GetMaxScreenRect(RSRect r);
    void GetMaxMapRect(RSRect r);

  // Object manipulation functions

    TObjectInstance* GetInstance(int32_t item) const
        { return objects[item]; }
    int32_t AddObject(TObjectInstance* oi, int32_t item = -1);
    int32_t SetObject(TObjectInstance* oi, int32_t item);
    TObjectInstance* RemoveObject(int32_t item);
    int32_t NumItems() const { return objects.NumItems(); }
    int32_t GetObjIndex(const TObjectInstance* oi) const;
    PTObjectArray ObjectArray()
      { return &objects; }

    // Monotonic counter incremented every time the sector's object
    // contents change (Load/Add/Remove/Set). Consumers that cache
    // anything derived from `objects` (e.g. the renderer's draw list)
    // remember the last seen version and rebuild when it advances.
    // Reset to 1 on Load so a fresh load reads as "changed since 0".
    [[nodiscard]] int32_t ContentVer() const { return contentver; }

  // Object set functions
  // --------------------
  // Object sets allow super fast iteration through subsets of objects.  To add a new 
  // object set, add to NUMOBJSETS macro, add a test in InObjSet, and create a new OBJSET_xxx
  // macro.
  //
  // All MapPane object functions use object sets, and most (including the map iterator), 
  // allow you to pass the object set parameter to the various search functions.
    bool InObjSet(TObjectInstance* oi, int32_t objset) const
        {
            switch (objset) 
            { 
                case OBJSET_MOVING:     return oi->IsMoving();
                case OBJSET_CHARACTER:  return oi->IsCharacter();
                case OBJSET_LIGHTS:     return oi->IsLight();
                case OBJSET_PULSE:      return oi->IsPulsed();
                case OBJSET_ANIMATE:    return oi->IsAnimated();
                case OBJSET_NOTIFY:     return oi->IsNotify();
            }
            return true; 
        }
    void ObjectFlagsChanged(TObjectInstance* oi, uint32_t oldflags, uint32_t newflags);
      // Removes object from old sets and places in new sets if objset flags change
      // Checks flags in OBJSETOBJFLAGS macro define to check change
    int32_t NumObjSetItems(int32_t objset) const { if (!objset) return objects.NumItems(); else return objsets[objset-1].NumItems(); }
      // Returns number of moving objects (objects with OF_MOVING flag set)
    int32_t GetObjSetIndex(int32_t objset, int32_t item) const
        { if (!objset) return item; else return (objsets[objset-1])[item]; }
    TObjectInstance* GetObjSetInstance(int32_t objset, int32_t item) const
        { if (!objset) return objects[item]; else return objects[(objsets[objset-1])[item]]; }

  // Walkmap stuff
    int32_t ReturnWalkmap(int32_t x, int32_t y) const
      { return walkmap ? walkmap[(y << (SECTORWSHIFT - WALKMAPSHIFT)) + x] : 0; }
      // Returns walkmap value given sector position
    void SetWalkmap(int32_t x, int32_t y, int32_t height)
      { if (walkmap) walkmap[(y << (SECTORWSHIFT - WALKMAPSHIFT)) + x] = (uint16_t)height; }
      // Sets walkmap at x, y to have height of height
    void WalkmapHandler(int32_t mode, uint8_t *walk, int32_t zpos, int32_t x, int32_t y, int32_t width, int32_t length, int32_t stride, bool override = false);
      // Handler for various walkmap manipulations (clips world wm pos to sector)
    void ClearWalkmap();
      // Sets walkmap to 0

  // Preloaded sector system
    static bool LoadPreloadSectors(int32_t level, int32_t numrects, SRect *rects);
      // Preload sectors in this area
    static void ClearPreloadSectors(int32_t level = -1, int32_t numrects = 0, SRect *rects = nullptr);
      // Clear preload sectors (if level, numrects, and rects are supplied, keeps any
      // sectors within the indicated level and rectangle set from being deleted)
    static TSector* FindPreloadSector(int32_t level, int32_t sectorx, int32_t sectory);
      // find a particular preloaded sector
    static int32_t PreloadSectorLevel() { return preloadlevel; }
      // Get preload sector level
    static int32_t NumPreloadRects(const SRect &r) { return numpreloadrects; }
      // Get preload sector rectangle
    static const SRect& GetPreloadRect(int32_t rectnum) { return preloadrects[rectnum]; }
      // Get preload sector rectangle
    static bool InPreloadArea(const S3DPoint& p, int32_t level);
      // Are we in the preload area?

  private:
    uint16_t *walkmap;
    int32_t  level, sectorx, sectory;
    bool preloaded;
    char filename[FILENAMELEN];
    int32_t usecount;

    // v14+ adds a 4-byte hash between sector version and numobjects.
    // Retail regenerates it at save time over sector coords, numobjects,
    // and the serialized body of state-bearing objects (obj classes
    // 0xb/0xc — TCharacter/TPlayer). See TSector::Save at FUN_00498c90
    // and hash helper FUN_00499e90 in data/Revenant.exe. We round-trip
    // the read value for now; regeneration is future work.
    int32_t statehash = 0;

    // Bumped on every Add/Remove/Set, set to 1 in Load. Renderer
    // (and any other cache derived from `objects`) compares against
    // its own cached version per frame to detect changes.
    int32_t contentver = 0;

    TObjectArray objects;
    TObjSetArray objsets[NUMOBJSETS];

    static TSectorArray preloads;
    static int32_t preloadlevel;
    static int32_t numpreloadrects;
    static SRect preloadrects[MAXPRELOADRECTS];
};


