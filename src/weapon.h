// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      weapon.h - TWeapon module                        *
// *************************************************************************

#pragma once

#include "revenant.h"

#include "object.h"

// Weapon type defines
#define WT_HAND         0           // Hand, claw, tail, etc.   
#define WT_KNIFE        1           // Daggers, knives
#define WT_SWORD        2           // Swords
#define WT_BLUDGEON     3           // Clubs, maces, hammers
#define WT_AXE          4           // Axes
#define WT_STAFF        5           // Staffs, polearms, spears, etc.
#define WT_BOW          6           // Bow
#define WT_CROSSBOW     7           // Crossbow
#define WT_LAST         7           // Last weapon type

// Weapon mask defines (Note: make sure these don't match Windows WM_ messages)
#define WM_HAND         0x0001
#define WM_KNIFE        0x0002
#define WM_SWORD        0x0004
#define WM_BLUDGEON     0x0008
#define WM_AXE          0x0010
#define WM_STAFF        0x0020
#define WM_BOW          0x0040
#define WM_CROSSBOW     0x0080

#define MAX_WEAPONTYPES 6

_CLASSDEF(TWeapon)
class TWeapon : public TObjectInstance
{
  public:
    TWeapon(TObjectImagery* newim) : TObjectInstance(newim) { ClearWeapon(); }
    TWeapon(SObjectDef* def, TObjectImagery* newim) : TObjectInstance(def, newim) { ClearWeapon(); }

    _NODEFAULTCONS(TWeapon);

    void ClearWeapon();

    virtual bool Use(TObjectInstance* user, int32_t with = -1);
    virtual int32_t CursorType(TObjectInstance* with = nullptr);

    virtual void Load(RTInputStream is, int32_t version, int32_t objversion);
    virtual void Save(RTOutputStream os);

  // Weapon stats
    STATFUNC(EqSlot)
    STATFUNC(Type)
    STATFUNC(Damage)
    STATFUNC(Combining)
    STATFUNC(Poison)
    STATFUNC(Value)

  protected:
    int32_t poison;             // how much posion is currently on the blade
};

DEFINE_BUILDER("WEAPON", TWeapon)
