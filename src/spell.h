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

_STRUCTDEF(SSpellVariant)
struct SSpellVariant
{
    int32_t flags;                      // Spell variant flags
    char name[RESNAMELEN];          // Name of spell variant
    char talismans[MAXTALISMANLEN]; // Talismans for this variant
    char effect[RESNAMELEN];        // Number of effect id for this variant (0-whatever)
    int32_t mana;                       // Amount of mana for this variant
    int32_t nextspellwait;              // Amount of time to wait before next spell
    int32_t mindamage,maxdamage;        // Damage values for this variant
    int32_t skilllevel;                 // What skill you must be to cast this spell
    int32_t type;                       // the type of variant
    int32_t height;                     // the height of the spell
    int32_t facing;                     // the facing of the spell
    int32_t ani_delay;                  // animation delay
};

typedef TVirtualArray<SSpellVariant, 16, 16> TSpellVariantArray;

// Spell flags
#define SF_NONE 0

_STRUCTDEF(SSpellData)
struct SSpellData
{
    SSpellData();
      // Sets default values
    ~SSpellData();
    bool Load(char *aname, TToken &t);
      // Loads the spell from the "SPELL.DEF" file

    TSpellVariantArray variants;        // Spell variants

    int32_t flags;                          // Spell flags
    char name[NAMELEN];                 // Name of spell type (not individual name)
    char objname[RESNAMELEN];           // Name of spell object to build when spell is cast
    char *desc;                         // Spell description for spell book
//  PTBitmap icon;                      // Spell book icon for spell
    int32_t damagetype;                     // Type of damage spell does
    char invoke[RESNAMELEN];            // Invoke animation
    int32_t effectstart;                    // Frames after starting invoke to start effect
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
    ~TSpellList() { Close(); }

    bool Initialize();
      // Initializes spell data stuff
    void Close();
      // Kills all spell data stuff
    int32_t NumSpells() { return spelldata.NumItems(); }
      // Returns number of spells
    PSSpellData GetSpellData(int32_t num) { return spelldata[num]; }
      // Gets pointer to spell data based on talisman list
    PSSpellData GetSpellDataByTalismans(char *talismans);
      // Gets pointer to spell data based on talisman list
    PSSpellData GetSpellDataByName(char *name);
      // Gets pointer to spell data based on spell name
    PSSpellVariant GetVariantDataByName(char *name);
      // Gets pointer to a variant data based on spell name
    PSSpellVariant GetVariantDataByTalismans(char *talismans);
      // Gets pointer to a variant data based on talismans name
    bool Load();
      // Loads spell data from SPELL.DEF file

    // return an array of MAXTALISMANLEN filled with appropriate stuff
    static void GetTalList(char* string, int32_t* tal);
    
    // compare talisman lists, return true for same, false otherwise
    static bool CompareTalList(int32_t* tal1, int32_t* tal2);
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
    { invoker = nullptr; targets[0] = nullptr; targetnum = 0;
      timer = 0; master = nullptr; spell = nullptr; variant = nullptr; frame = 0; 
      magic_offense = 0; magic_defense = 0;
      source.x = source.y = source.z = -1; }

    // Functions to return current spell values
    TObjectInstance* GetInvoker() { return invoker; }
      // Returns the character that invoked the spell
    TObjectInstance* GetTarget(int32_t numtarg = 0) 
        { if (numtarg >= targetnum) return nullptr; else return targets[numtarg]; }
      // Returns the target for the spell
    int32_t GetTargetNum() { return targetnum; }
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
    TObjectInstance* effect;                // The effect for this spell

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
    ~TSpellManager() { spells.Clear(); }
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

