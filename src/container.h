// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   container.h - TContainer module                     *
// *************************************************************************

#ifndef _CONTAINER_H
#define _CONTAINER_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _OBJECT_H
#include "object.h"
#endif

_CLASSDEF(TContainer)
class TContainer : public TObjectInstance
{
  public:
    TContainer(TObjectImagery* newim) : TObjectInstance(newim) { }
    TContainer(SObjectDef* def, TObjectImagery* newim) : TObjectInstance(def, newim) { }

    virtual bool Use(TObjectInstance* user, int32_t with = -1);
    virtual int32_t CursorType(TObjectInstance* inst = nullptr);

    virtual void Load(RTInputStream is, int32_t version, int32_t objversion);
    virtual void Save(RTOutputStream os);

    int32_t NumObjects();
        // Count the number of objects in the container

    bool CheckKeyUse(TObjectInstance* user, TObjectInstance* inst);
        // Try to use inst to unlock this object
    
    // Container stats
    STATFUNC(Openable)
    STATFUNC(Value)
    OBJSTATFUNC(Locked)
    OBJSTATFUNC(KeyId)
    OBJSTATFUNC(PickDifficulty)

    virtual bool AddToInventory(TObjectInstance* inst, int32_t slot = -1);
        // Add inst to this object's inventory, in the given slot (first free slot if none specified)
};

DEFINE_BUILDER("CONTAINER", TContainer)

#endif

