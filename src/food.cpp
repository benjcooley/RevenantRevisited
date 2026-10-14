// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                       food.cpp - TFood module                         *
// *************************************************************************

#include "revenant.h"
#include "dialog.h"
#include "effect.h"
#include "food.h"
#include "imagery.h"
#include "logging.h"
#include "mappane.h"
#include "player.h"
#include "rules.h"
#include "sound.h"
#include "spell.h"
#include "textbar.h"

REGISTER_BUILDER(TFood)

TObjectClass FoodClass("FOOD", OBJCLASS_FOOD, 0);

// Food hard coded stats
DEFSTAT(Food, Value, VAL, 0, 0, 0, 1000000)
DEFSTAT(Food, Health, HLTH, 1, 0, 0, 1000000)
DEFSTAT(Food, Mana, MANA, 2, 0, 0, 1000000)
DEFSTAT(Food, Fatigue, FATG, 3, 0, 0, 1000000)
DEFSTAT(Food, Poison, PSN, 4, 0, 0, 1000000)
DEFSTAT(Food, Cure, CURE, 5, 0, 0, 1000000)
DEFSTAT(Food, Fill, FILL, 5, 0, 0, 1000000)

// REVSYNC: the FOOD object stat registered @ 0x0050e750 (index 0, 1..1000).
DEFOBJSTAT(Food, Amount, AMT, 0, 1, 1, 1000)

// REVSYNC: TFood::Load @ 0x0050eae0 (also POTION's; SAVE_GAME.md §11.3).
void TFood::Load(RTInputStream is, int32_t version, int32_t objversion)
{
    TObjectInstance::Load(is, version, objversion);
    if (Amount() == 0)
        SetAmount(1);
}

// REVSYNC: TFood::MergeInto @ 0x0050ea80 (vtable 0x98, POTION's too). Food
// joins the first item named as its type in the new owner's inventory, bags
// included, whatever that item's class.
bool TFood::MergeInto(TObjectInstance* newowner)
{
    TObjectInstance* stack = newowner->FindObjInventory(GetTypeName());
    if (!stack)
        return false;

    stack->SetAmount(stack->Amount() + Amount());
    log_debug("[inv] %s: %d %s merged into the stack in %s, now %d", newowner->GetName(),
              Amount(), GetName(), stack->GetOwner()->GetName(), stack->Amount());
    return true;
}


namespace
{
// How near a poison spell's effect must be to the one it poisons for food to
// end it (retail's map iterator 0x0044ceb0 and Distance 0x0046de60 at 0x80).
constexpr int32_t kPoisonEffectReach = 0x80;

// Poison cured (0x0050e8a6): the character's poison mark cleared, and the
// effects about it of each poison spell whose first target it is ended, or
// they would mark it again -- the pulsed objects within reach on its level
// (no inventories, the loaded sectors: retail's iterator flags 0xe0).
void CurePoison(TObjectInstance* user)
{
    user->SetPoisoned(0);
    const S3DPoint at = user->Pos();
    SRect around{at.x - kPoisonEffectReach, at.y - kPoisonEffectReach,
                 at.x + kPoisonEffectReach, at.y + kPoisonEffectReach};
    for (TMapIterator i(user->GetLevel(), &around, CHECK_NOINVENT, OBJSET_PULSE); i; i++)
    {
        if (Distance(at, i->Pos()) > kPoisonEffectReach || i->ObjClass() != OBJCLASS_EFFECT)
            continue;
        auto* effect = static_cast<TEffect*>(i.Item());
        TSpell* spell = effect->GetSpell();
        if (spell && spell->GetTarget(0) == user && spell->SpellData() &&
            spell->SpellData()->damagetype == DT_POISON)
            effect->KillThisEffect();
    }
}
}  // namespace

// REVSYNC: TFood::Use @ 0x0050e780 (FOOD and POTION, vtables 0x005b2cd4 /
// 0x005b2ee4 slot 0xbc; GRIND.md §6.2) -- eaten or drunk by `user`, after
// the base Use (its USE triggers; done if that took it). Not by a dead
// character, not with another object, not in a state but the first. A
// character gains the item's Health and Mana, each held to its maximum, and
// an item with Poison cures it; the item's Fatigue is never applied. The
// player hears "food" for an item with an inventory icon, else "potion". In
// an inventory the amount drops by one, and the last is deleted from the
// owner by name; on the map nothing is used up. The player reads FOODGOOD1.
// Not ported: the network guard 0x00676e5d set about the decrement.
bool TFood::Use(TObjectInstance* user, int32_t with)
{
    if (TObjectInstance::Use(user, with))
        return true;
    if (user && user->IsCharacter() && user->Health() <= 0)
        return false;
    if (with != -1 || GetState() != 0)
        return false;
    if (!user)
        return false;   // (retail would crash)

    if (user->IsCharacter())
    {
        user->SetHealth(user->Health() + Health());
        if (user->Health() > user->MaxHealth())
            user->SetHealth(user->MaxHealth());
        user->SetMana(user->Mana() + Mana());
        if (user->Mana() > user->MaxMana())
            user->SetMana(user->MaxMana());
        if (Poison())
            CurePoison(user);
    }

    TObjectImagery* imagery = GetImagery();
    const char* sound = imagery && imagery->GetInvImage(0, 0) ? "food" : "potion";
    if (user == Player)
    {
        const int32_t id = SoundPlayer.FindSound(sound);
        if (id >= 0 && SoundPlayer.Mount(id))
            SoundPlayer.Play(id);
    }

  // The last one goes from its owner's inventory, which deletes this object.
    if (TObjectInstance* owner = GetOwner())
    {
        const int32_t amount = Amount();
        if (amount > 1)
            SetAmount(amount - 1);
        else
            owner->DeleteFromInventory(GetName(), 1);
    }

    if (user == Player)
        TextBar.Print("%s", DialogList.GetLine("FOODGOOD1"));
    return true;
}
