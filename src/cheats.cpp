// *************************************************************************
// *                         Revenant Revisited                            *
// *              cheats.cpp - The cheat words of the text bar             *
// *************************************************************************

#include "cheats.h"

#include "food.h"
#include "player.h"
#include "potion.h"
#include "revenant.h"
#include "spell.h"
#include "talisman.h"

#include <algorithm>
#include <cstring>

bool CheatLookUnderTheHood = false;
bool CheatDebug = false;

namespace {

constexpr int32_t kAlchemyGold   = 999999;
constexpr int32_t kNoAmnesiaLevel = 30;
constexpr int32_t kTopUpAmount   = 5;
constexpr const char* kSpellPouch = "spell pouch";     // 0x005e58e0

ECheatResult Toggle(bool& flag)
{
    flag = !flag;
    return flag ? ECheatResult::Enabled : ECheatResult::Disabled;
}

// potionsnlotions, gimmesomegrub: at least five of every type of the class
// (POTION, class 0x0066d268; FOOD, 0x0066d2a8), added when missing.
// Retail reads through a hole in the type array; the port skips one.
void TopUp(const TObjectClass& cl)
{
    for (int32_t i = 0; i < cl.NumTypes(); ++i)
    {
        const SObjectInfo* type = cl.GetObjType(i);
        if (!type || !type->name)
            continue;
        if (TObjectInstance* item = Player->FindObjInventory(type->name))
            item->SetAmount((std::max)(item->Amount(), kTopUpAmount));
        else
            Player->AddToInventory(type->name, kTopUpAmount);
    }
}

// abracadabra: one of every talisman, gathered into a spell pouch (added
// when missing; the player himself when none can be had), and every spell
// variant learned. Retail learns through the spellbook pane (0x00544fb0 on
// 0x0065a9d8); the port's spellbook reads the player's known spells.
void GiveAllMagic()
{
    TObjectInstance* pouch = Player->FindObjInventory(kSpellPouch);
    if (!pouch)
    {
        Player->AddToInventory(kSpellPouch);
        pouch = Player->FindObjInventory(kSpellPouch);
        if (!pouch)
            pouch = Player;
    }

    for (int32_t i = 0; i < TalismanClass.NumTypes(); ++i)
    {
        const SObjectInfo* type = TalismanClass.GetObjType(i);
        if (!type || !type->name)
            continue;
        TObjectInstance* talisman = Player->FindObjInventory(type->name);
        if (!talisman)
        {
            pouch->AddToInventory(type->name);
            talisman = Player->FindObjInventory(type->name);
        }
        if (talisman && talisman->GetOwner() != pouch)
            pouch->AddToInventory(talisman);
    }

    for (int32_t s = 0; s < SpellList.NumSpells(); ++s)
    {
        SSpellData* spell = SpellList.GetSpellData(s);
        for (int32_t v = 0; spell && v < spell->variants.NumItems(); ++v)
            Player->LearnSpell(spell->variants[v].talismans);
    }
}

}  // namespace

// REVSYNC: TTextBar::SubmitInput @ 0x0054d700, its single-player cheat words.
// `alchemy` and `noamnesia` say "enabled" even with no player.
ECheatResult ApplyCheatWord(const char* word)
{
    if (!word)
        return ECheatResult::Unknown;

    if (!stricmp(word, "alreadydead"))
        return Toggle(CheatAlreadyDead);
    if (!stricmp(word, "alchemy"))
    {
        if (Player)
            Player->SetMoney(kAlchemyGold);
        return ECheatResult::Enabled;
    }
    if (!stricmp(word, "nahkranoth"))
        return Toggle(CheatNahkranoth);
    if (!stricmp(word, "noamnesia"))
    {
        if (Player)
        {
            Player->SetLevel(kNoAmnesiaLevel);
            Player->SetAttackLevel(Player->Level());
        }
        return ECheatResult::Enabled;
    }
    if (!stricmp(word, "lookunderthehood"))
        return Toggle(CheatLookUnderTheHood);
    if (!stricmp(word, "dummies"))
        return Toggle(NoAI);
    if (!stricmp(word, "abracadabra"))
    {
        MagicCheat = !MagicCheat;
        if (Player)
            GiveAllMagic();
        return MagicCheat ? ECheatResult::Enabled : ECheatResult::Disabled;
    }
    if (!stricmp(word, "potionsnlotions"))
    {
        if (Player)
            TopUp(PotionClass);
        return ECheatResult::Enabled;
    }
    if (!stricmp(word, "gimmesomegrub"))
    {
        if (Player)
            TopUp(FoodClass);
        return ECheatResult::Enabled;
    }
    if (!stricmp(word, "debug"))
    {
        CheatDebug = CheatLookUnderTheHood = !CheatDebug;
        return CheatDebug ? ECheatResult::Enabled : ECheatResult::Disabled;
    }
    return ECheatResult::Unknown;
}
