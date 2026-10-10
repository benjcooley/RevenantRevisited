// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                        ammo.h - TAmmo module                          *
// *************************************************************************

#pragma once

#include "revenant.h"

#include "imagery.h"
#include "object.h"

#define AT_MISC     0
#define AT_ARROW    1
#define AT_BOLT     2
#define AT_HAND     3

#define MAXAMMOTYPES    16      // REVSYNC: retail's limit (0x004bfda0 errors at 16 types)
#define MAXAMMOIMAGE    32

_CLASSDEF(TAmmo)
class TAmmo : public TObjectInstance
{
  public:
    TAmmo(TObjectImagery* newim) : TObjectInstance(newim) {}
    TAmmo(SObjectDef* def, TObjectImagery* newim) : TObjectInstance(def, newim) {}
    virtual ~TAmmo();

    static bool Initialize();
        // Set up static vars
    static void Close();
        // Clear static vars

    static void AllocGroundItem(TObjectImagery* img, int32_t state, int32_t type, int32_t count);
        // Allocate a new ground item for the given count and return it
    static void FreeGroundItem(int32_t type, int32_t count);
        // Free up use of an instance of this count

    virtual void Load(RTInputStream is, int32_t version, int32_t objversion);
        // Loads data from the sector
    virtual void Save(RTOutputStream os);
        // Saves data to the sector

    SInvIcon InventoryIcon() override;
        // REVSYNC: 0x004bfda0. A stack of copies of the inventory image

    virtual void GetScreenRect(SRect &r);
        // Get screen bounding rectangle for object (in world coordinates)
    virtual void DrawUnlit(TSurface* surface);
        // Returns bitmap for the inventory image
    virtual PTBitmap GetStillImage(int32_t ostate = -1);
        // Returns still image

    // Ammo stats
    STATFUNC(EqSlot)
    STATFUNC(Value)
    STATFUNC(Type)
    OBJSTAT(Amount)
    virtual int32_t Amount() { return GetObjStat(se_Amount.id); }
    virtual void SetAmount(int32_t amt);  // We redefine this in Ammo.cpp

};

DEFINE_BUILDER("AMMO", TAmmo)
