// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *      shadowpurse.cpp - what Locke wears, and what he has no use for     *
// *************************************************************************

#include "shadowpurse.h"

#include "shadowlog.h"

#include "../object.h"
#include "../player.h"
#include "../textencoding.h"

#include <vector>

namespace {

// The armour slots (player.h EQ_*): jewellery's 1, 5 and 6 are left out.
constexpr int32_t kArmourSlots[] = {EQ_HEAD, EQ_BODY, EQ_OFFHAND, EQ_LEGS, EQ_FEET};

int32_t Stat(TObjectInstance& item, const char* stat)
{
    return item.FindStat(stat) >= 0 ? item.GetStat(stat) : 0;
}

bool IsArmourSlot(int32_t slot)
{
    for (const int32_t s : kArmourSlots)
        if (s == slot)
            return true;
    return false;
}

// Everything he carries that isn't worn or on the belt: the pack and bags.
std::vector<TObjectInstance*> Carried(TPlayer& locke)
{
    std::vector<TObjectInstance*> items;
    for (TInventoryIterator i(&locke, EInvWalk::Nested); i; i++)
    {
        TObjectInstance* item = i.Item();
        if (item->GetOwner() == &locke && item->InventNum() > kInvSlotLastCarried)
            continue;
        items.push_back(item);
    }
    return items;
}

} // namespace

int32_t ShadowReadyArmour(TPlayer& locke, TShadowLog& log)
{
    int32_t worn = 0;
    for (const int32_t slot : kArmourSlots)
    {
        TObjectInstance* wearing = locke.GetEquip(slot);
        int32_t best = wearing ? Stat(*wearing, "Protection") : 0;
        TObjectInstance* better = nullptr;
        for (TObjectInstance* item : Carried(locke))
        {
            if (item->ObjClass() != OBJCLASS_ARMOR || Stat(*item, "EqSlot") != slot ||
                Stat(*item, "Protection") <= best)
                continue;
            if (!locke.CanEquip(item, slot))
            {
                log.Event("ready", ToUtf8(item->GetName()) + " is better but needs Strength " +
                                       std::to_string(Stat(*item, "MinStrength")) + " and Constitution " +
                                       std::to_string(Stat(*item, "MinConstitution")));
                continue;
            }
            best = Stat(*item, "Protection");
            better = item;
        }
        if (better && locke.Equip(better, slot))
        {
            log.Event("ready", ToUtf8(better->GetName()) + " on, protection " + std::to_string(best));
            ++worn;
        }
    }
    return worn;
}

std::map<std::string, int32_t> ShadowJunk(TPlayer& locke)
{
    std::map<std::string, int32_t> junk;
    // Weapons: all but the best spare.
    TObjectInstance* spare = nullptr;
    std::vector<TObjectInstance*> weapons;
    for (TObjectInstance* item : Carried(locke))
        if (item->ObjClass() == OBJCLASS_WEAPON)
        {
            weapons.push_back(item);
            if (!spare || Stat(*item, "Damage") > Stat(*spare, "Damage"))
                spare = item;
        }
    for (TObjectInstance* weapon : weapons)
        if (weapon != spare)
            junk[weapon->GetName()] += 1;

    // Armour no better than what he wears in its slot.
    for (TObjectInstance* item : Carried(locke))
    {
        if (item->ObjClass() != OBJCLASS_ARMOR)
            continue;
        const int32_t slot = Stat(*item, "EqSlot");
        TObjectInstance* wearing = IsArmourSlot(slot) ? locke.GetEquip(slot) : nullptr;
        if (wearing && Stat(*item, "Protection") <= Stat(*wearing, "Protection"))
            junk[item->GetName()] += 1;
    }
    return junk;
}
