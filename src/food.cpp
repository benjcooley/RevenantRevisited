// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                       food.cpp - TFood module                         *
// *************************************************************************

#include "food.h"

#include "dialog.h"
#include "effect.h"
#include "imagery.h"
#include "logging.h"
#include "mappane.h"
#include "player.h"
#include "revenant.h"
#include "rules.h"
#include "saferef.h"
#include "sound.h"
#include "spell.h"
#include "textbar.h"

#include <vector>

REGISTER_BUILDER(TFood)

TObjectClass FoodClass("FOOD", OBJCLASS_FOOD, 0);

// Food hard coded stats
DEFSTAT(Food, Value, VAL, 0, 0, 0, 1000000)
DEFSTAT(Food, Health, HLTH, 1, 0, 0, 1000000)
DEFSTAT(Food, Mana, MANA, 2, 0, 0, 1000000)
DEFSTAT(Food, Fatigue, FATG, 3, 0, 0, 1000000)
DEFSTAT(Food, Poison, PSN, 4, 0, 0, 1000000)
DEFSTAT(Food, Cure, CURE, 5, 0, 0, 1000000)
DEFSTAT(Food, Fill, FILL, 6, 0, 0, 1000000)

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

namespace {

// REVSYNC: 0x0050e8b2..0x0050e95c -- the poison at work on `who` ends: each
// effect within 0x80 of him (the pulsed objects of the loaded sectors on his
// level) whose spell does poison damage and has him as its first target.
// UNCONFIRMED (INVENTORY_ACTIONS.md §10): that damage type 9 is all poison.
void EndPoisonOn(TObjectInstance* who)
{
    constexpr int32_t kRange = 0x80;
    const S3DPoint pos = who->Pos();
    SRect r{pos.x - kRange, pos.y - kRange, pos.x + kRange, pos.y + kRange};
    std::vector<TSafeRef<TObjectInstance>> ending;
    TMapPane::ForObjectsIn(who->GetLevel(), r, OBJSET_PULSE, [&](TObjectInstance* oi) {
        if (oi->ObjClass() != OBJCLASS_EFFECT || ::Distance(pos, oi->Pos()) > kRange)
            return;
        TSpell* spell = static_cast<TEffect*>(oi)->GetSpell();
        if (spell && spell->GetTarget(0) == who && spell->SpellData() && spell->SpellData()->damagetype == DT_POISON)
            ending.emplace_back(oi);
    });
    // Ended after the walk: ending one changes the sets the walk reads.
    for (const TSafeRef<TObjectInstance>& effect : ending)
        if (TObjectInstance* oi = effect.Get())
            static_cast<TEffect*>(oi)->KillThisEffect();
}

} // namespace

// REVSYNC: TFood::Use @ 0x0050e780 (POTION's too). Eaten or drunk alone by a
// living user: his health and mana go up by the item's (clamped to his
// maximums), a "Poison" item cures him, the main player hears it ("food"
// for an item with a still inventory picture, "potion" for an animated one)
// and reads FOODGOOD1, and the pile in an inventory loses one. Fatigue, Cure
// and Fill aren't read. Not ported (multiplayer): the network guard around
// the use-up (0x00676e5d).
bool TFood::Use(TObjectInstance* user, int32_t with)
{
    if (TObjectInstance::Use(user, with))
        return true;
    if (user && user->IsCharacter() && user->Health() <= 0)
        return false;
    if (with != -1 || GetState() != 0)
        return false;

    if (user && user->IsCharacter())
    {
        user->SetHealth(user->Health() + Health());
        if (user->Health() > user->MaxHealth())
            user->SetHealth(user->MaxHealth());
        user->SetMana(user->Mana() + Mana());
        if (user->Mana() > user->MaxMana())
            user->SetMana(user->MaxMana());
        if (Poison())
        {
            user->SetPoisoned(0);
            EndPoisonOn(user);
        }
    }

    if (user == Player)
    {
        TObjectImagery* const im = GetImagery();
        const int32_t id = SoundPlayer.FindSound(im && im->GetInvImage(0) ? "food" : "potion");
        if (id >= 0 && SoundPlayer.Mount(id))
            SoundPlayer.Play(id);
    }

    // The last of a pile leaves its owner's inventory, deleted.
    if (TObjectInstance* const own = GetOwner())
    {
        if (Amount() > 1)
            SetAmount(Amount() - 1);
        else
            own->DeleteFromInventory(GetName(), 1);
    }

    if (user == Player)
        TextBar.Print("%s", DialogList.GetLine("FOODGOOD1"));
    return true;
}
