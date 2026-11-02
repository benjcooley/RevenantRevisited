// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   objectdef.cpp - instance state and the archetype                    *
// *************************************************************************

#include "game/object/objectdef.h"

#include "engine/core/logging.h"
#include "engine/scene/intworld.h"
#include "game/object/object.h"
#include "game/world/lightcomponent.h"
#include "game/object/statscomponent.h"

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

namespace {

// Stats 3-5 (health, fatigue, mana) are an object's own; an object untouched
// since it was placed (OF_VIRGIN) takes its type's current values for the
// rest (TObjectInstance::Load, 0x0046f0b0).
constexpr int32_t kFirstKeptStat = 3;
constexpr int32_t kLastKeptStat  = 5;

} // namespace

// REVSYNC: the record half of TObjectInstance::TObjectInstance(SObjectState*,
// ...) @ 0x0046e1f0 and of TObjectInstance::Load @ 0x0046f0b0. Flags go on
// wholesale (the legacy codec applies retail's fixed-flag rule before
// calling); the constructor and Reset add the class-derived bits after.
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
    if (!state.name.empty())
        inst.SetName(const_cast<char*>(state.name.c_str()));
    inst.inventnum = short(state.inventnum);
    inst.invindex  = short(state.invindex);
    inst.shadow    = state.shadow;
    if (state.mapindex >= 0)
        inst.SetMapIndex(state.mapindex);
    inst.frame     = short(state.frame);
    inst.framerate = short(state.framerate);

    // Stats: each saved stat is matched to the class's by unique id (the
    // fast path expects the class's order; the slow path searches), so a
    // record survives a reordered class.def. An empty list keeps what the
    // object has (the type's values a spawn starts with).
    if (!state.stats.empty() && inst.cl)
    {
        const int32_t numstats = inst.cl->NumObjStats();
        TStatsComponent& stats = inst.EnsureStats();
        if (numstats > 0)
            stats.Resize(numstats);
        int32_t statid = 0;
        for (const SObjectStatState& saved : state.stats)
        {
            if (statid >= stats.Count())
                break;
            if (saved.id == inst.cl->ObjStatUniqueId(statid))
                stats.Set(statid++, saved.value);
            else
                for (int32_t c = 0; c < numstats; c++)
                    if (saved.id == inst.cl->ObjStatUniqueId(c))
                        stats.Set(c, saved.value);
        }
        if (inst.flags & OF_VIRGIN)
            for (int32_t c = 0; c < numstats; c++)
                if (c < kFirstKeptStat || c > kLastKeptStat)
                    inst.ResetObjStat(c);
    }

    if (inst.flags & OF_LIGHT)
    {
        TLightComponent& light = inst.EnsureLight();
        light.flags_      = uint8_t(state.light.flags);
        light.offset_     = state.light.offset;
        light.color_      = SColor{ uint8_t(state.light.red), uint8_t(state.light.green), uint8_t(state.light.blue) };
        light.intensity_  = uint8_t(state.light.intensity);
        light.multiplier_ = short(state.light.multiplier);
    }
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
    // Only a renamed object carries a name; the type's is implicit
    const bool renamed = inst.name && inst.inf && inst.inf->name && strcmp(inst.name, inst.inf->name) != 0;
    state.name     = renamed ? inst.name : "";
    state.inventnum = inst.inventnum;
    state.invindex  = inst.invindex;
    state.shadow    = inst.shadow;
    state.mapindex  = inst.FileMapIndex();
    state.frame     = inst.frame;
    state.framerate = inst.framerate;

    state.stats.clear();
    if (inst.cl)
        for (int32_t c = 0; c < inst.cl->NumObjStats(); c++)
            state.stats.push_back({ inst.cl->ObjStatUniqueId(c), inst.GetObjStat(c) });

    state.light = SLightState{};
    if (const TLightComponent* light = inst.Light())
    {
        state.light.flags      = light->flags_;
        state.light.offset     = light->offset_;
        state.light.red        = light->color_.red;
        state.light.green      = light->color_.green;
        state.light.blue       = light->color_.blue;
        state.light.intensity  = light->intensity_;
        state.light.multiplier = light->multiplier_;
    }
}
