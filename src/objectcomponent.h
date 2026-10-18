// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           objectcomponent.h - per-instance optional behavior         *
// *************************************************************************
//
// Components are owned by a TNode and are the intended place for optional
// systems such as specialized visuals, effects, or animation transition
// logic that should override or extend the class-definition default
// without adding more ad-hoc checks to instance/sector code. A component
// is a behaviour on a node; it is never a node itself. The map object is
// one too: TObjectInstance, the root of the object class hierarchy, is the
// first component of a map object's node (docs/OBJECT_MODEL_PLAN.md §3b).
//
// Lifecycle
//   - Constructed however the caller chooses, then handed to
//     TNode::AddComponent which records it in the node's component
//     slot and (unless the node is a map object still waiting for its map
//     index) calls Activate() -> OnAttach().
//   - Update() is the externally-callable hook; OnUpdate() is the virtual
//     subclasses override. RegisterUpdate / UnregisterUpdate plug into a
//     global update list driven once per pulse from RunUpdateList(). Code
//     that wants explicit ordering relative to the owner instance can skip
//     auto-registration and have the owner drive Update() directly.
//
// Tweakable state on a component is annotated with PROP(id) (refl.h);
// the registry serves the packed codec, DEF text, the editor and
// animation tracks without the component knowing.
//
// Pulled out of object.h so headers that need to declare component
// subclasses (imagery.h, anim system, etc.) can include this without
// dragging the rest of the TObjectInstance / sector machinery along.
//
// *************************************************************************

#pragma once

#include "refl.h"
#include "revenant.h"

#include <cstdint>
#include <string>

_CLASSDEF(TNode)
_CLASSDEF(TObjectInstance)
_CLASSDEF(TObjectComponent)

using TObjectComponentUpdateMethod = void (TObjectComponent::*)();

class TObjectComponent
{
  public:
    TObjectComponent() = default;
    virtual ~TObjectComponent() = default;

    [[nodiscard]] TNode* Owner() const { return node_; }
    // The map object on the owning node, or nullptr when the node is a
    // plain prefab node.
    [[nodiscard]] TObjectInstance* OwnerInstance() const;
    // The object component answers itself; every other component nullptr.
    // Lets TNode cache its object at attach time without a search.
    [[nodiscard]] virtual TObjectInstance* AsObjectInstance() { return nullptr; }
    [[nodiscard]] int32_t ComponentSlot() const { return slot; }
    [[nodiscard]] uint32_t Generation() const { return generation; }
    [[nodiscard]] virtual const char* ComponentName() const { return "component"; }
    // Optional per-instance label (the DEF block's name: `mesh outer { }`),
    // for addressing one of several components of a type on a node.
    [[nodiscard]] const std::string& Label() const { return label_; }
    void SetLabel(std::string label) { label_ = std::move(label); }
    virtual void OnAttach() {}
    virtual void OnDetach() {}
    void Update() { OnUpdate(); }

    static void RunUpdateList();

  protected:
    void RegisterUpdate(TObjectComponentUpdateMethod method);
    void UnregisterUpdate(TObjectComponentUpdateMethod method);
    virtual void OnUpdate() {}

  private:
    friend class TNode;
    void Attach(TNode* newowner, int32_t newslot, uint32_t newgeneration)
        { node_ = newowner; slot = newslot; generation = newgeneration; }
    void Activate()
        { if (!active) { active = true; OnAttach(); } }
    void Detach()
        { if (active) { OnDetach(); active = false; } UnregisterUpdate(nullptr); node_ = nullptr; slot = -1; ++generation; }

    std::string label_;
    TNode* node_ = nullptr;
    int32_t slot = -1;
    uint32_t generation = 1;
    bool active = false;
};
