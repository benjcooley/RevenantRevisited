// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   rules.h - TRules object module                      *
// *************************************************************************

// This file contains the TRules object, which stores the special global
// character type information for the various characters in the game, class 
// information, and any other user defined rules that are stored in the
// RULES.DEF file.  
//
// See the RULES.DEF file for the definitions of the data stored there

#pragma once

#include "revenant.h"
#include "charstats.h"
#include "playerstats.h"

#include <array>
#include <string>
#include <vector>

#define MAXCHARATTACKS 32

// Character flags

#define CF_UNDEAD       0x0001  // This character can only be damaged by magical/silver weapons
#define CF_MAGICAL      0x0002  // This character can only be damaged by magical weapons
#define CF_INFRAVISION  0x0004  // This character can see in the dark
#define CF_LIGHTBLIND   0x0008  // This character is blinded by light (reverse dark/light sight)
constexpr int32_t CF_BADBLEEDER = 0x0010; // FLAGS bit a BLEEDER value that fails to parse sets (the parse is
                                          // then fatal): ResolveImpact bleeds without it, ResolveDead with it

// Note, searches for attack by searching for 'combo' attacks first, then 'special'
// attacks, then ordinary attacks.  This allows the same controller key to be used for
// different attacks based on the opponent, and for extra attacks

#define CA_SPECIAL       0x0001 // Special attack, set when this attack may override a normal attack
#define CA_RESPONSE      0x0002 // Set when 'response' non-empty (response enabled by opponent state)
#define CA_SPLATTER      0x0004 // Causes opponent to splatter if successful
#define CA_DEATH         0x0008 // Used when damage value is high enough to kill enemy
#define CA_HAND          0x0010 // Is a thrust type attack (can't do with axe)
#define CA_THRUST        0x0020 // Is a thrust type attack (can't do with axe)
#define CA_SLASH         0x0040 // This is a slashing attack (if axe, uses CHOP damage type)
#define CA_CHOP          0x0080 // Is a chop type attack
#define CA_SPARKS        0x0100 // Show sparks on block
#define CA_BLOOD         0x0200 // Show blood on impact
#define CA_SNAPIMPACT    0x0400 // Snap the impact
#define CA_SNAPBLOCK     0x0800 // Snap the block
#define CA_NOMISS        0x1000 // Will play entire attack ani even if misses
#define CA_SNEAK         0x2000 // This attack only available in sneak mode
#define CA_CHAIN         0x4000 // Will automatically play next attack when next button pressed
#define CA_AUTOCOMBO     0x8000 // Will play this attack, then the next attack after waiting
#define CA_ATTACKDOWN    0x40000 // Attack for when opponent is on ground only!
#define CA_ATTACKSTUN    0x80000 // Attack for when opponent is stunned only!
#define CA_MOVING        0x100000 // This attack is only available when character is moving
#define CA_ONETARGET     0x200000 // This attack is applied to only one character
#define CA_NOPUSH        0x400000 // This attack will not push the other character
#define CA_MAGICATTACK   0x800000 // This is a magical attack (uses the MAGICATTACK tag)
#define CA_PLAYANIM      0x1000000 // Just plays the given animation
#define CA_INTERACTIVE   0x2000000 // This is an interactive attack (syncs impacts, nopush, nocharblock)
#define CA_SNEAKMODE     0x4000000 // This attack works in sneak mode
#define CA_WALKMODE      0x8000000 // This attack works in walk mode
#define CA_BOWMODE       0x10000000 // This attack works in bow mode
#define CA_RUNNING       0x20000000 // This attack is a running attack
#define CA_ACTION        0x40000000 // This is an action, not an attack
#define CA_NOINTERRUPT   0x80000000 // This is an uninterruptable animation (retail char.def)

// Damage types

#define NUMDAMAGETYPES 10

#define DT_NONE     -1      // Does no damage whatsoever
#define DT_MISC     0       // Just damage (no specific type)
#define DT_HAND     1       // Damage from hand to hand combat
#define DT_PUNCTURE 2       // Puncture damage (arrows, knives and swords thrusts)
#define DT_CUT      3       // Cutting or slashing damage (knive and sword swings)
#define DT_CHOP     4       // Chopping damage (knive, sword, or axe chops)
#define DT_BLUDGEON 5       // Bludgeoning damage (warhammers, clubs, maces)
#define DT_MAGICAL  6       // Magical damage (other than any that fit below)
#define DT_BURN     7       // Burn damage (i.e. lava, fileball, etc.)
#define DT_FREEZE   8       // Freeze damage (i.e. frost, etc.)
#define DT_POISON   9       // Poison damage

#define ANIMLISTLEN 48
#define CHARGROUPLEN 80     // GROUPS / ENEMIES: retail `%80s` (0x0048a14d, 0x0048a17c)

#define CAI_STUN        0x0001  // Results in a looping stun using 'loopname' and 'loopwait'
#define CAI_KNOCKDOWN   0x0002  // Results in a looping knockdown using 'loopname' and 'loopwait'
#define CAI_DEATH       0x0004  // This describes a death impact (other than default "death" state)
#define CAI_WHENSTUNNED 0x0008  // This impact will only be used when the character is stunned
#define CAI_WHENDOWN    0x0010  // This impact will only be used when the character is knocked down
#define CAI_FLYBACK     0x0020  // This impact causes character to fly back (use with CAI_KNOCKDOWN!)
#define CAI_BLOOD       0x0040  // This impact spouts blood
#define CAI_INTERACTIVE 0x0080  // Interactive impact (retail char.def; Go refuses to step out of one)

#define CAI_CHARIMPACT  0x1000  // Set on a CHARIMPACT, cleared on an IMPACT (retail 0x0048a926 / 0x0048a95e)

// Impact flags the shipped char.def uses beyond the 1998 list (COMBAT_HIT.md
// §3.4). A death or impact block turns to face whoever it answers, then by
// these; TCharacter::Damage also takes the three turns as a filter on the
// CHARIMPACTs: where the character's combat target stands, by absolute
// bearing.
constexpr int32_t CAI_KEEPANGLE   = 0x0100;  // no turn
constexpr int32_t CAI_TURN180     = 0x0200;  // turned away; target at bearing 0x60-0x9f
constexpr int32_t CAI_TURNPLUS90  = 0x0400;  // +64; target at 0x20-0x5f
constexpr int32_t CAI_TURNMINUS90 = 0x0800;  // -64; target at 0xa0-0xdf

#define CA_FATIGUEATTACK 0x10000 // A FATIGUEATTACK (retail 0x00489ca2); not among char.def's CA_ defines

// MAGICATTACK conditions (char.def MASTAT_*): when the AI may cast it
#define MASTAT_NONE     1
#define MASTAT_HEALTHLT 2
#define MASTAT_HEALTHGT 3
#define MASTAT_MANALT   4
#define MASTAT_MANAGT   5

#define MAXATTACKIMPACTS 6
#define MAXCHARIMPACTS   6
#define MAXATTACHEFFECTS 4

// The attack and impact records are plain data: the loaders zero a record
// and fill it from one tag (retail builds them on the stack the same way).

// REVSYNC: impact record, 0x5c bytes (IMPACT / CHARIMPACT, 0x00489850)
_STRUCTDEF(SCharAttackImpact)
struct SCharAttackImpact
{
    char impactname[MAXANIMNAME];   // Impact name for this impact
    int32_t index;                      // Its slot in the attack's / character's impact list (retail +0x20)
    int32_t flags;
    char loopname[MAXANIMNAME];     // Animation played when opponent stunned, knocked down, killed
    int32_t looptime;                   // Amount of time to play loop animation
    int32_t damagemin, damagemax;       // Damage range for this impact
    int32_t snapdist, snaptime;         // Dist from chr to snapto (or pushto) in snaptime frames
};

// REVSYNC: attack record, 0x320 bytes (ATTACK / FATIGUEATTACK / MAGICATTACK /
// PLAYANIM, 0x00489850; added by TCharAttackArray::Add 0x0048d230). Retail
// also has an index at +0x20 that the stored copy never receives (it is
// written into the loader's local copy after the add), so it is always 0.
_STRUCTDEF(SCharAttackData)
struct SCharAttackData
{
    char attackname[MAXANIMNAME];   // Name of attack animation to use for attack
    int32_t flags;                      // Defines attack flags
    int32_t button;                     // Attack button id (for joystick/keyboard button)
    int32_t attackpcnt;                 // MONSTER: Percentage 0-100 monster will attempt this attack
    int32_t mindist, maxdist;           // Minimum and maximum distance this attack can be used for
    union
    {
      struct    // Information for ordinary attack
      {
        char responsename[MAXANIMNAME]; // Is response combo/counter when opponent in this state
        char blockname[MAXANIMNAME];    // Block opponent may use to block shot
        char missname[MAXANIMNAME];     // Miss animation to use if character is blocked or misses
        char chainname[MAXANIMNAME];    // This attack follows this previous attack in attack chain
        int32_t blocktime;                  // Frames after attack in which opponent can block/counterattack
        int32_t impacttime;                 // Frames to wait to start impact
        int32_t chainexptime;               // Next attack in chain must be done before this time expires
        int32_t nextwait;                   // Frames to wait before doing next attack
        int32_t hitminrange, hitmaxrange;   // Hits character only when within this range
        int32_t hitangle;                   // Hits character only when he is within this angle
        int32_t damagemod;                  // Damage modifier for attack (10=+10%,-30=-30%)
        int32_t fatigue;                    // Base fatigue value for attack
        int32_t attackskill;                // PLAYER: Min offensive skill level to enable attack
        int32_t weaponmask;                 // PLAYER: Weapon mask for this attack
        int32_t weaponskill;                // PLAYER: Min weapon skill level to enable attack
        int32_t numimpacts;                 // Number of impacts for attack
        int32_t swipeframeon;               // Weapon swipe from this frame (ATTACK field 23)
        int32_t swipeframeoff;              // ... to this frame (ATTACK field 24)
        int32_t maxfatigue;                 // FATIGUEATTACK field 23 (0 for an ATTACK)
        SCharAttackImpact impacts[MAXATTACKIMPACTS];  // Impacts for attack
     };
     struct     // Information for magical attack
     {
        char spellname[RESNAMELEN];     // Name of spell to cast
        S3DPoint spellsource;           // Offset of spell source for this character
        int32_t condition;              // MASTAT_*: when the AI may cast it
        int32_t conditionvalue;         // The health / mana the condition compares with
     };
    };
};

typedef TVirtualArray<SCharAttackData, 16, 16> TCharAttackArray;

_STRUCTDEF(SClassData)
struct SClassData
{
    bool Load(char *aname, TToken &t);
      // Loads the class info from the "CHAR.DEF" file

    char     name[RESNAMELEN]      = {}; // Name of character type (not individual name)
    int32_t  statreqs[NUM_PLRSTATS] = {}; // Stat requirements (-x=below,+x=above,0=don't care)
    int32_t  skillmods[NUM_SKILLS]  = {}; // Plus or minus modifiers for skills
    int32_t  healthmod  = 0;
    int32_t  fatiguemod = 0;
    int32_t  manamod    = 0;
    int32_t  weapons    = 0xffff;         // WEAPONS: the WM_* weapons the class may use (retail +0x70)
};

typedef TPointerArray<SClassData, 16, 16> TClassDataArray;

// REVSYNC: a WEAPON.DEF / ARMOR.DEF entry (retail records of 0xd0 / 0xcc
// bytes, loaders 0x0048ac30 / 0x0048b1a0). Retail binds an entry to the
// WEAPON or ARMOR object type of the same name (0x0048c930); the port looks
// it up by that name.
struct SItemData
{
    bool Load(const char *aname, TToken &t);
      // Loads the block after `WEAPON "<name>"` / `ARMOR "<name>"`

    std::string name;
    std::string description;                // DESCRIPTION: a dialog tag
    std::array<int32_t, 8> basicmods{};     // BASICMODS, in file order (the comment row above each)
    std::string statline;                   // STATLINE as retail stores it (PlayerStats::ReadStatLine); empty when none
};

// REVSYNC: one ATTACHEFFECT slot of a character (0x4c bytes at +0x458,
// 0x00489850). Nothing in the shipped data uses the tag and its readers are
// not traced, so the fields keep the tag's order rather than meanings.
struct SCharAttachEffect
{
    int32_t values[3]  = {};                // fields 2-4
    char    arg5[20]   = {};                // field 5 ("none" -> "")
    char    arg6[20]   = {};                // field 6 ("none" -> "")
    char    name[20]   = {};                // field 1
    bool    used       = false;
};

// REVSYNC: SCharData, 0x590 bytes. The member initializers are retail's
// defaults: malloc's zero fill plus SetDefaults 0x004895d0. The loader then
// sets objtype / objclass (0x00489850: "Default" -1 / -1, any other name -2
// / -2 until TRules::BindTypes finds its object type).
_STRUCTDEF(SCharData)
struct SCharData
{
    SCharData() = default;
    ~SCharData();
      // Kills attack array
    bool Load(char *aname, TToken &t);
      // Loads the character from the "CHAR.DEF" file

    char     name[RESNAMELEN]                = {}; // Name of character type (not individual name)
    char     groups[CHARGROUPLEN]            = {}; // Groups this character belongs to (not including char type)
    char     enemies[CHARGROUPLEN]           = {}; // Groups this character attacks (can include char types as well as groups)
    int32_t  objtype                          = -1; // Type data is associated with (-1 = default, -2 = no such type)
    int32_t  objclass                         = OBJCLASS_CHARACTER; // Class data is associated with
    int32_t  flags                            = 0;
    TCharAttackArray attacks;
    int32_t  damagemods[NUMDAMAGETYPES]      = {}; // Damage modifiers (100=100% of damage value)
    char     blocksounds[SOUNDLISTLEN]       = "block1,block2,block2";
    char     misssounds[SOUNDLISTLEN]        = {};
    int32_t  playerblockmin                  = 10;
    int32_t  playerblockstep                 = 5;
    int32_t  playerblockinc                  = 10;
    int32_t  combatrangemin                  = 128;
    int32_t  combatrangemax                  = 128 + 64;
    int32_t  maxattackrange                  = 32;
    int32_t  bleeder                         = 1;   // BLEEDER / BLEADER
    SColor   swipecolor                      = {};  // Sword swipe color
    uint8_t  swipefull                       = 0;   // SWIPEFULL
    char     bodytype[MAXANIMNAME]           = "normal"; // PLAYER: Type of body
    PSClassData classdata                    = nullptr; // PLAYER: Class of character
    int32_t  blockfreq                       = 10;
    int32_t  blockmin                        = 5;
    int32_t  blockmax                        = 15;
    int32_t  sightmin                        = 30;
    int32_t  sightmax                        = 100;
    int32_t  sightrange                      = 64 * 5;
    int32_t  sightangle                      = 64;
    int32_t  hearingmin                      = 10;
    int32_t  hearingmax                      = 50;
    int32_t  hearingrange                    = 64 * 5;
    int32_t  weapontype                      = 0;   // WT_HAND
    int32_t  weapondamage                    = 2;
    int32_t  armorvalue                      = 1;
    int32_t  defensemod                      = 0;
    int32_t  attackmod                       = 0;
    int32_t  minattackfreq                   = 100;
    int32_t  maxattackfreq                   = 250;
    int32_t  minmagicfreq                    = 100; // MAGICFREQ
    int32_t  maxmagicfreq                    = 250;
    int32_t  mana                            = 0;
    int32_t  fatigue                         = 25;
    int32_t  health                          = 25;
    int32_t  walkspeed                       = -1;
    int32_t  runspeed                        = -1;
    int32_t  sneakspeed                      = -1;
    int32_t  combatwalkspeed                 = -1;
    S3DPoint arrowpos                        = {-1, -1, -1}; // -1: set by name after the block (Load)
    int32_t  arrowspeed                      = 20;  // 20 units per tick for arrow speed
    int32_t  bowwait                         = 12;  // Half a second
    int32_t  bowaimspeed                     = 8;   // Pivot speed when aiming bow
    int32_t  numimpacts                      = 0;
    SCharAttackImpact impacts[MAXCHARIMPACTS] = {};
    int32_t  retreatat                       = 0;   // RETREATAT
    int32_t  retreatatmana                   = 0;   // RETREATATMANA
    int32_t  retreatfor                      = -1;  // RETREATFOR
    int32_t  runfatigue[2]                   = {1, 100}; // RUNFATIGUE (readers not traced)
    bool     noparalyze                      = false; // NOPARALYZE
    SCharAttachEffect attacheffects[MAXATTACHEFFECTS] = {};
    int32_t  poisonchance                    = 0;   // POISONCHANCE
    int32_t  labelheight                     = -1;  // LABELHEIGHT; -1: set by name after the block (Load)
};

typedef TPointerArray<SCharData, 16, 16> TCharDataArray;

_CLASSDEF(TRules)
class TRules
{
  private:
    bool initialized = false;       // Are we ready
    TClassDataArray classdata;      // Class data
    TCharDataArray chardata;        // Data array
    PSCharData def = nullptr;       // Default data object (used for ordinary chars)

  public:
    TRules() = default;
    // Trivial dtor: explicit Close() runs from ShutdownGlobals, so by the
    // time the global is destroyed the data arrays are already empty.
    // We can't walk chardata/classdata from the dtor without risking
    // cross-TU static-destruction-order bugs.
    ~TRules() = default;

    bool Initialize();
      // Initializes character data stuff (idempotent via `initialized`)
    void Close();
      // Kills all character data stuff (idempotent)
    int32_t GetNumClasses() { return classdata.NumItems(); }
      // Returns number of character classes in data manager
    PSClassData GetClass(char *classname);
      // Gets a character class based on the class name
    PSClassData GetClass(int32_t num) { return classdata[num]; }
      // Gets class data given class num
    int32_t GetNumCharData() { return chardata.NumItems(); }
      // Returns number of character data items in manager
    PSCharData GetCharData(int32_t num) { return chardata[num]; }
      // Returns a particular character data item
    PSCharData GetCharData(int32_t objtype, int32_t objclass);
      // Gets pointer to character data structure
    static constexpr int32_t kMaxSkillLevel = 30;
      // Skills stop rising here (retail TPlayer::AddSkillExp @ 0x0051ac90)
    int32_t SkillExpForLevel(int32_t level) const;
      // Experience a skill needs to reach 'level' (retail 0x0048cc90)
    static constexpr int32_t kMaxPlayerLevel = 30;
      // Players stop rising here (retail level-up @ 0x0051a630)
    int32_t ExpForLevel(int32_t level) const;
      // Experience a player needs to reach 'level' (retail 0x0048cc40)
    int32_t StatLevel(int32_t plyrstat, int32_t value) const { return statlevels.Get(plyrstat, value); }
      // The STATLEVEL percent of attribute 'plyrstat' (PLRSTAT_*) at 'value' (retail 0x0048cc20)
    void SetStatLevel(int32_t plyrstat, int32_t level, int32_t value) { statlevels.Set(plyrstat, level, value); }
      // One STATLEVEL entry (the retail A/B fixtures set the tables a case gives)
    const SItemData *GetItemData(int32_t objclass, const char *type) const;
      // The WEAPON.DEF / ARMOR.DEF entry of a weapon or armor type; null if none (retail 0x0048cb50)
    const std::vector<SItemData> &Weapons() const { return weapons; }
    const std::vector<SItemData> &Armors() const { return armors; }
      // Every WEAPON.DEF / ARMOR.DEF entry, in load order
    const SCharData *DefaultCharData() const { return def; }
      // The "Default" CHARACTER; null if none
    bool Load();
      // Loads the six rules files (retail 0x0048b990)
    void BindTypes();
      // Binds every CHARACTER to the object type of its name (retail 0x0048cab0)
    bool LoadFile(const char* fname);
      // Internal: parses one .def through the shared tag loop

  // Daytime...
    int32_t daylength = 0;              // Length of day in 100ths of a second
    int32_t twilight = 0, twilightsteps = 0; // Length of twilight period (morning/evening) in 100ths of a second

  // The values below are zero until Initialize, which gives the recovery
  // and poison rates, TOHIT* and AMMODATA their defaults before loading.

  // Health recovery values
    int32_t healthperlevel = 0, fatigueperlevel = 0, manaperlevel = 0;
    int32_t healthrecovval = 0, fatiguerecovval = 0, manarecovval = 0;
    int32_t healthrecovrate = 0, fatiguerecovrate = 0, manarecovrate = 0;

  // Poison damage values
    int32_t poisondamageval = 0;
    int32_t poisondamagerate = 0;

  // To-hit values (rules.def TOHIT*). The rules.def comment block gives the
  // formula: tohit = ((TOHITCENTER - Def) + Off + FacingBonus) - BlockBonus,
  // Off / Def adding level * TOHITRANGECHAR (characters) or TOHITRANGEPLYR.
    int32_t tohitcenter = 0;
    int32_t tohitrangechar = 0;
    int32_t tohitrangeplyr = 0;
    int32_t tohitblock = 0;
    int32_t tohitface = 0;

  // TOHITDAMAGE: five {MinValue, DamagePercent} rows (no defaults).
    struct SToHitDamage
    {
        int32_t minvalue = 0;
        int32_t damagepercent = 0;
    };
    static constexpr int32_t kToHitDamageRows = 5;
    std::array<SToHitDamage, kToHitDamageRows> tohitdamage{};

  // Stealth mode values
    int32_t maxstealth = 0, sneakstealth = 0, minstealth = 0;

  // AMMODATA (rules.def comment: base value, per monster level, per character
  // level, per skill level, minimum damage % per hit). The tag parses four
  // values; the fifth only ever holds its default.
    std::array<int32_t, 5> ammodata{};

  private:
    void BindType(const char *name, int32_t objtype);
      // One type of BindTypes: the CHARACTER of that name (retail 0x0048c930)

    PlayerStats::TStatLevels statlevels;    // STATLEVEL tables (rules.def)
    std::vector<SItemData> weapons;         // WEAPON.DEF entries
    std::vector<SItemData> armors;          // ARMOR.DEF entries
};

