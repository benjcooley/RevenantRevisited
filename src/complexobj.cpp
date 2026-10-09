// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                complexobj.cpp - TComplexObject module                 *
// *************************************************************************

#include <stdlib.h>

#include "revenant.h"
#include "textbar.h"
#include "complexobj.h"

//#define SHOWSTATES

#ifdef SHOWSTATES
char desname1[RESNAMELEN], desname2[RESNAMELEN], desname3[RESNAMELEN], desname4[RESNAMELEN];
#endif

// ******************** Action Block ************************

TActionBlock::TActionBlock(const char *n, ACTION a)
{
    ClearBlock();
    strcpy(name, n);
    action = a;
}

TActionBlock::TActionBlock(const char *n, const char *str, ACTION a)
{
    ClearBlock();
    sprintf(name, "%s%s", n, str);
    action = a;
}

TActionBlock::TActionBlock(TActionBlock &ab, const char *str, ACTION a)
{
    memcpy(this, &ab, sizeof(TActionBlock));
    if (str)
        strcpy(name, str);
    if (a != ACTION_NONE)
        action = a;
}

void TActionBlock::ClearBlock()
{
    name[0] = '\0';
    action = ACTION_ANIMATE;
    frame = -1; // Means set frame to default
    wait = 0;
    angle = moveangle = 0;
    turnrate = 16; // Default turn rate
    target.x = target.y = target.z = 0;
    obj = nullptr;
    data = nullptr;
    flags = 0;
    damage = 0;
    tohit = roll = 0;
    firsttime = true;
    attack = nullptr;
    impact = nullptr;
}

bool TActionBlock::Is(const char *s) const
{
    const char *p1 = name;
    const char *p2 = s;

    while (*p1 && *p2 && 
             (
                *p2 == '?' ||                     // 1 char
                *p2 == '*' ||                     // 0 or more chars
                *p2 == '[' ||                     // Beginning of char list
                *p2 == ']' ||                     // Ending of char list
                (*p2 == '#' && isdigit(*p1)) ||   // Any digit
                tolower(*p2) == tolower(*p1)      // Regular match
             )
          )
    {
        if (*p2 == '[') // Multichar match
        {
            p2++;
            while (*p2 && *p2 != ']')
            {
                if (*p1 == *p2)
                {
                    p1++;
                    break;
                }
            }
            if (*p2)
                p2++;
        }
        else            // Ordinary match
        {
            p1++;
            if (*p2 != '*')
                p2++;
        }
    }
    
    if (*p2 == '*')
        p2++;

    if (*p1 || *p2)
        return false;

    return true;
}

bool TActionBlock::IsRight(const char *state) const
{
    int32_t len = strlen(state);
    if (!strnicmp(name, state, len) && name[len] == 'r' && name[len + 1] == '\0')
        return true;

    return false;
}

bool TActionBlock::IsLeft(const char *state) const
{
    int32_t len = strlen(state);
    if (!strnicmp(name, state, len) && name[len] == 'l' && name[len + 1] == '\0')
        return true;

    return false;
}

bool TActionBlock::IsStep(const char *state) const
{
    int32_t len = strlen(state);
    if (!strnicmp(name, state, len) &&
      (name[len] == 'r' || name[len] == 'l') && name[len + 1] == '\0')
        return true;

    return false;
}

// true if state is one of the master state (i.e. "attack1" is one of "attack")
bool TActionBlock::IsOneOf(const char *state) const
{
    int32_t len = strlen(state);
    if (!strnicmp(name, state, len) &&
      (name[len] >= '0' && name[len] <= '9') && name[len + 1] == '\0')
        return true;

    return false;
}

// Returns the number at the end of a state
int32_t TActionBlock::StateNum() const
{
    int32_t len = strlen(name);
    if (len <= 1)
        return 0;
    else
        return atol(name + len - 1);
}

static char stnamebuf[RESNAMELEN];

// Helper function to make it easy to make state names
const char *StName(const char *name, int32_t num)
{
    sprintf(stnamebuf, "%s%d", name, num);
    return stnamebuf;
}

// Helper function to make it easy to make state names
char *StName(const char *name, const char *str)
{
    sprintf(stnamebuf, "%s%s", name, str);
    return stnamebuf;
}

bool TActionBlock::IsPartOf(const char *prefix, const char *state) const
{
    if (state)
        sprintf(stnamebuf, "%s%s*", prefix, state);
    else
        sprintf(stnamebuf, "%s*", prefix);
    return Is(stnamebuf);
}

// Swaps the root part of the name with a new root
void TActionBlock::SwapRoot(const char *root, const char *newroot)
{
    int32_t l = strlen(root);
    if (strnicmp(name, root, l) != 0)
        return;

    char buf[RESNAMELEN];
    strncpyz(buf, newroot, RESNAMELEN);
    strncatz(buf, name + l, RESNAMELEN);
    strcpy(name, buf);
}

// ******************** Complex Object ************************

// NOTE: DefaultRootState() will not be virtual (will only call COMPLEXOBJ version)
// when ClearComplexObj() is called from constructor!

void TComplexObject::ClearComplexObj()
{
    flags |= OF_PULSE | OF_COMPLEX; // Causes the PULSE function to be called for this object
    SetNotify(N_DELETING);          // Check for deleted objs in action blocks
    root = doing = desired = new TActionBlock(DefaultRootState() /* SEE ABOVE NOTE */);
    state = FindState(root->name);
}

void TComplexObject::Pulse()
{
    if (UpdatingBoundingRect)
        return;

    UpdateAction(GetMoveBits());        // Update current state given current movement flags
}

void TComplexObject::UpdateAction(int32_t bits)
{
    int32_t comstate = ResolveAction(bits);

    if (comstate == 0)
        comstate = commanddone ? COM_COMPLETED : COM_EXECUTING;

    if (doing)
    {
        // check firsttime (first frame) and priority (on completion)
        if (doing->firsttime)
            doing->firsttime = false;

        if (comstate == COM_COMPLETED && doing->priority)
            doing->priority = false;
    }

    if (desired)
    {
        if (desired == root)
            comstate = COM_COMPLETED;
        else
            comstate = TryCommand(desired, bits);
    }

    if (comstate == COM_COMPLETED || comstate == COM_IMPOSSIBLE)
    {
        SetDesired(nullptr);
        TryCommand(root);
    }
}

int32_t TComplexObject::ResolveAction(int32_t bits)
{
    if (!doing)
        return COM_IMPOSSIBLE;

    return 0;
}

void TComplexObject::SetRoot(PTActionBlock ab)
{
    if (ab->noroot)
        return;

    if (ab != root)
    {
        if (root && root != doing && root != desired && root != ab)
            delete root;
        root = ab;
    }

    root->nowaitdone = true;    // We can always interrupt a root state
    root->interrupt = false;    // We don't ever interrupt another state
}

void TComplexObject::SetDoing(PTActionBlock ab)
{
    if (ab != doing)
    {
        if (doing && doing != root && doing != desired && doing != ab)
            delete doing;
        doing = ab;
    }
    root->interrupt = false;    // Now that we're doing, don't interrupt
}

// REVSYNC: SetDesired @ 0x004db3a0 -- a block already waiting stays while
// the one being done has priority (retail tests doing's priority; the 1998
// code tested the waiting block's); false then, and the caller keeps `ab`.
// An interrupting block is forced at once (ForceCommand with `flags`).
bool TComplexObject::SetDesired(PTActionBlock ab, uint32_t flags)
{
    if (ab == nullptr)             // nullptr indicates we want to go back to root
        ab = root;

    if (desired && desired != doing && doing && doing->priority)
        return false;

    if (ab != desired)                  // Set the desired command
    {
        if (desired && desired != doing && desired != root)
            delete desired;
        desired = ab;

#ifdef SHOWSTATES
        strcpy(desname4, desname3);
        strcpy(desname3, desname2);
        strcpy(desname2, desname1);
        strcpy(desname1, ab->name);
#endif
    }

    if (ab->interrupt && doing != ab && !doing->priority) // Interrupt flag means DO IT RIGHT NOW
    {
        ForceCommand(ab, 0, flags);
        ab->interrupt = false;
    }
    return true;
}

// Have object attempt to move into a new state
int32_t TComplexObject::TryCommand(PTActionBlock ab, int32_t bits, uint32_t flags)
{
  // Use root if desired is nullptr
    if (ab == nullptr)
        ab = root;

  // Wait till we're done animating (unless we're in root)
    if ((!doing->priority || desired == doing) &&       // Can't do if trying to change and priority isn't cleared
        (commanddone || !doing ||                       // Okay, command done, or not doing anything
        (doing && ab != doing && doing->nowaitdone) ||  // Or doing action is nowait action
        (desired && ab != desired && desired->interrupt))) // Or desired action is interrupt
        return ForceCommand(ab, bits, flags); // Do new state when old one is done (or interrupted)
    else
        return COM_EXECUTING;   // Otherwise tell program we're still waiting
}

// Force the object into the given state. REVSYNC: retail 0x004db4d0 takes a
// third argument whose bit 0 (kCommandNoIncidentals) looks every state up
// with pcnt 100 instead of a random roll: the 100% variant, never an "NN:"
// incidental. The rest of this body is still the pre-release one.
// REVSYNC: ForceCommand @ 0x004db4d0 -- start `ab` now (root when null):
// refused while doing has priority and a block waits; the state is the
// transition from doing (the root itself for a root-to-root animation going
// home), else, unless dontforce, forced (flag `forced`): the state, a
// transition from or to the root, the root's own. With no incidentals
// (flags & kCommandNoIncidentals) each lookup tries percent 100 first, then
// a random percent. Then the frame, synchronised frames, the root, and what
// comes next (itself if it loops or is a root, else the root).
int32_t TComplexObject::ForceCommand(PTActionBlock ab, int32_t bits, uint32_t flags)
{
    const bool exact = (flags & kCommandNoIncidentals) != 0;
    auto state = [&](const char* name) {
        int32_t s = exact ? FindState(name, 100) : -1;
        return s >= 0 ? s : FindState(name, -1);
    };
    auto transition = [&](const char* from, const char* to) {
        int32_t s = exact ? FindTransitionState(from, to, 100) : -1;
        return s >= 0 ? s : FindTransitionState(from, to, -1);
    };

    if (doing && doing->priority && desired != doing)
        return COM_EXECUTING;

    if (ab == nullptr)
        ab = root;

#ifdef SHOWSTATES
    if (this == (PTComplexObject)Player)
        TextBar.Print("[%s,%s,%s] %s %s %s %s", root->name, desired->name, doing->name,
            desname1, desname2, desname3, desname4);
#endif

    ab->interrupt = false;
    ab->transition = false;
    ab->forced = false;

    int32_t newstate;
    if (doing == nullptr)
        newstate = exact ? FindState(ab->name, 100) : FindState(ab->name, -1);
    else
    {
      // ROOT2ROOT: the transition into this animation already brings it home,
      // so going back to the root needs no transition of its own.
        if ((GetAniFlags() & AF_ROOT2ROOT) && desired && desired == root)
        {
            ab = root;
            newstate = exact ? FindState(ab->name, 100) : FindState(ab->name, -1);
        }
        else
            newstate = exact ? FindTransitionState(doing->name, ab->name, 100)
                             : FindTransitionState(doing->name, ab->name, -1);

        ab->transition = stricmp(doing->name, ab->name) != 0;
        if (newstate < 0)
        {
            if (ab->dontforce)
                return COM_IMPOSSIBLE;
            ab->forced = ab->transition;
            newstate = state(ab->name);
            if (newstate < 0)
                newstate = transition(root->name, ab->name);
            if (newstate < 0)
                newstate = transition(ab->name, root->name);
            if (newstate < 0)
                newstate = transition(root->name, root->name);
        }
    }

    if (newstate < 0)
        return COM_IMPOSSIBLE;

    SetState(newstate);
    SetDoing(ab);

  // Set the first frame (if valid)
    if (ab->frame >= 0)
    {
        SetFrame(min(max(ab->frame, 0), imagery->GetHeader()->states[newstate].frames - 1));
        ab->frame = -1;
    }

  // Synchronise with the opponent's animation (block / impact to an attack).
    if (ab->obj && GetAniFlags() & AF_SYNCHRONIZE)
        SetFrame(ab->obj->GetFrame() + 1);

  // Make this block the root block if current or next animation is a root animation
    uint32_t nextaniflags = imagery ? imagery->GetAniFlags(exact ? FindState(ab->name, 100) : FindState(ab->name, -1)) : 0;
    if ((GetAniFlags() & AF_ROOT || nextaniflags & AF_ROOT) && !ab->noroot)
        SetRoot(ab);

  // If this is a looping animation, set desired to the same action again, otherwise
  // set it to go back to the root state
    if (ab->loop ||
        (GetAniFlags() & AF_LOOPING) || (GetAniFlags() & AF_ROOT) ||
        (nextaniflags & AF_LOOPING) || (nextaniflags & AF_ROOT))
        SetDesired(ab);
    else
        SetDesired(nullptr);

    return COM_EXECUTING;
}

// REVSYNC: TComplexObject::Load @ 0x004db930 (SAVE_GAME.md §11.3).
void TComplexObject::Load(RTInputStream is, int32_t version, int32_t objversion)
{
    uint8_t basever = 0;
    if (objversion >= 1)
        is >> basever;
    TObjectInstance::Load(is, version, basever);

  // Load root state
    PTActionBlock ab;
    if (version < 7)
    {
        ab = new TActionBlock(DefaultRootState());
    }
    else
    {
        uint8_t action;
        is >> action;
        const std::string rootname = is.ReadString();
        ab = new TActionBlock(rootname.substr(0, RESNAMELEN - 1).c_str(), (ACTION)action);
    }

    SetRoot(ab);
    SetDoing(ab);
    SetDesired(ab);

  // Keep the saved state when it is the root's own (an animation state is
  // named "<group>:<name>"); otherwise start the root.
    if (const char* statename = imagery ? imagery->GetAniName(state) : nullptr)
    {
        if (const char* colon = strchr(statename, ':'))
            statename = colon + 1;
        if (stricmp(ab->name, statename) == 0)
            return;
    }
    state = FindState(ab->name);
}

// REVSYNC: TComplexObject::Save @ 0x004dbb80.
void TComplexObject::Save(RTOutputStream os)
{
    os << (uint8_t)TObjectInstance::ObjVersion();
    TObjectInstance::Save(os);

  // Save root state
    os << (uint8_t)root->action;
    os << root->name;
}

void TComplexObject::Notify(int32_t notify, void *ptr)
{
    // **** WARNING!!! MAKE SURE YOU CHECK FOR BROKEN LINKS AND DELETED OBJECTS HERE!!! ****
    // If you want to be notified, you must call SetNotify() in your contsructor

    if (sector == (TSector*)ptr)
        return;

    TObjectInstance::Notify(notify, ptr);

    if (root && root->obj && NOTIFY_DELETED(ptr, root->obj))
        root->obj = nullptr;
    if (doing && doing->obj && NOTIFY_DELETED(ptr, doing->obj))
        doing->obj = nullptr;
    if (desired && desired->obj && NOTIFY_DELETED(ptr, desired->obj))
        desired->obj = nullptr;
}
