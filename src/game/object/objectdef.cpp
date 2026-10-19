// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   objectdef.cpp - instance state and the archetype                    *
// *************************************************************************

#include "game/object/objectdef.h"

#include "engine/core/logging.h"
#include "engine/scene/intworld.h"
#include "game/object/object.h"

#include <cstdlib>
#include <cstring>
#include <vector>

namespace {

struct SDefClass
{
    const char* name;
    TObjectDef::FMake make;
};

std::vector<SDefClass>& DefClasses()
{
    static auto* v = new std::vector<SDefClass>();
    return *v;
}

} // namespace

TObjectDef::~TObjectDef()
{
    free(name);
    stats.Clear();
    objstats.Clear();
}

void TObjectDef::Register(const char* name, FMake make)
{
    DefClasses().push_back({name, make});
}

std::unique_ptr<TObjectDef> TObjectDef::Make(const char* name)
{
    if (!name)
        return nullptr;
    for (const SDefClass& c : DefClasses())
        if (stricmp(name, c.name) == 0)
        {
            std::unique_ptr<TObjectDef> def = c.make();
            def->defname_ = c.name;
            return def;
        }
    return nullptr;
}

// REVSYNC: the record half of TObjectInstance::TObjectInstance(SObjectState*,
// ...) @ 0x0046e1f0. Flags go on wholesale; the constructor and Reset add
// the class-derived bits after.
void TObjectDef::Apply(const SObjectState& state, TObjectInstance& inst) const
{
    inst.flags = state.flags;
    inst.state = state.state;
    inst.Node()->SetLevel(state.level);
    inst.vel   = state.vel;
    inst.accum = state.accum;
    inst.group = state.group;
    SetIntPos(inst, state.pos);
    SetIntRotateX(inst, state.rotatex);
    SetIntRotateY(inst, state.rotatey);
    SetIntFacing(inst, state.rotatez);
    if (state.uid.IsValid())
        inst.Node()->SetUid(state.uid);
    if (inst.flags & OF_LIGHT)
        inst.EnsureLight();
}

void TObjectDef::Capture(const TObjectInstance& inst, SObjectState& state) const
{
    state.objclass = inst.objclass;
    state.objtype  = inst.objtype;
    state.flags    = inst.flags;
    state.state    = inst.state;
    state.level    = uint16_t(inst.GetLevel());
    state.pos      = IntPos(inst);
    state.vel      = inst.vel;
    state.accum    = inst.accum;
    state.rotatex  = uint8_t(IntRotateX(inst));
    state.rotatey  = uint8_t(IntRotateY(inst));
    state.rotatez  = uint8_t(IntFacing(inst));
    state.group    = inst.group;
    state.uid      = inst.Node()->Uid();
}
