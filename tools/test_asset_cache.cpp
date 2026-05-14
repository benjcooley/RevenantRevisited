#include "../src/assetcache.h"

#include <cassert>
#include <memory>

int main()
{
    TAssetCache cache;
    assert(cache.Init());

    const AssetUid uid_a = MakeClassicAssetUid("Data\\I3D", "LOCKE.I3D");
    const AssetUid uid_b = MakeClassicAssetUid("data/i3d/", "locke.i3d");
    assert(uid_a == uid_b);

    auto payload = std::make_shared<int>(42);
    TAssetRef<> ref = cache.RegisterClassic("Data\\I3D", "LOCKE.I3D",
                                            EAssetKind::I3D, payload, sizeof(int));
    assert(ref.IsValid());
    assert(ref->Uid() == uid_a);
    assert(ref->Kind() == EAssetKind::I3D);
    assert(ref->Path() == "data/i3d");
    assert(ref->Name() == "locke.i3d");
    assert(ref->PayloadAs<int>() && *ref->PayloadAs<int>() == 42);

    TAssetRef<> same = cache.FindClassic("data/i3d", "locke.i3d");
    assert(same == ref);
    assert(cache.AssetCount() == 1);

    assert(cache.AddRef(ref) == 1);
    assert(cache.AddRef(ref, 2) == 3);
    assert(cache.ReleaseRef(ref) == 2);
    assert(cache.ReleaseRef(ref, 99) == 0);
    assert(ref.IsValid());

    std::vector<SAssetKey> set;
    set.push_back(SAssetKey::Classic("data/i3d", "guard.i3d", EAssetKind::I3D));
    set.push_back(SAssetKey::Classic("data/i3d", "torch.i3d", EAssetKind::I3D));
    const std::vector<TAssetRef<>> refs = cache.RegisterAssets(set);
    assert(refs.size() == 2);
    assert(refs[0].IsValid());
    assert(refs[1].IsValid());
    assert(cache.AssetCount() == 3);

    int load_count = 0;
    TAssetRef<> loaded = cache.GetOrLoadClassic(
        "data/i3d", "mage.i3d", EAssetKind::I3D,
        [&load_count](const SAssetKey& key) {
            assert(key.kind == EAssetKind::I3D);
            ++load_count;
            SAssetPayload out;
            out.ptr = std::static_pointer_cast<void>(std::make_shared<int>(77));
            out.type = std::type_index(typeid(int));
            out.bytes = sizeof(int);
            return out;
        });
    assert(loaded.IsValid());
    assert(load_count == 1);
    assert(*loaded->PayloadAs<int>() == 77);

    TAssetRef<> loaded_again = cache.GetOrLoadClassic(
        "DATA\\I3D\\", "MAGE.I3D", EAssetKind::I3D,
        [&load_count](const SAssetKey&) {
            ++load_count;
            return SAssetPayload{};
        });
    assert(loaded_again == loaded);
    assert(load_count == 1);

    int released_gpu_refs = 0;
    loaded->AddReferencedResource(EAssetReferencedResourceKind::RendererImagePair,
                                  1001,
                                  77,
                                  "renderer.image_pair.mage",
                                  [&released_gpu_refs]() {
                                      ++released_gpu_refs;
                                  });
    assert(loaded->ReferencedResourceCount() == 1);
    assert(loaded->ReferencedResourceHandle(EAssetReferencedResourceKind::RendererImagePair, 1001) == 77);
    assert(loaded->ReferencedResourceHandle(EAssetReferencedResourceKind::RendererImagePair) == 77);

    assert(cache.IncrementRef(loaded) == 1);
    assert(cache.ClearUnreferencedAssets() == 3);
    assert(loaded.IsValid());
    assert(released_gpu_refs == 0);

    assert(cache.DecrementRef(loaded) == 0);
    assert(cache.ClearUnreferencedAssets() == 1);
    assert(!loaded.IsValid());
    assert(released_gpu_refs == 1);

    cache.Shutdown();
    assert(!ref.IsValid());
    return 0;
}
