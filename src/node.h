// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   node.h - TNode: a named, soft-ref-able transform node with         *
// *            components. The unit a map object and a VFX prefab are    *
// *            built from.                                               *
// *************************************************************************
//
// A node is the smallest thing in the scene that can be addressed by a
// soft ref, placed in a transform hierarchy, and given behaviour through
// components. It is final: there is one kind of node. A map object is a
// root node in a sector whose first component is a TObjectInstance (the
// root of the object class hierarchy, docs/OBJECT_MODEL_PLAN.md §3b); the
// node caches that component at attach time, so node->Object() and
// instance->Node() are pointer reads. Prefab nodes (docs/vfx/PREFAB_SYSTEM.md
// §1) have no object: never sector members, never saved, never pulsed by
// MapPane. Sectors are not nodes: the world is a streaming grid of sectors
// holding root nodes, with no scene graph above a root.
//
// Identity: TSafeObjectBase<TNode> gives every node an id + generation
// in the node registry, so TSafeRef<TNode> and TSafeComponentRef<T>
// resolve through it. A map object also carries its map index, the id the
// sector and save streams use and TSafeRef<TObjectInstance> captures; the
// two id spaces coexist until the registries are unified.
//
// Hierarchy lives on the embedded TTransform (parent / children links are
// non-owning). TTransform carries a back-pointer to its node so traversal
// by name and path resolution ("a/b/c") work from the transform tree.
// Ownership of child nodes stays with whoever created them.
//
// Components: owned (unique_ptr), slot-addressed, generation-stamped on
// attach so TSafeComponentRef<T> rejects a recycled slot. A component is
// activated (OnAttach, update registration) on AddComponent unless the
// node is a map object without a map index yet; SetMapIndex activates
// them then. The node destroys its object component first (the object's
// destructor detaches the rest while the imagery is still whole) and
// deletes everything else after it.
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
class TSector;

class TNode final : public TSafeObjectBase<TNode>
{
  public:
    TNode();
    explicit TNode(std::string name);
    ~TNode();
    TNode(const TNode&) = delete;
    TNode& operator=(const TNode&) = delete;

    // ---- Identity --------------------------------------------------------
    DEF_BLOCK("node")
    PROP_FN(1, std::string, name,   GetName,     );                   // read-only
    PROP_FN(2, hmm_vec3,    pos,    LocalPos,    SetLocalPos);
    PROP_FN(3, hmm_vec3,    rot,    LocalRotDeg, SetLocalRotDeg);     // euler, degrees
    PROP_FN(4, hmm_vec3,    scl,    LocalScl,    SetLocalScl, def({1, 1, 1}));
    PROP_FN(5, bool,        active, Active,      SetActive, def(true), "Inactive: subtree neither ticked nor drawn");

    // The node's own name, or its object's name when it has none of its
    // own (a map object is addressed by the object name).
    [[nodiscard]] const char* GetName() const;
    void SetNodeName(std::string name) { node_name_ = std::move(name); }

    // The node-registry id (what TSafeRef<TNode> captures). Distinct
    // from a map object's map index; the particle runtime keys owners by it.
    [[nodiscard]] int32_t NodeId() const { return TSafeObjectBase<TNode>::SafeRefId(); }

    // The object component, when this node is a map object: cached at
    // attach, never searched. nullptr for a plain prefab node.
    [[nodiscard]] TObjectInstance* Object() const { return object_; }

    // ---- Map membership --------------------------------------------------
    // A node with a sector is in the map; a node without one is not. A whole
    // tree shares one sector: SetSector recurses, and a child adopts its
    // parent's on attach. Only a root may be a member of the sector's object
    // list (the things the map pulses and draws); a contained object, say an
    // item in a chest, is in the map through its root.
    // The map index is the id the sector and save streams use;
    // TObjectInstance::SetMapIndex keeps the map index registry in step and
    // is the one to call, the setter here is the field. The level is the map
    // level, which a NONMAP object such as the player carries without a
    // sector.
    [[nodiscard]] bool     IsRoot() const   { return Parent() == nullptr; }
    [[nodiscard]] bool     InMap() const    { return sector_ != nullptr; }
    [[nodiscard]] int32_t  MapIndex() const { return mapindex_; }
    [[nodiscard]] TSector* Sector() const   { return sector_; }
    [[nodiscard]] int32_t  Level() const    { return level_; }
    void SetMapIndex(int32_t index) { mapindex_ = index; }
    void SetSector(TSector* sector);        // this node and its whole subtree
    void SetLevel(int32_t level)    { level_ = level; }

    [[nodiscard]] bool Active() const { return active_; }
    void SetActive(bool active) { active_ = active; }

    // ---- Transform / hierarchy ------------------------------------------
    [[nodiscard]] TTransform&       Transform()       { return transform_; }
    [[nodiscard]] const TTransform& Transform() const { return transform_; }

    // SetLocalPos on a map object is the map move (TObjectInstance::SetPos:
    // sector, walkmap, shadow and light bookkeeping), so a track, the
    // inspector or a prefab moving the node keeps the map consistent. The
    // raw write is SetIntPos (intworld.h) or the transform itself.
    [[nodiscard]] const hmm_vec3& LocalPos() const           { return transform_.LocalPos(); }
    void                          SetLocalPos(const hmm_vec3& p);
    [[nodiscard]] hmm_vec3        LocalRotDeg() const;
    void                          SetLocalRotDeg(const hmm_vec3& deg);
    [[nodiscard]] const hmm_vec3& LocalScl() const           { return transform_.LocalScl(); }
    void                          SetLocalScl(const hmm_vec3& s) { transform_.SetLocalScl(s); }

    [[nodiscard]] TNode* Parent() const;
    void                     SetParent(TNode* parent);
    [[nodiscard]] int32_t    GetChildCount() const { return transform_.GetChildCount(); }
    [[nodiscard]] TNode* GetChildAt(int32_t index) const;
    [[nodiscard]] TNode* FindChild(std::string_view name) const;     // direct child, case-insensitive
    [[nodiscard]] TNode* FindPath(std::string_view path) const;      // "a/b/c" relative to this

    // ---- Components ------------------------------------------------------
    int32_t AddComponent(std::unique_ptr<TObjectComponent> component);
        // Adds an owned component and returns its component slot, or -1.
        // Activates it at once unless this is a map object without a map
        // index yet.
    void RemoveComponent(int32_t component_slot);
        // Detaches, deletes and frees the slot. The object component cannot
        // be removed; delete the node.
    void ActivateComponents();
        // Calls Activate() (OnAttach / RegisterUpdate hooks) on every owned
        // component that isn't active yet. TObjectInstance::SetMapIndex
        // calls this once the object has a map index; the --test=vfx
        // harness calls it on standalone instances.
    void DetachComponents();
        // Detaches and destroys every component but the object. Called by
        // the destructor; TObjectInstance calls it earlier in its own
        // teardown so components release resources before the imagery
        // goes. Re-entrant: a nested call finds nothing left to do.
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

  private:
    [[nodiscard]] bool ActivatesComponentsOnAdd() const { return !object_ || mapindex_ >= 0; }

    TTransform transform_;          // Authoritative spatial state
    std::string node_name_;
    bool active_ = true;
    std::vector<std::unique_ptr<TObjectComponent>> components_;
    TObjectInstance* object_ = nullptr;     // the object component, cached at attach
    int32_t object_slot_ = -1;
    int32_t mapindex_ = -1;
    TSector* sector_ = nullptr;
    int32_t level_ = 0;
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
        TNode* node = owner.Get();
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
    TSafeRef<TNode> owner;
    int32_t component_slot = -1;
    uint32_t generation = 0;
};
