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
// TSafeComponentRef stays in object.h because it depends on
// TObjectInstance / TObjectComponent's full definitions.
// *************************************************************************

#pragma once

#include <atomic>
#include <cstdint>
#include <unordered_map>

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
// Implementation note: the registry is currently an unordered_map keyed
// by monotonically-increasing ids (in TObjectInstance's case the existing
// MapPane mapindex registry). The traditional AAA-perf shape is a
// versioned slot pool -- a contiguous std::vector indexed by slot, with
// per-slot generation counters bumped on free + match-on-lookup. Same
// observable contract on the SafeRef side; if we ever need it the swap
// is a registry-internal change with no call-site impact. For now the
// hash registry is well within budget.
//
// Both options for satisfying the contract are supported:
//
//   (a) Inherit TSafeObjectBase<Self> -- provides a per-type registry and
//       all three contract methods (id, gen, lookup) automatically. Use
//       for new types that don't already have an identity scheme.
//
//   (b) Implement the methods directly. TObjectInstance does this --
//       its existing GetMapIndex() + the LookupMapIndex() free function
//       are wrapped as SafeRefId / SafeRefLookup, plus a fresh
//       per-instance safe_ref_gen for SafeRefGen.
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
    [[nodiscard]] int32_t MapIndex() const { return idx; }
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
// contract via a per-type unordered registry. New types that don't already
// have an identity scheme inherit this and get id assignment, generation
// assignment, registration, and SafeRefLookup automatically.
//
// Construction allocates a fresh id (monotonic) and gen (global), inserts
// into the per-type registry. Destruction removes from the registry --
// any TSafeRef captured against this instance will resolve to nullptr.
//
// Usage:
//   class TGameMap : public TSafeObjectBase<TGameMap> { ... };
//   TSafeRef<TGameMap> ref = ptr;            // captures (id, gen)
//   if (TGameMap* live = ref.Get()) { ... }  // null if destroyed
// ---------------------------------------------------------------------------

template <typename Self>
class TSafeObjectBase
{
  public:
    TSafeObjectBase()
        : safe_id_(NextId())
        , safe_gen_(NextGen())
    {
        Registry()[safe_id_] = static_cast<Self*>(this);
    }

    ~TSafeObjectBase()
    {
        Registry().erase(safe_id_);
    }

    TSafeObjectBase(const TSafeObjectBase&)            = delete;
    TSafeObjectBase& operator=(const TSafeObjectBase&) = delete;
    TSafeObjectBase(TSafeObjectBase&&)                 = delete;
    TSafeObjectBase& operator=(TSafeObjectBase&&)      = delete;

    [[nodiscard]] int32_t  SafeRefId()  const { return safe_id_; }
    [[nodiscard]] uint32_t SafeRefGen() const { return safe_gen_; }

    static Self* SafeRefLookup(int32_t id)
    {
        if (id < 0) return nullptr;
        auto& m  = Registry();
        auto  it = m.find(id);
        return (it != m.end()) ? it->second : nullptr;
    }

  private:
    static std::unordered_map<int32_t, Self*>& Registry()
    {
        // Safe-ref registries are process-lifetime tables. Do not make this
        // a destructed function-local static: global teardown can delete
        // SafeRefable assets/objects after the registry's destructor has
        // already run, especially after FatalError/exit paths. Leaking this
        // tiny table is intentional and keeps late destructors safe.
        static auto* m = new std::unordered_map<int32_t, Self*>();
        return *m;
    }
    static int32_t NextId()
    {
        // Monotonic per-type, never reused. Same shape MapPane.MakeIndex
        // uses for TObjectInstance ids -- collisions are statistically
        // impossible inside a process lifetime.
        static std::atomic<int32_t> s_next{1};
        return s_next.fetch_add(1, std::memory_order_relaxed);
    }
    static uint32_t NextGen()
    {
        static std::atomic<uint32_t> s_next{1u};
        return s_next.fetch_add(1u, std::memory_order_relaxed);
    }

    int32_t  safe_id_;
    uint32_t safe_gen_;
};
