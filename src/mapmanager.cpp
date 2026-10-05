// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           mapmanager.cpp - cache of loaded TGameMap levels           *
// *************************************************************************

#include "mapmanager.h"

#include "gamemap.h"
#include "logging.h"
#include "object.h"
#include "sector.h"
#include "sectorstore.h"

TMapManager::TMapManager()  = default;
// Trivial dtor: Shutdown() must have run already (via ShutdownGlobals) so
// the cache is empty here. If a developer adds a new caller path that
// skips Shutdown, the unique_ptr<TGameMap> dtors will still run, but
// that's the bug we're avoiding by making Shutdown explicit -- see the
// header comment.
TMapManager::~TMapManager() = default;

bool TMapManager::Init()
{
    if (initialized) return true;
    // No allocations / listener registration today; the registry just
    // becomes "live" so Shutdown knows to actually tear things down.
    initialized = true;
    return true;
}

void TMapManager::Shutdown()
{
    if (!initialized) return;
    EvictAll();         // walks cache + fires CurrentMapChanged
    initialized = false;
}

TGameMap* TMapManager::GetOrLoad(int32_t level)
{
    if (TGameMap* cached = GetCached(level))
    {
        cached->LoadSectors(INT32_MAX);     // a staged load finishes now
        return cached;
    }

    auto fresh = std::make_unique<TGameMap>();
    if (!fresh->Load(level))
        return nullptr;

    TGameMap* raw = fresh.get();
    cache.emplace_back(std::move(fresh));
    return raw;
}

TGameMap* TMapManager::LoadStaged(int32_t level, int32_t count)
{
    TGameMap* map = GetCached(level);
    if (!map)
    {
        auto fresh = std::make_unique<TGameMap>();
        if (!fresh->BeginLoad(level))
            return nullptr;
        map = fresh.get();
        cache.emplace_back(std::move(fresh));
    }
    map->LoadSectors(count);
    return map;
}

TGameMap* TMapManager::GetCached(int32_t level) const
{
    for (const std::unique_ptr<TGameMap>& m : cache)
        if (m && m->Level() == level)
            return m.get();
    return nullptr;
}

void TMapManager::SetCurrentMap(TGameMap* map)
{
    if (current == map) return;
    current = map;
    listeners.Notify(EMapManagerEvent::CurrentMapChanged, this);
}

TGameMap* TMapManager::SetCurrentLevel(int32_t level)
{
    TGameMap* m = GetOrLoad(level);
    SetCurrentMap(m);
    return m;
}

int32_t TMapManager::CurrentLevel() const
{
    return current ? current->Level() : -1;
}

void TMapManager::ForEachLoadedMap(const std::function<void(TGameMap*)>& fn) const
{
    if (!fn)
        return;
    for (const std::unique_ptr<TGameMap>& m : cache)
        if (m)
            fn(m.get());
}

void TMapManager::Evict(int32_t level)
{
    for (auto it = cache.begin(); it != cache.end(); ++it)
    {
        if (!*it || (*it)->Level() != level) continue;
        const bool was_current = (current == it->get());
        // unique_ptr destructor -> ~TGameMap -> Unload -> Notify(Unloaded).
        cache.erase(it);
        if (was_current)
        {
            current = nullptr;
            listeners.Notify(EMapManagerEvent::CurrentMapChanged, this);
        }
        return;
    }
}

void TMapManager::EvictAll()
{
    const bool had_current = (current != nullptr);
    current = nullptr;
    cache.clear();        // each unique_ptr -> ~TGameMap -> Unload notify
    if (had_current)
        listeners.Notify(EMapManagerEvent::CurrentMapChanged, this);
}

void TMapManager::Notify(uint32_t notify, void* ptr) const
{
    for (const std::unique_ptr<TGameMap>& map : cache)
    {
        if (!map)
            continue;
        for (TSector* sector : map->Sectors())
        {
            if (!sector)
                continue;
            for (int32_t i = 0; i < sector->NumObjSetItems(OBJSET_NOTIFY); i++)
            {
                TObjectInstance* object = sector->GetObjSetInstance(OBJSET_NOTIFY, i);
                if (object && (object->Flags() & OF_NOTIFY))
                    object->Notify(notify, ptr);
            }
        }
    }
}

void TMapManager::FlushSectors() const
{
    for (const std::unique_ptr<TGameMap>& m : cache)
        if (m)
            m->Flush();
}

void TMapManager::ClearCurMap()
{
    for (const std::unique_ptr<TGameMap>& m : cache)
        if (m)
            m->Discard();
    EvictAll();
    SectorStore::Clear();
}

void TMapManager::LoadCurMap(const std::filesystem::path& dir)
{
    ClearCurMap();
    SectorStore::ImportFrom(dir);
}

void TMapManager::SaveCurMap(const std::filesystem::path& dir) const
{
    FlushSectors();
    SectorStore::ExportTo(dir);
}

void TMapManager::ReloadLevel(int32_t level, const std::function<void()>& editFiles)
{
    TGameMap* map = GetCached(level);
    if (!map)
    {
        if (editFiles)
            editFiles();
        return;
    }

    // The players outlive the sectors (a sector lets go of its OF_NONMAP
    // objects when it is freed). Hold them by reference: `editFiles` runs
    // console commands.
    const std::vector<TSafeRef<TObjectInstance>> nonmap = map->NonMapObjects();
    const bool wasCurrent = (map == current);

    Evict(level);               // writes the sectors to the working set, then frees them
    if (editFiles)
        editFiles();

    TGameMap* reloaded = GetOrLoad(level);
    if (!reloaded)
    {
        log_error("[mapmanager] level %d didn't load again", level);
        return;
    }
    int32_t placed = 0;
    for (const TSafeRef<TObjectInstance>& ref : nonmap)
    {
        TObjectInstance* oi = ref.Get();
        if (!oi || oi->GetSector())
            continue;
        if (TSector* sector = reloaded->SectorAt(oi->Pos()))
        {
            sector->AddObject(oi);
            ++placed;
        }
        else
            log_warn("[mapmanager] level %d: no sector under '%s' after the reload",
                     level, oi->GetName() ? oi->GetName() : "?");
    }
    log_info("[mapmanager] level %d reloaded from its sector files; %d of %zu non-map "
             "object(s) back in place", level, placed, nonmap.size());
    if (wasCurrent)
        SetCurrentMap(reloaded);
}

void TMapManager::ReloadSectors()
{
    std::vector<int32_t> levels;
    for (const std::unique_ptr<TGameMap>& m : cache)
        if (m)
            levels.push_back(m->Level());
    for (const int32_t level : levels)
        ReloadLevel(level);
}
