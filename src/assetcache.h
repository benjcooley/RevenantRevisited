// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *              assetcache.h - shared engine asset registry              *
// *************************************************************************
//
// TAssetCache is the engine-side cache for source/CPU assets. It maps a
// stable AssetUid to a SafeRef-able TAsset record. Renderer GPU resources are
// not owned here; TRenderer keeps the GPU realization cache and may use an
// AssetUid as its source key.
//
// Current invariants:
//   * An asset is identified by AssetUid. Classic assets derive that uid from
//     normalized path + name, matching the way the original game names data.
//   * A TAsset is the SafeRef target. Callers keep TSafeRef<TAsset> handles;
//     they do not own asset memory or raw pointers.
//   * Refcount zero never evicts as a side effect of decrement. Counts are
//     bookkeeping for residency; ClearUnreferencedAssets is the explicit trim.
//   * Payloads are type-erased shared_ptrs so legacy malloc-backed data,
//     modern parsed CPU assets, and small generated records can all live here.
//   * Wrapper assets may retain renderer GPU refs through referenced-resource
//     callbacks. Clearing the source asset releases those refs, but TRenderer
//     still owns the actual GPU resource lifetime.
//
// *************************************************************************

#pragma once

#include "saferef.h"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <typeindex>
#include <unordered_map>
#include <vector>

using AssetUid = uint64_t;

enum class EAssetKind : uint16_t
{
    Unknown = 0,
    ClassicResource,
    Bitmap,
    Imagery,
    I2D,
    I3D,
    Animation,
    Sound,
};

struct SAssetKey
{
    AssetUid uid = 0;
    EAssetKind kind = EAssetKind::Unknown;
    std::string path;
    std::string name;

    static SAssetKey Classic(std::string_view path,
                             std::string_view name,
                             EAssetKind kind = EAssetKind::ClassicResource);
};

struct SAssetPayload
{
    std::shared_ptr<void> ptr;
    std::type_index type = std::type_index(typeid(void));
    size_t bytes = 0;
};

using FAssetLoader = std::function<SAssetPayload(const SAssetKey& key)>;
using FAssetRefRelease = std::function<void()>;

enum class EAssetReferencedResourceKind : uint16_t
{
    Unknown = 0,
    RendererImagePair,
    RendererMesh,
    RendererTexture,
};

struct SAssetReferencedResource
{
    EAssetReferencedResourceKind kind = EAssetReferencedResourceKind::Unknown;
    uint64_t key = 0;
    uint32_t handle = 0;
    std::string debug_name;
    FAssetRefRelease release;
};

// TAsset is the source/CPU asset contract. The asset cache owns lookup and
// residency policy; concrete assets own any renderer refs they acquire while
// realizing themselves for drawing. Renderer resources remain renderer-owned.
//
// Most real assets should be concrete engine classes (TObjectImagery/
// T3DImagery/etc.) that inherit this. TGenericAsset is only the fallback
// payload record for small data that does not yet have a richer owner class.
class TAsset : public TSafeObjectBase<TAsset>
{
  public:
    virtual ~TAsset();

    [[nodiscard]] virtual AssetUid AssetId() const = 0;
    [[nodiscard]] virtual EAssetKind AssetKind() const = 0;
    [[nodiscard]] virtual const char* AssetPath() const = 0;
    [[nodiscard]] virtual const char* AssetName() const = 0;
    [[nodiscard]] virtual const char* AssetDebugName() const { return AssetName(); }

    [[nodiscard]] AssetUid Uid() const { return AssetId(); }
    [[nodiscard]] EAssetKind Kind() const { return AssetKind(); }
    [[nodiscard]] std::string Path() const { return AssetPath() ? AssetPath() : ""; }
    [[nodiscard]] std::string Name() const { return AssetName() ? AssetName() : ""; }

    [[nodiscard]] uint32_t AssetRefCount() const { return asset_ref_count_; }
    uint32_t AddAssetRef(uint32_t count = 1);
    uint32_t ReleaseAssetRef(uint32_t count = 1);
    void ResetAssetRefCount() { asset_ref_count_ = 0; }

    [[nodiscard]] uint32_t RefCount() const { return AssetRefCount(); }
    uint32_t AddRef(uint32_t count = 1) { return AddAssetRef(count); }
    uint32_t ReleaseRef(uint32_t count = 1) { return ReleaseAssetRef(count); }
    void ResetRefCount() { ResetAssetRefCount(); }

    // Balanced renderer/resource refs owned by the source asset. A T3DImagery
    // may hold renderer texture/mesh handles here; clearing the imagery asset
    // releases those refs, while TRenderer remains the GPU object owner.
    void AddReferencedResource(EAssetReferencedResourceKind kind,
                               uint64_t key,
                               uint32_t handle,
                               std::string debug_name,
                               FAssetRefRelease release);
    void ReleaseReferencedResources(EAssetReferencedResourceKind kind);
    void ReleaseReferencedResources();
    [[nodiscard]] const SAssetReferencedResource* FindReferencedResource(
        EAssetReferencedResourceKind kind,
        uint64_t key = 0) const;
    [[nodiscard]] uint32_t ReferencedResourceHandle(
        EAssetReferencedResourceKind kind,
        uint64_t key = 0) const;
    [[nodiscard]] size_t ReferencedResourceCount() const { return referenced_resources_.size(); }

    [[nodiscard]] bool HasPayload() const { return payload_.ptr != nullptr; }
    [[nodiscard]] const SAssetPayload& Payload() const { return payload_; }
    void SetPayload(std::shared_ptr<void> ptr,
                    std::type_index type = std::type_index(typeid(void)),
                    size_t bytes = 0);

    template <typename T>
    void SetPayload(std::shared_ptr<T> ptr, size_t bytes = 0)
    {
        SetPayload(std::static_pointer_cast<void>(ptr),
                   std::type_index(typeid(T)),
                   bytes);
    }

    template <typename T>
    [[nodiscard]] std::shared_ptr<T> PayloadAs() const
    {
        if (!payload_.ptr || payload_.type != std::type_index(typeid(T)))
            return {};
        return std::static_pointer_cast<T>(payload_.ptr);
    }

  private:
    SAssetPayload payload_;
    std::vector<SAssetReferencedResource> referenced_resources_;
    uint32_t asset_ref_count_ = 0;
};

template <typename T = TAsset>
using TAssetRef = TSafeRef<T>;

[[nodiscard]] std::string NormalizeAssetPath(std::string_view path);
[[nodiscard]] std::string NormalizeAssetName(std::string_view name);
[[nodiscard]] AssetUid MakeClassicAssetUid(std::string_view path, std::string_view name);

class TGenericAsset final : public TAsset
{
  public:
    explicit TGenericAsset(SAssetKey key);
    ~TGenericAsset() override = default;

    [[nodiscard]] const SAssetKey& Key() const { return key_; }
    [[nodiscard]] AssetUid AssetId() const override { return key_.uid; }
    [[nodiscard]] EAssetKind AssetKind() const override { return key_.kind; }
    [[nodiscard]] const char* AssetPath() const override { return key_.path.c_str(); }
    [[nodiscard]] const char* AssetName() const override { return key_.name.c_str(); }

  private:
    SAssetKey key_;
};

class TAssetCache
{
  public:
    TAssetCache() = default;
    ~TAssetCache();

    TAssetCache(const TAssetCache&) = delete;
    TAssetCache& operator=(const TAssetCache&) = delete;

    bool Init();
    void Shutdown();

    [[nodiscard]] bool IsInitialized() const { return initialized_; }
    [[nodiscard]] size_t AssetCount() const { return assets_by_uid_.size(); }

    [[nodiscard]] TAssetRef<> Find(AssetUid uid) const;
    [[nodiscard]] TAssetRef<> FindClassic(std::string_view path, std::string_view name) const;

    TAssetRef<> RegisterAsset(const SAssetKey& key,
                              std::shared_ptr<void> payload = {},
                              std::type_index payload_type = std::type_index(typeid(void)),
                              size_t payload_bytes = 0);
    std::vector<TAssetRef<>> RegisterAssets(const std::vector<SAssetKey>& keys);

    TAssetRef<> GetOrLoadAsset(const SAssetKey& key, const FAssetLoader& loader);
    std::vector<TAssetRef<>> GetOrLoadAssets(const std::vector<SAssetKey>& keys,
                                             const FAssetLoader& loader);

    TAssetRef<> RegisterClassic(std::string_view path,
                                std::string_view name,
                                EAssetKind kind = EAssetKind::ClassicResource,
                                std::shared_ptr<void> payload = {},
                                std::type_index payload_type = std::type_index(typeid(void)),
                                size_t payload_bytes = 0);

    TAssetRef<> GetOrLoadClassic(std::string_view path,
                                 std::string_view name,
                                 EAssetKind kind,
                                 const FAssetLoader& loader);

    template <typename T>
    TAssetRef<> RegisterClassic(std::string_view path,
                                std::string_view name,
                                EAssetKind kind,
                                std::shared_ptr<T> payload,
                                size_t payload_bytes = 0)
    {
        return RegisterClassic(path,
                               name,
                               kind,
                               std::static_pointer_cast<void>(payload),
                               std::type_index(typeid(T)),
                               payload_bytes);
    }

    uint32_t IncrementRef(TAssetRef<> asset, uint32_t count = 1);
    uint32_t DecrementRef(TAssetRef<> asset, uint32_t count = 1);
    uint32_t AddRef(TAssetRef<> asset, uint32_t count = 1) { return IncrementRef(asset, count); }
    uint32_t ReleaseRef(TAssetRef<> asset, uint32_t count = 1) { return DecrementRef(asset, count); }
    void ResetRefCounts();
    size_t ClearUnreferencedAssets();

    // Register an externally-owned concrete asset, such as TObjectImagery.
    // The cache maps its AssetId to a safe handle but does not own memory.
    bool RegisterExternalAsset(TAsset* asset);
    void UnregisterExternalAsset(TAsset* asset);

  private:
    TAsset* FindPtr(AssetUid uid) const;

    std::vector<std::unique_ptr<TGenericAsset>> assets_;
    std::unordered_map<AssetUid, TAsset*> assets_by_uid_;
    bool initialized_ = false;
};
