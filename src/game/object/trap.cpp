// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                       trap.cpp - TTrap module                         *
// *************************************************************************

#include "game/app/revenant.h"
#include "game/object/trap.h"
#include "game/world/mappane.h"
#include "game/audio/sound.h"

REGISTER_OBJECT_DEF(TTrap)

TObjectClass TrapClass("TRAP", OBJCLASS_TRAP, 0);

// **************
// * TArrowWall *
// **************

_CLASSDEF(TArrowWall)
class TArrowWall : public TTrap
{
  public:
    TArrowWall(TObjectImagery* newim) : TTrap(newim) { Init(); }
    TArrowWall(SObjectState* def, TObjectImagery* newim) : TTrap(def, newim) { Init(); }

    void Init();

    virtual bool Use(TObjectInstance* user, int32_t with);
    int32_t CursorType(TObjectInstance* with = nullptr) override { return CURSOR_NONE; }

    virtual void Pulse();

  protected:
    int32_t frame;                  // what frame of fire sequence
    int32_t lastshot;               // last frame shot was fired
    int32_t wallfacing;             // which way to fire arrows
};

DEFINE_OBJECT_DEF("ArrowWallS", TArrowWall)
REGISTER_OBJECT_DEF(TArrowWall)

_CLASSDEF(TArrowWall2)
class TArrowWall2 : public TArrowWall
{
  public:
    TArrowWall2(TObjectImagery* newim) : TArrowWall(newim) { }
    TArrowWall2(SObjectState* def, TObjectImagery* newim) : TArrowWall(def, newim) { }
};

DEFINE_OBJECT_DEF("ArrowWallE", TArrowWall2)
REGISTER_OBJECT_DEF(TArrowWall2)


void TArrowWall::Init()
{
    if (stricmp(GetName(), "ArrowWallS") == 0)
        wallfacing = 128;
    else
        wallfacing = 64;

    frame = -1;
}


bool TArrowWall::Use(TObjectInstance* user, int32_t with)
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
                SObjectState def;
                memset(&def, 0, sizeof(SObjectState));
                def.objclass = OBJCLASS_AMMO;
                def.objtype = AmmoClass.FindObjType("Arrow3D");
                def.flags = OF_WEIGHTLESS;

                def.pos = IntPos(*this);
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

