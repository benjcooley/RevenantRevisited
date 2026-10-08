// Retired from src/exit.cpp (1998 pre-release source): exit behaviour the
// shipped game replaced (docs/gameflow/forensics/EXITS.md §6).
//
//   TExit::Use       toggled the door open/closed itself. Retail's Use
//                    (0x0050d1a0) only checks the lock and runs the USE
//                    trigger; the master.s door scripts open the door with
//                    `operate`.
//   TExit::Activate  took no user, stopped on EX_FROMEXIT and pushed the
//                    player 24 units past a "Door". Retail's Activate(user,
//                    forced) (0x0050d3a0) places the user exactly on the
//                    exit-list target.
//   TExit::Pulse     global Player only, a Delay counter and Unactivate on
//                    leaving the strip. Retail (0x0050d640) checks every
//                    player, excludes the scripted door types and relies on
//                    the player's OF_ONEXIT; the Delay counter survives only
//                    in TLever.
//   TSpikeWall       no SpikeWall builder exists in retail; spike walls are
//                    TRAP objects (SpikeTrapS/N).
// Kept for reference. Not built.

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

        // check if the player is over the strip of walkmap immediately past
        // the bounding box in the given direction
        if (delta.x >= 0 && delta.y >= 0 &&
            delta.x < width && delta.y < length)
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
    int32_t CursorType(TObjectInstance* with = nullptr) override { return CURSOR_NONE; }

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
