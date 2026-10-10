// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                        ammo.h - TAmmo module                          *
// *************************************************************************

#pragma once

#include "revenant.h"

#include "imagery.h"
#include "object.h"

#include <cstdint>

#define AT_MISC     0
#define AT_ARROW    1
#define AT_BOLT     2
#define AT_HAND     3

#define MAXAMMOTYPES    16      // REVSYNC: retail's limit (0x004bfda0 errors at 16 types)
#define MAXAMMOIMAGE    32

// Ammunition, and the arrow in flight: ResolveBowShoot makes a weightless
// AMMO object of the equipped ammo's type, and its Move hits what it meets.
// Retail's five arrow builders ("Arrow", "Fire Arrow", "Poison Arrow", "Ice
// Arrow", "Magic Arrow", vtables 0x005a6b68..0x005a73f8) differ only in their
// destructors, so every AMMO type is a TAmmo here.
_CLASSDEF(TAmmo)
class TAmmo : public TObjectInstance
{
  public:
    // REVSYNC: TAmmo::TAmmo @ 0x004c01b0 -- a moving, pulsed object
    TAmmo(TObjectImagery* newim) : TObjectInstance(newim) { flags |= OF_MOVING | OF_PULSE; }
    TAmmo(SObjectDef* def, TObjectImagery* newim) : TObjectInstance(def, newim) { flags |= OF_MOVING | OF_PULSE; }
    ~TAmmo() override;

    static bool Initialize();
        // Set up static vars
    static void Close();
        // Clear static vars

    static void AllocGroundItem(TObjectImagery* img, int32_t state, int32_t type, int32_t count);
        // Allocate a new ground item for the given count and return it
    static void FreeGroundItem(int32_t type, int32_t count);
        // Free up use of an instance of this count

    void Load(RTInputStream is, int32_t version, int32_t objversion) override;
        // Loads data from the sector
    void Save(RTOutputStream os) override;
        // Saves data to the sector

    SInvIcon InventoryIcon() override;
        // REVSYNC: 0x004bfda0. A stack of copies of the inventory image

    void GetScreenRect(SRect &r) override;
        // Get screen bounding rectangle for object (in world coordinates)
    void DrawUnlit(TSurface* surface) override;
        // Returns bitmap for the inventory image
    PTBitmap GetStillImage(int32_t ostate = -1) override;
        // Returns still image

    uint32_t Move() override;
      // REVSYNC: TAmmo::Move @ 0x004c01f0 -- the base move, then a flying
      // arrow's hit: the character it meets (or a wall) ends its flight;
      // an enemy of the shooter takes the arrow's damage
    void Pulse() override;
      // REVSYNC: TAmmo::Pulse @ 0x004c0880 -- an arrow that hit nothing is
      // removed; so is one lying still outside any inventory
    void SetShooter(TObjectInstance* who);
      // REVSYNC: 0x004c00b0 -- who shot it (retail keeps its id at +0xd8)

    // Retail A/B fixtures only: when set, it answers the base move
    // (TObjectInstance::Move, retail 0x00470920) a flying arrow makes.
    using FlightSeam = uint32_t (*)(TAmmo* self);
    static inline FlightSeam flightSeam = nullptr;

    // Ammo stats (retail ids 0-7, AmmoClass 0x0066b160)
    int32_t EqSlot() override { return GetStat(se_EqSlot.id); }
    void SetEqSlot(int32_t v) override { SetStat(se_EqSlot.id, v); }
    int32_t Value() override { return GetStat(se_Value.id); }
    void SetValue(int32_t v) override { SetStat(se_Value.id, v); }
    STATFUNC(Type)
    STATFUNC(SaleType)
    STATFUNC(MagicType)     // the arrow's damage type
    STATFUNC(DamageMod)     // percent added to a player's arrow damage
    STATFUNC(Duration)
    STATFUNC(Stack)
    OBJSTAT(Amount)
    int32_t Amount() override { return GetObjStat(se_Amount.id); }
    void SetAmount(int32_t amt) override;

    [[nodiscard]] int32_t KillWait() const { return killwait; }
    [[nodiscard]] TObjectInstance* Shooter() const { return shooter.Get(); }

  protected:
    int32_t killwait = -1;               // +0xdc: -1 while it may still hit; 0: remove it

  private:
    static SStatEntry se_EqSlot;
    static SStatEntry se_Value;

    void IceAround();
      // An ice arrow's hit: an Iced effect on each Solifuge near it

    TSafeRef<TObjectInstance> shooter;   // +0xd8: who shot it (none: a trap's)
};

DEFINE_BUILDER("AMMO", TAmmo)
