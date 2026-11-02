// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *       shadowpurse.h - what Locke wears, and what he has no use for      *
// *************************************************************************
//
// docs/shadow/ARCHITECTURE.md §8.1 and §8.10, the parts the duel model
// isn't needed for:
//
// - **Readying armour.** Each armour slot wears its best piece by
//   Protection, through the real TPlayer::Equip, whose CanEquip keeps out
//   what his Strength or Constitution can't carry. Jewellery (neck,
//   bracelet, ring: EqSlot 1, 5, 6) waits for the duel to value its
//   STATLINE, and the weapon is the plan's and the style's (§8.4).
// - **Junk.** What he would sell: every weapon but the one in hand and the
//   best spare, and every piece of armour no better than what he wears in
//   its slot. Never what he wears, potions, food, bags, talismans, scrolls,
//   keys or jewellery; quest items (Value -1) no shop lists anyway.

#pragma once

#include <cstdint>
#include <map>
#include <string>

class TPlayer;
class TShadowLog;

// Puts on the best armour he carries for each armour slot. The pieces put
// on, logged; how many.
int32_t ShadowReadyArmour(TPlayer& locke, TShadowLog& log);

// What he has no use for, by type name: how many of each to sell.
[[nodiscard]] std::map<std::string, int32_t> ShadowJunk(TPlayer& locke);
