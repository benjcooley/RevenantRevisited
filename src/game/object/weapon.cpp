// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     weapon.cpp - TWeapon module                       *
// *************************************************************************

#include "game/app/revenant.h"
#include "game/object/weapon.h"
#include "game/world/mappane.h"
#include "game/ui/textbar.h"

REGISTER_OBJECT_DEF(TWeapon)
TObjectClass WeaponClass("WEAPON", OBJCLASS_WEAPON, 0);

DEFSTAT(Weapon, EqSlot,             EQSL, 0, 3, 0, 9)
DEFSTAT(Weapon, Type,               TYPE, 1, WT_HAND, WT_HAND, WT_LAST)
DEFSTAT(Weapon, Damage,             DMG,  2, 0, 0, 10000)
DEFSTAT(Weapon, Combining,          COMB, 3, 0, 0, 16)
DEFSTAT(Weapon, Poison,             PSN,  4, 0, 0, 32)
DEFSTAT(Weapon, Value,              VAL,  5, 0, 0, 1000000)
// REVSYNC: the weapon class's stats as retail registers them (0x00528b90..),
// DamageType the seventh (0x00528cb0)
DEFSTAT(Weapon, DamageType,         DTYP, 6, -1, -1, 9)
DEFSTAT(Weapon, MinStrength,        MSTR, 7, 0, 0, 100)

#define VIAL_STATE_FULL     0
#define VIAL_STATE_EMPTY    1

void TWeapon::ClearWeapon()
{
}

bool TWeapon::Use(TObjectInstance* user, int32_t with)
{
    TObjectInstance* inst = MapPane.GetInstance(with);

    if (inst && strcmp(inst->GetName(), "Poison Vial") == 0 &&
        inst->GetState() == VIAL_STATE_FULL && Type() == WT_KNIFE)
    {
        inst->SetState(VIAL_STATE_EMPTY);
        TextBar.Print("%s envenomed.", GetName());

        return true;
    }

    return false;
}

int32_t TWeapon::CursorType(TObjectInstance* with)
{
    if (with && strcmp(with->GetName(), "Poison Vial") == 0 &&
        with->GetState() == VIAL_STATE_FULL && Type() == WT_KNIFE)
        return CURSOR_HAND;

    return CURSOR_NONE;
}

