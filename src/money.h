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
    TMoney(TObjectImagery* newim) : TObjectInstance(newim) { }
    TMoney(SObjectDef* def, TObjectImagery* newim) : TObjectInstance(def, newim) {}
    virtual ~TMoney();

    static bool Initialize();
        // Set up static vars
    static void Close();
        // Clear static vars

    static void AllocGroundItem(PTBitmap ground, int32_t type, int32_t count);
        // Allocate a new ground item for the given count and return it
    static void FreeGroundItem(int32_t type, int32_t count);
        // Free up use of an instance of this count

    virtual bool Use(TObjectInstance* user, int32_t with = -1);
        // Combine money
    int32_t CursorType(TObjectInstance* with = nullptr) override;
        // Returns type of cursor that should appear when mouse arrow is over the object

    virtual void Load(RTInputStream is, int32_t version, int32_t objversion);
        // Loads data from the sector
    virtual void Save(RTOutputStream os);
        // Saves data to the sector

    bool MergeInto(TObjectInstance* newowner) override;
        // REVSYNC: 0x00515b50. Joins newowner's gold pile
    SInvIcon InventoryIcon() override;
        // REVSYNC: 0x00516370. A pile of copies of the inventory image

    virtual void GetScreenRect(SRect &r);
        // Get screen bounding rectangle for object (in world coordinates)
    virtual void DrawUnlit(TSurface* surface);
        // Returns bitmap for the inventory image
    virtual PTBitmap GetStillImage(int32_t ostate = -1);
        // Returns still image

    // Money stats
    STATFUNC(Value)
    OBJSTAT(Amount)
    virtual int32_t Amount() { return GetObjStat(se_Amount.id); }
    virtual void SetAmount(int32_t amt);
        // REVSYNC: 0x00515c90. Also shows the pile's size in its state

};

DEFINE_BUILDER("MONEY", TMoney)

