// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   nodeinst.h - TNodeInst: a named, soft-ref-able transform node with  *
// *                components. Base of TObjectInstance; the unit a VFX    *
// *                prefab is built from.                                   *
// *************************************************************************
//
// A node is the smallest thing in the scene that can be addressed by a
// soft ref, placed in a transform hierarchy, and given behaviour through
// components. TObjectInstance derives from it and adds everything that is
// map-shaped (map index, sector, class / type info, imagery, inventory,
// script, stats). Prefab nodes (docs/vfx/PREFAB_SYSTEM.md §1) are plain
// TNodeInsts: never sector members, never saved, never pulsed by MapPane.
//
// Identity: TSafeObjectBase<TNodeInst> gives every node an id + generation
// in the node registry, so TSafeRef<TNodeInst> and TSafeComponentRef<T>
// resolve through it. TObjectInstance keeps its map index as the id for
// TSafeRef<TObjectInstance> (it hides this contract) because existing code
// compares TSafeRef::MapIndex() against map indices. The two id spaces
// coexist.
//
// Hierarchy lives on the embedded TTransform (parent / children links are
// non-owning). TTransform carries a back-pointer to its node so traversal
// by name and path resolution ("a/b/c") work from the transform tree.
// Ownership of child nodes stays with whoever created them.
//
// Components: owned (unique_ptr), slot-addressed, generation-stamped on
// attach so TSafeComponentRef<T> rejects a recycled slot. A component is
// activated (OnAttach, update registration) on AddComponent unless the
// derived class defers it -- TObjectInstance waits for a map index.
//
// Properties: pos / rot / scl / active / name are annotated (refl.h), so a
// track or the inspector addresses a node and its components through the
// same registry.
//
// *************************************************************************

#pragma once

#include "objectcomponent.h"
#include "refl.h"
#include "saferef.h"
#include "transform.h"

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

class TObjectInstance;

class TNodeInst : public TSafeObjectBase<TNodeInst>
{
  public:
    TNodeInst();
    explicit TNodeInst(std::string name);
    virtual ~TNodeInst();

    // ---- Identity --------------------------------------------------------
    DEF_BLOCK("node")
    PROP_FN(1, std::string, name,   GetName,     );                   // read-only
    PROP_FN(2, hmm_vec3,    pos,    LocalPos,    SetLocalPos);
    PROP_FN(3, hmm_vec3,    rot,    LocalRotDeg, SetLocalRotDeg);     // euler, degrees
    PROP_FN(4, hmm_vec3,    scl,    LocalScl,    SetLocalScl, def({1, 1, 1}));
    PROP_FN(5, bool,        active, Active,      SetActive, def(true), "Inactive: subtree neither ticked nor drawn");

    // Instance-specific name. TObjectInstance overrides this with its
    // legacy strdup'd / objinfo-backed name.
    [[nodiscard]] virtual const char* GetName() const { return node_name_.c_str(); }
    void SetNodeName(std::string name) { node_name_ = std::move(name); }

    // Checked downcast without RTTI. nullptr for plain nodes.
    [[nodiscard]] virtual TObjectInstance*       AsObjectInstance()       { return nullptr; }
    [[nodiscard]] virtual const TObjectInstance* AsObjectInstance() const { return nullptr; }

    [[nodiscard]] bool Active() const { return active_; }
    void SetActive(bool active) { active_ = active; }

    // ---- Transform / hierarchy ------------------------------------------
    [[nodiscard]] TTransform&       Transform()       { return transform_; }
    [[nodiscard]] const TTransform& Transform() const { return transform_; }

    // Virtual so a map object can route them through its own setters
    // (TObjectInstance keeps a legacy position / rotation in lockstep).
    [[nodiscard]] const hmm_vec3& LocalPos() const           { return transform_.LocalPos(); }
    virtual void                  SetLocalPos(const hmm_vec3& p) { transform_.SetLocalPos(p); }
    [[nodiscard]] virtual hmm_vec3 LocalRotDeg() const;
    virtual void                  SetLocalRotDeg(const hmm_vec3& deg);
    [[nodiscard]] const hmm_vec3& LocalScl() const           { return transform_.LocalScl(); }
    virtual void                  SetLocalScl(const hmm_vec3& s) { transform_.SetLocalScl(s); }

    [[nodiscard]] TNodeInst* Parent() const;
    void                     SetParent(TNodeInst* parent);
    [[nodiscard]] int32_t    GetChildCount() const { return transform_.GetChildCount(); }
    [[nodiscard]] TNodeInst* GetChildAt(int32_t index) const;
    [[nodiscard]] TNodeInst* FindChild(std::string_view name) const;     // direct child, case-insensitive
    [[nodiscard]] TNodeInst* FindPath(std::string_view path) const;      // "a/b/c" relative to this

    // ---- Components ------------------------------------------------------
    int32_t AddComponent(std::unique_ptr<TObjectComponent> component);
        // Adds an owned component and returns its component slot, or -1.
        // Activates it immediately when ActivatesComponentsOnAdd() is true.
    void RemoveComponent(int32_t component_slot);
        // Detaches, deletes and frees the slot.
    void ActivateComponents();
        // Calls Activate() (OnAttach / RegisterUpdate hooks) on every owned
        // component that isn't active yet. TObjectInstance defers
        // activation until it has a map index and calls this from
        // SetMapIndex; the --test=vfx harness calls it on standalone
        // instances.
    void DetachComponents();
        // Detaches and destroys every component. Called by the destructor;
        // TObjectInstance calls it earlier in its own teardown so
        // components release resources before the imagery goes.
    [[nodiscard]] TObjectComponent* GetComponent(int32_t component_slot) const;
    [[nodiscard]] int32_t NumComponents() const { return int32_t(components_.size()); }
    template <class T>
    [[nodiscard]] T* GetComponent() const
    {
        for (const auto& component : components_)
            if (TObjectComponent* c = component.get())
                if (T* typed = dynamic_cast<T*>(c))
                    return typed;
        return nullptr;
    }
    template <class T>
    [[nodiscard]] T* FindComponent() const { return GetComponent<T>(); }

  protected:
    [[nodiscard]] virtual bool ActivatesComponentsOnAdd() const { return true; }

    TTransform transform_;
        // Authoritative spatial state. TObjectInstance keeps its legacy
        // `pos` field in lockstep with this (see SyncTransformPos).

  private:
    std::string node_name_;
    bool active_ = true;
    std::vector<std::unique_ptr<TObjectComponent>> components_;
};

// Safe reference to a component owned by a node. Resolves the owner
// through the node registry, then validates the component slot and
// generation before returning a pointer.
template <typename T = TObjectComponent>
class TSafeComponentRef
{
  public:
    TSafeComponentRef() = default;
    TSafeComponentRef(const T* component) { Set(component); }

    void Set(const T* component)
    {
        owner = component ? component->Owner() : nullptr;
        component_slot = component ? component->ComponentSlot() : -1;
        generation = component ? component->Generation() : 0;
    }
    void Clear()
    {
        owner.Clear();
        component_slot = -1;
        generation = 0;
    }

    [[nodiscard]] T* Get() const
    {
        TNodeInst* node = owner.Get();
        if (!node || component_slot < 0) return nullptr;
        TObjectComponent* component = node->GetComponent(component_slot);
        if (!component || component->Generation() != generation) return nullptr;
        return static_cast<T*>(component);
    }
    [[nodiscard]] bool IsValid() const { return Get() != nullptr; }

    explicit operator bool() const { return IsValid(); }
    T* operator->() const { return Get(); }
    T& operator*() const { return *Get(); }

  private:
    TSafeRef<TNodeInst> owner;
    int32_t component_slot = -1;
    uint32_t generation = 0;
};
