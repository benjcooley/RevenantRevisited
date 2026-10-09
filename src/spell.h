// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                           Spell.h - Spell                             *
// *************************************************************************

#pragma once

#include "revenant.h"

#include "object.h"

// *************************************************************
// * TSpellData/Variant - Describes the static info for spells *
// *************************************************************

// Spell variant flags
#define SVF_NONE 0

#define SPELLSIZE       (6)
#define MAXTALISMANLEN  (SPELLSIZE + 1)

// A VARIANT's talisman string holds at most this many codes: retail's is a
// char[6] at SSpellVariant+0x24, filled by strncpy(.., 5) (0x0053e82c).
constexpr int32_t kVariantTalismanCodes = 5;

// CONTROLDATA: how a variant's spell class (retail registers "Spell" and
// "Strike") runs its effects. Retail SSpellControlData, 0xc4 bytes, zeroed,
// built by SSpellData::Load (0x0053e856) and filled by LoadControlData
// (0x0053dd10), one tag per call.
enum ESpellControlFlags : int32_t
{
    SCF_SHAKE    = 0x2,                 // SHAKE given
    SCF_PLAY     = 0x4,                 // PLAY: loop the sound
    SCF_PLAYONCE = 0x8,                 // PLAYONCE: play the sound once
};

// PATTERN names, in retail's order (1..6; 0 when none is given).
enum ESpellPattern : int32_t
{
    SPAT_NONE, SPAT_RANDOM, SPAT_CIRCLE, SPAT_LINE, SPAT_CLUSTER, SPAT_X, SPAT_SPIRAL
};

constexpr int32_t kMaxAttachMultiple = 20;  // ATTACHMULTIPLE refuses HITS above this

struct SSpellVariant;

struct SSpellControlData
{
    char name[RESNAMELEN] = {};         // +0x00 spell class: CONTROLDATA "Strike"
    int32_t flags = 0;                  // +0x20 ESpellControlFlags
    int32_t radius = 0;                 // +0x24 RADIUS
    int32_t hits = 1;                   // +0x28 HITS
    int32_t duration = -1;              // +0x2c DURATION, first number
    int32_t wait = 0;                   // +0x30 WAIT
    int32_t duration2 = 0;              // +0x34 DURATION, second number
    int32_t pattern = SPAT_NONE;        // +0x38 PATTERN
    int32_t shake = 0;                  // +0x3c SHAKE
    int32_t unknown40 = -1;             // +0x40 set to -1 by the loader, never parsed
    int32_t repeatdamage = 0;           // +0x44 REPEATDAMAGE
    int32_t rangedamage[2] = {};        // +0x48 RANGEDAMAGE
    char attach[RESNAMELEN] = {};       // +0x50 ATTACH
    char *attachmultiple = nullptr;     // +0x70 ATTACHMULTIPLE: HITS names of 32 bytes (new[])
    char sound[RESNAMELEN] = {};        // +0x74 PLAY / PLAYONCE
    int32_t imagery = -1;               // +0x94 IMAGERY, the registered imagery id (or -1)
    char imageryname[MAXPATHLEN] = {};  // the IMAGERY file named (retail keeps only the id)
    int32_t hittarget = 0;              // +0x9c HITTARGET
    int32_t oncaster = 0;               // +0xa0 ONCASTER
    int32_t attachset = 0;              // +0xa4 ATTACH or ATTACHMULTIPLE given
    int32_t follow = 0;                 // +0xa8 FOLLOW
    int32_t posset = 0;                 // +0xac POS given
    int32_t repeatset = 0;              // +0xb0 REPEATDAMAGE given
    int32_t multipletargets = 0;        // +0xb4 MULTIPLETARGETS
    int32_t pos[3] = {};                // +0xb8 POS x y z

    bool Load(TToken &t, SSpellVariant &variant);
      // One tag of the CONTROLDATA block (retail 0x0053dd10)
};

// A VARIANT line. Retail SSpellVariant, 0x78 bytes; offsets noted.
_STRUCTDEF(SSpellVariant)
struct SSpellVariant
{
    int32_t flags = 0;                          // +0x00 Spell variant flags
    char name[RESNAMELEN] = {};                 // +0x04 Name of spell variant
    char talismans[MAXTALISMANLEN] = {};        // +0x24 Talisman codes (kVariantTalismanCodes at most)
    char effect[RESNAMELEN] = {};               // +0x2a Effect object type made when the spell starts
    int32_t mana = 0;                           // +0x4c Mana it costs
    int32_t nextspellwait = 0;                  // +0x50 Pulses before the caster's next spell (raw)
    int32_t mindamage = 0, maxdamage = 0;       // +0x54 / +0x58 Damage range
    int32_t skilllevel = 0;                     // +0x5c Invoke skill needed (-1: no skill roll)
    int32_t type = 0;                           // +0x60 Variant type (TP_BASIC)
    int32_t height = 0;                         // +0x64
    int32_t facing = 0;                         // +0x68 The effect takes the caster's facing
    int32_t ani_delay = 0;                      // +0x6c Invoke animation delay
    SSpellControlData *controldata = nullptr;   // +0x70 CONTROLDATA block (owned by the SSpellData)
    char *statline = nullptr;                   // +0x74 STATLINE, 100 bytes (owned by the SSpellData)
};

typedef TVirtualArray<SSpellVariant, 16, 16> TSpellVariantArray;

// Spell flags
#define SF_NONE 0

// LIGHT <COLOR r,g,b> <INT i> <MULT m> <POS x,y,z> <FADEIN f> <FADEOUT f>:
// retail SSpellData+0xa8..+0xc7, with the constructor's defaults.
struct SSpellLight
{
    uint8_t color[3] = {};              // +0xaa / +0xa9 / +0xa8: r, g, b
    int32_t mult = 0;                   // +0xac MULT
    int32_t intensity = 0;              // +0xb0 INT
    int32_t pos[3] = {65, 65, 65};      // +0xb4 POS
    int32_t fadein = 5;                 // +0xc0 FADEIN
    int32_t fadeout = 5;                // +0xc4 FADEOUT
};

// A SPELL block. Retail SSpellData, 0xcc bytes; offsets noted.
_STRUCTDEF(SSpellData)
struct SSpellData
{
    SSpellData() = default;
    ~SSpellData();
    SSpellData(const SSpellData&) = delete;
    SSpellData& operator=(const SSpellData&) = delete;
    bool Load(char *aname, TToken &t);
      // Loads the spell from the "SPELL.DEF" file

    TSpellVariantArray variants;            // +0x00 Spell variants

    int32_t flags = SF_NONE;                // +0x18 FLAGS
    char name[NAMELEN] = {};                // +0x1c Name of spell type (not individual name)
    char objname[RESNAMELEN] = {};          // +0x3c NAME
    char iconname[RESNAMELEN] = {};         // +0x5c ICONNAME
    char *desc = nullptr;                   // +0x7c DESCRIPTION
    int32_t damagetype = 0;                 // +0x80 DAMAGETYPE
    char invoke[RESNAMELEN] = {};           // +0x84 ANIMATION: the invoke animation
    int32_t effectstart = 0;                // +0xa4 DELAY: pulses after the cast before the effect
    SSpellLight light;                      // +0xa8 LIGHT
    int32_t poisonchance = 0;               // +0xc8 POISONCHANCE
};

typedef TPointerArray<SSpellData, 16, 16> TSpellDataArray;

// ***************************************************
// * TSpellList - Contains a list of all game spells *
// ***************************************************

_CLASSDEF(TSpellList)
class TSpellList
{
  private:
    bool initialized;               // Are we ready
    TSpellDataArray spelldata;      // Data array

  public:
    TSpellList() { initialized = false; }
    // Trivial dtor: explicit Close() runs from ShutdownGlobals.
    ~TSpellList() = default;

    bool Initialize();
      // Initializes spell data stuff
    void Close();
      // Kills all spell data stuff
    int32_t NumSpells() { return spelldata.NumItems(); }
      // Returns number of spells
    PSSpellData GetSpellData(int32_t num) { return spelldata[num]; }
      // The num'th SPELL block, in file order

    // The lookups. Each is a first match in file order (spells, then their
    // variants), compared case-blind (_stricmp); talismans match as a whole
    // string, so order matters ("IL" is not "LI").
    PSSpellData GetSpellDataByTalismans(const char *talismans);
      // REVSYNC: TSpellList::GetSpellDataByTalismans @ 0x0053ed70 -- the
      // spell holding the first variant with these talismans
    PSSpellData GetSpellDataByName(const char *name);
      // REVSYNC: TSpellList::GetSpellDataByName @ 0x0053ede0 -- the first
      // spell whose SPELL name, or one of whose variant names, matches
    PSSpellVariant GetVariantDataByName(const char *name);
      // REVSYNC: TSpellList::GetVariantDataByName @ 0x0053f010
    PSSpellVariant GetVariantDataByTalismans(const char *talismans);
      // REVSYNC: TSpellList::GetVariantDataByTalismans @ 0x0053ef90
    bool Load();
      // Loads spell data from SPELL.DEF file

    // Talisman counts by TALISMAN type (one int per type) and their compare.
    // Retail has both (0x0053ee70, 0x0053ef50) but nothing calls them.
    static void GetTalList(const char* string, int32_t* tal);
    static bool CompareTalList(const int32_t* tal1, const int32_t* tal2);
};

// ***************************************************
// * TSpell - Represents an active spell in the game *
// ***************************************************

#define MAXSPELLTARGETS 64

_CLASSDEF(TSpell)
class TSpell
{
  public:
    // The constructor for the tspell object
    TSpell(TObjectInstance* invoke, TObjectInstance* *targ, int32_t numtargs, S3DPoint* sourcepos, PSSpellData dat, PSSpellVariant var, PTSpell mtr = nullptr);
    TSpell()
    { invoker = nullptr; effect.Clear();
      for (auto& target : targets) target = nullptr;
      targetnum = 0; wait = -1;
      timer = 0; master = nullptr; spell = nullptr; variant = nullptr; frame = 0; 
      magic_offense = 0; magic_defense = 0;
      source.x = source.y = source.z = -1; }
    virtual ~TSpell();

    // Functions to return current spell values
    TObjectInstance* GetInvoker() { return invoker; }
    TSafeRef<TObjectInstance> GetInvokerRef() const { return invoker_ref; }
      // Returns the character that invoked the spell
    TObjectInstance* GetTarget(int32_t numtarg = 0) 
        { if (numtarg >= targetnum) return nullptr; else return targets[numtarg]; }
      // Returns the target for the spell
    int32_t GetTargetNum() { return targetnum; }
    TSafeRef<TObjectInstance> GetTargetRef(int32_t numtarg = 0) const
        { return numtarg >= 0 && numtarg < targetnum ? target_refs[numtarg] : TSafeRef<TObjectInstance>{}; }
    int32_t PoisonChance() const { return spell ? spell->poisonchance : 0; }
      // Returns the number of targets
    bool GetSourcePos(S3DPoint &pos);
      // Gets the source position of the spell.  Either the position passed by 'sourcepos'
      // in the contstructor, or the current 'right hand' position of the character if
      // 'sourcepos' was set to -1,-1,-1.

    PSSpellData SpellData() { return spell; }
      // The spell data structure from the SPELL.DEF file
    PSSpellVariant VariantData() { return variant; }
      // The variant structure from the SPELL.DEF file

    void Timer(int32_t value) { timer = value; }
    void SetByName(char* name);
    void SetByTalismans(char* talismans);

    void Damage(TObjectInstance* ch);

    virtual void Pulse() {}
      // pulse through the spell, this is standard

    virtual bool Timer();
      // returns true if spell is done

    virtual void Kill() { timer = 0; }
      // Kills this spell    

    int32_t GetDefense()        { return magic_defense; }
    void SetDefense(int32_t i)  { magic_defense = i; }

    int32_t GetOffense()        { return magic_offense; }
    void SetOffense(int32_t i)  { magic_offense = i; }

    // drain off the character's mana
    void ManaDrain();

  protected:
    TObjectInstance* invoker;               // Object that invoked the spell
    TSafeRef<TObjectInstance> invoker_ref;
    TSafeRef<TObjectInstance> target_refs[MAXSPELLTARGETS];
    TSafeRef<TObjectInstance> effect;       // Weak identity; reaping/reuse cannot dangle

    int32_t targetnum;                      // Number of targets in target list
    TObjectInstance* targets[MAXSPELLTARGETS]; // Spell's target list

    int32_t timer;                          // Nice timer value for time based spells
    int32_t frame;                          // Frame number for spells
    PTSpell master;                         // Master spell object (if slave)
    
    PSSpellData spell;                      // Data for spell
    PSSpellVariant variant;                 // Variant
    int32_t wait;                           // Wait before the spell starts
    S3DPoint source;                        // Source of spell relative to char pos (-1,-1,-1 if not used)

    int32_t magic_defense;
    int32_t magic_offense;
};

typedef TPointerArray<TSpell, 32, 16> TSpellArray;

// *********************************************
// * TSpellManager - Contains a list of spells *
// *********************************************

_CLASSDEF(TSpellManager)
class TSpellManager
{
  protected:
    TSpellArray spells;                     // Currently active spells
    int32_t wait;                           // wait for spells to be cast
  public:
    TSpellManager() { spells.Clear(); wait = 0; }
      // default constructor
    ~TSpellManager() { spells.DeleteAll(); }
      // default destructor

    void Pulse();
      // pulse through all the spells

    bool CastByName(char* name, TObjectInstance* invoker, TObjectInstance* *targets, int32_t numtargs, S3DPoint* sourcepos = nullptr, PTSpell mst = nullptr);
      // cast a spell by its name, returns success or failure
    bool CastByTalismans(char* talismans, TObjectInstance* invoker, TObjectInstance* *targets, int32_t numtargs, S3DPoint* sourcepos = nullptr, PTSpell mst = nullptr);
      // cast a spell by its talismans, returns success or failure

    int32_t GetDefense();
    int32_t GetOffense();
    int32_t GetSpellCount(char* spell);
};
