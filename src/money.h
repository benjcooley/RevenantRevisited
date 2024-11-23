// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                       money.h - TMoney module                         *
// *************************************************************************

#pragma once

#include "revenant.h"
#include "object.h"

#define MAXMONEYTYPES   1
#define MAXMONEYIMAGE   64

_CLASSDEF(TMoney)
class TMoney : public TObjectInstance
{
  public:
    TMoney(PTObjectImagery newim) : TObjectInstance(newim) { }
    TMoney(PSObjectDef def, PTObjectImagery newim) : TObjectInstance(def, newim) {}
    virtual ~TMoney();

    static bool Initialize();
        // Set up static vars
    static void Close();
        // Clear static vars

    static void AllocInvItem(PTBitmap inv, int32_t type, int32_t count);
        // Allocate a new inventory item for the given count and return it
    static void FreeInvItem(int32_t type, int32_t count);
        // Free up use of an instance of this count
    static void AllocGroundItem(PTBitmap ground, int32_t type, int32_t count);
        // Allocate a new ground item for the given count and return it
    static void FreeGroundItem(int32_t type, int32_t count);
        // Free up use of an instance of this count

    virtual bool Use(PTObjectInstance user, int32_t with = -1);
        // Combine money
    virtual int32_t CursorType(PTObjectInstance with = nullptr);
        // Returns type of cursor that should appear when mouse arrow is over the object

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

    // Money stats
    STATFUNC(Value)
    OBJSTAT(Amount)
    virtual int32_t Amount() { return GetObjStat(se_Amount.id); }
    virtual void SetAmount(int32_t amt);  // We redefine this in Money.cpp

};

DEFINE_BUILDER("MONEY", TMoney)

