// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   objectstates.h - the per-class instance state records              *
// *************************************************************************
//
// One record per object class that has state of its own beyond the base
// (objectdef.h SObjectState): the fields that class's retail stream
// carries, as reflected props, each class in its own hundred of ids so a
// base can grow without colliding (REFL_IDS). The class's def
// (characterdef.h) applies a record to a live object and captures one
// from it; the legacy codec reads retail bytes into a record and writes a
// record to retail bytes; the packed codec and the DEF text move them
// generically. This file is the serialization schema of the object
// classes: what a sector or a save stores for each.
//
// *************************************************************************

#pragma once

#include "game/object/objectdef.h"

// TComplexObject: the root action block the object returns to
// (TComplexObject::Load @ 0x004db930).
struct SComplexObjectState : SObjectState
{
    REFL_IDS(100, 199)
    PROP(100, "Root action (ACTION)")                int32_t     rootaction = 0;
    PROP(101, "Root state name; empty is the type's") std::string rootname;

    [[nodiscard]] std::unique_ptr<SObjectState> Clone() const override
        { return std::make_unique<SComplexObjectState>(*this); }
};

// TCharacter: recovery clocks, poison, the teleport point
// (TCharacter::Load @ 0x004d4eb0).
struct SCharacterState : SComplexObjectState
{
    REFL_IDS(200, 299)
    PROP(200, "Game time health last recovered")     int32_t  lasthealthrecov  = 0;
    PROP(201, "Game time fatigue last recovered")    int32_t  lastfatiguerecov = 0;
    PROP(202, "Game time mana last recovered")       int32_t  lastmanarecov    = 0;
    PROP(203, "Last poison charge; -1 not poisoned") int32_t  lastpoisondamage = -1;
    PROP(204, "Teleport point; -1 none")             S3DPoint teleport_position = {-1, -1, -1};
    PROP(205, "Teleport level; -1 none")             int32_t  teleport_level   = -1;

    [[nodiscard]] std::unique_ptr<SObjectState> Clone() const override
        { return std::make_unique<SCharacterState>(*this); }
};

// One level of a player's explored automap: the grid as retail packs it.
struct SAutoMapLevelState
{
    REFL_IDS(1, 9)
    PROP(1, "Map level")                             int32_t              level = 0;
    PROP(2, "The visited grid, words little-endian") std::vector<uint8_t> mask;
};

// A player's explored automap (TPlayer::Load @ 0x0051b960, the record).
struct SAutoMapRecordState
{
    REFL_IDS(1, 9)
    PROP(1, "The module it belongs to")              std::string                     module;
    PROP(2, "The explored levels")                   std::vector<SAutoMapLevelState> levels;
};

// TPlayer: the shipped-game player record, in retail's save order
// (SAVE_GAME.md §11.4, objversion 15).
struct SPlayerState : SCharacterState
{
    REFL_IDS(800, 899)
    PROP(800, "Quick spells: the composer, then the four buttons") std::vector<std::string> quickspells;
    PROP(801, "Talisman codes learned")                            std::vector<std::string> knownspells;
    PROP(802, "HUD: the sidebar is showing")                       int32_t sidebarOpen = 0;
    PROP(803, "HUD: 0 equip, 1 stats, 2 spellbook")                int32_t upperMode   = 0;
    PROP(804, "HUD: 0 inventory, 1 map, 2 spell composer")         int32_t lowerMode   = 0;
    PROP(805, "HUD: the bottom drawer is open")                    int32_t drawerOpen  = 0;
    PROP(806, "Player-stat indices the level-up messages use")     std::vector<int32_t> levelupstats;
    PROP(807, "Multiplayer team id")                               int32_t     teamid     = 0;
    PROP(808, "Multiplayer team name")                             std::string teamname;
    PROP(809, "Multiplayer team name, second")                     std::string teamname2;
    PROP(810, "Multiplayer team value")                            int32_t     teamvalue  = 0;
    PROP(811, "Multiplayer team index")                            int32_t     teamindex  = 0;
    PROP(812, "The module the player is in")                       std::string modulename;
    PROP(813, "Player state bits")                                 int32_t playerstate = 1;
    PROP(814, "The play clock, game time in 1/100 s")              int32_t statetime   = 0;
    PROP(815, "Multiplayer lobby text")                            std::vector<std::string> profile;
    PROP(816, "Multiplayer kill counts")                           std::vector<int32_t> frags;
    PROP(817, "The explored automap")                              SAutoMapRecordState automap;

    [[nodiscard]] std::unique_ptr<SObjectState> Clone() const override
        { return std::make_unique<SPlayerState>(*this); }
};

// TExit (and TLever, TPressPlate, TUpBlock): the exit flags.
struct SExitState : SObjectState
{
    REFL_IDS(300, 399)
    PROP(300, "EXIT_* flags")                        int32_t exitflags = 0;

    [[nodiscard]] std::unique_ptr<SObjectState> Clone() const override
        { return std::make_unique<SExitState>(*this); }
};

// TScroll: the text written on it.
struct SScrollState : SObjectState
{
    REFL_IDS(400, 499)
    PROP(400, "The scroll's text")                   std::string text;

    [[nodiscard]] std::unique_ptr<SObjectState> Clone() const override
        { return std::make_unique<SScrollState>(*this); }
};

// TWeapon: poison on the blade.
struct SWeaponState : SObjectState
{
    REFL_IDS(500, 599)
    PROP(500, "Poison on the blade")                 int32_t poison = 0;

    [[nodiscard]] std::unique_ptr<SObjectState> Clone() const override
        { return std::make_unique<SWeaponState>(*this); }
};

// TDripEffect: the drip's ripple.
struct SDripState : SObjectState
{
    REFL_IDS(600, 699)
    PROP(600, "Ripple size")                         int32_t ripplesize = 64;
    PROP(601, "Drop height")                         int32_t height     = 128;
    PROP(602, "Period in ticks")                     int32_t period     = 48;

    [[nodiscard]] std::unique_ptr<SObjectState> Clone() const override
        { return std::make_unique<SDripState>(*this); }
};

// One of a monster generator's slots.
struct SMonsterGenSlotState
{
    REFL_IDS(1, 9)
    PROP(1, "The CHARACTER type it makes")           std::string type;
    PROP(2, "Game time between monsters, 1/100 s")   int32_t frequency = 0;
    PROP(3, "Treasure tier")                         int32_t treasure  = 0;
    PROP(4, "Game time its clock started; 0 not yet") int32_t start    = 0;
};

// TMonsterGen: the slots and the cap (TMonsterGen::Load @ 0x00516e00).
struct SMonsterGenState : SObjectState
{
    REFL_IDS(700, 799)
    PROP(700, "The used slots")                      std::vector<SMonsterGenSlotState> slots;
    PROP(701, "Monsters alive at once")              int32_t maxmonsters = 2;

    [[nodiscard]] std::unique_ptr<SObjectState> Clone() const override
        { return std::make_unique<SMonsterGenState>(*this); }
};
