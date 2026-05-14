// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *              assetcache.cpp - shared engine asset registry            *
// *************************************************************************

#include "assetcache.h"

#include <algorithm>
#include <cctype>
#include <utility>

namespace {

constexpr AssetUid kFnvOffset = 1469598103934665603ull;
constexpr AssetUid kFnvPrime = 1099511628211ull;

static char NormalizeAssetChar(char c)
{
    if (c == '\\')
        return '/';
    return char(std::tolower((unsigned char)c));
}

static void HashByte(AssetUid& hash, uint8_t byte)
{
    hash ^= byte;
    hash *= kFnvPrime;
}

static void HashNormalizedText(AssetUid& hash, std::string_view text)
{
    for (char c : text)
        HashByte(hash, uint8_t(NormalizeAssetChar(c)));
}

static SAssetKey NormalizeKey(SAssetKey key)
{
    if (key.uid == 0)
        key.uid = MakeClassicAssetUid(key.path, key.name);
    key.path = NormalizeAssetPath(key.path);
    key.name = NormalizeAssetName(key.name);
    return key;
}

static void ReleaseResourceRefs(std::vector<SAssetReferencedResource>& refs)
{
    for (SAssetReferencedResource& ref : refs)
        if (ref.release)
            ref.release();
    refs.clear();
}

} // namespace

std::string NormalizeAssetPath(std::string_view path)
{
    std::string out;
    out.reserve(path.size());
    bool previous_slash = false;
    for (char c : path)
    {
        const char n = NormalizeAssetChar(c);
        if (n == '/')
        {
            if (previous_slash)
                continue;
            previous_slash = true;
        }
        else
        {
            previous_slash = false;
        }
        out.push_back(n);
    }

    while (!out.empty() && out.back() == '/')
        out.pop_back();
    return out;
}

std::string NormalizeAssetName(std::string_view name)
{
    std::string out;
    out.reserve(name.size());
    for (char c : name)
        out.push_back(NormalizeAssetChar(c));
    return out;
}

AssetUid MakeClassicAssetUid(std::string_view path, std::string_view name)
{
    const std::string norm_path = NormalizeAssetPath(path);
    const std::string norm_name = NormalizeAssetName(name);
    AssetUid hash = kFnvOffset;
    HashNormalizedText(hash, norm_path);
    HashByte(hash, 0);
    HashNormalizedText(hash, norm_name);
    return hash ? hash : 1ull;
}

SAssetKey SAssetKey::Classic(std::string_view path, std::string_view name, EAssetKind kind)
{
    SAssetKey key;
    key.uid = MakeClassicAssetUid(path, name);
    key.kind = kind;
    key.path = NormalizeAssetPath(path);
    key.name = NormalizeAssetName(name);
    return key;
}

TGenericAsset::TGenericAsset(SAssetKey key)
    : key_(std::move(key))
{
}

TAsset::~TAsset()
{
    ReleaseReferencedResources();
}

uint32_t TAsset::AddAssetRef(uint32_t count)
{
    asset_ref_count_ += count;
    return asset_ref_count_;
}

uint32_t TAsset::ReleaseAssetRef(uint32_t count)
{
    if (count >= asset_ref_count_)
        asset_ref_count_ = 0;
    else
        asset_ref_count_ -= count;
    return asset_ref_count_;
}

void TAsset::AddReferencedResource(EAssetReferencedResourceKind kind,
                                   uint64_t key,
                                   uint32_t handle,
                                   std::string debug_name,
                                   FAssetRefRelease release)
{
    if (!release || handle == 0)
        return;

    SAssetReferencedResource ref;
    ref.kind = kind;
    ref.key = key;
    ref.handle = handle;
    ref.debug_name = std::move(debug_name);
    ref.release = std::move(release);
    referenced_resources_.push_back(std::move(ref));
}

void TAsset::ReleaseReferencedResources()
{
    std::vector<SAssetReferencedResource> refs = std::move(referenced_resources_);
    referenced_resources_.clear();
    ReleaseResourceRefs(refs);
}

void TAsset::ReleaseReferencedResources(EAssetReferencedResourceKind kind)
{
    std::vector<SAssetReferencedResource> refs;
    std::vector<SAssetReferencedResource> keep;
    refs.reserve(referenced_resources_.size());
    keep.reserve(referenced_resources_.size());

    for (SAssetReferencedResource& ref : referenced_resources_)
    {
        if (ref.kind == kind)
            refs.push_back(std::move(ref));
        else
            keep.push_back(std::move(ref));
    }

    referenced_resources_ = std::move(keep);
    ReleaseResourceRefs(refs);
}

const SAssetReferencedResource* TAsset::FindReferencedResource(
    EAssetReferencedResourceKind kind,
    uint64_t key) const
{
    for (const SAssetReferencedResource& ref : referenced_resources_)
    {
        if (ref.kind != kind)
            continue;
        if (key != 0 && ref.key != key)
            continue;
        return &ref;
    }
    return nullptr;
}

uint32_t TAsset::ReferencedResourceHandle(EAssetReferencedResourceKind kind, uint64_t key) const
{
    const SAssetReferencedResource* ref = FindReferencedResource(kind, key);
    return ref ? ref->handle : 0;
}

void TAsset::SetPayload(std::shared_ptr<void> ptr, std::type_index type, size_t bytes)
{
    payload_.ptr = std::move(ptr);
    payload_.type = payload_.ptr ? type : std::type_index(typeid(void));
    payload_.bytes = payload_.ptr ? bytes : 0;
}

TAssetCache::~TAssetCache()
{
    Shutdown();
}

bool TAssetCache::Init()
{
    initialized_ = true;
    return true;
}

void TAssetCache::Shutdown()
{
    assets_by_uid_.clear();
    assets_.clear();
    initialized_ = false;
}

TAsset* TAssetCache::FindPtr(AssetUid uid) const
{
    if (uid == 0)
        return nullptr;
    auto it = assets_by_uid_.find(uid);
    return (it != assets_by_uid_.end()) ? it->second : nullptr;
}

TAssetRef<> TAssetCache::Find(AssetUid uid) const
{
    return TAssetRef<>(FindPtr(uid));
}

TAssetRef<> TAssetCache::FindClassic(std::string_view path, std::string_view name) const
{
    return Find(MakeClassicAssetUid(path, name));
}

TAssetRef<> TAssetCache::RegisterAsset(const SAssetKey& in_key,
                                       std::shared_ptr<void> payload,
                                       std::type_index payload_type,
                                       size_t payload_bytes)
{
    SAssetKey key = NormalizeKey(in_key);

    if (TAsset* existing = FindPtr(key.uid))
    {
        if (payload && !existing->HasPayload())
            if (TGenericAsset* generic = dynamic_cast<TGenericAsset*>(existing))
                generic->SetPayload(std::move(payload), payload_type, payload_bytes);
        return TAssetRef<>(existing);
    }

    auto asset = std::make_unique<TGenericAsset>(std::move(key));
    if (payload)
        asset->SetPayload(std::move(payload), payload_type, payload_bytes);

    TAsset* raw = asset.get();
    assets_by_uid_[raw->Uid()] = raw;
    assets_.push_back(std::move(asset));
    return TAssetRef<>(raw);
}

std::vector<TAssetRef<>> TAssetCache::RegisterAssets(const std::vector<SAssetKey>& keys)
{
    std::vector<TAssetRef<>> refs;
    refs.reserve(keys.size());
    for (const SAssetKey& key : keys)
        refs.push_back(RegisterAsset(key));
    return refs;
}

TAssetRef<> TAssetCache::GetOrLoadAsset(const SAssetKey& in_key, const FAssetLoader& loader)
{
    SAssetKey key = NormalizeKey(in_key);
    if (TAsset* existing = FindPtr(key.uid))
    {
        TGenericAsset* generic = dynamic_cast<TGenericAsset*>(existing);
        if (generic && !existing->HasPayload() && loader)
        {
            SAssetPayload loaded = loader(generic->Key());
            if (loaded.ptr)
                existing->SetPayload(std::move(loaded.ptr), loaded.type, loaded.bytes);
        }
        return (!loader || existing->HasPayload() || !generic) ? TAssetRef<>(existing) : TAssetRef<>();
    }

    if (loader)
    {
        SAssetPayload loaded = loader(key);
        if (!loaded.ptr)
            return {};
        return RegisterAsset(key, std::move(loaded.ptr), loaded.type, loaded.bytes);
    }

    return RegisterAsset(key);
}

std::vector<TAssetRef<>> TAssetCache::GetOrLoadAssets(const std::vector<SAssetKey>& keys,
                                                      const FAssetLoader& loader)
{
    std::vector<TAssetRef<>> refs;
    refs.reserve(keys.size());
    for (const SAssetKey& key : keys)
        refs.push_back(GetOrLoadAsset(key, loader));
    return refs;
}

TAssetRef<> TAssetCache::RegisterClassic(std::string_view path,
                                         std::string_view name,
                                         EAssetKind kind,
                                         std::shared_ptr<void> payload,
                                         std::type_index payload_type,
                                         size_t payload_bytes)
{
    return RegisterAsset(SAssetKey::Classic(path, name, kind),
                         std::move(payload),
                         payload_type,
                         payload_bytes);
}

TAssetRef<> TAssetCache::GetOrLoadClassic(std::string_view path,
                                          std::string_view name,
                                          EAssetKind kind,
                                          const FAssetLoader& loader)
{
    return GetOrLoadAsset(SAssetKey::Classic(path, name, kind), loader);
}

uint32_t TAssetCache::IncrementRef(TAssetRef<> asset, uint32_t count)
{
    TAsset* live = asset.Get();
    return live ? live->AddRef(count) : 0;
}

uint32_t TAssetCache::DecrementRef(TAssetRef<> asset, uint32_t count)
{
    TAsset* live = asset.Get();
    return live ? live->ReleaseRef(count) : 0;
}

void TAssetCache::ResetRefCounts()
{
    for (auto& item : assets_by_uid_)
        if (item.second)
            item.second->ResetAssetRefCount();
}

size_t TAssetCache::ClearUnreferencedAssets()
{
    const size_t before = assets_.size();
    auto keep = std::remove_if(
        assets_.begin(),
        assets_.end(),
        [this](const std::unique_ptr<TGenericAsset>& asset) {
            if (!asset)
                return true;
            if (asset->RefCount() != 0)
                return false;

            auto it = assets_by_uid_.find(asset->Uid());
            if (it != assets_by_uid_.end() && it->second == asset.get())
                assets_by_uid_.erase(it);
            return true;
        });
    assets_.erase(keep, assets_.end());
    return before - assets_.size();
}

bool TAssetCache::RegisterExternalAsset(TAsset* asset)
{
    if (!asset || asset->AssetId() == 0)
        return false;

    auto it = assets_by_uid_.find(asset->AssetId());
    if (it != assets_by_uid_.end() && it->second != asset)
        return false;

    assets_by_uid_[asset->AssetId()] = asset;
    return true;
}

void TAssetCache::UnregisterExternalAsset(TAsset* asset)
{
    if (!asset || asset->AssetId() == 0)
        return;

    auto it = assets_by_uid_.find(asset->AssetId());
    if (it != assets_by_uid_.end() && it->second == asset)
        assets_by_uid_.erase(it);
}
