// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     Object.h - TObject module                         *
// *************************************************************************

#pragma once

#include "revenant.h"
#include "objectcomponent.h"
#include "imagery.h"
#include "saferef.h"
#include "stream.h"
#include "lightdef.h"
#include "transform.h"

#include <atomic>
#include <memory>
#include <unordered_map>
#include <vector>

/*
#define MAKEINDEX(level, sx, sy, item)  ((level<<24) | (sx<<18) | (sy<<12) | (item & 0xFFF))
#define GETLEVEL(index)                 (((unsigned)index) >> 24)
#define GETSECTORX(index)               ((index >> 18) & 0x3F)
#define GETSECTORY(index)               ((index >> 12) & 0x3F)
#define GETITEM(index)                  (index & 0xFFF)
*/

// hack alert - z vals are going nuts, so here's a quick fix (map pos to 3D pos)
#define FIX_Z_VALUE(zmap)                  ((float)zmap / ((float)1.46/* - ((float)0.01 * ((float)zmap / (float)300.0))*/))

// This fix goes from a 3D pos to a map pos
#define REV_FIX_Z_VALUE(z3d)                ((float)z3d * (float)1.46)

// Coordinate converters (between world/map and screen/pixel)
void WorldToScreen(const S3DPoint& pos, int32_t &x, int32_t &y);
void WorldToScreen(const S3DPoint& pos, S3DPoint& spos);
void WorldToScreenZ(const S3DPoint& pos, int32_t &x);
void ScreenToWorld(int32_t x, int32_t y, S3DPoint& pos, int32_t zheight = 0);
void ConvertToVector(int32_t angle, int32_t speed, S3DPoint& vect, int32_t zangle = 0);
int32_t ConvertToFacing(const S3DPoint& pos, const S3DPoint& target);
int32_t ConvertToFacing(const S3DPoint& target);
int32_t ConvertZToFacing(const S3DPoint& pos, const S3DPoint& target);
int32_t ConvertZToFacing(const S3DPoint& target);
int32_t Distance(const S3DPoint& pos);
int32_t Distance(const S3DPoint& pos, const S3DPoint& target);
int32_t AngleDiff(int32_t angle1, int32_t angle2);

// Create new unique id
uint32_t GenerateUniqueID();

_CLASSDEF(TObjectAnimator)

// Forward-declared free function used by TObjectInstance::SafeRefLookup
// (and by TSafeRef<TObjectInstance>) to avoid pulling the mappane /
// sector include chain into object.h. Defined in mappane.cpp.
TObjectInstance* LookupMapIndex(int32_t mapindex);

// *************************************************************
// * TObjectBuilder - Creates objects for a given object class *
// *************************************************************

#define MAXOBJECTTYPES 2048

_CLASSDEF(SObjectDef)
// TObjectComponent + TObjectInstance forward decls and the update-method
// typedef now live in objectcomponent.h; included above.

_CLASSDEF(TObjectBuilder)
class TObjectBuilder
{
  public:
    TObjectBuilder(const char *name);
      // Sets name and adds builder to builder array
    virtual TObjectInstance* Build(TObjectImagery* img) = 0;
      // Creates a new object from the given 'def' structure
    virtual TObjectInstance* Build(SObjectDef* def, TObjectImagery* img) = 0;
      // Creates a new object from the given 'def' structure

    static TObjectBuilder* GetBuilder(int32_t objtype);
      // Gets a pointer to a builder in the builder array
    static TObjectBuilder* GetBuilder(const char *name);
      // Gets a pointer to a builder in the builder array

    char *GetTypeName() { return objtypename; }

  private:
    static int32_t numobjtypes;                         // Number of object types
    static TObjectBuilder* builders[MAXOBJECTTYPES];    // Object builder for each type

    char *objtypename;
};

#define DEFINE_BUILDER(name, obj)                                               \
class obj##Builder : public TObjectBuilder                                      \
{                                                                               \
  public:                                                                       \
    obj##Builder() : TObjectBuilder(name) {}                                    \
    virtual TObjectInstance* Build(TObjectImagery* img)                         \
        { return new obj(img); }                                                \
    virtual TObjectInstance* Build(SObjectDef* def, TObjectImagery* img)        \
        { return new obj(def, img); }                                           \
};

#define REGISTER_BUILDER(obj) obj##Builder obj##BuilderInstance;

// **********************
// * TInventoryIterator *
// **********************

_CLASSDEF(TInventoryIterator)
class TInventoryIterator
{
  public:
    TInventoryIterator(TObjectInstance* own)
        { owner = own; invindex = 0; item = nullptr; NextItem(); }

    TObjectInstance* Item() const { return item; }
      // Returns current item.
    int32_t InvIndex() const { return invindex - 1; }
      // Returns current inventory index
    TObjectInstance* NextItem();
      // Advance to the next item, and return it (nullptr if end of list)
    bool operator ++ (int32_t) { return (NextItem() != nullptr); }
      // Allows moving forwards through array in sequential order
    operator TObjectInstance* () const { return item; }
      // Typecast operator for object
    TObjectInstance* operator -> () const { return item; }
      // Allows iterator to be used as pointer
    RTObjectInstance operator * () const { return *item; }
      // Dereferencing operator

  protected:
    TObjectInstance* owner;     // inventory parent
    int32_t invindex;           // index into inventory
    TObjectInstance* item;      // current object
};

_CLASSDEF(TConstInventoryIterator)
class TConstInventoryIterator
{
  public:
    TConstInventoryIterator(const TObjectInstance* own)
        { owner = own; invindex = 0; item = nullptr; NextItem(); }

    TObjectInstance* Item() const { return item; }
      // Returns current item.
    int32_t InvIndex() const { return invindex - 1; }
      // Returns current inventory index
    const TObjectInstance* NextItem() const;
      // Advance to the next item, and return it (nullptr if end of list)
    bool operator ++ (int32_t) const { return (NextItem() != nullptr); }
      // Allows moving forwards through array in sequential order
    operator TObjectInstance* () const { return item; }
      // Typecast operator for object
    TObjectInstance* operator -> () const { return item; }
      // Allows iterator to be used as pointer
    RTObjectInstance operator * () const { return *item; }
      // Dereferencing operator

  protected:
    const TObjectInstance* owner;   // inventory parent
    mutable int32_t invindex;       // index into inventory
    mutable TObjectInstance* item;  // current object
};

// ************************************************
// * TObjectClass - Represents a class of objects *
// ************************************************

// The TObjectClass contains the functions to create and manage a paticular class
// of object.  Each class object represents a class of object in the system like
// inventory items, wall objects, characters, the player character, spell effects
// groups, etc.

// Class IDs must stay byte-for-byte compatible with the retail Revenant.exe
// jump table at 0x00483850..0x004839a4 (see recon/docs/OBJCLASS_IDS.md).
// Retail's FindClassByID switch dispatches IDs 0..26 with "???" placeholders
// at 19/20/24 and three retail-only classes (INVCONTAINER=17, POTION=18,
// MAPSCROLL=26) that were added after the 1998 source snapshot.
enum
{
    OBJCLASS_ITEM,          // 0
    OBJCLASS_WEAPON,        // 1
    OBJCLASS_ARMOR,         // 2
    OBJCLASS_TALISMAN,      // 3
    OBJCLASS_FOOD,          // 4
    OBJCLASS_CONTAINER,     // 5
    OBJCLASS_LIGHTSOURCE,   // 6
    OBJCLASS_TOOL,          // 7
    OBJCLASS_MONEY,         // 8
    OBJCLASS_TILE,          // 9
    OBJCLASS_EXIT,          // 10
    OBJCLASS_PLAYER,        // 11
    OBJCLASS_CHARACTER,     // 12
    OBJCLASS_TRAP,          // 13
    OBJCLASS_SHADOW,        // 14
    OBJCLASS_HELPER,        // 15
    OBJCLASS_KEY,           // 16
    OBJCLASS_INVCONTAINER,  // 17  (retail add: inventory containers / bags)
    OBJCLASS_POTION,        // 18  (retail add: potions)
    OBJCLASS_UNUSED3,       // 19  ("???" placeholder in retail)
    OBJCLASS_UNUSED4,       // 20  ("???" placeholder in retail)
    OBJCLASS_AMMO,          // 21
    OBJCLASS_SCROLL,        // 22
    OBJCLASS_RANGEDWEAPON,  // 23
    OBJCLASS_UNUSED5,       // 24  ("???" placeholder in retail)
    OBJCLASS_EFFECT,        // 25
    OBJCLASS_MAPSCROLL,     // 26  (retail add: player-persistent scroll artifacts)
};

#define MAXSTATSTRINGLEN    64
#define MAXNAMELEN          64

_STRUCTDEF(SStatisticDef)
struct SStatisticDef
{
    char name[MAXNAMELEN];
    int32_t def, min, max;
    uint32_t uniqueid;
};

class TStatisticDefList
{
  private:
    TSizableArray<SStatisticDef, 0, 4> statdefs;

  public:           
    int32_t NumStats() const { return statdefs.NumItems(); }
        // Number of statistics for this class
    int32_t AddStat(SStatisticDef &newstatdef, int32_t id = -1);
        // Add a new statistic
    void DeleteStat(int32_t statid) { if ((uint32_t)statid < (uint32_t)statdefs.NumItems()) statdefs.Collapse(statid); }
        // Deletes a statistic
    PSStatisticDef GetStatisticDef(int32_t statid) const
        { if ((uint32_t)statid < (uint32_t)statdefs.NumItems()) return &(statdefs[statid]); else return nullptr; }
        // Get StatisticDef structure
    bool ParseStat(SStatisticDef &stat, TToken &t);
        // Adds a new stat by parsing stream (reload causes stat def to be parsed but ignored)
    const char *GetStatDefString(int32_t statid, char *buf) const;
        // Returns a parsable def string
    const char *StatName(int32_t statid) const { if ((uint32_t)statid < (uint32_t)statdefs.NumItems()) return statdefs[statid].name; else return ""; }
        // Returns statistic name
    int32_t StatDef(int32_t statid) const { if ((uint32_t)statid < (uint32_t)statdefs.NumItems()) return statdefs[statid].def; else return 0; }
        // Returns statistic default value
    int32_t StatMin(int32_t statid) const { if ((uint32_t)statid < (uint32_t)statdefs.NumItems()) return statdefs[statid].min; else return 0; }
        // Returns statistic minimum value
    int32_t StatMax(int32_t statid) const { if ((uint32_t)statid < (uint32_t)statdefs.NumItems()) return statdefs[statid].max; else return 0; }
        // Returns statistic maximum value
    uint32_t StatUniqueId(int32_t statid) const { if ((uint32_t)statid < (uint32_t)statdefs.NumItems()) return statdefs[statid].uniqueid; else return 0; }
        // Returns statistic maximum value
    int32_t FindStat(const char *statname) const;
        // Locates stat by name and returns its statid
    void Clear() { statdefs.Clear(); }
};

_STRUCTDEF(SStatEntry)
struct SStatEntry 
{
    SStatEntry(TObjectClass* cl, const char *statname, const char *uniqueid, int32_t statid, 
        int32_t def, int32_t min, int32_t max, bool objstat);
    int32_t id;
};

// Define a stat entry (put these immediatly after the class registration in you CPP file)
#define DEFSTAT(c, n, u, i, df, mn, mx)\
  SStatEntry T##c::se_##n(&c##Class, #n, #u, i, df, mn, mx, false);

#define DEFOBJSTAT(c, n, u, i, df, mn, mx)\
  SStatEntry T##c::se_##n(&c##Class, #n, #u, i, df, mn, mx, true);
 
// Pre Define a Stat (put this in the base class (usually TObjectInstance)
#define PSTATFUNC(n)\
    virtual int32_t n(){return 0;}\
    virtual void Set##n(int32_t v) {}

#define PSTATFUNCVAL(n, r)\
    virtual int32_t n(){return r;}\
    virtual void Set##n(int32_t v) {}

#define POBJSTATFUNC(n)\
    virtual int32_t n(){return 0;}\
    virtual void Set##n(int32_t v) {}

#define POBJSTATFUNCVAL(n, r)\
    virtual int32_t n(){return r;}\
    virtual void Set##n(int32_t v) {}

// Define an actual stat (put this in the actual object i.e. TCharacter or TPlayer)
#define STATFUNC(n)\
  private:\
    static SStatEntry se_##n;\
  public:\
    virtual int32_t n() { return GetStat(se_##n.id); }\
    virtual void Set##n(int32_t v) { SetStat(se_##n.id, v); }

#define OBJSTATFUNC(n)\
  private:\
    static SStatEntry se_##n;\
  public:\
    virtual int32_t n() { return GetObjStat(se_##n.id); }\
    virtual void Set##n(int32_t v) { SetObjStat(se_##n.id, v); }

// Define an actual stat (put this in the actual object i.e. TCharacter or TPlayer)
#define STAT(n)\
  private:\
    static SStatEntry se_##n;\
  public:
  
#define OBJSTAT(n)\
  private:\
    static SStatEntry se_##n;\
  public:

typedef TSizableArray<int32_t, 0, 1> TStatisticList;

#define MAXOBJECTCLASSES    64
#define MAXRESNAMELEN       64

// THIS STRUCTURE IS MULTIPLIED BY THE NUMBER OF OBJECT USED IN THE SYSTEM (WHICH IS IN THE
// THOUSANDS.  THE LARGER THIS STRUCTURE IS, THE MORE MEMORY THE SYSTEM USES.  KEEP THIS
// STRUCTURE AS *****SMALL***** AS POSSIBLE!!  BEN

_STRUCTDEF(SObjectInfo)
struct SObjectInfo
{   // HEY!  KEEP ME SMALL!
    SObjectInfo()
      { name = nullptr; objbuilder = nullptr; imageryid = 0; imagery = nullptr; uniqueid = 0; stats.Clear(); objstats.Clear(); }
    SObjectInfo(char *n, TObjectBuilder* b, int32_t id, uint32_t uid)
      { name = _strdup(n); objbuilder = b; imageryid = id; imagery = nullptr; uniqueid = uid; stats.Clear(); objstats.Clear(); }
    ~SObjectInfo() { delete [] name; stats.Clear(); objstats.Clear(); }

    char *name;                             // Name of object (allocated with _strdup()
    TObjectBuilder* objbuilder;             // Object builder for this object
    int32_t  imageryid;                     // Imagery object for this object
    TObjectImagery* imagery;                // Imagery object for this object
    TStatisticList stats, objstats;         // Statistics (class and object)
    uint32_t uniqueid;                      // Unique ID for this object
};

_CLASSDEF(TObjectClass)
class TObjectClass
{
  public:
    TObjectClass(char *classname, int32_t classid, uint16_t flags, TObjectClass* base = nullptr);
        // Creates object class and adds it to class array
    ~TObjectClass();
        // Destroys the object class and all object types and stats inside it
    void Clear();
        // Called by constructor and destructor to clear all internal arrays/buffers, etc.

  // Load and save class data
    static bool LoadClasses(bool lock = false, bool reload = false);
      // Loads classes (keeps file locked for updating if lock true)
    static bool SaveClasses(bool lock = false);
      // Saves classes (leaves file locked if lock is true)
    static void FreeClasses();
      // Deallocate class arrays

  // Class functions
    static TObjectClass* GetClass(int32_t objclass)
        { return ((uint32_t)objclass >= (uint32_t)numclasses) ? nullptr : classes[objclass]; }
      // Returns the given class from the class array
    static int32_t FindClass(char *name);
      // Returns the given class from the class array
    static int32_t NumClasses() { return numclasses; }
      // Number of classes
    const char *ClassName() const { return name; }
        // Name of class
    int32_t ClassId() const { return id; }
        // Id number for class
    virtual uint32_t ObjectFlags() const { return objflags; }
        // Object flag for class
    int32_t AddType(char *name, char *imgfilename, uint32_t uniqueid = 0);
        // Add a new object type (if uniqueid=0, a new unique id will be generated)
    bool RemoveType(int32_t objtype);
        // Removes an object type

  // Object functions
    TObjectInstance* NewObject(SObjectDef* objectdef);
        // Create a new object based on the given object definition

  // Type functions
    int32_t NumTypes() const { return objinfo.NumItems(); }
        // Returns number of object types in the class
    SObjectInfo* GetObjType(int32_t index) const { if (!objinfo.Used(index)) return nullptr; else return &(objinfo[index]); }
        // Return objinfo pointer for given index
    int32_t FindObjType(const char *objtypename, bool partial = false) const;
        // Return type of object given name
    int32_t FindObjType(uint32_t uniqueid) const;
        // Return type of object given its unique ID

  // Class statistic functions
    int32_t AddStat(SStatisticDef &stat, int32_t id = -1);
        // Add a new statistic
    void DeleteStat(int32_t statid);
        // Deletes a statistic
    PSStatisticDef GetStatisticDef(int32_t statid) const { return statdefs.GetStatisticDef(statid); }
        // Get StatisticDef structure
    bool ParseNewStat(TToken &t, bool reload = false);
        // Adds a new stat by parsing stream (reload causes stat def to be parsed but ignored)
    int32_t NumStats() const { return statdefs.NumStats(); }
        // Number of statistics for this class
    const char *StatName(int32_t statid) const { return statdefs.StatName(statid); }
        // Returns statistic name
    int32_t StatDef(int32_t statid) const { return statdefs.StatDef(statid); }
        // Returns statistic default value
    int32_t StatMin(int32_t statid) const { return statdefs.StatMin(statid); }
        // Returns statistic minimum value
    int32_t StatMax(int32_t statid) const { return statdefs.StatMax(statid); }
        // Returns statistic maximum value
    uint32_t StatUniqueId(int32_t statid) const { return statdefs.StatUniqueId(statid); }
        // Returns statistic maximum value
    int32_t FindStat(const char *statname) const { return statdefs.FindStat(statname); }
        // Locates stat by name and returns its statid
    const char *GetStatDefString(int32_t statid, char *buf) const { return statdefs.GetStatDefString(statid, buf); }
        // Gets a parsable string that defines the stat

        // Stuff for map generator

    int32_t FindStatVal(int32_t statid, int32_t searchvalue) const;
        // Find first object in class with searchvalue for given stat (for map generator)
    int32_t FindRandStatVal(int32_t statid, int32_t searchvalue, int32_t *heightflux = nullptr) const;
        // Find random object in class with searchvalue for given stat (for map generator

        // Individual stat access stuff

    int32_t GetStat(int32_t objtype, int32_t statid) const { return objinfo[objtype].stats[statid]; }
        // Returns a statistic for an object
    int32_t GetStat(int32_t objtype, char *statname) const
        { int32_t statid = statdefs.FindStat(statname); 
          if (statid >= 0) return objinfo[objtype].stats[statid]; else return 0; }
        // Returns a statistic for an object
    void SetStat(int32_t objtype, int32_t statid, int32_t newvalue) { objinfo[objtype].stats[statid] = newvalue; }
        // Sets a statistic for an object (Note: converted to a string if stat is string stat)
    void SetStat(int32_t objtype, char *statname, int32_t newvalue)
        { int32_t statid = statdefs.FindStat(statname); 
          if (statid >= 0) objinfo[objtype].stats[statid] = newvalue; }
        // Sets a statistic for an object (Note: converted to a string if stat is a string stat)
    void ResetStat(int32_t objtype, int32_t statid)
        { objinfo[objtype].stats[statid] = StatDef(statid); }
        // Reset Statistic to default value for class

  // Object statistic functions
    int32_t AddObjStat(SStatisticDef &stat, int32_t id = -1);
        // Add a new statistic
    void DeleteObjStat(int32_t statid);
        // Deletes a statistic
    PSStatisticDef GetObjStatisticDef(int32_t statid) const { return objstatdefs.GetStatisticDef(statid); }
        // Get StatisticDef structure
    bool ParseNewObjStat(TToken &t, bool reload = false);
        // Adds a new stat by parsing stream (reload causes stat def to be parsed but ignored)
    int32_t NumObjStats() const { return objstatdefs.NumStats(); }
        // Number of statistics for this class
    const char *ObjStatName(int32_t statid) const { return objstatdefs.StatName(statid); }
        // Returns statistic name
    int32_t ObjStatDef(int32_t statid) const { return objstatdefs.StatDef(statid); }
        // Returns statistic default value
    int32_t ObjStatMin(int32_t statid) const { return objstatdefs.StatMin(statid); }
        // Returns statistic minimum value
    int32_t ObjStatMax(int32_t statid) const { return objstatdefs.StatMax(statid); }
        // Returns statistic maximum value
    uint32_t ObjStatUniqueId(int32_t statid) const { return objstatdefs.StatUniqueId(statid); }
        // Returns statistic maximum value
    int32_t FindObjStat(const char *statname) const { return objstatdefs.FindStat(statname); }
        // Locates stat by name and returns its statid
    const char *GetObjStatDefString(int32_t statid, char *buf) const { return objstatdefs.GetStatDefString(statid, buf); }
        // Gets a parsable string that defines the stat

        // Individual obj stat access stuff

    int32_t GetObjStat(int32_t objtype, int32_t statid) const { return objinfo[objtype].objstats[statid]; }
        // Returns a statistic for an object
    int32_t GetObjStat(int32_t objtype, char *statname) const
        { int32_t statid = objstatdefs.FindStat(statname); 
          if (statid >= 0) return objinfo[objtype].objstats[statid]; else return 0; }
        // Returns a statistic for an object
    void SetObjStat(int32_t objtype, int32_t statid, int32_t newvalue) { objinfo[objtype].objstats[statid] = newvalue; }
        // Sets a statistic for an object (Note: converted to a string if stat is string stat)
    void SetObjStat(int32_t objtype, const char *statname, int32_t newvalue)
        { int32_t statid = objstatdefs.FindStat(statname); 
          if (statid >= 0) objinfo[objtype].objstats[statid] = newvalue; }
        // Sets a statistic for an object (Note: converted to a string if stat is a string stat)
    void ResetObjStat(int32_t objtype, int32_t statid)
        { objinfo[objtype].objstats[statid] = ObjStatDef(statid); }
        // Reset Statistic to default value for class

    void CopyStats(const TObjectClass* from);
        // Copies stats from base class TObjectClass to this class 
        // (copies character class stats to player class, etc.)

    static void SetClassesDirty();
        // For manual changes to the classes

  private:
    static bool ParseClass(TToken &t, bool reload = false);
        // Called by LoadClasses() to parse a class
    bool WriteClass(FILE *fp);
        // Called by SaveClasses() to write out a class

    static int32_t numclasses;
    static TObjectClass* classes[MAXOBJECTCLASSES];

    static FILE *classfp;                               // Stream file pointer when reading/writing
    static bool classesdirty;                           // Set to true when CLASS.DEF needs to be resaved

    const char*     name;
    int32_t         id;
    uint16_t        objflags;
    TObjectClass*   basedon;

    TStatisticDefList statdefs, objstatdefs;// Statistics for objects
    TVirtualArray<SObjectInfo> objinfo;     // There are no limits to these arrays!!
};

// ****************************************************
// * TObjectInstance - Map object instance base class *
// ****************************************************

// The TObjectInstance class represents an instance of an object in a sector.
// Objects have a fixed sector position, flags indicating thier current settings,
// a state for their current imagery characteristics, etc.

#define OF_IMMOBILE      (1<<0)     // Not affected by gravity etc
#define OF_EDITORLOCK    (1<<1)     // Object is locked down (can't move in editor)
#define OF_LIGHT         (1<<2)     // Object generates light (a light is on for object)
#define OF_MOVING        (1<<3)     // Object is a moving object (characters, exits, players, missiles, etc.)
#define OF_ANIMATING     (1<<4)     // Has animating imagery (animator pointer is set)
#define OF_AI            (1<<5)     // Object has A.I.
#define OF_DISABLED      (1<<6)     // Object A.I. is disabled
#define OF_INVISIBLE     (1<<7)     // Not visible in map pane during normal play
#define OF_EDITOR        (1<<8)     // Is editor only object
#define OF_DRAWFLIP      (1<<9)     // Reverse on the horizontal
#define OF_SELDRAW       (1<<10)    // Editor is manipulating object
#define OF_REVEAL        (1<<11)    // Player needs to see behind object (shutter draw)
#define OF_KILL          (1<<12)    // Suicidal (tells system to kill object next frame)
#define OF_GENERATED     (1<<13)    // Created by map generator
#define OF_ANIMATE       (1<<14)    // Call the objects Animate() func AND create object animators
#define OF_PULSE         (1<<15)    // Call the object Pulse() function
#define OF_WEIGHTLESS    (1<<16)    // Object can move, but is not affected by gravity
#define OF_COMPLEX       (1<<17)    // Object is a complex object
#define OF_NOTIFY        (1<<18)    // Notify object of a system change (see notify codes below)
#define OF_NONMAP        (1<<19)    // Not created, deleted, saved, or loaded by map (see below)
#define OF_ONEXIT        (1<<20)    // Object is currently on an exit (used to prevent exit loops)
#define OF_PAUSE         (1<<21)    // Script is paused      
#define OF_NOWALK        (1<<22)    // Don't use walk map for this tile
#define OF_PARALIZE      (1<<23)    // Freeze the object in mid-animation
#define OF_NOCOLLISION   (1<<24)    // Let the object go through boundries
#define OF_ICED          (1<<25)    // Used to know when to end the iced effect

// !!! Remember to put your flags in arrays below too !!!


// These flags are set by the object constructor and shouldn't ever be changed
// Note: Put any flags in here that you ALWAYS want set for objects of a given class
#define OF_FIXEDFLAGS (OF_COMPLEX | OF_NOTIFY | OF_NONMAP | OF_AI | OF_MOVING)

#define OBJFLAGNAMES { "IMMOBILE", "EDITORLOCK", "LIGHT", "MOVING", "ANIMATING", "AI", "DISABLED",\
            "INVISIBLE", "EDITOR", "DRAWFLIP", "SELDRAW", "REVEAL", "KILL", "GENERATED",\
            "ANIMATE", "PULSE", "WEIGHTLESS", "COMPLEX", "NOTIFY", "NONMAP", "ONEXIT",\
            "PAUSE", "NOWALK", "PARALIZE", "NOCOLLISION", "ICED"}

// NOTE: OF_NONMAP
// ----------------
//
// OF_NONMAP tells the map system that this object is managed outside of the regular map
// system.  This object will not be LOADED, SAVED, CREATED, or DELETED by the map or
// sector system.  Any object with this flag can be inserted into the map and assume that
// it won't be deleted by the map systemThis flag is intended for players, but can be used for
// other objects. 



// Notify System:
// -------------
//
// To allow objects to have references (i.e. pointers) to other objects, which may be 
// deleted at any time, we have included a notify system.  This means that...
//
// !!!! WHENEVER YOU USE A POINTER TO ANOTHER OBJECT IN YOUR OBJECT, YOU MUST CHECK  !!!!
// !!!! SEE IF IT HAS BEEN DELETED AND RESPOND ACCORDINGLY IN YOUR NOTIFY() FUNCTION !!!!
//
// Neglecting to do this will cause the system to HANG!
//
// Other notify codes are useful for system events that are infrequent, and need to be
// handled immediately (not handled by checking global values in an objects Pulse()
// function on the next frame).

// Notify Flags
#define N_DELETINGOBJECT    0x00001     // Objects should check if their object pointers are valid
#define N_DELETINGSECTOR    0x00002     // Objects should check if their object pointers are valid
#define N_DELETING          (N_DELETINGOBJECT | N_DELETINGSECTOR) // Is my object pointer still valid?
#define N_CANCELCONTROL     0x00004     // Object scripts should break if this is recieved
#define N_SCRIPTDELETED     0x00005     // Objects should check if their script is still valid
#define N_SCRIPTADDED       0x00006     // Objects should check if they have a new script

// Useful Notify Macros
#define NOTIFY_DELETED(ptr, obj)\
    ( (notify == N_DELETINGOBJECT && obj == (TObjectInstance*)ptr) ||\
      (notify == N_DELETINGSECTOR && obj->GetSector() == (TSector*)ptr) )

#define NOTIFY_CONTROLCANCELLED(ptr, obj)\
  (notify == N_CANCELCONTROL && (TObjectInstance*)ptr == obj)

#define NOTIFY_MOVED(ptr, obj)\
  (notify == N_MOVED && (TObjectInstance*)ptr == obj)

#define NOTIFY_STATECHANGED(ptr, obj)\
  (notify == N_STATECHANGED && (TObjectInstance*)ptr == obj)

// Move() return value flags
#define MOVE_NOTHING    0           // Nothing happened; no movement whatsoever
#define MOVE_MOVED      (1 << 0)    // The object changed position
#define MOVE_BLOCKED    (1 << 1)    // Tried to move but hit an obstacle
#define MOVE_FALLING    (1 << 2)    // Object is falling due to gravity
#define MOVE_NOTMOVING  (1 << 3)    // This object has no current velocity

// This structure is passed to the NewObject function in MapPane()
// Items placed in this structure should describe the initial state of the object
// and not values that will be automatically set by the object class once the object
// has been created.

_CLASSDEF(SObjectDef)
class SObjectDef
{
  public:
    short       objclass;   // Class of item (-1 if no object)
    short       objtype;    // Type of item in class (-1 if no object)
    uint32_t    flags;      // Flags for object

    uint16_t    state;      // State of object
    uint16_t    level;      // Level of object

    S3DPoint    pos;        // Position of object relative in the map (pos is the exact center of the object)
    S3DPoint    vel;        // Movement velocity vector (divide by 0x100 to get world position units)
    S3DPoint    accum;      // Accumulator for movement
    
    uint8_t     rotatex;    // Rotation around the x axis (0 - 255)
    uint8_t     rotatey;    // Rotation around the y axis (0 - 255)
    union
    {
        uint8_t     rotatez;    // Rotation around the z axis (0 - 255)
        uint8_t     facing;     // Direction object is facing (0 to 255 on x-y plane)
    };

    uint8_t     group;      // Group of object
};

// Instance-specific names - keep this number smallish
#define MAXCONTEXTNAME      20

// Damage types
#define DAMAGE_UNDEFINED    0       // unknown type, is just applied as-is (avoid using this)
    // weapon damage
#define DAMAGE_BLUNT        1       // getting hit on the head with a brick
#define DAMAGE_SLASHING     2       // getting slashed by a GinsuKnife(tm)
#define DAMAGE_PIERCING     3       // getting poked with a sticting needle
    // elemental damage
#define DAMAGE_EARTH        10      // between a rock and a hard place
#define DAMAGE_FIRE         11      // getting burnt
#define DAMAGE_LIGHTNING    12      // getting eletricuted
#define DAMAGE_ICE          13      // brr...chilly
    // special damage
#define DAMAGE_POISON       20      // taking poison damage

#define LOAD_BASE(n) { uint8_t n##ver; is >> n##ver; n::Load(is, version, n##ver); }
#define SAVE_BASE(n) { os << (uint8_t)n::ObjVersion(); n::Save(os); }

_CLASSDEF(TObjectInstance)
class TObjectInstance : protected SObjectDef
{
  // Function Members

  public:

  // Clearing function
    void ClearObject();

  // Constructors/Destructors
    TObjectInstance() { ClearObject(); }
    TObjectInstance(TObjectImagery* img);
        // Constructor for new objects
    TObjectInstance(SObjectDef* def, TObjectImagery* img);
        // Constructor for new objects
    virtual ~TObjectInstance();
        // Destructor - call class to decrease usage count

  // Object status functions
    SObjectInfo* GetInfo() const { return inf; }
        // Get the object's info struct
    const char *GetClassName() const { return cl->ClassName(); }
        // Get the object's info struct
    const char *GetTypeName() const { return inf->name; }
        // Get type name
    void SetTypeName(char *newname) { strcpy(inf->name, newname); }
        // Set type name
    const char *GetName() const { return name; }
        // Get instance-specific name
    void SetName(char *newname);
        // Set context name
    uint32_t ObjId() const { return inf->uniqueid; }
        // Returns object's unique id
    int32_t ObjClass() const { return objclass; }
        // Returns object class
    int32_t ObjType() const { return objtype; }
        // Returns object class
    int32_t GetGroup() const { return group; }
        // Returns group number of object
    void SetGroup(int32_t newgroup) { group = newgroup; }
        // Sets the group number of the object
    void GetPos(S3DPoint& getpos) const { getpos = Pos(); }
        // Gets current object position (world space).
    S3DPoint Pos() const;
        // World-space position. Reads from the transform's global
        // matrix, so the cost is at worst a parent-chain recompute
        // (cached afterwards). Returns by value -- the const-ref
        // overload is gone because the value is computed, not a
        // direct member reference.
    int32_t Distance(const TObjectInstance* inst) const;
        // Returns the distance on the x-y plane between this and inst
    int32_t SqrDist(const TObjectInstance* inst) const { return SQRDIST(pos, inst->pos); }
        // Returns the square of the distance between this and inst
    int32_t AngleTo(const TObjectInstance* inst) const;
        // Returns the angle to the other instance
    int32_t FaceAngleTo(const TObjectInstance* inst) const;
        // Returns the + or - difference between this objects facing and the dest obj
    void GetScreenPos(int32_t &x, int32_t &y) const;
        // Gets x and y pixel position of object
    void GetScreenPos(S3DPoint &s) const;
        // Converts tile pos z to screen zbuffer z
    virtual int32_t SetPos(const S3DPoint& newpos, int32_t newlevel = -1, bool override = false);
        // Sets current object position, returns object's sector number (override to ignore bounds checking)
    void ForcePos(const S3DPoint& newpos);
        // Forces current position to new postion with NO validity checking, sector transfer, etc.
        // Same single setter as SetPos -- routes through transform_ so the world matrix stays
        // coherent -- but skips the side effects.
    virtual void MoveTo(const S3DPoint& newpos) { SetPos(newpos, -1, false); }
        // Moves object to new position (does walk checking for characters).
        // Use this function instead of SetPos() to avoid moving objects through or onto
        // barriers.
    void GetSnapPos(const TObjectInstance* oi, int32_t dist, S3DPoint &p) const;
        // Return the snap position from this character given this distance
    void SnapDist(TObjectInstance* oi, int32_t dist);
        // Moves this object so that it is exactly 'dist' distance from the object 'oi', while
        // keeping the angle between them the same.
    int32_t GetLevel() const { return level; }
        // Returns level of object
    int32_t SetLevel(int32_t newlevel) { return SetPos(pos, newlevel, false); }
        // Sets the level of the object
        // Note: Only OF_NONMAP objects can move between levels!
    void ForceLevel(int32_t newlevel) { level = newlevel; }
        // Sets the level variable of an object directly (useful for various load/save functions) 
    void ClearAccum() { memset(&accum, 0, sizeof(S3DPoint)); }
        // Clear out object's movement accumulator
    uint32_t GetNotify() const { return notifyflags; }
        // Gets object notify flags
    void SetNotify(uint32_t newflags);
        // Sets object notify flags
    virtual void Notify(int32_t notify, void *ptr);
        // Notify Action
    void ForceSector(TSector* newsector) { sector = newsector; }
        // Directly sets the sector pointer for this object
    TSector* GetSector() const { return sector; }
        // Get the object's current sector
    int32_t GetState() const { return state; }
        // Gets current object state
    int32_t GetPrevState() const { return prevstate; }
        // Returns the state we were in last time SetState() was called
    const char *GetStateName() const { return GetAniName(); }
        // Returns the name of the current state
    virtual bool SetState(int32_t state);
        // Sets current object state
    virtual bool SetState(const char *statename) { return SetState(FindState(statename)); }
        // Sets current object state
    int32_t NumStates() const { if (imagery) return imagery->NumStates(); return 1; }
        // Return number of states for object
    int32_t GetFrame() const { return frame; }
        // Returns frame number
    int32_t GetPrevFrame() const { return prevframe; }
        // Gets the previous frame we were in last time we called SetFrame()
    void SetFrame(int32_t newframe) { frame = newframe; }
        // Sets frame number
    int32_t GetFrameRate() const { return framerate; }
        // Gets the current framerate
    void SetFrameRate(int32_t newframerate) { framerate = newframerate; }
        // Sets the current framerate
    TObjectImagery* GetImagery() const;
        // Returns object's imagery
    virtual bool HasAnimator() const { return GetComponent<TObjectAnimator>() != nullptr; }
        // Returns true if object is an animating object.
    virtual PTObjectAnimator GetAnimator() const { return GetComponent<TObjectAnimator>(); }
        // Returns object's animator (now a TObjectComponent under the hood).
    virtual bool CreateAnimator();
        // Creates Animator
    virtual void FreeAnimator();
        // Releases Animator
    virtual bool NeedsAnimator() const;
        // Check to see if an animator is necessary for object at the moment
    virtual bool IsAnimatorPermanent() const { return false; }
        // True for object types whose animator is attached at construction
        // and retained for the object's lifetime (e.g. TCharacter and
        // descendants). Animator-toggling code paths (OffScreen / SetState /
        // sector-unload) skip FreeAnimator() on objects that report true,
        // so an attached animator survives sector load/unload churn. The
        // animator is still released by the destructor when the object
        // itself is destroyed.
    virtual bool DrawShadow() { return false; }
        // Returns true of the object wants a simple alpha-channel circle shadow to follow it

    [[nodiscard]] TTransform&       Transform()       { return transform_; }
    [[nodiscard]] const TTransform& Transform() const { return transform_; }
        // Direct access to this instance's spatial transform. Use it
        // for hierarchy operations (SetParent / GetChildAt) and for
        // reading the cached world matrix. Phase A: position is the
        // only field driven through transform_; rotation continues to
        // live on the legacy rotatex / rotatey / rotatez fields.

  // Component functions
    int32_t AddComponent(std::unique_ptr<TObjectComponent> component);
        // Adds an owned component and returns its component slot, or -1.
    void RemoveComponent(int32_t component_slot);
        // Deletes and removes the owned component in the given slot.
    TObjectComponent* GetComponent(int32_t component_slot) const;
        // Returns the component in the given slot, or nullptr.
    int32_t NumComponents() const { return int32_t(components.size()); }
        // Number of component slots currently in use.
    template <class T>
    T* GetComponent() const
    {
        for (const auto& component : components)
            if (TObjectComponent* c = component.get())
                if (T* typed = dynamic_cast<T*>(c))
                    return typed;
        return nullptr;
    }
    template <class T>
    T* FindComponent() const { return GetComponent<T>(); }

  // Object flag functions  
    int32_t GetNumFlags() const;
        // Gets number of object flags
    const char *GetFlagName(int32_t flagnum) const;
        // Returns the name of a flag
    int32_t GetFlagNum(const char *flagname) const;
        // Returns the number of the flag (use (1 << flagnum) to get flag)
    uint32_t Flags() const { return flags; }
    uint32_t GetFlags()const { return flags; }
        // Gets object flags
    void SetFlags(uint32_t newflags, uint32_t newmask = 0xFFFFFFFF)
        { ResetFlags((flags | newflags) & newmask); }
        // Sets object flags
    void SetFlag(uint32_t newflag, bool on = true)
        { if (on) ResetFlags(flags | newflag); else ResetFlags(flags & ~newflag); }
        // Sets a particular flag
    void ClearFlag(uint32_t newflag)
        { ResetFlags(flags & ~newflag); }
    void SetFlag(const char *flag, bool on);
        // Sets a particular flag given the flag name
    void ResetFlags(uint32_t newflags = 0);
        // Resets flags

    bool IsFlagSet(const char *flag) const;
        // Returns true if flag is set
    bool IsComplex() const { return flags & OF_COMPLEX; }
        // Is this a complex object
    bool IsOnExit() const { return flags & OF_ONEXIT; }
        // Is character on an exit?
    bool IsMoving() const { return flags & OF_MOVING; }
        // This object is a moving object (can move, doesn't mean it's moving right now)
    bool IsLight() const { return flags & OF_LIGHT; }
        // This object is a moving object (can move, doesn't mean it's moving right now)
    bool IsPulsed() const { return flags & OF_PULSE; }
        // True if this object is pulsed by the system Pulse() function (for ai, control, etc.)
    bool IsAnimated() const { return flags & OF_ANIMATE; }
        // True if this object is animated by the system Animate() function
        // for drawing on screen, etc.
    bool IsCharacter() const { return ObjClass() == OBJCLASS_CHARACTER || ObjClass() == OBJCLASS_PLAYER; }
        // True if object is a character or player object
    bool IsNotify() const { return flags & OF_NOTIFY; }
        // Does this object want to be notified of system events (Notify() function called)

    bool IsParalized() const { return flags & OF_PARALIZE; }
        // True if this object is paralized - froze in time
    void SetParalize(bool on){SetFlag(OF_PARALIZE, on);}
        // Paralize the object, added by Pepper, if this is screwy, hunt down Hersh Reddy
    bool IsNoCollision() const { return flags & OF_NOCOLLISION; }
        // True if this object can pass through thingies
    void SetNoCollision(bool on){SetFlag(OF_NOCOLLISION, on);}
        // Let this object pass through thingies, added by Pepper

    bool IsIced() const { return flags & OF_ICED; }
        // True if this object is frozen in ice
    void SetIced(bool on){SetFlag(OF_ICED, on);}
        // Used to set wether this object is frozen, added by Pepper


  // Object manipulation functions
    virtual void Damage(int32_t damage, int32_t type = DAMAGE_UNDEFINED);
        // Apply damage to an object
    virtual void RepaintObject();
        // Cause object to repaint itself on the screen
    virtual bool AddToInventory(TObjectInstance* inst, int32_t slot = -1);
        // Add inst to this object's inventory, in the given slot (first free slot if none specified)
    virtual bool AddToInventory(const char *name, int32_t number = 1, int32_t slot = -1);
        // Add object of type 'name', amount of 'number' to this objects inventory at slot 'slot'
    virtual void RemoveFromInventory();
        // Remove this object from whatever inventory it is in
    virtual int32_t GiveInventoryTo(TObjectInstance* to, const char *name, int32_t number = 1);
        // Gives the object 'name' to another object.  Will move multiple objects, 
        // or objects with varying amounts if 'number' > 1.
    virtual int32_t DeleteFromInventory(const char *name, int32_t number = 1)
      { return GiveInventoryTo(nullptr, name, number); }
        // Uses the GiveInventoryTo function with a null destination to delete inventory objects
        // from an object.  
    virtual int32_t GetInventoryAmount(const char *name) const;
        // Returns how many 'name' objects are in inventory
    virtual bool HasEmptySlot() const;
        // Returns true if there is an empty slot available
    virtual bool AddToMap();
        // Add this object to the map pane
    virtual void RemoveFromMap();
        // Remove this object from the map pane
    virtual int32_t FindFreeInventorySlot() const;
        // Find the first free inventory slot in the object's inventory
    virtual void SignalAddedToInventory();
        // Called to signal object that it was added to a new inventory
    virtual TObjectInstance* FindObjInventory(const char *name) const;
        // Find an object by name in inventory
    virtual TObjectInstance* FindObjInventory(int32_t objclass, int32_t type = -1) const;
        // Find an object by class and type in inventory
    virtual bool IsInInventory() const { return (inventnum >= 0); }
      // Returns true if the object is in another object's inventory and should not be drawn
    virtual bool Use(TObjectInstance* user, int32_t with = -1);
        // Uses object (user is the person using it, with is the object to use with this)
        // Returns true if the object was actually used, false if nothing could be done with it
    virtual int32_t CursorType(TObjectInstance* with = nullptr) const { return CURSOR_NONE; }
        // Returns type of cursor that should appear when mouse arrow is over the object
    virtual void UseRange(int32_t &mindist, int32_t &maxdist, int32_t &minang, int32_t &maxang)
        { mindist = 20; maxdist = 50; minang = 0; maxang = 255; }
        // Valid locations character can be standing in order to use the object

  // Rotation and velocity and scale functions
    void SetRotateX(int32_t ang) { rotatex = uint8_t(ang); SyncTransformRot(); }
    int32_t GetRotateX() const { return rotatex; }
    void SetRotateY(int32_t ang) { rotatey = uint8_t(ang); SyncTransformRot(); }
    int32_t GetRotateY() const { return rotatey; }
    void SetRotateZ(int32_t ang) { rotatez = uint8_t(ang); SyncTransformRot(); }
    int32_t GetRotateZ() const { return rotatez; }
        // All of the above set or get rotations about the given axis. Setters
        // re-sync transform_ from the full triple after every legacy field
        // write so transform_.LocalRot() / .Matrix() always reflect the
        // current object orientation. The composition order matches the
        // legacy renderer's Rz * Rx * Ry (see SyncTransformRot impl).
    void Face(int32_t newfacing) { SetRotateZ(newfacing); SetMoveAngle(newfacing); }
        // Faces object towards the given direction and sets motion to go in that direction
    void FaceOnly(int32_t newfacing) { SetRotateZ(newfacing); }
        // Faces object towards the given direction, but does not set movement angle to that dir
    int32_t GetFace() const { return GetRotateZ(); }
      // Returns current facing
    void SetVel(S3DPoint &v) { vel = v; }
        // Set object's movement speed
    void GetVel(S3DPoint &v) const { v = vel; }
        // Get object's current movement speed

  // Statistic functions (shortcuts to the object's class)
    const char *StatName(int32_t statid) const { return cl->StatName(statid); }
        // Returns a statistic for an class
    int32_t GetStat(int32_t statid) const { return cl->GetStat(objtype, statid); }
        // Returns a statistic for an class
    int32_t FindStat(const char *statname) const { return cl->FindStat(statname); }
        // Finds a stat and returns its stat id or -1 if not found
    const char *ObjStatName(int32_t statid) const { return cl->ObjStatName(statid); }
        // Returns a statistic for an class
    int32_t GetObjStat(int32_t statid) const { if ((uint32_t)statid < (uint32_t)stats.NumItems()) return stats[statid]; else return 0; }
        // Returns a statistic for an object
    int32_t FindObjStat(char *statname) const { return cl->FindObjStat(statname); }
        // Finds a stat and returns its stat id or -1 if not found
    void SetStat(int32_t statid, int32_t value) { cl->SetStat(objtype, statid, value); }
        // Sets a class statistic
    void SetObjStat(int32_t statid, int32_t value) { if ((uint32_t)statid < (uint32_t)stats.NumItems()) stats[statid] = value; }
        // Sets an object statistic
    void ResetStat(int32_t statid) { cl->ResetStat(objtype, statid); }
        // Resets class stat to default value
    void ResetObjStat(int32_t statid) { stats[statid] = cl->GetObjStat(objtype, statid); }
        // Resets object stat to default value
    int32_t GetStat(const char *statname) const;
        // Returns a statistic given the statistic name (stat can be object or class stat)
    int32_t GetStat(const char *statname, char *str, int32_t id = -1) const;
        // Returns a statistic via sprintf format 'StatnameId.Str' (stat can be object or class stat)
    void SetStat(const char *statname, int32_t value);
        // Sets a statistic given the stat name (stat can be object or class stat)
    void DelStat(int32_t statid) { stats.Collapse(statid); }
        // Deletes a stat in stat array (Used by classes DeleteStat(), don't call directly)

  // Object drawing functions (Called by MapPane only.. don't call directly)
    virtual bool OnObject(SPoint &p)
      { SRect r; 
        if (flags & OF_LIGHT) imagery->GetScreenRect(this, r); // Get just the light icon
            else GetScreenRect(r);
        return r.In(p); }
        // Returns true if map point is on object
    virtual void GetScreenRect(SRect &r)
      { if (flags & OF_LIGHT) GetLightRect(r); else if (imagery) imagery->GetScreenRect(this, r); }
        // Get screen bounding rectangle for object (in world coordinates)
    virtual void GetAnimRect(SRect &r) { if (imagery) imagery->GetAnimRect(this, r); }
        // Get animation bounding rect for the object, if different from screen rect
    virtual int32_t BgDrawMode();
        // Determines the bgdraw pipeline position this object should start at when drawing to bg
    virtual void DrawUnlit(TSurface* surface) { if (imagery) imagery->DrawUnlit(this, surface); }
        // Causes the object to draw itself to the unlit background
    void DrawLit(TSurface* surface) { if (imagery) imagery->DrawLit(this, surface); }
        // Causes the object to draw itself to the lit background
    virtual bool GetZ(TSurface* surface) { return (imagery) ? imagery->GetZ(this, surface) : false; }
        // Get first uncliped zbuffer point by simulating drawing to the surface
    void DrawSelected(TSurface* surface)
        { if (imagery) imagery->DrawSelected(this, surface); }
        // Causes image to draw selection (hilighting) around itself
    virtual void DrawInvItem(int32_t x, int32_t y) { if (imagery) imagery->DrawInvItem(this, x, y); }
        // Draws inventory icon for object
    void DrawLight(TSurface* surface, bool resetid = false);
        // Causes the object to draw light for object
    void RedrawBackground(int32_t bgdraw = -1);
        // Forces the background for the current object rect to be redrawn
        // Allows moving objects to erase and redraw themselves, and redraws lighting
    virtual bool AlwaysOnTop() { if (imagery) return imagery->AlwaysOnTop(this); return false; }

  // More spiffy virtuals
    virtual void Pulse();
        // Master function called every frame (if OF_PULSE is set)
    virtual uint32_t Move();
        // Cause object to make next time unit of movement based on facing and speed
        // if OF_PULSE and OF_MOVING (no drawing done here)
    virtual void SetObjectMotion();
        // Called ONLY by MapPane MoveObjects() function to set
        // face,nextmove,moveangle for next frame
    virtual void Animate(bool draw);
        // Causes the object to prepare the next frame of its animation (if OF_ANIMATE is set)
    virtual void NextFrame();
        // Called to advance objects animation to the next frame (if OF_ANIMATE is set)
    virtual void OnScreen();
        // Called by pane when object comes on screen.
    virtual void OffScreen();
        // Called by pane when object goes off screen.

  // Imagery pass through functions
    virtual PTBitmap GetStillImage(int32_t ostate = -1) { return imagery ? imagery->GetStillImage(ostate < 0 ? GetState() : ostate) : nullptr; }
        // Get still bitmap, if any
    virtual PTBitmap InventoryImage() { return imagery ? imagery->GetInvImage(GetState()) : nullptr; }
        // Returns bitmap for the inventory image
    virtual bool IsInventoryItem() { return (imagery != nullptr && imagery->GetInvImage(GetState()) != nullptr); }
        // Returns whether or not this item can go into an inventory
    virtual int32_t FindState(const char *name) const { return imagery ? imagery->FindState(name) : -1; }
        // Find a state in the object's imagery
    virtual int32_t FindTransitionState(const char *from, const char *to) const { return imagery ? imagery->FindTransitionState(from, to) : -1; }
        // Find a state in the object's imagery
    const char *GetAniName() const { return imagery ? imagery->GetAniName(GetState()) : nullptr; }
        // Gets current animation name
    uint32_t GetAniFlags() const { return imagery ? imagery->GetAniFlags(GetState()) : 0; }
        // Gets current animation flags
    void SaveHeader() { if (imagery) imagery->SaveHeader(); }
        // Causes the header info for the imagery to be saved
    virtual void ResetState();
        // Resets the state back to the first frame again and restarts animation
    void GetFacingBoundBox(int32_t &nx, int32_t &ny, int32_t &nsx, int32_t &nsy);
        // Get the walkmap bounding box with the object's facing calculated in

  // Script functions
    void InitScript(PTScript newscr);
      // Set script to newscr and initialize
    void ResetScript();
      // Reset the current script
    void ContinueScript();
      // Continue script execution (call every frame)
    void ScriptJump(char *label);
      // Jump to a given label in the script
    PTScript GetScript() { return script; }
      // Returns object's script

  // Command parsing function
    virtual int32_t ParseCommand(const TToken& t);
      // Takes a script/editor command line and parses it

  // Sound playing functions
    bool PlayWave(char *soundname, int32_t nr = -1, int32_t volume = -1, int32_t freq = -1);
      // Plays the given sound at this objects current 3D x,y,z pos
      // If 'nr' is not -1, the number in nr is added to end of soundname (i.e. "sound1")
      // Volume and freq are DirectSound volume and frequency values (-1 is default)

  // Light functions
    void RedrawLight();
      // Causes the objects light to be redrawn
    void GetLightRect(RSRect r);
      // Gets the extent of the objects lighting rectangle
    void SetLightIntensity(uint8_t newintensity);
      // Sets the light intensity
    uint8_t GetLightIntensity() { return lightdef.intensity; }
      // Returns objects light intensity
    void SetLightMultiplier(int32_t mult);
      // Sets the light intensity
    short GetLightMultiplier() { return lightdef.multiplier; }
      // Returns objects light intensity
    void SetLightPos(S3DPoint& newpos);
      // Sets light relative position
    void GetLightPos(S3DPoint& getpos) { getpos = lightdef.pos; }
      // Gets the light relative position
    void SetLightColor(SColor color);
      // Sets the light color
    void GetLightColor(SColor &color) { color = lightdef.color; }
      // Returns objects light color
    void SetLightFlags(uint8_t newflags) { lightdef.flags |= newflags; }
      // Sets light flags
    void ResetLightFlags(uint8_t newflags) { lightdef.flags = newflags; }
      // Resets light flags
    uint8_t GetLightFlags() { return (uint8_t)lightdef.flags; }
      // Gets the light flags
    PSLightDef GetLightDef() { return &lightdef; }
      // Returns light data for object
    int32_t GetIllumination(TObjectInstance* oi);
      // Get the amount of illumination from this light to this particular object
    int32_t GetShadow() { return shadow; }
      // Returns shadow index
    void SetShadow(int32_t index) { shadow = index; }
      // Sets shadow index

  // State handling functions
    virtual bool CommandDone() { if (!HasAnimator()) return true; return commanddone; }
    virtual void SetCommandDone(bool newcmd);

  // Streaming functions
    static TObjectInstance* LoadObject(RTInputStream is, int32_t version, bool ismap = false);
        // Loads and creates a new object from stream 
        // (objs with OF_NOSAVEMAP i.e. players will be ignored if ismap is true)
    static void SaveObject(TObjectInstance* inst, RTOutputStream os, bool ismap = false);
        // Saves object to stream (includes header and block information)
        // (objs with OF_NOSAVEMAP i.e. players will be ignored if ismap is true)
    virtual int32_t ObjVersion() { return 0; }
        // The version id of the object for the Load()/Save() functions.
        // This version number allows objects to change what they stream to the map
        // while still being able to load previous versions of the object.
        // !!!!! IMPORTANT !!!!! When you override the ObjVersion() function, you
        // MUST use the LOAD_BASE() and SAVE_BASE() macros to load and save the base
        // class or the object version for your base class will be WRONG!
    virtual void Load(RTInputStream is, int32_t version, int32_t objversion);
        // Loads object data from the sector 
        // NOTE: When you need to call the base class load function, make sure you 
        // use LOAD_BASE(TBaseClass) instead of TBaseClass:Load(is, version, objversion)
        // or the base class version number will be WRONG!!
    virtual void Save(RTOutputStream os);
        // Saves object data to the sector
        // NOTE: When you need to call the base class load function, make sure you 
        // use SAVE_BASE(TBaseClass) instead of TBaseClass::Save(os)
        // or the base class version number will be WRONG!!
    virtual void LoadInventory(RTInputStream is, int32_t version);
        // Load inventory recursively
    virtual void SaveInventory(RTOutputStream os);
        // Save inventory recursively

  // Inventory access functions
    TObjectInstance* GetInventory(int32_t index) const;
        // Gets inventory object, index is NOT the same as inventory slot
    TObjectInstance* GetInventorySlot(int32_t slot) const;
        // Searches for inventory object occupying the given slot
    int32_t NumInventoryItems() const { return inventory.NumItems(); }
        // Number of objects in the array
    int32_t RealNumInventoryItems();
        // Number of *used* objects in the array
    int32_t InventNum() { return inventnum; }
    int32_t InvIndex() { return invindex; }
    void SetInventNum(short i) { inventnum = i; }

  // Owner functions
    TObjectInstance* GetOwner() { return owner; }
    TObjectInstance* GetTopOwner()
        { TObjectInstance* inst = this;
          while (inst->owner) inst = inst->owner;
          return inst == this ? nullptr : inst; }
        // Recurse through the owner list and get the topmost owner
    void SetOwner(TObjectInstance* newown) { owner = newown; }

  // Mapindex functions
    void SetMapIndex(int32_t newindex);   // out-of-line: also syncs the
                                          // MapPane mapindex→instance registry
    int32_t GetMapIndex() const { return mapindex; }

  // TSafeRef<T> contract -- see comment block before TSafeRef in this file.
  // SafeRefId is the mapindex (the existing stable id for instances).
  // SafeRefGen is a per-instance generation counter, fresh per construction;
  // SafeRef captures (id, gen) so a recycled slot resolves to nullptr
  // through a stale ref instead of the new tenant.
    int32_t  SafeRefId()  const { return mapindex; }
    uint32_t SafeRefGen() const { return safe_ref_gen; }
    static TObjectInstance* SafeRefLookup(int32_t idx) { return ::LookupMapIndex(idx); }

  // Nextmove functions
    void SetNextMove(S3DPoint& p);
      // Set next frame's movement.
    void GetNextMove(S3DPoint& p);
      // Get next frame's movement.
    void SetMoveAngle(int32_t ang) { moveangle = ang; }
    int32_t GetMoveAngle() { return moveangle; }
      // Angle of motion - does NOT have to match facing, though it usually does
    void SetMoveDist(int32_t dist) { movedist = dist; }
    int32_t GetMoveDist() { return movedist; }
      // Angle of motion - does NOT have to match facing, though it usually does
    void SetMoveVert(int32_t vert) { movevert = vert; }
    int32_t GetMoveVert() { return movevert; }
      // Angle of motion - does NOT have to match facing, though it usually does
    void Halt() { movedist = 0; }
      // Halt forward character motion.
    void SetMoveBits(uint32_t newbits) { movebits = newbits; }
      // Called only by the MoveObjects() function in MapPane when all Move() funcs called
    uint32_t GetMoveBits() { return movebits; }
      // Returns result flags from latest move

  // Basic object/character stats
  // Note: The macro's below pre-define empty virtual XXXX() and SetXXXX(v) functions
  // You only need to predefine stats here that you want to be able to use from
  // any object.  Some stats, (i.e. player strength, dexterity) that are only accessed
  // in the local object should not be defined here, but only in that object.

      // Money, inventory, etc.
    PSTATFUNC(EqSlot)
    PSTATFUNC(Value)
    PSTATFUNCVAL(Amount, 1)

      // Character, player, etc.
    POBJSTATFUNC(Aggressive)
    POBJSTATFUNC(Poisoned)
    POBJSTATFUNC(Sleeping)
    POBJSTATFUNC(Health)
    POBJSTATFUNC(Fatigue)
    POBJSTATFUNC(Mana)
    POBJSTATFUNC(MaxHealth)
    POBJSTATFUNC(MaxFatigue)
    POBJSTATFUNC(MaxMana)

  protected:

    // ****************************************************************************
    // **** !!!WARNING!!! *** !!!WARNING!!! *** !!!WARNING!!! *** !!!WARNING!!! ***
    // ****************************************************************************

    // Every data member you add to this structure is repeated THOUSANDS of 
    // times (once for each object in the map).  PLEASE use dynamically 
    // allocated sub structures.. put derived class specific data in derived 
    // classes, and just try to keep things small!

    // ****************************************************************************
    // **** !!!WARNING!!! *** !!!WARNING!!! *** !!!WARNING!!! *** !!!WARNING!!! ***
    // ****************************************************************************

  // Identifier
    char *name;                 // What is my name
    uint32_t notifyflags;       // Notify Objects of changes
    int32_t mapindex;           // Unique instance id
    uint32_t safe_ref_gen;      // Per-instance generation; TSafeRef<T>
                                // captures (id, gen) and rejects lookups
                                // where gen mismatches the current
                                // tenant of the slot. Assigned fresh in
                                // ClearObject() from a global counter.
    TSector* sector;            // Which sector am I in
    TObjectClass* cl;           // Pointer to object's class
    SObjectInfo* inf;           // Pointer to object type info
    int32_t shadow;             // Attached shadow (-1 if none)

  //Animation/drawing
    TObjectImagery* imagery;    // Pointer to current imagery object
    short frame, framerate;     // Frame number and framerate for object
    uint16_t prevstate;         // Previous state
    short prevframe;            // Previous state's last frame (not previous frame for this state)
    // Animator + visual components live here. The animator was a dedicated
    // pointer field; it is now stored as a TObjectComponent and looked up
    // through GetAnimator() / GetComponent<TObjectAnimator>().
    std::vector<std::unique_ptr<TObjectComponent>> components;

    // Spatial transform: SafeRef-able, supports parent / children
    // hierarchy, lazy / sweep matrix recompute. Phase A integration
    // tracks position only -- rotation / scale stay on the legacy
    // rotatex / rotatey / rotatez fields for now (rotation needs a
    // ZXY-vs-XYZ Tait-Bryan conversion that's its own follow-up).
    // The legacy `pos` field is kept in lockstep with transform_
    // by SyncTransformPos() so existing const S3DPoint& Pos()
    // callers see the same value through either path.
    TTransform transform_;
        // Authoritative spatial state. Pos() / GetPos() and the world
        // matrix all flow through this. The legacy `pos` field stays
        // as a synced shadow (mirrored from transform_ on every
        // setter) so the 349-odd direct `oi->pos.x` reads scattered
        // across the codebase don't have to change in lockstep.

    void SyncTransformRot();
        // Rebuild transform_'s rotation from the current rotatex /
        // rotatey / rotatez triple. Called after any legacy-side
        // rotation write. Composition matches BuildRootMatrixSource:
        // Rz * Rx * Ry, applied to a row vector as v * Rz * Rx * Ry,
        // i.e. rotation order is Z, then X, then Y.

  // Inventory
    TObjectInstance* owner;     // What container it is in
    TPointerArray<TObjectInstance, 0, 4> inventory;
    short inventnum;            // Inventory slot number
    short invindex;             // Inventory index (NOT necessarily equal to slot number)

  // Script
    bool  commanddone;          // Indicates if command is complete
    PTScript script;            // Pointer to object's script elements

  // Light
    SLightDef lightdef;         // Current light definition

  // Statistics                 
    TStatisticList stats;       // Stats for object

  // Working members
    int32_t                 moveangle;  // Angle to move next move
    int32_t                 movedist;   // Distance to move next move
    int32_t                 movevert;   // Vertical distance to move next move
    uint32_t                movebits;   // Result of last move

    // members to optimize function calls
    mutable S3DPoint oldpos;                    // Where it was last time function was called
    mutable int32_t screenx, screeny, screenz;  // Pixel/zbuf coords as of oldpos
};

// TSafeRef<T>, safe_cast helpers, and TSafeObjectBase<Self> live in
// saferef.h so types that just need them (e.g. TTransform) don't have
// to drag the umbrella include chain in. TSafeComponentRef stays here
// because it needs full TObjectInstance / TObjectComponent definitions.

// Safe reference to a component owned by a TObjectInstance. It resolves the
// owner through the normal mapindex registry, then validates the component slot
// and generation before returning a pointer.
template <typename T = TObjectComponent>
class TSafeComponentRef
{
  public:
    TSafeComponentRef() = default;
    TSafeComponentRef(const T* component) { Set(component); }

    void Set(const T* component)
    {
        owner = component ? component->Owner() : nullptr;
        component_slot = component ? component->ComponentSlot() : -1;
        generation = component ? component->Generation() : 0;
    }
    void Clear()
    {
        owner.Clear();
        component_slot = -1;
        generation = 0;
    }

    [[nodiscard]] T* Get() const
    {
        TObjectInstance* inst = owner.Get();
        if (!inst || component_slot < 0) return nullptr;
        TObjectComponent* component = inst->GetComponent(component_slot);
        if (!component || component->Generation() != generation) return nullptr;
        return static_cast<T*>(component);
    }
    [[nodiscard]] bool IsValid() const { return Get() != nullptr; }

    explicit operator bool() const { return IsValid(); }
    T* operator->() const { return Get(); }
    T& operator*() const { return *Get(); }

  private:
    TSafeRef<TObjectInstance> owner;
    int32_t component_slot = -1;
    uint32_t generation = 0;
};

inline void rollover(int32_t &i, int32_t &j)
{
    if (absval(i) >= ROLLOVER)
    {
        int32_t k = i / ROLLOVER;
        j += k;
        i -= k * ROLLOVER;
    }
}

