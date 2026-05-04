// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           gamemap.h - one loaded level's sectors, observable          *
// *************************************************************************
//
// TGameMap: per-level container. Holds every TSector at that level (no
// streaming -- modern hardware loads the whole map up front), plus a
// typed local-event listener list so consumers (MapRenderer, debug HUD,
// editor overlays, etc.) can react to Loaded / Updated / Unloaded.
//
// Owned by TMapManager (the cache of currently-loaded levels). Renderer
// holds a TSafeRef<TGameMap> against the current map -- generation check
// guards against silently rebinding after a level swap freed the map.
//
// Lifecycle:
//   TGameMap m;
//   m.Load(0);                      // loads all sectors at level 0
//   ... game runs ...
//   m.Unload();                     // frees sectors, fires Unloaded
//
// Construction registers the instance with TSafeObjectBase<TGameMap>'s
// per-type registry so TSafeRef<TGameMap> resolves through the same
// (id, gen) handle pattern TObjectInstance uses.
//
// *************************************************************************

#pragma once

#include "listenerlist.h"
#include "object.h"

#include <cstdint>
#include <vector>

class TSector;

enum class EGameMapEvent : uint8_t
{
    Loaded,    // Sectors freshly loaded; consumers should rebuild caches.
    Updated,   // Contents changed in place (object add/remove, sector
               // stream, etc.) -- consumers should re-scan or invalidate.
    Unloaded,  // About to free; consumers must drop any references RIGHT
               // NOW. After Notify returns, sector pointers are invalid.
};

class TGameMap : public TSafeObjectBase<TGameMap>
{
  public:
    TGameMap() = default;
    ~TGameMap() { Unload(); }

    // Load every sector at the given level. Idempotent: if already
    // loaded, a same-level Load is a no-op; a different-level Load
    // unloads first.
    bool Load(int32_t level);

    // Free all sectors. Fires Unloaded BEFORE the sectors are deleted
    // so subscribers can drop refs while the pointers are still
    // dereferenceable (for last-frame cleanup).
    void Unload();

    [[nodiscard]] bool    IsLoaded() const { return level >= 0; }
    [[nodiscard]] int32_t Level()    const { return level;       }

    // Full sector list (vector, not 2D windowed). Order is whatever
    // Load() inserted -- typically (sy, sx) sorted by the underlying
    // sector-coord scan.
    [[nodiscard]] const std::vector<TSector*>& Sectors() const { return sectors; }

    // Find a sector by (sx, sy). O(N) linear scan; small loaded counts
    // make this fine for now. Returns nullptr if not loaded.
    [[nodiscard]] TSector* FindSector(int32_t sx, int32_t sy) const;

    // ---- Listener API -------------------------------------------------
    using Listeners      = TListenerList<EGameMapEvent, TGameMap*>;
    using EventListenerId = Listeners::ListenerId;
    using EventCallback  = Listeners::Listener;

    EventListenerId AddListener(EventCallback fn)   { return listeners.Add(std::move(fn)); }
    void            RemoveListener(EventListenerId id) { listeners.Remove(id); }

    // Notify subscribers that the contents changed in place (no
    // load/unload boundary). Called by code that mutates a sector's
    // objects array via the editor or scripts.
    void NotifyUpdated() { listeners.Notify(EGameMapEvent::Updated, this); }

  private:
    int32_t               level = -1;
    std::vector<TSector*> sectors;
    Listeners             listeners;
};
