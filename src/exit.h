// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                        Exit.h - TExit object                          *
// *************************************************************************
//
// Doors, stairs, teleport stones, levers: objects that move a player
// somewhere else. The shipped game's behaviour (retail vtable 0x5b27cc) is
// documented in docs/gameflow/forensics/EXITS.md; this is a port of it.
//
// - The exit list (exit.def) maps an exit's instance name to a destination
//   position and level. It lives as long as a game session.
// - A player standing on an exit's strip activates it, unless the exit's
//   type is one of the scripted doors; a scripted exit only asks its script
//   for ACTIVATE.
// - Doors are opened by scripts (`operate`), never by Use, which checks the
//   lock and runs the USE trigger.

#pragma once

#include "container.h"

// Exit states (retail SetExitState 0x0050d530): the state order of
// Door1.I3D. EXIT_OPENING / EXIT_CLOSING are the 1998 names; retail's code
// that uses them (TUpBlock 0x0050dae0 / 0x0050da80) was built against these
// values, so they name the "out" states.
enum
{
    EXIT_OPENINGOUT,
    EXIT_OPEN,
    EXIT_CLOSINGOUT,
    EXIT_CLOSED,
    EXIT_OPENINGIN,
    EXIT_CLOSINGIN,

    EXIT_OPENING = EXIT_OPENINGOUT,
    EXIT_CLOSING = EXIT_CLOSINGOUT,
};

// Exit is derived from container in order to get the lock functionality
_CLASSDEF(TExit)
class TExit : public TContainer
{
  public:
    // REVSYNC: Build @ 0x0050e360 -- exits never move and always pulse.
    TExit(TObjectImagery* newim) : TContainer(newim) { flags |= OF_IMMOBILE | OF_PULSE; }
    TExit(SObjectDef* def, TObjectImagery* newim) : TContainer(def, newim) { flags |= OF_IMMOBILE | OF_PULSE; }

  // The exit list (exit.def), read and written as a session starts and ends.
    // REVSYNC: 0x0050c880 -- read the active module's exit.def, else the shared one.
    static bool Initialize();
    // REVSYNC: 0x0050c8a0 -- write the list if the editor changed it, then free it.
    static void Close();
    // REVSYNC: AddExit @ 0x0050cfc0 (editor `exit`) -- the list entry `name`
    // takes inst's place: the middle of its strip for an exit, else its position.
    static bool AddExit(const char *name, TObjectInstance* inst, bool getamb = true);
    // REVSYNC: WriteExitList @ 0x0050cca0 -- only when changed.
    static bool WriteExitList();

    void Load(RTInputStream is, int32_t version, int32_t objversion) override;
    void Save(RTOutputStream os) override;

    // REVSYNC: 0x0050d640 -- state auto-step, then every player on the strip.
    void Pulse() override;
    // REVSYNC: 0x0050d1a0 -- lock check, then the USE trigger. Never opens.
    bool Use(TObjectInstance* user, int32_t with = -1) override;
    // REVSYNC: 0x0050d370
    int32_t CursorType(TObjectInstance* with = nullptr) override;
    // REVSYNC: 0x0050d980 (empty, as 1998)
    void UseRange(int32_t &mindist, int32_t &maxdist, int32_t &minang, int32_t &maxang) override;

    // REVSYNC: 0x0050d3a0 (vtable 0x248) -- take `user` (null: the main
    // player) through the exit. Unforced, a scripted exit only asks its
    // script for ACTIVATE and an unscripted one needs AutoActivate; forced
    // (`activate`) goes straight to the exit list. True when something
    // happened or a script was asked.
    virtual bool Activate(TObjectInstance* user, bool forced);
    // REVSYNC: 0x0050d510 (vtable 0x24c) -- only TLever's Pulse calls it.
    virtual void Unactivate();
    // REVSYNC: 0x0050d230 (vtable 0x250, `operate`) -- open or close, away
    // from `user`.
    virtual void Operate(TObjectInstance* user);
    // REVSYNC: 0x0050d2b0 (`isoutside`) -- obj is behind the exit's facing.
    [[nodiscard]] bool IsOutside(const TObjectInstance* obj);

    // REVSYNC: 0x0050d530 -- the state named for `es` (EXIT_xxx), else `es`.
    void SetExitState(int32_t es);

    // REVSYNC: setfromexit @ 0x00428a40 -- keeps only the "arrived on the
    // strip already on an exit" bit. Retail's `&=`; nothing reads the bits on
    // a TExit (TLever's "activated" bit is what it clears).
    void SetFromExit() { exitflags &= kExitArrivedOnExit; }

    // Exit stats (registrations 0x0050c6a0..0x0050c850)
    STATFUNC(Openable)
    STATFUNC(Facing)
    STATFUNC(UseCenter)
    STATFUNC(StopMoving)
    STATFUNC(Delay)
    STATFUNC(TileFlags)
    OBJSTATFUNC(Locked)
    OBJSTATFUNC(KeyId)
    OBJSTATFUNC(PickDifficulty)
    OBJSTATFUNC(AutoActivate)

  protected:
    // exitflags (+0xe4). Pulse recomputes bits 0 and 2 every tick.
    static constexpr uint32_t kExitPlayerOn      = 1 << 0;  // a player is on the strip
    static constexpr uint32_t kExitActivated     = 1 << 1;  // TLever: activated, not yet left
    static constexpr uint32_t kExitArrivedOnExit = 1 << 2;  // that player was already on an exit

    // REVSYNC: 0x0050ce70 (vtable 0x27c) -- the walkmap cells, relative to
    // the exit, a player activates it from (1998, unchanged).
    virtual void GetExitStrip(int32_t &regx, int32_t &regy, int32_t &regz, int32_t &width, int32_t &length, int32_t &height);
    // `pos` lies on the strip (z isn't tested; EXITS.md §1.4).
    [[nodiscard]] bool OnStrip(const S3DPoint& at, int32_t regx, int32_t regy, int32_t width, int32_t length) const;
    // The start of Pulse: a finished OPENING/CLOSING animation moves on to
    // open/closed. TLever steps CLOSING to `closedstate` (EXITS.md §2.3).
    void StepAnimation(int32_t closedstate, bool needsmultiframe);

    uint32_t exitflags = 0;     // +0xe4
    int32_t  wait      = 0;     // +0xe8, TLever's delay counter
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

// Retail vtable 0x5b2a4c: keeps the 1998 exit model (main player only,
// Delay counter, Unactivate on leaving) in its own Pulse.
_CLASSDEF(TLever)
class TLever : public TExit
{
  public:
    TLever(TObjectImagery* newim) : TExit(newim) {}
    TLever(SObjectDef* def, TObjectImagery* newim) : TExit(def, newim) {}

    // REVSYNC: 0x0050dc20 -- TExit::Use, and always handled.
    bool Use(TObjectInstance* user, int32_t with = -1) override;
    // REVSYNC: 0x0050dc40
    void Pulse() override;
    // REVSYNC: 0x0050de60 -- raw states; the user doesn't matter.
    void Operate(TObjectInstance* user) override;

    S3DPoint targetpos;     // +0xf8 (unused by retail; TCharacter::Pull reads its z)
    int32_t  usedir = 3;    // +0x104, 0 = NE, SE, SW, NW
};

DEFINE_BUILDER("LEVER", TLever)
