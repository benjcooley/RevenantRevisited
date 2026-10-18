// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 monstergen.cpp - TMonsterGen object                   *
// *************************************************************************
//
// Retail behaviour: docs/playtest/GRIND.md §3.1 (the generator) and §4.1
// (what its monsters carry). Retail built the class into Money.cpp.
//
// Not ported, as the port has no multiplayer: a client never pulses or
// spawns; a server takes "off screen" as no player within 0x500 (0x00516c50)
// and throttles spawns through its network object (0x005877a0, 0x00587880),
// which single player passes untouched.

#include "game/object/monstergen.h"

#include "game/imagery/imagery.h"
#include "engine/core/logging.h"
#include "game/world/mappane.h"
#include "game/ui/playscreen.h"
#include "game/app/revutils.h"
#include "engine/core/stream.h"

#include <algorithm>
#include <iterator>

REGISTER_BUILDER(TMonsterGen)

namespace {

// REVSYNC: 0x00517050 -- the longest frequency; the test is unsigned, so a
// negative one becomes this too.
constexpr int32_t kMaxFrequency = 19999;

// REVSYNC: 0x00516d91 -- what a slot counts against the cap: characters
// (FindObjectsInRange 0x00452060, class and set "character") within 300.
constexpr int32_t kCountRadius = 300;
constexpr int32_t kCountMax    = 10;

// REVSYNC: 0x00517240 -- 20 tries at a spot 50-150 units away in a random
// direction, 10 above the generator, which passes when no walk cell within
// 16 of it is a hole and no two neighbouring ones differ by more than 4.
constexpr int32_t kSpawnTries   = 20;
constexpr int32_t kSpawnMinDist = 50;
constexpr int32_t kSpawnMaxDist = 150;
constexpr int32_t kSpawnRise    = 10;
constexpr int32_t kSpawnRadius  = 16;
constexpr int32_t kSpawnMaxStep = 4;

// The spawn kit's tables (0x005e1eb0..0x005e20a0).
constexpr const char* kFoods[] = {
    "Meat", "Cheese", "Apple", "Bread", "Grapes", "Fish", "Watermelon", "Celery",
    "Banana", "Carrot", "Chocolate", "Drum Stick", "Lettuce", "Muffin", "Orange", "Sausage",
};
constexpr const char* kLesserPotions[]    = { "Lesser Healing", "Lesser Mana", "Lesser Restoration" };
constexpr const char* kGreaterPotions[]   = { "Greater Healing", "Greater Mana", "Greater Restoration" };
constexpr const char* kEnchantedPotions[] = { "Enchanted Healing", "Enchanted Mana", "Enchanted Restoration" };

// A treasure tier: the chances in percent of each potion grade (tried in
// turn until one is given) and of Cure Poison, and the gold range.
struct STreasureTier
{
    int32_t lesser    = 0;
    int32_t greater   = 0;
    int32_t enchanted = 0;
    int32_t cure      = 0;
    int32_t goldmin   = 0;
    int32_t goldmax   = 0;
};

// By tier, 0-8 (0x005e1fb4, 0x005e1fd8, 0x005e1ffc, 0x005e2020, 0x005e2044,
// 0x005e2068). Tier 0 gets nothing whatever its row says; shipped slots use
// 1-8.
constexpr STreasureTier kTreasureTiers[] = {
    {  0,  0,  0,  2,   0,  25 },
    {  5,  0,  0,  5,   0,  50 },
    {  7,  5,  0,  7,  25,  75 },
    { 10,  7,  0, 10,  50, 125 },
    {  7, 10,  5, 10,  50, 175 },
    {  5, 10,  7, 10,  75, 200 },
    {  0,  7, 10, 10, 100, 300 },
    {  0,  5, 10, 10, 100, 500 },
    {  0,  0,  0,  0,   0,   0 },
};

// REVSYNC: 0x005169b0 -- the kit a generator's monster is made with: a Pouch
// (7%), a food (30%), a potion of the first grade whose roll passes (the
// kind at random), "Cure Posion" and the tier's gold. The game asks for Cure
// Poison misspelled, which names no type, so it never comes. Retail raised
// its network layer's quiet flag (0x00676e5d) around the adds; single
// player never reads it.
void GiveSpawnTreasure(TObjectInstance* monster, int32_t tier)
{
    if (tier <= 0)
        return;

    if (random(1, 100) <= 7)
        monster->AddToInventory("Pouch");
    if (random(1, 100) <= 30)
        monster->AddToInventory(kFoods[random(0, int32_t(std::size(kFoods)) - 1)]);

    // REVSYNC-DIVERGENCE: retail read the tier's chances and gold past the
    // end of its tables for a tier above 8. No shipped slot has one; only a
    // typed addmonstertype can.
    if (tier >= int32_t(std::size(kTreasureTiers)))
    {
        log_warn("[monstergen] treasure tier %d is past retail's tables (0-8): no potion or gold", tier);
        return;
    }
    const STreasureTier& t = kTreasureTiers[tier];

    if (random(1, 100) <= t.lesser)
        monster->AddToInventory(kLesserPotions[random(0, 2)]);
    else if (random(1, 100) <= t.greater)
        monster->AddToInventory(kGreaterPotions[random(0, 2)]);
    else if (random(1, 100) <= t.enchanted)
        monster->AddToInventory(kEnchantedPotions[random(0, 2)]);

    if (random(1, 100) <= t.cure)
        monster->AddToInventory("Cure Posion");

    monster->AddToInventory("Gold", random(t.goldmin, t.goldmax));
}

} // namespace

// REVSYNC: TMonsterGen::OnScreen @ 0x00517660 / OffScreen @ 0x00517640 kept
// +0xf0 as retail's animate pass (TMapPane::AnimateObjects 0x00458750) told
// the generator on each drawn frame: on screen while its rect met the view
// grown by 192 pixels. In the port OnScreen means "about to be drawn" (the
// renderer calls it for every animated object it builds), so the generator
// asks the map pane for the pass's verdict as its pulse starts. The pass ran
// after the last tick moved the view, which is where the view still is.
void TMonsterGen::Pulse()
{
    const std::optional<bool> onscreen = MapPane.AnimatePassOnScreen(this);
    if (onscreen && *onscreen == offscreen)
    {
        offscreen = !*onscreen;
        log_debug("[monstergen] %s at (%d, %d) is %s screen", GetName(), IntPos(*this).x, IntPos(*this).y, offscreen ? "off" : "on");
    }

    // The "dummies" cheat (0x00668110) stops generators with the monsters.
    if (NoAI || !offscreen)
        return;

    for (int32_t i = 0; i < kMaxTypes; i++)
    {
        if (!slots[i])
            continue;
        SSlot& slot = *slots[i];

        // A clock not started yet, or one that ran on past twice its
        // frequency while nothing watched it (the generator on screen, its
        // sector away), starts over.
        const int32_t elapsed = int32_t(uint32_t(PlayScreen.GameTime()) - uint32_t(slot.start));
        if (slot.start == 0 || elapsed < 0 || elapsed > 2 * slot.frequency)
        {
            slot.start = PlayScreen.GameTime();
            continue;
        }
        if (elapsed <= slot.frequency)
            continue;

        slot.start = PlayScreen.GameTime();

        int32_t found[kCountMax] = {};
        const int32_t near = MapPane.FindObjectsInRange(IntPos(*this), GetLevel(), found, kCountRadius, 0,
                                                        OBJCLASS_CHARACTER, kCountMax, OBJSET_CHARACTER);
        if (near < maxmonsters)
            Spawn(i);
        else
            log_debug("[monstergen] %s at (%d, %d): %s due, but %d characters within %d (cap %d)",
                      GetName(), IntPos(*this).x, IntPos(*this).y, slot.type.c_str(), near, kCountRadius, maxmonsters);
    }
}

bool TMonsterGen::AddType(const char* type, int32_t frequency, int32_t treasure)
{
    const auto free = std::find_if(slots.begin(), slots.end(),
                                   [](const std::optional<SSlot>& slot) { return !slot; });
    if (free == slots.end())
        return false;

    SSlot& slot    = free->emplace();
    slot.type      = type;
    slot.frequency = uint32_t(frequency) >= uint32_t(kMaxFrequency) ? kMaxFrequency : frequency;
    slot.treasure  = treasure;
    slot.start     = PlayScreen.GameTime();
    return true;
}

bool TMonsterGen::DelType(const char* type)
{
    for (std::optional<SSlot>& slot : slots)
        if (slot && slot->type == type)
        {
            slot.reset();
            return true;
        }
    return false;
}

int32_t TMonsterGen::NumTypes() const
{
    return int32_t(std::count_if(slots.begin(), slots.end(),
                                 [](const std::optional<SSlot>& slot) { return slot.has_value(); }));
}

const char* TMonsterGen::TypeName(int32_t index) const
{
    if (index < 0 || index >= kMaxTypes || !slots[index])
        return "";
    return slots[index]->type.c_str();
}

void TMonsterGen::Spawn(int32_t index)
{
    if (Editor)
        return;

    const SSlot& slot = *slots[index];
    const S3DPoint home = IntPos(*this);

    S3DPoint at{};
    int32_t facing = 0;
    bool blocked = true;
    for (int32_t tries = kSpawnTries; tries > 0 && blocked; --tries)
    {
        facing = random(0, 255);
        const int32_t distance = random(kSpawnMinDist, kSpawnMaxDist);
        S3DPoint offset{};
        ConvertToVector(facing, distance, offset);
        at = S3DPoint(home.x + offset.x, home.y + offset.y, home.z + kSpawnRise);

        int32_t maxdelta = 0, height = 0;
        bool hole = false;
        MapPane.GetWalkHeightRadius(at, GetLevel(), kSpawnRadius, maxdelta, height, hole);
        blocked = maxdelta > kSpawnMaxStep || hole;
    }
    if (blocked)
    {
        log_debug("[monstergen] %s at (%d, %d): no room for a %s", GetName(), home.x, home.y, slot.type.c_str());
        return;
    }

    TObjectClass* const characters = TObjectClass::GetClass(OBJCLASS_CHARACTER);
    const int32_t objtype = characters ? characters->FindObjType(slot.type.c_str()) : -1;
    if (objtype < 0)
    {
        log_debug("[monstergen] %s at (%d, %d): no character type \"%s\"", GetName(), home.x, home.y,
                  slot.type.c_str());
        return;
    }

    // Retail put the monster on the map pane's level, not the generator's.
    SObjectDef def{};
    def.objclass = OBJCLASS_CHARACTER;
    def.objtype  = short(objtype);
    def.level    = uint16_t(MapPane.GetMapLevel());
    def.pos      = at;
    TObjectInstance* const monster = MapPane.GetInstance(MapPane.NewObject(&def));
    if (!monster)
    {
        // TMapPane::AddObject places only in the pane's sector window.
        log_debug("[monstergen] %s at (%d, %d): the map didn't take a %s at (%d, %d)", GetName(), home.x,
                  home.y, slot.type.c_str(), at.x, at.y);
        return;
    }

    if (TObjectImagery* const imagery = monster->GetImagery())
        imagery->CacheImagery();
    GiveSpawnTreasure(monster, slot.treasure);
    monster->Face(facing);

    log_info("[monstergen] %s at (%d, %d, %d) level %d: made %s (map index %d) at (%d, %d, %d), slot %d, "
             "treasure tier %d, game time %d",
             GetName(), home.x, home.y, home.z, GetLevel(), monster->GetName(), monster->GetMapIndex(),
             at.x, at.y, at.z, index, slot.treasure, PlayScreen.GameTime());
}
