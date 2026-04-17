// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      money.cpp - TMoney module                        *
// *************************************************************************

#include "revenant.h"
#include "bitmap.h"
#include "imagery.h"
#include "money.h"
#include "mappane.h"
#include "inventory.h"

REGISTER_BUILDER(TMoney)
TObjectClass MoneyClass("MONEY", OBJCLASS_MONEY, 0);

// Hard coded class stats
DEFSTAT(Money, Value,       VAL, 0, 1, 1, 10000)

// Hard coded object stats
DEFOBJSTAT(Money, Amount,   AMT, 0, 1, 1, 10000)


// These were originally static members of TMoney but the compiler wasn't
// very hip on that, so they are now here.
PTBitmap invitem[MAXMONEYTYPES][MAXMONEYIMAGE]; // Bitmaps built on the fly
int32_t invusecount[MAXMONEYTYPES][MAXMONEYIMAGE];      // Use count for each image

PTBitmap grounditem[MAXMONEYTYPES][MAXMONEYIMAGE];  // Ground images
int32_t groundusecount[MAXMONEYTYPES][MAXMONEYIMAGE];   // Use count for ground

struct { int32_t x, y; } MoneyPos[MAXMONEYIMAGE] =
{ { 20, 28 }, { 13, 22 }, { 22, 18 }, { 22, 31 }, { 13, 35 }, { 0, 27 }, { 9, 25 },
  { 32, 27 }, { 5, 32 }, { 29, 30 }, { 12, 29 }, { 25, 24 }, { 6, 17 }, { 30, 34 }, { 19, 33 },
  // stacks
  { 19, 19 }, { 8, 21 }, { 15, 24 }, { 20, 17 }, { 9, 19 }, { 30, 23 }, { 20, 15 }, { 8, 17 }, 
  { 8, 15 }, { 16, 22 }, { 15, 20 }, { 19, 13 }, { 31, 21 }, { 31, 19 }, { 4, 25 }, { 21, 28 }, 
  { 19, 11 }, { 8, 13 }, { 9, 11 }, { 15, 18 }, { 14, 16 }, { 14, 14 }, { 30, 17 }, { 19, 9 },
  { 20, 7 }, { 5, 23 }, { 11, 28 }, { 30, 15 }, { 31, 13 }, { 22, 26 }, { 8, 9 }, { 5, 21 }, 
  { 29, 11 }, { 19, 5 }, { 18, 3 }, { 9, 7 }, { 19, 1 }, { 15, 12 }, { 29, 9 }, { 4, 19 },
  { 4, 17 }, { 21, 24 }, { 22, 22 }, { 10, 26 }, { 15, 10 }, { 30, 7 }, { 5, 15 }, { 22, 20 },
  { 10, 24 }
};

bool TMoney::Initialize()
{
    for (int32_t t = 0; t < MAXMONEYTYPES; t++)
        for (int32_t i = 0; i < MAXMONEYIMAGE; i++)
        {
            invitem[t][i] = grounditem[t][i] = nullptr;
            invusecount[t][i] = groundusecount[t][i] = 0;
        }

    return true;
}

void TMoney::Close()
{
    for (int32_t t = 0; t < MAXMONEYTYPES; t++)
        for (int32_t i = 0; i < MAXMONEYIMAGE; i++)
        {
            if (invusecount[t][i])
            {
                invusecount[t][i] = 1;
                FreeInvItem(t, i);
            }

            if (groundusecount[t][i])
            {
                groundusecount[t][i] = 1;
                FreeGroundItem(t, i);
            }
        }
}

TMoney::~TMoney()
{
    int32_t count = max(1, min(Amount(), MAXMONEYIMAGE-1));

    if (invusecount[objtype][count] && GetOwner() == Inventory.GetContainer())
        FreeInvItem(objtype, count);

    if (groundusecount[objtype][count])
        FreeGroundItem(objtype, count);
}

void TMoney::SignalAddedToInventory()
{
    TObjectInstance::SignalAddedToInventory();

    if (GetOwner() == Inventory.GetContainer() && imagery)
        AllocInvItem(imagery->GetInvImage(GetState()), objtype, Amount());
}

void TMoney::RemoveFromInventory()
{
    TObjectInstance::RemoveFromInventory();

    if (GetOwner() == Inventory.GetContainer())
        FreeInvItem(objtype, Amount());
}

void TMoney::SetAmount(int32_t amt)
{
    if (Amount() == amt || amt < 1)
        return;

    if (GetOwner() == Inventory.GetContainer())
    {
        FreeInvItem(objtype, Amount());
        if (imagery)
            AllocInvItem(imagery->GetInvImage(GetState()), objtype, amt);
    }

    if (!GetOwner())
    {
        FreeGroundItem(objtype, Amount());
        if (imagery)
            AllocGroundItem(imagery->GetStillImage(GetState()), objtype, amt);
    }

    SetObjStat(se_Amount.id, amt);
}

bool TMoney::Use(TObjectInstance* user, int32_t with)
{
    TObjectInstance* inst = MapPane.GetInstance(with);
    if (inst && inst->ObjClass() == ObjClass() && inst->ObjType() == ObjType() &&
        inst->Amount() < MAXMONEYIMAGE && Amount() < MAXMONEYIMAGE)
    {
        int32_t amt = inst->Amount() + Amount();

        if (amt > MAXMONEYIMAGE)
        {
            // keep both, just adjust their values
            SetAmount(MAXMONEYIMAGE);
            inst->SetAmount(amt - MAXMONEYIMAGE);
        }
        else
        {
            // nuke one, and combine values into the second
            MapPane.RemoveObject(with);
            delete inst;

            SetAmount(amt);
        }

        return true;
    }

    return false;
}

int32_t TMoney::CursorType(TObjectInstance* with)
{
    if (with && with->ObjClass() == ObjClass() && with->ObjType() == ObjType() &&
        with->Amount() < MAXMONEYIMAGE && Amount() < MAXMONEYIMAGE)
        return CURSOR_HAND;

    return CURSOR_NONE;
}

void TMoney::Load(RTInputStream is, int32_t version, int32_t objversion)
{
    TObjectInstance::Load(is, version, objversion);

    if (version < 5)
    {
        int32_t amount;
        is >> amount;
        amount++;
        SetStat("Amount", amount);
    }
}

void TMoney::Save(RTOutputStream os)
{
    TObjectInstance::Save(os);
}

void TMoney::AllocInvItem(PTBitmap inv, int32_t type, int32_t count)
{
    if (!inv)
        return;

    if (type >= MAXMONEYTYPES)
        Error("Only one type of money object 'Gold' allowed in class.def");

    count = max(1, min(count, MAXMONEYIMAGE-1));

    if (invusecount[type][count] < 1)
    {
        invitem[type][count] = TBitmap::NewBitmap(INVITEMREALWIDTH, INVITEMREALHEIGHT, BM_15BIT);

        memset(invitem[type][count]->data16, 0, INVITEMREALWIDTH*INVITEMREALHEIGHT*2);

        for (int32_t i = 0; i < count; i++)
            invitem[type][count]->Put(MoneyPos[i].x, MoneyPos[i].y, inv, DM_TRANSPARENT);

        invusecount[type][count] = 1;
    }
    else
        invusecount[type][count]++;
}

void TMoney::FreeInvItem(int32_t type, int32_t count)
{
    count = max(1, min(count, MAXMONEYIMAGE-1));

    if (invusecount[type][count] == 0)
        return;

    if (--(invusecount[type][count]) < 1 && invitem[count])
    {
        delete invitem[type][count];
        invitem[type][count] = nullptr;
    }
}

void TMoney::AllocGroundItem(PTBitmap ground, int32_t type, int32_t count)
{
    if (!ground)
        return;

    count = max(1, min(count, MAXMONEYIMAGE-1));

    if (groundusecount[type][count] < 1)
    {
        grounditem[type][count] = TBitmap::NewBitmap(INVITEMREALWIDTH/2, INVITEMREALHEIGHT/2, BM_8BIT | BM_PALETTE);

        memset(grounditem[type][count]->data16, 0, (INVITEMREALWIDTH/2)*(INVITEMREALHEIGHT/2));
        memcpy(grounditem[type][count]->palette.ptr(), ground->palette.ptr(), ground->palettesize);

        for (int32_t i = 0; i < count; i++)
            grounditem[type][count]->Put(MoneyPos[i].x >> 1, MoneyPos[i].y >> 1, ground, DM_TRANSPARENT);

        groundusecount[type][count] = 1;
    }
    else
        groundusecount[type][count]++;
}

void TMoney::FreeGroundItem(int32_t type, int32_t count)
{
    count = max(1, min(count, MAXMONEYIMAGE-1));

    if (groundusecount[type][count] == 0)
        return;

    if (--(groundusecount[type][count]) < 1 && grounditem[count])
    {
        delete grounditem[type][count];
        grounditem[type][count] = nullptr;
    }
}

void TMoney::DrawInvItem(int32_t x, int32_t y)
{
    int32_t count = max(1, min(Amount(), MAXMONEYIMAGE-1));

    if (invusecount[objtype][count] < 1 && imagery)
        AllocInvItem(imagery->GetInvImage(GetState()), objtype, count);

    imagery->DrawInvItem(this, x, y);
}

PTBitmap TMoney::InventoryImage()
{
    return invitem[objtype][max(1, min(Amount(), MAXMONEYIMAGE-1))];
}

void TMoney::GetScreenRect(SRect &r)
{
    if (imagery)
    {
        PSImageryStateHeader st = imagery->GetState(GetState());

        int32_t x, y;
        WorldToScreen(pos, x, y);

        r.left   = x - st->regx;
        r.right  = r.left + (INVITEMREALWIDTH / 2) - 1;
        r.top    = y - st->regy;
        r.bottom = r.top + (INVITEMREALHEIGHT / 2) - 1;
    }
}

void TMoney::DrawUnlit(TSurface* surface)
{
    int32_t count = min(Amount(), MAXMONEYIMAGE-1);

    if (groundusecount[objtype][count] < 1 && imagery)
        AllocGroundItem(imagery->GetStillImage(GetState()), objtype, count);

    imagery->DrawUnlit(this, surface);
}

PTBitmap TMoney::GetStillImage(int32_t ostate)
{
    return grounditem[objtype][min(Amount(), MAXMONEYIMAGE-1)];
}
