// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   objectstates.h - the per-class instance state records              *
// *************************************************************************
//
// One record per object class that has state of its own beyond the base
// (objectdef.h SObjectState): the fields that class's retail stream
// carries, as reflected props, each class in its own hundred of ids so a
// base can grow without colliding (REFL_IDS). The class's def
// (characterdef.h) applies a record to a live object and captures one
// from it; the legacy codec reads retail bytes into a record and writes a
// record to retail bytes; the packed codec and the DEF text move them
// generically. This file is the serialization schema of the object
// classes: what a sector or a save stores for each.
//
// *************************************************************************

#pragma once

#include "game/object/objectdef.h"

// TComplexObject: the root action block the object returns to
// (TComplexObject::Load @ 0x004db930).
struct SComplexObjectState : SObjectState
{
    REFL_IDS(100, 199)
    PROP(100, "Root action (ACTION)")                int32_t     rootaction = 0;
    PROP(101, "Root state name; empty is the type's") std::string rootname;

    [[nodiscard]] std::unique_ptr<SObjectState> Clone() const override
        { return std::make_unique<SComplexObjectState>(*this); }
};

// TCharacter: recovery clocks, poison, the teleport point
// (TCharacter::Load @ 0x004d4eb0).
struct SCharacterState : SComplexObjectState
{
    REFL_IDS(200, 299)
    PROP(200, "Game time health last recovered")     int32_t  lasthealthrecov  = 0;
    PROP(201, "Game time fatigue last recovered")    int32_t  lastfatiguerecov = 0;
    PROP(202, "Game time mana last recovered")       int32_t  lastmanarecov    = 0;
    PROP(203, "Last poison charge; -1 not poisoned") int32_t  lastpoisondamage = -1;
    PROP(204, "Teleport point; -1 none")             S3DPoint teleport_position = {-1, -1, -1};
    PROP(205, "Teleport level; -1 none")             int32_t  teleport_level   = -1;

    [[nodiscard]] std::unique_ptr<SObjectState> Clone() const override
        { return std::make_unique<SCharacterState>(*this); }
};
