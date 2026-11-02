// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *      shadowpack.cpp - Shadow Smoke keeps Locke's pack as a player does  *
// *************************************************************************

#include "game/shadow/shadowpack.h"

#include "game/shadow/shadowlog.h"

#include "game/object/object.h"
#include "game/object/player.h"
#include "engine/core/revdefs.h"
#include "game/app/revutils.h"
#include "engine/core/textencoding.h"

#include <string>
#include <vector>

namespace {

// The belt boxes the F5-F9 keys drink from (GRIND.md §6.2).
constexpr int32_t kBeltBoxes = 5;
// A tidy is a handful of moves; past this many it is going round in circles.
constexpr int32_t kMaxMoves = 64;

int32_t Stat(TObjectInstance& item, const char* stat)
{
    return item.FindStat(stat) >= 0 ? item.GetStat(stat) : 0;
}

bool IsBelt(const TObjectInstance& item, const TPlayer& locke)
{
    return item.GetOwner() == &locke && item.InventNum() >= kInvSlotBeltFirst &&
           item.InventNum() < kInvSlotBeltFirst + kBeltBoxes;
}

// Loose in the pack: carried by Locke himself, not equipped or on the belt.
bool IsLoose(const TObjectInstance& item, const TPlayer& locke)
{
    return item.GetOwner() == &locke && item.InventNum() >= 0 && item.InventNum() <= kInvSlotLastCarried;
}

bool IsSpellPouch(TObjectInstance& item)
{
    return !stricmp(item.GetTypeName(), "Spell Pouch") || !stricmp(item.GetTypeName(), "SpellPouch");
}

// A bag of the kind AddToInventory routes into: named exactly "Pouch".
bool IsPouch(TObjectInstance& item)
{
    return !stricmp(item.GetName(), "Pouch");
}

// Which potions the belt holds first: healing, the strongest first, then
// mana, then the rest (Cure Poison).
int32_t BeltRank(TObjectInstance& potion)
{
    if (const int32_t health = Stat(potion, "Health"); health > 0)
        return 20000 + health;
    if (const int32_t mana = Stat(potion, "Mana"); mana > 0)
        return 10000 + mana;
    return 0;
}

std::string NameOf(TObjectInstance& item)
{
    return ToUtf8(item.GetName());
}

// What sorts into a Pouch of its own: scrolls, and the tradeable odd bits.
// Quest items (Value -1: nobody buys them) stay loose.
bool Sorts(TObjectInstance& item)
{
    if (item.ObjClass() == OBJCLASS_SCROLL)
        return true;
    return item.ObjClass() == OBJCLASS_ITEM && item.FindStat("Value") >= 0 && item.GetStat("Value") >= 0;
}

class TTidy
{
  public:
    TTidy(TPlayer& locke, TShadowLog& log) : locke(locke), log(log) {}

    // One move toward order, from a fresh look at the pack (a move can merge
    // and delete what an earlier look saw); false with nothing left to do.
    bool Step()
    {
        return Talisman() || Potion() || Sort();
    }

  private:
    std::vector<TObjectInstance*> Everything() const
    {
        std::vector<TObjectInstance*> items;
        for (TInventoryIterator i(&locke, EInvWalk::Nested); i; i++)
            items.push_back(i.Item());
        return items;
    }

    bool Move(TObjectInstance& item, TObjectInstance& into, int32_t slot, const std::string& where)
    {
        const std::string name = NameOf(item);
        if (!into.AddToInventory(&item, slot))
            return false;
        log.Event("pack", name + " to " + where);
        return true;
    }

    bool Talisman()
    {
        TObjectInstance* pouch = nullptr;
        for (TObjectInstance* item : Everything())
            if (IsSpellPouch(*item))
                pouch = item;
        if (!pouch)
            return false;
        for (TObjectInstance* item : Everything())
            if (item->ObjClass() == OBJCLASS_TALISMAN && item->GetOwner() != pouch &&
                Move(*item, *pouch, -1, "the Spell Pouch"))
                return true;
        return false;
    }

    bool Potion()
    {
        // The best kind not yet on the belt.
        TObjectInstance* best = nullptr;
        for (TObjectInstance* item : Everything())
            if (item->ObjClass() == OBJCLASS_POTION && !IsBelt(*item, locke) &&
                (!best || BeltRank(*item) > BeltRank(*best)))
                best = item;
        if (!best)
            return false;
        // Its kind's box (the pile merges), else an empty box, else the box
        // of a kind it outranks (that pile goes back to the pack).
        int32_t into = -1, weakest = -1;
        for (int32_t box = 0; box < kBeltBoxes; ++box)
        {
            TObjectInstance* held = locke.GetInventorySlot(kInvSlotBeltFirst + box);
            if (held && !stricmp(held->GetName(), best->GetName()))
            {
                into = box;
                break;
            }
            if (!held && into < 0)
                into = box;
            if (held && BeltRank(*held) < BeltRank(*best) &&
                (weakest < 0 ||
                 BeltRank(*held) < BeltRank(*locke.GetInventorySlot(kInvSlotBeltFirst + weakest))))
                weakest = box;
        }
        if (into < 0)
            into = weakest;
        return into >= 0 && Move(*best, locke, kInvSlotBeltFirst + into, "belt box " + std::to_string(into + 1));
    }

    bool Sort()
    {
        std::vector<TObjectInstance*> pouches;
        for (TObjectInstance* item : Everything())
            if (IsPouch(*item) && !IsSpellPouch(*item))
                pouches.push_back(item);
        if (pouches.empty())
            return false;
        // Scrolls before the odd bits: the first sorted takes the first empty Pouch.
        for (const bool scrolls : {true, false})
            for (TObjectInstance* item : Everything())
            {
                if (!IsLoose(*item, locke) || !Sorts(*item) || (item->ObjClass() == OBJCLASS_SCROLL) != scrolls)
                    continue;
                // The Pouch of its kind, else the first empty one.
                TObjectInstance* kind = nullptr;
                TObjectInstance* empty = nullptr;
                for (TObjectInstance* pouch : pouches)
                {
                    const TInventoryIterator first(pouch);
                    if (!first.Item())
                        empty = empty ? empty : pouch;
                    else if (first.Item()->ObjClass() == item->ObjClass())
                    {
                        kind = pouch;
                        break;
                    }
                }
                TObjectInstance* into = kind ? kind : empty;
                if (into && Move(*item, *into, -1, scrolls ? "the scroll Pouch" : "the Pouch of odd bits"))
                    return true;
            }
        return false;
    }

    TPlayer&    locke;
    TShadowLog& log;
};

} // namespace

void ShadowTidyPack(TPlayer& locke, TShadowLog& log)
{
    TTidy tidy(locke, log);
    for (int32_t moves = 0; moves < kMaxMoves && tidy.Step(); ++moves)
    {
    }
}

TObjectInstance* ShadowBeltHealing(TPlayer& locke, int32_t missing,
                                   const std::map<const TObjectInstance*, int32_t>& drunk)
{
    TObjectInstance* enough = nullptr;
    TObjectInstance* biggest = nullptr;
    for (int32_t box = 0; box < kBeltBoxes; ++box)
    {
        TObjectInstance* held = locke.GetInventorySlot(kInvSlotBeltFirst + box);
        const int32_t health = held && held->ObjClass() == OBJCLASS_POTION ? Stat(*held, "Health") : 0;
        if (health <= 0)
            continue;
        if (const auto gone = drunk.find(held); gone != drunk.end() && gone->second >= std::max(held->Amount(), 1))
            continue;
        if (health >= missing && (!enough || health < Stat(*enough, "Health")))
            enough = held;
        if (!biggest || health > Stat(*biggest, "Health"))
            biggest = held;
    }
    return enough ? enough : biggest;
}
