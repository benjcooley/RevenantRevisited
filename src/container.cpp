// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  container.cpp - TContainer module                    *
// *************************************************************************

#include "revenant.h"
#include "container.h"
#include "mappane.h"
#include "inventory.h"
#include "textbar.h"
#include "tool.h"
#include "player.h"
#include "key.h"
#include "sound.h"
#include "spellpane.h"

REGISTER_BUILDER(TContainer)
TObjectClass ContainerClass("CONTAINER", OBJCLASS_CONTAINER, 0);

// Hard coded class stats
DEFSTAT(Container, Openable,    OPEN, 0, 0, 0, 1)
DEFSTAT(Container, Value,       VAL,  1, 0, 0, 1000000)

// Hard coded object stats
DEFOBJSTAT(Container, Locked,           LOCK, 0, 0, 0, 0)
DEFOBJSTAT(Container, KeyId,            KEY,  1, 0, 0, 0)
DEFOBJSTAT(Container, PickDifficulty,   PICK, 2, 0, 0, 0)

extern TObjectInstance* TakenObject;
extern TObjectInstance* DroppedObject;

// Container states
#define CLOSED      0
#define OPEN        1

bool TContainer::Use(TObjectInstance* user, int32_t with)
{
    TObjectInstance::Use(user, with);

    TObjectInstance* inst = MapPane.GetInstance(with);

    if (state == CLOSED && CheckKeyUse(user, inst))
        return true;

    if (Locked())
    {
        TextBar.Print("It seems to be locked.");
        return false;
    }

    char buf[80];

    if (Openable() && state == CLOSED)
    {
        if (inst)
        {
            sprintf(buf, "The %s is closed.", GetName());
            TextBar.Print(buf);
            return false;
        }

        SetState(OPEN);
        
        sprintf(buf, "%s opened.", GetName());
        TextBar.Print(buf);
        return true;
    }

    if (GetTopOwner() == Inventory.GetTopContainer())
    {
        // it's in inventory
        if (!inst)
            Inventory.SetContainer(this);
        else if (NumObjects() < (MAXINVITEMS - 1))
        {
            if ((uint32_t)FindFreeInventorySlot() < MAXINVITEMS)
            {
                inst->RemoveFromInventory();
                AddToInventory(inst);
            }
        }
    }
    else
    {
        // it's on the ground
        if (!inst)
        {
            // get from
            TInventoryIterator i(this);
            TObjectInstance* oi = i.Item();

            if (oi)
            {
                if (Inventory.GetContainer() && (uint32_t)Inventory.GetContainer()->FindFreeInventorySlot() < MAXINVITEMS)
                {
                    oi->RemoveFromInventory();
                    Inventory.GetContainer()->AddToInventory(oi);

                    sprintf(buf, "%s taken from %s.", oi->GetName(), GetName());
                    TextBar.Print(buf);

                    TakenObject = oi;
                }
                else
                    TextBar.Print("Can't carry any more.");
            }
            else
            {
                if (!Openable())
                    return false;

                SetState(CLOSED);
                sprintf(buf, "%s closed.", GetName());
                TextBar.Print(buf);
            }
        }
        else
        {
            // add to
            inst->RemoveFromInventory();
            AddToInventory(inst);

            sprintf(buf, "%s put in %s.", inst->GetName(), GetName());
            TextBar.Print(buf);

            DroppedObject = inst;
        }

    }

    return true;
}

int32_t TContainer::CursorType(TObjectInstance* inst)
{
    if (Openable() || NumObjects() > 0)
        return CURSOR_HAND;

    return CURSOR_NONE;
}

int32_t TContainer::NumObjects()
{
    return RealNumInventoryItems();
}

void TContainer::Load(RTInputStream is, int32_t version, int32_t objversion)
{
    TObjectInstance::Load(is, version, objversion);

    if (version >= 2 && version < 5) // We're using object stats now
    {
        int32_t contflags, pickdifficulty;
        is >> contflags >> pickdifficulty;
        SetStat("Locked", contflags != 0);
        SetStat("PickDifficulty", pickdifficulty);
    }

}

void TContainer::Save(RTOutputStream os)
{
    TObjectInstance::Save(os);
}

bool TContainer::CheckKeyUse(TObjectInstance* user, TObjectInstance* inst)
{
    if (!inst || !Locked())
        return false;

    // check for the key unlock
    if (inst->ObjClass() == OBJCLASS_KEY)
    {
        if (((PTKey)inst)->KeyId() == KeyId())
        {   
            PLAY("unlock succeed");
            TextBar.Print("Unlocked.");
            SetLocked(false);
            return true;
        }
        else
        {
            PLAY("unlock failed");
            TextBar.Print("This is the wrong key.");
            return true;
        }
    }

    // check for a lockpick attempt
    if (inst->ObjClass() == OBJCLASS_TOOL)
    {
        int32_t abil = ((PTTool)inst)->Pick();
        if (abil > 0)
        {
            if (user->ObjClass() == OBJCLASS_PLAYER)
                abil += ((TPlayer*)user)->Agil() + ((TPlayer*)user)->Skill(SK_LOCKPICK);

            if (abil < PickDifficulty())
            {
                PLAY("unlock fail");
                TextBar.Print("The lock is too difficult to pick.");
            }
            else if (random(0, abil) < PickDifficulty())
            {
                PLAY("unlock fail");
                TextBar.Print("You fail to pick the lock.");
            }
            else
            {
                PLAY("unlock succeed");
                TextBar.Print("The lock quietly yields to your skills.");
                SetLocked(false);
            }

            return true;
        }
    }

    return false;
}


// *************
// * TVialRack *
// *************

_CLASSDEF(TVialRack)
class TVialRack : public TContainer
{
  public:
    TVialRack(TObjectImagery* newim) : TContainer(newim) {}
    TVialRack(SObjectDef* def, TObjectImagery* newim) : TContainer(def, newim) {}

    virtual bool Use(TObjectInstance* user, int32_t with = -1);
    virtual int32_t CursorType(TObjectInstance* inst = nullptr);
    virtual void Save(RTOutputStream os);
};

DEFINE_BUILDER("VIAL RACK", TVialRack)
REGISTER_BUILDER(TVialRack)

bool TVialRack::Use(TObjectInstance* user, int32_t with)
{
    TObjectInstance* inst = MapPane.GetInstance(with);

    if (!inst)
    {
        if (state > 0)
        {
            if ((uint32_t)Inventory.GetContainer()->FindFreeInventorySlot() >= MAXINVITEMS)
                TextBar.Print("Can't carry any more.");
            else
            {
                // generate a new vial
                SObjectDef def;
                memset(&def, 0, sizeof(SObjectDef));
                def.objclass = OBJCLASS_CONTAINER;
                def.objtype = ContainerClass.FindObjType("Empty Vial");
                GetPos(def.pos);
                int32_t index = MapPane.NewObject(&def);

                TakenObject = MapPane.GetInstance(index);
                Inventory.GetContainer()->AddToInventory(TakenObject);

                TextBar.Print("Poison Vial taken from Vial Rack.");
                SetState(state - 1);

                return true;
            }
        }
    }
    else
    {
        if (strcmp(inst->GetName(), "Poison Empty Vial") == 0 && state < 4)
        {
            // Originally I just deleted the vial here, but this caused problems
            // in shop scripts.  Now I just stash it into the rack, which clears itself
            // out when it saves to disk.

            //MapPane.RemoveObject(with);
            //delete inst;

            inst->RemoveFromInventory();
            AddToInventory(inst);

            TextBar.Print("Poison Vial put in Vial Rack.");
            SetState(state + 1);

            DroppedObject = inst;

            return true;
        }
    }

    return false;
}

int32_t TVialRack::CursorType(TObjectInstance* inst)
{
    if (state > 0 && (!inst || (state < 4 && strcmp(inst->GetName(), "Poison Vial") == 0)))
        return CURSOR_HAND;

    return CURSOR_NONE;
}

void TVialRack::Save(RTOutputStream os)
{
    inventory.DeleteAll();

    TContainer::Save(os);
}


bool TContainer::AddToInventory(TObjectInstance* inst, int32_t slot)
{
    bool ret = TObjectInstance::AddToInventory(inst, slot);
    if (stricmp(GetTypeName(), "Spell Pouch") == 0 || stricmp(GetTypeName(), "SpellPouch") == 0)
        SpellPane.Update();

    return ret;
}