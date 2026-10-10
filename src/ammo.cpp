// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                       ammo.cpp - TAmmo module                         *
// *************************************************************************

#include "ammo.h"

#include "bitmap.h"
#include "character.h"
#include "dialog.h"
#include "editorstub.h"
#include "imagery.h"
#include "mappane.h"
#include "player.h"
#include "rules.h"
#include "sound.h"
#include "spell.h"
#include "textbar.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

extern TObjectClass EffectClass;

REGISTER_BUILDER(TAmmo)
TObjectClass AmmoClass("AMMO", OBJCLASS_AMMO, 0);

// Hard coded class stats (retail's, 0x004bf120..0x004bf2a0)
DEFSTAT(Ammo, EqSlot,       EQSL, 0, 8, 0, 10)
DEFSTAT(Ammo, Value,        VAL,  1, 0, 0, 1000000)
DEFSTAT(Ammo, Type,         TYPE, 2, 1, 0, 4)
DEFSTAT(Ammo, SaleType,     STYP, 3, 0, 0, 3)
DEFSTAT(Ammo, MagicType,    MTYP, 4, 0, 0, 10)
DEFSTAT(Ammo, DamageMod,    DMOD, 5, 0, -100, 100)
DEFSTAT(Ammo, Duration,     DURA, 6, 3, -1, 500)
DEFSTAT(Ammo, Stack,        STCK, 7, 0, 0, 1)

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


// ********************
// * Arrows in flight *
// ********************

namespace
{

// An arrow's spell on its victim: the victim's own spells cast it by name,
// with the shooter as invoker (retail then echoes the cast to the network,
// 0x00584870, which does nothing in a single-player game).
void Proc(TCharacter* victim, const char* spell, TCharacter* shooter)
{
    char name[RESNAMELEN];
    strncpy(name, spell, sizeof(name) - 1);
    name[sizeof(name) - 1] = 0;
    TObjectInstance* target = victim;
    victim->GetSpellManager()->CastByName(name, shooter, &target, 1, nullptr, nullptr);
}

}  // namespace

// REVSYNC: 0x004c00b0
void TAmmo::SetShooter(TObjectInstance* who)
{
    if (who)
        shooter = who;
}

// REVSYNC: TAmmo::Move @ 0x004c01f0
uint32_t TAmmo::Move()
{
    const uint32_t bits = flightSeam ? flightSeam(this) : TObjectInstance::Move();
    if (!(flags & OF_WEIGHTLESS))
        return bits;

    // The character where the arrow is; one far above or below it stops
    // everything this step, a wall included.
    TCharacter* victim = TCharacter::CharBlocking(this, pos, 0);
    if (victim && abs(victim->Pos().z - pos.z) > 80)
        return bits;
    if (killwait >= 0 || (!(bits & MOVE_BLOCKED) && !victim))
        return bits;

    const bool icearrow = !stricmp(GetTypeName(), "ice arrow");
    if (icearrow)
        IceAround();

    char sound[] = "impact*bow";
    sound[6] = char('1' + random(0, 2));
    PlayAt(sound, pos);

    // REVSYNC-DIVERGENCE: retail reads the class of whatever its shooter's
    // id gives, so a shooter removed while the arrow flew crashes it; here
    // that arrow has no shooter.
    TObjectInstance* by = shooter.Get();
    TCharacter* archer = (by && (by->ObjClass() == OBJCLASS_CHARACTER || by->ObjClass() == OBJCLASS_PLAYER))
                             ? static_cast<TCharacter*>(by)
                             : nullptr;
    if (!victim || (archer && !archer->IsEnemy(victim)))
    {
        killwait = 0;
        return bits;
    }

    // The knockback comes from two steps behind the arrow.
    const S3DPoint from(pos.x - 2 * vel.x, pos.y - 2 * vel.y, pos.z - 2 * vel.z);

    // AMMODATA: base, per level of a monster (or of a player a trap hits),
    // per level of a player shooting, per point of bow skill, the least
    // percent of it a hit does.
    const auto& ammo = Rules.ammodata;
    TPlayer* playerarcher = nullptr;
    int32_t base;
    if (archer && archer->ObjClass() == OBJCLASS_PLAYER)
    {
        playerarcher = static_cast<TPlayer*>(archer);
        const int32_t level = playerarcher->Level();
        const int32_t bows = playerarcher->GetStat("bows");
        base = ammo[0] + bows * ammo[3] + level * ammo[2];
        int32_t mod = DamageMod();
        if (TObjectInstance* bow = playerarcher->GetEquip(EQ_RANGEDWEAPON))
            mod += bow->GetStat("damagemod");
        base += mod * base / 100;
        playerarcher->AwardSkillExp(SK_BOWS, victim);
    }
    // A monster's level is retail's Value() (vtable +0x190), which no
    // character class answers: 0.
    else if (archer)
        base = archer->Value() * ammo[1] + ammo[0];
    else if (victim->ObjClass() == OBJCLASS_PLAYER)
        base = static_cast<TPlayer*>(victim)->Level() * ammo[1] + ammo[0];
    else
        base = victim->Value() * ammo[1] + ammo[0];

    const int32_t damage = random(base * ammo[4] / 100, base);
    if (damage != 0 && !icearrow)
    {
        if (MagicType() == DT_POISON && random(0, 100) < 33)
            Proc(victim, "Poison", archer);
        else if (MagicType() == DT_BURN && random(0, 100) < 66)
            Proc(victim, "Fire Flash", archer);
        victim->Damage(damage, MagicType(), 0, nullptr, nullptr);
        victim->KnockBack(from, -1);
    }

    // REVSYNC-DIVERGENCE: retail hands the text bar the message as its
    // format; here it is printed as text.
    if (playerarcher && playerarcher == Player)
    {
        char text[256];
        if (DialogList.FindLine("FULLARROWDMG") >= 0)
            snprintf(text, sizeof(text), DialogList.GetLine("FULLARROWDMG"), victim->GetName(), damage);
        else
            snprintf(text, sizeof(text), "%s %s %s:%d", DialogList.GetLine("BASEARROW"), victim->GetName(),
                     DialogList.GetLine("BASEDMG"), damage);
        TextBar.Print("%s", text);
    }

    if (archer)
        victim->SignalAttack(archer, victim, 1);
    SetFlags(flags | OF_KILL);
    return bits;
}

// The ice arrow's burst (in TAmmo::Move @ 0x004c01f0): an Iced effect at
// every Solifuge among the first ten characters the map finds within 200
// of the arrow, but the main player, the dead, one held in an interactive
// attack or impact, and one skywalking.
void TAmmo::IceAround()
{
    int32_t found[10];
    const int32_t n = MapPane.FindObjectsInRange(pos, found, 200, 0, -1, 10, OBJSET_CHARACTER);
    for (int32_t i = 0; i < n; i++)
    {
        auto* c = static_cast<TCharacter*>(MapPane.GetInstance(found[i]));
        if (!c || stricmp(c->GetTypeName(), "Solifuge") != 0 || c == Player || c->Health() <= 0 ||
            c->InteractiveLocked() || strstr(c->DoingName(), "skywalk"))
            continue;
        SObjectDef def{};
        def.objclass = OBJCLASS_EFFECT;
        def.objtype = EffectClass.FindObjType("Iced");
        def.level = MapPane.GetMapLevel();
        def.pos = c->Pos();
        TObjectInstance* iced = MapPane.GetInstance(MapPane.NewObject(&def));
        if (!iced)
            continue;
        if (!iced->HasAnimator())
            iced->CreateAnimator();
        // Retail then gives the Iced effect its target (0x004ec930: the
        // target paralysed and iced, "FREEZECUBE" played at it). The port
        // has no Iced effect yet (effects), so nobody is frozen.
    }
}

// REVSYNC: TAmmo::Pulse @ 0x004c0880
void TAmmo::Pulse()
{
    if (killwait > 0)
        killwait--;
    if (killwait == 0 || (!Editor && !GetOwner() && vel.x == 0 && vel.y == 0 && vel.z == 0))
        SetFlags(flags | OF_KILL);
    TObjectInstance::Pulse();
}
