// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                       trap.cpp - TTrap module                         *
// *************************************************************************

#include "revenant.h"
#include "trap.h"
#include "mappane.h"
#include "sound.h"

REGISTER_BUILDER(TTrap)

TObjectClass TrapClass("TRAP", OBJCLASS_TRAP, 0);

// **************
// * TArrowWall *
// **************

_CLASSDEF(TArrowWall)
class TArrowWall : public TTrap
{
  public:
    TArrowWall(PTObjectImagery newim) : TTrap(newim) { Init(); }
    TArrowWall(PSObjectDef def, PTObjectImagery newim) : TTrap(def, newim) { Init(); }

    void Init();

    virtual bool Use(PTObjectInstance user, int32_t with);
    virtual int32_t CursorType(PTObjectInstance with = nullptr) { return CURSOR_NONE; }

    virtual void Pulse();

  protected:
    int32_t frame;                  // what frame of fire sequence
    int32_t lastshot;               // last frame shot was fired
    int32_t wallfacing;             // which way to fire arrows
};

DEFINE_BUILDER("ArrowWallS", TArrowWall)
REGISTER_BUILDER(TArrowWall)

_CLASSDEF(TArrowWall2)
class TArrowWall2 : public TArrowWall
{
  public:
    TArrowWall2(PTObjectImagery newim) : TArrowWall(newim) { }
    TArrowWall2(PSObjectDef def, PTObjectImagery newim) : TArrowWall(def, newim) { }
};

DEFINE_BUILDER("ArrowWallE", TArrowWall2)
REGISTER_BUILDER(TArrowWall2)


void TArrowWall::Init()
{
    if (stricmp(GetName(), "ArrowWallS") == 0)
        wallfacing = 128;
    else
        wallfacing = 64;

    frame = -1;
}


bool TArrowWall::Use(PTObjectInstance user, int32_t with)
{
    if (user == this && with == -1)
    {
        frame = 0;
        lastshot = -1;
        SoundPlayer.Mount("arrow");
    }

    return false;
}

#define ARROW_FREQUENCY     12
#define ARROW_RANDOMNESS    5
#define ARROW_SPEED         (20 * ROLLOVER)
#define MAX_ARROWS          7

void TArrowWall::Pulse()
{
    extern TObjectClass AmmoClass;

    if (frame >= 0)
    {
        if (frame++ > (MAX_ARROWS * ARROW_FREQUENCY))
        {
            SoundPlayer.Unmount("arrow");
            frame = -1;
        }
        else
        {
            if ((frame + random(0, ARROW_RANDOMNESS)) > (lastshot + ARROW_FREQUENCY))
            {
                lastshot = frame;

                // generate a new arrow and fire it
                SObjectDef def;
                memset(&def, 0, sizeof(SObjectDef));
                def.objclass = OBJCLASS_AMMO;
                def.objtype = AmmoClass.FindObjType("Arrow3D");
                def.flags = OF_WEIGHTLESS;

                GetPos(def.pos);
                def.pos.x += random(-50, 50);
                def.pos.y += random(-50, 50);
                def.pos.z += 50 + random(-30, 30);
                def.level = MapPane.GetMapLevel();
                ConvertToVector(wallfacing, ARROW_SPEED, def.vel);
                def.facing = wallfacing;

                MapPane.NewObject(&def);

                SoundPlayer.Play("arrow");
            }
        }
    }
}

