// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                        Exit.h - TExit object                          *
// *************************************************************************

#pragma once

#include "revenant.h"

#include "container.h"
#include "object.h"

enum
{
    EXIT_CLOSED,
    EXIT_OPEN,
    EXIT_CLOSING,
    EXIT_OPENING,
};

_STRUCTDEF(SExitRef)
struct SExitRef
{
    char *name;             // name of exit
    S3DPoint target;        // position on level
    int32_t level;              // level to change to
    int32_t mapindex;           // object character is transfered to (usually another exit)
    int32_t ambient;            // level of ambient light
    SColor ambcolor;        // color of ambient light

    PSExitRef next;         // next in list
};

// Exit flags
#define EX_ON           (1 << 0)        // player is on exit strip
#define EX_ACTIVATED    (1 << 1)        // exit has been activated
#define EX_FROMEXIT     (1 << 2)        // player just came from another exit.. don't do anything

// Exit is derived from container in order to get the lock functionality
_CLASSDEF(TExit)
class TExit : public TContainer
{
  public:
    TExit(PTObjectImagery newim) : TContainer(newim) { flags |= OF_PULSE; exitflags = 0;  wait = 0; }
    TExit(PSObjectDef def, PTObjectImagery newim) : TContainer(def, newim) { flags |= OF_PULSE; exitflags = 0; wait = 0; }

    static bool Initialize();
        // Set up static vars
    static void Close();
        // Clear static vars

    virtual void Load(RTInputStream is, int32_t version, int32_t objversion);
        // Loads data from the sector
    virtual void Save(RTOutputStream os);
        // Saves data to the sector

    static bool AddExit(char *name, PTObjectInstance inst, bool getamb = true);
        // Add an exit from name to inst, closing inst if it is an exit, and getting ambient color and level
    static bool WriteExitList();
        // Save exit list

    virtual bool Activate();
        // Take the player through the exit
    virtual void Unactivate();
        // After they leave, do whatever

    virtual void Pulse();

    virtual bool Use(PTObjectInstance user, int32_t with = -1);
        // Open or close the exit
    virtual int32_t CursorType(PTObjectInstance with = nullptr);
        // Show that they can enter this object
    virtual void UseRange(int32_t &mindist, int32_t &maxdist, int32_t &minang, int32_t &maxang);
        // Character needs to be standing in front of the door

    bool SetExitState(int32_t es);
        // Sets the exit state given the EXIT_XXX macro defines

  // Functions to prevent reflecting exits
    void SetOn() { exitflags |= EX_ON | EX_ACTIVATED; }
        // Use this immediately after teleports to avoid screen flipping
    bool IsOn() { return (exitflags & EX_ON); }
        // Returns whether character is on the exit

    // Exit stats
    STATFUNC(Openable)
    STATFUNC(Facing)
    STATFUNC(UseCenter)
    STATFUNC(StopMoving)
    STATFUNC(Delay)
    OBJSTATFUNC(Locked)
    OBJSTATFUNC(KeyId)
    OBJSTATFUNC(PickDifficulty)

  protected:
    static bool ReadExitList(bool reload = false);
        // Read master list of exits
    static void DestroyExitList();
        // Delete exit list
    static PSExitRef FindExit(char *exitname);
        // Find a given exit
    virtual void GetExitStrip(int32_t &regx, int32_t &regy, int32_t &regz, int32_t &width, int32_t &length, int32_t &height);
        // Get the strip of walkmap that Locke steps on the activate the exit

    uint32_t exitflags;        // Exit flags
    int32_t wait;               // Wait until exit activates
};

DEFINE_BUILDER("EXIT", TExit)

enum
{
    LEVER_PULLSOUTH,
    LEVER_PUSHNORTH,
    LEVER_PULLNORTH,
    LEVER_PUSHSOUTH,
};

// ***************
// * TLever      *
// ***************

_CLASSDEF(TLever)
class TLever : public TExit
{
  public:
    TLever(PTObjectImagery newim) : TExit(newim) { usedir = 3; }    // 0 = NE, SE, SW, NW
    TLever(PSObjectDef def, PTObjectImagery newim) : TExit(def, newim) { usedir = 3; }

    virtual bool Use(PTObjectInstance user, int32_t with = -1);

    virtual void Pulse();

    S3DPoint targetpos;
    int32_t     usedir;
};

DEFINE_BUILDER("LEVER", TLever)
