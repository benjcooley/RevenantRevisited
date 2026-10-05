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
// Ownership: TMapManager owns the maps, each TGameMap owns its sectors and
// each TSector owns its objects (except OF_NONMAP ones, the players, which
// it only holds while they stand in it). Everything that loads, saves or
// frees sectors goes through here; other holders of a sector pointer
// borrow it and must drop it on the map's Unloaded event (TMapPane's
// window, the renderer) or hold a TSafeRef<TGameMap> and re-resolve.
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
#include <filesystem>
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

    // REVSYNC: TMapPane::Notify @ 0x0045a680 — tell every object that asked
    // for notifications (OF_NOTIFY) about a world change (N_SCRIPTADDED,
    // N_SCRIPTDELETED, ...). Retail walked the loaded sectors, which were
    // MapPane's; in the port they are every loaded map's.
    void Notify(uint32_t notify, void* ptr) const;

    // Force-evict a single level. The map is unloaded (fires its
    // Unloaded event) and removed from the cache. If the evicted map
    // was the current map, current is cleared and CurrentMapChanged
    // fires. No-op if the level isn't cached.
    void Evict(int32_t level);

    // Evict every cached map. Clears current (and notifies). Used at
    // shutdown.
    void EvictAll();

    // ---- Working set (retail TMapPane curmap functions) ---------------
    // The sectors a game has modified live in the working set
    // (SectorStore). These move it between the loaded world and save slots.
    // See docs/gameflow/forensics/SAVE_GAME.md §4-5.

    // REVSYNC: 0x00499e60 — write every loaded sector to the working set.
    void FlushSectors() const;

    // REVSYNC: TMapPane::ClearCurMap @ 0x0044e460 — drop every loaded map
    // and empty the working set, so the world reverts to the base map.
    // Loaded sectors are discarded rather than saved (retail saved them and
    // then deleted the files; the outcome is the same).
    void ClearCurMap();

    // REVSYNC: TMapPane::LoadCurMap @ 0x0044e050 — ClearCurMap, then take
    // the working set from `dir` (a save slot's CurMap).
    void LoadCurMap(const std::filesystem::path& dir);

    // REVSYNC: TMapPane::SaveCurMap @ 0x0044e250 — flush, then copy the
    // working set into `dir`. Retail first copied the previously loaded
    // slot's CurMap into `dir`; LoadCurMap already imported that into the
    // working set, so the copy is a no-op and isn't repeated here.
    void SaveCurMap(const std::filesystem::path& dir) const;

    // ---- Reloading from the sector files (editor commands) ------------
    // REVSYNC: TMapPane::FreeAllSectors @ 0x00458ff0 and the reload that
    // followed it (the next sector update). Write `level`'s sectors to the
    // working set and drop them, run `editFiles` while the level is out of
    // memory -- its sector files are authoritative then -- and load the
    // level again if it was loaded. Objects the map doesn't own (OF_NONMAP:
    // the players) stay in the world and go back into the sector under
    // them, as retail's sector update re-added them. A level that isn't
    // loaded just runs `editFiles`.
    void ReloadLevel(int32_t level, const std::function<void()>& editFiles = {});

    // REVSYNC: TMapPane::ReloadSectors @ 0x004590e0 — ReloadLevel for every
    // loaded level.
    void ReloadSectors();

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
