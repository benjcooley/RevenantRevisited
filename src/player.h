// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      player.h - TPlayer module                        *
// *************************************************************************

#pragma once

#include "revenant.h"

#include "character.h"
#include "charstats.h"
#include "equip.h"
#include "multi.h"
#include "rules.h"
#include "weapon.h"

#include <array>
#include <string>
#include <vector>

// Equipment slot defines
#define NUM_EQ_SLOTS    11

#define EQ_HEAD         0
#define EQ_NECK         1
#define EQ_BODY         2
#define EQ_OFFHAND      3
#define EQ_PRIMEHAND    4
#define EQ_R_ACCESSORY  5
#define EQ_L_ACCESSORY  6
#define EQ_RANGEDWEAPON 7
#define EQ_AMMO         8
#define EQ_LEGS         9
#define EQ_FEET         10

#define PLYRSTATMOD_NORMAL          0
#define PLYRSTATMOD_HALFROUNDDOWN   1
#define PLYRSTATMOD_HALFROUNDUP     2

#define QSPELL_CONSTRUCT 0
#define QSPELL_1         1
#define QSPELL_2         2
#define QSPELL_3         3
#define QSPELL_4         4
#define QSPELL_NUM       5

// **************
// * Skill Tree *
// **************

_STRUCTDEF(SSkill)
struct SSkill
{
    char *name;             // Name of the skill
    bool children;          // Whether or not it has children
    bool lastchild;         // End of child list
    int32_t ancestor;           // Parent skill
    int32_t difficulty;         // Difficulty of learning
};

extern SSkill SkillTree[NUM_SKILLS];    // Master skill tree

// *******************************************
// * Player record (shipped-game player data) *
// *******************************************

// What the shipped TPlayer keeps beyond the 1998 source, as its saves
// carry it (objversion 15; docs/gameflow/forensics/SAVE_GAME.md §11.4-11.5).
// The multiplayer parts are empty in single player but round-trip so a save
// is never changed by passing through the port.

// A learned spell's talisman code. Retail stores it as char[6], NUL padded;
// the extra byte keeps the code a C string.
constexpr int32_t kSpellCodeBytes = 6;
using TSpellCode = std::array<char, kSpellCodeBytes + 1>;

// The HUD state retail writes into the player record: its globals at save
// time (DAT_0065d190, DAT_0065d1b8, DAT_0065d1bc, DAT_0065d19c).
struct SPlayerHudWords
{
    int32_t sidebarOpen = 0;    // DAT_0065d190: the sidebar is showing
    int32_t upperMode   = 0;    // DAT_0065d1b8: 0 equip, 1 stats, 2 spellbook
    int32_t lowerMode   = 0;    // DAT_0065d1bc: 0 inventory, 1 map, 2 spell composer
    int32_t unknown19c  = 0;    // DAT_0065d19c: unidentified (written, never read)
};

// The multiplayer team record (retail +0x490, 0x60 bytes). Set from the
// network as a whole and compared field by field (0x0051e4d0); players are
// grouped into teams by the two names (0x0051f370), and the team index
// groups the frag totals (0x0051f6b0).
struct SPlayerTeamRecord
{
    int32_t     id        = 0;  // +0x490
    std::string name;           // +0x494 char[50]
    std::string name2;          // +0x4c6 char[18]
    int32_t     value     = 0;  // +0x4d8
    int32_t     teamindex = 0;  // +0x4dc
};

// The player's explored automap (retail +0x314; Load 0x00529770, Save
// 0x00529830). One compressed explored-area mask per level visited
// (compressor 0x005295e0, decompressor 0x00529220), for the module named.
struct SAutoMapLevel
{
    int32_t              level = 0;
    std::vector<int16_t> mask;
};

struct SAutoMapRecord
{
    std::string                module;
    std::vector<SAutoMapLevel> levels;

    void Load(RTInputStream is);
    void Save(RTOutputStream os) const;
};

// ***********
// * TPlayer *
// ***********

_CLASSDEF(TPlayer)
class TPlayer : public TCharacter
{
  public:
    void ClearPlayer();
    void SetPlayerLevel(int32_t level);
      // Rebuild as a fresh level-'level' character of its class (`playerlevel`)

    TPlayer(TObjectImagery* newim);
    TPlayer(SObjectDef* def, TObjectImagery* newim);
    ~TPlayer();

    virtual bool GetZ(TSurface* surface) { if (!Editor) return false; return TCharacter::GetZ(surface); }
    virtual bool AlwaysOnTop() { if (!Editor) return false; return TCharacter::AlwaysOnTop(); }
    virtual bool Use(TObjectInstance* user, int32_t with = -1) { return false; }
    virtual int32_t CursorType(TObjectInstance* inst = nullptr) { return CURSOR_NONE; }
        // These functions make sure the player never clicks on themselves

    virtual void Pulse();
    virtual uint32_t Move();

    virtual void AI() { }

    virtual void Damage(int32_t damage, int32_t type = DAMAGE_UNDEFINED);
        // Apply damage to the player

//  virtual int32_t SwingRange();
//  virtual int32_t ThrustRange();
        // These are computed from the currently wielded weapon

    TObjectInstance* PrimeHand() { return equipment[EQ_PRIMEHAND]; }
    TObjectInstance* OffHand() { return equipment[EQ_OFFHAND]; }
    TObjectInstance* Head() { return equipment[EQ_HEAD]; }
    TObjectInstance* Body() { return equipment[EQ_BODY]; }
    TObjectInstance* Neck() { return equipment[EQ_NECK]; }
    TObjectInstance* RAccessory() { return equipment[EQ_R_ACCESSORY]; }
    TObjectInstance* LAccessory() { return equipment[EQ_L_ACCESSORY]; }
    TObjectInstance* RangedWeapon() { return equipment[EQ_RANGEDWEAPON]; }
    TObjectInstance* Legs() { return equipment[EQ_LEGS]; }
    TObjectInstance* Feet() { return equipment[EQ_FEET]; }

    void RefreshEquip();
        // Sets up all equipment pointers, and refreshes equipment pane if necessary
    bool CanEquip(TObjectInstance* oi, int32_t slot);
        // Returns true if player can be equiped by the given object
    bool Equip(TObjectInstance* oi, int32_t slot);
        // Set up equipment pointers from objects in player's inventory
    TObjectInstance* GetEquip(int32_t slot) { return equipment[slot]; }
        // Returns object pointer to equipment in the given slot

    // Player stat access
    int32_t PlyrStat(int32_t plyrstat) { return GetObjStat(plyrstat + PLRSTAT_FIRST); }
      // Returns the value of a given physical attribute for player
    int32_t PlyrStatMod(int32_t plyrstat, int32_t flags = PLYRSTATMOD_NORMAL);
      // Returns the modifier value for the given player stat
    int32_t PlyrStatPcnt(int32_t plyrstat, int32_t inc = 10) { return PlyrStatMod(plyrstat) * inc; }
      // Returns a percentage modifier value for a physical attribute
    int32_t PlyrStatSkillMod(int32_t skill);
      // Returns a percentage modifier value for a physical attribute

    // Skill access
    int32_t Skill(int32_t skillnum) { return GetObjStat(skillnum + SK_FIRST); }
      // Get skill
    int32_t SkillExp(int32_t skillnum) { return GetObjStat(skillnum + SKE_FIRST); }
      // Get skill experience
    int32_t SkillMod(int32_t skillnum)
        { return chardata->classdata->skillmods[skillnum] + PlyrStatSkillMod(skillnum); }
    int32_t SkillPcnt(int32_t skillnum, int32_t pcnt = 5) 
        { return (Skill(skillnum) + 
                  chardata->classdata->skillmods[skillnum] +
                  PlyrStatSkillMod(skillnum)) * pcnt; }
      // Returns total of character skill level plus class skill modifier plus stat skill
      // modifierUses exponential skill value to get linear power.  Min is minimum power,
      // base is the base skill value which must double for each increase 'inc' in 
      // power.  (i.e. min=5, base=5, inc=5... skill 0=5, 5=10, 10=15, 20=20, 40=25, 80=30, etc.)
    void SetSkill(int32_t skillnum, int32_t v) { SetObjStat(skillnum, v); }
      // Sets the skill value
    void AddSkillExp(int32_t skillnum, int32_t exp);
      // Adds skill experience; reaching the next threshold raises the skill a level
    int32_t WeaponSkill(int32_t weapontype) { return Skill(SK_WEAPONSKILLS + weapontype); }

    // Spell casting
    bool HasTalismans(char *talismans);
      // Player has talismans for spell
    char *GetQuickSpell(int32_t button);
      // Gets the talisman list for the given quickspell
    void SetQuickSpell(int32_t button, char *talismans);
      // Sets the quickspell button
    bool InvokeQuickSpell(int32_t button);
      // Invokes the given quickspell for the player

    // Info functions
    virtual int32_t GetResistance(int32_t type);
      // Get character's resistance to the given damage type

    // Resolve functions
    virtual int32_t ResolveCombat(PTActionBlock ab, int32_t bits);

  // Streaming functions
    int32_t ObjVersion() override { return 15; }
        // The shipped game's player layout (SAVE_GAME.md §11.4)
    void Load(RTInputStream is, int32_t version, int32_t objversion) override;
        // Loads object data from the sector
    void Save(RTOutputStream os) override;
        // Saves object data to the sector
    void LoadInventory(RTInputStream is, int32_t version, uint32_t streamflags) override;
        // Loads the inventory, re-equips, and keeps health/fatigue/mana

  // Shipped-game player record (SAVE_GAME.md §11.4)
    [[nodiscard]] int32_t PlayerState() const { return playerstate; }
    void SetPlayerState(int32_t newstate);
        // Player state bits (retail +0x36c)
    [[nodiscard]] const SPlayerHudWords& HudWords() const { return hudwords; }
    void SetHudWords(const SPlayerHudWords& words) { hudwords = words; }
        // The HUD state written into a save
    [[nodiscard]] int32_t NumKnownSpells() const { return (int32_t)knownspells.size(); }
    [[nodiscard]] const char* KnownSpell(int32_t i) const { return knownspells[i].data(); }
        // Talisman codes of the spells the player has learned (retail +0x2ec), in the order learned
    bool LearnSpell(const char* talismans);
        // Adds a spell's talisman code; false when it is already known

    // Cheat - ride that hog
    void GetOnYerHog();

    // Overloaded Health, Fatigue, and Mana stats to update screen bars
    virtual void SetHealth(int32_t v);
    virtual void SetFatigue(int32_t v);
    virtual void SetMana(int32_t v);

    // Miscellaneous player values
    OBJSTATFUNC(Level)
    OBJSTATFUNC(Exp)
    OBJSTAT(NextExp)
    OBJSTAT(AttackLevel)
    OBJSTAT(HealthPct)
    OBJSTAT(ManaPct)
    OBJSTAT(FatiguePct)
    OBJSTAT(MaxHealthFlat)
    OBJSTAT(MaxManaFlat)
    OBJSTAT(MaxFatigueFlat)
    OBJSTAT(MaxHealthPct)
    OBJSTAT(MaxManaPct)
    OBJSTAT(MaxFatiguePct)
    OBJSTAT(ACBonus)
    OBJSTAT(ManaCostPct)
    OBJSTAT(SpellDamageInc)
    OBJSTAT(EdgeBonus)

    // Player stats
    OBJSTATFUNC(Strn)
    OBJSTATFUNC(Cons)
    OBJSTATFUNC(Agil)
    OBJSTATFUNC(Rflx)
    OBJSTATFUNC(Mind)
    OBJSTATFUNC(Luck)

    // Player skills
    OBJSTAT(Attack)
    OBJSTAT(Defense)
    OBJSTAT(Invoke)
    OBJSTAT(Hands)
    OBJSTAT(Knife)
    OBJSTAT(Sword)
    OBJSTAT(Bludgeon)
    OBJSTAT(Axes)
    OBJSTAT(Bows)
    OBJSTAT(Stealth)
    OBJSTAT(LockPick)

    // Player skill experience
    OBJSTAT(AttackExp)
    OBJSTAT(DefenseExp)
    OBJSTAT(InvokeExp)
    OBJSTAT(HandsExp)
    OBJSTAT(KnifeExp)
    OBJSTAT(SwordExp)
    OBJSTAT(BludgeonExp)
    OBJSTAT(AxesExp)
    OBJSTAT(BowsExp)
    OBJSTAT(StealthExp)
    OBJSTAT(LockPickExp)

    // Experience for the next skill level, and the skill caps
    OBJSTAT(AttackNxtExp)
    OBJSTAT(DefenseNxtExp)
    OBJSTAT(InvokeNxtExp)
    OBJSTAT(HandsNxtExp)
    OBJSTAT(KnifeNxtExp)
    OBJSTAT(SwordNxtExp)
    OBJSTAT(BludgeonNxtExp)
    OBJSTAT(AxesNxtExp)
    OBJSTAT(BowsNxtExp)
    OBJSTAT(StealthNxtExp)
    OBJSTAT(LockPickNxtExp)
    OBJSTAT(AttackCap)
    OBJSTAT(DefenseCap)
    OBJSTAT(InvokeCap)
    OBJSTAT(HandsCap)
    OBJSTAT(KnifeCap)
    OBJSTAT(SwordCap)
    OBJSTAT(BludgeonCap)
    OBJSTAT(AxesCap)
    OBJSTAT(BowsCap)
    OBJSTAT(StealthCap)
    OBJSTAT(LockPickCap)

   // Calculated stats
    virtual int32_t MaxHealth() 
        { return Rules.healthperlevel * Level() * 
            (100 + chardata->classdata->healthmod + PlyrStatPcnt(PLRSTAT_CONS)) / 100; }
      // Calculates maximum health as 10 * level * (100% + classmod% + constitutionmod%)
    virtual int32_t MaxFatigue() 
        { return Rules.fatigueperlevel * Level() * 
            (100 + chardata->classdata->fatiguemod + PlyrStatPcnt(PLRSTAT_CONS)) / 100; }
      // Calculates maximum health as 10 * level * (100% + classmod% + constitutionmod%)
    virtual int32_t MaxMana() 
        { return Rules.manaperlevel * Level() * 
            (100 + chardata->classdata->manamod + PlyrStatPcnt(PLRSTAT_MIND)) / 100; }
      // Calculates maximum health as 10 * level * (100% + classmod% + constitutionmod%)
//  virtual int32_t BlockPcnt() { return SkillPcnt(SK_DEFENSE) + 10; }
      // Returns percentage of time character will block an attack
    virtual int32_t ArmorValue() { if (Body()) return Body()->GetStat("Protection"); else return 0; }
      // Return armor value
    virtual int32_t WeaponType() { if (PrimeHand() && PrimeHand()->ObjClass() == OBJCLASS_WEAPON)
        return ((PTWeapon)PrimeHand())->Type();
        else return WT_HAND; }
      // Returns the type of weapon being used
//  virtual int32_t WeaponDamage() { if (PrimeHand()) return PrimeHand()->GetStat("Damage"); else return chardata->handdamage; }
      // Returns the current weapon's damage value
    virtual int32_t StealthMod() { 
        return SkillPcnt(SK_STEALTH, 10) + 
            (Body()?Body()->GetStat("Stealth"):0) + 
            (Feet()?Feet()->GetStat("Stealth"):0);  }
      // Get stealth by combining clothing stealth value and stealth stat
    virtual char *BodyType() { return chardata->bodytype; }
      // Returns the player's body type for equipment replacement.
      // The standard body types are "normal", "large", "small", "dwarf", "lithe".
      // Normal - Locke - Characters with standard strong builds (warriors, etc.)
      // Large - Bayne - Characters with large/giant builds
      // Small - xxx - Characters with smaller male builds (thieves, rouges, etc.)
      // Dwarf - Navarro - Halflings, dwarves, etc.
      // Lithe - Morgana - Sexy female characters (longer legs, hips, breasts, etc.)
      // NOTE: Not all armor or clothing needs to fit all body types.
    virtual char *GetCombatRoot(TObjectInstance* oi = nullptr);
      // Returns combat root given the weapon oi or current weapon if oi is nullptr
    virtual char *GetBowRoot(TObjectInstance* oi = nullptr); 
      // Returns bow root given the bow 'oi' or current bow if oi is nullptr

  private:
    TObjectInstance* equipment[NUM_EQ_SLOTS];       // Player's weapons and armor
    char quickspells[QSPELL_NUM][MAXTALISMANLEN];   // Quickspells (0-construction, 1-4 quick buttons)
    bool OnTheHog;                                  // hog cheat
    int32_t deathcountdown = 0xc0;                  // frames left before the death screen (retail +0x39c)

  // Shipped-game player record, in retail's save order (SAVE_GAME.md §11.4)
    std::vector<TSpellCode> knownspells;            // +0x2ec: talisman codes learned
    SPlayerHudWords hudwords;                       // HUD state as last saved/loaded
    std::array<int32_t, 3> levelupstats{};          // +0x360: player-stat indices the level-up messages use
    SPlayerTeamRecord team;                         // +0x490: multiplayer team record
    std::string modulename;                         // +0x4f0: the module the player is in
    int32_t playerstate = 1;                        // +0x36c: player state bits (SetPlayerState 0x0051d680)
    int32_t statetime = 0;                          // +0x370: game time of the last state change
    std::array<std::string, 4> profile;             // +0x378, +0x570, +0x590, +0x5d0: multiplayer lobby text
    std::array<int32_t, 4> frags{};                 // +0x650..+0x65c: multiplayer kill counts
    SAutoMapRecord automap;                         // +0x314: explored automap
};

DEFINE_BUILDER("Player", TPlayer)

typedef TPointerArray<TPlayer, 4, 4> TPlayerArray;

// ******************
// * TPlayerManager *
// ******************

// The player manager holds the current game player list.  This list can be initialized
// before the beginning of PlayScreen by setting up the players in the network player
// setup screen, or by loading a saved game, etc.  Players are not automatically added
// to the manager when they are created, but are automatically removed when they are
// deleted. 
//
// Net games may use the player manager id# for the player id in the game.
//
// Also, since players are marked as NONMAP objects in the game, they will continue to
// exist after all maps have been deleted.  This means that, to delete all players, you
// must Close(), or Clear() the PlayerManager for the players to be deallocated.

_CLASSDEF(TPlayerManager)
class TPlayerManager
{
  public:
    bool Initialize();
      // Initialize the player manager
    void Close();
      // Closes the player manager
    void Clear();
      // Deletes all players
    int32_t NumPlayers() { return players.NumItems(); }
      // Returns number of areas
    TPlayer* GetPlayer(int32_t playernum) { return players[playernum]; }
      // Returns the control entry for the given index
    void SetMainPlayer(int32_t playernum);
      // Sets the main player for the game
    void SetMainPlayer(TPlayer* player);
      // Sets the main player for the game
    int32_t GetMainPlayerNum() { return mainplayernum; }
      // Returns the main player number
    TPlayer* GetMainPlayer() { return Player; }
      // Returns the game's main player (same as just accessing the Player global)
    int32_t AddPlayer(TPlayer* player);
      // Adds another player and returns index into player array
    void RemovePlayer(int32_t removenum, bool collapse = true);
      // Remove a player, if collapse is true, collapses player list so no empty slot is left
      // Use collapse when setting up a game, and no collapse when in game so that player id
      // nums remain the same for network messages.
    void RemovePlayer(TPlayer* removeplayer, bool collapse = true);
      // Remove a player (given player pointer)

  private:
    TPlayerArray players;
    int32_t mainplayernum = -1;
    bool    initialized   = false;
};
