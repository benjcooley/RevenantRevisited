// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                       Exit.cpp - TExit object                         *
// *************************************************************************

#include "exit.h"

#include "3dimage.h"
#include "dls.h"
#include "file.h"
#include "mappane.h"
#include "parse.h"
#include "player.h"
#include "playscreen.h"
#include "savegame.h"
#include "script.h"
#include "sound.h"
#include "textbar.h"

REGISTER_BUILDER(TExit)
TObjectClass ExitClass("EXIT", OBJCLASS_EXIT, 0);

// Hard coded class stats
DEFSTAT(Exit, Openable,             OPEN, 0, 0, 0, 1)
DEFSTAT(Exit, Facing,               FACE, 1, 0, 0, 255)
DEFSTAT(Exit, UseCenter,            USE,  2, 0, 0, 2)
DEFSTAT(Exit, StopMoving,           STMV, 3, 0, 1, 1)
DEFSTAT(Exit, Delay,                DLY,  4, 0, 0, 1000)

// Hard coded object stats
DEFOBJSTAT(Exit, Locked,            LOCK, 0, 0, 0, 1)
DEFOBJSTAT(Exit, KeyId,             KEY,  1, 0, 0, 100000)
DEFOBJSTAT(Exit, PickDifficulty,    PICK, 2, 0, 0, 100000)


// these should be static members of TExit, but it doesn't seem to recognize
// their existance when I do it that way, so here they are
PSExitRef exitlist;     // master list of exits
bool exitlistdirty;     // if we need to save out the exit list

bool TExit::Initialize()
{
    exitlistdirty = false;
    return ReadExitList();
}

void TExit::Close()
{
    WriteExitList();
    DestroyExitList();
}

bool TExit::ReadExitList(bool reload)
{
    if (!reload)
        exitlist = nullptr;

    char fname[MAXPATHLEN];
    sprintf(fname, "%sexit.def", ClassDefPath);
    
    FILE *fp = TryOpen(fname, "rb");
    if (!fp)
        return false;

    TFileParseStream s(fp, fname);
    TToken t(s);

    t.Get();

    PSExitRef ref;
    char name[128];

    do
    {
        if (t.Type() == TKN_RETURN || t.Type() == TKN_WHITESPACE)
            t.LineGet();

        if (t.Type() == TKN_EOF)
            break;

        ref = new SExitRef;

        if (!Parse(t, "%t (%d, %d, %d) level %d mapindex %d ambient %d (%d, %d, %d)",
                    name, &ref->target.x, &ref->target.y, &ref->target.z, &ref->level, &ref->mapindex,
                    &ref->ambient, &ref->ambcolor.red, &ref->ambcolor.green, &ref->ambcolor.blue))
            return false;

        if (reload)
        {
            // don't overwrite any that already exist in the loaded version
            if (FindExit(name))
            {
                delete ref;
                ref = nullptr;
            }
        }

        if (ref)
        {
            ref->name = strdup(name);
            ref->next = exitlist;
            exitlist = ref;
        }

        t.SkipLine();       // skip past any other garbage on the line, including the newline

    } while (t.Type() != TKN_EOF);

    fclose(fp);

    exitlistdirty = false;
    return true;
}

bool TExit::WriteExitList()
{
    if (!exitlistdirty)
        return true;        // nothing to do, no changes have been made since last load/save

    if (!ReadExitList(true))    // get any exits that have been added since last load
        return false;

    char fname[MAXPATHLEN];
    sprintf(fname, "%sexit.def", ClassDefPath);
    
    FILE *fp = TryOpen(fname, "wb");
    if (!fp)
        return false;

    for (PSExitRef ref = exitlist; ref; ref = ref->next)
        if (!fprintf(fp, "%s (%d, %d, %d) level %d mapindex 0x%x ambient %d (%d, %d, %d)\r\n",
                        ref->name, ref->target.x, ref->target.y, ref->target.z, ref->level, ref->mapindex,
                        ref->ambient, ref->ambcolor.red, ref->ambcolor.green, ref->ambcolor.blue))
        {
            fclose(fp);
            return false;
        }

    fclose(fp);
    exitlistdirty = false;
    return true;
}

void TExit::DestroyExitList()
{
    PSExitRef next;

    for (PSExitRef ref = exitlist; ref; ref = next)
    {
        next = ref->next;

        delete ref->name;
        delete ref;
    }

    exitlist = nullptr;
    exitlistdirty = false;
}

void TExit::GetExitStrip(int32_t &regx, int32_t &regy, int32_t &regz, int32_t &width, int32_t &length, int32_t &height)
{
    // get all the bounding box data
    GetFacingBoundBox(regx, regy, width, length);

    int32_t dummy;
    GetImagery()->GetWorldBoundBox(state, dummy, dummy, height);

    height = max(1, height);

    if (UseCenter())
    {
        // special case for objects that use the center as the activation area
        if (UseCenter() == 2)
        {
            // rotating walls
            regx = (regx / 4) * 3;
            regy = (regy + 1) / 2;
            width = (width / 4) * 3;
            length = (length + 1) / 2;
        }
        else
        {
            // elevators and teleporters
            regx = (regx / 2);
            regy = (regy / 2);
            width /= 2;
            length /= 2;
        }

        return;
    }

    // get the facing data
    int32_t dir = Facing();
    if (dir < 0)
        dir = GetFace();

    // find the strip based on the direction the exit is facing
    if (dir >= 0xE0 || dir < 0x20)
    {
        // north-facing exit
        //regy += 1;
        length = 1;
    }
    else if (dir < 0x60)
    {
        // east-facing exit
        regx -= width - 1;
        width = 1;
    }
    else if (dir < 0xA0)
    {
        // south-facing exit
        regy -= length - 1;
        length = 1;
    }
    else if (dir < 0xE0)
    {
        // west-facing exit
        //regx += 1;
        width = 1;
    }
}

bool TExit::AddExit(char *name, TObjectInstance* inst, bool getamb)
{
    if (!name || !*name || !inst)
        return false;

    PSExitRef ref = FindExit(name);

    // if it already exists we can skip some stuff
    if (!ref)
    {
        ref = new SExitRef;
        ref->name = strdup(name);

        ref->next = exitlist;
        exitlist = ref;
    }

    if (inst->ObjClass() != OBJCLASS_EXIT)
    {
        inst->GetPos(ref->target);
        ref->mapindex = -1;
    }
    else
    {
        if (inst->GetImagery() == nullptr)
            memset(&ref->target, 0, sizeof(S3DPoint));
        else
        {
            int32_t regx, regy, regz, width, length, height;
            ((PTExit)inst)->GetExitStrip(regx, regy, regz, width, length, height);
            S3DPoint start(0, 0, 0);
            start.x -= regx * GRIDSIZE;
            start.y -= regy * GRIDSIZE;
            S3DPoint end = start;
            end.x += width * GRIDSIZE;
            end.y += length * GRIDSIZE;

            ref->target.x = (start.x + end.x) / 2;
            ref->target.y = (start.y + end.y) / 2;
            ref->target.z = (start.z + end.z) / 2;
        }

        S3DPoint pos;
        inst->GetPos(pos);
        ref->target += pos;

        // remember to close the door on the way out...
        ref->mapindex = inst->GetMapIndex();

        if (getamb)
        {
            ref->ambient = MapPane.GetAmbientLight();
            GetAmbientColor(ref->ambcolor);
        }
        else
        {
            ref->ambcolor.red = ref->ambcolor.green = ref->ambcolor.blue = 255;
            ref->ambient = -1;
        }
    }

    ref->level = MapPane.GetMapLevel();     // hrm...no level on objects...is that bad?

    exitlistdirty = true;

    return true;
}

PSExitRef TExit::FindExit(char *exitname)
{
    for (PSExitRef ref = exitlist; ref; ref = ref->next)
        if (stricmp(ref->name, exitname) == 0)
            return ref;

    return nullptr;
}

bool TExit::Use(TObjectInstance* user, int32_t with)
{
    TObjectInstance::Use(user, with);

    if (Openable())
    {
        if (CheckKeyUse(user, MapPane.GetInstance(with)))
            return true;

        if (Locked())
        {
            TextBar.Print("It seems to be locked.");
            return false;
        }

        if (state == EXIT_OPEN || state == EXIT_OPENING)
            SetExitState(EXIT_CLOSING);
        else if (state == EXIT_CLOSED || state == EXIT_CLOSING)
            SetExitState(EXIT_OPENING);

        return true;
    }

    return false;
}

int32_t TExit::CursorType(TObjectInstance* with)
{
    if (Openable())
    {
        if (with)
            return CURSOR_HAND;

        return CURSOR_DOOR;
    }

    return CURSOR_NONE;
}

bool TExit::Activate()
{
    if (GetScript())
        GetScript()->Trigger(TRIGGER_ACTIVATE);

    if (exitflags & EX_FROMEXIT)        // if we just came from an exit, don't reflect back
        return false;

    PSExitRef ref = FindExit(name);     // find this exit in the master list
    if (!ref)
        return false;

    S3DPoint targ = ref->target;

    // minor hack, for now
    if (stricmp(GetTypeName(), "Door") == 0 && Player)
    {
        S3DPoint vect;
        ConvertToVector(Player->GetFace(), 24, vect);
        targ += vect;
    }

    // Set new position
    Player->SetPos(targ, ref->level);

    return true;
}

void TExit::Unactivate()
{
    if (Openable())
        SetExitState(EXIT_CLOSING);
}

bool TExit::SetExitState(int32_t es)
{
    int32_t st;

    if (es == EXIT_OPEN)
        st = FindState("open");
    else if (es == EXIT_CLOSED)
        st = FindState("closed");
    else if (es == EXIT_OPENING)
    {
        st = FindState("opening");
        if (st < 0)
            st = FindState("closed to open");
    }
    else if (es == EXIT_CLOSING)
    {
        st = FindState("closing");
        if (st < 0)
            st = FindState("open to closed");
    }

    if (st < 0)
        st = es;

    return SetState(st);
}

void TExit::Pulse()
{
    TContainer::Pulse();

    if (!Editor && CommandDone() && Openable())
    {
        if (state == EXIT_CLOSING)
            SetExitState(EXIT_CLOSED);
        else if (state == EXIT_OPENING)
            SetExitState(EXIT_OPEN);
    }

    if (Player && !Editor && GetImagery())
    {
        int32_t regx, regy, regz, width, length, height;
        GetExitStrip(regx, regy, regz, width, length, height);

        // get the player's relative position to the exit
        S3DPoint delta;
        Player->GetPos(delta);
        delta -= pos;
        delta.x = (delta.x + (regx * GRIDSIZE)) / GRIDSIZE;
        delta.y = (delta.y + (regy * GRIDSIZE)) / GRIDSIZE;
        delta.z = (delta.z + (GetImagery()->GetWorldRegZ(state) * GRIDSIZE)) / GRIDSIZE;

        bool activate = true;
//      if (StopMoving())
//      {
//          // for usecenter exits, wait until the character stops moving
//          S3DPoint lnextmove;
//          Player->GetNextMove(lnextmove);
//          if (lnextmove.x != 0 || lnextmove.y != 0 || lnextmove.z != 0)
//              activate = false;
//      }

        // check if the player is over the strip of walkmap immediately past
        // the bounding box in the given direction
        if (delta.x >= 0 && delta.y >= 0 && /*delta.z >= 0 &&*/
            delta.x < width && delta.y < length/* && delta.z < height*/)
        {
            if (Player->IsOnExit() && !(exitflags & EX_ON))
                exitflags |= EX_FROMEXIT; // Looks like we just poped here from another exit

            Player->SetOnExit(); // Indicate we're on an exit
            exitflags |= EX_ON;

            if (activate &&                     // Activation enabled
                !(exitflags & EX_ACTIVATED))    // Hasn't already been activated
            {
                if (wait++ > Delay())
                {
                    wait = 0;
                    if (Activate())
                        exitflags |= EX_ACTIVATED;
                }
            }
        }
        else
        {
            if (exitflags & EX_ACTIVATED)
                Unactivate();

            exitflags &= ~(EX_ON | EX_ACTIVATED | EX_FROMEXIT);
        }
    }
}

void TExit::UseRange(int32_t &mindist, int32_t &maxdist, int32_t &minang, int32_t &maxang)
{
}

void TExit::Load(RTInputStream is, int32_t version, int32_t objversion)
{
    TContainer::Load(is, version, objversion);
    is >> exitflags;
}

void TExit::Save(RTOutputStream os)
{
    TContainer::Save(os);
    os << exitflags;
}

// ***************
// * TPressPlate *
// ***************

#define PLATE_UP        0
#define PLATE_DOWN      1

_CLASSDEF(TPressPlate)
class TPressPlate : public TExit
{
  public:
    TPressPlate(TObjectImagery* newim) : TExit(newim) { }
    TPressPlate(SObjectDef* def, TObjectImagery* newim) : TExit(def, newim) { }

    virtual bool Use(TObjectInstance* user, int32_t with = -1) { return false; }
    virtual int32_t CursorType(TObjectInstance* with = nullptr) { return CURSOR_NONE; }

    virtual bool Activate();
    virtual void Unactivate();
};

DEFINE_BUILDER("PressPlate", TPressPlate)
REGISTER_BUILDER(TPressPlate)

bool TPressPlate::Activate()
{
    TExit::Activate();

    SetState(PLATE_DOWN);

    return true;
}

void TPressPlate::Unactivate()
{
    SetState(PLATE_UP);
}

// ************
// * TUpBlock *
// ************

_CLASSDEF(TUpBlock)
class TUpBlock : public TExit
{
  public:
    TUpBlock(TObjectImagery* newim) : TExit(newim) { }
    TUpBlock(SObjectDef* def, TObjectImagery* newim) : TExit(def, newim) { }

    virtual bool Use(TObjectInstance* user, int32_t with = -1);
    virtual int32_t CursorType(TObjectInstance* with = nullptr) { return CURSOR_NONE; }

    virtual void Pulse();
};

DEFINE_BUILDER("UpBlock", TUpBlock)
REGISTER_BUILDER(TUpBlock)

void TUpBlock::Pulse()
{
    if (!Editor && CommandDone())
    {
        if (state == EXIT_CLOSING)
            SetState(EXIT_CLOSED);
        else if (state == EXIT_OPENING)
            SetState(EXIT_OPEN);
    }

    TExit::Pulse();
}

bool TUpBlock::Use(TObjectInstance* user, int32_t with)
{
    if (with == -1)
    {
        if (state == EXIT_CLOSING || state == EXIT_CLOSED)
            SetState(EXIT_OPENING);
        else if (state == EXIT_OPENING || state == EXIT_OPEN)
            SetState(EXIT_CLOSING);

        PLAY("grind rock");

        return true;
    }

    return false;
}

// **********************
// * TDragonEntAnimator *
// **********************

_CLASSDEF(TDragonEntAnimator)
class TDragonEntAnimator : public T3DAnimator
{
  public:
    TDragonEntAnimator(TObjectInstance* oi) : T3DAnimator(oi) { }
    virtual ~TDragonEntAnimator() { Close(); }

    virtual void Animate(bool draw);
    virtual bool Render();
    virtual void GetExitStrip(int32_t &regx, int32_t &regy, int32_t &regz, int32_t &width, int32_t &length, int32_t &height);
};

REGISTER_3DANIMATOR("DragonEnt", TDragonEntAnimator)

void TDragonEntAnimator::GetExitStrip(int32_t &regx, int32_t &regy, int32_t &regz, int32_t &width, int32_t &length, int32_t &height)
{
    regx = 2;
    regy = 2;
    width = 4;
    length = 4;
}

void TDragonEntAnimator::Animate(bool draw)
{
    T3DAnimator::Animate(draw);
}

bool TDragonEntAnimator::Render()
{
    /*
    uint32_t savedcull;
    TRY_D3D(Device2->GetRenderState(D3DRENDERSTATE_CULLMODE, &savedcull));

    TRY_D3D(Device2->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE));

    TRY_D3D(Device2->SetRenderState(D3DRENDERSTATE_CULLMODE, savedcull));
    */

    T3DAnimator::Render();

    return true;
}

// **************
// * TSpikeWall *
// **************

_CLASSDEF(TSpikeWall)
class TSpikeWall : public TExit
{
  public:
    TSpikeWall(TObjectImagery* newim) : TExit(newim) { }
    TSpikeWall(SObjectDef* def, TObjectImagery* newim) : TExit(def, newim) { }

    virtual bool Use(TObjectInstance* user, int32_t with = -1) { return false; }
    virtual int32_t CursorType(TObjectInstance* with = nullptr) { return CURSOR_NONE; }

    virtual bool Activate();
    virtual void Unactivate();
};

DEFINE_BUILDER("SpikeWall", TSpikeWall)
REGISTER_BUILDER(TSpikeWall)

bool TSpikeWall::Activate()
{
    TExit::Activate();
    
    if (Player)
    {
        Player->Force("impale");
        Player->Damage(10000, DAMAGE_PIERCING);     // make sure he's good n' dead
    }

    SetState(EXIT_OPENING);

    PLAY("spike");

    return true;
}

void TSpikeWall::Unactivate()
{
    SetState(EXIT_CLOSING);

    PLAY("spike");
}

// ***************
// * TLever      *
// ***************

REGISTER_BUILDER(TLever)

bool TLever::Use(TObjectInstance* user, int32_t with)
{
    TExit::Use(user, with);

    return true;
}                                              

void TLever::Pulse()
{
    TExit::Pulse();

    if (Editor)
        return;

/*  if (AtActivatePos() == ACTIVATE_IMTHERE)
    {
        S3DPoint newpos;

        Player->Face( (usedir * 64));   // 0 = NE, SE, SW, NW
        Player->GetPos( newpos);
        newpos.x = targetpos.x;
        newpos.y = targetpos.y;
        Player->SetPos( newpos);
        ((PTCharacter)Player)->Pull( this);
        return;
    }
*/
}

