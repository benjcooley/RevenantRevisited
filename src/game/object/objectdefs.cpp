// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   objectdefs.cpp - the defs of the classes with a record of their own *
// *************************************************************************

#include "game/object/objectdefs.h"

#include "game/object/exit.h"
#include "game/object/monstergen.h"
#include "game/object/scroll.h"
#include "game/object/weapon.h"
#include "game/effects/effect.h"

#include <cstring>

// ---- TExit ------------------------------------------------------------------

TObjectInstance* TExitDef::Create(SObjectState& state, TObjectImagery* img) const { return new TExit(&state, img); }
TObjectInstance* TExitDef::Create(TObjectImagery* img) const { return new TExit(img); }

void TExitDef::Apply(const SObjectState& state, TObjectInstance& inst) const
{
    TObjectDef::Apply(state, inst);
    if (const auto* es = dynamic_cast<const SExitState*>(&state))
        static_cast<TExit&>(inst).exitflags = uint32_t(es->exitflags);
}

void TExitDef::Capture(const TObjectInstance& inst, SObjectState& state) const
{
    TObjectDef::Capture(inst, state);
    if (auto* es = dynamic_cast<SExitState*>(&state))
        es->exitflags = int32_t(static_cast<const TExit&>(inst).exitflags);
}

// ---- TScroll ----------------------------------------------------------------

TObjectInstance* TScrollDef::Create(SObjectState& state, TObjectImagery* img) const { return new TScroll(&state, img); }
TObjectInstance* TScrollDef::Create(TObjectImagery* img) const { return new TScroll(img); }

void TScrollDef::Apply(const SObjectState& state, TObjectInstance& inst) const
{
    TObjectDef::Apply(state, inst);
    const auto* ss = dynamic_cast<const SScrollState*>(&state);
    if (!ss)
        return;
    auto& o = static_cast<TScroll&>(inst);
    delete[] o.text;
    o.text = nullptr;
    if (!ss->text.empty())
    {
        o.text = new char[ss->text.size() + 1];
        memcpy(o.text, ss->text.c_str(), ss->text.size() + 1);
    }
}

void TScrollDef::Capture(const TObjectInstance& inst, SObjectState& state) const
{
    TObjectDef::Capture(inst, state);
    if (auto* ss = dynamic_cast<SScrollState*>(&state))
    {
        const auto& o = static_cast<const TScroll&>(inst);
        ss->text = o.text ? o.text : "";
    }
}

// ---- TWeapon ----------------------------------------------------------------

TObjectInstance* TWeaponDef::Create(SObjectState& state, TObjectImagery* img) const { return new TWeapon(&state, img); }
TObjectInstance* TWeaponDef::Create(TObjectImagery* img) const { return new TWeapon(img); }

void TWeaponDef::Apply(const SObjectState& state, TObjectInstance& inst) const
{
    TObjectDef::Apply(state, inst);
    if (const auto* ws = dynamic_cast<const SWeaponState*>(&state))
        static_cast<TWeapon&>(inst).poison = ws->poison;
}

void TWeaponDef::Capture(const TObjectInstance& inst, SObjectState& state) const
{
    TObjectDef::Capture(inst, state);
    if (auto* ws = dynamic_cast<SWeaponState*>(&state))
        ws->poison = static_cast<const TWeapon&>(inst).poison;
}

// ---- TDripEffect ------------------------------------------------------------

TObjectInstance* TDripDef::Create(SObjectState& state, TObjectImagery* img) const { return new TDripEffect(&state, img); }
TObjectInstance* TDripDef::Create(TObjectImagery* img) const { return new TDripEffect(img); }

void TDripDef::Apply(const SObjectState& state, TObjectInstance& inst) const
{
    TObjectDef::Apply(state, inst);
    if (const auto* ds = dynamic_cast<const SDripState*>(&state))
    {
        auto& o = static_cast<TDripEffect&>(inst);
        o.ripplesize = ds->ripplesize;
        o.height     = ds->height;
        o.period     = ds->period;
    }
}

void TDripDef::Capture(const TObjectInstance& inst, SObjectState& state) const
{
    TObjectDef::Capture(inst, state);
    if (auto* ds = dynamic_cast<SDripState*>(&state))
    {
        const auto& o = static_cast<const TDripEffect&>(inst);
        ds->ripplesize = o.ripplesize;
        ds->height     = o.height;
        ds->period     = o.period;
    }
}

// ---- TMonsterGen ------------------------------------------------------------

TObjectInstance* TMonsterGenDef::Create(SObjectState& state, TObjectImagery* img) const { return new TMonsterGen(&state, img); }
TObjectInstance* TMonsterGenDef::Create(TObjectImagery* img) const { return new TMonsterGen(img); }

// REVSYNC: the record half of TMonsterGen::Load @ 0x00516e00: the slots
// refilled through AddType (the first free slot), each clock's start on
// the slot it filled.
void TMonsterGenDef::Apply(const SObjectState& state, TObjectInstance& inst) const
{
    TObjectDef::Apply(state, inst);
    const auto* ms = dynamic_cast<const SMonsterGenState*>(&state);
    if (!ms)
        return;
    auto& o = static_cast<TMonsterGen&>(inst);
    o.slots = {};
    int32_t i = 0;
    for (const SMonsterGenSlotState& slot : ms->slots)
    {
        o.AddType(slot.type.c_str(), slot.frequency, slot.treasure);
        if (i < TMonsterGen::kMaxTypes && o.slots[size_t(i)])
            o.slots[size_t(i)]->start = slot.start;
        ++i;
    }
    o.maxmonsters = ms->maxmonsters;
}

void TMonsterGenDef::Capture(const TObjectInstance& inst, SObjectState& state) const
{
    TObjectDef::Capture(inst, state);
    auto* ms = dynamic_cast<SMonsterGenState*>(&state);
    if (!ms)
        return;
    const auto& o = static_cast<const TMonsterGen&>(inst);
    ms->slots.clear();
    for (const auto& slot : o.slots)
        if (slot)
            ms->slots.push_back({ slot->type, slot->frequency, slot->treasure, slot->start });
    ms->maxmonsters = o.maxmonsters;
}

// ---- Registration -----------------------------------------------------------

static TObjectDef::SRegistrar g_exit_def("EXIT", []() -> std::unique_ptr<TObjectDef> { return std::make_unique<TExitDef>(); });
static TObjectDef::SRegistrar g_lever_def("LEVER", &MakeObjectDef<TLever, TExitDef>);
static TObjectDef::SRegistrar g_scroll_def("SCROLL", []() -> std::unique_ptr<TObjectDef> { return std::make_unique<TScrollDef>(); });
static TObjectDef::SRegistrar g_weapon_def("WEAPON", []() -> std::unique_ptr<TObjectDef> { return std::make_unique<TWeaponDef>(); });
static TObjectDef::SRegistrar g_monstergen_def("MonsterGen", []() -> std::unique_ptr<TObjectDef> { return std::make_unique<TMonsterGenDef>(); });
static TObjectDef::SRegistrar g_drip_def("Drip", []() -> std::unique_ptr<TObjectDef> { return std::make_unique<TDripDef>(); });
