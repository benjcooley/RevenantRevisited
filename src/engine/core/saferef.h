// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           saferef.h - id-backed weak reference + base helper          *
// *************************************************************************
//
// Two pieces of the engine's "safe reference" infrastructure, factored
// out of object.h so types that just need TSafeRef / TSafeObjectBase
// (e.g. TTransform) don't have to drag the umbrella include chain in.
//
// TSafeRef<T>     -- typed weak ref (id + generation).
// TSafeObjectBase -- CRTP helper that implements the TSafeRef<T>
//                    contract automatically via a per-type registry.
//
// TSafeComponentRef lives in node.h because it depends on
// TObjectInstance / TComponent's full definitions.
// *************************************************************************

#pragma once

#include <cstdint>
#include <vector>

class TObjectInstance;  // for the default template arg of TSafeRef<>

// ---------------------------------------------------------------------------
// TSafeRef<T> — safe, id-backed reference to any "saferef-able" type.
//
// Holds an integer id (not a raw pointer) so the referent can be deleted,
// paged out, or reassigned between uses and this ref never dangles.
// Resolution goes through whatever per-type registry T provides, returning
// nullptr when the referent no longer exists.
//
// Contract a type T must satisfy to be referenced by TSafeRef<T>:
//
//   int32_t  SafeRefId()  const;           // member: stable id, -1 if none
//   uint32_t SafeRefGen() const;           // member: per-instance generation
//   static T* SafeRefLookup(int32_t id);   // static: id -> pointer or null
//
// The generation guards against id recycling: a SafeRef captures
// (id, generation) at construction. If the original referent is destroyed
// and a new instance later inherits the same slot id, its generation will
// differ -- TSafeRef<T>::Get() returns nullptr instead of silently
// resolving to the wrong object. Generation is per-instance and globally
// unique, so even reuse-free workloads pay only an int32 compare per
// dereference.
//
// The id is a slot in the type's handle table (TSafeObjectBase below: a
// contiguous array indexed by slot, each slot carrying the generation of
// its tenant, bumped when the slot is freed), and the generation is that
// slot's generation when the reference was taken. A slot is reused; the
// generation says whether the tenant is still the one referenced. It is
// never a map index: a map index is the retail file's field for an
// object, only mostly unique, not an identity. A link that must survive
// a save is a node uid (TNodeRef, TObjectRef<T>), not a TSafeRef.
//
// Both options for satisfying the contract are supported:
//
//   (a) Inherit TSafeObjectBase<Self> -- the handle table and all three
//       contract methods (id, gen, lookup) come with it.
//
//   (b) Forward the methods. TObjectInstance does this through its node:
//       a reference to an object is a reference to its node.
//
// Typical use is unchanged from when this was TObjectInstance-only:
//   TSafeRef<TObjectInstance> target;     // empty
//   target = oi;                          // capture the id
//   if (TObjectInstance* live = target.Get()) { ... } // nullptr if gone
//   if (target) target->Use(...);         // operator-> nullptr-safe
// ---------------------------------------------------------------------------

template <typename T = TObjectInstance>
class TSafeRef
{
  public:
    TSafeRef() = default;
    TSafeRef(const T* inst)
        : idx(inst ? inst->SafeRefId()  : -1)
        , gen(inst ? inst->SafeRefGen() : 0u) {}
    TSafeRef(int32_t id, uint32_t gen_) : idx(id), gen(gen_) {}

    TSafeRef& operator=(const T* inst)
    {
        idx = inst ? inst->SafeRefId()  : -1;
        gen = inst ? inst->SafeRefGen() : 0u;
        return *this;
    }

    [[nodiscard]] T* Get() const
    {
        if (idx < 0) return nullptr;
        // T::SafeRefLookup may be inherited from a base (e.g. TPlayer
        // inherits TObjectInstance::SafeRefLookup which returns
        // TObjectInstance*); the static_cast handles the down-cast.
        // Same retype contract as a C-style downcast.
        T* p = static_cast<T*>(T::SafeRefLookup(idx));
        // Generation check: the slot may have been reused by a fresh
        // instance with a different gen; in that case the ref is stale
        // and must resolve to nullptr instead of the wrong object.
        if (!p || p->SafeRefGen() != gen) return nullptr;
        return p;
    }
    [[nodiscard]] bool IsValid() const { return Get() != nullptr; }
    [[nodiscard]] int32_t Id() const { return idx; }
    [[nodiscard]] uint32_t Gen() const { return gen; }
    void Clear() { idx = -1; gen = 0u; }

    explicit operator bool() const { return IsValid(); }
    T* operator->() const { return Get(); }
    T& operator*()  const { return *Get(); }

    // Equality compares (id, gen) so two refs to recycled-then-freshly-
    // assigned slots aren't conflated.
    bool operator==(const TSafeRef& rhs) const { return idx == rhs.idx && gen == rhs.gen; }
    bool operator!=(const TSafeRef& rhs) const { return !(*this == rhs); }

  private:
    int32_t  idx = -1;
    uint32_t gen = 0u;
};

// Resolve an id (or any TSafeRef<U>) to a typed pointer of choice. Same
// retype contract as a C-style downcast.
//
// safe_cast<T>(id) is the *unchecked* form -- skips the generation check.
// Use only when the caller knows the id is fresh (e.g. just-issued in the
// same frame). Prefer the (id, gen) or TSafeRef<U> overloads everywhere
// else so a recycled id doesn't silently resolve to the new tenant.
template <class T>
[[nodiscard]] inline T* safe_cast(int32_t id)
{
    if (id < 0) return nullptr;
    return static_cast<T*>(T::SafeRefLookup(id));
}

template <class T>
[[nodiscard]] inline T* safe_cast(int32_t id, uint32_t gen)
{
    if (id < 0) return nullptr;
    T* p = static_cast<T*>(T::SafeRefLookup(id));
    return (p && p->SafeRefGen() == gen) ? p : nullptr;
}

template <class T, class U>
[[nodiscard]] inline T* safe_cast(const TSafeRef<U>& ref)
{
    return safe_cast<T>(ref.Id(), ref.Gen());
}

// ---------------------------------------------------------------------------
// TSafeObjectBase<Self> -- CRTP helper that implements the TSafeRef<T>
// contract with a per-type handle table: a contiguous array of slots,
// each holding its tenant and a generation. Construction takes a slot
// (a freed one first, else a new one); destruction frees it and bumps its
// generation, so every TSafeRef taken against the old tenant resolves to
// nullptr, and the slot can serve a new tenant under a new generation.
//
// Usage:
//   class TGameMap : public TSafeObjectBase<TGameMap> { ... };
//   TSafeRef<TGameMap> ref = ptr;            // captures (slot, gen)
//   if (TGameMap* live = ref.Get()) { ... }  // null if destroyed
// ---------------------------------------------------------------------------

template <typename Self>
class TSafeObjectBase
{
  public:
    TSafeObjectBase()
    {
        safe_id_  = Table().Acquire(static_cast<Self*>(this));
        safe_gen_ = Table().Generation(safe_id_);
    }

    ~TSafeObjectBase()
    {
        Table().Release(safe_id_);
    }

    TSafeObjectBase(const TSafeObjectBase&)            = delete;
    TSafeObjectBase& operator=(const TSafeObjectBase&) = delete;
    TSafeObjectBase(TSafeObjectBase&&)                 = delete;
    TSafeObjectBase& operator=(TSafeObjectBase&&)      = delete;

    [[nodiscard]] int32_t  SafeRefId()  const { return safe_id_; }
    [[nodiscard]] uint32_t SafeRefGen() const { return safe_gen_; }

    // The slot's tenant, whoever it is now: TSafeRef::Get compares the
    // generation to tell the one it referenced from a successor.
    static Self* SafeRefLookup(int32_t id) { return Table().At(id); }

  private:
    // The handle table: slots in a vector, freed slots on a free list.
    // Process-lifetime, leaked on purpose: global teardown destroys
    // referenced objects after a function-local static's destructor would
    // have run, especially on the FatalError and exit paths.
    class THandleTable
    {
      public:
        int32_t Acquire(Self* tenant)
        {
            int32_t slot;
            if (!free_.empty())
            {
                slot = free_.back();
                free_.pop_back();
            }
            else
            {
                slot = int32_t(slots_.size());
                slots_.push_back({});
            }
            slots_[size_t(slot)].tenant = tenant;
            return slot;
        }
        void Release(int32_t slot)
        {
            SSlot& s = slots_[size_t(slot)];
            s.tenant = nullptr;
            ++s.generation;             // every reference taken so far is stale
            free_.push_back(slot);
        }
        [[nodiscard]] Self* At(int32_t slot) const
        {
            return slot >= 0 && size_t(slot) < slots_.size() ? slots_[size_t(slot)].tenant : nullptr;
        }
        [[nodiscard]] uint32_t Generation(int32_t slot) const { return slots_[size_t(slot)].generation; }

      private:
        struct SSlot
        {
            Self*    tenant     = nullptr;
            uint32_t generation = 1u;   // 0 is no generation: a default TSafeRef never matches
        };
        std::vector<SSlot>   slots_;
        std::vector<int32_t> free_;
    };
    static THandleTable& Table()
    {
        static auto* table = new THandleTable();
        return *table;
    }

    int32_t  safe_id_  = -1;
    uint32_t safe_gen_ = 0u;
};
