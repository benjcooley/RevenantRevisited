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
// Owned by TMapManager (the cache of currently-loaded levels). The map is
// the only owner of its sectors: Load / InitializeTransient create them;
// Unload / Discard free them. Borrowers of a sector pointer (TMapPane's
// window, the renderer's draw records) drop it on Unloaded. Renderer holds
// a TSafeRef<TGameMap> against the current map -- generation check guards
// against silently rebinding after a level swap freed the map.
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

#include "engine/core/listenerlist.h"
#include "game/object/object.h"

#include <cstdint>
#include <functional>
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
    // unloads first. A transient map rejects a different-level load.
    bool Load(int32_t level);

    // Load in stages, so a loading screen can show it filling (retail filled
    // its bar per sector, 0x004997d0): BeginLoad finds the level's sector
    // files (as Load: a same-level map is already done); LoadSectors reads
    // up to `count` more and, once every one is in, stamps the tile walkmaps,
    // fires Loaded and returns true. Load is BeginLoad + all of LoadSectors.
    bool BeginLoad(int32_t level);
    bool LoadSectors(int32_t count);
    [[nodiscard]] bool  Loading() const { return loading; }
    [[nodiscard]] float LoadFraction() const;

    // A sector's place on its level.
    struct SSectorCoord { int32_t sx = 0; int32_t sy = 0; };

    // Create empty disposable sectors directly in memory, without consulting
    // sector files. Bounds are inclusive, 0..255 on each axis, at most 8192
    // sectors; level must fit 0..255. Fails without changing a loaded map.
    // Fires Loaded after creation. Transient maps never save their sectors.
    bool InitializeTransient(int32_t level, int32_t min_sx, int32_t min_sy,
                             int32_t max_sx, int32_t max_sy);

    // Free all sectors, writing persistent sectors to the working set. Fires
    // Unloaded BEFORE the sectors are deleted so subscribers can drop refs
    // while the pointers are still dereferenceable (for last-frame cleanup).
    void Unload();

    // Free all sectors without writing them, for when the working set is
    // about to be replaced (new or loaded game). Fires Unloaded like Unload.
    void Discard();

    // Write persistent sectors to the working set. Transient maps are skipped.
    void Flush() const;

    [[nodiscard]] bool    IsTransient() const { return transient; }
    [[nodiscard]] bool    IsLoaded() const { return level >= 0; }
    [[nodiscard]] int32_t Level()    const { return level;       }

    // Full sector list (vector, not 2D windowed). Order is whatever
    // Load() inserted -- typically (sy, sx) sorted by the underlying
    // sector-coord scan.
    [[nodiscard]] const std::vector<TSector*>& Sectors() const { return sectors; }

    // Find a sector by (sx, sy), through the grid index. Returns nullptr
    // if not loaded.
    [[nodiscard]] TSector* FindSector(int32_t sx, int32_t sy) const;

    // The sector containing world position `pos`, or nullptr.
    [[nodiscard]] TSector* SectorAt(const S3DPoint& pos) const;

    // The OF_NONMAP objects (the players) standing in this map's sectors.
    // The map doesn't own them: they outlive its sectors.
    [[nodiscard]] std::vector<TSafeRef<TObjectInstance>> NonMapObjects() const;

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

    // Stamp `oi`'s walkmap footprint (any object whose imagery has one:
    // tiles, exits such as elevators, ...) into whatever sectors
    // `find_sector(sx, sy)` resolves to. Static so both TGameMap::Load
    // (initial stamp with self-FindSector resolver) and TMapPane's
    // runtime path (object moves with its own window-aware resolver)
    // can share the bbox / facing-rotation math.
    //
    // `mode` is one of WALK_TRANSFER / WALK_CAPTURE / WALK_CLEAR /
    // WALK_EXTRACT (defined in mappane.h).
    using FindSectorFn = std::function<TSector*(int32_t sx, int32_t sy)>;
    static bool StampWalkmap(TObjectInstance* oi, int32_t mode, const FindSectorFn& find_sector);
      // False when there was nothing to stamp (no map index, NOWALK on a
      // transfer, no walkmap for the object's state)

  private:
    enum class ESectorRelease : uint8_t { Save, Discard };
    void Release(ESectorRelease how);

    void FinishLoad();
    void AddSector(TSector* sec);

    // Sector coordinates run 0..255 on each axis.
    static constexpr int32_t kGridSide = 256;

    int32_t               level = -1;
    std::vector<TSector*> sectors;
    // (sx, sy) -> its sector, kGridSide x kGridSide, sized with the first
    // sector. Walk queries resolve every cell outside the pane's window
    // through FindSector (retail searched a list, 0x00499e10), and a
    // whole-level walk analysis makes millions of them.
    std::vector<TSector*> grid;
    Listeners             listeners;

    std::vector<SSectorCoord> pending;      // the level's sector files, in load order
    size_t                nextpending = 0;  // the next one to read
    int32_t               loadedobjs  = 0;
    bool                  loading     = false;
    bool                  transient   = false;
};
