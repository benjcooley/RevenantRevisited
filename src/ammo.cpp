// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                       ammo.cpp - TAmmo module                         *
// *************************************************************************

#include "ammo.h"

#include "bitmap.h"
#include "character.h"
#include "imagery.h"
#include "mappane.h"
#include "sound.h"

REGISTER_BUILDER(TAmmo)
TObjectClass AmmoClass("AMMO", OBJCLASS_AMMO, 0);

// Hard coded class stats
DEFSTAT(Ammo, EqSlot,       EQSL, 0, 8, 0, 10)
DEFSTAT(Ammo, Value,        VAL,  1, 0, 0, 1000000)
DEFSTAT(Ammo, Type,         TYPE, 2, 1, 0, 4)

// Hard coded object stats
DEFOBJSTAT(Ammo, Amount,    AMT,  0, 0, 0, 1000)

// These were originally static members of TAmmo but the compiler wasn't
// very hip on that, so they are now here.
PTBitmap ammogrounditem[MAXAMMOTYPES][MAXAMMOIMAGE];    // Ground images
int32_t ammogroundusecount[MAXAMMOTYPES][MAXAMMOIMAGE]; // Use count for ground

#define IMGAMOUNT(x)    ((x) < MAXAMMOIMAGE ? (x) : ((x) - (MAXAMMOIMAGE/2)) % (MAXAMMOIMAGE/2) + (MAXAMMOIMAGE/2))

namespace {

// REVSYNC: 0x004bf990 -- each copy in an inventory icon sits AmmoPos less this.
constexpr int32_t kAmmoNudge = 4;

// REVSYNC: 0x004bfda0 / 0x004bfcb0 -- how many arrows an inventory icon
// shows: a stack of "Arrow" its amount (16..31 over again past 31), any
// other ammo one.
int32_t InvImageCount(TAmmo& ammo)
{
    return stricmp(ammo.GetName(), "Arrow") == 0 ? IMGAMOUNT(ammo.Amount()) : 1;
}

}  // namespace

struct { int32_t num, x, y; } AmmoPos[MAXAMMOIMAGE] =
{ { 0, 4, 4 }, { 0, 5, 7 }, { 0, 1, 5 },
  { 0, 4, 6 }, { 0, 3, 2 }, { 0, 8, 5 },
  { 0, 3, 0 }, { 0, 8, 8 }, { 0, 5, 2 },
  { 0, 7, 8 }, { 0, 6, 4 }, { 0, 2, 6 },
  { 0, 6, 3 }, { 0, 2, 0 }, { 0, 1, 5 },
  { 0, 5, 1 },
  { 0, 4, 4 }, { 0, 5, 7 }, { 0, 1, 5 },
  { 0, 4, 6 }, { 0, 3, 2 }, { 0, 8, 5 },
  { 0, 3, 0 }, { 0, 8, 8 }, { 0, 5, 2 },
  { 0, 7, 8 }, { 0, 6, 4 }, { 0, 2, 6 },
  { 0, 6, 3 }, { 0, 2, 0 }, { 0, 1, 5 },
  { 0, 5, 1 }
};

bool TAmmo::Initialize()
{
    for (int32_t t = 0; t < MAXAMMOTYPES; t++)
        for (int32_t i = 0; i < MAXAMMOIMAGE; i++)
        {
            ammogrounditem[t][i] = nullptr;
            ammogroundusecount[t][i] = 0;
        }

    return true;
}

void TAmmo::Close()
{
    for (int32_t t = 0; t < MAXAMMOTYPES; t++)
        for (int32_t i = 0; i < MAXAMMOIMAGE; i++)
        {
            if (ammogroundusecount[t][i])
            {
                ammogroundusecount[t][i] = 1;
                FreeGroundItem(t, i);
            }
        }
}

TAmmo::~TAmmo()
{
    int32_t count = IMGAMOUNT(Amount());

    if (ammogroundusecount[objtype][count])
        FreeGroundItem(objtype, count);
}

void TAmmo::SetAmount(int32_t amt)
{
    if (Amount() == amt || amt < 1)
        return;

    if (!GetOwner())
    {
        FreeGroundItem(objtype, Amount());
        if (imagery)
            AllocGroundItem(imagery, state, objtype, amt);
    }

    SetObjStat(se_Amount.id, amt);
}


void TAmmo::Load(RTInputStream is, int32_t version, int32_t objversion)
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

void TAmmo::Save(RTOutputStream os)
{
    TObjectInstance::Save(os);
}

void TAmmo::AllocGroundItem(TObjectImagery* img, int32_t state, int32_t type, int32_t count)
{
    if (!img || !img->GetStillImage(state))
        return;                                 // 3D imagery has no still image to build it from

    count = min(count, MAXAMMOIMAGE-1);

    if (ammogroundusecount[type][count] < 1)
    {
        ammogrounditem[type][count] = TBitmap::NewBitmap(INVITEMREALWIDTH/2, INVITEMREALHEIGHT/2, BM_8BIT | BM_PALETTE);

        memset(ammogrounditem[type][count]->data16, 0, (INVITEMREALWIDTH/2)*(INVITEMREALHEIGHT/2));
        memcpy(ammogrounditem[type][count]->palette.ptr(), img->GetStillImage(state)->palette.ptr(), img->GetStillImage(state)->palettesize);

        for (int32_t i = 0; i < count+1; i++)
            ammogrounditem[type][count]->Put(AmmoPos[i].x >> 1, AmmoPos[i].y >> 1, img->GetStillImage(state, AmmoPos[i].num), DM_TRANSPARENT);

        ammogroundusecount[type][count] = 1;
    }
    else
        ammogroundusecount[type][count]++;
}

void TAmmo::FreeGroundItem(int32_t type, int32_t count)
{
    count = min(count, MAXAMMOIMAGE-1);

    if (ammogroundusecount[type][count] == 0)
        return;

    if (--(ammogroundusecount[type][count]) < 1 && ammogrounditem[count])
    {
        delete ammogrounditem[type][count];
        ammogrounditem[type][count] = nullptr;
    }
}

// REVSYNC: 0x004bfda0 / 0x004bf990 -- the stack retail composed into its
// inventory icon: InvImageCount copies of the state's inventory image, each
// at AmmoPos less 4.
SInvIcon TAmmo::InventoryIcon()
{
    SInvIcon icon;
    TBitmap* arrow = imagery ? imagery->GetInvImage(state, 0) : nullptr;
    for (int32_t i = 0, count = InvImageCount(*this); i < count; i++)
        icon.Add(arrow, AmmoPos[i].x - kAmmoNudge, AmmoPos[i].y - kAmmoNudge);
    return icon;
}

void TAmmo::GetScreenRect(SRect &r)
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

void TAmmo::DrawUnlit(TSurface* surface)
{
    int32_t count = IMGAMOUNT(Amount());

    if (ammogroundusecount[objtype][count] < 1 && imagery)
        AllocGroundItem(imagery, state, objtype, count);

    imagery->DrawUnlit(this, surface);
}

PTBitmap TAmmo::GetStillImage(int32_t ostate)
{
    return ammogrounditem[objtype][IMGAMOUNT(Amount())];
}


// ************
// * TArrow3D *
// ************

_CLASSDEF(TArrow3D)
class TArrow3D : public TObjectInstance
{
  public:
    TArrow3D(TObjectImagery* newim) : TObjectInstance(newim) { killwait = -1; }
    TArrow3D(SObjectDef* def, TObjectImagery* newim) : TObjectInstance(def, newim) { killwait = -1; }

    virtual uint32_t Move();
    virtual void Pulse();

    int32_t killwait;
};

DEFINE_BUILDER("Arrow3D", TArrow3D)
REGISTER_BUILDER(TArrow3D)

uint32_t TArrow3D::Move()
{
    uint32_t bits = TObjectInstance::Move();

    TObjectInstance* inst = (TObjectInstance*)TCharacter::CharBlocking(this, pos);

    if (killwait < 0 && ((bits & MOVE_BLOCKED) || inst))
    {
        // collision!

        PLAY("arrow impact");       // play sound

        if (inst)
        {
            // hit character
            inst->Damage(random(20, 50), DAMAGE_PIERCING);
        }

        killwait = FRAMERATE * 3;
    }

    return bits;
}

void TArrow3D::Pulse()
{
    if (killwait > 0)
        killwait--;
    
    if (killwait == 0)
        SetFlags(OF_KILL);
}
