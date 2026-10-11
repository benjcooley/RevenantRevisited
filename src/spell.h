// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                           Spell.h - Spell                             *
// *************************************************************************

#pragma once

#include "revenant.h"

#include "object.h"

#include <vector>

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

// The "abracadabra" text cheat (retail 0x0066810c): a cast costs no mana and
// can't fail its skill roll, and ManaDrain leaves a player's mana alone.
// Toggled by the cheat word at the text-bar prompt (cheats.cpp).
extern bool MagicCheat;

class TCharacter;
class TPlayer;

// Retail TSpell, 0x150 bytes (vtable 0x005b99c0); offsets noted.
_CLASSDEF(TSpell)
class TSpell
{
  public:
    // REVSYNC: TSpell::TSpell @ 0x0053f090
    TSpell(TObjectInstance* invoke, TObjectInstance* *targ, int32_t numtargs, S3DPoint* sourcepos, PSSpellData dat, PSSpellVariant var, PTSpell mtr = nullptr);
    TSpell()
    { invoker = nullptr; effect.Clear();
      for (auto& target : targets) target = nullptr;
      targetnum = 0; wait = -1;
      timer = 0; master = nullptr; spell = nullptr; variant = nullptr; frame = 0;
      magic_offense = 0; magic_defense = 0;
      source.x = source.y = source.z = -1; }
    virtual ~TSpell();

    // The registry name of the spell's class (CONTROLDATA's, or "Spell").
    [[nodiscard]] virtual const char* ClassName() const { return "Spell"; }

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
    void SetData(PSSpellData dat, PSSpellVariant var) { spell = dat; variant = var; }
      // The cast sets them again after building the spell (retail +0x11c / +0x120)

    // The state a spell runs on, as retail lays it out (the A/B dumps read it).
    [[nodiscard]] int32_t TimerValue() const { return timer; }
    [[nodiscard]] int32_t WaitValue() const { return wait; }
    [[nodiscard]] int32_t FrameValue() const { return frame; }
    [[nodiscard]] TSpell* Master() const { return master; }
    [[nodiscard]] const S3DPoint& Source() const { return source; }

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

    // REVSYNC: TSpell::ManaDrain @ 0x0053f680 -- the invoker pays the
    // variant's mana (a player less ManaCostPct percent; nothing under the
    // cheat), kept within MaxMana
    void ManaDrain();

  protected:
    TObjectInstance* invoker;               // +0x04 Object that invoked the spell
    TSafeRef<TObjectInstance> invoker_ref;
    TSafeRef<TObjectInstance> target_refs[MAXSPELLTARGETS];
    TSafeRef<TObjectInstance> effect;       // +0x08 the last effect made; weak identity, reaping/reuse cannot dangle

    int32_t targetnum;                      // +0x0c Number of targets in target list
    TObjectInstance* targets[MAXSPELLTARGETS]; // +0x10 Spell's target list

    int32_t timer;                          // +0x110 -1 until Kill; the spell ends at 0
    int32_t frame;                          // +0x114 Pulses so far (the Strike class's)
    PTSpell master;                         // +0x118 Master spell object (if slave)

    PSSpellData spell;                      // +0x11c Data for spell
    PSSpellVariant variant;                 // +0x120 Variant
    int32_t wait;                           // +0x124 DELAY: pulses before the effect starts
    S3DPoint source;                        // +0x128 Source of spell relative to char pos (-1,-1,-1 if not used)

    int32_t magic_defense;                  // +0x134
    int32_t magic_offense;                  // +0x138
};

// The CONTROLDATA "Strike" class (retail 0x154 bytes, vtable 0x005b9bdc,
// built by 0x00542290): a TSpell plus the target it strikes (+0x150).
// REVSYNC: its Timer 0x00540eb0, Pulse 0x00540d70 and Kill 0x00542340 are
// not ported yet; TSpell's run.
class TStrikeSpell : public TSpell
{
  public:
    TStrikeSpell(TObjectInstance* invoke, TObjectInstance** targ, int32_t numtargs, S3DPoint* sourcepos,
                 SSpellData* dat, SSpellVariant* var, TSpell* mtr)
        : TSpell(invoke, targ, numtargs, sourcepos, dat, var, mtr) {}

    [[nodiscard]] const char* ClassName() const override { return "Strike"; }
    [[nodiscard]] TObjectInstance* StrikeTarget() const { return striketarget; }

  protected:
    TObjectInstance* striketarget = nullptr;    // +0x150 the target struck
};

// The spell classes, by the name CONTROLDATA gives (retail's registry
// 0x00670220 / count 0x0067021c, filled at static init: "Spell" by
// 0x00540b80, "Strike" by 0x00540ba0).
struct SSpellClass
{
    const char* name;
    TSpell* (*create)(TObjectInstance* invoker, TObjectInstance** targets, int32_t numtargs, S3DPoint* sourcepos,
                      SSpellData* dat, SSpellVariant* var, TSpell* master);
};
[[nodiscard]] const SSpellClass* FindSpellClass(const char* name);
  // The class of that name, case-blind; nullptr if none

// REVSYNC: AreaDamage @ 0x004de3c0 -- every living enemy character between
// `minradius` and `radius` of `pos` and not in an impact takes
// random(min, max) of `damagetype` (a player attacker's SpellDamageInc
// added, a player target's DmgResMagical taken off), knocked back from
// `pos` first; a player attacker is offered each kill. FireBall's blast and
// the area spells' damage.
void AreaDamage(TObjectInstance* attacker, const S3DPoint& pos, int32_t radius, int32_t mindamage,
                int32_t maxdamage, int32_t damagetype, int32_t minradius);

typedef TPointerArray<TSpell, 32, 16> TSpellArray;

// *********************************************
// * TSpellManager - Contains a list of spells *
// *********************************************

// Retail: at TCharacter+0x170, the spell array (count +0x00, items +0x10)
// and the cooldown (+0x14).
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

    // REVSYNC: TSpellManager::CastByName @ 0x0053f920
    bool CastByName(char* name, TObjectInstance* invoker, TObjectInstance* *targets, int32_t numtargs, S3DPoint* sourcepos = nullptr, PTSpell mst = nullptr);
      // cast a spell by its name, returns success or failure
    // REVSYNC: TSpellManager::CastByTalismans @ 0x0053fe80
    bool CastByTalismans(char* talismans, TObjectInstance* invoker, TObjectInstance* *targets, int32_t numtargs, S3DPoint* sourcepos = nullptr, PTSpell mst = nullptr);
      // cast a spell by its talismans, returns success or failure

    int32_t GetDefense();
    int32_t GetOffense();
    int32_t GetSpellCount(char* spell);

    // The cooldown and the spells, as the A/B fixtures set and read them.
    [[nodiscard]] int32_t Wait() const { return wait; }
    void SetWait(int32_t w) { wait = w; }
    [[nodiscard]] int32_t NumSpells() const { return spells.NumItems(); }
    [[nodiscard]] TSpell* GetSpell(int32_t i) { return spells.Used(i) ? spells[i] : nullptr; }

    // Retail A/B fixtures only: when set, the objects around a caster that
    // a buff cast looks through (retail's map iterator 0x0044cf10 /
    // 0x0044d080 over OBJSET_ANIMATE), instead of the map.
    using NearbySeam = std::vector<TObjectInstance*> (*)(TObjectInstance* center);
    static inline NearbySeam nearbySeam = nullptr;
    // Likewise, when set, it answers CastByName (retail 0x0053f920).
    using CastByNameSeam = bool (*)(TSpellManager* self, const char* name, TObjectInstance* invoker,
                                    TObjectInstance** targets, int32_t numtargs, const S3DPoint* sourcepos,
                                    const TSpell* master);
    static inline CastByNameSeam castByNameSeam = nullptr;

  private:
    bool CanAfford(TCharacter* caster, const SSpellVariant& variant, bool byname) const;
    bool SkillRollPasses(TCharacter* caster, const SSpellVariant& variant, int32_t roll, bool byname) const;
    static void AwardInvokeExp(TPlayer* player, const SSpellVariant& variant);
    static void EndOtherBuffs(TCharacter* caster);
    void Commit(TSpell* spell, TCharacter* caster, TObjectInstance** targets, SSpellData* spelldata,
                SSpellVariant* variant);
};
