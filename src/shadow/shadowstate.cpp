// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *       shadowstate.cpp - Locke as a save should keep him, for checking   *
// *************************************************************************

#include "shadowstate.h"

#include "../jsonout.h"
#include "../player.h"
#include "../revdefs.h"
#include "../script.h"
#include "../textencoding.h"

#include <algorithm>
#include <vector>

namespace {

// Where an item is kept, in words that survive a reload: the pack, a belt
// box, an equipment slot, or a bag (named with where the bag itself is).
std::string Place(const TObjectInstance& item, const TPlayer& locke)
{
    const TObjectInstance* owner = item.GetOwner();
    const int32_t slot = item.InventNum();
    if (owner == &locke)
    {
        if (slot >= kInvSlotBeltFirst)
            return "belt " + std::to_string(slot - kInvSlotBeltFirst + 1);
        if (slot >= kInvSlotEquipFirst)
            return "equip " + std::to_string(slot - kInvSlotEquipFirst);
        return "pack " + std::to_string(slot);
    }
    if (!owner)
        return "nowhere";
    return Place(*owner, locke) + " / " + ToUtf8(owner->GetName()) + " " + std::to_string(slot);
}

} // namespace

std::string ShadowStateJson(TPlayer& locke)
{
    JsonOut out;
    out.Begin('{');
    out.Field("maplevel", locke.GetLevel());
    out.Key("pos").Begin('[').Value(locke.Pos().x).Value(locke.Pos().y).Value(locke.Pos().z).End(']');
    out.Key("health").Begin('[').Value(locke.Health()).Value(locke.MaxHealth()).End(']');
    out.Key("mana").Begin('[').Value(locke.Mana()).Value(locke.MaxMana()).End(']');
    out.Key("fatigue").Begin('[').Value(locke.Fatigue()).Value(locke.MaxFatigue()).End(']');
    out.Field("level", locke.Level());
    out.Field("exp", locke.Exp());
    out.Field("nextexp", locke.NextExp());
    out.Field("attacklevel", locke.AttackLevel());
    out.Field("poisoned", locke.Poisoned());

    out.Key("stats").Begin('{');
    for (int32_t i = 0; i < NUM_PLRSTATS; ++i)
        out.Field(kAttributeNames[i], locke.PlyrStat(i));
    out.End('}');
    out.Key("skills").Begin('{');
    for (int32_t i = 0; i < NUM_SKILLS; ++i)
        out.Key(kSkillNames[i]).Begin('[').Value(locke.Skill(i)).Value(locke.SkillExp(i)).End(']');
    out.End('}');
    out.Key("levelup").Begin('[');
    for (const int32_t pick : locke.LevelUpStats())
        out.Value(pick);
    out.End(']');
    out.Field("gold", locke.GetInventoryAmount("Gold"));

    // Every item where it is, sorted: a reload may list them in another
    // order, but must keep each one where it was.
    std::vector<std::string> items;
    for (TInventoryIterator i(&locke, EInvWalk::Nested); i; i++)
        items.push_back(Place(*i.Item(), locke) + ": " + ToUtf8(i.Item()->GetName()) + " x" +
                        std::to_string(std::max(i.Item()->Amount(), 1)));
    std::sort(items.begin(), items.end());
    out.Key("items").Begin('[');
    for (const std::string& item : items)
        out.String(item);
    out.End(']');

    out.Key("spells").Begin('[');
    for (int32_t i = 0; i < locke.NumKnownSpells(); ++i)
        out.String(locke.KnownSpell(i));
    out.End(']');
    out.Key("quickspells").Begin('[');
    for (int32_t i = 0; i < QSPELL_NUM; ++i)
        out.String(locke.GetQuickSpell(i));
    out.End(']');

    const TGameState& states = ScriptManager.GameStates();
    out.Key("states").Begin('{');
    for (int32_t i = 0; i < states.NumStates(); ++i)
        if (const char* name = states.StateName(i))
            out.Field(name, states.State(i));
    out.End('}');
    out.End('}');
    return out.str();
}
