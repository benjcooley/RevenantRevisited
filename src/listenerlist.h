// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           listenerlist.h - typed local-event delegate list           *
// *************************************************************************
//
// TListenerList<Args...> -- a small reusable callback list used by emitters
// that want to expose typed local-event subscriptions (e.g. TGameMap's
// Loaded / Updated / Unloaded events). Compile-time-typed payloads, no
// runtime type erasure beyond std::function. AddListener returns a stable
// id; RemoveListener takes that id back. Notify() fires each listener in
// insertion order with the given args.
//
// We use local subscription lists per emitter rather than a global event
// bus on purpose -- listener-of relationships stay greppable, payloads
// stay strongly typed, and lifetime is owned by the emitter.
//
// *************************************************************************

#pragma once

#include <algorithm>
#include <cstdint>
#include <functional>
#include <utility>
#include <vector>

template <typename... Args>
class TListenerList
{
  public:
    using ListenerId = uint32_t;
    using Listener   = std::function<void(Args...)>;

    // Subscribe. Returned id is stable for the listener's lifetime; pass
    // it to Remove() to unsubscribe (typically from the subscriber's
    // destructor).
    ListenerId Add(Listener fn)
    {
        const ListenerId id = ++next_id;
        entries.emplace_back(id, std::move(fn));
        return id;
    }

    // Unsubscribe. Safe if id is unknown / already removed.
    void Remove(ListenerId id)
    {
        entries.erase(
            std::remove_if(entries.begin(), entries.end(),
                           [id](const SEntry& e) { return e.id == id; }),
            entries.end());
    }

    // Fire all listeners in insertion order. Listeners may call Add /
    // Remove on this list during dispatch (we iterate by index against
    // the cached size at entry, so re-entrant adds fire on the next
    // Notify and re-entrant removes are honored next iteration).
    void Notify(Args... args) const
    {
        // Copy the snapshot so re-entrant Add/Remove during dispatch
        // don't invalidate iteration.
        const std::vector<SEntry> snapshot = entries;
        for (const SEntry& e : snapshot)
            e.fn(args...);
    }

    // Drop all listeners (destructor convenience).
    void Clear() { entries.clear(); }

    [[nodiscard]] size_t Size() const { return entries.size(); }

  private:
    struct SEntry
    {
        ListenerId id;
        Listener   fn;
        SEntry(ListenerId i, Listener f) : id(i), fn(std::move(f)) {}
    };

    std::vector<SEntry> entries;
    ListenerId          next_id = 0;
};
