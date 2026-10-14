// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *       shadowpack.h - Shadow Smoke keeps Locke's pack as a player does   *
// *************************************************************************
//
// docs/shadow/ARCHITECTURE.md §8.9. Every move is the game's own
// AddToInventory -- what a drag in the inventory pane does -- so piles
// merge, Pouches take their kind and the ledger, which counts by name
// across the whole tree, sees nothing come or go:
//
// - **Talismans into the Spell Pouch,** the only place casting looks
//   (TPlayer::HasTalismans).
// - **Potions onto the belt,** the action bar's boxes that F5-F9 drink
//   from: one kind a box, healing first. A fight drinks only from here.
// - **Scrolls into a Pouch of their own,** and the tradeable odd bits loot
//   brings (gems and the like: ITEM class, with a Value) into another,
//   while he has Pouches for them. A Pouch whose first item is of a class
//   takes every new one of it by itself (AddToInventory), so one sorted
//   item makes the Pouch that kind's. Quest items (Value -1, which no shop
//   takes) and keys stay loose, where a player keeps them.

#pragma once

#include <cstdint>

class TObjectInstance;
class TPlayer;
class TShadowLog;

// Puts the pack in order, logging each move.
void ShadowTidyPack(TPlayer& locke, TShadowLog& log);

// The healing potion on the belt to drink, `missing` health short: the
// smallest that makes it up, else the biggest. Null with none on the belt.
[[nodiscard]] TObjectInstance* ShadowBeltHealing(TPlayer& locke, int32_t missing);
