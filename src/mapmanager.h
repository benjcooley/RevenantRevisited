// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           mapmanager.h - cache of loaded TGameMap levels             *
// *************************************************************************
//
// TMapManager: owns the cache of loaded TGameMap instances and the
// "current map" pointer. Mirrors PlayerManager's pattern -- declared
// global in revenant.h, defined in revmain.cpp.
//
// Why a manager (not on MapPane): MapPane is the on-screen widget; this
// is pure game state (which levels are loaded, which is active). They
// have different lifetimes and concerns. MapPane / MapRenderer
// reference MapManager.CurrentMap(); the manager owns it.
//
// Multi-map cache: levels stay loaded once visited (Misthaven exteriors
// + interiors swap constantly; reloading from disk every door is the
// kind of friction we're done with). Manual EvictLevel / EvictAll for
// shutdown or memory pressure.
//
// Listener API mirrors TGameMap's: subscribers (MapPane, MapRenderer)
// register a callback for CurrentMapChanged and react -- e.g. the
// renderer drops draw caches and rebuilds against the new map.
//
// *************************************************************************

#pragma once

#include "listenerlist.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

class TGameMap;

enum class EMapManagerEvent : uint8_t
{
    CurrentMapChanged,    // current map pointer flipped (or cleared)
};

class TMapManager
{
  public:
    TMapManager();
    // Trivial destructor: at process exit the cache must already have
    // been torn down via Shutdown() under InitGlobals/ShutdownGlobals control.
    // Walking unique_ptr<TGameMap> from a global dtor would race other
    // TUs (TMapPane / TPlayer / inventories live elsewhere) and ASan
    // catches the resulting heap-use-after-free in TSector::Save.
    // Defined out-of-line so the unique_ptr<TGameMap> dtor sees the
    // complete type (gamemap.h is included in mapmanager.cpp).
    ~TMapManager();

    TMapManager(const TMapManager&)            = delete;
    TMapManager& operator=(const TMapManager&) = delete;

    // ---- Lifecycle ----------------------------------------------------
    // Init/Shutdown pattern: the global ctor/dtor must be trivial so
    // static-destruction order across TUs is harmless. Real wiring
    // happens explicitly from InitGlobals/ShutdownGlobals.
    bool Init();
    void Shutdown();

    // Returns the cached map at `level`, loading it from disk if not
    // already in the cache. Returns nullptr only if the load fails
    // (no sector files for that level). Caller does not own.
    TGameMap* GetOrLoad(int32_t level);

    // Lookup-only: returns the cached map for `level` or nullptr.
    // Doesn't trigger a disk load.
    [[nodiscard]] TGameMap* GetCached(int32_t level) const;

    // Set the current map. Pass nullptr to clear. Map must already be
    // in the cache (use GetOrLoad first or SetCurrentLevel below).
    // Fires CurrentMapChanged if the pointer actually changes.
    void SetCurrentMap(TGameMap* map);

    // Convenience: GetOrLoad(level) + SetCurrentMap(...). Returns the
    // new current map (may be nullptr if the load failed).
    TGameMap* SetCurrentLevel(int32_t level);

    [[nodiscard]] TGameMap* CurrentMap() const { return current; }
    [[nodiscard]] int32_t   CurrentLevel() const;

    // Visit every loaded/cached map. Asset residency code uses this to count
    // references from loaded maps, independent of which map is currently drawn.
    void ForEachLoadedMap(const std::function<void(TGameMap*)>& fn) const;

    // Force-evict a single level. The map is unloaded (fires its
    // Unloaded event) and removed from the cache. If the evicted map
    // was the current map, current is cleared and CurrentMapChanged
    // fires. No-op if the level isn't cached.
    void Evict(int32_t level);

    // Evict every cached map. Clears current (and notifies). Used at
    // shutdown.
    void EvictAll();

    // ---- Listener API -------------------------------------------------
    using Listeners       = TListenerList<EMapManagerEvent, TMapManager*>;
    using EventListenerId = Listeners::ListenerId;
    using EventCallback   = Listeners::Listener;

    EventListenerId AddListener(EventCallback fn)   { return listeners.Add(std::move(fn)); }
    void            RemoveListener(EventListenerId id) { listeners.Remove(id); }

  private:
    std::vector<std::unique_ptr<TGameMap>> cache;
    TGameMap*                              current     = nullptr;
    Listeners                              listeners;
    bool                                   initialized = false;
};
