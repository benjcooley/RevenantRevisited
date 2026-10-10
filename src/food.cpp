// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                       food.cpp - TFood module                         *
// *************************************************************************

#include "revenant.h"
#include "character.h"
#include <typeinfo>
#include "food.h"
#include "logging.h"

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


bool TFood::Use(TObjectInstance* user, int32_t with)
{
    if (GetState() == 0)
    {
        if (((PTCharacter)user)->IsCharacter())
        {
        // make the user healthier
            ((PTCharacter)user)->SetHealth(user->Health() + Health());

            if (((PTCharacter)user)->Health() > ((PTCharacter)user)->MaxHealth())
                ((PTCharacter)user)->SetHealth(((PTCharacter)user)->MaxHealth());

        // give the user more mana
            ((PTCharacter)user)->SetMana(user->Mana() + Mana());

            if (((PTCharacter)user)->Mana() > ((PTCharacter)user)->MaxMana())
                ((PTCharacter)user)->SetMana(((PTCharacter)user)->MaxMana());
        }
        SetState(1);
        return true;
    }

    return false;
}
