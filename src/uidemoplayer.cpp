// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uidemoplayer.cpp - demo subject for the --test=ui-* HUD modes        *
// *************************************************************************

#include "uidemoplayer.h"

#include "logging.h"
#include "player.h"
#include "time.h"

#include <cmath>

extern TObjectClass PlayerClass;

namespace UIDemoPlayer
{
namespace
{

// The sample player the panes were built against: Locke from
// docs/ui/sample_screen_1.jpg / plyr_stats_panel.png. The port's rules give
// a level-26 Locke smaller maxima than that retail capture, so the bars are
// filled to the sample's ratios (1833/1930, 2174/2650, 191/380) of the
// player's own maxima rather than to the sample's numbers.
struct SSampleStats
{
    int32_t level       = 0;
    int32_t exp         = 0;
    int32_t attributes[NUM_PLRSTATS] = {};   // Strn..Luck; 0 keeps the class default
    double  healthFill  = 1.0;
    double  manaFill    = 1.0;
    double  fatigueFill = 1.0;
};
constexpr SSampleStats kPlayerSample = {
    26, 12500, { 18, 16, 14, 12, 10, 8 },
    1833.0 / 1930.0, 2174.0 / 2650.0, 191.0 / 380.0,
};
constexpr SSampleStats kOpponentSample = { 18 };   // bars swept by Pulse
constexpr const char*  kOpponentName   = "Vermis";

// The sample kit: what the equipment, inventory and belt panes were built
// showing. Equipment is worn through TPlayer::Equip; carried items sit at
// carried slots, the belt at slot 0x10b + n. Food and potions don't stack
// (their classes have no Amount), so only gold shows a count.
struct SSampleItem
{
    const char* name  = nullptr;
    int32_t     slot  = 0;        // inventory slot, or EQ_* for kEquipment
    int32_t     amount = 1;
};
constexpr SSampleItem kEquipment[] = {
    { "Brown Leather Helmet",      EQ_HEAD         },
    { "Emerald Collar",            EQ_NECK         },
    { "Brown Leather Chest Plate", EQ_BODY         },
    { "Brown Leather Gloves",      EQ_OFFHAND      },
    { "Short Sword",               EQ_PRIMEHAND    },
    { "Emerald Ring",              EQ_R_ACCESSORY  },
    { "Torch",                     EQ_L_ACCESSORY  },
    { "Light Bow",                 EQ_RANGEDWEAPON },
    { "Arrow",                     EQ_AMMO         },
    { "Brown Leather Leg Plate",   EQ_LEGS         },
    { "Brown Leather Boots",       EQ_FEET         },
};
// The first page of the grid, plus the rest of the 14475 gold on page two.
constexpr SSampleItem kCarried[] = {
    { "Brown Leather Helmet", 0 },
    { "Ale Mug",              1 },
    { "Golden Sun Key",       2 },
    { "Gold",                 3, 42 },
    { "Lesser Healing",       4 },
    { "Golden Sun Key",       5 },
    { "Gold",                12, 10000 },
    { "Gold",                13, 4433 },
};
constexpr SSampleItem kBelt[] = {
    { "Lesser Healing", kInvSlotBeltFirst + 0 },
    { "Ale Mug",        kInvSlotBeltFirst + 1 },
    { "Pouch",          kInvSlotBeltFirst + 2 },
    { "Golden Sun Key", kInvSlotBeltFirst + 3 },
};
constexpr const char* kPouchContents     = "Lesser Healing";
constexpr int32_t     kPouchContentCount = 7;

// Opponent cycle: engaged for 5 s, released for 2 s; its bars sweep on a
// 4 s triangle wave between these fractions of its maxima.
constexpr double kCycleSeconds    = 7.0;
constexpr double kEngagedSeconds  = 5.0;
constexpr double kSweepSeconds    = 4.0;
constexpr double kHealthRange[2]  = { 0.25, 0.85 };
constexpr double kManaRange[2]    = { 0.15, 0.85 };
constexpr double kFatigueRange[2] = { 0.40, 0.90 };

TPlayer* g_player   = nullptr;
TPlayer* g_opponent = nullptr;

TPlayer* NewLocke(const char* name)
{
    const int32_t objtype = PlayerClass.FindObjType("Locke");
    if (objtype < 0)
    {
        log_warn("[ui-demo] no player type 'Locke'");
        return nullptr;
    }

    SObjectDef def = {};
    def.objclass = OBJCLASS_PLAYER;
    def.objtype  = static_cast<short>(objtype);
    auto* player = dynamic_cast<TPlayer*>(PlayerClass.NewObject(&def));
    if (!player)
    {
        log_warn("[ui-demo] could not build a player 'Locke'");
        return nullptr;
    }
    if (name)
        player->SetName(const_cast<char*>(name));
    player->OnScreen();    // stream the imagery body (portrait, paperdoll)
    return player;
}

// Level and attributes first: the maxima scale with them.
void ApplySample(TPlayer* player, const SSampleStats& sample)
{
    player->SetLevel(sample.level);
    player->SetExp(sample.exp);
    void (TPlayer::*const setters[NUM_PLRSTATS])(int32_t) = {
        &TPlayer::SetStrn, &TPlayer::SetCons, &TPlayer::SetAgil,
        &TPlayer::SetRflx, &TPlayer::SetMind, &TPlayer::SetLuck,
    };
    for (int32_t i = 0; i < NUM_PLRSTATS; ++i)
        if (sample.attributes[i] > 0)
            (player->*setters[i])(sample.attributes[i]);
    player->SetHealth(int32_t(player->MaxHealth() * sample.healthFill));
    player->SetMana(int32_t(player->MaxMana() * sample.manaFill));
    player->SetFatigue(int32_t(player->MaxFatigue() * sample.fatigueFill));
}

// Build the item type of that name (whatever its class) into `owner`'s
// inventory at `slot`.
TObjectInstance* AddItem(TObjectInstance* owner, const char* name, int32_t slot,
                         int32_t amount = 1)
{
    for (int32_t c = 0; c < MAXOBJECTCLASSES; ++c)
    {
        TObjectClass* cl = TObjectClass::GetClass(c);
        const int32_t type = cl ? cl->FindObjType(name) : -1;
        if (type < 0)
            continue;

        SObjectDef def = {};
        def.objclass = static_cast<short>(cl->ClassId());
        def.objtype  = static_cast<short>(type);
        TObjectInstance* item = cl->NewObject(&def);
        if (!item)
            break;
        if (amount != 1)
            item->SetAmount(amount);
        if (owner->AddToInventory(item, slot))
            return item;
        delete item;
        break;
    }
    log_warn("[ui-demo] could not add '%s' at slot %d", name, slot);
    return nullptr;
}

void AddSampleKit(TPlayer* player)
{
    for (const SSampleItem& entry : kEquipment)
        if (TObjectInstance* item = AddItem(player, entry.name, -1))
            if (!player->Equip(item, entry.slot))
                log_warn("[ui-demo] '%s' doesn't fit equipment slot %d", entry.name, entry.slot);
    for (const SSampleItem& entry : kCarried)
        AddItem(player, entry.name, entry.slot, entry.amount);
    for (const SSampleItem& entry : kBelt)
        if (TObjectInstance* item = AddItem(player, entry.name, entry.slot);
            item && stricmp(entry.name, "Pouch") == 0)
            for (int32_t i = 0; i < kPouchContentCount; ++i)
                AddItem(item, kPouchContents, -1);
}

void DeletePlayer(TPlayer*& player)
{
    if (!player)
        return;
    player->OffScreen();
    delete player;      // ~TPlayer removes it from the player manager
    player = nullptr;
}

double Lerp(const double range[2], double t)
{
    return range[0] + (range[1] - range[0]) * t;
}

} // namespace

bool Install()
{
    if (g_player)
        return true;

    g_player = NewLocke(nullptr);
    if (!g_player)
        return false;
    ApplySample(g_player, kPlayerSample);
    AddSampleKit(g_player);
    PlayerManager.AddPlayer(g_player);
    PlayerManager.SetMainPlayer(g_player);

    g_opponent = NewLocke(kOpponentName);
    if (g_opponent)
    {
        ApplySample(g_opponent, kOpponentSample);
        if (!g_player->BeginCombat(g_opponent))
            log_warn("[ui-demo] the demo player cannot enter combat; no target card");
    }

    log_info("[ui-demo] demo player '%s' L%d H%d/%d M%d/%d F%d/%d installed",
             g_player->GetName(), g_player->Level(),
             g_player->Health(), g_player->MaxHealth(),
             g_player->Mana(), g_player->MaxMana(),
             g_player->Fatigue(), g_player->MaxFatigue());
    return true;
}

void Pulse()
{
    if (!g_player || !g_opponent || !g_player->IsFighting())
        return;

    const double t = TTime::Time();
    const bool engaged = std::fmod(t, kCycleSeconds) < kEngagedSeconds;
    g_player->SetFighting(engaged ? g_opponent : nullptr);

    const double u   = t / kSweepSeconds;
    const double tri = 1.0 - 2.0 * std::fabs(u - std::floor(u + 0.5));
    g_opponent->SetHealth(int32_t(g_opponent->MaxHealth() * Lerp(kHealthRange, tri)));
    g_opponent->SetMana(int32_t(g_opponent->MaxMana() * Lerp(kManaRange, tri)));
    g_opponent->SetFatigue(int32_t(g_opponent->MaxFatigue() * Lerp(kFatigueRange, tri)));
}

void Remove()
{
    if (g_player && g_player->IsFighting())
        g_player->SetFighting(nullptr);
    DeletePlayer(g_opponent);
    DeletePlayer(g_player);
}

} // namespace UIDemoPlayer
