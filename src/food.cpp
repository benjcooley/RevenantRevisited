// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                       food.cpp - TFood module                         *
// *************************************************************************

#include "food.h"

#include "dialog.h"
#include "effect.h"
#include "logging.h"
#include "mappane.h"
#include "player.h"
#include "rules.h"
#include "sound.h"
#include "spell.h"
#include "textbar.h"

#include <string>

REGISTER_BUILDER(TFood)

TObjectClass FoodClass("FOOD", OBJCLASS_FOOD, 0);

// REVSYNC: the FOOD stats registered @ 0x0050e5ec..0x0050e771, by id
// (docs/gameplay/forensics/CONSUMABLES.md §3). class.def adds SaleType (7).
DEFSTAT(Food, Value, VAL, 0, 0, -1, 1000000)
DEFSTAT(Food, Health, HLTH, 1, 0, 0, 1000000)
DEFSTAT(Food, Mana, MANA, 2, 0, 0, 1000000)
DEFSTAT(Food, Fatigue, FATG, 3, 0, 0, 1000000)
DEFSTAT(Food, Poison, PSN, 4, 0, 0, 1000000)
DEFSTAT(Food, Cure, CURE, 5, 0, 0, 1000000)
DEFSTAT(Food, Fill, FILL, 6, 0, 0, 1000000)

// REVSYNC: the FOOD object stat registered @ 0x0050e750 (index 0, 1..1000).
DEFOBJSTAT(Food, Amount, AMT, 0, 1, 1, 1000)

namespace
{
// How near the user a cure ends a poison spell's effect (retail 0x80).
constexpr int32_t kCureRange = 0x80;

// REVSYNC: 0x0050e8a6..0x0050e95e. A cure clears the user's poisoned flag
// and ends each poison spell (DAMAGETYPE 9) aimed at the user whose effect is
// within kCureRange of them: the pulse set of the square round the user on
// their level (0x0044ceb0(pos, 0x80, 0xe0, 4, 0, level)), by the approximate
// distance (0x0046de60), each through TEffect::KillThisEffect.
void CurePoison(TObjectInstance* user)
{
    user->SetPoisoned(0);
    const S3DPoint at = user->Pos();
    SRect square(at.x - kCureRange, at.y - kCureRange, at.x + kCureRange, at.y + kCureRange);
    TMapPane::ForObjectsIn(user->GetLevel(), square, OBJSET_PULSE, [&](TObjectInstance* obj) {
        if (::Distance(at, obj->Pos()) > kCureRange || obj->ObjClass() != OBJCLASS_EFFECT)
            return;
        auto* effect = dynamic_cast<TEffect*>(obj);
        TSpell* spell = effect ? effect->GetSpell() : nullptr;
        if (spell && spell->GetTargetRef(0).Get() == user && spell->SpellData() &&
            spell->SpellData()->damagetype == DT_POISON)
            effect->KillThisEffect();
    });
}
}  // namespace

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

// REVSYNC: TFood::Use @ 0x0050e780 (CONSUMABLES.md §2).
bool TFood::Use(TObjectInstance* user, int32_t with)
{
    // The USE triggers first; an item that did something with another
    // counts as used.
    if (TObjectInstance::Use(user, with))
        return true;

    // REVSYNC-DIVERGENCE: retail reads a null user's class (a fault); the
    // port refuses it.
    if (!user)
        return false;

    // Eaten on its own, while whole, and not by the dead (0x0050e7a9).
    if ((user->IsCharacter() && user->Health() < 1) || with != -1 || GetState() != 0)
        return false;

    const int32_t healthBefore = user->Health();
    const int32_t manaBefore = user->Mana();
    if (user->IsCharacter())
    {
        // The item's Health and Mana are added and each total is then held to
        // the user's maximum, so one already above it comes down to it.
        user->SetHealth(user->Health() + Health());
        if (user->MaxHealth() < user->Health())
            user->SetHealth(user->MaxHealth());
        user->SetMana(user->Mana() + Mana());
        if (user->MaxMana() < user->Mana())
            user->SetMana(user->MaxMana());
        if (Poison())
            CurePoison(user);
    }

    // REVSYNC: 0x0050e95f..0x0050e9f0. The main player hears it eaten when
    // the item has a still inventory picture ("food"), drunk when it shows
    // only an inventory animation, as every potion does ("potion").
    // REVSYNC-DIVERGENCE: retail's 2D imagery answers a copy of state 0's
    // picture once a synchronous body load has made one (0x00447ccf), and a
    // potion with one sounds like food; the port's imagery keeps no copy
    // (CONSUMABLES.md §5).
    const char* const sound = imagery && imagery->GetInvImage(0) ? "food" : "potion";
    const bool mainPlayer = user == Player;
    if (mainPlayer)
        PLAY(sound);

    log_info("[food] %s used %s (%s): health %d -> %d, mana %d -> %d", user->GetName(), GetName(), sound,
             healthBefore, user->Health(), manaBefore, user->Mana());

    // REVSYNC: 0x0050e9f5..0x0050ea2e. One is used up: the last of a stack
    // leaves its holder by name (DeleteFromInventory, vtable 0x78: the first
    // item of that name in the holder's inventory, bags included, which is
    // this one unless two stacks share the name) and is deleted; a bigger
    // stack loses one. An item lying on the map has no holder and stays.
    if (TObjectInstance* holder = GetOwner())
    {
        const int32_t amount = Amount();
        if (amount <= 1)
        {
            const std::string name = GetName();     // the item goes with the call
            holder->DeleteFromInventory(name.c_str(), 1);
        }
        else
            SetAmount(amount - 1);
    }

    // The item may be gone from here on.
    if (mainPlayer)
        TextBar.Print("%s", DialogList.GetLine("FOODGOOD1"));
    return true;
}
