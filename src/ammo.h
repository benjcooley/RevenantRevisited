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

#define MAXAMMOTYPES    4
#define MAXAMMOIMAGE    32

_CLASSDEF(TAmmo)
class TAmmo : public TObjectInstance
{
  public:
    TAmmo(PTObjectImagery newim) : TObjectInstance(newim) {}
    TAmmo(PSObjectDef def, PTObjectImagery newim) : TObjectInstance(def, newim) {}
    virtual ~TAmmo();

    static bool Initialize();
        // Set up static vars
    static void Close();
        // Clear static vars

    static void AllocInvItem(PTObjectImagery img, int32_t state, int32_t type, int32_t count);
        // Allocate a new inventory item for the given count and return it
    static void FreeInvItem(int32_t type, int32_t count);
        // Free up use of an instance of this count
    static void AllocGroundItem(PTObjectImagery img, int32_t state, int32_t type, int32_t count);
        // Allocate a new ground item for the given count and return it
    static void FreeGroundItem(int32_t type, int32_t count);
        // Free up use of an instance of this count

    virtual void Load(RTInputStream is, int32_t version, int32_t objversion);
        // Loads data from the sector
    virtual void Save(RTOutputStream os);
        // Saves data to the sector

    virtual void SignalAddedToInventory();
        // Called to signal object that it was added to a new inventory
    virtual void RemoveFromInventory();
        // Remove this object from whatever inventory it is in

    virtual void DrawInvItem(int32_t x, int32_t y);
        // Returns bitmap for the inventory image
    virtual PTBitmap InventoryImage();
        // Returns bitmap for the inventory image

    virtual void GetScreenRect(SRect &r);
        // Get screen bounding rectangle for object (in world coordinates)
    virtual void DrawUnlit(PTSurface surface);
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
