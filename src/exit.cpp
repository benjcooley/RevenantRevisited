// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                       Exit.cpp - TExit object                         *
// *************************************************************************
//
// Retail behaviour: docs/gameflow/forensics/EXITS.md. The 1998 bodies the
// shipped game replaced are in attic/src/exit_1998.cpp.

#include "exit.h"

#include "3dimage.h"
#include "dialog.h"
#include "file.h"
#include "logging.h"
#include "mappane.h"
#include "module.h"
#include "parse.h"
#include "player.h"
#include "script.h"
#include "sound.h"
#include "textbar.h"

#include <cstdio>
#include <cstring>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

extern short DistX[256];
extern short DistY[256];

REGISTER_BUILDER(TExit)
TObjectClass ExitClass("EXIT", OBJCLASS_EXIT, 0);

// Hard coded class stats
DEFSTAT(Exit, Openable,             OPEN, 0, 0, 0, 1)
DEFSTAT(Exit, Facing,               FACE, 1, 0, 0, 255)
DEFSTAT(Exit, UseCenter,            USE,  2, 0, 0, 2)
DEFSTAT(Exit, StopMoving,           STMV, 3, 0, 1, 1)
DEFSTAT(Exit, Delay,                DLY,  4, 0, 0, 1000)
DEFSTAT(Exit, TileFlags,            TFLG, 5, 0, 1, 32)

// Hard coded object stats
DEFOBJSTAT(Exit, Locked,            LOCK, 0, 0, 0, 1)
DEFOBJSTAT(Exit, KeyId,             KEY,  1, 0, 0, 100000)
DEFOBJSTAT(Exit, PickDifficulty,    PICK, 2, 0, 0, 100000)
DEFOBJSTAT(Exit, AutoActivate,      AACT, 3, 0, 0, 1)

// *****************
// * The exit list *
// *****************

namespace {

// One exit.def entry (retail SExitRef, 0x24 bytes): where going through the
// exit named `name` puts you.
struct SExitRef
{
    std::string name;
    S3DPoint    target;
    int32_t     level    = 0;
    int32_t     mapindex = -1;   // written by the editor, never read (as retail)
    int32_t     ambient  = -1;   // likewise
    SColor      ambcolor{};
};

// In file order. Retail pushed each entry at the head of its list and
// searched from the head, so for a repeated name the last entry wins.
std::vector<SExitRef> exitlist;
bool exitlistdirty = false;

SExitRef* FindExit(const char *name)
{
    if (!name)
        return nullptr;
    for (auto it = exitlist.rbegin(); it != exitlist.rend(); ++it)
        if (!stricmp(it->name.c_str(), name))
            return &*it;
    return nullptr;
}

struct SFileCloser
{
    void operator()(FILE *fp) const { fclose(fp); }
};

// REVSYNC: ReadExitList @ 0x0050c8f0 -- the active module's exit.def, else
// the shared one. `reload` keeps the entries already read and adds only
// names not yet listed (WriteExitList merges with it). A malformed line
// ends the read, keeping what came before it.
bool ReadExitList(bool reload)
{
    if (!reload)
        exitlist.clear();

    const std::string path = ModuleManager.DataFilePath("exit.def");
    std::unique_ptr<FILE, SFileCloser> fp(TryOpen(path.c_str(), "rb"));
    if (!fp)
    {
        log_warn("[exit] %s: can't open; no exits", path.c_str());
        return false;
    }

    TFileParseStream s(fp.get(), path.c_str());
    TToken t(s);
    t.Get();

    bool ok = true;
    do
    {
        if (t.Type() == TKN_RETURN || t.Type() == TKN_WHITESPACE)
            t.LineGet();
        if (t.Type() == TKN_EOF)
            break;

        char name[128];
        SExitRef ref;
        if (!Parse(t, "%t (%d, %d, %d) level %d mapindex %d ambient %d (%d, %d, %d)",
                   name, &ref.target.x, &ref.target.y, &ref.target.z, &ref.level,
                   &ref.mapindex, &ref.ambient, &ref.ambcolor.red, &ref.ambcolor.green,
                   &ref.ambcolor.blue))
        {
            log_warn("[exit] %s: line %d unreadable; list ends there", path.c_str(), t.LineNum());
            ok = false;
            break;
        }

        if (!reload || !FindExit(name))
        {
            ref.name = name;
            exitlist.push_back(std::move(ref));
        }

        t.SkipLine();       // the rest of the line, newline included
    } while (t.Type() != TKN_EOF);

    exitlistdirty = false;
    log_info("[exit] %s: %d exits", path.c_str(), (int32_t)exitlist.size());
    return ok;
}

// Retail's IsOutside builds its two points with 0x0046db20 on an angle it
// doesn't wrap: the Facing stat plus the object's facing byte (and that
// plus 0x7f). The sine table (DistX) lies right before the cosine table
// (DistY) in memory, so angles 256..383 take their x from DistY; the port's
// ConvertToVector wraps instead. This reproduces every read that stays
// inside the two tables. Other angles read memory outside them, unknown
// from the executable, and are wrapped (logged once).
S3DPoint RetailFacingPoint(int32_t angle, int32_t length)
{
    if (angle < 0 || angle >= 256 + 128)
    {
        static bool logged = false;
        if (!logged)
        {
            log_warn("[exit] IsOutside angle %d is outside retail's tables; wrapped", angle);
            logged = true;
        }
        S3DPoint v;
        ConvertToVector(angle & 255, length, v);
        return v;
    }

    const int32_t x = angle < 256 ? DistX[angle] : DistY[angle - 256];
    const int32_t y = DistY[angle <= 128 ? 128 - angle : angle - 128];
    return S3DPoint(x * length / 256, y * length / 256, 0);
}

// Walking onto these doesn't activate them: their master.s USE scripts own
// them (Pulse 0x0050d640, a case-sensitive strcmp on the type name).
bool IsScriptedDoorType(const char *type)
{
    return type && (!strcmp(type, "Door1") || !strcmp(type, "Door2") ||
                    !strcmp(type, "PortEW") || !strcmp(type, "PortNS"));
}

}  // namespace

// REVSYNC: 0x0050c880
bool TExit::Initialize()
{
    exitlistdirty = false;
    return ReadExitList(false);
}

// REVSYNC: 0x0050c8a0
void TExit::Close()
{
    WriteExitList();
    exitlist.clear();
    exitlistdirty = false;
}

// REVSYNC: WriteExitList @ 0x0050cca0. Retail wrote the module's exit.def
// when that file existed on disk, else the shared one; DataFilePath picks
// the same, and the file layer puts writes under the save path. Entries go
// out in retail's list order (newest first).
bool TExit::WriteExitList()
{
    if (!exitlistdirty)
        return true;        // nothing to do, no changes have been made since last load/save

    if (!ReadExitList(true))    // get any exits that have been added since last load
        return false;

    const std::string path = ModuleManager.DataFilePath("exit.def");
    std::unique_ptr<FILE, SFileCloser> fp(TryOpen(path.c_str(), "wb"));
    if (!fp)
        return false;

    for (auto it = exitlist.rbegin(); it != exitlist.rend(); ++it)
        if (fprintf(fp.get(), "%s (%d, %d, %d) level %d mapindex 0x%x ambient %d (%d, %d, %d)\r\n",
                    it->name.c_str(), it->target.x, it->target.y, it->target.z, it->level,
                    it->mapindex, it->ambient, it->ambcolor.red, it->ambcolor.green,
                    it->ambcolor.blue) < 0)
            return false;

    exitlistdirty = false;
    return true;
}

bool TExit::AddExit(const char *name, TObjectInstance* inst, bool getamb)
{
    if (!name || !*name || !inst)
        return false;

    SExitRef* ref = FindExit(name);
    if (!ref)
    {
        exitlist.push_back(SExitRef{});
        ref = &exitlist.back();
        ref->name = name;
    }

    if (inst->ObjClass() != OBJCLASS_EXIT)
    {
        inst->GetPos(ref->target);
        ref->mapindex = -1;
    }
    else
    {
        if (inst->GetImagery() == nullptr)
            ref->target = S3DPoint(0, 0, 0);
        else
        {
            int32_t regx, regy, regz, width, length, height;
            static_cast<TExit*>(inst)->GetExitStrip(regx, regy, regz, width, length, height);
            const S3DPoint start(-regx * GRIDSIZE, -regy * GRIDSIZE, 0);
            const S3DPoint end(start.x + width * GRIDSIZE, start.y + length * GRIDSIZE, 0);
            ref->target = S3DPoint((start.x + end.x) / 2, (start.y + end.y) / 2, 0);
        }
        ref->target += inst->Pos();

        // remember to close the door on the way out...
        ref->mapindex = inst->GetMapIndex();

        if (getamb)
        {
            ref->ambient = MapPane.GetAmbientLight();
            ref->ambcolor = MapPane.GetAmbientColor();
        }
        else
        {
            ref->ambcolor.red = ref->ambcolor.green = ref->ambcolor.blue = 255;
            ref->ambient = -1;
        }
    }

    ref->level = MapPane.GetMapLevel();
    exitlistdirty = true;
    return true;
}

// *********
// * TExit *
// *********

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
        width = 1;
    }
}

// Cells as retail computes them (C division, so a point up to a cell short
// of the strip's near edge still counts).
bool TExit::OnStrip(const S3DPoint& at, int32_t regx, int32_t regy, int32_t width, int32_t length) const
{
    const int32_t gx = (regx * GRIDSIZE - pos.x + at.x) / GRIDSIZE;
    const int32_t gy = (regy * GRIDSIZE - pos.y + at.y) / GRIDSIZE;
    return gx >= 0 && gy >= 0 && gx < width && gy < length;
}

// Driven by the animation's name in the imagery, not the state number: a
// finished CLOSING animation goes to `closedstate`, OPENING to open.
void TExit::StepAnimation(int32_t closedstate, bool needsmultiframe)
{
    if (!CommandDone() || !Openable() || !HasAnimator())
        return;

    TObjectImagery* im = GetImagery();
    if (!im || (needsmultiframe && im->GetAniLength(state) <= 1))
        return;

    const char *ani = im->GetAniName(state);
    if (!ani)
        return;
    if (!stricmp(ani, "CLOSING"))
        SetExitState(closedstate);
    if (!stricmp(ani, "OPENING"))
        SetExitState(EXIT_OPEN);
}

void TExit::Pulse()
{
    TObjectInstance::Pulse();       // retail skips TContainer (0x004708e0)

    if (!Editor)
    {
        StepAnimation(EXIT_CLOSED, true);

        // Retail ran the scan on the server only; the port is single player.
        if (GetImagery())
        {
            int32_t regx, regy, regz, width, length, height;
            GetExitStrip(regx, regy, regz, width, length, height);

            exitflags &= ~(kExitPlayerOn | kExitArrivedOnExit);
            for (int32_t i = 0; i < PlayerManager.NumPlayers(); i++)
            {
                TPlayer* player = PlayerManager.GetPlayer(i);
                if (!player || !OnStrip(player->Pos(), regx, regy, width, length) ||
                    IsScriptedDoorType(GetTypeName()))
                    continue;

                // A player already on an exit -- just arrived through one --
                // doesn't set this one off; it has to step off every strip
                // first (TCharacter clears OF_ONEXIT after 5 frames).
                exitflags |= kExitPlayerOn;
                if (player->IsOnExit())
                    exitflags |= kExitArrivedOnExit;
                else
                    Activate(player, false);
                player->SetOnExit();
            }
        }
    }

    if (!HasAnimator())
        SetCommandDone(true);
}

bool TExit::Use(TObjectInstance* user, int32_t with)
{
    if (!Openable())
        return false;

    // A key or lockpick attempt answers for itself, and the USE trigger
    // still runs even when it failed (EXITS.md §1.8).
    if (!CheckKeyUse(user, MapPane.GetInstance(with)) && Locked())
    {
        if (user == Player)
            TextBar.Print("%s", DialogList.GetLine("DOORLOCKED"));
        return false;
    }

    TObjectInstance::Use(user, with);
    return true;
}

int32_t TExit::CursorType(TObjectInstance* with)
{
    if (Openable() && !(flags & OF_INVISIBLE))
        return with ? CURSOR_HAND : CURSOR_DOOR;

    return CURSOR_NONE;
}

void TExit::UseRange(int32_t &mindist, int32_t &maxdist, int32_t &minang, int32_t &maxang)
{
}

bool TExit::Activate(TObjectInstance* user, bool forced)
{
    if (!user)
        user = Player;
    if (!user || Locked())
        return false;

    if (GetScript() && !forced)
    {
        GetScript()->Trigger(TRIGGER_ACTIVATE, nullptr, nullptr, user, kAliasUser);
        return true;
    }

    if (!forced && !AutoActivate())
        return false;

    const SExitRef* ref = FindExit(GetName());
    if (!ref)
        return GetScript() != nullptr;

    user->Teleport(ref->target, ref->level);
    return true;
}

void TExit::Unactivate()
{
    if (Openable())
        SetExitState(EXIT_CLOSINGOUT);
}

void TExit::Operate(TObjectInstance* user)
{
    if (state == EXIT_OPEN || state == EXIT_OPENINGOUT)
        SetExitState(IsOutside(user) ? EXIT_CLOSINGOUT : EXIT_CLOSINGIN);
    else if (state == EXIT_CLOSED || state == EXIT_CLOSINGOUT)
        SetExitState(IsOutside(user) ? EXIT_OPENINGOUT : EXIT_OPENINGIN);
}

// `obj` is outside when it is nearer the point 10 units behind the exit's
// facing than the one 10 units in front of it (retail's approximate
// distance, 0x0046de60 = Distance).
bool TExit::IsOutside(const TObjectInstance* obj)
{
    if (!obj)
        return false;

    const int32_t facing = Facing() + GetFace();
    const S3DPoint front = RetailFacingPoint(facing, 10);
    const S3DPoint back  = RetailFacingPoint(facing + 0x7f, 10);
    S3DPoint delta = obj->Pos();
    delta -= pos;
    return ::Distance(back, delta) < ::Distance(front, delta);
}

void TExit::SetExitState(int32_t es)
{
    struct SStateName { const char *name; const char *fallback; };
    static constexpr SStateName kNames[] = {
        { "openingout", "closed to open" },     // EXIT_OPENINGOUT
        { "open",       nullptr },              // EXIT_OPEN
        { "closingout", "open to closed" },     // EXIT_CLOSINGOUT
        { "closed",     nullptr },              // EXIT_CLOSED
        { "openingin",  "closed to open" },     // EXIT_OPENINGIN
        { "closingin",  "open to closed" },     // EXIT_CLOSINGIN
    };

    int32_t st = es;
    if (es >= 0 && es < (int32_t)std::size(kNames))
    {
        st = FindState(kNames[es].name);
        if (st < 0 && kNames[es].fallback)
            st = FindState(kNames[es].fallback);
        if (st < 0)
            st = es;
    }
    SetState(st);
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

// Retail vtable 0x5b2258. Its 1998 Activate (TExit::Activate, then the plate
// goes down) became 0x0050da20, but in a new vtable slot (0x280) that
// nothing calls: the engine never moves a plate, which behaves as a plain
// auto-activating exit (EXITS.md §2.1). Not ported, since it never runs.
_CLASSDEF(TPressPlate)
class TPressPlate : public TExit
{
  public:
    TPressPlate(TObjectImagery* newim) : TExit(newim) {}
    TPressPlate(SObjectDef* def, TObjectImagery* newim) : TExit(def, newim) {}

    // REVSYNC: 0x0050df30
    bool Use(TObjectInstance* user, int32_t with = -1) override { return false; }
    // REVSYNC: 0x0050df40
    int32_t CursorType(TObjectInstance* with = nullptr) override { return CURSOR_NONE; }
    // REVSYNC: 0x0050da50 -- the plate up (state 0).
    void Unactivate() override { SetState(0); }
};

DEFINE_BUILDER("PressPlate", TPressPlate)
REGISTER_BUILDER(TPressPlate)

// ************
// * TUpBlock *
// ************

// Retail vtable 0x5b24e0 (types UpBlock: UpBlock.I3D and WallBlock.I3D).
_CLASSDEF(TUpBlock)
class TUpBlock : public TExit
{
  public:
    TUpBlock(TObjectImagery* newim) : TExit(newim) {}
    TUpBlock(SObjectDef* def, TObjectImagery* newim) : TExit(def, newim) {}

    // REVSYNC: 0x0050dae0
    bool Use(TObjectInstance* user, int32_t with = -1) override;
    // REVSYNC: 0x0050e020
    int32_t CursorType(TObjectInstance* with = nullptr) override { return CURSOR_NONE; }
    // REVSYNC: 0x0050da80
    void Pulse() override;
};

DEFINE_BUILDER("UpBlock", TUpBlock)
REGISTER_BUILDER(TUpBlock)

// A finished move settles (raw states).
void TUpBlock::Pulse()
{
    if (!Editor && CommandDone())
    {
        if (state == EXIT_CLOSING || state == EXIT_CLOSINGIN)
            SetState(EXIT_CLOSED);
        else if (state == EXIT_OPENING || state == EXIT_OPENINGIN)
            SetState(EXIT_OPEN);
    }

    TExit::Pulse();
}

// A USE script that handles it wins; otherwise a bare use (no item) moves
// the block the other way.
bool TUpBlock::Use(TObjectInstance* user, int32_t with)
{
    if (TObjectInstance::Use(user, with))
        return true;
    if (with != -1)
        return false;

    if (state == EXIT_CLOSING || state == EXIT_CLOSED)
        SetState(EXIT_OPENING);
    else if (state == EXIT_OPENING || state == EXIT_OPEN)
        SetState(EXIT_CLOSING);

    PLAY("grind rock");
    return true;
}

// **********************
// * TDragonEntAnimator *
// **********************

// Retail animator vtable 0x5b2764, as 1998. TExit::GetExitStrip never asks
// the animator, so its strip is unused (EXITS.md §2.4).
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
    T3DAnimator::Render();
    return true;
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

// Raw states: anything but open or closed goes to closed, open or closed
// starts closing.
void TLever::Operate(TObjectInstance* user)
{
    if (state != EXIT_OPEN && state != EXIT_CLOSED)
        SetState(EXIT_CLOSED);
    else
        SetState(EXIT_CLOSINGOUT);
}

// The 1998 exit model, for the main player: Delay frames on the strip
// activate it once; stepping off unactivates it.
void TLever::Pulse()
{
    TObjectInstance::Pulse();

    if (!Editor)
        StepAnimation(EXIT_OPENINGOUT, false);  // retail's CLOSING -> 0 (author question 33)

    if (!Player || Editor || !GetImagery())
        return;

    int32_t regx, regy, regz, width, length, height;
    GetExitStrip(regx, regy, regz, width, length, height);

    if (!OnStrip(Player->Pos(), regx, regy, width, length))
    {
        if (exitflags & kExitActivated)
            Unactivate();
        exitflags &= ~(kExitPlayerOn | kExitActivated | kExitArrivedOnExit);
        return;
    }

    if (Player->IsOnExit() && !(exitflags & kExitPlayerOn))
        exitflags |= kExitArrivedOnExit;
    Player->SetOnExit();

    const uint32_t before = exitflags;
    exitflags |= kExitPlayerOn;
    if (!(before & kExitActivated) && wait++ > Delay())
    {
        wait = 0;
        if (Activate(nullptr, false))
            exitflags |= kExitActivated;
    }
}
