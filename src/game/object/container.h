// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   container.h - TContainer module                     *
// *************************************************************************

#ifndef _CONTAINER_H
#define _CONTAINER_H

#ifndef _REVENANT_H
#include "game/app/revenant.h"
#endif

#ifndef _OBJECT_H
#include "game/object/object.h"
#endif

_CLASSDEF(TContainer)
class TContainer : public TObjectInstance
{
  public:
    TContainer(TObjectImagery* newim) : TObjectInstance(newim) { }
    TContainer(SObjectDef* def, TObjectImagery* newim) : TObjectInstance(def, newim) { }

    bool Use(TObjectInstance* user, int32_t with = -1) override;
        // REVSYNC: 0x004dc180 (INVCONTAINER's too); in the user's inventory,
        // UseCarried
    int32_t CursorType(TObjectInstance* inst = nullptr) override;
    bool CanTrade() override;
        // REVSYNC: 0x004dd3a0 -- every container, whatever its Value, but the
        // multiplayer Swag Bag

    int32_t NumObjects();
        // Count the number of objects in the container
    [[nodiscard]] bool IsOpen() const;
        // Open (state OPEN): a use takes an item out rather than opening it

    bool CheckKeyUse(TObjectInstance* user, TObjectInstance* inst);
        // Try to use inst to unlock this object
    
    // Container stats
    STATFUNC(Openable)
    STATFUNC(Value)
    OBJSTATFUNC(Locked)
    OBJSTATFUNC(KeyId)
    OBJSTATFUNC(PickDifficulty)
    OBJSTATFUNC(Explosive)
    OBJSTATFUNC(Poisonous)
    OBJSTATFUNC(CombineID)
    OBJSTATFUNC(TargetID)
    OBJSTATFUNC(Regen)

    virtual bool AddToInventory(TObjectInstance* inst, int32_t slot = -1);
        // Add inst to this object's inventory, in the given slot (first free slot if none specified)

  private:
    bool UseCarried(TObjectInstance* user, TObjectInstance* inst);
        // Used by the user who carries it: shown or closed, combined, or inst put in
    bool TakeOut(TObjectInstance* user);
        // Used alone on the ground: an item out, or shut when empty
    bool PutIn(TObjectInstance* user, TObjectInstance* inst);
        // inst used on it on the ground: inst goes in
    bool Shut(TObjectInstance* user);
        // An openable container closes
    void SpringTraps(TObjectInstance* user);
        // Opening it: the Explosive and Poisonous traps on the user
    bool Finish();
        // The end of a use that did something (retail 0x004dd33f)

    int32_t regentime = 0;      // retail +0xdc: the game frame a Regen cycle ended
    int32_t regencursor = 0;    // retail +0xe0: the item a Regen container hands out next
};

DEFINE_BUILDER("CONTAINER", TContainer)

// A rack of vials: a container whose contents are never saved.
class TVialRack : public TContainer
{
  public:
    TVialRack(TObjectImagery* newim) : TContainer(newim) {}
    TVialRack(SObjectDef* def, TObjectImagery* newim) : TContainer(def, newim) {}

    bool Use(TObjectInstance* user, int32_t with = -1) override;
    int32_t CursorType(TObjectInstance* inst = nullptr) override;
};

#endif
