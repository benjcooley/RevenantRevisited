// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           objectcomponent.h - per-instance optional behavior         *
// *************************************************************************
//
// Components are owned by TObjectInstance and are the intended place for
// optional systems such as specialized visuals, effects, or animation
// transition logic that should override or extend the class-definition
// default without adding more ad-hoc checks to instance/sector code.
//
// Lifecycle
//   - Constructed however the caller chooses, then handed to
//     TObjectInstance::AddComponent which records it in the instance's
//     component slot and (if the instance is in the map) calls Activate()
//     -> OnAttach().
//   - Update() is the externally-callable hook; OnUpdate() is the virtual
//     subclasses override. RegisterUpdate / UnregisterUpdate plug into a
//     global update list driven once per pulse from RunUpdateList(). Code
//     that wants explicit ordering relative to the owner instance can skip
//     auto-registration and have the owner drive Update() directly.
//
// Pulled out of object.h so headers that need to declare component
// subclasses (imagery.h, anim system, etc.) can include this without
// dragging the rest of the TObjectInstance / sector machinery along.
//
// *************************************************************************

#pragma once

#include "revenant.h"

#include <cstdint>

_CLASSDEF(TObjectInstance)
_CLASSDEF(TObjectComponent)

using TObjectComponentUpdateMethod = void (TObjectComponent::*)();

class TObjectComponent
{
  public:
    TObjectComponent() = default;
    virtual ~TObjectComponent() = default;

    [[nodiscard]] TObjectInstance* Owner() const { return owner; }
    [[nodiscard]] int32_t ComponentSlot() const { return slot; }
    [[nodiscard]] uint32_t Generation() const { return generation; }
    [[nodiscard]] virtual const char* ComponentName() const { return "component"; }
    virtual void OnAttach() {}
    virtual void OnDetach() {}
    void Update() { OnUpdate(); }

    static void RunUpdateList();

  protected:
    void RegisterUpdate(TObjectComponentUpdateMethod method);
    void UnregisterUpdate(TObjectComponentUpdateMethod method);
    virtual void OnUpdate() {}

  private:
    friend class TObjectInstance;
    void Attach(TObjectInstance* newowner, int32_t newslot, uint32_t newgeneration)
        { owner = newowner; slot = newslot; generation = newgeneration; }
    void Activate()
        { if (!active) { active = true; OnAttach(); } }
    void Detach()
        { if (active) { OnDetach(); active = false; } UnregisterUpdate(nullptr); owner = nullptr; slot = -1; ++generation; }

    TObjectInstance* owner = nullptr;
    int32_t slot = -1;
    uint32_t generation = 1;
    bool active = false;
};
