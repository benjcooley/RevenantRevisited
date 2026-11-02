// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *        shadowloot.h - Shadow Smoke's chests: what they hold, taking     *
// *************************************************************************
//
// Loot is most of Locke's income (docs/shadow/ARCHITECTURE.md §8.8). The
// survey lists a level's chests with what they hold and how they are
// locked; the run logs it as Locke arrives on each level, which is where the
// plan's chest tours come from. Taking from one goes through the real
// TContainer::Use, one use a tick as a player clicks: the first opens it,
// each after takes one item, the last closes it.

#pragma once

#include "engine/core/revtypes.h"
#include "engine/core/saferef.h"

#include <cstdint>
#include <map>
#include <string>
#include <vector>

class TContainer;
class TObjectInstance;
class TShadowLog;

struct SShadowChest
{
    TSafeRef<TObjectInstance> chest;
    std::string               name;         // its instance name, else its type
    std::string               type;
    S3DPoint                  pos{};
    bool                      openable = false;
    bool                      locked   = false;
    int32_t                   key      = 0; // KEY: the key that opens it
    int32_t                   pick     = 0; // PickDifficulty
    std::map<std::string, int32_t> contents; // by lower-case name, gold left out
    int32_t                   gold     = 0;

    // "ChestS@4724,5092".
    [[nodiscard]] std::string Handle() const;
};

// The containers on `level` (not exits, which are containers too).
[[nodiscard]] std::vector<SShadowChest> ShadowSurveyChests(int32_t level);
// One container as the survey reads it.
[[nodiscard]] SShadowChest ShadowReadChest(TContainer& chest);
// Logs the survey of `level` as a `chests` event.
void ShadowLogChests(int32_t level, TShadowLog& log);
