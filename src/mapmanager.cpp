// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           mapmanager.cpp - cache of loaded TGameMap levels           *
// *************************************************************************

#include "mapmanager.h"

#include "gamemap.h"

TMapManager::TMapManager()  = default;
TMapManager::~TMapManager() { EvictAll(); }

TGameMap* TMapManager::GetOrLoad(int32_t level)
{
    if (TGameMap* cached = GetCached(level))
        return cached;

    auto fresh = std::make_unique<TGameMap>();
    if (!fresh->Load(level))
        return nullptr;

    TGameMap* raw = fresh.get();
    cache.emplace_back(std::move(fresh));
    return raw;
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
